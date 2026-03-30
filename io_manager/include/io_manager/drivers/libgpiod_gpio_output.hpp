/**
 * @brief Implementa uma saída GPIO utilizando a biblioteca libgpiod.
 *
 * Esta classe permite configurar e controlar um pino GPIO do Raspberry Pi
 * através da interface libgpiod, garantindo compatibilidade com a interface
 * abstrata GpioOutput.
 *
 * @param bcm_pin Número do pino BCM a ser configurado como saída.
 * @param initial_value Valor inicial da saída (true = HIGH, false = LOW). Padrão: false.
 */

#pragma once

#include "io_manager/interfaces/gpio_output.hpp"
#include <gpiod.h>

class LibgpiodGpioOutput : public GpioOutput {
public:
    explicit LibgpiodGpioOutput(unsigned int bcm_pin,
                              bool initial_value = false);

    void write(bool value) override;

    ~LibgpiodGpioOutput() override;

private:
    gpiod_chip* _chip{nullptr};
    gpiod_line* _line{nullptr};
};
