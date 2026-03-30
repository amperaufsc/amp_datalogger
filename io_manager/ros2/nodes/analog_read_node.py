#!/usr/bin/env python3

"""
AnalogReadNode: Nó ROS 2 para leitura de múltiplos sinais analógicos via ADS1115 e multiplexador.

Este nó permite adquirir sinais analógicos de várias fontes conectadas a um
conversor ADC ADS1115, possivelmente multiplexados por um MUX controlado
por três pinos GPIO. Cada canal é lido periodicamente e publicado em tópicos
separados para cada sinal.

Características principais:
- Configuração do ADS1115 via parâmetros ROS 2 (i2c_device, i2c_address, canais)
- Controle de um MUX analógico via pinos GPIO (a, b, c)
- Publicação de sinais individuais como tópicos Float32
- Gerenciamento de tempos de conversão e estabilidade do MUX
- Opção "strict" para falha crítica na inicialização do ADS1115

Parâmetros ROS 2:
- read_period_s (float): período de leitura de todos os canais (s)
- strict (bool, opcional): se True, falha no ADS1115 interrompe o nó
- ads1115.i2c_device (str): caminho do dispositivo I2C (ex: "/dev/i2c-1")
- ads1115.i2c_address (int): endereço I2C do ADS1115
- ads1115.channels (list[int]): lista de canais do ADS1115 a serem lidos
- ads1115.conversion_time_ms (float, opcional): tempo de conversão do ADC (ms)
- mux.select_pins.a/b/c (int): pinos GPIO para controle do MUX
- mux.map (dict): mapeamento de estados do MUX para sinais
- mux.settle_time_ms (float, opcional): tempo de estabilização do MUX (ms)

Tópicos publicados:
- /analog/<signal_name> (std_msgs/msg/Float32)
  Conteúdo: valor do sinal analógico lido do ADS1115 após multiplexação

Exemplo de uso:
- Configurar os parâmetros via YAML ou launch file
- O nó lê periodicamente os canais configurados e publica os valores
  correspondentes aos sinais no ROS 2
"""

import time

import rclpy
from rclpy.node import Node

from std_msgs.msg import Float32
from utils import unflatten_dict

import io_manager_bindings as io


class AnalogReadNode(Node):

    def __init__(self):
        super().__init__(
            'analog_read_node',
            automatically_declare_parameters_from_overrides=True
        )

        self.read_period = self.get_parameter('read_period_s').value
        self.strict = self.get_parameter_or('strict', False)

        ads_params = self.get_parameters_by_prefix('ads1115')
        ads_cfg = {k: v.value for k, v in ads_params.items()}

        if not ads_cfg:
            raise RuntimeError("ads1115 parameters not found")

        required_ads = ['i2c_device', 'i2c_address', 'channels']
        for key in required_ads:
            if key not in ads_cfg:
                raise RuntimeError(f"ads1115.{key} parameter missing")

        self.ads_channels = ads_cfg['channels']
        self.ads_conversion_time = ads_cfg.get(
            'conversion_time_ms', 2
        ) / 1000.0

        try:
            self.ads = io.create_ads1115(
                ads_cfg['i2c_device'],
                ads_cfg['i2c_address'],
                0
            )

            self.ads.read_channel(0)

            self.get_logger().info(
                f"ADS1115 OK | dev={ads_cfg['i2c_device']} "
                f"addr=0x{ads_cfg['i2c_address']:02X}"
            )

        except Exception as e:
            msg = f"ADS1115 init failed: {e}"

            if self.strict:
                self.get_logger().fatal(msg)
                raise
            else:
                self.get_logger().error(msg)
                self.ads = None

        mux_params = self.get_parameters_by_prefix('mux')
        mux_flat = {k: v.value for k, v in mux_params.items()}

        if not mux_flat:
            raise RuntimeError("mux parameters not found")

        mux_cfg = unflatten_dict(mux_flat)

        if 'select_pins' not in mux_cfg or 'map' not in mux_cfg:
            raise RuntimeError("mux.select_pins or mux.map missing")

        pins = mux_cfg['select_pins']
        self.mux_map = mux_cfg['map']
        self.mux_settle_time = mux_cfg.get(
            'settle_time_ms', 5
        ) / 1000.0

        for pin in ['a', 'b', 'c']:
            if pin not in pins:
                raise RuntimeError(f"mux.select_pins.{pin} missing")

        self.gpio_a = io.create_gpio_output(pins['a'])
        self.gpio_b = io.create_gpio_output(pins['b'])
        self.gpio_c = io.create_gpio_output(pins['c'])

        self.publishers_by_signal = {}
        self._create_publishers()

        self.create_timer(self.read_period, self._read_all)

        self.get_logger().info("AnalogReadNode initialized successfully")

    def _create_publishers(self):
        for state in self.mux_map.values():
            for _, signal_name in state.items():
                if signal_name not in self.publishers_by_signal:
                    topic = f"/analog/{signal_name}"
                    self.publishers_by_signal[signal_name] = self.create_publisher(
                        Float32,
                        topic,
                        10
                    )
                    self.get_logger().info(
                        f"Publishing '{signal_name}' on {topic}"
                    )

    def _set_mux(self, key: str):
        try:
            c, b, a = [int(bit) for bit in key]
        except ValueError:
            self.get_logger().error(f"Invalid mux key: {key}")
            return False

        self.gpio_a.write(a)
        self.gpio_b.write(b)
        self.gpio_c.write(c)

        time.sleep(self.mux_settle_time)
        return True

    def _read_all(self):
        if self.ads is None:
            return

        for mux_key, channels in self.mux_map.items():
            if not self._set_mux(mux_key):
                continue

            for ch in self.ads_channels:
                if ch not in channels:
                    continue

                signal_name = channels[ch]

                try:
                    value = self.ads.read_channel(ch)

                except RuntimeError as e:
                    self.get_logger().warn(
                        f"ADS read failed (mux={mux_key}, ch={ch}): {e}"
                    )
                    continue

                msg = Float32()
                msg.data = float(value)
                self.publishers_by_signal[signal_name].publish(msg)

                time.sleep(self.ads_conversion_time)

def main():
    rclpy.init()
    node = AnalogReadNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
