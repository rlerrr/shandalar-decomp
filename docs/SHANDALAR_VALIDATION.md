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
