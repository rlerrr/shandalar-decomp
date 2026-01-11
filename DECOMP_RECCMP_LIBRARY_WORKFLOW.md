# Reccmp Library-Annotation Workflow (MSVC 4.20-era)

Notes for fixing `reccmp-reccmp --target <TARGET>` “library” mismatches/noise across multiple DLLs.

## Key Concepts

### What `SYMBOL` means in annotations

`// LIBRARY: <TARGET> 0xXXXXXXXX SYMBOL` makes reccmp match this original address using the recompiled PDB’s **PUBLICS decorated symbol string** (reccmp stores it as `kvstore.symbol`).

If the function is only a **local proc** in the PDB (`S_LPROC32`) and not present in **PUBLICS**, the recompiled side has no `symbol` string, so `SYMBOL` annotations will always produce “Failed to match ... with symbol ...” even when the function exists.

Rule of thumb:
- Use `SYMBOL` only when you know the exact PUBLIC/decorated name exists and you need the uniqueness.
- For local CRT helpers, drop `SYMBOL` and match by name instead.

### How reccmp gets names from a PDB

Reccmp parses the PDB via `cvdump.exe`:
- **PUBLICS** provides `decorated_name` (what `SYMBOL` matching uses).
- **SYMBOLS** provides `friendly_name` for `S_GPROC32/S_LPROC32` (what name-based function matching uses).

## Fixing “missing library functions” that cause <100% diffs

Symptom:
- A library function shows <100%.
- Running `reccmp-reccmp --verbose 0x<orig_addr>` shows calls like `call <OFFSET#>` on the original side, but the recompiled side shows a real callee name (e.g. `_unlock`, `_errno`).
- That usually means the callee’s original entrypoint is not annotated as a LIBRARY function, so reccmp can’t replace the original call target with a name.

### Workflow

1) Generate a summary with recompiled addresses:
   - `$env:PYTHONUTF8='1'; reccmp-reccmp --target <TARGET> --no-color --print-rec-addr | Tee-Object temp\\reccmp_summary.txt`

2) For a failing library function, dump the verbose diff:
   - `$env:PYTHONUTF8='1'; reccmp-reccmp --target <TARGET> --no-color --verbose 0xXXXXXXXX | Tee-Object temp\\reccmp_verbose_0xXXXXXXXX.txt`

3) Find unresolved call targets in the verbose log:
   - `rg -n "call .*<OFFSET" temp\\reccmp_verbose_0xXXXXXXXX.txt`

4) Compute the original callee entrypoint for a `call rel32`:
   - Read 5 bytes at the call site in Ghidra (`E8 xx xx xx xx`).
   - Decode `rel32` as signed little-endian.
   - `target = (call_site + 5) + rel32`.

   Example:
   - bytes: `E8 DF F2 FF FF`
   - `rel32 = 0xFFFFF2DF` (signed)
   - `target = 0x(call_site + 5) + rel32`

5) Identify the callee name:
   - Prefer the name shown on the recompiled side of the verbose diff (`+call <name> (FUNCTION)`).
   - If needed, confirm via PDB (`cvdump -s` for SYMBOLS name; `cvdump -p` for PUBLICS decorated name).

6) Add a new LIBRARY marker to the target’s `library_*.h` in original-address order:
   - Prefer **name-based** markers for local-proc style helpers:
     - `// LIBRARY: <TARGET> 0xXXXXXXXX`
     - `// <friendly_name>`
   - Use `SYMBOL` only if you are matching against a PUBLIC/decorated symbol string and it is unique:
     - `// LIBRARY: <TARGET> 0xXXXXXXXX SYMBOL`
     - `// <decorated_name>`

7) Re-run the same `--verbose` to verify the `<OFFSET#>` becomes a named call on the original side.

## Fixing “missing GLOBALs” (data placeholders like `<OFFSET#>`)

Symptom:
- A function’s diff shows placeholders for absolute data references, e.g.
  - `mov eax, dword ptr [<OFFSET#>]`
  - `cmp dword ptr [<OFFSET#>], 0`
  - `call dword ptr [<OFFSET#>]` (function pointer stored in a global)
- The recompiled side shows `+... <name> (DATA)` (or a named `FUNCTION` pointer).

What’s happening:
- Reccmp replaces **absolute memory addresses** (`[imm32]` or `[base+disp32]`) with `<OFFSET#>` if the original-side address is known to be an address but has no annotation name.
- For these, you usually need `// GLOBAL:` annotations (not `// LIBRARY:`).

### Workflow (GLOBALs)

1) Dump verbose diff for the function:
   - `$env:PYTHONUTF8='1'; reccmp-reccmp --target <TARGET> --no-color --verbose 0xXXXXXXXX | Tee-Object temp\\reccmp_verbose_0xXXXXXXXX.txt`

2) Identify the instruction address that uses `<OFFSET#>` (left/original side).

3) Use Ghidra to recover the real referenced address:
   - Easiest: disassemble the original function in Ghidra and look at that exact instruction address.
   - If you’re decoding bytes, common encodings:
     - `A1 imm32` → `mov eax, [imm32]`
     - `A3 imm32` → `mov [imm32], eax`
     - `83 3D imm32 00` → `cmp dword ptr [imm32], 0`
     - `FF 15 imm32` → `call dword ptr [imm32]`
     - `F6 04 85 imm32 01` → `test byte ptr [eax*4 + imm32], 1` (array/table base is `imm32`)
     - `83 3C 85 imm32 FF` → `cmp dword ptr [eax*4 + imm32], -1` (array/table base is `imm32`)

4) Add the original address to an annotation file as a `GLOBAL` marker (keep address order):
   - `// GLOBAL: <TARGET> 0xXXXXXXXX`
   - `// <symbolic_name>`

5) Avoid duplicate address annotations:
   - Reccmp drops reused addresses across the whole target; if the address is already annotated elsewhere (often as `DAT_...`), rename that existing annotation instead of adding a second one.

### Caution: annotating `.text` data (jump tables)

Some compiler jump tables live inside `.text` (e.g. `jmp dword ptr [reg*4 + imm32]` where `imm32` points into the middle of the function).

Adding a `GLOBAL` marker at that table address can make diffs worse (reccmp may treat it as a separate data entity and the function diff can balloon). Prefer:
- Fixing the *real* missing globals around it, or
- Leaving the jump-table base unnamed if everything else matches.

## Dealing with ambiguity

Some helpers appear multiple times in the PDB SYMBOLS (same friendly name in multiple object files), e.g. `wcsncnt`, `fix_grouping`, `strncnt`.

If you see reccmp warnings like “Ambiguous match ... on name ...”, switch to one of:
- Leave it name-based and accept ambiguity (often harmless).
- Use a `SYMBOL` marker if a unique PUBLIC/decorated symbol exists.
- If there are multiple identical local procs at different addresses, annotate by address + name and accept that reccmp may still warn.

## Useful commands

- PDB PUBLICS:
  - `C:\\dev\\reccmp\\reccmp\\bin\\cvdump.exe -p <path-to.pdb>`
- PDB SYMBOLS:
  - `C:\\dev\\reccmp\\reccmp\\bin\\cvdump.exe -s <path-to.pdb>`
- Grep for unresolved calls in a verbose diff:
  - `rg -n "call .*<OFFSET" temp\\reccmp_verbose_*.txt`

## Output hygiene

Write logs to `temp\\` (or another throwaway folder) and keep that folder in `.gitignore` so diffs/logs don’t pollute the repo.
