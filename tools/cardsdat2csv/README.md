# `cardsdat_dump`

Reads a `Cards.dat` file using the same parse logic as
`read_db_guts()` in `src/deckdll/src/deckdll.c`, then writes a CSV file.

Usage:

```powershell
.\build\tools\cardsdat_dump.exe [input_cards_dat] [output_csv]
```

Parameters:

- `input_cards_dat` (optional): Path to `Cards.dat`.
  - Default: `Cards.dat` (current working directory)
- `output_csv` (optional): Path to output CSV file.