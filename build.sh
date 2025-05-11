#!/bin/bash

set -e

SOURCE_DIR=$(pwd)
BUILD_DIR="$SOURCE_DIR/build"

mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"

cmake "$SOURCE_DIR"

cmake --build .

