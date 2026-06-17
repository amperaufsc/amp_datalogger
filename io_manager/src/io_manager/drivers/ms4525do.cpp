#include "io_manager/drivers/ms4525do.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>

MS4525DO::MS4525DO(const char* device, uint8_t address)
    : device(device), address(address) {}

    
bool MS4525DO::init() {
    fd = open(device, O_RDWR);
    if (fd < 0) {
        perror("open");
        return false;
    }

    if (ioctl(fd, I2C_SLAVE, address) < 0) {
        perror("ioctl");
        close(fd);
        fd = -1;
        return false;
    }

    uint8_t buffer[4];
    if (::read(fd, buffer, 4) != 4) {
        std::cerr << "Sensor not responding\n";
        close(fd);
        fd = -1;
        return false;
    }

    return true;
}

PitotData MS4525DO::read() {
    uint8_t data[4];
    PitotData result{0, 0, false};

    if (::read(fd, data, 4) != 4)
        return result;

    uint8_t status = data[0] >> 6;
    if (status != 0)
        return result;

    uint16_t raw_pressure = ((data[0] & 0x3F) << 8) | data[1];
    uint16_t raw_temp = (data[2] << 3) | (data[3] >> 5);

    float Pmin = -1.0f;
    float Pmax = 1.0f;

    float pressure_raw = ((raw_pressure - 1638.0f) * (Pmax - Pmin) / 13107.0f) + Pmin;
    float pressure = pressure_raw - offset;

    float temperature = (raw_temp * 200.0f / 2047.0f) - 50.0f;

    result.pressure = pressure;
    result.temperature = temperature;
    result.valid = true;

    return result;
}

MS4525DO::~MS4525DO() {
    if (fd >= 0) {
        close(fd);
    }
}