# function-comment-inserter

Small utility script for annotating matched functions across two codebases using a Ghidra Version Tracking export.

Given a CSV exported from Ghidra, this script:

* Reads `Source Address` → `Dest Address` mappings

* Searches a source tree for lines like:

  // FUNCTION: {SOURCEPROJECT} 0xXXXXXXXX

* Inserts a corresponding line above each match:

  // FUNCTION: {DESTPROJECT} 0xXXXXXXXX

This is useful when aligning/reconciling functions between two reverse-engineered projects.

---

## Input

CSV exported from **Ghidra Version Tracking** containing at least:

* `Source Address`
* `Dest Address`

Example:

```
Source Address,Dest Address
00401234,00505678
401ABC,500DEF
```

---

## Usage

```
python add_function_comments.py <csv> <folder> <SOURCEPROJECT> <DESTPROJECT>
```

Example:

```
python add_function_comments.py facemaker_to_shandalar.csv src FACEMAKER SHANDALAR
```

Dry run (no file modifications):

```
python add_function_comments.py facemaker_to_shandalar.csv src FACEMAKER SHANDALAR --dry-run
```

---

## Behavior

* Matches both 6-digit and 8-digit addresses:

  * `0x401234` ⇄ `0x00401234`
* Case-insensitive matching
* Preserves indentation
* Avoids duplicate insertion if the destination line already exists above
* Recursively scans the target folder

By default, only common source/text files are processed. Use:

```
--all-files
```

to scan everything.

---

## Example

### Before

```
// FUNCTION: FACEMAKER 0x00401234
```

### After

```
// FUNCTION: SHANDALAR 0x00505678
// FUNCTION: FACEMAKER 0x00401234
```

---

## Notes

* Destination addresses are normalized to `0xXXXXXXXX` format.
* Non-text/binary files are skipped automatically.
* Designed for reverse engineering workflows (e.g., Ghidra + decomp sync).

---
