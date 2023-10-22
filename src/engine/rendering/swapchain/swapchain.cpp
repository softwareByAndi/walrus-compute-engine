#include "swapchain.hpp"

#include <limits>
#include <algorithm>
#include <iostream>

namespace walrus {

  /// @brief returns capabilities, formats, and present modes available to the swapchain for the given device and surface
  Swapchain::SupportDetails Swapchain::querySwapchainSupport(
          VkPhysicalDevice &vkPhysicalDevice,
          VkSurfaceKHR &vkSurface
  ){
    Swapchain::SupportDetails details{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            vkPhysicalDevice,
            vkSurface,
            &details.capabilities
    );

    /**
     * 1. call once to get the number of surface formats
     * 2. resize the vector to hold all surface formats
     * 3. call again to get the actual surface formats
     */
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
            vkPhysicalDevice,
            vkSurface,
            &formatCount,
            nullptr
    );
    if (formatCount != 0) {
      details.formats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(
              vkPhysicalDevice,
              vkSurface,
              &formatCount,
              details.formats.data()
      );
    }

    /**
     * 1. call once to get the number of surface present modes
     * 2. resize the vector to hold all surface present modes
     * 3. call again to get the actual surface present modes
     */
    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
            vkPhysicalDevice,
            vkSurface,
            &presentModeCount,
            nullptr
    );
    if (presentModeCount != 0) {
      details.presentModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(
              vkPhysicalDevice,
              vkSurface,
              &presentModeCount,
              details.presentModes.data()
      );
    }
    return details;
  }



  /// @brief returns (4 byte, nonlinear, sRGBA format) if available -- otherwise, returns the first available format
  VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(
          const Swapchain::SupportDetails &swapchainSupportDetails
  ){
    for (const VkSurfaceFormatKHR &availableFormat: swapchainSupportDetails.formats) {
      /// TODO : let user change this preference
      /// hardcoded preference for nonlinear sRGB
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
          && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
      ){
        return availableFormat;
      }
    }
    std::cerr << "Note: desired swapchain format not found. Selecting first format in list." << std::endl;
    return swapchainSupportDetails.formats[0];
  }



  /// @brief returns the best presentation mode available according to a hard coded preference list
  VkPresentModeKHR Swapchain::choosePresentationMode(
          const Swapchain::SupportDetails &swapchainSupportDetails
  ){
    /// TODO : let user change this preference
    /// hardcoded preferences
    std::vector<VkPresentModeKHR> desiredOptions = {
      VK_PRESENT_MODE_FIFO_RELAXED_KHR,
      VK_PRESENT_MODE_FIFO_KHR, // guaranteed
      VK_PRESENT_MODE_MAILBOX_KHR, // optimal if energy usage isn't a concern
      VK_PRESENT_MODE_IMMEDIATE_KHR, // can result in screen tearing.
    };
    // select first available desired option
    for (const VkPresentModeKHR &option: desiredOptions) {
      for (const VkPresentModeKHR &presentation: swapchainSupportDetails.presentModes) {
        if (presentation == option) {
          return option;
        }
      }
    }
    return VK_PRESENT_MODE_FIFO_KHR; // only  FIFO is guaranteed to be available
  }



  /// @brief if not already instantiated, returns an extent that is the same size as the window,
  /// (within the capabilities of the swapchain) -- otherwise, returns the current extent
  VkExtent2D Swapchain::chooseExtent(
          const Swapchain::SupportDetails &swapchainSupportDetails,
          Window &window
  ){
    // the extent is the resolution of the swapchain images
    // and is almost always exactly equal to the resolution of the window that is drawn to (in pixels)
    /// TODO : why are we comparing to max uint32_t here?
    if (swapchainSupportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
      return swapchainSupportDetails.capabilities.currentExtent;
    } else {
      // create a new extent struct the same size as the window
      int width, height;
      glfwGetFramebufferSize(window.getGLFWwindow(), &width, &height);
      VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
      };
      // clamp to the min and max extents supported by the swapchain
      actualExtent.width = std::clamp(actualExtent.width,
                                      swapchainSupportDetails.capabilities.minImageExtent.width,
                                      swapchainSupportDetails.capabilities.maxImageExtent.width);
      actualExtent.height = std::clamp(actualExtent.height,
                                       swapchainSupportDetails.capabilities.minImageExtent.height,
                                       swapchainSupportDetails.capabilities.maxImageExtent.height);
      return actualExtent;
    }
  }

} // namespace walrus