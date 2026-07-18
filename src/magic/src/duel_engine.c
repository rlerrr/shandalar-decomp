#define DUEL_ENGINE_IMPL

#include <windows.h>
#include <commctrl.h>
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
#include "manalinkinterface/manalinkinterface.h"
extern HWND global_main_hwnd;
#else
#include "shandalar/src/shandalar_global_strings.h"
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
extern int g_graveyard_window_extra_bytes;
extern int g_expanded_graveyard_window_extra_bytes;
extern int g_graveyard_cards_window_extra_bytes;
int load_text_with_tab_escapes(char *filename, char *section_name);
HBITMAP load_pic(char *filename);

// GLOBAL: MAGIC 0x0055e15c
int g_icon_button_icon_long_offset = 0;

// GLOBAL: MAGIC 0x0055e160
int g_icon_button_pressed_long_offset = 4;

// GLOBAL: MAGIC 0x005710cc
char s__WINBK_SpellMin_pic_005710cc[0x14] = "\\WINBK_SpellMin.pic";

// GLOBAL: MAGIC 0x00579c84
char s__WINBK_AttackMin_pic_00579c84[0x15] = "\\WINBK_AttackMin.pic";

// GLOBAL: MAGIC 0x00637ea8
char g_spell_minimized_menu_help_text[0x28];

// GLOBAL: MAGIC 0x00637ec4
HMENU g_spell_minimized_popup_menu;

// GLOBAL: MAGIC 0x00637ee8
HBITMAP g_spell_minimized_background_bitmap;

// GLOBAL: MAGIC 0x00637ef0
char g_spell_minimized_menu_restore_text[0x1c];

// GLOBAL: MAGIC 0x0069c678
char g_attack_minimized_menu_help_text[0x20];

// GLOBAL: MAGIC 0x0069c698
HMENU g_attack_minimized_popup_menu;

// GLOBAL: MAGIC 0x0069c6b8
HBITMAP g_attack_minimized_background_bitmap;

// GLOBAL: MAGIC 0x0069c6c8
char g_attack_minimized_menu_restore_text[0x20];

int register_window_classes(void);
int destroy_windowclasses(void);
int register_MAGICGAME_LifeClass(LPCSTR class_name);
void destroy_MAGICGAME_LifeClass(void);
int register_MAGICGAME_ManaSummaryClass(LPCSTR class_name);
void destroy_MAGICGAME_ManaSummaryClass(void);
int register_MAGICGAME_FullCardClass(LPCSTR class_name);
void destroy_MAGICGAME_FullCardClass(void);
int register_MAGICGAME_HandClass(LPCSTR class_name);
void destroy_MAGICGAME_HandClass(void);
int register_MAGICGAME_CardClass(LPCSTR class_name);
void destroy_MAGICGAME_CardClass(void);
int register_MAGICGAME_PhaseDisplayClass(LPCSTR class_name);
int register_MAGICGAME_LibraryClass(LPCSTR class_name);
int register_MAGICGAME_GraveyardClass(LPCSTR class_name);
int register_MAGICGAME_FaceClass(LPCSTR class_name);
void destroy_MAGICGAME_FaceClass(void);
int register_MAGICGAME_TerritoryClass(LPCSTR class_name);
void destroy_MAGICGAME_TerritoryClass(void);
int register_MAGIC_TellUserClass(LPCSTR class_name);
void destroy_MAGIC_TellUserClass(void);

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
    s.width = global_screen_width;
    s.height = global_screen_height;
  }
  else
  {
    s.width = GetSystemMetrics(0) - 1;
    s.height = GetSystemMetrics(1);
  }

  g_duel_window_hwnd = CreateWindowExA(0, CLASS_MAGICGAME_MAINCLASS, gs_magic_the_gathering_title_00789460,
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
void destroy_MAGICGAME_ChatClass(void)
{
  if (g_magicgame_chat_font != (HFONT)0)
  {
    DeleteObject(g_magicgame_chat_font);
  }

  g_magicgame_chat_font = (HFONT)0;
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

// FUNCTION: MAGIC 0x00498142
// FUNCTION: SHANDALAR 0x004c19a2
void destroy_MAGICGAME_LibraryClass(void)
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
  g_library_submenu = (HMENU)0;;
}

// FUNCTION: MAGIC 0x00450ca9
// FUNCTION: SHANDALAR 0x0050d809
void destroy_MAGICGAME_GraveyardClass(void)
{
  if (g_graveyard_popup_menu != (HMENU)0)
  {
    DestroyMenu(g_graveyard_popup_menu);
  }
  g_graveyard_popup_menu = (HMENU)0;
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

// FUNCTION: MAGIC 0x0040a4d5
// FUNCTION: SHANDALAR 0x004bac45
LRESULT CALLBACK wndproc_MAGICGAME_ChatClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: MAGIC 0x00537760
// FUNCTION: SHANDALAR 0x0055bf91
LRESULT CALLBACK wndproc_MAGICGAME_AttackPhaseDisplayClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
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
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

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
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
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
  struct
  {
    int padding_140;
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
    strcpy((char *)wparam, gs_cuecard_minimized_attack_window_008b4280);
    return 1;

  case WM_CLOSE:
    ShowWindow(hwnd, SW_HIDE);
    ShowWindow(g_duel_attack_phase_window_hwnd, SW_HIDE);
    return 0;

  case WM_COMMAND:
    return SendMessageA(g_duel_attack_phase_window_hwnd, msg, wparam, lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.client_rect);
    IntersectClipRect(s.erase_dc, 0, 0, s.client_rect.right, s.client_rect.bottom);
    if (g_attack_minimized_background_bitmap == (HBITMAP)0)
    {
      strcpy(s.background_path, global_duelart_path);
      strcat(s.background_path, s__WINBK_AttackMin_pic_00579c84);
      g_attack_minimized_background_bitmap = load_pic(s.background_path);
    }
    if (g_attack_minimized_background_bitmap != (HBITMAP)0)
    {
      DrawBitmapToRect(s.erase_dc, &s.client_rect, g_attack_minimized_background_bitmap);
    }
    else
    {
      FillRect(s.erase_dc, &s.client_rect, GetStockObject(LTGRAY_BRUSH));
    }
    return 1;

  case WM_LBUTTONDOWN:
    SendMessageA(g_duel_attack_phase_window_hwnd, WM_COMMAND, 0x66, 0);
    return 0;

  case WM_RBUTTONDOWN:
    s.popup_point.x = (unsigned int)lparam & 0xffff;
    s.popup_point.y = (unsigned int)lparam >> 16;
    ClientToScreen(hwnd, &s.popup_point);
    SetRect(&s.popup_rect, s.popup_point.x, s.popup_point.y,
            s.popup_point.x + 1, s.popup_point.y + 1);
    TrackPopupMenu(g_attack_minimized_popup_menu, TPM_RIGHTBUTTON, s.popup_point.x,
                   s.popup_point.y, 0, hwnd, &s.popup_rect);
    return 0;

  case WM_INITMENU:
    AppendMenuA(g_attack_minimized_popup_menu, 0, 0x66, g_attack_minimized_menu_restore_text);
    AppendMenuA(g_attack_minimized_popup_menu, 0, 100, g_attack_minimized_menu_help_text);
    return 0;

  case WM_MENUSELECT:
    if (HIWORD(wparam) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_attack_minimized_popup_menu);
      while (s.menu_item_count-- != 0)
      {
        DeleteMenu(g_attack_minimized_popup_menu, 0, MF_BYPOSITION);
      }
    }
    return 0;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
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
  struct
  {
    int padding_140;
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
      strcat(s.background_path, s__WINBK_SpellMin_pic_005710cc);
      g_spell_minimized_background_bitmap = load_pic(s.background_path);
    }
    if (g_spell_minimized_background_bitmap != (HBITMAP)0)
    {
      DrawBitmapToRect(s.erase_dc, &s.client_rect, g_spell_minimized_background_bitmap);
    }
    else
    {
      FillRect(s.erase_dc, &s.client_rect, GetStockObject(WHITE_BRUSH));
    }
    return 1;

  case WM_LBUTTONDOWN:
    SendMessageA(g_duel_phase_display_window_hwnd, WM_COMMAND, 0x66, 0);
    return 0;

  case WM_RBUTTONDOWN:
    s.popup_point.x = (unsigned int)lparam & 0xffff;
    s.popup_point.y = (unsigned int)lparam >> 16;
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
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
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
  struct
  {
    int padding_3c;
    int padding_38;
    RECT mouse_up_rect;
    unsigned int mouse_x;
    unsigned int mouse_y;
    RECT mouse_move_rect;
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
    return GetWindowLongA(hwnd, g_icon_button_pressed_long_offset);

  case WM_CREATE:
    s.icon = LoadIconA(g_app_instance, ((CREATESTRUCTA *)lparam)->lpszName);
    SetWindowLongA(hwnd, g_icon_button_icon_long_offset, (LONG)s.icon);
    s.pressed = 0;
    SetWindowLongA(hwnd, g_icon_button_pressed_long_offset, 0);
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
    SetWindowLongA(hwnd, g_icon_button_pressed_long_offset, 1);
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;

  case WM_MOUSEMOVE:
    if (GetCapture() == hwnd)
    {
      s.mouse_x = (unsigned int)lparam & 0xffff;
      s.mouse_y = (unsigned int)lparam >> 16;
      GetClientRect(hwnd, &s.mouse_move_rect);
      s.pressed = PtInRect(&s.mouse_move_rect, *(POINT *)&s.mouse_x);
      SetWindowLongA(hwnd, g_icon_button_pressed_long_offset, s.pressed);
      InvalidateRect(hwnd, NULL, TRUE);
    }
    return 0;

  case WM_LBUTTONUP:
    if (GetCapture() == hwnd)
    {
      s.mouse_x = (unsigned int)lparam & 0xffff;
      s.mouse_y = (unsigned int)lparam >> 16;
      GetClientRect(hwnd, &s.mouse_up_rect);
      s.pressed = PtInRect(&s.mouse_up_rect, *(POINT *)&s.mouse_x);
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
    SendMessageA(GetParent(hwnd), WM_COMMAND, LOWORD(GetDlgCtrlID(hwnd)), (LPARAM)hwnd);
    return 0;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
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
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);
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
