#!/bin/bash
set -e

MISSING_PKGS=()
if ! command -v cmake &>/dev/null; then
  MISSING_PKGS+=cmake
fi
if ! command -v ninja &>/dev/null; then
  MISSING_PKGS+=ninja
fi

if [[ ${#MISSING_PKGS[@]} -gt 0 ]]; then
  echo "Installing missing packages: ${MISSING_PKGS[*]}"
  sudo pacman -Sy --noconfirm "${MISSING_PKGS[@]}"
fi

SOURCE_DIR=$(pwd)
BUILD_DIR="$SOURCE_DIR/build"

mkdir -p "$BUILD_DIR"

cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S "$SOURCE_DIR" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR"

