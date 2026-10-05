#!/bin/sh
set -e

ROOT="$(git rev-parse --show-toplevel)"

"$ROOT/scripts/build.sh"

echo "🧪 Running..."
xmake run -P "$ROOT" Ivy
