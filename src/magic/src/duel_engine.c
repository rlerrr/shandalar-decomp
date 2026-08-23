#define DUEL_ENGINE_IMPL

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "drawcardlib/Drawcardlib.h"
#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/global_other.h"
#include "magic/src/shared_startup.h"
#include "duel_engine.h"

#ifndef SHANDALAR
extern HWND global_main_hwnd;
#include "manalinkinterface/manalinkinterface.h"
#else
#include "shandalar/src/shandalar_global_strings.h"
// GLOBAL: SHANDALAR 0x008a92b0
HWND global_main_hwnd;
extern HINSTANCE g_app_instance;
extern int global_screen_width;
extern int global_screen_height;
#endif

extern int g_duel_modal_action_active;
extern int DAT_007a7d74;
extern int DAT_008a8dec;
extern int DAT_008a8d78;
extern HMENU g_library_popup_menu;
extern HMENU g_library_submenu;
extern char g_library_menu_count_text[];
extern char g_library_menu_help_text[];
extern HMENU g_graveyard_popup_menu;
extern char g_graveyard_menu_view_text[];
extern char g_graveyard_menu_view_exile_text[];
extern char g_graveyard_menu_view_antes_text[];
extern char g_graveyard_menu_help_text[];
extern char g_graveyard_view_antes_opponent_text[];
extern char g_graveyard_view_antes_player_text[];
extern char s_MENU_GRAVEYARD_0056e9e0[];
extern char s_DIALOG_VIEWANTES_0056e9f0[];
extern HWND g_duel_toggleable_status_window_hwnd;
extern HPALETTE global_cart_art_hpalette;
extern HBITMAP g_spell_minimized_background_bitmap;
extern HMENU g_spell_minimized_popup_menu;
extern char g_spell_minimized_menu_help_text[];
extern char g_spell_minimized_menu_restore_text[];
extern int g_graveyard_window_extra_bytes;
extern int g_expanded_graveyard_window_extra_bytes;
extern int g_graveyard_cards_window_extra_bytes;
int load_text_with_tab_escapes(char *filename, char *section_name);
HBITMAP load_pic(char *filename);
LOGFONTA *LoadFontFromIni(char *section, int must_load);

#define CHAT_WIDTH_WINDOW_LONG_OFFSET g_magicgame_chat_width_window_long_offset
#define CHAT_FONT_WINDOW_LONG_OFFSET g_magicgame_chat_font_window_long_offset

// GLOBAL: MAGIC 0x0055fcb4
// GLOBAL: SHANDALAR 0x00582d94
int g_duel_directive_hover_timer_id = 0;

// GLOBAL: MAGIC 0x00573068
// GLOBAL: SHANDALAR 0x00589d60
HWND g_duel_last_cue_card_hwnd = (HWND)0;

// GLOBAL: MAGIC 0x0057306c
int DAT_0057306c = 0;

// GLOBAL: MAGIC 0x00573070
// GLOBAL: SHANDALAR 0x00589d68
int g_duel_last_cue_card_x = 0;

// GLOBAL: MAGIC 0x00573074
int g_duel_last_cue_card_y = 0;

// GLOBAL: MAGIC 0x0055e0d0
// GLOBAL: SHANDALAR 0x0057f0dc
int g_duel_cue_card_font_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e0d4
// GLOBAL: SHANDALAR 0x0057f0e0
int g_duel_cue_card_window_extra_bytes = 4;

// GLOBAL: MAGIC 0x0064f0ac
// GLOBAL: SHANDALAR 0x005b7c68
HFONT g_duel_cue_card_font;

// GLOBAL: MAGIC 0x0064f0b0
// GLOBAL: SHANDALAR 0x005b7c6c
COLORREF g_duel_cue_card_text_color;

// GLOBAL: MAGIC 0x0064f0b4
// GLOBAL: SHANDALAR 0x005b7c70
HBRUSH g_duel_cue_card_shadow_brush;

// GLOBAL: MAGIC 0x0064f0b8
// GLOBAL: SHANDALAR 0x005b7c74
HBRUSH g_duel_cue_card_background_brush;

// GLOBAL: MAGIC 0x007ab2c4
// GLOBAL: SHANDALAR 0x007bf4c0
int g_duel_cue_card_mouse_threshold;

// GLOBAL: MAGIC 0x008a8df4
// GLOBAL: SHANDALAR 0x008bcff4
int g_duel_cue_card_timer_elapsed;

// GLOBAL: MAGIC 0x008b44d8
// GLOBAL: SHANDALAR 0x008c8658
int g_duel_cue_card_y_offset;

// GLOBAL: MAGIC 0x008ce7d4
// GLOBAL: SHANDALAR 0x008e2924
int g_duel_cue_card_timer_id;

// GLOBAL: MAGIC 0x0093a7f8
// GLOBAL: SHANDALAR 0x0094e918
int g_duel_cue_card_x_offset;

// GLOBAL: MAGIC 0x0055e15c
// GLOBAL: SHANDALAR 0x0057f178
int g_icon_button_icon_long_offset = 0;

// GLOBAL: MAGIC 0x0055e160
// GLOBAL: SHANDALAR 0x0057f17c
int g_icon_button_pressed_long_offset = 4;

// GLOBAL: MAGIC 0x005707a8
// GLOBAL: SHANDALAR 0x005854b0
int g_palette_selected_color_window_long_offset = 0;

// GLOBAL: MAGIC 0x005707b0
// GLOBAL: SHANDALAR 0x005854b8
int g_palette_grid_y_offset = 0;

// GLOBAL: MAGIC 0x005707ac
// GLOBAL: SHANDALAR 0x005854b4
int g_palette_grid_x_offset = 0;

// GLOBAL: MAGIC 0x00560188
// GLOBAL: SHANDALAR 0x00583268
char g_kim_debug_class_name_00560188[12] = "KimDebug";

// GLOBAL: MAGIC 0x00560194
// GLOBAL: SHANDALAR 0x00583274
char g_kim_debug_window_title_00560194[4] = "Kim";

// GLOBAL: MAGIC 0x00560198
// GLOBAL: SHANDALAR 0x00583278
char g_kim_debug_class_name_duplicate_00560198[12] = "KimDebug";

// GLOBAL: MAGIC 0x005601a4
// GLOBAL: SHANDALAR 0x00583284
char g_kim_debug_empty_text_005601a4[4] = "";


// GLOBAL: MAGIC 0x00561270
// GLOBAL: SHANDALAR 0x00580680
const char *g_duel_sound_filenames[] = {
    "artifact.wav", "buried.wav", "draw.wav", "enchant.wav", "endphase.wav",
    "endturn.wav", "instant.wav", "interupt.wav", "GREY.wav", "BLACK.wav",
    "BLUE.wav", "GREEN.wav", "RED.wav", "WHITE.wav", "lifeloss.wav",
    "sacrfice.wav", "sorcery.wav", "summon.wav", "tap.wav", "untap.wav",
    "attack2.wav", "block2.wav", "damage.wav", "destroy.wav", "discard.wav",
    "kill.wav", "regen.wav", "BLACKRED.wav", "GREENBLACK.wav", "WHITERED.wav",
    "WHITEGREEN.wav", "BLACKWHITE.wav", "GREENRED.wav", "GREENBLUE.wav", "WHITEBLUE.wav",
    "BLUEBLACK.wav", "REDBLUE.wav", "counter.wav", "fastfx.wav", (char *)0, "changec.wav",
    "changet.wav", "control.wav", "manaburn.wav", "shuffle.wav", "shell_loseduel.wav",
    "shell_winduel.wav", (char *)0, "aswanjag.wav", "callgrav.wav", "faerdrag.wav", "gembazar.wav",
    "necrazar.wav", "polkamix.wav", "pandora.wav", "prsmdrag.wav", "pwrstrgl.wav",
    "catatap.wav", "orcart.wav", "whimsy.wav", "rainbowk.wav", "toss.wav",
    "shell_shandalar.wav", "shell_tooltime.wav", "shell_helpme.wav", "shell_hallofrecords.wav",
    "shell_duelmenow.wav", (char *)0, "exp1_openfoil.wav", "exp1_openbox.wav", "exp1_outofpack.wav",
    "exp1_backinpack.wav"};

// GLOBAL: MAGIC 0x00637554
// GLOBAL: SHANDALAR 0x005a8b2c
HWND g_kim_debug_listbox_hwnd;

#define KIM_DEBUG_APPEND_MESSAGE 0x4c8

int register_window_classes(void);
int destroy_windowclasses(void);
int register_MAGICGAME_LifeClass(LPCSTR class_name);
void destroy_MAGICGAME_LifeClass(LPCSTR class_name);
int register_MAGICGAME_ManaSummaryClass(LPCSTR class_name);
void destroy_MAGICGAME_ManaSummaryClass(LPCSTR class_name);
int register_MAGICGAME_FullCardClass(LPCSTR class_name);
void destroy_MAGICGAME_FullCardClass(LPCSTR class_name);
int register_MAGICGAME_HandClass(LPCSTR class_name);
void destroy_MAGICGAME_HandClass(LPCSTR class_name);
int register_MAGICGAME_CardClass(LPCSTR class_name);
void destroy_MAGICGAME_CardClass(LPCSTR class_name);
int register_MAGICGAME_PhaseDisplayClass(LPCSTR class_name);
void destroy_MAGICGAME_PhaseDisplayClass(LPCSTR class_name);
int register_MAGICGAME_LibraryClass(LPCSTR class_name);
int register_MAGICGAME_GraveyardClass(LPCSTR class_name);
int register_MAGICGAME_FaceClass(LPCSTR class_name);
void destroy_MAGICGAME_FaceClass(LPCSTR class_name);
int register_MAGICGAME_TerritoryClass(LPCSTR class_name);
void destroy_MAGICGAME_TerritoryClass(LPCSTR class_name);
int register_MAGICGAME_AttackClass(LPCSTR class_name);
void destroy_MAGICGAME_AttackClass(LPCSTR class_name);
int register_MAGICGAME_SpellChainClass(LPCSTR class_name);
void destroy_MAGICGAME_SpellChainClass(LPCSTR class_name);
int register_MAGICGAME_ScrollbarClass(LPCSTR class_name);
int register_MAGICGAME_BigCardChoiceClass(LPCSTR class_name);
void destroy_MAGICGAME_ScrollbarClass(LPCSTR class_name);
int register_MAGIC_TellUserClass(LPCSTR class_name);
void destroy_MAGIC_TellUserClass(LPCSTR class_name);

// FUNCTION: MAGIC 0x0044385c
// FUNCTION: SHANDALAR 0x0040f0f6
void prepare_duel_video_mode_transition(void)
{
  sound_unload_all();
}

// FUNCTION: MAGIC 0x004437cd
// FUNCTION: SHANDALAR 0x0040f067
void finish_duel_video_mode_transition(void)
{
  struct
  {
    char path[264];
    int sound_id;
    int unused[7];
    unsigned int flags;
  } s;

  s.flags &= 0xfffffffb;
  sound_unload_all();
  for (s.sound_id = 0; s.sound_id < 20; s.sound_id++)
  {
    strcpy(s.path, global_duelsounds_path);
    strcat(s.path, "\\");
    strcat(s.path, g_duel_sound_filenames[s.sound_id]);
    sound_load(s.path, s.sound_id, (Sound *)0);
  }
}

// FUNCTION: MAGIC 0x004224b5
// FUNCTION: SHANDALAR 0x00430bfb
LRESULT CALLBACK wndproc_KimDebug(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  LRESULT result;

  if (msg == KIM_DEBUG_APPEND_MESSAGE)
  {
    if (SendMessageA(g_kim_debug_listbox_hwnd, LB_INSERTSTRING, 0, lparam) == LB_ERRSPACE)
    {
      result = SendMessageA(g_kim_debug_listbox_hwnd, LB_GETCOUNT, 0, 0);
      SendMessageA(g_kim_debug_listbox_hwnd, LB_DELETESTRING, result - 1, 0);
      SendMessageA(g_kim_debug_listbox_hwnd, LB_DELETESTRING, result - 2, 0);
      SendMessageA(g_kim_debug_listbox_hwnd, LB_INSERTSTRING, 0, lparam);
    }
    return 0;
  }

  switch (msg)
  {
  case WM_SIZE:
    MoveWindow(g_kim_debug_listbox_hwnd, 0, 0, lparam & 0xffff, HIWORD(lparam), TRUE);
    return 0;

  case WM_MOVE:
    SendMessageA(g_kim_debug_listbox_hwnd, LB_RESETCONTENT, 0, 0);
    return 0;

  case WM_CREATE:
    g_kim_debug_listbox_hwnd = CreateWindowExA(0,
                                               "LISTBOX",
                                               g_kim_debug_empty_text_005601a4,
                                               0x50240000,
                                               0,
                                               0,
                                               0,
                                               0,
                                               hwnd,
                                               (HMENU)0,
                                               g_app_instance,
                                               (LPVOID)0);
    return 0;

  case WM_DESTROY:
    KillTimer(hwnd, 1);
    return 0;

  case WM_TIMER:
    InvalidateRect(hwnd, (RECT *)0, TRUE);
    return 0;

  case WM_LBUTTONDOWN:
    InvalidateRect(hwnd, (RECT *)0, TRUE);
    return 0;

  case WM_CLOSE:
    ShowWindow(hwnd, SW_HIDE);
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: MAGIC 0x00422405
// FUNCTION: SHANDALAR 0x00430b4b
int create_kim_debug_window(void)
{
  WNDCLASSA wndclass;

  wndclass.style = 0;
  wndclass.lpfnWndProc = wndproc_KimDebug;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = g_app_instance;
  wndclass.hIcon = (HICON)0;
  wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  wndclass.hbrBackground = GetStockObject(1);
  wndclass.lpszMenuName = (LPCSTR)0;
  wndclass.lpszClassName = g_kim_debug_class_name_00560188;
  RegisterClassA(&wndclass);

  g_duel_toggleable_status_window_hwnd = CreateWindowExA(0,
                                                         g_kim_debug_class_name_duplicate_00560198,
                                                         g_kim_debug_window_title_00560194,
                                                         0x80cc0000,
                                                         10,
                                                         10,
                                                         0xfa,
                                                         0x113,
                                                         g_duel_window_hwnd,
                                                         (HMENU)0,
                                                         g_app_instance,
                                                         (LPVOID)0);
  return 1;
}

// FUNCTION: MAGIC 0x00421f0a
// FUNCTION: SHANDALAR 0x00430650
int handle_duel_hover_help_message(MSG *message, UINT timer_elapsed)
{
  struct
  {
    int overrun;
    POINT point;
    RECT rect;
  } s;

  switch (message->message)
  {
  case WM_NCMOUSEMOVE:
  case WM_MOUSEMOVE:
    if (g_duel_directive_hover_timer_id != 0)
    {
      KillTimer((HWND)0, g_duel_directive_hover_timer_id);
      g_duel_directive_hover_timer_id = 0;
    }
    if (g_duel_interface_options.directive_tracks_mouse != 0 &&
        IsWindowVisible((HWND)DAT_007a7d74) != 0)
    {
      g_duel_directive_hover_timer_id = SetTimer((HWND)0, 0, timer_elapsed, (TIMERPROC)0);
    }
    return 0;

  case WM_TIMER:
    if (message->hwnd == (HWND)0 && message->wParam == (WPARAM)g_duel_directive_hover_timer_id)
    {
      KillTimer((HWND)0, g_duel_directive_hover_timer_id);
      g_duel_directive_hover_timer_id = 0;
      GetCursorPos(&s.point);
      s.point.x += GetSystemMetrics(0xd);
      s.point.y += GetSystemMetrics(0xe);
      GetWindowRect((HWND)DAT_007a7d74, &s.rect);
      s.overrun = (s.rect.right - s.rect.left) + s.point.x - GetSystemMetrics(SM_CXSCREEN);
      if (s.overrun > 0)
      {
        s.point.x -= s.overrun;
      }
      s.overrun = (s.rect.bottom - s.rect.top) + s.point.y - GetSystemMetrics(SM_CYSCREEN);
      if (s.overrun > 0)
      {
        s.point.y -= s.overrun;
      }
      SetWindowPos((HWND)DAT_007a7d74, (HWND)0, s.point.x, s.point.y, 0, 0,
                   SWP_NOSIZE | SWP_NOZORDER);
      return 1;
    }
    else
    {
      return 0;
    }

  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x00422350
// FUNCTION: SHANDALAR 0x00430a96
int dispatch_duel_window_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  (void)hwnd;
  (void)wparam;
  (void)lparam;
  switch (msg)
  {
  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x004b1e33
// FUNCTION: SHANDALAR 0x004cc6c0
int handle_duel_tooltip_message(MSG *message)
{
  struct
  {
    int timer_cue_card_visible;
    POINT timer_client_point;
    POINT timer_screen_point;
    HWND target_hwnd;
    int cue_card_visible;
    char current_text[100];
    POINT screen_point;
    int refresh_cue_card;
    char cue_text[100];
  } s;

  switch (message->message)
  {
  case WM_MOUSEMOVE:
    if (IsWindowVisible(g_duel_cue_card_window_hwnd) == 0)
    {
      if (g_duel_cue_card_timer_id != 0)
      {
        KillTimer((HWND)0, g_duel_cue_card_timer_id);
        g_duel_cue_card_timer_id = 0;
      }
      g_duel_cue_card_timer_id = SetTimer((HWND)0, 0, g_duel_cue_card_timer_elapsed, (TIMERPROC)0);
    }
    else
    {
      if (message->hwnd != g_duel_last_cue_card_hwnd)
      {
        if (g_duel_cue_card_timer_id != 0)
        {
          KillTimer((HWND)0, g_duel_cue_card_timer_id);
          g_duel_cue_card_timer_id = 0;
        }
        g_duel_cue_card_timer_id = SetTimer((HWND)0, 0, g_duel_cue_card_timer_elapsed, (TIMERPROC)0);
        g_duel_last_cue_card_hwnd = message->hwnd;
        ShowWindow(g_duel_cue_card_window_hwnd, SW_HIDE);
      }
      else
      {
        s.refresh_cue_card = 1;
        s.cue_card_visible = 1;
        if (message->hwnd != g_duel_last_cue_card_hwnd)
        {
          s.cue_card_visible = SendMessageA(message->hwnd, 0x437, (WPARAM)s.cue_text, message->lParam);
        }
        else if (abs((unsigned short)((message->lParam >> 16) & 0xffff) - g_duel_last_cue_card_y) +
                     abs((unsigned short)message->lParam - g_duel_last_cue_card_x) >
                 g_duel_cue_card_mouse_threshold)
        {
          s.cue_card_visible = SendMessageA(message->hwnd, 0x437, (WPARAM)s.cue_text, message->lParam);
        }
        else
        {
          s.refresh_cue_card = 0;
        }

        if (s.cue_card_visible == 0)
        {
          ShowWindow(g_duel_cue_card_window_hwnd, SW_HIDE);
        }
        else if (s.refresh_cue_card != 0)
        {
          g_duel_last_cue_card_hwnd = message->hwnd;
          g_duel_last_cue_card_x = (unsigned short)message->lParam;
          g_duel_last_cue_card_y = (unsigned short)((message->lParam >> 16) & 0xffff);
          GetCursorPos(&s.screen_point);
          if (g_duel_interface_options.show_cue_cards != 0)
          {
            SendMessageA(g_duel_cue_card_window_hwnd, 0x401, 0, (LPARAM)s.current_text);
            if (strcmp(s.current_text, s.cue_text) != 0)
            {
              SendMessageA(g_duel_cue_card_window_hwnd, 0x400,
                           MAKELONG(s.screen_point.x + g_duel_cue_card_x_offset,
                                    s.screen_point.y + g_duel_cue_card_y_offset),
                           (LPARAM)s.cue_text);
            }
          }
          else
          {
            ShowWindow(g_duel_cue_card_window_hwnd, SW_HIDE);
          }
        }
      }
    }
    return 0;

  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
  case WM_MBUTTONDOWN:
    ShowWindow(g_duel_cue_card_window_hwnd, SW_HIDE);
    if (g_duel_cue_card_timer_id != 0)
    {
      KillTimer((HWND)0, g_duel_cue_card_timer_id);
      g_duel_cue_card_timer_id = 0;
    }
    return 0;

  case WM_TIMER:
    if (message->hwnd == (HWND)0 && message->wParam == (WPARAM)g_duel_cue_card_timer_id)
    {
      KillTimer((HWND)0, g_duel_cue_card_timer_id);
      g_duel_cue_card_timer_id = 0;
      GetCursorPos(&s.timer_screen_point);
      s.target_hwnd = WindowFromPoint(s.timer_screen_point);
      s.timer_client_point.x = s.timer_screen_point.x;
      s.timer_client_point.y = s.timer_screen_point.y;
      ScreenToClient(s.target_hwnd, &s.timer_client_point);
      s.timer_cue_card_visible = SendMessageA(s.target_hwnd, 0x437, (WPARAM)s.cue_text,
                                              MAKELONG(s.timer_client_point.x, s.timer_client_point.y));
      if (s.timer_cue_card_visible != 0 &&
          g_duel_interface_options.show_cue_cards != 0)
      {
        SendMessageA(g_duel_cue_card_window_hwnd, 0x400,
                     MAKELONG(s.timer_screen_point.x + g_duel_cue_card_x_offset,
                              s.timer_screen_point.y + g_duel_cue_card_y_offset),
                     (LPARAM)s.cue_text);
      }

      return 1;
    }
    else
    {
      return 0;
    }
  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x00421ca2
// FUNCTION: SHANDALAR 0x00430442
void dispatch_duel_engine_message(MSG *message)
{
  WPARAM prompt_result;
  int prompt_flags;

#ifndef SHANDALAR
  if ((g_duel_network_flags & 2) != 0 &&
      ((message->message == WM_KEYDOWN && (message->wParam == 'u' || message->wParam == VK_INSERT)) ||
       (message->message == WM_CHAR && message->wParam == '\'')))
  {
    FamInterface_Taunt();
  }
#endif

  if (TranslateAcceleratorA(g_duel_window_hwnd, g_duel_accelerators, message) == 0)
  {
    if (handle_duel_hover_help_message(message, 0x4b) != 0)
    {
    }
    else
    {
      if (g_duel_modal_action_active != 0 &&
          IsWindowVisible(g_duel_prompt_context_hwnd) != 0 &&
          SendMessageA(g_duel_prompt_context_hwnd, 0x402, 0, 0) != 0 &&
          message->message == WM_CHAR &&
          (message->wParam == VK_SPACE || message->wParam == VK_RETURN || message->wParam == VK_ESCAPE))
      {
        prompt_flags = SendMessageA(g_duel_prompt_context_hwnd, 0x402, 0, 0);
        prompt_result = (WPARAM)-1000;
        if ((prompt_flags & 2) != 0 && (prompt_flags & 1) != 0)
        {
          if (message->wParam == VK_RETURN)
          {
            prompt_result = g_tell_user_default_button_state;
          }
          else if (message->wParam == VK_ESCAPE)
          {
            prompt_result = g_tell_user_button_state;
          }
        }
        else
        {
          if (message->wParam == VK_RETURN || message->wParam == VK_SPACE)
          {
            prompt_result = 0;
          }
          else if (message->wParam == VK_ESCAPE && (prompt_flags & 1) != 0)
          {
            prompt_result = g_tell_user_button_state;
          }
        }

        if (prompt_result != (WPARAM)-1000)
        {
          SendMessageA(g_duel_prompt_context_hwnd, 0x401, prompt_result, 0);
        }
      }
      else if (handle_duel_tooltip_message(message) == 0)
      {
        dispatch_duel_window_message(message->hwnd, message->message, message->wParam, message->lParam);
        if ((message->message == WM_LBUTTONDOWN || message->message == WM_RBUTTONDOWN) &&
            g_duel_modal_action_active == 0)
        {
        }
        else
        {
          TranslateMessage(message);
          DispatchMessageA(message);
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x0050d20f
// FUNCTION: SHANDALAR 0x005472f1
int initialize_duel_engine_window(void)
{
  struct
  {
    DWORD style;
    int created_window;
    int height;
    int width;
    char startup_error[1000];
  } s;

  s.created_window = 1;
  SetThreadPriority(GetCurrentThread(), 1);
  InitializeCriticalSection(&g_duel_render_lock);
#ifdef _DEBUG
  // This is annoying to toggle since F12 is the debugger pause hotkey in VS, so start enabled
  g_duel_cheats_state = 1;
#else
  g_duel_cheats_state = 0;
#endif
  g_duel_surface_reset_state = 0;
  g_shared_startup_completed = 1;
  g_duel_dialog_refresh_state = 0;
  global_DIB_debug = 0;
  g_duel_palette_refresh_pending = 0;
  g_duel_tick_adjustment = 0;
  LoadDuelInterfaceRegistryOptions();
  register_window_classes();

  s.style = 0x82000000;
  if ((g_duel_mode_flags & 1) == 0)
  {
    s.style |= 0x40000;
  }

  if ((g_duel_mode_flags & 1) != 0)
  {
    s.width = global_screen_width;
    s.height = global_screen_height;
  }
  else
  {
    s.width = GetSystemMetrics(0) - 1;
    s.height = GetSystemMetrics(1);
  }

  g_duel_window_hwnd = CreateWindowExA(0, CLASS_MAGICGAME_MAINCLASS, gs_magic_the_gathering_title_00789460,
                                       s.style, 1, 0, s.width, s.height, g_main_window_hwnd,
                                       (HMENU)0, g_app_instance, (LPVOID)0);
  if (g_duel_window_hwnd == (HWND)0)
  {
    s.created_window = 0;
    load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
    strcat(s.startup_error, g_text_lines[8]);
    strcat(s.startup_error, "\n");
  }

  g_duel_accelerators = LoadAcceleratorsA(g_app_instance, (LPCSTR)0x6f);
  if ((g_duel_mode_flags & 1) == 0)
  {
    prepare_duel_video_mode_transition();
    sound_close();
    init_sound_dll(g_duel_window_hwnd, 0, 0);
    finish_duel_video_mode_transition();
  }
  else
  {
    finish_duel_video_mode_transition();
  }

  if (s.created_window == 0)
  {
    return 0;
  }
  else
  {
#ifdef _DEBUG
    create_kim_debug_window();
#endif
    return 1;
  }
}

// FUNCTION: MAGIC 0x0050ad0f
// FUNCTION: SHANDALAR 0x00544e8f
WPARAM run_duel_engine_message_loop(void)
{
  struct
  {
    MSG message;
    char help_path[264];
    WPARAM result;
  } s;

  strcpy(s.help_path, global_base_directory);
  strcat(s.help_path, "\\duel.hlp");
  WinHelpA(global_main_hwnd, s.help_path, 2, 0);
  g_duel_message_loop_active = 1;
  SendMessageA(g_duel_window_hwnd, WM_USER, 0, 0);

  while (GetMessageA(&s.message, (HWND)0, 0, 0) != 0)
  {
    dispatch_duel_engine_message(&s.message);
  }

  s.result = s.message.wParam;
  if (s.result == 0xffff)
  {
    s.result = (WPARAM)-1;
  }

  g_duel_active = 0;
  g_duel_message_loop_active = 0;
  return s.result;
}

// FUNCTION: MAGIC 0x0050d3ff
// FUNCTION: SHANDALAR 0x005474e1
void shutdown_duel_engine_window(void)
{
  if ((g_duel_mode_flags & 1) == 0)
  {
    prepare_duel_video_mode_transition();
    sound_close();
#ifdef SHANDALAR
    init_sound_dll(g_duel_help_owner_hwnd, 0, 0);
#else
    init_sound_dll(global_main_hwnd, 0, 0);
#endif
  }
  else
  {
    prepare_duel_video_mode_transition();
  }

  DestroyWindow(g_duel_window_hwnd);
  g_duel_window_hwnd = (HWND)0;
  g_duel_life_window_hwnd = 0;
  g_duel_full_card_window_hwnd = g_duel_life_window_hwnd;
  g_duel_phase_display_window_hwnd = 0;
  g_duel_attack_phase_window_hwnd = g_duel_phase_display_window_hwnd;
  g_duel_card_preview_window_hwnd = (HWND)0;
  destroy_windowclasses();
  DeleteCriticalSection(&g_duel_render_lock);
  GdiFlush();
  SetThreadPriority(GetCurrentThread(), 0);
}

// FUNCTION: MAGIC 0x00421830
// FUNCTION: SHANDALAR 0x0042ffd0
DWORD WINAPI RunDuelEngineThreadProc(LPVOID creature_type)
{
  struct
  {
    char stack_padding[0x7ec];
    WPARAM result;
  } s;

  g_duel_mode_flags = 1;
  g_duel_network_state = 0;
  g_duel_extra_turn_player = -1;
  g_duel_ai_mode_state = 0;
  g_duel_use_previous_backdrop_colors = 0;
  InitializeDuelUiGlobalIds();
  g_duel_run_mode = 4;
  g_duel_creature_type = (int)creature_type;

  if (g_duel_creature_type != -1)
  {
    load_text("Menus.txt", "DECKFACES");
    strcpy(unk_009266d0, g_text_lines[g_duel_creature_type - 1]);
  }
  else
  {
    load_text(global_ui_strings_filename, "OPPONENT");
    strcpy(unk_009266d0, g_text_lines[0]);
  }

  if (initialize_duel_engine_window() != 0)
  {
    s.result = run_duel_engine_message_loop();
    shutdown_duel_engine_window();
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004da380
// FUNCTION: SHANDALAR 0x00541040
int register_MAGICGAME_MainClass(LPCSTR class_name)
{
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0x20, wndproc_MAGICGAME_MainClass, 0,
                    LoadIconA(g_app_instance, (LPCSTR)0x66), (HBRUSH)GetStockObject(4), class_name);

  if (RegisterClassA(&wndclass) == 0)
    return 0;

  return 1;
}

// FUNCTION: MAGIC 0x0040a3f0
// FUNCTION: SHANDALAR 0x004bab60
int register_MAGICGAME_ChatClass(LPCSTR class_name)
{
  struct
  {
    int registered;
    WNDCLASSA wndclass;
  } s;

  s.registered = 1;
  SET_DUEL_WNDCLASS(s.wndclass, 0, wndproc_MAGICGAME_ChatClass, g_magicgame_chat_window_extra_bytes,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0x6, class_name);
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.registered = 0;
  }

  g_magicgame_chat_font = CreateFontIndirectA(LoadFontFromIni("Chat", 0));
  g_magicgame_chat_text_color = 0x2565656;
  return s.registered;
}

// FUNCTION: MAGIC 0x004e2f70
// FUNCTION: SHANDALAR 0x005638b0
int register_MAGICTHEME_IconButtonClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 8, wndproc_MAGICTHEME_IconButtonClass, 8,
                    (HICON)0, (HBRUSH)0x10, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x00507c60
// FUNCTION: SHANDALAR 0x004c6961
int register_MAGICGAME_BigCardCardClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_MAGICGAME_BigCardCardClass, 0x14,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x00464c30
// FUNCTION: SHANDALAR 0x00454350
int register_MAGIC_PaletteClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 3, wndproc_MAGIC_PaletteClass, 4,
                    (HICON)0, (HBRUSH)0x6, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x004b1710
// FUNCTION: SHANDALAR 0x004cbfa0
int register_MAGIC_CueCardClass(LPCSTR class_name)
{
  struct
  {
    int result;
    WNDCLASSA wndclass;
  } s;

  s.result = 1;
  s.wndclass.style = 0x800;
  s.wndclass.lpfnWndProc = wndproc_MAGIC_CueCardClass;
  s.wndclass.cbClsExtra = 0;
  s.wndclass.cbWndExtra = g_duel_cue_card_window_extra_bytes;
  s.wndclass.hInstance = g_app_instance;
  s.wndclass.hIcon = (HICON)0;
  s.wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  s.wndclass.hbrBackground = (HBRUSH)0x6;
  s.wndclass.lpszMenuName = (LPCSTR)0;
  s.wndclass.lpszClassName = class_name;
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.result = 0;
  }
  g_duel_cue_card_timer_id = 0;
  g_duel_cue_card_timer_elapsed = 500;
  g_duel_cue_card_x_offset = 10;
  g_duel_cue_card_y_offset = 0x12;
  g_duel_cue_card_mouse_threshold = 0x14;
  g_duel_cue_card_font = CreateFontIndirectA(LoadFontFromIni("CueCard", 0));
  g_duel_cue_card_background_brush = CreateSolidBrush(0x296bed2);
  g_duel_cue_card_shadow_brush = CreateSolidBrush(0x27f7f7f);
  g_duel_cue_card_text_color = 0x2505050;
  if (g_duel_cue_card_background_brush == (HBRUSH)0 ||
      g_duel_cue_card_shadow_brush == (HBRUSH)0)
  {
    s.result = 0;
  }
  return s.result;
}

// FUNCTION: MAGIC 0x0043db50
// FUNCTION: SHANDALAR 0x004093d0
int register_MAGIC_PlayerDirectiveClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 1, wndproc_MAGIC_PlayerDirectiveClass, 0,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0x6, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x004220ac
// FUNCTION: SHANDALAR 0x004307f2
int register_window_classes(void)
{
  int registration_failed;

  InitCommonControls();
  registration_failed = 0;
  registration_failed &= register_MAGICGAME_MainClass(CLASS_MAGICGAME_MAINCLASS);
  registration_failed &= register_MAGICGAME_LifeClass(CLASS_MAGICGAME_LIFE);
  registration_failed &= register_MAGICGAME_FullCardClass(CLASS_MAGICGAME_FULL_CARD);
  registration_failed &= register_MAGICGAME_ManaSummaryClass(CLASS_MAGICGAME_MANA_SUMMARY);
  registration_failed &= register_MAGICGAME_HandClass(CLASS_MAGICGAME_HAND);
  registration_failed &= register_MAGICGAME_ChatClass(CLASS_MAGICGAME_CHAT);
  registration_failed &= register_MAGICGAME_CardClass(CLASS_MAGICGAME_CARD);
  registration_failed &= register_MAGICGAME_PhaseDisplayClass(CLASS_MAGICGAME_PHASE_DISPLAY);
  registration_failed &= register_MAGICGAME_AttackPhaseDisplayClass(CLASS_MAGICGAME_ATTACK_PHASE_DISPLAY);
  registration_failed &= register_MAGICGAME_TerritoryClass(CLASS_MAGICGAME_TERRITORY);
  registration_failed &= register_MAGICGAME_LibraryClass(CLASS_MAGICGAME_LIBRARY);
  registration_failed &= register_MAGICGAME_GraveyardClass(CLASS_MAGICGAME_GRAVEYARD);
  registration_failed &= register_MAGICGAME_AttackClass(CLASS_MAGICGAME_ATTACK);
  registration_failed &= register_MAGICGAME_SpellChainClass(CLASS_MAGICGAME_SPELL_CHAIN);
  registration_failed &= register_MAGICGAME_FaceClass(CLASS_MAGICGAME_FACE);
  registration_failed &= register_MAGICGAME_ScrollbarClass(CLASS_MAGICGAME_SCROLLBAR);
  registration_failed &= register_MAGICTHEME_IconButtonClass(CLASS_MAGICTHEME_ICON_BUTTON);
  registration_failed &= register_MAGICGAME_BigCardChoiceClass(CLASS_MAGICGAME_BIG_CARD_CHOICE);
  registration_failed &= register_MAGICGAME_BigCardCardClass(CLASS_MAGICGAME_BIG_CARD_CARD);
  registration_failed &= register_MAGIC_PaletteClass(CLASS_MAGIC_PALETTE);
  registration_failed &= register_MAGIC_CueCardClass(CLASS_MAGIC_CUE_CARD);
  registration_failed &= register_MAGIC_PlayerDirectiveClass(CLASS_MAGIC_PLAYER_DIRECTIVE);
  registration_failed &= register_MAGIC_TellUserClass(CLASS_MAGIC_TELL_USER);
  if (registration_failed != 0)
  {
    return 0;
  }

  return 1;
}

// FUNCTION: MAGIC 0x0040a4a7
// FUNCTION: SHANDALAR 0x004bac17
void destroy_MAGICGAME_ChatClass(LPCSTR class_name)
{
  if (g_magicgame_chat_font != (HFONT)0)
  {
    DeleteObject(g_magicgame_chat_font);
  }

  g_magicgame_chat_font = (HFONT)0;
}

// FUNCTION: MAGIC 0x00498142
// FUNCTION: SHANDALAR 0x004c19a2
void destroy_MAGICGAME_LibraryClass(LPCSTR class_name)
{
  if (g_library_popup_menu != (HMENU)0)
  {
    DestroyMenu(g_library_popup_menu);
  }
  if (g_library_submenu != (HMENU)0)
  {
    DestroyMenu(g_library_submenu);
  }
  g_library_popup_menu = (HMENU)0;
  g_library_submenu = (HMENU)0;
}

// FUNCTION: MAGIC 0x00450ca9
// FUNCTION: SHANDALAR 0x0050d809
void destroy_MAGICGAME_GraveyardClass(LPCSTR class_name)
{
  if (g_graveyard_popup_menu != (HMENU)0)
  {
    DestroyMenu(g_graveyard_popup_menu);
  }
  g_graveyard_popup_menu = (HMENU)0;
}

// FUNCTION: MAGIC 0x00507f12
// FUNCTION: SHANDALAR 0x004c6c10
void destroy_MAGICGAME_BigCardChoiceClass(LPCSTR class_name)
{
  if (g_magicgame_big_card_choice_pic != (HANDLE)0)
  {
    delete_and_close_object(g_magicgame_big_card_choice_pic);
  }

  g_magicgame_big_card_choice_pic = (HANDLE)0;
}

// FUNCTION: MAGIC 0x00507cf2
// FUNCTION: SHANDALAR 0x004c69f3
void destroy_MAGICGAME_BigCardCardClass(LPCSTR class_name)
{
}

// FUNCTION: MAGIC 0x004b1831
// FUNCTION: SHANDALAR 0x004cc0c1
void destroy_MAGIC_CueCardClass(LPCSTR class_name)
{
  if (g_duel_cue_card_timer_id != 0)
  {
    KillTimer((HWND)0, g_duel_cue_card_timer_id);
    g_duel_cue_card_timer_id = 0;
  }
  if (g_duel_cue_card_font != (HFONT)0)
  {
    DeleteObject(g_duel_cue_card_font);
  }
  if (g_duel_cue_card_background_brush != (HBRUSH)0)
  {
    DeleteObject(g_duel_cue_card_background_brush);
  }
  if (g_duel_cue_card_shadow_brush != (HBRUSH)0)
  {
    DeleteObject(g_duel_cue_card_shadow_brush);
  }
  g_duel_cue_card_font = (HFONT)0;
  g_duel_cue_card_background_brush = (HBRUSH)0;
  g_duel_cue_card_shadow_brush = (HBRUSH)0;
}

// FUNCTION: MAGIC 0x0042224e
// FUNCTION: SHANDALAR 0x00430994
int destroy_windowclasses(void)
{
  destroy_MAGICGAME_LifeClass(CLASS_MAGICGAME_LIFE);
  destroy_MAGICGAME_FullCardClass(CLASS_MAGICGAME_FULL_CARD);
  destroy_MAGICGAME_ManaSummaryClass(CLASS_MAGICGAME_MANA_SUMMARY);
  destroy_MAGICGAME_HandClass(CLASS_MAGICGAME_HAND);
  destroy_MAGICGAME_ChatClass(CLASS_MAGICGAME_CHAT);
  destroy_MAGICGAME_CardClass(CLASS_MAGICGAME_CARD);
  destroy_MAGICGAME_PhaseDisplayClass(CLASS_MAGICGAME_PHASE_DISPLAY);
  destroy_MAGICGAME_AttackPhaseDisplayClass(CLASS_MAGICGAME_ATTACK_PHASE_DISPLAY);
  destroy_MAGICGAME_TerritoryClass(CLASS_MAGICGAME_TERRITORY);
  destroy_MAGICGAME_LibraryClass(CLASS_MAGICGAME_LIBRARY);
  destroy_MAGICGAME_GraveyardClass(CLASS_MAGICGAME_GRAVEYARD);
  destroy_MAGICGAME_AttackClass(CLASS_MAGICGAME_ATTACK);
  destroy_MAGICGAME_SpellChainClass(CLASS_MAGICGAME_SPELL_CHAIN);
  destroy_MAGICGAME_FaceClass(CLASS_MAGICGAME_FACE);
  destroy_MAGICGAME_ScrollbarClass(CLASS_MAGICGAME_SCROLLBAR);
  destroy_MAGICGAME_BigCardChoiceClass(CLASS_MAGICGAME_BIG_CARD_CHOICE);
  destroy_MAGICGAME_BigCardCardClass(CLASS_MAGICGAME_BIG_CARD_CARD);
  destroy_MAGIC_CueCardClass(CLASS_MAGIC_CUE_CARD);
  destroy_MAGIC_TellUserClass(CLASS_MAGIC_TELL_USER);
}

// FUNCTION: MAGIC 0x0040a4d5
// FUNCTION: SHANDALAR 0x004bac45
LRESULT CALLBACK wndproc_MAGICGAME_ChatClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    char large_text[500];
    char text[100];
    HDC dc;
    PAINTSTRUCT paint;
    RECT client_rect;
    RECT create_rect;
    RECT window_rect;
    HFONT font;
    int width;
  } s;

  switch (msg)
  {
  case 0x400:
    s.width = GetWindowLongA(hwnd, CHAT_WIDTH_WINDOW_LONG_OFFSET);
    return s.width;

  case 0x401:
    s.width = wparam;
    SetWindowLongA(hwnd, CHAT_WIDTH_WINDOW_LONG_OFFSET, s.width);
    GetWindowRect(hwnd, &s.window_rect);
    if (s.window_rect.right - s.window_rect.left < s.width)
    {
      SetWindowPos(hwnd, (HWND)0, 0, 0, s.width, s.window_rect.bottom - s.window_rect.top, SWP_NOMOVE | SWP_NOZORDER);
    }
    return 0;

  case WM_CLOSE:
    ShowWindow(hwnd, SW_HIDE);
    return 0;

  case WM_CREATE:
    s.font = g_magicgame_chat_font;
    SetWindowLongA(hwnd, CHAT_FONT_WINDOW_LONG_OFFSET, (LONG)s.font);
    GetWindowRect(hwnd, &s.create_rect);
    s.width = s.create_rect.right - s.create_rect.left;
    SetWindowLongA(hwnd, CHAT_WIDTH_WINDOW_LONG_OFFSET, s.width);
    return 0;

  case WM_GETFONT:
    s.font = (HFONT)GetWindowLongA(hwnd, CHAT_FONT_WINDOW_LONG_OFFSET);
    return (LRESULT)s.font;

  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, WM_TIMER, 1, 0);
    return 0;

  case WM_PAINT:
    s.font = (HFONT)GetWindowLongA(hwnd, CHAT_FONT_WINDOW_LONG_OFFSET);
    s.dc = BeginPaint(hwnd, &s.paint);
    if (s.dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.dc);
      SetTextColor(s.dc, g_magicgame_chat_text_color);
      SetBkMode(s.dc, TRANSPARENT);
      SelectObject(s.dc, s.font);
      GetWindowTextA(hwnd, s.text, 100);
      GetClientRect(hwnd, &s.client_rect);
      s.client_rect.left += 10;
      SetMapMode(s.dc, MM_ANISOTROPIC);
      SetWindowExtEx(s.dc, s.client_rect.right - s.client_rect.left, 20, (LPSIZE)0);
      SetViewportExtEx(s.dc, s.client_rect.right - s.client_rect.left, s.client_rect.bottom - s.client_rect.top, (LPSIZE)0);
      DrawTextA(s.dc, s.text, -1, &s.client_rect, DT_BOTTOM);
      EndPaint(hwnd, &s.paint);
    }
    return 0;

  case WM_SETFONT:
    s.font = (HFONT)wparam;
    if (s.font == (HFONT)0)
    {
      s.font = g_magicgame_chat_font;
    }
    SetWindowLongA(hwnd, CHAT_FONT_WINDOW_LONG_OFFSET, (LONG)s.font);
    InvalidateRect(hwnd, (RECT *)0, 1);
    GetWindowTextA(hwnd, s.large_text, 500);
    SetWindowTextA(hwnd, s.large_text);
    return 0;

  case WM_SHOWWINDOW:
    if (wparam != 0)
    {
      SetTimer(hwnd, 1, 10000, (TIMERPROC)0);
    }
    return DefWindowProcA(hwnd, msg, wparam, lparam);

  case WM_TIMER:
    KillTimer(hwnd, 1);
    ShowWindow(hwnd, SW_HIDE);
    return 0;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00498cd2
// FUNCTION: SHANDALAR 0x004c252a
LRESULT CALLBACK wndproc_ShuffleCard(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC dc;
    RECT client_rect;
  } s;

  switch (msg)
  {
  case WM_ERASEBKGND:
    s.dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.dc);
    GetClientRect(hwnd, &s.client_rect);
    DrawCardBack(s.dc, &s.client_rect);
    return 0;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004517ac
// FUNCTION: SHANDALAR 0x0050e2fc
LRESULT CALLBACK wndproc_ExpandedGraveyard(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    POINT mouse_point;
    HWND target_hwnd;
  } s;

  switch (msg)
  {
  case 0x437:
    return 0;
  case WM_CREATE:
    return 0;

  case WM_LBUTTONDOWN:
    SendMessageA(g_duel_player_graveyard_window_hwnd, 0x400, 0, 0);
    SendMessageA(g_phasebar_your_untap_window_hwnd, 0x400, 0, 0);
    return 0;

  case WM_MOUSEMOVE:
  case WM_RBUTTONDBLCLK:
    GetCursorPos(&s.mouse_point);
    s.target_hwnd = WindowFromPoint(s.mouse_point);
    MapWindowPoints((HWND)0, s.target_hwnd, &s.mouse_point, 1);
    if (hwnd != s.target_hwnd)
    {
      SendMessageA(s.target_hwnd, msg, wparam, MAKELONG(s.mouse_point.x, s.mouse_point.y));
    }
    return 0;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00488f71
// FUNCTION: SHANDALAR 0x004cbb3f
LRESULT CALLBACK wndproc_SpellMinimized(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int menu_item_count;
    POINT popup_point;
    RECT popup_rect;
    char background_path[264];
    HDC erase_dc;
    RECT client_rect;
  } s;

  switch (msg)
  {
  case 0x437:
    strcpy((char *)wparam, gs_cuecard_minimized_spell_chain_00926090);
    return 1;

  case WM_CLOSE:
    ShowWindow(hwnd, SW_HIDE);
    ShowWindow(g_duel_phase_display_window_hwnd, SW_HIDE);
    return 0;

  case WM_COMMAND:
    return SendMessageA(g_duel_phase_display_window_hwnd, msg, wparam, lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.client_rect);
    IntersectClipRect(s.erase_dc, 0, 0, s.client_rect.right, s.client_rect.bottom);
    if (g_spell_minimized_background_bitmap == (HBITMAP)0)
    {
      strcpy(s.background_path, global_duelart_path);
      strcat(s.background_path, "\\WINBK_SpellMin.pic");
      g_spell_minimized_background_bitmap = load_pic(s.background_path);
    }
    if (g_spell_minimized_background_bitmap != (HBITMAP)0)
    {
      DrawBitmapToRect(s.erase_dc, &s.client_rect, g_spell_minimized_background_bitmap);
    }
    else
    {
      FillRect(s.erase_dc, &s.client_rect, GetStockObject(GRAY_BRUSH));
    }
    return 1;

  case WM_LBUTTONDOWN:
    SendMessageA(g_duel_phase_display_window_hwnd, WM_COMMAND, 0x66, 0);
    return 0;

  case WM_RBUTTONDOWN:
    s.popup_point.x = (unsigned int)lparam & 0xffff;
    s.popup_point.y = HIWORD(lparam);
    ClientToScreen(hwnd, &s.popup_point);
    SetRect(&s.popup_rect, s.popup_point.x, s.popup_point.y,
            s.popup_point.x + 1, s.popup_point.y + 1);
    TrackPopupMenu(g_spell_minimized_popup_menu, TPM_RIGHTBUTTON, s.popup_point.x,
                   s.popup_point.y, 0, hwnd, &s.popup_rect);
    return 0;

  case WM_INITMENU:
    AppendMenuA(g_spell_minimized_popup_menu, 0, 0x66, g_spell_minimized_menu_restore_text);
    AppendMenuA(g_spell_minimized_popup_menu, 0, 100, g_spell_minimized_menu_help_text);
    return 0;

  case WM_MENUSELECT:
    if (HIWORD(wparam) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_spell_minimized_popup_menu);
      while (s.menu_item_count-- != 0)
      {
        DeleteMenu(g_spell_minimized_popup_menu, 0, MF_BYPOSITION);
      }
    }
    return 0;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004e2ff4
// FUNCTION: SHANDALAR 0x00563934
LRESULT CALLBACK wndproc_MAGICTHEME_IconButtonClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int pressed;
    HICON icon;
  } s;

  switch (msg)
  {
  case 0x400:
    s.pressed = (int)wparam;
    SetWindowLongA(hwnd, g_icon_button_pressed_long_offset, s.pressed);
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;

  case 0x401:
    return s.pressed = GetWindowLongA(hwnd, g_icon_button_pressed_long_offset);

  case WM_CREATE:
    s.icon = LoadIconA(g_app_instance, ((CREATESTRUCTA *)lparam)->lpszName);
    SetWindowLongA(hwnd, g_icon_button_icon_long_offset, (LONG)s.icon);
    s.pressed = 0;
    SetWindowLongA(hwnd, g_icon_button_pressed_long_offset, s.pressed);
    return 0;

  case WM_DESTROY:
    s.icon = (HICON)GetWindowLongA(hwnd, g_icon_button_icon_long_offset);
    if (s.icon != (HICON)0)
    {
      DestroyIcon(s.icon);
    }
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_GETDLGCODE:
    return DLGC_BUTTON;

  case WM_LBUTTONDOWN:
    SetCapture(hwnd);
    s.pressed = 1;
    SetWindowLongA(hwnd, g_icon_button_pressed_long_offset, s.pressed);
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;

  case WM_MOUSEMOVE:
    if (GetCapture() == hwnd)
    {
      POINT mouse_move_point;
      RECT mouse_move_rect;
      mouse_move_point.x = (unsigned int)lparam & 0xffff;
      mouse_move_point.y = HIWORD(lparam);
      GetClientRect(hwnd, &mouse_move_rect);
      s.pressed = PtInRect(&mouse_move_rect, mouse_move_point);
      SetWindowLongA(hwnd, g_icon_button_pressed_long_offset, s.pressed);
      InvalidateRect(hwnd, NULL, TRUE);
    }
    return 0;

  case WM_LBUTTONUP:
    if (GetCapture() == hwnd)
    {
      POINT mouse_up_point;
      RECT mouse_up_rect;
      mouse_up_point.x = (unsigned int)lparam & 0xffff;
      mouse_up_point.y = HIWORD(lparam);
      GetClientRect(hwnd, &mouse_up_rect);
      s.pressed = PtInRect(&mouse_up_rect, mouse_up_point);
      SetWindowLongA(hwnd, g_icon_button_pressed_long_offset, s.pressed);
      InvalidateRect(hwnd, NULL, TRUE);
      ReleaseCapture();
      if (s.pressed != 0)
      {
        SetFocus(hwnd);
      }
    }
    return 0;

  case WM_LBUTTONDBLCLK:
    SendMessageA(GetParent(hwnd), WM_COMMAND, 1, (LPARAM)GetDlgItem(GetParent(hwnd), 1));
    return 0;

  case WM_SETFOCUS:
    SendMessageA(GetParent(hwnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(hwnd), 0), (LPARAM)hwnd);
    return 0;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: MAGIC 0x00507cfd
// FUNCTION: SHANDALAR 0x004c69fe
LRESULT CALLBACK wndproc_MAGICGAME_BigCardCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    POINT mouse_point;
  } s;

  switch (msg)
  {
  case WM_CREATE:
  case WM_DESTROY:
  case WM_PAINT:
  case WM_ERASEBKGND:
  case 0x30f:
  case 0x310:
  case 0x311:
    return CallWindowProcA(wndproc_MAGICGAME_CardClass, hwnd, msg, wparam, lparam);

  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
    s.mouse_point.x = lparam & 0xffff;
    s.mouse_point.y = (unsigned short)(((unsigned int)lparam >> 16) & 0xffff);
    MapWindowPoints(hwnd, GetParent(hwnd), &s.mouse_point, 1);
    SendMessageA(GetParent(hwnd), msg, wparam, MAKELONG(s.mouse_point.x, s.mouse_point.y));
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: MAGIC 0x00464cb4
// FUNCTION: SHANDALAR 0x004543d4
LRESULT CALLBACK wndproc_MAGIC_PaletteClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    char color_text[100];
    int paint_column;
    HDC paint_dc;
    int paint_row;
    PAINTSTRUCT paint_struct;
    PALETTEENTRY palette_entry;
    HBRUSH color_brush;
    RECT paint_rect;
    int hit_color;
    int column;
    int mouse_x;
    int mouse_y;
    int row;
    RECT mouse_rect;
    int hit_found;
    int selected_color;
  } s;

  switch (msg)
  {
  case WM_CLOSE:
    ShowWindow(hwnd, SW_HIDE);
    return 0;

  case WM_CREATE:
    s.selected_color = 0;
    SetWindowLongA(hwnd, g_palette_selected_color_window_long_offset, s.selected_color);
    return 0;

  case WM_MOUSEMOVE:
    s.selected_color = GetWindowLongA(hwnd, g_palette_selected_color_window_long_offset);
    s.mouse_x = lparam & 0xffff;
    s.mouse_y = (unsigned short)((lparam >> 16) & 0xffff);
    s.hit_found = 0;
    for (s.row = 0; s.row < 16 && s.hit_found == 0; s.row = s.row + 1)
    {
      for (s.column = 0; s.column < 16 && s.hit_found == 0; s.column = s.column + 1)
      {
        SetRect(&s.mouse_rect, s.column << 4, s.row << 4, (s.column + 1) << 4, (s.row + 1) << 4);
        OffsetRect(&s.mouse_rect, g_palette_grid_x_offset, g_palette_grid_y_offset);
        if (((BOOL(WINAPI *)(const RECT *, int, int))PtInRect)(&s.mouse_rect, s.mouse_x, s.mouse_y) != 0)
        {
          s.hit_found = 1;
          s.hit_color = (s.row << 4) + s.column;
        }
      }
    }
    if (s.hit_found != 0)
    {
      if (s.selected_color != s.hit_color)
      {
        s.selected_color = s.hit_color;
        SetWindowLongA(hwnd, g_palette_selected_color_window_long_offset, s.selected_color);
        InvalidateRect(hwnd, (RECT *)0, FALSE);
      }
    }
    return 0;

  case WM_PAINT:
    s.selected_color = GetWindowLongA(hwnd, g_palette_selected_color_window_long_offset);
    s.paint_dc = BeginPaint(hwnd, &s.paint_struct);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetClientRect(hwnd, &s.paint_rect);
      g_palette_grid_x_offset = ((s.paint_rect.right - s.paint_rect.left) - 0x100) / 2;
      g_palette_grid_y_offset = ((s.paint_rect.bottom - s.paint_rect.top) - 0x100) / 2;
      for (s.paint_row = 0; s.paint_row < 16; s.paint_row = s.paint_row + 1)
      {
        for (s.paint_column = 0; s.paint_column < 16; s.paint_column = s.paint_column + 1)
        {
          s.color_brush = CreateSolidBrush(PALETTEINDEX((WORD)((s.paint_row << 4) + s.paint_column)));
          SetRect(&s.paint_rect, s.paint_column << 4, s.paint_row << 4, (s.paint_column + 1) << 4, (s.paint_row + 1) << 4);
          OffsetRect(&s.paint_rect, g_palette_grid_x_offset, g_palette_grid_y_offset);
          FillRect(s.paint_dc, &s.paint_rect, s.color_brush);
          DeleteObject(s.color_brush);
        }
      }
      if (GetPaletteEntries(global_cart_art_hpalette, s.selected_color, 1, &s.palette_entry) != 0)
      {
        sprintf(s.color_text, "#%3d: %3d,%3d,%3d         ", s.selected_color, s.palette_entry.peRed,
                s.palette_entry.peGreen, s.palette_entry.peBlue);
      }
      else
      {
        sprintf(s.color_text, "#%3d: not in palette         ", s.selected_color);
      }
      TextOutA(s.paint_dc, 0, 0, s.color_text, strlen(s.color_text));
      EndPaint(hwnd, &s.paint_struct);
    }
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004b18ca
// FUNCTION: SHANDALAR 0x004cc15a
LRESULT CALLBACK wndproc_MAGIC_CueCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    unsigned int font_param;
    char paint_text[100];
    HDC paint_dc;
    PAINTSTRUCT paint_struct;
    RECT inner_rect;
    RECT client_rect;
    unsigned int get_text_wparam;
    LPARAM get_text_lparam;
    int height;
    HDC dc;
    int width;
    unsigned int y;
    unsigned int x;
    int screen_width;
    LPCSTR text;
    SIZE text_size;
    int screen_height;
    HGDIOBJ font;
  } s;

  switch (msg)
  {
  case 0x400:
    s.font = (HGDIOBJ)GetWindowLongA(hwnd, g_duel_cue_card_font_window_long_offset);
    s.x = wparam & 0xffff;
    s.y = (unsigned short)((wparam >> 16) & 0xffff);
    s.text = (LPCSTR)lparam;
    s.dc = GetDC(hwnd);
    if (s.dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.dc);
      SelectObject(s.dc, s.font);
      GetTextExtentPoint32A(s.dc, s.text, lstrlenA(s.text), &s.text_size);
      s.width = s.text_size.cx + 10;
      s.height = s.text_size.cy + 6;
      ReleaseDC(hwnd, s.dc);
      s.screen_width = GetSystemMetrics(SM_CXSCREEN);
      s.screen_height = GetSystemMetrics(SM_CYSCREEN);
      if ((int)s.x < 1)
      {
        s.x = 1;
      }
      if ((int)(s.width + s.x) > s.screen_width - 1)
      {
        s.x = (s.screen_width - 1) - s.width;
      }
      if ((int)s.y < 1)
      {
        s.y = 1;
      }
      if ((int)(s.height + s.y) > s.screen_height - 1)
      {
        s.y = (s.screen_height - 1) - s.height;
      }
      MoveWindow(hwnd, s.x, s.y, s.width, s.height, 1);
      SetWindowTextA(hwnd, s.text);
      ShowWindow(hwnd, SW_SHOW);
      InvalidateRect(hwnd, (RECT *)0, 1);
    }
    return 0;

  case 0x401:
    s.get_text_lparam = lparam;
    s.get_text_wparam = wparam;
    GetWindowTextA(hwnd, (LPSTR)s.get_text_lparam, s.get_text_wparam);
    return 0;

  case WM_CREATE:
    s.font = (HGDIOBJ)g_duel_cue_card_font;
    SetWindowLongA(hwnd, g_duel_cue_card_font_window_long_offset, (LONG)s.font);
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_GETFONT:
    s.font = (HGDIOBJ)GetWindowLongA(hwnd, g_duel_cue_card_font_window_long_offset);
    return (LRESULT)s.font;

  case WM_KEYDOWN:
    SetFocus(g_duel_window_hwnd);
    PostMessageA(GetFocus(), msg, wparam, lparam);
    return 0;

  case WM_PAINT:
    s.font = (HGDIOBJ)GetWindowLongA(hwnd, g_duel_cue_card_font_window_long_offset);
    s.paint_dc = BeginPaint(hwnd, &s.paint_struct);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetClientRect(hwnd, &s.client_rect);
      SetRect(&s.inner_rect, s.client_rect.left, s.client_rect.top,
              s.client_rect.right - 2, s.client_rect.bottom - 2);
      FillRect(s.paint_dc, &s.client_rect, g_duel_cue_card_shadow_brush);
      FillRect(s.paint_dc, &s.inner_rect, g_duel_cue_card_background_brush);
      SetTextColor(s.paint_dc, g_duel_cue_card_text_color);
      SetBkMode(s.paint_dc, TRANSPARENT);
      GetWindowTextA(hwnd, s.paint_text, 100);
      SelectObject(s.paint_dc, s.font);
      DrawTextA(s.paint_dc, s.paint_text, -1, &s.inner_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
      EndPaint(hwnd, &s.paint_struct);
    }
    return 0;

  case WM_SETFONT:
    s.font_param = wparam;
    if (s.font_param == 0)
    {
      s.font_param = (unsigned int)g_duel_cue_card_font;
    }
    SetWindowLongA(hwnd, g_duel_cue_card_font_window_long_offset, s.font_param);
    InvalidateRect(hwnd, (RECT *)0, 1);
    return 0;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0043dbe2
// FUNCTION: SHANDALAR 0x00409462
LRESULT CALLBACK wndproc_MAGIC_PlayerDirectiveClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC paint_dc;
    PAINTSTRUCT paint_struct;
    char window_text[200];
    RECT text_rect;
    HDC erase_dc;
    RECT erase_rect;
    HBRUSH erase_brush;
  } s;

  switch (msg)
  {
  case WM_CREATE:

    return 0;
  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    s.erase_brush = CreateSolidBrush(0xffff);
    FillRect(s.erase_dc, &s.erase_rect, s.erase_brush);
    DeleteObject(s.erase_brush);
    return 1;
  case WM_PAINT:
    s.paint_dc = BeginPaint(hwnd, &s.paint_struct);
    if (s.paint_dc != NULL)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetWindowTextA(hwnd, s.window_text, 200);
      SetTextAlign(s.paint_dc, 6);
      SetBkMode(s.paint_dc, 1);
      SetTextColor(s.paint_dc, 0);
      GetClientRect(hwnd, &s.text_rect);
      DrawManaText(s.paint_dc, &s.text_rect, s.window_text, 1);
      EndPaint(hwnd, &s.paint_struct);
    }
    return 0;

  case WM_SETTEXT:
    InvalidateRect(hwnd, NULL, TRUE);
    return DefWindowProcA(hwnd, msg, wparam, lparam);

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);
  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004dbea9
// FUNCTION: SHANDALAR 0x00542b64
int create_duel_child_windows(HWND parent_window)
{
  struct
  {
    int visible_style;
    int popup_style;
    RECT client_rect;
  } s;

  GetClientRect(parent_window, &s.client_rect);
  s.visible_style = 0x50000000;
  s.popup_style = 0x80c00000;
  g_duel_cue_card_window_hwnd =
      CreateWindowExA(0, CLASS_MAGIC_CUE_CARD, "", 0x80000000, 0, 0, 0, 0, parent_window, (HMENU)0, g_app_instance, (LPVOID)0);
  DAT_007a7d74 =
      (int)CreateWindowExA(0, CLASS_MAGIC_PLAYER_DIRECTIVE, "", 0x80000000, 0, 0, 100, 30, parent_window, (HMENU)0, g_app_instance, (LPVOID)0);
  g_duel_prompt_context_hwnd =
      CreateWindowExA(0, CLASS_MAGIC_TELL_USER, "", 0x80000001, 0, 0, 0, 0, parent_window, (HMENU)0, g_app_instance, (LPVOID)0);
  DAT_008a8dec =
      (int)CreateWindowExA(0, CLASS_MAGICGAME_PHASE_DISPLAY, "Phase Display", s.visible_style, 0, 0, 0, 0, parent_window, (HMENU)0x77, g_app_instance, (LPVOID)0);
  DAT_008a8d78 =
      (int)CreateWindowExA(0, CLASS_MAGICGAME_ATTACK_PHASE_DISPLAY, "Attack Phase Display", s.visible_style, 0, 0, 0, 0, parent_window, (HMENU)0x78, g_app_instance, (LPVOID)0);
  g_duel_card_preview_window_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_FULL_CARD, "Full-size card", 0x90000000, 0, 0, 0, 0, parent_window, (HMENU)0, g_app_instance, (LPVOID)0);
  g_duel_life_status_window_2_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_LIFE, "Oppon Life", s.visible_style, 0, 0, 0, 0, parent_window, (HMENU)0x65, g_app_instance, (LPVOID)0);
  g_duel_life_status_window_1_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_LIFE, "Player Life", s.visible_style, 0, 0, 0, 0, parent_window, (HMENU)0x66, g_app_instance, (LPVOID)0);
  g_phasebar_your_untap_window_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_GRAVEYARD, "Oppon Graveyard", s.visible_style | 0x2000000, 0, 0, 0, 0, parent_window, (HMENU)0x6c, g_app_instance, (LPVOID)0);
  g_duel_player_graveyard_window_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_GRAVEYARD, "Player Graveyard", s.visible_style | 0x2000000, 0, 0, 0, 0, parent_window, (HMENU)0x6e, g_app_instance, (LPVOID)0);
  DAT_0092680c =
      CreateWindowExA(0, CLASS_MAGICGAME_LIBRARY, "Oppon Library", s.visible_style, 0, 0, 0, 0, parent_window, (HMENU)0x6d, g_app_instance, (LPVOID)0);
  DAT_0091ce30 =
      CreateWindowExA(0, CLASS_MAGICGAME_LIBRARY, "Player Library", s.visible_style, 0, 0, 0, 0, parent_window, (HMENU)0x6f, g_app_instance, (LPVOID)0);
  unk_00939344 =
      CreateWindowExA(0, CLASS_MAGICGAME_MANA_SUMMARY, "Oppon Mana", s.visible_style, 0, 0, 0, 0, parent_window, (HMENU)0x68, g_app_instance, (LPVOID)0);
  unk_008ce534 =
      CreateWindowExA(0, CLASS_MAGICGAME_MANA_SUMMARY, "Player Mana", s.visible_style, 0, 0, 0, 0, parent_window, (HMENU)0x69, g_app_instance, (LPVOID)0);
  g_duel_opponent_face_window_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_FACE, "Oppon Face", 0x40000000, 0, 0, 0, 0, parent_window, (HMENU)0x7c, g_app_instance, (LPVOID)0);
  g_duel_player_face_window_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_FACE, "Player Face", 0x40000000, 0, 0, 0, 0, parent_window, (HMENU)0x7b, g_app_instance, (LPVOID)0);
  global_opponent_chat_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_CHAT, "Oppon Chat", 0x80800000, 0, 0, 0, 0, parent_window, (HMENU)0, g_app_instance, (LPVOID)0);
  g_duel_player_chat_window_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_CHAT, "Player Chat", 0x80800000, 0, 0, 0, 0, parent_window, (HMENU)0, g_app_instance, (LPVOID)0);
  g_duel_player_battlefield_window_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_TERRITORY, "Player Territory", s.visible_style | 0x2000000, 0, 0, 0, 0, parent_window, (HMENU)0x79, g_app_instance, (LPVOID)0);
  g_duel_help_owner_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_TERRITORY, "Oppon Territory", s.visible_style | 0x2000000, 0, 0, 0, 0, parent_window, (HMENU)0x7a, g_app_instance, (LPVOID)0);
  g_duel_life_window_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_HAND, gs_window_title_opponent_0091c820, 0x82000000,
                      (s.client_rect.right * 80) / 100, (s.client_rect.bottom * 40) / 100, 0, 0, parent_window, (HMENU)0, g_app_instance, (LPVOID)0);
  g_duel_full_card_window_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_HAND, gs_window_title_your_hand_00777bf0, 0x82000000,
                      (s.client_rect.right * 80) / 100, (s.client_rect.bottom * 60) / 100, 0, 0, parent_window, (HMENU)0, g_app_instance, (LPVOID)0);
  g_duel_attack_phase_window_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_ATTACK, "", s.popup_style | 0x2000000, 0, 0, 0, 0, parent_window, (HMENU)0, g_app_instance, (LPVOID)0);
  g_duel_phase_display_window_hwnd =
      CreateWindowExA(0, CLASS_MAGICGAME_SPELL_CHAIN, gs_window_title_spell_chain_008b42f0, s.popup_style, 0, 0, 0, 0, parent_window, (HMENU)0, g_app_instance, (LPVOID)0);

  if (DAT_007a7d74 == 0 || g_duel_card_preview_window_hwnd == (HWND)0 ||
      g_duel_life_status_window_2_hwnd == (HWND)0 || g_duel_life_status_window_1_hwnd == (HWND)0 ||
      unk_00939344 == (HWND)0 || unk_008ce534 == (HWND)0 ||
      g_phasebar_your_untap_window_hwnd == (HWND)0 || DAT_0092680c == (HWND)0 ||
      g_duel_player_graveyard_window_hwnd == (HWND)0 || DAT_0091ce30 == (HWND)0 ||
      global_opponent_chat_hwnd == (HWND)0 || g_duel_player_chat_window_hwnd == (HWND)0 ||
      DAT_008a8dec == (HWND)0 || DAT_008a8d78 == (HWND)0 ||
      g_duel_attack_phase_window_hwnd == (HWND)0 || g_duel_phase_display_window_hwnd == (HWND)0 ||
      g_duel_player_battlefield_window_hwnd == (HWND)0 || g_duel_help_owner_hwnd == (HWND)0 ||
      g_duel_prompt_context_hwnd == (HWND)0 || g_duel_opponent_face_window_hwnd == (HWND)0 ||
      g_duel_player_face_window_hwnd == (HWND)0 || g_duel_life_window_hwnd == (HWND)0 ||
      g_duel_full_card_window_hwnd == (HWND)0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}
