cmake_minimum_required(VERSION 3.28)

project(
  2DDrawPlaygroundTests
  VERSION 0.1.0
  LANGUAGES CXX)

find_package(doctest CONFIG REQUIRED)
function(register_test target)
  add_executable("${target}" "${target}.cpp")

  target_link_libraries("${target}" PRIVATE doctest::doctest)
  target_include_directories("${target}" PRIVATE ../app)
  target_compile_options("${target}" PRIVATE --coverage)
  target_link_options("${target}" PRIVATE --coverage)
  add_test(
    NAME ${target}
    COMMAND ${target}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

endfunction()

file(GLOB SOURCES "*.cpp")
foreach(source ${SOURCES})
  message(STATUS "Found test: ${source}")
  get_filename_component(target ${source} NAME_WE)
  register_test(${target})
endforeach()
