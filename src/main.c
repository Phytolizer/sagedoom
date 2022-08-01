#include "doom/framerate.h"
#include "doom/gl.h"
#include "doom/key_cb.h"
#include "doom/mouse_cb.h"
#include "doom/movement.h"
#include "doom/render.h"
#include "doom/resolution.h"
#include "doom/state.h"

#include <stdio.h>
#include <stdlib.h>

static const float VERTICES[] = {
    // TL
    -1.0F,
    1.0F,
    0.0F,
    0.0F,
    // TR
    1.0F,
    1.0F,
    1.0F,
    0.0F,
    // BL
    1.0F,
    -1.0F,
    1.0F,
    1.0F,
    // BR
    -1.0F,
    -1.0F,
    0.0F,
    1.0F,
};

static const GLuint ELEMENTS[] = {
    0,
    1,
    2,
    2,
    3,
    0,
};

// Vertex shader for a simple texture covering the entire screen.
static const char* const VERTEX_SHADER =
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 texcoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "    texcoord = aTexCoord;\n"
    "}";

static const char* const FRAGMENT_SHADER =
    "#version 330 core\n"
    "in vec2 texcoord;\n"
    "out vec4 color;\n"
    "uniform sampler2D tex;\n"
    "void main() {\n"
    "    color = texture(tex, texcoord);\n"
    "}";

GLuint compile_shaders(void) {
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &VERTEX_SHADER, NULL);
  glCompileShader(vertex_shader);
  GLint success;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    printf("%s\n", info_log);
    return (GLuint)(-1);
  }
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &FRAGMENT_SHADER, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    printf("%s\n", info_log);
    return (GLuint)(-1);
  }
  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetProgramInfoLog(shader_program, 512, NULL, info_log);
    printf("%s\n", info_log);
    return (GLuint)(-1);
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  GLint pos_attrib = glGetAttribLocation(shader_program, "aPos");
  glEnableVertexAttribArray(pos_attrib);
  glVertexAttribPointer(
      pos_attrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  GLint tex_attrib = glGetAttribLocation(shader_program, "aTexCoord");
  glEnableVertexAttribArray(tex_attrib);
  glVertexAttribPointer(tex_attrib,
      2,
      GL_FLOAT,
      GL_FALSE,
      4 * sizeof(float),
      (void*)(2 * sizeof(float)));
  glUseProgram(shader_program);
  return shader_program;
}

void set_pixel(float* pixels, size_t x, size_t y, float r, float g, float b) {
  size_t index = (x + y * DOOM_WIDTH) * 3;
  pixels[index] = r;
  pixels[index + 1] = g;
  pixels[index + 2] = b;
}

void show_pixels(const float* pixel_buffer) {
  glTexImage2D(GL_TEXTURE_2D,
      0,
      GL_RGB,
      DOOM_WIDTH,
      DOOM_HEIGHT,
      0,
      GL_RGB,
      GL_FLOAT,
      pixel_buffer);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void APIENTRY gl_debug_callback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* user_param) {
  (void)source;
  (void)type;
  (void)id;
  (void)severity;
  (void)length;
  (void)user_param;
  printf("%s\n", message);
}

int main(void) {
  if (glfwInit() == GLFW_FALSE) {
    return 1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  GLFWwindow* window = glfwCreateWindow(
      DOOM_WINDOW_WIDTH, DOOM_WINDOW_HEIGHT, "Doom", NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
    return 1;
  }
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  DoomState state = {0};
  state.player.x = 70;
  state.player.y = -110;
  state.player.z = 20;
  state.player.ang = 0;
  glfwSetWindowUserPointer(window, &state);
  glfwSetKeyCallback(window, key_cb);
  glfwSetCursorPosCallback(window, mouse_cb);
  glfwMakeContextCurrent(window);
  if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == GL_FALSE) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  GLint flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_callback, NULL);
    glDebugMessageControl(
        GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
  }

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, sizeof(ELEMENTS), ELEMENTS, GL_STATIC_DRAW);

  GLuint program = compile_shaders();
  if (program == (GLuint)(-1)) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glUniform1i(glGetUniformLocation(program, "tex"), 0);

  for (DoomFrameBufferId id = DOOM_FRAME_BUFFER_ID_PRIMARY;
       id < DOOM_FRAME_BUFFER_IDS_COUNT;
       ++id) {
    state.frame_buffer.screens[id] =
        calloc(DOOM_WIDTH * DOOM_HEIGHT, sizeof(float) * 3);
    if (state.frame_buffer.screens[id] == NULL) {
      glfwDestroyWindow(window);
      glfwTerminate();
      return 1;
    }
  }

  double last_frame = glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
    double now = glfwGetTime();
    double delta = now - last_frame;
    if (delta >= DOOM_FRAME_TIME) {
      glfwPollEvents();
      handle_movement(&state);
      render_frame(&state);
      show_pixels(state.frame_buffer.screens[DOOM_FRAME_BUFFER_ID_PRIMARY]);
      glfwSwapBuffers(window);

      last_frame = now;
    }
  }

  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glDeleteVertexArrays(1, &vao);

  for (DoomFrameBufferId id = DOOM_FRAME_BUFFER_ID_PRIMARY;
       id < DOOM_FRAME_BUFFER_IDS_COUNT;
       ++id) {
    free(state.frame_buffer.screens[id]);
  }
  glfwDestroyWindow(window);
  glfwTerminate();
}
