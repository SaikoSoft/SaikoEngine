add_executable(engine
  main.cpp main.hpp
  core/logging.hpp core/logging.cpp
  core/test_module.hpp
  debug/root_debug_menu.cpp debug/root_debug_menu.hpp
  debug/log_window.cpp debug/log_window.hpp
  debug/imgui_log_sink.cpp debug/imgui_log_sink.hpp
  debug/widgets/filter.cpp debug/widgets/filter.hpp
)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine PRIVATE ${CONAN_LIBS})
target_link_libraries(engine PRIVATE
  Magnum::Application
  Magnum::GL
  Magnum::Magnum
  MagnumIntegration::ImGui
)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)
set_property(TARGET engine PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET engine PROPERTY CXX_EXTENSIONS FALSE)

add_executable(tests core/tst/test.cpp)
add_test(NAME tests COMMAND tests)
target_link_libraries(tests ${CONAN_LIBS})
