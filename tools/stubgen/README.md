# `stubgen`

Generates a `.c` file containing card stub functions with paired reccmp
annotations from:

- `carddata_dump/magic_card_data.csv`
- `carddata_dump/shandalar_card_data.csv`
- `cardsdat2csv/Cards.csv`

Behavior:

- Emits one stub per unique (`magic code_pointer`, `shandalar code_pointer`) pair.
- When multiple cards reuse the same pair, reuses the first-seen stub name.
- Collapses the repeated dummy pair (`0x0053AA50` / `0x00488CA0`) into a single
  `card_dummy` stub.
- Uses `Cards.csv:name` by `id`; if that value is missing or `None`, falls back
  to the `_card_data.csv` `name`.
- Converts names to snake_case and preserves `FX` uppercase suffix tokens
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
  --output C:\dev\shandalar-decomp\src\cards\cards.c `
  --output-h C:\dev\shandalar-decomp\src\cards\cards.h
```
