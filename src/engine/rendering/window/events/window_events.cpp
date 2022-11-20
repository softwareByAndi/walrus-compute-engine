#include "window_events.hpp"

namespace walrus {

  bool WindowEvents::keyPress(int key) {
    return (glfwGetKey(_window, key) == GLFW_PRESS);
  }
  
} // walrus