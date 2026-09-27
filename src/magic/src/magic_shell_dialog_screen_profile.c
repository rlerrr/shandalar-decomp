#include "magic_shell_screen_name.h"
#include <stdio.h>
#include <string.h>

extern char global_base_directory[];

// GLOBAL: MAGIC 0x006381c0
screen_name_playface_resources_t g_screen_name_playface_resources;

// GLOBAL: MAGIC 0x00638518
screen_name_text_resources_t g_screen_name_text_resources;

// GLOBAL: MAGIC 0x006385e8
screen_name_unknown_resources_t g_screen_name_unknown_resources;

// FUNCTION: MAGIC 0x0048f88d
void shell_populate_screen_names(HWND hwnd)
{
  struct
  {
    size_t name_length;
    char filename[264];
    char previous_directory[264];
    WPARAM name_index;
    HANDLE find_handle;
    WIN32_FIND_DATAA find_data;
  } s;

  s.name_index = 0;
  GetCurrentDirectoryA(0x104, s.previous_directory);
  SetCurrentDirectoryA(global_base_directory);
  strcpy(s.filename, "ScreenNames\\*.scn");
  SendDlgItemMessageA(hwnd, 0x741, CB_RESETCONTENT, 0, 0);
  s.find_handle = FindFirstFileA(s.filename, &s.find_data);
  if (s.find_handle == INVALID_HANDLE_VALUE)
    return;
  do
  {
    GetFileTitleA(s.find_data.cFileName, s.filename, 0x104);
    if (_stricmp(s.filename + (strlen(s.filename) - 4), ".SCN") == 0)
    {
      s.name_length = strlen(s.filename);
      s.filename[s.name_length - 4] = '\0';
    }
    SendDlgItemMessageA(hwnd, 0x741, CB_INSERTSTRING, s.name_index++, (LPARAM)s.filename);
  } while (FindNextFileA(s.find_handle, &s.find_data) != 0);
  FindClose(s.find_handle);
  SendDlgItemMessageA(hwnd, 0x741, CB_SETCURSEL, 0, 0);
  SetCurrentDirectoryA(s.previous_directory);
}

// FUNCTION: MAGIC 0x0048fbc6
void shell_save_active_screen_name(void)
{
  FILE *file;

  SetFileAttributesA("ScreenNames\\ActiveName.dat", FILE_ATTRIBUTE_NORMAL);
  file = fopen("ScreenNames\\ActiveName.dat", "wb");
  fwrite(g_screen_name_profile.screen_name, 0xe, 1, file);
  fclose(file);
}

// FUNCTION: MAGIC 0x004908c5
void shell_get_playface_rect(HWND hwnd, RECT *rect)
{
  GetWindowRect(GetDlgItem(hwnd, 0x715), rect);
  MapWindowPoints(NULL, hwnd, (LPPOINT)rect, 2);
}

// FUNCTION: MAGIC 0x0048fadd
void shell_save_match_screen_name_profile(void)
{
  struct
  {
    FILE *file;
    char filename[100];
  } s;

  if (strcmp(g_screen_name_profile.screen_name, g_screen_name_playface_resources.default_name) == 0)
    return;
  sprintf(s.filename, "ScreenNames\\%s.scn", g_screen_name_profile.screen_name);
  SetFileAttributesA(s.filename, FILE_ATTRIBUTE_NORMAL);
  s.file = fopen(s.filename, "wb");
  fwrite(&g_screen_name_profile, 0x748, 1, s.file);
  fclose(s.file);
  SetFileAttributesA("ScreenNames\\ActiveName.dat", FILE_ATTRIBUTE_NORMAL);
  s.file = fopen("ScreenNames\\ActiveName.dat", "wb");
  fwrite(g_screen_name_profile.screen_name, 0xe, 1, s.file);
  fclose(s.file);
}

// GLOBAL: MAGIC 0x0093e1d0
char g_screen_name_skill_names[10][300];

// GLOBAL: MAGIC 0x00637f40
char g_screen_name_record_label[100];

// GLOBAL: MAGIC 0x00637fb0
char g_screen_name_rank_format[100];

// GLOBAL: MAGIC 0x00638018
char g_screen_name_disconnect_format[100];

// GLOBAL: MAGIC 0x00638080
char g_screen_name_concede_format[100];

// GLOBAL: MAGIC 0x006380f0
char g_screen_name_dci_record_label[100];

// GLOBAL: MAGIC 0x00638158
char g_screen_name_multiplayer_record_label[100];

// GLOBAL: MAGIC 0x00637f38
HBRUSH g_screen_name_entry_brush;

// GLOBAL: MAGIC 0x00637f3c
COLORREF g_screen_name_entry_text_color;

// GLOBAL: MAGIC 0x00637fa8
HPEN g_screen_name_entry_dark_pen;

// GLOBAL: MAGIC 0x006380e4
HPEN g_screen_name_entry_light_pen;

// GLOBAL: MAGIC 0x006380e8
HBITMAP g_screen_name_entry_background;

// GLOBAL: MAGIC 0x006380ec
COLORREF g_screen_name_entry_button_color;


// GLOBAL: MAGIC 0x00638580
char g_screen_name_skill_format[100];
