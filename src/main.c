#include "doom/gl.h"
#include "doom/resolution.h"

#include <stdio.h>
#include <stdlib.h>

static const float VERTICES[] = {
    // TL
    -0.5F,
    0.5F,
    0.0F,
    0.0F,
    // TR
    0.5F,
    0.5F,
    1.0F,
    0.0F,
    // BL
    0.5F,
    -0.5F,
    1.0F,
    1.0F,
    // BR
    -0.5F,
    -0.5F,
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
    return -1;
  }
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &FRAGMENT_SHADER, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    printf("%s\n", info_log);
    return -1;
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
    return -1;
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
  return shader_program;
}

void set_pixel(float* pixels, size_t x, size_t y, float r, float g, float b) {
  size_t index = (x + y * DOOM_WINDOW_WIDTH) * 3;
  pixels[index] = r;
  pixels[index + 1] = g;
  pixels[index + 2] = b;
}

void show_pixels(float* pixel_buffer) {
  glTexImage2D(GL_TEXTURE_2D,
      0,
      GL_RGB,
      DOOM_WINDOW_WIDTH,
      DOOM_WINDOW_HEIGHT,
      0,
      GL_RGB,
      GL_FLOAT,
      pixel_buffer);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main(void) {
  if (glfwInit() == GLFW_FALSE) {
    return 1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  GLFWwindow* window = glfwCreateWindow(
      DOOM_WINDOW_WIDTH, DOOM_WINDOW_HEIGHT, "Doom", NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);
  if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == GL_FALSE) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
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

  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glGenerateMipmap(GL_TEXTURE_2D);
  glUniform1i(glGetUniformLocation(program, "tex"), 0);

  float* pixel_buffer =
      calloc(DOOM_WINDOW_WIDTH * DOOM_WINDOW_HEIGHT, sizeof(float) * 3);
  if (pixel_buffer == NULL) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    for (size_t i = 0; i < DOOM_WINDOW_HEIGHT; i++) {
      for (size_t j = 0; j < DOOM_WINDOW_WIDTH; j++) {
        set_pixel(pixel_buffer,
            j,
            i,
            (float)j / DOOM_WINDOW_WIDTH,
            (float)i / DOOM_WINDOW_HEIGHT,
            0.0F);
      }
    }
    show_pixels(pixel_buffer);
    glfwSwapBuffers(window);
  }

  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glDeleteVertexArrays(1, &vao);

  free(pixel_buffer);
  glfwDestroyWindow(window);
  glfwTerminate();
}
