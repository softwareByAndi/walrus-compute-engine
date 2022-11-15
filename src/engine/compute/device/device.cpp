#include "device.hpp"
#include "pretty_io.hpp"
#include "engine/rendering/swapchain/swapchain.hpp"

#include "vk_initializers.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <set>

namespace walrus {

  void printTaskFeatures(DeviceTask &task) {
    io::printExists(task & GRAPHICS, "graphics", false);
    io::printExists(task & COMPUTE, "compute", false);
  }

  DeviceInfo::QueueFamilyData::QueueFamilyData(VkQueueFamilyProperties &vkProperties) {
    support = Support{};
    support.graphics = (vkProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) > 0;
    support.compute = (vkProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) > 0;
  }




  std::vector<DeviceInfo> DeviceInfo::getDeviceInfos(
    VkInstance &instance,
    std::vector<VkPhysicalDevice> *vkPhysicalDevices,
    VkSurfaceKHR vkSurface,
    DeviceTask deviceTask
  ) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
      throw std::runtime_error("failed to find GPUs with Vulkan support");
    }
    vkPhysicalDevices->clear();
    vkPhysicalDevices->resize(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, vkPhysicalDevices->data());
    std::vector<DeviceInfo> devices;
    for (auto &physicalDevice: *vkPhysicalDevices) {
      devices.emplace_back(DeviceInfo{physicalDevice, vkSurface, deviceTask});
    }
    return devices;
  }




  DeviceInfo::DeviceInfo(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR vkSurface, DeviceTask deviceTask) {
    init(vkPhysicalDevice, vkSurface, deviceTask);
  }




  void DeviceInfo::init(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR vkSurface, DeviceTask deviceTask) {
    task = deviceTask;
    getPhysicalDeviceProperties(vkPhysicalDevice);
    getQueueFamilyProperties(vkPhysicalDevice);
    updateQueueSurfaceSupport(vkPhysicalDevice, (task & GRAPHICS) ? vkSurface : VK_NULL_HANDLE);
    updateDeviceSupportSummary(vkPhysicalDevice, (task & GRAPHICS) ? vkSurface : VK_NULL_HANDLE);
    calculateDeviceScore(vkPhysicalDevice);
  }




  void DeviceInfo::clone(DeviceInfo const &deviceInfo) {
    queueData.clear();
    queueData = deviceInfo.queueData;
    properties = deviceInfo.properties;
    features = deviceInfo.features;
    task = deviceInfo.task;
    score = deviceInfo.score;
    supportSummary = deviceInfo.supportSummary;
    _bestQueueIndex = deviceInfo._bestQueueIndex;
    _queueFamilies.clear();
    _queueFamilies = deviceInfo._queueFamilies;
  }




  void DeviceInfo::getPhysicalDeviceProperties(VkPhysicalDevice &vkPhysicalDevice) {
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);
    vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &features);
  }




  void DeviceInfo::getQueueFamilyProperties(VkPhysicalDevice &vkPhysicalDevice) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
    _queueFamilies.clear();
    _queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, _queueFamilies.data());
    for (auto queueProperties: _queueFamilies) {
      queueData.emplace_back(DeviceInfo::QueueFamilyData{queueProperties});
    }
  }




  void DeviceInfo::updateQueueSurfaceSupport(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR vkSurface) {
    for (unsigned int i = 0; i < queueData.size(); i++) {
      VkBool32 supportsSurface = 0;
      if (vkSurface != VK_NULL_HANDLE) {
        vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, vkSurface, &supportsSurface);
      }
      queueData.at(i).support.surface = supportsSurface;
    }
  }




  void DeviceInfo::updateDeviceSupportSummary(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR vkSurface) {
    for (auto &queue: queueData) {
      if (queue.support.graphics) { supportSummary.graphics = true; }
      if (queue.support.compute) { supportSummary.compute = true; }
      if (queue.support.surface) { supportSummary.surface = true; }
    }
    if (vkSurface != VK_NULL_HANDLE && Swapchain::querySwapchainSupport(vkPhysicalDevice, vkSurface).isValid()) {
      supportSummary.swapchain = true;
    }
  }




  void DeviceInfo::selectMostSuitedQueue() {
    _bestQueueIndex = -1;
    uint32_t bestScore = 0;
    int i = 0;
    for (auto &queue: queueData) {
      queue.score = 0;
      if (queue.support.graphics) { queue.score += 1; }
      if (queue.support.compute) { queue.score += 1; }
      if (queue.support.surface) { queue.score += 1; }
      if ((task & GRAPHICS && (!queue.support.graphics || !queue.support.surface)) ||
        (task & COMPUTE && !queue.support.compute) ||
        (task == ALL && !queue.support.isComplete())) {
        std::cerr << "queue not complete" << std::endl;
        queue.score = 0;
      }
      if (bestScore < queue.score) {
        bestScore = queue.score;
        _bestQueueIndex = i;
      }
      i++;
    }
  }




  bool DeviceInfo::checkDeviceExtensionSupport(VkPhysicalDevice &vkPhysicalDevice) const {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
      vkPhysicalDevice,
      nullptr,
      &extensionCount,
      availableExtensions.data());
    const auto extensions = DeviceInfo::getExtensions(task);
    std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());
    for (const auto &extension: availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }
    if (task & GRAPHICS) {
      if (!supportSummary.swapchain) {
        return false;
      }
    }
    return requiredExtensions.empty();
  }




  void DeviceInfo::calculateDeviceScore(VkPhysicalDevice &vkPhysicalDevice) {
    score = 0;
    if (!checkDeviceExtensionSupport(vkPhysicalDevice)) {
      std::cerr << "device extension not supported" << std::endl;
      return;
    }

    // discrete gpus have significant performance advantage
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score += 1000;
    }
    // TODO : come up with better metrics
    if (task & GRAPHICS) {
      // maximum possible size of textures affects graphics quality
      score += properties.limits.maxImageDimension2D / 10;
    }
    // bonus if compute
    if (supportSummary.compute) {
      score += 1000;
    }

    selectMostSuitedQueue();
    uint32_t queueScore = 0;
    for (auto &queue: queueData) {
      queueScore += queue.score;
    }
    if (queueScore == 0) { score = 0; }
  }




  void DeviceInfo::createLogicalDevice(DeviceInfo &deviceInfo, VkPhysicalDevice &vkPhysicalDevice, VkDevice *device, std::vector<VkQueue> &queues) {
    if (deviceInfo._bestQueueIndex == -1) {
      throw std::runtime_error(
        "physical device does not have a suitable Queue. deviceName = " + std::string(deviceInfo.properties.deviceName)
      );
    }
    if (!deviceInfo.checkDeviceExtensionSupport(vkPhysicalDevice)) {
      throw std::runtime_error(
        "physical device does not support required extensions. deviceName: " + std::string(deviceInfo.properties.deviceName)
      );
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    float queuePriority = 1.0f;
    for (uint32_t i = 0; i < deviceInfo.queueData.size(); i++) {
      VkDeviceQueueCreateInfo queueCreateInfo = {};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = i;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    const auto extensions = DeviceInfo::getExtensions(deviceInfo.task);

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    // TODO: add allocator?
    if (vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, device) != VK_SUCCESS) {
      throw std::runtime_error("failed to create logical device!");
    }
    for (uint32_t i = 0; i < deviceInfo.queueData.size(); i++) {
      if (deviceInfo.queueData[i].score > 0) {
        queues.push_back(VK_NULL_HANDLE);
        vkGetDeviceQueue(*device, deviceInfo._bestQueueIndex, 0, &queues.back());
      }
    }
  }




  void DeviceInfo::print() {
    std::cout << io::to_color_string(io::LIGHT_GRAY, "device name:   ") << properties.deviceName << std::endl;
    std::cout << io::to_color_string(io::LIGHT_GRAY, "device score:  ") << score << std::endl;
    std::cout << io::to_color_string(io::LIGHT_GRAY, "declared task(s): ");
    printTaskFeatures(task);
    std::cout << std::endl;
    for (unsigned int i = 0; i < queueData.size(); i++) {
      std::cout << io::COLORS[io::LIGHT_GRAY] << "queueFamily " << i << " : ";
      io::printExists(queueData[i].support.graphics, "graphics", false);
      io::printExists(queueData[i].support.compute, "compute", false);
      io::printExists(queueData[i].support.surface, "surface", false);
      std::cout << std::endl;
    }
    io::printExists(features.geometryShader, "geometryShader");
    io::printExists(features.tessellationShader, "tessellationShader");
    io::printExists(features.alphaToOne, "alphaToOne");
    io::printExists(features.depthBiasClamp, "depthBiasClamp");
    io::printExists(features.depthBounds, "depthBounds");
    std::cout << io::to_color_string(io::Color::LIGHT_GRAY, "etc...") << std::endl;
    std::cout << std::endl;
  }



} // namespace walrus