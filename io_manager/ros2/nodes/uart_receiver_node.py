#!/usr/bin/env python3

import serial

import rclpy
from rclpy.node import Node

from std_msgs.msg import UInt8MultiArray


class UARTReceiverNode(Node):

    def __init__(self):
        super().__init__("uart_receiver_node")

        self.declare_parameter("port", "/dev/ttyAMA0")
        self.declare_parameter("baudrate", 115200)
        self.declare_parameter("timeout", 0.1)

        port = self.get_parameter("port").value
        baudrate = self.get_parameter("baudrate").value
        timeout = self.get_parameter("timeout").value

        self.publisher_ = self.create_publisher(
            UInt8MultiArray,
            "/uart/rx",
            10,
        )

        try:
            self.serial = serial.Serial(
                port=port,
                baudrate=baudrate,
                timeout=timeout,
            )

            self.get_logger().info(
                f"Opened UART {port} @ {baudrate} baud"
            )

        except serial.SerialException as e:
            self.get_logger().fatal(str(e))
            raise

        self.timer = self.create_timer(
            0.001,
            self.read_serial,
        )

    def read_serial(self):
        try:
            data = self.serial.read(256)

            if len(data) == 0:
                return

            msg = UInt8MultiArray()
            msg.data = list(data)

            self.publisher_.publish(msg)

            self.get_logger().info(
                f"RX {len(data)} bytes: {data.hex(' ')}"
            )

        except serial.SerialException as e:
            self.get_logger().error(str(e))


def main():

    rclpy.init()

    node = UARTReceiverNode()

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