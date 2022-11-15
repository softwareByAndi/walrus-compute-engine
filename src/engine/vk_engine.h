#pragma once

#include "engine/rendering/window/window.hpp"
#include "engine/compute/device/device.hpp"

#include "vk_types.h"
#include <vector>
#include <string>

namespace walrus {

  class VulkanEngine {

  public:

    explicit VulkanEngine(DeviceTask task = ALL) { init(task); };

    ~VulkanEngine() { destroy(); }

    void init(DeviceTask task = ALL);

    void run();

    void destroy();

  private:

    void init_vulkan();

    void init_commands();

    void init_swapchain();

    void draw();

    void runRender();

    void runCompute();

    /// TASK NEUTRAL
    bool _isInitialized{false};
    DeviceTask _task = ALL;

    /// VALIDATION & DEBUG
#ifdef NDEBUG
    const bool _enableValidationLayers = false;
#else
    const bool _enableValidationLayers = true;
#endif
    VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
    const std::vector<const char *> _validationLayers = {"VK_LAYER_KHRONOS_validation"};

    /// COMPUTE
    VkInstance _instance = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    DeviceInfo _deviceInfo{};
    std::vector<VkQueue> _queues{};
    std::vector<VkPhysicalDevice> _physicalDevices{};

    VkCommandPool _commandPool{};
    VkCommandBuffer _commandBuffer{};

    /// RENDERING
    int _frameNumber{0};
    Window _window{800, 600, "Vulkan Window"};
    VkSurfaceKHR _surface = VK_NULL_HANDLE;

    VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
    VkFormat _swapchainImageFormat{};
    std::vector<VkImage> _swapchainImages{};
    std::vector<VkImageView> _swapchainImageViews{};

  };
}
