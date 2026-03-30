#include "io_manager/factory/gpio_factory.hpp"
#include "io_manager/drivers/libgpiod_gpio_output.hpp"
#include "io_manager/drivers/libgpiod_gpio_input.hpp"

std::unique_ptr<GpioOutput> create_gpio_output(
    unsigned int bcm_pin,
    bool initial_value) {
  return std::make_unique<LibgpiodGpioOutput>(bcm_pin, initial_value);
}

std::unique_ptr<GpioInput> create_gpio_input(
    unsigned int bcm_pin) {
  return std::make_unique<LibgpiodGpioInput>(bcm_pin);
}
