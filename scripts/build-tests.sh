#!/bin/sh
set -e

ROOT="$(git rev-parse --show-toplevel)"

echo "🔨 Building Tests..."

xmake build -P "$ROOT" IvyTests

echo "✅ Built Tests."
