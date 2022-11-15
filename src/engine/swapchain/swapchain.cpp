#include "swapchain.hpp"

#include <limits>
#include <algorithm>
#include <iostream>

namespace walrus {

  Swapchain::SupportDetails
  Swapchain::querySwapchainSupport(VkPhysicalDevice &vkPhysicalDevice, VkSurfaceKHR &vkSurface) {
    Swapchain::SupportDetails details{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, vkSurface, &details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &formatCount, nullptr);
    if (formatCount != 0) {
      details.formats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
      details.presentModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &presentModeCount,
                                                details.presentModes.data());
    }
    return details;
  }




  VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(const Swapchain::SupportDetails &swapchainSupportDetails) {
    for (const auto &availableFormat: swapchainSupportDetails.formats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return availableFormat;
      }
    }
    std::cerr << "Note: desired swapchain format not found. Selecting first format in list." << std::endl;
    return swapchainSupportDetails.formats[0];
  }




  VkPresentModeKHR Swapchain::choosePresentationMode(const Swapchain::SupportDetails &swapchainSupportDetails) {
    std::vector<VkPresentModeKHR> desiredOptions = {
      VK_PRESENT_MODE_FIFO_RELAXED_KHR,
      VK_PRESENT_MODE_FIFO_KHR, // guaranteed
      VK_PRESENT_MODE_MAILBOX_KHR, // optimal if energy usage isn't a concern
      VK_PRESENT_MODE_IMMEDIATE_KHR, // can result in screen tearing.
    };
    // select first available desired option
    for (const auto &option: desiredOptions) {
      for (const auto &presentation: swapchainSupportDetails.presentModes) {
        if (presentation == option) {
          return option;
        }
      }
    }
    return VK_PRESENT_MODE_FIFO_KHR; // only  FIFO is guaranteed to be available
  }




  VkExtent2D Swapchain::chooseExtent(const Swapchain::SupportDetails &swapchainSupportDetails, Window &window) {
    // the extent is the resolution of the swapchain images and is almost always exactly equal to the resolution of the window that we're drawing to (in pixels)
    if (swapchainSupportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
      return swapchainSupportDetails.capabilities.currentExtent;
    } else {
      int width, height;
      glfwGetFramebufferSize(window.getGLFWwindow(), &width, &height);
      VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
      };
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