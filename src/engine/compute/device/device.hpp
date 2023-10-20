#ifndef WALRUS_COMPUTE_DEVICE_HPP
#define WALRUS_COMPUTE_DEVICE_HPP

#include <vector>
#include "vk_types.h"

namespace walrus {

  /**
   * @brief flags describing the intended use of the device
   * @note COMPUTE = computation tasks support. queues must support computation operations
   * @note GRAPHICS = graphics tasks support. queues must support graphics, surface and swapchain
   * @note ALL = both computation and graphics requirements.
   */
  enum DeviceTask {
    COMPUTE = 1,
    GRAPHICS = 1 << 1,
    ALL = (COMPUTE | GRAPHICS)
  };

  /**
   * @brief print to the terminal whether or not a task is for compute &/or graphics
   * @param task the DeviceTask enum
   */
  void printTaskFeatures(DeviceTask &task);

  class DeviceInfo {
  public:

    /**
     * @brief ...
     * @param instance
     * @param vkPhysicalDevices
     * @param vkSurface
     * @param deviceTask
     * @return
     */
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
     /// @brief Support is a simplified reference of what a queue family supports
    struct Support {
      bool graphics = false;
      bool compute = false;
      bool surface = false;
      bool swapchain = false;

      [[nodiscard]] bool isComplete() const { return graphics && compute && surface; }
    };

    /// @brief a redundant wrapper for walrus::DeviceInfo::Support?
    struct QueueFamilyData {
      /// @brief initializes support var. and sets graphics and compute booleans
      explicit QueueFamilyData(VkQueueFamilyProperties &vkProperties);

      /// @brief a variable to reference what a given queue supports
      Support support;
      /// @brief a score variable to track how close this queue supports the users use-case
      uint32_t score = 0;
    };

    /// @brief a vector of support features for each queue in the device
    std::vector<QueueFamilyData> queueData{};
    /// TODO : what is this?
    VkPhysicalDeviceProperties properties{};
    /// TODO : what is this?
    VkPhysicalDeviceFeatures features{};


    /**
     * @brief sets the default required tasks for the device. \n\n
     * the default of ALL is only used by the default constructor
     * -- and is overwritten by the explicit constructor which also defaults to ALL
     */
    DeviceTask task = ALL;

    /// FIXME : do we need a global score?
    /// @brief a variable to score how close the device queue support matches with `task`
    uint32_t score = 0;
    /// FIXME : do we need a global support reference? the queueData vector already holds this data for each queue family
    /// @brief a global support reference as relates to all queues?
    Support supportSummary{};

    /// default constructor
    DeviceInfo() = default;

    /// @brief a wrapper for init()
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
