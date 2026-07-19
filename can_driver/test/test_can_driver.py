from pathlib import Path
import sys
import time

PROJECT_ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(PROJECT_ROOT / "src"))

from can_driver.algorithms.dbc_parser import DbcParser
from can_driver.models.can_frame import CanFrame
from can_driver.drivers.socketcan_driver import SocketCanDriver

DBC_PATH = PROJECT_ROOT / "dbc" / "amp226.dbc"


def main():
    dbc = DbcParser(DBC_PATH)

    with SocketCanDriver(channel="vcan0", receive_timeout=1.0) as can_bus:

        print("CAN aberto:", can_bus.is_open)

        msg = dbc.encode(
            "MotorData",
            {
                "MotorRPM": 1500,
                "MotorTemperature": 65,
                "MotorTorque": 120,
                "InverterTemperature": 70,
            },
        )

        print("\nEnviando MotorData:", msg)
        can_bus.send(msg)

        time.sleep(0.1)

        frame = can_bus.receive()

        if frame:
            print("\nDEBUG IMEDIATO (send → receive):")
            print("ID:", hex(frame.arbitration_id))
            print("Raw:", frame.data)

            try:
                decoded = dbc.decode(frame)
                print("Decoded:", decoded)
            except Exception as e:
                print("Erro decode:", e)

        print("\nAguardando mensagem...")

        for _ in range(10):
            frame = can_bus.receive()
            if frame is None:
                continue

            print("\nFrame recebido:")
            print("ID:", hex(frame.arbitration_id))
            print("Raw:", frame.data)

            try:
                decoded = dbc.decode(frame)
                print("Decoded:", decoded)
            except Exception as e:
                print("Erro decode:", e)

            break

        print("\nTeste contínuo (Ctrl+C para parar)")
        try:
            while True:
                frame = can_bus.receive()
                if frame:
                    print(hex(frame.arbitration_id), dbc.decode(frame))

        except KeyboardInterrupt:
            print("\nFinalizando...")


if __name__ == "__main__":
    main()