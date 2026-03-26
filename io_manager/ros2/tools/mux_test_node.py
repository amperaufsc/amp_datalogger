#!/usr/bin/env python3

"""
MuxTestNode: Nó ROS 2 para controle e teste de um multiplexador (MUX).

Este nó inicializa um MUX digital (3 pinos de seleção) utilizando as bindings
do pacote io_manager. Ele realiza a varredura periódica dos canais (0–7)
e publica o canal atualmente selecionado.

Características:
- Controle de MUX via GPIO (A, B, C).
- Varredura sequencial dos canais (round-robin).
- Publicação do canal ativo.

Parâmetros:
- pin_a (int): GPIO do pino A (LSB)
- pin_b (int): GPIO do pino B
- pin_c (int): GPIO do pino C (MSB)
- period (float): período de varredura em segundos

Tópicos:
- mux/channel
  Mensagem: std_msgs/msg/Int32
  Conteúdo: canal atual (0–7)
"""

import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32

import io_manager_bindings as io


class MuxTestNode(Node):

    def __init__(self):
        super().__init__('mux_test_node')

        self.declare_parameter('pin_a', 17)
        self.declare_parameter('pin_b', 27)
        self.declare_parameter('pin_c', 22)
        self.declare_parameter('period', 0.1)

        pin_a = self.get_parameter('pin_a').value
        pin_b = self.get_parameter('pin_b').value
        pin_c = self.get_parameter('pin_c').value
        period = self.get_parameter('period').value

        self._mux = io.create_mux(pin_a, pin_b, pin_c)

        if not self._mux.init():
            raise RuntimeError("Falha ao inicializar MUX")

        self._channel = 0

        self._pub = self.create_publisher(Int32, "mux/channel", 10)
        self._timer = self.create_timer(period, self.update)

    def update(self):
        self._mux.set(self._channel)

        msg = Int32()
        msg.data = self._channel
        self._pub.publish(msg)

        self._channel = (self._channel + 1) % 8


def main(args=None):
    rclpy.init(args=args)
    node = MuxTestNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()