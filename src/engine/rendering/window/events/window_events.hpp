#ifndef WALRUS_COMPUTE_ENGINE_WINDOW_EVENTS_HPP
#define WALRUS_COMPUTE_ENGINE_WINDOW_EVENTS_HPP

#include <GLFW/glfw3.h>
#include <vk_types.h>
#include "keys/keys.hpp"

#include <vector>
#include <map>
#include <functional>

namespace walrus {

  class WindowEvents {
  public:
    explicit WindowEvents(GLFWwindow *window);

    ~WindowEvents() = default;

    void addKey(keys::key_t key);

    bool poll();

    bool keyPress(keys::key_t key);

    bool keyRelease(keys::key_t key);

    bool keyActive(keys::key_t key);

  private:
    GLFWwindow *_window = nullptr;
    std::map<keys::key_t, bool> _currentKeys{};
    std::map<keys::key_t, bool> _previousKeys{};
  };

} // walrus

#endif //WALRUS_COMPUTE_ENGINE_WINDOW_EVENTS_HPP
