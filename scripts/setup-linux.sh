
#!/bin/sh

set -eu

echo "Installing Ivy development dependencies..."

if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
elif command -v sudo >/dev/null 2>&1; then
    SUDO="sudo"
else
    echo "Error: Run as root or install sudo." >&2
    exit 1
fi

if command -v pacman >/dev/null 2>&1; then
    $SUDO pacman -S --needed \
        base-devel \
        clang clang-tools-extra libc++ libc++abi lld \
        ccache cmake ninja \
        git curl zip unzip tar \
        pkgconf python doxygen gdb \
        xmake

elif command -v apt-get >/dev/null 2>&1; then
    $SUDO apt-get update
    $SUDO apt-get install -y \
        build-essential \
        clang clangd clang-format clang-tidy \
        libc++-dev libc++abi-dev lld \
        ccache cmake ninja-build \
        git curl zip unzip tar \
        pkg-config python3 doxygen gdb \
        xmake

elif command -v dnf >/dev/null 2>&1; then
    $SUDO dnf install -y \
        gcc gcc-c++ make \
        clang clang-tools-extra lld \
        ccache cmake ninja-build \
        git curl zip unzip tar \
        pkgconf-pkg-config python3 doxygen gdb \
        xmake

elif command -v xbps-install >/dev/null 2>&1; then
    $SUDO xbps-install -S
    $SUDO xbps-install -y \
        base-devel \
        clang clang-tools-extra \
        ccache cmake ninja \
        git curl zip unzip tar \
        pkg-config python3 doxygen gdb \
        xmake

else
    echo "Error: Unsupported package manager." >&2
    exit 1
fi

echo "Verifying development tools..."

for tool in clang clang++ cmake ninja git python3 doxygen gdb xmake; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "Error: $tool was not installed correctly." >&2
        exit 1
    fi
done

echo "Ivy development environment ready!"
