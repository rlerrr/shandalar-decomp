#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

extern int g_showlist_smallcard_width;
extern int g_showlist_smallcard_height;
extern int DAT_00939508;
extern char global_base_directory[];

LOGFONTA *LoadFontFromIni(char *section, int must_load);
int load_text_with_tab_escapes(char *filename, char *section_name);
void delete_and_close_object(HANDLE obj);
card_id_t get_displayed_card_id(int player, int card);
int card_window_matches_player_and_card(HWND hwnd, int *player_and_card);
int card_window_matches_card_id(HWND hwnd, card_id_t card_id);
card_id_t get_card_window_displayed_card_id(HWND hwnd);

// GLOBAL: MAGIC 0x0055e0d8
// GLOBAL: SHANDALAR 0x0057f068
int g_hand_card_windows_long_offset = 0;

// GLOBAL: MAGIC 0x0055e0dc
// GLOBAL: SHANDALAR 0x0057f06c
int g_hand_card_count_long_offset = 4;

// GLOBAL: MAGIC 0x0055e0e0
// GLOBAL: SHANDALAR 0x0057f070
int g_hand_art_width_long_offset = 8;

// GLOBAL: MAGIC 0x0055e0e4
// GLOBAL: SHANDALAR 0x0057f074
int g_hand_art_title_height_long_offset = 0xc;

// GLOBAL: MAGIC 0x0055e0e8
// GLOBAL: SHANDALAR 0x0057f078
int g_hand_art_side_width_long_offset = 0x10;

// GLOBAL: MAGIC 0x0055e0ec
// GLOBAL: SHANDALAR 0x0057f07c
int g_hand_art_bottom_height_long_offset = 0x14;

// GLOBAL: MAGIC 0x0055e0f0
// GLOBAL: SHANDALAR 0x0057f080
int g_hand_background_bitmap_long_offset = 0x18;

// GLOBAL: MAGIC 0x0055e0f4
// GLOBAL: SHANDALAR 0x0057f084
int g_hand_selected_index_long_offset = 0x1c;

// GLOBAL: MAGIC 0x0055e0f8
// GLOBAL: SHANDALAR 0x0057f088
int g_hand_window_extra_bytes = 0x20;

// GLOBAL: MAGIC 0x0064f8f8
// GLOBAL: SHANDALAR 0x005a9d58
COLORREF g_hand_title_text_color;

// GLOBAL: MAGIC 0x0064f8fc
// GLOBAL: SHANDALAR 0x005a9d5c
COLORREF g_hand_title_shadow_color;

// GLOBAL: MAGIC 0x0064f900
// GLOBAL: SHANDALAR 0x005a9d60
char g_hand_menu_help_text[0x1c];

// GLOBAL: MAGIC 0x0064f91c
// GLOBAL: SHANDALAR 0x005a9d7c
HMENU g_hand_popup_menu;

// GLOBAL: MAGIC 0x0064f920
// GLOBAL: SHANDALAR 0x005a9d80
HFONT g_hand_title_font;

// FUNCTION: MAGIC 0x004bbae0
// FUNCTION: SHANDALAR 0x0046ee20
int register_MAGICGAME_HandClass(LPCSTR class_name)
{
  struct
  {
    int registered;
    WNDCLASSA wndclass;
  } s;

  s.registered = 1;
  SET_DUEL_WNDCLASS(s.wndclass, 3, wndproc_MAGICGAME_HandClass, g_hand_window_extra_bytes,
                    (HICON)0, (HBRUSH)0x6, class_name);
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.registered = 0;
  }
  g_hand_popup_menu = CreatePopupMenu();
  load_text_with_tab_escapes(global_ui_strings_filename, "MENU_HAND");
  strcpy(g_hand_menu_help_text, g_text_lines[0]);
  g_hand_title_font = CreateFontIndirectA(LoadFontFromIni("Hand", 0));
  g_hand_title_text_color = 0x10000bf;
  g_hand_title_shadow_color = 0x10000c9;
  return s.registered;
}

// FUNCTION: MAGIC 0x004bbbc7
// FUNCTION: SHANDALAR 0x0046ef07
void destroy_MAGICGAME_HandClass(LPCSTR class_name)
{
  if (g_hand_popup_menu != (HMENU)0)
  {
    DestroyMenu(g_hand_popup_menu);
  }
  g_hand_popup_menu = (HMENU)0;
  if (g_hand_title_font != (HFONT)0)
  {
    DeleteObject(g_hand_title_font);
  }
  g_hand_title_font = (HFONT)0;
}

// FUNCTION: MAGIC 0x004bd6dd
// FUNCTION: SHANDALAR 0x00470a12
void calculate_hand_art_offsets(int card_width, int card_height, int art_width,
                                int title_art_height, int side_art_width,
                                int bottom_art_height, int *title_height,
                                int *bottom_margin, int *side_margin,
                                int *edge_width)
{
  (void)card_width;
  if (art_width != 0)
  {
    *title_height = (card_height * 0x1f) / 100;
    *bottom_margin = (*title_height * title_art_height) / art_width;
    *side_margin = (*title_height * side_art_width) / art_width;
    *edge_width = (*title_height * bottom_art_height) / art_width;
  }
  else
  {
    *title_height = 0;
    *bottom_margin = 0;
    *side_margin = 0;
    *edge_width = 0;
  }
}

// FUNCTION: MAGIC 0x004bd468
// FUNCTION: SHANDALAR 0x0047079d
void draw_hand_window_frame(HDC dc, int *outer_rect, int *inner_rect,
                            int edge_width, int art_width, int title_art_height,
                            int side_art_width, int bottom_art_height,
                            HBITMAP bitmap)
{
  struct
  {
    BITMAP bitmap_info;
    int y;
    RECT rect;
    int y_step;
  } s;

  if (dc == (HDC)0 || outer_rect == NULL || inner_rect == NULL || bitmap == (HBITMAP)0)
  {
    return;
  }

  GetObjectA(bitmap, sizeof(BITMAP), &s.bitmap_info);
  s.y_step = ((inner_rect[1] - outer_rect[1]) * s.bitmap_info.bmHeight) / art_width;
  for (s.y = inner_rect[1]; s.y < inner_rect[3]; s.y += s.y_step)
  {
    SetRect(&s.rect, inner_rect[0], s.y, inner_rect[0] + edge_width, s.y + s.y_step);
    DrawBitmapSubrectToRect(dc, &s.rect, bitmap, s.bitmap_info.bmWidth - bottom_art_height, 0,
                            bottom_art_height, s.bitmap_info.bmHeight);
    SetRect(&s.rect, inner_rect[2] - edge_width, s.y, inner_rect[2], s.y + s.y_step);
    DrawBitmapSubrectToRect(dc, &s.rect, bitmap, s.bitmap_info.bmWidth - bottom_art_height, 0,
                            bottom_art_height, s.bitmap_info.bmHeight);
  }

  SetRect(&s.rect, inner_rect[0], outer_rect[1], inner_rect[2], inner_rect[1]);
  DrawBitmapSubrectToRect(dc, &s.rect, bitmap, 0, 0,
                          (s.bitmap_info.bmWidth - side_art_width) - bottom_art_height,
                          art_width);
  SetRect(&s.rect, inner_rect[0], inner_rect[3], inner_rect[2], outer_rect[3]);
  DrawBitmapSubrectToRect(dc, &s.rect, bitmap, 0, s.bitmap_info.bmHeight - title_art_height,
                          (s.bitmap_info.bmWidth - side_art_width) - bottom_art_height,
                          title_art_height);

  for (s.y = outer_rect[1]; s.y < outer_rect[3]; s.y += s.y_step)
  {
    SetRect(&s.rect, outer_rect[0], s.y, inner_rect[0], s.y + s.y_step);
    DrawBitmapSubrectToRect(dc, &s.rect, bitmap,
                            (s.bitmap_info.bmWidth - side_art_width) - bottom_art_height,
                            0, side_art_width, s.bitmap_info.bmHeight);
    SetRect(&s.rect, inner_rect[2], s.y, outer_rect[2], s.y + s.y_step);
    DrawBitmapSubrectToRect(dc, &s.rect, bitmap,
                            (s.bitmap_info.bmWidth - side_art_width) - bottom_art_height,
                            0, side_art_width, s.bitmap_info.bmHeight);
  }
}

// FUNCTION: MAGIC 0x004bd768
// FUNCTION: SHANDALAR 0x00470a9d
void update_hand_window_title(char *title, HWND hwnd, int card_count)
{
  sprintf(title, "%s (%d)",
          hwnd == g_duel_full_card_window_hwnd ? gs_window_title_your_hand_00777bf0 : gs_window_title_opponent_0091c820,
          card_count);
  SetWindowTextA(hwnd, title);
  InvalidateRect(hwnd, NULL, TRUE);
}

// FUNCTION: MAGIC 0x004bd131
// FUNCTION: SHANDALAR 0x00470468
void resize_duel_hand_window(HWND hwnd)
{
  struct
  {
    int side_margin;
    int edge_width;
    int loop_index;
    int art_bottom_height;
    int card_count;
    int art_width;
    int art_title_height;
    int art_side_width;
    int window_width;
    int card_index;
    int card_y;
    int card_x;
    HWND previous_card;
    HWND *card_windows;
    int visible_count;
    int max_visible_cards;
    int selected_index;
    int title_height;
    HBITMAP background_bitmap;
    int window_height;
    int bottom_margin;
  } s;

  s.max_visible_cards = 8;
  s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
  s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
  s.art_width = GetWindowLongA(hwnd, g_hand_art_width_long_offset);
  s.art_title_height = GetWindowLongA(hwnd, g_hand_art_title_height_long_offset);
  s.art_side_width = GetWindowLongA(hwnd, g_hand_art_side_width_long_offset);
  s.art_bottom_height = GetWindowLongA(hwnd, g_hand_art_bottom_height_long_offset);
  s.background_bitmap = (HBITMAP)GetWindowLongA(hwnd, g_hand_background_bitmap_long_offset);
  s.selected_index = GetWindowLongA(hwnd, g_hand_selected_index_long_offset);
  calculate_hand_art_offsets(g_showlist_smallcard_width, g_showlist_smallcard_height,
                             s.art_width, s.art_title_height, s.art_side_width, s.art_bottom_height,
                             &s.title_height, &s.bottom_margin, &s.side_margin, &s.edge_width);

  if (g_duel_full_card_window_hwnd == hwnd ||
      (g_duel_life_window_hwnd == hwnd && g_duel_surface_reset_state != 0))
  {
    s.visible_count = s.max_visible_cards < s.card_count ? s.max_visible_cards : s.card_count;
    s.window_width = s.edge_width + s.edge_width + s.side_margin + s.side_margin +
                     g_showlist_smallcard_width;
    if (s.card_count == 0)
    {
      s.window_height = s.bottom_margin + s.title_height;
    }
    else
    {
      s.window_height = (s.visible_count - 1) * DAT_00939508 + s.bottom_margin +
                        s.title_height + g_showlist_smallcard_height;
    }

    if (s.card_count > 0)
    {
      s.card_x = s.edge_width + s.side_margin;
      s.card_y = (s.window_height - s.bottom_margin) - g_showlist_smallcard_height;
      s.card_index = s.selected_index;
      s.previous_card = (HWND)0;
      for (s.loop_index = 1; s.loop_index <= s.visible_count; s.loop_index++)
      {
        MoveWindow(s.card_windows[s.card_index], s.card_x, s.card_y,
                   g_showlist_smallcard_width, g_showlist_smallcard_height, TRUE);
        if (s.previous_card == (HWND)0)
        {
          BringWindowToTop(s.card_windows[s.card_index]);
        }
        else
        {
          SetWindowPos(s.card_windows[s.card_index], s.previous_card, 0, 0, 0, 0,
                       SWP_NOMOVE | SWP_NOSIZE);
        }
        s.previous_card = s.card_windows[s.card_index];
        if (s.card_index == 0)
        {
          s.card_index = s.card_count - 1;
        }
        else
        {
          s.card_index--;
        }
        s.card_y -= DAT_00939508;
      }
      for (s.loop_index = s.visible_count; s.card_count > s.loop_index; s.loop_index++)
      {
        MoveWindow(s.card_windows[s.card_index], -1, -1, 0, 0, TRUE);
        if (s.card_index == 0)
        {
          s.card_index = s.card_count - 1;
        }
        else
        {
          s.card_index--;
        }
      }
    }

    UpdateWindow(hwnd);
    SetWindowPos(hwnd, (HWND)0, 0, 0, s.window_width, s.window_height,
                 SWP_NOMOVE | SWP_NOZORDER);
  }
  else
  {
    s.window_width = s.edge_width + s.edge_width + s.side_margin + s.side_margin +
                     g_showlist_smallcard_width;
    s.window_height = s.bottom_margin + s.title_height;
    for (s.card_index = 0; s.card_count > s.card_index; s.card_index++)
    {
      MoveWindow(s.card_windows[s.card_index], -1, -1, 0, 0, TRUE);
    }
    SetWindowPos(hwnd, (HWND)0, 0, 0, s.window_width, s.window_height,
                 SWP_NOMOVE | SWP_NOZORDER);
  }
}

// FUNCTION: MAGIC 0x004bbc18
// FUNCTION: SHANDALAR 0x0046ef58
LRESULT CALLBACK wndproc_MAGICGAME_HandClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int menu_item_count;
    POINT menu_point;
    RECT menu_rect;
    int side_margin;
    int edge_width;
    char window_title[100];
    RECT client_rect;
    HDC paint_dc;
    PAINTSTRUCT paint_struct;
    int saved_dc;
    RECT title_rect;
    int title_height;
    RECT blit_rect;
    int bottom_margin;
    RECT inner_rect;
    int hit_side_margin;   // ebp - 0x278
    int hit_edge_width;    // ebp - 0x274
    unsigned int click_x;  // ebp - 0x270
    unsigned int click_y;  // ebp - 0x26c
    int hit_title_height;  // ebp - 0x268
    int hit_bottom_margin; // ebp - 0x264
    int target_index;
    RECT before_move_rect;
    RECT after_move_rect;
    char help_path[264];
    DWORD help_context;
    int find_index;
    int *find_player_card;
    LRESULT find_result;
    int find_found;
    int *bring_player_card;
    int bring_shift_index;
    int bring_index;
    HWND bring_hwnd;
    int bring_found;
    char clear_title[52];
    int clear_index;
    char remove_title[52];
    int *remove_player_card;
    int remove_shift_index;
    int remove_index;
    int remove_found;
    char add_title[52];
    int add_shift_index;
    HWND existing_hwnd;
    int *add_player_card;
    int add_index;      // ebp - 0x50
    HWND selected_hwnd; // ebp - 0x4c
    HBITMAP new_bitmap; // ebp - 0x48
    RECT *bitmap_rect;
    RECT *invalidate_rect;
    int *invalidate_player_card; // ebp - 0x3c
    int invalidate_index;        // ebp - 0x38
    int invalidate_found;        // ebp - 0x34
    int invalidate_all_index;    // ebp - 0x30
    int refresh_index;           // ebp - 0x2c
    card_id_t card_id;           // ebp - 0x28
    int card_id_index;           // ebp - 0x24
    LONG art_bottom_height;      // ebp - 0x20
    int card_count;              // ebp - 0x1c
    LONG art_width;              // ebp - 0x18
    LONG art_title_height;       // ebp - 0x14
    LONG art_side_width;         // ebp - 0x10
    HWND *card_windows;          // ebp - 0xc
    int selected_index;          // ebp - 0x8
    HWND background_bitmap;      // ebp - 0x4
  } s;

  switch (msg)
  {
  case 0x433:
  case 0x434:
    s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
    s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
    s.card_id = (int)wparam;
    for (s.card_id_index = 0; s.card_id_index < s.card_count; s.card_id_index++)
    {
      if (card_window_matches_card_id(s.card_windows[s.card_id_index], s.card_id) != 0)
      {
        InvalidateRect(s.card_windows[s.card_id_index], NULL, FALSE);
      }
    }
    return 0;

  case 0x432:
    s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
    s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
    for (s.refresh_index = 0; s.refresh_index < s.card_count; s.refresh_index++)
    {
      SendMessageA(s.card_windows[s.refresh_index], 0x432, 0, 0);
    }
    return 0;

  case 0x435:
    s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
    s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
    for (s.invalidate_all_index = 0; s.invalidate_all_index < s.card_count; s.invalidate_all_index++)
    {
      InvalidateRect(s.card_windows[s.invalidate_all_index], NULL, FALSE);
    }
    return 0;

  case 0x436:
    s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
    s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
    s.invalidate_player_card = (int *)wparam;
    s.invalidate_rect = (RECT *)lparam;
    if (s.invalidate_player_card == 0)
    {
      return 0;
    }
    s.invalidate_found = 0;
    s.invalidate_index = 0;
    for (; s.invalidate_index < s.card_count && s.invalidate_found == 0;
         s.invalidate_index++)
    {
      if (card_window_matches_player_and_card(s.card_windows[s.invalidate_index], s.invalidate_player_card) != 0)
      {
        s.invalidate_found = 1;
        if (s.invalidate_rect != NULL)
        {
          SendMessageA(s.card_windows[s.invalidate_index], 0x432, 0, 0);
        }
        else
        {
          InvalidateRect(s.card_windows[s.invalidate_index], NULL, FALSE);
        }
      }
    }
    return 0;

  case 0x439:
    s.new_bitmap = (HBITMAP)wparam;
    s.bitmap_rect = (RECT *)lparam;
    s.background_bitmap = (HWND)GetWindowLongA(hwnd, g_hand_background_bitmap_long_offset);
    if (s.background_bitmap != (HWND)0)
    {
      DeleteObject(s.background_bitmap);
    }
    s.background_bitmap = (HWND)s.new_bitmap;
    s.art_width = s.bitmap_rect->left;
    s.art_title_height = s.bitmap_rect->top;
    s.art_side_width = s.bitmap_rect->right;
    s.art_bottom_height = s.bitmap_rect->bottom;
    SetWindowLongA(hwnd, g_hand_art_width_long_offset, s.art_width);
    SetWindowLongA(hwnd, g_hand_art_title_height_long_offset, s.art_title_height);
    SetWindowLongA(hwnd, g_hand_art_side_width_long_offset, s.art_side_width);
    SetWindowLongA(hwnd, g_hand_art_bottom_height_long_offset, s.art_bottom_height);
    SetWindowLongA(hwnd, g_hand_background_bitmap_long_offset, (LONG)s.background_bitmap);
    resize_duel_hand_window(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;

  case 0x438:
    return (LRESULT)(s.background_bitmap = (HWND)GetWindowLongA(hwnd, g_hand_background_bitmap_long_offset));

  case 0x400:
    s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
    s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
    s.selected_hwnd = (HWND)wparam;
    if (s.selected_hwnd == 0)
    {
      return 0;
    }
    for (s.add_index = 0; s.add_index < s.card_count; s.add_index++)
    {
      if (s.card_windows[s.add_index] == s.selected_hwnd)
      {
        s.selected_index = s.add_index;
        SetWindowLongA(hwnd, g_hand_selected_index_long_offset, s.selected_index);
        resize_duel_hand_window(hwnd);
      }
    }
    return 0;

  case 0x40a:
    s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
    s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
    s.selected_index = GetWindowLongA(hwnd, g_hand_selected_index_long_offset);
    if (s.card_count >= 0x32)
    {
      return 0;
    }
    s.add_player_card = (int *)wparam;
    s.existing_hwnd = (HWND)SendMessageA(hwnd, 0x40f, (WPARAM)s.add_player_card, 0);
    if (s.existing_hwnd == (HWND)0)
    {
      s.existing_hwnd = CreateWindowExA(0, "MAGICGAME_CardClass", "Hand Card",
                                        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, 0, 0, hwnd, (HMENU)1,
                                        g_app_instance, (LPVOID)s.add_player_card);
      if (s.existing_hwnd == (HWND)0)
      {
        return 0;
      }
      s.card_windows[s.card_count] = s.existing_hwnd;
      s.card_count++;
      SetWindowLongA(hwnd, g_hand_card_count_long_offset, s.card_count);
      if (s.card_count - 1 == s.selected_index + 1)
      {
        s.selected_index = s.card_count - 1;
        SetWindowLongA(hwnd, g_hand_selected_index_long_offset, s.selected_index);
      }
      else
      {
        for (s.add_shift_index = s.card_count - 1; s.selected_index + 1 < s.add_shift_index; s.add_shift_index--)
        {
          s.card_windows[s.add_shift_index] = s.card_windows[s.add_shift_index - 1];
        }
        s.card_windows[s.selected_index + 1] = s.existing_hwnd;
        s.selected_index++;
        SetWindowLongA(hwnd, g_hand_selected_index_long_offset, s.selected_index);
      }
      resize_duel_hand_window(hwnd);
      update_hand_window_title(s.add_title, hwnd, s.card_count);
    }
    else if (card_window_matches_card_id(s.existing_hwnd,
                                         get_displayed_card_id(s.add_player_card[0], s.add_player_card[1])) == 0)
    {
      SendMessageA(hwnd, 0x40b, (WPARAM)s.add_player_card, 0);
      SendMessageA(hwnd, 0x40a, (WPARAM)s.add_player_card, 0);
    }
    return s.card_count;

  case 0x40b:
    s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
    s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
    s.selected_index = GetWindowLongA(hwnd, g_hand_selected_index_long_offset);
    s.remove_player_card = (int *)wparam;
    s.remove_found = 0;
    s.remove_index = 0;
    for (; s.remove_index < s.card_count && s.remove_found == 0;
         s.remove_index++)
    {
      if (card_window_matches_player_and_card(s.card_windows[s.remove_index], s.remove_player_card) != 0)
      {
        s.remove_found = 1;
        DestroyWindow(s.card_windows[s.remove_index]);
        s.card_count--;
        SetWindowLongA(hwnd, g_hand_card_count_long_offset, s.card_count);
        for (s.remove_shift_index = s.remove_index; s.remove_shift_index < s.card_count; s.remove_shift_index++)
        {
          s.card_windows[s.remove_shift_index] = s.card_windows[s.remove_shift_index + 1];
        }
        if (s.selected_index == s.remove_index)
        {
          if (s.selected_index == 0)
          {
            s.selected_index = s.card_count - 1;
          }
          else
          {
            s.selected_index--;
          }
          SetWindowLongA(hwnd, g_hand_selected_index_long_offset, s.selected_index);
        }
        else
        {
          if (s.selected_index == 0)
          {
            s.selected_index = s.card_count - 1;
          }
          else
          {
            s.selected_index--;
          }
          SetWindowLongA(hwnd, g_hand_selected_index_long_offset, s.selected_index);
        }
        update_hand_window_title(s.remove_title, hwnd, s.card_count);
        UpdateWindow(hwnd);
      }
    }
    if (s.remove_found != 0)
    {
      resize_duel_hand_window(hwnd);
    }
    return s.remove_found;

  case 0x40c:
    s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
    s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
    for (s.clear_index = 0; s.clear_index < s.card_count; s.clear_index++)
    {
      DestroyWindow(s.card_windows[s.clear_index]);
    }
    s.card_count = 0;
    SetWindowLongA(hwnd, g_hand_card_count_long_offset, s.card_count);
    s.selected_index = -1;
    SetWindowLongA(hwnd, g_hand_selected_index_long_offset, s.selected_index);
    resize_duel_hand_window(hwnd);
    update_hand_window_title(s.clear_title, hwnd, s.card_count);
    return 0;

  case 0x40d:
    s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
    s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
    s.bring_player_card = (int *)wparam;
    s.bring_found = 0;
    s.bring_index = 0;
    for (; s.card_count > s.bring_index && s.bring_found == 0; s.bring_index++)
    {
      if (card_window_matches_player_and_card(s.card_windows[s.bring_index], s.bring_player_card) != 0)
      {
        s.bring_found = 1;
        s.bring_hwnd = s.card_windows[s.bring_index];
        BringWindowToTop(s.bring_hwnd);
        for (s.bring_shift_index = s.bring_index; s.bring_shift_index < s.card_count - 1; s.bring_shift_index++)
        {
          s.card_windows[s.bring_shift_index] = s.card_windows[s.bring_shift_index + 1];
        }
        s.card_windows[s.card_count - 1] = s.bring_hwnd;
      }
    }
    return s.bring_found;

  case 0x40e:
  case 0x40f:
    s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
    s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
    s.find_player_card = (int *)wparam;
    s.find_found = 0;
    s.find_index = 0;
    for (; s.find_index < s.card_count && s.find_found == 0;
         s.find_index++)
    {
      if (card_window_matches_player_and_card(s.card_windows[s.find_index], s.find_player_card) != 0)
      {
        s.find_found = 1;
        if (msg == 0x40e)
        {
          s.find_result = get_card_window_displayed_card_id(s.card_windows[s.find_index]);
        }
        else
        {
          s.find_result = (LRESULT)s.card_windows[s.find_index];
        }
      }
    }
    if (s.find_found != 0)
    {
      return s.find_result;
    }
    else
    {
      return (msg == 0x40e) ? -1 : 0;
    }

  case WM_CLOSE:
    ShowWindow(hwnd, SW_HIDE);
    return 0;

  case WM_COMMAND:
    switch ((unsigned int)wparam & 0xffff)
    {
    case 100:
      s.help_context = 0x7e3;
      strcpy(s.help_path, global_base_directory);
      strcat(s.help_path, "\\duel.hlp");
      WinHelpA(g_duel_window_hwnd, s.help_path, HELP_CONTEXT, s.help_context);
      break;
    default:
      break;
    }
    return 0;

  case WM_CREATE:
    s.card_count = 0;
    SetWindowLongA(hwnd, g_hand_card_count_long_offset, s.card_count);
    s.card_windows = malloc(200);
    SetWindowLongA(hwnd, g_hand_card_windows_long_offset, (LONG)s.card_windows);
    s.art_width = 0;
    s.art_title_height = 0;
    s.art_side_width = 0;
    s.art_bottom_height = 0;
    SetWindowLongA(hwnd, g_hand_art_width_long_offset, s.art_width);
    SetWindowLongA(hwnd, g_hand_art_title_height_long_offset, s.art_title_height);
    SetWindowLongA(hwnd, g_hand_art_side_width_long_offset, s.art_side_width);
    SetWindowLongA(hwnd, g_hand_art_bottom_height_long_offset, s.art_bottom_height);
    s.background_bitmap = (HWND)0;
    SetWindowLongA(hwnd, g_hand_background_bitmap_long_offset, (LONG)s.background_bitmap);
    s.selected_index = -1;
    SetWindowLongA(hwnd, g_hand_selected_index_long_offset, s.selected_index);
    if (s.card_windows == NULL)
    {
      return -1;
    }
    resize_duel_hand_window(hwnd);
    return 0;

  case WM_DESTROY:
    s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
    free(s.card_windows);
    s.background_bitmap = (HWND)GetWindowLongA(hwnd, g_hand_background_bitmap_long_offset);
    if (s.background_bitmap != (HWND)0)
    {
      delete_and_close_object(s.background_bitmap);
    }
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_LBUTTONDOWN:
    GetWindowRect(hwnd, &s.before_move_rect);
    SendMessageA(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
    GetWindowRect(hwnd, &s.after_move_rect);
    if (abs(s.after_move_rect.top - s.before_move_rect.top) +
            abs(s.after_move_rect.left - s.before_move_rect.left) <
        5)
    {
      s.click_x = (unsigned int)lparam & 0xffff;
      s.click_y = HIWORD(lparam);
      s.card_windows = (HWND *)GetWindowLongA(hwnd, g_hand_card_windows_long_offset);
      s.card_count = GetWindowLongA(hwnd, g_hand_card_count_long_offset);
      s.selected_index = GetWindowLongA(hwnd, g_hand_selected_index_long_offset);
      s.art_width = GetWindowLongA(hwnd, g_hand_art_width_long_offset);
      s.art_title_height = GetWindowLongA(hwnd, g_hand_art_title_height_long_offset);
      s.art_side_width = GetWindowLongA(hwnd, g_hand_art_side_width_long_offset);
      s.art_bottom_height = GetWindowLongA(hwnd, g_hand_art_bottom_height_long_offset);
      if (s.card_count > 1)
      {
        calculate_hand_art_offsets(g_showlist_smallcard_width, g_showlist_smallcard_height,
                                   s.art_width, s.art_title_height, s.art_side_width,
                                   s.art_bottom_height, &s.hit_title_height,
                                   &s.hit_bottom_margin, &s.hit_side_margin, &s.hit_edge_width);
        if ((int)s.click_y < s.hit_title_height)
        {
          GetClientRect(hwnd, &s.after_move_rect);
          if ((int)s.click_x < (s.after_move_rect.right * 0x14) / 100)
          {
            if (s.selected_index > 0)
            {
              s.target_index = s.selected_index - 1;
            }
            else
            {
              s.target_index = s.card_count - 1;
            }
            SendMessageA(hwnd, 0x400, (WPARAM)s.card_windows[s.target_index], 0);
          }
          else if ((s.after_move_rect.right * 0x50) / 100 < (int)s.click_x)
          {
            if (s.selected_index >= s.card_count - 1)
            {
              s.target_index = 0;
            }
            else
            {
              s.target_index = s.selected_index + 1;
            }
            SendMessageA(hwnd, 0x400, (WPARAM)s.card_windows[s.target_index], 0);
          }
        }
      }
    }
    return 0;

  case WM_PAINT:
    s.art_width = GetWindowLongA(hwnd, g_hand_art_width_long_offset);
    s.art_title_height = GetWindowLongA(hwnd, g_hand_art_title_height_long_offset);
    s.art_side_width = GetWindowLongA(hwnd, g_hand_art_side_width_long_offset);
    s.art_bottom_height = GetWindowLongA(hwnd, g_hand_art_bottom_height_long_offset);
    s.background_bitmap = (HWND)GetWindowLongA(hwnd, g_hand_background_bitmap_long_offset);
    calculate_hand_art_offsets(g_showlist_smallcard_width, g_showlist_smallcard_height,
                               s.art_width, s.art_title_height, s.art_side_width,
                               s.art_bottom_height, &s.title_height, &s.bottom_margin,
                               &s.side_margin, &s.edge_width);
    GetClientRect(hwnd, &s.client_rect);
    s.inner_rect.top = s.client_rect.top + s.title_height;
    s.inner_rect.bottom = s.client_rect.bottom - s.bottom_margin;
    s.inner_rect.left = s.client_rect.left + s.side_margin;
    s.inner_rect.right = s.client_rect.right - s.side_margin;
    EnterCriticalSection(&g_card_render_lock);
    s.saved_dc = SaveDC(g_shared_offscreen_dc);
    FillRect(g_shared_offscreen_dc, &s.client_rect, GetStockObject(4));
    draw_hand_window_frame(g_shared_offscreen_dc, (int *)&s.client_rect, (int *)&s.inner_rect,
                           s.edge_width, s.art_width, s.art_title_height,
                           s.art_side_width, s.art_bottom_height, (HBITMAP)s.background_bitmap);
    SetMapMode(g_shared_offscreen_dc, MM_ANISOTROPIC);
    SetWindowExtEx(g_shared_offscreen_dc, s.client_rect.right - s.client_rect.left, 0x14, NULL);
    SetViewportExtEx(g_shared_offscreen_dc, s.client_rect.right - s.client_rect.left,
                     (s.title_height * 0x30) / 100, NULL);
    SelectObject(g_shared_offscreen_dc, g_hand_title_font);
    GetWindowTextA(hwnd, s.window_title, 100);
    SetBkMode(g_shared_offscreen_dc, TRANSPARENT);
    SetRect(&s.title_rect, s.client_rect.left, s.client_rect.top,
            s.client_rect.right, s.inner_rect.top);
    DPtoLP(g_shared_offscreen_dc, (LPPOINT)&s.title_rect, 2);
    OffsetRect(&s.title_rect, 2, 2);
    SetTextColor(g_shared_offscreen_dc, g_hand_title_shadow_color);
    DrawTextA(g_shared_offscreen_dc, s.window_title, -1, &s.title_rect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    OffsetRect(&s.title_rect, -2, -2);
    SetTextColor(g_shared_offscreen_dc, g_hand_title_text_color);
    DrawTextA(g_shared_offscreen_dc, s.window_title, -1, &s.title_rect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    RestoreDC(g_shared_offscreen_dc, s.saved_dc);
    s.paint_dc = BeginPaint(hwnd, &s.paint_struct);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetClientRect(hwnd, &s.blit_rect);
      BitBlt(s.paint_dc, 0, 0, s.blit_rect.right, s.blit_rect.bottom,
             g_shared_offscreen_dc, 0, 0, SRCCOPY);
      EndPaint(hwnd, &s.paint_struct);
    }
    LeaveCriticalSection(&g_card_render_lock);
    return 0;

  case WM_RBUTTONDOWN:
    s.menu_point.x = (unsigned int)lparam & 0xffff;
    s.menu_point.y = HIWORD(lparam);
    ClientToScreen(hwnd, &s.menu_point);
    SetRect(&s.menu_rect, s.menu_point.x, s.menu_point.y, s.menu_point.x + 1, s.menu_point.y + 1);
    TrackPopupMenu(g_hand_popup_menu, TPM_RIGHTBUTTON, s.menu_point.x, s.menu_point.y, 0, hwnd, &s.menu_rect);
    return 0;

  case WM_INITMENU:
    AppendMenuA(g_hand_popup_menu, 0, 100, g_hand_menu_help_text);
    return 0;

  case WM_MENUSELECT:
    if (HIWORD(wparam) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_hand_popup_menu);
      while (s.menu_item_count-- != 0)
      {
        DeleteMenu(g_hand_popup_menu, 0, MF_BYPOSITION);
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
