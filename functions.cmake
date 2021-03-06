# Usage: add_unit_test_executable(NAME SOURCES [sources...] LIBRARIES [libs...])
function(add_unit_test_executable NAME)
  set(FLAG_ARGS)
  set(SINGLE_VALUE_ARGS)
  set(MULTI_VALUE_ARGS SOURCES LIBRARIES)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "${FLAG_ARGS}" "${SINGLE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}")

  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "add_unit_test_executable: Unparsed arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()
  if(ARG_KEYWORDS_MISSING_VALUES)
    message(FATAL_ERROR "add_unit_test_executable: Keywords missing values: ${ARG_KEYWORDS_MISSING_VALUES}")
  endif()
  if(NOT ARG_SOURCES)
    message(FATAL_ERROR "add_unit_test_executable: SOURCES is required")
  endif()

  add_executable(${NAME} ${CMAKE_SOURCE_DIR}/src/unit_test_util/unit_test_main.cpp ${ARG_SOURCES})
  set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 20)
  set_property(TARGET ${NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)
  set_property(TARGET ${NAME} PROPERTY CXX_EXTENSIONS FALSE)
  target_include_directories(${NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME ${NAME} COMMAND ${NAME})
  target_link_libraries(${NAME} ${ARG_LIBRARIES})
endfunction()
