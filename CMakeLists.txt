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
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  get_target_property(OLD_CXX_FLAGS app CXX_FLAGS)
  set_target_properties(app PROPERTIES CXX_FLAGS "${OLD_CXX_FLAGS} -O0")
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
