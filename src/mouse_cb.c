#include "doom/mouse_cb.h"

#include "doom/state.h"

void mouse_cb(GLFWwindow* window, double xpos, double ypos) {
  DoomState* state = glfwGetWindowUserPointer(window);
  state->mouse.dx = xpos - state->mouse.px;
  state->mouse.dy = ypos - state->mouse.py;
  state->mouse.px = xpos;
  state->mouse.py = ypos;
}
