﻿#include "vk_engine.h"

#include "pretty_io.hpp"
#include "vk_initializers.h"

#include "engine/compute/commands/command.hpp"

#include "engine/rendering/renderpasses/render_pass.hpp"
#include "engine/rendering/pipelines/builder/pipeline_builder.hpp"
#include "engine/rendering/pipelines/defaults/pipeline_defaults.hpp"
#include "engine/rendering/window/events/window_events.hpp"
#include "engine/rendering/window/events/keys/keys.hpp"

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define VMA_IMPLEMENTATION

#include "vk_mem_alloc.h"

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <cassert>
#include <fstream>

#define VK_CHECK(x) assert(x == VK_SUCCESS)

namespace walrus {

  /// Init various vulkan structures.
  void VulkanEngine::init(DeviceTask task) {
    /// need to destroy before reinitializing
    if (!_isInitialized) {
      _task = task;
      /// output requested features to console
      std::cout << "\nenabled features:";
      printTaskFeatures(_task);
      std::cout << "\n\n";

      /// engine_initialization required structures
      init_vulkan();           /// vulkan instance, surface(opt), debug(opt), messenger, device, memory allocator
      init_commands();         /// command pool, command buffers, destructor queue
      init_sync_structures();  /// fences, semaphores, destructor queue

      /// engine_initialization graphics structures
      if (_task & DeviceTask::GRAPHICS) {
        init_swapchain();     /// swapchain & images, image views, destructor queue
        init_renderpass();    /// render pass, destructor queue
        init_framebuffers();  /// framebuffers, destructor queue
        init_pipelines();     /// load shaders, pipeline-layout, pipelines, destroy shaders, destructor queue
        /// FIXME : mesh loading doesn't work yet. the triangle is hard coded into the shader...
//        load_meshes();        /// test triangle
      }

      /// initialization complete
      std::cout << io::to_color_string(io::LIGHT_BLUE, "vulkan initialized!") << std::endl;
      _isInitialized = true;
    } else {
      std::cout << io::to_color_string(io::RED, "VulkanEngine is already initialized") << std::endl;
    }
  }

  /// destroy all vulkan objects.
  void VulkanEngine::destroy() {
    if (_isInitialized) {
      /// wait for device to complete current tasks. not waiting can cause segfaults
      vkDeviceWaitIdle(_device);

      /// the main destruction queue is populated during initialization
      /// and is used to ensure that objects are destroyed in the proper order (typically reverse engine_initialization order)
      _mainDestructionQueue.destroyAll();

      /// manually destroy surface, memory allocator, device, validation layers & debug messenger, vulkan instance, window
      if (_task & GRAPHICS) {
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
      }
      vmaDestroyAllocator(_allocator);
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
      vkDestroyInstance(_instance, nullptr);
      _window.destroy();
      _isInitialized = false;
    }
  }



  void VulkanEngine::init_vulkan() {
    /// INSTANCE
    {
      VkApplicationInfo appInfo{};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "Vulkan Application";
      appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.pEngineName = "Walrus Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.apiVersion = VK_API_VERSION_1_3;

      std::vector<const char *> extensions = vkInit::defaults::getRequiredExtensions(_enableValidationLayers);
      if (_task & DeviceTask::GRAPHICS) {
        // todo: const char * memory leak?
        auto winExt = Window::getRequiredExtensions(_enableValidationLayers);
        for (auto ext: winExt) {
          extensions.push_back(ext);
        }
      }
      // FIXME: check extension support
      // instantiate debug info here, so that the reference remains in scope
      // when struct is passed to create function
      VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = vkInit::defaults::debugMessengerCreateInfo();

      // define the instance parameters (used to compile parameters /info for application, extensions, and validation layers)
      VkInstanceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pApplicationInfo = &appInfo;
      #ifdef __APPLE__
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR; // portability flag is only needed for macOS
      #elif defined(__linux__)
        // do nothing?
      #else
        #error "This code has only been build for macOS or Linux systems."
      #endif
      createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
      createInfo.ppEnabledExtensionNames = extensions.data();
      createInfo.enabledLayerCount = 0;
      createInfo.pNext = nullptr;
      if (_enableValidationLayers) {
        // attach validation layers
        createInfo.enabledLayerCount = _validationLayers.size();
        createInfo.ppEnabledLayerNames = _validationLayers.data();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
      }
      assert(!_enableValidationLayers || vkInit::checkValidationLayerSupport(_validationLayers));

      // TODO: add allocator?
      VK_CHECK(vkCreateInstance(&createInfo, nullptr, &_instance));
    }

    /// SURFACE
    if (_task & DeviceTask::GRAPHICS) {
      _window.createWindowSurface(_instance, &_surface);
    } else {
      assert(_surface == VK_NULL_HANDLE && "compute only tasks don't need a surface");
    }

    /// DEBUG MESSENGER
    if (_enableValidationLayers) {
      VkDebugUtilsMessengerCreateInfoEXT createInfo = vkInit::defaults::debugMessengerCreateInfo();
      auto createExtension = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
        _instance,
        "vkCreateDebugUtilsMessengerEXT"
      );
      assert(createExtension != nullptr);
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
      auto devicesInfos = DeviceInfo::getDeviceInfos(
        _instance,
        &_physicalDevices,
        _surface,
        _task
      );
      std::cout << std::endl;
      std::cout << "physical device count: " << _physicalDevices.size() << std::endl;
      std::cout << "available devices: \n____________________________________\n";
      for (auto deviceInfo: devicesInfos) {
        deviceInfo.print();
      }
      // TODO: add option to allow user to select device
      // TODO: create device manager, select all devices, and abstract device calls to use the next available suitable device (i.e. swarm logic / device multithreading / etc...)
      // select most suited device
      int selectedDeviceIndex = -1;
      for (int i = 0; i < devicesInfos.size(); i++) {
        // FIXME : QUEUE REFACTOR -- add support for non-general purpose queues
        // NOTE : currently only supports devices with complete support
        if (selectedDeviceIndex < 0
            || devicesInfos[i].score > devicesInfos[selectedDeviceIndex].score
        ){
          selectedDeviceIndex = i;
        }
      }
      if (selectedDeviceIndex < 0
          || devicesInfos[selectedDeviceIndex].score <= 0
      ){
        throw std::runtime_error("failed to find a suitable GPU!");
      }
      _deviceInfo.clone(devicesInfos[selectedDeviceIndex]);
      _physicalDevice = _physicalDevices.at(selectedDeviceIndex);
      DeviceInfo::createLogicalDevice(
        _deviceInfo,
        _physicalDevice,
        &_device,
        _queues
      );

      std::cout << "queue size: " << _queues.size() << std::endl;
      std::cout << "queue data size: " << _deviceInfo.queueData.size() << std::endl;

      // TODO: QUEUE REFACTOR -- update commands and queue logic to work with not-complete-support queue families.

      bool graphics = _task & DeviceTask::GRAPHICS;
      bool compute = _task & DeviceTask::COMPUTE;

      assert( !_deviceInfo.queueData.empty()
              && "missing queues?");
      assert( _deviceInfo.getBestQueueIndex() >= 0
              && "best queue not selected...");
      auto bestQueue = _deviceInfo.queueData[_deviceInfo.getBestQueueIndex()];

      assert( !compute
              || bestQueue.support.compute
              && "for current logic, all queues must support the device specified task");
      assert( !graphics
              || bestQueue.support.isComplete()
              && "for current logic, all graphics tasks must have complete support");
    }


    /// MEMORY ALLOCATOR
    {
      VmaAllocatorCreateInfo info{};
      info.physicalDevice = _physicalDevice;
      info.device = _device;
      info.instance = _instance;
      vmaCreateAllocator(&info, &_allocator);
    }
  }




  void VulkanEngine::init_commands() {
    /// COMMAND POOL
    {
      // TODO - QUEUE REFACTOR : add support for multiple command pools & queues (each queue family should have its own command pool)
      const int queueFamilyIndex = _deviceInfo.getBestQueueIndex();
      assert( _device != VK_NULL_HANDLE
              && "device not setup");
      assert( !_queues.empty()
              && !_deviceInfo.queueData.empty()
              && "missing queues?");
      assert( _deviceInfo.queueData[queueFamilyIndex].support.compute
              && "current logic only supports queues with at least compute...");
      assert( !(_task & DeviceTask::GRAPHICS)
              || _deviceInfo.queueData[queueFamilyIndex].support.isComplete()
              && "current logic only supports queues with complete support...");

      // TODO: add option to disable transient bit
      auto createInfo = CommandPool::createInfo(
        queueFamilyIndex,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
        | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT // optimizes memory allocation for temporary/single-use command buffers
      );
      VK_CHECK(vkCreateCommandPool(
        _device,
        &createInfo,
        nullptr,
        &_commandPool
      ));
    }

    /// COMMAND BUFFER
    {
      auto allocInfo = CommandBuffer::allocateInfo(_commandPool);
      VK_CHECK(vkAllocateCommandBuffers(_device, &allocInfo, &_commandBuffer));
    }

    /// DESTROY
    _mainDestructionQueue.addDestructor([=]() {
      vkDestroyCommandPool(
        _device,
        _commandPool,
        nullptr
      );
    });
  }




  void VulkanEngine::init_swapchain() {
    assert((_task & DeviceTask::GRAPHICS) && "cannot initialize swapchain for non-graphics task");
    /// TODO : add swapchain reset / recreation logic
    assert(_swapchainImages.empty() && "swapchain re-creation not supported yet.");

    /// SWAPCHAIN
    {
      /// query swapchain info
      _swapchainSupportDetails = Swapchain::querySwapchainSupport(_physicalDevice, _surface);
      VkSurfaceFormatKHR vkSurfaceFormat = Swapchain::chooseSurfaceFormat(_swapchainSupportDetails);
      VkPresentModeKHR vkPresentMode = Swapchain::choosePresentationMode(_swapchainSupportDetails);
      _swapchainExtent = Swapchain::chooseExtent(_swapchainSupportDetails, _window);
      _swapchainImageFormat = vkSurfaceFormat.format;

      /// FIXME : this is creating 4 images on linux... only need 3 though, right?
      /// FIXME : this value is passed to `createInfo.minImageCount`, so wy not just pass `capabilities.minImageCount` instead?
      // set image counts to one more than the minimum to allow for triple? buffering
      uint32_t imageCount = _swapchainSupportDetails.capabilities.minImageCount + 1;
      if (_swapchainSupportDetails.capabilities.maxImageCount > 0
          && imageCount > _swapchainSupportDetails.capabilities.maxImageCount
      ){
        imageCount = _swapchainSupportDetails.capabilities.maxImageCount;
      }

      /// set creation params
      VkSwapchainCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      createInfo.surface = _surface;
      createInfo.minImageCount = imageCount;
      createInfo.imageFormat = vkSurfaceFormat.format;
      createInfo.imageColorSpace = vkSurfaceFormat.colorSpace;
      createInfo.imageExtent = _swapchainExtent;
      createInfo.imageArrayLayers = 1; // always 1 unless creating stereoscopic 3D application
      createInfo.preTransform = _swapchainSupportDetails.capabilities.currentTransform; // 90deg, flip, etc...
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

      /// number of QueueFamilies will determine concurrent v.s. exclusive
      // only 1 queue family is supported for now
      const std::vector<uint32_t> queueFamilyIndices = {
              static_cast<uint32_t>(_deviceInfo.getBestQueueIndex())
              // TODO : QUEUE REFACTOR - add support for multiple queue families
      };
      if (queueFamilyIndices.size() > 1) {
        /// FIXME : this code will never run until support for multiple queue families
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        // specify between which queue families image ownership will be shared.
        // the current code states that image ownership will be shared across all queues.
        createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
      } else {
        /// always this code for now
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // param is ignored if imageSharingMode is exclusive
        createInfo.pQueueFamilyIndices = nullptr; // param is ignored if imageSharingMode is exclusive
      }

      // create swapchain
      VK_CHECK(vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapchain));

      // get swapchain images
      /**
       * 1. call once to get the number of images
       * 2. resize the vector to hold all images
       * 3. call again to get the actual images
       */
      vkGetSwapchainImagesKHR(
        _device,
        _swapchain,
        &imageCount, // overwriting a variable that's no longer needed
        nullptr
      );
      assert(imageCount > 0 && "no images?");
      _swapchainImages.resize(imageCount);
      vkGetSwapchainImagesKHR(
        _device,
        _swapchain,
        &imageCount,
        _swapchainImages.data()
      );
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
        VK_CHECK(vkCreateImageView(
                _device,
                &createInfo,
                nullptr,
                &_swapchainImageViews[i]
        ));
        /// image views are destroyed with the framebuffers.
      }
    }

    /// DESTROY
    _mainDestructionQueue.addDestructor([=]() {
      vkDestroySwapchainKHR(
        _device,
        _swapchain,
        nullptr
      );
    });
  }




  void VulkanEngine::init_renderpass() {
    /// TODO : implement reset / re-initialization
    assert(_renderPass == VK_NULL_HANDLE && "RenderPass re-initialization not supported yet");
    assert(_swapchain != VK_NULL_HANDLE && "initialize swapchain before render pass");

    /// RENDER PASS
    /// FIXME : why does this follow vulkan patterns but other engine code doesn't?
    /// TODO : standardize engine patterns?
    auto renderPassCreateInfo = RenderPass::CreateInfo{};
    RenderPass::GetDefaultRenderPassCreateInfo(renderPassCreateInfo, _swapchainImageFormat);
    VK_CHECK(vkCreateRenderPass(_device, &renderPassCreateInfo.createInfo, nullptr, &_renderPass));

    /// DESTROY
    _mainDestructionQueue.addDestructor([=]() {
      vkDestroyRenderPass(
        _device,
        _renderPass,
        nullptr
      );
    });
  }




  void VulkanEngine::init_framebuffers() {
    assert(_renderPass != VK_NULL_HANDLE && "initialize render pass before frame buffers");
    assert(_swapchain != VK_NULL_HANDLE && "if renderpass is defined, then swapchain should be too");
    assert(!_swapchainImages.empty() && "can't create a frame buffer if no images...");
    assert(!_swapchainImageViews.empty() && "can't create a frame buffer if no image views...");
    assert(_framebuffers.empty() && "framebuffer re-initialization not supported yet");

    /// FRAME BUFFERS
    VkFramebufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.renderPass = _renderPass;
    info.attachmentCount = 1;
    info.width = _swapchainExtent.width;
    info.height = _swapchainExtent.height;
    info.layers = 1;

    // create a frame buffer for each image in the swapchain
    const uint32_t imageCount = _swapchainImages.size();
    _framebuffers.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++) {
            info.pAttachments = &_swapchainImageViews[i];
            VK_CHECK(vkCreateFramebuffer(
                    _device,
                    &info,
                    nullptr,
                    &_framebuffers[i]
            ));
    }

    /// DESTROY
    _mainDestructionQueue.addDestructor([=]() {
      // destroy framebuffers before image views
      for (auto &framebuffer: _framebuffers) {
        vkDestroyFramebuffer(_device, framebuffer, nullptr);
      }
      // image views now safe to destroy
      for (auto &imageView: _swapchainImageViews) {
        vkDestroyImageView(_device, imageView, nullptr);
      }
    });
  }




  void VulkanEngine::init_sync_structures() {
    /// FENCES
    {
      VkFenceCreateInfo info{};
      info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      info.pNext = nullptr;
      info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // signal == mutex lock

      _fencePool.push_back(VK_NULL_HANDLE);
      VK_CHECK(vkCreateFence(_device, &info, nullptr, &_fencePool.back()));
      _fences.pRender = &_fencePool.back();
    }

    /// SEMAPHORES
    {
      VkSemaphoreCreateInfo info{};
      info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
      info.pNext = nullptr;
      info.flags = 0;

      size_t index = _semaphorePool.size();
      _semaphorePool.push_back(VK_NULL_HANDLE);
      _semaphorePool.push_back(VK_NULL_HANDLE);
      VK_CHECK(vkCreateSemaphore(_device, &info, nullptr, &_semaphorePool.at(index)));
      VK_CHECK(vkCreateSemaphore(_device, &info, nullptr, &_semaphorePool.at(index + 1)));
      _semaphores.pPresent = &_semaphorePool.at(index);
      _semaphores.pRender = &_semaphorePool.at(index + 1);
    }

    /// DESTROY
    _mainDestructionQueue.addDestructor([=]() {
      vkWaitForFences(
        _device,
        _fencePool.size(),
        _fencePool.data(),
        true,
        1'000'000'000
      );
      for (auto &fence: _fencePool) {
        vkDestroyFence(
          _device,
          fence,
          nullptr
        );
      }
      // TODO: are we not supposed to wait for semaphores the same as for fences?
      for (auto &semaphore: _semaphorePool) {
        vkDestroySemaphore(
          _device,
          semaphore,
          nullptr
        );
      }
    });
  }




  /// @brief reads a shader file into a buffer and creates a shader module from it.
  bool VulkanEngine::load_shader_module(const char *filePath, VkShaderModule *outShaderModule) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
      return false;
    }

    // gets file size, because std::ios::ate sets the position to the end of the file
    size_t fileSize = (size_t) file.tellg();
    /// TODO : is a vector the optimal container for buffer?
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
    file.seekg(0);
    file.read((char *) buffer.data(), (long) fileSize);
    file.close();

    VkShaderModuleCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pNext = nullptr;
    info.codeSize = buffer.size() * sizeof(uint32_t);
    info.pCode = buffer.data();

    VkShaderModule shaderModule;
    if (
      vkCreateShaderModule(
        _device,
        &info,
        nullptr,
        &shaderModule
      ) != VK_SUCCESS
    ){
      return false;
    }
    *outShaderModule = shaderModule;
    return true;
  }




  void VulkanEngine::init_pipelines() {
    assert(_swapchain != VK_NULL_HANDLE && "must initialize swapchain before pipelines");

    /// CONSTANTS
    VkPipelineLayoutCreateInfo info = defaults::pipeline::layoutCreateInfo();
    PipelineBuilder builder{this->_swapchainExtent};
    /// FIXME : topology and polygon mode is hard coded
    builder.inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    builder.rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;

    for (auto &shaderPath: _shaders.filePaths) {
      /// LOAD SHADERS
      VkShaderModule fragmentShader; // will be instantiated in `load_shader_module` call
      VkShaderModule vertexShader;   // will be instantiated in `load_shader_module` call
      {
        std::string fragFilePath = shaderPath + ".frag.spv";
        std::string vertFilePath = shaderPath + ".vert.spv";
        io::printExists(
          load_shader_module(fragFilePath.data(), &fragmentShader),
          fragFilePath
        );
        io::printExists(
          load_shader_module(vertFilePath.data(), &vertexShader),
          vertFilePath
        );
      }

      /// LAYOUT
      {
        // TODO: add descriptor sets and other stuff
        _pipelineLayouts.push_back(VK_NULL_HANDLE);
        VK_CHECK(vkCreatePipelineLayout(
                _device,
                &info,
                nullptr,
                &_pipelineLayouts.back()
        ));
      }

      /// PIPELINE
      {
        /// FIXME : pipeline defaults are in a defaults namespace, but renderpass isn't
        /// FIXME : some classes have default creator functions and some don't...
        /// FIXME : standardize how default createInfo structs are created.

        builder.shaderStages.clear();
        builder.shaderStages.push_back(
          defaults::pipeline::shaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader)
        );
        builder.shaderStages.push_back(
          defaults::pipeline::shaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader)
        );
        builder.pipelineLayout = _pipelineLayouts.back();

        _pipelines.push_back(VK_NULL_HANDLE);
        builder.build(
                _device,
                _renderPass,
                &_pipelines.back()
        );
      }
      vkDestroyShaderModule(_device, fragmentShader, nullptr);
      vkDestroyShaderModule(_device, vertexShader, nullptr);
    }

    /// DESTROY
    _mainDestructionQueue.addDestructor([=]() {
      // must destroy pipelines before pipeline layouts
      for (auto &pipeline: _pipelines) {
        vkDestroyPipeline(_device, pipeline, nullptr);
      }
      // pipeline layouts are now safe to destroy
      for (auto &layout: _pipelineLayouts) {
        vkDestroyPipelineLayout(_device, layout, nullptr);
      }
    });
  }




  // FIXME: refactor out test code
  void VulkanEngine::load_meshes() {
    _test.mesh.vertices.resize(3);
    _test.mesh.vertices[0].position = {1.f, 1.f, 0.f};
    _test.mesh.vertices[1].position = {-1.f, 1.f, 0.f};
    _test.mesh.vertices[2].position = {0.f, -1.f, 0.f};
    _test.mesh.vertices[0].color = {0.f, 1.f, 1.f};
    _test.mesh.vertices[1].color = {0.f, 1.f, 0.f};
    _test.mesh.vertices[2].color = {0.f, 1.f, 0.f};
    // TODO: add normals
    upload_mesh(_test.mesh);
  }




  void VulkanEngine::upload_mesh(Mesh &mesh) {
    /// VMA BUFFER
    {
      VkBufferCreateInfo bufferInfo{};
      bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferInfo.size = mesh.vertices.size() * sizeof(walrus::Vertex);
      bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

      VmaAllocationCreateInfo vmaAllocInfo{};
      vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

      VK_CHECK(vmaCreateBuffer(
              _allocator,
              &bufferInfo,
              &vmaAllocInfo,
              &mesh.vertexBuffer.buffer,
              &mesh.vertexBuffer.allocation,
              nullptr
      ));
    }

    /// DESTROY
    _mainDestructionQueue.addDestructor([=]() {
      vmaDestroyBuffer(
              _allocator,
              mesh.vertexBuffer.buffer,
              mesh.vertexBuffer.allocation
      );
    });


    /// FIXME : what's the optimal way to do this? Will this be slow?
    /// TODO : how often might this run? Is this a bottleneck?
    /**
     * transfering data from the cpu to the gpu has the following steps:
     * 1. map the gpu memory to the cpu
     * 2. copy the data to the mapped memory
     * 3. unmap the gpu memory
     */
    void *data;
    vmaMapMemory(
        _allocator,
        mesh.vertexBuffer.allocation,
        &data
    );
    memcpy(
        data,
        mesh.vertices.data(),
        mesh.vertices.size() * sizeof(walrus::Vertex)
    );
    vmaUnmapMemory(
        _allocator,
        mesh.vertexBuffer.allocation
    );
  }










  /// --------------------------------------------------
  /// PRESENTATION
  /// --------------------------------------------------

  void VulkanEngine::draw() {
    uint32_t imageIndex;
    VK_CHECK(vkWaitForFences(
            _device,
            1,
            _fences.pRender,
            true,
            1'000'000'000
    ));
    VK_CHECK(vkResetFences(
            _device,
            1,
            _fences.pRender
    ));
    VK_CHECK(vkAcquireNextImageKHR(
            _device,
            _swapchain,
            1'000'000'000,
            *_semaphores.pPresent,
            nullptr,
            &imageIndex
    ));

    auto graphicsQueue = _queues.front();

    VkClearValue clearValue{};
    float flash = abs(sin((float) _frameNumber / 120.f));
    clearValue.color = {{
      0.f,
      0.f,
      flash,
      1.f
    }};
    _frameNumber++;

    /// CMD BUFFER BEGIN
    {
      VK_CHECK(vkResetCommandBuffer(_commandBuffer, 0));
      VkCommandBufferBeginInfo info{};
      info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      info.pNext = nullptr;
      info.pInheritanceInfo = nullptr; // used for secondary cmd buffers.
      info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
      VK_CHECK(vkBeginCommandBuffer(_commandBuffer, &info));
    }

    /// RENDER PASS
    {
      VkRenderPassBeginInfo info{};
      info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      info.pNext = nullptr;
      info.renderPass = _renderPass;
      info.framebuffer = _framebuffers[imageIndex]; // the image we will render into.
      info.clearValueCount = 1;
      info.pClearValues = &clearValue;
      // offset and extent can be set if we want to render a small renderpass into a bigger image
      info.renderArea.offset.x = 0;
      info.renderArea.offset.y = 0;
      info.renderArea.extent = _swapchainExtent;

      vkCmdBeginRenderPass(
        _commandBuffer,
        &info,
        VK_SUBPASS_CONTENTS_INLINE
      );
      vkCmdBindPipeline(
        _commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        _pipelines[_shaders.currentIndex]
      );
      vkCmdDraw(
        _commandBuffer,
        3, /// FIXME : this should be the number of vertices in the mesh
        1,
        0,
        0
      );
      vkCmdEndRenderPass(
        _commandBuffer
      );
    }
    VK_CHECK(vkEndCommandBuffer(_commandBuffer));

    /// SUBMIT TO QUEUE
    {
      VkSubmitInfo info{};
      info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      info.pNext = nullptr;
      VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
      info.pWaitDstStageMask = &waitStage;
      info.waitSemaphoreCount = 1;
      info.pWaitSemaphores = _semaphores.pPresent; // mutex is locked from vkAcquireNextImageKHR above
      info.signalSemaphoreCount = 1;
      info.pSignalSemaphores = _semaphores.pRender; // set rendering mutex
      info.commandBufferCount = 1;
      info.pCommandBuffers = &_commandBuffer;
      VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &info, *_fences.pRender));
    }

    /// PRESENT
    {
      VkPresentInfoKHR info{};
      info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      info.pNext = nullptr;
      info.swapchainCount = 1;
      info.pSwapchains = &_swapchain;
      info.waitSemaphoreCount = 1;
      info.pWaitSemaphores = _semaphores.pRender; // mutex is locked from vkQueueSubmit above
      info.pImageIndices = &imageIndex;
      VK_CHECK(vkQueuePresentKHR(graphicsQueue, &info));
    }
  }

  void VulkanEngine::run() {
    if (_task & DeviceTask::GRAPHICS) {
      runRender();
    } else if (_task & DeviceTask::COMPUTE){
      runCompute();
    } else {
      throw std::runtime_error("only graphics and compute tasks are currently supported");
    }
  }

  void VulkanEngine::runRender() {
    std::cout << "window extent: " << _window.getExtent().width << " : " << _window.getExtent().height << std::endl;
    std::cout << "swap extent:   " << _swapchainExtent.width << " : " << _swapchainExtent.height << std::endl;
    WindowEvents winEvents{_window.getGLFWwindow()};
    winEvents.addKey(keys::SPACE);
    while (!_window.shouldClose()) {
      glfwPollEvents();
      winEvents.poll();
      if (winEvents.keyPress(keys::SPACE)) {
        _shaders.currentIndex = (_shaders.currentIndex + 1) % _shaders.filePaths.size();
      }
      draw();
    }
  }

  void VulkanEngine::runCompute() {
    bool done = false;
    while(!done) {
      std::cout << io::to_color_string(io::Color::CYAN, "\nRUN COMPUTE\n\n");
      done = true;
    }
  }

}