/**
 * @brief Driver para leitura do sensor MS4525DO via I2C.
 *
 * Esta classe implementa a leitura de dados do sensor de pressão diferencial
 * MS4525DO utilizando um descritor de arquivo I2C já inicializado. A leitura
 * retorna valores de pressão e temperatura encapsulados em uma estrutura
 * PitotData.
 *
 * Características principais:
 * - Leitura simples de dados via I2C
 * - Conversão dos dados brutos para pressão e temperatura
 * - Interface leve focada em leitura direta do sensor
 *
 * Parâmetros:
 * - i2c_fd (int): descritor de arquivo do dispositivo I2C já configurado
 *
 * Métodos principais:
 * - read(): realiza a leitura do sensor e retorna os dados interpretados
 *
 * Observações:
 * - Assume que o descritor I2C já está corretamente configurado (endereço, modo)
 * - Não realiza gerenciamento de abertura/fechamento do dispositivo
 * - Pode ser adaptado para implementar a interface IMs4525DO, se necessário
 */

#pragma once

#include <cstdint>
#include <string>

class MS4525DO
{
public:

    enum class Status
    {
        OK,
        STALE_DATA,
        COMMAND_MODE,
        DIAGNOSTIC_FAULT,
        I2C_ERROR
    };

    struct Measurement
    {
        double differential_pressure_pa;
        double temperature_c;

        bool valid;
        Status status;
    };

    MS4525DO(
        int bus = 1,
        uint8_t address = 0x28,
        double pressure_min_pa = -1000.0,
        double pressure_max_pa = 1000.0);

    ~MS4525DO();

    bool initialize();

    Measurement read();

private:
    int bus_;
    uint8_t address_;
    int fd_;

    double pressure_min_pa_;
    double pressure_max_pa_;

    bool readRaw(
        uint16_t &pressure_counts,
        uint16_t &temperature_counts,
        uint8_t &status);
};