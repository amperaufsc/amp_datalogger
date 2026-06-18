from pathlib import Path
import sys

PROJECT_ROOT = Path(__file__).resolve().parent.parent

sys.path.insert(0, str(PROJECT_ROOT / "src"))

from can_driver.algorithms.dbc_parser import CanFrame, DbcParser

dbc = DbcParser(
    PROJECT_ROOT / "dbc" / "amp226.dbc"
)

frame = dbc.encode(
    "STEERING_DATA",
    {
        "SteeringAngle": 15.3,
        "LimitSwitchLeft": False,
        "LimitSwitchRight": True,
    },
)

print("=== Encoded Frame ===")
print(f"ID  : 0x{frame.arbitration_id:X}")
print(f"DLC : {frame.dlc}")
print(f"DATA: {frame.data.hex(' ')}")