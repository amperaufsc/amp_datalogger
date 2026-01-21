/**
 * @brief Fábrica de entradas analógicas ADS1115.
 *
 * Este arquivo fornece uma função de fábrica responsável por criar instâncias
 * de entradas analógicas baseadas no conversor ADC ADS1115. A função retorna
 * um ponteiro inteligente (`std::shared_ptr`) para a interface abstrata
 * AnalogInput, permitindo desacoplamento da implementação concreta.
 *
 * A fábrica encapsula os detalhes de inicialização do dispositivo I2C,
 * como caminho do barramento, endereço do ADS1115 e canal de leitura,
 * garantindo gerenciamento automático de memória.
 *
 * @param i2c_device Caminho do dispositivo I2C (ex: "/dev/i2c-1").
 * @param address Endereço I2C do ADS1115.
 * @param channel Canal analógico a ser utilizado (0–3).
 *
 * @return std::shared_ptr<AnalogInput> com a instância de entrada analógica criada.
 */

#pragma once

#include "io_manager/interfaces/analog_input.hpp"
#include <memory>
#include <string>

std::shared_ptr<AnalogInput> create_ads1115(
  const std::string& i2c_device,
  uint8_t address,
  uint8_t channel
);
