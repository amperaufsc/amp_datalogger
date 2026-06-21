/**
 * @brief Interface abstrata para comunicação CAN.
 *
 * Esta classe define a interface base para qualquer implementação de driver CAN,
 * permitindo abrir uma interface, enviar e receber frames sem depender da
 * implementação específica do hardware ou da biblioteca utilizada.
 *
 * Características principais:
 * - Abstração de diferentes backends CAN (ex: SocketCAN, simulação, etc.)
 * - Interface padronizada para envio e recepção de frames CAN
 * - Desacoplamento entre lógica da aplicação e driver CAN
 *
 * Métodos principais:
 * - open(): inicializa e conecta à interface CAN
 * - close(): encerra a comunicação
 * - send(): envia um frame CAN
 * - receive(): recebe um frame CAN
 *
 * Observações:
 * - Utiliza struct can_frame (linux/can.h) como formato padrão de dados
 * - Todas as implementações devem sobrescrever os métodos virtuais
 */

#pragma once

#include <linux/can.h>
#include <string>

class CanInterface {
public:
    virtual ~CanInterface() = default;

    virtual bool open(const std::string& interface) = 0;
    virtual void close() = 0;

    virtual bool send(const struct can_frame& frame) = 0;
    virtual bool receive(struct can_frame& frame) = 0;
};