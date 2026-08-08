#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "defs.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

typedef struct
{
  int player;
  int card;
  target_t targets[20];
  int number_of_targets;
} spell_chain_display_entry_t;
STATIC_ASSERT(sizeof(spell_chain_display_entry_t) == 0xac, spell_chain_display_entry_t_wrong_size);

typedef struct
{
  HWND spell_window;
  HWND target_windows[20];
  int target_count;
} spell_chain_window_entry_t;
STATIC_ASSERT(sizeof(spell_chain_window_entry_t) == 0x58, spell_chain_window_entry_t_wrong_size);

extern char global_base_directory[];
extern HINSTANCE g_app_instance;
extern HWND DAT_008a8dec;
extern int g_showlist_smallcard_width;
extern int g_showlist_smallcard_height;
extern spell_chain_display_entry_t DAT_008b1140[32];

int load_text_with_tab_escapes(char *filename, char *section_name);
HBITMAP load_pic(char *filename);
void ApplyCardArtPaletteToDc(HDC dc);
void TileBitmapIntoRect(HDC dc, RECT *rect, HBITMAP bitmap);
int handle_button_palette_message(int hwnd, unsigned int msg, int wparam, int lparam);
int card_window_matches_card_id(HWND hwnd, card_id_t card_id);
int card_window_matches_player_and_card(HWND hwnd, int *player_and_card);
void layout_phase_display_window(HWND hwnd, LPRECT rect);
int handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void restack_duel_child_windows(void);
void delete_and_close_object(HANDLE obj);

// GLOBAL: MAGIC 0x0055e03c
// GLOBAL: SHANDALAR 0x0057f0d0
int g_spell_chain_windows_long_offset = 0;

// GLOBAL: MAGIC 0x0055e040
// GLOBAL: SHANDALAR 0x0057f0d4
int g_spell_chain_count_long_offset = 4;

// GLOBAL: MAGIC 0x0055e044
// GLOBAL: SHANDALAR 0x0057f0d8
int g_spell_chain_window_extra_bytes = 8;

// GLOBAL: MAGIC 0x00570f64
// GLOBAL: SHANDALAR 0x00589be0
int g_spell_chain_scrollbar_direction = 3;

// GLOBAL: MAGIC 0x00637e58
// GLOBAL: SHANDALAR 0x005b7ba8
int g_spell_minimized_hwnd;

// GLOBAL: MAGIC 0x00637e60
// GLOBAL: SHANDALAR 0x005b7bb0
char g_spell_chain_menu_minimize_text[0x1c];

// GLOBAL: MAGIC 0x00637e7c
// GLOBAL: SHANDALAR 0x005b7bcc
COLORREF g_spell_chain_title_text_color;

// GLOBAL: MAGIC 0x00637e80
// GLOBAL: SHANDALAR 0x005b7bd0
HPEN g_spell_chain_shadow_pen;

// GLOBAL: MAGIC 0x00637e88
// GLOBAL: SHANDALAR 0x005b7bd8
char g_spell_chain_menu_help_text[0x20];

// GLOBAL: MAGIC 0x00637ea8
// GLOBAL: SHANDALAR 0x005b7bf8
char g_spell_minimized_menu_help_text[0x1c];

// GLOBAL: MAGIC 0x00637ec4
// GLOBAL: SHANDALAR 0x005b7c14
HMENU g_spell_minimized_popup_menu;

// GLOBAL: MAGIC 0x00637ec8
// GLOBAL: SHANDALAR 0x005b7c18
HPEN g_spell_chain_dark_pen;

// GLOBAL: MAGIC 0x00637ed0
// GLOBAL: SHANDALAR 0x005b7c20
RECT g_spell_chain_layout_rect;

// GLOBAL: MAGIC 0x00637ee0
// GLOBAL: SHANDALAR 0x005b7c30
HBITMAP g_spell_chain_scrollbar_track_bitmap;

// GLOBAL: MAGIC 0x00637ee4
// GLOBAL: SHANDALAR 0x005b7c34
HBITMAP g_spell_chain_scrollbar_thumb_bitmap;

// GLOBAL: MAGIC 0x00637ee8
// GLOBAL: SHANDALAR 0x005b7c38
HBITMAP g_spell_minimized_background_bitmap;

// GLOBAL: MAGIC 0x00637ef0
// GLOBAL: SHANDALAR 0x005b7c40
char g_spell_minimized_menu_restore_text[0x1c];

// GLOBAL: MAGIC 0x00637f0c
// GLOBAL: SHANDALAR 0x005b7c5c
HBITMAP g_spell_chain_background_bitmap;

// GLOBAL: MAGIC 0x00637f10
// GLOBAL: SHANDALAR 0x005b7c60
HBRUSH g_spell_chain_title_brush;

// GLOBAL: MAGIC 0x00637f14
// GLOBAL: SHANDALAR 0x005b7c64
HPEN g_spell_chain_highlight_pen;

// FUNCTION: MAGIC 0x0044a08e
// FUNCTION: SHANDALAR 0x00453c0f
int copy_spell_chain_display_entries(spell_chain_display_entry_t *entries)
{
  int count;

  if (entries == NULL)
  {
    return 0;
  }

  EnterCriticalSection(&g_duel_render_lock);
  count = g_multiblock_creature_count;
  memcpy(entries, DAT_008b1140, sizeof(DAT_008b1140));
  LeaveCriticalSection(&g_duel_render_lock);

  return count;
}

// FUNCTION: MAGIC 0x00486050
// FUNCTION: SHANDALAR 0x004c8c30
int register_MAGICGAME_SpellChainClass(LPCSTR class_name)
{
  struct
  {
    char path[264];
    int registered;
    WNDCLASSA wndclass;
  } s;

  s.registered = 1;
  SET_DUEL_WNDCLASS(s.wndclass, 0x800, wndproc_MAGICGAME_SpellChainClass,
                    g_spell_chain_window_extra_bytes, (HICON)0, (HBRUSH)0x6, class_name);
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.registered = 0;
  }

  SET_DUEL_WNDCLASS(s.wndclass, 3, wndproc_SpellMinimized, 0, (HICON)0, (HBRUSH)0x6, "SpellMinimized");
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.registered = 0;
  }

  g_spell_minimized_popup_menu = CreatePopupMenu();
  load_text_with_tab_escapes(global_ui_strings_filename, "MENU_SPELLCHAIN");
  strcpy(g_spell_chain_menu_minimize_text, text_lines[0]);
  strcpy(g_spell_chain_menu_help_text, text_lines[1]);
  load_text_with_tab_escapes(global_ui_strings_filename, "MENU_MINIMIZEDSPELLCHAIN");
  strcpy(g_spell_minimized_menu_restore_text, text_lines[0]);
  strcpy(g_spell_minimized_menu_help_text, text_lines[1]);

  strcpy(s.path, global_duelart_path);
  strcat(s.path, "\\WINBK_SpellChain.pic");
  g_spell_chain_background_bitmap = load_pic(s.path);
  strcpy(s.path, global_duelart_path);
  strcat(s.path, "\\WINBK_SpellMushrooms.pic");
  g_spell_chain_scrollbar_thumb_bitmap = load_pic(s.path);
  strcpy(s.path, global_duelart_path);
  strcat(s.path, "\\WINBK_SpellSnails.pic");
  g_spell_chain_scrollbar_track_bitmap = load_pic(s.path);
  g_spell_chain_scrollbar_direction = 3;
  strcpy(s.path, global_duelart_path);
  strcat(s.path, "\\WINBK_SpellMin.pic");
  g_spell_minimized_background_bitmap = load_pic(s.path);

  SetRect(&g_spell_chain_layout_rect, 0, 0, 0, 0);
  g_spell_chain_shadow_pen = CreatePen(0, 0, 0x1000040);
  g_spell_chain_highlight_pen = CreatePen(0, 0, 0x1000037);
  g_spell_chain_dark_pen = CreatePen(0, 0, 0x1000016);
  g_spell_chain_title_brush = CreateSolidBrush(0x1000037);
  g_spell_chain_title_text_color = 0x1000048;
  if (g_spell_chain_shadow_pen == (HPEN)0 || g_spell_chain_highlight_pen == (HPEN)0 ||
      g_spell_chain_dark_pen == (HPEN)0 || g_spell_chain_title_brush == (HBRUSH)0)
  {
    s.registered = 0;
  }

  return s.registered;
}

// FUNCTION: MAGIC 0x00486369
// FUNCTION: SHANDALAR 0x004c8f49
void destroy_MAGICGAME_SpellChainClass(LPCSTR class_name)
{
  if (g_spell_minimized_popup_menu != (HMENU)0)
  {
    DestroyMenu(g_spell_minimized_popup_menu);
  }
  g_spell_minimized_popup_menu = (HMENU)0;
  if (g_spell_chain_background_bitmap != (HBITMAP)0)
  {
    delete_and_close_object(g_spell_chain_background_bitmap);
  }
  if (g_spell_chain_scrollbar_thumb_bitmap != (HBITMAP)0)
  {
    delete_and_close_object(g_spell_chain_scrollbar_thumb_bitmap);
  }
  if (g_spell_chain_scrollbar_track_bitmap != (HBITMAP)0)
  {
    delete_and_close_object(g_spell_chain_scrollbar_track_bitmap);
  }
  if (g_spell_minimized_background_bitmap != (HBITMAP)0)
  {
    delete_and_close_object(g_spell_minimized_background_bitmap);
  }
  if (g_spell_chain_shadow_pen != (HPEN)0)
  {
    DeleteObject(g_spell_chain_shadow_pen);
  }
  if (g_spell_chain_highlight_pen != (HPEN)0)
  {
    DeleteObject(g_spell_chain_highlight_pen);
  }
  if (g_spell_chain_dark_pen != (HPEN)0)
  {
    DeleteObject(g_spell_chain_dark_pen);
  }
  if (g_spell_chain_title_brush != (HBRUSH)0)
  {
    DeleteObject(g_spell_chain_title_brush);
  }
  g_spell_chain_background_bitmap = (HBITMAP)0;
  g_spell_chain_scrollbar_thumb_bitmap = (HBITMAP)0;
  g_spell_chain_scrollbar_track_bitmap = (HBITMAP)0;
  g_spell_minimized_background_bitmap = (HBITMAP)0;
  g_spell_chain_shadow_pen = (HPEN)0;
  g_spell_chain_highlight_pen = (HPEN)0;
  g_spell_chain_dark_pen = (HPEN)0;
  g_spell_chain_title_brush = (HBRUSH)0;
}

// FUNCTION: MAGIC 0x00488229
// FUNCTION: SHANDALAR 0x004cadfe
int find_spell_chain_entry(HWND hwnd, spell_chain_display_entry_t entry, int start_index)
{
  struct
  {
    spell_chain_window_entry_t *windows;
    int result;
    int i;
    int window_count;
  } s;

  if (hwnd == (HWND)0)
  {
    return -1;
  }

  s.windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
  s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
  s.result = -1;
  for (s.i = start_index; s.i < s.window_count && s.result == -1; ++s.i)
  {
    if (card_window_matches_player_and_card(s.windows[s.i].spell_window, &entry.player) != 0)
    {
      s.result = s.i;
    }
  }
  return s.result;
}

// FUNCTION: MAGIC 0x004882d8
// FUNCTION: SHANDALAR 0x004caead
void remove_spell_chain_entry(HWND hwnd, int entry_index)
{
  struct
  {
    spell_chain_window_entry_t *windows;
    int target_index;
    int i;
    int window_count;
  } s;

  if (hwnd == (HWND)0)
  {
    return;
  }
  if (entry_index < 0 || entry_index > 0x64)
  {
    return;
  }

  s.windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
  s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
  DestroyWindow(s.windows[entry_index].spell_window);
  for (s.target_index = 0; s.target_index < s.windows[entry_index].target_count; ++s.target_index)
  {
    if (s.windows[entry_index].target_windows[s.target_index] != (HWND)0)
    {
      DestroyWindow(s.windows[entry_index].target_windows[s.target_index]);
    }
  }
  for (s.i = entry_index; s.i < s.window_count - 1; ++s.i)
  {
    memcpy(&s.windows[s.i], &s.windows[s.i + 1], sizeof(s.windows[s.i]));
  }
  --s.window_count;
  SetWindowLongA(hwnd, g_spell_chain_count_long_offset, s.window_count);
}

// FUNCTION: MAGIC 0x00488425
// FUNCTION: SHANDALAR 0x004caffa
int spell_chain_entry_matches(spell_chain_window_entry_t window_entry, spell_chain_display_entry_t display_entry)
{
  struct
  {
    int matches;
    int i;
  } s;
  s.matches = 1;

  if (window_entry.target_count != display_entry.number_of_targets)
  {
    s.matches = 0;
  }

  for (s.i = 0; !(window_entry.target_count <= s.i); ++s.i)
  {
    if (card_window_matches_player_and_card(window_entry.target_windows[s.i], &display_entry.targets[s.i].player) == 0)
    {
      s.matches = 0;
    }
  }
  return s.matches;
}

// FUNCTION: MAGIC 0x004884a2
// FUNCTION: SHANDALAR 0x004cb077
int add_spell_chain_entry(HWND hwnd, spell_chain_display_entry_t display_entry, int insert_index)
{
  struct
  {
    target_t create_target;
    int result;
    int target_index;
    int shift_index;
    int window_count;
    HWND spell_window;
    HWND target_windows[20];
    int created_target_count;
  } s;
  spell_chain_window_entry_t *windows;

  if (hwnd == (HWND)0)
  {
    return 0;
  }

  windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
  s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
  if (s.window_count == 100)
  {
    return 0;
  }

  s.result = 1;
  s.create_target.player = display_entry.player;
  s.create_target.card = display_entry.card;
  s.spell_window = CreateWindowExA(0, "MAGICGAME_CardClass", "Spell Card", 0x50000000,
                                   0, 0, 0, 0, hwnd, (HMENU)1, g_app_instance, &s.create_target);
  if (s.spell_window == (HWND)0)
  {
    s.result = 0;
  }

  s.created_target_count = 0;
  for (s.target_index = 0; s.target_index < display_entry.number_of_targets && s.result != 0; ++s.target_index)
  {
    s.create_target = display_entry.targets[s.target_index];
    s.target_windows[s.target_index] =
        CreateWindowExA(0, "MAGICGAME_CardClass", "Spell Target Card", 0x50000000,
                        0, 0, 0, 0, hwnd, (HMENU)1, g_app_instance, &s.create_target);
    if (s.target_windows[s.target_index] == (HWND)0)
    {
      s.result = 0;
    }
    else
    {
      ++s.created_target_count;
    }
  }

  if (s.result != 0)
  {
    for (s.shift_index = insert_index; s.shift_index < s.window_count; ++s.shift_index)
    {
      memcpy(&windows[s.shift_index + 1], &windows[s.shift_index], sizeof(windows[s.shift_index]));
    }
    memcpy(&windows[insert_index], &s.spell_window, sizeof(windows[insert_index]));
    ++s.window_count;
    SetWindowLongA(hwnd, g_spell_chain_count_long_offset, s.window_count);
  }
  else
  {
    if (s.spell_window != (HWND)0)
    {
      DestroyWindow(s.spell_window);
    }
    for (s.target_index = 0; s.target_index < s.created_target_count; ++s.target_index)
    {
      if (s.target_windows[s.target_index] != (HWND)0)
      {
        DestroyWindow(s.target_windows[s.target_index]);
      }
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004886db
// FUNCTION: SHANDALAR 0x004cb2b0
void destroy_spell_chain_target_windows(HWND hwnd, int entry_index)
{
  struct
  {
    spell_chain_window_entry_t *windows;
    int target_index;
    int window_count;
  } s;

  if (hwnd == (HWND)0)
  {
    return;
  }
  if (entry_index < 0 || entry_index > 0x64)
  {
    return;
  }

  s.windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
  s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
  for (s.target_index = 0; s.target_index < s.windows[entry_index].target_count; ++s.target_index)
  {
    if (s.windows[entry_index].target_windows[s.target_index] != (HWND)0)
    {
      DestroyWindow(s.windows[entry_index].target_windows[s.target_index]);
    }
  }
  s.windows[entry_index].target_count = 0;
}

// FUNCTION: MAGIC 0x004887c0
// FUNCTION: SHANDALAR 0x004cb395
int update_spell_chain_entry_targets(HWND hwnd, spell_chain_display_entry_t display_entry, int entry_index)
{
  struct
  {
    target_t create_target;
    int result;
    int target_index;
    int window_count;
    HWND spell_window;
    HWND target_windows[20];
    int created_target_count;
  } s;
  spell_chain_window_entry_t *windows;

  if (hwnd == (HWND)0)
  {
    return 0;
  }

  windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
  s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
  s.result = 1;
  s.spell_window = windows[entry_index].spell_window;
  s.created_target_count = 0;
  for (s.target_index = 0; s.target_index < display_entry.number_of_targets && s.result != 0; ++s.target_index)
  {
    s.create_target = display_entry.targets[s.target_index];
    s.target_windows[s.target_index] =
        CreateWindowExA(0, "MAGICGAME_CardClass", "Spell Target Card", 0x50000000,
                        0, 0, 0, 0, hwnd, (HMENU)1, g_app_instance, &s.create_target);
    if (s.target_windows[s.target_index] == (HWND)0)
    {
      s.result = 0;
    }
    else
    {
      ++s.created_target_count;
    }
  }

  if (s.result != 0)
  {
    memcpy(&windows[entry_index], &s.spell_window, sizeof(windows[entry_index]));
  }
  else
  {
    for (s.target_index = 0; s.target_index < s.created_target_count; ++s.target_index)
    {
      if (s.target_windows[s.target_index] != (HWND)0)
      {
        DestroyWindow(s.target_windows[s.target_index]);
      }
    }
    windows[entry_index].target_count = 0;
  }

  return s.result;
}

static __inline void draw_spell_chain_frame(HWND hwnd, HDC dc)
{
  struct
  {
    RECT window_rect;
    RECT client_rect;
    RECT title_rect;
    RECT button_rect;
    char title[100];
    BITMAP button_bitmap;
    HBITMAP close_button;
    int client_border_width;
    int bottom_border_height;
    int x;
    int y;
    int i;
  } s;

  GetWindowRect(hwnd, &s.window_rect);
  GetClientRect(hwnd, &s.client_rect);
  MapWindowPoints(hwnd, (HWND)0, (LPPOINT)&s.client_rect, 2);
  OffsetRect(&s.client_rect, -s.window_rect.left, -s.window_rect.top);
  OffsetRect(&s.window_rect, -s.window_rect.left, -s.window_rect.top);
  GetWindowTextA(hwnd, s.title, 100);
  s.client_border_width = s.window_rect.right - s.client_rect.right;
  s.bottom_border_height = s.window_rect.bottom - s.client_rect.bottom;

  SelectObject(dc, g_spell_chain_highlight_pen);
  MoveToEx(dc, 0, 0, NULL);
  LineTo(dc, s.window_rect.right - 1, 0);
  SelectObject(dc, g_spell_chain_shadow_pen);
  s.y = 1;
  for (s.i = 1; s.i <= s.bottom_border_height - 2; ++s.i)
  {
    MoveToEx(dc, 1, s.y, NULL);
    LineTo(dc, s.window_rect.right - s.client_border_width + 1, s.y);
    ++s.y;
  }
  SelectObject(dc, g_spell_chain_highlight_pen);
  s.y = s.bottom_border_height - 1;
  MoveToEx(dc, s.client_border_width - 1, s.y, NULL);
  LineTo(dc, s.client_rect.right + 1, s.y);

  SelectObject(dc, g_spell_chain_highlight_pen);
  s.x = 0;
  MoveToEx(dc, 0, 0, NULL);
  LineTo(dc, s.x, s.window_rect.bottom - 1);
  SelectObject(dc, g_spell_chain_shadow_pen);
  s.x = 1;
  for (s.i = 1; s.i <= s.client_border_width - 2; ++s.i)
  {
    MoveToEx(dc, s.x, 1, NULL);
    LineTo(dc, s.x, s.window_rect.bottom - 1);
    ++s.x;
  }
  SelectObject(dc, g_spell_chain_highlight_pen);
  s.x = s.client_rect.left - 1;
  MoveToEx(dc, s.x, s.bottom_border_height - 1, NULL);
  LineTo(dc, s.x, s.client_rect.bottom + 1);

  SelectObject(dc, GetStockObject(BLACK_PEN));
  s.x = s.window_rect.right - 1;
  MoveToEx(dc, s.x, 0, NULL);
  LineTo(dc, s.x, s.window_rect.bottom);
  SelectObject(dc, g_spell_chain_dark_pen);
  s.x = s.window_rect.right - 2;
  for (s.i = 1; s.i <= s.client_border_width - 2; ++s.i)
  {
    MoveToEx(dc, s.x, 1, NULL);
    LineTo(dc, s.x, s.window_rect.bottom - 1);
    --s.x;
  }
  SelectObject(dc, g_spell_chain_highlight_pen);
  s.x = s.client_rect.right;
  MoveToEx(dc, s.client_rect.right, s.bottom_border_height - 1, NULL);
  LineTo(dc, s.x, s.client_rect.bottom + 1);

  SelectObject(dc, GetStockObject(BLACK_PEN));
  s.y = s.window_rect.bottom - 1;
  MoveToEx(dc, 0, s.y, NULL);
  LineTo(dc, s.window_rect.right, s.y);
  SelectObject(dc, g_spell_chain_dark_pen);
  s.y = s.window_rect.bottom - 2;
  for (s.i = 1; s.i <= s.bottom_border_height - 2; ++s.i)
  {
    MoveToEx(dc, 1, s.y, NULL);
    LineTo(dc, s.window_rect.right - 1, s.y);
    --s.y;
  }
  SelectObject(dc, g_spell_chain_highlight_pen);
  s.y = s.window_rect.bottom - s.bottom_border_height;
  MoveToEx(dc, s.client_border_width - 1, s.y, NULL);
  LineTo(dc, s.window_rect.right - 2, s.y);
  SelectObject(dc, g_spell_chain_highlight_pen);
  s.y = s.client_rect.top - 1;
  MoveToEx(dc, s.client_rect.left, s.y, NULL);
  LineTo(dc, s.client_rect.right + 1, s.y);

  SetRect(&s.title_rect, s.client_rect.left, s.bottom_border_height, s.client_rect.right, s.client_rect.top - 1);
  FillRect(dc, &s.title_rect, g_spell_chain_title_brush);
  SetTextColor(dc, g_spell_chain_title_text_color);
  SetBkMode(dc, TRANSPARENT);
  s.title_rect.left += 5;
  DrawTextA(dc, s.title, -1, &s.title_rect, DT_SINGLELINE | DT_VCENTER);

  s.close_button = LoadBitmapA((HINSTANCE)0, (LPCSTR)0x7fed);
  GetObjectA(s.close_button, sizeof(s.button_bitmap), &s.button_bitmap);
  SetRect(&s.button_rect, s.client_rect.right - s.button_bitmap.bmWidth, s.client_rect.top - s.button_bitmap.bmHeight,
          s.client_rect.right, s.client_rect.top);
  DrawBitmapToRect(dc, &s.button_rect, g_spell_minimized_background_bitmap);
  DeleteObject(s.close_button);
}

// FUNCTION: MAGIC 0x004864b7
// FUNCTION: SHANDALAR 0x004c9097
LRESULT CALLBACK wndproc_MAGICGAME_SpellChainClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    char stack_padding[0x70];
    int menu_item_count;
    POINT popup_point;
    RECT popup_rect;
    UINT scrollbar_new_pos;
    char background_path[264];
    HDC dc;
    BITMAP bitmap;
    int scrollbar_height;
    int scrollbar_y;
    RECT client_rect;
    RECT tile_rect;
    LRESULT scroll_pos;
    HWND scrollbar_hwnd;
    RECT scroll_rect;
    RECT window_rect;
    int hit_border_width;
    LRESULT hit_test;
    int paint_result;
    HWND create_scrollbar_hwnd;
    char help_path[264];
    DWORD help_topic;
    int minimized_height;
    BITMAP minimized_bitmap;
    int minimized_width;
    int minimized_y;
    int minimized_x;
    RECT face_rect;
    int clear_target_index;
    int clear_entry_index;
    int sync_existing_index;
    int sync_target_index;
    int sync_j;
    int sync_changed;
    spell_chain_display_entry_t display_entries[32];
    int sync_count;
    int changed;
    int existing_index;
    int j;
    LPARAM message_lparam;
    HWND child_hwnd;
    int min_scroll_pos;
    int max_scroll_pos;
    int current_scroll_pos;
    int scroll_step;
    WPARAM card_id;
    int target_index;
    int i;
    spell_chain_window_entry_t *windows;
    LONG window_count;
  } s;

  switch (msg)
  {
  case 0x437:
    return 0;

  case 0x433:
  case 0x434:
    s.windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
    s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
    s.card_id = wparam;
    for (s.i = 0; s.i < s.window_count; ++s.i)
    {
      if (card_window_matches_card_id(s.windows[s.i].spell_window, s.card_id) != 0)
      {
        InvalidateRect(s.windows[s.i].spell_window, NULL, FALSE);
      }
      for (s.target_index = 0; s.target_index < s.windows[s.i].target_count; ++s.target_index)
      {
        if (card_window_matches_card_id((&s.windows[s.i])->target_windows[s.target_index], s.card_id) != 0)
        {
          InvalidateRect((&s.windows[s.i])->target_windows[s.target_index], NULL, FALSE);
        }
      }
    }
    return 0;

  case 0x432:
    s.windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
    s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
    for (s.scroll_step = 0; s.scroll_step < s.window_count; ++s.scroll_step)
    {
      SendMessageA(s.windows[s.scroll_step].spell_window, 0x432, 0, 0);
      for (s.current_scroll_pos = 0; s.current_scroll_pos < s.windows[s.scroll_step].target_count; ++s.current_scroll_pos)
      {
        SendMessageA((&s.windows[s.scroll_step])->target_windows[s.current_scroll_pos], 0x432, 0, 0);
      }
    }
    return 0;

  case 0x435:
    s.windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
    s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
    for (s.max_scroll_pos = 0; !(s.window_count <= s.max_scroll_pos); ++s.max_scroll_pos)
    {
      InvalidateRect(s.windows[s.max_scroll_pos].spell_window, NULL, FALSE);
      for (s.min_scroll_pos = 0; s.min_scroll_pos < s.windows[s.max_scroll_pos].target_count; ++s.min_scroll_pos)
      {
        InvalidateRect((&s.windows[s.max_scroll_pos])->target_windows[s.min_scroll_pos], NULL, FALSE);
      }
    }
    return 0;

  case 0x436:
    s.windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
    s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
    s.existing_index = wparam;
    s.changed = lparam;
    if (s.existing_index == 0)
    {
      return 0;
    }
    s.child_hwnd = 0;
    for (s.message_lparam = 0; s.message_lparam < s.window_count && s.child_hwnd == 0; ++s.message_lparam)
    {
      if (card_window_matches_player_and_card(s.windows[s.message_lparam].spell_window, (int *)s.existing_index) != 0)
      {
        s.child_hwnd = (HWND)1;
        if (s.changed != 0)
        {
          SendMessageA(s.windows[s.message_lparam].spell_window, 0x432, 0, 0);
        }
        else
        {
          InvalidateRect(s.windows[s.message_lparam].spell_window, NULL, FALSE);
        }
      }
      for (s.j = 0; s.j < s.windows[s.message_lparam].target_count; ++s.j)
      {
        if (card_window_matches_player_and_card((&s.windows[s.message_lparam])->target_windows[s.j], (int *)s.existing_index) != 0)
        {
          s.child_hwnd = (HWND)1;
          if (s.changed != 0)
          {
            SendMessageA((&s.windows[s.message_lparam])->target_windows[s.j], 0x432, 0, 0);
          }
          else
          {
            InvalidateRect((&s.windows[s.message_lparam])->target_windows[s.j], NULL, FALSE);
          }
        }
      }
    }
    return 0;

  case 0x412:
    s.windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
    s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
    s.window_count = s.window_count;
    s.sync_count = copy_spell_chain_display_entries(s.display_entries);
    s.sync_changed = 0;
    for (s.sync_j = 0; !(s.sync_count <= s.sync_j); ++s.sync_j)
    {
      s.sync_existing_index = find_spell_chain_entry(hwnd, s.display_entries[s.sync_j], s.sync_j);
      if (s.sync_existing_index == s.sync_j)
      {
        if (spell_chain_entry_matches(s.windows[s.sync_existing_index], s.display_entries[s.sync_existing_index]) == 0)
        {
          destroy_spell_chain_target_windows(hwnd, s.sync_existing_index);
          update_spell_chain_entry_targets(hwnd, s.display_entries[s.sync_existing_index], s.sync_existing_index);
          s.sync_changed = 1;
        }
      }
      else if (s.sync_existing_index == -1)
      {
        add_spell_chain_entry(hwnd, s.display_entries[s.sync_j], s.sync_j);
        s.sync_changed = 1;
      }
      else if (!(s.sync_existing_index <= s.sync_j))
      {
        for (s.sync_target_index = s.sync_j; !(s.sync_existing_index <= s.sync_target_index); ++s.sync_target_index)
        {
          remove_spell_chain_entry(hwnd, s.sync_target_index);
        }
        s.sync_changed = 1;
      }
    }
    s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
    for (s.sync_target_index = s.sync_j; !(s.window_count <= s.sync_target_index); ++s.sync_target_index)
    {
      remove_spell_chain_entry(hwnd, s.sync_target_index);
      s.sync_changed = 1;
    }
    if (s.sync_changed != 0)
    {
      layout_phase_display_window(hwnd, &g_spell_chain_layout_rect);
    }
    return 0;

  case 0x40c:
    s.windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
    s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
    ShowWindow(hwnd, SW_HIDE);
    ShowWindow((HWND)g_spell_minimized_hwnd, SW_HIDE);
    for (s.clear_entry_index = 0; !(s.window_count <= s.clear_entry_index); ++s.clear_entry_index)
    {
      DestroyWindow(s.windows[s.clear_entry_index].spell_window);
      for (s.clear_target_index = 0; !(s.windows[s.clear_entry_index].target_count <= s.clear_target_index); ++s.clear_target_index)
      {
        DestroyWindow(s.windows[s.clear_entry_index].target_windows[s.clear_target_index]);
      }
      s.windows[s.clear_entry_index].target_count = 0;
    }
    s.window_count = 0;
    SetWindowLongA(hwnd, g_spell_chain_count_long_offset, s.window_count);
    SendMessageA(GetDlgItem(hwnd, 0), 0xe0, 0, TRUE);
    layout_phase_display_window(hwnd, &g_spell_chain_layout_rect);
    return 0;

  case WM_CLOSE:
    ShowWindow(hwnd, SW_HIDE);
    ShowWindow((HWND)g_spell_minimized_hwnd, SW_HIDE);
    return 0;

  case WM_COMMAND:
    switch (wparam & 0xffff)
    {
    case 0x65:
      ShowWindow(hwnd, SW_HIDE);
      UpdateWindow(g_duel_help_owner_hwnd);
      GetWindowRect(DAT_008a8dec, &s.face_rect);
      s.minimized_x = s.face_rect.left;
      s.minimized_width = s.face_rect.right - s.face_rect.left;
      if (g_spell_minimized_background_bitmap != (HBITMAP)0)
      {
        GetObjectA(g_spell_minimized_background_bitmap, sizeof(s.minimized_bitmap), &s.minimized_bitmap);
        s.minimized_height = (s.minimized_bitmap.bmHeight * s.minimized_width) / s.minimized_bitmap.bmWidth;
      }
      else
      {
        s.minimized_height = s.minimized_width * 2;
      }
      s.minimized_y = (s.face_rect.bottom - s.face_rect.top) / 2 - s.minimized_height / 2;
      MoveWindow((HWND)g_spell_minimized_hwnd, s.minimized_x, s.minimized_y, s.minimized_width, s.minimized_height, TRUE);
      ShowWindow((HWND)g_spell_minimized_hwnd, SW_SHOW);
      BringWindowToTop((HWND)g_spell_minimized_hwnd);
      SendMessageA(g_duel_prompt_context_hwnd, 0x403, 0, 0);
      break;

    case 0x66:
      ShowWindow((HWND)g_spell_minimized_hwnd, SW_HIDE);
      ShowWindow(hwnd, SW_SHOW);
      SendMessageA(g_duel_prompt_context_hwnd, 0x403, 0, 0);
      restack_duel_child_windows();
      break;

    case 100:
      s.help_topic = 0xbd2;
      strcpy(s.help_path, global_base_directory);
      strcat(s.help_path, "\\duel.hlp");
      WinHelpA(g_duel_window_hwnd, s.help_path, HELP_CONTEXT, s.help_topic);
      break;
    }
    return 0;

  case WM_CREATE:
    s.window_count = 0;
    SetWindowLongA(hwnd, g_spell_chain_count_long_offset, s.window_count);
    s.windows = (spell_chain_window_entry_t *)malloc(0x2260);
    SetWindowLongA(hwnd, g_spell_chain_windows_long_offset, (LONG)s.windows);
    s.create_scrollbar_hwnd = CreateWindowExA(0, "MAGICGAME_ScrollbarClass", "", WS_CHILD | WS_VISIBLE,
                                              0, 0, 0, 0, hwnd, (HMENU)0, g_app_instance, NULL);
    if (s.create_scrollbar_hwnd != (HWND)0)
    {
      SendMessageA(s.create_scrollbar_hwnd, 0x464, (WPARAM)g_spell_chain_scrollbar_thumb_bitmap, 0);
      SendMessageA(s.create_scrollbar_hwnd, 0x466, (WPARAM)g_spell_chain_scrollbar_track_bitmap, g_spell_chain_scrollbar_direction);
    }
    g_spell_minimized_hwnd = (int)CreateWindowExA(0, "SpellMinimized", "", WS_POPUP,
                                                  0, 0, 0, 0, hwnd, (HMENU)0, g_app_instance, NULL);
    if (s.windows == NULL || s.create_scrollbar_hwnd == (HWND)0 || (HWND)g_spell_minimized_hwnd == (HWND)0)
    {
      if (s.windows != NULL)
      {
        free(s.windows);
      }
      return -1;
    }
    return 0;

  case WM_DESTROY:
    s.windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
    free(s.windows);
    return 0;

  case WM_ERASEBKGND:
    s.dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.dc);
    GetClientRect(hwnd, &s.client_rect);
    s.scroll_pos = SendDlgItemMessageA(hwnd, 0, 0xe1, 0, 0);
    if (g_spell_chain_background_bitmap == (HBITMAP)0)
    {
      strcpy(s.background_path, global_duelart_path);
      strcat(s.background_path, "\\WINBK_SpellChain.pic");
      g_spell_chain_background_bitmap = load_pic(s.background_path);
    }
    if (g_spell_chain_background_bitmap != (HBITMAP)0)
    {
      CopyRect(&s.tile_rect, &s.client_rect);
      GetObjectA(g_spell_chain_background_bitmap, sizeof(s.bitmap), &s.bitmap);
      s.tile_rect.left = -(s.scroll_pos % s.bitmap.bmWidth);
      TileBitmapIntoRect(s.dc, &s.tile_rect, g_spell_chain_background_bitmap);
    }
    else
    {
      FillRect(s.dc, &s.client_rect, GetStockObject(GRAY_BRUSH));
    }
    return 1;

  case WM_HSCROLL:
    s.scrollbar_hwnd = GetDlgItem(hwnd, 0);
    SendMessageA(s.scrollbar_hwnd, 0xe3, (WPARAM)&s.min_scroll_pos, (LPARAM)&s.max_scroll_pos);
    s.current_scroll_pos = SendMessageA(s.scrollbar_hwnd, 0xe1, 0, 0);
    s.scroll_step = g_showlist_smallcard_width;
    GetClientRect(hwnd, &s.client_rect);
    switch (wparam & 0xffff)
    {
    case SB_LINELEFT:
      s.scrollbar_new_pos = s.current_scroll_pos - s.scroll_step;
      break;
    case SB_LINERIGHT:
      s.scrollbar_new_pos = s.current_scroll_pos + s.scroll_step;
      break;
    case SB_PAGELEFT:
      s.scrollbar_new_pos = s.current_scroll_pos - s.client_rect.right;
      break;
    case SB_PAGERIGHT:
      s.scrollbar_new_pos = s.current_scroll_pos + s.client_rect.right;
      break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
      s.scrollbar_new_pos = HIWORD(wparam);
      break;
    case SB_LEFT:
      s.scrollbar_new_pos = s.min_scroll_pos;
      break;
    case SB_RIGHT:
      s.scrollbar_new_pos = s.max_scroll_pos;
      break;
    default:
      s.scrollbar_new_pos = s.current_scroll_pos;
      break;
    }
    if ((int)s.scrollbar_new_pos < s.min_scroll_pos)
    {
      s.scrollbar_new_pos = s.min_scroll_pos;
    }
    if (s.max_scroll_pos < (int)s.scrollbar_new_pos)
    {
      s.scrollbar_new_pos = s.max_scroll_pos;
    }
    if (s.scrollbar_new_pos != (UINT)s.current_scroll_pos)
    {
      SendMessageA(s.scrollbar_hwnd, 0xe0, s.scrollbar_new_pos, TRUE);
      GetWindowRect(s.scrollbar_hwnd, &s.scroll_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.scroll_rect, 2);
      ScrollWindow(hwnd, s.current_scroll_pos - s.scrollbar_new_pos, 0, NULL, NULL);
      MoveWindow(s.scrollbar_hwnd, s.scroll_rect.left, s.scroll_rect.top,
                 s.scroll_rect.right - s.scroll_rect.left, s.scroll_rect.bottom - s.scroll_rect.top, FALSE);
      UpdateWindow(hwnd);
    }
    return 0;

  case WM_LBUTTONDOWN:
    return 0;

  case WM_NCHITTEST:
    s.hit_test = DefWindowProcA(hwnd, msg, wparam, lparam);
    if (s.hit_test != HTCAPTION)
    {
      return s.hit_test;
    }
    s.hit_border_width = GetSystemMetrics(SM_CYSIZEFRAME);
    GetClientRect(hwnd, &s.client_rect);
    MapWindowPoints(hwnd, (HWND)0, (LPPOINT)&s.client_rect, 2);
    return HTLEFT;

  case WM_NCLBUTTONDOWN:
    if (wparam == HTLEFT)
    {
      SendMessageA(hwnd, WM_COMMAND, 0x65, 0);
      return 0;
    }
    return DefWindowProcA(hwnd, msg, wparam, lparam);

  case WM_NCPAINT:
  case WM_NCACTIVATE:
    GetWindowRect(hwnd, &s.window_rect);
    OffsetRect(&s.window_rect, -s.window_rect.left, -s.window_rect.top);
    if (s.window_rect.right == s.window_rect.left || s.window_rect.bottom == s.window_rect.top)
    {
      return DefWindowProcA(hwnd, msg, wparam, lparam);
    }

    s.paint_result = msg != WM_NCPAINT;
    s.dc = GetWindowDC(hwnd);
    if (s.dc == (HDC)0)
    {
      return s.paint_result;
    }
    ApplyCardArtPaletteToDc(s.dc);
    draw_spell_chain_frame(hwnd, s.dc);
    ReleaseDC(hwnd, s.dc);
    return s.paint_result;

  case WM_SETCURSOR:
    return handle_duel_inactive_cursor(hwnd, msg, wparam, lparam);

  case WM_QUERYDRAGICON:
    PostMessageA(g_duel_prompt_context_hwnd, 0x403, 0, 0);
    return DefWindowProcA(hwnd, msg, wparam, lparam);

  case WM_SIZE:
    GetClientRect(hwnd, &s.client_rect);
    if (g_spell_chain_scrollbar_thumb_bitmap == (HBITMAP)0)
    {
      s.scrollbar_height = GetSystemMetrics(SM_CYHSCROLL) * 2;
    }
    else
    {
      GetObjectA(g_spell_chain_scrollbar_thumb_bitmap, sizeof(s.bitmap), &s.bitmap);
      if (GetSystemMetrics(SM_CYHSCROLL) * 2 < s.bitmap.bmHeight)
      {
        s.scrollbar_height = s.bitmap.bmHeight;
      }
      else
      {
        s.scrollbar_height = GetSystemMetrics(SM_CYHSCROLL) * 2;
      }
    }
    s.scrollbar_y = s.client_rect.bottom - s.scrollbar_height;
    MoveWindow(GetDlgItem(hwnd, 0), 0, s.scrollbar_y, s.client_rect.right, s.scrollbar_height, TRUE);
    return 0;

  case WM_SYSCOMMAND:
    if ((wparam & 0xfff0) == SC_MOVE)
    {
      return 0;
    }
    return DefWindowProcA(hwnd, msg, wparam, lparam);

  case WM_NCRBUTTONDOWN:
  case WM_RBUTTONDOWN:
    s.popup_point.x = LOWORD(lparam);
    s.popup_point.y = HIWORD(lparam);
    if (msg == WM_RBUTTONDOWN)
    {
      ClientToScreen(hwnd, &s.popup_point);
    }
    SetRect(&s.popup_rect, s.popup_point.x, s.popup_point.y, s.popup_point.x + 1, s.popup_point.y + 1);
    TrackPopupMenu(g_spell_minimized_popup_menu, TPM_RIGHTBUTTON, s.popup_point.x, s.popup_point.y, 0, hwnd, &s.popup_rect);
    return 0;

  case WM_INITMENU:
    AppendMenuA(g_spell_minimized_popup_menu, 0, 0x65, g_spell_chain_menu_minimize_text);
    AppendMenuA(g_spell_minimized_popup_menu, 0, 100, g_spell_chain_menu_help_text);
    return 0;

  case WM_MENUSELECT:
    if (HIWORD(wparam) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_spell_minimized_popup_menu);
      while (s.menu_item_count != 0)
      {
        DeleteMenu(g_spell_minimized_popup_menu, 0, MF_BYPOSITION);
        --s.menu_item_count;
      }
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

// FUNCTION: MAGIC 0x0048894d
// FUNCTION: SHANDALAR 0x004cb522
void layout_phase_display_window(HWND hwnd, LPRECT rect)
{
  struct
  {
    spell_chain_window_entry_t *windows;
    int spell_gap_x;
    int scrollbar_height;
    int max_scroll_pos;
    int card_player_and_card[2];
    int top_spell_y;
    int target_x_step;
    int target_x;
    int bottom_spell_y;
    int target_gap_y;
    int window_height;
    int target_index;
    int window_width;
    int y;
    int entry_index;
    int x;
    int left_margin;
    int top_margin;
    LONG window_count;
    RECT content_rect;
    int min_scroll_pos;
    int target_y;
    int has_targets;
    RECT window_rect;
    RECT battlefield_rect;
    HWND scrollbar_hwnd;
    LRESULT scroll_pos;
  } s;

  s.windows = (spell_chain_window_entry_t *)GetWindowLongA(hwnd, g_spell_chain_windows_long_offset);
  s.window_count = GetWindowLongA(hwnd, g_spell_chain_count_long_offset);
  s.scrollbar_hwnd = GetDlgItem(hwnd, 0);
  s.scroll_pos = SendMessageA(s.scrollbar_hwnd, 0xe1, 0, 0);
  if (s.window_count == 0)
  {
    if (IsWindowVisible(hwnd) != 0 || IsWindowVisible((HWND)g_spell_minimized_hwnd) != 0)
    {
      ShowWindow(hwnd, SW_HIDE);
      ShowWindow((HWND)g_spell_minimized_hwnd, SW_HIDE);
      UpdateWindow(g_duel_help_owner_hwnd);
    }
    if (rect != (LPRECT)0)
    {
      SetRect(rect, 0, 0, 0, 0);
    }
  }
  else
  {
    GetWindowRect(s.scrollbar_hwnd, &s.window_rect);
    s.scrollbar_height = s.window_rect.bottom - s.window_rect.top;
    s.left_margin = 5;
    s.top_margin = 5;
    s.spell_gap_x = 5;
    s.target_gap_y = 5;
    s.target_x_step = g_showlist_smallcard_width / 4;
    s.has_targets = 0;
    for (s.entry_index = 0; s.entry_index < s.window_count; s.entry_index++)
    {
      if (s.windows[s.entry_index].target_count != 0)
      {
        s.has_targets = 1;
      }
    }
    SetRect(&s.content_rect, 500, 500, 0, 0);
    s.x = s.left_margin;
    s.top_spell_y = s.top_margin;
    s.bottom_spell_y = s.top_spell_y + g_showlist_smallcard_height / 2 + g_showlist_smallcard_height;
    for (s.entry_index = 0; s.entry_index < s.window_count; s.entry_index++)
    {
      SendMessageA(s.windows[s.entry_index].spell_window, 0x401, (WPARAM)s.card_player_and_card, 0);
      if (s.card_player_and_card[0] == 0)
      {
        s.y = s.bottom_spell_y;
        s.target_y = (s.y - g_showlist_smallcard_height) - s.target_gap_y;
      }
      else
      {
        s.y = s.top_spell_y;
        s.target_y = g_showlist_smallcard_height + s.y + s.target_gap_y;
      }
      MoveWindow(s.windows[s.entry_index].spell_window, s.x, s.y,
                 g_showlist_smallcard_width, g_showlist_smallcard_height, TRUE);
      if (s.content_rect.left > s.x)
      {
        s.content_rect.left = s.x;
      }
      if (s.content_rect.right < g_showlist_smallcard_width + s.x)
      {
        s.content_rect.right = g_showlist_smallcard_width + s.x;
      }
      if (s.content_rect.top > s.y)
      {
        s.content_rect.top = s.y;
      }
      if (s.content_rect.bottom < g_showlist_smallcard_height + s.y)
      {
        s.content_rect.bottom = g_showlist_smallcard_height + s.y;
      }
      s.target_x = s.x;
      for (s.target_index = 0; s.target_index < s.windows[s.entry_index].target_count; s.target_index++)
      {
        MoveWindow(s.windows[s.entry_index].target_windows[s.target_index], s.target_x, s.target_y,
                   g_showlist_smallcard_width, g_showlist_smallcard_height, TRUE);
        if (s.content_rect.left > s.target_x)
        {
          s.content_rect.left = s.target_x;
        }
        if (s.content_rect.right < s.target_x + g_showlist_smallcard_width)
        {
          s.content_rect.right = s.target_x + g_showlist_smallcard_width;
        }
        if (s.content_rect.top > s.target_y)
        {
          s.content_rect.top = s.target_y;
        }
        if (s.content_rect.bottom < s.target_y + g_showlist_smallcard_height)
        {
          s.content_rect.bottom = s.target_y + g_showlist_smallcard_height;
        }
        s.target_x = s.target_x + s.target_x_step;
      }
      s.x = s.x + g_showlist_smallcard_width + s.spell_gap_x;
      if (s.windows[s.entry_index].target_count != 0)
      {
        s.x = s.x + (s.windows[s.entry_index].target_count - 1) * s.target_x_step;
      }
    }
    s.x -= -(-s.spell_gap_x);
    s.x += s.left_margin;
    s.content_rect.right = s.content_rect.right + s.content_rect.left;
    s.content_rect.left = 0;
    s.content_rect.top = 0;
    s.content_rect.bottom = s.content_rect.bottom + s.top_margin;
    s.window_rect.left = 0;
    s.window_rect.right = s.x - s.spell_gap_x + s.left_margin;
    s.window_rect.top = 0;
    s.window_rect.bottom = s.top_margin + s.top_margin + g_showlist_smallcard_height + s.bottom_spell_y + s.scrollbar_height;
    AdjustWindowRect(&s.window_rect, GetWindowLongA(hwnd, GWL_STYLE), FALSE);
    GetWindowRect(g_duel_player_battlefield_window_hwnd, &s.battlefield_rect);
    s.x = s.battlefield_rect.left;
    s.y = max(0, s.battlefield_rect.top - (s.window_rect.bottom - s.window_rect.top));
    s.window_width = min(s.window_rect.right - s.window_rect.left, s.battlefield_rect.right - s.battlefield_rect.left);
    s.window_height = s.window_rect.bottom - s.window_rect.top;
    MoveWindow(hwnd, s.x, s.y, s.window_width, s.window_height, TRUE);
    s.scrollbar_hwnd = GetDlgItem(hwnd, 0);
    if (s.window_width < s.window_rect.right - s.window_rect.left)
    {
      s.min_scroll_pos = 0;
      s.max_scroll_pos = (s.window_rect.right - s.window_rect.left) - s.window_width;
      SendMessageA(s.scrollbar_hwnd, 0xe2, s.min_scroll_pos, s.max_scroll_pos);
      SendMessageA(s.scrollbar_hwnd, 0x468, 1, 0);
    }
    else
    {
      SendMessageA(s.scrollbar_hwnd, 0x468, 0, 0);
    }
    SendMessageA(s.scrollbar_hwnd, 0xe3, (WPARAM)&s.min_scroll_pos, (LPARAM)&s.max_scroll_pos);
    if (s.min_scroll_pos > s.scroll_pos)
    {
      s.scroll_pos = s.min_scroll_pos;
    }
    if (s.scroll_pos > s.max_scroll_pos)
    {
      s.scroll_pos = s.max_scroll_pos;
    }
    SendMessageA(s.scrollbar_hwnd, 0xe0, 0, 0);
    SendMessageA(s.scrollbar_hwnd, WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, s.scroll_pos), (LPARAM)s.scrollbar_hwnd);
    if (IsWindowVisible(hwnd) == 0 && IsWindowVisible((HWND)g_spell_minimized_hwnd) == 0)
    {
      ShowWindow(hwnd, SW_SHOW);
      restack_duel_child_windows();
    }
    UpdateWindow(hwnd);
    if (rect != (LPRECT)0)
    {
      CopyRect(rect, &s.content_rect);
    }
  }
}
