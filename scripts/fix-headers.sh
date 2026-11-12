
#!/bin/sh

set -eu

# Usage: ./scripts/fix-headers.sh <compile_database_directory>

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <compile_database_directory>" >&2
    exit 1
fi

BUILD_DIR="$1"

if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
    echo "Error: compile_commands.json not found in '$BUILD_DIR'." >&2
    exit 1
fi

if ! command -v iwyu_tool.py >/dev/null 2>&1; then
    echo "Error: iwyu_tool.py not found." >&2
    exit 1
fi

if ! command -v fix_includes.py >/dev/null 2>&1; then
    echo "Error: fix_includes.py not found." >&2
    exit 1
fi

echo "Analyzing headers with Include-What-You-Use..."

OUTPUT=$(mktemp)
trap 'rm -f "$OUTPUT"' 0

iwyu_tool.py -p "$BUILD_DIR" > "$OUTPUT"

echo "Applying header fixes..."

fix_includes.py < "$OUTPUT"

echo "Header fixes completed!"
