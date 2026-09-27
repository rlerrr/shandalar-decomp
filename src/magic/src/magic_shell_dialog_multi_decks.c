#include "magic_shell_network_match.h"
#include "magic_shell_dialogs.h"
#include "global_state.h"
#include "global_strings.h"
#include "shared_startup.h"
#include <stdio.h>
#include <string.h>
#include <direct.h>

extern HINSTANCE g_app_instance;
extern char global_base_directory[];
int IsCardAvailable(int card_id, int expansion);
int check_basic(int card_id);
int check_restricted(int card_id);
int check_banned(int card_id);
int check_ante(int card_id);

// FUNCTION: MAGIC 0x004e03c0
int shell_classify_multiplayer_deck(char *filename, int requested_type, int ante, int minimum_size)
{
  struct
  {
    int line_count;
    int singleton;
    int card_id;
    int quantity;
    int deck_size;
    int valid;
    int deck_type;
    char line[500];
    FILE *file;
    int scan_result;
  } s;
  s.singleton = 1;
  s.valid = 1;
  s.line_count = 0;
  s.deck_size = 0;
  s.card_id = s.quantity = -1;
  s.deck_type = 99;
  s.file = fopen(filename, "rt");
  if (s.file == (FILE *)0x0)
  {
    return -1;
  }
  s.scan_result = fscanf(s.file, "%[^\n]", s.line);
  s.scan_result = fscanf(s.file, "%[\n]", s.line);
  do
  {
    s.scan_result = fscanf(s.file, "%[^\n]", s.line);
    if (s.line[0] == '.')
    {
      if (s.line[1] != 'v')
      {
        sscanf((s.line + 1), "%d %d", &s.card_id, &s.quantity);
        if (((IsCardAvailable(s.card_id, 0) == 0) && (IsCardAvailable(s.card_id, 1) == 0)) &&
            (IsCardAvailable(s.card_id, 2) == 0))
        {
          s.valid = -1;
        }
        if ((check_basic(s.card_id) == 0) && (0 < s.deck_type))
        {
          if (1 < s.quantity)
          {
            s.singleton = 0;
          }
          if (s.quantity > 4)
          {
            s.deck_type = 0;
          }
          else
          {
            if ((check_restricted(s.card_id) == 0) && (check_banned(s.card_id) == 0))
            {
              if (s.quantity <= 4)
              {
                if (s.deck_type >= 3)
                {
                  s.deck_type = 3;
                }
              }
              else if (4 < s.quantity)
              {
                s.deck_type = 0;
              }
            }
            else
            {
              if (check_banned(s.card_id) == 0)
              {
                if (check_restricted(s.card_id) != 0)
                {
                  if (s.quantity == 1)
                  {
                    if (s.deck_type >= 2)
                    {
                      s.deck_type = 2;
                    }
                  }
                  else if (s.quantity <= 4)
                  {
                    if (s.deck_type >= 1)
                    {
                      s.deck_type = 1;
                    }
                  }
                  else if (4 < s.quantity)
                  {
                    s.deck_type = 0;
                  }
                }
              }
              else
              {
                s.deck_type = 0;
              }
            }
          }
        }
        s.deck_size += s.quantity;
        if ((requested_type != 4) && (s.deck_type < requested_type))
        {
          s.valid = -1;
        }
        if ((ante == 0) && (check_ante(s.card_id) != 0))
        {
          s.valid = -1;
        }
      }
      s.line_count++;
    }
    s.scan_result = fscanf(s.file, "%[\n]", s.line);
  } while ((((s.line_count < 200) && (s.scan_result != -1)) && ((s.card_id != 0 || (s.quantity != 0)))) &&
      (s.valid != -1));
  fclose(s.file);
  if (requested_type == 4)
  {
    if (s.singleton)
    {
      s.deck_type = 4;
    }
    else
    {
      s.valid = -1;
    }
  }
  if (s.deck_type == 99)
  {
    s.deck_type = 0;
  }
  if (s.deck_size < minimum_size)
  {
    s.valid = -1;
  }
  if (s.valid == -1)
  {
    return s.valid;
  }
  return s.deck_type;


}

// FUNCTION: MAGIC 0x004dfbe6
LRESULT shell_populate_multiplayer_decks(HWND hwnd, int control_id, int deck_type, int ante, int minimum_size)
{
  struct
  {
    LRESULT deck_count;
    char deck_path[264];
    char filename[264];
    char type_names[100];
    HWND listbox;
    char previous_directory[264];
    char deck_name[100];
    WPARAM deck_index;
    int deck_type;
    FILE *file;
    char display_name[100];
    char *character;
  } s;
  s.deck_count = load_text("MP_UIStrings.txt", "DECKTYPES");
  for (s.deck_index = 0; (int)s.deck_index < s.deck_count; s.deck_index++)
  {
    strcpy(s.type_names + s.deck_index * 20, g_text_lines[s.deck_index]);
  }
  s.listbox = CreateWindowExA(0, "LISTBOX", "", 0x40a00003, 0, 0, 0, 0, hwnd, NULL, g_app_instance, NULL);
  if (s.listbox == NULL)
  {
    return -1;
  }
  GetCurrentDirectoryA(0x104, s.previous_directory);
  _chdir(global_base_directory);
  strcpy(s.filename, "playdeck\\*.DCK");
  SendMessageA(s.listbox, 0x18d, 0, (LPARAM)s.filename);
  s.deck_count = SendMessageA(s.listbox, 0x18b, 0, 0);
  for (s.deck_index = 0; (int)s.deck_index < s.deck_count; s.deck_index++)
  {
    SendMessageA(s.listbox, 0x189, s.deck_index, (LPARAM)s.filename);
    strcpy(s.deck_path, "playdeck\\");
    strcat(s.deck_path, s.filename);
    s.deck_type = shell_classify_multiplayer_deck(s.deck_path, deck_type, ante, minimum_size);
    if (s.deck_type == -1)
      continue;
    s.file = fopen(s.deck_path, "r");
    if (s.file != NULL)
    {
      s.deck_name[0] = '\0';
      s.character = s.deck_name;
      while ((*s.character = (char)fgetc(s.file)) != '\n')
      {
        if (*s.character != ';')
        {
          s.character++;
        }
      }
      *s.character = '\0';
      strcpy(s.display_name, s.deck_name);
      strcat(s.display_name, ".    (");
      strcat(s.display_name, s.type_names + s.deck_type * 20);
      strcat(s.display_name, ")");
      fclose(s.file);
      strcpy(s.deck_path, "playdeck\\");
      strcat(s.deck_path, s.deck_name);
      strcat(s.deck_path, ".dck");
      s.file = fopen(s.deck_path, "r");
      if (s.file != NULL)
      {
        if (control_id != -1)
        {
          SendDlgItemMessageA(hwnd, control_id, 0x143, 0, (LPARAM)s.display_name);
        }
        fclose(s.file);
      }
    }
  }
  s.deck_count = 0;
  if (control_id != -1)
  {
    s.deck_count = SendDlgItemMessageA(hwnd, control_id, 0x146, 0, 0);
  }
  _chdir(s.previous_directory);
  DestroyWindow(s.listbox);

  return s.deck_count;
}
