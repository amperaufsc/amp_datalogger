#include "io_manager/drivers/libgpiod_gpio_output.hpp"
#include <stdexcept>

LibgpiodGpioOutput::LibgpiodGpioOutput(unsigned int bcm_pin,
                                       bool initial_value) {
    _chip = gpiod_chip_open_by_name("gpiochip0");
    if (!_chip) {
        throw std::runtime_error("Erro ao abrir gpiochip");
    }
    _line = gpiod_chip_get_line(_chip, bcm_pin);

    if (!_line) {
        gpiod_chip_close(_chip);
        throw std::runtime_error("Erro ao obter linha GPIO");
    }

    if (gpiod_line_request_output(
            _line, "io_manager", initial_value) < 0) {
        gpiod_chip_close(_chip);
        throw std::runtime_error("Erro ao configurar GPIO como output");
    }
}

void LibgpiodGpioOutput::write(bool value) {
    gpiod_line_set_value(_line, value ? 1 : 0);
}

LibgpiodGpioOutput::~LibgpiodGpioOutput() {
    if (_line) gpiod_line_release(_line);
    if (_chip) gpiod_chip_close(_chip);
}
