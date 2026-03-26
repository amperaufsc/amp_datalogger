/**
 * @brief Interface abstrata para multiplexadores digitais (MUX).
 *
 * Esta classe define a interface base para qualquer implementação de MUX,
 * permitindo inicializar o hardware e selecionar um estado sem se preocupar
 * com a implementação específica dos pinos ou da biblioteca utilizada.
 *
 * @param state Valor do MUX a ser definido (cada bit representa um pino de controle).
 *
 * @note Todas as implementações devem sobrescrever os métodos init() e set().
 */
#pragma once

#include <cstdint>

class IMux
{
public:
    virtual ~IMux() = default;

    virtual bool init() = 0;
    virtual void set(uint8_t state) = 0;
};