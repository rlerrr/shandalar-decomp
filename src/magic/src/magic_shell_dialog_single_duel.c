#include "magic_shell_dialogs.h"
#include "magic_shell_single_duel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commdlg.h>
#include "cardartlib/src/palette.h"
#include "deckdll/src/shared_resources.h"
#include "game_support.h"
#include "global_duel_ui_ids.h"
#include "global_other.h"
#include "global_state.h"
#include "global_strings.h"
#include "magic_shell.h"
#include "shared_startup.h"

extern HWND global_main_hwnd;
extern char global_base_directory[];
extern char g_exp1_art_path[];
extern void *g_duel_player_face_pic;
extern char shell_gauntlet_opponent_deck_path[264];
void __stdcall LoadSoloDuelRegistryOptions(void);
void save_solo_duel_options_to_registry(void);
int initialize_duel_engine_window(void);
void shutdown_duel_engine_window(void);
LRESULT shell_populate_deck_choices(HWND hwnd, int first_control, int second_control, int allow_ante);
void shell_open_duel_interface_options(HWND hwnd);

// GLOBAL: MAGIC 0x00637afc
static HFONT shell_single_dialog_font;
// GLOBAL: MAGIC 0x00637ac4
static HFONT shell_single_dialog_low_resolution_font;

// FUNCTION: MAGIC 0x0045abc4
BOOL CALLBACK shell_single_duel_dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  struct

  {
    LOGFONTA resized_font_description;
    HFONT replacement_font;
    int group_index;
    int group_count;
    int group_last[5];
    int control_index;
    int base_heights[5];
    HWND controls[50];
    int group_first[5];
    LOGFONTA *resize_font_description;
    int control_count;
    RECT shell_rect;
    HFONT old_fonts[5];
    HDC erase_dc;
    RECT erase_rect;
    COLORREF button_color;
    DRAWITEMSTRUCT *draw_item;
    HWND color_control;
    int control_id;
    HDC color_dc;
    HBRUSH color_brush;
    HWND previous_focus;
    HWND focus_control;
    UINT notify_id;
    int best_position;
    int best_error;
    LRESULT opponent_selection;
    LRESULT player_selection;
    char autosave_path[264];
    unsigned int save_load_result;
    MSG pending_message;
    int user_cancelled;
    BOOL autosave_queued;
    int resume_result;
    WPARAM selected_deck_index;
    char deck_name[52];
    int random_player;
    int match_result;
    int random_opponent;
    int command_id;
    HWND command_hwnd;
    unsigned int notification;
    HFONT destroy_font;
    HFONT init_font;
    LOGFONTA *init_font_description;
    RECT *initial_rect;
  } s;

  switch (message)
  {
  case WM_INITDIALOG:

    shell_prepare_save_game_dialog(1);
    LoadSoloDuelRegistryOptions();
    s.init_font_description = LoadFontFromIni("ShellDialog", 0);
    shell_single_dialog_font = CreateFontIndirectA(s.init_font_description);
    s.init_font_description = LoadFontFromIni("ShellDialog_lores", 0);
    shell_single_dialog_low_resolution_font = CreateFontIndirectA(s.init_font_description);
    s.init_font = shell_single_dialog_font;
    SendDlgItemMessageA(hwnd, 0x644, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x645, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x642, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x643, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x647, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x705, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x706, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6b5, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x64b, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x64c, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x64d, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x64e, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x650, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x64f, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 1, WM_SETFONT, (WPARAM)s.init_font, 0);
    s.init_font_description = LoadFontFromIni("ShellDialogLabel", 0);
    s.init_font = CreateFontIndirectA(s.init_font_description);
    SendDlgItemMessageA(hwnd, 0x641, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x640, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x646, WM_SETFONT, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x64a, WM_SETFONT, (WPARAM)s.init_font, 0);
    load_active_screen_name_profile();
    SetDlgItemTextA(hwnd, 0x641, g_screen_name_profile.screen_name);
    sprintf((char *)g_duel_state_block_008b3fc0, "%s\\PlayFace\\%s.PIC", global_base_directory,
        g_screen_name_profile.playface_name);
    SendDlgItemMessageA(hwnd, 0x645, CB_RESETCONTENT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x643, CB_RESETCONTENT, 0, 0);
    shell_populate_deck_choices(hwnd, 0x645, 0x643, g_solo_duel_options.ante);
    SendDlgItemMessageA(hwnd, 0x645, CB_SETCURSEL, 0, 0);
    SendDlgItemMessageA(hwnd, 0x643, CB_SETCURSEL, 0, 0);
    load_text(global_ui_strings_filename, "SHELLPAGE_SINGLEDUEL");
    SendDlgItemMessageA(hwnd, 0x645, CB_INSERTSTRING, 0, (LPARAM)g_text_lines[13]);
    SendDlgItemMessageA(hwnd, 0x643, CB_INSERTSTRING, 0, (LPARAM)g_text_lines[13]);
    SendDlgItemMessageA(hwnd, 0x645, CB_SETCURSEL, 0, 0);
    if (g_solo_duel_options.player_random == 0)
    {
      SendDlgItemMessageA(hwnd, 0x645, CB_SELECTSTRING, 0, (LPARAM)g_solo_duel_options.player_deck);
    }
    SendDlgItemMessageA(hwnd, 0x643, CB_SETCURSEL, 0, 0);
    if (g_solo_duel_options.opponent_random == 0)
    {
      SendDlgItemMessageA(hwnd, 0x643, CB_SELECTSTRING, 0, (LPARAM)g_solo_duel_options.opponent_deck);
    }
    if (g_solo_duel_options.ante != 0)
    {
      CheckDlgButton(hwnd, 0x647, 1);
    }
    SendDlgItemMessageA(hwnd, 0x707, 0x465, 0, 0x10005);
    SendDlgItemMessageA(hwnd, 0x707, 0x467, 0, (WORD)g_solo_duel_options.best_of);
    SendDlgItemMessageA(hwnd, 0x706, EM_SETLIMITTEXT, 1, 0);
    if (g_solo_duel_options.allow_sideboarding != 0)
    {
      CheckDlgButton(hwnd, 0x6b5, 1);
    }
    if (g_solo_duel_options.best_of == 1)
    {
      EnableWindow(GetDlgItem(hwnd, 0x6b5), 0);
    }
    g_solo_duel_options.allow_sideboarding = 0;
    ShowWindow(GetDlgItem(hwnd, 0x6b5), 0);
    if (g_solo_duel_options.difficulty == 0)
    {
      CheckDlgButton(hwnd, 0x64b, 1);
    }
    else if (g_solo_duel_options.difficulty == 1)
    {
      CheckDlgButton(hwnd, 0x64c, 1);
    }
    else if (g_solo_duel_options.difficulty == 2)
    {
      CheckDlgButton(hwnd, 0x64d, 1);
    }
    else if (g_solo_duel_options.difficulty == 3)
    {
      CheckDlgButton(hwnd, 0x64e, 1);
    }
    else
    {
      CheckDlgButton(hwnd, 0x64b, 1);
    }
    load_text(global_ui_strings_filename, "SHELLPAGE_SINGLEDUEL");
    SetDlgItemTextA(hwnd, 0x640, g_text_lines[0]);
    SetDlgItemTextA(hwnd, 0x64a, g_text_lines[1]);
    SetDlgItemTextA(hwnd, 0x64b, g_text_lines[2]);
    SetDlgItemTextA(hwnd, 0x64c, g_text_lines[3]);
    SetDlgItemTextA(hwnd, 0x64d, g_text_lines[4]);
    SetDlgItemTextA(hwnd, 0x64e, g_text_lines[5]);
    SetDlgItemTextA(hwnd, 0x646, g_text_lines[6]);
    SetDlgItemTextA(hwnd, 0x647, g_text_lines[7]);
    SetDlgItemTextA(hwnd, 0x705, g_text_lines[8]);
    SetDlgItemTextA(hwnd, 0x6b5, g_text_lines[9]);
    SetDlgItemTextA(hwnd, 0x650, g_text_lines[10]);
    SetDlgItemTextA(hwnd, 0x642, g_text_lines[0xb]);
    SetDlgItemTextA(hwnd, 0x644, g_text_lines[0xc]);
    SetDlgItemTextA(hwnd, 1, g_text_lines[0xe]);
    SetDlgItemTextA(hwnd, 0x64f, g_text_lines[0xf]);
    SetFocus(GetDlgItem(hwnd, 1));
    SendMessageA(hwnd, 0x401, 1, 0);
    change_buttonclass_wndproc(hwnd);
    s.initial_rect = (RECT *)lparam;
    MoveWindow(hwnd, s.initial_rect->left, s.initial_rect->top, s.initial_rect->right - s.initial_rect->left,
        s.initial_rect->bottom - s.initial_rect->top, 1);
    return FALSE;

  case WM_DESTROY:

    s.destroy_font = (HFONT)SendDlgItemMessageA(hwnd, 0x641, WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x641, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x640, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x646, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x64a, WM_SETFONT, 0, 0);
    DeleteObject(s.destroy_font);
    s.destroy_font = (HFONT)SendDlgItemMessageA(hwnd, 0x644, WM_GETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x644, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x645, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x642, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x643, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x647, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x705, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x706, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6b5, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x64b, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x64c, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x64d, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x64e, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x650, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 0x64f, WM_SETFONT, 0, 0);
    SendDlgItemMessageA(hwnd, 1, WM_SETFONT, 0, 0);
    DeleteObject(s.destroy_font);
    return FALSE;

  case WM_COMMAND:
    s.command_id = (unsigned int)wparam & 0xffff;
    s.notification = HIWORD(wparam);
    s.command_hwnd = (HWND)lparam;
    switch (s.command_id)
    {
    case 1:

      g_duel_network_flags = 1;
      GetDlgItemTextA(hwnd, 0x641, g_player_name, 100);
      g_opponent_starting_card_id_1 = -1;
      g_opponent_starting_card_id_2 = g_opponent_starting_card_id_1;
      InitializeDuelUiGlobalIds();
      g_duel_player_face_pic = NULL;
      if (IsDlgButtonChecked(hwnd, 0x64b) != 0)
        g_shandalar_difficulty = 0;
      else if (IsDlgButtonChecked(hwnd, 0x64c) != 0)
        g_shandalar_difficulty = 1;
      else if (IsDlgButtonChecked(hwnd, 0x64d) != 0)
        g_shandalar_difficulty = 2;
      else if (IsDlgButtonChecked(hwnd, 0x64e) != 0)
        g_shandalar_difficulty = 3;
      else
        g_shandalar_difficulty = 0;
      g_duel_state_00789104 = IsDlgButtonChecked(hwnd, 0x647);
      s.random_player = 0;
      s.selected_deck_index = SendDlgItemMessageA(hwnd, 0x645, CB_GETCURSEL, 0, 0);
      if (s.selected_deck_index == 0)
      {
        s.selected_deck_index = SendDlgItemMessageA(hwnd, 0x645, CB_GETCOUNT, 0, 0);
        if ((int)s.selected_deck_index > 1)
        {
          s.selected_deck_index = rand() % (int)(s.selected_deck_index - 1) + 1;
          SendDlgItemMessageA(hwnd, 0x645, CB_SETCURSEL, s.selected_deck_index, 0);
          s.random_player = 1;
        }
        else
        {
          s.selected_deck_index = (WPARAM)-1;
        }
      }
      if (s.selected_deck_index == (WPARAM)-1)
      {
        strcpy(shell_gauntlet_player_deck_path, "");
      }
      else
      {
        SendDlgItemMessageA(hwnd, 0x645, CB_GETLBTEXT, s.selected_deck_index, (LPARAM)s.deck_name);
        *strchr(s.deck_name, '.') = '\0';
        strcpy(g_solo_duel_options.player_deck, s.deck_name);
        save_solo_duel_options_to_registry();
        sprintf(shell_gauntlet_player_deck_path, "%s\\%s.dck", global_playdeck_path, g_solo_duel_options.player_deck);
      }
      s.random_opponent = 0;
      s.selected_deck_index = SendDlgItemMessageA(hwnd, 0x643, CB_GETCURSEL, 0, 0);
      if (s.selected_deck_index == 0)
      {
        s.selected_deck_index = SendDlgItemMessageA(hwnd, 0x643, CB_GETCOUNT, 0, 0);
        if ((int)s.selected_deck_index > 1)
        {
          s.selected_deck_index = rand() % (int)(s.selected_deck_index - 1) + 1;
          SendDlgItemMessageA(hwnd, 0x643, CB_SETCURSEL, s.selected_deck_index, 0);
          s.random_opponent = 1;
        }
        else
        {
          s.selected_deck_index = (WPARAM)-1;
        }
      }
      if (s.selected_deck_index == (WPARAM)-1)
      {
        strcpy(shell_gauntlet_opponent_deck_path, "");
      }
      else
      {
        SendDlgItemMessageA(hwnd, 0x643, CB_GETLBTEXT, s.selected_deck_index, (LPARAM)s.deck_name);
        *strchr(s.deck_name, '.') = '\0';
        strcpy(g_solo_duel_options.opponent_deck, s.deck_name);
        save_solo_duel_options_to_registry();
        sprintf(shell_gauntlet_opponent_deck_path, "%s\\%s.dck", global_playdeck_path, g_solo_duel_options.opponent_deck);
      }
      shell_load_rogue_profiles();
      s.selected_deck_index = rand() % shell_rogue_count;
      while (shell_rogue_profiles[s.selected_deck_index].difficulty != g_shandalar_difficulty)
        s.selected_deck_index = (int)(s.selected_deck_index + 1) % shell_rogue_count;
      sprintf((char *)g_duel_state_block_008ce570, "%s\\Rogues\\%s", g_exp1_art_path,
          shell_rogue_profiles[s.selected_deck_index].face_name);
      strcpy(g_saved_player_name, shell_rogue_profiles[s.selected_deck_index].name);
      shell_enable_animation(0);
      shell_show_gauntlet_matchup(global_main_hwnd, "", g_player_name, shell_gauntlet_player_deck_path,
          g_duel_state_block_008b3fc0, g_saved_player_name, shell_gauntlet_opponent_deck_path,
          g_duel_state_block_008ce570, 0);
      PostMessageA(global_main_hwnd, 0x496, 0, 0);
      if (shell_load_gauntlet_decks(shell_gauntlet_player_deck_path, shell_gauntlet_opponent_deck_path) != 0 &&
          initialize_duel_engine_window() != 0)
      {
        g_duel_run_mode = 1;
        s.match_result = shell_execute_gauntlet_match(0, g_solo_duel_options.best_of,
            g_solo_duel_options.allow_sideboarding, 0, 1, 0, NULL, 0);
        shutdown_duel_engine_window();
      }
      if (s.random_player != 0)
      {
        SendDlgItemMessageA(hwnd, 0x645, CB_SETCURSEL, 0, 0);
      }
      if (s.random_opponent != 0)
      {
        SendDlgItemMessageA(hwnd, 0x643, CB_SETCURSEL, 0, 0);
      }
      SetFocus(GetDlgItem(hwnd, 1));
      SendMessageA(global_main_hwnd, 0x496, 1, 0);
      shell_enable_animation(1);
      break;
    case 0x64f:
      g_duel_network_flags = 1;
      Sleep(GetDoubleClickTime());
      s.autosave_queued = PeekMessageA(&s.pending_message, NULL, 0x203, 0x203, 1);
      shell_prepare_save_game_dialog(1);
      g_duel_save_game_openfilename.hwndOwner = global_main_hwnd;
      load_text(global_ui_strings_filename, "WINDOWTITLES");
      g_duel_save_game_openfilename.lpstrTitle = g_text_lines[6];
      g_duel_save_game_openfilename.Flags = 0x2a100c;
      if (s.autosave_queued != 0)
      {
        strcpy(s.autosave_path, global_savegame_path);
        strcat(s.autosave_path, "\\AUTOSAVE.");
        strcat(s.autosave_path, g_duel_save_game_openfilename.lpstrDefExt);
        strcpy(g_duel_save_game_openfilename.lpstrFile, s.autosave_path);
        InitializeDuelUiGlobalIds();
        s.save_load_result = load_duel_run_mode_1_save(s.autosave_path);
        s.user_cancelled = 0;
      }
      else
      {
        if (GetOpenFileNameA(&g_duel_save_game_openfilename) != 0)
        {
          InitializeDuelUiGlobalIds();
          s.save_load_result = load_duel_run_mode_1_save(g_duel_save_game_openfilename.lpstrFile);
          s.user_cancelled = 0;
        }
        else
        {
          s.save_load_result = 0;
          s.user_cancelled = 1;
        }
      }
      shell_enable_animation(0);
      if (s.save_load_result != 0)
      {
        SendDlgItemMessageA(hwnd, 0x707, 0x467, 0, (WORD)g_solo_duel_options.best_of);
        if (g_solo_duel_options.allow_sideboarding != 0)
        {
          CheckDlgButton(hwnd, 0x6b5, 1);
        }
        if (g_solo_duel_options.best_of == 1)
        {
          EnableWindow(GetDlgItem(hwnd, 0x6b5), 0);
        }
        PostMessageA(global_main_hwnd, 0x496, 0, 0);
        if (initialize_duel_engine_window() != 0)
        {
          g_duel_run_mode = 1;
          s.resume_result = shell_execute_gauntlet_match(1, g_solo_duel_options.best_of,
              g_solo_duel_options.allow_sideboarding, 0, 1, 0, NULL, 0);
          shutdown_duel_engine_window();
        }
        SendMessageA(global_main_hwnd, 0x496, 1, 0);
      }
      else
      {
        if (s.user_cancelled == 0)
        {
          MessageBoxA(hwnd, "Couldn't load the save game; corrupt file or from a different version", "Load saved game", 0);
        }
      }
      shell_enable_animation(1);
      break;
    case 0x650:
      shell_enable_animation(0);
      shell_open_duel_interface_options(global_main_hwnd);
      shell_enable_animation(1);
      SetFocus(GetDlgItem(hwnd, 1));
      break;

    case 0x645:
      if (s.notification == 1)
      {
        s.player_selection = SendMessageA(s.command_hwnd, CB_GETCURSEL, 0, 0);
        g_solo_duel_options.player_random = (int)(s.player_selection == 0);
        save_solo_duel_options_to_registry();
      }
      if (s.notification == 9)
      {
        s.player_selection = SendMessageA(s.command_hwnd, CB_GETCURSEL, 0, 0);
        if (s.player_selection != 0)
        {
          GetDlgItemTextA(hwnd, 0x645, g_solo_duel_options.player_deck, 0x32);
        }
        save_solo_duel_options_to_registry();
      }
      break;
    case 0x643:

      if (s.notification == 1)
      {
        s.opponent_selection = SendMessageA(s.command_hwnd, CB_GETCURSEL, 0, 0);
        g_solo_duel_options.opponent_random = (int)(s.opponent_selection == 0);
        save_solo_duel_options_to_registry();
      }
      if (s.notification == 9)
      {
        s.opponent_selection = SendMessageA(s.command_hwnd, CB_GETCURSEL, 0, 0);
        if (s.opponent_selection != 0)
        {
          GetDlgItemTextA(hwnd, 0x643, g_solo_duel_options.opponent_deck, 0x32);
        }
        save_solo_duel_options_to_registry();
      }
      break;
    case 0x64b:
    case 0x64c:
    case 0x64d:
    case 0x64e:
      if (IsDlgButtonChecked(hwnd, 0x64b) != 0)
        g_solo_duel_options.difficulty = 0;
      else if (IsDlgButtonChecked(hwnd, 0x64c) != 0)
        g_solo_duel_options.difficulty = 1;
      else if (IsDlgButtonChecked(hwnd, 0x64d) != 0)
        g_solo_duel_options.difficulty = 2;
      else if (IsDlgButtonChecked(hwnd, 0x64e) != 0)
        g_solo_duel_options.difficulty = 3;
      save_solo_duel_options_to_registry();
      break;
    case 0x647:
      g_solo_duel_options.ante = IsDlgButtonChecked(hwnd, 0x647);
      save_solo_duel_options_to_registry();
      SendDlgItemMessageA(hwnd, 0x645, CB_RESETCONTENT, 0, 0);
      SendDlgItemMessageA(hwnd, 0x643, CB_RESETCONTENT, 0, 0);
      shell_populate_deck_choices(hwnd, 0x645, 0x643, g_solo_duel_options.ante);
      load_text(global_ui_strings_filename, "SHELLPAGE_SINGLEDUEL");
      SendDlgItemMessageA(hwnd, 0x645, CB_INSERTSTRING, 0, (LPARAM)g_text_lines[13]);
      SendDlgItemMessageA(hwnd, 0x643, CB_INSERTSTRING, 0, (LPARAM)g_text_lines[13]);
      SendDlgItemMessageA(hwnd, 0x645, CB_SETCURSEL, 0, 0);
      SendDlgItemMessageA(hwnd, 0x643, CB_SETCURSEL, 0, 0);
      break;
    case 0x706:
      if (s.notification == 0x200)
      {
        s.best_position = SendDlgItemMessageA(hwnd, 0x707, 0x468, 0, 0);
        s.best_error = HIWORD(s.best_position) != 0;
        if (s.best_error != 0)
        {
          SendDlgItemMessageA(hwnd, 0x707, 0x467, 0, (WORD)g_solo_duel_options.best_of);
        }
        else
        {
          if (s.best_position % 2 == 0)
          {
            if (g_solo_duel_options.best_of < (int)s.best_position)
            {
              ++s.best_position;
            }
            else
            {
              --s.best_position;
            }
            SendDlgItemMessageA(hwnd, 0x707, 0x467, 0, s.best_position & 0xffff);
            s.best_position = SendDlgItemMessageA(hwnd, 0x707, 0x468, 0, 0);
          }
          if (g_solo_duel_options.best_of != s.best_position)
          {
            g_solo_duel_options.best_of = s.best_position;
            save_solo_duel_options_to_registry();
          }
          EnableWindow(GetDlgItem(hwnd, 0x6b5), g_solo_duel_options.best_of > 1);
        }
      }
      break;
    case 0x6b5:
      g_solo_duel_options.allow_sideboarding = IsDlgButtonChecked(hwnd, 0x6b5);
      save_solo_duel_options_to_registry();
      break;
    }
    return TRUE;
  case WM_NOTIFY:

    s.notify_id = (UINT)wparam;
    if (s.notify_id == 0x707)
    {
      PostMessageA(hwnd, 0x111, 0x2000706, (LPARAM)GetDlgItem(hwnd, 0x706));
    }
    return FALSE;

  case 0x4c8:

    s.focus_control = (HWND)wparam;
    s.previous_focus = (HWND)lparam;
    if (s.focus_control == GetDlgItem(hwnd, 1) ||
        s.focus_control == GetDlgItem(hwnd, 0x64f) ||
        s.focus_control == GetDlgItem(hwnd, 0x650))
      SendMessageA(hwnd, 0x401, GetDlgCtrlID(s.focus_control), 0);
    else
      SendMessageA(hwnd, 0x401, 1, 0);
    if (s.focus_control != NULL)
    {
      InvalidateRect(s.focus_control, NULL, 1);
    }
    if (s.previous_focus != NULL)
    {
      InvalidateRect(s.previous_focus, NULL, 1);
    }
    return FALSE;

  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:

    s.color_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.color_dc);
    s.color_control = (HWND)lparam;
    s.control_id = GetDlgCtrlID(s.color_control);
    if (s.control_id == 0x64a || s.control_id == 0x646)

    {
      SetTextColor(s.color_dc, shell_draw_resources.button_text_color);
      SetBkMode(s.color_dc, TRANSPARENT);
      s.color_brush = shell_draw_resources.light_brush;
    }
    else if (s.control_id == 0x706 || s.control_id == 0x645 || s.control_id == 0x643)

    {
      SetTextColor(s.color_dc, shell_draw_resources.background_color);
      SetBkMode(s.color_dc, OPAQUE);
      SetBkColor(s.color_dc, shell_draw_resources.panel_color);
      s.color_brush = shell_draw_resources.panel_brush;
    }
    else

    {
      if (GetFocus() == s.color_control)
        SetTextColor(s.color_dc, shell_draw_resources.selected_button_color);
      else if (s.control_id == 0x641)
        SetTextColor(s.color_dc, shell_draw_resources.selected_button_color);
      else
        SetTextColor(s.color_dc, shell_draw_resources.button_text_color);
      SetBkMode(s.color_dc, TRANSPARENT);
      s.color_brush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    return (BOOL)s.color_brush;

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
      s.button_color = shell_draw_resources.selected_button_color;
    else if (s.draw_item->itemState & ODS_GRAYED)
      s.button_color = 0x10000c6;
    else
      s.button_color = shell_draw_resources.regular_button_color;
    if (shell_draw_resources.button_face != NULL)
      shell_draw_bitmap_button(
        s.draw_item, shell_draw_resources.button_face, shell_draw_resources.light_pen,
            shell_draw_resources.dark_pen, s.button_color, 0);
    else
      draw_owner_draw_button_centered(
        s.draw_item, (HBRUSH)GetStockObject(2), shell_draw_resources.light_pen, shell_draw_resources.dark_pen,
            s.button_color, 0);
    return TRUE;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, message, (int)wparam, lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    MapWindowPoints(hwnd, GetParent(hwnd), (LPPOINT)&s.erase_rect, 2);
    SendMessageA(GetParent(hwnd), 0x498, (WPARAM)s.erase_dc, (LPARAM)&s.erase_rect);
    return TRUE;

  case WM_SIZE:
    s.group_count = 0;
    s.control_count = 0;
    s.group_first[s.group_count] = s.control_count;
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x640);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x641);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x646);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x64a);
    s.group_last[s.group_count] = s.control_count - 1;
    s.old_fonts[s.group_count] =
    (HFONT)SendMessageA(s.controls[s.control_count - 1], WM_GETFONT, 0, 0);
    s.resize_font_description = LoadFontFromIni("ShellDialogLabel", 0);
    s.base_heights[s.group_count] = s.resize_font_description->lfHeight;
    ++s.group_count;
    s.group_first[s.group_count] = s.control_count;
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x644);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x645);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x642);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x643);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x647);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x705);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x706);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x707);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x6b5);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x64b);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x64c);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x64d);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x64e);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x650);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x64f);
    s.controls[s.control_count++] = GetDlgItem(hwnd, 0x1);
    s.group_last[s.group_count] = s.control_count - 1;
    GetClientRect(global_main_hwnd, &s.shell_rect);
    if (s.shell_rect.right < 0x2ee || s.shell_rect.bottom < 0x226)

    {
      s.resize_font_description = LoadFontFromIni("ShellDialog_lores", 0);
      s.old_fonts[s.group_count] =
      shell_single_dialog_low_resolution_font;
    }
    else

    {
      s.resize_font_description = LoadFontFromIni("ShellDialog", 0);
      s.old_fonts[s.group_count] = shell_single_dialog_font;
    }
    s.base_heights[s.group_count] = s.resize_font_description->lfHeight;
    ++s.group_count;
    LockWindowUpdate(hwnd);
    for (s.group_index = 0; s.group_index < s.group_count;
        ++s.group_index)

    {
      GetObjectA(s.old_fonts[s.group_index], sizeof(s.resized_font_description), &s.resized_font_description);
      GetClientRect(global_main_hwnd, &s.shell_rect);
      s.resized_font_description.lfHeight =
      (s.shell_rect.bottom - s.shell_rect.top) /
      s.base_heights[s.group_index];
      if (s.resized_font_description.lfHeight % 2 != 0)
        ++s.resized_font_description.lfHeight;
      s.replacement_font = CreateFontIndirectA(
          &s.resized_font_description);
      if (s.old_fonts[s.group_index] ==
          shell_single_dialog_low_resolution_font)
      shell_single_dialog_low_resolution_font = s.replacement_font;
      else if (s.old_fonts[s.group_index] == shell_single_dialog_font)
        shell_single_dialog_font = s.replacement_font;
      for (s.control_index = s.group_first[s.group_index];
          s.control_index <= s.group_last[s.group_index];
          ++s.control_index)
      SendMessageA(s.controls[s.control_index], WM_SETFONT, (WPARAM)s.replacement_font, 0);
      DeleteObject(s.old_fonts[s.group_index]);
    }
    shell_layout_single_duel_controls(hwnd);
    LockWindowUpdate(NULL);
    return TRUE;

  case 0x499:
    return FALSE;
  default:
    return FALSE;
  }
}
