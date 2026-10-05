#!/bin/sh
set -e

ROOT="$(git rev-parse --show-toplevel)"

echo "📦 Checking Submodules..."
git -C "$ROOT" submodule update --init --recursive

echo "✅ Submodules Initialized"