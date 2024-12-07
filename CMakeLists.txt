cmake_minimum_required(VERSION 3.28)

project(
  2DDrawPlayground
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "app/src/*.cpp")

add_executable(app)

target_sources(app PRIVATE ${SOURCES})
target_include_directories(app PRIVATE "app")
