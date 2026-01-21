/**
 * @brief Implementa uma entrada analógica utilizando o conversor ADS1115 via I2C.
 *
 * Esta classe encapsula a comunicação com o ADC ADS1115 através do barramento I2C,
 * permitindo a leitura de sinais analógicos em diferentes canais do dispositivo.
 * Ela implementa a interface abstrata AnalogInput, possibilitando sua integração
 * transparente com o restante do sistema de IO.
 *
 * A classe é responsável por:
 *  - Abrir e gerenciar o dispositivo I2C no Linux
 *  - Configurar e acessar registradores do ADS1115
 *  - Realizar leituras de conversão analógica em um canal específico
 *
 * @param i2c_device Caminho do dispositivo I2C (ex: "/dev/i2c-1")
 * @param i2c_address Endereço I2C do ADS1115
 * @param channel Canal analógico inicial a ser utilizado (0–3)
 */

#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "io_manager/interfaces/analog_input.hpp"

class Ads1115 : public AnalogInput {
public:
  Ads1115(const std::string& i2c_device,
          uint8_t i2c_address,
          uint8_t channel);

  ~Ads1115();

  int read() override;
  int read_channel(uint8_t channel);

private:
  int _fd;
  uint8_t _i2c_address;
  uint8_t _channel;

  void write_register(uint8_t reg, uint16_t value);
  uint16_t read_register(uint8_t reg);
};
