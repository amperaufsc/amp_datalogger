/**
 * @brief Implementa um multiplexador (MUX) utilizando três pinos GPIO via libgpiod.
 *
 * Esta classe permite controlar um multiplexador digital de 3 bits conectado a
 * pinos GPIO do Raspberry Pi. Através da interface abstrata IMux, é possível
 * inicializar o hardware e selecionar o estado desejado do MUX.
 *
 * @param pinA Pino GPIO correspondente ao bit A do MUX.
 * @param pinB Pino GPIO correspondente ao bit B do MUX.
 * @param pinC Pino GPIO correspondente ao bit C do MUX.
 *
 * @note A classe utiliza a biblioteca libgpiod para manipulação direta dos
 * pinos GPIO, garantindo compatibilidade com sistemas baseados em Linux.
 */
#pragma once

#include <cstdint>
#include <gpiod.h>

#include "io_manager/interfaces/imux.hpp"

class Mux : public IMux
{
public:
    Mux(uint8_t pinA, uint8_t pinB, uint8_t pinC);
    ~Mux() override;

    bool init() override;
    void set(uint8_t state) override;

private:
    uint8_t pinA_;
    uint8_t pinB_;
    uint8_t pinC_;

    gpiod_chip* chip_{nullptr};
    gpiod_line* lineA_{nullptr};
    gpiod_line* lineB_{nullptr};
    gpiod_line* lineC_{nullptr};
};