add_subdirectory(core)
add_subdirectory(dbg)

add_executable(engine
  main.cpp
  main.hpp
)
# These are the same dir, but including both for consistency with internal libraries
target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine PUBLIC
  # internal
  core
  dbg

  # external
  ${CONAN_LIBS}
  Magnum::Application
  Magnum::GL
  Magnum::Magnum
)

# TODO: I think this is the recommended way, but how to transitively make sure that extensions are disabled?
# target_compile_features(engine PUBLIC cxx_std_20)
set_property(TARGET engine PROPERTY CXX_STANDARD 20)
set_property(TARGET engine PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET engine PROPERTY CXX_EXTENSIONS FALSE)
