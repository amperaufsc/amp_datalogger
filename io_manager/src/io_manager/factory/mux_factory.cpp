#include "io_manager/factory/mux_factory.hpp"
#include "io_manager/drivers/mux.hpp"

std::unique_ptr<IMux> create_mux(
    uint8_t pinA,
    uint8_t pinB,
    uint8_t pinC)
{
    return std::make_unique<Mux>(pinA, pinB, pinC);
}