#include "engine/vk_engine.h"
#include "engine/compute/device/device.hpp"
#include "pretty_io.hpp"

#include <iostream>

/**
 * for now, this is a wrapper to run the engine.
 * perhaps later this will be refactored to present an options/configuration screen
 */
int main(int argc, char *argv[]) {
//  io::printColorTest();
  {
    walrus::VulkanEngine engine{walrus::DeviceTask::ALL};
    engine.run();
  }
  return 0;
}
