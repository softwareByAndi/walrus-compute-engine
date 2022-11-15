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

  class DeviceInfo {
  public:

    static std::vector<DeviceInfo>
    getDeviceInfos(VkInstance &instance, VkSurfaceKHR &vkSurface, std::vector<VkPhysicalDevice> *vkPhysicalDevices);

    static std::vector<const char *> getExtensions(DeviceTask task = DeviceTask::ALL) {
      // TODO: only macOS needs the portability subset?
      std::vector<const char *> extensions = {"VK_KHR_portability_subset"};
      if (task & DeviceTask::GRAPHICS) {
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
    DeviceTask task = DeviceTask::ALL;
    uint32_t score = 0;
    Support supportSummary{};

    DeviceInfo() = default;

    explicit DeviceInfo(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR *vkSurface = nullptr);

    void init(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR *vkSurface = nullptr);

    void clone(DeviceInfo const &deviceInfo);

    static void createLogicalDevice(DeviceInfo &deviceInfo,
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

    void updateQueueSurfaceSupport(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR *vkSurface = nullptr);

    void updateDeviceSupportSummary(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR *vkSurface = nullptr);

    void selectMostSuitedQueue();

    bool checkDeviceExtensionSupport(VkPhysicalDevice &vkPhysicalDevice) const;

    void calculateDeviceScore(VkPhysicalDevice &vkPhysicalDevice);
  };

} // namespace walrus

#endif //WALRUS_COMPUTE_DEVICE_HPP
