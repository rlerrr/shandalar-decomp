#include <windows.h>
#include <string.h>

#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "defs.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

#ifndef SHANDALAR
#define DUEL_SHELL_WINDOW_HWND g_duel_parent_window_hwnd
#else
extern HWND g_main_window_hwnd;
#define DUEL_SHELL_WINDOW_HWND g_main_window_hwnd
#endif

extern int life[2];
extern HINSTANCE g_app_instance;
void checked_DeleteDC_DeleteObject(HDC dc, HGDIOBJ obj);

#ifdef SHANDALAR
int g_duel_timer_id;
extern int DAT_0074d268;
#define g_duel_exit_code DAT_0074d268
#else
// GLOBAL: MAGIC 0x00716240
int g_duel_exit_code;
#endif

// GLOBAL: MAGIC 0x00579f28
// GLOBAL: SHANDALAR 0x0058f658
char s_Congratulations_00579f28[0x14] = "Congratulations!";

// GLOBAL: MAGIC 0x00579f3c
// GLOBAL: SHANDALAR 0x0058f66c
char s_Too_bad_00579f3c[8] = "Too bad";

// GLOBAL: MAGIC 0x00579f44
// GLOBAL: SHANDALAR 0x0058f674
char s_Oh_well_00579f44[0xc] = "Oh well...";

// GLOBAL: MAGIC 0x00579f50
// GLOBAL: SHANDALAR 0x0058f680
char s_Want_to_play_again_00579f50[0x18] = "\n\nWant to play again?";

// GLOBAL: MAGIC 0x00579f68
// GLOBAL: SHANDALAR 0x0058f698
char s_End_of_duel_00579f68[0xc] = "End of duel";

// GLOBAL: MAGIC 0x0091c9a4
// GLOBAL: SHANDALAR 0x00930ae4
HANDLE g_duel_thread_handle;

// GLOBAL: MAGIC 0x00579f24
// GLOBAL: SHANDALAR 0x0058f654
int g_duel_main_window_minimized;

// GLOBAL: MAGIC 0x008b3bcc
// GLOBAL: SHANDALAR 0x008c7d4c
HWND g_duel_palette_window_hwnd;

// GLOBAL: MAGIC 0x008b3274
// GLOBAL: SHANDALAR 0x008c7424
HWND g_duel_player_battlefield_window_hwnd;

// GLOBAL: MAGIC 0x008cf078
// GLOBAL: SHANDALAR 0x008e31c8
HWND g_duel_opponent_battlefield_window_hwnd;

// GLOBAL: MAGIC 0x008cc700
// GLOBAL: SHANDALAR 0x008e0880
int g_duel_modal_action_active;

// GLOBAL: MAGIC 0x00777aa4
// GLOBAL: SHANDALAR 0x0078e834
int g_duel_selected_player_card;

// GLOBAL: MAGIC 0x008a8d70
// GLOBAL: SHANDALAR 0x008bcf70
int g_duel_selected_opponent_card;

// GLOBAL: MAGIC 0x008b294c
// GLOBAL: SHANDALAR 0x008c6afc
int g_duel_selection_pending;

// GLOBAL: MAGIC 0x00925d34
// GLOBAL: SHANDALAR 0x00939e60
int g_duel_attacker_count;

// GLOBAL: MAGIC 0x008b2940
// GLOBAL: SHANDALAR 0x008c6af0
int g_duel_blocker_count;

// GLOBAL: MAGIC 0x0093a97c
// GLOBAL: SHANDALAR 0x0094ea9c
int g_duel_combat_selection_count;

// GLOBAL: MAGIC 0x007ab14c
// GLOBAL: SHANDALAR 0x007bf34c
int g_duel_pending_prompt_mode;

// GLOBAL: MAGIC 0x008a8c18
// GLOBAL: SHANDALAR 0x008bce18
int g_duel_pending_phase_action;

// GLOBAL: MAGIC 0x0091c4f4
// GLOBAL: SHANDALAR 0x00930634
int g_duel_life_window_state;

// GLOBAL: MAGIC 0x008cefb4
// GLOBAL: SHANDALAR 0x008e3100
int g_duel_player_mana_pool_dirty;

// GLOBAL: MAGIC 0x008ce4f8
// GLOBAL: SHANDALAR 0x008e2678
int g_duel_opponent_mana_pool_dirty;

// GLOBAL: MAGIC 0x0093a7f4
// GLOBAL: SHANDALAR 0x0094e914
int g_duel_spell_chain_state;

// GLOBAL: MAGIC 0x00925f9c
// GLOBAL: SHANDALAR 0x0093a0cc
int g_duel_attack_phase_state;

// GLOBAL: MAGIC 0x008ce500
// GLOBAL: SHANDALAR 0x008e2680
int g_duel_phasebar_state;

// GLOBAL: MAGIC 0x009266a8
// GLOBAL: SHANDALAR 0x0093a7d8
int g_duel_fast_effect_state;

// GLOBAL: MAGIC 0x007ab290
// GLOBAL: SHANDALAR 0x007bf490
int g_duel_pending_player_slots[7];

// GLOBAL: MAGIC 0x00896670
// GLOBAL: SHANDALAR 0x008aa870
int g_duel_pending_opponent_slots[7];

// GLOBAL: MAGIC 0x008b4370
// GLOBAL: SHANDALAR 0x008c84f0
BITMAPINFO g_duel_backbuffer_bmi;

// GLOBAL: MAGIC 0x00925ad0
// GLOBAL: SHANDALAR 0x00939c00
char g_duel_action_request_copy[0xe8];

#ifdef SHANDALAR
// GLOBAL: SHANDALAR 0x0094e8d0
HWND g_duel_prompt_context_hwnd;
#else
extern HWND DAT_0093a7b0;
#define g_duel_prompt_context_hwnd DAT_0093a7b0
#endif

// GLOBAL: MAGIC 0x008961b0
// GLOBAL: SHANDALAR 0x008aa3b0
int g_duel_current_selection_forced;

// GLOBAL: MAGIC 0x0074a230
// GLOBAL: SHANDALAR 0x00746b08
int g_duel_main_window_closing;

// GLOBAL: MAGIC 0x00579f20
// GLOBAL: SHANDALAR 0x0058f650
int g_duel_main_window_deferred_resize;

// FUNCTION: MAGIC 0x00447627
// FUNCTION: SHANDALAR 0x004511a5
void notify_duel_action(int player, unsigned int value)
{
}

// FUNCTION: MAGIC 0x004dbea9
// FUNCTION: SHANDALAR 0x00542b64
int create_duel_child_windows(HWND parent_window)
{
  return 1;
}

// FUNCTION: MAGIC 0x004dc512
// FUNCTION: SHANDALAR 0x005431cd
void layout_duel_child_windows(HWND hwnd, int layout)
{
}

// FUNCTION: MAGIC 0x004e8f49
// FUNCTION: SHANDALAR 0x00506d61
void refresh_duel_window(HWND hwnd)
{
}

// FUNCTION: MAGIC 0x004ddbf4
// FUNCTION: SHANDALAR 0x005448a5
void run_duel_timer_tick(void)
{
}

// FUNCTION: MAGIC 0x0049d77b
// FUNCTION: SHANDALAR 0x00539128
void show_post_duel_draws(int duel_result)
{
}

// FUNCTION: MAGIC 0x0046452a
// FUNCTION: SHANDALAR 0x00426c59
void set_player_directive_value(int player, int value)
{
}

// FUNCTION: MAGIC 0x0044a1ca
// FUNCTION: SHANDALAR 0x00453d4d
void get_current_duel_selection(int *selected_player, int *selected_card)
{
}

// FUNCTION: MAGIC 0x0044a2bd
// FUNCTION: SHANDALAR 0x00453e40
int can_use_current_duel_selection(void)
{
  return 0;
}

int dispatch_drawcardlib_wndproc_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return FUN_10025b5e((int)hwnd, msg, (int)wparam, (int)lparam);
}

int dispatch_duel_engine_message(MSG *message);

// FUNCTION: MAGIC 0x004da411
// FUNCTION: SHANDALAR 0x005410d1
LRESULT CALLBACK wndproc_MAGICGAME_MainClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    char help_path[264];
    int slot_index;
    BOOL visible;
    int screen_height;
    int screen_width;
    HWND duel_result;
    int should_quit;
    int has_replay_prompt;
    char replay_prompt[100];
    HWND action_request_hwnd;
    int *action_request;
    int *action_result;
    POINT cursor_pos;
    POINT cursor_point;
    HWND cursor_window;
    unsigned int hit_test;
    int selected_player;
    int selected_card;
    MSG modal_msg;
    DWORD thread_exit_code;
    BOOL have_message;
    int done;
    unsigned int action_result_ok;
    int *posted_action_result;
  } s;

  switch (msg)
  {
  case WM_CREATE:
    g_duel_thread_handle = (HANDLE)0;
    g_duel_selected_player_card = -1;
    g_duel_selected_opponent_card = -1;
    g_duel_selection_pending = 0;
    g_duel_attacker_count = 0;
    g_duel_blocker_count = 0;
    g_duel_combat_selection_count = 0;
    g_duel_pending_prompt_mode = 0;
    g_duel_pending_phase_action = 0;
    g_duel_life_window_state = 0;
    for (s.slot_index = 0; s.slot_index < 7; s.slot_index = s.slot_index + 1)
    {
      g_duel_pending_player_slots[s.slot_index] = 0;
      g_duel_pending_opponent_slots[s.slot_index] = g_duel_pending_player_slots[s.slot_index];
    }
    g_duel_player_mana_pool_dirty = 1;
    g_duel_opponent_mana_pool_dirty = 1;
    g_duel_spell_chain_state = 0;
    g_duel_attack_phase_state = 0;
    g_duel_phasebar_state = 0;
    g_duel_fast_effect_state = 0;
    g_multiblock_creature_count = 0;
    g_duel_modal_action_active = 0;
    if (create_duel_child_windows(hwnd) == 0)
    {
      return -1;
    }
    g_duel_palette_window_hwnd = CreateWindowExA(0, "MAGIC_PaletteClass", "Palette", 0x80cc0000,
                                                0x14, 0x14, 300, 0x15e, hwnd, (HMENU)0,
                                                g_app_instance, (LPVOID)0);
    g_duel_timer_id = 0x14;
    g_your_attack_window_hwnd = (HWND)0;
    return 0;

  case WM_DESTROY:
    strcpy(s.help_path, global_base_directory);
    strcat(s.help_path, "\\duel.hlp");
    WinHelpA(g_duel_window_hwnd, s.help_path, HELP_QUIT, 0);
    KillTimer(hwnd, g_duel_timer_id);
    return 0;

  case WM_SIZE:
    if ((wparam == SIZE_RESTORED) && (g_duel_main_window_minimized == 0))
    {
      LockWindowUpdate(hwnd);
      layout_duel_child_windows(hwnd, g_duel_interface_options.layout);
      refresh_duel_window(g_duel_player_battlefield_window_hwnd);
      refresh_duel_window(g_duel_opponent_battlefield_window_hwnd);
      LockWindowUpdate((HWND)0);
    }

    if (wparam == SIZE_MINIMIZED)
    {
      g_duel_main_window_minimized = 1;
    }
    else if (wparam == SIZE_RESTORED)
    {
      g_duel_main_window_minimized = 0;
    }

    if ((wparam == SIZE_MINIMIZED) && (DUEL_SHELL_WINDOW_HWND != (HWND)0))
    {
      ShowWindow(DUEL_SHELL_WINDOW_HWND, SW_SHOW);
    }
    return 0;

  case WM_DISPLAYCHANGE:
    g_display_color_depth = (int)wparam;
    EnterCriticalSection(&g_duel_render_lock);
    checked_DeleteDC_DeleteObject(g_shared_offscreen_dc, g_shared_offscreen_bitmap);
    s.screen_height = GetSystemMetrics(SM_CYSCREEN);
    s.screen_width = GetSystemMetrics(SM_CXSCREEN);
    if (CreateOffscreen32bppDibSection(s.screen_width, s.screen_height,
                                       &g_shared_offscreen_dc, &g_duel_backbuffer_bmi,
                                       &g_shared_offscreen_bitmap, &g_shared_offscreen_prev_object,
                                       &g_shared_offscreen_bits) == 0)
    {
      MessageBoxA(hwnd, DAT_007898b0, gs_magic_the_gathering_title_00789460, MB_ICONEXCLAMATION);
      ShowWindow(hwnd, SW_HIDE);
    }
    else
    {
      ShowWindow(hwnd, SW_SHOW);
    }
    LeaveCriticalSection(&g_duel_render_lock);
    if (IsIconic(hwnd) == 0)
    {
      MoveWindow(hwnd, 1, 0, LOWORD(lparam) - 1, HIWORD(lparam), 1);
    }
    else
    {
      g_duel_main_window_deferred_resize = 1;
    }
    return 0;

  case WM_TIMER:
    if (((int)wparam == g_duel_timer_id) && (g_duel_modal_action_active == 0) && (g_your_attack_window_hwnd == (HWND)0))
    {
      run_duel_timer_tick();
    }
    return 0;

  case 0x30f:
  case 0x310:
  case 0x311:
    return dispatch_drawcardlib_wndproc_message(hwnd, msg, wparam, lparam);

  case 0x401:
    s.duel_result = (HWND)wparam;
    s.should_quit = 1;
    if (g_duel_thread_handle != (HANDLE)0)
    {
      WaitForSingleObject(g_duel_thread_handle, INFINITE);
      CloseHandle(g_duel_thread_handle);
      g_duel_thread_handle = (HANDLE)0;
    }
    if ((g_duel_mode_flags & 1) == 0)
    {
      if ((g_duel_mode_flags & 2) != 0)
      {
        s.has_replay_prompt = 1;
        if (s.duel_result == (HWND)1)
        {
          strcpy(s.replay_prompt, s_Congratulations_00579f28);
        }
        else if (s.duel_result == (HWND)0)
        {
          strcpy(s.replay_prompt, s_Too_bad_00579f3c);
        }
        else if (s.duel_result == (HWND)-1)
        {
          strcpy(s.replay_prompt, s_Oh_well_00579f44);
        }
        else
        {
          s.has_replay_prompt = 0;
        }
        if (s.has_replay_prompt != 0)
        {
          strcat(s.replay_prompt, s_Want_to_play_again_00579f50);
          if (MessageBoxA(hwnd, s.replay_prompt, s_End_of_duel_00579f68, MB_YESNO) == IDYES)
          {
            s.should_quit = 0;
            SendMessageA(hwnd, 0x400, 0, 0);
          }
        }
      }
    }
    else if (g_duel_interface_options.see_next_draws_at_end_of_duel != 0)
    {
      show_post_duel_draws((int)s.duel_result);
    }
    if (s.should_quit != 0)
    {
      g_duel_exit_code = (int)s.duel_result;
      PostQuitMessage((int)s.duel_result);
    }
    return 0;

  case 0x403:
    if (g_your_attack_window_hwnd != (HWND)0)
    {
      SendMessageA(g_your_attack_window_hwnd, WM_CLOSE, 0, 0);
    }
    KillTimer(hwnd, g_duel_timer_id);
    s.action_request_hwnd = (HWND)wparam;
    s.action_request = (int *)wparam;
    s.action_result = (int *)lparam;
    g_duel_modal_action_active = 1;
    memcpy(g_duel_action_request_copy, (void *)wparam, 0xe8);
    GetCursorPos(&s.cursor_pos);
    s.cursor_point.y = s.cursor_pos.y;
    s.cursor_point.x = s.cursor_pos.x;
    s.cursor_window = WindowFromPoint(s.cursor_point);
    s.hit_test = SendMessageA(s.cursor_window, WM_NCHITTEST, 0,
                              (s.cursor_pos.y << 0x10) | (s.cursor_pos.x & 0xffffU));
    SendMessageA(s.cursor_window, WM_SETCURSOR, (WPARAM)s.cursor_window, (s.hit_test & 0xffff) | 0x2000000);
    set_player_directive_value(1, s.action_request[0x38]);
    set_player_directive_value(0, s.action_request[0x39]);
    set_duel_prompt_context(g_duel_prompt_context_hwnd, (char *)(s.action_request + 6), s.action_request[5]);
    get_current_duel_selection(&s.selected_player, &s.selected_card);
    if (((s.selected_card == 0x15) && (s.selected_player == 1)) && (can_use_current_duel_selection() != 0))
    {
      g_duel_current_selection_forced = 1;
    }
    s.done = 0;
    while (s.done == 0)
    {
      GetExitCodeThread(g_duel_thread_handle, &s.thread_exit_code);
      if (s.thread_exit_code != STILL_ACTIVE)
      {
        s.done = 1;
      }
      s.have_message = PeekMessageA(&s.modal_msg, (HWND)0, 0, 0, PM_REMOVE);
      if (g_duel_main_window_closing != 0)
      {
        if ((s.have_message != 0) && (s.modal_msg.message == 0x464))
        {
          s.have_message = 0;
        }
        s.done = 1;
        s.action_result[0] = -5;
        s.action_result[1] = -1;
        s.action_result[2] = -1;
        if (s.action_result[0] == 0)
        {
          s.action_result_ok = 1;
        }
        else
        {
          s.action_result_ok = 0;
        }
      }
      if (s.have_message != 0)
      {
        if (s.modal_msg.message == 0x464)
        {
          s.posted_action_result = (int *)s.modal_msg.lParam;
          s.done = 1;
          s.action_result_ok = (unsigned int)(s.posted_action_result[0] == 0);
          memcpy(s.action_result, s.posted_action_result, 0x10);
        }
        else if (s.modal_msg.message == WM_QUIT)
        {
          PostQuitMessage(s.modal_msg.wParam);
          s.done = 1;
          s.action_result_ok = 0;
          s.action_result[0] = -2;
        }
        else
        {
          dispatch_duel_engine_message(&s.modal_msg);
        }
      }
    }
    set_duel_prompt_context(g_duel_prompt_context_hwnd, (char *)0, 0);
    set_player_directive_value(1, 0);
    set_player_directive_value(0, 0);
    g_duel_modal_action_active = 0;
    SetTimer(hwnd, g_duel_timer_id, 45000, (TIMERPROC)0);
    return s.action_result_ok;

  case 0x433:
  case 0x434:
    return 0;

  case 0x464:
    notify_duel_action(0, (unsigned int)wparam);
    return 0;

  case WM_CLOSE:
    g_duel_main_window_closing = 1;
    life[0] = 0;
    return 0;

  case WM_QUERYOPEN:
    if (g_duel_main_window_deferred_resize != 0)
    {
      PostMessageA(hwnd, 0x501, 0, 0);
    }

    if (DUEL_SHELL_WINDOW_HWND != (HWND)0)
    {
      ShowWindow(DUEL_SHELL_WINDOW_HWND, SW_SHOW);
    }

    return 1;

  case WM_ERASEBKGND:
    return 1;

  case WM_GETMINMAXINFO:
    ((int *)lparam)[6] = 0x32;
    ((int *)lparam)[7] = 0x32;
    return 0;

  case 0x501:
    g_duel_main_window_deferred_resize = 0;
    MoveWindow(hwnd, 1, 0, GetSystemMetrics(SM_CXSCREEN) - 1, GetSystemMetrics(SM_CYSCREEN), 1);
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}
