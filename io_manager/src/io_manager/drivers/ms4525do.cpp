#include "io_manager/drivers/ms4525do.hpp"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cmath>
#include <cstring>
#include <stdexcept>

MS4525DO::MS4525DO(
    int bus,
    uint8_t address,
    double pressure_min_pa,
    double pressure_max_pa)
    : bus_(bus),
      address_(address),
      fd_(-1),
      pressure_min_pa_(pressure_min_pa),
      pressure_max_pa_(pressure_max_pa)
{
}

MS4525DO::~MS4525DO()
{
    if (fd_ >= 0)
    {
        close(fd_);
    }
}

bool MS4525DO::initialize()
{
    std::string device = "/dev/i2c-" + std::to_string(bus_);

    fd_ = open(device.c_str(), O_RDWR);

    if (fd_ < 0)
    {
        return false;
    }

    if (ioctl(fd_, I2C_SLAVE, address_) < 0)
    {
        close(fd_);
        fd_ = -1;
        return false;
    }


    uint8_t test_byte;

    int ret = ::read(fd_, &test_byte, 1);

    if (ret < 0)
    {
        close(fd_);
        fd_ = -1;
        return false;
    }

    return true;
}

bool MS4525DO::readRaw(
    uint16_t &pressure_counts,
    uint16_t &temperature_counts,
    uint8_t &status)
{
    uint8_t data[4];

    int result = ::read(fd_, data, 4);

    if (result != 4)
    {
        return false;
    }

    status = (data[0] >> 6) & 0x03;

    pressure_counts =
        ((data[0] & 0x3F) << 8) |
        data[1];

    temperature_counts =
        (data[2] << 3) |
        (data[3] >> 5);

    return true;
}

MS4525DO::Measurement MS4525DO::read()
{
    Measurement m;

    uint16_t pressure_counts;
    uint16_t temperature_counts;
    uint8_t raw_status;

    if (!readRaw(
            pressure_counts,
            temperature_counts,
            raw_status))
    {
        m.valid = false;
        m.status = Status::I2C_ERROR;
        return m;
    }

    switch (raw_status)
    {
        case 0:
            m.status = Status::OK;
            break;

        case 1:
            m.valid = false;
            m.status = Status::STALE_DATA;
            return m;

        case 2:
            m.valid = false;
            m.status = Status::COMMAND_MODE;
            return m;

        case 3:
            m.valid = false;
            m.status = Status::DIAGNOSTIC_FAULT;
            return m;
    }

    constexpr double OUTPUT_MIN = 1638.0;
    constexpr double OUTPUT_MAX = 14745.0;

    m.differential_pressure_pa =
        ((pressure_counts - OUTPUT_MIN) *
        (pressure_max_pa_ - pressure_min_pa_) /
        (OUTPUT_MAX - OUTPUT_MIN))
        + pressure_min_pa_;

    m.temperature_c =
        (temperature_counts * 200.0 / 2047.0)
        - 50.0;

    m.valid = true;

    return m;
}