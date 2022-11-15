#pragma once

#include <vk_types.h>
#include <vector>
#include <string>

namespace vkInit {

  bool checkValidationLayerSupport(const std::vector<const char *> &validationLayers);


  namespace defaults {

    std::vector<const char *> getRequiredExtensions(bool enableValidationLayers = true);

    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
      void *pUserData
    );


    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo();

  }

}

