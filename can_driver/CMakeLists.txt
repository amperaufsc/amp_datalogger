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

# =========================
# Core C++ library
# =========================
add_library(can_driver_core SHARED
  # Drivers
  src/can_driver/drivers/socketcan_driver.cpp

  # Factories
  src/can_driver/factory/can_factory.cpp
)

target_include_directories(can_driver_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${LIBGPIOD_INCLUDE_DIRS}
)

target_link_libraries(can_driver_core
  PUBLIC
    ${LIBGPIOD_LIBRARIES}
)

ament_target_dependencies(can_driver_core)

# =========================
# Install
# =========================

# Core C++ library
install(
  TARGETS can_driver_core
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

# Headers
install(
  DIRECTORY include/
  DESTINATION include
)

# =========================
# ROS nodes (Python)
# =========================
install(PROGRAMS
  ros2/tools/simple_can_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME simple_can_node
)

# =========================
# Export
# =========================
ament_export_targets(export_${PROJECT_NAME})
ament_export_include_directories(include)

# =========================
# Ament package
# =========================
ament_package()