#!/bin/sh
set -e

ROOT="$(git rev-parse --show-toplevel)"

"$ROOT/scripts/build-tests.sh"

echo "🧪 Running Tests..."
xmake run -P "$ROOT" IvyTests

echo "✅ All tests passed."