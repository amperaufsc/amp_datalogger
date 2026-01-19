"""
GpioInterruptNode: Nó ROS 2 para monitorar interrupções de GPIO usando io_manager_bindings.

* Inicializa parâmetros:

  * bcm_pin: número do pino BCM a ser monitorado (padrão: 17)
  * edge: tipo de borda que dispara a interrupção

    * 'RISING'  → borda de subida
    * 'FALLING' → borda de descida
    * 'BOTH'    → ambas (padrão: 'RISING')

* Cria um objeto de interrupção GPIO usando a fábrica Python
  (`io.create_gpio_interrupt`), baseada em libgpiod.

* Registra um callback que é executado sempre que ocorre a interrupção
  no pino configurado.

* Publica uma mensagem do tipo `std_msgs/Bool` no tópico:

  * /gpio_interrupt

* Implementa shutdown limpo:

  * Encerra corretamente a thread de interrupção
  * Libera recursos do GPIO
  * Responde imediatamente a Ctrl+C e ao shutdown do ROS 2

Este nó é destinado a testes e validação de interrupções GPIO em sistemas
Linux (ex: Raspberry Pi) e serve como base para integração com sensores,
encoders, botões e sinais digitais orientados a evento.
"""


#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from std_msgs.msg import Bool

import io_manager_bindings as io


class GpioInterruptNode(Node):
    def __init__(self):
        super().__init__('gpio_interrupt_node')

        self.declare_parameter('bcm_pin', 17)
        self.declare_parameter('edge', 'RISING')

        bcm_pin = self.get_parameter('bcm_pin').value
        edge_str = self.get_parameter('edge').value.upper()

        edge_map = {
            'RISING': io.Edge.RISING,
            'FALLING': io.Edge.FALLING,
            'BOTH': io.Edge.BOTH
        }

        if edge_str not in edge_map:
            raise ValueError(f"Invalid edge type: {edge_str}")

        edge = edge_map[edge_str]

        self.publisher_ = self.create_publisher(
            Bool,
            'gpio_interrupt',
            10
        )

        self.get_logger().info(
            f"Initializing GPIO interrupt on BCM {bcm_pin}, edge={edge_str}"
        )

        self.gpio = io.create_gpio_interrupt(bcm_pin, edge)

        self.gpio.start(self._gpio_callback)

    def _gpio_callback(self):
        """
        Callback chamado a partir da interrupção GPIO
        """
        msg = Bool()
        msg.data = True

        self.publisher_.publish(msg)
        self.get_logger().info('GPIO interrupt triggered')

    def destroy_node(self):
        """
        Cleanup correto
        """
        self.get_logger().info('Stopping GPIO interrupt')
        self.gpio.stop()
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    node = GpioInterruptNode()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('Keyboard interrupt, shutting down...')
    finally:
        node.destroy_node()
        rclpy.shutdown()



if __name__ == '__main__':
    main()
