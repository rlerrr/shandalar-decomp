#!/usr/bin/env python3
"""
Convert single-line C block comments like:

    /* hi */

into:

    // hi

Leaves true multiline block comments untouched, e.g.:

    /*
      hi
    */

or:

    /* hi
       there */

Usage:
    python convert_comments.py path/to/file.c
"""

import argparse
import re
from pathlib import Path


# Matches block comments that begin and end on the same line only.
SINGLE_LINE_BLOCK_COMMENT_RE = re.compile(
    r"/\*\s*(.*?)\s*\*/",
    re.DOTALL,
)


def convert_single_line_block_comments(text: str) -> str:
    lines = text.splitlines(keepends=True)
    out_lines = []

    for line in lines:
        newline = ""
        if line.endswith("\r\n"):
            newline = "\r\n"
            content = line[:-2]
        elif line.endswith("\n"):
            newline = "\n"
            content = line[:-1]
        else:
            content = line

        def repl(match: re.Match) -> str:
            inner = match.group(1)

            # If somehow contains newline, leave alone
            if "\n" in inner or "\r" in inner:
                return match.group(0)

            return f"// {inner}"

        new_content = SINGLE_LINE_BLOCK_COMMENT_RE.sub(repl, content)
        out_lines.append(new_content + newline)

    return "".join(out_lines)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Convert single-line /* ... */ comments into // ... comments."
    )
    parser.add_argument("file", help="Path to source file")

    args = parser.parse_args()

    path = Path(args.file)

    if not path.is_file():
        raise SystemExit(f"File not found: {path}")

    original = path.read_text(encoding="utf-8", errors="replace")
    converted = convert_single_line_block_comments(original)

    if converted != original:
        path.write_text(converted, encoding="utf-8", newline="")
        print(f"Updated: {path}")
    else:
        print(f"No changes: {path}")


if __name__ == "__main__":
    main()