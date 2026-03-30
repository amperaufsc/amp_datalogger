#include "io_manager/factory/analog_input_factory.hpp"
#include "io_manager/drivers/ads1115.hpp"

std::shared_ptr<AnalogInput> create_ads1115(
  const std::string& i2c_device,
  uint8_t address,
  uint8_t channel
) {
  return std::make_shared<Ads1115>(i2c_device, address, channel);
}
