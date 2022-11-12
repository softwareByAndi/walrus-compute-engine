#include "device.hpp"
#include "pretty_io.hpp"

#include <vk_initializers.h>

#include <iostream>
#include <string>
#include <stdexcept>
#include <set>

namespace walrus {

  DeviceInfo::QueueFamilyData::QueueFamilyData(VkQueueFamilyProperties &vkProperties) {
    support = Support{};
    support.graphics = (vkProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) > 0;
    support.compute = (vkProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) > 0;
  }




  std::vector<DeviceInfo> DeviceInfo::getDeviceInfos(VkInstance &instance, VkSurfaceKHR &surface, std::vector<VkPhysicalDevice> *vkPhysicalDevices) {
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
      devices.emplace_back(DeviceInfo{physicalDevice, &surface});
    }
    return devices;
  }




  DeviceInfo::DeviceInfo(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR *surface) {
    init(vkPhysicalDevice, surface);
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




  void DeviceInfo::init(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR *surface) {
    getPhysicalDeviceProperties(vkPhysicalDevice);
    getQueueFamilyProperties(vkPhysicalDevice);
    updateQueueSurfaceSupport(vkPhysicalDevice, surface);
    updateDeviceSupportSummary();
    selectMostSuitedQueue();
    calculateDeviceScore(vkPhysicalDevice);
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




  void DeviceInfo::updateQueueSurfaceSupport(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR *surface) {
    for (unsigned int i = 0; i < queueData.size(); i++) {
      VkBool32 supportsSurface = 0;
      if (surface != nullptr) {
        vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, *surface, &supportsSurface);
      }
      queueData.at(i).support.surface = (supportsSurface > 0);
    }
  }




  void DeviceInfo::updateDeviceSupportSummary() {
    for (auto &queue: queueData) {
      if (queue.support.graphics) { supportSummary.graphics = true; }
      if (queue.support.compute) { supportSummary.compute = true; }
      if (queue.support.surface) { supportSummary.surface = true; }
    }
  }




  void DeviceInfo::selectMostSuitedQueue() {
    _bestQueueIndex = -1;
    int bestScore = 0;
    int i = 0;
    for (auto &queue: queueData) {
      queue.score = 0;
      if (queue.support.graphics) { queue.score += 1; }
      if (queue.support.compute) { queue.score += 1; }
      if (queue.support.surface) { queue.score += 1; }
      if ((task == DeviceTask::GRAPHICS && !queue.support.graphics) ||
        (task == DeviceTask::GRAPHICS && !queue.support.surface) ||
        (task == DeviceTask::COMPUTE && !queue.support.compute) ||
        (task == DeviceTask::ALL && !queue.support.isComplete())) {
        queue.score = 0;
      }
      if (bestScore < queue.score) {
        bestScore = queue.score;
        _bestQueueIndex = i;
      }
      i++;
    }
  }




  bool DeviceInfo::checkDeviceExtensionSupport(VkPhysicalDevice &vkPhysicalDevice) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
      vkPhysicalDevice,
      nullptr,
      &extensionCount,
      availableExtensions.data());
    const auto extensions = DeviceInfo::getExtensions();
    std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());
    for (const auto &extension: availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
  }




  void DeviceInfo::calculateDeviceScore(VkPhysicalDevice &vkPhysicalDevice) {
    score = 0;
    if (!checkDeviceExtensionSupport(vkPhysicalDevice)) { return; }

    // discrete gpus have significant performance advantage
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score += 1000;
    }
    // TODO : come up with better metrics
    if (task == DeviceTask::GRAPHICS || task == DeviceTask::ALL) {
      // maximum possible size of textures affects graphics quality
      score += properties.limits.maxImageDimension2D / 10;
    }
    // bonus if compute
    if (supportSummary.compute) {
      score += 1000;
    }

    uint32_t queueScore = 0;
    for (auto &data: queueData) {
      queueScore += data.score;
    }
    if (queueScore == 0) {
      score = 0;
    }
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
    const auto extensions = DeviceInfo::getExtensions();

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
    std::cout << "device name:   " << properties.deviceName << std::endl;
    std::cout << "device score:  " << score << std::endl;
    std::cout << "declared task: " << task << std::endl;
    for (unsigned int i = 0; i < queueData.size(); i++) {
      std::cout << "queueFamily " << i << " : ";
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
    std::cout << "etc..." << std::endl;
    std::cout << std::endl;
  }



} // namespace walrus