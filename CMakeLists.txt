cmake_minimum_required(VERSION 3.17)
project(L1Compiler)

option(BUILD_TEST_CASES "Build Unit Test Case" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "/Ox")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# copy test resources
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

include_directories(src)
include_directories(compiler include)

add_subdirectory(src)

add_executable(compiler L1Compiler.cpp)
target_link_libraries(compiler core)

if(BUILD_TEST_CASES)
  add_subdirectory(test)
endif()