#include "engine/vk_engine.h"
#include "engine/compute/device/device.hpp"
#include "pretty_io.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
//  io::printColorTest();
  {
    walrus::VulkanEngine engine{walrus::DeviceTask::ALL};
    engine.run();
  }
  return 0;
}
