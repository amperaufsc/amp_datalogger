"""
BlinkNode: Nó ROS 2 para piscar um LED usando GPIO via io_manager_bindings.

- Inicializa parâmetros:
    - gpio_pin: número do pino BCM (padrão: 17)
    - period: intervalo de piscada em segundos (padrão: 1.0)
- Cria um objeto de saída GPIO usando a fábrica Python (`io.create_gpio_output`).
- Alterna o estado do LED a cada período definido.
- Loga cada mudança de estado no console.

Este nó é útil para testar a integração do ROS 2 com GPIO em Python.
"""

#!/usr/bin/env python3

import rclpy
from rclpy.node import Node

import io_manager_bindings as io


class BlinkNode(Node):
    def __init__(self):
        super().__init__('blink_node')

        self.declare_parameter('gpio_pin', 17)
        self.declare_parameter('period', 1.0)

        gpio_pin = self.get_parameter('gpio_pin').value
        period = self.get_parameter('period').value

        self.get_logger().info(
            f'Blink node iniciado | GPIO={gpio_pin} | period={period}s'
        )

        self.led = io.create_gpio_output(gpio_pin, False)

        self.state = False

        self.timer = self.create_timer(period, self.on_timer)

    def on_timer(self):
        self.state = not self.state
        self.led.write(self.state)

        self.get_logger().info(
            f'LED {"ON" if self.state else "OFF"}'
        )


def main(args=None):
    rclpy.init(args=args)
    node = BlinkNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == '__main__':
    main()
