#!/usr/bin/env python3
import argparse
import json
import re
from collections import Counter
from dataclasses import dataclass
from pathlib import Path


SOURCE_EXTENSIONS = {".c"}
COMMENT_RE = re.compile(r"^(\s*)//\s+(FUNCTION|GLOBAL):\s+(\w+)\s+(0x[0-9a-fA-F]+)\s*$")
FUNC_NAME_RE = re.compile(r"\b([A-Za-z_][A-Za-z0-9_]*)\s+\(FUNCTION\)")
FUNC_OFFSET_RE = re.compile(r"\b(call|jmp)\s+<OFFSET(0x[0-9a-fA-F]+)>")
OFFSET_RE = re.compile(r"<OFFSET(0x[0-9a-fA-F]+)>")
DATA_NAME_RE = re.compile(
    r"\b([A-Za-z_][A-Za-z0-9_]*)(?:\[[^\]]+\]|\.[A-Za-z_][A-Za-z0-9_]*)*\s+\(DATA\)"
)
IDENT_RE_TEMPLATE = r"\b{}\b"


@dataclass
class Definition:
    path: Path
    line_index: int
    kind: str
    symbol: str


@dataclass
class Candidate:
    kind: str
    symbol: str
    address: str
    source: str


def format_addr(addr: str) -> str:
    addr = addr.lower()
    if addr.startswith("0x"):
        addr = addr[2:]
    return "0x" + addr.zfill(8)


def read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        return path.read_text(encoding="cp1252")


def has_balanced_parens(text: str) -> bool:
    depth = 0
    for ch in text:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
            if depth == 0:
                return True
    return False


def function_definition_start(lines: list[str], start: int, name: str):
    first = lines[start].strip()
    if first.startswith("//") or first.startswith("#") or first.startswith("typedef "):
        return None
    if ";" in first and "{" not in first:
        return None
    name_match = re.search(IDENT_RE_TEMPLATE.format(re.escape(name)) + r"\s*\(", first)
    if not name_match:
        return None

    prefix = first[:name_match.start()]
    insert_index = start
    if not prefix.strip():
        prev = start - 1
        while prev >= 0 and not lines[prev].strip():
            prev -= 1
        if prev < 0:
            return None
        previous_type = lines[prev].strip()
        if (
            previous_type.startswith("//")
            or previous_type.startswith("#")
            or not re.match(r"^[A-Za-z_][A-Za-z0-9_\s\*]*$", previous_type)
        ):
            return None
        first = previous_type + " " + first
        insert_index = prev
    if re.search(r"[^A-Za-z0-9_\s\*]", prefix):
        return None

    signature = first
    i = start
    while not has_balanced_parens(signature):
        i += 1
        if i >= len(lines):
            return None
        stripped = lines[i].strip()
        if stripped.startswith("//"):
            return None
        signature += " " + stripped
        if ";" in stripped and "{" not in stripped:
            return None

    while i < len(lines):
        stripped = lines[i].strip()
        if "{" in stripped:
            return insert_index
        if stripped.endswith(";"):
            return None
        if stripped and not stripped.startswith("//"):
            i += 1
            continue
        i += 1

    return None


def find_function_definitions(root: Path) -> dict[tuple[str, str], list[Definition]]:
    definitions: dict[tuple[str, str], list[Definition]] = {}

    for path in root.rglob("*"):
        if path.suffix.lower() not in SOURCE_EXTENSIONS:
            continue
        try:
            lines = read_text(path).splitlines()
        except UnicodeDecodeError:
            continue

        for i, line in enumerate(lines):
            for match in re.finditer(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(", line):
                name = match.group(1)
                insert_index = function_definition_start(lines, i, name)
                if insert_index is not None:
                    definitions.setdefault(("FUNCTION", name), []).append(
                        Definition(path, insert_index, "FUNCTION", name)
                    )

    return definitions


def find_global_definitions(root: Path, symbols: set[str]) -> dict[tuple[str, str], list[Definition]]:
    definitions: dict[tuple[str, str], list[Definition]] = {}

    for path in root.rglob("*"):
        if path.suffix.lower() not in SOURCE_EXTENSIONS:
            continue
        try:
            lines = read_text(path).splitlines()
        except UnicodeDecodeError:
            continue

        for i, line in enumerate(lines):
            stripped = line.strip()
            if (
                not stripped
                or line[:1].isspace()
                or stripped.startswith("//")
                or stripped.startswith("#")
                or stripped.startswith("extern ")
                or stripped.startswith("typedef ")
            ):
                continue

            for symbol in symbols:
                match = re.search(IDENT_RE_TEMPLATE.format(re.escape(symbol)), line)
                if not match:
                    continue
                prefix = line[:match.start()]
                suffix = line[match.end():]
                if "(" in prefix or ")" in suffix.split(";", 1)[0]:
                    continue
                if not re.match(r"^[A-Za-z_][A-Za-z0-9_\s\*]*$", prefix.strip()):
                    continue
                if not re.match(r"^\s*(\[|=|;|,)", suffix):
                    continue
                definitions.setdefault(("GLOBAL", symbol), []).append(Definition(path, i, "GLOBAL", symbol))

    return definitions


def collect_existing_comments(root: Path, project: str) -> set[tuple[str, str]]:
    existing: set[tuple[str, str]] = set()
    for path in root.rglob("*"):
        if path.suffix.lower() not in {".c", ".h", ".inc"}:
            continue
        try:
            lines = read_text(path).splitlines()
        except UnicodeDecodeError:
            continue
        for line in lines:
            match = COMMENT_RE.match(line)
            if match and match.group(3).upper() == project.upper():
                existing.add((match.group(2).upper(), format_addr(match.group(4))))
    return existing


def definition_has_project_comment(lines: list[str], line_index: int, kind: str, project: str) -> bool:
    i = line_index - 1
    while i >= 0:
        stripped = lines[i].strip()
        if not stripped:
            i -= 1
            continue
        match = COMMENT_RE.match(lines[i].rstrip("\r\n"))
        if not match:
            return False
        if match.group(2).upper() == kind.upper() and match.group(3).upper() == project.upper():
            return True
        i -= 1

    return False


def function_candidates_from_json(json_path: Path, project: str) -> list[Candidate]:
    with json_path.open(encoding="utf-8") as f:
        data = json.load(f)

    candidates: dict[tuple[str, str], Candidate] = {}
    entry_addresses: dict[str, str] = {}
    function_offsets: dict[str, Counter[str]] = {}
    global_addresses: dict[str, set[str]] = {}
    for entry in data.get("data", []):
        name = entry.get("name")
        addr = entry.get("address")
        if isinstance(name, str) and isinstance(addr, str) and name:
            entry_addresses.setdefault(name, format_addr(addr))

        for diff_section in entry.get("diff", []):
            if len(diff_section) != 2:
                continue
            for block in diff_section[1]:
                orig = block.get("orig") or []
                recomp = block.get("recomp") or []
                if len(orig) != len(recomp):
                    continue
                for orig_insn, recomp_insn in zip(orig, recomp):
                    if len(orig_insn) < 2 or len(recomp_insn) < 2:
                        continue
                    orig_match = FUNC_OFFSET_RE.search(orig_insn[1])
                    recomp_match = FUNC_NAME_RE.search(recomp_insn[1])
                    if not orig_match or not recomp_match:
                        continue
                    symbol = recomp_match.group(1)
                    address = format_addr(orig_match.group(2))
                    function_offsets.setdefault(symbol, Counter())[address] += 1

                for orig_insn, recomp_insn in zip(orig, recomp):
                    if len(orig_insn) < 2 or len(recomp_insn) < 2:
                        continue
                    orig_match = OFFSET_RE.search(orig_insn[1])
                    recomp_match = DATA_NAME_RE.search(recomp_insn[1])
                    if not orig_match or not recomp_match:
                        continue
                    global_addresses.setdefault(recomp_match.group(1), set()).add(
                        format_addr(orig_match.group(1))
                    )

    for symbol, address in entry_addresses.items():
        candidates.setdefault(
            ("FUNCTION", symbol),
            Candidate("FUNCTION", symbol, address, "entry"),
        )

    for symbol, addresses in function_offsets.items():
        if ("FUNCTION", symbol) in candidates:
            continue
        if len(addresses) == 1:
            address = next(iter(addresses))
        else:
            most_common = addresses.most_common(2)
            if len(most_common) > 1 and most_common[0][1] == most_common[1][1]:
                continue
            if most_common[0][1] < 2:
                continue
            address = most_common[0][0]
        candidates.setdefault(
            ("FUNCTION", symbol),
            Candidate("FUNCTION", symbol, address, "offset"),
        )

    for symbol, addresses in global_addresses.items():
        if len(addresses) != 1:
            continue
        address = next(iter(addresses))
        candidates.setdefault(
            ("GLOBAL", symbol),
            Candidate("GLOBAL", symbol, address, "offset"),
        )

    return [c for c in candidates.values() if c.address]


def insert_comments(
    definitions: dict[tuple[str, str], list[Definition]],
    candidates: list[Candidate],
    existing: set[tuple[str, str]],
    project: str,
    dry_run: bool,
) -> tuple[int, int, int]:
    pending_by_file: dict[Path, list[tuple[int, str]]] = {}
    skipped_existing = 0
    skipped_ambiguous = 0
    file_lines: dict[Path, list[str]] = {}

    for candidate in candidates:
        key = (candidate.kind, candidate.address)
        if key in existing:
            skipped_existing += 1
            continue

        defs = definitions.get((candidate.kind, candidate.symbol)) or []
        if len(defs) != 1:
            skipped_ambiguous += 1
            continue

        definition = defs[0]
        if definition.path not in file_lines:
            file_lines[definition.path] = read_text(definition.path).splitlines(keepends=True)
        if definition_has_project_comment(
            file_lines[definition.path],
            definition.line_index,
            candidate.kind,
            project,
        ):
            skipped_existing += 1
            continue

        comment = f"// {candidate.kind}: {project} {candidate.address}"
        pending_by_file.setdefault(definition.path, []).append((definition.line_index, comment))

    inserted = 0
    for path, insertions in pending_by_file.items():
        original = read_text(path)
        lines = original.splitlines(keepends=True)
        for line_index, comment in sorted(set(insertions), reverse=True):
            newline = "\n"
            if lines[line_index].endswith("\r\n"):
                newline = "\r\n"
            if line_index > 0 and lines[line_index - 1].rstrip("\r\n") == comment:
                continue
            lines.insert(line_index, comment + newline)
            inserted += 1
        if not dry_run:
            path.write_text("".join(lines), encoding="utf-8", newline="")
        action = "Would update" if dry_run else "Updated"
        print(f"{action}: {path} ({len(insertions)} comments)")

    return inserted, skipped_existing, skipped_ambiguous


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Insert SHANDALAR function comments inferred from reccmp JSON offsets."
    )
    parser.add_argument("json", type=Path, help="reccmp JSON from --offset-addresses --json")
    parser.add_argument("folder", type=Path, help="Source folder to scan recursively")
    parser.add_argument("--project", default="SHANDALAR", help="Project name for inserted comments")
    parser.add_argument("--dry-run", action="store_true", help="Print changes without editing files")
    args = parser.parse_args()

    existing = collect_existing_comments(args.folder, args.project)
    candidates = function_candidates_from_json(args.json, args.project)
    definitions = find_function_definitions(args.folder)
    definitions.update(
        find_global_definitions(
            args.folder,
            {c.symbol for c in candidates if c.kind == "GLOBAL"},
        )
    )

    inserted, skipped_existing, skipped_ambiguous = insert_comments(
        definitions=definitions,
        candidates=candidates,
        existing=existing,
        project=args.project,
        dry_run=args.dry_run,
    )

    print()
    print(f"Candidates: {len(candidates)}")
    print(f"Inserted: {inserted}")
    print(f"Already annotated: {skipped_existing}")
    print(f"Ambiguous or missing definition: {skipped_ambiguous}")


if __name__ == "__main__":
    main()
