#!/usr/bin/env python3

"""
PressureSensorNode: Nó ROS 2 para conversão e publicação da pressão dos sensores de freio.

Este nó recebe os valores brutos provenientes do conversor A/D (ADS1115),
converte esses valores para tensão considerando a referência do ADC e o
divisor resistivo utilizado no hardware, e aplica a calibração de cada
sensor para obter a pressão correspondente.

Cada sensor possui uma calibração linear independente, definida pelos
coeficientes "a" e "b", permitindo compensar diferenças entre sensores.

O nó desacopla a taxa de aquisição da taxa de publicação:
- O cálculo da pressão é realizado sempre que uma nova leitura do ADC é recebida.
- A publicação ocorre em uma frequência fixa configurável.

Parâmetros:
- sensor1_a (float): coeficiente angular da calibração do sensor 1
- sensor1_b (float): coeficiente linear da calibração do sensor 1
- sensor2_a (float): coeficiente angular da calibração do sensor 2
- sensor2_b (float): coeficiente linear da calibração do sensor 2
- publish_rate_hz (float): frequência de publicação das pressões
- min_voltage (float): tensão mínima esperada do sensor
- max_voltage (float): tensão máxima esperada do sensor

Tópicos assinados:
- /sensor_mux/brake_0
  Mensagem: std_msgs/msg/Float64
  Conteúdo: valor bruto do ADC referente ao sensor de pressão 1

- /sensor_mux/brake_1
  Mensagem: std_msgs/msg/Float64
  Conteúdo: valor bruto do ADC referente ao sensor de pressão 2

Tópicos publicados:
- /pressure_sensor_1/pressure
  Mensagem: std_msgs/msg/Float64
  Conteúdo: pressão calculada do sensor 1

- /pressure_sensor_2/pressure
  Mensagem: std_msgs/msg/Float64
  Conteúdo: pressão calculada do sensor 2
"""

import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64

from src.datalogger.algorithms.pressure_measurement import PressureMeasurement

class PressureSensorNode(Node):

    def __init__(self):
        super().__init__('pressure_sensor_node')

        self.declare_parameters(
            namespace='',
            parameters=[
                ('sensor1_a', 26.0),
                ('sensor1_b', -12.7),
                ('sensor2_a', 27.93),
                ('sensor2_b', -13.52),
                ('publish_rate_hz', 40.0),
                ('min_voltage', 0.0),
                ('max_voltage', 5.0),
            ]
        )

        self.min_voltage = self.get_parameter('min_voltage').value
        self.max_voltage = self.get_parameter('max_voltage').value

        self.sensor_1 = PressureMeasurement(
            self.get_parameter('sensor1_a').value,
            self.get_parameter('sensor1_b').value
        )

        self.sensor_2 = PressureMeasurement(
            self.get_parameter('sensor2_a').value,
            self.get_parameter('sensor2_b').value
        )

        self.pressure_1 = None
        self.pressure_2 = None

        self.create_subscription(
            Float64,
            '/sensor_mux/brake_0',
            self.sensor_1_callback,
            10
        )

        self.create_subscription(
            Float64,
            '/sensor_mux/brake_1',
            self.sensor_2_callback,
            10
        )

        self.pub_1 = self.create_publisher(Float64, '/pressure_sensor_1/pressure', 10)
        self.pub_2 = self.create_publisher(Float64, '/pressure_sensor_2/pressure', 10)

        publish_rate = self.get_parameter('publish_rate_hz').value
        self.create_timer(1.0 / publish_rate, self.publish_pressures)

        self.get_logger().info("PressureSensorNode subscribing direct sensor topics")
    
    def adc_to_voltage(self, raw: float) -> float:
        adc_voltage = raw * 4.096 / 32768.0
        return adc_voltage * 1.5

    def sensor_1_callback(self, msg):
        raw = msg.data
        voltage = self.adc_to_sensor_voltage(raw)

        self.pressure_1 = self.sensor_1.update(voltage)

    def sensor_2_callback(self, msg: Float64):
        raw = msg.data
        voltage = self.adc_to_voltage(raw)

        self.pressure_2 = self.sensor_2.update(voltage)

    def publish_pressures(self):

        if self.pressure_1 is not None:
            msg = Float64()
            msg.data = float(self.pressure_1)
            self.pub_1.publish(msg)

        if self.pressure_2 is not None:
            msg = Float64()
            msg.data = float(self.pressure_2)
            self.pub_2.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = PressureSensorNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()