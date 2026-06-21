"""
DbcParser: Interface para codificação e decodificação de mensagens CAN utilizando DBC.

Esta classe encapsula a biblioteca cantools, fornecendo uma interface simples
para carregar um arquivo DBC, codificar sinais em frames CAN e decodificar
frames CAN em seus respectivos sinais.

Características principais:
- Carregamento de arquivos DBC
- Decodificação de frames CAN
- Codificação de sinais em frames CAN
- Consulta de mensagens por nome ou Frame ID
- Listagem de todas as mensagens definidas no DBC

Funcionamento:
- O arquivo DBC é carregado durante a inicialização.
- A decodificação utiliza o Arbitration ID do frame para localizar a
  mensagem correspondente.
- A codificação recebe o nome da mensagem e os sinais desejados,
  produzindo um frame CAN pronto para transmissão.

Observações:
- A implementação utiliza a biblioteca cantools.
- O Frame ID utilizado corresponde ao Arbitration ID definido no DBC.
"""

from pathlib import Path

import cantools

from can_frame import CanFrame


class DbcParser:
    def __init__(self, dbc_path: str | Path):
        self._db = cantools.database.load_file(str(dbc_path))

    def decode(self, frame: CanFrame) -> dict:
        return self._db.decode_message(
            frame.arbitration_id,
            frame.data,
        )

    def encode(self, message_name: str, signals: dict) -> CanFrame:
        message = self._db.get_message_by_name(message_name)

        data = message.encode(signals)

        return CanFrame(
            arbitration_id=message.frame_id,
            data=data,
            dlc=len(data),
        )

    def get_message(self, name: str):
        return self._db.get_message_by_name(name)

    def get_message_by_id(self, frame_id: int):
        return self._db.get_message_by_frame_id(frame_id)

    def list_messages(self):
        return self._db.messages