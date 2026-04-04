# WndProc Matching Notes (MSVC 4.20 /Od)

Practical tips for decompiling large window procedures (WndProcs) in this repo, where the goal is **binary matching** (not readability).

## Ground rules

- Toolchain is **MSVC 4.20**, **C89**, **/Od** (no optimizations).
- Prefer structured control flow; avoid `goto`.
- Avoid "pointer-math hacks" (type-puns like `*(int *)&ptr += ...`) unless absolutely required.
- Don't try to force the compiler's **`switch()` temp slot** layout; MSVC usually copies the dispatch value into a stack slot automatically.

## Workflow (case-by-case)

1. Build: `nmake`
2. Diff: `reccmp-reccmp --target <TARGET> --no-color --verbose 0x<ADDR> *> temp\\reccmp_0x<ADDR>.txt`
3. Pick **one case** (usually `WM_COMMAND` subcases) and match it fully before touching the next.
4. Use the verbose log to locate the case by **distinctive calls/strings** (e.g. `MessageBoxA`, `DialogBoxParamA`, `load_text`, `SendMessageA` with unique constants).
5. Edit only the smallest possible region until the diff for that case collapses.

## Stack layout strategy (big WndProcs)

### Prefer a single locals struct

Large WndProcs in the original often use one huge frame. The most stable way to keep MSVC emitting similar stack access is:

- Use **one** `struct Locals { ... } s;` at function top.
- Add padding fields to hit important `ebp-XX` slots.
- Avoid introducing "loose" locals outside the struct.

### Don't rely on `STATIC_ASSERT(sizeof(struct) == ...)`

This toolchain can reject `sizeof(struct)`-based enum asserts as "not a constant expression" in some builds. Keep size checks as comments and validate via `reccmp` frame size (`sub esp, 0x...`) instead.

### Keep "hot" case locals near the small slots

For `WM_COMMAND` listbox/horzlist synchronization, it's often worth reserving explicit slots near `ebp-0x9c/-0x98/-0x94/-0x90` (or whatever the original uses) for:

- `HWND cmd_hwnd;`
- `HWND cmd_tgt;`
- `int cmd_cursel;`
- `LRESULT cmd_itemdata;`

This reduces spurious spills and makes `SendMessageA` sequences line up.

## `WM_COMMAND` matching tips

### Prefer bit-extraction over LOWORD/HIWORD macros

To steer MSVC into the original `and eax, 0xffff` / `shr` patterns, prefer:

- `switch ((unsigned int)wparam & 0xffff)`
- `(((unsigned int)wparam >> 16) & 0xffff)` for notification codes

### Loop compare direction matters

MSVC /Od is very literal about compare ordering. If the original shows:

- `mov eax, [limit]; cmp [i], eax; jge ...`

then writing the loop condition as `limit > i` is often closer than `i < limit`.

### Use string literals vs. static consts intentionally

Reccmp will show either:

- `push "literal" (STRING)` (original pushed a literal)
- `push <symbol> (DATA)` (original pushed an address of a global string object)

Match what the original does:

- If original is `(STRING)`, use a **string literal** in the call.
- If original is `(DATA)`, use a **named static/global** string so the compiler emits a data reference.

### Avoid "helpful" temporaries that create extra spills

If the verbose diff shows an unexpected `mov [ebp-XX], eax` (a spill) right before a call, remove intermediate locals and inline the expression (as long as it keeps C89 and doesn't change evaluation order).

## Diagnosing "missing case" blocks

Symptoms:

- A large mismatch chunk appears between two cases you believe are adjacent.
- The original has a long straight-line block, but your C skips to the next case.

Approach:

- Use the verbose log to find the original block boundaries (jump targets around the switch epilogue).
- Identify the command ID by finding where the original computes `wparam & 0xffff` then compares to an immediate.
- Add the missing `case <ID>:` with minimal locals, and only then start matching instruction order.

## Common "gotchas"

- Changing one buffer size inside the locals struct can silently:
  - Change `sub esp, 0x...` (frame size)
  - Shift every later `ebp-XX` slot
  - Break previously-matched cases

When a buffer is needed for one case:

- Prefer reusing an existing padding region or buffer already present in the original frame.
- If you must change size, re-verify "hot" offsets (like `ebp-0x9c`) immediately with `reccmp`.

