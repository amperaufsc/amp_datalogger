#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "io_manager/drivers/ms4525do.hpp"

int main() {
    MS4525DO sensor;

    if (!sensor.init()) {
        std::cerr << "Sensor not found!" << std::endl;
        return 1;
    }

    while (true) {
        auto data = sensor.read();

        if (data.valid) {
            std::cout << "Pressure: " << data.pressure
                      << " | Temp: " << data.temperature << std::endl;
        } else {
            std::cout << "Invalid read" << std::endl;
        }

        usleep(100000);
    }
}