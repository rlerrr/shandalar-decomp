#!/usr/bin/env python3
import argparse
import csv
import os
import re
from pathlib import Path


TEXT_FILE_EXTENSIONS = {
    ".c", ".h", ".cpp", ".hpp", ".cc", ".hh",
    ".inl", ".asm", ".inc", ".txt"
}


def normalize_hex(value: str) -> str:
    """
    Normalize something like:
      401234
      0x401234
      00401234
    into lowercase hex with no 0x prefix.
    """
    value = value.strip()
    if value.lower().startswith("0x"):
        value = value[2:]
    return value.lower()


def source_address_variants(addr: str) -> set[str]:
    """
    Return valid source address spellings for matching:
    - 6 hex digits
    - 8 hex digits
    where 8-digit form may just be zero-padded 6-digit form.
    """
    addr = normalize_hex(addr)

    variants = {addr}

    # Also include 8-digit zero-padded form
    if len(addr) < 8:
        variants.add(addr.zfill(8))

    # If address is 8 digits and starts with 00, also include 6-digit form
    if len(addr) == 8 and addr.startswith("00"):
        variants.add(addr[2:])

    return variants


def dest_address_format(addr: str) -> str:
    """
    Keep destination address in a consistent 0xXXXXXXXX style if it fits in 8 digits,
    otherwise just uppercase whatever hex was supplied.
    """
    addr = normalize_hex(addr)
    if len(addr) <= 8:
        return f"0x{addr.zfill(8)}"
    return f"0x{addr}"


def load_mappings(csv_path: Path) -> list[tuple[set[str], str]]:
    """
    Returns a list of tuples:
      ({source_address_variants...}, dest_line_text)
    """
    mappings = []

    with csv_path.open(newline="", encoding="utf-8-sig") as f:
        reader = csv.DictReader(f)

        required = {"Source Address", "Dest Address"}
        missing = required - set(reader.fieldnames or [])
        if missing:
            raise ValueError(f"CSV is missing required columns: {sorted(missing)}")

        for row_num, row in enumerate(reader, start=2):
            src = (row.get("Source Address") or "").strip()
            dst = (row.get("Dest Address") or "").strip()

            if not src or not dst:
                continue

            try:
                src_variants = source_address_variants(src)
                dst_text = dest_address_format(dst)
            except Exception as e:
                print(f"Skipping row {row_num}: bad address data ({e})")
                continue

            mappings.append((src_variants, dst_text))

    return mappings


def compile_source_regex(source_project: str, source_variants: set[str]) -> re.Pattern:
    """
    Match a line like:
      // FUNCTION: FACEMAKER 0x401234
      // FUNCTION: FACEMAKER 0x00401234
    allowing leading whitespace.
    """
    addr_alt = "|".join(re.escape(v) for v in sorted(source_variants))
    pattern = rf"^(\s*)// FUNCTION:\s*{re.escape(source_project)}\s+0x(?:{addr_alt})\s*$"
    return re.compile(pattern, re.IGNORECASE)


def should_process_file(path: Path, all_files: bool) -> bool:
    if not path.is_file():
        return False
    if all_files:
        return True
    return path.suffix.lower() in TEXT_FILE_EXTENSIONS


def process_file(
    path: Path,
    source_project: str,
    dest_project: str,
    mappings: list[tuple[set[str], str]],
    dry_run: bool = False,
) -> int:
    """
    Returns number of inserted lines.
    """
    try:
        original = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        try:
            original = path.read_text(encoding="cp1252")
        except UnicodeDecodeError:
            print(f"Skipping non-text/binary-ish file: {path}")
            return 0

    lines = original.splitlines(keepends=True)
    new_lines = []
    insertions = 0

    # Precompile regexes
    compiled = [
        (compile_source_regex(source_project, src_variants), f"// FUNCTION: {dest_project} {dst_addr}")
        for src_variants, dst_addr in mappings
    ]

    i = 0
    while i < len(lines):
        line = lines[i]
        inserted = False

        for regex, dest_line in compiled:
            m = regex.match(line.rstrip("\r\n"))
            if not m:
                continue

            indent = m.group(1)
            newline = "\n"
            if line.endswith("\r\n"):
                newline = "\r\n"

            dest_line_full = f"{indent}{dest_line}{newline}"

            # Avoid duplicating if the exact line is already immediately above
            if new_lines and new_lines[-1].rstrip("\r\n") == dest_line_full.rstrip("\r\n"):
                pass
            else:
                new_lines.append(dest_line_full)
                insertions += 1

            new_lines.append(line)
            inserted = True
            break

        if not inserted:
            new_lines.append(line)

        i += 1

    if insertions and not dry_run:
        path.write_text("".join(new_lines), encoding="utf-8", newline="")

    return insertions


def main():
    parser = argparse.ArgumentParser(
        description="Insert DESTPROJECT // FUNCTION lines above matching SOURCEPROJECT lines based on a CSV mapping."
    )
    parser.add_argument("csv", type=Path, help="Path to CSV file")
    parser.add_argument("folder", type=Path, help="Folder to scan recursively")
    parser.add_argument("source_project", help="Value for {SOURCEPROJECT}")
    parser.add_argument("dest_project", help="Value for {DESTPROJECT}")
    parser.add_argument(
        "--all-files",
        action="store_true",
        help="Process all files, not just common source/text extensions",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Show what would change without modifying files",
    )

    args = parser.parse_args()

    mappings = load_mappings(args.csv)
    if not mappings:
        print("No usable Source Address / Dest Address rows found in CSV.")
        return

    total_files_changed = 0
    total_insertions = 0

    for path in args.folder.rglob("*"):
        if not should_process_file(path, args.all_files):
            continue

        count = process_file(
            path=path,
            source_project=args.source_project,
            dest_project=args.dest_project,
            mappings=mappings,
            dry_run=args.dry_run,
        )

        if count:
            total_files_changed += 1
            total_insertions += count
            action = "Would update" if args.dry_run else "Updated"
            print(f"{action}: {path} ({count} insertions)")

    print()
    print(f"Files changed: {total_files_changed}")
    print(f"Total insertions: {total_insertions}")


if __name__ == "__main__":
    main()