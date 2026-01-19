#include "io_manager/drivers/libgpiod_gpio_interrupt.hpp"
#include <iostream>
#include <unistd.h>

int main() {
    LibgpiodGpioInterrupt irq("gpiochip0", 17);

    irq.start([]() {
        std::cout << "GPIO interrompido!" << std::endl;
    });

    while (true) {
        sleep(1);
    }
}
