#!/bin/sh
set -e

echo "📦 Installing Nova system dependencies..."

if command -v pacman >/dev/null 2>&1; then
     pacman -S --needed \
        base-devel clang ccache ninja git curl zip unzip tar \
        pkgconf autoconf autoconf-archive automake libtool doxygen xmake

elif command -v apt >/dev/null 2>&1; then
     apt install \
        build-essential clang ccache ninja-build git curl zip unzip tar \
        pkg-config autoconf autoconf-archive automake libtool doxygen xmake

elif command -v dnf >/dev/null 2>&1; then
     dnf install \
        gcc gcc-c++ clang ccache ninja-build git curl zip unzip tar \
        pkgconf-pkg-config autoconf autoconf-archive automake libtool doxygen xmake

elif command -v xbps-install >/dev/null 2>&1; then
     xbps-install -S
     xbps-install -y \
        base-devel clang ccache ninja git curl zip unzip tar \
        pkg-config autoconf autoconf-archive automake libtool doxygen xmake

else
    echo "❌ Unsupported package manager."
    exit 1
fi

echo "🔍 Verifying tools..."

command -v clang
command -v clang++
command -v ninja
command -v git
command -v xmake
command -v doxygen

echo "🥳 Dependencies Installed!"

ROOT="$(git rev-parse --show-toplevel)"
"$ROOT/scripts/init-submodules.sh"
