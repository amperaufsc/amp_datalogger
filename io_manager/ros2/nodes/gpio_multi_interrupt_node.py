#!/usr/bin/env python3

"""
GpioMultiInterruptNode: Nó ROS 2 para publicar eventos de interrupção GPIO.

- Monitora múltiplos GPIOs via interrupção.
- Cada interrupção gera uma mensagem contendo apenas o timestamp do evento.
- A interrupção é considerada um evento de nível alto configurado no driver.

Parâmetros:
- gpio_pins (list[int]): lista de pinos BCM
- edge (str): tipo de borda ('RISING', 'FALLING', 'BOTH')

Tópicos:
- /gpio/pin<bcm_pin>/interrupt
  Mensagem: builtin_interfaces/msg/Time
"""

import rclpy
from rclpy.node import Node
from builtin_interfaces.msg import Time

import io_manager_bindings as io


class GpioMultiInterruptNode(Node):

    def __init__(self):
        super().__init__('gpio_multi_interrupt_node')

        self.declare_parameter('gpio_pins', [17])
        self.declare_parameter('edge', 'RISING')

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

        for pin in pins:
            topic = f"/gpio/pin{pin}/interrupt"

            pub = self.create_publisher(Time, topic, 10)

            gpio = io.create_gpio_interrupt(pin, self._edge)
            gpio.start(self._make_callback(pin))

            self._gpios[pin] = gpio
            self._gpio_publishers[pin] = pub

            self.get_logger().info(f"GPIO {pin} -> publishing events on '{topic}'")

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
