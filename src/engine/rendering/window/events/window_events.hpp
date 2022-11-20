#ifndef WALRUS_COMPUTE_ENGINE_WINDOW_EVENTS_HPP
#define WALRUS_COMPUTE_ENGINE_WINDOW_EVENTS_HPP

#include <GLFW/glfw3.h>

namespace walrus {

  class WindowEvents {
  public:
    explicit WindowEvents(GLFWwindow *window) : _window{window} {};

    ~WindowEvents() = default;

    bool keyPress(int key);



  private:
    GLFWwindow *_window;
  };

} // walrus

#endif //WALRUS_COMPUTE_ENGINE_WINDOW_EVENTS_HPP
