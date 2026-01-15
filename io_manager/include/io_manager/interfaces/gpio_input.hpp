/**
 * @brief Interface abstrata para entradas GPIO.
 *
 * Esta classe define a interface base para qualquer implementação de entrada
 * digital (GPIO). Permite ler o estado de um pino sem se preocupar com a
 * implementação específica do hardware.
 *
 * @note Todas as implementações devem sobrescrever o método read().
 */

#pragma once

class GpioInput {
public:
    virtual ~GpioInput() = default;
    virtual bool read() = 0;
};
