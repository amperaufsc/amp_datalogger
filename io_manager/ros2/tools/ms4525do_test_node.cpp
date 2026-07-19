#include <chrono>
#include <iostream>
#include <thread>

#include "io_manager/drivers/ms4525do.hpp"

int main()
{
    MS4525DO sensor(
        1,          // I2C bus
        0x28,       // I2C address
        -1000.0,    // Min pressure (Pa)
        1000.0      // Max pressure (Pa)
    );

    if (!sensor.initialize())
    {
        std::cerr << "Failed to initialize MS4525DO" << std::endl;
        return -1;
    }

    std::cout << "MS4525DO initialized successfully." << std::endl;

    while (true)
    {
        auto measurement = sensor.read();

        if (!measurement.valid)
        {
            switch (measurement.status)
            {
            case MS4525DO::Status::I2C_ERROR:
                std::cout << "I2C communication error" << std::endl;
                break;

            case MS4525DO::Status::STALE_DATA:
                std::cout << "Sensor returned stale data" << std::endl;
                break;

            case MS4525DO::Status::COMMAND_MODE:
                std::cout << "Sensor is in command mode" << std::endl;
                break;

            case MS4525DO::Status::DIAGNOSTIC_FAULT:
                std::cout << "Sensor diagnostic fault" << std::endl;
                break;

            default:
                break;
            }
        }
        else
        {
            std::cout
                << "Pressure: "
                << measurement.differential_pressure_pa
                << " Pa | Temperature: "
                << measurement.temperature_c
                << " °C"
                << std::endl;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(100));
    }

    return 0;
}