#!/bin/sh

set -eu

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
REPORT_DIR="$ROOT_DIR/Benchmarks/Reports"
TIMESTAMP="$(date '+%Y-%m-%d-%H-%M')"
REPORT_FILE="$REPORT_DIR/Report-$TIMESTAMP.json"

mkdir -p "$REPORT_DIR"

cd "$ROOT_DIR"

echo "Running benchmarks..."
xmake f -m release
xmake run IvyBenchmarks --benchmark_out="$REPORT_FILE"

echo "Report saved to: $REPORT_FILE"
