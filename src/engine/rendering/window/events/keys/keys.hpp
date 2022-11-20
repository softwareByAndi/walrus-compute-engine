#ifndef WALRUS_COMPUTE_ENGINE_KEYS_HPP
#define WALRUS_COMPUTE_ENGINE_KEYS_HPP

#include <GLFW/glfw3.h>

namespace walrus::keys {

  const int SPACE = GLFW_KEY_SPACE;
  const int ENTER = GLFW_KEY_ENTER;

  const int LEFT = GLFW_KEY_LEFT;
  const int RIGHT = GLFW_KEY_RIGHT;
  const int UP = GLFW_KEY_UP;
  const int DOWN = GLFW_KEY_DOWN;

  struct Move {
    static const int LEFT = GLFW_KEY_A;
    static const int RIGHT = GLFW_KEY_D;
    static const int FORWARD = GLFW_KEY_W;
    static const int BACKWARD = GLFW_KEY_S;
    static const int UP = GLFW_KEY_E;
    static const int DOWN = GLFW_KEY_Q;
  };

  struct Look {
    static const int LEFT = GLFW_KEY_LEFT;
    static const int RIGHT = GLFW_KEY_RIGHT;
    static const int UP = GLFW_KEY_UP;
    static const int DOWN = GLFW_KEY_DOWN;
  };
  
} // walrus::keys

#endif //WALRUS_COMPUTE_ENGINE_KEYS_HPP
