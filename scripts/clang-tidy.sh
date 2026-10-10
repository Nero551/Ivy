#!/bin/sh
set -e

find src/ -type f \( -name "*.cpp" -o -name "*.hpp" \) -print0 | \
xargs -0 -P $(nproc) -I {} clang-tidy -p . --fix --fix-errors {}
