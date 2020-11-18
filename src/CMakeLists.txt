cmake_minimum_required(VERSION 2.8.12)
project(SaikoEngine)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(engine main.cpp)
target_link_libraries(engine ${CONAN_LIBS})

set_property(TARGET engine PROPERTY CXX_STANDARD 20)
set_property(TARGET engine PROPERTY CXX_STANDARD_REQUIRED TRUE)

enable_testing()

add_executable(tests core/tst/test.cpp)
add_test(NAME tests COMMAND tests)
target_link_libraries(tests ${CONAN_LIBS})
