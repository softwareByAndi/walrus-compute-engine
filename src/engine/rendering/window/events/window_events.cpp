#include "window_events.hpp"

namespace walrus {

  WindowEvents::WindowEvents(GLFWwindow *window) : _window{window} {}

  void WindowEvents::addKey(keys::key_t key) {
    _currentKeys[key] = false;
    _previousKeys[key] = false;
  }

  bool WindowEvents::poll() {
    for (auto &[key, value]: _currentKeys) {
      _previousKeys[key] = _currentKeys[key];
      _currentKeys[key] = glfwGetKey(_window, key) == GLFW_PRESS;
    }
  }

  bool WindowEvents::keyPress(keys::key_t key) {
    return _currentKeys[key] && !_previousKeys[key];
  }

  bool WindowEvents::keyActive(keys::key_t key) {
    return _currentKeys[key];
  }

  bool WindowEvents::keyRelease(keys::key_t key) {
    return !_currentKeys[key] && _previousKeys[key];
  }


} // walrus