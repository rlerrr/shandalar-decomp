#!/usr/bin/env python3
"""Suggest annotations corroborated by a Ghidra project and IDA trace."""

import argparse
import csv
import json
import re
import sys
from collections import Counter
from pathlib import Path


ADDRESS_RE = re.compile(r"^//-+ \(([0-9A-Fa-f]+)\)")
NAME_RE = re.compile(r"(?:^|\s)([A-Za-z_$?][\w$?@]*)?(?:<[^>]*>)?\s*\(")


def normalized(name):
    while name.startswith(("C_", "j_")):
        name = name[2:]
    return name


def read_trace(path):
    functions = []
    pending_address = None
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        match = ADDRESS_RE.match(line)
        if match:
            pending_address = int(match.group(1), 16)
            continue
        if pending_address is None or not line.strip():
            continue
        match = NAME_RE.search(line)
        if match and match.group(1):
            functions.append((pending_address, normalized(match.group(1))))
        pending_address = None
    return functions


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("ghidra_functions", type=Path)
    parser.add_argument("ida_trace", type=Path)
    parser.add_argument("reference_report", type=Path)
    parser.add_argument("annotations", type=Path)
    args = parser.parse_args()

    with args.ghidra_functions.open(newline="", encoding="utf-8-sig") as csv_file:
        ghidra = list(csv.DictReader(csv_file))
    with args.annotations.open(newline="", encoding="utf-8-sig") as csv_file:
        annotations = list(csv.DictReader(csv_file))
    report = json.loads(args.reference_report.read_text(encoding="utf-8"))

    compiled_names = {
        row["name"] for row in report["data"] if row.get("type") == 1
    }
    known_names = {row["name"] for row in annotations}
    known_addresses = {int(row["address"], 16) for row in annotations}

    trace = read_trace(args.ida_trace)
    trace_counts = Counter(trace)
    trace_by_address = {
        address: name
        for address, name in trace
        if trace_counts[(address, name)] == 1
    }

    writer = csv.writer(sys.stdout, lineterminator="\n")
    writer.writerow(("address", "name", "type", "orig_size", "ghidra_name"))
    for row in ghidra:
        address = int(row["address"], 16)
        name = normalized(row["name"])
        if address in known_addresses or name in known_names:
            continue
        if name not in compiled_names or trace_by_address.get(address) != name:
            continue
        writer.writerow(
            (row["address"], name, "function", row["orig_size"], row["name"])
        )


if __name__ == "__main__":
    main()
