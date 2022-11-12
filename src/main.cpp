#include "engine/vk_engine.h"
#include <iostream>

int main(int argc, char *argv[]) {
  {
    walrus::VulkanEngine engine{};
    engine.run();
  }
  return 0;
}
