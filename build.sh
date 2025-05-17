#!/bin/bash
set -e

SOURCE_DIR=$(pwd)
BUILD_DIR="$SOURCE_DIR/build"

mkdir -p "$BUILD_DIR"

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -S "$SOURCE_DIR" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" -- -j$(nproc)