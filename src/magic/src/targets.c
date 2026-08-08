#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "cardartlib/src/palette.h"
#include "game_support.h"
#include "global_state.h"
#include "global_strings.h"

int GetNextManaSymbol(char **text);

extern int g_spell_minimized_hwnd;

// GLOBAL: MAGIC 0x007a7d74
// GLOBAL: SHANDALAR 0x007beb00
int DAT_007a7d74;

// GLOBAL: MAGIC 0x007aa928
// GLOBAL: SHANDALAR 0x007beb20
int DAT_007aa928;

// GLOBAL: MAGIC 0x007aaeec
// GLOBAL: SHANDALAR 0x007bf0ec
int DAT_007aaeec;

// GLOBAL: MAGIC 0x008a8d78
// GLOBAL: SHANDALAR 0x008bcf78
int DAT_008a8d78;

// GLOBAL: MAGIC 0x008a8dec
// GLOBAL: SHANDALAR 0x008bcfec
int DAT_008a8dec;

// GLOBAL: MAGIC 0x0093d840
// GLOBAL: SHANDALAR 0x00951960
int DAT_0093d840;

int show_spell_window_for_target_selection(void);
int restore_spell_window_after_target_selection(void);

// FUNCTION: MAGIC 0x0055d91b
// FUNCTION: SHANDALAR 0x00573c35
int get_text_width_with_mana_symbols(HDC dc, char *text)
{
  struct
  {
    int char_width;
    int text_width;
    char mana_symbol;
    TEXTMETRICA text_metrics;
    ABC abc_widths;
  } s;

  if (text == NULL)
  {
    return 0;
  }

  GetTextMetricsA(dc, &s.text_metrics);
  s.text_width = 0;
  while (*text != '\0')
  {
    s.mana_symbol = (char)GetNextManaSymbol(&text);
    if (s.mana_symbol != '\0')
    {
      s.text_width += s.text_metrics.tmHeight;
    }
    else
    {
      if (GetCharABCWidthsA(dc, (int)*text, (int)*text, &s.abc_widths))
      {
        s.text_width = s.abc_widths.abcB + s.abc_widths.abcC + s.abc_widths.abcA + s.text_width;
      }
      else
      {
        GetCharWidthA(dc, (int)*text, (int)*text, &s.char_width);
        s.text_width += s.char_width;
      }
      ++text;
    }
  }

  return s.text_width;
}

// FUNCTION: MAGIC 0x0049535a
// FUNCTION: SHANDALAR 0x00465df5
int get_tooltip_text_width(HWND window, char *text)
{
  struct
  {
    HDC dc;
    int text_width;
    HGDIOBJ font;
    char window_text[200];
    TEXTMETRICA text_metrics;
  } s;

  if (window == NULL)
  {
    return 0;
  }

  s.font = (HGDIOBJ)SendMessageA(window, 0x31, 0, 0);
  if (text != NULL)
  {
    strcpy(s.window_text, text);
  }
  else
  {
    GetWindowTextA(window, s.window_text, 200);
  }

  s.dc = GetDC(window);
  ApplyCardArtPaletteToDc(s.dc);
  if (s.font != NULL)
  {
    SelectObject(s.dc, s.font);
  }

  s.text_width = get_text_width_with_mana_symbols(s.dc, s.window_text);
  GetTextMetricsA(s.dc, &s.text_metrics);
  s.text_width += s.text_metrics.tmHeight * 3;
  ReleaseDC(window, s.dc);

  return s.text_width;
}

// FUNCTION: MAGIC 0x004480e4
// FUNCTION: SHANDALAR 0x00451c62
void set_duel_tooltip_text(char *text)
{
  struct
  {
    int tooltip_height;
    POINT cursor_pos;
    int tooltip_width;
    int tooltip_y;
    int tooltip_x;
    int screen_width;
    int screen_height;
    char *tooltip_text;
  } s;

  if (text == NULL)
  {
    s.tooltip_text = "";
  }
  else
  {
    s.tooltip_text = text;
  }

  if (g_duel_interface_options.layout == 0)
  {
    return;
  }

  if (*s.tooltip_text == '\0')
  {
    ShowWindow((HWND)DAT_007a7d74, 0);
    SetWindowTextA((HWND)DAT_007a7d74, s.tooltip_text);
  }
  else
  {
      s.screen_width = GetSystemMetrics(0);
      s.screen_height = GetSystemMetrics(1);
      if (g_duel_interface_options.directive_tracks_mouse != 0)
      {
        if ((GetSystemMetrics(1) * 2) / 100 > 0xc)
        {
          s.tooltip_height = (GetSystemMetrics(1) * 2) / 100;
        }
        else
        {
          s.tooltip_height = 0xc;
        }

        s.tooltip_width = get_tooltip_text_width((HWND)DAT_007a7d74, s.tooltip_text);
        s.tooltip_width += s.tooltip_height * 2;
        GetCursorPos(&s.cursor_pos);
        s.cursor_pos.y += GetSystemMetrics(0xe);
        if (s.screen_width < s.cursor_pos.x + s.tooltip_width)
        {
          s.cursor_pos.x = s.screen_width - s.tooltip_width;
        }
        if (s.screen_height < s.cursor_pos.y + s.tooltip_height)
        {
          s.cursor_pos.y = s.screen_height - s.tooltip_height;
        }
        s.tooltip_x = s.cursor_pos.x;
        s.tooltip_y = s.cursor_pos.y;
      }
      else
      {
        if ((GetSystemMetrics(1) * 3) / 100 > 0x12)
        {
          s.tooltip_height = (GetSystemMetrics(1) * 3) / 100;
        }
        else
        {
          s.tooltip_height = 0x12;
        }

        s.tooltip_width = get_tooltip_text_width((HWND)DAT_007a7d74, s.tooltip_text);
        s.tooltip_width += s.tooltip_height * 2;
        s.tooltip_x = (s.screen_width - (s.screen_width * 0x14) / 100) - s.tooltip_width;
        s.tooltip_y = (s.screen_height - s.tooltip_height) / 2;
      }

      SetWindowPos((HWND)DAT_007a7d74, (HWND)0, s.tooltip_x, s.tooltip_y, s.tooltip_width, s.tooltip_height, 4);
      SetWindowTextA((HWND)DAT_007a7d74, s.tooltip_text);
      ShowWindow((HWND)DAT_007a7d74, 5);
      BringWindowToTop((HWND)DAT_007a7d74);
  }
}

// FUNCTION: MAGIC 0x004466b5
// FUNCTION: SHANDALAR 0x00450233
int run_target_selection_modal(int who_chooses,
                               int player,
                               char *prompt,
                               int allow_cancel,
                               int type_flags,
                               int color_flags,
                               int owner,
                               int zone_flags,
                               int *out_selection_code,
                               int *out_target_player,
                               int allow_ai_player,
                               int allow_human_player)
{
  struct
  {
    POINT cleanup_cursor_pos;              /* ebp - 0x3d4 */
    HWND cleanup_cursor_window;            /* ebp - 0x3cc */
    POINT cursor_pos;                      /* ebp - 0x3c8 */
    HWND cursor_window;                    /* ebp - 0x3c0 */
    char prompt_buffer[200];               /* ebp - 0x3bc */
    char status_text[500];                 /* ebp - 0x2f4 */
    target_selection_result_t action_result; /* ebp - 0x100 */
    int result;                            /* ebp - 0xf0 */
    WPARAM thread_exit_code;               /* ebp - 0xec */
    target_selection_request_t request;    /* ebp - 0xe8 */
  } s;

  s.thread_exit_code = 0;
  if (prompt != NULL)
  {
    strcpy(s.prompt_buffer, prompt);
    strcpy(s.status_text, gs_waiting_for_opponent_007a7d30);
    strcat(s.status_text, prompt);
  }
  else
  {
    strcpy(s.prompt_buffer, "");
    strcpy(s.status_text, gs_waiting_for_opponent_007a7d30);
  }

  EnterCriticalSection(&g_duel_render_lock);
  if (DAT_007abc74 != -1)
  {
    if ((g_duel_network_flags & 2) == 0)
    {
      DAT_007abc74 = -1;
    }

    if (IsWindowVisible((HWND)DAT_008a8dec))
    {
      InvalidateRect((HWND)DAT_008a8dec, NULL, 1);
    }
    else
    {
      InvalidateRect((HWND)DAT_008a8d78, NULL, 1);
    }
  }
  LeaveCriticalSection(&g_duel_render_lock);

  TENTATIVE_reassess_all_cards(0, 0xff);
  GetCursorPos(&s.cursor_pos);
  s.cursor_window = WindowFromPoint(s.cursor_pos);
  SendMessageA(s.cursor_window, 0x20, (WPARAM)s.cursor_window, 0x2000001);

  s.request.player = player;
  s.request.type_flags = type_flags;
  s.request.color_flags = color_flags;
  s.request.owner = owner;
  s.request.zone_flags = zone_flags;
  s.request.allow_ai_player = allow_ai_player;
  s.request.allow_human_player = allow_human_player;
  strcpy(s.request.prompt, s.prompt_buffer);
  s.request.allow_cancel = allow_cancel;

  if (active_player == who_chooses || (g_duel_network_flags & 2) == 0)
  {
    if (DAT_0093d840 != 0)
    {
      play_sound_effect(0x25);
      DAT_0093d840 = 0;
    }

    if (DAT_007aaeec != 0 && current_phase == 10 && response_action_taken == 0)
    {
      s.result = 0;
      s.action_result.selection_code = -2;
      s.action_result.target_card = -2;
      s.action_result.target_player = -1;
      s.thread_exit_code = 0;
      stop_phase_player = -1;
      stop_phase = -1;
      unk_00715fb0 = 0;
      DAT_0072c8e0 = 0;
      DAT_00715fa4 = 0;
    }
    else
    {
      s.result = SendMessageA(g_duel_window_hwnd, 0x403, (WPARAM)&s.request, (LPARAM)&s.action_result);
    }

    if ((g_duel_network_flags & 2) != 0)
    {
      g_target_selection_network_packet.packet_type = 0xc;
      g_target_selection_network_packet.result = s.result;
      g_target_selection_network_packet.selection_code = s.action_result.selection_code;
      g_target_selection_network_packet.thread_exit_code = 1 - s.thread_exit_code;
      if (stop_phase_player == -1)
      {
        g_target_selection_network_packet.previous_player = stop_phase_player;
      }
      else
      {
        g_target_selection_network_packet.previous_player = 1 - stop_phase_player;
      }
      g_target_selection_network_packet.previous_phase = stop_phase;
      g_target_selection_network_packet.target_card = s.action_result.target_card;
      if (s.result != 0)
      {
        g_target_selection_network_packet.target_player = 1 - s.action_result.target_player;
      }
      else
      {
        g_target_selection_network_packet.target_player = s.action_result.target_player;
      }
      g_target_selection_network_packet.aux_player = unk_00715fb0;
      g_target_selection_network_packet.aux_phase = DAT_0072c8e0;
      g_target_selection_network_packet.aux_controller = 1 - DAT_00715fa4;
      TENTATIVE_send_network_result(who_chooses, 0xc);
      Sleep(0xfa);
      send_battlefield_status_packet(who_chooses);
    }
  }
  else
  {
    set_duel_prompt_text(gs_waiting_for_opponent_007a7d30);
    DAT_0093d840 = 1;
    TENTATIVE_wait_for_network_result(who_chooses, 0xc);
    s.result = g_target_selection_network_packet.result;
    s.action_result.selection_code = g_target_selection_network_packet.selection_code;
    previous_stop_phase_player = g_target_selection_network_packet.previous_player;
    previous_stop_phase = g_target_selection_network_packet.previous_phase;
    s.action_result.target_card = g_target_selection_network_packet.target_card;
    s.action_result.target_player = g_target_selection_network_packet.target_player;
    unk_00715fb0 = g_target_selection_network_packet.aux_player;
    DAT_0072c8e0 = g_target_selection_network_packet.aux_phase;
    DAT_00715fa4 = g_target_selection_network_packet.aux_controller;
    s.thread_exit_code = (WPARAM)g_target_selection_network_packet.thread_exit_code;
    receive_battlefield_status_packet(who_chooses);
    set_duel_prompt_text("");
  }

  *out_selection_code = s.action_result.selection_code;
  *out_target_player = s.action_result.target_player;
  out_target_player[1] = s.action_result.target_card;
  set_duel_tooltip_text(NULL);

  if (s.action_result.selection_code == -5)
  {
    PostMessageA(g_duel_window_hwnd, 0x401, s.thread_exit_code, 0);
    ExitThread((DWORD)s.thread_exit_code);
  }

  {
    GetCursorPos(&s.cleanup_cursor_pos);
    s.cleanup_cursor_window = WindowFromPoint(s.cleanup_cursor_pos);
    SendMessageA(s.cleanup_cursor_window, 0x20, (WPARAM)s.cleanup_cursor_window, 0x2000001);
    EnterCriticalSection(&g_duel_render_lock);
    if (g_target_selection_status_code == -2)
    {
      if (stop_phase == -1)
      {
        if ((g_duel_network_flags & 2) != 0)
        {
          DAT_007aa928 = stop_phase_player;
          DAT_007abc74 = stop_phase;
        }
        else
        {
          DAT_007aa928 = DAT_007abc74 = -1;
        }
      }
      else
      {
        DAT_007aa928 = stop_phase_player;
        DAT_007abc74 = stop_phase;
      }
    }
    else if ((g_duel_network_flags & 2) != 0)
    {
      DAT_007aa928 = stop_phase_player;
      DAT_007abc74 = stop_phase;
    }
    else
    {
      DAT_007aa928 = DAT_007abc74 = -1;
    }

    if (IsWindowVisible((HWND)DAT_008a8dec))
    {
      InvalidateRect((HWND)DAT_008a8dec, NULL, 1);
    }
    else
    {
      InvalidateRect((HWND)DAT_008a8d78, NULL, 1);
    }
    LeaveCriticalSection(&g_duel_render_lock);
    return s.result;
  }
}

// FUNCTION: MAGIC 0x004c0080
// FUNCTION: SHANDALAR 0x004c0efa
int C_real_select_target(int who_chooses,
                         int allowed_controller,
                         int preferred_controller,
                         int zone,
                         int required_type,
                         int illegal_type,
                         unsigned int required_abilities,
                         unsigned int illegal_abilities,
                         unsigned int required_color,
                         unsigned int illegal_color,
                         int extra,
                         int required_subtype,
                         int power_requirement,
                         int toughness_requirement,
                         unsigned int special,
                         unsigned int required_state,
                         unsigned int illegal_state,
                         char *prompt,
                         int allow_cancel,
                         target_t *ret_tgt)
{
  struct
  {
    char *selection_prompt;
    int selection_code;
    int selected_player;
    int selected_card;
    int show_validation_message;
    char validation_message[200];
    char target_error[200];
    int retry_selection;
    int selected_internal_card_id;
    int valid_cards[60];
    int valid_count;
    int current_card;
    int current_player;
    int valid_players[60];
    int who_chooses_copy;
    int required_color_copy;
    int normalized_preferred_controller;
    int required_type_copy;
    int normalized_allowed_controller;
    int allow_ai_player;
    int result;
    int window_was_visible;
    int allow_human_player;
  } s;

  if (((who_chooses == 1 && (g_duel_network_flags & 2) == 0) || g_duel_ai_mode_state == 1) || g_duel_network_state != 0)
  {
    if ((ai_search_flags & 1) != 0 && (allowed_controller & 2) != 0)
    {
      preferred_controller = allowed_controller;
    }

    s.who_chooses_copy = who_chooses;
    if ((allowed_controller & 2) != 0)
    {
      s.normalized_allowed_controller = -1;
    }
    else
    {
      s.normalized_allowed_controller = allowed_controller & 1;
    }
    if ((preferred_controller & 2) != 0)
    {
      s.normalized_preferred_controller = -1;
    }
    else
    {
      s.normalized_preferred_controller = preferred_controller & 1;
    }
    s.required_type_copy = required_type;
    s.required_color_copy = required_color;

    if (zone == 0 || (zone & TARGET_ZONE_PLAYERS) != 0)
    {
      if ((preferred_controller & 2) != 0)
      {
        s.allow_human_player = 1;
        s.allow_ai_player = 1;
      }
      else
      {
        if ((preferred_controller & 1) != 0)
        {
          s.allow_human_player = 0;
          s.allow_ai_player = 1;
        }
        else
        {
          s.allow_human_player = 1;
          s.allow_ai_player = 0;
        }
      }
    }
    else
    {
      s.allow_human_player = 0;
      s.allow_ai_player = 0;
    }

    if (spell_fizzled == 1)
    {
      return 0;
    }

    s.valid_count = 0;
    for (s.current_player = 0; s.current_player < 2; ++s.current_player)
    {
      for (s.current_card = 0; s.current_card < active_cards_count[s.current_player]; ++s.current_card)
      {
        if (PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id != -1)
        {
          if (C_real_validate_target(s.current_player,
                                     s.current_card,
                                     NULL,
                                     who_chooses,
                                     allowed_controller,
                                     preferred_controller,
                                     zone,
                                     required_type,
                                     illegal_type,
                                     required_abilities,
                                     illegal_abilities,
                                     required_color,
                                     illegal_color,
                                     extra,
                                     required_subtype,
                                     power_requirement,
                                     toughness_requirement,
                                     special,
                                     required_state,
                                     illegal_state) != 0)
          {
            s.valid_players[s.valid_count] = s.current_player;
            s.valid_cards[s.valid_count] = s.current_card;
            ++s.valid_count;
          }
        }
      }
    }

    if (s.allow_ai_player != 0)
    {
      s.valid_players[s.valid_count] = 1;
      s.valid_cards[s.valid_count] = -1;
      ++s.valid_count;
    }
    if (s.allow_human_player != 0)
    {
      s.valid_players[s.valid_count] = 0;
      s.valid_cards[s.valid_count] = -1;
      ++s.valid_count;
    }

    if (s.valid_count == 0)
    {
      return 0;
    }

    if (g_duel_ai_mode_state == 1)
    {
      ai_recorded_choice = internal_rand(s.valid_count);
      ai_recorded_action = (((s.valid_players[ai_recorded_choice] == 0) ? 0 : 0x100)
                      | (s.valid_cards[ai_recorded_choice] & 0xff))
                     | 0x4000;
      ai_recorded_action_type = 3;
      record_ai_action_selection();
    }
    else
    {
      ai_recorded_action_type = 3;
      replay_ai_action_selection();
      if (ai_recorded_choice == 99 || s.valid_count <= ai_recorded_choice)
      {
        ai_recorded_choice = internal_rand(s.valid_count);
      }
    }

    unk_00742fcc = s.valid_players[ai_recorded_choice];
    ret_tgt->player = s.valid_players[ai_recorded_choice];
    ret_tgt->card = s.valid_cards[ai_recorded_choice];
    s.result = 1;
    return s.result;
  }
  else
  {
    if (zone == 0 || (zone & TARGET_ZONE_PLAYERS) != 0)
    {
      if ((allowed_controller & 2) != 0)
      {
        s.allow_human_player = 1;
        s.allow_ai_player = 1;
      }
      else
      {
        if ((allowed_controller & 1) != 0)
        {
          s.allow_human_player = 0;
          s.allow_ai_player = 1;
        }
        else
        {
          s.allow_human_player = 1;
          s.allow_ai_player = 0;
        }
      }
    }
    else
    {
      s.allow_human_player = 0;
      s.allow_ai_player = 0;
    }

    s.window_was_visible = show_spell_window_for_target_selection();
    s.retry_selection = 1;
    while (s.retry_selection != 0)
    {
      if (prompt != NULL)
      {
        s.selection_prompt = prompt;
      }
      else
      {
        s.selection_prompt = g_ui_message_buffer;
      }

      s.result = run_target_selection_modal(who_chooses,
                                            -1,
                                            s.selection_prompt,
                                            allow_cancel,
                                            -1,
                                            -1,
                                            -1,
                                            -1,
                                            &s.selection_code,
                                            &s.selected_player,
                                            s.allow_ai_player,
                                            s.allow_human_player);
      if (s.result != 0)
      {
        s.selected_internal_card_id = PLAYER_CARD_INSTANCE(s.selected_player, s.selected_card).internal_card_id;
        if (s.selected_internal_card_id >= 0
            && (special & TARGET_SPECIAL_ALLOW_MULTIBLOCKER) == 0
            && global_cards_data[s.selected_internal_card_id].code_pointer == card_two_headed_giant_of_foriys_legacy)
        {
          s.show_validation_message = 1;
          sprintf(s.validation_message, gs_illegal_target_with_reason_008ce7e0, gs_illegal_target_why_type_008a9a00);
          if (g_duel_ai_mode_state != 1)
          {
            set_duel_prompt_text(s.validation_message);
            Sleep(2000);
            set_duel_prompt_text("");
          }
        }
        else
        {
          if (C_real_validate_target(s.selected_player,
                                     s.selected_card,
                                     s.target_error,
                                     who_chooses,
                                     allowed_controller,
                                     preferred_controller,
                                     zone,
                                     required_type,
                                     illegal_type,
                                     required_abilities,
                                     illegal_abilities,
                                     required_color,
                                     illegal_color,
                                     extra,
                                     required_subtype,
                                     power_requirement,
                                     toughness_requirement,
                                     special,
                                     required_state,
                                     illegal_state) != 0)
          {
            s.show_validation_message = 0;
            ret_tgt->player = s.selected_player;
            ret_tgt->card = s.selected_card;
            s.retry_selection = 0;
          }
          else
          {
            s.show_validation_message = 1;
            if (strlen(s.target_error) != 0)
            {
              sprintf(s.validation_message, gs_illegal_target_with_reason_008ce7e0, s.target_error);
            }
            else
            {
              strcpy(s.validation_message, gs_illegal_target_008b3e50);
            }

            if (g_duel_ai_mode_state != 1)
            {
              set_duel_prompt_text(s.validation_message);
              Sleep(2000);
              set_duel_prompt_text("");
            }
          }
        }
      }
      else
      {
        if (s.selection_code == -3)
        {
        }
        else if (s.selection_code == -2)
        {
          if (stop_phase_player == -1 && stop_phase == -1)
          {
            ret_tgt->player = s.selected_player;
            ret_tgt->card = s.selected_card;
            s.retry_selection = 0;
          }
          else if ((zone & TARGET_ZONE_0x2000) != 0)
          {
            ret_tgt->player = -1;
            ret_tgt->card = -3;
            s.retry_selection = 0;
          }
        }
      }
    }

    if (s.window_was_visible == 0)
    {
      restore_spell_window_after_target_selection();
    }
    if (s.result != 0)
    {
      ai_action_replay_available = 0;
    }
    set_duel_prompt_text("");
    strcpy(text_lines[0], "");
  }

  return s.result;
}

// FUNCTION: MAGIC 0x0048930a
// FUNCTION: SHANDALAR 0x004cbed2
int show_spell_window_for_target_selection(void)
{
  int visible;

  visible = IsWindowVisible((HWND)g_spell_minimized_hwnd);
  if (!visible && IsWindowVisible((HWND)g_duel_phase_display_window_hwnd))
  {
    SendMessageA((HWND)g_duel_phase_display_window_hwnd, 0x111, 0x65, 0);
  }

  return visible;
}

// FUNCTION: MAGIC 0x00489362
// FUNCTION: SHANDALAR 0x004cbf2a
int restore_spell_window_after_target_selection(void)
{
  int hidden;

  hidden = !IsWindowVisible((HWND)g_spell_minimized_hwnd);
  if (!hidden && !IsWindowVisible((HWND)g_duel_phase_display_window_hwnd))
  {
    SendMessageA((HWND)g_duel_phase_display_window_hwnd, 0x111, 0x66, 0);
  }

  return hidden;
}
