#ifndef WALRUS_COMPUTE_ENGINE_COMMAND_HPP
#define WALRUS_COMPUTE_ENGINE_COMMAND_HPP

#include <vk_types.h>

namespace walrus {

  struct CommandPool {
    static VkCommandPoolCreateInfo createInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
  };

  struct CommandBuffer {
    static VkCommandBufferAllocateInfo allocateInfo(VkCommandPool &pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  };

} // namespace walrus

#endif //WALRUS_COMPUTE_ENGINE_COMMAND_HPP
