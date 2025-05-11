#!/bin/bash
set -e

INPUT_FILE="$1"
OUTPUT_FILE="$2"

INPUT_FILE_ABS="$(realpath "$INPUT_FILE")"
OUTPUT_FILE_ABS="$(realpath -m "$OUTPUT_FILE")"

mkdir -p runtime

build/compiler -i "$INPUT_FILE_ABS" -o runtime/ASM.S \
&& gcc -o "$OUTPUT_FILE_ABS" runtime/ASM.S