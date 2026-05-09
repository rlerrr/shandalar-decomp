#include <direct.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "drawcardlib/src/pic.h"
#include "game_support.h"
#include "manalinkinterface/manalinkinterface.h"
#include "deckdll/src/shared_resources.h"

#define STARTUP_DIALOG_COUNT 5
#define STARTUP_DIALOG_ROW_STRIDE 0x8b8
#define STARTUP_DIALOG_CHOICE_OFFSET 0x10c
#define STARTUP_DIALOG_CHOICE_STRIDE 0x48
#define STARTUP_DIALOG_COUNT_OFFSET 0x8b4

#define STARTUP_DIALOG_ROW(group_) \
  (&DAT_007a7d90[(group_) * STARTUP_DIALOG_ROW_STRIDE])
#define STARTUP_DIALOG_CHOICE_COUNT(group_) \
  (*(int *)(STARTUP_DIALOG_ROW(group_) + STARTUP_DIALOG_COUNT_OFFSET))
#define STARTUP_DIALOG_CHOICE_PRESENT(group_, choice_) \
  (*(int *)(STARTUP_DIALOG_ROW(group_) + STARTUP_DIALOG_CHOICE_OFFSET + (choice_) * STARTUP_DIALOG_CHOICE_STRIDE))

// GLOBAL: MAGIC 0x00789460
char DAT_Magic_The_Gathering_00789460[300];

// GLOBAL: MAGIC 0x0079141c
HWND DAT_0079141c;

// GLOBAL: MAGIC 0x0079154c
HANDLE global_mutex_UpdateLowerDialog;

// GLOBAL: MAGIC 0x007a7d90
unsigned char DAT_007a7d90[STARTUP_DIALOG_COUNT * STARTUP_DIALOG_ROW_STRIDE];

// GLOBAL: MAGIC 0x0074b62c
int DAT_0074b62c;

// GLOBAL: MAGIC 0x007775b4
int DAT_007775b4;

// GLOBAL: MAGIC 0x008950b0
HWND global_main_hwnd;

// GLOBAL: MAGIC 0x008950c0
char global_duelsounds_path[300];

// GLOBAL: MAGIC 0x00895204
int DAT_00895204;

// GLOBAL: MAGIC 0x008962f0
char DAT_UIStrings_txt_008962f0[300];

// GLOBAL: MAGIC 0x008b3bd8
int DAT_008b3bd8;

// GLOBAL: MAGIC 0x008b4dd4
int DAT_008b4dd4;

// GLOBAL: MAGIC 0x008cc704
HANDLE global_mutex_GameInit;

// GLOBAL: MAGIC 0x008a98f0
char global_savegame_path[300];

// GLOBAL: MAGIC 0x008b40d0
extern int global_available_slots;

// GLOBAL: MAGIC 0x008b44e0
char global_faces_path[300];

// GLOBAL: MAGIC 0x008ce540
char DAT_008ce540[10];

// GLOBAL: MAGIC 0x008ce54a
char DAT_008ce54a[10];

// GLOBAL: MAGIC 0x008ce554
char DAT_008ce554[10];

// GLOBAL: MAGIC 0x008ce55e
char DAT_008ce55e[10];

// GLOBAL: MAGIC 0x008cff0c
int DAT_008cff0c;

// GLOBAL: MAGIC 0x008cff14
HANDLE global_mutex_LowerDialog;

// GLOBAL: MAGIC 0x008cefb0
void *DAT_008cefb0;

// GLOBAL: MAGIC 0x0091bbd0
int DAT_0091bbd0;

extern int DAT_00896714;
extern int DAT_0091c0f0;

// GLOBAL: MAGIC 0x0091c4f8
int DAT_0091c4f8;

// GLOBAL: MAGIC 0x0091ce40
char DAT_0091ce40[300];

// GLOBAL: MAGIC 0x00926100
char global_playdeck_path[300];

// GLOBAL: MAGIC 0x00926808
HBITMAP DAT_00926808;

// GLOBAL: MAGIC 0x009266b0
CRITICAL_SECTION DAT_009266b0;

// GLOBAL: MAGIC 0x00926910
CRITICAL_SECTION DAT_00926910;

// GLOBAL: MAGIC 0x0093932c
HANDLE global_mutex_WritePacket;

// GLOBAL: MAGIC 0x00939334
int DAT_00939334;

// GLOBAL: MAGIC 0x0093a980
HGDIOBJ DAT_0093a980;

// GLOBAL: MAGIC 0x00789310
HDC DAT_00789310;

// GLOBAL: MAGIC 0x008cf290
char global_duelart_path[300];

typedef DWORD(WINAPI *get_file_version_info_size_a_t)(LPCSTR filename, LPDWORD handle);
typedef BOOL(WINAPI *get_file_version_info_a_t)(LPCSTR filename, DWORD handle, DWORD len, LPVOID data);
typedef BOOL(WINAPI *ver_query_value_a_t)(const LPVOID block, LPCSTR sub_block, LPVOID *buffer, PUINT len);

int InitLicenseSecretsFromRegistry(void);
static void load_version_string(char *out_string, const char *filename);
static void append_startup_error(char *message_buffer, const char *path, int line_index);
static void set_global_base_directory(char *path);
static int setup_paths_and_load_text_etc(char *message_buffer);
static void FUN_00491f1e(char *filename);
static int FUN_004537d8(const char *filename);
static int FUN_004c0c20(const char *filename);
static int FUN_00509210(void);
static int FUN_004e027c(void);
static void FUN_0048fcb5(void);
static int FUN_00497c8d(void);
static void FUN_00500c56(void);
static void FUN_00500d46(void);
static void FUN_00422bea(void);
static int FUN_004a7b3d(void);
static void FUN_00459b6e(void *window);
static LRESULT CALLBACK FUN_005539c3(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int read_db_guts(char *cards_dat_filename);
void InitBitmapInfo24bppTopDown(BITMAPINFO *bmi, int width, int height);

static __inline void load_version_string(char *out_string, const char *filename)
{
  HMODULE version_dll;
  get_file_version_info_size_a_t pGetFileVersionInfoSizeA;
  get_file_version_info_a_t pGetFileVersionInfoA;
  ver_query_value_a_t pVerQueryValueA;
  DWORD handle;
  DWORD version_size;
  void *version_info;
  char *version_string;
  UINT version_length;

  out_string[0] = '\0';

  version_dll = LoadLibraryA("version.dll");
  if (version_dll == NULL)
  {
    return;
  }

  pGetFileVersionInfoSizeA = (get_file_version_info_size_a_t)GetProcAddress(version_dll, "GetFileVersionInfoSizeA");
  pGetFileVersionInfoA = (get_file_version_info_a_t)GetProcAddress(version_dll, "GetFileVersionInfoA");
  pVerQueryValueA = (ver_query_value_a_t)GetProcAddress(version_dll, "VerQueryValueA");
  if (pGetFileVersionInfoSizeA == NULL || pGetFileVersionInfoA == NULL || pVerQueryValueA == NULL)
  {
    FreeLibrary(version_dll);
    return;
  }

  version_size = pGetFileVersionInfoSizeA(filename, &handle);
  if (version_size == 0)
  {
    FreeLibrary(version_dll);
    return;
  }

  version_info = malloc(version_size);
  if (version_info == NULL)
  {
    FreeLibrary(version_dll);
    return;
  }

  if (pGetFileVersionInfoA(filename, 0, version_size, version_info) &&
      pVerQueryValueA(version_info, "\\StringFileInfo\\040904b0\\FileVersion", (LPVOID *)&version_string, &version_length))
  {
    strcpy(out_string, version_string);
  }

  free(version_info);
  FreeLibrary(version_dll);
}

static __inline void append_startup_error(char *message_buffer, const char *path, int line_index)
{
  sprintf(message_buffer + strlen(message_buffer), text_lines[line_index], path);
  strcat(message_buffer, "\n");
}

// FUNCTION: MAGIC 0x00493bc3
static void set_global_base_directory(char *path)
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

// FUNCTION: MAGIC 0x005532e9
void FUN_005532e9(void)
{
  load_version_string(DAT_008ce540, "magic.exe");
  load_version_string(DAT_008ce54a, "manalink.exe");
  load_version_string(DAT_008ce554, "manalinkInterface.dll");
  load_version_string(DAT_008ce55e, "deckdll.dll");
}

// FUNCTION: MAGIC 0x0055357b
unsigned int FUN_0055357b(HWND hwnd, MSG *msg)
{
  CHAR text[100];
  int accelerator_index;
  int key_index;
  char *ampersand;
  HWND dialog_item;

  if (!IsWindowVisible(hwnd))
  {
    return 0;
  }

  if (msg->message == WM_KEYDOWN || msg->message == WM_KEYUP || msg->message == WM_SYSKEYDOWN ||
      msg->message == WM_SYSKEYUP)
  {
    accelerator_index = (int)msg->wParam;

    switch (accelerator_index)
    {
    case VK_F1:
    case VK_F2:
    case VK_F3:
    case VK_F4:
    case VK_F5:
      if (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN)
      {
        SendMessageA(hwnd, WM_COMMAND, accelerator_index - 0x3e, 0);
      }
      return 1;

    case VK_PRIOR:
    case VK_NEXT:
    case VK_END:
    case VK_HOME:
      if ((msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN) &&
          DAT_008b3bd8 != -1)
      {
        dialog_item = GetDlgItem(hwnd, 99);
        if (dialog_item != NULL && IsWindowVisible(dialog_item))
        {
          if (accelerator_index == VK_PRIOR)
          {
            SendMessageA(hwnd, WM_VSCROLL, 0, (LPARAM)dialog_item);
          }
          else if (accelerator_index == VK_NEXT)
          {
            SendMessageA(hwnd, WM_VSCROLL, 1, (LPARAM)dialog_item);
          }
          else if (accelerator_index == VK_HOME)
          {
            SendMessageA(hwnd, WM_VSCROLL, 6, (LPARAM)dialog_item);
          }
          else if (accelerator_index == VK_END)
          {
            SendMessageA(hwnd, WM_VSCROLL, 7, (LPARAM)dialog_item);
          }
        }
      }
      return 1;

    default:
      return 0;
    }
  }

  if (msg->message == WM_SYSCHAR)
  {
    if (DAT_008b3bd8 == -1)
    {
      return 0;
    }

    accelerator_index = -1;
    for (key_index = 0; key_index < STARTUP_DIALOG_CHOICE_COUNT(DAT_008b3bd8) &&
                        accelerator_index == -1;
         ++key_index)
    {
      GetDlgItemTextA(hwnd, key_index + 100, text, sizeof(text));
      ampersand = text;
      while (ampersand != NULL && *ampersand != '\0' && accelerator_index == -1)
      {
        ampersand = strchr(ampersand, '&');
        if (ampersand != NULL)
        {
          ++ampersand;
          if ((char)CharLowerA((LPSTR)msg->wParam) == *ampersand ||
              (char)CharUpperA((LPSTR)msg->wParam) == *ampersand)
          {
            accelerator_index = key_index;
          }
        }
      }
    }

    if (accelerator_index != -1)
    {
      SendMessageA(hwnd, WM_COMMAND, accelerator_index + 100, 0);
      return 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0055392b
int register_MagicShellClass(LPCSTR class_name)
{
  WNDCLASSA wndclass;

  wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
  wndclass.lpfnWndProc = FUN_005539c3;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0x1e;
  wndclass.hInstance = (HINSTANCE)global_hinstance;
  wndclass.hIcon = LoadIconA((HINSTANCE)global_hinstance, (LPCSTR)0x66);
  wndclass.hCursor = LoadCursorA(NULL, (LPCSTR)IDC_ARROW);
  wndclass.hbrBackground = NULL;
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = class_name;

  if (RegisterClassA(&wndclass) == 0)
    return 0;

  return 1;
}

// FUNCTION: MAGIC 0x005539b8
void FUN_005539b8(void* unused)
{
}

// FUNCTION: MAGIC 0x004226c0
static int setup_paths_and_load_text_etc(char *message_buffer)
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
  strcpy(DAT_UIStrings_txt_008962f0, "UIStrings.txt");
  strcpy(global_savegame_path, global_base_directory);
  strcat(global_savegame_path, "\\SaveGame");
  _mkdir(global_savegame_path);

  FUN_00491f1e(DAT_UIStrings_txt_008962f0);

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\CARDS.DAT");
  global_available_slots = read_db_guts(s.path);
  if (global_available_slots == 0)
  {
    s.ok = 0;
    load_text(DAT_UIStrings_txt_008962f0, "PROMPT_STARTUPERROR");
    append_startup_error(message_buffer, s.path, 1);
  }

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\LEGACY.CSV");
  if (!FUN_004537d8(s.path))
  {
    s.ok = 0;
    load_text(DAT_UIStrings_txt_008962f0, "PROMPT_STARTUPERROR");
    append_startup_error(message_buffer, s.path, 2);
  }

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\RARITY.CSV");
  if (!FUN_004c0c20(s.path))
  {
    s.ok = 0;
    load_text(DAT_UIStrings_txt_008962f0, "PROMPT_STARTUPERROR");
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
    load_text(DAT_UIStrings_txt_008962f0, "PROMPT_STARTUPERROR");
    strcat(message_buffer, text_lines[3]);
    strcat(message_buffer, "\n");
  }

  if (!SetupDuelPalette())
  {
    s.ok = 0;
    load_text(DAT_UIStrings_txt_008962f0, "PROMPT_STARTUPERROR");
    strcat(message_buffer, text_lines[4]);
    strcat(message_buffer, "\n");
  }

  if (!CreateOffscreen32bppDibSection(GetSystemMetrics(SM_CXSCREEN),
                                      GetSystemMetrics(SM_CYSCREEN),
                                      &DAT_00789310,
                                      (BITMAPINFO *)&unk_008b4330[0x40],
                                      &DAT_00926808,
                                      &DAT_0093a980,
                                      &DAT_008cefb0))
  {
    s.ok = 0;
    load_text(DAT_UIStrings_txt_008962f0, "PROMPT_STARTUPERROR");
    strcat(message_buffer, text_lines[5]);
    strcat(message_buffer, "\n");
  }

  if (!create_fonts())
  {
    s.ok = 0;
    load_text(DAT_UIStrings_txt_008962f0, "PROMPT_STARTUPERROR");
    strcat(message_buffer, text_lines[6]);
    strcat(message_buffer, "\n");
  }

  if (!InitCardArtGdiResources())
  {
    s.ok = 0;
    load_text(DAT_UIStrings_txt_008962f0, "PROMPT_STARTUPERROR");
    strcat(message_buffer, text_lines[7]);
    strcat(message_buffer, "\n");
  }

  FUN_004a7b3d();
  return s.ok;
}

// FUNCTION: MAGIC 0x00491f1e
static void FUN_00491f1e(char *filename)
{
  if (load_text(filename, "GAMETITLE") > 0)
  {
    strcpy(DAT_Magic_The_Gathering_00789460, text_lines[0]);
  }
}

// FUNCTION: MAGIC 0x004537d8
static int FUN_004537d8(const char *filename)
{
  return GetFileAttributesA(filename) != 0xffffffff;
}

// FUNCTION: MAGIC 0x004c0c20
static int FUN_004c0c20(const char *filename)
{
  return GetFileAttributesA(filename) != 0xffffffff;
}

// FUNCTION: MAGIC 0x00509210
static int FUN_00509210(void)
{
  return 1;
}

// FUNCTION: MAGIC 0x004e027c
static int FUN_004e027c(void)
{
  return 1;
}

// FUNCTION: MAGIC 0x0048fcb5
static void FUN_0048fcb5(void)
{
}

// FUNCTION: MAGIC 0x00497c8d
static int FUN_00497c8d(void)
{
  return 0;
}

// FUNCTION: MAGIC 0x00500c56
static void FUN_00500c56(void)
{
}

// FUNCTION: MAGIC 0x00500d46
static void FUN_00500d46(void)
{
}

// FUNCTION: MAGIC 0x00422bea
static void FUN_00422bea(void)
{
}

// FUNCTION: MAGIC 0x004a7b3d
static int FUN_004a7b3d(void)
{
  return 1;
}

// FUNCTION: MAGIC 0x00459b6e
static void FUN_00459b6e(void *window)
{
  (void)window;
}

// FUNCTION: MAGIC 0x005539c3
static LRESULT CALLBACK FUN_005539c3(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00552b90
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
  struct
  {
    MSG msg;
    char *slash;
    int duel_number;
    int duel_group;
    char startup_message[2000];
    HWND legacy_magic_shell;
    HWND existing_magic_shell;
    HWND fam_window;
    unsigned int startup_ok;
    int random_spin_count;
  } s;

  (void)prevInstance;
  (void)nShowCmd;

  s.fam_window = NULL;
  s.startup_ok = 1;

  unk_008cf1b4 = 0;
  DAT_0091c4f8 = 0;
  DAT_00895204 = DAT_0091c4f8;
  DAT_0091c0f0 = 0;
  DAT_008cff0c = DAT_0091c0f0;
  DAT_00896714 = 0;
  DAT_008b4dd4 = 0;
  DAT_007775b4 = DAT_008b4dd4;
  unk_00742fc4 = DAT_007775b4;
  DAT_0074b62c = 0;

  FUN_005532e9();
  if (InitLicenseSecretsFromRegistry() != 0)
  {
    return 0;
  }

  s.existing_magic_shell = FindWindowA("MAGICGAME_MagicShellClass", NULL);
  if (s.existing_magic_shell != NULL)
  {
    if (IsWindowVisible(s.existing_magic_shell))
    {
      ShowWindow(s.existing_magic_shell, SW_RESTORE);
      SetForegroundWindow(s.existing_magic_shell);
    }
    return 0;
  }

  s.legacy_magic_shell = FindWindowA("Magic Shell", NULL);
  if (s.legacy_magic_shell != NULL && _strnicmp(cmdLine, "/MTGshell", 9) != 0)
  {
    PostMessageA(s.legacy_magic_shell, WM_USER, 1, 0);
    return 0;
  }

  srand(GetTickCount());
  
  for (s.random_spin_count = 0;s.random_spin_count < rand() % 50000; ++s.random_spin_count)
  {
    rand();
  }

  global_hinstance = hInstance;
  s.startup_message[0] = '\0';

  InitializeCriticalSection(&DAT_00926910);
  s.startup_ok &= setup_paths_and_load_text_etc(s.startup_message);

  strcpy(DAT_0091ce40, global_base_directory);
  strcat(DAT_0091ce40, "\\ShellArt");

  global_mutex_GameInit = CreateMutexA(NULL, FALSE, "GameInit");
  global_mutex_ReadPacket = CreateMutexA(NULL, FALSE, "ReadPacket");
  global_mutex_WritePacket = CreateMutexA(NULL, FALSE, "WritePacket");
  global_mutex_UpdateLowerDialog = CreateMutexA(NULL, FALSE, "UpdateLowerDialog");
  global_mutex_LowerDialog = CreateMutexA(NULL, TRUE, "LowerDialog");
  unk_0092607c = (int)CreateEventA(NULL, TRUE, TRUE, NULL);

  if (FUN_004e027c() == 0)
  {
    MessageBoxA(NULL, "Deckbuilder.dat is either missing or currupt!", "Magic: The Gathering", MB_ICONHAND);
    return 0;
  }

  FUN_0048fcb5();
  if (FUN_00497c8d() != 0)
  {
    WinExec("manalink.exe RUNFROMSHELL", SW_SHOW);
  }

  FamInterface_SetDuelVersion(1, 3);
  FUN_00500c56();
  InitializeCriticalSection(&DAT_009266b0);

  if (register_MagicShellClass("MAGICGAME_MagicShellClass"))
  {
    global_main_hwnd = CreateWindowExA(0,
                                   "MAGICGAME_MagicShellClass",
                                   DAT_Magic_The_Gathering_00789460,
                                   0x90040000,
                                   1,
                                   0,
                                   GetSystemMetrics(SM_CXSCREEN) - 1,
                                   GetSystemMetrics(SM_CYSCREEN),
                                   NULL,
                                   NULL,
                                   (HINSTANCE)global_hinstance,
                                   NULL);
    if (global_main_hwnd == NULL)
    {
      load_text(DAT_UIStrings_txt_008962f0, "PROMPT_STARTUPERROR");
      strcat(s.startup_message, text_lines[8]);
      strcat(s.startup_message, "\n");
      s.startup_ok = 0;
    }
  }
  else
  {
    load_text(DAT_UIStrings_txt_008962f0, "PROMPT_STARTUPERROR");
    strcat(s.startup_message, text_lines[8]);
    strcat(s.startup_message, "\n");
    s.startup_ok = 0;
  }

  if (!s.startup_ok)
  {
    MessageBoxA(NULL, s.startup_message, DAT_Magic_The_Gathering_00789460, 0x1030);
    return 0;
  }

  _beginthread(FUN_00459b6e, 0, global_main_hwnd);

  s.slash = cmdLine;
  while (*s.slash != '\0' && (s.slash = strchr(s.slash, '/')) != NULL)
  {
    if (_strnicmp(s.slash, "/start", 6) == 0)
    {
      s.slash += 6;
      s.duel_group = 0;
      while (*s.slash != '\0' && (*s.slash < '0' || *s.slash > '9'))
      {
        ++s.slash;
      }
      if (*s.slash != '\0')
      {
        s.duel_group = *s.slash - '0';
        ++s.slash;
      }

      s.duel_number = 0;
      while (*s.slash != '\0' && (*s.slash < '0' || *s.slash > '9'))
      {
        ++s.slash;
      }
      if (*s.slash != '\0')
      {
        s.duel_number = *s.slash - '0';
        ++s.slash;
      }

      if (s.duel_group > 0 && s.duel_group <= DAT_0091bbd0)
      {
        PostMessageA(global_main_hwnd, WM_COMMAND, s.duel_group + 0x31, 0);
        if (s.duel_number > 0 &&
            s.duel_number <= STARTUP_DIALOG_CHOICE_COUNT(s.duel_group - 1) &&
            STARTUP_DIALOG_CHOICE_PRESENT(s.duel_group - 1, s.duel_number - 1) != 0)
        {
          PostMessageA(global_main_hwnd, WM_COMMAND, s.duel_number + 99, 0);
        }
      }
    }

    ++s.slash;
  }

  while (GetMessageA(&s.msg, NULL, 0, 0) != 0)
  {
    if ((unk_00926804 & 2) != 0 &&
        ((s.msg.message == WM_KEYDOWN && (s.msg.wParam == 'u' || s.msg.wParam == VK_INSERT)) ||
         (s.msg.message == WM_CHAR && s.msg.wParam == '\'')))
    {
      FamInterface_Taunt();
    }
    else if (!FUN_0055357b(global_main_hwnd, &s.msg) &&
             (DAT_0079141c == NULL || !IsDialogMessageA(DAT_0079141c, &s.msg)))
    {
      TranslateMessage(&s.msg);
      DispatchMessageA(&s.msg);
    }
  }

  s.fam_window = FindWindowA("fam_WindowClass", NULL);
  if (s.fam_window != NULL)
  {
    SendMessageA(s.fam_window, WM_CLOSE, 0, 0);
  }

  FUN_00422bea();
  DeleteCriticalSection(&DAT_00926910);
  DeleteCriticalSection(&DAT_009266b0);
  FUN_005539b8("MAGICGAME_MagicShellClass");
  FUN_00500d46();
  CloseHandle(global_mutex_GameInit);
  CloseHandle((HANDLE)global_mutex_ReadPacket);
  CloseHandle(global_mutex_WritePacket);
  CloseHandle(global_mutex_UpdateLowerDialog);

  CloseHandle(global_mutex_LowerDialog);
}
