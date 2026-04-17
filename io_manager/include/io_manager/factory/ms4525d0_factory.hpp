/**
 * @brief Fábrica para criação de sensores MS4525DO (Pitot).
 *
 * Esta classe implementa o padrão Factory para instanciar diferentes
 * implementações da interface IMs4525DO com base em um identificador
 * de tipo. Permite desacoplar a lógica de criação do sensor da aplicação.
 *
 * Características principais:
 * - Criação dinâmica de sensores MS4525DO a partir de uma string (type)
 * - Suporte a múltiplas implementações (ex: I2C real, mock para testes)
 * - Retorno via std::unique_ptr garantindo gerenciamento seguro de memória
 *
 * Métodos principais:
 * - create(): cria e retorna uma instância de IMs4525DO conforme o tipo solicitado
 *
 * Observações:
 * - Novas implementações devem ser adicionadas no método create()
 * - Pode exigir parâmetros adicionais (ex: file descriptor I2C)
 * - Retorna nullptr ou lança exceção caso o tipo não seja suportado
 */
#pragma once

#include <memory>
#include <string>

#include "io_manager/interfaces/ims4525do.hpp"

class MS4525DOFactory {
public:
    static std::unique_ptr<IMs4525DO> create(const std::string& type, int i2c_fd);
private:
    float offset = 0.0f;
};