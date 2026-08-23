#!/usr/bin/env python3
"""Build SHANDALAR validation reports from reccmp JSON and manual notes."""

import argparse
import csv
import json
import re
import sqlite3
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
FUNCTION_COMMENT_RE = re.compile(
    r"//\s*FUNCTION:\s+SHANDALAR\s+(0x[0-9a-fA-F]+)"
)


def normalize_address(value):
    text = str(value).strip()
    if text.lower().startswith("0x"):
        return "0x%08x" % int(text, 16)
    return "0x%08x" % int(text, 16)


def read_ghidra_functions(path):
    functions = {}
    with path.open(newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if row.get("type") != "function":
                continue
            address = normalize_address(row["address"])
            functions[address] = {
                "address": address,
                "orig_size": row.get("orig_size", ""),
            }
    return functions


def read_reccmp_json(path):
    with path.open() as f:
        payload = json.load(f)
    functions = {}
    for item in payload.get("data", []):
        address = normalize_address(item["address"])
        functions[address] = {
            "name": item.get("name", ""),
            "matching": item.get("matching"),
            "effective": bool(item.get("effective", False)),
            "recomp": item.get("recomp", ""),
        }
    return functions


def scan_source_locations(src_root):
    locations = {}
    for path in src_root.rglob("*"):
        if path.suffix.lower() not in (".c", ".cpp", ".h", ".inc"):
            continue
        try:
            lines = path.read_text(errors="replace").splitlines()
        except OSError:
            continue
        for index, line in enumerate(lines, 1):
            match = FUNCTION_COMMENT_RE.search(line)
            if not match:
                continue
            address = normalize_address(match.group(1))
            relpath = path.relative_to(REPO_ROOT).as_posix()
            locations.setdefault(address, []).append("%s:%d" % (relpath, index))
    return locations


def read_manual_notes(path):
    if not path.exists():
        return {}
    with path.open(newline="") as f:
        reader = csv.DictReader(f)
        notes = {}
        for row in reader:
            address = normalize_address(row["address"])
            notes[address] = row
        return notes


def status_for(row, note, exact_match):
    manual_status = (note.get("status") or "").strip()
    if manual_status:
        return manual_status
    if row["matching"] == "":
        return "not-implemented"
    if exact_match:
        return "exact"
    if row["effective"] == "yes":
        return "reccmp-effective"
    return "needs-review"


def build_rows(ghidra, reccmp, locations, notes):
    rows = []
    all_addresses = sorted(
        set(ghidra.keys()) | set(reccmp.keys()) | set(notes.keys()),
        key=lambda value: int(value, 16),
    )
    for address in all_addresses:
        rec = reccmp.get(address, {})
        matching = rec.get("matching")
        exact_match = matching == 1.0
        note = notes.get(address, {})
        row = {
            "address": address,
            "name": rec.get("name", ""),
            "orig_size": ghidra.get(address, {}).get("orig_size", ""),
            "source_locations": "; ".join(locations.get(address, [])),
            "recomp": rec.get("recomp", ""),
            "matching": "" if matching is None else "%.2f" % (matching * 100.0),
            "effective": "yes" if rec.get("effective") else "",
            "status": "",
            "confidence": (note.get("confidence") or "").strip(),
            "reviewer": (note.get("reviewer") or "").strip(),
            "reviewed_commit": (note.get("reviewed_commit") or "").strip(),
            "notes": (note.get("notes") or "").strip(),
        }
        row["status"] = status_for(row, note, exact_match)
        rows.append(row)
    return rows


def write_csv(path, rows):
    path.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = [
        "address",
        "name",
        "status",
        "confidence",
        "matching",
        "effective",
        "orig_size",
        "source_locations",
        "recomp",
        "reviewer",
        "reviewed_commit",
        "notes",
    ]
    with path.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def write_sqlite(path, rows):
    path.parent.mkdir(parents=True, exist_ok=True)
    conn = sqlite3.connect(str(path))
    try:
        conn.execute("drop table if exists shandalar_validation")
        conn.execute(
            """
            create table shandalar_validation (
              address text primary key,
              name text,
              status text,
              confidence text,
              matching real,
              effective integer,
              orig_size integer,
              source_locations text,
              recomp text,
              reviewer text,
              reviewed_commit text,
              notes text
            )
            """
        )
        for row in rows:
            conn.execute(
                """
                insert into shandalar_validation values
                (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                """,
                (
                    row["address"],
                    row["name"],
                    row["status"],
                    row["confidence"],
                    None if row["matching"] == "" else float(row["matching"]),
                    1 if row["effective"] == "yes" else 0,
                    None if row["orig_size"] == "" else int(row["orig_size"]),
                    row["source_locations"],
                    row["recomp"],
                    row["reviewer"],
                    row["reviewed_commit"],
                    row["notes"],
                ),
            )
        conn.commit()
    finally:
        conn.close()


def write_markdown(path, rows):
    path.parent.mkdir(parents=True, exist_ok=True)
    counts = {}
    for row in rows:
        counts[row["status"]] = counts.get(row["status"], 0) + 1
    implemented = sum(1 for row in rows if row["matching"] != "")
    exact = counts.get("exact", 0)
    effective = counts.get("reccmp-effective", 0)
    reviewed = counts.get("reviewed", 0)
    accepted_risk = counts.get("accepted-risk", 0)
    needs_work = counts.get("needs-work", 0)
    needs_review = counts.get("needs-review", 0)
    not_implemented = counts.get("not-implemented", 0)

    review_rows = [
        row for row in rows if row["status"] == "needs-review"
    ]
    review_rows.sort(key=lambda row: float(row["matching"] or "0"))
    highest_review_rows = list(reversed(review_rows))

    with path.open("w") as f:
        f.write("# SHANDALAR Validation Report\n\n")
        f.write("Generated from `Ghidra/shandalar.csv`, reccmp JSON, source comments, and manual notes.\n\n")
        f.write("## Summary\n\n")
        f.write("| Metric | Count |\n")
        f.write("| --- | ---: |\n")
        f.write("| Total functions | %d |\n" % len(rows))
        f.write("| Implemented by reccmp | %d |\n" % implemented)
        f.write("| Exact assembly matches | %d |\n" % exact)
        f.write("| reccmp effective nonmatches | %d |\n" % effective)
        f.write("| Manually accepted reviews | %d |\n" % (reviewed + accepted_risk))
        f.write("| Needs-work non-accepted reviews | %d |\n" % needs_work)
        f.write("| Needs manual review | %d |\n" % needs_review)
        f.write("| Not implemented | %d |\n\n" % not_implemented)

        f.write("## Status Counts\n\n")
        f.write("| Status | Count |\n")
        f.write("| --- | ---: |\n")
        for status in sorted(counts):
            f.write("| `%s` | %d |\n" % (status, counts[status]))

        f.write("\n## Highest Matching Implemented Functions Needing Review\n\n")
        f.write("| Address | Match | Name | Source |\n")
        f.write("| --- | ---: | --- | --- |\n")
        for row in highest_review_rows[:50]:
            f.write(
                "| `%s` | %s%% | `%s` | `%s` |\n"
                % (
                    row["address"],
                    row["matching"],
                    row["name"],
                    row["source_locations"],
                )
            )

        f.write("\n## Lowest Matching Implemented Functions Needing Review\n\n")
        f.write("| Address | Match | Name | Source |\n")
        f.write("| --- | ---: | --- | --- |\n")
        for row in review_rows[:50]:
            f.write(
                "| `%s` | %s%% | `%s` | `%s` |\n"
                % (
                    row["address"],
                    row["matching"],
                    row["name"],
                    row["source_locations"],
                )
            )


def validate_notes(notes, known_addresses):
    unknown = sorted(set(notes.keys()) - known_addresses, key=lambda value: int(value, 16))
    if unknown:
        raise SystemExit(
            "Manual notes contain unknown SHANDALAR addresses: %s"
            % ", ".join(unknown)
        )


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--reccmp-json", default="temp/shandalar_reccmp.json")
    parser.add_argument("--notes", default="docs/shandalar_validation_notes.csv")
    parser.add_argument("--csv", default="temp/shandalar_validation.csv")
    parser.add_argument("--sqlite", default="temp/shandalar_validation.sqlite")
    parser.add_argument("--md", default="temp/shandalar_validation.md")
    return parser.parse_args()


def main():
    args = parse_args()
    ghidra = read_ghidra_functions(REPO_ROOT / "Ghidra" / "shandalar.csv")
    reccmp = read_reccmp_json(REPO_ROOT / args.reccmp_json)
    locations = scan_source_locations(REPO_ROOT / "src")
    notes = read_manual_notes(REPO_ROOT / args.notes)
    validate_notes(notes, set(ghidra.keys()) | set(reccmp.keys()) | set(locations.keys()))
    rows = build_rows(ghidra, reccmp, locations, notes)
    write_csv(REPO_ROOT / args.csv, rows)
    write_sqlite(REPO_ROOT / args.sqlite, rows)
    write_markdown(REPO_ROOT / args.md, rows)
    print("Wrote %d SHANDALAR validation rows." % len(rows))


if __name__ == "__main__":
    main()
