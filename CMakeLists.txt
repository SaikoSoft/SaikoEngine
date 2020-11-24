cmake_minimum_required(VERSION 2.8.12)
project(SaikoEngine)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

enable_testing()

add_subdirectory(src)
