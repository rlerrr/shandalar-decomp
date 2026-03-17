/*
 * Dump card_data_t records from a PE image data section (e.g. magic.exe) to CSV.
 *
 * Usage:
 *   carddata_dump <magic.exe> <image_base> <card_data_va> <count> [out.csv]
 *
 * Example:
 *   carddata_dump magic.exe 0x00400000 0x00561960 722 cards_data.csv
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <windows.h>

#include "defs.h"

static int
parse_u32(const char *txt, unsigned long *out)
{
  char *endp;
  unsigned long v;

  if (txt == NULL || out == NULL)
    return 0;

  v = strtoul(txt, &endp, 0);
  if (endp == txt || *endp != '\0')
    return 0;

  *out = v;
  return 1;
}

static int
read_entire_file(const char *path, unsigned char **out_buf, unsigned long *out_size)
{
  FILE *f;
  long sz;
  unsigned char *buf;
  size_t got;

  *out_buf = NULL;
  *out_size = 0;

  f = fopen(path, "rb");
  if (!f)
    return 0;

  if (fseek(f, 0, SEEK_END) != 0)
  {
    fclose(f);
    return 0;
  }

  sz = ftell(f);
  if (sz <= 0)
  {
    fclose(f);
    return 0;
  }

  if (fseek(f, 0, SEEK_SET) != 0)
  {
    fclose(f);
    return 0;
  }

  buf = (unsigned char *)malloc((size_t)sz);
  if (!buf)
  {
    fclose(f);
    return 0;
  }

  got = fread(buf, 1, (size_t)sz, f);
  fclose(f);
  if (got != (size_t)sz)
  {
    free(buf);
    return 0;
  }

  *out_buf = buf;
  *out_size = (unsigned long)sz;
  return 1;
}

static int
rva_to_file_offset(const unsigned char *image, unsigned long image_size,
                   unsigned long rva, unsigned long *out_file_off)
{
  IMAGE_DOS_HEADER *dos;
  IMAGE_NT_HEADERS *nt;
  IMAGE_SECTION_HEADER *sec;
  unsigned int i;

  if (image_size < sizeof(IMAGE_DOS_HEADER))
    return 0;

  dos = (IMAGE_DOS_HEADER *)image;
  if (dos->e_magic != IMAGE_DOS_SIGNATURE)
    return 0;

  if ((unsigned long)dos->e_lfanew + sizeof(IMAGE_NT_HEADERS) > image_size)
    return 0;

  nt = (IMAGE_NT_HEADERS *)(image + dos->e_lfanew);
  if (nt->Signature != IMAGE_NT_SIGNATURE)
    return 0;

  if (rva < nt->OptionalHeader.SizeOfHeaders)
  {
    *out_file_off = rva;
    return (*out_file_off < image_size);
  }

  sec = IMAGE_FIRST_SECTION(nt);
  for (i = 0; i < nt->FileHeader.NumberOfSections; ++i)
  {
    unsigned long va = sec[i].VirtualAddress;
    unsigned long raw_size = sec[i].SizeOfRawData;
    unsigned long virt_size = sec[i].Misc.VirtualSize;
    unsigned long span = raw_size > virt_size ? raw_size : virt_size;

    if (rva >= va && rva < va + span)
    {
      unsigned long delta = rva - va;
      unsigned long file_off = sec[i].PointerToRawData + delta;
      if (delta >= raw_size)
        return 0;
      if (file_off >= image_size)
        return 0;
      *out_file_off = file_off;
      return 1;
    }
  }

  return 0;
}

static void
csv_write_escaped(FILE *out, const char *s)
{
  const char *p;
  int need_quotes;

  if (s == NULL)
    s = "";

  need_quotes = 0;
  for (p = s; *p; ++p)
  {
    if (*p == ',' || *p == '"' || *p == '\n' || *p == '\r')
    {
      need_quotes = 1;
      break;
    }
  }

  if (!need_quotes)
  {
    fputs(s, out);
    return;
  }

  fputc('"', out);
  for (p = s; *p; ++p)
  {
    if (*p == '"')
      fputc('"', out);
    fputc(*p, out);
  }
  fputc('"', out);
}

static unsigned long
read_code_pointer_raw(const card_data_t *cd)
{
  /* Dump function pointer as the raw 32-bit address stored in the PE image. */
  const unsigned char *p = (const unsigned char *)cd + offsetof(card_data_t, code_pointer);
  return (unsigned long)p[0] |
         ((unsigned long)p[1] << 8) |
         ((unsigned long)p[2] << 16) |
         ((unsigned long)p[3] << 24);
}

static void
write_header(FILE *out)
{
  fputs("index,secret,name,id,type,subtype,color,cc0,cc1,cc2,power,toughness,new_field,reserved3,"
        "code_pointer,static_ability,extra_ability,rarity,act_phases,expansion,creature_rating\n",
        out);
}

static void
write_row(FILE *out, int idx, const card_data_t *cd)
{
  fprintf(out, "%d,", idx);
  fprintf(out, "%u,", (unsigned)cd->secret);
  csv_write_escaped(out, cd->name);
  fprintf(out, ",%u,", (unsigned)cd->id);
  fprintf(out, "%u,%u,%u,", (unsigned)cd->type, (unsigned)cd->subtype, (unsigned)cd->color);
  fprintf(out, "%u,%u,%u,", (unsigned)cd->cc[0], (unsigned)cd->cc[1], (unsigned)cd->cc[2]);
  fprintf(out, "%d,%d,", (int)cd->power, (int)cd->toughness);
  fprintf(out, "%u,%u,", (unsigned)cd->new_field, (unsigned)cd->reserved3);
  fprintf(out, "0x%08lX,", read_code_pointer_raw(cd));
  fprintf(out, "0x%08lX,0x%08lX,", (unsigned long)cd->static_ability, (unsigned long)cd->extra_ability);
  fprintf(out, "%u,%u,%u,%u\n",
          (unsigned)cd->rarity, (unsigned)cd->act_phases, (unsigned)cd->expansion, (unsigned)cd->creature_rating);
}

int
main(int argc, char **argv)
{
  const char *exe_path;
  const char *out_path;
  unsigned long image_base;
  unsigned long card_data_va;
  unsigned long count;
  unsigned long card_data_rva;
  unsigned long file_off;
  unsigned char *image;
  unsigned long image_size;
  FILE *out;
  unsigned long i;

  if (argc < 5 || argc > 6)
  {
    fprintf(stderr, "Usage: %s <magic.exe> <image_base> <card_data_va> <count> [out.csv]\n", argv[0]);
    return 1;
  }

  exe_path = argv[1];
  out_path = (argc >= 6) ? argv[5] : "card_data_dump.csv";

  if (!parse_u32(argv[2], &image_base) || !parse_u32(argv[3], &card_data_va) || !parse_u32(argv[4], &count))
  {
    fprintf(stderr, "Failed to parse numeric args. Use decimal or 0xHEX.\n");
    return 1;
  }

  if (count == 0)
  {
    fprintf(stderr, "count must be > 0\n");
    return 1;
  }

  if (sizeof(card_data_t) != 72)
  {
    fprintf(stderr, "Unexpected sizeof(card_data_t)=%lu (expected 72)\n", (unsigned long)sizeof(card_data_t));
    return 1;
  }

  if (!read_entire_file(exe_path, &image, &image_size))
  {
    fprintf(stderr, "Failed to read input file: %s\n", exe_path);
    return 1;
  }

  if (card_data_va < image_base)
  {
    fprintf(stderr, "card_data_va (0x%08lX) must be >= image_base (0x%08lX)\n", card_data_va, image_base);
    free(image);
    return 1;
  }

  card_data_rva = card_data_va - image_base;
  if (!rva_to_file_offset(image, image_size, card_data_rva, &file_off))
  {
    fprintf(stderr, "Could not translate RVA 0x%08lX to file offset\n", card_data_rva);
    free(image);
    return 1;
  }

  if (file_off + count * sizeof(card_data_t) > image_size)
  {
    fprintf(stderr, "Requested range exceeds file size (offset=0x%08lX count=%lu size=%lu)\n",
            file_off, count, (unsigned long)sizeof(card_data_t));
    free(image);
    return 1;
  }

  out = fopen(out_path, "wb");
  if (!out)
  {
    fprintf(stderr, "Failed to open output file: %s\n", out_path);
    free(image);
    return 1;
  }

  write_header(out);
  for (i = 0; i < count; ++i)
  {
    const card_data_t *cd = (const card_data_t *)(image + file_off + i * sizeof(card_data_t));
    write_row(out, (int)i, cd);
  }

  fclose(out);
  free(image);
  return 0;
}
