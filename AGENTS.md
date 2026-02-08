<INSTRUCTIONS>
These instructions apply to the entire repository.

## Decomp / Matching Rules
- Toolchain: MSVC 4.20 (C89 only). Avoid C99+ features (e.g., declarations after statements, `for (int i=...)`, `stdint.h` assumptions, etc.).
- Try to update local variable names while matching.
- Never use inline assembly (`__asm`, `asm`, etc.).
- Try hard not to use `goto`. Most `jmp` patterns translate to structured control flow:
  - A single `jmp` with a negative offset is usually the end of a loop.  A conditional jump with a negative offset is typically the end of a `do { } while ()`
  - A single `jmp` with a small offset is often the start of a for loop, jumping over the increment/decrement statement which MSVC always puts at the top of the loop.
  - Other single `jmp` instructions are usually an `else` or an early `return`.
  - Two `jmp`s in a row is very commonly a `return` followed by an `else` block.
- If the compiler’s stack layout won’t match, you may force a stable layout by grouping locals in a single struct:
  - Example: `struct { int var1; int var2; } s;`
  - Prefer expanding this struct rather than adding loose locals.
  - If the stack is already laid out as a struct don't attempt to "unwrap" it.
- If a 100% match seems impossible fall back to the best possible match.
- Do not attempt to fix any perceived bugs in the code.  We aim to faithfully match the original in all aspects.
- Ignore files in the Ghidra folder, especially magic-trace.c which has nothing to do with the current task.

## Matching Tips (MSVC 4.20)
- Prefer compound assignments to coax “op [mem], reg” codegen:
  - `ptr += n;` / `global += n;` / `param += n;` often matches better than load/add/store sequences.
  - When necessary (matching-only), a controlled type-pun can force pointer-in-place updates: `*(int *)&ptr += n;` (use sparingly).
- Avoid “helpful” masks on shift counts (e.g. `count & 0x1f`); x86 already masks via `cl` and the extra ops usually break matching.
- Use `byte` casts on shift counts when you expect `mov cl, al` / `mov cl, [mem]` patterns.
- For lookup tables, write `idx * 3` to encourage `lea reg, [reg + reg*2]` patterns (instead of more complex arithmetic).
- Watch for signed compares against `0xFF`: `cmp reg, 0FFh` is `-1` (imm8 sign-extended), so match it in C as `== -1` when appropriate.
- The order of parameters in `cmp` and other commutative opcodes like `test`, `add`, `imul`, `or`, `and`, and `xor` are "randomly" swapped and cannot be reliably controlled.  Don't bother trying.

## Build
- Build the `cardartlib` target with: `nmake cardartlib`

## Check Assembly Match (reccmp)
- Use `reccmp-reccmp` to compare the recompiled function against the original:
  - PowerShell example:
    - `$env:PYTHONUTF8='1'; reccmp-reccmp --target CARDARTLIB --no-color --verbose 0xXXXXXXXX`
  - Note: set `PYTHONUTF8=1` to avoid Windows console encoding issues in verbose output.
- The hex address (`0xXXXXXXXX`) comes from the `reccmp`-style comment immediately above the function implementation, e.g.:
  - `// FUNCTION: CARDARTLIB 0x10002f70`
- Prefer writing `reccmp` logs into `temp/` and keep them out of git:
  - `mkdir -Force temp | Out-Null`
  - `$env:PYTHONUTF8='1'; reccmp-reccmp --target CARDARTLIB --no-color --verbose 0xXXXXXXXX *> temp\\reccmp_0xXXXXXXXX.txt`

</INSTRUCTIONS>
