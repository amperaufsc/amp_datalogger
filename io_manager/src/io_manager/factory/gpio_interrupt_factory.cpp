#include "io_manager/factory/gpio_interrupt_factory.hpp"

#include "io_manager/drivers/libgpiod_gpio_interrupt.hpp"

std::shared_ptr<GpioInterrupt>
create_gpio_interrupt(unsigned int bcm_pin, Edge edge) {
  return std::make_shared<LibgpiodGpioInterrupt>(
      "gpiochip0",
      bcm_pin,
      edge
  );
}
