#include "io_manager/drivers/mux.hpp"

#include <iostream>

Mux::Mux(uint8_t pinA, uint8_t pinB, uint8_t pinC)
    : pinA_(pinA), pinB_(pinB), pinC_(pinC)
{
}

Mux::~Mux()
{
    if (lineA_) gpiod_line_release(lineA_);
    if (lineB_) gpiod_line_release(lineB_);
    if (lineC_) gpiod_line_release(lineC_);
    if (chip_) gpiod_chip_close(chip_);
}

bool Mux::init()
{
    chip_ = gpiod_chip_open_by_name("gpiochip0");
    if (!chip_)
        return false;

    lineA_ = gpiod_chip_get_line(chip_, pinA_);
    lineB_ = gpiod_chip_get_line(chip_, pinB_);
    lineC_ = gpiod_chip_get_line(chip_, pinC_);

    if (!lineA_ || !lineB_ || !lineC_)
        return false;

    if (gpiod_line_request_output(lineA_, "mux", 0) < 0) return false;
    if (gpiod_line_request_output(lineB_, "mux", 0) < 0) return false;
    if (gpiod_line_request_output(lineC_, "mux", 0) < 0) return false;

    return true;
}

void Mux::set(uint8_t state)
{
    state &= 0x07;

    int a = (state & 0x01) ? 1 : 0;
    int b = (state & 0x02) ? 1 : 0;
    int c = (state & 0x04) ? 1 : 0;

    gpiod_line_set_value(lineA_, a);
    gpiod_line_set_value(lineB_, b);
    gpiod_line_set_value(lineC_, c);
}