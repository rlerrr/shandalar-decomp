#include <direct.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "defs.h"
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/card_db.h"
#include "deckdll/src/shared_resources.h"
#include "magic/src/global_strings.h"
#include "shared_startup.h"

extern int global_available_slots;
extern card_ptr_t global_raw_cards_storage[];
extern card_data_t global_cards_data[];

// GLOBAL: SHANDALAR 0x0093a230
// GLOBAL: MAGIC 0x00926100
char global_playdeck_path[300];

// GLOBAL: SHANDALAR 0x008c8660
// GLOBAL: MAGIC 0x008b44e0
char global_faces_path[300];

// GLOBAL: SHANDALAR 0x008e33e0
// GLOBAL: MAGIC 0x008cf290
char global_duelart_path[300];

// GLOBAL: SHANDALAR 0x008a92c0
// GLOBAL: MAGIC 0x008950c0
char global_duelsounds_path[300];

// GLOBAL: SHANDALAR 0x008bdaf0
// GLOBAL: MAGIC 0x008a98f0
char global_savegame_path[300];

// GLOBAL: SHANDALAR 0x007a00a0
// GLOBAL: MAGIC 0x00789310
HDC DAT_00789310;

// GLOBAL: SHANDALAR 0x0093a938
// GLOBAL: MAGIC 0x00926808
HBITMAP DAT_00926808;

// GLOBAL: SHANDALAR 0x0094eaa0
// GLOBAL: MAGIC 0x0093a980
HGDIOBJ DAT_0093a980;

// GLOBAL: SHANDALAR 0x008e30fc
// GLOBAL: MAGIC 0x008cefb0
void *DAT_008cefb0;

// GLOBAL: SHANDALAR 0x0094d460
// GLOBAL: MAGIC 0x00939334
int DAT_00939334;

// GLOBAL: SHANDALAR 0x006531a0
// GLOBAL: MAGIC 0x00638ca8
int DAT_00638ca8;

// GLOBAL: SHANDALAR 0x0073ea90
// GLOBAL: MAGIC 0x00776518
HANDLE DAT_00776518;

// GLOBAL: SHANDALAR 0x0078df78
// GLOBAL: MAGIC 0x00715fa0
int DAT_00715fa0;

// GLOBAL: SHANDALAR 0x0093aa40
// GLOBAL: MAGIC 0x00926910
CRITICAL_SECTION DAT_00926910;

// GLOBAL: SHANDALAR 0x007beb08
// GLOBAL: MAGIC 0x007a7d7c
int DAT_007a7d7c;

// GLOBAL: SHANDALAR 0x0093a7e0
// GLOBAL: MAGIC 0x009266b0
CRITICAL_SECTION DAT_009266b0;

// GLOBAL: SHANDALAR 0x005a8b30
// GLOBAL: MAGIC 0x00637a94
int DAT_00637a94;

static __inline void append_startup_error(char *message_buffer, const char *path, int line_index)
{
  sprintf(message_buffer + strlen(message_buffer), text_lines[line_index], path);
  strcat(message_buffer, "\n");
}

// FUNCTION: MAGIC 0x004a59ad
// FUNCTION: SHANDALAR 0x00557b2d
int CardIDFromType(unsigned int type)
{
  if (type == -1)
  {
    return -1;
  }
  else
  {
    type &= 0xfff;
    return global_cards_data[type].id;
  }
}

// FUNCTION: MAGIC 0x004a5929
// FUNCTION: SHANDALAR 0x00557aa9
int CardTypeFromID(int csvid)
{
  int result;
  int internal_card_id;

  if (csvid == -1)
  {
    return -1;
  }
  else
  {
    result = -1;
    for (internal_card_id = 0; global_cards_data[internal_card_id].id != -1 && result == -1; ++internal_card_id)
    {
      if (global_cards_data[internal_card_id].id == csvid)
      {
        result = internal_card_id;
      }
    }
    return result;
  }
}

// FUNCTION: MAGIC 0x004a59ea
// FUNCTION: SHANDALAR 0x00557b6a
int CardInDeck(int param_1)
{
  if (param_1 == -1) {
    return -1;
  }
  else {
    return param_1 & 0x4000;
  }
}

// FUNCTION: MAGIC 0x004e1de1
// FUNCTION: SHANDALAR 0x004f1bcc
int SellPrice(void)
{
  //TODO: this needs a real implementation in shandalar eventually
  return 10;
}

// FUNCTION: SHANDALAR 0x00464663
// FUNCTION: MAGIC 0x00493bc3
void set_global_base_directory(char *path)
{
#ifdef _DEBUG
  // Allow debugging directly from output directory
  getcwd(path, 0x105);

#else
  char *last_slash;

  GetModuleFileNameA(NULL, path, 0x105);
  last_slash = strrchr(path, '\\');
  *last_slash = '\0';
#endif
}
// FUNCTION: SHANDALAR 0x00522880
// FUNCTION: MAGIC 0x004226c0
int setup_paths_and_load_text_etc(char *message_buffer)
{
  struct
  {
    char path[264];
    int ok;
    HDC desktop_dc;
  } s;

  s.ok = 1;

  set_global_base_directory(global_base_directory);
  _chdir(global_base_directory);

  strcpy(global_playdeck_path, global_base_directory);
  strcat(global_playdeck_path, "\\PlayDeck");
  strcpy(global_faces_path, global_base_directory);
  strcat(global_faces_path, "\\Faces");
  strcpy(global_cardart_path, global_base_directory);
  strcat(global_cardart_path, "\\CardArt");
  strcpy(global_duelart_path, global_base_directory);
  strcat(global_duelart_path, "\\DuelArt");
  strcpy(global_duelsounds_path, global_base_directory);
  strcat(global_duelsounds_path, "\\DuelSounds");
  strcpy(global_duel_dat_path, global_duelart_path);
  strcat(global_duel_dat_path, "\\Duel.dat");
  strcpy(global_ui_strings_filename, "UIStrings.txt");
  strcpy(global_savegame_path, global_base_directory);
  strcat(global_savegame_path, "\\SaveGame");
  _mkdir(global_savegame_path);

  FUN_00491f1e(global_ui_strings_filename);

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\CARDS.DAT");
  global_available_slots = read_db_guts(s.path);
  if (global_available_slots == 0)
  {
    s.ok = 0;
    load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
    append_startup_error(message_buffer, s.path, 1);
  }

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\LEGACY.CSV");
  if (!ReadLegacyCsv(s.path))
  {
    s.ok = 0;
    load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
    append_startup_error(message_buffer, s.path, 2);
  }

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\RARITY.CSV");
  if (!FUN_004c0c20(s.path))
  {
    s.ok = 0;
    load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
    append_startup_error(message_buffer, s.path, 1);
  }

  FUN_00509210();

  s.desktop_dc = GetDC(NULL);
  if (s.desktop_dc != NULL)
  {
    DAT_00939334 = GetDeviceCaps(s.desktop_dc, BITSPIXEL) * GetDeviceCaps(s.desktop_dc, PLANES);
    ReleaseDC(NULL, s.desktop_dc);
  }
  else
  {
    s.ok = 0;
    load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
    strcat(message_buffer, text_lines[3]);
    strcat(message_buffer, "\n");
  }

  if (!SetupDuelPalette())
  {
    s.ok = 0;
    load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
    strcat(message_buffer, text_lines[4]);
    strcat(message_buffer, "\n");
  }

  if (!CreateOffscreen32bppDibSection(GetSystemMetrics(SM_CXSCREEN),
                                      GetSystemMetrics(SM_CYSCREEN),
                                      &DAT_00789310,
                                      (BITMAPINFO *)&gs_cardtitle_draw_a_card_008b4330[0x40],
                                      &DAT_00926808,
                                      &DAT_0093a980,
                                      &DAT_008cefb0))
  {
    s.ok = 0;
    load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
    strcat(message_buffer, text_lines[5]);
    strcat(message_buffer, "\n");
  }

  if (!create_fonts())
  {
    s.ok = 0;
    load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
    strcat(message_buffer, text_lines[6]);
    strcat(message_buffer, "\n");
  }

  if (!InitCardArtGdiResources())
  {
    s.ok = 0;
    load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
    strcat(message_buffer, text_lines[7]);
    strcat(message_buffer, "\n");
  }

  FUN_004a7b3d();
  return s.ok;
}

// FUNCTION: SHANDALAR 0x00468a60
// FUNCTION: MAGIC 0x00509210
int FUN_00509210(void)
{
  struct
  {
    int card_index;
    int card_type;
  } s;

  for (s.card_index = 0; s.card_index < global_available_slots; ++s.card_index)
  {
    s.card_type = CardTypeFromID(s.card_index);
    if (s.card_type == -1)
    {
      continue;
    }

    if (global_raw_cards_storage[s.card_index].rarity == 1)
    {
      global_cards_data[s.card_type].rarity = 1;
    }
    else if (global_raw_cards_storage[s.card_index].rarity == 2)
    {
      global_cards_data[s.card_type].rarity = 3;
    }
    else if (global_raw_cards_storage[s.card_index].rarity == 3)
    {
      global_cards_data[s.card_type].rarity = 4;
    }
    else if (global_raw_cards_storage[s.card_index].rarity == 4)
    {
      global_cards_data[s.card_type].rarity = 2;
    }
    else
    {
      global_cards_data[s.card_type].rarity = 1;
    }

    strncpy(global_cards_data[s.card_type].name, global_raw_cards_storage[s.card_index].full_name, 0x23);
    global_cards_data[s.card_type].name[0x22] = '\0';
  }

  return 1;
}

// FUNCTION: MAGIC 0x004a7b3d
// FUNCTION: SHANDALAR 0x00559cc3
int FUN_004a7b3d(void)
{
  OSVERSIONINFOA version = {sizeof(OSVERSIONINFOA)};
  GetVersionExA(&version);
  DAT_00638ca8 = version.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;

  if (DAT_00776518 == NULL && DAT_00638ca8 != 0)
  {
    int ioctl_result;
    DAT_00776518 = CreateFileA("\\\\.\\MPStime.VXD", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL);
    assert((unsigned int)(DAT_00776518 != INVALID_HANDLE_VALUE), "D:\\Newmagic\\multiplayer\\sid\\glue.c", 0x360,
           "Could Not Load Dave's Extra Cool Timer\n");
    DeviceIoControl(DAT_00776518, 1, NULL, 0, &ioctl_result, 4, NULL, NULL);
    assert((unsigned int)(ioctl_result == 0x100), "D:\\Newmagic\\multiplayer\\sid\\glue.c", 0x367,
           "Could Not Initialize Dave's Extra Cool Timer\n");
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x00442f6a
// FUNCTION: MAGIC 0x004537a7
void FUN_00442f6a(void)
{
  if (global_base_txt != (char *)0)
  {
    free(global_base_txt);
  }
  global_base_txt = (char *)0;
}

// FUNCTION: SHANDALAR 0x004432ff
// FUNCTION: MAGIC 0x00453b3c
void FUN_004432ff(void)
{
  if (DAT_00637a94 != 0)
  {
    free((void *)DAT_00637a94);
  }
}

// FUNCTION: SHANDALAR 0x004a5d72
// FUNCTION: MAGIC 0x0048a0e2
void FUN_004a5d72(void) {}

// FUNCTION: SHANDALAR 0x00522e00
// FUNCTION: MAGIC 0x004c0c20
int FUN_004c0c20(const char *filename)
{
  //TODO: This seems to initialize useless stuff for SHANDALAR, only used in MAGIC
  (void)filename;
  return 0;
}

// FUNCTION: SHANDALAR 0x0046901b
// FUNCTION: MAGIC 0x005097cb
unsigned int setup_shared_startup(void)
{
  struct
  {
    unsigned int local_7d8;
    char local_7d4[2000];
  } s;

  s.local_7d8 = 1;
  if (DAT_00715fa0 == 0)
  {
    InitializeCriticalSection(&DAT_00926910);
    DAT_00715fa0 = 1;
  }
  s.local_7d4[0] = '\0';
  s.local_7d8 |= setup_paths_and_load_text_etc(s.local_7d4);
  FUN_004a5d72();
  DAT_007a7d7c = 1;
  InitializeCriticalSection(&DAT_009266b0);
  return s.local_7d8;
}

// FUNCTION: MAGIC 0x00422bea
// FUNCTION: SHANDALAR 0x00522daa
void FUN_00422bea(void)
{
  FUN_00442f6a();
  FUN_004432ff();
  FreeRaritiesCsvRaw();
  DestroyCardArtPalette();
  checked_DeleteDC_DeleteObject(DAT_00789310, DAT_00926808);
  DAT_00926808 = (HBITMAP)0;
  DAT_00789310 = (HDC)0;
  ShutdownCardArtGdiResources();
  destroy_create_fonts_resources();
}

// FUNCTION: MAGIC 0x00509849
// FUNCTION: SHANDALAR 0x00469099
int FUN_00469099(void)
{
  FUN_00422bea();
  if (DAT_00715fa0 != 0)
  {
    DeleteCriticalSection(&DAT_00926910);
    DAT_00715fa0 = 0;
  }
  DeleteCriticalSection(&DAT_009266b0);
}
