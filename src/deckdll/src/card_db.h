#include "defs.h"

typedef struct
{
  char *damage_text;
  char *effect_title;
  char *effect_text;
  char *legacy_title;
  char *legacy_text;
} name_table_entry_t;
STATIC_ASSERT(sizeof(name_table_entry_t) == 0x14, name_table_entry_t_wrong_size);

extern name_table_entry_t global_legacy_names[2000];
extern char *global_base_txt;
extern card_ptr_t global_raw_cards_storage[2000];

void FreeRaritiesCsvRaw(void);
int ReadLegacyCsv(const char *filename);
