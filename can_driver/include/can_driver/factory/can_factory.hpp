/**
 * @brief Fábrica para criação de interfaces CAN.
 *
 * Esta classe implementa o padrão Factory para instanciar diferentes
 * implementações da interface CanInterface com base em um identificador
 * de tipo. Permite desacoplar a lógica de criação do driver CAN do restante
 * do sistema.
 *
 * Características principais:
 * - Criação dinâmica de drivers CAN a partir de uma string (type)
 * - Suporte a múltiplas implementações (ex: SocketCAN, mocks, etc.)
 * - Retorno via std::unique_ptr garantindo gerenciamento seguro de memória
 *
 * Métodos principais:
 * - create(): cria e retorna uma instância de CanInterface conforme o tipo solicitado
 *
 * Observações:
 * - Novas implementações de CAN devem ser registradas/tratadas dentro do método create()
 * - Retorna nullptr ou lança exceção caso o tipo não seja suportado
 */

#pragma once

#include <memory>
#include <string>
#include "can_driver/interfaces/can_interface.hpp"

class CanFactory {
public:
    static std::unique_ptr<CanInterface> create(const std::string& type);
};