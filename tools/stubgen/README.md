# `stubgen`

Generates split card stub `.c` and `.h` files with paired reccmp annotations from:

- `carddata_dump/magic_card_data.csv`
- `carddata_dump/shandalar_card_data.csv`
- `cardsdat2csv/Cards.csv`
- `C:\Magic\Program\Rarity.csv`

Behavior:

- Matches `Rarity.csv:Game ID` against the `id` column in both `_card_data.csv` files.
- Uses `Rarity.csv:Original Set` to choose the output filename.
- Normalizes set filenames to lowercase and replaces non-alphanumeric runs with `_`
  (example: `Arabian Nights` -> `arabian_nights.c` / `arabian_nights.h`).
- Emits one stub per unique (`magic code_pointer`, `shandalar code_pointer`) pair.
- When multiple cards reuse the same pair, reuses the first-seen stub name and first-seen set file.
- Collapses the repeated dummy pair (`0x0053AA50` / `0x00488CA0`) into a single
  `card_dummy` stub.
- Uses `Cards.csv:name` by `id`; if that value is missing or `None`, falls back
  to the `_card_data.csv` `name`.
- Converts card names to snake_case and preserves `FX` uppercase suffix tokens
  (example: `LivingLandFX` -> `card_living_land_FX`).

## Usage

From `tools/`:

```powershell
python .\stubgen\generate_card_stubs.py
```

Optional arguments:

```powershell
python .\stubgen\generate_card_stubs.py `
  --magic-csv .\carddata_dump\magic_card_data.csv `
  --shandalar-csv .\carddata_dump\shandalar_card_data.csv `
  --cards-csv .\cardsdat2csv\Cards.csv `
  --rarity-csv C:\Magic\Program\Rarity.csv `
  --output-dir C:\dev\shandalar-decomp\src\shandalar\src\cards `
  --output-h-dir C:\dev\shandalar-decomp\src\shandalar\src\cards
```

`--output` and `--output-h` are still accepted as aliases for the two directory
arguments.
