#ifndef WALRUS_COMPUTE_DEVICE_HPP
#define WALRUS_COMPUTE_DEVICE_HPP

#include <vector>
#include <vk_types.h>

namespace walrus {

  enum DeviceTask {
    COMPUTE,
    GRAPHICS,
    ALL
  };


  class DeviceInfo {
  public:

    static std::vector<DeviceInfo>
    getDeviceInfos(VkInstance &instance, VkSurfaceKHR &surface, std::vector<VkPhysicalDevice> *vkPhysicalDevices);

    static std::vector<const char *> getExtensions() {
      return {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_KHR_portability_subset"
      };
    }

    struct Support {
      bool graphics = false;
      bool compute = false;
      bool surface = false;

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

    DeviceInfo(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR *surface);

    void clone(DeviceInfo const &deviceInfo);

    void init(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR *surface);

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

    void updateQueueSurfaceSupport(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR *surface);

    void updateDeviceSupportSummary();

    void selectMostSuitedQueue();

    bool checkDeviceExtensionSupport(VkPhysicalDevice &vkPhysicalDevice);

    void calculateDeviceScore(VkPhysicalDevice &vkPhysicalDevice);
  };

} // namespace walrus

#endif //WALRUS_COMPUTE_DEVICE_HPP
