#ifndef WALRUS_COMPUTE_DEVICE_HPP
#define WALRUS_COMPUTE_DEVICE_HPP

#include <vector>
#include "vk_types.h"

namespace walrus {
  /*
   * TOC:
   * ---------
   * task
   * constructors
   * queue family support
   * static functions
   * instance functions
   * variables
   */





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

  /// --------------------------------------------------
  /// CONSTRUCTORS
  /// --------------------------------------------------
  public:
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




  /// --------------------------------------------------
  /// QUEUE FAMILY SUPPORT
  /// --------------------------------------------------
  public:
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
        explicit QueueFamilyData(
                VkQueueFamilyProperties &vkQueueFamilyProperties,
                int queueFamilyIndex
        );
        /// @brief a variable to reference what a given queue supports
        Support support{};
        /// @brief used to index/reference other data structures
        int queueFamilyIndex = -1;
        /// @brief the number of queues in this family -- used when creating the logical device
        unsigned int queueCount = 0;
        /// @brief a score variable to track how close this queue supports the users use-case
        uint32_t score = 0;
    };

  private:
    void updateQueueSurfaceSupport(
            VkPhysicalDevice &vkPhysicalDevice,
            VkSurfaceKHR vkSurface = VK_NULL_HANDLE
    );

    void updateDeviceSupportSummary(
            VkPhysicalDevice &vkPhysicalDevice,
            VkSurfaceKHR vkSurface = VK_NULL_HANDLE
    );





  /// --------------------------------------------------
  /// STATIC FUNCTIONS
  /// --------------------------------------------------
  public:
    static std::vector<DeviceInfo>
    getDeviceInfos(
            VkInstance &instance,
            std::vector<VkPhysicalDevice> *vkPhysicalDevices,
            VkSurfaceKHR vkSurface = VK_NULL_HANDLE,
            DeviceTask deviceTask = ALL
    );

    static std::vector<const char *> getExtensions(DeviceTask task = ALL);





  /// --------------------------------------------------
  /// INSTANCE FUNCTIONS
  /// --------------------------------------------------
  public:
    void print();

    static void createLogicalDevice(
            DeviceInfo &deviceInfo,
            VkPhysicalDevice &vkPhysicalDevice,
            VkDevice *device,
            std::vector<VkQueue> &queues
    );

  private:
    void getPhysicalDeviceProperties(VkPhysicalDevice &vkPhysicalDevice);

    void getQueueFamilyProperties(VkPhysicalDevice &vkPhysicalDevice);



    bool checkDeviceExtensionSupport(VkPhysicalDevice &vkPhysicalDevice) const;

    void calculateDeviceScore(VkPhysicalDevice &vkPhysicalDevice);

    void selectMostSuitedQueue();





  /// --------------------------------------------------
  /// VARIABLES
  /// --------------------------------------------------
  private:
    int _bestQueueIndex = -1;
    std::vector<VkQueueFamilyProperties> _queueFamilies{};

  public:
    /// @brief a vector of support features for each queue in the device
    std::vector<QueueFamilyData> queueData{};
    VkPhysicalDeviceProperties properties{};
    VkPhysicalDeviceFeatures features{};

    /**
     * @brief sets the default required tasks for the device. \n\n
     * the default of ALL is only used by the default constructor
     * -- and is overwritten by the explicit constructor which also defaults to ALL
     * @TODO : should we factor out a device level task in favor of queue level tasks? then compare requested task with queue level tasks to pull the correct queue from the device?
     */
    DeviceTask task = ALL;

    /**
     * @brief a variable to score how close the device queue support matches with `task`
     * @TODO : this used as a quick suitability reference. consider refactoring to be more descriptive than just a score...?
     */
    uint32_t score = 0;

    /**
     * @brief a device level support reference as across all queues?
     * FIXME : do we need a global support reference? the queueData vector already holds this data for each queue family
     */
    Support supportSummary{};


  }; // class DeviceInfo

} // namespace walrus

#endif //WALRUS_COMPUTE_DEVICE_HPP
