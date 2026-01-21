#!/usr/bin/env python3

"""
FakeEncoderNode: Nó ROS 2 para simulação de encoder incremental.

Este nó simula o comportamento de um encoder incremental publicando eventos
periódicos de interrupção com base em uma velocidade angular configurável.
Cada mensagem publicada representa a passagem por um setor do encoder e
contém apenas o timestamp do evento.

O nó é útil para:
- Testar algoritmos de cálculo de velocidade sem hardware real
- Validar pipelines de processamento baseados em interrupções GPIO
- Simular diferentes velocidades de rotação de forma determinística

Parâmetros:
- omega_rad_s (float): velocidade angular simulada em rad/s
- sectors_per_rev (int): número de setores (pulsos) por revolução do encoder

Tópicos publicados:
- /gpio/pin17/interrupt
  Mensagem: builtin_interfaces/msg/Time
  Conteúdo: timestamp do pulso do encoder simulado
"""

import math
import rclpy
from rclpy.node import Node
from builtin_interfaces.msg import Time
from std_msgs.msg import Float64

class FakeEncoderNode(Node):
    def __init__(self):
        super().__init__('fake_encoder_node')

        self.declare_parameter('omega_rad_s', 150.0)
        self.declare_parameter('sectors_per_rev', 31)

        self.omega = self.get_parameter('omega_rad_s').value
        self.sectors = self.get_parameter('sectors_per_rev').value
        
        self.pub = self.create_publisher(
            Time,
            '/gpio/pin17/interrupt',
            10
        )

        self.timer = None
        self.update_timer()

        self.get_logger().info(
            f'Simulando encoder | ω={self.omega:.2f} rad/s | '
            f'sectores={self.sectors}'
        )

    def update_timer(self):
        if self.omega <= 0.0:
            self.get_logger().warn('Velocidade <= 0, simulador parado')
            return

        period = (2.0 * math.pi) / (self.omega * self.sectors)

        if self.timer is not None:
            self.timer.cancel()

        self.timer = self.create_timer(period, self.publish_pulse)

    def publish_pulse(self):
        msg = self.get_clock().now().to_msg()
        self.pub.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = FakeEncoderNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
