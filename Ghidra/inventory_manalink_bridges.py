#!/usr/bin/env python3
"""Inventory MOK functions redirected into ManalinkEh and EXE_* literals."""

import argparse
import csv
import json
import math
import re
from collections import Counter, defaultdict
from pathlib import Path

from capstone import CS_ARCH_X86, CS_MODE_32, Cs
from reccmp.formats import detect_image


VENEER_RE = re.compile(
    r"^\s*jmp\s+near\s+_(\S+)\s*;\s*(?:0x)?(0?2[0-9a-f]{6})\b",
    re.IGNORECASE,
)
MACRO_PATTERNS = (
    re.compile(r"\b(EXE_(?:FN|STDCALL_FN))\s*\(\s*[^,]+,\s*(0x[0-9a-f]+)", re.I),
    re.compile(r"\b(EXE_(?:TYP|TYP_PTR))\s*\(\s*[^,]+,\s*(0x[0-9a-f]+)", re.I),
    re.compile(r"\b(EXE_2D_ARR)\s*\(\s*[^,]+,\s*[^,]+,\s*[^,]+,\s*(0x[0-9a-f]+)", re.I),
    re.compile(r"\b(EXE_(?:PTR_VOID|VOID_PTR|DWORD_PTR|DWORD|WORD_PTR|WORD|BYTE_PTR|BYTE|STR))\s*\(\s*(0x[0-9a-f]+)", re.I),
)


def read_csv(path):
    with path.open(newline="", encoding="utf-8-sig") as csv_file:
        return list(csv.DictReader(csv_file))


def read_veneers(path):
    veneers = {}
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        match = VENEER_RE.match(line)
        if match:
            veneers[int(match.group(2), 16)] = match.group(1)
    return veneers


def first_direct_jump(image, disassembler, address):
    try:
        instructions = disassembler.disasm(bytes(image.read(address, 16)), address, 1)
        instruction = next(instructions, None)
    except Exception:
        return None
    if instruction is None or instruction.mnemonic != "jmp":
        return None
    try:
        return int(instruction.op_str, 16)
    except ValueError:
        return None


def rank_correlation(pairs):
    if len(pairs) < 2:
        return None
    ordered = sorted(pairs)
    dll_order = {address: rank for rank, address in enumerate(sorted({b for _, b in pairs}))}
    values = [dll_order[dll] for _, dll in ordered]
    mean_x = (len(values) - 1) / 2
    mean_y = sum(values) / len(values)
    numerator = sum((x - mean_x) * (y - mean_y) for x, y in enumerate(values))
    denominator = math.sqrt(
        sum((x - mean_x) ** 2 for x in range(len(values)))
        * sum((y - mean_y) ** 2 for y in values)
    )
    return numerator / denominator if denominator else None


def read_exe_references(source_dir):
    references = []
    for path in source_dir.rglob("*"):
        if path.suffix.lower() not in (".c", ".h", ".cpp"):
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        seen = set()
        for pattern in MACRO_PATTERNS:
            for match in pattern.finditer(text):
                key = (match.start(), match.group(1), match.group(2))
                if key in seen:
                    continue
                seen.add(key)
                references.append(
                    {
                        "macro": match.group(1).upper(),
                        "address": int(match.group(2), 16),
                        "file": str(path.relative_to(source_dir)),
                        "line": text.count("\n", 0, match.start()) + 1,
                    }
                )
    return references


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("magic_exe", type=Path)
    parser.add_argument("functions_csv", type=Path)
    parser.add_argument("annotations_csv", type=Path)
    parser.add_argument("manalink_asm", type=Path)
    parser.add_argument("source_dir", type=Path)
    parser.add_argument("--json", type=Path)
    args = parser.parse_args()

    image = detect_image(args.magic_exe)
    disassembler = Cs(CS_ARCH_X86, CS_MODE_32)
    veneers = read_veneers(args.manalink_asm)
    functions = read_csv(args.functions_csv)
    annotations = read_csv(args.annotations_csv)
    annotated_by_address = {
        int(row["address"], 16): row["name"]
        for row in annotations
        if row["type"].lower() == "function"
    }
    globals_by_address = {
        int(row["address"], 16): row["name"]
        for row in annotations
        if row["type"].lower() == "global"
    }
    all_annotations_by_address = dict(globals_by_address)
    all_annotations_by_address.update(annotated_by_address)

    redirects = []
    for row in functions:
        address = int(row["address"], 16)
        target = first_direct_jump(image, disassembler, address)
        if target not in veneers:
            continue
        redirects.append(
            {
                "exe_address": address,
                "exe_name": annotated_by_address.get(address, row["name"]),
                "veneer_address": target,
                "veneer_name": veneers[target],
            }
        )

    exe_references = read_exe_references(args.source_dir)
    refs_by_address = defaultdict(list)
    for reference in exe_references:
        refs_by_address[reference["address"]].append(reference)
    exe_addresses = []
    for address, references in sorted(refs_by_address.items()):
        kinds = sorted({reference["macro"] for reference in references})
        exe_addresses.append(
            {
                "address": address,
                "name": all_annotations_by_address.get(address),
                "references": len(references),
                "macros": kinds,
                "examples": references[:3],
            }
        )

    correlation = rank_correlation(
        [(row["exe_address"], row["veneer_address"]) for row in redirects]
    )
    result = {
        "veneer_count": len(veneers),
        "redirect_count": len(redirects),
        "redirect_name_matches": sum(
            row["exe_name"].lstrip("_") == row["veneer_name"].lstrip("_")
            for row in redirects
        ),
        "redirect_order_correlation": correlation,
        "exe_macro_reference_count": len(exe_references),
        "exe_macro_address_count": len(exe_addresses),
        "exe_macro_annotated_count": sum(row["name"] is not None for row in exe_addresses),
        "exe_macro_kinds": Counter(row["macro"] for row in exe_references),
        "redirects": redirects,
        "exe_addresses": exe_addresses,
    }

    if args.json:
        args.json.write_text(json.dumps(result, indent=2), encoding="utf-8")

    print(f"Veneers: {result['veneer_count']}")
    print(f"Redirected MOK functions: {result['redirect_count']}")
    print(f"Exact redirect name matches: {result['redirect_name_matches']}")
    print(f"EXE_* references: {result['exe_macro_reference_count']}")
    print(f"Unique EXE_* addresses: {result['exe_macro_address_count']}")
    print(f"Already annotated EXE_* addresses: {result['exe_macro_annotated_count']}")
    if correlation is not None:
        print(f"EXE/DLL redirect order correlation: {correlation:.3f}")
    for row in redirects:
        print(
            f"{row['exe_address']:08x},{row['exe_name']},"
            f"{row['veneer_address']:08x},{row['veneer_name']}"
        )


if __name__ == "__main__":
    main()
