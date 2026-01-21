#include "io_manager/drivers/ads1115.hpp"

#include <iostream>
#include <unistd.h>

int main() {
  try {
    Ads1115 ads_front("/dev/i2c-1", 0x48, 0);
    //Ads1115 ads_rear("/dev/i2c-1", 0x49, 1);

    while (true) {
      int v0 = ads_front.read();
      //int v1 = ads_rear.read();

      std::cout << "Front AIN0: " << v0
                << " | Rear AIN1: " /*<< v1*/ << std::endl;

      usleep(500000);
    }
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
