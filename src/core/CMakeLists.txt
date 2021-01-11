add_library(core
  logging.cpp
  logging.hpp
  test_module.hpp
)
target_include_directories(core PRIVATE ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(core PUBLIC
  # external
  ${CONAN_LIBS}
  Corrade::Utility
)

set_property(TARGET core PROPERTY CXX_STANDARD 20)
set_property(TARGET core PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET core PROPERTY CXX_EXTENSIONS FALSE)
