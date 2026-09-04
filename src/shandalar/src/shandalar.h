
#ifndef SHANDALAR_H
#define SHANDALAR_H

#include <windows.h>

typedef struct AdvMenuControl AdvMenuControl;

typedef int(__cdecl *AdvMenuRenderCallback)(AdvMenuControl *control, int mode);
typedef int(__cdecl *AdvMenuActivateCallback)(AdvMenuControl *control);
typedef int(__cdecl *TownDialogCallback)(int x, int y, int width, int height, int frame_style);

extern TownDialogCallback g_town_dialog_callback;

struct AdvMenuControl
{
  int base_x;
  int base_y;
  int base_width;
  int base_height;
  int x;
  int y;
  int width;
  int height;
  int enabled;
  AdvMenuRenderCallback on_render;
  AdvMenuActivateCallback on_activate;
  int selection_value;
  int data_value;
  char *navigate_hotkeys;
  char *activate_hotkeys;
  int direct_hotkey;
  int state;
  int mode_data[4];
};

typedef struct AdvMenuRect
{
  int x;
  int y;
  int width;
  int height;
} AdvMenuRect;

typedef struct OpeningMenuSpriteWorkEntry
{
  struct EncodedImage *sprites[0x2d];
} OpeningMenuSpriteWorkEntry;

typedef char AdvMenuControl_size_must_be_0x54[(sizeof(AdvMenuControl) == 0x54) ? 1 : -1];
typedef char AdvMenuRect_size_must_be_0x10[(sizeof(AdvMenuRect) == 0x10) ? 1 : -1];
typedef char OpeningMenuSpriteWorkEntry_size_must_be_0xb4[(sizeof(OpeningMenuSpriteWorkEntry) == 0xb4) ? 1 : -1];

#ifdef SHANDALAR
void StopWorldLocationMusic(void);
char *GetCreatureName(int creature_type);
int SeedRandomFromTickCount(void);
int internal_rand(int max_exclusive);
int AddCardToDeckSorted(int card_id);
void ReadCsvFieldByCsvid(char *out, int csvid, int field, const char *csv_name);
int GetCardRarity(int card_id);
int ScaleUiCoordinate(int value);
unsigned int InitializeSoundPresenceState(void);
void ShutdownUiTimer(void);
void RestoreSystemPaletteAtExit(void);
void CALLBACK UiTimerTickCallback(UINT timer_id, UINT msg, DWORD user_data,
                                  DWORD param1, DWORD param2);
#endif

#endif
