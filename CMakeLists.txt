cmake_minimum_required(VERSION 3.16)
project(SaikoEngine
  HOMEPAGE_URL https://github.com/SaikoSoft/SaikoEngine
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
enable_testing()

include(functions.cmake)

# Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

# Make Conan include all libaries as SYSTEM includes, which suppresses warnings from those files
set(CONAN_SYSTEM_INCLUDES TRUE)
# Conan by default sets CMAKE_*_OUTPUT_DIRECTORY_<CONFIG> variables, which causes Magnum's plugin import to fail because
# the plugin library gets built into a non-default directory: https://github.com/mosra/magnum/issues/486
# Using NO_OUTPUT_DIRS to skip this behavior.
conan_basic_setup(NO_OUTPUT_DIRS)

# Corrade + Magnum
add_subdirectory(external/corrade EXCLUDE_FROM_ALL)
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

set(WITH_SDL2APPLICATION ON CACHE BOOL "Use SDL2 with Magnum" FORCE)

# `cmake_policy` settings get reset when Magnum calls project(), so override via the variable instead
set(CMAKE_POLICY_DEFAULT_CMP0074 NEW) # `find_package()` uses `<PackageName>_ROOT` variables.
add_subdirectory(external/magnum EXCLUDE_FROM_ALL)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/external/magnum/modules/")
find_package(Magnum REQUIRED
  GL
  Sdl2Application
)

add_subdirectory(external/magnum-plugins EXCLUDE_FROM_ALL)

set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/external/imgui")
set(WITH_IMGUI ON CACHE BOOL "Enable Magnum + Dear ImGui integration" FORCE)
add_subdirectory(external/magnum-integration EXCLUDE_FROM_ALL)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/external/magnum-integration/modules/")
find_package(MagnumIntegration REQUIRED ImGui)

# spdlog
add_compile_definitions(SPDLOG_FMT_EXTERNAL) # use my version of {fmt} instead of the version bundled with spdlog

# SaikoEngine
add_subdirectory(src)
