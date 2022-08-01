cmake_minimum_required(VERSION 3.23)

project(str LANGUAGES C)

add_library(str repo/str.c)
target_include_directories(str PUBLIC repo)
add_library(str::str ALIAS str)
