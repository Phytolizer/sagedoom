cmake_minimum_required(VERSION 3.23)

project(sagedoom LANGUAGES C)

add_subdirectory(third/glad)
add_subdirectory(third/str)

find_package(glfw3 REQUIRED)

add_executable(
  sagedoom
  src/str_ext.c
  src/main.c
  src/key_cb.c
  src/mouse_cb.c
  src/math.c
  src/movement.c
  src/render.c
  src/intermission.c
  src/video.c
  src/wad.c
  src/error.c
  src/file_util.c
  src/zone.c
)
target_include_directories(sagedoom PRIVATE include)
target_link_libraries(sagedoom PRIVATE glad::glad str::str glfw m)
