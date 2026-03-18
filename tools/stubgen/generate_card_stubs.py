#!/usr/bin/env python3
from __future__ import annotations

import argparse
import csv
import re
from collections import Counter
from itertools import zip_longest
from pathlib import Path


def parse_args() -> argparse.Namespace:
    script_dir = Path(__file__).resolve().parent
    tools_dir = script_dir.parent

    parser = argparse.ArgumentParser(
        description="Generate card stub functions from magic/shandalar card_data CSV dumps."
    )
    parser.add_argument(
        "--magic-csv",
        default=str(tools_dir / "carddata_dump" / "magic_card_data.csv"),
        help="Path to magic_card_data.csv",
    )
    parser.add_argument(
        "--shandalar-csv",
        default=str(tools_dir / "carddata_dump" / "shandalar_card_data.csv"),
        help="Path to shandalar_card_data.csv",
    )
    parser.add_argument(
        "--cards-csv",
        default=str(tools_dir / "cardsdat2csv" / "Cards.csv"),
        help="Path to Cards.csv",
    )
    parser.add_argument(
        "--output",
        default=str(script_dir / "generated_card_stubs.c"),
        help="Path to output .c file",
    )
    return parser.parse_args()


def read_csv_rows(path: Path) -> list[dict[str, str]]:
    encodings = ("utf-8-sig", "cp1252", "latin-1")
    last_error: UnicodeDecodeError | None = None
    encoding: str

    for encoding in encodings:
        try:
            with path.open("r", newline="", encoding=encoding) as handle:
                reader = csv.DictReader(handle)
                return list(reader)
        except UnicodeDecodeError as error:
            last_error = error

    if last_error is not None:
        raise last_error
    raise RuntimeError(f"Unable to read CSV file: {path}")


def sanitize_name(name: str) -> str:
    tokens: list[str] = []
    raw = name.strip()

    for cluster in re.findall(r"[A-Za-z0-9]+", raw):
        split_tokens = re.findall(r"[A-Z]+(?=[A-Z][a-z]|$)|[A-Z]?[a-z]+|[0-9]+", cluster)
        if split_tokens:
            tokens.extend(split_tokens)
        else:
            tokens.append(cluster)

    normalized_tokens: list[str] = []
    for token in tokens:
        if token.upper() == "FX":
            normalized_tokens.append("FX")
        else:
            normalized_tokens.append(token.lower())

    suffix = "_".join(normalized_tokens).strip("_")
    if not suffix:
        return "unknown_card"
    if suffix[0].isdigit():
        return "n_" + suffix
    return suffix


def build_cards_name_lookup(cards_rows: list[dict[str, str]]) -> dict[str, str]:
    names_by_id: dict[str, str] = {}
    for row in cards_rows:
        card_id = row.get("id", "").strip()
        card_name = row.get("name", "").strip()
        if card_id and card_name and card_id not in names_by_id:
            names_by_id[card_id] = card_name
    return names_by_id


def normalize_pointer(pointer: str) -> str:
    return pointer.strip().lower()


def generate_stubs(
    magic_rows: list[dict[str, str]],
    shandalar_rows: list[dict[str, str]],
    cards_name_by_id: dict[str, str],
) -> tuple[list[str], int, int]:
    stubs: list[str] = []
    used_names: dict[str, int] = {}
    fallback_name_count = 0
    skipped_conflict_count = 0
    emitted_dummy_none = False
    dummy_magic_ptr = "0x0053aa50"
    dummy_shandalar_ptr = "0x00488ca0"

    magic_ptr_counts: Counter[str] = Counter()
    shandalar_ptr_counts: Counter[str] = Counter()

    for row_index, rows in enumerate(zip_longest(magic_rows, shandalar_rows), start=1):
        magic_row, shandalar_row = rows
        if magic_row is None or shandalar_row is None:
            raise ValueError(
                "Input CSV row counts do not match while zipping: "
                f"mismatch at row {row_index}."
            )

        magic_id = magic_row.get("id", "").strip()
        shandalar_id = shandalar_row.get("id", "").strip()
        if magic_id != shandalar_id:
            raise ValueError(
                f"ID mismatch at row {row_index}: magic id {magic_id!r}, "
                f"shandalar id {shandalar_id!r}."
            )

        magic_ptr = normalize_pointer(magic_row.get("code_pointer", ""))
        shandalar_ptr = normalize_pointer(shandalar_row.get("code_pointer", ""))
        if not magic_ptr or not shandalar_ptr:
            raise ValueError(f"Missing code_pointer at row {row_index}, id {magic_id!r}.")

        magic_ptr_counts[magic_ptr] += 1
        shandalar_ptr_counts[shandalar_ptr] += 1

    for row_index, rows in enumerate(zip_longest(magic_rows, shandalar_rows), start=1):
        magic_row, shandalar_row = rows
        if magic_row is None or shandalar_row is None:
            raise ValueError(
                "Input CSV row counts do not match while zipping: "
                f"mismatch at row {row_index}."
            )

        magic_id = magic_row.get("id", "").strip()
        shandalar_id = shandalar_row.get("id", "").strip()
        if magic_id != shandalar_id:
            raise ValueError(
                f"ID mismatch at row {row_index}: magic id {magic_id!r}, "
                f"shandalar id {shandalar_id!r}."
            )

        magic_ptr = normalize_pointer(magic_row.get("code_pointer", ""))
        shandalar_ptr = normalize_pointer(shandalar_row.get("code_pointer", ""))
        if not magic_ptr or not shandalar_ptr:
            raise ValueError(f"Missing code_pointer at row {row_index}, id {magic_id!r}.")

        if magic_ptr == dummy_magic_ptr and shandalar_ptr == dummy_shandalar_ptr:
            if emitted_dummy_none:
                continue
            emitted_dummy_none = True
            suffix = "dummy"
        elif magic_ptr_counts[magic_ptr] > 1 or shandalar_ptr_counts[shandalar_ptr] > 1:
            skipped_conflict_count += 1
            continue
        else:
            card_name = cards_name_by_id.get(magic_id, "").strip()
            if card_name.lower() == "none":
                card_name = ""
            if not card_name:
                card_name = magic_row.get("name", "").strip()
                if not card_name:
                    raise ValueError(
                        f"Could not find card name in Cards.csv for id {magic_id!r}, "
                        "and fallback dump name is missing."
                    )
                fallback_name_count += 1

            suffix = sanitize_name(card_name)

        if suffix in used_names:
            used_names[suffix] += 1
            suffix = f"{suffix}_{used_names[suffix]}"
        else:
            used_names[suffix] = 0

        stubs.append(
            "\n".join(
                (
                    f"// FUNCTION: MAGIC {magic_ptr}",
                    f"// FUNCTION: SHANDALAR {shandalar_ptr}",
                    f"int card_{suffix}(int player, int card, event_t event)",
                    "{",
                    "",
                    "}",
                )
            )
        )

    return stubs, fallback_name_count, skipped_conflict_count


def main() -> int:
    args = parse_args()

    magic_path = Path(args.magic_csv)
    shandalar_path = Path(args.shandalar_csv)
    cards_path = Path(args.cards_csv)
    output_path = Path(args.output)

    magic_rows = read_csv_rows(magic_path)
    shandalar_rows = read_csv_rows(shandalar_path)
    cards_rows = read_csv_rows(cards_path)

    cards_name_by_id = build_cards_name_lookup(cards_rows)
    stubs, fallback_name_count, skipped_conflict_count = generate_stubs(
        magic_rows, shandalar_rows, cards_name_by_id
    )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n\n".join(stubs) + "\n", encoding="utf-8")
    print(f"Wrote {len(stubs)} stubs to {output_path}")
    if skipped_conflict_count:
        print(
            "Skipped "
            f"{skipped_conflict_count} rows due to conflicting reused code pointers."
        )
    if fallback_name_count:
        print(
            "Used fallback names from card_data CSV for "
            f"{fallback_name_count} rows with missing/None names in Cards.csv."
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
