#include "io_manager/drivers/libgpiod_gpio_input.hpp"
#include <stdexcept>

LibgpiodGpioInput::LibgpiodGpioInput(unsigned int bcm_pin) {
    _chip = gpiod_chip_open_by_name("gpiochip0");
    if (!_chip) {
        throw std::runtime_error("Erro ao abrir gpiochip");
    }

    _line = gpiod_chip_get_line(_chip, bcm_pin);
    if (!_line) {
        gpiod_chip_close(_chip);
        throw std::runtime_error("Erro ao obter linha GPIO");
    }

    if (gpiod_line_request_input(_line, "io_manager") < 0) {
        gpiod_chip_close(_chip);
        throw std::runtime_error("Erro ao configurar GPIO como input");
    }
}

bool LibgpiodGpioInput::read() {
    int value = gpiod_line_get_value(_line);
    if (value < 0) {
        throw std::runtime_error("Erro ao ler GPIO");
    }
    return value == 1;
}

LibgpiodGpioInput::~LibgpiodGpioInput() {
      if (_line) gpiod_line_release(_line);
    if (_chip) gpiod_chip_close(_chip);
}
