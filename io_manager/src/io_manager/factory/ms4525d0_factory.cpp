#include "io_manager/factory/ms4525d0_factory.hpp"
#include "io_manager/drivers/ms4525do.hpp"

std::unique_ptr<IMs4525DO> MS4525DOFactory::create(const std::string& type, int i2c_fd)
{
    if (type == "ms4525do") {
        return std::make_unique<MS4525DO>(i2c_fd);
    }
    return nullptr;
}
