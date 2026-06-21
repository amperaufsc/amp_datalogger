"""
CanTopicManager: Gerenciador de tópicos ROS para mensagens CAN decodificadas.

Esta classe é responsável por criar automaticamente um publisher ROS para cada
mensagem definida no arquivo DBC e publicar os frames CAN recebidos após sua
decodificação.

Características principais:
- Criação automática de publishers a partir do DBC
- Associação entre Arbitration ID e publisher correspondente
- Conversão de frames CAN para CanDecodedMessage
- Preenchimento automático dos metadados da mensagem
- Suporte a sinais numéricos e textuais

Funcionamento:
- Durante a inicialização, percorre todas as mensagens presentes no DBC
- Cria um tópico ROS para cada mensagem no formato:
    /can/<NomeDaMensagem>
- Quando um frame é recebido:
    - identifica a mensagem correspondente pelo Arbitration ID
    - realiza a decodificação dos sinais
    - converte os dados para CanDecodedMessage
    - publica a mensagem no tópico correspondente

Observações:
- O nome dos tópicos é obtido diretamente do arquivo DBC.
- Cada Arbitration ID possui exatamente um publisher.
- Sinais numéricos e textuais são armazenados em vetores distintos.
"""

from manager_msgs.msg import CanDecodedMessage

class CanTopicManager:

    def __init__(self, node, dbc):

        self._node = node
        self._dbc = dbc

        self._publishers = {}

        self._create_publishers()

    def _create_publishers(self):

        for message in self._dbc.list_messages():

            topic = f"/can/{message.name}"

            self._publishers[message.frame_id] = self._node.create_publisher(
                CanDecodedMessage,
                topic,
                10,
            )

            self._node.get_logger().info(
                f"Created topic {topic}"
            )

    def publish(self, frame):

        message = self._dbc.get_message_by_id(
            frame.arbitration_id
        )

        signals = self._dbc.decode(frame)

        ros_msg = CanDecodedMessage()

        ros_msg.header.stamp = self._node.get_clock().now().to_msg()

        ros_msg.arbitration_id = frame.arbitration_id
        ros_msg.dlc = frame.dlc

        ros_msg.message_name = message.name
        ros_msg.sender = message.senders[0] if message.senders else ""

        for signal in message.signals:

            ros_msg.signal_names.append(signal.name)
            ros_msg.signal_units.append(signal.unit or "")

            value = signals[signal.name]

            if isinstance(value, (bool, int, float)):

                ros_msg.signal_types.append("numeric")
                ros_msg.numeric_values.append(float(value))
                ros_msg.string_values.append("")

            else:

                ros_msg.signal_types.append("string")
                ros_msg.numeric_values.append(0.0)
                ros_msg.string_values.append(str(value))

        self._publishers[
            frame.arbitration_id
        ].publish(ros_msg)