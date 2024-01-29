/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Parker-Lord Driver Definition File
//
// Copyright (c) 2017, Brian Bingham
// Copyright (c) 2020, Parker Hannifin Corp
//
// This code is licensed under MIT license (see LICENSE file for details)
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

#include "microstrain_inertial_driver/microstrain_inertial_driver_component.h"

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
   
  rclcpp::executors::SingleThreadedExecutor exe;

  rclcpp::NodeOptions options;
  auto node = std::make_shared<microstrain::MicroStrainInertialDriverComponent>(options);

  exe.add_node(node->get_node_base_interface());

  exe.spin();

  rclcpp::shutdown();

  return 0;
}
