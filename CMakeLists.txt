cmake_minimum_required(VERSION 2.6)

project(chess-ai)

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)

option(build_tests "Build all tests." OFF)

file(GLOB_RECURSE chess-ai_SOURCES "src/chess/*.cpp")

add_library(chess-ai-lib SHARED ${chess-ai_SOURCES})

add_executable(chess-ai "src/main.cpp")
target_link_libraries(chess-ai chess-ai-lib)

if(build_tests)
  add_subdirectory(lib/googletest/googletest/)

  enable_testing()

  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

  file(GLOB_RECURSE chess-ai_TESTS "tests/*.cpp")
  add_executable(unitTests ${chess-ai_TESTS})

  target_link_libraries(unitTests gtest gtest_main chess-ai-lib)
  
  #add_test(NAME runUnitTests COMMAND unitTests)

  add_test(runUnitTests unitTests)
endif()
