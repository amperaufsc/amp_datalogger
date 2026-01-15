/**
 * @brief Interface abstrata para saídas GPIO.
 *
 * Esta classe define a interface base para qualquer implementação de saída
 * digital (GPIO). Permite escrever um valor lógico em um pino sem se preocupar
 * com a implementação específica do hardware.
 *
 * @param value Valor lógico a ser escrito no pino (true = HIGH, false = LOW).
 *
 * @note Todas as implementações devem sobrescrever o método write().
 */

#pragma once

class GpioOutput {
public:
    virtual ~GpioOutput() = default;
    virtual void write(bool value) = 0;
};
