# Data Acquisition System

This repository groups the packages responsible for data acquisition, processing, and communication within the ROS 2 stack of a Formula Student Vehicle (FSAE).

These modules interface directly with onboard sensors, convert raw signals into physical quantities, and distribute structured data via ROS 2 and CAN to other systems (e.g., control, telemetry, and logging).

## The system was designed to:

    → Acquire data from embedded sensors (analog and digital)
    → Interface with ADCs, multiplexers, and peripherals
    → Convert raw signals into calibrated physical values
    → Publish data via ROS 2 topics
    → Transmit critical signals via CAN bus
    → Log data for later analysis

The repository standardizes data at the ROS 2 and CAN interface levels, ensuring compatibility between hardware access and high-level modules, without including control logic.

--- 

# Architecture

The system is divided into three main layers:

## IO (Acquisition)

Responsible for direct communication with the hardware.

- Sensors (direction, pressure, temperature, etc.)
- ADC (e.g., ADS1115)
- Multiplexers (MUX)
- GPIO / I2C / SPI Interfaces

Responsibility: acquiring raw data from the physical world.

## Data Processing

Responsible for transforming data into useful information.

- Conversion (voltage → physical quantity)
- Calibration 
- Filtering

Responsibility: to provide interpretable and consistent data.

## Communication

Responsible for distributing data within the system.

- CAN Interface 
- ROS 2 Nodes 
- Integration with other modules

Responsibility: to ensure reliable data delivery.

# Estrutura de Pacotes

## ```io_manager```

| Functionality  | Description                            |
| -------------- | -------------------------------------- |
| Interface ADC  | Reading analog signals (ADS1115)       |     
| Control  MUX   | Channel selection                      |
| Drivers        | Sensor reading                         |

## ```can_interface```

CAN communication layer.

| Functionality  | Description                  |
| -------------- | ---------------------------- |
| CAN TX/RX      | Sending and receiving frames |
| IDs CAN        | Defining identifiers         |
| SocketCAN      | Interface Linux              |

## ```datalogger```

Processing and storage.

| Functionality  | Description              |
| -------------- | -------------------------|
| Conversion     | Raw data → physical data |
| Logging        | ROS bags                 |
| Organization   | Data structuring         |

# Fluxo de Dados

# ROS Interfaces

## Topics (IO)

| Module     | Direction | Topics         | Message Type             | Description       |
| ---------- | ------- | -------------- | ---------------------------- | --------------- |
| IO Manager | Pub     | `/adc/values`  | `std_msgs/Float32MultiArray` | Leituras do ADC |

## Topics (Processing)

| Module     | Direction | Topics         | Message Type             | Description       |
| ---------- | ------- | -------------- | ---------------------------- | --------------- |

## Tópicos (CAN)

| Module     | Direction | Topics         | Message Type             | Description       |
| ---------- | ------- | -------------- | ---------------------------- | --------------- |

# Dependencies

Main dependencies:

- ROS 2 Humble
- rclcpp / rclpy
- std_msgs, sensor_msgs, can_msgs
- tf2, tf2_ros
- python-can
- colcon

