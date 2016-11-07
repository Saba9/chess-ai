cmake_minimum_required(VERSION 2.6)

project(chess-ai)

option(build_tests "Build all tests." OFF)

file(GLOB_RECURSE chess-ai_SOURCES "src/*.cpp")
add_executable(chess-ai ${chess-ai_SOURCES})

if(build_tests)
  add_subdirectory(lib/googletest/googletest/)

  enable_testing()

  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

  file(GLOB_RECURSE chess-ai_TESTS "tests/*.cpp")
  add_executable(unitTests ${chess-ai_TESTS})

  target_link_libraries(unitTests gtest gtest_main)
  
  #add_test(NAME runUnitTests COMMAND unitTests)

  add_test(runUnitTests unitTests)
endif()
