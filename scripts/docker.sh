#!/bin/sh
set -e

docker build -t engine-dev .

echo "🥳 Built Image"

docker run --rm -it \
    --runtime=nvidia \
    --gpus all \
    -e GLFW_PLATFORM=wayland \
    -v "$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY:/tmp/$WAYLAND_DISPLAY" \
    -e WAYLAND_DISPLAY="$WAYLAND_DISPLAY" \
    -e XDG_RUNTIME_DIR=/tmp \
    -v "$PWD/xmake.lua:/Ivy/xmake.lua" \
    -v "$PWD/src:/Ivy/src" \
    -v "$PWD/scripts:/Ivy/scripts" \
    -v "$PWD/Tests:/Ivy/Tests" \
    -v "$PWD/vcpkg.json:/Ivy/vcpkg.json" \
    -v "$PWD/Doxyfile:/Ivy/Doxyfile" \
    -v "$PWD/icon.svg:/Ivy/icon.svg" \
    -v "$PWD/README.md:/Ivy/README.md" \
    -v "$PWD/.clang-format:/Ivy/.clang-format" \
    -v "$PWD/Dockerfile:/Ivy/Dockerfile" \
    -v "$PWD/.clangd:/Ivy/.clangd" \
    -v "$PWD/.git:/Ivy/.git" \
    -v nova-vcpkg-cache:/root/.cache/vcpkg \
    nova-dev