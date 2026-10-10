#!/usr/bin/env python3

import argparse
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SOURCE_DIR = ROOT / "src"


def analyze(file: Path, fix: bool) -> None:
    command = ["clang-tidy"]

    if fix:
        command.append("-fix")

    command.append(str(file))

    print(f"Analyzing {file.relative_to(ROOT)}", flush=True)

    result = subprocess.run(command, cwd=ROOT)

    if result.returncode != 0:
        print(f"Failed: {file.relative_to(ROOT)}", flush=True)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("-fix", action="store_true")
    parser.add_argument("-j", type=int, default=4, help="Number of parallel jobs")
    args = parser.parse_args()

    files = sorted(
        path
        for path in SOURCE_DIR.rglob("*")
        if path.is_file() and path.suffix in {".cpp", ".hpp"}
    )

    if not files:
        sys.exit("No .cpp or .hpp files found.")

    with ThreadPoolExecutor(max_workers=args.j) as executor:
        list(executor.map(lambda file: analyze(file, args.fix), files))

    print("Done.")


if __name__ == "__main__":
    main()
