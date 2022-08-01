#include "doom/key_cb.h"

#include "doom/state.h"

void key_cb(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  (void)scancode;
  (void)mods;

  DoomState* state = glfwGetWindowUserPointer(window);
  switch (key) {
    case GLFW_KEY_W:
      state->keys.w = action == GLFW_PRESS;
      break;
    case GLFW_KEY_S:
      state->keys.s = action == GLFW_PRESS;
      break;
    case GLFW_KEY_A:
      state->keys.a = action == GLFW_PRESS;
      break;
    case GLFW_KEY_D:
      state->keys.d = action == GLFW_PRESS;
      break;
    default:
      break;
  }
}
