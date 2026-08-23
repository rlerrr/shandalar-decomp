#!/usr/bin/env python3
"""Group remaining SHANDALAR needs-work functions by source file.

Scans source files for // FUNCTION: comments to map addresses to filenames
and function names, then reads the validation CSV and groups by filename.
"""

import csv
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
VALIDATION_CSV = REPO_ROOT / "docs" / "shandalar_validation_notes.csv"
SRC_DIR = REPO_ROOT / "src"

FUNC_RE = re.compile(r"//\s*FUNCTION:\s*\S+\s+(0x[0-9a-fA-F]+)")
FUNC_DEF_RE = re.compile(
    r"^(?:[\w]+\s+)*(?:\*\s*)?(?:__\w+\s+)*(\w+)\s*\("
)


def build_address_maps(src_dir):
    """Scan .c files for // FUNCTION: comments and build address-to-file
    and address-to-function-name maps."""
    file_map = {}
    func_map = {}

    for path in sorted(src_dir.rglob("*.c")):
        rel = path.relative_to(src_dir)
        filename = rel.name
        addresses = []

        with open(path, encoding="utf-8", errors="replace") as f:
            lines = f.readlines()

        for i, line in enumerate(lines):
            m = FUNC_RE.search(line)
            if m:
                addresses.append((m.group(1).lower(), filename))

            # Check if this line starts a function definition (for the
            # most recently seen FUNCTION comment)
            if addresses:
                stripped = line.lstrip()
                if stripped and not stripped.startswith("//") and not stripped.startswith("#"):
                    dm = FUNC_DEF_RE.match(stripped)
                    if dm:
                        funcname = dm.group(1)
                        for addr, _ in addresses:
                            file_map[addr] = filename
                            func_map[addr] = funcname
                        addresses = []

        # If any addresses remain unmatched (e.g. the function def was on
        # a line we didn't recognize), still record the filename.
        for addr, fn in addresses:
            file_map[addr] = fn

    return file_map, func_map


def main():
    file_map, func_map = build_address_maps(SRC_DIR)

    with open(VALIDATION_CSV, newline="") as f:
        reader = csv.DictReader(f)
        rows = [r for r in reader if r.get("status", "").strip() == "needs-work"]

    by_file = {}
    for row in rows:
        addr = row["address"].strip()
        filename = file_map.get(addr, "UNKNOWN")
        funcname = func_map.get(addr, "???")
        by_file.setdefault(filename, []).append((addr, funcname))

    for filename in sorted(by_file.keys()):
        print(filename)
        for addr, funcname in sorted(by_file[filename]):
            print(f"  {addr} {funcname}")
        print()


if __name__ == "__main__":
    main()
