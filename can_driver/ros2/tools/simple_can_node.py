"""
SteeringToCan: Nó ROS 2 para envio de dados de direção via barramento CAN.

Este nó recebe dados provenientes do sistema de aquisição (sensor_mux/data),
filtra o sinal correspondente à direção (steering) com base no estado do MUX
e transmite o valor através do barramento CAN utilizando SocketCAN.

Características principais:
- Assinatura do tópico /sensor_mux/data (Int32MultiArray)
- Filtragem por estado do MUX para identificar o sinal de direção
- Saturação do valor para faixa válida de int16
- Empacotamento binário do dado (little-endian)
- Envio de frames CAN via python-can (SocketCAN)
- Log de envio e tratamento de erro na comunicação CAN

Parâmetros ROS 2:
- can_interface (string): interface CAN a ser utilizada (ex: "can0")

Tópicos assinados:
- /sensor_mux/data (std_msgs/msg/Int32MultiArray)
  Conteúdo:
    [0] -> estado do MUX
    [1] -> valor do canal 3
    [2] -> valor do canal 2 (utilizado para steering)

Funcionamento:
- O nó verifica se o estado do MUX corresponde ao canal de direção (estado 4)
- Caso válido, extrai o valor do canal 2
- Aplica saturação para o intervalo [-32768, 32767]
- Converte o valor para formato binário (int16, little-endian)
- Envia o dado via CAN com ID configurado

Observações:
- O ID CAN está fixo em 0x4A1
- Apenas frames com mux_state == 4 são transmitidos
- Requer suporte ao SocketCAN no sistema operacional
"""

#!/usr/bin/env python3

import rclpy
from rclpy.node import Node

from std_msgs.msg import Int32MultiArray

import can
import struct


class SteeringToCan(Node):
    def __init__(self):
        super().__init__('steering_to_can')

        self.declare_parameter('can_interface', 'can0')

        can_interface = self.get_parameter('can_interface').value

        self.bus = can.Bus(
            channel=can_interface,
            interface='socketcan'
        )

        self.can_id = 0b10010100001  # 1185 0x4A1

        self.sub = self.create_subscription(
            Int32MultiArray,
            'sensor_mux/data',
            self.callback,
            10
        )

        self.get_logger().info('Steering -> CAN node started')

    def callback(self, msg: Int32MultiArray):
        if len(msg.data) < 3:
            return

        mux_state = msg.data[0]
        ch2_value = msg.data[2]

        if mux_state != 4:
            return

        value = int(max(min(ch2_value, 32767), -32768))

        data = struct.pack('<h', value)

        message = can.Message(
            arbitration_id=self.can_id,
            data=data,
            is_extended_id=False
        )

        try:
            self.bus.send(message)
            self.get_logger().info(f'Steering CAN: {value}')
        except can.CanError as e:
            self.get_logger().error(f'CAN send failed: {e}')


def main(args=None):
    rclpy.init(args=args)
    node = SteeringToCan()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()