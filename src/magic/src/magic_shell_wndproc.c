#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cardartlib/src/palette.h"
#include "deckdll/src/shared_resources.h"
#include "deckdll/src/magsnd.h"
#include "game_support.h"
#include "global_strings.h"
#include "duel_engine.h"
#include "magic_shell.h"
#include "shared_startup.h"

#define SHELL_PAGE(index) (&g_startup_dialog_choices[(index)])
#define SHELL_CHOICE_COUNT(index) (g_startup_dialog_choices[(index)].choice_count)
#define SHELL_OPTION(page, index) (&g_startup_dialog_choices[(page)].choices[(index)])
/* POINT occupies two LONG arguments on the Win32 stack. */
#define SHELL_POINT_IN_RECT(rect, point) \
  ((BOOL(WINAPI *)(const RECT *, LONG, LONG))PtInRect)((rect), (point).x, (point).y)

extern HINSTANCE g_app_instance;
extern HWND global_main_hwnd;
extern HWND g_startup_modeless_dialog;
extern HWND g_manalink_lower_dialog_hwnd;
extern int g_startup_dialog_group;
extern int g_startup_dialog_group_count;
extern char g_shell_art_directory[];
extern char global_base_directory[];

void change_buttonclass_wndproc(HWND hwnd);
void draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HBITMAP bitmap);
void prepare_duel_video_mode_transition(void);
void sound_close(void);
void draw_item(DRAWITEMSTRUCT *item, HBRUSH brush, HANDLE background_bitmap,
               HPEN pen1, HPEN pen2, COLORREF color, int draw_focus, int flags);
WPARAM WINAPI DeckBuilderMain(HWND parent_hwnd, int db_flags_1, int db_flags_2);
unsigned int HasExpansion(unsigned char expansion_mask);

/* The shell's page rectangles are four DWORDs each in the original data segment. */
// GLOBAL: MAGIC 0x00709490
static RECT shell_box_rect;
// GLOBAL: MAGIC 0x007094a0
static RECT shell_dialog_rect;
// GLOBAL: MAGIC 0x007094b0
static RECT shell_background_rect;
// GLOBAL: MAGIC 0x007094c0
static RECT shell_sphere_rects[5];
// GLOBAL: MAGIC 0x00709510
static RECT shell_close_rect;
// GLOBAL: MAGIC 0x00709520
static RECT shell_help_rect;
// GLOBAL: MAGIC 0x00709530
static RECT shell_maximize_rect;
// GLOBAL: MAGIC 0x00709540
static int shell_visible_rows;
// GLOBAL: MAGIC 0x00709550
static RECT shell_choices_rect;
// GLOBAL: MAGIC 0x00709560
static RECT shell_minimize_rect;

// GLOBAL: MAGIC 0x00708a9c
static int shell_animation_frame;
// GLOBAL: MAGIC 0x00708aa0
static HBITMAP shell_sphere_bitmaps[5];
// GLOBAL: MAGIC 0x00708abc
static int shell_hovered_page;
// GLOBAL: MAGIC 0x00708ac0
static HDC shell_sphere_dc;
// GLOBAL: MAGIC 0x00708ac4
static int shell_scroll_position;
// GLOBAL: MAGIC 0x00708acc
static COLORREF shell_bitmap_mask_color;
// GLOBAL: MAGIC 0x00708ad0
static HBITMAP shell_glow_bitmap;
// GLOBAL: MAGIC 0x00708ad8
static BITMAP shell_background_info;
// GLOBAL: MAGIC 0x00708af4
static HDC shell_background_dc;
// GLOBAL: MAGIC 0x00708af0
static COLORREF shell_bitmap_background_color;
// GLOBAL: MAGIC 0x00708af8
static HBITMAP shell_box_bitmap;
// GLOBAL: MAGIC 0x00708afc
static int shell_box_visible;
// GLOBAL: MAGIC 0x00708b04
static HBITMAP shell_highlight_bitmap;
// GLOBAL: MAGIC 0x00708b08
static HBITMAP shell_background_bitmap;
// GLOBAL: MAGIC 0x00708a90
static int shell_help_hovered;
// GLOBAL: MAGIC 0x00708a94
static COLORREF shell_selected_text_color;
// GLOBAL: MAGIC 0x00708a98
static COLORREF shell_regular_text_color;
// GLOBAL: MAGIC 0x00708ab4
static int shell_resources_loaded;
// GLOBAL: MAGIC 0x00708ab8
static HFONT shell_text_font;
// GLOBAL: MAGIC 0x00708a88
static HFONT shell_button_font;
// GLOBAL: MAGIC 0x00708b00
static HFONT shell_explanation_font;
// GLOBAL: MAGIC 0x0057eeb8
static int shell_restore_pending;
// GLOBAL: MAGIC 0x0057eebc
static int shell_size_suspended;
// GLOBAL: MAGIC 0x0057ec58
static int shell_animation_period = 100;
// GLOBAL: MAGIC 0x007ab154
static int shell_selected_choice;
// GLOBAL: MAGIC 0x0056ef78
static HWND shell_single_duel_dialog;
// GLOBAL: MAGIC 0x0056ef7c
static HWND shell_gauntlet_dialog;
// GLOBAL: MAGIC 0x0056ef80
static HWND shell_sealed_deck_dialog;
// GLOBAL: MAGIC 0x005719f8
static HWND shell_screen_name_dialog;

typedef struct
{
  COLORREF button_text_color;
  HBITMAP button_face;
  HPEN light_pen;
  HPEN dark_pen;
  COLORREF regular_button_color;
  COLORREF selected_button_color;
  HBRUSH light_brush;
  HBRUSH dark_brush;
  COLORREF dark_color;
  COLORREF panel_color;
  HBRUSH panel_brush;
  COLORREF background_color;
} shell_draw_resources_t;
// GLOBAL: MAGIC 0x007ab400
static shell_draw_resources_t shell_draw_resources;

static void shell_set_map_mode(HDC dc, HWND hwnd, HBITMAP background);
static void shell_invalidate_logical_rect(HWND hwnd, HBITMAP background, RECT *rect);
static void shell_enable_animation(int enabled);

/* Large dialog procedures are isolated as annotated recursion points. */
// FUNCTION: MAGIC 0x00457296
static BOOL CALLBACK shell_multi_duel_dialog_proc(HWND hwnd, UINT message,
                                                  WPARAM wparam, LPARAM lparam)
{
  (void)hwnd;
  (void)message;
  (void)wparam;
  (void)lparam;
  return FALSE;
}

// FUNCTION: MAGIC 0x0045abc4
static BOOL CALLBACK shell_single_duel_dialog_proc(HWND hwnd, UINT message,
                                                   WPARAM wparam, LPARAM lparam)
{
  (void)hwnd;
  (void)message;
  (void)wparam;
  (void)lparam;
  return FALSE;
}

// FUNCTION: MAGIC 0x0045d89e
static BOOL CALLBACK shell_gauntlet_dialog_proc(HWND hwnd, UINT message,
                                                WPARAM wparam, LPARAM lparam)
{
  (void)hwnd;
  (void)message;
  (void)wparam;
  (void)lparam;
  return FALSE;
}

// FUNCTION: MAGIC 0x0045ff30
static BOOL CALLBACK shell_blank_dialog_proc(HWND hwnd, UINT message,
                                             WPARAM wparam, LPARAM lparam)
{
  (void)hwnd;
  (void)message;
  (void)wparam;
  (void)lparam;
  return FALSE;
}

// FUNCTION: MAGIC 0x0046032a
static BOOL CALLBACK shell_sealed_deck_dialog_proc(HWND hwnd, UINT message,
                                                   WPARAM wparam, LPARAM lparam)
{
  (void)hwnd;
  (void)message;
  (void)wparam;
  (void)lparam;
  return FALSE;
}

// FUNCTION: MAGIC 0x0048c879
static BOOL CALLBACK shell_screen_name_dialog_proc(HWND hwnd, UINT message,
                                                   WPARAM wparam, LPARAM lparam)
{
  (void)hwnd;
  (void)message;
  (void)wparam;
  (void)lparam;
  return FALSE;
}

/* Movie playback recursively depends on the MagVid interface and CD search. */
// FUNCTION: MAGIC 0x004554b0
static int shell_play_movie(HWND hwnd, const char *movie_name, int directory,
                            RECT *rect)
{
  (void)hwnd;
  (void)movie_name;
  (void)directory;
  (void)rect;
  return 1;
}

// FUNCTION: MAGIC 0x005565b3
static void shell_layout_choices(void)
{
  SetRect(&shell_choices_rect, shell_background_rect.left + 0x2e,
          shell_background_rect.top + 0x38, shell_background_rect.left + 0x246,
          shell_background_rect.top + 0xb6);
  shell_visible_rows = 4;
}

// FUNCTION: MAGIC 0x00455766
static HWND shell_open_single_duel(LPARAM rect)
{
  shell_single_duel_dialog =
      CreateDialogParamA(g_app_instance, "SingleDuelPage", global_main_hwnd,
                         shell_single_duel_dialog_proc, rect);
  return shell_single_duel_dialog;
}

// FUNCTION: MAGIC 0x004557a0
static int shell_close_single_duel(void)
{
  if (shell_single_duel_dialog != NULL)
  {
    DestroyWindow(shell_single_duel_dialog);
    shell_single_duel_dialog = NULL;
  }
  return 1;
}

// FUNCTION: MAGIC 0x004557d8
static HWND shell_open_gauntlet(LPARAM rect)
{
  shell_gauntlet_dialog =
      CreateDialogParamA(g_app_instance, "GauntletPage", global_main_hwnd,
                         shell_gauntlet_dialog_proc, rect);
  return shell_gauntlet_dialog;
}

// FUNCTION: MAGIC 0x00455812
static int shell_close_gauntlet(void)
{
  if (shell_gauntlet_dialog != NULL)
  {
    DestroyWindow(shell_gauntlet_dialog);
    shell_gauntlet_dialog = NULL;
  }
  return 1;
}

// FUNCTION: MAGIC 0x0045584a
static HWND shell_open_sealed_deck(LPARAM rect)
{
  if (HasExpansion(2) != 0)
    shell_sealed_deck_dialog =
        CreateDialogParamA(g_app_instance, "SealedDeckPage", global_main_hwnd,
                           shell_sealed_deck_dialog_proc, rect);
  else
    shell_sealed_deck_dialog =
        CreateDialogParamA(g_app_instance, "BLANKDIALOG", global_main_hwnd,
                           shell_blank_dialog_proc, rect);
  return shell_sealed_deck_dialog;
}

// FUNCTION: MAGIC 0x004558c0
static int shell_close_sealed_deck(void)
{
  if (shell_sealed_deck_dialog != NULL)
  {
    DestroyWindow(shell_sealed_deck_dialog);
    shell_sealed_deck_dialog = NULL;
  }
  return 1;
}

// FUNCTION: MAGIC 0x004556f4
static HWND shell_open_multi_duel(LPARAM rect)
{
  g_manalink_lower_dialog_hwnd =
      CreateDialogParamA(g_app_instance, "MultiDuelPage", global_main_hwnd,
                         shell_multi_duel_dialog_proc, rect);
  return g_manalink_lower_dialog_hwnd;
}

// FUNCTION: MAGIC 0x0045572e
static int shell_close_multi_duel(void)
{
  if (g_manalink_lower_dialog_hwnd != NULL)
  {
    DestroyWindow(g_manalink_lower_dialog_hwnd);
    g_manalink_lower_dialog_hwnd = NULL;
  }
  return 1;
}

// FUNCTION: MAGIC 0x0048c7e0
static HWND shell_open_screen_name(LPARAM rect)
{
  shell_screen_name_dialog =
      CreateDialogParamA(g_app_instance, "SCREENNAMEPAGE", global_main_hwnd,
                         shell_screen_name_dialog_proc, rect);
  return shell_screen_name_dialog;
}

// FUNCTION: MAGIC 0x0048c81a
static int shell_close_screen_name(void)
{
  if (shell_screen_name_dialog != NULL)
  {
    DestroyWindow(shell_screen_name_dialog);
    shell_screen_name_dialog = NULL;
  }
  return 1;
}

// FUNCTION: MAGIC 0x0048c852
static HWND shell_open_records_placeholder(LPARAM rect)
{
  (void)rect;
  return NULL;
}

// FUNCTION: MAGIC 0x0048c864
static int shell_close_records_placeholder(void)
{
  return 1;
}

// FUNCTION: MAGIC 0x004558f8
static void shell_open_deck_builder(RECT *rect)
{
  (void)rect;
  prepare_duel_video_mode_transition();
  sound_close();
  shell_enable_animation(0);
  PostMessageA(global_main_hwnd, 0x496, 0, 0);
  DeckBuilderMain(global_main_hwnd, 2, 1);
  PostMessageA(global_main_hwnd, 0x496, 1, 0);
  shell_enable_animation(1);
  init_sound_dll(global_main_hwnd, 0, 0);
}

// FUNCTION: MAGIC 0x0045596c
static void shell_open_face_maker(RECT *rect)
{
  struct
  {
    PROCESS_INFORMATION process;
    char path[264];
    STARTUPINFOA startup;
  } s;

  (void)rect;
  shell_enable_animation(0);
  PostMessageA(global_main_hwnd, 0x496, 0, 0);
  memset(&s.startup, 0, sizeof(s.startup));
  s.startup.cb = sizeof(s.startup);
  s.startup.dwFlags = STARTF_USESHOWWINDOW;
  s.startup.wShowWindow = SW_SHOWNORMAL;
  sprintf(s.path, "%s\\FaceMaker.exe", global_base_directory);
  CreateProcessA(NULL, s.path, NULL, NULL, FALSE, 0, NULL, NULL,
                 &s.startup, &s.process);
  WaitForSingleObject(s.process.hProcess, INFINITE);
  CloseHandle(s.process.hThread);
  CloseHandle(s.process.hProcess);
  PostMessageA(global_main_hwnd, 0x496, 1, 0);
  shell_enable_animation(1);
}

/* Other direct actions still need their own recursive decompilation. */
// FUNCTION: MAGIC 0x00455a48
static void shell_play_intro(RECT *rect)
{
  (void)rect;
  shell_enable_animation(0);
  shell_play_movie(global_main_hwnd, "INTRO.AVI", 0, rect);
  shell_enable_animation(1);
}
// FUNCTION: MAGIC 0x00455a80
static void shell_open_shandalar(RECT *rect)
{
  struct
  {
    UINT result;
    char command[264];
  } s;

  (void)rect;
  prepare_duel_video_mode_transition();
  sound_close();
  g_duel_run_mode = 4;
  strcpy(s.command, global_base_directory);
  strcat(s.command, "\\Shandalar.exe");
  strcat(s.command, " /MTGshell");
  s.result = WinExec(s.command, SW_SHOW);
  if (s.result >= 32)
  {
    PostMessageA(global_main_hwnd, WM_CLOSE, 0, 0);
  }

  switch (s.result)
  {
  case 0:
    OutputDebugStringA("Run_MetaGame_Shandalar() The system is out of memory or other resources.\n");
    break;
  case 2:
  case 3:
    OutputDebugStringA("Run_MetaGame_Shandalar() Shandalar.exe is invalid (non-Win32 .exe or error in .exe image)\n");
    break;
  case 11:
    OutputDebugStringA("Run_MetaGame_Shandalar() Shandalar.exe could not be located on your system.\n");
    break;
  }
}
// FUNCTION: MAGIC 0x00455b99
static void shell_open_help(RECT *rect)
{
  struct
  {
    char path[280];
    char command[264];
    STARTUPINFOA startup;
  } s;

  shell_enable_animation(0);
  sprintf(s.path, "%s\\Duel.hlp", global_base_directory);
  memset(&s.startup, 0, sizeof(s.startup));
  s.startup.cb = sizeof(s.startup);
  s.startup.dwFlags = STARTF_USESHOWWINDOW | STARTF_USEPOSITION | STARTF_USESIZE;
  s.startup.wShowWindow = SW_SHOWNORMAL;
  s.startup.dwX = rect->left;
  s.startup.dwY = rect->top;
  s.startup.dwXSize = rect->right - rect->left;
  s.startup.dwYSize = rect->bottom - rect->top;
  GetWindowsDirectoryA(s.command, 0x104);
  strcat(s.command, "\\WinHelp.exe ");
  strcat(s.command, s.path);
  WinHelpA(global_main_hwnd, s.path, 0xb, 0);
  shell_enable_animation(1);
}
// FUNCTION: MAGIC 0x00455c7d
static void shell_open_tutorial(RECT *rect)
{
  struct
  {
    FILE *file;
    char path[264];
    PROCESS_INFORMATION process;
    char current_directory[264];
    int found;
    int drive_letter;
    STARTUPINFOA startup;
  } s;

  (void)rect;
  s.found = 0;
  prepare_duel_video_mode_transition();
  sound_close();
  GetCurrentDirectoryA(0x104, s.current_directory);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tutorial.exe");
  s.file = fopen(s.path, "rb");
  if (s.file != NULL)
  {
    s.found = 1;
    fclose(s.file);
  }
  else
  {
    for (s.drive_letter = 'D'; s.drive_letter < 'Z' && s.found == 0;
         ++s.drive_letter)
    {
      sprintf(s.path, "%c:\\Tutorial", s.drive_letter);
      SetCurrentDirectoryA(s.path);
      if (GetDriveTypeA(NULL) == DRIVE_CDROM)
      {
        strcat(s.path, "\\Tutorial.exe");
        s.file = fopen(s.path, "rb");
        if (s.file != NULL)
        {
          fclose(s.file);
          s.found = 1;
        }
      }
    }
  }
  if (s.found != 0)
  {
    memset(&s.startup, 0, sizeof(s.startup));
    s.startup.cb = sizeof(s.startup);
    s.startup.dwFlags = STARTF_USESHOWWINDOW;
    s.startup.wShowWindow = SW_SHOWNORMAL;
    CreateProcessA(NULL, s.path, NULL, NULL, FALSE, 0, NULL, NULL,
                   &s.startup, &s.process);
    WaitForSingleObject(s.process.hProcess, INFINITE);
    CloseHandle(s.process.hThread);
    CloseHandle(s.process.hProcess);
  }
  else
  {
    load_text(global_ui_strings_filename, "TUTORIAL_ERROR");
    MessageBoxA(global_main_hwnd, g_text_lines[0],
                gs_magic_the_gathering_title_00789460, MB_ICONHAND);
  }
  SetCurrentDirectoryA(s.current_directory);
  init_sound_dll(global_main_hwnd, 0, 0);
}
static void shell_show_credits(HWND hwnd, void *text, RECT *rect);

// FUNCTION: MAGIC 0x00455e9a
static void shell_open_credits(RECT *rect)
{
  struct
  {
    char path[264];
    DWORD bytes_read;
    HANDLE file;
    unsigned char file_flags;
    char padding[135];
    void *text;
    RECT window_rect;
    DWORD size;
  } s;

  (void)rect;
  shell_enable_animation(0);
  GetWindowRect(global_main_hwnd, &s.window_rect);
  sprintf(s.path, "%s\\Credits.txt", global_base_directory);
  s.file_flags = 0x88;
  s.file = CreateFileA(s.path, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, 0x08000080, NULL);
  if (s.file != INVALID_HANDLE_VALUE)
  {
    s.size = GetFileSize(s.file, NULL);
    if (s.size != 0xffffffff)
    {
      s.text = malloc(s.size);
      if (s.text != NULL)
      {
        if (ReadFile(s.file, s.text, s.size, &s.bytes_read, NULL) != 0)
        {
          ((char *)s.text)[s.size] = '\0';
          shell_show_credits(global_main_hwnd, s.text, &s.window_rect);
        }
        free(s.text);
      }
    }
    CloseHandle(s.file);
  }
  shell_enable_animation(1);
}

/* Credits UI is a separate dialog helper, pending recursive decompilation. */
// FUNCTION: MAGIC 0x00552260
static void shell_show_credits(HWND hwnd, void *text, RECT *rect)
{
  (void)hwnd;
  (void)text;
  (void)rect;
}
// FUNCTION: MAGIC 0x00455fb8
static void shell_open_readme(RECT *rect)
{
  struct
  {
    char path[264];
    PROCESS_INFORMATION process;
    char command[264];
    STARTUPINFOA startup;
  } s;

  shell_enable_animation(0);
  sprintf(s.path, "%s\\ReadMe.txt", global_base_directory);
  memset(&s.startup, 0, sizeof(s.startup));
  s.startup.cb = sizeof(s.startup);
  s.startup.dwFlags = STARTF_USESHOWWINDOW | STARTF_USEPOSITION | STARTF_USESIZE;
  s.startup.wShowWindow = SW_SHOWNORMAL;
  s.startup.dwX = rect->left;
  s.startup.dwY = rect->top;
  s.startup.dwXSize = rect->right - rect->left;
  s.startup.dwYSize = rect->bottom - rect->top;
  GetWindowsDirectoryA(s.command, 0x104);
  strcat(s.command, "\\Write.exe ");
  strcat(s.command, s.path);
  CreateProcessA(NULL, s.command, NULL, NULL, FALSE, 0, NULL, NULL,
                 &s.startup, &s.process);
  WaitForSingleObject(s.process.hThread, INFINITE);
  CloseHandle(s.process.hThread);
  CloseHandle(s.process.hProcess);
  shell_enable_animation(1);
}

typedef startup_dialog_open_callback_t shell_open_callback_t;
typedef startup_dialog_close_callback_t shell_close_callback_t;
typedef startup_dialog_action_callback_t shell_action_callback_t;

// GLOBAL: MAGIC 0x0057ec60
static shell_open_callback_t shell_open_callbacks[5][10] = {
    {shell_open_single_duel, shell_open_gauntlet, shell_open_sealed_deck,
     shell_open_multi_duel},
    {0},
    {0},
    {0},
    {shell_open_screen_name, shell_open_records_placeholder}};

// GLOBAL: MAGIC 0x0057ed28
static shell_close_callback_t shell_close_callbacks[5][10] = {
    {shell_close_single_duel, shell_close_gauntlet, shell_close_sealed_deck,
     shell_close_multi_duel},
    {0},
    {0},
    {0},
    {shell_close_screen_name, shell_close_records_placeholder}};

// GLOBAL: MAGIC 0x0057edf0
static shell_action_callback_t shell_action_callbacks[5][10] = {
    {0},
    {shell_open_deck_builder, shell_open_face_maker},
    {shell_play_intro, shell_open_shandalar},
    {shell_open_help, shell_open_tutorial, shell_open_credits, shell_open_readme},
    {0}};

// FUNCTION: MAGIC 0x005562f0
static void shell_initialize_page(int page, const char *name)
{
  struct
  {
    startup_dialog_choice_t *choice;
    char *label_cursor;
    char accelerator;
    char padding[3];
    int index;
    startup_dialog_page_t *page_data;
    int callback_index;
    char *line;
    char *description;
  } s;
  s.page_data = SHELL_PAGE(page);
  s.page_data->choice_count = load_text(global_ui_strings_filename, name) - 1;
  strcpy(s.page_data->title, g_text_lines[0]);
  for (s.index = 0; s.index < s.page_data->choice_count; ++s.index)
  {
    s.line = g_text_lines[s.index + 1];
    s.accelerator = *s.line;
    ++s.line;
    if (s.accelerator >= '1' && s.accelerator <= '9')
      s.callback_index = s.accelerator - '1';
    else if (s.accelerator >= 'A' && s.accelerator <= 'Z')
    {
      s.accelerator = 'A';
      s.callback_index = s.accelerator;
    }
    else if (s.accelerator >= 'a' && s.accelerator <= 'z')
    {
      s.accelerator = 'a';
      s.callback_index = s.accelerator;
    }
    else
      s.callback_index = s.index;
    s.choice = &s.page_data->choices[s.index];
    s.label_cursor = s.choice->label;
    s.description = s.choice->description;
    while (*s.line != '\0' && *s.line != ':')
      *s.label_cursor++ = *s.line++;
    if (*s.line == ':')
      ++s.line;
    while (*s.line != '\0')
      *s.description++ = *s.line++;
    *s.label_cursor = '\0';
    *s.description = '\0';
    s.choice->is_modeless =
        shell_action_callbacks[page][s.callback_index] == NULL;
    s.choice->open =
        shell_open_callbacks[page][s.callback_index];
    s.choice->close =
        shell_close_callbacks[page][s.callback_index];
    s.choice->action =
        shell_action_callbacks[page][s.callback_index];
  }
  for (; s.index < 10; ++s.index)
  {
    s.choice = &s.page_data->choices[s.index];
    strcpy(s.choice->label, "");
    s.choice->open = NULL;
    s.choice->close = NULL;
    s.choice->action = NULL;
  }
}

// FUNCTION: MAGIC 0x005560c0
static int shell_initialize_pages(void)
{
  SetRect(&SHELL_PAGE(0)->page_rect, 0x43, 0x2c, 0xaf, 0x98);
  shell_initialize_page(0, "SHELLSCREEN_DUEL");
  SetRect(&SHELL_PAGE(1)->page_rect, 0x21, 0x92, 0x8d, 0xfe);
  shell_initialize_page(1, "SHELLSCREEN_TOOLS");
  SetRect(&SHELL_PAGE(2)->page_rect, 0x17, 0xfc, 0x83, 0x168);
  shell_initialize_page(2, "SHELLSCREEN_METAGAME");
  SetRect(&SHELL_PAGE(3)->page_rect, 0x23, 0x16a, 0x8f, 0x1d6);
  shell_initialize_page(3, "SHELLSCREEN_HELP");
  SetRect(&SHELL_PAGE(4)->page_rect, 0x48, 0x1d1, 0xb4, 0x23d);
  shell_initialize_page(4, "SHELLSCREEN_RECORDS");
  g_startup_dialog_group_count = 5;
  SetRect(&shell_background_rect, 0xb9, 0, 800, 0x243);
  SetRect(&shell_minimize_rect, 0x2d1, 3, 0x2e6, 0x17);
  SetRect(&shell_maximize_rect, 0x2e9, 3, 0x2ff, 0x17);
  SetRect(&shell_close_rect, 0x304, 3, 0x31a, 0x17);
  SetRect(&shell_help_rect, 0xec, 0x181, 0x28d, 0x1b5);
  SetRect(&shell_box_rect, 0x179, 0x16d, 0x2ab, 0x1c2);
  SetRect(&shell_sphere_rects[0], 0x34, 0x1c, 0, 0);
  SetRect(&shell_sphere_rects[1], 0x10, 0x86, 0, 0);
  SetRect(&shell_sphere_rects[2], 5, 0xeb, 0, 0);
  SetRect(&shell_sphere_rects[3], 0x10, 0x15c, 0, 0);
  SetRect(&shell_sphere_rects[4], 0x36, 0x1c1, 0, 0);
  shell_layout_choices();
  return 1;
}

// FUNCTION: MAGIC 0x00557409
static void shell_set_map_mode(HDC dc, HWND hwnd, HBITMAP background)
{
  struct
  {
    BITMAP bitmap;
    RECT rect;
  } s;

  SetMapMode(dc, MM_ANISOTROPIC);
  GetClientRect(hwnd, &s.rect);
  SetViewportExtEx(dc, s.rect.right, s.rect.bottom, NULL);
  if (background != NULL)
  {
    GetObjectA(background, sizeof(BITMAP), &s.bitmap);
    SetWindowExtEx(dc, s.bitmap.bmWidth, s.bitmap.bmHeight, NULL);
  }
  else
    SetWindowExtEx(dc, s.rect.right, s.rect.bottom, NULL);
}

// FUNCTION: MAGIC 0x0055748c
static void shell_logical_rect_to_device(HWND hwnd, HBITMAP background, RECT *source, RECT *result)
{
  struct
  {
    HDC dc;
    RECT rect;
  } s;

  s.dc = GetDC(hwnd);
  shell_set_map_mode(s.dc, hwnd, background);
  CopyRect(&s.rect, source);
  LPtoDP(s.dc, (LPPOINT)&s.rect, 2);
  ReleaseDC(hwnd, s.dc);
  if (result != NULL)
    CopyRect(result, &s.rect);
  else
    CopyRect(source, &s.rect);
}

// FUNCTION: MAGIC 0x00557512
static void shell_invalidate_logical_rect(HWND hwnd, HBITMAP background, RECT *rect)
{
  RECT device_rect;

  shell_logical_rect_to_device(hwnd, background, rect, &device_rect);
  InflateRect(&device_rect, 5, 5);
  InvalidateRect(hwnd, &device_rect, TRUE);
}

// FUNCTION: MAGIC 0x00557556
static void shell_enable_animation(int enabled)
{
  if (*(int *)(gs_phasebar_your_draw_0091b110 + 52) != 0)
    KillTimer(global_main_hwnd, 1);
  else
  {
    if (enabled != 0)
      SetTimer(global_main_hwnd, 1, shell_animation_period, NULL);
    else
      KillTimer(global_main_hwnd, 1);
  }
}

/* The original loader reuses this BITMAPINFO storage for CreateDIBSection. */
// GLOBAL: MAGIC 0x006386a0
static struct
{
  BITMAPINFOHEADER header;
  RGBQUAD colors[256];
} shell_dib_info;

// FUNCTION: MAGIC 0x004945ad
static HBITMAP shell_create_dib_section(BITMAPINFO *info, void *copy_info, int unused)
{
  struct
  {
    int padding;
    HBITMAP bitmap;
    DWORD image_size;
    HDC dc;
    HANDLE section;
    BITMAPINFO *scratch;
    DWORD bits_offset;
    BYTE *bits;
    void *section_bits;
    DWORD size;
    int color_count;
  } s;

  s.scratch = (BITMAPINFO *)&shell_dib_info;
  s.bitmap = NULL;
  switch (info->bmiHeader.biBitCount)
  {
  case 1:
    s.color_count = 2;
    break;
  case 4:
    s.color_count = 16;
    break;
  case 8:
    s.color_count = 256;
    break;
  default:
    s.color_count = 0;
    break;
  }

  s.bits_offset = info->bmiHeader.biSize + s.color_count * 4;
  s.bits = (BYTE *)info + s.bits_offset;
  if (s.scratch != NULL)
  {
    memcpy(s.scratch, info, 0x28);
    memcpy(s.scratch->bmiColors, info->bmiColors, s.color_count << 2);
    if (info->bmiHeader.biWidth % 3 != 0)
      s.padding = 4 - info->bmiHeader.biWidth % 3;
    else
      s.padding = 0;
    s.image_size = (info->bmiHeader.biWidth + s.padding) * info->bmiHeader.biHeight;
    switch (info->bmiHeader.biBitCount)
    {
    case 1:
      break;
    case 4:
      break;
    case 8:
      break;
    case 16:
      s.image_size *= 2;
      break;
    case 24:
      s.image_size *= 3;
      break;
    case 32:
      s.image_size *= 4;
      break;
    }
    s.size = info->bmiHeader.biSizeImage;
    s.section = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, 0x08000004,
                                   0, ((int)s.image_size > (int)s.size ? s.image_size : s.size) + 1000, NULL);
    if (s.section != NULL)
    {
      s.dc = GetDC(NULL);
      ApplyCardArtPaletteToDc(s.dc);
      s.scratch->bmiHeader.biSizeImage = s.image_size;
      s.scratch->bmiHeader.biCompression = BI_RGB;
      if (s.color_count == 0)
        s.bitmap = CreateDIBSection(s.dc, s.scratch, DIB_RGB_COLORS,
                                    &s.section_bits, s.section, 0);
      else
        s.bitmap = CreateDIBSection(s.dc, s.scratch, DIB_RGB_COLORS,
                                    &s.section_bits, s.section, 0);
      s.scratch->bmiHeader.biSizeImage = s.size;
      if (s.bitmap == NULL)
        CloseHandle(s.section);
      else
      {
        if (s.color_count == 0)
          SetDIBits(s.dc, s.bitmap, 0, info->bmiHeader.biHeight, s.bits, info, DIB_RGB_COLORS);
        else
          SetDIBits(s.dc, s.bitmap, 0, info->bmiHeader.biHeight, s.bits, info, DIB_RGB_COLORS);
        if (copy_info != NULL)
        {
          memcpy(copy_info, info, 0x28);
          memcpy((char *)copy_info + 0x28, info->bmiColors, s.color_count << 2);
        }
      }
      ReleaseDC(NULL, s.dc);
    }
  }
  return s.bitmap;
}

// FUNCTION: MAGIC 0x004944ac
static HBITMAP shell_load_bitmap_file(const char *path, void *copy_info, int unused)
{
  struct
  {
    char debug_text[500];
    HBITMAP bitmap;
    HANDLE file;
    HANDLE section;
    LPVOID view;
    BITMAPINFO *info;
  } s;

  s.bitmap = NULL;
  s.file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, 0x08000000, NULL);
  if (s.file != INVALID_HANDLE_VALUE)
  {
#ifdef MODERN_FIXES
    s.section = CreateFileMappingA(s.file, NULL, SEC_COMMIT | PAGE_READONLY, 0, 0, NULL);
#else
    s.section = CreateFileMappingA(s.file, NULL, 0x08000000, 0, 0, NULL);
#endif
    if (s.section != NULL)
    {
      s.view = MapViewOfFile(s.section, FILE_MAP_READ, 0, 0, 0);
      if (s.view != NULL)
      {
        s.info = (BITMAPINFO *)((char *)s.view + 0xe);
        s.bitmap = shell_create_dib_section(s.info, copy_info, unused);
#ifdef MODERN_FIXES
        UnmapViewOfFile(s.view);
#else
        UnmapViewOfFile(s.info);
#endif
      }
      CloseHandle(s.section);
    }
    CloseHandle(s.file);
  }
  if (global_DIB_debug != 0)
  {
    sprintf(s.debug_text, "%08X LoadDIBSectionFromFile (%s)\n", s.bitmap, path);
    OutputDebugStringA(s.debug_text);
  }
  return s.bitmap;
}

// FUNCTION: MAGIC 0x00556847
static void shell_load_fonts(HWND hwnd, HFONT *text_font, WPARAM *button_font,
                             WPARAM *explanation_font)
{
  struct
  {
    HWND item;
    int size_selector;
    int index;
    HWND second_item;
    LOGFONTA *font_desc;
    RECT client;
  } s;

  GetClientRect(hwnd, &s.client);
  if (s.client.right <= 800 || s.client.bottom <= 600)
    s.size_selector = 4;
  else if (s.client.right <= 640 || s.client.bottom <= 480)
    s.size_selector = 8;
  else
    s.size_selector = 0;
  s.font_desc = LoadFontFromIni("ShellText", 0);
  s.font_desc->lfHeight = (s.client.bottom - s.client.top) / s.font_desc->lfHeight;
  *text_font = CreateFontIndirectA(s.font_desc);
  s.font_desc = LoadFontFromIni("ShellButtons", 0);
  s.font_desc->lfHeight = (s.client.bottom - s.client.top) / s.font_desc->lfHeight;
  *button_font = (WPARAM)CreateFontIndirectA(s.font_desc);
  s.font_desc = LoadFontFromIni("ShellExplains", 0);
  s.font_desc->lfHeight = (s.client.bottom - s.client.top) / s.font_desc->lfHeight;
  *explanation_font = (WPARAM)CreateFontIndirectA(s.font_desc);
  for (s.index = 0; s.index < 10; ++s.index)
  {
    s.item = GetDlgItem(hwnd, s.index + 100);
    if (s.item != NULL)
      SendMessageA(s.item, WM_SETFONT, *button_font, 0);
    s.second_item = GetDlgItem(hwnd, s.index + 200);
    if (s.second_item != NULL)
      SendMessageA(s.second_item, WM_SETFONT, *explanation_font, 0);
  }
}

// FUNCTION: MAGIC 0x005565fb
static void shell_load_resources(HWND hwnd, HBITMAP *background_bitmap,
                                 BITMAP *background_info,
                                 HBITMAP *highlight_bitmap,
                                 HBITMAP *sphere_bitmaps,
                                 HBITMAP *glow_bitmap, HFONT *text_font,
                                 COLORREF *background_color,
                                 COLORREF *mask_color, HBITMAP *box_bitmap,
                                 WPARAM *button_font, WPARAM *explanation_font,
                                 COLORREF *regular_text_color,
                                 COLORREF *selected_text_color)
{
  struct
  {
    char path[264];
    int index;
  } s;

  if (g_display_color_depth == 8)
  {
    sprintf(s.path, "%s\\WINBK_ShellScreen.pic", g_shell_art_directory);
    *background_bitmap = load_pic(s.path);
    *background_color = 0x1000001;
    *mask_color = 0x10000bf;
    sprintf(s.path, "%s\\WINBK_ShellSphereHighlight.pic", g_shell_art_directory);
    *highlight_bitmap = load_pic(s.path);
    sprintf(s.path, "%s\\WINBK_ShellBox.pic", g_shell_art_directory);
    *box_bitmap = load_pic(s.path);
    *regular_text_color = 0x10000b4;
    *selected_text_color = 0x10000ba;
    sprintf(s.path, "%s\\WINBK_ShellScimGlow.pic", g_shell_art_directory);
    *glow_bitmap = load_pic(s.path);
  }
  else
  {
    sprintf(s.path, "%s\\WINBK_ShellScreen16.bmp", g_shell_art_directory);
    *background_bitmap = shell_load_bitmap_file(s.path, NULL, 0);
    *background_color = 0;
    *mask_color = 0xfefefe;
    sprintf(s.path, "%s\\WINBK_ShellSphereHighlight16.bmp", g_shell_art_directory);
    *highlight_bitmap = shell_load_bitmap_file(s.path, NULL, 0);
    sprintf(s.path, "%s\\WINBK_ShellBox16.bmp", g_shell_art_directory);
    *box_bitmap = shell_load_bitmap_file(s.path, NULL, 0);
    *regular_text_color = 0xedd0b1;
    *selected_text_color = 0xd0f0f7;
    sprintf(s.path, "%s\\WINBK_ShellScimGlow16.bmp", g_shell_art_directory);
    *glow_bitmap = shell_load_bitmap_file(s.path, NULL, 0);
  }
  if (*background_bitmap != NULL)
    GetObjectA(*background_bitmap, sizeof(BITMAP), background_info);
  for (s.index = 0; s.index < 5; ++s.index)
    sphere_bitmaps[s.index] = NULL;
  shell_load_fonts(hwnd, text_font, button_font, explanation_font);
}

// FUNCTION: MAGIC 0x00556b33
static void shell_release_fonts(HWND hwnd, HGDIOBJ text_font,
                                HGDIOBJ button_font, HGDIOBJ explanation_font)
{
  struct
  {
    HWND first_item;
    int index;
    HWND second_item;
  } s;

  for (s.index = 0; s.index < 10; ++s.index)
  {
    s.first_item = GetDlgItem(hwnd, s.index + 100);
    if (s.first_item != NULL)
      SendMessageA(s.first_item, WM_SETFONT, 0, 0);
    s.second_item = GetDlgItem(hwnd, s.index + 200);
    if (s.second_item != NULL)
      SendMessageA(s.second_item, WM_SETFONT, 0, 0);
  }
  if (text_font != NULL)
    DeleteObject(text_font);
  text_font = NULL;
  if (button_font != NULL)
    DeleteObject(button_font);
  button_font = NULL;
  if (explanation_font != NULL)
    DeleteObject(explanation_font);
  explanation_font = NULL;
}

// FUNCTION: MAGIC 0x005569d0
static void shell_release_resources(HWND hwnd, HANDLE background_bitmap,
                                    HANDLE highlight_bitmap,
                                    HBITMAP *sphere_bitmaps,
                                    HANDLE glow_bitmap, HGDIOBJ text_font,
                                    HANDLE box_bitmap, HGDIOBJ button_font,
                                    HGDIOBJ explanation_font)
{
  struct
  {
    HWND item;
    int index;
    HWND second_item;
  } s;

  for (s.index = 0; s.index < 10; ++s.index)
  {
    s.item = GetDlgItem(hwnd, s.index + 100);
    if (s.item != NULL)
      SendMessageA(s.item, WM_SETFONT, 0, 0);
    s.second_item = GetDlgItem(hwnd, s.index + 200);
    if (s.second_item != NULL)
      SendMessageA(s.second_item, WM_SETFONT, 0, 0);
  }
  if (background_bitmap != NULL)
    delete_and_close_object(background_bitmap);
  background_bitmap = NULL;
  if (highlight_bitmap != NULL)
    delete_and_close_object(highlight_bitmap);
  highlight_bitmap = NULL;
  if (glow_bitmap != NULL)
    delete_and_close_object(glow_bitmap);
  glow_bitmap = NULL;
  if (box_bitmap != NULL)
    delete_and_close_object(box_bitmap);
  box_bitmap = NULL;
  shell_release_fonts(hwnd, text_font, button_font, explanation_font);
  for (s.index = 0; s.index < 5; ++s.index)
    if (sphere_bitmaps[s.index] != NULL)
    {
      delete_and_close_object(sphere_bitmaps[s.index]);
      sphere_bitmaps[s.index] = NULL;
    }
}

// FUNCTION: MAGIC 0x00556c0e
static void shell_load_draw_resources(shell_draw_resources_t *resources)
{
  char path[264];

  if (g_display_color_depth == 8)
  {
    resources->button_text_color = 0x10000b4;
    resources->light_pen = CreatePen(PS_SOLID, 0, 0x10000a1);
    resources->dark_pen = CreatePen(PS_SOLID, 0, 0x100000e);
    resources->regular_button_color = 0x1000010;
    resources->selected_button_color = 0x10000ba;
    resources->light_brush = CreateSolidBrush(0x10000e5);
    resources->dark_brush = CreateSolidBrush(0x1000058);
    resources->dark_color = 0x1000058;
    resources->background_color = 0x1000001;
    resources->panel_color = 0x10000cb;
    resources->panel_brush = CreateSolidBrush(resources->panel_color);
  }
  else
  {
    resources->button_text_color = 0xedd0b1;
    resources->light_pen = CreatePen(PS_SOLID, 0, 0xcfb090);
    resources->dark_pen = CreatePen(PS_SOLID, 0, 0x4d2e14);
    resources->regular_button_color = (COLORREF)(g_savegame_data_buffer + 0x50d1);
    resources->selected_button_color = 0xd0f0f7;
    resources->light_brush = CreateSolidBrush(0x3f44a5);
    resources->dark_brush = CreateSolidBrush(0x8b736e);
    resources->dark_color = 0x8b736e;
    resources->background_color = 0x121212;
    resources->panel_color = 0x98a7cd;
    resources->panel_brush = CreateSolidBrush(resources->panel_color);
  }
  sprintf(path, "%s\\WINBK_ShellButtonFace.bmp", g_shell_art_directory);
  resources->button_face = shell_load_bitmap_file(path, NULL, 0);
  if (resources->light_pen == NULL)
    resources->light_pen = GetStockObject(WHITE_PEN);
  if (resources->dark_pen == NULL)
    resources->dark_pen = GetStockObject(BLACK_PEN);
  if (resources->light_brush == NULL)
    resources->light_brush = GetStockObject(GRAY_BRUSH);
  if (resources->panel_brush == NULL)
    resources->panel_brush = GetStockObject(GRAY_BRUSH);
}

// FUNCTION: MAGIC 0x00556e04
static void shell_release_draw_resources(shell_draw_resources_t *resources)
{
  if (resources->button_face != NULL)
    delete_and_close_object(resources->button_face);
  if (resources->light_pen != NULL)
    DeleteObject(resources->light_pen);
  if (resources->dark_pen != NULL)
    DeleteObject(resources->dark_pen);
  if (resources->light_brush != NULL)
    DeleteObject(resources->light_brush);
}

// FUNCTION: MAGIC 0x00556e79
static void shell_play_page_sound(int page)
{
  int sound_id;

  sound_id = -1;
  if (page == 2)
    sound_id = 0x3c;
  else if (page == 1)
    sound_id = 0x3d;
  else if (page == 3)
    sound_id = 0x3e;
  else if (page == 4)
    sound_id = 0x3f;
  else if (page == 0)
    sound_id = 0x40;
  if (sound_id != -1)
    play_sound_effect(sound_id);
}

// FUNCTION: MAGIC 0x00556f0d
static void shell_play_choice_sound(int page, int choice)
{
  int sound_id;

  sound_id = -1;
  switch (page)
  {
  case 0:
    break;
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
  case 4:
    break;
  }
  if (sound_id != -1)
    play_sound_effect(sound_id);
}

// FUNCTION: MAGIC 0x0049560c
static void shell_draw_bitmap_button(DRAWITEMSTRUCT *item, HBITMAP background,
                                     HPEN light_pen, HPEN dark_pen,
                                     COLORREF color, int draw_focus)
{
  draw_item(item, NULL, background, light_pen, dark_pen, color, draw_focus, 0x25);
}

// FUNCTION: MAGIC 0x00556f89
void shell_position_choices(HWND hwnd)
{
  struct
  {
    char text[100];
    HWND button;
    HDC dc;
    int index;
    HWND explanation;
    HGDIOBJ font;
    SIZE text_size;
    int row_height;
    HWND scrollbar;
    int button_width;
    int padding;
  } s;
  int measured_button_height;
  int explanation_height;
  int explanation_width;

  s.dc = GetDC(hwnd);
  s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 200, WM_GETFONT, 0, 0);
  SelectObject(s.dc, s.font);
  GetTextExtentPoint32A(s.dc, "fun", 3, &s.text_size);
  explanation_height = s.text_size.cy;
  s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 100, WM_GETFONT, 0, 0);
  SelectObject(s.dc, s.font);
  GetTextExtentPoint32A(s.dc, "fun", 3, &s.text_size);
  s.padding = s.text_size.cy * 3 / 2;
  measured_button_height = s.text_size.cy;
  s.row_height = (shell_choices_rect.bottom - shell_choices_rect.top - 20) /
                 shell_visible_rows;
  s.row_height = s.text_size.cy <= s.row_height ? s.row_height : s.text_size.cy;
  for (s.index = 0; s.index < 10; ++s.index)
  {
    s.button = GetDlgItem(hwnd, s.index + 100);
    if (s.button != NULL)
    {
      GetWindowTextA(s.button, s.text, sizeof(s.text));
      GetTextExtentPoint32A(s.dc, s.text, strlen(s.text), &s.text_size);
      s.button_width = s.text_size.cx + s.padding;
      MoveWindow(s.button, shell_choices_rect.left + 10, 0, s.button_width,
                 s.row_height, TRUE);
    }
    s.explanation = GetDlgItem(hwnd, s.index + 200);
    if (s.explanation != NULL)
    {
      explanation_width = shell_choices_rect.right - shell_choices_rect.left -
                          10 - s.button_width;
      MoveWindow(s.explanation, shell_choices_rect.left + s.button_width + 10, 0,
                 explanation_width, explanation_height, TRUE);
    }
  }
  ReleaseDC(hwnd, s.dc);
  s.scrollbar = GetDlgItem(hwnd, 99);
  if (s.scrollbar != NULL)
    MoveWindow(s.scrollbar, shell_choices_rect.right, shell_choices_rect.top,
               GetSystemMetrics(SM_CXVSCROLL),
               shell_choices_rect.bottom - shell_choices_rect.top, TRUE);
}

// FUNCTION: MAGIC 0x005571b5
static int shell_scroll_choices(HWND hwnd, int page, int position)
{
  struct
  {
    int explanation_height;
    HWND button;
    int index;
    int top;
    int left;
    HWND explanation;
    startup_dialog_page_t *page_data;
    RECT rect;
    int button_height;
  } s;

  if (page == -1)
    return 0;
  {
    LockWindowUpdate(hwnd);
    s.page_data = SHELL_PAGE(page);
    if (position < 0)
      position = 0;
    if (s.page_data->choice_count <= position)
      position = s.page_data->choice_count - 1;
    if (position >= 10)
      position = 9;

    GetWindowRect(GetDlgItem(hwnd, 100), &s.rect);
    MapWindowPoints(NULL, hwnd, (LPPOINT)&s.rect, 2);
    s.left = s.rect.left;
    s.button_height = s.rect.bottom - s.rect.top;
    GetWindowRect(GetDlgItem(hwnd, 200), &s.rect);
    s.explanation_height = s.rect.bottom - s.rect.top;
    s.top = shell_choices_rect.top + 10;

    for (s.index = 0; s.index < 10; ++s.index)
    {
      s.button = GetDlgItem(hwnd, s.index + 100);
      s.explanation = GetDlgItem(hwnd, s.index + 200);
      if (position > s.index || position + shell_visible_rows <= s.index ||
          s.page_data->choice_count <= s.index)
      {
        ShowWindow(s.button, SW_HIDE);
        ShowWindow(s.explanation, SW_HIDE);
      }
      else
      {
        SetWindowPos(s.button, NULL, s.left, s.top, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER);
        GetWindowRect(s.explanation, &s.rect);
        MapWindowPoints(NULL, hwnd, (LPPOINT)&s.rect, 2);
        SetWindowPos(s.explanation, NULL, s.rect.left,
                     s.top + s.button_height / 2 - s.explanation_height / 2,
                     0, 0, SWP_NOSIZE | SWP_NOZORDER);
        s.top += s.button_height;
        ShowWindow(s.button, SW_SHOW);
        ShowWindow(s.explanation, SW_SHOW);
      }
    }
    SetScrollRange(GetDlgItem(hwnd, 99), SB_CTL, 0,
                   s.page_data->choice_count - 1, TRUE);
    SetScrollPos(GetDlgItem(hwnd, 99), SB_CTL, position, TRUE);
    LockWindowUpdate(NULL);
  }
  return position;
}

// FUNCTION: MAGIC 0x005575b4
BOOL CALLBACK shell_help_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int line_count;
    HDC dc;
    int line_height;
    int line_spacing;
    int index;
    int text_top;
    int left_margin;
    int right_margin;
    int initial_top;
    HGDIOBJ font;
    RECT rect;
  } s;

  (void)lparam;
  switch (msg)
  {
  case WM_INITDIALOG:
    ShowWindow(GetDlgItem(hwnd, 0x403), SW_HIDE);
    return 1;
  case WM_COMMAND:
    EndDialog(hwnd, 1);
    return 1;
  case WM_ERASEBKGND:
  {
    s.dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.dc);
    GetClientRect(hwnd, &s.rect);
    FillRect(s.dc, &s.rect, GetStockObject(BLACK_BRUSH));
    s.line_count = load_text(global_ui_strings_filename, "ABOUTBOX_EXPANSION1");
    s.font = (HGDIOBJ)SendMessageA(hwnd, WM_GETFONT, 0, 0);
    SelectObject(s.dc, s.font);
    SetTextColor(s.dc, 0xfefefe);
    SetBkMode(s.dc, TRANSPARENT);
    s.right_margin = 10;
    s.initial_top = 10;
    s.line_spacing = 10;
    s.left_margin = s.right_margin;
    s.text_top = s.initial_top;
    for (s.index = 0; s.index < s.line_count; ++s.index)
    {
      GetClientRect(hwnd, &s.rect);
      s.rect.right -= s.right_margin;
      s.rect.left = s.left_margin;
      s.rect.top = s.text_top;
      s.line_height = DrawTextA(s.dc, g_text_lines[s.index], -1, &s.rect,
                                DT_CALCRECT | DT_WORDBREAK);
      GetClientRect(hwnd, &s.rect);
      s.rect.right -= s.right_margin;
      s.rect.left = s.left_margin;
      s.rect.top = s.text_top;
      DrawTextA(s.dc, g_text_lines[s.index], -1, &s.rect, DT_WORDBREAK);
      s.text_top += s.line_height + s.line_spacing;
    }
    s.text_top += 0 - s.line_spacing + s.initial_top;
    GetClientRect(hwnd, &s.rect);
    if (s.rect.bottom < s.text_top)
    {
      s.line_height = s.text_top - s.rect.bottom;
      GetWindowRect(hwnd, &s.rect);
      SetWindowPos(hwnd, NULL, 0, 0, s.rect.right - s.rect.left,
                   s.rect.bottom - s.rect.top + s.line_height,
                   SWP_NOMOVE | SWP_NOZORDER);
    }
    return 1;
  }
  case WM_CHAR:
  case WM_LBUTTONDOWN:
    EndDialog(hwnd, 1);
    return 1;
  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x005539c3
LRESULT CALLBACK magic_shell_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    // int double_click_delay;
    int pad;
    RECT logical;
    POINT point;
    POINT up_point;
    HDC up_dc;
    int down_page;
    MSG double_click;
    POINT down_point;
    HDC down_dc;
    int down_loop;
    int hovered_page;
    RECT hover_update;
    POINT hover_point;
    HDC hover_dc;
    int hover_loop;
    RECT paint_logical;
    char paint_path[264];
    HBITMAP paint_sphere;
    int sphere_height;
    BITMAP bitmap;
    int sphere_width;
    HDC dc;
    HDC paint_offscreen_dc;
    PAINTSTRUCT paint;
    int saved_dc;
    RECT update;
    RECT client;
    char destroy_path[264];
    COLORREF draw_color;
    int draw_id;
    DRAWITEMSTRUCT *draw_item;
    HWND ctl_window;
    int ctl_id;
    HDC ctl_dc;
    HWND invalidate_second;
    HWND invalidate_first;
    HWND create_button;
    int create_ok;
    int create_button_height;
    int create_button_width;
    int create_page;
    int create_button_top;
    int create_button_left;
    HWND create_explanation;
    HWND create_scrollbar;
    RECT command_screen_rect;
    int command_old_selected;
    HBITMAP command_sphere;
    char command_path[264];
    BITMAP command_bitmap;
    int command_loop;
    int command_page;
    int command_old_page;
    int command_id;
    int command_notification;
    startup_dialog_page_t *command_page_data;
    HDC child_paint_dc;
    int child_paint_saved_dc;
    RECT child_paint_update;
    RECT *child_paint_rect;
    RECT child_paint_client;
  } s;

  switch (msg)
  {
  case 0x496:
    if (wparam != 0)
    {
      ShowWindow(hwnd, SW_SHOW);
      UpdateWindow(hwnd);
    }
    else
      ShowWindow(hwnd, SW_HIDE);
    return 0;

  case 0x498:
  {
    HDC target_dc = (HDC)wparam;
    RECT *target_rect = (RECT *)lparam;

    s.child_paint_dc = target_dc;
    s.child_paint_rect = target_rect;
    s.child_paint_saved_dc = SaveDC(s.child_paint_dc);
    ApplyCardArtPaletteToDc(s.child_paint_dc);
    if (shell_box_visible)
    {
      shell_set_map_mode(s.child_paint_dc, hwnd, shell_background_bitmap);
      OffsetViewportOrgEx(s.child_paint_dc, -s.child_paint_rect->left,
                          -s.child_paint_rect->top, NULL);
      if (shell_box_bitmap != NULL)
        DrawBitmapToRect(s.child_paint_dc, &shell_background_rect,
                         shell_box_bitmap);
      CopyRect(&s.child_paint_update, &shell_background_rect);
      LPtoDP(s.child_paint_dc, (LPPOINT)&s.child_paint_update, 2);
    }
    SetMapMode(s.child_paint_dc, MM_TEXT);
    SetViewportOrgEx(s.child_paint_dc, 0, 0, NULL);
    SetWindowOrgEx(s.child_paint_dc, 0, 0, NULL);
    ExcludeClipRect(s.child_paint_dc, s.child_paint_update.left, s.child_paint_update.top,
                    s.child_paint_update.right, s.child_paint_update.bottom);
    GetClientRect(hwnd, &s.child_paint_client);
    if (shell_background_bitmap != NULL)
      StretchBlt(s.child_paint_dc, s.child_paint_client.left - s.child_paint_rect->left,
                 s.child_paint_client.top - s.child_paint_rect->top,
                 s.child_paint_client.right - s.child_paint_client.left,
                 s.child_paint_client.bottom - s.child_paint_client.top,
                 shell_background_dc, 0, 0, shell_background_info.bmWidth,
                 shell_background_info.bmHeight, SRCCOPY);
    else
      FillRect(s.child_paint_dc, s.child_paint_rect, GetStockObject(GRAY_BRUSH));
    RestoreDC(s.child_paint_dc, s.child_paint_saved_dc);
  }
    return 0;

  case WM_CLOSE:
    DestroyWindow(hwnd);
    return 0;

  case WM_COMMAND:
    s.command_id = wparam & 0xffff;
    s.command_notification = HIWORD(wparam);
    if (s.command_id >= 50 && s.command_id <= g_startup_dialog_group_count + 50)
    {
      s.command_old_page = g_startup_dialog_group;
      s.command_page = s.command_id - 50;
      g_startup_dialog_group = -1;
      if (s.command_old_page != s.command_page || s.command_notification == 2)
      {
        shell_enable_animation(0);
        if (s.command_old_page != -1)
        {
          LockWindowUpdate(hwnd);
          shell_invalidate_logical_rect(
              hwnd, shell_background_bitmap,
              &SHELL_PAGE(s.command_old_page)->page_rect);
          if (s.command_old_page != -1 && shell_selected_choice != -1)
          {
            if (SHELL_OPTION(s.command_old_page, shell_selected_choice)->is_modeless != 0)
            {
              SHELL_OPTION(s.command_old_page, shell_selected_choice)->close();
              g_startup_modeless_dialog = NULL;
              shell_selected_choice = -1;
            }
          }
          if (shell_sphere_bitmaps[s.command_old_page] != NULL)
          {
            delete_and_close_object(shell_sphere_bitmaps[s.command_old_page]);
            shell_sphere_bitmaps[s.command_old_page] = NULL;
          }
          for (s.command_loop = 0;
               s.command_loop < SHELL_CHOICE_COUNT(s.command_old_page);
               ++s.command_loop)
          {
            ShowWindow(GetDlgItem(hwnd, s.command_loop + 100), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, s.command_loop + 200), SW_HIDE);
          }
          LockWindowUpdate(NULL);
          UpdateWindow(hwnd);
        }
        g_startup_dialog_group = s.command_page;
        shell_play_page_sound(g_startup_dialog_group);
        shell_animation_frame = 0;
        s.command_page_data = SHELL_PAGE(g_startup_dialog_group);
        if (!shell_box_visible)
        {
          shell_box_visible = 1;
          shell_invalidate_logical_rect(hwnd, shell_background_bitmap,
                                        &shell_background_rect);
        }
        if (shell_sphere_bitmaps[g_startup_dialog_group] == NULL)
        {
          if (g_display_color_depth == 8)
          {
            sprintf(s.command_path, "%s\\WINBK_ShellSphereAnimation-%d.pcx",
                    g_shell_art_directory, g_startup_dialog_group + 1);
            shell_sphere_bitmaps[g_startup_dialog_group] = load_pic(s.command_path);
          }
          else
          {
            sprintf(s.command_path, "%s\\WINBK_ShellSphereAnimation16-%d.bmp",
                    g_shell_art_directory, g_startup_dialog_group + 1);
            shell_sphere_bitmaps[g_startup_dialog_group] = shell_load_bitmap_file(s.command_path, NULL, 0);
          }
          s.command_sphere = shell_sphere_bitmaps[g_startup_dialog_group];
          if (s.command_sphere != NULL)
          {
            GetObjectA(s.command_sphere, sizeof(BITMAP), &s.command_bitmap);
            shell_sphere_rects[g_startup_dialog_group].right = shell_sphere_rects[g_startup_dialog_group].left +
                                                               s.command_bitmap.bmWidth / 12;
            shell_sphere_rects[g_startup_dialog_group].bottom = shell_sphere_rects[g_startup_dialog_group].top +
                                                                s.command_bitmap.bmHeight;
          }
        }
        SelectObject(shell_sphere_dc, shell_sphere_bitmaps[g_startup_dialog_group]);
        shell_invalidate_logical_rect(hwnd, shell_background_bitmap,
                                      &s.command_page_data->page_rect);
        for (s.command_loop = 0; s.command_loop < s.command_page_data->choice_count;
             ++s.command_loop)
        {
          SetDlgItemTextA(hwnd, s.command_loop + 100,
                          s.command_page_data->choices[s.command_loop].label);
          SetDlgItemTextA(hwnd, s.command_loop + 200,
                          s.command_page_data->choices[s.command_loop].description);
          if (s.command_page_data->choices[s.command_loop].is_modeless != 0)
            SendDlgItemMessageA(hwnd, s.command_loop + 100, BM_SETSTYLE,
                                WS_CHILD | BS_RADIOBUTTON, TRUE);
          else
            SendDlgItemMessageA(hwnd, s.command_loop + 100, BM_SETSTYLE,
                                WS_CHILD | BS_OWNERDRAW, TRUE);
        }
        shell_position_choices(hwnd);
        shell_scroll_position = 0;
        shell_scroll_choices(hwnd, g_startup_dialog_group, shell_scroll_position);
        if (s.command_page_data->choice_count > shell_visible_rows)
        {
          ShowScrollBar(GetDlgItem(hwnd, 99), SB_CTL, TRUE);
        }
        else
        {
          ShowScrollBar(GetDlgItem(hwnd, 99), SB_CTL, FALSE);
        }
        if (s.command_page_data->choices[0].is_modeless != 0)
        {
          SendMessageA(hwnd, WM_COMMAND, 0x20064, 0);
        }
        else
        {
        }
        shell_enable_animation(1);
      }
      else if (s.command_page == s.command_old_page)
      {
        if (s.command_old_page != -1)
        {
          LockWindowUpdate(hwnd);
          shell_invalidate_logical_rect(hwnd, shell_background_bitmap,
                                        &SHELL_PAGE(s.command_old_page)->page_rect);
          for (s.command_loop = 0;
               s.command_loop < SHELL_CHOICE_COUNT(s.command_old_page); ++s.command_loop)
          {
            ShowWindow(GetDlgItem(hwnd, s.command_loop + 100), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, s.command_loop + 200), SW_HIDE);
          }
          if (s.command_old_page != -1 && shell_selected_choice != -1)
          {
            if (SHELL_OPTION(s.command_old_page, shell_selected_choice)->is_modeless != 0)
            {
              SHELL_OPTION(s.command_old_page, shell_selected_choice)->close();
              g_startup_modeless_dialog = NULL;
              shell_selected_choice = -1;
            }
          }
          LockWindowUpdate(NULL);
        }
        g_startup_dialog_group = -1;
        shell_box_visible = 0;
        InvalidateRect(hwnd, NULL, TRUE);
      }
    }
    else if (g_startup_dialog_group != -1 && s.command_id >= 100 &&
             s.command_id < SHELL_CHOICE_COUNT(g_startup_dialog_group) + 100)
    {
      s.command_old_selected = shell_selected_choice;
      shell_selected_choice = s.command_id - 100;
      if (s.command_old_selected != shell_selected_choice ||
          s.command_notification == 2)
      {
        shell_enable_animation(0);
        LockWindowUpdate(hwnd);
        s.command_page_data = SHELL_PAGE(g_startup_dialog_group);
        if (s.command_old_selected != -1)
        {
          InvalidateRect(GetDlgItem(hwnd, s.command_old_selected + 100), NULL, FALSE);
          InvalidateRect(GetDlgItem(hwnd, s.command_old_selected + 200), NULL, FALSE);
          if (s.command_page_data->choices[s.command_old_selected].is_modeless != 0)
            s.command_page_data->choices[s.command_old_selected].close();
        }
        CheckRadioButton(hwnd, 100, 109, shell_selected_choice + 100);
        if (shell_selected_choice < shell_scroll_position)
          shell_scroll_position = shell_scroll_choices(
              hwnd, g_startup_dialog_group, shell_selected_choice);
        else if (shell_visible_rows + shell_scroll_position - 1 < shell_selected_choice)
          shell_scroll_position = shell_scroll_choices(
              hwnd, g_startup_dialog_group,
              shell_selected_choice - shell_visible_rows + 1);
        else
        {
          InvalidateRect(GetDlgItem(hwnd, shell_selected_choice + 100), NULL, FALSE);
          InvalidateRect(GetDlgItem(hwnd, shell_selected_choice + 200), NULL, FALSE);
        }
        shell_play_choice_sound(g_startup_dialog_group, shell_selected_choice);
        if (s.command_page_data->choices[shell_selected_choice].is_modeless != 0)
        {
          g_startup_modeless_dialog =
              s.command_page_data->choices[shell_selected_choice].open(
                  (LPARAM)&shell_dialog_rect);
          if (g_startup_modeless_dialog != NULL)
          {
            if (!shell_box_visible)
            {
              shell_box_visible = 1;
              shell_invalidate_logical_rect(hwnd, shell_background_bitmap,
                                            &shell_background_rect);
            }
            ShowWindow(g_startup_modeless_dialog, SW_SHOW);
          }
        }
        else
        {
          CopyRect(&s.command_screen_rect, &shell_dialog_rect);
          MapWindowPoints(hwnd, NULL, (LPPOINT)&s.command_screen_rect, 2);
          s.command_page_data->choices[shell_selected_choice].action(
              &s.command_screen_rect);
          InvalidateRect(GetDlgItem(hwnd, shell_selected_choice + 100), NULL, FALSE);
          InvalidateRect(GetDlgItem(hwnd, shell_selected_choice + 200), NULL, FALSE);
          shell_selected_choice = -1;
        }
        LockWindowUpdate(NULL);
        shell_enable_animation(1);
      }
    }
    return 0;

  case WM_CREATE:
    s.create_ok = 1;
    g_startup_dialog_group = -1;
    shell_selected_choice = -1;
    shell_box_visible = 0;
    shell_help_hovered = 0;
    shell_hovered_page = -1;
    init_sound_dll(hwnd, 0, 0);
    if (!shell_initialize_pages())
      s.create_ok = 0;
    s.create_button_left = shell_choices_rect.left + 10;
    s.create_button_top = shell_choices_rect.top + 10;
    s.create_button_width = shell_choices_rect.right - shell_choices_rect.left - 10;
    s.create_button_height = (shell_choices_rect.bottom - shell_choices_rect.top - 20) /
                             shell_visible_rows;
    for (s.create_page = 0; s.create_page < 10; ++s.create_page)
    {
      s.create_button = CreateWindowExA(0, "BUTTON", "", WS_CHILD | BS_RADIOBUTTON,
                                        s.create_button_left, s.create_button_top,
                                        s.create_button_width, s.create_button_height, hwnd,
                                        (HMENU)(s.create_page + 100), g_app_instance, NULL);
      if (s.create_button == NULL)
        s.create_ok = 0;
      s.create_explanation = CreateWindowExA(0, "STATIC", "", WS_CHILD,
                                             s.create_button_left, s.create_button_top,
                                             s.create_button_width, s.create_button_height, hwnd,
                                             (HMENU)(s.create_page + 200), g_app_instance, NULL);
      if (s.create_explanation == NULL)
        s.create_ok = 0;
      s.create_button_top += s.create_button_height;
    }
    CheckDlgButton(hwnd, 100, BST_CHECKED);
    change_buttonclass_wndproc(hwnd);
    shell_scroll_position = 0;
    s.create_scrollbar = CreateWindowExA(0, "SCROLLBAR", "", WS_CHILD | SBS_VERT,
                                         shell_choices_rect.right, shell_choices_rect.top,
                                         GetSystemMetrics(SM_CXVSCROLL),
                                         shell_choices_rect.bottom - shell_choices_rect.top,
                                         hwnd, (HMENU)99, g_app_instance, NULL);
    if (s.create_scrollbar != NULL)
    {
      SetScrollRange(s.create_scrollbar, SB_CTL, 0, 9, TRUE);
      SetScrollPos(s.create_scrollbar, SB_CTL, 0, TRUE);
    }
    else
      s.create_ok = 0;
    SetFocus(hwnd);
    shell_load_resources(hwnd, &shell_background_bitmap, &shell_background_info,
                         &shell_highlight_bitmap, shell_sphere_bitmaps,
                         &shell_glow_bitmap, &shell_text_font,
                         &shell_bitmap_background_color, &shell_bitmap_mask_color,
                         &shell_box_bitmap, (WPARAM *)&shell_button_font,
                         (WPARAM *)&shell_explanation_font,
                         &shell_regular_text_color, &shell_selected_text_color);
    shell_background_dc = CreateCompatibleDC(NULL);
    ApplyCardArtPaletteToDc(shell_background_dc);
    shell_sphere_dc = CreateCompatibleDC(NULL);
    ApplyCardArtPaletteToDc(shell_sphere_dc);
    if (shell_background_bitmap == NULL || shell_box_bitmap == NULL ||
        shell_background_dc == NULL || shell_sphere_dc == NULL)
      s.create_ok = 0;
    else
      SelectObject(shell_background_dc, shell_background_bitmap);
    shell_resources_loaded = 1;
    shell_load_draw_resources(&shell_draw_resources);
    shell_animation_frame = 0;
    SetTimer(hwnd, 1, shell_animation_period, NULL);
    if (!s.create_ok)
    {
      KillTimer(hwnd, 1);
      shell_release_resources(hwnd, shell_background_bitmap, shell_highlight_bitmap,
                              shell_sphere_bitmaps, shell_glow_bitmap,
                              shell_text_font, shell_box_bitmap, shell_button_font,
                              shell_explanation_font);
      shell_release_draw_resources(&shell_draw_resources);
      return -1;
    }
    else
    {
      return 0;
    }

  case 0x4c8:
    s.invalidate_first = (HWND)wparam;
    s.invalidate_second = (HWND)lparam;
    if (s.invalidate_first != NULL)
      InvalidateRect(s.invalidate_first, NULL, TRUE);
    if (s.invalidate_second != NULL)
      InvalidateRect(s.invalidate_second, NULL, TRUE);
    return 0;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.ctl_dc = (HDC)wparam;
    s.ctl_window = (HWND)lparam;
    ApplyCardArtPaletteToDc(s.ctl_dc);
    s.ctl_id = GetDlgCtrlID(s.ctl_window);
    SetBkMode(s.ctl_dc, TRANSPARENT);
    if (GetDlgCtrlID(s.ctl_window) - 100 == shell_selected_choice ||
        GetDlgCtrlID(s.ctl_window) - 200 == shell_selected_choice)
      SetTextColor(s.ctl_dc, shell_selected_text_color);
    else
      SetTextColor(s.ctl_dc, shell_regular_text_color);
    return (LRESULT)GetStockObject(NULL_BRUSH);

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    s.draw_id = (int)wparam;
    if (s.draw_id - 100 == shell_selected_choice ||
        s.draw_id - 200 == shell_selected_choice)
      s.draw_color = shell_draw_resources.selected_button_color;
    else
      s.draw_color = shell_draw_resources.regular_button_color;
    InflateRect(&s.draw_item->rcItem, -4, -4);
    if (shell_draw_resources.button_face != NULL)
      shell_draw_bitmap_button(s.draw_item, shell_draw_resources.button_face,
                               shell_draw_resources.light_pen,
                               shell_draw_resources.dark_pen, s.draw_color, 0);
    else
      draw_owner_draw_button_centered(s.draw_item, GetStockObject(GRAY_BRUSH),
                                      shell_draw_resources.light_pen,
                                      shell_draw_resources.dark_pen, s.draw_color, 0);
    return 1;

  case WM_DESTROY:
    strcpy(s.destroy_path, global_base_directory);
    strcat(s.destroy_path, "\\duel.hlp");
    WinHelpA(global_main_hwnd, s.destroy_path, HELP_QUIT, 0);
    KillTimer(hwnd, 1);
    prepare_duel_video_mode_transition();
    sound_close();
    DeleteDC(shell_background_dc);
    DeleteDC(shell_sphere_dc);
    shell_release_resources(hwnd, shell_background_bitmap, shell_highlight_bitmap,
                            shell_sphere_bitmaps, shell_glow_bitmap,
                            shell_text_font, shell_box_bitmap, shell_button_font,
                            shell_explanation_font);
    shell_release_draw_resources(&shell_draw_resources);
    PostQuitMessage(0);
    return 0;

  case WM_PAINT:
    EnterCriticalSection(&g_card_render_lock);
    s.paint_offscreen_dc = g_shared_offscreen_dc;
    s.saved_dc = SaveDC(g_shared_offscreen_dc);
    ApplyCardArtPaletteToDc(s.paint_offscreen_dc);
    GetClientRect(hwnd, &s.client);
    GetUpdateRect(hwnd, &s.update, FALSE);
    IntersectClipRect(s.paint_offscreen_dc, s.update.left, s.update.top,
                      s.update.right, s.update.bottom);
    s.dc = BeginPaint(hwnd, &s.paint);
    SetMapMode(s.paint_offscreen_dc, MM_TEXT);
    if (shell_background_bitmap != NULL)
      StretchBlt(s.paint_offscreen_dc, s.client.left, s.client.top,
                 s.client.right - s.client.left, s.client.bottom - s.client.top,
                 shell_background_dc, 0, 0, shell_background_info.bmWidth,
                 shell_background_info.bmHeight, SRCCOPY);
    else
      FillRect(s.paint_offscreen_dc, &s.client, GetStockObject(GRAY_BRUSH));
    shell_set_map_mode(s.paint_offscreen_dc, hwnd, shell_background_bitmap);
    if (g_startup_dialog_group != -1)
    {
      if (GetSystemMetrics(0x49) != 0)
      {
        if (shell_highlight_bitmap != NULL)
          draw_masked_bitmap_left_half_to_rect(
              s.paint_offscreen_dc,
              &SHELL_PAGE(g_startup_dialog_group)->page_rect,
              shell_highlight_bitmap);
      }
      else
      {
        if (shell_sphere_bitmaps[g_startup_dialog_group] == NULL)
        {
          if (g_display_color_depth == 8)
          {
            sprintf(s.paint_path, "%s\\WINBK_ShellSphereAnimation-%d.pcx",
                    g_shell_art_directory, g_startup_dialog_group + 1);
            shell_sphere_bitmaps[g_startup_dialog_group] = load_pic(s.paint_path);
          }
          else
          {
            sprintf(s.paint_path, "%s\\WINBK_ShellSphereAnimation16-%d.bmp",
                    g_shell_art_directory, g_startup_dialog_group + 1);
            shell_sphere_bitmaps[g_startup_dialog_group] =
                shell_load_bitmap_file(s.paint_path, NULL, 0);
          }
          s.paint_sphere = shell_sphere_bitmaps[g_startup_dialog_group];
          if (s.paint_sphere != NULL)
          {
            GetObjectA(s.paint_sphere, sizeof(BITMAP), &s.bitmap);
            shell_sphere_rects[g_startup_dialog_group].right =
                shell_sphere_rects[g_startup_dialog_group].left + s.bitmap.bmWidth / 12;
            shell_sphere_rects[g_startup_dialog_group].bottom =
                shell_sphere_rects[g_startup_dialog_group].top + s.bitmap.bmHeight;
          }
        }
        s.paint_sphere = shell_sphere_bitmaps[g_startup_dialog_group];
        if (s.paint_sphere != NULL)
        {
          GetObjectA(s.paint_sphere, sizeof(BITMAP), &s.bitmap);
          s.sphere_width = s.bitmap.bmWidth / 12;
          s.sphere_height = s.bitmap.bmHeight;
          CopyRect(&s.paint_logical, &shell_sphere_rects[g_startup_dialog_group]);
          StretchBlt(s.paint_offscreen_dc, s.paint_logical.left, s.paint_logical.top,
                     s.paint_logical.right - s.paint_logical.left,
                     s.paint_logical.bottom - s.paint_logical.top, shell_sphere_dc,
                     shell_animation_frame * s.sphere_width, 0,
                     s.sphere_width, s.sphere_height, SRCCOPY);
        }
      }
    }
    if (shell_help_hovered && !shell_box_visible && shell_glow_bitmap != NULL)
      DrawBitmapToRect(s.paint_offscreen_dc, &shell_box_rect, shell_glow_bitmap);
    if (shell_box_visible && shell_box_bitmap != NULL)
      DrawBitmapToRect(s.paint_offscreen_dc, &shell_background_rect, shell_box_bitmap);
    RestoreDC(g_shared_offscreen_dc, s.saved_dc);
    s.paint_offscreen_dc = s.dc;
    ApplyCardArtPaletteToDc(s.paint_offscreen_dc);
    BitBlt(s.paint_offscreen_dc, s.update.left, s.update.top, s.update.right - s.update.left,
           s.update.bottom - s.update.top, g_shared_offscreen_dc,
           s.update.left, s.update.top, SRCCOPY);
    shell_set_map_mode(s.paint_offscreen_dc, hwnd, shell_background_bitmap);
    if (shell_hovered_page != -1 && shell_highlight_bitmap != NULL)
      draw_masked_bitmap_left_half_to_rect(s.paint_offscreen_dc,
                                           &SHELL_PAGE(shell_hovered_page)->page_rect, shell_highlight_bitmap);
    EndPaint(hwnd, &s.paint);
    LeaveCriticalSection(&g_card_render_lock);
    return 1;

  case WM_MOUSEMOVE:
    s.hover_point.x = lparam & 0xffff;
    s.hover_point.y = HIWORD(lparam);
    s.hover_dc = GetDC(hwnd);
    shell_set_map_mode(s.hover_dc, hwnd, shell_background_bitmap);
    DPtoLP(s.hover_dc, &s.hover_point, 1);
    CopyRect(&s.hover_update, &shell_box_rect);
    LPtoDP(s.hover_dc, (LPPOINT)&s.hover_update, 2);
    ReleaseDC(hwnd, s.hover_dc);
    s.hovered_page = -1;
    for (s.hover_loop = 0; s.hover_loop < g_startup_dialog_group_count; ++s.hover_loop)
      if (SHELL_POINT_IN_RECT(&SHELL_PAGE(s.hover_loop)->page_rect, s.hover_point))
        s.hovered_page = s.hover_loop;
    if (s.hovered_page == g_startup_dialog_group)
      s.hovered_page = -1;
    if (s.hovered_page != shell_hovered_page)
    {
      if (shell_hovered_page != -1)
        shell_invalidate_logical_rect(hwnd, shell_background_bitmap,
                                      &SHELL_PAGE(shell_hovered_page)->page_rect);
      shell_hovered_page = s.hovered_page;
      if (shell_hovered_page != -1)
        shell_invalidate_logical_rect(hwnd, shell_background_bitmap,
                                      &SHELL_PAGE(shell_hovered_page)->page_rect);
    }
    if (!shell_box_visible)
    {
      if (SHELL_POINT_IN_RECT(&shell_help_rect, s.hover_point))
      {
        if (!shell_help_hovered)
        {
          shell_help_hovered = 1;
          InvalidateRect(hwnd, &s.hover_update, TRUE);
        }
      }
      if (!SHELL_POINT_IN_RECT(&shell_help_rect, s.hover_point) && shell_help_hovered)
      {
        shell_help_hovered = 0;
        InvalidateRect(hwnd, &s.hover_update, TRUE);
      }
    }
    return 0;

  case WM_LBUTTONDOWN:
    s.down_point.x = lparam & 0xffff;
    s.down_point.y = HIWORD(lparam);
    s.down_dc = GetDC(hwnd);
    shell_set_map_mode(s.down_dc, hwnd, shell_background_bitmap);
    DPtoLP(s.down_dc, &s.down_point, 1);
    ReleaseDC(hwnd, s.down_dc);
    s.down_page = -1;
    for (s.down_loop = 0; s.down_loop < g_startup_dialog_group_count; ++s.down_loop)
      if (SHELL_POINT_IN_RECT(&SHELL_PAGE(s.down_loop)->page_rect, s.down_point))
        s.down_page = s.down_loop;
    if (s.down_page != -1)
    {
      SendMessageA(hwnd, WM_COMMAND, s.down_page + 50, 0);
      UpdateWindow(hwnd);
      GetDoubleClickTime();
      GetTickCount();
      GetMessageTime();
      {
        int double_click_delay = 0;
        if (0)
        {
          double_click_delay = GetDoubleClickTime() - (GetTickCount() - GetMessageTime());
        }

        Sleep(double_click_delay);
      }

      if (g_startup_dialog_group != -1 &&
          PeekMessageA(&s.double_click, NULL, WM_LBUTTONDBLCLK,
                       WM_LBUTTONDBLCLK, PM_REMOVE))
      {
        if (g_startup_modeless_dialog != NULL)
          SendMessageA(g_startup_modeless_dialog, WM_COMMAND, 1,
                       (LPARAM)GetDlgItem(g_startup_modeless_dialog, 1));
        else
          SendMessageA(hwnd, WM_COMMAND, 0x20064, 0);
      }
    }
    else
    {
      if (SHELL_POINT_IN_RECT(&shell_minimize_rect, s.down_point))
      {
        SendMessageA(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
      }
      else if (SHELL_POINT_IN_RECT(&shell_maximize_rect, s.down_point))
      {
        SendMessageA(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
      }
      else if (SHELL_POINT_IN_RECT(&shell_close_rect, s.down_point))
      {
      }
      else
      {
        if (g_startup_dialog_group == -1)
        {
          if (SHELL_POINT_IN_RECT(&shell_help_rect, s.down_point))
          {
            DialogBoxParamA(g_app_instance, (LPCSTR)0xf7, hwnd,
                            shell_help_dialog_proc, 0);
          }
        }
        else
        {
          SendMessageA(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
        }
      }
    }
    return 0;

  case WM_LBUTTONUP:
    s.up_point.x = lparam & 0xffff;
    s.up_point.y = HIWORD(lparam);
    s.up_dc = GetDC(hwnd);
    shell_set_map_mode(s.up_dc, hwnd, shell_background_bitmap);
    DPtoLP(s.up_dc, &s.up_point, 1);
    ReleaseDC(hwnd, s.up_dc);
    if (SHELL_POINT_IN_RECT(&shell_close_rect, s.up_point))
      SendMessageA(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
    return 0;

  case WM_VSCROLL:
  {
    struct
    {
      int code;
      LPARAM scrollbar;
      int thumb_position;
      int position;
    } scroll;

    scroll.code = wparam & 0xffff;
    scroll.thumb_position = HIWORD(wparam);
    scroll.scrollbar = lparam;
    scroll.position = shell_scroll_position;
    switch (scroll.code)
    {
    case SB_LINEUP:
      scroll.position--;
      break;
    case SB_PAGEUP:
      scroll.position -= shell_visible_rows;
      break;
    case SB_LINEDOWN:
      scroll.position++;
      break;
    case SB_PAGEDOWN:
      scroll.position += shell_visible_rows;
      break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
      scroll.position = scroll.thumb_position;
      break;
    case SB_TOP:
      scroll.position = 0;
      break;
    case SB_BOTTOM:
      scroll.position = 9;
      break;
    }
    if (scroll.position != shell_scroll_position)
      shell_scroll_position = shell_scroll_choices(hwnd, g_startup_dialog_group,
                                                   scroll.position);
  }
    return 0;

  case WM_DISPLAYCHANGE:
  {
    HCURSOR previous_cursor = GetCursor();
    int depth_changed;

    SetCursor(LoadCursorA(NULL, IDC_WAIT));
    if (g_display_color_depth != (int)wparam)
    {
      depth_changed = 1;
      g_display_color_depth = wparam;
    }
    else
      depth_changed = 0;
    if (depth_changed)
    {
      shell_release_resources(hwnd, shell_background_bitmap, shell_highlight_bitmap,
                              shell_sphere_bitmaps, shell_glow_bitmap,
                              shell_text_font, shell_box_bitmap, shell_button_font,
                              shell_explanation_font);
      shell_release_draw_resources(&shell_draw_resources);
      shell_load_resources(hwnd, &shell_background_bitmap, &shell_background_info,
                           &shell_highlight_bitmap, shell_sphere_bitmaps,
                           &shell_glow_bitmap, &shell_text_font,
                           &shell_bitmap_background_color, &shell_bitmap_mask_color,
                           &shell_box_bitmap, (WPARAM *)&shell_button_font,
                           (WPARAM *)&shell_explanation_font,
                           &shell_regular_text_color, &shell_selected_text_color);
      shell_load_draw_resources(&shell_draw_resources);
      if (g_startup_dialog_group != -1 && shell_selected_choice != -1 &&
          SHELL_OPTION(g_startup_dialog_group, shell_selected_choice)->is_modeless != 0 &&
          g_startup_modeless_dialog != NULL)
        SendMessageA(g_startup_modeless_dialog, 0x499, wparam, lparam);
    }
    if (!IsIconic(hwnd) && IsWindowVisible(hwnd))
    {
      InvalidateRect(hwnd, NULL, TRUE);
      MoveWindow(hwnd, 1, 0, (lparam & 0xffff) - 1, HIWORD(lparam), TRUE);
    }
    else
      shell_restore_pending = 1;
    SetCursor(previous_cursor);
  }
    return 0;

  case 0x501:
    shell_restore_pending = 0;
    MoveWindow(hwnd, 1, 0, GetSystemMetrics(SM_CXSCREEN) - 1,
               GetSystemMetrics(SM_CYSCREEN), TRUE);
    return 0;

  case WM_QUERYOPEN:
    if (shell_restore_pending)
      PostMessageA(hwnd, 0x501, 0, 0);
    return 1;

  case WM_SETFOCUS:
    if (g_startup_dialog_group != -1 && shell_selected_choice != -1 &&
        g_startup_modeless_dialog != NULL &&
        IsWindowVisible(g_startup_modeless_dialog))
      SetFocus(g_startup_modeless_dialog);
    return 0;

  case WM_SHOWWINDOW:
  {
    WPARAM show_request;

    show_request = wparam;
    if (show_request != 0)
    {
      if (!shell_resources_loaded)
      {
        shell_load_resources(hwnd, &shell_background_bitmap, &shell_background_info,
                             &shell_highlight_bitmap, shell_sphere_bitmaps,
                             &shell_glow_bitmap, &shell_text_font,
                             &shell_bitmap_background_color, &shell_bitmap_mask_color,
                             &shell_box_bitmap, (WPARAM *)&shell_button_font,
                             (WPARAM *)&shell_explanation_font,
                             &shell_regular_text_color, &shell_selected_text_color);
        shell_resources_loaded = 1;
      }
    }
    else
    {
    }
  }
    return 0;

  case WM_GETMINMAXINFO:
  {
    MINMAXINFO *limits;

    limits = (MINMAXINFO *)lparam;
    limits->ptMinTrackSize.x = 0x32;
    limits->ptMinTrackSize.y = 0x32;
  }
    return 0;

  case WM_SIZE:
  {
    HDC size_dc;

    if (wparam == SIZE_RESTORED && !shell_size_suspended)
    {
      shell_release_fonts(hwnd, shell_text_font, shell_button_font,
                          shell_explanation_font);
      shell_load_fonts(hwnd, &shell_text_font, (WPARAM *)&shell_button_font,
                       (WPARAM *)&shell_explanation_font);
      size_dc = GetDC(hwnd);
      shell_set_map_mode(size_dc, hwnd, shell_background_bitmap);
      shell_layout_choices();
      SetRect(&shell_dialog_rect, shell_background_rect.left + 0x1e,
              shell_background_rect.top + 0xeb,
              shell_background_rect.left + 0x23a,
              shell_background_rect.top + 0x221);
      LPtoDP(size_dc, (LPPOINT)&shell_choices_rect, 2);
      LPtoDP(size_dc, (LPPOINT)&shell_dialog_rect, 2);
      ReleaseDC(hwnd, size_dc);
      shell_position_choices(hwnd);
      shell_scroll_choices(hwnd, g_startup_dialog_group, shell_scroll_position);
      if (g_startup_modeless_dialog != NULL)
        MoveWindow(g_startup_modeless_dialog, shell_dialog_rect.left,
                   shell_dialog_rect.top,
                   shell_dialog_rect.right - shell_dialog_rect.left,
                   shell_dialog_rect.bottom - shell_dialog_rect.top, TRUE);
    }
    if (wparam == SIZE_MINIMIZED)
      shell_size_suspended = 1;
    else if (wparam == SIZE_RESTORED)
      shell_size_suspended = 0;
  }
    return 0;

  case WM_SYSCOMMAND:
  {
    LRESULT system_result;

    if ((wparam & 0xfff0) == SC_MAXIMIZE)
    {
      MoveWindow(hwnd, 1, 0, GetSystemMetrics(SM_CXSCREEN) - 1,
                 GetSystemMetrics(SM_CYSCREEN), TRUE);
      system_result = 0;
    }
    else
      system_result = DefWindowProcA(hwnd, msg, wparam, lparam);
    return system_result;
  }

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_TIMER:
  {
    struct
    {
      HDC dc;
      RECT rect;
    } timer;

    if (wparam == 1 && g_startup_dialog_group != -1)
    {
      shell_animation_frame = (shell_animation_frame + 1) % 12;
      CopyRect(&timer.rect, &shell_sphere_rects[g_startup_dialog_group]);
      timer.dc = GetDC(hwnd);
      shell_set_map_mode(timer.dc, hwnd, shell_background_bitmap);
      LPtoDP(timer.dc, (LPPOINT)&timer.rect, 2);
      ReleaseDC(hwnd, timer.dc);
      InvalidateRect(hwnd, &timer.rect, TRUE);
    }
  }
    return 0;

  case WM_ACTIVATEAPP:
    if (wparam != 0)
      shell_enable_animation(1);
    else
      shell_enable_animation(0);
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}
