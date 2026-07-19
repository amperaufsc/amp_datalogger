#include "can_driver/factory/can_factory.hpp"
#include "can_driver/drivers/socketcan_driver.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main() {
    auto can = CanFactory::create("socketcan");

    if (!can) {
        std::cerr << "Erro ao criar driver CAN\n";
        return 1;
    }

    if (!can->open("can0")) {
        std::cerr << "Erro ao abrir CAN\n";
        return 1;
    }

    auto driver = dynamic_cast<SocketCanDriver*>(can.get());

    if (!driver) {
        std::cerr << "Erro no cast para SocketCanDriver\n";
        return 1;
    }

    driver->start_rx();

    while (true) {
        struct can_frame frame;

        if (driver->receive(frame)) {
            std::cout << "ID: 0x"
                      << std::hex << frame.can_id
                      << " DLC: " << std::dec << (int)frame.can_dlc
                      << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}