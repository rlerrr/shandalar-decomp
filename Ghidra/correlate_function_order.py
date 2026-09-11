#!/usr/bin/env python3
"""Correlate functions in two related executables by their relative order.

Known names in the reference reccmp report and target annotation CSV are used as
anchors.  A longest-increasing subsequence discards anchors that were moved.  If
the two Ghidra function lists contain the same number of starts between adjacent
anchors, the intervening starts are paired by ordinal position.
"""

import argparse
import bisect
import csv
import json
import sys
from pathlib import Path


PLACEHOLDER_PREFIXES = ("fun_", "sub_", "thunk_", "rfalse")


def read_csv(path):
    with path.open(newline="", encoding="utf-8-sig") as csv_file:
        return list(csv.DictReader(csv_file))


def address(value):
    return int(value, 16)


def increasing_anchors(anchors):
    """Return one longest subsequence increasing in target address."""
    tails = []
    tail_indices = []
    previous = [-1] * len(anchors)

    for index, anchor in enumerate(anchors):
        position = bisect.bisect_left(tails, anchor[1])
        if position == len(tails):
            tails.append(anchor[1])
            tail_indices.append(index)
        else:
            tails[position] = anchor[1]
            tail_indices[position] = index
        if position:
            previous[index] = tail_indices[position - 1]

    if not tail_indices:
        return []
    index = tail_indices[-1]
    result = []
    while index >= 0:
        result.append(anchors[index])
        index = previous[index]
    result.reverse()
    return result


def between(values, lower, upper):
    return values[
        bisect.bisect_right(values, lower) : bisect.bisect_left(values, upper)
    ]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("reference_functions", type=Path)
    parser.add_argument("reference_report", type=Path)
    parser.add_argument("target_functions", type=Path)
    parser.add_argument("target_annotations", type=Path)
    args = parser.parse_args()

    reference_rows = read_csv(args.reference_functions)
    target_rows = read_csv(args.target_functions)
    annotation_rows = read_csv(args.target_annotations)
    report = json.loads(args.reference_report.read_text(encoding="utf-8"))

    reference_starts = sorted(address(row["address"]) for row in reference_rows)
    target_starts = sorted(address(row["address"]) for row in target_rows)
    target_by_address = {
        address(row["address"]): row for row in target_rows
    }
    reference_names = {
        address(row["address"]): row["name"]
        for row in report["data"]
        if row.get("name") and row.get("type") == 1
    }
    annotated_functions = [
        row for row in annotation_rows if row["type"] == "function"
    ]
    annotated_by_name = {
        row["name"]: address(row["address"]) for row in annotated_functions
    }
    annotated_addresses = {
        address(row["address"]) for row in annotated_functions
    }

    anchors = sorted(
        (reference_address, annotated_by_name[name], name)
        for reference_address, name in reference_names.items()
        if name in annotated_by_name
    )
    anchors = increasing_anchors(anchors)

    equal_gaps = 0
    candidates = []
    for lower, upper in zip(anchors, anchors[1:]):
        reference_gap = between(reference_starts, lower[0], upper[0])
        target_gap = between(target_starts, lower[1], upper[1])
        if len(reference_gap) != len(target_gap):
            continue
        equal_gaps += 1
        for reference_address, target_address in zip(reference_gap, target_gap):
            name = reference_names.get(reference_address)
            if not name or target_address in annotated_addresses:
                continue
            if name in annotated_by_name:
                continue
            target_row = target_by_address[target_address]
            curated_name = target_row["name"]
            candidates.append(
                {
                    "address": "%08x" % target_address,
                    "name": name,
                    "type": "function",
                    "orig_size": target_row["orig_size"],
                    "reference_address": "%08x" % reference_address,
                    "curated_name": curated_name,
                    "curated_placeholder": str(
                        curated_name.lower().startswith(PLACEHOLDER_PREFIXES)
                    ).lower(),
                    "lower_anchor": lower[2],
                    "upper_anchor": upper[2],
                }
            )

    # Measure the method without trusting it: hide each interior anchor in turn
    # and see whether its two neighbors predict its target position.
    tested = 0
    correct = 0
    for index in range(1, len(anchors) - 1):
        lower = anchors[index - 1]
        hidden = anchors[index]
        upper = anchors[index + 1]
        reference_gap = between(reference_starts, lower[0], upper[0])
        target_gap = between(target_starts, lower[1], upper[1])
        if len(reference_gap) != len(target_gap) or hidden[0] not in reference_gap:
            continue
        tested += 1
        predicted = target_gap[reference_gap.index(hidden[0])]
        if predicted == hidden[1]:
            correct += 1

    total_gaps = max(0, len(anchors) - 1)
    precision = correct / tested if tested else 0.0
    print(
        "anchors=%d equal_gaps=%d/%d validation=%d/%d (%.2f%%) candidates=%d"
        % (
            len(anchors),
            equal_gaps,
            total_gaps,
            correct,
            tested,
            precision * 100,
            len(candidates),
        ),
        file=sys.stderr,
    )

    fieldnames = (
        "address",
        "name",
        "type",
        "orig_size",
        "reference_address",
        "curated_name",
        "curated_placeholder",
        "lower_anchor",
        "upper_anchor",
    )
    writer = csv.DictWriter(sys.stdout, fieldnames=fieldnames, lineterminator="\n")
    writer.writeheader()
    writer.writerows(candidates)


if __name__ == "__main__":
    main()
