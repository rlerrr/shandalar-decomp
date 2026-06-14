#include <windows.h>
#include <mmsystem.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>

#include "cardartlib/src/assert.h"

// GLOBAL: SHANDALAR 0x007483f8
HDC global_main_hdc;

// GLOBAL: SHANDALAR 0x00589e04
char s_ShandalarMainClass_00589e04[] = "ShandalarMainClass";
// GLOBAL: SHANDALAR 0x00589e18
char s_Magic_Shell_00589e18[] = "Magic Shell";
// GLOBAL: SHANDALAR 0x00589e24
char s__MTGshell_00589e24[] = "/MTGshell";
// GLOBAL: SHANDALAR 0x00589e30
char s_Low_Memory_Swap_File_Space_Warni_00589e30[] = "Low Memory/Swap File Space Warning";
// GLOBAL: SHANDALAR 0x00589e54
char s_We_have_determined_that_you_may_n_00589e54[] =
    "We have determined that you may not have enough free swap file space to play Magic: The Gathering reliably.\n"
    "This is just a warning, and you will be able to continue from here, but you may want to free space on your windows drive before you play.";

// GLOBAL: SHANDALAR 0x00589f4c
char s_AdvStrings_txt_00589f4c[] = "AdvStrings.txt";
// GLOBAL: SHANDALAR 0x00589f5c
char s_Couldn_t_load_the_strings_from_t_00589f5c[] =
    "Couldn't load the strings from the AdvStrings.TXT file";
// GLOBAL: SHANDALAR 0x00589f94
char s_AdvBlocks_txt_00589f94[] = "AdvBlocks.txt";
// GLOBAL: SHANDALAR 0x00589fa4
char s_Couldn_t_load_the_strings_from_t_00589fa4[] =
    "Couldn't load the strings from the AdvBlocks.TXT file";

// GLOBAL: SHANDALAR 0x00589fdc
char s_ShandalarMainClass_00589fdc[] = "ShandalarMainClass";
// GLOBAL: SHANDALAR 0x00589ff0
char s_Couldn_t_register_the_classes_00589ff0[] = "Couldn't register the classes";

// GLOBAL: SHANDALAR 0x0058a010
char s_advinter_pic_0058a010[] = "advinter.pic";
// GLOBAL: SHANDALAR 0x0058a020
char s_advinter800_pic_0058a020[] = "advinter800.pic";
// GLOBAL: SHANDALAR 0x0058a030
char s_advinter1024_pic_0058a030[] = "advinter1024.pic";
// GLOBAL: SHANDALAR 0x0058a044
char s_advinter_pic_0058a044[] = "advinter.pic";
// GLOBAL: SHANDALAR 0x0058a054
char s_advinter800_pic_0058a054[] = "advinter800.pic";
// GLOBAL: SHANDALAR 0x0058a064
char s_advinter1024_pic_0058a064[] = "advinter1024.pic";

// GLOBAL: SHANDALAR 0x0058a078
char s_Magic__Shandalar_0058a078[] = "Magic: Shandalar";
// GLOBAL: SHANDALAR 0x0058a08c
char s_ShandalarMainClass_0058a08c[] = "ShandalarMainClass";

// GLOBAL: SHANDALAR 0x0058a0a0
char s_Timer_failed_to_initialize__0058a0a0[] = "Timer failed to initialize!\n";
// GLOBAL: SHANDALAR 0x0058a0c0
char s_D__Newmagic_multiplayer_sid_Test_0058a0c0[] = "D:\\Newmagic\\multiplayer\\sid\\Test.c";
// GLOBAL: SHANDALAR 0x0058a0e4
char s_Could_not_start_timer_0058a0e4[] = "Could not start timer";
// GLOBAL: SHANDALAR 0x0058a0fc
char s_D__Newmagic_multiplayer_sid_Test_0058a0fc[] = "D:\\Newmagic\\multiplayer\\sid\\Test.c";

// GLOBAL: SHANDALAR 0x0058a120
char s__MTGshell_0058a120[] = "/MTGshell";
// GLOBAL: SHANDALAR 0x0058a12c
char s__Magic_exe_0058a12c[] = "\\Magic.exe";
// GLOBAL: SHANDALAR 0x0058a138
char s__start3_1_0058a138[] = " /start3,1";

// GLOBAL: SHANDALAR 0x0058a144
char s_Current_Palette_0058a144[] = "Current Palette";
// GLOBAL: SHANDALAR 0x0058a154
char s_ShowPaletteClass_0058a154[] = "ShowPaletteClass";

// GLOBAL: SHANDALAR 0x0058a168
char s_sound_locmus1_wav_0058a168[] = "sound\\locmus1.wav";

// GLOBAL: SHANDALAR 0x0078df78
int DAT_0078df78;
// GLOBAL: SHANDALAR 0x0078e5f0
char DAT_0078e5f0[260];

// GLOBAL: SHANDALAR 0x005863b8
int DAT_005863b8;
// GLOBAL: SHANDALAR 0x005863bc
int DAT_005863bc;
// GLOBAL: SHANDALAR 0x00589de8
char *PTR_s_advinter800_pic_00589de8;
// GLOBAL: SHANDALAR 0x005832b4
unsigned char *PTR_DAT_005832b4;

// GLOBAL: SHANDALAR 0x00748420
HWND DAT_00748420;
// GLOBAL: SHANDALAR 0x00939160
HINSTANCE DAT_00939160;
// GLOBAL: SHANDALAR 0x005a1608
int DAT_005a1608;
// GLOBAL: SHANDALAR 0x00986950
unsigned char *DAT_00986950;
// GLOBAL: SHANDALAR 0x00591210
int DAT_00591210;
// GLOBAL: SHANDALAR 0x00748418
UINT DAT_00748418;
// GLOBAL: SHANDALAR 0x007483fc
UINT DAT_007483fc;
// GLOBAL: SHANDALAR 0x00589de4
UINT DAT_00589de4;
// GLOBAL: SHANDALAR 0x00589df0
int DAT_00589df0;
// GLOBAL: SHANDALAR 0x00748400
int DAT_00748400;
// GLOBAL: SHANDALAR 0x00748404
HANDLE DAT_00748404;
// GLOBAL: SHANDALAR 0x00748424
HANDLE DAT_00748424;
// GLOBAL: SHANDALAR 0x00748408
unsigned int DAT_00748408;
// GLOBAL: SHANDALAR 0x0074840c
unsigned int _DAT_0074840c;
// GLOBAL: SHANDALAR 0x00748410
HANDLE _DAT_00748410;

// GLOBAL: SHANDALAR 0x005b7d90
int DAT_005b7d90;
// GLOBAL: SHANDALAR 0x005b7d94
int DAT_005b7d94;
// GLOBAL: SHANDALAR 0x005b7d98
HWND DAT_005b7d98;

// GLOBAL: SHANDALAR 0x00986d94
int DAT_00986d94;
// GLOBAL: SHANDALAR 0x00986d98
int DAT_00986d98;
// GLOBAL: SHANDALAR 0x00986d9c
int DAT_00986d9c;
// GLOBAL: SHANDALAR 0x00986da0
int DAT_00986da0;

// GLOBAL: SHANDALAR 0x0093aa40
CRITICAL_SECTION DAT_0093aa40;

// GLOBAL: SHANDALAR 0x00589de4
// NOTE: hardcoded in binary; keep as a global for now.

int InitLicenseSecretsFromRegistry(void);
void FUN_00464663(char *out_dir);
int FUN_00564ee7(const char *filename);
int FUN_00565dbc(const char *filename);
void FUN_00559999(void);
void *CreateGraphicsPage(int page_number, int width, int height, int bits_per_pixel);
int FUN_0056cf20(int hwnd_as_int, int param_2, int param_3);
void FUN_00562d03(void);
void FUN_00565faa(void);
char ***__cdecl __p___argv(void);
void FUN_0056d081(void);
void QueueKeyInputFromMessage(WPARAM wparam, LPARAM lparam);
ATOM RegisterPaletteClass(HINSTANCE hinst);
HWND CreatePalettePopupWindow(HINSTANCE hinst, HWND parent_hwnd);
void FUN_0056d476(void);
int FUN_00417dc6(const char *filename);
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
  (void)filename;
  return 1;
}

// FUNCTION: SHANDALAR 0x00565dbc
int FUN_00565dbc(const char *filename)
{
  (void)filename;
  return 1;
}

// FUNCTION: SHANDALAR 0x00559999
void FUN_00559999(void)
{
}

// FUNCTION: SHANDALAR 0x0056cf20
int FUN_0056cf20(int hwnd_as_int, int param_2, int param_3)
{
  (void)hwnd_as_int;
  (void)param_2;
  (void)param_3;
  return 0;
}

// FUNCTION: SHANDALAR 0x00562d03
void FUN_00562d03(void)
{
}

// FUNCTION: SHANDALAR 0x00565faa
void FUN_00565faa(void)
{
}

// FUNCTION: SHANDALAR 0x0056d081
void FUN_0056d081(void)
{
}

// FUNCTION: SHANDALAR 0x0056d476
void FUN_0056d476(void)
{
}

// FUNCTION: SHANDALAR 0x00417dc6
int FUN_00417dc6(const char *filename)
{
  (void)filename;
  return 1;
}

// FUNCTION: SHANDALAR 0x0046e6f0
DWORD WINAPI FUN_0046e6f0(LPVOID param_1)
{
  (void)param_1;
  return 0;
}

// FUNCTION: SHANDALAR 0x004cea4c
// (Also present in FACEMAKER at 0x004061bd as ChangeDisplayResolution.)
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
// (Also present in FACEMAKER at 0x0040623c as RestoreDisplayResolution.)
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
  if (msg < 0x10)
  {
    if (msg == 0x0f)
    {
      return DefWindowProcA(hwnd, msg, wparam, lparam);
    }

    if (msg == 2)
    {
      FUN_0056d081();
      PostQuitMessage(0);
      return DefWindowProcA(hwnd, 2, wparam, lparam);
    }
  }
  else if (msg < 0x105)
  {
    if (msg == 0x104 || msg == 0x100)
    {
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
    }
  }
  else if (msg < 0x10101011)
  {
    if (msg == 0x10101010)
    {
      FUN_0056d476();
      return 0;
    }

    switch (msg)
    {
    case 0x200:
      DAT_00986d9c = (int)(lparam & 0xffff);
      DAT_00986d98 = (int)((unsigned int)lparam >> 0x10);
      break;
    case 0x201:
      DAT_00986d94 = 1;
      DAT_00986d9c = (int)(lparam & 0xffff);
      DAT_00986d98 = (int)((unsigned int)lparam >> 0x10);
      break;
    case 0x202:
      DAT_00986da0 |= 2;
      DAT_00986d94 = 0;
      break;
    case 0x204:
      DAT_00986d94 = 2;
      DAT_00986d9c = (int)(lparam & 0xffff);
      DAT_00986d98 = (int)((unsigned int)lparam >> 0x10);
      break;
    case 0x205:
      DAT_00986da0 |= 1;
      DAT_00986d94 = 0;
      break;
    default:
      return DefWindowProcA(hwnd, msg, wparam, lparam);
    }

    return DefWindowProcA(hwnd, msg, wparam, lparam);
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
