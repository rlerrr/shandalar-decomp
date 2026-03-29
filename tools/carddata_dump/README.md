# `carddata_dump`

Reads `card_data_t` defined in `src/defs.h` records directly from a PE image (for example `magic.exe`)
and writes a CSV dump. This is meant for data that lives in the executable's
mapped image, so it takes image-base and virtual-address inputs.

Usage:

```
carddata_dump.exe <magic_exe> <image_base> <card_data_va> <count> [output_csv]
```

Parameters:

- `magic_exe`: Path to `magic.exe` (or another compatible PE image).
- `image_base`: Preferred load base of the image, in decimal or hex (for
  example `0x00400000`).
- `card_data_va`: Virtual address of the first `card_data_t` record, in decimal
  or hex.
- `count`: Number of `card_data_t` entries to dump.
- `output_csv` (optional): Path to output CSV file.
  - Default: `card_data_dump.csv` (current working directory)

Example:

```
carddata_dump.exe C:\Magic\Program\magic.exe 0x00400000 0x00561960 722 magic_card_data.csv
carddata_dump.exe C:\Magic\Program\shandalar.exe 0x00400000 0x00594208 722 shandalar_card_data.csv
```
