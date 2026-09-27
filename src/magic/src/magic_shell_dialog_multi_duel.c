#include "magic_shell_dialogs.h"
#include "magic_shell_network_match.h"
#include "network.h"
#include <process.h>
#include "manalinkinterface/manalinkinterface.h"
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
int initialize_duel_engine_window(void);
void shell_enable_animation(int enabled);
void shutdown_duel_engine_window(void);
void shell_open_duel_interface_options(HWND hwnd);

extern HANDLE global_mutex_LowerDialog;
extern int g_manalink_is_host;
extern int g_manalink_current_opponent_present;
void shell_load_multiplayer_opponent_profile(void);
void __cdecl shell_multiplayer_idle_worker(void *context);
int shell_send_multiplayer_save(char *filename);

// GLOBAL: MAGIC 0x00637aa4
static HFONT shell_multi_dialog_font;
// GLOBAL: MAGIC 0x00637ac0
static HFONT shell_multi_dialog_low_resolution_font;

// FUNCTION: MAGIC 0x00457296
BOOL CALLBACK shell_multi_duel_dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
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
    int command_id;
    HWND command_hwnd;
    unsigned int notification;
    HGDIOBJ destroy_font;
    HFONT init_font;
    LOGFONTA *init_font_description;
    RECT *initial_rect;
    int retry_index;
  } s;

  switch (message)
  {
  case WM_INITDIALOG:

    *(int *)(gs_phasebar_your_draw_0091b110 + 52) = 1;
    g_multiplayer_duel_ready = 0;
    g_multiplayer_resume_requested = 0;
    g_multiplayer_no_valid_decks = 0;
    g_network_packet_filter_enabled = 0;
    shell_prepare_save_game_dialog(1);
    shell_load_multiplayer_options();
    if (g_shell_multiplayer_options.free_play != 0)
    {
      g_shell_multiplayer_options.best_of = 99;
    }
    s.init_font_description = LoadFontFromIni("ShellDialog", 0);
    shell_multi_dialog_font = CreateFontIndirectA((LOGFONTA *)s.init_font_description);
    s.init_font_description = LoadFontFromIni("ShellDialog_lores", 0);
    shell_multi_dialog_low_resolution_font = CreateFontIndirectA((LOGFONTA *)s.init_font_description);
    s.init_font = shell_multi_dialog_font;
    SendDlgItemMessageA(hwnd, 0x722, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x723, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x725, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x727, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x731, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x732, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x728, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x729, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x72a, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x72b, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x72c, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x730, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x72d, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x72e, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x733, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x734, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x735, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 1, 0x30, (WPARAM)s.init_font, 0);
    s.init_font_description = LoadFontFromIni("ShellDialogLabel", 0);
    s.init_font = CreateFontIndirectA((LOGFONTA *)s.init_font_description);
    SendDlgItemMessageA(hwnd, 0x721, 0x30, (WPARAM)s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x724, 0x30, (WPARAM)s.init_font, 0);
    SetDlgItemTextA(hwnd, 0x721, "Opponent");
    load_active_screen_name_profile();
    sprintf((char *)g_duel_state_block_008b3fc0, "%s\\PlayFace\\%s.PIC", global_base_directory,
        g_screen_name_profile.playface_name);
    load_text("MP_UIStrings.txt", "MULTIDUELSTATUS");
    for (s.retry_index = 0; s.retry_index < 6; s.retry_index = s.retry_index + 1)
    {
      strcpy(g_multiplayer_status_text[s.retry_index], g_text_lines[s.retry_index]);
    }
    load_text("MP_UIStrings.txt", "SHELLPAGE_MULTIDUEL");
    strcpy(g_multiplayer_button_text[0], g_text_lines[0xc]);
    strcpy(g_multiplayer_button_text[1], g_text_lines[0xd]);
    strcpy(g_multiplayer_button_text[2], g_text_lines[0xe]);
    strcpy(g_multiplayer_button_text[3], g_text_lines[0x13]);
    strcpy(g_multiplayer_button_text[4], g_text_lines[0x14]);
    strcpy(g_multiplayer_button_text[5], g_text_lines[0x15]);
    SetDlgItemTextA(hwnd, 0x729, g_text_lines[1]);
    SetDlgItemTextA(hwnd, 0x72b, g_text_lines[2]);
    SetDlgItemTextA(hwnd, 0x72c, g_text_lines[3]);
    SetDlgItemTextA(hwnd, 0x730, g_text_lines[4]);
    SetDlgItemTextA(hwnd, 0x72a, g_text_lines[5]);
    SetDlgItemTextA(hwnd, 0x724, g_text_lines[6]);
    SetDlgItemTextA(hwnd, 0x725, g_text_lines[7]);
    SetDlgItemTextA(hwnd, 0x731, g_text_lines[8]);
    SetDlgItemTextA(hwnd, 0x732, g_text_lines[0xf]);
    SetDlgItemTextA(hwnd, 0x728, g_text_lines[9]);
    SetDlgItemTextA(hwnd, 0x72d, g_text_lines[10]);
    SetDlgItemTextA(hwnd, 0x722, g_text_lines[0xb]);
    if (g_manalink_is_host != 0 || g_manalink_current_opponent_present == 0)
    {
      SetDlgItemTextA(hwnd, 1, g_text_lines[0x13]);
      SetDlgItemTextA(hwnd, 0x72e, g_text_lines[0xe]);
    }
    else
    {
      SetDlgItemTextA(hwnd, 1, g_text_lines[0x14]);
      SetDlgItemTextA(hwnd, 0x72e, g_text_lines[0x15]);
    }
    SetDlgItemTextA(hwnd, 0x733, g_text_lines[0x10]);
    SetDlgItemTextA(hwnd, 0x734, g_text_lines[0x11]);
    SetDlgItemTextA(hwnd, 0x735, g_text_lines[0x12]);
    SendDlgItemMessageA(hwnd, 0x72f, 0x465, 0, 0x10005);
    SendDlgItemMessageA(hwnd, 0x72f, 0x467, 0, (uint)(WORD)g_solo_duel_options.best_of);
    SendDlgItemMessageA(hwnd, 0x727, 0xc5, 1, 0);
    SetFocus(GetDlgItem(hwnd, 1));
    SendMessageA(hwnd, 0x401, 1, 0);
    change_buttonclass_wndproc(hwnd);
    s.initial_rect = (RECT *)lparam;
    MoveWindow(hwnd, s.initial_rect->left, s.initial_rect->top, s.initial_rect->right-s.initial_rect->left,
        s.initial_rect->bottom-s.initial_rect->top, TRUE);
    shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
    ReleaseMutex(global_mutex_LowerDialog);
    return FALSE;

  case WM_DESTROY:

    WaitForSingleObject(global_mutex_LowerDialog, 0xffffffff);
    *(int *)(gs_phasebar_your_draw_0091b110 + 52) = 0;
    s.destroy_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x721, 0x31, 0, 0);
    SendDlgItemMessageA(hwnd, 0x721, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x724, 0x30, 0, 0);
    DeleteObject(s.destroy_font);
    s.destroy_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x722, 0x31, 0, 0);
    SendDlgItemMessageA(hwnd, 0x722, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x723, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x725, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x732, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x731, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x727, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x728, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x729, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x72a, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x72b, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x72c, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x730, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x72d, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x72e, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 1, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x733, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x734, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x735, 0x30, 0, 0);
    DeleteObject(s.destroy_font);
    return FALSE;

  case WM_COMMAND:
    s.command_id = wparam & 0xffff;
    s.notification = HIWORD(wparam);
    s.command_hwnd = (HWND)lparam;
    switch(s.command_id)
    {
    case 0x1:

      if ((g_multiplayer_dialog_state == 0) && (g_manalink_is_host != 0))
      {
        if (g_multiplayer_no_valid_decks == 1)
        {
          LoadTextSectionLines(global_ui_strings_filename, "GAUNTLETERRORS");
          MessageBoxA(hwnd, g_text_lines[1], g_text_lines[0], 0x10);
          break;
        }
        shell_begin_duel_trace("Start of Magic: The Gathering\n");
        g_multiplayer_resume_requested = 0;
        g_multiplayer_dialog_state = 1;
        shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
        g_duel_parameters_network_packet.packet_type = 7;
        g_duel_parameters_network_packet.resume_requested = g_multiplayer_resume_requested;
        g_duel_parameters_network_packet.minimum_deck_size = g_shell_multiplayer_options.minimum_deck_size;
        g_duel_parameters_network_packet.deck_type = g_shell_multiplayer_options.deck_type;
        g_duel_parameters_network_packet.ante = g_shell_multiplayer_options.ante;
        g_duel_parameters_network_packet.free_play = g_shell_multiplayer_options.free_play;
        g_duel_parameters_network_packet.best_of = g_shell_multiplayer_options.best_of;
        g_duel_parameters_network_packet.allow_sideboarding = g_shell_multiplayer_options.allow_sideboarding;
        TENTATIVE_send_network_result(0, 7);
        _beginthread(shell_wait_for_multiplayer_response, 0, hwnd);
      }
      else if ((g_multiplayer_dialog_state == 1) && (g_manalink_is_host == 0))
      {
        if (g_multiplayer_no_valid_decks == 1)
        {
          LoadTextSectionLines(global_ui_strings_filename, "GAUNTLETERRORS");
          MessageBoxA(hwnd, g_text_lines[1], g_text_lines[0], 0x10);
          break;
        }
        g_multiplayer_dialog_state = 2;
        g_network_result_packet.packet_type = 8;
        g_network_result_packet.result = 1;
        TENTATIVE_send_network_result(0, 8);
        shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
        g_network_result_packet.packet_type = 9;
        TENTATIVE_send_network_result(0, 9);
        _beginthread(shell_wait_for_multiplayer_start, 0, hwnd);
      }
      if (g_multiplayer_duel_ready != 0)
      {
        strcpy(g_player_name, g_screen_name_profile.screen_name);
        g_opponent_starting_card_id_2 = g_opponent_starting_card_id_1 = -1;
        InitializeDuelUiGlobalIds();
        g_duel_player_face_pic = NULL;
        g_duel_state_00789104 = IsDlgButtonChecked(hwnd, 0x725);
        s.random_player = 0;
        s.retry_index = 0;
        s.selected_deck_index = 0xffffffff;
        while (s.selected_deck_index == 0xffffffff && s.retry_index < 10)
        {
          s.selected_deck_index = SendDlgItemMessageA(hwnd, 0x723, 0x147, 0, 0);
          if (s.selected_deck_index == 0)
          {
            s.selected_deck_index = SendDlgItemMessageA(hwnd, 0x723, 0x146, 0, 0);
            if ((int)s.selected_deck_index > 1)
            {
              s.selected_deck_index = rand() % (int)(s.selected_deck_index + -1) + 1;
              SendDlgItemMessageA(hwnd, 0x723, 0x14e, s.selected_deck_index, 0);
              s.random_player = 1;
            }
            else
            {
              s.selected_deck_index = 0xffffffff;
            }
          }
          Sleep(500);
          ++s.retry_index;
        }
        if (s.selected_deck_index == 0xffffffff)
        {
          append_to_trace_txt("Could not get a valid deck from the deck list.\n");
          strcpy(shell_gauntlet_player_deck_path, "");
        }
        else
        {
          SendDlgItemMessageA(hwnd, 0x723, 0x148, s.selected_deck_index, (LPARAM)s.deck_name);
          *strchr(s.deck_name, 0x2e) = '\0';
          strcpy(g_shell_multiplayer_options.player_deck, s.deck_name);
          shell_save_multiplayer_options();
          sprintf(shell_gauntlet_player_deck_path, "%s\\%s.dck", global_playdeck_path,
              g_shell_multiplayer_options.player_deck);
          append_to_trace_txt("Using deck ");
          append_to_trace_txt(shell_gauntlet_player_deck_path);
          append_to_trace_txt(" .\n");
        }
        sprintf((char *)g_duel_state_block_008ce570, "Manalink\\Opponent.pic");
        shell_enable_animation(0);
        shell_show_gauntlet_matchup(global_main_hwnd, "", g_player_name, shell_gauntlet_player_deck_path,
            g_duel_state_block_008b3fc0, g_saved_player_name, NULL, g_duel_state_block_008ce570, 0);
        PostMessageA(global_main_hwnd, 0x496, 0, 0);
        g_duel_network_flags = 2;
        OutputDebugStringA("***Dueling***\n");
        if (shell_classify_multiplayer_deck(shell_gauntlet_player_deck_path,
            g_shell_multiplayer_options.deck_type, g_shell_multiplayer_options.ante,
            g_shell_multiplayer_options.minimum_deck_size) == -1)
        {
          FamInterface_EndSession();
          FamInterface_Flush();
          break;
        }

        if ((shell_load_gauntlet_decks(shell_gauntlet_player_deck_path,
            shell_gauntlet_opponent_deck_path) != 0) && (initialize_duel_engine_window() != 0))
        {
          KillTimer(hwnd, 1);
          g_duel_run_mode = 1;
          g_network_packet_filter_enabled = 1;
          FamInterface_SetDuelState(1);
          _beginthread(shell_multiplayer_idle_worker, 0, global_main_hwnd);
          s.match_result = shell_execute_gauntlet_match(0,
              g_shell_multiplayer_options.free_play != 0 ? 99 : g_shell_multiplayer_options.best_of,
              g_shell_multiplayer_options.allow_sideboarding, 0, 1, 0, NULL, 1);
          FamInterface_SetDuelState(0);
          g_network_packet_filter_enabled = 0;
          shutdown_duel_engine_window();
        }
        append_to_trace_txt("End of the duel.\n");
        g_multiplayer_dialog_state = g_multiplayer_duel_ready = g_manalink_is_host = 0;
        shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
        if (g_manalink_current_opponent_present != 0)
        {
          g_manalink_is_host = FamInterface_IsHost();
          if (g_manalink_is_host == 0)
            _beginthread(shell_wait_for_multiplayer_parameters, 0, hwnd);
        }
        shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
        if (s.random_player != 0)
        {
          SendDlgItemMessageA(hwnd, 0x723, 0x14e, 0, 0);
        }
        SetFocus(GetDlgItem(hwnd, 1));
        SendMessageA(global_main_hwnd, 0x496, 1, 0);
        shell_enable_animation(1);

      }

      break;
    case 0x72e:
      if ((g_manalink_is_host == 0) && (g_multiplayer_dialog_state == 1))
      {
        g_network_result_packet.packet_type = 8;
        g_network_result_packet.result = 0;
        TENTATIVE_send_network_result(0, 8);
        g_multiplayer_dialog_state = 0;
        g_multiplayer_resume_requested = 0;
        _beginthread(shell_wait_for_multiplayer_parameters, 0, hwnd);
        shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
      }
      else if ((g_manalink_is_host != 0) && (g_multiplayer_dialog_state == 0))
      {
        g_multiplayer_resume_requested = 1;
        g_multiplayer_dialog_state = 1;
        shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
        shell_begin_duel_trace("Start of Magic: The Gathering\n");
        g_duel_parameters_network_packet.packet_type = 7;
        g_duel_parameters_network_packet.resume_requested = g_multiplayer_resume_requested;
        g_duel_parameters_network_packet.minimum_deck_size = g_shell_multiplayer_options.minimum_deck_size;
        g_duel_parameters_network_packet.deck_type = g_shell_multiplayer_options.deck_type;
        g_duel_parameters_network_packet.ante = g_shell_multiplayer_options.ante;
        g_duel_parameters_network_packet.free_play = g_shell_multiplayer_options.free_play;
        g_duel_parameters_network_packet.best_of = g_shell_multiplayer_options.best_of;
        g_duel_parameters_network_packet.allow_sideboarding = g_shell_multiplayer_options.allow_sideboarding;
        TENTATIVE_send_network_result(0, 7);
        _beginthread(shell_wait_for_multiplayer_response, 0, hwnd);
      }
      else
      {
        if ((g_multiplayer_duel_ready != 0) && (g_multiplayer_dialog_state == 2))
        {
          Sleep(GetDoubleClickTime());
          s.autosave_queued = PeekMessageA(&s.pending_message, NULL, 0x203, 0x203, 1);
          shell_prepare_save_game_dialog(1);
          g_duel_save_game_openfilename.hwndOwner = global_main_hwnd;
          load_text(global_ui_strings_filename, "WINDOWTITLES");
          g_duel_save_game_openfilename.lpstrTitle = g_text_lines[6];
          g_duel_save_game_openfilename.Flags = 0x2a100c;
          if (g_manalink_is_host == 0)
          {
            strcpy(s.autosave_path, "manalink\\mpautosave.");
            strcat(s.autosave_path, g_duel_save_game_openfilename.lpstrDefExt);
            strcpy(g_duel_save_game_openfilename.lpstrFile, s.autosave_path);
            InitializeDuelUiGlobalIds();
            s.save_load_result = load_duel_run_mode_1_save(s.autosave_path);
            s.user_cancelled = 0;
          }
          else if (s.autosave_queued != 0)
          {
            strcpy(s.autosave_path, global_savegame_path);
            strcat(s.autosave_path, "\\AUTOSAVE.");
            strcat(s.autosave_path, g_duel_save_game_openfilename.lpstrDefExt);
            strcpy(g_duel_save_game_openfilename.lpstrFile, s.autosave_path);
            InitializeDuelUiGlobalIds();
            s.save_load_result = load_duel_run_mode_1_save(s.autosave_path);
            s.user_cancelled = 0;
            shell_send_multiplayer_save(s.autosave_path);
          }
          else
          {
            if (GetOpenFileNameA((LPOPENFILENAMEA)&g_duel_save_game_openfilename) != 0)
            {
              InitializeDuelUiGlobalIds();
              s.save_load_result = load_duel_run_mode_1_save(g_duel_save_game_openfilename.lpstrFile);
              s.user_cancelled = 0;
              shell_send_multiplayer_save(g_duel_save_game_openfilename.lpstrFile);
            }
            else
            {
              s.save_load_result = 0;
              s.user_cancelled = 1;
              shell_send_multiplayer_save(s.autosave_path);
            }
          }
          shell_enable_animation(0);
          if (s.save_load_result != 0)
          {
            SendDlgItemMessageA(hwnd, 0x72f, 0x467, 0, (unsigned short)g_shell_multiplayer_options.best_of);
            if (g_shell_multiplayer_options.allow_sideboarding != 0)
            {
              CheckDlgButton(hwnd, 0x728, 1);
            }
            if (g_shell_multiplayer_options.best_of == 1)
            {
              EnableWindow(GetDlgItem(hwnd, 0x728), FALSE);
            }
            PostMessageA(global_main_hwnd, 0x496, 0, 0);
            if (initialize_duel_engine_window() != 0)
            {
              g_duel_run_mode = 1;
              s.resume_result = shell_execute_gauntlet_match(1, g_shell_multiplayer_options.best_of,
                  g_shell_multiplayer_options.allow_sideboarding, 0, 1, 0
                  , NULL, 1);
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
        }
      }
      break;
    case 0x72d:
      shell_enable_animation(0);
      shell_open_duel_interface_options(global_main_hwnd);
      shell_enable_animation(1);
      SetFocus(GetDlgItem(hwnd, 1));
      break;
    case 0x723:

      if (s.notification == 1)
      {
        s.player_selection = SendMessageA(s.command_hwnd, 0x147, 0, 0);
        g_shell_multiplayer_options.player_random = s.player_selection == 0;
        shell_save_multiplayer_options();
      }
      if (s.notification == 9)
      {
        s.player_selection = SendMessageA(s.command_hwnd, 0x147, 0, 0);
        if (s.player_selection != 0)
        {
          GetDlgItemTextA(hwnd, 0x723, g_shell_multiplayer_options.player_deck, 0x32);
        }
        shell_save_multiplayer_options();
      }

      break;
    case 0x729:
    case 0x72a:
    case 0x72b:
    case 0x72c:
    case 0x730:
      if (g_manalink_is_host == 0 || g_multiplayer_dialog_state != 0)
        break;

      if (IsDlgButtonChecked(hwnd, 0x729) != 0)
        g_shell_multiplayer_options.deck_type = 0;
      else if (IsDlgButtonChecked(hwnd, 0x72b) != 0)
        g_shell_multiplayer_options.deck_type = 1;
      else if (IsDlgButtonChecked(hwnd, 0x72c) != 0)
        g_shell_multiplayer_options.deck_type = 2;
      else if (IsDlgButtonChecked(hwnd, 0x730) != 0)
        g_shell_multiplayer_options.deck_type = 3;
      else if (IsDlgButtonChecked(hwnd, 0x72a) != 0)
        g_shell_multiplayer_options.deck_type = 4;
      shell_save_multiplayer_options();
      SendDlgItemMessageA(hwnd, 0x723, 0x14b, 0, 0);
      if (shell_populate_multiplayer_decks(hwnd, 0x723, g_shell_multiplayer_options.deck_type,
          g_shell_multiplayer_options.ante, g_shell_multiplayer_options.minimum_deck_size) < 1)
        g_multiplayer_no_valid_decks = 1;
      else
        g_multiplayer_no_valid_decks = 0;
      SendDlgItemMessageA(hwnd, 0x723, 0x14a, 0, (LPARAM)g_multiplayer_button_text[0]);
      SendDlgItemMessageA(hwnd, 0x723, 0x14e, 0, 0);
      if (g_shell_multiplayer_options.player_random == 0)
      {
        SendDlgItemMessageA(hwnd, 0x723, 0x14d, 0, (LPARAM)g_shell_multiplayer_options.player_deck);
      }

      break;
    case 0x725:
      if (g_manalink_is_host == 0 || g_multiplayer_dialog_state != 0)
        break;

      g_shell_multiplayer_options.ante = IsDlgButtonChecked(hwnd, 0x725);
      shell_save_multiplayer_options();
      SendDlgItemMessageA(hwnd, 0x723, 0x14b, 0, 0);
      if (shell_populate_multiplayer_decks(hwnd, 0x723, g_shell_multiplayer_options.deck_type,
          g_shell_multiplayer_options.ante, g_shell_multiplayer_options.minimum_deck_size) < 1)
        g_multiplayer_no_valid_decks = 1;
      else
        g_multiplayer_no_valid_decks = 0;
      SendDlgItemMessageA(hwnd, 0x723, 0x14a, 0, (LPARAM)g_multiplayer_button_text[0]);
      SendDlgItemMessageA(hwnd, 0x723, 0x14e, 0, 0);
      if (g_shell_multiplayer_options.player_random == 0)
      {
        SendDlgItemMessageA(hwnd, 0x723, 0x14d, 0, (LPARAM)g_shell_multiplayer_options.player_deck);
      }

      break;
    case 0x734:
      if (g_manalink_is_host == 0 || g_multiplayer_dialog_state != 0)
        break;

      g_shell_multiplayer_options.minimum_deck_size = 40;
      shell_save_multiplayer_options();
      SendDlgItemMessageA(hwnd, 0x723, 0x14b, 0, 0);
      if (shell_populate_multiplayer_decks(hwnd, 0x723, g_shell_multiplayer_options.deck_type,
          g_shell_multiplayer_options.ante, g_shell_multiplayer_options.minimum_deck_size) < 1)
        g_multiplayer_no_valid_decks = 1;
      else
        g_multiplayer_no_valid_decks = 0;
      SendDlgItemMessageA(hwnd, 0x723, 0x14a, 0, (LPARAM)g_multiplayer_button_text[0]);
      SendDlgItemMessageA(hwnd, 0x723, 0x14e, 0, 0);
      if (g_shell_multiplayer_options.player_random == 0)
      {
        SendDlgItemMessageA(hwnd, 0x723, 0x14d, 0, (LPARAM)g_shell_multiplayer_options.player_deck);
      }

      break;
    case 0x735:
      if (g_manalink_is_host == 0 || g_multiplayer_dialog_state != 0)
        break;

      g_shell_multiplayer_options.minimum_deck_size = 60;
      shell_save_multiplayer_options();
      SendDlgItemMessageA(hwnd, 0x723, 0x14b, 0, 0);
      if (shell_populate_multiplayer_decks(hwnd, 0x723, g_shell_multiplayer_options.deck_type,
          g_shell_multiplayer_options.ante, g_shell_multiplayer_options.minimum_deck_size) < 1)
        g_multiplayer_no_valid_decks = 1;
      else
        g_multiplayer_no_valid_decks = 0;
      SendDlgItemMessageA(hwnd, 0x723, 0x14a, 0, (LPARAM)g_multiplayer_button_text[0]);
      SendDlgItemMessageA(hwnd, 0x723, 0x14e, 0, 0);
      if (g_shell_multiplayer_options.player_random == 0)
      {
        SendDlgItemMessageA(hwnd, 0x723, 0x14d, 0, (LPARAM)g_shell_multiplayer_options.player_deck);
      }

      break;
    case 0x732:
      if (g_manalink_is_host == 0 || g_multiplayer_dialog_state != 0)
        break;

      g_shell_multiplayer_options.free_play = 1;
      g_shell_multiplayer_options.best_of = 99;
      shell_save_multiplayer_options();

      break;
    case 0x731:
      if (g_manalink_is_host == 0 || g_multiplayer_dialog_state != 0)
        break;
      g_shell_multiplayer_options.free_play = 0;
      shell_save_multiplayer_options();
    case 0x727:
      if (g_manalink_is_host == 0 || g_multiplayer_dialog_state != 0)
        break;
      if (s.notification == 0x200)
      {
        s.best_position = SendDlgItemMessageA(hwnd, 0x72f, 0x468, 0, 0);
        s.best_error = HIWORD(s.best_position) != 0;
        if (s.best_error != 0)
        {
          SendDlgItemMessageA(hwnd, 0x72f, 0x467, 0, (unsigned short)g_shell_multiplayer_options.best_of);
        }
        else
        {
          if (s.best_position == 99)
          {
            g_shell_multiplayer_options.best_of = s.best_position;
            g_shell_multiplayer_options.free_play = 1;
            shell_save_multiplayer_options();
            shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
          }
          else
          {
            if (s.best_position % 2 == 0)
            {
              if ((int)g_shell_multiplayer_options.best_of < (int)s.best_position)
              {
                s.best_position++;
              }
              else
              {
                s.best_position--;
              }
              SendDlgItemMessageA(hwnd, 0x72f, 0x467, 0, s.best_position & 0xffff);
              s.best_position = SendDlgItemMessageA(hwnd, 0x72f, 0x468, 0, 0);
            }
            if (g_shell_multiplayer_options.best_of != s.best_position)
            {
              g_shell_multiplayer_options.best_of = s.best_position;
            }
            g_shell_multiplayer_options.free_play = 0;
            shell_save_multiplayer_options();
            shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
          }
        }
      }
      break;
    case 0x728:
      if (g_manalink_is_host == 0 || g_multiplayer_dialog_state != 0)
        break;

      g_shell_multiplayer_options.allow_sideboarding = IsDlgButtonChecked(hwnd, 0x728);
      shell_save_multiplayer_options();

      break;
    case 0x404:
      load_active_screen_name_profile();
      shell_load_multiplayer_opponent_profile();
      strcpy(g_saved_player_name, g_multiplayer_opponent_profile.screen_name);
      g_multiplayer_dialog_state = 0;
      shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
      InvalidateRect(hwnd, NULL, 1);
      OutputDebugStringA("Opponent registered.\n");
      if (g_manalink_is_host == 0)
      {
        _beginthread(shell_wait_for_multiplayer_parameters, 0, hwnd);
      }
      break;
    case 0x405:
      g_multiplayer_dialog_state = g_manalink_is_host = 0;
      FamInterface_Flush();
      shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
      InvalidateRect(hwnd, NULL, 1);
      OutputDebugStringA("Opponent unregistered.\n");

      break;
    case 0x401:

      g_multiplayer_resume_requested = g_duel_parameters_network_packet.resume_requested;
      g_shell_multiplayer_options.minimum_deck_size = g_duel_parameters_network_packet.minimum_deck_size;
      g_shell_multiplayer_options.deck_type = g_duel_parameters_network_packet.deck_type;
      g_shell_multiplayer_options.ante = g_duel_parameters_network_packet.ante;
      g_shell_multiplayer_options.free_play = g_duel_parameters_network_packet.free_play;
      g_shell_multiplayer_options.best_of = g_duel_parameters_network_packet.best_of;
      g_shell_multiplayer_options.allow_sideboarding = g_duel_parameters_network_packet.allow_sideboarding;
      g_multiplayer_dialog_state = 1;
      shell_apply_multiplayer_options(hwnd);
      shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
      shell_save_multiplayer_options();
      OutputDebugStringA("Got duel Parameters.\n");

      break;

    case 0x403:
      SendMessageA(hwnd, 0x111, 0x72e, 0);
      break;
    case 0x402:
      if (lparam != 0)
      {
        g_network_result_packet.packet_type = 9;
        TENTATIVE_send_network_result(0, 9);
        _beginthread(shell_wait_for_multiplayer_start, 0, hwnd);
        g_multiplayer_dialog_state = 2;
        shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
        OutputDebugStringA("Guest Accepted Parameters.\n");
      }
      else
      {
        g_multiplayer_resume_requested = 0;
        g_multiplayer_dialog_state = 0;
        shell_update_multiplayer_dialog_state(hwnd, g_multiplayer_dialog_state);
        OutputDebugStringA("Guest Declined Parameters.\n");
      }
      break;
    }
    return TRUE;
  case WM_NOTIFY:

    s.notify_id = (UINT)wparam;
    if (s.notify_id == 0x707)
    {
      PostMessageA(hwnd, 0x111, 0x2000727, (LPARAM)GetDlgItem(hwnd, 0x706));
    }
    return FALSE;

  case 0x4c8:

    s.focus_control = (HWND)wparam;
    s.previous_focus = (HWND)lparam;
    if (s.focus_control == GetDlgItem(hwnd, 1) ||
        s.focus_control == GetDlgItem(hwnd, 0x72e) ||
        s.focus_control == GetDlgItem(hwnd, 0x72d))
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
    if (s.control_id == 0x64a || s.control_id == 0x724)
    {
      SetTextColor(s.color_dc, shell_draw_resources.button_text_color);
      SetBkMode(s.color_dc, TRANSPARENT);
      s.color_brush = shell_draw_resources.light_brush;
    }
    else if (s.control_id == 0x727 || s.control_id == 0x723)
    {
      SetTextColor(s.color_dc, shell_draw_resources.background_color);
      SetBkMode(s.color_dc, OPAQUE);
      SetBkColor(s.color_dc, shell_draw_resources.panel_color);
      s.color_brush = shell_draw_resources.panel_brush;
    }
    else if (s.control_id == 0x721)
    {
      SetTextColor(s.color_dc, shell_draw_resources.background_color);
      SetBkMode(s.color_dc, OPAQUE);
      SetBkColor(s.color_dc, shell_draw_resources.dark_color);
      s.color_brush = shell_draw_resources.dark_brush;
    }
    else
    {
      SetBkMode(s.color_dc, TRANSPARENT);
      if (GetFocus() == s.color_control)
        SetTextColor(s.color_dc, shell_draw_resources.selected_button_color);
      else
        SetTextColor(s.color_dc, shell_draw_resources.button_text_color);
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
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x721);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x724);
    s.control_count++;
    s.group_last[s.group_count] = s.control_count - 1;
    s.old_fonts[s.group_count] = (HFONT)SendMessageA((HWND)s.controls[s.control_count - 1], 0x31, 0, 0);
    s.resize_font_description = LoadFontFromIni("ShellDialogLabel", 0);
    s.base_heights[s.group_count] = s.resize_font_description->lfHeight;
    s.group_count++;
    s.group_first[s.group_count] = s.control_count;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x722);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x723);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x725);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x732);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x731);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x727);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x72f);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x728);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x729);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x72b);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x72c);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x730);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x72a);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x72d);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x72e);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x733);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x734);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x735);
    s.control_count++;
    s.controls[s.control_count] = GetDlgItem(hwnd, 1);
    s.control_count++;
    s.group_last[s.group_count] = s.control_count - 1;
    GetClientRect(global_main_hwnd, &s.shell_rect);
    if ((s.shell_rect.right < 0x2ee) || (s.shell_rect.bottom < 0x226))
    {
      s.resize_font_description = LoadFontFromIni("ShellDialog_lores", 0);
      s.old_fonts[s.group_count] = (HFONT)shell_multi_dialog_low_resolution_font;
    }
    else
    {
      s.resize_font_description = LoadFontFromIni("ShellDialog", 0);
      s.old_fonts[s.group_count] = (HFONT)shell_multi_dialog_font;
    }
    s.base_heights[s.group_count] = s.resize_font_description->lfHeight;
    s.group_count++;
    LockWindowUpdate(hwnd);
    for (s.group_index = 0; s.group_index < s.group_count; s.group_index = s.group_index + 1)
    {
      GetObjectA((HANDLE)s.old_fonts[s.group_index], 0x3c, &s.resized_font_description);
      GetClientRect(global_main_hwnd, &s.shell_rect);
      s.resized_font_description.lfHeight = (s.shell_rect.bottom - s.shell_rect.top) / s.base_heights[s.group_index];
      if (s.resized_font_description.lfHeight % 2 != 0)
      {
        s.resized_font_description.lfHeight++;
      }
      s.replacement_font = CreateFontIndirectA(&s.resized_font_description);
      if ((HFONT)s.old_fonts[s.group_index] == shell_multi_dialog_low_resolution_font)
        shell_multi_dialog_low_resolution_font = s.replacement_font;
      else if ((HFONT)s.old_fonts[s.group_index] == shell_multi_dialog_font)
        shell_multi_dialog_font = s.replacement_font;
      for (s.control_index = s.group_first[s.group_index]; s.control_index <= s.group_last[s.group_index];
          s.control_index = s.control_index + 1)
      {
        SendMessageA((HWND)s.controls[s.control_index], 0x30, (WPARAM)s.replacement_font, 0);
      }
      DeleteObject((HGDIOBJ)s.old_fonts[s.group_index]);
    }
    shell_layout_multiplayer_controls(hwnd);
    LockWindowUpdate(NULL);
    return TRUE;
  case 0x499:
    return FALSE;
  default:
    return FALSE;
  }
}
