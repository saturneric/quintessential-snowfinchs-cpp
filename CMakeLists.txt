cmake_minimum_required(VERSION 3.17)
project(syntaxParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_RELEASE "/Ox")

include_directories(src)

aux_source_directory(src SRC_FILES)

add_executable(compiler main.cpp ${SRC_FILES})