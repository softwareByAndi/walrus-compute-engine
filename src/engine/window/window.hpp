#ifndef WALRUS_COMPUTE_WINDOW_HPP
#define WALRUS_COMPUTE_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vk_initializers.h"
#include <string>
#include <vector>

namespace walrus {

  class Window {
  public:
    Window(int w, int height, std::string name);
    ~Window();

    void destroy();

    Window(const Window &) = delete;
    Window& operator=(const Window &) = delete;

    bool shouldClose() {  return glfwWindowShouldClose(window); }
    bool wasWindowResized() { return frameBufferResized; }
    void resetWindowResizedFlag() { frameBufferResized = false; }
    GLFWwindow *getGLFWwindow() const { return window; }

    VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
    static std::vector<const char *> getRequiredExtensions(bool enableValidationLayers = false);


  private:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    void init();

    int width;
    int height;
    bool frameBufferResized = false;
    bool _isInitialized = false;

    std::string name;
    GLFWwindow *window = nullptr;

  };


} // namespace walrus

#endif //WALRUS_COMPUTE_WINDOW_HPP
