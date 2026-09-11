#!/usr/bin/env python3
"""Suggest globals by aligning raw original literals with named recomp operands."""

import argparse
import csv
import json
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path


RAW_ADDRESS_RE = re.compile(r"0x([0-9a-f]+)")
NAMED_ADDRESS_RE = re.compile(
    r"\b([A-Za-z_$?][A-Za-z0-9_$?@]*)"
    r"(?:\+((?:0x)?[0-9a-f]+))? \((?:DATA|OFFSET)\)"
)
SOURCE_RE = re.compile(r"\s*\t.*$")


def instruction_text(row):
    return SOURCE_RE.sub("", row[1])


def parse_named_address(text):
    matches = list(NAMED_ADDRESS_RE.finditer(text))
    if len(matches) != 1:
        return None
    match = matches[0]
    offset_text = match.group(2)
    if offset_text is None:
        offset = 0
    elif offset_text.startswith("0x"):
        offset = int(offset_text, 16)
    else:
        offset = int(offset_text, 10)
    return match, match.group(1), offset


def matching_pair(orig_text, recomp_text, image_start, image_end):
    named = parse_named_address(recomp_text)
    if named is None:
        return None
    named_match, name, offset = named

    raw_matches = [
        match
        for match in RAW_ADDRESS_RE.finditer(orig_text)
        if image_start <= int(match.group(1), 16) < image_end
    ]
    if len(raw_matches) != 1:
        return None
    raw_match = raw_matches[0]
    raw_address = int(raw_match.group(1), 16)
    base_address = raw_address - offset
    if base_address < image_start:
        return None

    orig_shape = orig_text[: raw_match.start()] + "{ADDR}" + orig_text[raw_match.end() :]
    recomp_shape = (
        recomp_text[: named_match.start()]
        + "{ADDR}"
        + recomp_text[named_match.end() :]
    )
    if orig_shape != recomp_shape:
        return None
    return base_address, name


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("report", type=Path)
    parser.add_argument("annotations", type=Path)
    parser.add_argument("--image-start", type=lambda value: int(value, 0), default=0x400000)
    parser.add_argument("--image-end", type=lambda value: int(value, 0), default=0x9F7000)
    args = parser.parse_args()

    report = json.loads(args.report.read_text(encoding="utf-8"))
    with args.annotations.open(newline="", encoding="utf-8-sig") as csv_file:
        annotations = list(csv.DictReader(csv_file))
    known_addresses = {int(row["address"], 16) for row in annotations}
    known_names = {row["name"] for row in annotations}

    evidence = defaultdict(list)
    for entity in report["data"]:
        if entity.get("type") != 1:
            continue
        for _, groups in entity.get("diff", []):
            for group in groups:
                orig = group.get("orig", [])
                recomp = group.get("recomp", [])
                if len(orig) != len(recomp):
                    continue
                for orig_row, recomp_row in zip(orig, recomp):
                    pair = matching_pair(
                        instruction_text(orig_row),
                        instruction_text(recomp_row),
                        args.image_start,
                        args.image_end,
                    )
                    if pair is not None:
                        evidence[pair].append((entity["name"], orig_row[0]))

    addresses = defaultdict(set)
    names = defaultdict(set)
    for address, name in evidence:
        addresses[address].add(name)
        names[name].add(address)

    writer = csv.writer(sys.stdout, lineterminator="\n")
    writer.writerow(("references", "functions", "address", "name", "type", "examples"))
    for (address, name), references in sorted(
        evidence.items(), key=lambda item: (-len(item[1]), item[0])
    ):
        if address in known_addresses or name in known_names:
            continue
        if len(addresses[address]) != 1 or len(names[name]) != 1:
            continue
        functions = sorted({function for function, _ in references})
        examples = ";".join(
            f"{function}@{site}" for function, site in references[:3]
        )
        writer.writerow(
            (len(references), len(functions), f"{address:08x}", name, "global", examples)
        )


if __name__ == "__main__":
    main()
