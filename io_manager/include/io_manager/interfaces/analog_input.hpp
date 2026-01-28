/**
 * @brief Interface genérica para entradas analógicas.
 *
 * Esta interface define um contrato comum para dispositivos capazes
 * de realizar leituras analógicas, como conversores ADC externos
 * (ex: ADS1115) ou ADCs internos da plataforma.
 *
 * Implementações concretas devem fornecer a lógica necessária para
 * adquirir o valor analógico do hardware subjacente e retornar a
 * leitura em formato inteiro.
 */

#pragma once

#include <cstdint>

class AnalogInput {
public:
  virtual ~AnalogInput() = default;

  /**
   * @brief Realiza a leitura do valor analógico.
   *
   * @return Valor lido do conversor analógico.
   */
  virtual int read() = 0;

  /**
   * @brief Lê explicitamente um canal do ADC
   *
   * @param channel Canal analógico (0–3)
   */
  virtual int read_channel(uint8_t channel) = 0;
};