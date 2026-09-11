#!/usr/bin/env python3
"""Inventory the MOK symbols required by ManalinkEh and its C source."""

import argparse
import bisect
import csv
import json
import math
import re
import struct
from collections import Counter, defaultdict
from pathlib import Path

from reccmp.compare import Compare
from reccmp.formats import detect_image
from reccmp.formats.image import ImageSectionFlags
from reccmp.project.detect import RecCmpProject
from reccmp.types import ImageId


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
HEX_RE = re.compile(r"\b0x([0-9a-f]+)\b", re.IGNORECASE)
DEFINE_RE = re.compile(r"^\s*#\s*define\s+([A-Za-z_$][\w$]*)\b")


def read_csv(path):
    with path.open(newline="", encoding="utf-8-sig") as csv_file:
        return list(csv.DictReader(csv_file))


def parse_optional_int(value):
    if value is None or not value.strip():
        return None
    return int(value, 0)


def read_recompiled_global_sizes(project_dir, target_id):
    """Read type sizes for matched globals from the recompilation PDB."""
    project = RecCmpProject.from_directory(project_dir)
    compare = Compare.from_target(project.get(target_id))
    sizes = {}
    for variable in compare.get_variables():
        size = variable.size(ImageId.RECOMP)
        if size is not None and size > 0:
            sizes[variable.orig_addr] = {
                "size": size,
                "name": variable.best_name(),
                "recomp_address": variable.recomp_addr,
            }
    return sizes


def read_veneers(path):
    veneers = {}
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        match = VENEER_RE.match(line)
        if match:
            veneers[int(match.group(2), 16)] = match.group(1)
    return veneers


def scan_veneer_jumps(image, veneers):
    """Find every raw E9 rel32 in executable bytes that lands on a veneer."""
    jumps = []
    for section in image.sections:
        if not section.flags & ImageSectionFlags.EXECUTE:
            continue
        data = bytes(section.view)
        base = section.virtual_range.start
        for offset in range(len(data) - 4):
            if data[offset] != 0xE9:
                continue
            site = base + offset
            displacement = struct.unpack_from("<i", data, offset + 1)[0]
            target = site + 5 + displacement
            if target in veneers:
                jumps.append((site, target))
    return jumps


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


def mask_comments_and_strings(text):
    """Blank comments and quoted strings while preserving offsets and lines."""
    chars = list(text)
    index = 0
    state = "code"
    while index < len(chars):
        char = chars[index]
        following = chars[index + 1] if index + 1 < len(chars) else ""
        if state == "code":
            if char == "/" and following == "/":
                chars[index] = chars[index + 1] = " "
                index += 2
                state = "line_comment"
                continue
            if char == "/" and following == "*":
                chars[index] = chars[index + 1] = " "
                index += 2
                state = "block_comment"
                continue
            if char == '"':
                chars[index] = " "
                state = "string"
            elif char == "'":
                chars[index] = " "
                state = "character"
        elif state == "line_comment":
            if char == "\n":
                state = "code"
            else:
                chars[index] = " "
        elif state == "block_comment":
            if char == "*" and following == "/":
                chars[index] = chars[index + 1] = " "
                index += 2
                state = "code"
                continue
            if char != "\n":
                chars[index] = " "
        else:
            if char == "\\":
                chars[index] = " "
                if index + 1 < len(chars) and chars[index + 1] != "\n":
                    chars[index + 1] = " "
                    index += 2
                    continue
            elif (state == "string" and char == '"') or (
                state == "character" and char == "'"
            ):
                chars[index] = " "
                state = "code"
            elif char != "\n":
                chars[index] = " "
        index += 1
    return "".join(chars)


def inferred_naked_name(line, column):
    define = DEFINE_RE.match(line)
    if define:
        return define.group(1)
    prefix = line[:column]
    assignment = re.search(
        r"\b([A-Za-z_$][\w$]*)\s*=\s*(?:\([^;=]*\)\s*)?$", prefix
    )
    return assignment.group(1) if assignment else None


def inferred_reference_name(code_line, original_line, column):
    name = inferred_naked_name(code_line, column)
    if name:
        return name
    trailing_comment = re.search(
        r"//\s*([A-Za-z_$][\w$]*)\s*(?:\(\))?", original_line[column:]
    )
    return trailing_comment.group(1) if trailing_comment else None


def source_profile(path, source_lines, line_index):
    """Recognize the small DrawCardLib branch for Shandalar.exe addresses."""
    nearby = "\n".join(source_lines[max(0, line_index - 8) : line_index + 2])
    if path.name.lower() == "drawcardlib.c" and "PARENT_SHANDALAR" in nearby:
        return "alternate_shandalar_exe"
    return "mok"


def read_source_references(source_dir, image):
    macro_references = []
    naked_references = []
    for path in source_dir.rglob("*"):
        if path.suffix.lower() not in (".c", ".h", ".cpp"):
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        masked = mask_comments_and_strings(text)
        source_lines = text.splitlines()
        line_starts = [0]
        line_starts.extend(match.end() for match in re.finditer("\n", masked))
        covered = []
        for pattern in MACRO_PATTERNS:
            for match in pattern.finditer(masked):
                literal_address = int(match.group(2), 16)
                if not image.is_valid_vaddr(literal_address):
                    continue
                line_index = bisect.bisect_right(line_starts, match.start()) - 1
                line_start = line_starts[line_index]
                line_end = masked.find("\n", match.end())
                if line_end < 0:
                    line_end = len(masked)
                code_line = masked[line_start:line_end]
                original_line = text[line_start:line_end]
                column = match.start() - line_start
                key = (match.start(), match.group(1), match.group(2))
                covered.append(match.span())
                macro_references.append(
                    {
                        "macro": match.group(1).upper(),
                        "address": literal_address,
                        "file": str(path.relative_to(source_dir)),
                        "line": text.count("\n", 0, match.start()) + 1,
                        "context": original_line.strip()[:300],
                        "inferred_name": inferred_reference_name(
                            code_line, original_line, column
                        ),
                        "source_profile": source_profile(
                            path,
                            source_lines,
                            text.count("\n", 0, match.start()),
                        ),
                        "key": key,
                    }
                )
        for match in HEX_RE.finditer(masked):
            if any(lower <= match.start() < upper for lower, upper in covered):
                continue
            literal_address = int(match.group(1), 16)
            if not image.is_valid_vaddr(literal_address):
                continue
            line_index = bisect.bisect_right(line_starts, match.start()) - 1
            line_start = line_starts[line_index]
            line_end = masked.find("\n", match.end())
            if line_end < 0:
                line_end = len(masked)
            code_line = masked[line_start:line_end]
            original_line = text[line_start:line_end].strip()
            column = match.start() - line_start
            prefix = code_line[:column]
            pointer_cast = bool(re.search(r"\([^()]*(?:\*|FAR)[^()]*\)\s*$", prefix))
            naked_references.append(
                {
                    "address": literal_address,
                    "file": str(path.relative_to(source_dir)),
                    "line": line_index + 1,
                    "classification": "pointer_cast" if pointer_cast else "literal",
                    "source_profile": source_profile(path, source_lines, line_index),
                    "likely_pointer": not (
                        literal_address > 0
                        and literal_address & (literal_address - 1) == 0
                    ),
                    "inferred_name": inferred_naked_name(code_line, column),
                    "context": original_line[:300],
                }
            )

    # Some pattern alternatives overlap.  Collapse identical macro occurrences.
    deduplicated = {}
    for reference in macro_references:
        key = (reference["file"], reference.pop("key"))
        deduplicated[key] = reference
    return list(deduplicated.values()), naked_references


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("magic_exe", type=Path)
    parser.add_argument("functions_csv", type=Path)
    parser.add_argument("annotations_csv", type=Path)
    parser.add_argument("manalink_asm", type=Path)
    parser.add_argument("source_dir", type=Path)
    parser.add_argument("--project-dir", type=Path)
    parser.add_argument("--target")
    parser.add_argument("--json", type=Path)
    args = parser.parse_args()

    if bool(args.project_dir) != bool(args.target):
        parser.error("--project-dir and --target must be used together")

    image = detect_image(args.magic_exe)
    veneers = read_veneers(args.manalink_asm)
    functions = read_csv(args.functions_csv)
    annotations = read_csv(args.annotations_csv)
    recomp_global_sizes = {}
    if args.project_dir:
        recomp_global_sizes = read_recompiled_global_sizes(
            args.project_dir, args.target
        )
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
    annotations_by_address = {
        int(row["address"], 16): row for row in annotations
    }
    all_annotations_by_address = {
        address: row["name"] for address, row in annotations_by_address.items()
    }

    global_spans = []
    candidate_global_spans = []
    for row in annotations:
        if row["type"].lower() != "global":
            continue
        address = int(row["address"], 16)
        explicit_size = parse_optional_int(row.get("size"))
        inferred = recomp_global_sizes.get(address)
        if explicit_size is not None and explicit_size > 0:
            global_spans.append(
                {
                    "address": address,
                    "end_address": address + explicit_size,
                    "size": explicit_size,
                    "size_source": "csv",
                    "name": row["name"],
                }
            )
        elif inferred is not None:
            candidate_global_spans.append(
                {
                    "address": address,
                    "end_address": address + inferred["size"],
                    "size": inferred["size"],
                    "size_source": "recomp_pdb_candidate",
                    "name": row["name"],
                }
            )

    function_starts = {
        int(row["address"], 16): row for row in functions
    }
    function_spans = sorted(
        (
            int(row["address"], 16),
            int(row["address"], 16) + int(row["orig_size"]),
            row,
        )
        for row in functions
    )
    span_starts = [row[0] for row in function_spans]

    redirects = []
    for address, target in scan_veneer_jumps(image, veneers):
        function_row = function_starts.get(address)
        span_index = bisect.bisect_right(span_starts, address) - 1
        containing_row = None
        if span_index >= 0 and address < function_spans[span_index][1]:
            containing_row = function_spans[span_index][2]
        suggested_name = veneers[target]
        if function_row is not None:
            suggested_name = function_row["name"]
        redirects.append(
            {
                "exe_address": address,
                "exe_name": annotated_by_address.get(address, suggested_name),
                "veneer_address": target,
                "veneer_name": veneers[target],
                "at_function_start": function_row is not None,
                "containing_function": (
                    containing_row["name"] if containing_row is not None else None
                ),
                "annotation": all_annotations_by_address.get(address),
            }
        )

    exe_references, naked_references = read_source_references(args.source_dir, image)
    refs_by_address = defaultdict(list)
    for reference in exe_references:
        refs_by_address[reference["address"]].append(reference)
    exe_addresses = []
    for address, references in sorted(refs_by_address.items()):
        kinds = sorted({reference["macro"] for reference in references})
        inferred_names = sorted(
            {
                reference["inferred_name"]
                for reference in references
                if reference["inferred_name"]
            }
        )
        exe_addresses.append(
            {
                "address": address,
                "name": all_annotations_by_address.get(address),
                "references": len(references),
                "macros": kinds,
                "inferred_names": inferred_names,
                "mok_references": sum(
                    reference["source_profile"] == "mok" for reference in references
                ),
                "examples": references[:3],
            }
        )

    naked_by_address = defaultdict(list)
    for reference in naked_references:
        naked_by_address[reference["address"]].append(reference)
    naked_addresses = []
    for address, references in sorted(naked_by_address.items()):
        inferred_names = sorted(
            {
                reference["inferred_name"]
                for reference in references
                if reference["inferred_name"]
            }
        )
        naked_addresses.append(
            {
                "address": address,
                "name": all_annotations_by_address.get(address),
                "references": len(references),
                "pointer_cast_references": sum(
                    reference["classification"] == "pointer_cast"
                    for reference in references
                ),
                "likely_pointer_references": sum(
                    reference["likely_pointer"]
                    and reference["source_profile"] == "mok"
                    for reference in references
                ),
                "inferred_names": inferred_names,
                "examples": references[:5],
            }
        )

    required = defaultdict(
        lambda: {
            "reasons": set(),
            "suggested_types": set(),
            "suggested_names": set(),
        }
    )
    for row in redirects:
        item = required[row["exe_address"]]
        item["reasons"].add("manalink_rel32_jump")
        item["suggested_types"].add("function")
        item["suggested_names"].add(row["veneer_name"])
    for row in exe_addresses:
        if not row["mok_references"]:
            continue
        item = required[row["address"]]
        item["reasons"].add("exe_macro")
        kinds = set(row["macros"])
        if kinds & {"EXE_FN", "EXE_STDCALL_FN"}:
            item["suggested_types"].add("function")
        if "EXE_STR" in kinds:
            item["suggested_types"].add("string")
        if kinds - {"EXE_FN", "EXE_STDCALL_FN", "EXE_STR"}:
            item["suggested_types"].add("global")
        for name in row["inferred_names"]:
            item["suggested_names"].add(name)
    for row in naked_addresses:
        if not row["likely_pointer_references"]:
            continue
        item = required[row["address"]]
        item["reasons"].add("naked_source_literal")
        if row["address"] in function_starts:
            item["suggested_types"].add("function")
        for name in row["inferred_names"]:
            item["suggested_names"].add(name)

    required_addresses = []
    for address, evidence in sorted(required.items()):
        annotation = annotations_by_address.get(address)
        requires_exact_function = "function" in evidence["suggested_types"]
        containers = sorted(
            (
                span
                for span in global_spans
                if span["address"] < address < span["end_address"]
            ),
            key=lambda span: (span["size"], span["address"]),
        )
        container = containers[0] if containers else None
        candidate_containers = sorted(
            (
                span
                for span in candidate_global_spans
                if span["address"] < address < span["end_address"]
            ),
            key=lambda span: (span["size"], span["address"]),
        )
        candidate_container = (
            candidate_containers[0] if candidate_containers else None
        )
        if annotation is not None:
            coverage = "exact"
        elif container is not None and not requires_exact_function:
            coverage = "contained"
        elif candidate_container is not None and not requires_exact_function:
            coverage = "candidate_containment"
            container = candidate_container
            containers = candidate_containers
        else:
            coverage = "missing"
        required_addresses.append(
            {
                "address": address,
                "annotation_name": annotation["name"] if annotation else None,
                "annotation_type": annotation["type"] if annotation else None,
                "coverage": coverage,
                "container": (
                    {
                        **container,
                        "offset": address - container["address"],
                        "candidate_count": len(containers),
                    }
                    if container is not None
                    else None
                ),
                "reasons": sorted(evidence["reasons"]),
                "suggested_types": sorted(evidence["suggested_types"]),
                "suggested_names": sorted(evidence["suggested_names"]),
            }
        )

    correlation = rank_correlation(
        [(row["exe_address"], row["veneer_address"]) for row in redirects]
    )
    result = {
        "veneer_count": len(veneers),
        "redirect_count": len(redirects),
        "redirect_at_function_start_count": sum(
            row["at_function_start"] for row in redirects
        ),
        "redirect_annotated_count": sum(
            row["annotation"] is not None for row in redirects
        ),
        "redirect_name_matches": sum(
            row["exe_name"].lstrip("_") == row["veneer_name"].lstrip("_")
            for row in redirects
        ),
        "redirect_order_correlation": correlation,
        "exe_macro_reference_count": len(exe_references),
        "exe_macro_address_count": len(exe_addresses),
        "exe_macro_annotated_count": sum(row["name"] is not None for row in exe_addresses),
        "exe_macro_kinds": Counter(row["macro"] for row in exe_references),
        "naked_literal_reference_count": len(naked_references),
        "naked_literal_address_count": len(naked_addresses),
        "naked_likely_pointer_address_count": sum(
            row["likely_pointer_references"] > 0 for row in naked_addresses
        ),
        "naked_literal_annotated_count": sum(
            row["name"] is not None for row in naked_addresses
        ),
        "required_address_count": len(required_addresses),
        "required_annotated_count": sum(
            row["coverage"] == "exact" for row in required_addresses
        ),
        "required_contained_count": sum(
            row["coverage"] == "contained" for row in required_addresses
        ),
        "required_candidate_containment_count": sum(
            row["coverage"] == "candidate_containment"
            for row in required_addresses
        ),
        "required_missing_count": sum(
            row["coverage"] == "missing" for row in required_addresses
        ),
        "sized_global_count": len(global_spans),
        "pdb_candidate_global_count": len(candidate_global_spans),
        "redirects": redirects,
        "exe_addresses": exe_addresses,
        "naked_addresses": naked_addresses,
        "required_addresses": required_addresses,
    }

    if args.json:
        args.json.write_text(json.dumps(result, indent=2), encoding="utf-8")

    print(f"Veneers: {result['veneer_count']}")
    print(f"Manalink rel32 jumps: {result['redirect_count']}")
    print(
        "At known function starts: "
        f"{result['redirect_at_function_start_count']}"
    )
    print(f"Annotated rel32 sites: {result['redirect_annotated_count']}")
    print(f"Exact redirect name matches: {result['redirect_name_matches']}")
    print(f"EXE_* references: {result['exe_macro_reference_count']}")
    print(f"Unique EXE_* addresses: {result['exe_macro_address_count']}")
    print(f"Already annotated EXE_* addresses: {result['exe_macro_annotated_count']}")
    print(f"Naked in-image literal references: {result['naked_literal_reference_count']}")
    print(f"Unique naked literal addresses: {result['naked_literal_address_count']}")
    print(
        "Likely MOK pointer addresses: "
        f"{result['naked_likely_pointer_address_count']}"
    )
    print(
        "Already annotated naked literal addresses: "
        f"{result['naked_literal_annotated_count']}"
    )
    print(
        f"Required address union: {result['required_address_count']} "
        f"({result['required_annotated_count']} exact, "
        f"{result['required_contained_count']} contained, "
        f"{result['required_candidate_containment_count']} PDB candidates, "
        f"{result['required_missing_count']} missing)"
    )
    print(
        f"Explicitly sized globals: {result['sized_global_count']}; "
        f"recomp-PDB extent candidates: {result['pdb_candidate_global_count']}"
    )
    if correlation is not None:
        print(f"EXE/DLL redirect order correlation: {correlation:.3f}")
    for row in redirects:
        print(
            f"{row['exe_address']:08x},{row['exe_name']},"
            f"{row['veneer_address']:08x},{row['veneer_name']}"
        )


if __name__ == "__main__":
    main()
