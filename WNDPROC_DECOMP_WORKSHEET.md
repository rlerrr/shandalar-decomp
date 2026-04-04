# WndProc Decomp Worksheet (case-by-case)

Use this as a scratchpad/checklist when decompiling and matching large WndProc-style functions in this repo.

The goal here is **binary matching** with MSVC 4.20 `/Od`, not refactoring.

## Setup

- Target + address:
  - Target: `__________`
  - Function VA: `0x________`
  - Source file: `src/.../...c:____`
- Commands:
  - Build: `nmake`
  - Diff: `reccmp-reccmp --target <TARGET> --no-color --verbose 0x<ADDR> *> temp\\reccmp_0x<ADDR>_<tag>.txt`

## High-signal checks (do these first)

- Frame size: confirm `sub esp, 0x____` is close.
- Prologue shape: look for early `mov edi, edi` / `push ebp` patterns (depending on file).
- Switch temp: MSVC usually copies the `switch()` dispatch into a stack slot by itself; don't try to "declare" it into the right offset.
- String pushes:
  - If reccmp shows `(STRING)`, prefer string literals in the call.
  - If reccmp shows `(DATA)`, prefer a named global/static string.

## Stack layout playbook (recommended)

1. Put locals into one struct:

   - `struct WndprocXLocals { ... } s;`

2. Add padding to land important slots:

   - Identify 2-3 "anchor" locals in the diff (e.g. a `MessageBoxA` buffer at `lea eax, [ebp-0x60]`).
   - Pad until the anchor locals emit the right `lea` offsets.

3. Keep "hot" locals near the top of the frame:

   - Per-case `HWND`, `int`, and `LRESULT` temporaries are usually better as fixed struct members than ad-hoc locals.

## WM_COMMAND / menu command routine

### Command extraction

- Prefer explicit bit ops to coax original patterns:
  - `cmd = (unsigned int)wparam & 0xffff;`
  - `notify = ((unsigned int)wparam >> 16) & 0xffff;`

### Case-by-case workflow

For each command `case`:

1. Identify it in the reccmp log by a distinctive call or string.
2. Make the **block boundaries** match first:
   - One `return` vs `break`, missing `else`, or swapped if/else can balloon diffs.
3. Match call argument order and literal-vs-data pushes.
4. Only then micro-tune:
   - loop compare direction (`limit > i` vs `i < limit`)
   - reduce temporaries that force spills
   - use compound assignments when it improves store patterns

## "Unknown constant" procedure (when message IDs look wrong)

When the diff shows `push 0x??` for `SendMessageA`/`PostMessageA` and the symbolic constant in C doesn't match:

1. Find the call site address in the reccmp log.
2. Verify the original bytes in Ghidra (or by reading bytes at that VA).
3. Update C to use the concrete message ID that the original actually pushes.

Common example: a command handler might send a **custom message** to a child window (`0xCC`) instead of forwarding as `WM_COMMAND`.

## Popup/messagebox matching checklist

If the original uses a local buffer (common pattern is `lea eax, [ebp-0x60]`):

- Avoid inline helpers that build the string; they often:
  - introduce extra locals
  - change frame size
  - add extra calls/spills
- Prefer explicit in-case code:
  - `load_text(...)`
  - `strcpy/strcat/sprintf` into the same buffer
  - `MessageBoxA(hwnd, buf, title, flags)`

## Per-case notes

Keep a short running log of what you changed and why. This is useful when you come back weeks later.

- Case ID: `_____`
  - Distinctive signature in diff: `________________`
  - Stack anchors used: `________________`
  - Notes: `________________`

