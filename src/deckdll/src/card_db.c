#include "defs.h"
#include "deckdll.h"
#include "card_db.h"
#include <WINDOWS.H>
#include <STDIO.H>

char *CsvParseNextField(char **txt);

// GLOBAL: DECKDLL 0x100355d8
// GLOBAL: MAGIC 0x0056eab4
// GLOBAL: SHANDALAR 0x008e4514
static char read_db_empty_rules[1];

// GLOBAL: DECKDLL 0x100355ec
// GLOBAL: MAGIC 0x0056eac8
// GLOBAL: SHANDALAR 0x008e4518
static char read_db_empty_flavor[1];

// GLOBAL: DECKDLL 0x10033e48
// GLOBAL: MAGIC 0x57b668
// GLOBAL: SHANDALAR 0x005843f0
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
    "Edward Beard\xB8 Jr.",
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
    "N\xE8N\xE8 Thomas",
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
    "Tom W\xE4nerstrand",
    "Unknown",
};

// GLOBAL: DECKDLL 0x101427b4
// GLOBAL: MAGIC 0x8b40d0
// GLOBAL: SHANDALAR 0x008c8250
int global_available_slots;

// GLOBAL: DECKDLL 0x10147220
// GLOBAL: MAGIC 0x8d0350
// GLOBAL: SHANDALAR 0x008e44a0
card_ptr_t global_raw_cards_storage[2000];

// GLOBAL: SHANDALAR 0x007a82e4
// GLOBAL: MAGIC 0x00791558
// GLOBAL: DECKDLL 0x1012d7d0
static char *global_rarities_csv_raw;

// GLOBAL: DECKDLL 0x1012df3c
// GLOBAL: MAGIC 0x7a7d70
// GLOBAL: SHANDALAR 0x007beafc
char *global_base_txt;

// GLOBAL: MAGIC 0x00777e60
// GLOBAL: SHANDALAR 0x0078ebf0
name_table_entry_t unk_00777e60[866];

// FUNCTION: DECKDLL 0x1001a940
// FUNCTION: MAGIC 0x452cf0
// FUNCTION: SHANDALAR 0x004424b0
int read_db_guts(char *cards_dat_filename)
{
  struct
  {
    int init_idx;               /* local_808 */
    int flavor_out_idx;         /* local_804 */
    int flavor_card_idx;        /* local_800 */
    char flavor_text_buf[1000]; /* local_7fc */
    char *flavor_in;            /* local_414 */
    int rules_out_idx;          /* local_410 */
    int rules_card_idx;         /* local_40c */
    char rules_text_buf[1000];  /* local_408 */
    char *rules_in;             /* local_20 */
    int req_idx;                /* local_1c */
    int exp_idx;                /* local_18 */
    card_ptr_t *cp;
    size_t record_size;
    int card_idx; /* local_c */
    FILE *cards_dat;
  } s;

  s.cards_dat = fopen(cards_dat_filename, "rb");
  if (s.cards_dat == NULL)
    return 0;

  fread(&global_available_slots, 4, 1, s.cards_dat);
  fread(&s.record_size, 4, 1, s.cards_dat);

  global_base_txt = (char *)malloc(s.record_size);
  if (global_base_txt == NULL)
  {
    fclose(s.cards_dat);
    return 0;
  }

  fread(global_raw_cards_storage, 0x98, global_available_slots, s.cards_dat);
  fread(global_base_txt, 1, s.record_size, s.cards_dat);
  fclose(s.cards_dat);

  for (s.card_idx = 0; s.card_idx < global_available_slots; s.card_idx += 1)
  {
    *(int *)&global_raw_cards_storage[s.card_idx].full_name += (int)global_base_txt;
    *(int *)&global_raw_cards_storage[s.card_idx].name += (int)global_base_txt;
    *(int *)&global_raw_cards_storage[s.card_idx].type_text += (int)global_base_txt;
    *(int *)&global_raw_cards_storage[s.card_idx].rules_text += (int)global_base_txt;
    *(int *)&global_raw_cards_storage[s.card_idx].flavor_text += (int)global_base_txt;

    if (_strcmpi(global_raw_cards_storage[s.card_idx].rules_text, "None") == 0)
      global_raw_cards_storage[s.card_idx].rules_text = read_db_empty_rules;

    if ((_strcmpi(global_raw_cards_storage[s.card_idx].flavor_text, "None") == 0) ||
        (_strcmpi(global_raw_cards_storage[s.card_idx].flavor_text, "Blank") == 0))
      global_raw_cards_storage[s.card_idx].flavor_text = read_db_empty_flavor;

    global_raw_cards_storage[s.card_idx].artist =
        const_db_artist_names[(int)global_raw_cards_storage[s.card_idx].artist];

    if (global_raw_cards_storage[s.card_idx].num_pics == 0)
      global_raw_cards_storage[s.card_idx].num_pics = 1;
  }

  for (s.exp_idx = 0; s.exp_idx < global_available_slots; s.exp_idx += 1)
    if (global_raw_cards_storage[s.exp_idx].expansion & 0x40)
      global_raw_cards_storage[s.exp_idx].expansion = 0x80;

  for (s.req_idx = 0; s.req_idx < global_available_slots; s.req_idx += 1)
    if ((signed char)global_raw_cards_storage[s.req_idx].req.req_colorless == 0x11)
      global_raw_cards_storage[s.req_idx].req.req_colorless = 10;

  for (s.rules_card_idx = 0; s.rules_card_idx < global_available_slots;
       s.rules_card_idx += 1)
  {
    s.rules_out_idx = 0;
    for (s.rules_in = global_raw_cards_storage[s.rules_card_idx].rules_text;
         *s.rules_in;)
    {
      if (((*s.rules_in == '\\') && (s.rules_in[1] == '\\')) ||
          ((*s.rules_in == '\\') && (s.rules_in[1] == 'n')))
      {
        s.rules_in += 1;
        s.rules_text_buf[s.rules_out_idx] = '\n';
        s.rules_out_idx += 1;
      }
      else
      {
        s.rules_text_buf[s.rules_out_idx] = *s.rules_in;
        s.rules_out_idx += 1;
      }

      s.rules_in += 1;
    }

    s.rules_text_buf[s.rules_out_idx] = '\0';
    strcpy(global_raw_cards_storage[s.rules_card_idx].rules_text, s.rules_text_buf);
  }

  for (s.flavor_card_idx = 0; s.flavor_card_idx < global_available_slots;
       s.flavor_card_idx += 1)
  {
    s.flavor_out_idx = 0;
    for (s.flavor_in = global_raw_cards_storage[s.flavor_card_idx].flavor_text;
         *s.flavor_in;)
    {
      if (((*s.flavor_in == '\\') && (s.flavor_in[1] == '\\')) ||
          ((*s.flavor_in == '\\') && (s.flavor_in[1] == 'n')))
      {
        s.flavor_in += 1;
        s.flavor_text_buf[s.flavor_out_idx] = '\n';
        s.flavor_out_idx += 1;
      }
      else
      {
        s.flavor_text_buf[s.flavor_out_idx] = *s.flavor_in;
        s.flavor_out_idx += 1;
      }

      s.flavor_in += 1;
    }

    s.flavor_text_buf[s.flavor_out_idx] = '\0';
    strcpy(global_raw_cards_storage[s.flavor_card_idx].flavor_text, s.flavor_text_buf);
  }

#define SET_HACK(idx, col) \
  *(int *)&global_raw_cards_storage[(idx)].hack_colors = (col)

  for (s.init_idx = 0; s.init_idx < global_available_slots; s.init_idx += 1)
  {
    global_raw_cards_storage[s.init_idx].sleight_color = 0;
    SET_HACK(s.init_idx, 0);
  }

  global_raw_cards_storage[0x227].sleight_color = 0x28;
  global_raw_cards_storage[8].sleight_color = 2;
  global_raw_cards_storage[0x10].sleight_color = 0x20;
  global_raw_cards_storage[0x13].sleight_color = 2;
  global_raw_cards_storage[0x16].sleight_color = 0x10;
  global_raw_cards_storage[0x17].sleight_color = 4;
  global_raw_cards_storage[0x20].sleight_color = 0x10;
  global_raw_cards_storage[0x21].sleight_color = 2;
  global_raw_cards_storage[0x22].sleight_color = 4;
  global_raw_cards_storage[0x23].sleight_color = 8;
  global_raw_cards_storage[0x24].sleight_color = 0x10;
  global_raw_cards_storage[0x25].sleight_color = 0x20;
  global_raw_cards_storage[0x33].sleight_color = 0x20;
  global_raw_cards_storage[0x34].sleight_color = 4;
  global_raw_cards_storage[0x3a].sleight_color = 8;
  global_raw_cards_storage[0x3b].sleight_color = 2;
  global_raw_cards_storage[0x51].sleight_color = 2;
  global_raw_cards_storage[100].sleight_color = 0x20;
  global_raw_cards_storage[0x69].sleight_color = 8;
  global_raw_cards_storage[0x7b].sleight_color = 0x10;
  global_raw_cards_storage[0x80].sleight_color = 0x20;
  global_raw_cards_storage[0x8e].sleight_color = 2;
  global_raw_cards_storage[0x8f].sleight_color = 8;
  global_raw_cards_storage[0x1b7].sleight_color = 4;
  global_raw_cards_storage[0x367].sleight_color = 2;
  global_raw_cards_storage[0xaf].sleight_color = 2;
  global_raw_cards_storage[0xc4].sleight_color = 0x20;
  global_raw_cards_storage[199].sleight_color = 4;
  global_raw_cards_storage[200].sleight_color = 0x10;
  global_raw_cards_storage[0x30d].sleight_color = 0x20;
  global_raw_cards_storage[0xee].sleight_color = 0x30;
  global_raw_cards_storage[0x16c].sleight_color = 4;
  global_raw_cards_storage[0xf2].sleight_color = 2;
  global_raw_cards_storage[0xf5].sleight_color = 4;
  global_raw_cards_storage[0xf6].sleight_color = 2;
  global_raw_cards_storage[0x353].sleight_color = 2;
  global_raw_cards_storage[0x11b].sleight_color = 2;
  global_raw_cards_storage[0x11c].sleight_color = 0x20;
  global_raw_cards_storage[0x120].sleight_color = 8;

  SET_HACK(0x125, 2);
  SET_HACK(7, 8);
  SET_HACK(0x18, 2);
  SET_HACK(0x1a, 0x10);
  SET_HACK(0x132, 0x10);
  SET_HACK(0x2d, 0x30);
  SET_HACK(0x4d, 2);
  SET_HACK(0x56, 0x20);
  SET_HACK(0x5f, 8);
  SET_HACK(0x66, 0x10);
  SET_HACK(0x3a9, 0x10);
  SET_HACK(0x1ab, 4);
  SET_HACK(0x7f, 4);
  SET_HACK(0x86, 2);
  SET_HACK(0x88, 2);
  SET_HACK(0x154, 4);
  SET_HACK(0x90, 8);
  SET_HACK(0x93, 8);
  SET_HACK(0x96, 4);
  SET_HACK(0x2cf, 2);
  SET_HACK(0x367, 2);
  SET_HACK(0xae, 2);
  SET_HACK(0xba, 4);
  SET_HACK(0xda, 4);
  SET_HACK(0x30e, 4);
  SET_HACK(0xde, 8);
  SET_HACK(900, 2);
  SET_HACK(0xfd, 4);
  SET_HACK(0x109, 0x10);
  SET_HACK(0x123, 2);

  global_raw_cards_storage[0x1c2].sleight_color = 0x10;
  global_raw_cards_storage[0x225].sleight_color = 2;
  global_raw_cards_storage[0xd2].sleight_color = 2;
  global_raw_cards_storage[0x60].sleight_color = 0x10;

  SET_HACK(0x196, 4);
  SET_HACK(0x1a0, 8);
  SET_HACK(0x1b5, 8);
  SET_HACK(0x1b8, 4);
  SET_HACK(0x1c5, 4);
  SET_HACK(0x1c7, 4);
  SET_HACK(9, 0x12);
  SET_HACK(0xc, 10);
  SET_HACK(0xbd, 0x30);
  SET_HACK(0xd4, 0x28);
  SET_HACK(0xd8, 0x22);
  SET_HACK(0xdb, 2);
  SET_HACK(0xf1, 0x18);
  SET_HACK(0xfc, 0xc);
  SET_HACK(0xfe, 0x24);
  SET_HACK(0x102, 6);
  SET_HACK(0x10a, 0x14);
  SET_HACK(0x60, 0x10);

  //For some reason NOT included in deck builder?
#ifndef DECKDLL
  global_raw_cards_storage[603].sleight_color = 0x10;
  global_raw_cards_storage[605].sleight_color = 0x10;
  global_raw_cards_storage[704].sleight_color = 0x10;
  global_raw_cards_storage[730].sleight_color = 0x20;
  global_raw_cards_storage[844].sleight_color = 2;
  global_raw_cards_storage[338].sleight_color = 0x10;
  global_raw_cards_storage[596].sleight_color = 2;
  global_raw_cards_storage[316].sleight_color = 2;
  global_raw_cards_storage[374].sleight_color = 0x20;
  global_raw_cards_storage[692].sleight_color = 0x20;
  global_raw_cards_storage[355].sleight_color = 4;
  global_raw_cards_storage[793].sleight_color = 4;
  global_raw_cards_storage[564].sleight_color = 0x20;
  global_raw_cards_storage[579].sleight_color = 0x30;
  global_raw_cards_storage[660].sleight_color = 0x12;
  global_raw_cards_storage[335].sleight_color = 8;
  global_raw_cards_storage[375].sleight_color = 0x20;
  global_raw_cards_storage[686].sleight_color = 0x10;
  global_raw_cards_storage[621].sleight_color = 0x10;
  global_raw_cards_storage[345].sleight_color = 0x20;

  SET_HACK(612, 4);
  SET_HACK(590, 8);
  SET_HACK(774, 0x20);
  SET_HACK(552, 8);
  SET_HACK(715, 0x10);
  SET_HACK(940, 4);
  SET_HACK(323, 8);
  SET_HACK(730, 0x10);
  SET_HACK(827, 4);
  SET_HACK(951, 4);
  SET_HACK(315, 0x10);
  SET_HACK(335, 8);
  SET_HACK(727, 2);
  SET_HACK(796, 0x20);
  SET_HACK(385, 4);
  SET_HACK(299, 0x10);
  SET_HACK(621, 4);
  SET_HACK(332, 0x10);
  SET_HACK(389, 0xa);
#endif

#undef SET_HACK

  return global_available_slots;
}

// FUNCTION: DECKDLL 0x1001b2a2
// FUNCTION: SHANDALAR 0x00442f9b
// FUNCTION: MAGIC 0x004537d8
int ReadLegacyCsv(const char *filename)
{
  struct
  {
    char *next;
    HANDLE file;
    int result;
    DWORD bytes_read;
    DWORD size;
    int card_index;
    char *line;
  } s;

  s.result = 0;

  for (s.card_index = 0; s.card_index < global_available_slots; ++s.card_index)
  {
    unk_00777e60[s.card_index].damage_text = "";
    unk_00777e60[s.card_index].effect_title = "";
    unk_00777e60[s.card_index].effect_text = "";
    unk_00777e60[s.card_index].legacy_title = "";
    unk_00777e60[s.card_index].legacy_text = "";
  }

  s.file = CreateFileA(filename,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                       NULL);
  if (s.file != INVALID_HANDLE_VALUE)
  {
    s.size = GetFileSize(s.file, NULL);
    global_rarities_csv_raw = malloc(s.size + 1);
    if (global_rarities_csv_raw != NULL)
    {
      ReadFile(s.file, global_rarities_csv_raw, s.size, &s.bytes_read, NULL);
      s.line = global_rarities_csv_raw;
      s.line = strchr(s.line, '\n') + 1;

      for (s.card_index = 0; s.card_index < global_available_slots; ++s.card_index)
      {
        s.next = s.line;

        s.next = CsvParseNextField(&s.line);
        s.line = s.next;

        s.next = CsvParseNextField(&s.line);
        s.line = s.next;

        s.next = CsvParseNextField(&s.line);
        unk_00777e60[s.card_index].damage_text = s.line;
        s.line = s.next;

        s.next = CsvParseNextField(&s.line);
        unk_00777e60[s.card_index].effect_title = s.line;
        s.line = s.next;

        s.next = CsvParseNextField(&s.line);
        unk_00777e60[s.card_index].effect_text = s.line;
        s.line = s.next;

        s.next = CsvParseNextField(&s.line);
        unk_00777e60[s.card_index].legacy_title = s.line;
        s.line = s.next;

        s.next = CsvParseNextField(&s.line);
        unk_00777e60[s.card_index].legacy_text = s.line;
        s.line = s.next;
      }

      s.result = 1;
    }

    CloseHandle(s.file);
  }

  return s.result;
}

// FUNCTION: SHANDALAR 0x00443209
// FUNCTION: DECKDLL 0x1001b510
// FUNCTION: MAGIC 0x00453a46
char *CsvParseNextField(char **txt)
{
  struct
  {
    char *cr;
    char *next;
    char *comma;
    char *p;
    char *field;
  } s;

  s.field = *txt;

  if (*s.field == 0x22)
  {
    s.field += 1;
    s.p = strchr(s.field, 0x22);
    *s.p = '\0';
    s.p++;
    if (*s.p == 0x2c)
      s.next = s.p + 1;
    else
      s.next = s.p + 2;
  }
  else
  {
    s.comma = strchr(s.field, 0x2c);
    s.cr = strchr(s.field, 0xd);
    if (s.comma != NULL && s.cr > s.comma)
    {
      *s.comma = '\0';
      s.next = s.comma + 1;
    }
    else
    {
      *s.cr = '\0';
      s.next = s.cr + 2;
    }
  }

  *txt = s.field;
  return s.next;
}

// FUNCTION: DECKDLL 0x1001b4df
// FUNCTION: MAGIC 0x00453a15
// FUNCTION: SHANDALAR 0x004431d8
void FreeRaritiesCsvRaw(void)
{
  if (global_rarities_csv_raw != NULL)
  {
    free(global_rarities_csv_raw);
  }
  global_rarities_csv_raw = NULL;
}

// FUNCTION: DECKDLL 0x1001b832
// FUNCTION: MAGIC 0x00453d68
int ReadDeckbuilderCsv(const char *filename, OrigRarities *orig_rarities)
{
  struct
  {
    HANDLE hfile;     /* ebp - 0x8c */
    int rval;         /* ebp - 0x88 */
    DWORD bytes_read; /* ebp - 0x84 */
    DWORD size;       /* ebp - 0x80 */
    char *next;       /* ebp - 0x7c */
    char set_id;      /* ebp - 0x78 */
    char pad_set_id[3];
    int i;      /* ebp - 0x74 */
    char *line; /* ebp - 0x70 */
  } s;

  char set_names[7][15] = {
      "Antiquities",
      "Arabian",
      "Astral",
      "Dark",
      "Legends",
      "Promo",
      "Unlimited",
  };

  s.rval = 0;
  for (s.i = 0; s.i < global_available_slots; s.i++)
  {
    orig_rarities[s.i].set = SET_INVALID;
  }

  s.hfile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  if (s.hfile != INVALID_HANDLE_VALUE)
  {
    s.size = GetFileSize(s.hfile, NULL);
    global_rarities_csv_raw = (char *)malloc(s.size + 1);
    if (global_rarities_csv_raw)
    {
      ReadFile(s.hfile, global_rarities_csv_raw, s.size, &s.bytes_read, NULL);
      s.line = global_rarities_csv_raw;
      s.line = strchr(s.line, '\n') + 1;
      s.line = strchr(s.line, '\n') + 1;

      for (s.i = 0; s.i < global_available_slots; s.i = s.i + 1)
      {
        s.next = s.line;
        s.next = CsvParseNextField(&s.line);
        s.line = s.next;
        s.next = CsvParseNextField(&s.line);
        s.line = s.next;
        s.next = CsvParseNextField(&s.line);

        for (s.set_id = '\0'; s.set_id < '\a'; s.set_id++)
        {
          if (!strcmp(set_names[s.set_id], s.line))
            break;
        }

        if (s.set_id != '\a')
        {
          orig_rarities[s.i].set = s.set_id;

          s.line = s.next;
          s.next = CsvParseNextField(&s.line);
          orig_rarities[s.i].rarity = *s.line;

          s.line = s.next;
          s.next = CsvParseNextField(&s.line);
          orig_rarities[s.i].exp_rarities[0] = *s.line;

          s.line = s.next;
          s.next = CsvParseNextField(&s.line);
          orig_rarities[s.i].exp_rarities[1] = *s.line;

          s.line = s.next;
          s.next = CsvParseNextField(&s.line);
          orig_rarities[s.i].exp_rarities[2] = *s.line;

          s.line = s.next;
          s.next = CsvParseNextField(&s.line);
          orig_rarities[s.i].exp_rarities[3] = *s.line;
        }
        else
        {
          orig_rarities[s.i].set = SET_INVALID;
          orig_rarities[s.i].exp_rarities[0] = '-';
          orig_rarities[s.i].exp_rarities[1] = '-';
          orig_rarities[s.i].exp_rarities[2] = '-';
          orig_rarities[s.i].exp_rarities[3] = '-';
        }

        s.line = s.next;
        s.line = strchr(s.line, '\n') + 1;
      }

      s.rval = 1;
    }

    CloseHandle(s.hfile);
  }

  return s.rval;
}
