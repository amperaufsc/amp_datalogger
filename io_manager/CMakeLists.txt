cmake_minimum_required(VERSION 3.8)
project(io_manager)

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

find_package(io_manager_msgs REQUIRED)

# =========================
# Core C++ library
# =========================
add_library(io_manager_core SHARED
  # Drivers
  src/io_manager/drivers/libgpiod_gpio_input.cpp
  src/io_manager/drivers/libgpiod_gpio_output.cpp
  src/io_manager/drivers/libgpiod_gpio_interrupt.cpp
  src/io_manager/drivers/ads1115.cpp
  src/io_manager/drivers/mux.cpp

  # Factories
  src/io_manager/factory/gpio_factory.cpp
  src/io_manager/factory/gpio_interrupt_factory.cpp
  src/io_manager/factory/analog_input_factory.cpp
  src/io_manager/factory/mux_factory.cpp
)

target_include_directories(io_manager_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${LIBGPIOD_INCLUDE_DIRS}
)

target_link_libraries(io_manager_core
  PUBLIC
    ${LIBGPIOD_LIBRARIES}
)

ament_target_dependencies(io_manager_core)

# =========================
# Python bindings (pybind11)
# =========================
pybind11_add_module(io_manager_bindings
  src/io_manager/bindings/gpio_bindings.cpp
)

set_target_properties(io_manager_bindings
  PROPERTIES
    OUTPUT_NAME "io_manager_bindings"
)

target_link_libraries(io_manager_bindings PRIVATE io_manager_core)

target_include_directories(io_manager_bindings
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# =========================
# C++ ROS 2 node: sensor_mux_node
# =========================
add_executable(sensor_mux_node
  ros2/nodes/sensor_mux_node.cpp
)

target_include_directories(sensor_mux_node
  PRIVATE
    include
)

# Link com a biblioteca core
target_link_libraries(sensor_mux_node io_manager_core)

# Dependências ROS2
ament_target_dependencies(sensor_mux_node
  rclcpp
  std_msgs
  manager_msgs
)

install(TARGETS sensor_mux_node
  DESTINATION lib/${PROJECT_NAME}
)

# =========================
# Install
# =========================

# Core C++ library
install(
  TARGETS io_manager_core
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

# Python bindings
install(
  TARGETS io_manager_bindings
  LIBRARY DESTINATION lib/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages
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
  ros2/tools/blink_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME blink_node
)

install(PROGRAMS
  ros2/tools/gpio_interrupt_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME gpio_interrupt_node
)

install(PROGRAMS
  ros2/tools/ads1115_test_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME ads1115_test_node
)

install(PROGRAMS
  ros2/tools/mux_test_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME mux_test_node
)

install(PROGRAMS
  ros2/nodes/gpio_multi_interrupt_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME gpio_multi_interrupt_node
)

install(PROGRAMS
  ros2/nodes/analog_read_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME analog_read_node
)

install(PROGRAMS
  src/io_manager/algorithms/utils.py
  DESTINATION lib/${PROJECT_NAME}
)

# Config
install(
  DIRECTORY ros2/config
  DESTINATION share/${PROJECT_NAME}
)

# Launch
install(
  DIRECTORY ros2/launch
  DESTINATION share/${PROJECT_NAME}/launch
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