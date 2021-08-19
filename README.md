## Description

Interface (driver) software, including ROS node, for inertial sensors compatible with the [Microstrain Communication Library (MSCL)](https://github.com/LORD-MicroStrain/MSCL).

MSCL is developed by [LORD Sensing - Microstrain](http://microstrain.com) in Williston, VT. 


## Build Instructions

#### MSCL
MSCL is now installed in the [CMakeLists.txt](./ros_mscl/CMakeLists.txt). The version installed can be changed by passing the flag `-DMSCL_VERSION="62.0.0"`

If you already have MSCL installed and want to use your installed version instead of the one automatically downloaded, you can specify the location by passing the flag `-DMSCL_DIR=/usr/share/c++-mscl`

We do our best to keep ROS-MSCL up-to-date with the latest MSCL changes, but sometimes there is a delay. The currently supported version of MSCL is [v62.0.0](https://github.com/LORD-MicroStrain/MSCL/releases/tag/v62.0.0)

#### Building from source
1. Install ROS2 and create a workspace: [Configuring Your ROS2 Environment](https://docs.ros.org/en/foxy/Tutorials/Configuring-ROS2-Environment.html)

2. Move the entire ROS-MSCL folder (microstrain_inertial, microstrain_msgs , and microstrain_common for just source) to the your_workspace/src directory.

3. Locate and register the ros_mscl package: `rospack find microstrain_inertial`

4. Build your workspace:
        
        cd ~/your_workspace
        colcon build
        source ~/your_workspace/install/setup.bash
   The source command may need to be run in each terminal prior to launching a ROS node.
#### Launch the node and publish data
The following command will launch the driver. Keep in mind each instance needs to be run in a separate terminal.
            
        ros2 launch microstrain_inertial microstrain_launch.py

This driver is implemented as a lifecycle node.  Upon running, the node will be in the unconfigured state and no interaction has occurred with the device.  The node must be transitioned as follows for data to be available:

- transition to configure state: 

    ros2 lifecycle set /gx5 configure

- transition to active state: 

    ros2 lifecycle set /gx5 activate

You can stop data from streaming by putting the device into the "deactivate" state.  Both the "cleanup" and "shutdown" states will disconnect from the device and close the raw data log file (if enabled.)

To check published topics:
        
    ros2 topic list

**Example**: Connect to and publish data from two devices simultaneously  
In two different terminals:
    
    ros2 launch microstrain_inertial microstrain.launch name:=sensor1234

    roslaunch microstrain_inertial microstrain.launch name:=bestSensor port:=/dev/ttyACM1
This will launch two nodes that publish data to different namespaces:
- sensor1234, connected over port: /dev/ttyACM0
- bestSensor, connected over port: /dev/ttyACM1

An example subscriber node can be found here: [ROS-MSCL Examples](./microstrain_examples)  


## Docker Integration

### VSCode

The easiest way to use docker while still using an IDE is to use VSCode as an IDE. Follow the steps below to develop on this repo in a docker container

1. Install the following dependencies:
    1. [VSCode](https://code.visualstudio.com/)
    1. [Docker](https://docs.docker.com/get-docker/)
1. Open VSCode and install the following [plugins](https://code.visualstudio.com/docs/editor/extension-marketplace):
    1. [VSCode Docker plugin](https://marketplace.visualstudio.com/items?itemName=ms-azuretools.vscode-docker)
    1. [VSCode Remote Containers plugin](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)
1. Open this directory in a container by following [this guide](https://code.visualstudio.com/docs/remote/containers#_quick-start-open-an-existing-folder-in-a-container)
    1. Due to a bug in the remote container plugin, you will need to refresh the window once it comes up. To do this, type `Ctrl+Shift+p` and type `Reload Window` and hit enter. Note that this will have to be repeated every time the container is rebuilt
1. Once the folder is open in VSCode, you can build the project by running `Ctrl+Shift+B` to trigger a build, or `Ctrl+p` to open quick open, then type `task build` and hit enter
1. You can run the project by following [this guide](https://code.visualstudio.com/docs/editor/debugging)

### Make

If you are comfortable working from the command line, or want to produce runtime images, the [Makefile](./devcontainer/Makefile) in the [.devcontainer](./devcontainer) directory
can be used to build docker images, run a shell inside the docker images and produce a runtime image. Follow the steps below to setup your environment to use the `Makefile`

1. Install the following dependencies:
    1. [Make](https://www.gnu.org/software/make/)
    1. [Docker](https://docs.docker.com/get-docker/)
    1. [qemu-user-static](https://packages.ubuntu.com/bionic/qemu-user-static) (for multiarch builds)
        1. Run the following command to register the qemu binaries with docker: `docker run --rm --privileged multiarch/qemu-user-static:register`

The `Makefile` exposes the following tasks. They can all be run from the `.devcontainer` directory:
* `make build-shell` - Builds the docker image and starts a shell session in the image allowing the user to develop and build the ROS project using common commands such as `catkin_make`
* `make image` - Builds the runtim image that contains only the required dependencies and the ROS node. The resulting image is names `ros-mscl`
* `make clean` - Cleans up after the above two tasks

## License
ROS-MSCL is released under the MIT License - see the `LICENSE` file in the source distribution.

Copyright (c)  2021, Parker Hannifin Corp.
