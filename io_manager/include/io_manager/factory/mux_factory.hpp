/**
 * @brief Fábrica de objetos MUX.
 *
 * Este arquivo fornece uma função para criar instâncias de MUX
 * utilizando ponteiros inteligentes (std::unique_ptr), garantindo
 * gerenciamento automático de memória.
 *
 * @param pinA Pino de seleção A (LSB).
 * @param pinB Pino de seleção B.
 * @param pinC Pino de seleção C (MSB).
 * @return std::unique_ptr<IMux> com a instância criada.
 */

#pragma once

#include <memory>
#include <cstdint>

#include "io_manager/interfaces/imux.hpp"

std::unique_ptr<IMux> create_mux(
    uint8_t pinA,
    uint8_t pinB,
    uint8_t pinC);