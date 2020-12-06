cmake_minimum_required(VERSION 3.18)
project(SaikoEngine
  HOMEPAGE_URL https://github.com/SaikoSoft/SaikoEngine
  LANGUAGES CXX)

enable_testing()

# Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Corrade + Magnum
add_subdirectory(external/corrade EXCLUDE_FROM_ALL)
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

set(WITH_SDL2APPLICATION ON CACHE BOOL "Use SDL2 with Magnum" FORCE)
# `cmake_policy` settings get reset when Magnum calls project(), so override via the variable instead
set(CMAKE_POLICY_DEFAULT_CMP0074 NEW) # `find_package()` uses `<PackageName>_ROOT` variables.
add_subdirectory(external/magnum EXCLUDE_FROM_ALL)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/external/magnum/modules/")
find_package(Magnum REQUIRED GL Sdl2Application)

# SaikoEngine
add_subdirectory(src)
