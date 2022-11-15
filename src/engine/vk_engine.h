#pragma once

#include "engine/rendering/window/window.hpp"
#include "engine/compute/device/device.hpp"

#include "vk_types.h"
#include <vector>
#include <string>

namespace walrus {

  class VulkanEngine {
  public:

    explicit VulkanEngine(DeviceTask task = DeviceTask::ALL) { init(task); };

    ~VulkanEngine() { destroy(); }

    void init(DeviceTask task = DeviceTask::ALL);

    void init_vulkan();

    void init_swapchain();

    void init_commands();

    void destroy();

    void draw();

    void run();

  private:

    void runRender();

    void runCompute();


#ifdef NDEBUG
    const bool _enableValidationLayers = false;
#else
    const bool _enableValidationLayers = true;
#endif
    bool _isInitialized{false};
    int _frameNumber{0};
    VkInstance _instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
    const std::vector<const char *> _validationLayers = {"VK_LAYER_KHRONOS_validation"};

    Window _window{800, 600, "Vulkan Window"};
    VkSurfaceKHR _surface = VK_NULL_HANDLE;

    DeviceTask _task = DeviceTask::ALL;

    VkDevice _device = VK_NULL_HANDLE;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    DeviceInfo _deviceInfo{};
    std::vector<VkQueue> _queues{};
    std::vector<VkPhysicalDevice> _physicalDevices{};

    VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
    VkFormat _swapchainImageFormat{};
    std::vector<VkImage> _swapchainImages{};
    std::vector<VkImageView> _swapchainImageViews{};

    VkCommandPool _commandPool{};
    VkCommandBuffer _commandBuffer{};
  };
}
