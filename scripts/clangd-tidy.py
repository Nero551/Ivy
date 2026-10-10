#!/usr/bin/env python3

import argparse
import json
import os
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATABASE = ROOT / "compile_commands.json"


def IsProjectSource(entry: dict) -> bool:
    directory = Path(entry["directory"])
    source = Path(entry["file"])

    if not source.is_absolute():
        source = directory / source

    try:
        relative = source.resolve().relative_to(ROOT)
    except ValueError:
        return False

    return relative.parts[0] == "src"


def AnalyzeSource(source: str, fix: bool) -> tuple[str, int, str]:
    command = [
        "clang-tidy",
        f"-p={DATABASE.parent}",
        "--quiet",
    ]

    if fix:
        command.append("-fix")

    command.append(source)

    result = subprocess.run(
        command,
        cwd=ROOT,
        capture_output=True,
        text=True,
    )

    output = result.stdout + result.stderr
    return source, result.returncode, output


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Run clang-tidy on Ivy sources in parallel."
    )
    parser.add_argument(
        "-fix",
        action="store_true",
        help="Automatically apply suggested fixes.",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=None,
        help="Number of parallel jobs (default: CPU count; 1 when fixing).",
    )
    args = parser.parse_args()

    if not DATABASE.exists():
        sys.exit(f"Compilation database not found: {DATABASE}")

    if args.jobs is not None and args.jobs < 1:
        sys.exit("The number of jobs must be at least 1.")

    with DATABASE.open(encoding="utf-8") as file:
        database = json.load(file)

    sources = sorted(
        {
            str(
                (
                    Path(entry["directory"]) / entry["file"]
                    if not Path(entry["file"]).is_absolute()
                    else Path(entry["file"])
                ).resolve()
            )
            for entry in database
            if IsProjectSource(entry)
        }
    )

    if not sources:
        sys.exit("No Ivy source files found in the compilation database.")

    default_jobs = 1 if args.fix else (os.cpu_count() or 1)
    jobs = args.jobs if args.jobs is not None else default_jobs
    jobs = min(jobs, len(sources))

    print(
        f"Analyzing {len(sources)} Ivy translation units using {jobs} parallel job(s).",
        flush=True,
    )

    failures = 0

    with ThreadPoolExecutor(max_workers=jobs) as executor:
        futures = {
            executor.submit(AnalyzeSource, source, args.fix): source
            for source in sources
        }

        for future in as_completed(futures):
            source, returncode, output = future.result()
            relative = Path(source).relative_to(ROOT)

            print(f"[{'OK' if returncode == 0 else 'FAILED'}] {relative}")

            if output:
                print(output, end="" if output.endswith("\n") else "\n")

            if returncode != 0:
                failures += 1

    if failures:
        sys.exit(f"{failures} translation unit(s) failed.")

    print("clang-tidy completed successfully.")


if __name__ == "__main__":
    main()
