#include <direct.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "drawcardlib/src/pic.h"
#include "game_support.h"
#include "manalinkinterface/manalinkinterface.h"
#include "deckdll/src/shared_resources.h"
#include "deckdll/src/card_db.h"
#include "global_strings.h"
#include "shared_startup.h"

#define STARTUP_DIALOG_COUNT 5
#define STARTUP_DIALOG_ROW_STRIDE 0x8b8
#define STARTUP_DIALOG_CHOICE_OFFSET 0x10c
#define STARTUP_DIALOG_CHOICE_STRIDE 0xD8
#define STARTUP_DIALOG_COUNT_OFFSET 0x8b4

#define STARTUP_DIALOG_ROW(group_) \
  (&DAT_007a7d90[(group_) * STARTUP_DIALOG_ROW_STRIDE])
#define STARTUP_DIALOG_CHOICE_COUNT(group_) \
  (*(int *)(STARTUP_DIALOG_ROW(group_) + STARTUP_DIALOG_COUNT_OFFSET))
#define STARTUP_DIALOG_CHOICE_PRESENT(group_, choice_) \
  (*(int *)(STARTUP_DIALOG_ROW(group_) + STARTUP_DIALOG_CHOICE_OFFSET + (choice_) * STARTUP_DIALOG_CHOICE_STRIDE))

typedef struct
{
  char playface_name[0x105];
  char screen_name[0x0e];
  char real_name[0x100];
  char personal_quote[0x400];
  char email[0x100];
  char date_text[0x15];
  unsigned short unk_728;
  unsigned short unk_72a;
  unsigned short unk_72c;
  unsigned short dci_rank_display;
  unsigned short dci_wins;
  unsigned short dci_losses;
  unsigned short dci_draws_or_unused;
  unsigned short dci_rank_pending;
  unsigned short mp_wins;
  unsigned short mp_losses;
  unsigned short mp_draws;
  unsigned short concede_count;
  unsigned short disconnect_count;
  unsigned char has_profile_stats;
  unsigned char unk_743;
  unsigned int unk_744;
} screen_name_file_t;
STATIC_ASSERT(sizeof(screen_name_file_t) == 0x748, screen_name_file_t_wrong_size);

// GLOBAL: MAGIC 0x0079141c
HWND DAT_0079141c;


// GLOBAL: MAGIC 0x0079154c
HANDLE global_mutex_UpdateLowerDialog;

// GLOBAL: MAGIC 0x007a7d90
unsigned char DAT_007a7d90[STARTUP_DIALOG_COUNT * STARTUP_DIALOG_ROW_STRIDE];

// GLOBAL: MAGIC 0x0074b62c
int DAT_0074b62c;

// GLOBAL: MAGIC 0x007a79b8
int DAT_007a79b8;

extern char *global_base_txt;

// GLOBAL: MAGIC 0x008950b0
HWND global_main_hwnd;

// GLOBAL: MAGIC 0x008b3bd8
int DAT_008b3bd8;

// GLOBAL: MAGIC 0x008b32bc
int DAT_008b32bc;

// GLOBAL: MAGIC 0x008b4dd4
int DAT_008b4dd4;

// GLOBAL: MAGIC 0x008cc704
HANDLE global_mutex_GameInit;

// GLOBAL: MAGIC 0x008a91a0
screen_name_file_t DAT_008a91a0;

extern int global_available_slots;
extern card_ptr_t global_raw_cards_storage[2000];

// GLOBAL: MAGIC 0x008cff14
HANDLE global_mutex_LowerDialog;

// GLOBAL: MAGIC 0x0091bbd0
int DAT_0091bbd0;

// GLOBAL: MAGIC 0x0091ce40
char DAT_0091ce40[300];

// GLOBAL: MAGIC 0x0093932c
HANDLE global_mutex_WritePacket;

// GLOBAL: MAGIC 0x00939560
int DAT_00939560;

// GLOBAL: MAGIC 0x006381c0
char DAT_006381c0[0x358];

// GLOBAL: MAGIC 0x00638518
char DAT_00638518[0xd0];

// GLOBAL: MAGIC 0x006385e8
char DAT_006385e8[0x60];

// GLOBAL: MAGIC 0x007ab2c0
int DAT_007ab2c0;

// GLOBAL: MAGIC 0x00925d2c
int DAT_00925d2c;

// GLOBAL: MAGIC 0x0056ef74
HWND DAT_0056ef74;

int InitLicenseSecretsFromRegistry(void);
char *CsvParseNextField(char **txt);
static void FUN_0048fa0a(void);
static void FUN_0048fd9f(screen_name_file_t *screen_name_data, int use_current_time);
static int FUN_004e027c(void);
static void FUN_0048fcb5(void);
static int FUN_00497c8d(void);
static int FUN_00500c56(void);
static void FUN_00500d46(void);
static void FUN_00459b6e(void *window);
static LRESULT CALLBACK FUN_005539c3(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int read_db_guts(char *cards_dat_filename);
void InitBitmapInfo24bppTopDown(BITMAPINFO *bmi, int width, int height);
void checked_DeleteDC_DeleteObject(HDC dc, HGDIOBJ obj);
void DestroyCardArtPalette(void);


// FUNCTION: MAGIC 0x005532e9
void FUN_005532e9(void)
{
  typedef struct
  {
    char magic[10];
    char manalink[10];
    char manalink_interface[10];
    char deckdll[10];
  } binary_versions_t;

  // GLOBAL: MAGIC 0x008ce540
  static binary_versions_t binary_versions;

  struct
  {
    DWORD handle;
    UINT version_length;
    DWORD version_size;
    void *version_info;
    char *version_string;
  } s;

  s.version_size = GetFileVersionInfoSizeA("magic.exe", &s.handle);
  s.version_info = malloc(s.version_size);
  GetFileVersionInfoA("magic.exe", 0, s.version_size, s.version_info);
  VerQueryValueA(s.version_info, "\\StringFileInfo\\040904b0\\FileVersion", (LPVOID *)&s.version_string,
                 &s.version_length);
  strcpy(binary_versions.magic, s.version_string);
  free(s.version_info);

  s.version_size = GetFileVersionInfoSizeA("manalink.exe", &s.handle);
  s.version_info = malloc(s.version_size);
  GetFileVersionInfoA("manalink.exe", 0, s.version_size, s.version_info);
  VerQueryValueA(s.version_info, "\\StringFileInfo\\040904b0\\FileVersion", (LPVOID *)&s.version_string,
                 &s.version_length);
  strcpy(binary_versions.manalink, s.version_string);
  free(s.version_info);

  s.version_size = GetFileVersionInfoSizeA("manalinkInterface.dll", &s.handle);
  s.version_info = malloc(s.version_size);
  GetFileVersionInfoA("manalinkInterface.dll", 0, s.version_size, s.version_info);
  VerQueryValueA(s.version_info, "\\StringFileInfo\\040904b0\\FileVersion", (LPVOID *)&s.version_string,
                 &s.version_length);
  strcpy(binary_versions.manalink_interface, s.version_string);
  free(s.version_info);

  s.version_size = GetFileVersionInfoSizeA("deckdll.dll", &s.handle);
  s.version_info = malloc(s.version_size);
  GetFileVersionInfoA("deckdll.dll", 0, s.version_size, s.version_info);
  VerQueryValueA(s.version_info, "\\StringFileInfo\\040904b0\\FileVersion", (LPVOID *)&s.version_string,
                 &s.version_length);
  strcpy(binary_versions.deckdll, s.version_string);
  free(s.version_info);
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
  wndclass.hInstance = (HINSTANCE)g_app_instance;
  wndclass.hIcon = LoadIconA((HINSTANCE)g_app_instance, (LPCSTR)0x66);
  wndclass.hCursor = LoadCursorA(NULL, (LPCSTR)IDC_ARROW);
  wndclass.hbrBackground = NULL;
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = class_name;

  if (RegisterClassA(&wndclass) == 0)
    return 0;

  return 1;
}

// FUNCTION: MAGIC 0x005539b8
void FUN_005539b8(void *unused)
{
  (void)unused;
}

// FUNCTION: MAGIC 0x0048fa0a
static void FUN_0048fa0a(void)
{
  FILE *screen_name_file;
  char screen_name_filename[100];

  FUN_0048fd9f(&DAT_008a91a0, 0);
  sprintf(screen_name_filename, "ScreenNames\\%s.scn", DAT_008a91a0.screen_name);
  SetFileAttributesA(screen_name_filename, FILE_ATTRIBUTE_NORMAL);
  screen_name_file = fopen(screen_name_filename, "wb");
  fwrite(&DAT_008a91a0, 0x748, 1, screen_name_file);
  fclose(screen_name_file);
  SetFileAttributesA("ScreenNames\\ActiveName.dat", FILE_ATTRIBUTE_NORMAL);
  screen_name_file = fopen("ScreenNames\\ActiveName.dat", "wb");
  fwrite(DAT_008a91a0.screen_name, 0xe, 1, screen_name_file);
  fclose(screen_name_file);
}

// FUNCTION: MAGIC 0x004e027c
static int FUN_004e027c(void)
{
  return 1;
}

// FUNCTION: MAGIC 0x0048fcb5
static void FUN_0048fcb5(void)
{
  struct
  {
    FILE *screen_name_file;
    char screen_name_filename[100];
    char active_name[100];
  } s;

  s.screen_name_file = fopen("ScreenNames\\ActiveName.dat", "rb");
  if (s.screen_name_file == NULL)
  {
    FUN_0048fa0a();
  }
  else
  {
    fread(s.active_name, 0xe, 1, s.screen_name_file);
    fclose(s.screen_name_file);

    sprintf(s.screen_name_filename, "ScreenNames\\%s.scn", s.active_name);
    s.screen_name_file = fopen(s.screen_name_filename, "rb");
    if (s.screen_name_file == NULL)
    {
      FUN_0048fd9f(&DAT_008a91a0, 0);
    }
    else
    {
      fread(&DAT_008a91a0, 0x748, 1, s.screen_name_file);
      fclose(s.screen_name_file);
    }
  }
}

// FUNCTION: MAGIC 0x0048fd9f
static void FUN_0048fd9f(screen_name_file_t *screen_name_data, int use_current_time)
{
  time_t current_time;

  if (strlen(DAT_006381c0) == 0)
  {
    load_text("MP_UIStrings.txt", "SHELLPAGE_SCREENNAME");
    strcpy(DAT_006381c0, text_lines[2]);
    strcpy(DAT_006385e8, text_lines[0x11]);
  }

  strcpy(screen_name_data->screen_name, DAT_006381c0);
  strcpy(screen_name_data->playface_name, "0001");
  strcpy(screen_name_data->real_name, "");
  strcpy(screen_name_data->personal_quote, "");
  strcpy(screen_name_data->email, "");
  if (use_current_time != 0)
  {
    struct tm *current_tm;

    _tzset();
    time(&current_time);
    current_tm = localtime(&current_time);
    strftime(screen_name_data->date_text, 0x80, DAT_00638518, current_tm);
  }
  else
  {
    strcpy(screen_name_data->date_text, DAT_006385e8);
  }

  screen_name_data->disconnect_count = 0;
  screen_name_data->concede_count = screen_name_data->disconnect_count;
  screen_name_data->mp_draws = screen_name_data->concede_count;
  screen_name_data->mp_losses = screen_name_data->mp_draws;
  screen_name_data->mp_wins = screen_name_data->mp_losses;
  screen_name_data->dci_draws_or_unused = screen_name_data->mp_wins;
  screen_name_data->dci_losses = screen_name_data->dci_draws_or_unused;
  screen_name_data->dci_wins = screen_name_data->dci_losses;
  screen_name_data->unk_72c = screen_name_data->dci_wins;
  screen_name_data->unk_72a = screen_name_data->unk_72c;
  screen_name_data->unk_728 = screen_name_data->unk_72a;
  screen_name_data->dci_rank_pending = 1600;
  screen_name_data->dci_rank_display = screen_name_data->dci_rank_pending;
  screen_name_data->unk_744 = 0;
  screen_name_data->has_profile_stats = (unsigned char)screen_name_data->unk_744;
}

// FUNCTION: MAGIC 0x00497c8d
static int FUN_00497c8d(void)
{
  struct
  {
    BYTE value_buffer[256];
    HKEY key;
    DWORD value_size;
    int manalink_enabled;
    DWORD value_type;
  } s;
  int result;

  s.manalink_enabled = 0;
  if (RegOpenKeyA(HKEY_CURRENT_USER, "Software\\MicroProse\\Magic: The Gathering", &s.key) != ERROR_SUCCESS)
  {
    return 0;
  }

  s.value_type = REG_SZ;
  s.value_size = sizeof(s.value_buffer);
  RegQueryValueExA(s.key, "ManaLink", NULL, &s.value_type, s.value_buffer, &s.value_size);
  RegCloseKey(s.key);

  _strlwr((char *)s.value_buffer);
  if (strcmp((char *)s.value_buffer, "on") == 0)
  {
    s.manalink_enabled = 1;
  }

  if (FindWindowA("fam_WindowClass", NULL) == NULL && s.manalink_enabled != 0)
  {
    result = 1;
  }
  else
  {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x00500c56
static int FUN_00500c56(void)
{
  unk_007a7d6c = 0;
  unk_0091d07c = unk_007a7d6c;
  DAT_00939560 = 1;
  DAT_00925d2c = 0;
  DAT_007ab2c0 = DAT_00925d2c;
  DAT_008b32bc = 1;
  g_waiting_for_network_packet = 0;
  unk_008b27f0 = '\0';
  g_target_selection_network_packet.packet_type = '\0';
  unk_008b2938 = '\0';
  g_network_result_packet_type = '\0';
  unk_008b34a0 = '\0';
  unk_008cf3a0 = '\0';
  unk_00926080 = '\0';
  unk_0091ca90 = '\0';

  if (unk_0091ca98 != NULL)
  {
    free(unk_0091ca98);
  }

  unk_0091ca98 = malloc(0x20);
  if (unk_0091ca98 == NULL)
  {
    MessageBoxA(NULL, "Memory allocation error in InitActionPackets!", "Packet Error", MB_ICONHAND);
  }

  unk_0091ca94 = 0;
  unk_0091ca96 = 1;
  return 1;
}

// FUNCTION: MAGIC 0x00500d46
static void FUN_00500d46(void)
{
  free(unk_0091ca98);
  unk_0091ca98 = NULL;
  DAT_008b32bc = 0;
}

// FUNCTION: MAGIC 0x00459b6e
static void FUN_00459b6e(void *window)
{
  void *tmp = window;

  while (DAT_008b32bc != 0)
  {
    DAT_007ab2c0 = FamInterface_HasOpponent();
    if (DAT_00925d2c != DAT_007ab2c0 || DAT_00939560 != 0)
    {
      if (WaitForSingleObject(global_mutex_LowerDialog, 500) == 0)
      {
        DAT_00925d2c = DAT_007ab2c0;
        DAT_00939560 = 0;
        if (DAT_007ab2c0 != 0)
        {
          OutputDebugStringA("Registering an opponent.\n");
          DAT_007a79b8 = FamInterface_IsHost();
          SendMessageA(DAT_0056ef74, WM_COMMAND, 0x404, 0);
        }
        else
        {
          OutputDebugStringA("Unregistering an opponent.\n");
          SendMessageA(DAT_0056ef74, WM_COMMAND, 0x405, 0);
        }
      }
      else
      {
        DAT_00925d2c = -1;
      }

      ReleaseMutex(global_mutex_LowerDialog);
    }

    Sleep(500);
  }

  _endthread();
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

  g_duel_window_hwnd = 0;
  g_duel_life_window_hwnd = 0;
  g_duel_full_card_window_hwnd = g_duel_life_window_hwnd;
  g_duel_phase_display_window_hwnd = 0;
  g_duel_attack_phase_window_hwnd = g_duel_phase_display_window_hwnd;
  g_duel_card_preview_window_hwnd = 0;
  DAT_008b4dd4 = 0;
  g_duel_message_loop_active = DAT_008b4dd4;
  unk_00742fc4 = g_duel_message_loop_active;
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

  for (s.random_spin_count = 0; s.random_spin_count < rand() % 50000; ++s.random_spin_count)
  {
    rand();
  }

  g_app_instance = hInstance;
  s.startup_message[0] = '\0';

  InitializeCriticalSection(&g_shared_startup_lock);
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
  InitializeCriticalSection(&g_card_render_lock);

  if (register_MagicShellClass("MAGICGAME_MagicShellClass"))
  {
    global_main_hwnd = CreateWindowExA(0,
                                       "MAGICGAME_MagicShellClass",
                                       gs_magic_the_gathering_title_00789460,
                                       0x90040000,
                                       1,
                                       0,
                                       GetSystemMetrics(SM_CXSCREEN) - 1,
                                       GetSystemMetrics(SM_CYSCREEN),
                                       NULL,
                                       NULL,
                                       (HINSTANCE)g_app_instance,
                                       NULL);
    if (global_main_hwnd == NULL)
    {
      load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
      strcat(s.startup_message, text_lines[8]);
      strcat(s.startup_message, "\n");
      s.startup_ok = 0;
    }
  }
  else
  {
    load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
    strcat(s.startup_message, text_lines[8]);
    strcat(s.startup_message, "\n");
    s.startup_ok = 0;
  }

  if (!s.startup_ok)
  {
    MessageBoxA(NULL, s.startup_message, gs_magic_the_gathering_title_00789460, 0x1030);
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
    if ((g_duel_network_flags & 2) != 0 &&
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

  ShutdownSharedStartupResources();
  DeleteCriticalSection(&g_shared_startup_lock);
  DeleteCriticalSection(&g_card_render_lock);
  FUN_005539b8("MAGICGAME_MagicShellClass");
  FUN_00500d46();
  CloseHandle(global_mutex_GameInit);
  CloseHandle((HANDLE)global_mutex_ReadPacket);
  CloseHandle(global_mutex_WritePacket);
  CloseHandle(global_mutex_UpdateLowerDialog);

  CloseHandle(global_mutex_LowerDialog);
}
