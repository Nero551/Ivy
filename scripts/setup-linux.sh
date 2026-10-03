#!/bin/sh
set -e

echo "📦 Installing Nova system dependencies..."

if command -v pacman >/dev/null 2>&1; then
    sudo pacman -S --needed \
        base-devel clang ccache ninja git curl zip unzip tar \
        pkgconf autoconf autoconf-archive automake libtool doxygen

elif command -v apt >/dev/null 2>&1; then
    sudo apt install \
        build-essential clang ccache ninja-build git curl zip unzip tar \
        pkg-config autoconf autoconf-archive automake libtool doxygen

elif command -v dnf >/dev/null 2>&1; then
    sudo dnf install \
        gcc gcc-c++ clang ccache ninja-build git curl zip unzip tar \
        pkgconf-pkg-config autoconf autoconf-archive automake libtool doxygen

elif command -v xbps-install >/dev/null 2>&1; then
    sudo xbps-install -S
    sudo xbps-install -y \
        base-devel clang ccache ninja git curl zip unzip tar \
        pkg-config autoconf autoconf-archive automake libtool doxygen

else
    echo "❌ Unsupported package manager."
    exit 1
fi

echo "🔧 Checking CMake..."

CMAKE_VERSION="4.4.3"

if cmake --version 2>/dev/null | grep -q "cmake version $CMAKE_VERSION"; then
    echo "✅ CMake $CMAKE_VERSION already installed."
else
    echo "📥 Installing CMake $CMAKE_VERSION..."

    curl -L \
        "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.sh" \
        -o /tmp/cmake.sh

    chmod +x /tmp/cmake.sh

    sudo /tmp/cmake.sh \
        --skip-license \
        --prefix=/usr/local

    rm /tmp/cmake.sh

    echo "✅ CMake $CMAKE_VERSION installed."
fi

echo "🔍 Verifying tools..."

command -v clang
command -v clang++
command -v ninja
command -v git
command -v cmake
command -v doxygen

echo "🥳 Dependencies Installed!"

ROOT="$(git rev-parse --show-toplevel)"
"$ROOT/scripts/init-submodules.sh"