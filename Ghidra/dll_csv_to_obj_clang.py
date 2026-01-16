#!/usr/bin/env python3
import subprocess
import csv
import re
import sys
from pathlib import Path

def run(cmd):
    return subprocess.check_output(cmd, shell=True, text=True, errors="ignore")

def parse_int(s):
    s = s.strip()
    if s.lower().startswith("0x"):
        return int(s, 16)
    if re.fullmatch(r"[0-9a-fA-F]+", s):
        return int(s, 16)
    return int(s)

# --- dumpbin parsing (VC4 output format) ---

def get_text_section_info(dll_path: Path):
    out = run(f'dumpbin /headers "{dll_path}"')
    lines = out.splitlines()
    in_text = False
    raw_size = None
    raw_ptr = None

    for line in lines:
        l = line.strip()
        if l.startswith(".text") and "name" in l:
            in_text = True
            continue
        if in_text:
            if "size of raw data" in l:
                raw_size = int(l.split()[0], 16)
            elif "file pointer to raw data" in l:
                raw_ptr = int(l.split()[0], 16)
            elif l.startswith("SECTION HEADER"):
                break
        if raw_size is not None and raw_ptr is not None:
            break

    if raw_size is None or raw_ptr is None:
        raise RuntimeError("Could not find .text section in dumpbin output")

    return raw_ptr, raw_size

def extract_text_bin(dll_path: Path, out_bin: Path) -> int:
    off, size = get_text_section_info(dll_path)
    with open(dll_path, "rb") as f:
        f.seek(off)
        data = f.read(size)
    out_bin.write_bytes(data)
    print(f"[+] Extracted .text @ 0x{off:X}, size 0x{size:X}")
    return size

# --- Ghidra CSV parsing ---

def sanitize_name(name: str) -> str:
    name = name.replace(":", "__")
    if name == "assert":
        name = "_assert"
    return name

def parse_ghidra_csv(csv_path: Path):
    funcs = []
    with open(csv_path, newline="", encoding="utf-8", errors="ignore") as f:
        reader = csv.DictReader(f)
        for row in reader:
            name = (row.get("Name") or "").strip()
            loc  = (row.get("Location") or "").strip()
            if not name or not loc:
                continue
            addr = parse_int(loc)
            funcs.append((addr, sanitize_name(name)))
    funcs.sort()
    print(f"[+] Parsed {len(funcs)} functions from Ghidra CSV")
    return funcs

# --- ELF asm emission with real labels inside .text ---

def write_elf_with_labels(funcs, image_base: int, text_size: int, asm_path: Path, bin_name: str):
    # Convert VAs -> offsets in .text
    # If your Ghidra addresses are VAs, offset = VA - image_base.
    entries = []
    for va, name in funcs:
        off = va - image_base
        if off < 0 or off >= text_size:
            # skip anything outside .text
            continue
        entries.append((off, name))

    # Sort by offset
    entries.sort()

    # De-dup names *after* sorting so duplicates get unique suffixes deterministically
    seen = {}
    deduped = []
    for off, name in entries:
        if name in seen:
            name = f"{name}_{off:08X}"
        seen[name] = True
        deduped.append((off, name))

    with open(asm_path, "w", encoding="utf-8") as o:
        o.write('.section .text,"ax",@progbits\n')
        o.write(".p2align 4\n\n")

        cur = 0
        for off, name in deduped:
            if off < cur:
                # should not happen after sorting, but guard anyway
                continue

            # Emit bytes from cur..off
            length = off - cur
            if length:
                o.write(f'.incbin "{bin_name}", {cur}, {length}\n')

            # Define a real label at this offset
            o.write(f".globl {name}\n")
            o.write(f".type {name}, @function\n")
            o.write(f"{name}:\n")
            # no bytes emitted; next incbin continues from this same offset
            cur = off

        # Emit remaining bytes
        if cur < text_size:
            o.write(f'.incbin "{bin_name}", {cur}, {text_size - cur}\n')

    print(f"[+] Wrote ELF asm with in-section labels: {asm_path}")

def main():
    if len(sys.argv) < 4:
        print("usage: dll_ghidra_csv_to_elf_obj.py <dll> <ghidra.csv> <out.o> [--no-assemble]")
        sys.exit(1)

    dll_path = Path(sys.argv[1])
    csv_path = Path(sys.argv[2])
    out_o    = Path(sys.argv[3])
    no_asm   = "--no-assemble" in sys.argv

    text_bin = Path("text.bin")
    asm_path = Path("baseline.s")

    text_size = extract_text_bin(dll_path, text_bin)
    funcs = parse_ghidra_csv(csv_path)

    # Infer ImageBase from lowest VA (works for 0x10000000-style images)
    image_base = funcs[0][0] & 0xFFFF0000
    print(f"[+] Inferred ImageBase = 0x{image_base:08X}")

    write_elf_with_labels(funcs, image_base, text_size, asm_path, text_bin.name)

    if not no_asm:
        cmd = f'clang -c -target i386-linux-gnu -fno-pic "{asm_path}" -o "{out_o}"'
        print(f"[+] Assembling ELF object: {cmd}")
        subprocess.check_call(cmd, shell=True)
        print(f"[+] Wrote baseline object: {out_o}")

if __name__ == "__main__":
    main()
