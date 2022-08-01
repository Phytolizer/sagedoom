cmake_minimum_required(VERSION 3.23)

project(glad LANGUAGES C)

add_library(glad src/glad.c)
target_include_directories(glad PUBLIC include)
add_library(glad::glad ALIAS glad)
