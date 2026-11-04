#!/usr/bin/env python3

from pathlib import Path
import argparse


def MergeFiles(source: Path, output: Path, extensions: list[str]) -> None:
    files = sorted(
        (
            file for file in source.rglob("*")
            if file.is_file()
            and file.resolve() != output.resolve()
            and (not extensions or file.suffix.lower() in extensions)
        ),
        key=lambda file: file.relative_to(source).as_posix()
    )

    with output.open("w", encoding="utf-8") as destination:
        for index, file in enumerate(files):
            relative_path = file.relative_to(source)

            destination.write(f"// ==================================================\n")
            destination.write(f"// FILE {index + 1}: {relative_path}\n")
            destination.write(f"// ==================================================\n\n")

            destination.write(file.read_text(encoding="utf-8"))
            destination.write("\n\n")

    print(f"Merged {len(files)} files into {output}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Merge files into one sequential file.")
    parser.add_argument("source", type=Path, help="Source directory")
    parser.add_argument("output", type=Path, help="Output file")
    parser.add_argument(
        "--extensions",
        nargs="*",
        default=[],
        help="Extensions to include, e.g. .hpp .cpp .h"
    )

    args = parser.parse_args()

    extensions = [ext.lower() if ext.startswith(".") else f".{ext.lower()}"
                  for ext in args.extensions]

    MergeFiles(args.source, args.output, extensions)


if __name__ == "__main__":
    main()
