#!/usr/bin/env python3

"""
Ads1115TestNode: Nó ROS 2 para leitura e publicação de valores analógicos do ADS1115.

Este nó inicializa um conversor analógico-digital ADS1115 via I2C utilizando
as bindings do pacote io_manager. Ele realiza leituras periódicas de um canal
analógico específico e publica os valores lidos em um tópico ROS 2.

Características:
- Comunicação com o ADS1115 via barramento I2C.
- Leitura de um único canal analógico por instância do nó.
- Publicação periódica do valor bruto (raw) do ADC.

Parâmetros:
- i2c_device (str): caminho do dispositivo I2C (ex: "/dev/i2c-1")
- i2c_address (int): endereço I2C do ADS1115 (ex: 0x48)
- channel (int): canal analógico do ADS1115 (0–3)

Tópicos:
- ads1115/channel<channel>
  Mensagem: std_msgs/msg/Int32
  Conteúdo: valor bruto lido do ADC
"""

import rclpy
from rclpy.node import Node

from std_msgs.msg import Int32

import io_manager_bindings as io


class Ads1115TestNode(Node):

    def __init__(self):
        super().__init__('ads1115_test_node')

        self.declare_parameter('i2c_device', '/dev/i2c-1')
        self.declare_parameter('i2c_address', 0x48)
        self.declare_parameter('channel', 0)

        i2c_device = self.get_parameter('i2c_device').value
        i2c_address = self.get_parameter('i2c_address').value
        channel = self.get_parameter('channel').value

        try:
            self._ads = io.create_ads1115(
                i2c_device,
                i2c_address,
                channel
            )

            self.get_logger().info(
                f'ADS1115 inicializado em {i2c_device} '
                f'addr=0x{i2c_address:02X} '
                f'canal={channel}'
            )

        except Exception as e:
            self.get_logger().fatal(f'Erro ao iniciar ADS1115: {e}')
            raise

        self._pub = self.create_publisher(
            Int32,
            f'ads1115/channel{channel}',
            10
        )

        self._timer = self.create_timer(0.1, self.read_adc)

    def read_adc(self):
        try:
            value = self._ads.read()

            msg = Int32()
            msg.data = value
            self._pub.publish(msg)

            self.get_logger().debug(f'ADC = {value}')

        except Exception as e:
            self.get_logger().error(f'Erro na leitura do ADS1115: {e}')


def main(args=None):
    rclpy.init(args=args)
    node = Ads1115TestNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
