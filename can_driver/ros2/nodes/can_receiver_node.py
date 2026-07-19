#!/usr/bin/env python3

"""
CanReceiverNode: Nó ROS 2 para recepção, decodificação e publicação de mensagens CAN.

Este nó realiza a interface entre o barramento CAN (SocketCAN) e o ecossistema
ROS 2. Os frames recebidos são decodificados utilizando um arquivo DBC e
publicados automaticamente em tópicos ROS específicos para cada mensagem
definida no banco CAN.

Características principais:
- Recepção de frames CAN via SocketCAN
- Decodificação automática utilizando arquivo DBC
- Criação automática de publishers para todas as mensagens do DBC
- Publicação das mensagens decodificadas utilizando CanDecodedMessage
- Processamento assíncrono da recepção CAN em uma thread dedicada
- Tratamento de mensagens desconhecidas e erros de decodificação

Parâmetros ROS 2:
- dbc_path (string): caminho para o arquivo DBC utilizado na decodificação
- can_interface (string): interface SocketCAN (ex.: "can0" ou "vcan0")
- receive_timeout (double): tempo máximo de espera por um frame CAN

Tópicos publicados:
- /can/<NomeDaMensagem> (manager_msgs/msg/CanDecodedMessage)

Conteúdo da mensagem publicada:
- Timestamp da recepção
- Arbitration ID
- DLC
- Nome da mensagem CAN
- ECU remetente (Sender)
- Lista de sinais presentes na mensagem
- Unidade de cada sinal
- Tipo de cada sinal (numeric/string)
- Valor numérico ou textual correspondente

Funcionamento:
- Carrega o arquivo DBC informado
- Inicializa o driver SocketCAN
- Cria automaticamente um publisher para cada mensagem definida no DBC
- Executa uma thread dedicada para recepção contínua dos frames
- Cada frame recebido é:
    - identificado pelo Arbitration ID
    - decodificado utilizando o DBC
    - convertido para CanDecodedMessage
    - publicado no tópico correspondente

Observações:
- Frames inexistentes no DBC são ignorados.
- Cada mensagem CAN possui um tópico ROS exclusivo.
- A recepção CAN ocorre em thread separada do executor ROS.
"""

from pathlib import Path
import threading

import rclpy
from rclpy.node import Node
from ament_index_python.packages import get_package_share_directory

from dbc_parser import DbcParser
from socketcan_driver import SocketCanDriver
from can_topic_manager import CanTopicManager

class CanReceiverNode(Node):

    def __init__(self):
        super().__init__("can_receiver")

        default_dbc = str(
            Path(get_package_share_directory("can_driver"))
            / "dbc"
            / "amp226.dbc"
        )

        self.declare_parameter("dbc_path", default_dbc)
        self.declare_parameter("can_interface", "can0")
        self.declare_parameter("receive_timeout", 0.1)

        dbc_path = self.get_parameter("dbc_path").value
        can_interface = self.get_parameter("can_interface").value
        receive_timeout = self.get_parameter("receive_timeout").value

        self._dbc = DbcParser(Path(dbc_path))

        self._driver = SocketCanDriver(
            channel=can_interface,
            receive_timeout=receive_timeout,
        )

        self._driver.open()

        self._topic_manager = CanTopicManager(
            node=self,
            dbc=self._dbc,
        )

        self._running = True

        self._thread = threading.Thread(
            target=self._receive_loop,
            daemon=True,
        )
        self._thread.start()

        self.get_logger().info("CAN Receiver started.")

    def destroy_node(self):

        self._running = False

        if self._thread.is_alive():
            self._thread.join(timeout=1.0)

        self._driver.close()

        super().destroy_node()

    def _receive_loop(self):

        while rclpy.ok() and self._running:

            frame = self._driver.receive()

            if frame is None:
                continue

            try:
                self._topic_manager.publish(frame)

            except KeyError:
                continue

            except Exception as e:
                self.get_logger().warning(
                    f"Error processing frame 0x{frame.arbitration_id:X}: {e}"
                )


def main(args=None):

    rclpy.init(args=args)

    node = CanReceiverNode()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        pass

    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
