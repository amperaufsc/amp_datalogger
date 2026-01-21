cmake_minimum_required(VERSION 3.8)
project(datalogger)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclpy REQUIRED)
find_package(std_msgs REQUIRED)

install(
  PROGRAMS ros2/nodes/wheel_speed_node.py
  DESTINATION lib/${PROJECT_NAME}  
  RENAME wheel_speed_node
)

install(
  PROGRAMS ros2/tools/fake_encoder_node.py
  DESTINATION lib/${PROJECT_NAME}  
  RENAME fake_encoder_node
)

install(
  PROGRAMS src/datalogger/algorithms/wheel_speed_measurement.py
  DESTINATION lib/${PROJECT_NAME}/src/datalogger/algorithms
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()