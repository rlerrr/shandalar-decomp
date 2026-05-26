#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "cardartlib/src/palette.h"
#include "game_support.h"
#include "global_strings.h"

int GetNextManaSymbol(char **param_1);

// GLOBAL: MAGIC 0x00637e58
int DAT_00637e58;

// GLOBAL: MAGIC 0x007a7d74
int DAT_007a7d74;

// GLOBAL: MAGIC 0x007aa928
int DAT_007aa928;

// GLOBAL: MAGIC 0x007aaeec
int DAT_007aaeec;

// GLOBAL: MAGIC 0x008a8d78
int DAT_008a8d78;

// GLOBAL: MAGIC 0x008a8dec
int DAT_008a8dec;

// GLOBAL: MAGIC 0x0091c0f0
int DAT_0091c0f0;

// GLOBAL: MAGIC 0x0091c974
int DAT_0091c974;

// GLOBAL: MAGIC 0x0091c998
int DAT_0091c998;

// GLOBAL: MAGIC 0x0093d840
int DAT_0093d840;

int FUN_0048930a(void);
int FUN_00489362(void);

typedef struct
{
  int arg_2;
  int arg_5;
  int arg_6;
  int arg_7;
  int arg_8;
  int allow_cancel;
  char prompt[200];
  int allow_ai_player;
  int allow_human_player;
} target_selection_request_t;

typedef struct
{
  char packet_type;
  char pad_1[3];
  int result;
  int selection_code;
  int previous_player;
  int previous_phase;
  int target_card;
  int target_player;
  int aux_player;
  int aux_phase;
  int aux_controller;
  char thread_exit_code;
} target_selection_network_packet_t;

// FUNCTION: MAGIC 0x0055d91b
int FUN_0055d91b(HDC dc, char *text)
{
  int symbol;
  BOOL has_abc_widths;
  int char_width;
  int text_width;
  char mana_symbol;
  TEXTMETRICA text_metrics;
  ABC abc_widths;

  if (text == NULL)
  {
    text_width = 0;
  }
  else
  {
    GetTextMetricsA(dc, &text_metrics);
    text_width = 0;
    while (*text != '\0')
    {
      symbol = GetNextManaSymbol(&text);
      mana_symbol = (char)symbol;
      if (mana_symbol == '\0')
      {
        has_abc_widths = GetCharABCWidthsA(dc, (int)*text, (int)*text, &abc_widths);
        if (!has_abc_widths)
        {
          GetCharWidthA(dc, (int)*text, (int)*text, &char_width);
          text_width += char_width;
        }
        else
        {
          text_width += abc_widths.abcA + abc_widths.abcB + abc_widths.abcC;
        }
        ++text;
      }
      else
      {
        text_width += text_metrics.tmHeight;
      }
    }
  }

  return text_width;
}

// FUNCTION: MAGIC 0x0049535a
int FUN_0049535a(HWND window, char *text)
{
  int text_width;
  HGDIOBJ font;
  HDC dc;
  char window_text[200];
  TEXTMETRICA text_metrics;

  if (window == NULL)
  {
    text_width = 0;
  }
  else
  {
    font = (HGDIOBJ)SendMessageA(window, 0x31, 0, 0);
    if (text == NULL)
    {
      GetWindowTextA(window, window_text, 200);
    }
    else
    {
      strcpy(window_text, text);
    }

    dc = GetDC(window);
    ApplyCardArtPaletteToDc(dc);
    if (font != NULL)
    {
      SelectObject(dc, font);
    }

    text_width = FUN_0055d91b(dc, window_text);
    GetTextMetricsA(dc, &text_metrics);
    text_width += text_metrics.tmHeight * 3;
    ReleaseDC(window, dc);
  }

  return text_width;
}

// FUNCTION: MAGIC 0x004480e4
void FUN_004480e4(char *text)
{
  int screen_width;
  int screen_height;
  int tooltip_height;
  int tooltip_width;
  int tooltip_x;
  int tooltip_y;
  int cursor_height;
  POINT cursor_pos;
  char *tooltip_text;

  if (text == NULL)
  {
    tooltip_text = "";
  }
  else
  {
    tooltip_text = text;
  }

  if (DAT_0091c998 != 0)
  {
    if (*tooltip_text == '\0')
    {
      ShowWindow((HWND)DAT_007a7d74, 0);
      SetWindowTextA((HWND)DAT_007a7d74, tooltip_text);
    }
    else
    {
      screen_width = GetSystemMetrics(0);
      screen_height = GetSystemMetrics(1);
      if (DAT_0091c974 == 0)
      {
        tooltip_height = (GetSystemMetrics(1) * 3) / 100;
        if (tooltip_height < 0x13)
        {
          tooltip_height = 0x12;
        }

        tooltip_width = FUN_0049535a((HWND)DAT_007a7d74, tooltip_text);
        tooltip_width += tooltip_height * 2;
        tooltip_x = (screen_width - (screen_width * 0x14) / 100) - tooltip_width;
        tooltip_y = (screen_height - tooltip_height) / 2;
      }
      else
      {
        tooltip_height = (GetSystemMetrics(1) * 2) / 100;
        if (tooltip_height < 0xd)
        {
          tooltip_height = 0xc;
        }

        tooltip_width = FUN_0049535a((HWND)DAT_007a7d74, tooltip_text);
        tooltip_width += tooltip_height * 2;
        GetCursorPos(&cursor_pos);
        cursor_height = GetSystemMetrics(0xe);
        cursor_pos.y += cursor_height;
        if (screen_width < cursor_pos.x + tooltip_width)
        {
          cursor_pos.x = screen_width - tooltip_width;
        }
        if (screen_height < cursor_pos.y + tooltip_height)
        {
          cursor_pos.y = screen_height - tooltip_height;
        }
        tooltip_x = cursor_pos.x;
        tooltip_y = cursor_pos.y;
      }

      SetWindowPos((HWND)DAT_007a7d74, (HWND)0, tooltip_x, tooltip_y, tooltip_width, tooltip_height, 4);
      SetWindowTextA((HWND)DAT_007a7d74, tooltip_text);
      ShowWindow((HWND)DAT_007a7d74, 5);
      BringWindowToTop((HWND)DAT_007a7d74);
    }
  }
}

// FUNCTION: MAGIC 0x004466b5
int FUN_004466b5(int who_chooses,
                 int arg_2,
                 char *prompt,
                 int allow_cancel,
                 int arg_5,
                 int arg_6,
                 int arg_7,
                 int arg_8,
                 int *out_selection_code,
                 int *out_target_player,
                 int allow_ai_player,
                 int allow_human_player)
{
  BOOL bigcard_visible;
  POINT cursor_pos;
  HWND cursor_window;
  char prompt_buffer[200];
  char status_text[500];
  int selection_code;
  int target_player;
  int target_card;
  int result;
  WPARAM thread_exit_code;
  target_selection_request_t request;
  target_selection_network_packet_t *packet;

  thread_exit_code = 0;
  if (prompt == NULL)
  {
    strcpy(prompt_buffer, "");
    strcpy(status_text, "");
  }
  else
  {
    strcpy(prompt_buffer, prompt);
    strcpy(status_text, "");
    strcat(status_text, prompt);
  }

  EnterCriticalSection(&unk_00789110);
  if (DAT_007abc74 != -1)
  {
    if ((unk_00926804 & 2) == 0)
    {
      DAT_007abc74 = -1;
    }

    bigcard_visible = IsWindowVisible((HWND)DAT_008a8dec);
    if (!bigcard_visible)
    {
      InvalidateRect((HWND)DAT_008a8d78, NULL, 1);
    }
    else
    {
      InvalidateRect((HWND)DAT_008a8dec, NULL, 1);
    }
  }
  LeaveCriticalSection(&unk_00789110);

  TENTATIVE_reassess_all_cards();
  GetCursorPos(&cursor_pos);
  cursor_window = WindowFromPoint(cursor_pos);
  SendMessageA(cursor_window, 0x20, (WPARAM)cursor_window, 0x2000001);

  request.arg_2 = arg_2;
  request.arg_5 = arg_5;
  request.arg_6 = arg_6;
  request.arg_7 = arg_7;
  request.arg_8 = arg_8;
  request.allow_cancel = allow_cancel;
  strcpy(request.prompt, prompt_buffer);
  request.allow_ai_player = allow_ai_player;
  request.allow_human_player = allow_human_player;

  if (who_chooses == unk_008b35ec || (unk_00926804 & 2) == 0)
  {
    if (DAT_0093d840 != 0)
    {
      play_sound_effect(0x25);
      DAT_0093d840 = 0;
    }

    if (DAT_007aaeec == 0 || current_phase != 10 || *(int *)&unk_009266d0[0x7c] != 0)
    {
      result = SendMessageA(unk_008cf1b4, 0x403, (WPARAM)&request, (LPARAM)&selection_code);
      target_player = out_target_player[0];
      target_card = out_target_player[1];
    }
    else
    {
      result = 0;
      selection_code = -2;
      target_card = -2;
      target_player = -1;
      thread_exit_code = 0;
      unk_00716244 = -1;
      unk_00716248 = -1;
      unk_00715fb0 = 0;
      DAT_0072c8e0 = 0;
      DAT_00715fa4 = 0;
    }

    if ((unk_00926804 & 2) != 0)
    {
      packet = (target_selection_network_packet_t *)&unk_008cf200;
      packet->packet_type = 0xc;
      packet->result = result;
      packet->selection_code = selection_code;
      if (unk_00716244 == -1)
      {
        packet->previous_player = unk_00716244;
      }
      else
      {
        packet->previous_player = 1 - unk_00716244;
      }
      packet->previous_phase = unk_00716248;
      packet->target_card = target_card;
      if (result == 0)
      {
        packet->target_player = target_player;
      }
      else
      {
        packet->target_player = 1 - target_player;
      }
      packet->aux_player = unk_00715fb0;
      packet->aux_phase = DAT_0072c8e0;
      packet->aux_controller = 1 - DAT_00715fa4;
      packet->thread_exit_code = 1 - (char)thread_exit_code;
      TENTATIVE_send_network_result(who_chooses, 0xc);
      Sleep(0xfa);
      FUN_00501d78(who_chooses);
    }
  }
  else
  {
    FUN_004a61d6("");
    DAT_0093d840 = 1;
    TENTATIVE_wait_for_network_result(who_chooses, 0xc);
    packet = (target_selection_network_packet_t *)&unk_008cf200;
    result = packet->result;
    selection_code = packet->selection_code;
    _DAT_00743034 = packet->previous_player;
    unk_007161d4 = packet->previous_phase;
    target_card = packet->target_card;
    target_player = packet->target_player;
    unk_00715fb0 = packet->aux_player;
    DAT_0072c8e0 = packet->aux_phase;
    DAT_00715fa4 = packet->aux_controller;
    thread_exit_code = (WPARAM)packet->thread_exit_code;
    FUN_00501deb(who_chooses);
    FUN_004a61d6("");
  }

  *out_selection_code = selection_code;
  *out_target_player = target_player;
  out_target_player[1] = target_card;
  FUN_004480e4(NULL);

  if (selection_code != -5)
  {
    GetCursorPos(&cursor_pos);
    cursor_window = WindowFromPoint(cursor_pos);
    SendMessageA(cursor_window, 0x20, (WPARAM)cursor_window, 0x2000001);
    EnterCriticalSection(&unk_00789110);
    if (_DAT_0074303c == -2)
    {
      if (unk_00716248 == -1)
      {
        if ((unk_00926804 & 2) == 0)
        {
          DAT_007abc74 = -1;
          DAT_007aa928 = -1;
        }
        else
        {
          DAT_007aa928 = unk_00716244;
          DAT_007abc74 = unk_00716248;
        }
      }
      else
      {
        DAT_007aa928 = unk_00716244;
        DAT_007abc74 = unk_00716248;
      }
    }
    else if ((unk_00926804 & 2) == 0)
    {
      DAT_007abc74 = -1;
      DAT_007aa928 = -1;
    }
    else
    {
      DAT_007aa928 = unk_00716244;
      DAT_007abc74 = unk_00716248;
    }

    bigcard_visible = IsWindowVisible((HWND)DAT_008a8dec);
    if (!bigcard_visible)
    {
      InvalidateRect((HWND)DAT_008a8d78, NULL, 1);
    }
    else
    {
      InvalidateRect((HWND)DAT_008a8dec, NULL, 1);
    }
    LeaveCriticalSection(&unk_00789110);
    return result;
  }

  PostMessageA(unk_008cf1b4, 0x401, thread_exit_code, 0);
  ExitThread((DWORD)thread_exit_code);
  return 0;
}

// FUNCTION: MAGIC 0x004c0080
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
    int selection_code;
    int selected_player;
    int selected_card;
    int show_validation_message;
    int retry_selection;
    char validation_message[32];
    char target_error[200];
    int valid_cards[60];
    int valid_count;
    int current_card;
    int current_player;
    int valid_players[60];
    int allow_human_player;
    int window_was_visible;
    int result;
    int allow_ai_player;
  } s;
  unsigned int is_valid_target;
  char *selection_prompt;
  int selected_internal_card_id;

  if (((who_chooses == 1 && (unk_00926804 & 2) == 0) || unk_008a9000 == 1) || unk_009252e0 != 0)
  {
    if ((DAT_00777854 & 1) != 0 && (allowed_controller & 2) != 0)
    {
      preferred_controller = allowed_controller;
    }

    if (zone == 0 || (zone & TARGET_ZONE_PLAYERS) != 0)
    {
      if ((preferred_controller & 2) == 0)
      {
        if ((preferred_controller & 1) == 0)
        {
          s.allow_human_player = 1;
          s.allow_ai_player = 0;
        }
        else
        {
          s.allow_human_player = 0;
          s.allow_ai_player = 1;
        }
      }
      else
      {
        s.allow_human_player = 1;
        s.allow_ai_player = 1;
      }
    }
    else
    {
      s.allow_human_player = 0;
      s.allow_ai_player = 0;
    }

    if (spell_fizzled == 1)
    {
      s.result = 0;
    }
    else
    {
      s.valid_count = 0;
      for (s.current_player = 0; s.current_player < 2; ++s.current_player)
      {
        for (s.current_card = 0; s.current_card < active_cards_count[s.current_player]; ++s.current_card)
        {
          if (PLAYER_CARD_INSTANCE(s.current_player, s.current_card).internal_card_id != -1)
          {
            is_valid_target = C_real_validate_target(s.current_player,
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
                                                     illegal_state);
            if (is_valid_target != 0)
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
        s.result = 0;
      }
      else
      {
        if (unk_008a9000 == 1)
        {
          unk_00939340 = internal_rand(s.valid_count);
          unk_00925bb8 = (((s.valid_players[unk_00939340] == 0) ? 0 : 0x100)
                          | (s.valid_cards[unk_00939340] & 0xff))
                         | 0x4000;
          unk_0057aae8 = 3;
          FUN_004e4f11();
        }
        else
        {
          unk_0057aae8 = 3;
          FUN_004e5089();
          if (unk_00939340 == 99 || s.valid_count <= unk_00939340)
          {
            unk_00939340 = internal_rand(s.valid_count);
          }
        }

        unk_00742fcc = s.valid_players[unk_00939340];
        ret_tgt->player = s.valid_players[unk_00939340];
        ret_tgt->card = s.valid_cards[unk_00939340];
        s.result = 1;
      }
    }
  }
  else
  {
    if (zone == 0 || (zone & TARGET_ZONE_PLAYERS) != 0)
    {
      if ((allowed_controller & 2) == 0)
      {
        if ((allowed_controller & 1) == 0)
        {
          s.allow_human_player = 1;
          s.allow_ai_player = 0;
        }
        else
        {
          s.allow_human_player = 0;
          s.allow_ai_player = 1;
        }
      }
      else
      {
        s.allow_human_player = 1;
        s.allow_ai_player = 1;
      }
    }
    else
    {
      s.allow_human_player = 0;
      s.allow_ai_player = 0;
    }

    s.window_was_visible = FUN_0048930a();
    s.retry_selection = 1;
    while (s.retry_selection != 0)
    {
      if (prompt == NULL)
      {
        selection_prompt = unk_00748770;
      }
      else
      {
        selection_prompt = prompt;
      }

      s.result = FUN_004466b5(who_chooses,
                              -1,
                              selection_prompt,
                              allow_cancel,
                              -1,
                              -1,
                              -1,
                              -1,
                              &s.selection_code,
                              &s.selected_player,
                              s.allow_ai_player,
                              s.allow_human_player);
      if (s.result == 0)
      {
        if (s.selection_code != -3 && s.selection_code == -2)
        {
          if (unk_00716244 == -1 && unk_00716248 == -1)
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
      else
      {
        selected_internal_card_id = PLAYER_CARD_INSTANCE(s.selected_player, s.selected_card).internal_card_id;
        if (selected_internal_card_id < 0
            || (special & TARGET_SPECIAL_ALLOW_MULTIBLOCKER) != 0
            || global_cards_data[selected_internal_card_id].code_pointer != FUN_00481e25)
        {
          is_valid_target = C_real_validate_target(s.selected_player,
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
                                                   illegal_state);
          if (is_valid_target == 0)
          {
            s.show_validation_message = 1;
            if (strlen(s.target_error) == 0)
            {
              strcpy(s.validation_message, "");
            }
            else
            {
              sprintf(s.validation_message, "%s", s.target_error);
            }

            if (unk_008a9000 != 1)
            {
              FUN_004a61d6(s.validation_message);
              Sleep(2000);
              FUN_004a61d6("");
            }
          }
          else
          {
            s.show_validation_message = 0;
            ret_tgt->player = s.selected_player;
            ret_tgt->card = s.selected_card;
            s.retry_selection = 0;
          }
        }
        else
        {
          s.show_validation_message = 1;
          sprintf(s.validation_message, "%s", "");
          if (unk_008a9000 != 1)
          {
            FUN_004a61d6(s.validation_message);
            Sleep(2000);
            FUN_004a61d6("");
          }
        }
      }
    }

    if (s.window_was_visible == 0)
    {
      FUN_00489362();
    }
    if (s.result != 0)
    {
      DAT_0072c8e4 = 0;
    }
    FUN_004a61d6("");
    strcpy(text_lines[0], "");
  }

  return s.result;
}

// FUNCTION: MAGIC 0x0048930a
int FUN_0048930a(void)
{
  int visible;

  visible = IsWindowVisible((HWND)DAT_00637e58);
  if (!visible && IsWindowVisible((HWND)DAT_0091c0f0))
  {
    SendMessageA((HWND)DAT_0091c0f0, 0x111, 0x65, 0);
  }

  return visible;
}

// FUNCTION: MAGIC 0x00489362
int FUN_00489362(void)
{
  int hidden;

  hidden = !IsWindowVisible((HWND)DAT_00637e58);
  if (!hidden && !IsWindowVisible((HWND)DAT_0091c0f0))
  {
    SendMessageA((HWND)DAT_0091c0f0, 0x111, 0x66, 0);
  }

  return hidden;
}
