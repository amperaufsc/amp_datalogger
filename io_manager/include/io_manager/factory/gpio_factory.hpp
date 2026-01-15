/**
 * @brief Fábrica de objetos GPIO.
 *
 * Este arquivo fornece funções para criar instâncias de GPIO de entrada e saída
 * usando ponteiros inteligentes (std::unique_ptr), garantindo gerenciamento
 * automático de memória.
 *
 * @param bcm_pin Número do pino BCM a ser configurado.
 * @param initial_value Valor inicial para saídas GPIO (true = HIGH, false = LOW). Padrão: false.
 * @return std::unique_ptr<GpioOutput> ou std::unique_ptr<GpioInput> com a instância criada.
 */

#pragma once

#include <memory>
#include "io_manager/interfaces/gpio_output.hpp"
#include "io_manager/interfaces/gpio_input.hpp"

std::unique_ptr<GpioOutput> create_gpio_output(
    unsigned int bcm_pin,
    bool initial_value = false);

std::unique_ptr<GpioInput> create_gpio_input(
    unsigned int bcm_pin);
