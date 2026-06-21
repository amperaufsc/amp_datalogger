/**
 * @brief Driver CAN baseado em SocketCAN para comunicação em sistemas Linux.
 *
 * Esta classe implementa a interface CanInterface utilizando a API SocketCAN,
 * permitindo envio e recepção de frames CAN através de interfaces como "can0".
 * A recepção é realizada de forma assíncrona por uma thread dedicada, que armazena
 * os frames recebidos em uma fila protegida por mutex.
 *
 * Características principais:
 * - Abertura e fechamento de interface CAN via SocketCAN
 * - Envio e recepção de frames CAN (struct can_frame)
 * - Thread dedicada para recepção contínua (rx_loop)
 * - Buffer interno com fila para desacoplamento da recepção
 * - Proteção de acesso concorrente com mutex
 * - Controle de execução da thread via std::atomic
 *
 * Métodos principais:
 * - open(): inicializa e conecta à interface CAN
 * - close(): encerra a comunicação e libera recursos
 * - send(): envia um frame CAN
 * - receive(): lê um frame da fila interna
 * - start_rx(): inicia a thread de recepção
 * - stop_rx(): interrompe a thread de recepção
 *
 * Observações:
 * - A fila de recepção possui tamanho máximo limitado (MAX_QUEUE_SIZE)
 * - Frames excedentes podem ser descartados dependendo da implementação
 * - Requer suporte ao SocketCAN no sistema operacional
 */

#pragma once

#include "can_driver/interfaces/can_interface.hpp"

#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

class SocketCanDriver : public CanInterface {
public:
    SocketCanDriver();
    ~SocketCanDriver();

    bool open(const std::string& interface) override;
    void close() override;

    bool send(const struct can_frame& frame) override;
    bool receive(struct can_frame& frame) override;

    void start_rx();
    void stop_rx();

private:
    void rx_loop();

    int socket_fd_;

    std::thread rx_thread_;
    std::atomic<bool> running_;

    std::queue<struct can_frame> rx_queue_;
    std::mutex queue_mutex_;

    static constexpr size_t MAX_QUEUE_SIZE = 1000;
};