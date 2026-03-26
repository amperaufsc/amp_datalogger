#include "io_manager/drivers/ads1115.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include <stdexcept>

static constexpr uint8_t REG_CONVERSION = 0x00;
static constexpr uint8_t REG_CONFIG     = 0x01;

static constexpr uint16_t OS_SINGLE     = 0x8000;
static constexpr uint16_t MODE_SINGLE   = 0x0100;
static constexpr uint16_t PGA_4_096V    = 0x0200;
static constexpr uint16_t COMP_DISABLE = 0x0003;
static constexpr uint16_t DR_8SPS   = 0x0000;
static constexpr uint16_t DR_16SPS  = 0x0020;
static constexpr uint16_t DR_32SPS  = 0x0040;
static constexpr uint16_t DR_64SPS  = 0x0060;
static constexpr uint16_t DR_128SPS = 0x0080;
static constexpr uint16_t DR_250SPS = 0x00A0;
static constexpr uint16_t DR_475SPS = 0x00C0;
static constexpr uint16_t DR_860SPS = 0x00E0;

Ads1115::Ads1115(const std::string& i2c_device,
                 uint8_t i2c_address,
                 uint8_t channel)
: _fd(-1),
  _i2c_address(i2c_address),
  _channel(channel)
{
  if (channel > 3) {
    throw std::invalid_argument("Ads1115: channel must be 0..3");
  }

  _fd = ::open(i2c_device.c_str(), O_RDWR);
  if (_fd < 0) {
    throw std::runtime_error("Ads1115: failed to open I2C device");
  }

  if (::ioctl(_fd, I2C_SLAVE, _i2c_address) < 0) {
    ::close(_fd);
    throw std::runtime_error("Ads1115: failed to set I2C address");
  }
}

Ads1115::~Ads1115() {
  if (_fd >= 0) {
    ::close(_fd);
  }
}

int Ads1115::read() {
  return read_channel(_channel);
}

int Ads1115::read_channel(uint8_t channel) {
  if (channel > 3) {
    throw std::invalid_argument("Ads1115: channel must be 0..3");
  }

  uint16_t mux = 0x4000 | (channel << 12);

  uint16_t config =
      OS_SINGLE |
      mux |
      PGA_4_096V |
      MODE_SINGLE |
      DR_860SPS |
      COMP_DISABLE;

  write_register(REG_CONFIG, config);

  usleep(9000);  // ~8ms @ 128 SPS

  uint16_t raw = read_register(REG_CONVERSION);

  if (raw & 0x8000) {
    raw -= 1 << 16;
  }

  return static_cast<int>(raw);
}

void Ads1115::write_register(uint8_t reg, uint16_t value) {
  uint8_t buf[3] = {
    reg,
    static_cast<uint8_t>(value >> 8),
    static_cast<uint8_t>(value & 0xFF)
  };

  if (::write(_fd, buf, 3) != 3) {
    throw std::runtime_error("Ads1115: failed to write register");
  }
}

uint16_t Ads1115::read_register(uint8_t reg) {
  uint8_t buf[2];

  if (::write(_fd, &reg, 1) != 1) {
    throw std::runtime_error("Ads1115: failed to select register");
  }

  if (::read(_fd, buf, 2) != 2) {
    throw std::runtime_error("Ads1115: failed to read register");
  }

  return (buf[0] << 8) | buf[1];
}
