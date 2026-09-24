#include "magic_shell_dialogs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_support.h"
#include "global_state.h"

// GLOBAL: MAGIC 0x0074aa40
static char shell_last_loaded_deck_path[264];

#define SHELL_DECK_ENTRY(index) \
  (((csvid_and_numcards *)library_buffer)[index])

// FUNCTION: MAGIC 0x004d9458
int shell_parse_duel_deck(char *filename, void *library_buffer,
                          unsigned int flags, int mode)
{
  struct
  {
    unsigned int entry_index;
    unsigned int last_land_index;
    int entry_metadata;
    int internal_card_id;
    int card_id;
    int quantity;
    int header_line_count;
    char *space;
    int total_cards;
    int required_mode;
    unsigned int required_flags;
    char line[500];
    int result;
    FILE *file;
    int scan_result;
  } s;
  strcpy(shell_last_loaded_deck_path, filename);
  s.file = fopen(filename, "rt");
  if (s.file == NULL)
    return 0;

    s.entry_index = s.header_line_count = s.result = 0;
    s.scan_result = fscanf(s.file, "%[^\n]", s.line);
    s.scan_result = fscanf(s.file, "%[\n]", s.line);
    s.required_flags = 0;
    s.required_mode = -1;
    s.last_land_index = (unsigned int)-1;
    s.entry_metadata = 0;
    s.total_cards = 0;
    s.quantity = -1;
    s.card_id = s.quantity;
    do
    {
      s.scan_result = fscanf(s.file, "%[^\n]", s.line);
      if (s.line[0] == '.')
      {
        if (s.line[1] != 'v')
        {
          sscanf(s.line + 1, "%d %d", &s.card_id, &s.quantity);
          s.total_cards += s.quantity;
          if ((s.required_flags == 0 ||
               (flags & s.required_flags) != 0) &&
              (s.required_mode == -1 || s.required_mode == mode))
          {
            SHELL_DECK_ENTRY(s.entry_index).csvid = s.card_id;
            SHELL_DECK_ENTRY(s.entry_index).numcards = s.quantity;
            s.internal_card_id = find_internal_card_id_by_csv_id(s.card_id);
            if (s.quantity == 0)
              s.quantity = g_shandalar_difficulty;
            if (s.internal_card_id != -1)
            {
              if (s.internal_card_id <= 4)
              {
                s.last_land_index = s.entry_index;
                if ((global_cards_data[s.internal_card_id].type &
                     TYPE_LAND) != 0)
                  s.entry_metadata += s.entry_index;
              }
            }
            else
            {
              if (s.last_land_index != (unsigned int)-1 &&
                  s.quantity < SHELL_DECK_ENTRY(s.last_land_index).numcards)
                SHELL_DECK_ENTRY(s.last_land_index).numcards -=
                    (int)((s.entry_index & 1) + s.quantity) / 2;
            }
          }
        }
        else
        {
          s.space = strchr(s.line, ' ');
          if (s.space != NULL)
            *s.space = '\0';
          if (_strcmpi(s.line, ".vNONE") == 0)
            s.required_flags = 1;
          if (_strcmpi(s.line, ".vBLACK") == 0)
            s.required_flags = 2;
          if (_strcmpi(s.line, ".vBLUE") == 0)
            s.required_flags = 4;
          if (_strcmpi(s.line, ".vRED") == 0)
            s.required_flags = 0x10;
          if (_strcmpi(s.line, ".vGREEN") == 0)
            s.required_flags = 8;
          if (_strcmpi(s.line, ".vWHITE") == 0)
            s.required_flags = 0x20;
          if (_strcmpi(s.line, ".vFAST") == 0)
            s.required_mode = 0;
          if (_strcmpi(s.line, ".vLARGE") == 0)
            s.required_mode = 1;
          if (_strcmpi(s.line, ".vDIRECT") == 0)
            s.required_mode = 2;
          if (_strcmpi(s.line, ".vARTIFACT") == 0)
            s.required_mode = 6;
        }
        ++s.entry_index;
      }
      else
      {
        ++s.header_line_count;
        if (s.header_line_count == 5)
          s.result = atoi(s.line + 1);
        else if (s.header_line_count == 6 &&
                 strcmp(s.line + 1, "4th Edition") != 0)
          s.result = -2;
      }
      s.scan_result = fscanf(s.file, "%[\n]", s.line);
    } while ((int)s.entry_index < 200 && s.total_cards < 500 &&
             s.scan_result != -1 &&
             (s.card_id != 0 || s.quantity != 0));
    fclose(s.file);
    if (s.total_cards < 40)
      s.result = -3;
    if ((s.total_cards >= 500 || (int)s.entry_index >= 200) &&
        s.scan_result != -1)
      s.result = -4;
    if (s.result > 0x37)
      s.result = -1;
  return s.result;
}

#undef SHELL_DECK_ENTRY
