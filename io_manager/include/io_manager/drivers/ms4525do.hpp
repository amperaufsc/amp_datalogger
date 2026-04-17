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

#include "io_manager/interfaces/ims4525do.hpp"

class MS4525DO : public IMs4525DO {
public:
    MS4525DO(const char* device = "/dev/i2c-1", uint8_t address = 0x28);

    bool init() override;
    PitotData read() override;
    ~MS4525DO();

private:
    const char* device;
    uint8_t address;
    int fd = -1;
    float offset = 0.0f;
};