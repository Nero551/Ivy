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
    -v "$PWD/CMakeLists.txt:/Nova/CMakeLists.txt" \
    -v "$PWD/CMakePresets.json:/Nova/CMakePresets.json" \
    -v "$PWD/src:/Nova/src" \
    -v "$PWD/scripts:/Nova/scripts" \
    -v "$PWD/Tests:/Nova/Tests" \
    -v "$PWD/vcpkg.json:/Nova/vcpkg.json" \
    -v "$PWD/Doxyfile:/Nova/Doxyfile" \
    -v "$PWD/icon.svg:/Nova/icon.svg" \
    -v "$PWD/README.md:/Nova/README.md" \
    -v "$PWD/.clang-format:/Nova/.clang-format" \
    -v "$PWD/Dockerfile:/Nova/Dockerfile" \
    -v "$PWD/.clangd:/Nova/.clangd" \
    -v "$PWD/.git:/Nova/.git" \
    -v nova-vcpkg-cache:/root/.cache/vcpkg \
    nova-dev