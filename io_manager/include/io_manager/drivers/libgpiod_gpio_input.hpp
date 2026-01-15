/**
 * @brief Implementa uma entrada GPIO utilizando a biblioteca libgpiod.
 *
 * Esta classe permite configurar e ler o estado de um pino GPIO do Raspberry Pi
 * através da interface libgpiod, garantindo compatibilidade com a interface
 * abstrata GpioInput.
 *
 * @param bcm_pin Número do pino BCM a ser configurado como entrada.
 */


#pragma once

#include "io_manager/interfaces/gpio_input.hpp"
#include <gpiod.h>

class LibgpiodGpioInput : public GpioInput {
public:
    explicit LibgpiodGpioInput(unsigned int bcm_pin);
    bool read() override;

    ~LibgpiodGpioInput() override;

private:
    gpiod_chip* _chip{nullptr};
    gpiod_line* _line{nullptr};
};
