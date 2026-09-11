#!/usr/bin/env python3
"""Correlate functions in related executables using anchored call graphs."""

import argparse
import bisect
import csv
import json
import sys
from collections import defaultdict
from pathlib import Path

from capstone import CS_ARCH_X86, CS_MODE_32, Cs
from reccmp.formats import detect_image


def read_csv(path):
    with path.open(newline="", encoding="utf-8-sig") as csv_file:
        return list(csv.DictReader(csv_file))


def address(value):
    return int(value, 16)


def increasing_anchors(anchors):
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


def extract_graph(binary_path, function_rows):
    image = detect_image(filepath=binary_path)
    disassembler = Cs(CS_ARCH_X86, CS_MODE_32)
    starts = {address(row["address"]) for row in function_rows}
    outgoing = defaultdict(set)
    incoming = defaultdict(set)

    for row in function_rows:
        source = address(row["address"])
        try:
            code = bytes(image.read(source, int(row["orig_size"])))
        except Exception:
            continue
        for instruction in disassembler.disasm(code, source):
            if instruction.mnemonic not in ("call", "jmp"):
                continue
            try:
                target = int(instruction.op_str, 16)
            except ValueError:
                continue
            if target not in starts or target == source:
                continue
            outgoing[source].add(target)
            incoming[target].add(source)
    return outgoing, incoming


def signature(node, outgoing, incoming, anchor_names):
    return (
        frozenset(
            anchor_names[target]
            for target in outgoing[node]
            if target in anchor_names
        ),
        frozenset(
            anchor_names[source]
            for source in incoming[node]
            if source in anchor_names
        ),
    )


def nodes_between(nodes, lower, upper):
    return nodes[bisect.bisect_right(nodes, lower) : bisect.bisect_left(nodes, upper)]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("reference_binary", type=Path)
    parser.add_argument("reference_functions", type=Path)
    parser.add_argument("reference_report", type=Path)
    parser.add_argument("target_binary", type=Path)
    parser.add_argument("target_functions", type=Path)
    parser.add_argument("target_annotations", type=Path)
    parser.add_argument("--min-neighbors", type=int, default=1)
    args = parser.parse_args()

    reference_rows = read_csv(args.reference_functions)
    target_rows = read_csv(args.target_functions)
    annotation_rows = read_csv(args.target_annotations)
    report = json.loads(args.reference_report.read_text(encoding="utf-8"))

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
    annotated_names = set(annotated_by_name)

    anchors = sorted(
        (reference_address, annotated_by_name[name], name)
        for reference_address, name in reference_names.items()
        if name in annotated_by_name
    )
    anchors = increasing_anchors(anchors)
    reference_anchor_names = {row[0]: row[2] for row in anchors}
    target_anchor_names = {row[1]: row[2] for row in anchors}

    reference_outgoing, reference_incoming = extract_graph(
        args.reference_binary, reference_rows
    )
    target_outgoing, target_incoming = extract_graph(args.target_binary, target_rows)
    reference_nodes = sorted(address(row["address"]) for row in reference_rows)
    target_nodes = sorted(address(row["address"]) for row in target_rows)
    target_by_address = {
        address(row["address"]): row for row in target_rows
    }

    candidates = []
    for lower, upper in zip(anchors, anchors[1:]):
        reference_gap = nodes_between(reference_nodes, lower[0], upper[0])
        target_gap = nodes_between(target_nodes, lower[1], upper[1])
        reference_by_signature = defaultdict(list)
        target_by_signature = defaultdict(list)

        for node in reference_gap:
            node_signature = signature(
                node,
                reference_outgoing,
                reference_incoming,
                reference_anchor_names,
            )
            if sum(len(part) for part in node_signature) >= args.min_neighbors:
                reference_by_signature[node_signature].append(node)
        for node in target_gap:
            if node in annotated_addresses:
                continue
            node_signature = signature(
                node, target_outgoing, target_incoming, target_anchor_names
            )
            if sum(len(part) for part in node_signature) >= args.min_neighbors:
                target_by_signature[node_signature].append(node)

        for node_signature, reference_matches in reference_by_signature.items():
            named_matches = [
                node
                for node in reference_matches
                if node in reference_names
                and reference_names[node] not in annotated_names
            ]
            target_matches = target_by_signature.get(node_signature, ())
            if (
                len(reference_matches) != 1
                or len(named_matches) != 1
                or len(target_matches) != 1
            ):
                continue
            reference_address = named_matches[0]
            target_address = target_matches[0]
            target_row = target_by_address[target_address]
            candidates.append(
                {
                    "address": "%08x" % target_address,
                    "name": reference_names[reference_address],
                    "type": "function",
                    "orig_size": target_row["orig_size"],
                    "reference_address": "%08x" % reference_address,
                    "curated_name": target_row["name"],
                    "outgoing_anchors": ";".join(sorted(node_signature[0])),
                    "incoming_anchors": ";".join(sorted(node_signature[1])),
                    "lower_anchor": lower[2],
                    "upper_anchor": upper[2],
                }
            )

    # Hide each interior anchor conceptually and ask whether its call-graph
    # signature uniquely recovers its known target between the surrounding pair.
    tested = 0
    correct = 0
    for index in range(1, len(anchors) - 1):
        lower = anchors[index - 1]
        hidden = anchors[index]
        upper = anchors[index + 1]
        hidden_signature = signature(
            hidden[0],
            reference_outgoing,
            reference_incoming,
            reference_anchor_names,
        )
        if sum(len(part) for part in hidden_signature) < args.min_neighbors:
            continue
        matches = []
        for node in nodes_between(target_nodes, lower[1], upper[1]):
            if (
                signature(node, target_outgoing, target_incoming, target_anchor_names)
                == hidden_signature
            ):
                matches.append(node)
        if len(matches) == 1:
            tested += 1
            if matches[0] == hidden[1]:
                correct += 1

    precision = correct / tested if tested else 0.0
    print(
        "anchors=%d validation=%d/%d (%.2f%%) candidates=%d"
        % (len(anchors), correct, tested, precision * 100, len(candidates)),
        file=sys.stderr,
    )

    fieldnames = (
        "address",
        "name",
        "type",
        "orig_size",
        "reference_address",
        "curated_name",
        "outgoing_anchors",
        "incoming_anchors",
        "lower_anchor",
        "upper_anchor",
    )
    writer = csv.DictWriter(sys.stdout, fieldnames=fieldnames, lineterminator="\n")
    writer.writeheader()
    writer.writerows(candidates)


if __name__ == "__main__":
    main()
