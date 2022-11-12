#include "vk_engine.h"

#include <GLFW/glfw3.h>
#include "vk_initializers.h"
#include "pretty_io.hpp"

#include <iostream>
#include <map>
#include <algorithm>

namespace walrus {

  void VulkanEngine::init() {
    if (!_isInitialized) {
      // load the core vulkan structures
      init_vulkan();
      init_swapchain();

      //everything went fine
      _isInitialized = true;
    } else {
      std::cout << "VulkanEngine is already initialized" << std::endl;
    }
  }

  void VulkanEngine::init_vulkan() {
    { /// INSTANCE
      VkApplicationInfo appInfo{};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "Vulkan Application";
      appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.pEngineName = "Walrus Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.apiVersion = VK_API_VERSION_1_3;

      auto extensions = Window::getRequiredExtensions(_enableValidationLayers);
      // FIXME: check extension support
      VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = vkInit::defaults::debugMessengerCreateInfo();
      VkInstanceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pApplicationInfo = &appInfo;
      createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR; // ADD FLAG FOR M1
      createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
      createInfo.ppEnabledExtensionNames = extensions.data();
      createInfo.enabledLayerCount = 0;
      createInfo.pNext = nullptr;
      if (_enableValidationLayers) {
        createInfo.enabledLayerCount = _validationLayers.size();
        createInfo.ppEnabledLayerNames = _validationLayers.data();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
      }
      if (_enableValidationLayers && !vkInit::checkValidationLayerSupport(_validationLayers)) {
        throw std::runtime_error("validation layers requested, but not available!");
      }

      // TODO: add allocator?
      if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance");
      }
    }

    /// SURFACE
    _window.createWindowSurface(_instance, &_surface);

    /// DEBUG MESSENGER
    if (_enableValidationLayers) {
      VkDebugUtilsMessengerCreateInfoEXT createInfo = vkInit::defaults::debugMessengerCreateInfo();
      auto createExtension = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
        _instance,
        "vkCreateDebugUtilsMessengerEXT"
      );
      if (createExtension == nullptr) {
        throw std::runtime_error("debug messenger extension not present");
      }
      if (createExtension(
        _instance,
        &createInfo,
        nullptr, // TODO: add allocator?
        &_debugMessenger
      ) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger");
      }
    }

    /// DEVICE
    {
      auto devicesInfos = DeviceInfo::getDeviceInfos(_instance, _surface, &_physicalDevices);
      std::cout << "physical device count: " << _physicalDevices.size() << std::endl;
      std::cout << "available devices: " << std::endl;
      for (auto deviceInfo: devicesInfos) {
        deviceInfo.print();
      }
      // TODO: add option to allow user to select device
      // TODO: create device manager, select all devices, and abstract device calls to use the next available suitable device (i.e. swarm logic / device multithreading / etc...)
      // select most suited device
      int selectedDeviceIndex = -1;
      for (int i = 0; i < devicesInfos.size(); i++) {
        if (selectedDeviceIndex < 0 || devicesInfos[i].score > devicesInfos[selectedDeviceIndex].score) {
          selectedDeviceIndex = i;
        }
      }
      if (selectedDeviceIndex < 0 || devicesInfos[selectedDeviceIndex].score <= 0) {
        throw std::runtime_error("failed to find a suitable GPU!");
      }
      _deviceInfo.clone(devicesInfos[selectedDeviceIndex]);
      _physicalDevice = _physicalDevices.at(selectedDeviceIndex);
      DeviceInfo::createLogicalDevice(_deviceInfo, _physicalDevice, &_device, _queues);
    }
  }

  void VulkanEngine::init_swapchain() {
    //  vkb::SwapchainBuilder swapchainBuilder{_chosenGPU, _device, _surface};
    //  vkb::Swapchain vkbSwapchain = swapchainBuilder
    //    .use_default_format_selection()
    //    .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
    //    .set_desired_extent(_windowExtent.width, _windowExtent.height)
    //    .build()
    //    .value();
    //  _swapchain = vkbSwapchain.swapchain;
    //  _swapchainImages = vkbSwapchain.get_images().value();
    //  _swapchainImageViews = vkbSwapchain.get_image_views().value();
    //  _swapchainImageFormat = vkbSwapchain.image_format;
  }

  void VulkanEngine::destroy() {
    if (_isInitialized) {
      //    vkDestroySwapchainKHR(_device, _swapchain, nullptr);
      //    // destroy swapchain resources
      //    for (auto &_swapchainImageView: _swapchainImageViews) {
      //      vkDestroyImageView(_device, _swapchainImageView, nullptr);
      //    }
      vkDestroyDevice(_device, nullptr);
      if (_enableValidationLayers) {
        /// DESTROY DEBUG MESSENGER
        auto destroyDebug = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
          _instance,
          "vkDestroyDebugUtilsMessengerEXT"
        );
        if (destroyDebug != nullptr) {
          destroyDebug(_instance, _debugMessenger, nullptr);
        }
      }
      vkDestroySurfaceKHR(_instance, _surface, nullptr);
      vkDestroyInstance(_instance, nullptr);
      _window.destroy();
      _isInitialized = false;
    }
  }

  void VulkanEngine::draw() {
    //nothing yet
  }

  void VulkanEngine::run() {
    while (!_window.shouldClose()) {
      glfwPollEvents();
    }
    //    vkDeviceWaitIdle(_device);
  }


}