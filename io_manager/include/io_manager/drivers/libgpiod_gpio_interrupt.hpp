#pragma once

#include "io_manager/interfaces/gpio_interrupt.hpp"

#include <gpiod.h>
#include <atomic>
#include <thread>
#include <sys/eventfd.h>

/**
 * @brief Implementação de interrupção GPIO usando libgpiod.
 *
 * Usa a interface character device do Linux (/dev/gpiochipX)
 * e espera eventos de borda configuráveis.
 */
class LibgpiodGpioInterrupt : public GpioInterrupt {
public:
    /**
     * @param chip_name Nome do gpiochip (ex: "gpiochip0")
     * @param line_num  Número do GPIO (BCM)
     * @param edge      Tipo de borda (RISING, FALLING, BOTH)
     */
    LibgpiodGpioInterrupt(const char* chip_name,
                          unsigned int line_num,
                          Edge edge = Edge::FALLING);

    ~LibgpiodGpioInterrupt() override;

    void start(Callback cb) override;
    void stop() override;

private:
    void eventLoop();

    const char* _chip_name;
    unsigned int _line_num;
    Edge _edge;

    gpiod_chip* _chip{nullptr};
    gpiod_line* _line{nullptr};

    int _stop_fd{-1};

    Callback _callback;

    std::atomic<bool> _running{false};
    std::thread _thread;
};

