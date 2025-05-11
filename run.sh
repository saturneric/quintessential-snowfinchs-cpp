#!/bin/bash
set -e

INPUT_FILE="$1"
OUTPUT_FILE="$2"

INPUT_FILE_ABS="$(realpath "$INPUT_FILE")"
OUTPUT_FILE_ABS="$(realpath -m "$OUTPUT_FILE")"

build/compiler "$INPUT_FILE_ABS" \
&& gcc -o "$OUTPUT_FILE_ABS" ASM.S