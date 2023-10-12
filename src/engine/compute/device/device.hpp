#ifndef WALRUS_COMPUTE_DEVICE_HPP
#define WALRUS_COMPUTE_DEVICE_HPP

#include <vector>
#include "vk_types.h"

namespace walrus {

  enum DeviceTask {
    COMPUTE = 1,
    GRAPHICS = 1 << 1,
    ALL = (COMPUTE | GRAPHICS)
  };

  void printTaskFeatures(DeviceTask &task);

  class DeviceInfo {
  public:

    static std::vector<DeviceInfo>
    getDeviceInfos(
      VkInstance &instance,
      std::vector<VkPhysicalDevice> *vkPhysicalDevices,
      VkSurfaceKHR vkSurface = VK_NULL_HANDLE,
      DeviceTask deviceTask = ALL
    );

    static std::vector<const char *> getExtensions(DeviceTask task = ALL) {
      // FIXME : const char * memory leak?
      #ifdef __APPLE__
        std::vector<const char *> extensions = {"VK_KHR_portability_subset"}; // only macOS needs the portability subset
      #elif defined(__linux__)
        std::vector<const char *> extensions = {};
      #else
        #error "This code has only been build for macOS or Linux systems."
      #endif
      if (task & GRAPHICS) {
        extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
      }
      return extensions;
    }

    struct Support {
      bool graphics = false;
      bool compute = false;
      bool surface = false;
      bool swapchain = false;

      [[nodiscard]] bool isComplete() const { return graphics && compute && surface; }
    };

    struct QueueFamilyData {
      explicit QueueFamilyData(VkQueueFamilyProperties &vkProperties);

      Support support;
      uint32_t score = 0;
    };

    std::vector<QueueFamilyData> queueData{};
    VkPhysicalDeviceProperties properties{};
    VkPhysicalDeviceFeatures features{};
    DeviceTask task = ALL;
    uint32_t score = 0;
    Support supportSummary{};

    DeviceInfo() = default;

    explicit DeviceInfo(
      VkPhysicalDevice &vkPhysicalDevice,
      VkSurfaceKHR vkSurface = VK_NULL_HANDLE,
      DeviceTask deviceTask = ALL
    );

    void init(
      VkPhysicalDevice &vkPhysicalDevice,
      VkSurfaceKHR vkSurface = VK_NULL_HANDLE,
      DeviceTask deviceTask = ALL
    );

    void clone(DeviceInfo const &deviceInfo);

    static void createLogicalDevice(
      DeviceInfo &deviceInfo,
      VkPhysicalDevice &vkPhysicalDevice,
      VkDevice *device,
      std::vector<VkQueue> &queues
    );

    void print();

  private:
    int _bestQueueIndex = -1;
    std::vector<VkQueueFamilyProperties> _queueFamilies{};


    void getPhysicalDeviceProperties(VkPhysicalDevice &vkPhysicalDevice);

    void getQueueFamilyProperties(VkPhysicalDevice &vkPhysicalDevice);

    void updateQueueSurfaceSupport(
      VkPhysicalDevice &vkPhysicalDevice,
      VkSurfaceKHR vkSurface = VK_NULL_HANDLE
    );

    void updateDeviceSupportSummary(
      VkPhysicalDevice &vkPhysicalDevice,
      VkSurfaceKHR vkSurface = VK_NULL_HANDLE
    );

    void selectMostSuitedQueue();

    bool checkDeviceExtensionSupport(VkPhysicalDevice &vkPhysicalDevice) const;

    void calculateDeviceScore(VkPhysicalDevice &vkPhysicalDevice);
  };

} // namespace walrus

#endif //WALRUS_COMPUTE_DEVICE_HPP
