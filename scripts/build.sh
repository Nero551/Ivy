#!/bin/sh
set -e

ROOT="$(git rev-parse --show-toplevel)"

echo "🔨 Building..."

xmake build -P "$ROOT" Ivy

echo "✅ Built."