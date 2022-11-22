#pragma once

#include "engine/rendering/window/window.hpp"
#include "engine/rendering/swapchain/swapchain.hpp"

#include "engine/compute/device/device.hpp"
#include "engine/compute/synchronize/generics.hpp"

#include "vk_types.h"
#include <vector>
#include <string>
#include <functional>
#include <deque>

namespace walrus {

  struct DestructionQueue {
    std::deque<std::function<void()>> destructor{};

    void addDestructor(std::function<void()> &&function) {
      destructor.push_back(function);
    }

    void destroyAll() {
      for (auto it = destructor.rbegin(); it != destructor.rend(); it++) {
        (*it)();
      }
      destructor.clear();
    }
  };




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

    void init_renderpass();

    void init_framebuffers();

    void init_sync_structures();

    bool load_shader_module(const char* filePath, VkShaderModule* outShaderModule);

    void init_pipelines();


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

    /// CORE & COMPUTE
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
    Window _window{800, 600, "Vulkan Window"}; // TODO: don't create window for compute only tasks
    VkSurfaceKHR _surface = VK_NULL_HANDLE;

    Swapchain::SupportDetails _swapchainSupportDetails{};
    VkExtent2D _swapchainExtent{};
    VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
    VkFormat _swapchainImageFormat{};
    std::vector<VkImage> _swapchainImages{};
    std::vector<VkImageView> _swapchainImageViews{};

    VkRenderPass _renderPass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> _framebuffers{};

    std::vector<VkPipelineLayout> _pipelineLayouts{};
    std::vector<VkPipeline> _pipelines{};
    struct Shaders {
      uint32_t currentIndex = {0};
      std::vector<std::string> filePaths{
        "../../shaders/triangle_red",
        "../../shaders/triangle_RGB"
      };
    };
    Shaders _shaders{};

    std::vector<VkSemaphore> _semaphorePool{};
    std::vector<VkFence> _fencePool;
    sync::generics::RenderSync<VkSemaphore> _semaphores{};
    sync::generics::RenderSync<VkFence> _fences{};

    /// Destructors
    DestructionQueue _mainDestructionQueue{};
  };
}
