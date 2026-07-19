from os import system
from random import randint
from time import sleep
while True:
    control_word = f"{randint(0, 65535):04X}"
    inverter_status = f"{randint(0, 255):02X}"
    tms = f"{randint(0, 255):02X}"
    current_state = f"{randint(0, 3):02X}"
    ecu = f"{randint(0, 15):04X}"
    inverter = f"{randint(0, 7):02X}"

    inverter_temperatureMSB = f"{randint(0, 255):02X}"
    inverter_temperatureLSB = f"{randint(0, 255):02X}"
    motor_torqueMSB = f"{randint(0, 255):02X}"
    motor_torqueLSB = f"{randint(0, 255):02X}"
    motor_temperatureMSB = f"{randint(0, 255):02X}"
    motor_temperatureLSB = f"{randint(0, 255):02X}"
    motor_rpmMSB = f"{randint(0, 255):02X}"
    motor_rpmLSB = f"{randint(0, 255):02X}"

    battery_current = f"{randint(0, 4294967295):08X}"
    inverter_current = f"{randint(0, 4294967295):08X}"

    battery_voltage = f"{randint(0, 4294967295):08X}"
    inverter_voltage = f"{randint(0, 4294967295):08X}"

    max_temperature = f"{randint(0, 255):02X}"
    cebolinha = f"{randint(0, 1):02X}"
    brake_pedal = f"{randint(0, 127):02X}"
    acc_pedal = f"{randint(0, 127):02X}"
    state_of_charge = f"{randint(0, 255):02X}"
    max_cell_voltage = f"{randint(0, 255):02X}"
    avg_cell_voltage = f"{randint(0, 255):02X}"
    min_cell_voltage = f"{randint(0, 255):02X}"
    system("cansend vcan0 120#" + "".join((control_word, inverter_status, tms, current_state, ecu, inverter)))
    system("cansend vcan0 420#" + "".join((inverter_temperatureMSB, inverter_temperatureLSB, motor_torqueMSB, motor_torqueLSB, motor_temperatureMSB, motor_temperatureLSB,motor_rpmMSB, motor_rpmLSB)))
    system("cansend vcan0 220#" + "".join((battery_current, inverter_current)))
    system("cansend vcan0 421#" + "".join((battery_voltage, inverter_voltage)))
    system("cansend vcan0 121#" + "".join((max_temperature, cebolinha, brake_pedal, acc_pedal,state_of_charge, max_cell_voltage, avg_cell_voltage, min_cell_voltage)))
    sleep(randint(1, 50)/10)