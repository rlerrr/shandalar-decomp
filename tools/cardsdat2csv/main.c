/*
 * Dump Cards.dat (as parsed by deckdll's read_db_guts()) to CSV.
 *
 * This intentionally mirrors the on-load fixups in:
 *   C:\dev\shandalar-decomp\src\deckdll\src\deckdll.c : read_db_guts()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strcmpi _strcmpi
#endif

static const char *const_db_artist_names[] = {
    "None",
    "Amy Weber",
    "Andi Rusu",
    "Anson Maddocks",
    "Anthony Waters",
    "Brian Snoddy",
    "Bryon Wackwitz",
    "Christopher Rush",
    "Cornelius Brudi",
    "Dameon Willich",
    "Dan Frazier",
    "Daniel Gelon",
    "Dennis Detwiller",
    "Douglas Shuler",
    "Drew Tucker",
    "Edward Beard Jr.",
    "Fay Jones",
    "Frank Frazier",
    "Harold McNeill",
    "Heather Hudson",
    "Jeff A. Menges",
    "Jesper Myrfors",
    "Julie Baroh",
    "Justin Hampton",
    "Kaja & Phil Foglio",
    "Kaja Foglio",
    "Ken Meyer Jr.",
    "Kerstin Kaman",
    "Kev Brockschmidt",
    "Kristen Bishop",
    "Liz Danforth",
    "Margaret Organ-Kean",
    "Mark Poole",
    "Mark Tedin",
    "Melissa Benson",
    "Michael Whelan",
    "Mike Kimble",
    "Nicola Leonard",
    "Nene Thomas",
    "Pat Morrissey",
    "Pete Venters",
    "Phil Foglio",
    "Quinton Hoover",
    "Randy Asplund-Faith",
    "Richard Kane-Ferguson",
    "Richard Thomas",
    "Rob Alexander",
    "Ron Spencer",
    "Sandra Everingham",
    "Scott Kirschner",
    "Susan Van Camp",
    "Tom Wanerstrand",
    "Unknown",
};

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

static void
csv_write_u32(FILE *out, unsigned long v)
{
  fprintf(out, "%lu", v);
}

static void
csv_write_s32(FILE *out, long v)
{
  fprintf(out, "%ld", v);
}

static void
csv_write_hex_u32(FILE *out, unsigned long v)
{
  fprintf(out, "0x%08lX", v);
}

static void
fixup_base_txt_ptr(const void *field_ptr, char *base_txt)
{
  /* Matches: *(int *)&field += (int)base_txt; */
  *(int *)field_ptr += (int)base_txt;
}

static void
apply_read_db_guts_fixups(card_ptr_t *cards, int num_cards, char *base_txt)
{
  int i;

  for (i = 0; i < num_cards; ++i)
  {
    fixup_base_txt_ptr(&cards[i].full_name, base_txt);
    fixup_base_txt_ptr(&cards[i].name, base_txt);
    fixup_base_txt_ptr(&cards[i].type_text, base_txt);
    fixup_base_txt_ptr(&cards[i].rules_text, base_txt);
    fixup_base_txt_ptr(&cards[i].flavor_text, base_txt);

    {
      int artist_idx = (int)cards[i].artist;
      int max_idx = (int)(sizeof(const_db_artist_names) / sizeof(const_db_artist_names[0]));
      if (artist_idx < 0 || artist_idx >= max_idx)
        cards[i].artist = "Unknown";
      else
        cards[i].artist = const_db_artist_names[artist_idx];
    }
  }
}

static void
csv_write_card(FILE *out, int csvid, const card_ptr_t *c)
{
  int j;
  char abilities_hex[64];
  char *w;

  /* abilities[10] -> hex bytes without separators */
  w = abilities_hex;
  for (j = 0; j < 10; ++j)
  {
    sprintf(w, "%02X", (unsigned char)c->abilities[j]);
    w += 2;
  }
  *w = '\0';

  csv_write_s32(out, (long)csvid);
  fputc(',', out);

  csv_write_s32(out, (unsigned long)c->id);
  fputc(',', out);

  csv_write_escaped(out, c->full_name);
  fputc(',', out);
  csv_write_escaped(out, c->name);
  fputc(',', out);
  csv_write_hex_u32(out, (unsigned long)c->expansion);
  fputc(',', out);
  csv_write_hex_u32(out, (unsigned long)c->color);
  fputc(',', out);
  csv_write_hex_u32(out, (unsigned long)c->card_type);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->subtype);
  fputc(',', out);
  csv_write_escaped(out, c->type_text);
  fputc(',', out);
  csv_write_hex_u32(out, (unsigned long)c->db_card_type_2);
  fputc(',', out);
  csv_write_hex_u32(out, (unsigned long)c->rarity);
  fputc(',', out);

  /* casting_cost_t */
  csv_write_u32(out, (unsigned long)c->req.req_colorless);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->req.req_black);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->req.req_blue);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->req.req_hybrid);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->req.hybrid_type);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->req.req_green);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->req.unknown0x2e);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->req.req_red);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->req.req_white);
  fputc(',', out);

  csv_write_u32(out, (unsigned long)c->unused0x31);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->unused0x32);
  fputc(',', out);
  csv_write_hex_u32(out, (unsigned long)c->flags);
  fputc(',', out);
  csv_write_hex_u32(out, (unsigned long)c->unused0x38);
  fputc(',', out);
  csv_write_hex_u32(out, (unsigned long)c->unused0x3c);
  fputc(',', out);

  csv_write_escaped(out, c->artist);
  fputc(',', out);
  csv_write_s32(out, (long)c->num_pics);
  fputc(',', out);

  /* This field isn't fixed up by read_db_guts(); dump as raw 32-bit. */
  csv_write_hex_u32(out, (unsigned long)c->mana_cost_text);
  fputc(',', out);

  csv_write_hex_u32(out, (unsigned long)c->ai_modifiers1);
  fputc(',', out);
  csv_write_hex_u32(out, (unsigned long)c->ai_modifiers2);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)BYTE0(c->ai_inc_power_toughness));
  fputc(',', out);
  csv_write_u32(out, (unsigned long)BYTE1(c->ai_inc_power_toughness));
  fputc(',', out);
  csv_write_u32(out, (unsigned long)BYTE0(c->ai_power_toughness));
  fputc(',', out);
  csv_write_u32(out, (unsigned long)BYTE1(c->ai_power_toughness));
  fputc(',', out);
  csv_write_s32(out, (long)c->ai_base_value);
  fputc(',', out);
  csv_write_s32(out, (long)c->ai_dependencies);
  fputc(',', out);
  csv_write_s32(out, (long)c->sleight_color);
  fputc(',', out);
  csv_write_s32(out, (long)c->ai_against_color);
  fputc(',', out);
  csv_write_s32(out, (long)c->ai_for_color);
  fputc(',', out);
  csv_write_s32(out, (long)c->ai_counts_as_color);
  fputc(',', out);
  csv_write_s32(out, (long)c->ai_against_land);
  fputc(',', out);
  csv_write_s32(out, (long)c->ai_for_land);
  fputc(',', out);
  csv_write_s32(out, (long)c->ai_counts_as_land);
  fputc(',', out);
  csv_write_s32(out, (long)c->ai_abilities);
  fputc(',', out);
  csv_write_s32(out, (long)c->expansion_rarity);
  fputc(',', out);

  csv_write_escaped(out, c->rules_text);
  fputc(',', out);
  csv_write_escaped(out, c->flavor_text);
  fputc(',', out);

  csv_write_s32(out, (long)c->power);
  fputc(',', out);
  csv_write_s32(out, (long)c->toughness);
  fputc(',', out);
  csv_write_s32(out, (long)c->types[0]);
  fputc(',', out);
  csv_write_s32(out, (long)c->types[1]);
  fputc(',', out);

  csv_write_escaped(out, abilities_hex);
  fputc(',', out);

  csv_write_s32(out, (long)c->mana_source_colors);
  fputc(',', out);
  csv_write_s32(out, (long)c->inflatable);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->hack_colors);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->currently_zero_for_all_cards);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->enchant_type);
  fputc(',', out);
  csv_write_u32(out, (unsigned long)c->hack_mode);

  fputc('\n', out);
}

static void
csv_write_header(FILE *out)
{
  fputs("csvid,id,full_name,name,expansion,color,card_type,subtype,type_text,db_card_type_2,rarity,"
        "req_colorless,req_black,req_blue,req_hybrid,hybrid_type,req_green,unknown0x2e,req_red,req_white,"
        "unused0x31,unused0x32,flags,unused0x38,unused0x3c,artist,num_pics,mana_cost_text_raw,"
        "ai_modifiers1,ai_modifiers2,ai_inc_power,ai_inc_toughness,ai_power,ai_toughness,ai_base_value,ai_dependencies,"
        "sleight_color,ai_against_color,ai_for_color,ai_counts_as_color,ai_against_land,ai_for_land,ai_counts_as_land,"
        "ai_abilities,expansion_rarity,rules_text,flavor_text,power,toughness,types0,types1,abilities_hex,"
        "mana_source_colors,inflatable,hack_colors,currently_zero_for_all_cards,enchant_type,hack_mode\n",
        out);
}

int
main(int argc, char **argv)
{
  const char *in_path;
  const char *out_path;
  FILE *in;
  FILE *out;
  int num_cards;
  size_t record_size;
  card_ptr_t *cards;
  char *base_txt;
  size_t want;

  in_path = "Cards.dat";
  out_path = "Cards.csv";
  if (argc >= 2)
    in_path = argv[1];
  if (argc >= 3)
    out_path = argv[2];

  in = fopen(in_path, "rb");
  if (!in)
  {
    fprintf(stderr, "Failed to open input: %s\n", in_path);
    return 1;
  }

  num_cards = 0;
  record_size = 0;
  if (fread(&num_cards, 4, 1, in) != 1 || fread(&record_size, 4, 1, in) != 1)
  {
    fprintf(stderr, "Failed to read header from: %s\n", in_path);
    fclose(in);
    return 1;
  }

  if (num_cards <= 0 || num_cards > 100000)
  {
    fprintf(stderr, "Unreasonable card count: %d\n", num_cards);
    fclose(in);
    return 1;
  }

  if (sizeof(card_ptr_t) != 0x98)
  {
    fprintf(stderr, "Unexpected sizeof(card_ptr_t)=%lu (expected 0x98). Build must be 32-bit.\n",
            (unsigned long)sizeof(card_ptr_t));
    fclose(in);
    return 1;
  }

  cards = (card_ptr_t *)malloc((size_t)num_cards * sizeof(card_ptr_t));
  if (!cards)
  {
    fprintf(stderr, "Out of memory allocating cards array\n");
    fclose(in);
    return 1;
  }

  want = (size_t)num_cards * sizeof(card_ptr_t);
  if (fread(cards, 1, want, in) != want)
  {
    fprintf(stderr, "Failed to read card records\n");
    free(cards);
    fclose(in);
    return 1;
  }

  base_txt = (char *)malloc(record_size);
  if (!base_txt)
  {
    fprintf(stderr, "Out of memory allocating base text (%lu bytes)\n", (unsigned long)record_size);
    free(cards);
    fclose(in);
    return 1;
  }

  if (fread(base_txt, 1, record_size, in) != record_size)
  {
    fprintf(stderr, "Failed to read base text blob\n");
    free(base_txt);
    free(cards);
    fclose(in);
    return 1;
  }

  fclose(in);

  apply_read_db_guts_fixups(cards, num_cards, base_txt);

  out = fopen(out_path, "wb");
  if (!out)
  {
    fprintf(stderr, "Failed to open output: %s\n", out_path);
    free(base_txt);
    free(cards);
    return 1;
  }

  csv_write_header(out);
  {
    int i;
    for (i = 0; i < num_cards; ++i)
      csv_write_card(out, i, &cards[i]);
  }

  fclose(out);
  free(base_txt);
  free(cards);
  return 0;
}
