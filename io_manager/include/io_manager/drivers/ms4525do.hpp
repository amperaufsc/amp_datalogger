#pragma once

#include <cstdint>
#include <string>

class MS4525DO
{
public:

    enum class Status
    {
        OK,
        STALE_DATA,
        COMMAND_MODE,
        DIAGNOSTIC_FAULT,
        I2C_ERROR
    };

    struct Measurement
    {
        double differential_pressure_pa;
        double temperature_c;

        bool valid;
        Status status;
    };

    MS4525DO(
        int bus = 1,
        uint8_t address = 0x28,
        double pressure_min_pa = -1000.0,
        double pressure_max_pa = 1000.0);

    ~MS4525DO();

    bool initialize();

    Measurement read();

private:
    int bus_;
    uint8_t address_;
    int fd_;

    double pressure_min_pa_;
    double pressure_max_pa_;

    bool readRaw(
        uint16_t &pressure_counts,
        uint16_t &temperature_counts,
        uint8_t &status);
};