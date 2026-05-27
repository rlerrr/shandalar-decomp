#!/usr/bin/env python3

import argparse
import json
import sys
from dataclasses import dataclass, asdict
from pathlib import Path
import re
from typing import Dict, List, Optional, Tuple


FILE_HEADER_RE = re.compile(r"^\s*([A-Za-z0-9_.-]+\.c)\s*$")
FUNCTION_RE = re.compile(
    r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s*\(\s*(0x[0-9A-Fa-f]+|unused)\s*\)"
)

ANNOTATION_RE = re.compile(
    r"^\s*//\s*FUNCTION:\s*([A-Za-z_][A-Za-z0-9_]*)\s+0x([0-9A-Fa-f]{6,8})\s*$"
)
ANY_FUNCTION_ANNOTATION_RE = re.compile(r"^\s*//\s*FUNCTION:\s*")
COMMENT_LINE_RE = re.compile(r"^\s*//")


@dataclass
class FunctionEntry:
    name: str
    address: Optional[int]


@dataclass
class OutputGroup:
    file: str
    functions: List[FunctionEntry]


@dataclass
class IgnoreGroup:
    functions: List[FunctionEntry]


@dataclass(frozen=True)
class SourceBlock:
    source_file: Path
    start_line: int   # inclusive
    end_line: int     # exclusive
    text: str
    project: str
    address: int


@dataclass
class ResolvedFunction:
    output_file: str
    requested_name: str
    requested_address: int
    block: SourceBlock


class ParseError(Exception):
    pass


class ExtractionError(Exception):
    pass


def normalize_hex_address(addr: int) -> int:
    return addr & 0xFFFFFFFF


def parse_movefuncs_text(text: str) -> dict:
    output_groups: List[OutputGroup] = []
    ignore_groups: List[IgnoreGroup] = []

    seen_output_files = set()
    current_group = None
    line_num = 0

    for raw_line in text.splitlines():
        line_num += 1
        line = raw_line.strip()

        if not line:
            continue

        if line.startswith("//"):
            continue

        if line == "IGNORE":
            current_group = IgnoreGroup(functions=[])
            ignore_groups.append(current_group)
            continue

        m_file = FILE_HEADER_RE.match(line)
        if m_file:
            filename = m_file.group(1)

            if filename in seen_output_files:
                raise ParseError(
                    f"Line {line_num}: duplicate output file name: {filename}"
                )

            seen_output_files.add(filename)
            current_group = OutputGroup(file=filename, functions=[])
            output_groups.append(current_group)
            continue

        m_func = FUNCTION_RE.match(line)
        if m_func:
            if current_group is None:
                raise ParseError(
                    f"Line {line_num}: function entry found before any file header or IGNORE block: {line}"
                )

            func_name = m_func.group(1)
            addr_text = m_func.group(2)

            if addr_text.lower() == "unused":
                address = None
            else:
                address = int(addr_text, 16)

            current_group.functions.append(
                FunctionEntry(name=func_name, address=address)
            )
            continue

        raise ParseError(f"Line {line_num}: unrecognized line: {line}")

    return {
        "outputs": [
            {
                "file": group.file,
                "functions": [asdict(fn) for fn in group.functions],
            }
            for group in output_groups
        ],
        "ignore": [
            {
                "functions": [asdict(fn) for fn in group.functions],
            }
            for group in ignore_groups
        ],
    }


def parse_movefuncs_file(path: Path) -> dict:
    return parse_movefuncs_text(path.read_text(encoding="utf-8"))


def count_braces_in_line(line: str) -> int:
    return line.count("{") - line.count("}")


def find_function_body_end(lines: List[str], start_idx: int) -> int:
    brace_depth = 0
    saw_open_brace = False
    i = start_idx

    while i < len(lines):
        line = lines[i]
        if "{" in line:
            saw_open_brace = True
        brace_depth += count_braces_in_line(line)

        if saw_open_brace and brace_depth == 0:
            return i + 1

        i += 1

    raise ExtractionError(
        f"Could not find end of function body starting near line {start_idx + 1}"
    )


def extract_blocks_from_c_file(path: Path) -> List[SourceBlock]:
    text = path.read_text(encoding="utf-8")
    lines = text.splitlines(keepends=True)

    blocks: List[SourceBlock] = []
    i = 0

    while i < len(lines):
        if not ANY_FUNCTION_ANNOTATION_RE.match(lines[i]):
            i += 1
            continue

        anno_start = i
        anno_end = i + 1
        while anno_end < len(lines) and ANNOTATION_RE.match(lines[anno_end]):
            anno_end += 1

        annotations: List[Tuple[str, int]] = []
        for j in range(anno_start, anno_end):
            m = ANNOTATION_RE.match(lines[j])
            if not m:
                raise ExtractionError(
                    f"{path}: malformed FUNCTION annotation at line {j + 1}"
                )
            project = m.group(1)
            address = normalize_hex_address(int(m.group(2), 16))
            annotations.append((project, address))

        sig_start = anno_end
        while sig_start < len(lines):
            stripped = lines[sig_start].strip()
            if not stripped:
                sig_start += 1
                continue
            if COMMENT_LINE_RE.match(lines[sig_start]):
                sig_start += 1
                continue
            break

        if sig_start >= len(lines):
            raise ExtractionError(
                f"{path}: annotation block at line {anno_start + 1} is not followed by a function"
            )

        func_end = find_function_body_end(lines, sig_start)

        block_end = func_end
        while block_end < len(lines):
            if ANY_FUNCTION_ANNOTATION_RE.match(lines[block_end]):
                break
            block_end += 1

        block_text = "".join(lines[anno_start:block_end])

        for project, address in annotations:
            blocks.append(
                SourceBlock(
                    source_file=path,
                    start_line=anno_start,
                    end_line=block_end,
                    text=block_text,
                    project=project,
                    address=address,
                )
            )

        i = block_end

    return blocks


def build_block_index(srcdir: Path) -> Dict[Tuple[str, int], List[SourceBlock]]:
    index: Dict[Tuple[str, int], List[SourceBlock]] = {}

    for path in sorted(srcdir.rglob("*.c")):
        for block in extract_blocks_from_c_file(path):
            key = (block.project.upper(), normalize_hex_address(block.address))
            index.setdefault(key, []).append(block)

    return index


def resolve_move_plan(
    move_plan: dict,
    srcdir: Path,
    project: str,
) -> Dict[str, List[ResolvedFunction]]:
    block_index = build_block_index(srcdir)
    project_key = project.upper()

    resolved_outputs: Dict[str, List[ResolvedFunction]] = {}
    used_block_keys: Dict[Tuple[Path, int, int], str] = {}

    for output in move_plan["outputs"]:
        out_file = output["file"]
        resolved_outputs[out_file] = []

        for fn in output["functions"]:
            name = fn["name"]
            address = fn["address"]

            if address is None:
                raise ExtractionError(
                    f"{out_file}: function {name} has address 'unused'; cannot extract from source"
                )

            norm_addr = normalize_hex_address(address)
            key = (project_key, norm_addr)
            matches = block_index.get(key, [])

            if not matches:
                raise ExtractionError(
                    f"{out_file}: could not find // FUNCTION: {project} 0x{norm_addr:08x} for {name}"
                )

            if len(matches) > 1:
                locations = ", ".join(
                    f"{m.source_file}:{m.start_line + 1}" for m in matches
                )
                raise ExtractionError(
                    f"{out_file}: multiple matches for // FUNCTION: {project} 0x{norm_addr:08x} "
                    f"for {name}: {locations}"
                )

            block = matches[0]
            block_key = (block.source_file, block.start_line, block.end_line)
            prior_use = used_block_keys.get(block_key)
            if prior_use is not None:
                raise ExtractionError(
                    f"{out_file}: source block {block.source_file}:{block.start_line + 1}-{block.end_line} "
                    f"for {name} is already assigned to {prior_use}"
                )
            used_block_keys[block_key] = f"{out_file}:{name}"

            resolved_outputs[out_file].append(
                ResolvedFunction(
                    output_file=out_file,
                    requested_name=name,
                    requested_address=norm_addr,
                    block=block,
                )
            )

    return resolved_outputs


def write_target_files(
    resolved_outputs: Dict[str, List[ResolvedFunction]],
    targetdir: Path,
) -> None:
    targetdir.mkdir(parents=True, exist_ok=True)

    for out_file, resolved_functions in resolved_outputs.items():
        out_path = targetdir / out_file
        text = "".join(rf.block.text for rf in resolved_functions)
        out_path.write_text(text, encoding="utf-8")


def remove_blocks_from_sources(resolved_outputs: Dict[str, List[ResolvedFunction]]) -> None:
    removals_by_file: Dict[Path, List[Tuple[int, int]]] = {}

    for resolved_functions in resolved_outputs.values():
        for rf in resolved_functions:
            removals_by_file.setdefault(rf.block.source_file, []).append(
                (rf.block.start_line, rf.block.end_line)
            )

    for source_file, spans in removals_by_file.items():
        unique_spans = sorted(set(spans))

        for i in range(1, len(unique_spans)):
            prev_start, prev_end = unique_spans[i - 1]
            cur_start, cur_end = unique_spans[i]
            if cur_start < prev_end:
                raise ExtractionError(
                    f"Overlapping removals in {source_file}: "
                    f"{prev_start + 1}-{prev_end} overlaps {cur_start + 1}-{cur_end}"
                )

        lines = source_file.read_text(encoding="utf-8").splitlines(keepends=True)

        for start, end in sorted(unique_spans, reverse=True):
            del lines[start:end]

        source_file.write_text("".join(lines), encoding="utf-8")


def main(argv: List[str]) -> int:
    parser = argparse.ArgumentParser(
        description="Parse movefuncs.txt and emit new C files from RECCMP-style annotated sources."
    )
    parser.add_argument("movefile", help="Path to movefuncs.txt")
    parser.add_argument("--srcdir", required=True, help="Directory to scan for source .c files")
    parser.add_argument("--targetdir", required=True, help="Directory to write emitted .c files")
    parser.add_argument("--project", required=True, help="RECCMP project name, e.g. MAGIC or SHANDALAR")
    parser.add_argument("--dump-plan-json", help="Optional path to dump parsed move plan as JSON")
    parser.add_argument(
        "--move",
        action="store_true",
        help="Also remove the extracted blocks from the original source files",
    )

    args = parser.parse_args(argv[1:])

    movefile = Path(args.movefile)
    srcdir = Path(args.srcdir)
    targetdir = Path(args.targetdir)
    project = args.project

    try:
        move_plan = parse_movefuncs_file(movefile)

        if args.dump_plan_json:
            Path(args.dump_plan_json).write_text(
                json.dumps(move_plan, indent=2),
                encoding="utf-8",
            )

        resolved_outputs = resolve_move_plan(move_plan, srcdir, project)

        write_target_files(resolved_outputs, targetdir)

        if args.move:
            remove_blocks_from_sources(resolved_outputs)

    except (ParseError, ExtractionError) as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))