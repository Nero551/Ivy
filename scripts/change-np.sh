
#!/bin/sh

set -eu

# Usage: ./scripts/change-np.sh <old_namespace> <new_namespace> <directory>...

if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <old_namespace> <new_namespace> <directory>..." >&2
    exit 1
fi

OLD_NAMESPACE="$1"
NEW_NAMESPACE="$2"
shift 2

if ! command -v clang-change-namespace >/dev/null 2>&1; then
    echo "Error: clang-change-namespace not found." >&2
    exit 1
fi

if [ ! -f compile_commands.json ]; then
    echo "Error: compile_commands.json not found." >&2
    exit 1
fi

for dir in "$@"; do
    if [ ! -d "$dir" ]; then
        echo "Error: directory '$dir' not found." >&2
        exit 1
    fi
done

find "$@" -type f \
    \( -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \) \
    -exec clang-change-namespace \
        -p . \
        --old_namespace="$OLD_NAMESPACE" \
        --new_namespace="$NEW_NAMESPACE" \
        -i {} +

echo "Namespace migration completed: $OLD_NAMESPACE -> $NEW_NAMESPACE"
