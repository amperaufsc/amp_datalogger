"""
SocketCanDriver: Interface de comunicação CAN utilizando SocketCAN.

Esta classe encapsula a biblioteca python-can, fornecendo uma interface simples
para abertura da interface CAN, envio e recepção de frames, além do controle do
ciclo de vida da conexão.

Características principais:
- Abertura e fechamento da interface SocketCAN
- Envio de frames CAN
- Recepção de frames CAN com timeout configurável
- Limpeza da fila de recepção
- Suporte ao protocolo Context Manager (with)

Funcionamento:
- A interface SocketCAN é aberta através do método open().
- Frames CAN podem ser enviados utilizando send().
- Frames recebidos são convertidos para o modelo CanFrame.
- A interface pode ser utilizada com a instrução "with", garantindo o
  fechamento automático da conexão.

Observações:
- Requer uma interface SocketCAN configurada no sistema operacional.
- Lança RuntimeError caso operações sejam realizadas com a interface fechada.
"""

from __future__ import annotations
from typing import Optional

import can
from can_frame import CanFrame


class SocketCanDriver:
    def __init__(
        self,
        channel: str = "can0",
        receive_timeout: Optional[float] = None,
    ):
        self._channel = channel
        self._timeout = receive_timeout

        self._bus: Optional[can.BusABC] = None

    @property
    def is_open(self) -> bool:
        return self._bus is not None

    def open(self) -> None:
        if self.is_open:
            return

        self._bus = can.interface.Bus(
            interface="socketcan",
            channel=self._channel,
        )

    def close(self) -> None:
        if self._bus is not None:
            self._bus.shutdown()
            self._bus = None

    def send(self, frame: CanFrame) -> None:
        if self._bus is None:
            raise RuntimeError("SocketCAN interface is not open.")

        message = can.Message(
            arbitration_id=frame.arbitration_id,
            data=frame.data,
            is_extended_id=False,
            dlc=frame.dlc,
        )

        self._bus.send(message)

    def receive(self) -> Optional[CanFrame]:
        if self._bus is None:
            raise RuntimeError("SocketCAN interface is not open.")

        message = self._bus.recv(timeout=self._timeout)

        if message is None:
            return None

        return CanFrame(
            arbitration_id=message.arbitration_id,
            data=bytes(message.data),
            dlc=message.dlc,
        )

    def flush(self) -> None:
        if self._bus is None:
            raise RuntimeError("SocketCAN interface is not open.")

        while self._bus.recv(timeout=0.0) is not None:
            pass

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()