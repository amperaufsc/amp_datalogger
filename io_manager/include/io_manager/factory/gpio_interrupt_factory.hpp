#pragma once

#include <memory>

#include "io_manager/interfaces/gpio_interrupt.hpp"

/**
 * @brief Cria uma instância de GpioInterrupt para o backend padrão.
 *
 * @param bcm_pin Número do GPIO (BCM)
 * @param edge Tipo de borda que gera interrupção
 * @return std::shared_ptr<GpioInterrupt>
 */
std::shared_ptr<GpioInterrupt>
create_gpio_interrupt(unsigned int bcm_pin, Edge edge);
