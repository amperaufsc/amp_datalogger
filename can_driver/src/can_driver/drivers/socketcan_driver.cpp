#include "can_driver/drivers/socketcan_driver.hpp"

#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <iostream>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>

SocketCanDriver::SocketCanDriver()
    : socket_fd_(-1), running_(false) {}

SocketCanDriver::~SocketCanDriver() {
    stop_rx();
    close(); 
}

bool SocketCanDriver::open(const std::string& interface) {
    socket_fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_fd_ < 0) {
        perror("socket");
        return false;
    }

    if (fcntl(socket_fd_, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl");
        return false;
    }

    struct ifreq ifr{};
    std::strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    if (ioctl(socket_fd_, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl");
        return false;
    }

    struct sockaddr_can addr{};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socket_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return false;
    }

    return true;
}

void SocketCanDriver::close() {
    if (socket_fd_ >= 0) {
        ::close(socket_fd_);
        socket_fd_ = -1;
    }
}

bool SocketCanDriver::send(const struct can_frame& frame) {
    int nbytes = write(socket_fd_, &frame, sizeof(frame));

    if (nbytes < 0) {
        perror("write");
        return false;
    }

    return nbytes == sizeof(frame);
}

bool SocketCanDriver::receive(struct can_frame& frame) {
    std::lock_guard<std::mutex> lock(queue_mutex_);

    if (rx_queue_.empty()) {
        return false;
    }

    frame = rx_queue_.front();
    rx_queue_.pop();
    return true;
}

void SocketCanDriver::rx_loop() {
    struct pollfd fds[1];
    fds[0].fd = socket_fd_;
    fds[0].events = POLLIN;

    while (running_) {
        int ret = poll(fds, 1, 100);

        if (ret < 0) {
            if (errno == EINTR)
                continue;

            perror("poll");
            break;
        }

        if (ret > 0 && (fds[0].revents & POLLIN)) {
            struct can_frame frame;

            int nbytes = read(socket_fd_, &frame, sizeof(frame));

            if (nbytes < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    continue;

                perror("read");
                continue;
            }

            if (nbytes == sizeof(frame)) {
                std::lock_guard<std::mutex> lock(queue_mutex_);

                if (rx_queue_.size() < MAX_QUEUE_SIZE) {
                    rx_queue_.push(frame);
                }
            }
        }
    }
}

void SocketCanDriver::start_rx() {
    if (running_) return;

    running_ = true;
    rx_thread_ = std::thread(&SocketCanDriver::rx_loop, this);
}

void SocketCanDriver::stop_rx() {
    if (!running_) return;

    running_ = false;

    if (rx_thread_.joinable()) {
        rx_thread_.join();
    }
}