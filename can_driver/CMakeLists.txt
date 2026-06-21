cmake_minimum_required(VERSION 3.8)
project(can_driver)

# =========================
# Compiler options
# =========================
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -std=c++17)
endif()

# =========================
# Dependencies
# =========================
find_package(ament_cmake REQUIRED)

find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

find_package(rclpy REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGPIOD REQUIRED libgpiod)

find_package(rosidl_default_runtime REQUIRED)
find_package(manager_msgs REQUIRED)

# =========================
# Core C++ library
# =========================
add_library(can_driver_core SHARED
  src/can_driver/drivers/socketcan_driver.cpp
  src/can_driver/factory/can_factory.cpp
)

target_include_directories(can_driver_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${LIBGPIOD_INCLUDE_DIRS}
)

target_link_libraries(can_driver_core
  ${LIBGPIOD_LIBRARIES}
)

ament_target_dependencies(
  can_driver_core
  rclcpp
  std_msgs
  manager_msgs
)

# =========================
# Install
# =========================

install(
  DIRECTORY dbc
  DESTINATION share/${PROJECT_NAME}
)

install(
  TARGETS can_driver_core
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include/
  DESTINATION include
)

install(PROGRAMS
  src/can_driver/algorithms/dbc_parser.py
  DESTINATION lib/${PROJECT_NAME}
)

install(PROGRAMS
  src/can_driver/algorithms/can_topic_manager.py
  DESTINATION lib/${PROJECT_NAME}
)

install(PROGRAMS
  src/can_driver/drivers/socketcan_driver.py
  DESTINATION lib/${PROJECT_NAME}
)

install(PROGRAMS
  src/can_driver/models/can_frame.py
  DESTINATION lib/${PROJECT_NAME}
)

# =========================
# Python nodes
# =========================

install(PROGRAMS
  ros2/tools/simple_can_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME simple_can_node
)

install(PROGRAMS
  ros2/nodes/can_receiver_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME can_receiver_node
)

# =========================
# Export
# =========================

ament_export_targets(export_${PROJECT_NAME})
ament_export_include_directories(include)

# =========================
# Package
# =========================

ament_package()