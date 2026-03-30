#!/usr/bin/env python3

"""
WheelSpeedNode: Nó ROS 2 para cálculo e publicação da velocidade angular da roda.

Este nó calcula a velocidade angular (rad/s) de uma roda a partir de eventos
de interrupção gerados por um encoder incremental conectado a um GPIO.
Cada evento recebido contém o timestamp do pulso do encoder, que é utilizado
para calcular a velocidade angular com base no número de setores por revolução.

O nó desacopla a taxa de cálculo da taxa de publicação:
- O cálculo é feito a cada evento do encoder
- A publicação ocorre em uma frequência fixa configurável

Parâmetros:
- sectors_per_rev (int): número de setores (pulsos) do encoder por revolução
- publish_rate_hz (float): frequência de publicação da velocidade angular
- max_omega (float): valor máximo permitido de velocidade angular (rad/s)

Tópicos assinados:
- /gpio/pin17/interrupt
  Mensagem: builtin_interfaces/msg/Time
  Conteúdo: timestamp do evento de encoder

Tópicos publicados:
- /wheel/angular_velocity
  Mensagem: std_msgs/msg/Float64
  Conteúdo: velocidade angular da roda em rad/s
"""

import math
import rclpy
from rclpy.node import Node

from builtin_interfaces.msg import Time
from std_msgs.msg import Float64

from src.datalogger.algorithms.wheel_speed_measurement import WheelSpeedMeasurement

class WheelSpeedNode(Node):
    def __init__(self):
        super().__init__('wheel_speed_node')

        self.declare_parameter('sectors_per_rev', 31)
        self.declare_parameter('publish_rate_hz', 60.0)
        self.declare_parameter('max_omega', 200.0)

        sectors = self.get_parameter('sectors_per_rev').value
        publish_rate = self.get_parameter('publish_rate_hz').value
        self.max_omega = self.get_parameter('max_omega').value

        self.wheel_speed = WheelSpeedMeasurement(sectors)
        self.current_omega = None 

        self.subscription = self.create_subscription(
            Time,
            '/gpio/pin17/interrupt',
            self.encoder_callback,
            10
        )

        self.publisher = self.create_publisher(
            Float64,
            '/wheel/angular_velocity',
            10
        )

        period = 1.0 / publish_rate
        self.timer = self.create_timer(period, self.publish_velocity)

        self.get_logger().info(
            f'WheelSpeedNode iniciado | pub_rate={publish_rate} Hz'
        )

    def encoder_callback(self, msg: Time):
        timestamp = msg.sec + msg.nanosec * 1e-9

        omega = self.wheel_speed.update(timestamp)
        if omega is None:
            return

        if omega > self.max_omega:
            return

        self.current_omega = omega

    def publish_velocity(self):
        if self.current_omega is None:
            return

        msg = Float64()
        msg.data = self.current_omega
        self.publisher.publish(msg)

        self.get_logger().debug(
            f'ω publicado: {self.current_omega:.2f} rad/s'
        )

def main(args=None):
    rclpy.init(args=args)
    node = WheelSpeedNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
