#!/bin/bash
set -e

SOURCE_DIR=$(pwd)
BUILD_DIR="$SOURCE_DIR/build"

MAX_JOBS=6
JOBS=$(nproc)
echo "Detected CPU cores: $JOBS"

if [ "$JOBS" -gt "$MAX_JOBS" ]; then
  JOBS=$MAX_JOBS
fi

mkdir -p "$BUILD_DIR"
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -S "$SOURCE_DIR" -B "$BUILD_DIR"

echo "Using $JOBS parallel jobs for build"
cmake --build "$BUILD_DIR" -- -j$JOBS