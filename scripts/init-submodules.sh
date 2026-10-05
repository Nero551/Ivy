#!/bin/sh
set -e

ROOT="$(git rev-parse --show-toplevel)"

echo "📦 Checking Submodules..."
git -C "$ROOT" submodule update --init --recursive

VCPKG="$ROOT/External/vcpkg/vcpkg"

if [ ! -x "$VCPKG" ]; then
    echo "🔧 Bootstrapping vcpkg..."
    "$ROOT/External/vcpkg/bootstrap-vcpkg.sh --disable-metrics"
fi

echo "✅ Submodules Initialized"