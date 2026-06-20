#include <windows.h>
#include <mmsystem.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "cardartlib/src/assert.h"

// GLOBAL: SHANDALAR 0x007483f8
HDC global_main_hdc;

/* Text buffer lives in the original .exe at a fixed address. */
#define text_lines ((char(*)[300])0x008aa920)

typedef void(__cdecl *fn_void_void)(void);
typedef int(__cdecl *fn_int_void)(void);

// Strings
#define s_ShandalarMainClass_00589e04 EXE_STR(0x00589e04)
#define s_Magic_Shell_00589e18 EXE_STR(0x00589e18)
#define s__MTGshell_00589e24 EXE_STR(0x00589e24)
#define s_Low_Memory_Swap_File_Space_Warni_00589e30 EXE_STR(0x00589e30)
#define s_We_have_determined_that_you_may_n_00589e54 EXE_STR(0x00589e54)
#define s_AdvStrings_txt_00589f4c EXE_STR(0x00589f4c)
#define s_Couldn_t_load_the_strings_from_t_00589f5c EXE_STR(0x00589f5c)
#define s_AdvBlocks_txt_00589f94 EXE_STR(0x00589f94)
#define s_Couldn_t_load_the_strings_from_t_00589fa4 EXE_STR(0x00589fa4)
#define s_ShandalarMainClass_00589fdc EXE_STR(0x00589fdc)
#define s_Couldn_t_register_the_classes_00589ff0 EXE_STR(0x00589ff0)
#define s_advinter_pic_0058a010 EXE_STR(0x0058a010)
#define s_advinter800_pic_0058a020 EXE_STR(0x0058a020)
#define s_advinter1024_pic_0058a030 EXE_STR(0x0058a030)
#define s_advinter_pic_0058a044 EXE_STR(0x0058a044)
#define s_advinter800_pic_0058a054 EXE_STR(0x0058a054)
#define s_advinter1024_pic_0058a064 EXE_STR(0x0058a064)
#define s_Magic__Shandalar_0058a078 EXE_STR(0x0058a078)
#define s_ShandalarMainClass_0058a08c EXE_STR(0x0058a08c)
#define s_Timer_failed_to_initialize__0058a0a0 EXE_STR(0x0058a0a0)
#define s_D__Newmagic_multiplayer_sid_Test_0058a0c0 EXE_STR(0x0058a0c0)
#define s_Could_not_start_timer_0058a0e4 EXE_STR(0x0058a0e4)
#define s_D__Newmagic_multiplayer_sid_Test_0058a0fc EXE_STR(0x0058a0fc)
#define s__MTGshell_0058a120 EXE_STR(0x0058a120)
#define s__Magic_exe_0058a12c EXE_STR(0x0058a12c)
#define s__start3_1_0058a138 EXE_STR(0x0058a138)
#define s_Current_Palette_0058a144 EXE_STR(0x0058a144)
#define s_ShowPaletteClass_0058a154 EXE_STR(0x0058a154)
#define s_sound_locmus1_wav_0058a168 EXE_STR(0x0058a168)
#define s_PLAYERNAMES_00593938 EXE_STR(0x00593938)
#define s_DIFFICULTYLEVELS_00593944 EXE_STR(0x00593944)
#define s_DUNGEON_NAMES_00593958 EXE_STR(0x00593958)
#define s_LAIR_NAMES_00593968 EXE_STR(0x00593968)
#define s_CITYNAMES_FORMAT_00593974 EXE_STR(0x00593974)
#define s_CITYNAME_VILLAGE_00593988 EXE_STR(0x00593988)
#define s_CITYNAME_CASTLE_0059399c EXE_STR(0x0059399c)
#define s_CITYNAME_MANACASTLE_005939ac EXE_STR(0x005939ac)
#define s_CITYNAMES_FIRSTHALF_005939c0 EXE_STR(0x005939c0)
#define s_CITYNAMES_SECONDHALF_005939d4 EXE_STR(0x005939d4)
#define s_WORLDMAGIC_005939ec EXE_STR(0x005939ec)
#define s_WORLDMAGIC_NAMES_005939f8 EXE_STR(0x005939f8)
#define s_WORLDMAGIC_EXPLAINS_00593a0c EXE_STR(0x00593a0c)
#define s_LOGSTRINGS_00593a20 EXE_STR(0x00593a20)
#define s_WIZARDNAMES_00593a2c EXE_STR(0x00593a2c)
#define s_default_00593a38 EXE_STR(0x00593a38)
#define s_CREATURENAMES_00593a3c EXE_STR(0x00593a3c)
#define s_default_00593a4c EXE_STR(0x00593a4c)
#define s_CREATURENAME_ARTICLES_00593a50 EXE_STR(0x00593a50)
#define s_default_00593a68 EXE_STR(0x00593a68)
#define s_CREATURENAMES_PLURAL_00593a6c EXE_STR(0x00593a6c)
#define s_default_00593a84 EXE_STR(0x00593a84)
#define s_DIRECTIONS_00593a88 EXE_STR(0x00593a88)
#define s_at_005a0d2c EXE_STR(0x005a0d2c)
#define s_newline_005a0d30 EXE_STR(0x005a0d30)
#define s_backslash_005a0d34 EXE_STR(0x005a0d34)
#define s_rt_005a0d38 EXE_STR(0x005a0d38)
#define s_fmt_d_005a0d3c EXE_STR(0x005a0d3c)

/* These strings are patched at runtime; keep as writable pointers. */
#define s_x_sound_kwalkl_wav_00591880 EXE_BYTE_PTR(0x00591880)
#define s_x_sound_kwalkr_wav_00591894 EXE_BYTE_PTR(0x00591894)
#define s_x_sound_bwalkl_wav_005918a8 EXE_BYTE_PTR(0x005918a8)
#define s_x_sound_bwalkr_wav_005918bc EXE_BYTE_PTR(0x005918bc)
#define s_x_sound_gwalkl_wav_005918d0 EXE_BYTE_PTR(0x005918d0)
#define s_x_sound_gwalkr_wav_005918e4 EXE_BYTE_PTR(0x005918e4)
#define s_x_sound_rwalkl_wav_005918f8 EXE_BYTE_PTR(0x005918f8)
#define s_x_sound_rwalkr_wav_0059190c EXE_BYTE_PTR(0x0059190c)
#define s_x_sound_wwalkl_wav_00591920 EXE_BYTE_PTR(0x00591920)
#define s_x_sound_wwalkr_wav_00591934 EXE_BYTE_PTR(0x00591934)
#define s_x_sound_kbird1_wav_00591948 EXE_BYTE_PTR(0x00591948)
#define s_x_sound_bbird1_wav_0059195c EXE_BYTE_PTR(0x0059195c)
#define s_x_sound_gbird1_wav_00591970 EXE_BYTE_PTR(0x00591970)
#define s_x_sound_rbird1_wav_00591984 EXE_BYTE_PTR(0x00591984)
#define s_x_sound_wbird1_wav_00591998 EXE_BYTE_PTR(0x00591998)
#define s_empty_00593c50 EXE_STR(0x00593c50)

// Globals (accessed by absolute address to avoid duplicate storage)
#define DAT_0078df78 EXE_DWORD(0x0078df78)
#define DAT_0078e5f0 EXE_TYP_PTR(char, 0x0078e5f0)
#define DAT_005863b8 EXE_DWORD(0x005863b8)
#define DAT_005863bc EXE_DWORD(0x005863bc)
#define PTR_s_advinter800_pic_00589de8 EXE_TYP(char *, 0x00589de8)
#define PTR_DAT_005832b4 ((unsigned char *)EXE_PTR_VOID(0x005832b4))
#define DAT_00748420 EXE_TYP(HWND, 0x00748420)
#define DAT_00939160 EXE_TYP(HINSTANCE, 0x00939160)
#define DAT_005a1608 EXE_DWORD(0x005a1608)
#define DAT_00986950 ((unsigned char *)EXE_PTR_VOID(0x00986950))
#define DAT_00591210 EXE_DWORD(0x00591210)
#define DAT_00748418 EXE_TYP(UINT, 0x00748418)
#define DAT_007483fc EXE_TYP(UINT, 0x007483fc)
#define DAT_00589de4 EXE_TYP(UINT, 0x00589de4)
#define DAT_00589df0 EXE_DWORD(0x00589df0)
#define DAT_00748400 EXE_DWORD(0x00748400)
#define DAT_00748404 EXE_TYP(HANDLE, 0x00748404)
#define DAT_00748424 EXE_TYP(HANDLE, 0x00748424)
#define DAT_00748408 EXE_DWORD(0x00748408)
#define _DAT_0074840c EXE_DWORD(0x0074840c)
#define _DAT_00748410 EXE_TYP(HANDLE, 0x00748410)
#define DAT_005b7d90 EXE_DWORD(0x005b7d90)
#define DAT_005b7d94 EXE_DWORD(0x005b7d94)
#define DAT_005b7d98 EXE_TYP(HWND, 0x005b7d98)
#define DAT_00986d94 EXE_DWORD(0x00986d94)
#define DAT_00986d98 EXE_DWORD(0x00986d98)
#define DAT_00986d9c EXE_DWORD(0x00986d9c)
#define DAT_00986da0 EXE_DWORD(0x00986da0)
#define DAT_0093aa40 EXE_TYP(CRITICAL_SECTION, 0x0093aa40)
#define DAT_005a0d40 EXE_DWORD(0x005a0d40)
#define DAT_005a0d44 EXE_DWORD(0x005a0d44)
#define DAT_005a0d48 EXE_DWORD(0x005a0d48)
#define DAT_0073bf98 EXE_TYP(HMODULE, 0x0073bf98)
#define DAT_0073bfa0 EXE_TYP_PTR(FARPROC, 0x0073bfa0)
#define DAT_0073bfa4 EXE_TYP(fn_void_void, 0x0073bfa4)
#define DAT_0073bfa8 EXE_TYP(FARPROC, 0x0073bfa8)
#define DAT_0073bfe0 EXE_TYP(fn_int_void, 0x0073bfe0)
#define DAT_00669704 EXE_DWORD(0x00669704)
#define DAT_0066970c EXE_DWORD(0x0066970c)
#define DAT_0073e890 EXE_TYP_PTR(char, 0x0073e890)
#define DAT_0073e9d8 EXE_BYTE(0x0073e9d8)
#define DAT_0067a3b8 EXE_DWORD(0x0067a3b8)
#define DAT_005919ac EXE_DWORD(0x005919ac)
#define DAT_0074c930 EXE_TYP_PTR(char *, 0x0074c930)
#define DAT_0077c9e0 EXE_TYP_PTR(char *, 0x0077c9e0)
#define DAT_008bd200 EXE_DWORD(0x008bd200)
#define DAT_0077e2c0 EXE_TYP_PTR(char, 0x0077e2c0)
#define DAT_0077e57c EXE_TYP_PTR(char, 0x0077e57c)
#define DAT_0074d790 EXE_TYP_PTR(char, 0x0074d790)
#define DAT_0074d7f4 EXE_TYP_PTR(char, 0x0074d7f4)
#define DAT_0074c970 EXE_TYP_PTR(char, 0x0074c970)
#define DAT_0074ccc2 EXE_TYP_PTR(char, 0x0074ccc2)
#define DAT_0074bd30 EXE_TYP_PTR(char, 0x0074bd30)
#define DAT_0074c0e6 EXE_TYP_PTR(char, 0x0074c0e6)
#define DAT_0074c950 EXE_TYP_PTR(char, 0x0074c950)
#define DAT_0077f190 EXE_TYP_PTR(char, 0x0077f190)
#define DAT_0077cfd0 EXE_TYP_PTR(char, 0x0077cfd0)
#define DAT_00765dc0 EXE_TYP_PTR(char, 0x00765dc0)
#define DAT_0077de00 EXE_TYP_PTR(char, 0x0077de00)
#define DAT_0077d610 EXE_TYP_PTR(char, 0x0077d610)
#define DAT_0077dc50 EXE_TYP_PTR(char, 0x0077dc50)
#define DAT_0077e1d0 EXE_TYP_PTR(char, 0x0077e1d0)
#define DAT_0074b160 EXE_TYP_PTR(char, 0x0074b160)
#define DAT_0074b868 EXE_TYP_PTR(char, 0x0074b868)
#define DAT_0077c680 EXE_TYP_PTR(char, 0x0077c680)
#define DAT_0077c9a0 EXE_TYP_PTR(char, 0x0077c9a0)
#define DAT_0077ee70 EXE_TYP_PTR(char, 0x0077ee70)
#define DAT_00593934 EXE_DWORD(0x00593934)
#define DAT_00591a12 EXE_TYP_PTR(char, 0x00591a12)
#define DAT_00591a08 EXE_TYP_PTR(char, 0x00591a08)
#define DAT_00591a44 EXE_TYP_PTR(char, 0x00591a44)
#define DAT_00765d50 EXE_TYP_PTR(char, 0x00765d50)

// GLOBAL: SHANDALAR 0x005a0d4c (pointer to "magsnd")
#define PTR_s_magsnd_005a0d4c EXE_TYP(const char *, 0x005a0d4c)

/* 0x00589de4: hardcoded timer interval in original binary. */

int InitLicenseSecretsFromRegistry(void);
int load_text(const char *file_name, const char *section_name);
void FUN_00464663(char *out_dir);
int FUN_00564ee7(const char *filename);
int FUN_0056cc4d(const char *filename, const char *section);
int FUN_00565c7e(const char *filename, const char *section, int out_table, int max_entries, char *string_buf,
                 char *string_buf_end, char **out_next_buf);
int FUN_00565dbc(const char *filename);
int FUN_00565fdb(char *param_1, char *param_2, int *param_3, int *param_4);
void FUN_00559999(void);
void *CreateGraphicsPage(int page_number, int width, int height, int bits_per_pixel);
int FUN_0056cf20(int param_1, int param_2, unsigned int param_3);
void FUN_00562d03(void);
void FUN_00565faa(void);
char ***__cdecl __p___argv(void);
void FUN_0056d081(void);
void QueueKeyInputFromMessage(WPARAM wparam, LPARAM lparam);
ATOM RegisterPaletteClass(HINSTANCE hinst);
HWND CreatePalettePopupWindow(HINSTANCE hinst, HWND parent_hwnd);
int FUN_0056d476(void);
void FUN_0056d74e(void);
int FUN_00417dc6(const char *filename);
unsigned int FUN_00562e0d(char *filename);
char FUN_00562ed0(void);
unsigned int __cdecl FUN_0056d0f7(void *param_1, int param_2, int param_3);
unsigned int __cdecl FUN_00562f92(char *filename, int param_2, int param_3);
LONG ChangeDisplayResolution(DWORD width, DWORD height);
void RestoreDisplayResolution(void);
DWORD WINAPI FUN_0046e6f0(LPVOID);

// FUNCTION: SHANDALAR 0x00464663
void FUN_00464663(char *out_dir)
{
  char *slash;

  GetModuleFileNameA((HMODULE)0, out_dir, 260);
  slash = strrchr(out_dir, '\\');
  if (slash != 0)
  {
    *slash = 0;
  }
}

// FUNCTION: SHANDALAR 0x00564ee7
int FUN_00564ee7(const char *filename)
{
  struct
  {
    int ok;
    int local_c;
    char *local_8;
    int iVar36;
  } s;

  s.ok = 1;
  s.ok &= FUN_00565c7e(filename, s_PLAYERNAMES_00593938, 0x0077c5a0, 0xe, DAT_0077e2c0, DAT_0077e2c0 + 0x2bc, (char **)0);
  s.ok &= FUN_00565c7e(filename, s_DIFFICULTYLEVELS_00593944, 0x0077d130, 4, DAT_0074d790, DAT_0074d790 + 0x64, (char **)0);
  s.ok &= FUN_00565c7e(filename, s_DUNGEON_NAMES_00593958, 0x00780820, 0x11, DAT_0074c970, DAT_0074c970 + 0x352, (char **)0);
  s.ok &= FUN_00565c7e(filename, s_LAIR_NAMES_00593968, 0x0077c020, 0x13, DAT_0074bd30, DAT_0074bd30 + 0x3b6, (char **)0);

  FUN_0056cc4d(filename, s_CITYNAMES_FORMAT_00593974);
  strcpy(DAT_0074c950, (char *)text_lines);
  strcpy(DAT_0077f190, (char *)text_lines + 0x12c);

  FUN_0056cc4d(filename, s_CITYNAME_VILLAGE_00593988);
  strcpy(DAT_0077cfd0, text_lines[0]);

  FUN_0056cc4d(filename, s_CITYNAME_CASTLE_0059399c);
  strcpy(DAT_00765dc0, text_lines[0]);

  FUN_0056cc4d(filename, s_CITYNAME_MANACASTLE_005939ac);

  for (s.local_c = 0; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    s.iVar36 = s.local_c * 4 + 4;
    s.iVar36 = s.iVar36 + s.iVar36 * 4;
    s.iVar36 = s.iVar36 + s.iVar36 * 4;
    strcpy(DAT_0077de00 + s.iVar36, text_lines[s.local_c]);
  }
  strcpy(DAT_0077de00, DAT_00765dc0);

  s.local_8 = DAT_0077d610;
  s.ok &= FUN_00565c7e(filename, s_CITYNAMES_FIRSTHALF_005939c0, 0x0077e060, 0x10, DAT_0077d610, DAT_0077dc50, &s.local_8);
  s.ok &= FUN_00565c7e(filename, s_CITYNAMES_SECONDHALF_005939d4, 0x007653e0, 0x10, s.local_8, DAT_0077dc50, (char **)0);

  s.local_8 = DAT_0074b160;
  FUN_0056cc4d(filename, s_WORLDMAGIC_005939ec);
  strcpy(DAT_0077e1d0, text_lines[0]);
  s.ok &= FUN_00565c7e(filename, s_WORLDMAGIC_NAMES_005939f8, 0x00780660, 0xc, DAT_0074b160, DAT_0074b868, &s.local_8);
  s.ok &= FUN_00565c7e(filename, s_WORLDMAGIC_EXPLAINS_00593a0c, 0x0074b8f0, 0xc, s.local_8, DAT_0074b868, (char **)0);

  s.ok &= FUN_00565c7e(filename, s_LOGSTRINGS_00593a20, 0x0077c9a0, 0x10, DAT_0077c680, DAT_0077c9a0, (char **)0);

  FUN_0056cc4d(filename, s_WIZARDNAMES_00593a2c);
  for (s.local_c = 0; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0077ee70 + (s.local_c * 5 + 5) * 10, text_lines[s.local_c]);
  }
  strcpy(DAT_0077ee70, s_default_00593a38);

  return s.ok;
}

// FUNCTION: SHANDALAR 0x0056cc4d
int FUN_0056cc4d(const char *filename, const char *section)
{
  int x;
  int i;
  int j;
  size_t len;
  int out_pos;

  if (DAT_008bd200 == 1)
  {
    x = 0;
  }
  else
  {
    x = load_text(filename, section);
    i = 0;
    while (i < abs(x))
    {
      len = strlen(text_lines[i]);
      out_pos = 0;
      j = 0;
      while (j <= (int)len)
      {
        if (text_lines[i][j] == '\\' && text_lines[i][j + 1] == 'n')
        {
          text_lines[i][out_pos] = '\n';
          j = j + 1;
        }
        else
        {
          text_lines[i][out_pos] = text_lines[i][j];
        }
        out_pos = out_pos + 1;
        j = j + 1;
      }
      text_lines[i][out_pos] = '\0';
      i = i + 1;
    }
  }

  return x;
}

// FUNCTION: SHANDALAR 0x00565c7e
int FUN_00565c7e(const char *filename, const char *section, int out_table, int max_entries, char *string_buf,
                 char *string_buf_end, char **out_next_buf)
{
  int overflow;
  int count;
  size_t line_len;
  int i;
  char *cursor;

  overflow = 0;
  count = FUN_0056cc4d(filename, section);
  if (max_entries <= count)
  {
    count = max_entries;
  }

  cursor = string_buf;
  i = 0;
  while (i < count && !overflow)
  {
    line_len = strlen(text_lines[i]);
    if (cursor + line_len < string_buf_end)
    {
      strcpy(cursor, text_lines[i]);
      ((char **)out_table)[i] = cursor;
      cursor = cursor + line_len + 1;
    }
    else
    {
      overflow = 1;
    }
    ++i;
  }

  while (i = count, i < max_entries)
  {
    ((char **)out_table)[i] = (char *)s_empty_00593c50;
    count = i + 1;
  }

  if (out_next_buf != (char **)0)
  {
    *out_next_buf = cursor;
  }

  return !overflow;
}

// FUNCTION: SHANDALAR 0x00565fdb
int FUN_00565fdb(char *param_1, char *param_2, int *param_3, int *param_4)
{
  int cmp;

  if (param_1 == (char *)0 || param_2 == (char *)0 || param_2 <= param_1 || param_3 == (int *)0 || param_4 == (int *)0)
  {
    return 0;
  }

  while (*param_1 != '\0' && param_1 < param_2 && (cmp = strncmp(param_1, "STARTBLOCK", 10), cmp != 0))
  {
    ++param_1;
  }

  cmp = strncmp(param_1, "STARTBLOCK", 10);
  if (cmp != 0)
  {
    return 0;
  }

  param_1 += 0xc;
  if (param_3 != (int *)0)
  {
    *param_3 = (int)param_1;
  }

  while (*param_1 != '\0' && param_1 < param_2 && (cmp = strncmp(param_1, "ENDBLOCK", 8), cmp != 0))
  {
    ++param_1;
  }

  cmp = strncmp(param_1, "ENDBLOCK", 8);
  if (cmp == 0)
  {
    *param_1 = '\0';
    if (param_4 != (int *)0)
    {
      *param_4 = (int)(param_1 + 8);
    }
  }
  else if (param_4 != (int *)0)
  {
    *param_4 = 0;
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x00565dbc
int FUN_00565dbc(const char *filename)
{
  HANDLE file;
  int ok;
  DWORD bytes_read;
  DWORD file_size;
  int block_start;
  int i;
  char *buffer_end;
  char *cr;
  char *cursor;
  char *next;

  ok = 1;
  file = CreateFileA(filename, 0x80000000, 1, (LPSECURITY_ATTRIBUTES)0, 3, 0x8000080, (HANDLE)0);
  if (file == (HANDLE)-1)
  {
    ok = 0;
  }
  else
  {
    file_size = GetFileSize(file, (LPDWORD)0);
    DAT_0067a3b8 = (int)malloc(file_size + 1);
    if ((char *)DAT_0067a3b8 == (char *)0)
    {
      ok = 0;
    }
    else
    {
      ReadFile(file, (void *)DAT_0067a3b8, file_size, &bytes_read, (LPOVERLAPPED)0);
      cursor = (char *)DAT_0067a3b8;
      buffer_end = (char *)DAT_0067a3b8 + bytes_read;

      for (i = 0; i < 4; ++i)
      {
        if (FUN_00565fdb(cursor, buffer_end, &block_start, (int *)&next) == 0)
        {
          ok = 0;
        }
        else
        {
          DAT_0074c930[i] = (char *)block_start;
          cursor = next;
          while (cr = strchr(DAT_0074c930[i], 0xd), cr != (char *)0)
          {
            strcpy(cr, cr + 1);
          }
        }
      }

      for (i = 0; i < 0xc; ++i)
      {
        if (FUN_00565fdb(cursor, buffer_end, &block_start, (int *)&next) == 0)
        {
          ok = 0;
        }
        else
        {
          DAT_0077c9e0[i] = (char *)block_start;
          cursor = next;
          while (cr = strchr(DAT_0077c9e0[i], 0xd), cr != (char *)0)
          {
            strcpy(cr, cr + 1);
          }
        }
      }
    }

    CloseHandle(file);
  }

  return ok;
}

// FUNCTION: SHANDALAR 0x00559999
void FUN_00559999(void)
{
}

// FUNCTION: SHANDALAR 0x0056cf20
int FUN_0056cf20(int param_1, int param_2, unsigned int param_3)
{
  int i;
  int result;

  result = 0;
  if (DAT_005a0d44 != 0)
  {
    result = 2;
    goto done;
  }

  DAT_0073bf98 = LoadLibraryA(PTR_s_magsnd_005a0d4c);
  if (DAT_0073bf98 == (HMODULE)0)
  {
    result = 4;
    goto done;
  }

  for (i = 0; i < 0x1b; i = i + 1)
  {
    DAT_0073bfa0[i] = GetProcAddress(DAT_0073bf98, (LPCSTR)((i + 1U) & 0xffff));
    if (DAT_0073bfa0[i] == (FARPROC)0)
    {
      FreeLibrary(DAT_0073bf98);
      FUN_0056d74e();
      result = 4;
      goto done;
    }
  }

  if (param_1 == 0 && (param_3 & 2) == 0)
  {
    FreeLibrary(DAT_0073bf98);
    FUN_0056d74e();
    result = 5;
    goto done;
  }
  else
  {
    result = ((int(__cdecl *)(int, int, unsigned int))DAT_0073bfa0[0])(param_1, param_2, param_3);
    if (result != 0)
    {
      FreeLibrary(DAT_0073bf98);
      FUN_0056d74e();
      goto done;
    }

    DAT_005a0d48 = 1;
    if ((param_3 & 2) != 0)
    {
      DAT_005a0d40 = 1;
    }
    DAT_005a0d44 = 1;
    result = 0;
  }

done:
  return result;
}

// FUNCTION: SHANDALAR 0x00562d03
void FUN_00562d03(void)
{
  FUN_00562f92(s_x_sound_kwalkl_wav_00591880, 0, 0);
  FUN_00562f92(s_x_sound_kwalkr_wav_00591894, 1, 0);
  FUN_00562f92(s_x_sound_bwalkl_wav_005918a8, 2, 0);
  FUN_00562f92(s_x_sound_bwalkr_wav_005918bc, 3, 0);
  FUN_00562f92(s_x_sound_gwalkl_wav_005918d0, 4, 0);
  FUN_00562f92(s_x_sound_gwalkr_wav_005918e4, 5, 0);
  FUN_00562f92(s_x_sound_rwalkl_wav_005918f8, 6, 0);
  FUN_00562f92(s_x_sound_rwalkr_wav_0059190c, 7, 0);
  FUN_00562f92(s_x_sound_wwalkl_wav_00591920, 8, 0);
  FUN_00562f92(s_x_sound_wwalkr_wav_00591934, 9, 0);
  FUN_00562f92(s_x_sound_kbird1_wav_00591948, 10, 0);
  FUN_00562f92(s_x_sound_bbird1_wav_0059195c, 0xb, 0);
  FUN_00562f92(s_x_sound_gbird1_wav_00591970, 0xc, 0);
  FUN_00562f92(s_x_sound_rbird1_wav_00591984, 0xd, 0);
  FUN_00562f92(s_x_sound_wbird1_wav_00591998, 0xe, 0);
}

// FUNCTION: SHANDALAR 0x00565faa
void FUN_00565faa(void)
{
  if (DAT_0067a3b8 != 0)
  {
    free((void *)DAT_0067a3b8);
    DAT_0067a3b8 = 0;
  }
}

// FUNCTION: SHANDALAR 0x00562e0d
unsigned int FUN_00562e0d(char *filename)
{
  UINT drive_type;
  FILE *file;
  int i;
  DWORD *buf;
  char drive_string[260];
  DWORD scratch[63];

  *(DWORD *)drive_string = (DWORD)DAT_005919ac;

  buf = scratch;
  for (i = 0x3f; i != 0; --i)
  {
    *buf++ = 0;
  }

  for (;;)
  {
    if ('z' < drive_string[0])
    {
      return (unsigned int)drive_string[0];
    }

    drive_type = GetDriveTypeA(drive_string);
    if (drive_type == 5)
    {
      strcat(drive_string, filename);
      file = fopen(drive_string, "rb");
      if (file != (FILE *)0)
      {
        fclose(file);
        return (unsigned int)drive_string[0];
      }

      drive_string[3] = 0;
    }

    ++drive_string[0];
  }
}

// FUNCTION: SHANDALAR 0x00562ed0
char FUN_00562ed0(void)
{
  unsigned int drive;
  char cwd[256];

  if (DAT_0066970c == 0)
  {
    drive = FUN_00562e0d("sound\\locmus1.wav");
    DAT_0073e9d8 = (char)drive;
    DAT_0066970c = 1;
  }

  if (DAT_00748408 == 0)
  {
    _getcwd(cwd, 0x100);
    return cwd[0];
  }

  return DAT_0073e9d8;
}

// FUNCTION: SHANDALAR 0x0056d0f7
unsigned int __cdecl FUN_0056d0f7(void *param_1, int param_2, int param_3)
{
  if (DAT_005a0d44 == 0)
  {
    return 4;
  }

  return (unsigned int)((int(__cdecl *)(void *, int, int))DAT_0073bfa8)(param_1, param_2, param_3);
}

// FUNCTION: SHANDALAR 0x00562f92
unsigned int __cdecl FUN_00562f92(char *filename, int param_2, int param_3)
{
  char drive_letter;

  if (DAT_00669704 == 0)
  {
    _getcwd(DAT_0073e890, 0x100);
    DAT_00669704 = 1;
  }

  if (filename[0] == 'x')
  {
    filename[0] = DAT_0073e890[0];
    if (FUN_00417dc6(filename) == 0)
    {
      drive_letter = FUN_00562ed0();
      filename[0] = drive_letter;
    }
  }

  while (DAT_00748400 != 0)
  {
  }

  FUN_0056d0f7(filename, param_2, param_3);
  return 0;
}

// FUNCTION: SHANDALAR 0x0056d081
void FUN_0056d081(void)
{
  if (DAT_005a0d44 != 0)
  {
    DAT_005a0d44 = 0;
    if (DAT_005a0d48 != 0 && DAT_005a0d40 == 0)
    {
      DAT_0073bfa4();
    }
    FreeLibrary(DAT_0073bf98);
    FUN_0056d74e();
    DAT_0073bf98 = (HMODULE)0;
    DAT_005a0d40 = 0;
    DAT_005a0d48 = 0;
  }
}

// FUNCTION: SHANDALAR 0x0056d476
int FUN_0056d476(void)
{
  if (DAT_005a0d44 == 0 || DAT_005a0d44 == 2)
  {
    return 4;
  }

  return DAT_0073bfe0();
}

// FUNCTION: SHANDALAR 0x0056d74e
void FUN_0056d74e(void)
{
  int i;

  for (i = 0; i < 0x1b; ++i)
  {
    DAT_0073bfa0[i] = 0;
  }
}

// FUNCTION: SHANDALAR 0x00417dc6
int FUN_00417dc6(const char *filename)
{
  FILE *file;

  file = fopen(filename, "rt");
  if (file == (FILE *)0)
  {
    return 0;
  }

  fclose(file);
  return 1;
}

// FUNCTION: SHANDALAR 0x0046e6f0
DWORD WINAPI FUN_0046e6f0(LPVOID param_1)
{
  (void)param_1;
  return 0;
}

// FUNCTION: SHANDALAR 0x004cea4c
LONG ChangeDisplayResolution(DWORD width, DWORD height)
{
  DEVMODEA dev_mode;

  if (height == 0 && width == 0)
  {
    return ChangeDisplaySettingsA((DEVMODEA *)0, 0);
  }

  if (height == 0 || width == 0)
  {
    return -1;
  }

  dev_mode.dmSize = 0x9c;
  dev_mode.dmFields = 0x180000;
  dev_mode.dmPelsWidth = width;
  dev_mode.dmPelsHeight = height;
  return ChangeDisplaySettingsA(&dev_mode, 0);
}

// FUNCTION: SHANDALAR 0x004ceacb
void RestoreDisplayResolution(void)
{
  ChangeDisplayResolution(0, 0);
}

// FUNCTION: SHANDALAR 0x004cea02
unsigned int FUN_004cea02(void)
{
  DAT_00748408 = (unsigned int)(FUN_00417dc6(s_sound_locmus1_wav_0058a168) == 0);
  _DAT_0074840c = 0;
  return 0;
}

// FUNCTION: SHANDALAR 0x004ce9c6
void FUN_004ce9c6(void)
{
  timeKillEvent(DAT_007483fc);
  timeEndPeriod(DAT_00748418);
}

// FUNCTION: SHANDALAR 0x004ce9e9
void FUN_004ce9e9(void)
{
  SetSystemPaletteUse(global_main_hdc, 1);
}

// FUNCTION: SHANDALAR 0x004ce8cd
void CALLBACK FUN_004ce8cd(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
  (void)uID;
  (void)uMsg;
  (void)dwUser;
  (void)dw1;
  (void)dw2;

  if (DAT_005b7d94 == 0)
  {
    DAT_005b7d94 = 1;
    DAT_00748424 = GetCurrentThread();
    DuplicateHandle(GetCurrentProcess(), DAT_00748424, GetCurrentProcess(), &DAT_00748424, 0x1f03ff, FALSE, 0);
  }

  ++DAT_00589df0;

  if (DAT_00748400 == 0)
  {
    DAT_00748400 = 1;
    FUN_0056d476();
    DAT_00748400 = 0;
  }
}

// FUNCTION: SHANDALAR 0x004ce61a
LRESULT CALLBACK FUN_004ce61a(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg)
  {
  case 2:
    FUN_0056d081();
    PostQuitMessage(0);
    return DefWindowProcA(hwnd, 2, wparam, lparam);
  case 0x100:
  case 0x104:
    QueueKeyInputFromMessage(wparam, lparam);
    if (wparam != 0x7a)
    {
      return DefWindowProcA(hwnd, msg, wparam, lparam);
    }

    if (DAT_005b7d90 == 0)
    {
      RegisterPaletteClass(DAT_00939160);
      DAT_005b7d90 = 1;
    }

    DAT_005b7d98 = FindWindowExA((HWND)0, (HWND)0, s_ShowPaletteClass_0058a154, s_Current_Palette_0058a144);
    if (DAT_005b7d98 == (HWND)0)
    {
      DAT_005b7d98 = CreatePalettePopupWindow(DAT_00939160, (void *)0);
      if (DAT_005b7d98 == (HWND)0)
      {
        return 0;
      }
      ShowWindow(DAT_005b7d98, 5);
    }
    else
    {
      BringWindowToTop(DAT_005b7d98);
    }

    UpdateWindow(DAT_005b7d98);
    return DefWindowProcA(hwnd, msg, 0x7a, lparam);
  case 0x200:
    DAT_00986d9c = (int)((unsigned int)lparam & 0xffff);
    DAT_00986d98 = (int)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;
  case 0x201:
    DAT_00986d94 = 1;
    DAT_00986d9c = (int)((unsigned int)lparam & 0xffff);
    DAT_00986d98 = (int)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;
  case 0x202:
    DAT_00986da0 |= 2;
    DAT_00986d94 = 0;
    break;
  case 0x204:
    DAT_00986d94 = 2;
    DAT_00986d9c = (int)((unsigned int)lparam & 0xffff);
    DAT_00986d98 = (int)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;
  case 0x205:
    DAT_00986da0 |= 1;
    DAT_00986d94 = 0;
    break;
  case 0x10101010:
    FUN_0056d476();
    return 0;
  }

  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

/*
 * Main entrypoint for shandalar.exe.
 * Decompilation/matching work will replace/refine this as we recurse into callees.
 */
// FUNCTION: SHANDALAR 0x004cdd70
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
  int custom_mode_selected;
  HWND existing_main;
  HWND magic_shell_hwnd;
  int horzres;
  char module_path[100];
  char game_dir[256];
  char cmd_copy[100];
  char *slash;
  WNDCLASSA wndclass;
  HWND main_hwnd;
  MSG msg;
  DWORD thread_id;
  DWORD sectors_per_cluster;
  DWORD bytes_per_sector;
  DWORD number_of_free_clusters;
  DWORD total_number_of_clusters;
  int disk_free_bytes;
  int tick;
  int show_shell_on_exit;
  int resolution_option_matched;
  HDC screen_hdc;
  int event_timer_ok;
  HPALETTE stock_palette;

  (void)prevInstance;

  custom_mode_selected = 0;
  strcpy(cmd_copy, cmdLine);
  DAT_0078df78 = 0;

  existing_main = FindWindowA(s_ShandalarMainClass_00589e04, (LPCSTR)0);
  if (existing_main != (HWND)0)
  {
    ShowWindow(existing_main, 9);
    SetForegroundWindow(existing_main);
    return 0;
  }

  magic_shell_hwnd = FindWindowA(s_Magic_Shell_00589e18, (LPCSTR)0);
  if (magic_shell_hwnd != (HWND)0 && _strnicmp(cmdLine, s__MTGshell_00589e24, 9) != 0)
  {
    PostMessageA(magic_shell_hwnd, 0x400, 2, 0);
    return 0;
  }

  if (InitLicenseSecretsFromRegistry() != 0)
  {
    return 0;
  }

  GetWindowsDirectoryA(module_path, 0x100);
  module_path[3] = 0;
  GetDiskFreeSpaceA(module_path, &sectors_per_cluster, &bytes_per_sector, &number_of_free_clusters,
                    &total_number_of_clusters);

  disk_free_bytes = total_number_of_clusters * sectors_per_cluster * number_of_free_clusters;
  tick = disk_free_bytes + ((unsigned int)disk_free_bytes >> 0x1f & 0x1fU);
  tick = (tick >> 5) + ((unsigned int)tick >> 0x1f & 0x3ffU);

  if (((tick >> 10) + ((unsigned int)tick >> 0x1f & 0x3ffU)) >> 10 == 0)
  {
    MessageBoxA((HWND)0, s_We_have_determined_that_you_may_n_00589e54, s_Low_Memory_Swap_File_Space_Warni_00589e30,
                0x30);
  }

  srand(GetTickCount());
  FUN_00464663(DAT_0078e5f0);
  _chdir(DAT_0078e5f0);

  if (FUN_00564ee7(s_AdvStrings_txt_00589f4c) == 0)
  {
    MessageBoxA((HWND)0, s_Couldn_t_load_the_strings_from_t_00589f5c, (LPCSTR)0, 0x1010);
    return 0;
  }

  if (FUN_00565dbc(s_AdvBlocks_txt_00589f94) == 0)
  {
    MessageBoxA((HWND)0, s_Couldn_t_load_the_strings_from_t_00589fa4, (LPCSTR)0, 0x1010);
    return 0;
  }

  FUN_00559999();

  /* Switch to executable directory */
  strcpy(game_dir, (*__p___argv())[0]);
  slash = strrchr(game_dir, '\\');
  if (slash)
  {
    *slash = 0;
  }
  _chdir(game_dir);

  DAT_005a1608 = 0;
  DAT_00939160 = hInstance;

  memset(&wndclass, 0, sizeof(wndclass));
  wndclass.style = 0x23;
  wndclass.lpfnWndProc = FUN_004ce61a;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = LoadIconA(hInstance, (LPCSTR)0x65);
  wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  wndclass.hbrBackground = GetStockObject(4);
  wndclass.lpszClassName = s_ShandalarMainClass_00589fdc;

  if (RegisterClassA(&wndclass) == 0)
  {
    MessageBoxA((HWND)0, s_Couldn_t_register_the_classes_00589ff0, (LPCSTR)0, 0x1010);
    return 0;
  }

  atexit(RestoreDisplayResolution);

  screen_hdc = GetDC((HWND)0);
  horzres = GetDeviceCaps(screen_hdc, 8);
  if (0x400 < horzres)
  {
    ChangeDisplayResolution(0x400, 0x300);
  }

  /* Command line resolution override parsing (still being refined for matching) */
  resolution_option_matched = 0;
  show_shell_on_exit = 0;
  if (cmdLine && cmdLine[0] && cmdLine[1])
  {
    if (cmdLine[1] == '6')
    {
      DAT_005863b8 = 0x280;
      DAT_005863bc = 0x1e0;
      PTR_s_advinter800_pic_00589de8 = s_advinter_pic_0058a010;
      CreateGraphicsPage(0, 0x280, 0x1e0, -1);
      show_shell_on_exit = 1;
      resolution_option_matched = 1;
    }
    else if (cmdLine[1] == '8')
    {
      DAT_005863b8 = 800;
      DAT_005863bc = 600;
      PTR_s_advinter800_pic_00589de8 = s_advinter800_pic_0058a020;
      CreateGraphicsPage(0, 800, 600, -1);
      show_shell_on_exit = 1;
      resolution_option_matched = 1;
    }
    else if (cmdLine[1] == '1')
    {
      DAT_005863b8 = 0x400;
      DAT_005863bc = 0x300;
      PTR_s_advinter800_pic_00589de8 = s_advinter1024_pic_0058a030;
      CreateGraphicsPage(0, 0x400, 0x300, -1);
      show_shell_on_exit = 1;
      resolution_option_matched = 1;
    }
  }

  if (!resolution_option_matched)
  {
    screen_hdc = GetDC((HWND)0);
    horzres = GetDeviceCaps(screen_hdc, 8);
    if (horzres == 0x280)
    {
      DAT_005863b8 = 0x280;
      DAT_005863bc = 0x1e0;
      PTR_s_advinter800_pic_00589de8 = s_advinter_pic_0058a044;
    }
    else if (horzres == 800)
    {
      DAT_005863b8 = 800;
      DAT_005863bc = 600;
      PTR_s_advinter800_pic_00589de8 = s_advinter800_pic_0058a054;
    }
    else
    {
      DAT_005863b8 = 0x400;
      DAT_005863bc = 0x300;
      PTR_s_advinter800_pic_00589de8 = s_advinter1024_pic_0058a064;
    }
  }

  if (PTR_DAT_005832b4)
  {
    *(int *)(PTR_DAT_005832b4 + 0xc) = DAT_005863b8 - 1;
    *(int *)(PTR_DAT_005832b4 + 0x10) = DAT_005863bc - 1;
  }

  main_hwnd = CreateWindowExA(8, s_ShandalarMainClass_0058a08c, s_Magic__Shandalar_0058a078, 0x80000000, 0, 0,
                              DAT_005863b8, DAT_005863bc, (HWND)0, (HMENU)0, hInstance, (LPVOID)0);
  DAT_00748420 = main_hwnd;
  ShowWindow(main_hwnd, nShowCmd);
  global_main_hdc = GetDC(main_hwnd);

  if (show_shell_on_exit)
  {
    *(HDC *)(DAT_00986950 + 4) = global_main_hdc;
    SelectPalette(*(HDC *)(DAT_00986950 + 4), *(HPALETTE *)(DAT_00986950 + 0x14), FALSE);
    RealizePalette(*(HDC *)(DAT_00986950 + 4));
    SetStretchBltMode(*(HDC *)(DAT_00986950 + 4), 3);
  }

  FUN_004cea02();
  FUN_0056cf20((int)main_hwnd, 0, 1);

  if (DAT_00591210 == 0)
  {
    FUN_00562d03();
  }

  timeBeginPeriod(DAT_00748418);
  DAT_007483fc = timeSetEvent(DAT_00589de4, DAT_00748418, (LPTIMECALLBACK)FUN_004ce8cd, 0, 1);
  if (DAT_007483fc == 0)
  {
    assert(0, s_D__Newmagic_multiplayer_sid_Test_0058a0c0, 0x15a, s_Timer_failed_to_initialize__0058a0a0);
  }
  atexit(FUN_004ce9c6);
  assert((unsigned int)(DAT_007483fc != 0xffffffff), s_D__Newmagic_multiplayer_sid_Test_0058a0fc, 0x15c,
         s_Could_not_start_timer_0058a0e4);

  SetSystemPaletteUse(global_main_hdc, 2);
  atexit(FUN_004ce9e9);

  DAT_00748404 = GetCurrentThread();
  DuplicateHandle(GetCurrentProcess(), DAT_00748404, GetCurrentProcess(), &DAT_00748404, 0x1f03ff, FALSE, 0);

  if (DAT_0078df78 == 0)
  {
    InitializeCriticalSection(&DAT_0093aa40);
    DAT_0078df78 = 1;
  }

  _DAT_00748410 = CreateThread((LPSECURITY_ATTRIBUTES)0, 0x2000, FUN_0046e6f0, (LPVOID)0, 0, &thread_id);

  while (GetMessageA(&msg, (HWND)0, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }

  if (DAT_0078df78 != 0)
  {
    DeleteCriticalSection(&DAT_0093aa40);
    DAT_0078df78 = 0;
  }

  FUN_00565faa();
  ChangeDisplayResolution(0, 0);

  screen_hdc = GetDC((HWND)0);
  stock_palette = GetStockObject(0xf);
  SelectPalette(screen_hdc, stock_palette, FALSE);
  SetSystemPaletteUse(screen_hdc, 1);
  RealizePalette(screen_hdc);
  ReleaseDC((HWND)0, screen_hdc);

  if (_strnicmp(cmd_copy, s__MTGshell_0058a120, 8) == 0)
  {
    char exe_cmd[264];

    strcpy(exe_cmd, DAT_0078e5f0);
    strcat(exe_cmd, s__Magic_exe_0058a12c);
    strcat(exe_cmd, s__start3_1_0058a138);
    WinExec(exe_cmd, 5);
  }

  return (int)msg.wParam;
}
