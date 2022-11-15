#include "engine/vk_engine.h"
#include <iostream>
#include "engine/compute/device/device.hpp"

int main(int argc, char *argv[]) {
  {
    walrus::VulkanEngine engine{walrus::DeviceTask::COMPUTE};
    engine.run();
  }
  return 0;
}
