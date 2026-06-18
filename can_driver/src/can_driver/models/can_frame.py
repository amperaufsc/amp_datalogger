from dataclasses import dataclass


@dataclass(slots=True)
class CanFrame:
    arbitration_id: int
    data: bytes
    dlc: int