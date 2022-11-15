#include "engine/vk_engine.h"
#include <iostream>
#include "engine/compute/device/device.hpp"
#include "pretty_io.hpp"

int main(int argc, char *argv[]) {
  io::printColorTest();
  {
    walrus::VulkanEngine engine{walrus::ALL};
    engine.run();
  }
  return 0;
}
