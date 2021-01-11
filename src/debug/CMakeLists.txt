message("GOT HERE")
add_library(debug
  imgui_log_sink.cpp
  imgui_log_sink.hpp
  log_window.cpp
  log_window.hpp
  root_debug_menu.cpp
  root_debug_menu.hpp

  widgets/filter.cpp
  widgets/filter.hpp
)
target_include_directories(debug PRIVATE ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})

message("CONAN_LIBS: ${CONAN_LIBS}")
target_link_libraries(debug PUBLIC
  # internal
  core

  # external
  ${CONAN_LIBS} # TODO be explicit?
  Magnum::Application
  Magnum::Magnum
  MagnumIntegration::ImGui
)

# TODO probably make a function that does some of these common things
set_property(TARGET debug PROPERTY CXX_STANDARD 20)
set_property(TARGET debug PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET debug PROPERTY CXX_EXTENSIONS FALSE)
