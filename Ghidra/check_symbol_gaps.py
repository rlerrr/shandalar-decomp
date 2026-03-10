#!/usr/bin/env python3
"""Check symbol continuity in CSV exports.

Expected CSV columns include at least:
- address
- orig_size

A warning is emitted when `address + orig_size != next_address`.
"""

from __future__ import annotations

import argparse
import csv
import glob
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, List


@dataclass
class Symbol:
    row_number: int
    address_raw: str
    size_raw: str
    address: int
    size: int


def parse_address(value: str, row_number: int) -> int:
    s = value.strip().lower()
    if not s:
        raise ValueError(f"row {row_number}: empty address")
    if s.startswith("0x"):
        s = s[2:]
    if not all(ch in "0123456789abcdef" for ch in s):
        raise ValueError(f"row {row_number}: invalid address value '{value}'")
    return int(s, 16)


def parse_size(value: str, row_number: int) -> int:
    s = value.strip().lower()
    if not s:
        raise ValueError(f"row {row_number}: empty orig_size")

    if s.startswith("0x"):
        return int(s, 16)

    # Treat plain decimal as decimal; any alpha hex digit implies hex.
    if all(ch.isdigit() for ch in s):
        return int(s, 10)

    hex_chars = set("0123456789abcdef")
    if all(ch in hex_chars for ch in s):
        return int(s, 16)

    raise ValueError(f"row {row_number}: invalid orig_size value '{value}'")


def load_symbols(csv_path: Path) -> List[Symbol]:
    symbols: List[Symbol] = []

    with csv_path.open("r", newline="", encoding="utf-8-sig") as f:
        reader = csv.DictReader(f)
        if reader.fieldnames is None:
            raise ValueError("CSV has no header")

        required = {"address", "orig_size"}
        missing = required - set(reader.fieldnames)
        if missing:
            missing_str = ", ".join(sorted(missing))
            raise ValueError(f"missing required column(s): {missing_str}")

        for index, row in enumerate(reader, start=2):  # data starts at line 2
            addr_raw = (row.get("address") or "").strip()
            size_raw = (row.get("orig_size") or "").strip()

            if not addr_raw and not size_raw:
                continue

            address = parse_address(addr_raw, index)
            size = parse_size(size_raw, index)

            symbols.append(
                Symbol(
                    row_number=index,
                    address_raw=addr_raw,
                    size_raw=size_raw,
                    address=address,
                    size=size,
                )
            )

    return symbols


def classify_delta(expected_next: int, next_address: int, delta: int) -> str:
    if delta == 0:
        return "contiguous"
    if delta < 0:
        return "overlap"

    align_pad = (16 - (expected_next % 16)) % 16
    if align_pad == 0:
        return "probable_noncode_gap"
    if next_address % 16 == 0:
        if delta == align_pad:
            return "likely_alignment"
        if delta > align_pad:
            return "alignment_plus_extra"
    return "probable_noncode_gap"


def check_gaps(symbols: List[Symbol], csv_path: Path, show_alignment: bool) -> int:
    issues = 0
    counts = {
        "contiguous": 0,
        "likely_alignment": 0,
        "alignment_plus_extra": 0,
        "probable_noncode_gap": 0,
        "overlap": 0,
    }

    if len(symbols) < 2:
        print(f"{csv_path}: not enough rows to compare")
        return issues

    for current, nxt in zip(symbols, symbols[1:]):
        expected_next = current.address + current.size
        delta = nxt.address - expected_next
        category = classify_delta(expected_next, nxt.address, delta)
        counts[category] += 1

        if category != "contiguous":
            should_print = (
                category in {"overlap", "alignment_plus_extra", "probable_noncode_gap"}
                or show_alignment
            )
            if not should_print:
                continue
            extra = ""
            if category == "probable_noncode_gap" and delta > 0:
                fixed_size = nxt.address - current.address
                extra = f", fixed_size {fixed_size}"
            print(
                f"{csv_path}: {category} at row {current.row_number} -> {nxt.row_number} | "
                f"addr 0x{current.address:08x} + size {current.size} = 0x{expected_next:08x}, "
                f"next addr 0x{nxt.address:08x} (delta {delta:+d}{extra})"
            )
            if category != "likely_alignment":
                issues += 1

    print(
        f"{csv_path}: summary | contiguous={counts['contiguous']}, "
        f"likely_alignment={counts['likely_alignment']}, "
        f"alignment_plus_extra={counts['alignment_plus_extra']}, "
        f"probable_noncode_gap={counts['probable_noncode_gap']}, "
        f"overlap={counts['overlap']}"
    )
    if issues == 0:
        print(f"{csv_path}: no suspicious gaps found ({len(symbols)} symbols checked)")

    return issues


def expand_inputs(patterns: Iterable[str]) -> List[Path]:
    files: List[Path] = []
    for pattern in patterns:
        matches = [Path(p) for p in glob.glob(pattern)]
        if matches:
            files.extend(matches)
        else:
            files.append(Path(pattern))

    # Deduplicate while preserving order.
    seen = set()
    unique: List[Path] = []
    for p in files:
        rp = str(p.resolve()) if p.exists() else str(p)
        if rp not in seen:
            seen.add(rp)
            unique.append(p)

    return unique


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Warn about symbol address gaps in CSV files."
    )
    parser.add_argument(
        "inputs",
        nargs="+",
        help="CSV file path(s) or glob pattern(s), e.g. *.csv",
    )
    parser.add_argument(
        "--show-alignment",
        action="store_true",
        help="Also print likely pure 16-byte alignment padding gaps.",
    )
    args = parser.parse_args()

    files = expand_inputs(args.inputs)
    total_issues = 0
    files_checked = 0

    for csv_file in files:
        if not csv_file.exists():
            print(f"{csv_file}: file not found")
            total_issues += 1
            continue

        try:
            symbols = load_symbols(csv_file)
            total_issues += check_gaps(symbols, csv_file, args.show_alignment)
            files_checked += 1
        except Exception as exc:
            print(f"{csv_file}: error: {exc}")
            total_issues += 1

    print(
        f"Checked {files_checked} file(s). "
        f"Total issue count: {total_issues}"
    )

    return 1 if total_issues else 0


if __name__ == "__main__":
    raise SystemExit(main())
