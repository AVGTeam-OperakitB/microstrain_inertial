cmake_minimum_required(VERSION 3.0.2)
project(microstrain_inertial_driver)

# C++ 14 required
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands by default (helpful for clang-tidy and autocomplete for certain IDEs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

# Force some options for this module and the MIP SDK
set(BUILD_SHARED_LIBS_TEMP "${BUILD_SHARED_LIBS}")
set(BUILD_EXAMPLES_TEMP "${BUILD_EXAMPLES}")
set(BUILD_TESTING_TEMP "${BUILD_TESTING}")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Locate the common code and messages
set(COMMON_NAME "microstrain_inertial_driver_common")
set(COMMON_DIR "${${PROJECT_NAME}_SOURCE_DIR}/${COMMON_NAME}")
set(COMMON_SRC_DIR "${COMMON_DIR}/src")
set(COMMON_INC_DIR "${COMMON_DIR}/include/${COMMON_NAME}")

set(MIP_SDK_DIR "${COMMON_DIR}/mip_sdk")
set(MIP_SDK_SRC_DIR "${MIP_SDK_DIR}/src")

# Some helpful options for debugging
add_compile_options($<$<CONFIG:DEBUG>:-ggdb>)

# Try to find Git
find_package(Git)

# Make sure that the submodule has been properly initialized
if(NOT EXISTS "${COMMON_SRC_DIR}")
  message(STATUS "Initializing ${COMMON_DIR} submodule. This should only happen once.")

  # Make sure we can find the git executable
  if(NOT Git_FOUND)
    message(FATAL_ERROR "Unable to initialize submodule because we could not find the git executable. Please clone this repo using 'git clone --recursive'")
  endif()

  # Initialize and update the submodule
  execute_process(
    WORKING_DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E env ${GIT_EXECUTABLE} submodule update --recursive --init
  )
endif()

# Use Git to find the version
set(DEFAULT_DRIVER_GIT_VERSION "unknown")
if(NOT GIT_FOUND)
  message(STATUS "Unable to find git, will build with unknown version")
  set(DRIVER_GIT_VERSION ${DEFAULT_DRIVER_GIT_VERSION})
else()
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E env ${GIT_EXECUTABLE} describe --tags
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE DRIVER_GIT_VERSION_OUT
    ERROR_VARIABLE DRIVER_GIT_VERSION_ERR
    RESULT_VARIABLE DRIVER_GIT_VERSION_RET
  )
  if(NOT ${DRIVER_GIT_VERSION_RET} EQUAL 0)
    message(STATUS "Unable to determine version from Git, defaulting to version ${DEFAULT_DRIVER_GIT_VERSION}")
    set(DRIVER_GIT_VERSION ${DEFAULT_DRIVER_GIT_VERSION})
  else()
    set(DRIVER_GIT_VERSION ${DRIVER_GIT_VERSION_OUT})
    string(REGEX REPLACE "\n" "" DRIVER_GIT_VERSION "${DRIVER_GIT_VERSION}")
    message(STATUS "Microstrain Driver Version: ${DRIVER_GIT_VERSION}")
  endif()
endif()

# Set some general CMake flags
set(CMAKE_C_FLAGS "-Wno-implicit-function-declaration -Wno-incompatible-pointer-types -Wno-unused-variable -Wno-format -fno-builtin-memcpy")

# Include the MIP SDK source
add_subdirectory("${MIP_SDK_DIR}" EXCLUDE_FROM_ALL)
include_directories("${MIP_SDK_SRC_DIR}")

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roslint
  message_generation
  geometry_msgs
  nav_msgs
  nmea_msgs
  rtcm_msgs
  roscpp
  sensor_msgs
  std_msgs
  std_srvs
  tf2
  tf2_ros
  tf2_geometry_msgs
  microstrain_inertial_msgs
)

# Find some less ROS-y packages
list(APPEND CMAKE_MODULE_PATH "/usr/share/cmake/geographiclib/")
find_package(Eigen3 REQUIRED)
find_package(GeographicLib REQUIRED)

# We want to use the static library for libgeographic, but it doesn't seem to find it for us, so substitute it here
string(REGEX REPLACE "[.]so[^\\/\\s]*$" ".a" GeographicLib_LIBRARIES ${GeographicLib_LIBRARIES})

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
  INCLUDE_DIRS
    include
  CATKIN_DEPENDS
    roscpp
    cmake_modules
    tf2
    tf2_ros
    tf2_geometry_msgs
    std_msgs
    std_srvs
    geometry_msgs
    sensor_msgs
    nav_msgs
    nmea_msgs
    rtcm_msgs
    message_runtime
    microstrain_inertial_msgs
)
###########
## Build ##
###########

# Add the catkin includes
include_directories(
  include
  ${COMMON_DIR}/include
  ${catkin_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
  ${GeographicLib_INCLUDE_DIRS}
)

set(LIB_SRC_FILES
  ${COMMON_SRC_DIR}/subscribers.cpp
  ${COMMON_SRC_DIR}/publishers.cpp
  ${COMMON_SRC_DIR}/node_common.cpp
  ${COMMON_SRC_DIR}/services.cpp
  ${COMMON_SRC_DIR}/config.cpp
  ${COMMON_SRC_DIR}/utils/clock_bias_monitor.cpp

  ${COMMON_SRC_DIR}/utils/mappings/mip_mapping.cpp
  ${COMMON_SRC_DIR}/utils/mappings/mip_publisher_mapping.cpp

  ${COMMON_SRC_DIR}/utils/mip/ros_mip_device.cpp
  ${COMMON_SRC_DIR}/utils/mip/ros_mip_device_main.cpp
  ${COMMON_SRC_DIR}/utils/mip/ros_mip_device_aux.cpp
  ${COMMON_SRC_DIR}/utils/mip/ros_connection.cpp

  src/microstrain_inertial_driver.cpp
)
set(LIB_INC_FILES
  ${COMMON_INC_DIR}/subscribers.h
  ${COMMON_INC_DIR}/publishers.h
  ${COMMON_INC_DIR}/node_common.h
  ${COMMON_INC_DIR}/services.h
  ${COMMON_INC_DIR}/config.h
  ${COMMON_INC_DIR}/utils/ros_compat.h
  ${COMMON_INC_DIR}/utils/clock_bias_monitor.h

  ${COMMON_INC_DIR}/utils/mappings/mip_mapping.h
  ${COMMON_INC_DIR}/utils/mappings/mip_publisher_mapping.h

  ${COMMON_INC_DIR}/utils/mip/ros_mip_device.h
  ${COMMON_INC_DIR}/utils/mip/ros_mip_device_main.h
  ${COMMON_INC_DIR}/utils/mip/ros_mip_device_aux.h
  ${COMMON_INC_DIR}/utils/mip/ros_connection.h

  include/${PROJECT_NAME}/microstrain_inertial_driver.h
)

add_library(${PROJECT_NAME} ${LIB_SRC_FILES} ${LIB_INC_FILES})
add_dependencies(${PROJECT_NAME}
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
  ${catkin_EXPORTED_TARGETS}
)

# Executables
set(NODE_SRC_FILES
  src/microstrain_inertial_driver_node.cpp
)
set(NODE_INC_FILES
  include/${PROJECT_NAME}/microstrain_inertial_driver.h
)
add_executable(${PROJECT_NAME}_node ${NODE_SRC_FILES} ${NODE_INC_FILES})

# Annoying, but the ROS types don't match up with the MIP types for floats and doubles, so ignore those warnings for now
set_source_files_properties(${COMMON_SRC_DIR}/services.cpp PROPERTIES COMPILE_FLAGS "-Wno-narrowing")
set_source_files_properties(${COMMON_SRC_DIR}/services.cpp PROPERTIES COMPILE_OPTIONS "-Wno-narrowing")

# Tell the code the version of the driver that is being build
add_definitions(-DMICROSTRAIN_DRIVER_VERSION="${DRIVER_GIT_VERSION}")

# Let the code know if it is being compiled with ROS1 or ROS2
if(DEFINED ENV{ROS_VERSION})
  add_definitions(-DMICROSTRAIN_ROS_VERSION=$ENV{ROS_VERSION})
else()
  message(FATAL_ERROR "ROS_VERSION environment variable is not set.")
endif()

# Allow the MSCL include directory to be accessed
include_directories(${MSCL_INC_PATH} ${BOOST_INC_PATH})

# Linking
target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
  mip
)
target_link_libraries(${PROJECT_NAME}_node
  ${PROJECT_NAME}
  ${catkin_LIBRARIES}
  ${GeographicLib_LIBRARIES}
)

#############
## Install ##
#############

install(TARGETS ${PROJECT_NAME}_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  PUBLIC_HEADER DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

install(DIRECTORY launch config
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

install(DIRECTORY ${COMMON_DIR}/config
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/${COMMON_NAME}
)

# Reset the forced options
set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_TEMP}" CACHE BOOL "${BUILD_SHARED_LIBS_TEMP}" FORCE)
set(BUILD_EXAMPLES "${BUILD_EXAMPLES_TEMP}" CACHE BOOL "${BUILD_EXAMPLES_TEMP}" FORCE)
set(BUILD_TESTING "${BUILD_TESTING_TEMP}" CACHE BOOL "${BUILD_TESTING_TEMP}" FORCE)

#############
## Testing ##
#############

# build/c++11 and whitespace/braces only trip on the buildfarm which is melodic, maybe it's not needed in noetic?
set(ROSLINT_CPP_OPTS "--filter=-whitespace/line_length,-runtime/references,-readability/fn_size,-whitespace/parens,-build/c++11,-whitespace/braces")
get_target_property(ROSLINT_SOURCES ${PROJECT_NAME} SOURCES)
roslint_cpp(${ROSLINT_SOURCES})

roslint_add_test()

# Make sure that the wiki document is up to date
add_test(
  NAME wiki_publishers
  COMMAND ${CMAKE_COMMAND} -E env ${COMMON_DIR}/tools/check-wiki-publishers.sh
)