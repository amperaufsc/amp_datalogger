cmake_minimum_required(VERSION 3.8)
project(io_manager)

# =========================
# Compiler options
# =========================
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# =========================
# Dependencies
# =========================
find_package(ament_cmake REQUIRED)
find_package(rclpy REQUIRED)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGPIOD REQUIRED libgpiod)

# =========================
# Core C++ library
# =========================
add_library(io_manager_core SHARED
  # Drivers
  src/io_manager/drivers/libgpiod_gpio_input.cpp
  src/io_manager/drivers/libgpiod_gpio_output.cpp
  src/io_manager/drivers/libgpiod_gpio_interrupt.cpp

  # Factories
  src/io_manager/factory/gpio_factory.cpp
  src/io_manager/factory/gpio_interrupt_factory.cpp
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

target_link_libraries(io_manager_bindings
  PRIVATE
    io_manager_core
)

target_include_directories(io_manager_bindings
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
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
  LIBRARY DESTINATION
    lib/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages
)

# Headers
install(
  DIRECTORY include/
  DESTINATION include
)

# ROS nodes (Python)
install(
  PROGRAMS ros2/tools/blink_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME blink_node
)

install(
  PROGRAMS ros2/tools/gpio_interrupt_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME gpio_interrupt_node
)

install(
  PROGRAMS ros2/nodes/gpio_multi_interrupt_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME gpio_multi_interrupt_node
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
