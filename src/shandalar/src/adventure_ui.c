#include <windows.h>
#include <mmsystem.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>
#include <errno.h>

#include "defs.h"
#include "shandalar.h"
#include "shandalar_internal.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/global_other.h"
#include "magic/src/game_support.h"
#include "magic/src/duel_engine.h"
#include "magic/src/shared_startup.h"
#include "shandalar_global_strings.h"
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/card_db.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

// FUNCTION: SHANDALAR 0x005501fe
int ScaleUiCoordinate(int value)
{
  return (value * global_screen_width) / 0x280;
}

// FUNCTION: SHANDALAR 0x00412bff
int RunTextMenuAt(char *menu_text, int left_x, int top_y)
{
  g_text_menu_left = left_x;
  g_text_menu_top = top_y;
  g_text_menu_initial_selection = -1;
  return RunTextMenuCore(menu_text, 1);
}

// FUNCTION: SHANDALAR 0x00412bae
int RunTextMenuAtScaled(char *menu_text, int x_320_scale, unsigned int y_200_scale)
{
  return RunTextMenuAt(menu_text, (global_screen_width * x_320_scale) / 0x140,
                       (int)((y_200_scale - (y_200_scale & 1)) * global_screen_height) / 200 + (y_200_scale & 1));
}

// FUNCTION: SHANDALAR 0x00412c37
int RunTextMenuCore(char *menu_text, int clear_input_before_show)
{
  struct
  {
    int selected_menu_entry;
    int key_code;
    int current_menu_entry;
    int next_menu_entry;
    int done;
    int line_height_copy;
    int previous_menu_entry;
    char timer_text[4];
    int line_height;
    int timer_seconds_left;
    int mouse_active;
    int last_timer_seconds;
  } s;

  if ((clear_input_before_show != 0) && (g_text_menu_abort_requested == 0))
  {
    ClearInputAndWaitForMouseRelease();
  }

  s.previous_menu_entry = -1;
  s.selected_menu_entry = s.previous_menu_entry;
  g_text_menu_mouse_active = 0;
  g_text_menu_mouse_released = g_text_menu_mouse_active;
  if (g_legacy_mouse_active == 0)
  {
    if (s.selected_menu_entry == -1)
    {
      if (g_text_menu_ok_mode == 0)
      {
        if (g_text_menu_force_cancel == 0)
        {
          s.selected_menu_entry = 0;
        }
      }
    }
  }
  g_text_menu_finish_flash = 0;
  s.selected_menu_entry = g_text_menu_finish_flash;
  if (g_text_menu_initial_selection != -1)
  {
    s.selected_menu_entry = g_text_menu_initial_selection;
  }

  g_text_menu_first_option_line = -1;
  g_text_menu_needs_layout = 1;
  g_text_menu_hovered_selection = -1;
  g_text_menu_line_height = GetFontLineHeight(g_page0_window_bounds->font_slot);
  s.current_menu_entry = 3;
  s.line_height = 0x16;
  g_text_menu_color_base = 0xff;
  if (g_shandalar_state_00789934 == 0)
  {
    s.line_height = -1;
  }
  s.line_height_copy = g_text_menu_line_height;

  s.mouse_active = 0;
  s.done = s.mouse_active;
  HideMouseCursorNested();
  DrawTextMenu(menu_text, g_text_menu_initial_selection);
  ShowMouseCursorNested();

  if (g_text_menu_abort_requested != 0)
  {
    g_text_menu_abort_requested = 0;
    return -1;
  }

  if (g_text_menu_timeout_seconds != -1)
  {
    ConsumeUiTickCount();
    s.last_timer_seconds = -1;
  }

  do
  {
    g_text_menu_needs_layout = 0;
    g_mouse_button_mask_snapshot = 0;
    if (g_random_seed_initialized == 0)
    {
      SeedRandomFromTickCount();
    }

    if (g_text_menu_timeout_seconds != -1)
    {
      s.timer_seconds_left = g_text_menu_timeout_seconds - GetUiTickCount() / (g_game_time_scale * 0x3c);
      if (s.timer_seconds_left == 0)
      {
        s.selected_menu_entry = -1;
        s.done = 1;
      }
      if (s.timer_seconds_left != s.last_timer_seconds)
      {
        strcpy(s.timer_text, _itoa(s.timer_seconds_left, g_text_menu_timer_itoa_buffer, 10));
        FillGraphicsRect(g_page0_window_bounds, g_text_menu_right - 0xe, g_text_menu_top + 4, 0xc, 7, 0xbc);
        DrawCenteredTextLineClamped(s.timer_text, g_text_menu_right - 8, g_text_menu_top + 5, 0xff);
        s.last_timer_seconds = s.timer_seconds_left;
      }
    }

    UpdateMouseSnapshot();
    if ((g_mouse_button_mask_snapshot != 0) || (s.mouse_active != 0))
    {
      g_text_menu_mouse_active = 1;
      s.mouse_active = g_text_menu_mouse_active;
      if (g_mouse_button_mask_snapshot == 2)
      {
        g_text_menu_mouse_released = 1;
      }

      s.selected_menu_entry = ((g_mouse_y_snapshot - g_text_menu_top) - 4) / g_text_menu_line_height - g_text_menu_first_option_line;
      if ((g_mouse_x_snapshot < g_text_menu_left) || (g_text_menu_right < g_mouse_x_snapshot))
      {
        s.selected_menu_entry = -1;
      }

      if ((g_text_menu_disabled_option_mask & (1 << (unsigned char)s.selected_menu_entry)) != 0)
      {
        s.selected_menu_entry = s.previous_menu_entry;
      }
      else
      {
        if (g_mouse_button_mask_snapshot == 0)
        {
          s.done = 1;
        }
      }
    }
    else
    {
      if (HasQueuedKeyInput() != 0)
      {
        s.key_code = PopNormalizedQueuedKeyInput();
        switch (s.key_code)
        {
        case 0x5000:
          if (g_text_menu_option_count - 1 > s.selected_menu_entry)
          {
            s.selected_menu_entry = s.selected_menu_entry + 1;
          }
          break;
        case 0x4800:
          if (0 < s.selected_menu_entry)
          {
            s.selected_menu_entry = s.selected_menu_entry - 1;
          }
          break;
        case 0xd:
        case 0x20:
          if ((g_text_menu_disabled_option_mask & (1 << (unsigned char)s.selected_menu_entry)) == 0)
          {
            s.done = 1;
          }
          break;
        case 0x1b:
          s.selected_menu_entry = -1;
          s.done = 1;
          break;
        default:
          for (s.next_menu_entry = s.selected_menu_entry + 1; s.next_menu_entry < 0x20; s.next_menu_entry = s.next_menu_entry + 1)
          {
            if ((g_text_menu_hotkey_by_option[s.next_menu_entry] != -1) &&
                ((((int)(char)g_text_menu_hotkey_by_option[s.next_menu_entry] ^ (s.key_code & 0x1f)) & 0x1f) == 0))
            {
              s.selected_menu_entry = s.next_menu_entry;
              break;
            }
          }
          break;
        }
      }
    }

    if ((s.selected_menu_entry < 0) || (g_text_menu_option_count <= s.selected_menu_entry) || (g_text_menu_ok_mode != 0) || (g_text_menu_force_cancel != 0))
    {
      s.selected_menu_entry = -1;
    }

    if (s.previous_menu_entry != s.selected_menu_entry)
    {
      DrawTextMenu(menu_text, s.selected_menu_entry);
      s.previous_menu_entry = s.selected_menu_entry;
      g_text_menu_hovered_selection = s.previous_menu_entry;
    }
  } while (!s.done);

  g_text_menu_finish_flash = 1;
  if (s.selected_menu_entry != -1)
  {
    HideMouseCursorNested();
    DrawTextMenu(menu_text, s.selected_menu_entry);
    DelayUiTicks(0x14);
    ShowMouseCursorNested();
  }
  else
  {
    g_text_menu_mouse_released = 0;
  }
  g_text_menu_timeout_seconds = -1;
  g_text_menu_last_selection = g_text_menu_timeout_seconds;
  g_text_menu_initial_selection = g_text_menu_last_selection;
  g_text_menu_show_ok_button = 0;
  g_text_menu_force_cancel = g_text_menu_show_ok_button;
  g_text_menu_disabled_option_mask = g_text_menu_force_cancel;
  g_text_menu_caret_prefix_mask = g_text_menu_disabled_option_mask;
  return s.selected_menu_entry;
}

// FUNCTION: SHANDALAR 0x00430e00
void DrawUiLine(int x1, int y1, int x2, int y2, int color_index)
{
  DrawGraphicsLine(g_page0_window_bounds, x1, y1, x2, y2, color_index);
}

// FUNCTION: SHANDALAR 0x004138bd
void DrawTiledDialogBoxFrame(int x, int y, int width, int height, int frame_style)
{
  int tile_columns;
  int tile_rows;
  int row;
  int col;

  x -= 4;
  y -= 4;
  width += 8;
  height += 8;

  tile_columns = (width + 0xf) / 0x10;
  tile_rows = (height + 0xf) / 0x10;
  x -= ((tile_columns << 4) - width) / 2;
  y -= ((tile_rows << 4) - height) / 2;

  g_page0_window_bounds->page_number = 1;
  FillGraphicsRect(g_page0_window_bounds, x, y, tile_columns << 4, tile_rows << 4, 0xe3);

  for (row = 0; tile_rows > row; row = row + 1)
  {
    for (col = 0; tile_columns > col; col = col + 1)
    {
      DrawEncodedImageUnscaled(g_page0_window_bounds, col * 0x10 + x, row * 0x10 + y, g_dialog_box_sprite_bank.frame[frame_style][0]);
    }
  }

  for (col = 0; tile_columns > col; col = col + 1)
  {
    DrawEncodedImageUnscaled(g_page0_window_bounds, col * 0x10 + x, y - 10, g_dialog_box_sprite_bank.frame[frame_style][5]);
    DrawEncodedImageUnscaled(g_page0_window_bounds, col * 0x10 + x, tile_rows * 0x10 + y - 6, g_dialog_box_sprite_bank.frame[frame_style][7]);
  }

  for (row = 0; tile_rows > row; row = row + 1)
  {
    DrawEncodedImageUnscaled(g_page0_window_bounds, x - 10, row * 0x10 + y, g_dialog_box_sprite_bank.frame[frame_style][8]);
    DrawEncodedImageUnscaled(g_page0_window_bounds, tile_columns * 0x10 + x - 6, row * 0x10 + y, g_dialog_box_sprite_bank.frame[frame_style][6]);
  }

  DrawEncodedImageUnscaled(g_page0_window_bounds, x - 10, y - 10, g_dialog_box_sprite_bank.frame[frame_style][1]);
  DrawEncodedImageUnscaled(g_page0_window_bounds, tile_columns * 0x10 + x - 6, y - 10, g_dialog_box_sprite_bank.frame[frame_style][2]);
  DrawEncodedImageUnscaled(g_page0_window_bounds, x - 10, tile_rows * 0x10 + y - 6, g_dialog_box_sprite_bank.frame[frame_style][3]);
  DrawEncodedImageUnscaled(g_page0_window_bounds, tile_columns * 0x10 + x - 6, tile_rows * 0x10 + y - 6, g_dialog_box_sprite_bank.frame[frame_style][4]);

  g_page0_window_bounds->page_number = 0;
  BlitGraphicsRect(g_page1_window_bounds, x - 10, y - 10, tile_columns * 0x10 + 0x14, tile_rows * 0x10 + 0x14,
                   g_page0_window_bounds, x - 10, y - 10);
}

// FUNCTION: SHANDALAR 0x004136ab
void DrawDialogBoxFrameAutoStyle(int x, int y, int width, int height, int unk1)
{
  if (g_text_box_frame_color_override != 0)
  {
    g_town_dialog_callback(x, y, width, height, g_text_box_frame_color_override);
    g_text_box_frame_color_override = 0;
  }
  else
  {
    g_town_dialog_callback(x, y, width, height, (y / 8) & 3);
  }
}

// FUNCTION: SHANDALAR 0x00430e2d
void DrawTextLineClamped(char *text, int x, int y, int color_index)
{
  if (x < 0)
  {
    x = 0;
  }
  if (y < 0)
  {
    y = 0;
  }

  if (global_screen_width <= x + MeasureTextLineWidth(text))
  {
    x = global_screen_width - 1 - MeasureTextLineWidth(text);
  }

  if (global_screen_height <= y + GetFontLineHeight(g_page0_window_bounds->font_slot))
  {
    y = global_screen_height - 1 - GetFontLineHeight(g_page0_window_bounds->font_slot);
  }

  g_page0_window_bounds->text_color = color_index;
  DrawTextLine(g_page0_window_bounds, x, y, text);
}

// FUNCTION: SHANDALAR 0x00413796
void DrawRectangleBorder(int x, int y, int width, int height, int color_index)
{
  DrawUiLine(x, y, width + x, y, color_index);
  DrawUiLine(x, height + y, width + x, height + y, color_index);
  DrawUiLine(width + x, y, width + x, height + y, color_index);
  DrawUiLine(x, y, x, height + y, color_index);
}

// FUNCTION: SHANDALAR 0x00430ef4
void DrawTextLineNoShadow(char *text, int x, int y, int color_index)
{
  g_page0_window_bounds->draw_shadow_enabled = 0;
  DrawTextLineClamped(text, x, y, color_index);
  g_page0_window_bounds->draw_shadow_enabled = 1;
}

// FUNCTION: SHANDALAR 0x004310e8
int MeasureTextLineWidth(char *text)
{
  unsigned char *cursor = text;
  int font_slot = g_page0_window_bounds->font_slot;
  int result = 0;
  while (*cursor)
  {
    result += GetFontCharWidth(font_slot, *cursor++);
  }
  return result;
}

// FUNCTION: SHANDALAR 0x0043104f
void DrawCenteredTextLineClamped(char *text, int center_x, int y, int color_index)
{
  center_x -= MeasureTextLineWidth(text) / 2;
  g_page0_window_bounds->draw_shadow_enabled = 0;
  DrawTextLineClamped(text, center_x, y, color_index);
  g_page0_window_bounds->draw_shadow_enabled = 1;
}

// FUNCTION: SHANDALAR 0x004ce97d
int GetUiTickCount(void)
{
  return g_ui_tick_count;
}

// FUNCTION: SHANDALAR 0x004ce955
int ConsumeUiTickCount(void)
{
  int queued_ticks;

  queued_ticks = g_ui_tick_count;
  g_ui_tick_count = 0;
  return queued_ticks;
}

// FUNCTION: SHANDALAR 0x0041318b
int DrawTextMenu(char *menu_text, int selected_option)
{
  struct
  {
    int text_color;      // ebp - 0x14
    int highlight_color; // ebp - 0x18
    int line_idx;        // ebp - 0x10
    int menu_bottom;     // ebp - 0xc
    int menu_option_idx; // ebp - 0x8
    int line_width;      // ebp - 0x4
  } s;

  g_text_menu_line_offsets[0] = 0;
  if (g_text_menu_needs_layout == 1)
  {
    g_text_menu_line_height = GetFontLineHeight(g_page0_window_bounds->font_slot);
    for (s.line_idx = 0; s.line_idx < 0x20; s.line_idx = s.line_idx + 1)
    {
      g_text_menu_hotkey_by_option[s.line_idx] = -1;
    }

    s.menu_option_idx = 0;
    g_text_menu_line_count = s.menu_option_idx;
    s.line_width = g_text_menu_line_count;
    g_text_menu_max_line_width = s.line_width;
    for (s.line_idx = 0; s.line_idx < (int)strlen(menu_text); s.line_idx = s.line_idx + 1)
    {
      if (menu_text[s.line_idx] == '\n')
      {
        if (g_text_menu_max_line_width < s.line_width)
        {
          g_text_menu_max_line_width = s.line_width;
        }
        s.line_width = 0;
        g_text_menu_line_count = g_text_menu_line_count + 1;
        g_text_menu_line_offsets[g_text_menu_line_count] = s.line_idx + 1;
      }
      else
      {
        if ((s.line_width == 0) && ((menu_text[s.line_idx] == ' ') || (menu_text[s.line_idx] == '_')))
        {
          if (s.menu_option_idx < 0x20)
          {
            g_text_menu_hotkey_by_option[s.menu_option_idx] = menu_text[s.line_idx + 1];
          }
          if (g_text_menu_first_option_line == -1)
          {
            g_text_menu_first_option_line = g_text_menu_line_count;
          }
          s.menu_option_idx = s.menu_option_idx + 1;
        }
        s.line_width = s.line_width + GetFontCharWidth(g_page0_window_bounds->font_slot, menu_text[s.line_idx]);
      }
    }

    g_text_menu_line_count = ClampIntToRange(g_text_menu_line_count, 0, (global_screen_height - g_text_menu_top) / g_text_menu_line_height);
    if (g_text_menu_left == -1)
    {
      g_text_menu_left = 0xa0 - (g_text_menu_max_line_width + 8) / 2;
    }
    g_text_menu_right = g_text_menu_max_line_width + g_text_menu_left + 8;
    s.menu_bottom = g_text_menu_line_count * g_text_menu_line_height + g_text_menu_top + 6;
    if (g_text_menu_show_ok_button != 0)
    {
      s.menu_bottom = s.menu_bottom + 2;
    }
    if (menu_text[strlen(menu_text) - 1] != '\n')
    {
      s.menu_option_idx = s.menu_option_idx - 1;
    }
    g_text_menu_option_count = s.menu_option_idx;
    DrawDialogBoxFrameAutoStyle(g_text_menu_left, g_text_menu_top, g_text_menu_right - g_text_menu_left, s.menu_bottom - g_text_menu_top, 0xe3);
    if (g_text_menu_ok_mode != 0)
    {
      DrawTextLineClamped("OK", g_text_menu_right - 0x11, s.menu_bottom - 8, 0xfe);
      DrawRectangleBorder(g_text_menu_right - 0x14, s.menu_bottom - 10, 0x14, 10, 0xfe);
    }
  }

  if ((*menu_text == ' ') || (*menu_text == '_'))
  {
    s.menu_option_idx = 0;
  }
  else
  {
    s.menu_option_idx = -1;
  }

  g_text_menu_color_base = g_text_menu_color_normal;
  g_page0_window_bounds->text_color = g_text_menu_color_base;
  for (s.line_idx = 0; g_text_menu_line_count > s.line_idx; s.line_idx = s.line_idx + 1)
  {
    if ((g_text_menu_needs_layout != 0) || (s.menu_option_idx == g_text_menu_hovered_selection) || (s.menu_option_idx == selected_option))
    {
      menu_text[g_text_menu_line_offsets[s.line_idx + 1] - 1] = '\0';
      if ((s.menu_option_idx >= 0) && ((g_text_menu_caret_prefix_mask & (1 << ((unsigned char)s.menu_option_idx))) != 0))
      {
        menu_text[g_text_menu_line_offsets[s.line_idx]] = '^';
        if ((g_text_menu_finish_flash != 0) && (s.menu_option_idx == selected_option))
        {
          DrawTextLineNoShadow(menu_text + g_text_menu_line_offsets[s.line_idx], g_text_menu_left + 5, s.line_idx * g_text_menu_line_height + g_text_menu_top + 5, 0xff);
        }
        else
        {
          if (s.menu_option_idx >= 0)
          {
            if (s.menu_option_idx == selected_option)
            {
              s.highlight_color = g_text_menu_color_selected;
            }
            else
            {
              s.highlight_color = g_text_menu_color_normal;
            }
          }
          else
          {
            s.highlight_color = g_text_menu_color_base;
          }
          DrawTextLineNoShadow(menu_text + g_text_menu_line_offsets[s.line_idx], g_text_menu_left + 5, s.line_idx * g_text_menu_line_height + g_text_menu_top + 5, s.highlight_color);
        }
        menu_text[g_text_menu_line_offsets[s.line_idx]] = ' ';
      }
      else
      {
        if (s.menu_option_idx >= 0)
        {
          if (s.menu_option_idx == selected_option)
          {
            s.text_color = g_text_menu_color_selected;
          }
          else
          {
            s.text_color = g_text_menu_color_normal;
          }
        }
        else
        {
          s.text_color = g_text_menu_color_base;
        }
        DrawTextLineNoShadow(menu_text + g_text_menu_line_offsets[s.line_idx], g_text_menu_left + 5, s.line_idx * g_text_menu_line_height + g_text_menu_top + 5, s.text_color);
      }
      menu_text[g_text_menu_line_offsets[s.line_idx + 1] - 1] = '\n';
    }

    if ((menu_text[g_text_menu_line_offsets[s.line_idx + 1]] == ' ') || (menu_text[g_text_menu_line_offsets[s.line_idx + 1]] == '_'))
    {
      s.menu_option_idx = s.menu_option_idx + 1;
    }
  }
  return selected_option;
}

// FUNCTION: SHANDALAR 0x0046ed03
void ShowMouseCursorNested(void)
{
  g_cursor_visibility_depth = g_cursor_visibility_depth + 1;
  if ((g_legacy_mouse_active != 0) && (g_cursor_visibility_depth == 1))
  {
    ShowMouseCursor();
  }
}

// FUNCTION: SHANDALAR 0x0046ed33
void HideMouseCursorNested(void)
{
  if ((g_legacy_mouse_active != 0) && (g_cursor_visibility_depth == 1))
  {
    HideMouseCursor();
  }
  g_cursor_visibility_depth = g_cursor_visibility_depth - 1;
}

// FUNCTION: SHANDALAR 0x00500321
int BeginMenuContext(void)
{
  g_menu_context_index = g_menu_context_index + 1;
  g_menu_current_control_index = -1;
  g_menu_prev_control_index = -1;
  ResetMenuContext(g_menu_context_index);
  return g_menu_context_index;
}

// FUNCTION: SHANDALAR 0x005000fb
int ResetMenuContext(int context_index)
{
  g_menu_control_count_by_context[context_index] = 0;
  g_menu_allow_arrow_nav_by_context[context_index] = 1;
  return 0;
}

// FUNCTION: SHANDALAR 0x0050014e
int AddMenuControlsToContext(AdvMenuControl *controls, int control_count, int context_index)
{
  int write_index;
  int i;

  g_menu_render_guard = 1;
  write_index = g_menu_control_count_by_context[context_index];
  for (i = 0; i < control_count; write_index++, i++)
  {
    g_menu_controls_by_context[context_index][write_index] = &controls[i];
  }
  g_menu_control_count_by_context[context_index] = g_menu_control_count_by_context[context_index] + control_count;
  g_menu_render_guard = 0;
  return g_menu_control_count_by_context[context_index];
}

// FUNCTION: SHANDALAR 0x0050027e
int RenderMenuControlRange(int first_index, int count)
{
  struct
  {
    int i;
    int limit;
  } s;

  s.limit = MIN(g_menu_control_count_by_context[g_menu_context_index], first_index + count);

  g_menu_render_guard = 1;
  for (s.i = first_index; s.i < s.limit; s.i = s.i + 1)
  {
    g_menu_controls_by_context[g_menu_context_index][s.i]->on_render(g_menu_controls_by_context[g_menu_context_index][s.i], 0);
  }
  g_menu_render_guard = 0;
  return 1;
}

// FUNCTION: SHANDALAR 0x0050035e
int EndMenuContext(void)
{
  ResetMenuContext(g_menu_context_index);
  if (g_menu_context_index == 0)
  {
    g_menu_context_index = 0;
  }
  else
  {
    g_menu_context_index = g_menu_context_index - 1;
  }
  g_menu_current_control_index = -1;
  g_menu_prev_control_index = -1;
  return g_menu_context_index;
}

// FUNCTION: SHANDALAR 0x004ce992
void DelayUiTicks(int delay)
{
  int start = g_ui_tick_count;
  while ((g_ui_tick_count - start) < (delay / 2))
  {
  }
}

// FUNCTION: SHANDALAR 0x00431351
void DrawEncodedImageUiScaled(FacemakerWindowBounds *dst, int x_320, int y_200, EncodedImage *sprite, int width_320, int height_200)
{
  x_320 = (x_320 * global_screen_width) / 0x280;
  y_200 = (y_200 * global_screen_height) / 0x1e0;
  width_320 = (width_320 * global_screen_width) / 0x280;
  height_200 = (height_200 * global_screen_height) / 0x1e0;
  DrawEncodedImageResampled(dst, x_320, y_200, width_320, height_200, sprite);
}

// FUNCTION: SHANDALAR 0x004cea4c
LONG ChangeDisplayResolution(DWORD width, DWORD height)
{
  DEVMODEA dev_mode;

  if (height == 0 && width == 0)
  {
    return ChangeDisplaySettingsA((DEVMODEA *)0, 0);
  }

  if (height == 0 || width == 0)
  {
    return -1;
  }

  dev_mode.dmSize = 0x9c;
  dev_mode.dmFields = 0x180000;
  dev_mode.dmPelsWidth = width;
  dev_mode.dmPelsHeight = height;
  return ChangeDisplaySettingsA(&dev_mode, 0);
}

// FUNCTION: SHANDALAR 0x004ceacb
void RestoreDisplayResolution(void)
{
  ChangeDisplayResolution(0, 0);
}

// FUNCTION: SHANDALAR 0x004cea02
unsigned int InitializeSoundPresenceState(void)
{
  g_local_sound_missing = (unsigned int)(FileExists("sound\\locmus1.wav") == 0);
  g_local_sound_state = 0;
  return 0;
}

// FUNCTION: SHANDALAR 0x004ce9c6
void ShutdownUiTimer(void)
{
  timeKillEvent(g_timer_event_handle);
  timeEndPeriod(g_timer_resolution_ms);
}

// FUNCTION: SHANDALAR 0x004ce9e9
void RestoreSystemPaletteAtExit(void)
{
  SetSystemPaletteUse(global_main_hdc, 1);
}

// FUNCTION: SHANDALAR 0x004ce8cd
void CALLBACK UiTimerTickCallback(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
  (void)uID;
  (void)uMsg;
  (void)dwUser;
  (void)dw1;
  (void)dw2;

  if (g_timer_thread_handle_ready == 0)
  {
    g_timer_thread_handle_ready = 1;
    g_timer_thread_handle = GetCurrentThread();
    DuplicateHandle(GetCurrentProcess(), g_timer_thread_handle, GetCurrentProcess(), &g_timer_thread_handle, 0x1f03ff, FALSE, 0);
  }

  ++g_ui_tick_count;

  if (g_sound_loader_busy != 0)
  {
  }
  else
  {
    g_sound_loader_busy = 1;
    update_snd();
    g_sound_loader_busy = 0;
  }
}

