#!/bin/bash
set -e

INPUT_FILE="$1"
OUTPUT_FILE="$2"

build/compiler "$INPUT_FILE_ABS" \
&& gcc -g -no-pie -o "$OUTPUT_FILE" ASM.S