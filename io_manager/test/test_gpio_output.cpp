#include "io_manager/drivers/libgpiod_gpio_output.hpp"
#include <unistd.h>

int main() {
    LibgpiodGpioOutput led(17);

    while (true) {
        led.write(true);
        sleep(1);
        led.write(false);
        sleep(1);
    }
}
