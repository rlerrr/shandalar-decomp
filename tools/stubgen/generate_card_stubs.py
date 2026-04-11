#!/usr/bin/env python3
from __future__ import annotations

import argparse
import csv
import re
from itertools import zip_longest
from pathlib import Path


def parse_args() -> argparse.Namespace:
    script_dir = Path(__file__).resolve().parent
    tools_dir = script_dir.parent
    repo_dir = tools_dir.parent
    default_output_dir = repo_dir / "src" / "shandalar" / "src" / "cards"

    parser = argparse.ArgumentParser(
        description=(
            "Generate card stub functions from magic/shandalar card_data CSV dumps "
            "and split them by Original Set from Rarity.csv."
        )
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
        "--rarity-csv",
        default=r"C:\Magic\Program\Rarity.csv",
        help="Path to Rarity.csv",
    )
    parser.add_argument(
        "--output-dir",
        "--output",
        dest="output_dir",
        default=str(default_output_dir),
        help=(
            "Directory for generated .c files (one per set). "
            "Default: src/shandalar/src/cards"
        ),
    )
    parser.add_argument(
        "--output-h-dir",
        "--output-h",
        dest="output_h_dir",
        default=str(default_output_dir),
        help=(
            "Directory for generated .h files (one per set). "
            "Default: src/shandalar/src/cards"
        ),
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


def sanitize_filename(name: str) -> str:
    filename = re.sub(r"[^A-Za-z0-9]+", "_", name.strip().lower()).strip("_")
    if not filename:
        return "unknown_set"
    return filename


def normalize_card_id(card_id: str) -> str:
    card_id = card_id.strip()
    if not card_id:
        return ""
    if not card_id.isdigit():
        return card_id
    return str(int(card_id, 10))


def build_cards_name_lookup(cards_rows: list[dict[str, str]]) -> dict[str, str]:
    names_by_id: dict[str, str] = {}
    for row in cards_rows:
        card_id = normalize_card_id(row.get("id", ""))
        card_name = row.get("name", "").strip()
        if card_id and card_name and card_id not in names_by_id:
            names_by_id[card_id] = card_name
    return names_by_id


def build_rarity_set_lookup(rarity_rows: list[dict[str, str]]) -> dict[str, str]:
    set_by_id: dict[str, str] = {}

    for row_index, row in enumerate(rarity_rows, start=2):
        game_id = normalize_card_id(row.get("Game ID", ""))
        original_set = row.get("Original Set", "").strip()

        if not game_id:
            continue
        if not game_id.isdigit():
            continue
        if not original_set:
            raise ValueError(
                f"Missing Original Set in Rarity.csv at data row {row_index}, Game ID {game_id!r}."
            )
        if game_id not in set_by_id:
            set_by_id[game_id] = original_set

    return set_by_id


def normalize_pointer(pointer: str) -> str:
    return pointer.strip().lower()


def append_group_entry(
    stubs_by_set: dict[str, list[str]],
    prototypes_by_set: dict[str, list[str]],
    set_filenames: list[str],
    set_filename: str,
    prototype: str,
    stub: str,
) -> None:
    if set_filename not in stubs_by_set:
        stubs_by_set[set_filename] = []
        prototypes_by_set[set_filename] = []
        set_filenames.append(set_filename)

    prototypes_by_set[set_filename].append(prototype)
    stubs_by_set[set_filename].append(stub)


def generate_stubs(
    magic_rows: list[dict[str, str]],
    shandalar_rows: list[dict[str, str]],
    cards_name_by_id: dict[str, str],
    set_by_id: dict[str, str],
) -> tuple[
    dict[str, list[str]],
    dict[str, list[str]],
    list[str],
    int,
    int,
    int,
    int,
]:
    stubs_by_set: dict[str, list[str]] = {}
    prototypes_by_set: dict[str, list[str]] = {}
    set_filenames: list[str] = []
    used_names: dict[str, int] = {}
    fallback_name_count = 0
    reused_pair_count = 0
    reused_pair_name_conflict_count = 0
    magic_ptr_pair_conflict_count = 0
    shandalar_ptr_pair_conflict_count = 0
    dummy_magic_ptr = "0x0053aa50"
    dummy_shandalar_ptr = "0x00488ca0"

    pair_to_suffix: dict[tuple[str, str], str] = {}
    pair_to_candidate_suffix: dict[tuple[str, str], str] = {}
    pair_to_set_filename: dict[tuple[str, str], str] = {}
    ordered_pairs: list[tuple[str, str]] = []

    magic_ptr_to_shandalar_ptr: dict[str, str] = {}
    shandalar_ptr_to_magic_ptr: dict[str, str] = {}

    for row_index, rows in enumerate(zip_longest(magic_rows, shandalar_rows), start=1):
        magic_row, shandalar_row = rows
        if magic_row is None or shandalar_row is None:
            raise ValueError(
                "Input CSV row counts do not match while zipping: "
                f"mismatch at row {row_index}."
            )

        magic_id = normalize_card_id(magic_row.get("id", ""))
        shandalar_id = normalize_card_id(shandalar_row.get("id", ""))
        if magic_id != shandalar_id:
            raise ValueError(
                f"ID mismatch at row {row_index}: magic id {magic_id!r}, "
                f"shandalar id {shandalar_id!r}."
            )

        magic_ptr = normalize_pointer(magic_row.get("code_pointer", ""))
        shandalar_ptr = normalize_pointer(shandalar_row.get("code_pointer", ""))
        if not magic_ptr or not shandalar_ptr:
            raise ValueError(f"Missing code_pointer at row {row_index}, id {magic_id!r}.")

        pair = (magic_ptr, shandalar_ptr)
        original_set = set_by_id.get(magic_id)
        if original_set is None:
            if pair == (dummy_magic_ptr, dummy_shandalar_ptr):
                set_filename = "dummy"
            else:
                raise ValueError(f"Could not find Game ID {magic_id!r} in Rarity.csv.")
        else:
            set_filename = sanitize_filename(original_set)

        existing_shandalar_ptr = magic_ptr_to_shandalar_ptr.get(magic_ptr)
        if existing_shandalar_ptr is None:
            magic_ptr_to_shandalar_ptr[magic_ptr] = shandalar_ptr
        elif existing_shandalar_ptr != shandalar_ptr:
            magic_ptr_pair_conflict_count += 1

        existing_magic_ptr = shandalar_ptr_to_magic_ptr.get(shandalar_ptr)
        if existing_magic_ptr is None:
            shandalar_ptr_to_magic_ptr[shandalar_ptr] = magic_ptr
        elif existing_magic_ptr != magic_ptr:
            shandalar_ptr_pair_conflict_count += 1

        if magic_ptr == dummy_magic_ptr and shandalar_ptr == dummy_shandalar_ptr:
            candidate_suffix = "dummy"
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
            candidate_suffix = sanitize_name(card_name)

        if pair in pair_to_suffix:
            reused_pair_count += 1
            if pair_to_candidate_suffix.get(pair) != candidate_suffix:
                reused_pair_name_conflict_count += 1
            continue

        suffix = candidate_suffix
        pair_to_candidate_suffix[pair] = candidate_suffix
        pair_to_set_filename[pair] = set_filename

        if suffix in used_names:
            used_names[suffix] += 1
            suffix = f"{suffix}_{used_names[suffix]}"
        else:
            used_names[suffix] = 0

        pair_to_suffix[pair] = suffix
        ordered_pairs.append(pair)

    for magic_ptr, shandalar_ptr in ordered_pairs:
        pair = (magic_ptr, shandalar_ptr)
        suffix = pair_to_suffix[pair]
        set_filename = pair_to_set_filename[pair]
        prototype = f"int card_{suffix}(int player, int card, event_t event);"
        stub = "\n".join(
            (
                f"// FUNCTION: MAGIC {magic_ptr}",
                f"// FUNCTION: SHANDALAR {shandalar_ptr}",
                f"int card_{suffix}(int player, int card, event_t event)",
                "{",
                "",
                "}",
            )
        )
        append_group_entry(
            stubs_by_set,
            prototypes_by_set,
            set_filenames,
            set_filename,
            prototype,
            stub,
        )

    return (
        stubs_by_set,
        prototypes_by_set,
        set_filenames,
        fallback_name_count,
        reused_pair_count,
        reused_pair_name_conflict_count,
        magic_ptr_pair_conflict_count + shandalar_ptr_pair_conflict_count,
    )


def write_grouped_outputs(
    output_dir: Path,
    header_dir: Path,
    set_filenames: list[str],
    stubs_by_set: dict[str, list[str]],
    prototypes_by_set: dict[str, list[str]],
) -> tuple[int, int]:
    total_stub_count = 0
    total_prototype_count = 0

    output_dir.mkdir(parents=True, exist_ok=True)
    header_dir.mkdir(parents=True, exist_ok=True)

    for set_filename in set_filenames:
        stubs = stubs_by_set[set_filename]
        prototypes = prototypes_by_set[set_filename]
        c_path = output_dir / f"{set_filename}.c"
        h_path = header_dir / f"{set_filename}.h"

        c_path.write_text('#include "defs.h"\n\n' + "\n\n".join(stubs) + "\n", encoding="utf-8")
        h_path.write_text('#include "defs.h"\n' + "\n".join(prototypes) + "\n", encoding="utf-8")

        total_stub_count += len(stubs)
        total_prototype_count += len(prototypes)
        print(f"Wrote {len(stubs)} stubs to {c_path}")
        print(f"Wrote {len(prototypes)} prototypes to {h_path}")

    return total_stub_count, total_prototype_count


def main() -> int:
    args = parse_args()

    magic_path = Path(args.magic_csv)
    shandalar_path = Path(args.shandalar_csv)
    cards_path = Path(args.cards_csv)
    rarity_path = Path(args.rarity_csv)
    output_dir = Path(args.output_dir)
    header_dir = Path(args.output_h_dir)

    magic_rows = read_csv_rows(magic_path)
    shandalar_rows = read_csv_rows(shandalar_path)
    cards_rows = read_csv_rows(cards_path)
    rarity_rows = read_csv_rows(rarity_path)

    cards_name_by_id = build_cards_name_lookup(cards_rows)
    set_by_id = build_rarity_set_lookup(rarity_rows)
    (
        stubs_by_set,
        prototypes_by_set,
        set_filenames,
        fallback_name_count,
        reused_pair_count,
        reused_pair_name_conflict_count,
        ptr_pair_conflict_count,
    ) = generate_stubs(magic_rows, shandalar_rows, cards_name_by_id, set_by_id)

    total_stub_count, total_prototype_count = write_grouped_outputs(
        output_dir,
        header_dir,
        set_filenames,
        stubs_by_set,
        prototypes_by_set,
    )

    print(
        f"Generated {total_stub_count} stubs and {total_prototype_count} prototypes "
        f"across {len(set_filenames)} set files."
    )
    if reused_pair_count:
        print(f"Reused {reused_pair_count} duplicate code_pointer pairs (kept first name/set).")
    if reused_pair_name_conflict_count:
        print(
            "Saw "
            f"{reused_pair_name_conflict_count} duplicate pairs with conflicting names "
            "(kept first name)."
        )
    if ptr_pair_conflict_count:
        print(
            "Saw "
            f"{ptr_pair_conflict_count} pointer pairing mismatches between magic/shandalar "
            "(emitted stubs anyway)."
        )
    if fallback_name_count:
        print(
            "Used fallback names from card_data CSV for "
            f"{fallback_name_count} rows with missing/None names in Cards.csv."
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
