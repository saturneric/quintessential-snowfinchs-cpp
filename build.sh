#!/bin/bash

if ! command -v cmake &> /dev/null; then
  echo "cmake not found, installing..."
  sudo pacman -Sy --noconfirm cmake
fi

set -e

SOURCE_DIR=$(pwd)
BUILD_DIR="$SOURCE_DIR/build"

mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"

cmake "$SOURCE_DIR"

cmake --build .

