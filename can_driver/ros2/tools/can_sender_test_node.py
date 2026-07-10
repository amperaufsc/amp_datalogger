#!/usr/bin/env python3

from pathlib import Path
from ament_index_python.packages import get_package_share_directory

import rclpy
from rclpy.node import Node

from std_msgs.msg import Float64
from vectornav_msgs.msg import ImuGroup

from dbc_parser import DbcParser
from socketcan_driver import SocketCanDriver


class CanSenderNode(Node):

    def __init__(self):
        super().__init__("can_sender")

        self._steering = 0.0

        self._accel_x = 0.0
        self._accel_y = 0.0
        self._accel_z = 0.0

        default_dbc = str(
            Path(get_package_share_directory("can_driver"))
            / "dbc"
            / "amp226.dbc"
        )

        self._dbc = DbcParser(
            Path(default_dbc)
        )

        self._driver = SocketCanDriver(
            channel="can0"
        )

        self._driver.open()

        self.create_subscription(
            Float64,
            "/sensor_steering",
            self._steering_callback,
            10,
        )

        self.create_subscription(
            ImuGroup,
            "/vectornav/raw/imu",
            self._imu_callback,
            10,
        )

        self.create_timer(
            0.02,
            self._send_timer,
        )

        self.get_logger().info("CAN Sender started.")

    def _steering_callback(self, msg):

        self._steering = msg.data

    def _imu_callback(self, msg):

        self._accel_x = msg.accel.x
        self._accel_y = msg.accel.y
        self._accel_z = msg.accel.z

        self.get_logger().info(f"Aceel: {self._accel_x:.3f}, {self._accel_y:.3f}, {self._accel_z:.3f}")

    def _send_timer(self):

        steering = self._dbc.encode(
            "STEERING_DATA",
            {
                "SteeringAngle": self._steering,
                "LimitSwitchLeft": False,
                "LimitSwitchRight": False,
            },
        )

        imu = self._dbc.encode(
            "IMUAcceleration",
            {
                "AccelX": self._accel_x,
                "AccelY": self._accel_y,
                "AccelZ": self._accel_z,
            },
        )

        self._driver.send(steering)
        self._driver.send(imu)

    def destroy_node(self):

        self._driver.close()

        super().destroy_node()


def main(args=None):

    rclpy.init(args=args)

    node = CanSenderNode()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        pass

    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
