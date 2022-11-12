#include "window.hpp"
#include <stdexcept>
#include <utility>

namespace walrus {

  Window::Window(int w, int h, std::string windowName) : width{w}, height{h}, name{std::move(windowName)} {
    init();
  }


  Window::~Window() {
    destroy();
  }


  void Window::destroy() {
    if (_isInitialized) {
      glfwDestroyWindow(window);
      glfwTerminate();
      _isInitialized = false;
    }
  }


  void Window::init() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    _isInitialized = true;
  }




  void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *pSurface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, pSurface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface");
    }
  }




  std::vector<const char *> Window::getRequiredExtensions(bool enableValidationLayers) {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
  }




  void Window::framebufferResizeCallback(GLFWwindow *pWindow, int width, int height) {
    auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(pWindow));
    window->frameBufferResized = true;
    window->width = width;
    window->height = height;
  }


} // namespace walrus