/**
 * @brief Interface abstrata para sensor de pressão diferencial MS4525DO (Pitot).
 *
 * Esta interface define a estrutura de dados e os métodos necessários para
 * interação com sensores da família MS4525DO, tipicamente utilizados como
 * tubos de Pitot para medição de pressão diferencial e temperatura.
 *
 * Características principais:
 * - Leitura de pressão diferencial (float)
 * - Leitura de temperatura (float)
 * - Indicação de validade dos dados (status do sensor)
 * - Abstração da implementação de comunicação (ex: I2C)
 *
 * Estrutura de dados:
 * - PitotData:
 *     pressure    -> pressão diferencial medida
 *     temperature -> temperatura medida (precisão reduzida)
 *     valid       -> indica se a leitura é válida
 *
 * Métodos principais:
 * - init(): inicializa o sensor e a interface de comunicação
 * - read(): realiza a leitura dos dados do sensor
 *
 * Observações:
 * - A interpretação dos dados depende do modelo/configuração do sensor
 * - Implementações concretas devem tratar erros de comunicação e status do sensor
 */
#pragma once

#include <cstdint>

struct PitotData {
    float pressure;
    float temperature;
    bool valid;
};

class IMs4525DO
{
public:
    virtual ~IMs4525DO() = default;

    virtual bool init() = 0;
    virtual PitotData read() = 0;
};