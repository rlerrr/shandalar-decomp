# Ghidra resources

## `Magic-trace.c`

From [Shandalar abandonware updated with 2015 magic cards](https://github.com/ShandalarMagic/Shandalar) repo.  An IDA dump of Mok's modified `magic.exe` with a lot of symbol names.  

## `ImportMagicTraceAndParams.java`
PyGhidra script which attempts to import `magic-trace.c`. Ghidra's version tracking makes it possible to apply some of the symbol names to the vanilla `magic.exe` and `shandalar.exe`.

## `*.csv` files
Ghidra "Functions" window exports from their respective binaries.  Useful for matching with `reccmp`

## `check_symbol_gaps.py`
Detects issues in the `.csv` files exported from Ghidra.  Ghidra tends to not include unreachable/jumptable code in its `size` which is problematice when used with `reccmp`

## `DecompileToSingleC.java`
PyGhidra script to dump an entire binary to a single `.c` file.  Doesn't work super great.