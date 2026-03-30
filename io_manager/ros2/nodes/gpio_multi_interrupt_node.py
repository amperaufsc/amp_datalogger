#!/usr/bin/env python3

"""
GpioMultiInterruptNode: Nó ROS 2 para publicar eventos de interrupção de múltiplos GPIOs.

Este nó monitora vários pinos GPIO configurados como entrada com interrupção e publica
um timestamp sempre que ocorre uma borda configurada (RISING, FALLING ou BOTH).
Ele permite que múltiplos GPIOs sejam monitorados de forma simultânea, utilizando a
interface de interrupção do pacote io_manager.

Parâmetros:
- gpio_names (list[str]): nomes simbólicos para cada GPIO monitorado
- gpio_pins (list[int]): números BCM dos pinos GPIO a serem monitorados
- edge (str, opcional): tipo de borda para detecção ('RISING', 'FALLING', 'BOTH'). Padrão: 'RISING'

Tópicos publicados (um por GPIO):
- /<gpio_name>/gpio/pin<gpio_pin>/interrupt
  Mensagem: builtin_interfaces/msg/Time
  Conteúdo: timestamp do evento de interrupção detectado
"""

import rclpy
from rclpy.node import Node
from builtin_interfaces.msg import Time
import io_manager_bindings as io


class GpioMultiInterruptNode(Node):

    def __init__(self):
        super().__init__('gpio_multi_interrupt_node')

        self.declare_parameter('gpio_names')
        self.declare_parameter('gpio_pins')
        self.declare_parameter('edge', 'RISING')

        names = self.get_parameter('gpio_names').value
        pins = self.get_parameter('gpio_pins').value
        edge_str = self.get_parameter('edge').value.upper()

        edge_map = {
            'RISING': io.Edge.RISING,
            'FALLING': io.Edge.FALLING,
            'BOTH': io.Edge.BOTH
        }
        if edge_str not in edge_map:
            raise ValueError(f"Invalid edge type: {edge_str}")
        self._edge = edge_map[edge_str]

        self._gpios = {}
        self._gpio_publishers = {}


        for name, pin in zip(names, pins):
            try:
                gpio = io.create_gpio_interrupt(pin, self._edge)
                gpio.start(self._make_callback(pin))

                topic = f"/{name}/gpio/pin{pin}/interrupt"
                pub = self.create_publisher(Time, topic, 10)

                self._gpios[pin] = gpio
                self._gpio_publishers[pin] = pub
                self.get_logger().info(f"GPIO {pin} ({name}) -> publishing events on '{topic}'")

            except RuntimeError as e:
                self.get_logger().warn( f"[GPIO Init] Failed to initialize GPIO {pin} ({name}). Skipping this pin. Error: {e}")
                continue

    def _make_callback(self, pin):
        def callback():
            msg = Time()
            sec, nanosec = self.get_clock().now().seconds_nanoseconds()
            msg.sec = sec
            msg.nanosec = nanosec
            self._gpio_publishers[pin].publish(msg)
            self.get_logger().debug(f"GPIO {pin} interrupt at {msg.sec}.{msg.nanosec}")
        return callback

    def destroy_node(self):
        self.get_logger().info("Stopping GPIO interrupts")
        for gpio in self._gpios.values():
            gpio.stop()
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    node = GpioMultiInterruptNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info("Keyboard interrupt, shutting down...")
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
