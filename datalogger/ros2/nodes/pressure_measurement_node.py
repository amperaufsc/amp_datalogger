#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64

from src.datalogger.algorithms.pressure_measurement import PressureMeasurement

class PressureSensorNode(Node):

    def __init__(self):
        super().__init__('pressure_sensor_node')

        self.declare_parameters(
            namespace='',
            parameters=[
                ('sensor1_a', 26.0),
                ('sensor1_b', -12.7),
                ('sensor2_a', 27.93),
                ('sensor2_b', -13.52),
                ('publish_rate_hz', 40.0),
                ('min_voltage', 0.0),
                ('max_voltage', 5.0),
            ]
        )

        self.min_voltage = self.get_parameter('min_voltage').value
        self.max_voltage = self.get_parameter('max_voltage').value

        self.sensor_1 = PressureMeasurement(
            self.get_parameter('sensor1_a').value,
            self.get_parameter('sensor1_b').value
        )

        self.sensor_2 = PressureMeasurement(
            self.get_parameter('sensor2_a').value,
            self.get_parameter('sensor2_b').value
        )

        self.pressure_1 = None
        self.pressure_2 = None

        self.create_subscription(
            Float64,
            '/sensor_mux/brake_0',
            self.sensor_1_callback,
            10
        )

        self.create_subscription(
            Float64,
            '/sensor_mux/brake_1',
            self.sensor_2_callback,
            10
        )

        self.pub_1 = self.create_publisher(Float64, '/pressure_sensor_1/pressure', 10)
        self.pub_2 = self.create_publisher(Float64, '/pressure_sensor_2/pressure', 10)

        publish_rate = self.get_parameter('publish_rate_hz').value
        self.create_timer(1.0 / publish_rate, self.publish_pressures)

        self.get_logger().info("PressureSensorNode subscribing direct sensor topics")

    def sensor_1_callback(self, msg: Float64):
        v = msg.data
        if self.min_voltage <= v <= self.max_voltage:
            self.pressure_1 = self.sensor_1.update(v)

    def sensor_2_callback(self, msg: Float64):
        v = msg.data
        if self.min_voltage <= v <= self.max_voltage:
            self.pressure_2 = self.sensor_2.update(v)

    def publish_pressures(self):

        if self.pressure_1 is not None:
            msg = Float64()
            msg.data = float(self.pressure_1)
            self.pub_1.publish(msg)

        if self.pressure_2 is not None:
            msg = Float64()
            msg.data = float(self.pressure_2)
            self.pub_2.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = PressureSensorNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()