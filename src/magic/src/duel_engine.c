#define DUEL_ENGINE_IMPL

#include <windows.h>
#include <commctrl.h>
#include <string.h>

#include "cardartlib/src/assert.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/global_other.h"
#include "magic/src/shared_startup.h"
#include "duel_engine.h"

#ifndef SHANDALAR
#include "manalinkinterface/manalinkinterface.h"
extern HWND global_main_hwnd;
#else
#include "shandalar/src/shandalar_global_strings.h"
extern HINSTANCE g_app_instance;
extern int global_screen_width;
extern int global_screen_height;
#endif

extern int g_duel_modal_action_active;

#define CLASS_MAGICGAME_MAIN "MAGICGAME_MainClass"
#define CLASS_MAGICGAME_LIFE "MAGICGAME_LifeClass"
#define CLASS_MAGICGAME_FULL_CARD "MAGICGAME_FullCardClass"
#define CLASS_MAGICGAME_MANA_SUMMARY "MAGICGAME_ManaSummaryClass"
#define CLASS_MAGICGAME_HAND "MAGICGAME_HandClass"
#define CLASS_MAGICGAME_CHAT "MAGICGAME_ChatClass"
#define CLASS_MAGICGAME_CARD "MAGICGAME_CardClass"
#define CLASS_MAGICGAME_PHASE_DISPLAY "MAGICGAME_PhaseDisplayClass"
#define CLASS_MAGICGAME_ATTACK_PHASE_DISPLAY "MAGICGAME_AttackPhaseDisplayClass"
#define CLASS_MAGICGAME_TERRITORY "MAGICGAME_TerritoryClass"
#define CLASS_MAGICGAME_LIBRARY "MAGICGAME_LibraryClass"
#define CLASS_MAGICGAME_GRAVEYARD "MAGICGAME_GraveyardClass"
#define CLASS_MAGICGAME_ATTACK "MAGICGAME_AttackClass"
#define CLASS_MAGICGAME_SPELL_CHAIN "MAGICGAME_SpellChainClass"
#define CLASS_MAGICGAME_FACE "MAGICGAME_FaceClass"
#define CLASS_MAGICGAME_SCROLLBAR "MAGICGAME_ScrollbarClass"
#define CLASS_MAGICTHEME_ICON_BUTTON "MAGICTHEME_IconButtonClass"
#define CLASS_MAGICGAME_BIG_CARD_CHOICE "MAGICGAME_BigCardChoiceClass"
#define CLASS_MAGICGAME_BIG_CARD_CARD "MAGICGAME_BigCardCardClass"
#define CLASS_MAGIC_PALETTE "MAGIC_PaletteClass"
#define CLASS_MAGIC_CUE_CARD "MAGIC_CueCardClass"
#define CLASS_MAGIC_PLAYER_DIRECTIVE "MAGIC_PlayerDirectiveClass"
#define CLASS_MAGIC_TELL_USER "MAGIC_TellUserClass"
#define CLASS_SHUFFLE_CARD "ShuffleCard"
#define CLASS_EXPANDED_GRAVEYARD "ExpandedGraveyard"
#define CLASS_GRAVEYARD_CARDS "GraveyardCards"
#define CLASS_ATTACK_SWORD_SHIELD "AttackSwordShield"
#define CLASS_ATTACK_MINIMIZED "AttackMinimized"
#define CLASS_SPELL_MINIMIZED "SpellMinimized"

#define SET_DUEL_WNDCLASS(wndclass_, style_, wndproc_, extra_, icon_, background_, class_name_) \
  (wndclass_).style = (style_);                                                                 \
  (wndclass_).lpfnWndProc = (wndproc_);                                                         \
  (wndclass_).cbClsExtra = 0;                                                                   \
  (wndclass_).cbWndExtra = (extra_);                                                            \
  (wndclass_).hInstance = g_app_instance;                                                       \
  (wndclass_).hIcon = (icon_);                                                                  \
  (wndclass_).hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);                              \
  (wndclass_).hbrBackground = (background_);                                                    \
  (wndclass_).lpszMenuName = (LPCSTR)0;                                                         \
  (wndclass_).lpszClassName = (class_name_)

int register_window_classes(void);
int destroy_windowclasses(void);

LRESULT CALLBACK wndproc_MAGICGAME_MainClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_LifeClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_FullCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_ManaSummaryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_HandClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_ChatClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_CardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_PhaseDisplayClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_AttackPhaseDisplayClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_TerritoryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_LibraryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_ShuffleCard(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_GraveyardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_ExpandedGraveyard(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_GraveyardCards(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_AttackClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_AttackSwordShield(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_AttackMinimized(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_SpellChainClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_SpellMinimized(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_FaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_ScrollbarClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICTHEME_IconButtonClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_BigCardChoiceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_BigCardCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGIC_PaletteClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGIC_CueCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGIC_PlayerDirectiveClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGIC_TellUserClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

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
}

// FUNCTION: MAGIC 0x00421f0a
// FUNCTION: SHANDALAR 0x00430650
int handle_duel_hover_help_message(MSG *message, UINT timer_elapsed)
{
  (void)message;
  (void)timer_elapsed;
  return 0;
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
  (void)message;
  return 0;
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
  g_duel_startup_state = 0;
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
#ifdef SHANDALAR
    s.width = global_screen_width;
    s.height = global_screen_height;
#else
    s.width = unk_0057d9f0;
    s.height = unk_0057d9f4;
#endif
  }
  else
  {
    s.width = GetSystemMetrics(0) - 1;
    s.height = GetSystemMetrics(1);
  }

  g_duel_window_hwnd = CreateWindowExA(0, CLASS_MAGICGAME_MAIN, gs_magic_the_gathering_title_00789460,
                                       s.style, 1, 0, s.width, s.height, g_duel_parent_window_hwnd,
                                       (HMENU)0, g_app_instance, (LPVOID)0);
  if (g_duel_window_hwnd == (HWND)0)
  {
    s.created_window = 0;
    load_text(global_ui_strings_filename, "PROMPT_STARTUPERROR");
    strcat(s.startup_error, text_lines[8]);
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
#ifdef SHANDALAR
  WinHelpA(g_duel_help_owner_hwnd, s.help_path, 2, 0);
#else
  WinHelpA(global_main_hwnd, s.help_path, 2, 0);
#endif
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

  unk_00742fc4 = 0;
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
    strcpy(unk_009266d0, text_lines[g_duel_creature_type - 1]);
  }
  else
  {
    load_text(global_ui_strings_filename, "OPPONENT");
    strcpy(unk_009266d0, text_lines[0]);
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
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0x20, wndproc_MAGICGAME_MainClass, 0,
                    LoadIconA(g_app_instance, (LPCSTR)0x66), (HBRUSH)GetStockObject(4), class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x004e1e00
// FUNCTION: SHANDALAR 0x004ccc10
int register_MAGICGAME_LifeClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_MAGICGAME_LifeClass, 0x10,
                    (HICON)0, (HBRUSH)0x6, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x00557850
// FUNCTION: SHANDALAR 0x0056dbb0
int register_MAGICGAME_FullCardClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0x803, wndproc_MAGICGAME_FullCardClass, 0x14,
                    (HICON)0, (HBRUSH)GetStockObject(4), class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x00539320
// FUNCTION: SHANDALAR 0x00550c00
int register_MAGICGAME_ManaSummaryClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_MAGICGAME_ManaSummaryClass, 4,
                    (HICON)0, (HBRUSH)0x6, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x004bbae0
// FUNCTION: SHANDALAR 0x0046ee20
int register_MAGICGAME_HandClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 3, wndproc_MAGICGAME_HandClass, 0x20,
                    (HICON)0, (HBRUSH)0x6, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
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

// FUNCTION: MAGIC 0x004cd940
// FUNCTION: SHANDALAR 0x00573f30
int register_MAGICGAME_CardClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_MAGICGAME_CardClass, 0x14,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x00535650
// FUNCTION: SHANDALAR 0x00559e90
int register_MAGICGAME_PhaseDisplayClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_MAGICGAME_PhaseDisplayClass, 8,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0x6, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x0053582a
// FUNCTION: SHANDALAR 0x0055a06a
int register_MAGICGAME_AttackPhaseDisplayClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_MAGICGAME_AttackPhaseDisplayClass, 8,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0x6, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x004e6a30
// FUNCTION: SHANDALAR 0x00504850
int register_MAGICGAME_TerritoryClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_MAGICGAME_TerritoryClass, 0x10,
                    (HICON)0, (HBRUSH)0, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x00497fd0
// FUNCTION: SHANDALAR 0x004c1830
int register_MAGICGAME_LibraryClass(LPCSTR class_name)
{
  ATOM atom1;
  ATOM atom2;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 3, wndproc_MAGICGAME_LibraryClass, 4,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)GetStockObject(4), class_name);
  atom1 = RegisterClassA(&wndclass);
  SET_DUEL_WNDCLASS(wndclass, 0, wndproc_ShuffleCard, 0,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0, CLASS_SHUFFLE_CARD);
  atom2 = RegisterClassA(&wndclass);
  return atom1 != 0 && atom2 != 0;
}

// FUNCTION: MAGIC 0x00450a70
// FUNCTION: SHANDALAR 0x0050d5d0
int register_MAGICGAME_GraveyardClass(LPCSTR class_name)
{
  ATOM atom1;
  ATOM atom2;
  ATOM atom3;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_MAGICGAME_GraveyardClass, 0xc,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)GetStockObject(4), class_name);
  atom1 = RegisterClassA(&wndclass);
  SET_DUEL_WNDCLASS(wndclass, 8, wndproc_ExpandedGraveyard, 0,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)GetStockObject(4), CLASS_EXPANDED_GRAVEYARD);
  atom2 = RegisterClassA(&wndclass);
  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_GraveyardCards, 4,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0x6, CLASS_GRAVEYARD_CARDS);
  atom3 = RegisterClassA(&wndclass);
  return atom1 != 0 && atom2 != 0 && atom3 != 0;
}

// FUNCTION: MAGIC 0x004d2ac0
// FUNCTION: SHANDALAR 0x0045c730
int register_MAGICGAME_AttackClass(LPCSTR class_name)
{
  ATOM atom1;
  ATOM atom2;
  ATOM atom3;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0x800, wndproc_MAGICGAME_AttackClass, 8,
                    (HICON)0, (HBRUSH)0x6, class_name);
  atom1 = RegisterClassA(&wndclass);
  SET_DUEL_WNDCLASS(wndclass, 0x801, wndproc_AttackSwordShield, 0,
                    (HICON)0, (HBRUSH)0x6, CLASS_ATTACK_SWORD_SHIELD);
  atom2 = RegisterClassA(&wndclass);
  SET_DUEL_WNDCLASS(wndclass, 3, wndproc_AttackMinimized, 0,
                    (HICON)0, (HBRUSH)0x6, CLASS_ATTACK_MINIMIZED);
  atom3 = RegisterClassA(&wndclass);
  return atom1 != 0 && atom2 != 0 && atom3 != 0;
}

// FUNCTION: MAGIC 0x00486050
// FUNCTION: SHANDALAR 0x004c8c30
int register_MAGICGAME_SpellChainClass(LPCSTR class_name)
{
  ATOM atom1;
  ATOM atom2;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0x800, wndproc_MAGICGAME_SpellChainClass, 8,
                    (HICON)0, (HBRUSH)0x6, class_name);
  atom1 = RegisterClassA(&wndclass);
  SET_DUEL_WNDCLASS(wndclass, 3, wndproc_SpellMinimized, 0,
                    (HICON)0, (HBRUSH)0x6, CLASS_SPELL_MINIMIZED);
  atom2 = RegisterClassA(&wndclass);
  return atom1 != 0 && atom2 != 0;
}

// FUNCTION: MAGIC 0x004637d0
// FUNCTION: SHANDALAR 0x00425f10
int register_MAGICGAME_FaceClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0xb, wndproc_MAGICGAME_FaceClass, 8,
                    (HICON)0, (HBRUSH)GetStockObject(4), class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
}

// FUNCTION: MAGIC 0x00490900
// FUNCTION: SHANDALAR 0x005562d0
int register_MAGICGAME_ScrollbarClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 1, wndproc_MAGICGAME_ScrollbarClass, 0x28,
                    (HICON)0, (HBRUSH)0x6, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
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

// FUNCTION: MAGIC 0x00507e37
// FUNCTION: SHANDALAR 0x004c6b35
int register_MAGICGAME_BigCardChoiceClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0, wndproc_MAGICGAME_BigCardChoiceClass, 0x14,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0x6, class_name);
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
  ATOM atom;
  WNDCLASSA wndclass;

  SET_DUEL_WNDCLASS(wndclass, 0x800, wndproc_MAGIC_CueCardClass, 4,
                    (HICON)0, (HBRUSH)0x6, class_name);
  atom = RegisterClassA(&wndclass);
  return atom != 0;
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

// FUNCTION: MAGIC 0x004eb3e0
// FUNCTION: SHANDALAR 0x004690e0
int register_MAGIC_TellUserClass(LPCSTR class_name)
{
  ATOM atom;
  WNDCLASSA wndclass;

  g_tell_user_default_button_state = 2;
  g_tell_user_button_state = 1;
  SET_DUEL_WNDCLASS(wndclass, 1, wndproc_MAGIC_TellUserClass, 4,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)GetStockObject(2), class_name);
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
  registration_failed &= register_MAGICGAME_MainClass(CLASS_MAGICGAME_MAIN);
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

// FUNCTION: MAGIC 0x004e1f8e
// FUNCTION: SHANDALAR 0x004ccd9e
void destroy_MAGICGAME_LifeClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_LIFE, g_app_instance);
}

// FUNCTION: MAGIC 0x0055793c
// FUNCTION: SHANDALAR 0x0056dc9c
void destroy_MAGICGAME_FullCardClass(void)
{
  if (g_magicgame_full_card_menu != (HMENU)0)
  {
    DestroyMenu(g_magicgame_full_card_menu);
  }

  g_magicgame_full_card_menu = (HMENU)0;
}

// FUNCTION: MAGIC 0x005394da
// FUNCTION: SHANDALAR 0x00550dba
void destroy_MAGICGAME_ManaSummaryClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_MANA_SUMMARY, g_app_instance);
}

// FUNCTION: MAGIC 0x004bbbc7
// FUNCTION: SHANDALAR 0x0046ef07
void destroy_MAGICGAME_HandClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_HAND, g_app_instance);
}

// FUNCTION: MAGIC 0x0040a4a7
// FUNCTION: SHANDALAR 0x004bac17
void destroy_MAGICGAME_ChatClass(void)
{
  if (g_magicgame_chat_font != (HFONT)0)
  {
    DeleteObject(g_magicgame_chat_font);
  }

  g_magicgame_chat_font = (HFONT)0;
}

// FUNCTION: MAGIC 0x004cdf7b
// FUNCTION: SHANDALAR 0x0057456b
void destroy_MAGICGAME_CardClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_CARD, g_app_instance);
}

// FUNCTION: MAGIC 0x005357b4
// FUNCTION: SHANDALAR 0x00559ff4
void destroy_MAGICGAME_PhaseDisplayClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_PHASE_DISPLAY, g_app_instance);
}

// FUNCTION: MAGIC 0x005358fb
// FUNCTION: SHANDALAR 0x0055a13b
void destroy_MAGICGAME_AttackPhaseDisplayClass(void)
{
  if (g_magicgame_attack_phase_display_pic != (HANDLE)0)
  {
    delete_and_close_object(g_magicgame_attack_phase_display_pic);
  }

  g_magicgame_attack_phase_display_pic = (HANDLE)0;
}

// FUNCTION: MAGIC 0x004e6dc8
// FUNCTION: SHANDALAR 0x00504be8
void destroy_MAGICGAME_TerritoryClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_TERRITORY, g_app_instance);
}

// FUNCTION: MAGIC 0x00498142
// FUNCTION: SHANDALAR 0x004c19a2
void destroy_MAGICGAME_LibraryClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_LIBRARY, g_app_instance);
  UnregisterClassA(CLASS_SHUFFLE_CARD, g_app_instance);
}

// FUNCTION: MAGIC 0x00450ca9
// FUNCTION: SHANDALAR 0x0050d809
void destroy_MAGICGAME_GraveyardClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_GRAVEYARD, g_app_instance);
  UnregisterClassA(CLASS_EXPANDED_GRAVEYARD, g_app_instance);
  UnregisterClassA(CLASS_GRAVEYARD_CARDS, g_app_instance);
}

// FUNCTION: MAGIC 0x004d2f34
// FUNCTION: SHANDALAR 0x0045cba4
void destroy_MAGICGAME_AttackClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_ATTACK, g_app_instance);
  UnregisterClassA(CLASS_ATTACK_SWORD_SHIELD, g_app_instance);
  UnregisterClassA(CLASS_ATTACK_MINIMIZED, g_app_instance);
}

// FUNCTION: MAGIC 0x00486369
// FUNCTION: SHANDALAR 0x004c8f49
void destroy_MAGICGAME_SpellChainClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_SPELL_CHAIN, g_app_instance);
  UnregisterClassA(CLASS_SPELL_MINIMIZED, g_app_instance);
}

// FUNCTION: MAGIC 0x00463a6e
// FUNCTION: SHANDALAR 0x004261ae
void destroy_MAGICGAME_FaceClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_FACE, g_app_instance);
}

// FUNCTION: MAGIC 0x004909b5
// FUNCTION: SHANDALAR 0x00556385
void destroy_MAGICGAME_ScrollbarClass(void)
{
  UnregisterClassA(CLASS_MAGICGAME_SCROLLBAR, g_app_instance);
}

// FUNCTION: MAGIC 0x00507f12
// FUNCTION: SHANDALAR 0x004c6c10
void destroy_MAGICGAME_BigCardChoiceClass(void)
{
  if (g_magicgame_big_card_choice_pic != (HANDLE)0)
  {
    delete_and_close_object(g_magicgame_big_card_choice_pic);
  }

  g_magicgame_big_card_choice_pic = (HANDLE)0;
}

// FUNCTION: MAGIC 0x00507cf2
// FUNCTION: SHANDALAR 0x004c69f3
void destroy_MAGICGAME_BigCardCardClass(void)
{
}

// FUNCTION: MAGIC 0x004b1831
// FUNCTION: SHANDALAR 0x004cc0c1
void destroy_MAGIC_CueCardClass(void)
{
  UnregisterClassA(CLASS_MAGIC_CUE_CARD, g_app_instance);
}

// FUNCTION: MAGIC 0x004eb63c
// FUNCTION: SHANDALAR 0x0046933c
void destroy_MAGIC_TellUserClass(void)
{
  UnregisterClassA(CLASS_MAGIC_TELL_USER, g_app_instance);
}

// FUNCTION: MAGIC 0x0042224e
// FUNCTION: SHANDALAR 0x00430994
int destroy_windowclasses(void)
{
  destroy_MAGICGAME_LifeClass();
  destroy_MAGICGAME_FullCardClass();
  destroy_MAGICGAME_ManaSummaryClass();
  destroy_MAGICGAME_HandClass();
  destroy_MAGICGAME_ChatClass();
  destroy_MAGICGAME_CardClass();
  destroy_MAGICGAME_PhaseDisplayClass();
  destroy_MAGICGAME_AttackPhaseDisplayClass();
  destroy_MAGICGAME_TerritoryClass();
  destroy_MAGICGAME_LibraryClass();
  destroy_MAGICGAME_GraveyardClass();
  destroy_MAGICGAME_AttackClass();
  destroy_MAGICGAME_SpellChainClass();
  destroy_MAGICGAME_FaceClass();
  destroy_MAGICGAME_ScrollbarClass();
  destroy_MAGICGAME_BigCardChoiceClass();
  destroy_MAGICGAME_BigCardCardClass();
  destroy_MAGIC_CueCardClass();
  destroy_MAGIC_TellUserClass();
  return 1;
}

// FUNCTION: MAGIC 0x004e2029
// FUNCTION: SHANDALAR 0x004cce39
LRESULT CALLBACK wndproc_MAGICGAME_LifeClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x0055796a
// FUNCTION: SHANDALAR 0x0056dcca
LRESULT CALLBACK wndproc_MAGICGAME_FullCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00539550
// FUNCTION: SHANDALAR 0x00550e30
LRESULT CALLBACK wndproc_MAGICGAME_ManaSummaryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004bbc18
// FUNCTION: SHANDALAR 0x0046ef58
LRESULT CALLBACK wndproc_MAGICGAME_HandClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x0040a4d5
// FUNCTION: SHANDALAR 0x004bac45
LRESULT CALLBACK wndproc_MAGICGAME_ChatClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004cdfcc
// FUNCTION: SHANDALAR 0x005745bc
LRESULT CALLBACK wndproc_MAGICGAME_CardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x0053592b
// FUNCTION: SHANDALAR 0x0055a16b
LRESULT CALLBACK wndproc_MAGICGAME_PhaseDisplayClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00537760
// FUNCTION: SHANDALAR 0x0055bf91
LRESULT CALLBACK wndproc_MAGICGAME_AttackPhaseDisplayClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004e6e19
// FUNCTION: SHANDALAR 0x00504c39
LRESULT CALLBACK wndproc_MAGICGAME_TerritoryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00498193
// FUNCTION: SHANDALAR 0x004c19f3
LRESULT CALLBACK wndproc_MAGICGAME_LibraryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00498cd2
// FUNCTION: SHANDALAR 0x004c252a
LRESULT CALLBACK wndproc_ShuffleCard(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00450cd7
// FUNCTION: SHANDALAR 0x0050d837
LRESULT CALLBACK wndproc_MAGICGAME_GraveyardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004517ac
// FUNCTION: SHANDALAR 0x0050e2fc
LRESULT CALLBACK wndproc_ExpandedGraveyard(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00451926
// FUNCTION: SHANDALAR 0x0050e476
LRESULT CALLBACK wndproc_GraveyardCards(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004d3158
// FUNCTION: SHANDALAR 0x0045cdc8
LRESULT CALLBACK wndproc_MAGICGAME_AttackClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004d79ae
// FUNCTION: SHANDALAR 0x0046160c
LRESULT CALLBACK wndproc_AttackSwordShield(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004d84e0
// FUNCTION: SHANDALAR 0x0046213c
LRESULT CALLBACK wndproc_AttackMinimized(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004864b7
// FUNCTION: SHANDALAR 0x004c9097
LRESULT CALLBACK wndproc_MAGICGAME_SpellChainClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00488f71
// FUNCTION: SHANDALAR 0x004cbb3f
LRESULT CALLBACK wndproc_SpellMinimized(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00463b12
// FUNCTION: SHANDALAR 0x00426252
LRESULT CALLBACK wndproc_MAGICGAME_FaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004909e8
// FUNCTION: SHANDALAR 0x005563b8
LRESULT CALLBACK wndproc_MAGICGAME_ScrollbarClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004e2ff4
// FUNCTION: SHANDALAR 0x00563934
LRESULT CALLBACK wndproc_MAGICTHEME_IconButtonClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00507f42
// FUNCTION: SHANDALAR 0x004c6c40
LRESULT CALLBACK wndproc_MAGICGAME_BigCardChoiceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00507cfd
// FUNCTION: SHANDALAR 0x004c69fe
LRESULT CALLBACK wndproc_MAGICGAME_BigCardCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00464cb4
// FUNCTION: SHANDALAR 0x004543d4
LRESULT CALLBACK wndproc_MAGIC_PaletteClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004b18ca
// FUNCTION: SHANDALAR 0x004cc15a
LRESULT CALLBACK wndproc_MAGIC_CueCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x0043dbe2
// FUNCTION: SHANDALAR 0x00409462
LRESULT CALLBACK wndproc_MAGIC_PlayerDirectiveClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x004eb77a
// FUNCTION: SHANDALAR 0x0046947a
LRESULT CALLBACK wndproc_MAGIC_TellUserClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}
