#!/bin/bash
set -e

SOURCE_DIR=$(pwd)
BUILD_DIR="$SOURCE_DIR/build"

MAX_JOBS=16
JOBS=$(nproc)

echo "Detected CPU cores: $JOBS"

if [ "$JOBS" -gt "$MAX_JOBS" ]; then
  JOBS=$MAX_JOBS
fi

echo "Using $JOBS parallel jobs for build"
echo

echo "Current virtual memory (ulimit -v): $(ulimit -v) KB"
echo "All current ulimit settings:"
ulimit -a
echo

mkdir -p "$BUILD_DIR"

# select clang at first
if command -v clang >/dev/null 2>&1 && command -v clang++ >/dev/null 2>&1; then
    export CC=clang
    export CXX=clang++
    echo "Using clang/clang++"
elif command -v gcc >/dev/null 2>&1 && command -v g++ >/dev/null 2>&1; then
    export CC=gcc
    export CXX=g++
    echo "Using gcc/g++"
else
    echo "Error: Neither clang nor gcc is installed!"
    exit 1
fi

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -S "$SOURCE_DIR" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" -- -j$JOBS