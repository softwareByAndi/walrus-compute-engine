﻿#include "vk_engine.h"

#include "engine/rendering/swapchain/swapchain.hpp"
#include "engine/compute/commands/command.hpp"
#include "vk_initializers.h"
#include "pretty_io.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>
#include <cassert>

namespace walrus {

  void VulkanEngine::init() {
    if (!_isInitialized) {
      // load the core vulkan structures
      init_vulkan();
      init_swapchain();
      init_commands();

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
      std::cout << std::endl;
      std::cout << "physical device count: " << _physicalDevices.size() << std::endl;
      std::cout << "available devices: \n\n";
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

      // TODO: refactor commands and queue logic to work with not-complete-support queue families.
      assert(!_deviceInfo.queueData.empty() && "missing queues?");
      assert(_queues.size() == _deviceInfo.queueData.size() && "uh oh... something's wrong with the queue setup...");
      for (const auto &queue: _deviceInfo.queueData) {
        assert(queue.support.isComplete() &&
                 "Current logic only works with M1 chips or devices for which all queues have full support. Please request a refactor");
      }
    }
  }




  void VulkanEngine::init_swapchain() {
    /// SWAPCHAIN
    {
      Swapchain::SupportDetails swapchainSupportDetails = Swapchain::querySwapchainSupport(_physicalDevice, _surface);
      auto vkSurfaceFormat = Swapchain::chooseSurfaceFormat(swapchainSupportDetails);
      auto vkPresentMode = Swapchain::choosePresentationMode(swapchainSupportDetails);
      auto vkExtent = Swapchain::chooseExtent(swapchainSupportDetails, _window);
      _swapchainImageFormat = vkSurfaceFormat.format;
      uint32_t imageCount = swapchainSupportDetails.capabilities.minImageCount + 1;
      if (swapchainSupportDetails.capabilities.maxImageCount > 0 &&
        imageCount > swapchainSupportDetails.capabilities.maxImageCount) {
        imageCount = swapchainSupportDetails.capabilities.maxImageCount;
      }

      VkSwapchainCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      createInfo.surface = _surface;
      createInfo.minImageCount = imageCount;
      createInfo.imageFormat = vkSurfaceFormat.format;
      createInfo.imageColorSpace = vkSurfaceFormat.colorSpace;
      createInfo.imageExtent = vkExtent;
      createInfo.imageArrayLayers = 1; // always 1 unless creating stereoscopic 3D application
      createInfo.preTransform = swapchainSupportDetails.capabilities.currentTransform; // 90deg, flip, etc...
      createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
      createInfo.presentMode = vkPresentMode;
      createInfo.clipped = VK_TRUE; // delete pixels that are covered by other pixels
      createInfo.oldSwapchain = VK_NULL_HANDLE;

      /*
        imageUsage = swapchain operations
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT:
          - direct image rendering
          - in the case of post-processing: VK_IMAGE_USAGE_TRANSFER_DST_BIT should be used
        TODO: research Vulkan swapchain post processing
      */
      createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

      /*
        VK_SHARING_MODE_EXCLUSIVE:
          - image is owned by one queue family at a time and ownership must be explicitly transferred before using it in another queue family.
          - this option offers the best performance
        VK_SHARING_MODE_CONCURRENT:
          - images can be used across multiple queue families without explicit ownership transfers
          - requires declaring in advance which queues will share ownership using `queueFamilyIndexCount` and `pQueueFamilyIndices`
        TODO: cover ownership chapters to implement exclusive logic instead of concurrent logic
      */
      std::vector<uint32_t> queueIndices(_queues.size());
      for (uint32_t i = 0; i < _queues.size(); i++) { queueIndices[i] = i; }
      if (_queues.size() > 1) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        /// specify between which queue families image ownership will be shared. for now, image ownership will be shared across all queues.
        createInfo.queueFamilyIndexCount = _queues.size();
        createInfo.pQueueFamilyIndices = queueIndices.data();
      } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
      }

      if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapchain) != VK_SUCCESS) {
        throw std::runtime_error("unable to create swapchain...");
      }

      vkGetSwapchainImagesKHR(_device, _swapchain, &imageCount, nullptr);
      assert(imageCount > 0 && "no images?");
      assert(_swapchainImages.empty() && "swapchain re-creation not supported yet.");
      _swapchainImages.resize(imageCount);
      vkGetSwapchainImagesKHR(_device, _swapchain, &imageCount, _swapchainImages.data());
    }

    /// IMAGE VIEWS
    {
      _swapchainImageViews.resize(_swapchainImages.size());
      for (size_t i = 0; i < _swapchainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = _swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = _swapchainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(_device, &createInfo, nullptr, &_swapchainImageViews[i]) != VK_SUCCESS) {
          throw std::runtime_error("failed to create image view[" + std::to_string(i) + "]");
        }
      }
    }
  }



  void VulkanEngine::init_commands() {
    /// COMMAND POOL
    {
      assert(_device != VK_NULL_HANDLE && "device not setup");
      assert(!_queues.empty() && !_deviceInfo.queueData.empty() && "missing queues?");
      assert(_deviceInfo.queueData[0].support.isComplete() &&
               "current logic only supports queues with complete support...");
      auto createInfo = CommandPool::createInfo(0, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
      if (vkCreateCommandPool(_device, &createInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("unable to create command pool...");
      }
    }

    /// COMMAND BUFFER
    {
      auto allocInfo = CommandBuffer::allocateInfo(_commandPool);
      if (vkAllocateCommandBuffers(_device, &allocInfo, &_commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("unable to allocate command buffer");
      }
    }
  }




  void VulkanEngine::destroy() {
    if (_isInitialized) {
      vkDestroyCommandPool(_device, _commandPool, nullptr);
      vkDestroySwapchainKHR(_device, _swapchain, nullptr);
      // destroy swapchain resources
      for (auto &_swapchainImageView: _swapchainImageViews) {
        vkDestroyImageView(_device, _swapchainImageView, nullptr);
      }
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