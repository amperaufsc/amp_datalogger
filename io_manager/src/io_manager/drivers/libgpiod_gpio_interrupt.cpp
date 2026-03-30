#include "io_manager/drivers/libgpiod_gpio_interrupt.hpp"

#include <poll.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>

LibgpiodGpioInterrupt::LibgpiodGpioInterrupt(
    const char* chip_name,
    unsigned int line_num,
    Edge edge)
: _chip_name(chip_name),
  _line_num(line_num),
  _edge(edge)
{
    _chip = gpiod_chip_open_by_name(_chip_name);
    if (!_chip) {
        throw std::runtime_error("Failed to open gpio chip");
    }

    _line = gpiod_chip_get_line(_chip, _line_num);
    if (!_line) {
        gpiod_chip_close(_chip);
        throw std::runtime_error("Failed to get gpio line");
    }

    int ret = 0;
    switch (_edge) {
        case Edge::RISING:
            ret = gpiod_line_request_rising_edge_events(_line, "io_manager");
            break;
        case Edge::FALLING:
            ret = gpiod_line_request_falling_edge_events(_line, "io_manager");
            break;
        case Edge::BOTH:
            ret = gpiod_line_request_both_edges_events(_line, "io_manager");
            break;
    }

    if (ret) {
        gpiod_chip_close(_chip);
        throw std::runtime_error("Failed to request gpio events");
    }

    _stop_fd = eventfd(0, EFD_NONBLOCK);
    if (_stop_fd < 0) {
        gpiod_line_release(_line);
        gpiod_chip_close(_chip);
        throw std::runtime_error("Failed to create eventfd");
    }
}

LibgpiodGpioInterrupt::~LibgpiodGpioInterrupt() {
    stop();

    if (_stop_fd >= 0) {
        close(_stop_fd);
    }

    if (_line) {
        gpiod_line_release(_line);
    }

    if (_chip) {
        gpiod_chip_close(_chip);
    }
}

void LibgpiodGpioInterrupt::start(Callback cb) {
    if (_running) {
        return;
    }

    _callback = cb;
    _running = true;

    _thread = std::thread(&LibgpiodGpioInterrupt::eventLoop, this);
}

void LibgpiodGpioInterrupt::stop() {
    if (!_running) {
        return;
    }

    _running = false;

    uint64_t one = 1;
    write(_stop_fd, &one, sizeof(one));

    if (_thread.joinable()) {
        _thread.join();
    }
}

void LibgpiodGpioInterrupt::eventLoop() {
    pollfd fds[2]{};

    fds[0].fd = gpiod_line_event_get_fd(_line);
    fds[0].events = POLLIN;

    fds[1].fd = _stop_fd;
    fds[1].events = POLLIN;

    while (_running) {
        int ret = poll(fds, 2, -1);
        if (ret <= 0) {
            continue;
        }

        if (fds[1].revents & POLLIN) {
            break;
        }

        if (fds[0].revents & POLLIN) {
            gpiod_line_event event;
            if (gpiod_line_event_read(_line, &event) == 0) {
                if (_callback) {
                    _callback();
                }
            }
        }
    }
}
