#include <gpiod.h>
#include <iostream>
#include <unistd.h>  // sleep()

int main() {
    const char* chipname = "gpiochip0";
    const unsigned int line_num = 17; // BCM GPIO 17

    gpiod_chip* chip = gpiod_chip_open_by_name(chipname);
    if (!chip) {
        std::cerr << "Erro ao abrir gpiochip\n";
        return 1;
    }

    gpiod_line* line = gpiod_chip_get_line(chip, line_num);
    if (!line) {
        std::cerr << "Erro ao obter a linha GPIO\n";
        gpiod_chip_close(chip);
        return 1;
    }

    if (gpiod_line_request_output(line, "blink_led", 0) < 0) {
        std::cerr << "Erro ao configurar GPIO como saída\n";
        gpiod_chip_close(chip);
        return 1;
    }

    while (true) {
        gpiod_line_set_value(line, 1); // LED ON
        sleep(1);

        gpiod_line_set_value(line, 0); // LED OFF
        sleep(1);
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;
}
