#!/usr/bin/env python3
"""Print compact SHANDALAR reccmp diffs from a full JSON report."""

import argparse
import json
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]


def normalize_address(value):
    text = str(value).strip()
    if text.lower().startswith("0x"):
        return "0x%x" % int(text, 16)
    return "0x%x" % int(text, 16)


def iter_changed_blocks(diff):
    for hunk in diff:
        header = hunk[0]
        for block in hunk[1]:
            orig = block.get("orig")
            recomp = block.get("recomp")
            if orig is not None or recomp is not None:
                yield header, orig or [], recomp or []


def print_function(item, context):
    print("%s %.2f%% %s" % (
        item["address"],
        item.get("matching", 0.0) * 100.0,
        item.get("name", ""),
    ))
    for header, orig, recomp in iter_changed_blocks(item.get("diff", [])):
        print("  %s" % header)
        if orig:
            print("  orig:")
            for inst in orig[:context]:
                print("    %s: %s" % (inst[0], inst[1]))
            if len(orig) > context:
                print("    ... %d more" % (len(orig) - context))
        if recomp:
            print("  recomp:")
            for inst in recomp[:context]:
                print("    %s: %s" % (inst[0], inst[1]))
            if len(recomp) > context:
                print("    ... %d more" % (len(recomp) - context))
    print()


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--json", default="temp/shandalar_reccmp_full.json")
    parser.add_argument("--address", action="append", default=[])
    parser.add_argument("--top", type=int, default=0)
    parser.add_argument("--min-match", type=float, default=0.0)
    parser.add_argument("--context", type=int, default=12)
    return parser.parse_args()


def main():
    args = parse_args()
    with (REPO_ROOT / args.json).open() as f:
        payload = json.load(f)
    items = payload["data"]
    if args.address:
        wanted = {normalize_address(address) for address in args.address}
        selected = [
            item for item in items
            if normalize_address(item["address"]) in wanted
        ]
    else:
        selected = [
            item for item in items
            if item.get("matching") is not None
            and item.get("matching") < 1.0
            and item.get("diff")
            and item.get("matching") * 100.0 >= args.min_match
        ]
        selected.sort(key=lambda item: item.get("matching", 0.0), reverse=True)
        if args.top:
            selected = selected[:args.top]
    for item in selected:
        print_function(item, args.context)


if __name__ == "__main__":
    main()
