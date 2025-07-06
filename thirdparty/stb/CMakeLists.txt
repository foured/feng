cmake_minimum_required(VERSION 3.1)
project(stb)

add_library(stb)
target_sources(stb PRIVATE "src/stb_image.cpp" "src/stb_image_write.cpp")
target_include_directories(stb PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")