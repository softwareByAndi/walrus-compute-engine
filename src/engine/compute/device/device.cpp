#include "device.hpp"
#include "pretty_io.hpp"
#include "engine/rendering/swapchain/swapchain.hpp"

#include "vk_initializers.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <set>

namespace walrus {
  /**
   * print * display
   * queue family
   * device info
   */



  /// -----------------------------------------------------------------------------------------------
  /// PRINT & DISPLAY
  /// -----------------------------------------------------------------------------------------------

  /// FIXME : pull these values dynamically... see VkPhysicalDeviceType enum
  std::vector<std::string> DeviceTypeStrings = {
    "VK_PHYSICAL_DEVICE_TYPE_OTHER",
    "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU",
    "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU",
    "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU",
    "VK_PHYSICAL_DEVICE_TYPE_CPU"
  };

  void printTaskFeatures(
        DeviceTask &task
  ){
    io::printExists(task & GRAPHICS, "graphics", false);
    io::printExists(task & COMPUTE, "compute", false);
  }

  void DeviceInfo::print()
  {
    std::cout << io::to_color_string(io::LIGHT_GRAY, "device name:      ") << properties.deviceName << std::endl;
    std::cout << io::to_color_string(io::LIGHT_GRAY, "deviceType:       ") << DeviceTypeStrings[properties.deviceType] << std::endl;
    std::cout << io::to_color_string(io::LIGHT_GRAY, "device score:     ") << score << std::endl;
    std::cout << io::to_color_string(io::LIGHT_GRAY, "declared task(s): ");
    printTaskFeatures(task);
    std::cout << std::endl;
    for (unsigned int i = 0; i < queueData.size(); i++) {
      unsigned int queueCount = queueData[i].queueCount;
      std::string s = queueCount > 1 ? "s" : "";
      io::Color color = _bestQueueIndex == i ? io::Color::LIGHT_BLUE : io::Color::LIGHT_GRAY;

      std::cout << io::to_color_string(color, "queueFamily " + std::to_string(i) + ": ");
      io::printExists(queueData[i].support.graphics, "graphics", false);
      io::printExists(queueData[i].support.compute, "compute", false);
      io::printExists(queueData[i].support.surface, "surface", false);
      std::cout << io::to_color_string(io::LIGHT_GRAY, " (" + std::to_string(queueCount) + " queue" + s + ")");
      std::cout << std::endl;
    }
    io::printExists(features.samplerAnisotropy, "samplerAnisotropy");
    io::printExists(features.geometryShader, "geometryShader");
    io::printExists(features.tessellationShader, "tessellationShader");
    io::printExists(features.alphaToOne, "alphaToOne");
    io::printExists(features.depthBiasClamp, "depthBiasClamp");
    io::printExists(features.depthBounds, "depthBounds");
    std::cout << io::to_color_string(io::Color::LIGHT_GRAY, "etc...") << std::endl;
    std::cout << std::endl;
  }





  /// -----------------------------------------------------------------------------------------------
  /// QUEUE FAMILY SUPPORT
  /// -----------------------------------------------------------------------------------------------

  /// @brief init struct and identify support for graphics & compute
  /// surface and swapchain default to false
  DeviceInfo::QueueFamilyData::QueueFamilyData(
          VkQueueFamilyProperties &vkQueueFamilyProperties,
          int queueFamilyIndex
  ){
    support.graphics = (vkQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) > 0;
    support.compute = (vkQueueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) > 0;
    this->queueFamilyIndex = queueFamilyIndex;
    this->queueCount = vkQueueFamilyProperties.queueCount;
  }



  void DeviceInfo::updateQueueSurfaceSupport(
          VkPhysicalDevice &vkPhysicalDevice,
          VkSurfaceKHR vkSurface
  ){
    for (unsigned int i = 0; i < queueData.size(); i++) {
      VkBool32 supportsSurface = 0;
      if (vkSurface != VK_NULL_HANDLE) {
        vkGetPhysicalDeviceSurfaceSupportKHR(
                vkPhysicalDevice,
                i,
                vkSurface,
                &supportsSurface
        );
      }
      queueData.at(i).support.surface = supportsSurface;
    }
  }




  void DeviceInfo::updateDeviceSupportSummary(
          VkPhysicalDevice &vkPhysicalDevice,
          VkSurfaceKHR vkSurface
  ){
    for (auto &queue: queueData) {
      if (queue.support.graphics) { supportSummary.graphics = true; }
      if (queue.support.compute) { supportSummary.compute = true; }
      if (queue.support.surface) { supportSummary.surface = true; }
    }
    if (vkSurface != VK_NULL_HANDLE
        && Swapchain::querySwapchainSupport(vkPhysicalDevice, vkSurface).isValid()
    ){
      supportSummary.swapchain = true;
    }
  }






  /// -----------------------------------------------------------------------------------------------
  /// DEVICE INFO CONSTRUCTORS
  /// -----------------------------------------------------------------------------------------------



  DeviceInfo::DeviceInfo(
          VkPhysicalDevice &vkPhysicalDevice,
          VkSurfaceKHR vkSurface,
          DeviceTask deviceTask
  ){
    init(vkPhysicalDevice, vkSurface, deviceTask);
  }


  void DeviceInfo::init(
          VkPhysicalDevice &vkPhysicalDevice,
          VkSurfaceKHR vkSurface,
          DeviceTask deviceTask
  ){
    task = deviceTask;
    getPhysicalDeviceProperties(vkPhysicalDevice);
    getQueueFamilyProperties(vkPhysicalDevice);
    updateQueueSurfaceSupport(vkPhysicalDevice, (task & GRAPHICS) ? vkSurface : VK_NULL_HANDLE);
    updateDeviceSupportSummary(vkPhysicalDevice, (task & GRAPHICS) ? vkSurface : VK_NULL_HANDLE);
    calculateDeviceScore(vkPhysicalDevice);
  }


  void DeviceInfo::clone(
          DeviceInfo const &deviceInfo
  ){
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






  /// -----------------------------------------------------------------------------------------------
  /// DEVICE INFO STATIC FUNCTIONS
  /// -----------------------------------------------------------------------------------------------



  /// @brief list physical devices and get DeviceInfo from each
  std::vector<DeviceInfo> DeviceInfo::getDeviceInfos(
          VkInstance &instance,
          std::vector<VkPhysicalDevice> *vkPhysicalDevices,
          VkSurfaceKHR vkSurface,
          DeviceTask deviceTask
  ){
    /**
     * 1. call once to get the number of devices
     * 2. resize the vector to hold all devices
     * 3. call again to get the actual devices
     */
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
      throw std::runtime_error("failed to find GPUs with Vulkan support");
    }
    vkPhysicalDevices->clear();
    vkPhysicalDevices->resize(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, vkPhysicalDevices->data());

    /// get DeviceInfo from each physical device
    std::vector<DeviceInfo> devices;
    for (auto &physicalDevice: *vkPhysicalDevices) {
      devices.emplace_back(DeviceInfo{physicalDevice, vkSurface, deviceTask});
    }
    return devices;
  }



  std::vector<const char *> DeviceInfo::getExtensions(DeviceTask task) {
    // FIXME : const char * memory leak?
    std::vector<const char *> extensions = {};
    #ifdef __APPLE__
            extensions.push_back("VK_KHR_portability_subset"); // only macOS needs the portability subset
    #elif defined(__linux__)
            // pass
    #else
            #error "This code has only been built for macOS or Linux systems."
    #endif
    if (task & GRAPHICS) {
      extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
    return extensions;
  }





  /// -----------------------------------------------------------------------------------------------
  /// DEVICE INFO INSTANCE FUNCTIONS
  /// -----------------------------------------------------------------------------------------------



  /// @brief get properties and features of the physical device
  void DeviceInfo::getPhysicalDeviceProperties(
          VkPhysicalDevice &vkPhysicalDevice
  ){
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);
    vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &features);
  }



  /// @brief list queue families and get QueueFamilyData from each
  void DeviceInfo::getQueueFamilyProperties(
          VkPhysicalDevice &vkPhysicalDevice
  ){
    /**
     * 1. call once to get the number of queue families
     * 2. resize the vector to hold all queue families
     * 3. call again to get the actual queue families
     */
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
    _queueFamilies.clear();
    _queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, _queueFamilies.data());

    /// get a DeviceInfo::QueueFamilyData from each queue family
    for (int i = 0; i < _queueFamilies.size(); i++) {
      queueData.emplace_back(DeviceInfo::QueueFamilyData{_queueFamilies[i], i});
    }
  }








  bool DeviceInfo::checkDeviceExtensionSupport(
          VkPhysicalDevice &vkPhysicalDevice
  ) const {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(
      vkPhysicalDevice,
      nullptr,
      &extensionCount,
      nullptr
    );
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
        std::cerr << "swapchain not supported -- task = " << task << std::endl;
        return false;
      }
    }
    const bool ret = requiredExtensions.empty();
    if (!ret) {
      std::cerr << "required extensions not supported -- task = " << task << std::endl;
      std::cerr << "missing extensions: " << std::endl;
      for (const auto &extension: requiredExtensions) {
        std::cerr << "\t" << extension << std::endl;
      }
    }
    return ret;
  }




  void DeviceInfo::calculateDeviceScore(
          VkPhysicalDevice &vkPhysicalDevice
  ){
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




    /// FIXME : most suitable for what...? pass in task as an argument
    void DeviceInfo::selectMostSuitedQueue()
    {
      _bestQueueIndex = -1;
      uint32_t bestScore = 0;
      int i = 0;
      for (auto &queue: queueData) {
        queue.score = 0;
        if (queue.support.graphics) { queue.score += 1; }
        if (queue.support.compute) { queue.score += 1; }
        if (queue.support.surface) { queue.score += 1; }
        if ((task & GRAPHICS && (!queue.support.graphics || !queue.support.surface))
            || (task & COMPUTE && !queue.support.compute)
            || (task == ALL && !queue.support.isComplete())
        ){
          std::cerr << "queue " << i << " is not complete" << std::endl;
          queue.score = 0;
        } else {
          std::cout << "queue " << i << " is complete" << std::endl;
        }
        if (bestScore < queue.score) {
          bestScore = queue.score;
          _bestQueueIndex = i;
        }
        i++;
      }
    }



  /// @brief create a logical device and queues for the (single) queue family with the highest suitability score
  void DeviceInfo::createLogicalDevice(
          DeviceInfo &deviceInfo,
          VkPhysicalDevice &vkPhysicalDevice,
          VkDevice *device,
          std::vector<VkQueue> &queues
  ){
    if (deviceInfo._bestQueueIndex == -1) {
      throw std::runtime_error(
        "physical device does not have a suitable Queue. deviceName = "
        + std::string(deviceInfo.properties.deviceName)
      );
    }
    if (!deviceInfo.checkDeviceExtensionSupport(vkPhysicalDevice)) {
      throw std::runtime_error(
        "physical device does not support required extensions. deviceName: "
        + std::string(deviceInfo.properties.deviceName)
      );
    }
    /// TODO : QUEUE REFACTOR - support multiple queue families for a device

    auto bestQueueFamily = deviceInfo.queueData[deviceInfo._bestQueueIndex];

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    std::vector<float> queuePriorities(bestQueueFamily.queueCount, 1.0f);
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = bestQueueFamily.queueFamilyIndex;
    queueCreateInfo.queueCount = bestQueueFamily.queueCount;
    queueCreateInfo.pQueuePriorities = queuePriorities.data();
    queueCreateInfos.push_back(queueCreateInfo);
    std::cout << io::to_color_string(
            io::Color::LIGHT_PURPLE,
            "queueCreateInfos.size() = " + std::to_string(queueCreateInfos.size())
    ) << std::endl;

    // if samplerAnisotropy is supported, enable it
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = deviceInfo.features.samplerAnisotropy;
    const auto extensions = DeviceInfo::getExtensions(deviceInfo.task);

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    /// PRINT SELECTED DEVICE INFO
    std::cout << std::endl;
    std::cout << io::to_color_string(io::Color::CYAN, "creating logical device") << std::endl;
    deviceInfo.print();
    std::cout << std::endl;

    // TODO: add allocator?
    if (vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, device) != VK_SUCCESS) {
      throw std::runtime_error("failed to create logical device!");
    }

    if (bestQueueFamily.score > 0) {
      for (uint32_t qIndex = 0; qIndex < bestQueueFamily.queueCount; qIndex++) {
        queues.push_back(VK_NULL_HANDLE);
        vkGetDeviceQueue(
                *device,
                bestQueueFamily.queueFamilyIndex,
                qIndex,
                &queues.back()
        );
      }
    }
  }


} // namespace walrus