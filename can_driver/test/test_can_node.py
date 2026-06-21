import cantools
import can

db = cantools.database.load_file("../dbc/amp226.dbc")

msg = db.get_message_by_name("MotorData")

data = msg.encode({
    "MotorRPM": 1500,
    "MotorTemperature": 65,
    "MotorTorque": 120,
    "InverterTemperature": 70,
})

frame = can.Message(
    arbitration_id=msg.frame_id,
    data=data,
    is_extended_id=False
)

print(frame)