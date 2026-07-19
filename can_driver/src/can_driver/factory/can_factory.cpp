#include "can_driver/factory/can_factory.hpp"
#include "can_driver/drivers/socketcan_driver.hpp"

std::unique_ptr<CanInterface> CanFactory::create(const std::string& type) {
    if (type == "socketcan") {
        return std::make_unique<SocketCanDriver>();
    }

    return nullptr;
}