#include "command.hpp"

namespace walrus {
  VkCommandPoolCreateInfo CommandPool::createInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.queueFamilyIndex = queueFamilyIndex;
    info.flags = flags;
    return info;
  }


  VkCommandBufferAllocateInfo CommandBuffer::allocateInfo(VkCommandPool &pool,
                                                          uint32_t count,
                                                          VkCommandBufferLevel level
  ) {
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = pool;
    info.commandBufferCount = count; // number of buffers to allocate
    info.level = level;
    return info;
  }

} // namespace walrus