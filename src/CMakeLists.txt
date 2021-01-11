add_subdirectory(core)
add_subdirectory(debug)

add_executable(engine
  main.cpp
  main.hpp
)
# These are the same dir, but including both for consistency with internal libraries
target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine PUBLIC
  # internal
  core
  debug

  # external
  ${CONAN_LIBS}
  Magnum::Application
  Magnum::GL
  Magnum::Magnum
  MagnumIntegration::ImGui  # TODO test
)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)
set_property(TARGET engine PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET engine PROPERTY CXX_EXTENSIONS FALSE)

# TODO add a function that does this
add_executable(tests core/tst/test.cpp)
add_test(NAME tests COMMAND tests)
target_link_libraries(tests ${CONAN_LIBS})
