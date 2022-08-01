cmake_minimum_required(VERSION 3.23)

project(sagedoom LANGUAGES C)

add_subdirectory(third/glad)

find_package(glfw3 REQUIRED)

add_executable(
  sagedoom
  src/main.c
  src/key_cb.c
  src/mouse_cb.c
  src/math.c
  src/movement.c
  src/render.c
  src/intermission.c
  src/video.c
  src/wad.c
)
target_include_directories(sagedoom PRIVATE include)
target_link_libraries(sagedoom PRIVATE glad::glad glfw m)
