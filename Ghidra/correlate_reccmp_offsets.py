#!/usr/bin/env python3
"""Suggest CSV annotations from an offset-addressed reccmp JSON report."""

import argparse
import csv
import json
import re
from collections import defaultdict
from pathlib import Path


OFFSET_RE = re.compile(r"<OFFSET(0x[0-9a-fA-F]+)>")
ENTITY_RE = re.compile(r"([A-Za-z_][A-Za-z0-9_]*) \((DATA|FUNCTION)\)")
SOURCE_RE = re.compile(r"\s*\t.*$")


def instruction(row):
    return SOURCE_RE.sub("", str(row[1])).strip()


def reference(row, original):
    match = (OFFSET_RE if original else ENTITY_RE).search(instruction(row))
    if match is None:
        return None
    if original:
        return int(match.group(1), 16), None
    return match.group(1), match.group(2)


def shape(row, original):
    text = instruction(row)
    regex = OFFSET_RE if original else ENTITY_RE
    return regex.sub("<REFERENCE>", text)


def add_candidate(candidates, orig_row, recomp_row, source):
    orig_ref = reference(orig_row, True)
    recomp_ref = reference(recomp_row, False)
    if orig_ref is None or recomp_ref is None:
        return
    address = orig_ref[0]
    name, kind = recomp_ref
    if 0x401000 <= address < 0x800000:
        candidates[(address, name, kind)].append(source)


def pair_ordered_calls(candidates, orig, recomp, source):
    orig_calls = [
        row
        for row in orig
        if instruction(row).startswith("call ") and reference(row, True)
    ]
    recomp_calls = [
        row
        for row in recomp
        if instruction(row).startswith("call ")
        and reference(row, False)
        and reference(row, False)[1] == "FUNCTION"
    ]
    if orig_calls and len(orig_calls) == len(recomp_calls):
        for orig_row, recomp_row in zip(orig_calls, recomp_calls):
            add_candidate(candidates, orig_row, recomp_row, source)


def collect_candidates(report):
    candidates = defaultdict(list)
    for entity in report["data"]:
        entity_name = entity["name"]
        entity_orig = []
        entity_recomp = []
        for hunk in entity.get("diff", []):
            hunk_orig = []
            hunk_recomp = []
            for group in hunk[1]:
                orig = group.get("orig") or []
                recomp = group.get("recomp") or []
                hunk_orig.extend(orig)
                hunk_recomp.extend(recomp)
                entity_orig.extend(orig)
                entity_recomp.extend(recomp)
                if not orig or not recomp:
                    continue

                orig_by_shape = defaultdict(list)
                recomp_by_shape = defaultdict(list)
                for row in orig:
                    if reference(row, True):
                        orig_by_shape[shape(row, True)].append(row)
                for row in recomp:
                    if reference(row, False):
                        recomp_by_shape[shape(row, False)].append(row)

                for key in orig_by_shape.keys() & recomp_by_shape.keys():
                    left = orig_by_shape[key]
                    right = recomp_by_shape[key]
                    if len(left) == len(right):
                        for orig_row, recomp_row in zip(left, right):
                            add_candidate(
                                candidates, orig_row, recomp_row, entity_name
                            )

                pair_ordered_calls(
                    candidates, orig, recomp, entity_name + ":group-calls"
                )
            pair_ordered_calls(
                candidates, hunk_orig, hunk_recomp, entity_name + ":hunk-calls"
            )
        pair_ordered_calls(
            candidates, entity_orig, entity_recomp, entity_name + ":entity-calls"
        )
    return candidates


def read_known(csv_path):
    with csv_path.open(newline="", encoding="utf-8-sig") as csv_file:
        return {
            int(row["address"], 16): row.get("name", "")
            for row in csv.DictReader(csv_file)
        }


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("report", type=Path)
    parser.add_argument("annotations", type=Path)
    args = parser.parse_args()

    report = json.loads(args.report.read_text(encoding="utf-8"))
    known = read_known(args.annotations)
    candidates = collect_candidates(report)
    known_addresses_by_name = defaultdict(set)
    for address, name in known.items():
        known_addresses_by_name[name].add(address)

    addresses_by_name = defaultdict(set)
    names_by_address = defaultdict(set)
    for address, name, _ in candidates:
        addresses_by_name[name].add(address)
        names_by_address[address].add(name)

    rows = []
    for (address, name, kind), sources in candidates.items():
        if address in known:
            continue
        if known_addresses_by_name[name] and address not in known_addresses_by_name[name]:
            continue
        if len(addresses_by_name[name]) != 1 or len(names_by_address[address]) != 1:
            continue
        rows.append((len(sources), address, name, kind, len(set(sources))))

    rows.sort(key=lambda row: (-row[0], row[1], row[2]))
    writer = csv.writer(__import__("sys").stdout, lineterminator="\n")
    writer.writerow(("votes", "address", "name", "type", "contexts"))
    for votes, address, name, kind, contexts in rows:
        writer.writerow(
            (
                votes,
                f"{address:08x}",
                name,
                "global" if kind == "DATA" else "function",
                contexts,
            )
        )


if __name__ == "__main__":
    main()
