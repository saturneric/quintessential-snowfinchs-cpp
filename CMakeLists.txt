cmake_minimum_required(VERSION 3.17)
project(L1Compiler)

option(BUILD_TEST_CASES "Build Unit Test Case" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "/Ox")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src)
include_directories(compiler include)

aux_source_directory(src SRC_FILES)
add_executable(compiler main.cpp ${SRC_FILES})

if(BUILD_TEST_CASES)
  add_subdirectory(test)
endif()