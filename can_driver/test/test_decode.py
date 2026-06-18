from pathlib import Path
import sys

PROJECT_ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(PROJECT_ROOT / "src"))

from can_driver.algorithms.dbc_parser import DbcParser

dbc = DbcParser(PROJECT_ROOT / "dbc" / "amp226.dbc")

frame = dbc.encode(
    "STEERING_DATA",
    {
        "SteeringAngle": 32.5,
        "LimitSwitchLeft": True,
        "LimitSwitchRight": False,
    },
)

signals = dbc.decode(frame)

print("=== Decoded Signals ===")

for name, value in signals.items():
    print(f"{name:<20}: {value}")