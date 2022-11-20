#ifndef WALRUS_COMPUTE_ENGINE_KEYS_HPP
#define WALRUS_COMPUTE_ENGINE_KEYS_HPP

#include <GLFW/glfw3.h>

namespace walrus::keys {
  using key_t = int;

  const key_t SPACE = GLFW_KEY_SPACE;
  const key_t ENTER = GLFW_KEY_ENTER;

  const key_t LEFT = GLFW_KEY_LEFT;
  const key_t RIGHT = GLFW_KEY_RIGHT;
  const key_t UP = GLFW_KEY_UP;
  const key_t DOWN = GLFW_KEY_DOWN;

  struct Move {
    static const key_t LEFT = GLFW_KEY_A;
    static const key_t RIGHT = GLFW_KEY_D;
    static const key_t FORWARD = GLFW_KEY_W;
    static const key_t BACKWARD = GLFW_KEY_S;
    static const key_t UP = GLFW_KEY_E;
    static const key_t DOWN = GLFW_KEY_Q;
  };

  struct Look {
    static const key_t LEFT = GLFW_KEY_LEFT;
    static const key_t RIGHT = GLFW_KEY_RIGHT;
    static const key_t UP = GLFW_KEY_UP;
    static const key_t DOWN = GLFW_KEY_DOWN;
  };
  
} // walrus::keys

#endif //WALRUS_COMPUTE_ENGINE_KEYS_HPP
