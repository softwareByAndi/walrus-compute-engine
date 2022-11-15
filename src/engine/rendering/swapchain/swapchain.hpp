#ifndef WALRUS_COMPUTE_ENGINE_SWAPCHAIN_HPP
#define WALRUS_COMPUTE_ENGINE_SWAPCHAIN_HPP

#include "engine/window/window.hpp"

#include "vk_types.h"
#include <vector>

namespace walrus {

  struct Swapchain {
    Swapchain() = default;

    ~Swapchain() = default;

    struct SupportDetails {
      VkSurfaceCapabilitiesKHR capabilities{};
      std::vector<VkSurfaceFormatKHR> formats{};
      std::vector<VkPresentModeKHR> presentModes{};

      [[nodiscard]] bool isValid() const { return !formats.empty() && !presentModes.empty(); }
    };

    static Swapchain::SupportDetails querySwapchainSupport(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR &surface);

    [[nodiscard]] static VkSurfaceFormatKHR
    chooseSurfaceFormat(const Swapchain::SupportDetails &swapchainSupportDetails);

    [[nodiscard]] static VkPresentModeKHR
    choosePresentationMode(const Swapchain::SupportDetails &swapchainSupportDetails);

    [[nodiscard]] static VkExtent2D
    chooseExtent(const Swapchain::SupportDetails &swapchainSupportDetails, Window &window);

  };

} // namespace walrus

#endif //WALRUS_COMPUTE_ENGINE_SWAPCHAIN_HPP
