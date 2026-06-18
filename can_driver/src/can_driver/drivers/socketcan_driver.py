from __future__ import annotations

from typing import Optional

import can

from can_driver.models.can_frame import CanFrame


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