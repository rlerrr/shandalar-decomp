# SHANDALAR Validation

This tracks SHANDALAR function acceptance separately from raw assembly matching.
Generated artifacts live in `temp/`; only manual annotations are checked in.

## Generate A Report

Run reccmp once, then build the validation report:

```powershell
$env:PYTHONUTF8='1'
make.bat | Out-Null
reccmp-reccmp --target SHANDALAR --json temp\shandalar_reccmp.json --json-diet --silent
python tools\validation\build_shandalar_validation_report.py
```

Outputs:

- `temp/shandalar_validation.csv`
- `temp/shandalar_validation.sqlite`
- `temp/shandalar_validation.md`

For manual diff review, generate the larger JSON with per-function diffs:

```powershell
reccmp-reccmp --target SHANDALAR --json temp\shandalar_reccmp_full.json --silent
python tools\validation\show_shandalar_diffs.py --top 25 --min-match 98 --context 12
```

Use `--address 0xXXXXXXXX` to inspect specific functions from the full JSON
without rerunning reccmp.

## Manual Notes

Add accepted functions to `docs/shandalar_validation_notes.csv`.
The generated report joins those notes by SHANDALAR address.

Statuses:

- `exact`: assembly match; normally generated automatically.
- `reccmp-effective`: reccmp marked the nonmatch as effective.
- `reviewed`: manually reviewed against original assembly/decompile and accepted.
- `accepted-risk`: accepted with a known caveat documented in `notes`.
- `needs-work`: reviewed and not accepted; skip during normal near-match passes until deliberately revisited.
- `not-implemented`: present in `Ghidra/shandalar.csv` but not found by reccmp.
- `needs-review`: implemented but neither exact, effective, nor manually accepted.

Suggested confidence values are `low`, `medium`, and `high`.

## Manual Acceptance Rules

Apply these rules only after inspecting the complete changed instruction blocks.
Raw match percentage alone is not evidence that a function is correct.

The following differences are acceptable when the surrounding data flow remains
faithful:

- `cmp` operand or register reordering, provided the following conditional jump
  still implements the original condition. Swapping operands commonly requires
  the inverse signed or unsigned jump.
- Register or parameter reordering for commutative operations such as addition,
  multiplication, bitwise AND, OR, and XOR.
- Reordered address arithmetic for array and struct dereferences when it computes
  the same base, index, scale, and field offset.
- Array indexing with a negative bias, such as `array[index - 1]`, when the
  encoded base, scale, and bias still select the intended array element. Reccmp
  may label the biased base as an unrelated relocated global immediately before
  the array; matching that label depends on global binary layout this project is
  not attempting to reproduce exactly.
- Equivalent bit-shift instruction encodings. A one-byte encoding-size difference
  may shift surrounding branch displacements by one byte.
- Branch-displacement drift caused only by one of the accepted instruction-size
  differences above.
- Original-only unreachable instruction blocks when all reachable control flow,
  side effects, and results remain faithful. These may represent source constructs
  such as `if (0)` that were not decompiled.
- Equivalent control flow, replacing hand-rolled or macro-generated
  branch layouts that are not reproducible with structured `while` or `do { } while` loops.
  Most commonly seen as `loop: body(); if (!done) goto loop;` which can be more cleanly
  represented as `do { body(); } while (!done);`.
- Relocated indirect calls through function pointers when the complete pointer
  calculation selects the same array element and function-pointer field. Reccmp
  does not reliably normalize these call targets, so the original and recompiled
  base addresses may remain different.

Optimized original functions can have extensive structural drift because this
project generally has not attempted instruction-level matching for them. Validate
their reachable behavior and side effects rather than applying the normal `/Od`
code-generation expectations or accepting them from percentage alone.

Do not accept a function when a difference changes a non-commutative operand,
signedness or boundary condition, array/struct field, global identity, call target,
call argument, reachable side effect, or return behavior. Keep it as `needs-work`
until that difference is fixed or deliberately proven equivalent.

When an original diff ends immediately before a recompiled function epilogue,
check the function's `orig_size` in `Ghidra/shandalar.csv`. If the recorded range
cuts off the original epilogue, extend it through the complete function before
reviewing the remaining differences.
