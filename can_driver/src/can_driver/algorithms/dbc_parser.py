from pathlib import Path

import cantools

from can_driver.models.can_frame import CanFrame


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