from pathlib import Path
import sys

PROJECT_ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(PROJECT_ROOT / "src"))

from can_driver.algorithms.dbc_parser import DbcParser

dbc = DbcParser(PROJECT_ROOT / "dbc" / "amp226.dbc")

print("=== Messages ===\n")

for message in dbc.list_messages():
    print(
        f"{message.name:<20}"
        f" ID: {message.frame_id:<5}"
        f" (0x{message.frame_id:X})"
        f" DLC: {message.length}"
    )

    for signal in message.signals:
        print(f"    - {signal.name}")

    print()