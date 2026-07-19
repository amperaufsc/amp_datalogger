#!/usr/bin/env python3

import serial

import rclpy
from rclpy.node import Node


class UARTSenderNode(Node):

    def __init__(self):
        super().__init__("uart_sender_node")

        self.declare_parameter("port", "/dev/serial0")
        self.declare_parameter("baudrate", 115200)
        self.declare_parameter("period", 1.0)

        port = self.get_parameter("port").value
        baudrate = self.get_parameter("baudrate").value
        period = self.get_parameter("period").value

        self.serial = serial.Serial(
            port=port,
            baudrate=baudrate,
            timeout=0.1,
        )

        self.counter = 0

        self.timer = self.create_timer(
            period,
            self.send_data,
        )

        self.get_logger().info(
            f"Opened UART {port} @ {baudrate}"
        )

    def send_data(self):

        payload = bytes([
            0xAA,
            0x55,
            self.counter & 0xFF,
            0x10,
            0x20,
            0x30
        ])

        self.serial.write(payload)

        self.get_logger().info(
            f"TX: {payload.hex(' ')}"
        )

        self.counter += 1


def main():

    rclpy.init()

    node = UARTSenderNode()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        pass

    finally:
        node.serial.close()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()