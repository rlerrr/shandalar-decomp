#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/shared_resources.h"
#include "drawcardlib/src/pic.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

BOOL DrawMaskedBitmapToRect(HDC dc, RECT *rect, HANDLE bitmap, int width, int height, int src_x, int src_y, int mask_x, int mask_y);
extern int g_duel_modal_action_active;
void draw_duel_face_window(HDC dc, RECT *rect, int player);
void set_player_directive_value(int player, int value);
void post_face_directive_action(int player);
int is_face_directive_action_enabled(int player);
int draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HANDLE bitmap);
LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
int load_text_with_tab_escapes(char *filename, char *section_name);

// GLOBAL: MAGIC 0x0055e02c
int g_face_picture_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e030
int g_face_picture_ownership_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e034
int g_magicgame_face_window_extra_bytes = 8;

// GLOBAL: MAGIC 0x0056faa0
char s_MENU_FACE_0056faa0[0xc] = "MENU_FACE";

// GLOBAL: MAGIC 0x0056faac
char s__FACE_MULTI_pic_0056faac[0x10] = "\\FACE_MULTI.pic";

// GLOBAL: MAGIC 0x0056fabc
char s__FACE_BLACK_pic_0056fabc[0x10] = "\\FACE_BLACK.pic";

// GLOBAL: MAGIC 0x0056facc
char s__FACE_BLUE_pic_0056facc[0x10] = "\\FACE_BLUE.pic";

// GLOBAL: MAGIC 0x0056fadc
char s__FACE_GREEN_pic_0056fadc[0x10] = "\\FACE_GREEN.pic";

// GLOBAL: MAGIC 0x0056faec
char s__FACE_RED_pic_0056faec[0x10] = "\\FACE_RED.pic";

// GLOBAL: MAGIC 0x0056fafc
char s__FACE_WHITE_pic_0056fafc[0x10] = "\\FACE_WHITE.pic";

// GLOBAL: MAGIC 0x0056fb0c
char s_Face_0056fb0c[8] = "Face";

// GLOBAL: MAGIC 0x0056fb14
char s__duel_hlp_0056fb14[0xa] = "\\duel.hlp";

// GLOBAL: MAGIC 0x00637b48
COLORREF g_face_name_text_color;

// GLOBAL: MAGIC 0x00637b4c
int DAT_00637b4c;

// GLOBAL: MAGIC 0x00637b50
char g_face_menu_end_directive_text[0x34];

// GLOBAL: MAGIC 0x00637b84
COLORREF g_face_name_shadow_color;

// GLOBAL: MAGIC 0x00637b88
int g_face_directive_packet[3];

// GLOBAL: MAGIC 0x00637b94
int DAT_00637b94;

// GLOBAL: MAGIC 0x00637b98
char g_face_menu_help_text[0x1c];

// GLOBAL: MAGIC 0x00637bb4
HMENU g_face_popup_menu;

// GLOBAL: MAGIC 0x00637bb8
char g_face_menu_directive_format[100];

// GLOBAL: MAGIC 0x00637c1c
HFONT g_face_name_font;

// GLOBAL: MAGIC 0x00637c20
char g_face_menu_directive_self_text[0x38];

// GLOBAL: MAGIC 0x00637c58
HBITMAP g_face_background_pics[6];

// GLOBAL: MAGIC 0x00925bb0
int g_opponent_face_directive_action_enabled;

// GLOBAL: MAGIC 0x00925bb4
int g_player_face_directive_action_enabled;

// FUNCTION: MAGIC 0x004637d0
// FUNCTION: SHANDALAR 0x00425f10
int register_MAGICGAME_FaceClass(LPCSTR class_name)
{
  struct
  {
    char path[264];
    int result;
    WNDCLASSA wndclass;
  } s;

  s.result = 1;
  s.wndclass.style = 0xb;
  s.wndclass.lpfnWndProc = wndproc_MAGICGAME_FaceClass;
  s.wndclass.cbClsExtra = 0;
  s.wndclass.cbWndExtra = g_magicgame_face_window_extra_bytes;
  s.wndclass.hInstance = g_app_instance;
  s.wndclass.hIcon = (HICON)0;
  s.wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  s.wndclass.hbrBackground = GetStockObject(4);
  s.wndclass.lpszMenuName = (LPCSTR)0;
  s.wndclass.lpszClassName = class_name;
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.result = 0;
  }

  g_face_popup_menu = CreatePopupMenu();
  load_text_with_tab_escapes(global_ui_strings_filename, s_MENU_FACE_0056faa0);
  strcpy(g_face_menu_directive_format, text_lines[0]);
  strcpy(g_face_menu_directive_self_text, text_lines[1]);
  strcpy(g_face_menu_end_directive_text, text_lines[2]);
  strcpy(g_face_menu_help_text, text_lines[3]);

  strcpy(s.path, global_duelart_path);
  strcat(s.path, s__FACE_MULTI_pic_0056faac);
  g_face_background_pics[0] = load_pic(s.path);

  strcpy(s.path, global_duelart_path);
  strcat(s.path, s__FACE_BLACK_pic_0056fabc);
  g_face_background_pics[1] = load_pic(s.path);

  strcpy(s.path, global_duelart_path);
  strcat(s.path, s__FACE_BLUE_pic_0056facc);
  g_face_background_pics[2] = load_pic(s.path);

  strcpy(s.path, global_duelart_path);
  strcat(s.path, s__FACE_GREEN_pic_0056fadc);
  g_face_background_pics[3] = load_pic(s.path);

  strcpy(s.path, global_duelart_path);
  strcat(s.path, s__FACE_RED_pic_0056faec);
  g_face_background_pics[4] = load_pic(s.path);

  strcpy(s.path, global_duelart_path);
  strcat(s.path, s__FACE_WHITE_pic_0056fafc);
  g_face_background_pics[5] = load_pic(s.path);

  g_face_name_font = CreateFontIndirectA(LoadFontFromIni(s_Face_0056fb0c, 0));
  g_face_name_text_color = 0x2f6f7f7;
  g_face_name_shadow_color = 0x2565656;
  return s.result;
}

// FUNCTION: MAGIC 0x00463a6e
// FUNCTION: SHANDALAR 0x004261ae
void destroy_MAGICGAME_FaceClass(LPCSTR class_name)
{
  int i;

  if (g_face_popup_menu != (HMENU)0)
  {
    DestroyMenu(g_face_popup_menu);
  }
  g_face_popup_menu = (HMENU)0;

  for (i = 0; i < 6; i++)
  {
    if (g_face_background_pics[i] != (HBITMAP)0)
    {
      delete_and_close_object(g_face_background_pics[i]);
      g_face_background_pics[i] = (HBITMAP)0;
    }
  }

  if (g_face_name_font != (HFONT)0)
  {
    DeleteObject(g_face_name_font);
  }
  g_face_name_font = (HFONT)0;
}

// FUNCTION: MAGIC 0x00463b12
// FUNCTION: SHANDALAR 0x00426252
LRESULT CALLBACK wndproc_MAGICGAME_FaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int menu_item_count;
    char directive_text[100];
    int menu_player;
    char opponent_name[100];
    POINT menu_point;
    RECT popup_rect;
    HDC paint_dc;
    PAINTSTRUCT paint;
    RECT client_rect;
    MSG peek_msg;
    int click_player;
    char help_path[264];
    DWORD help_context;
    int command_player;
    HBITMAP face_bitmap;
    int owns_face_bitmap;
  } s;

  switch (msg)
  {

  case 0x439:
    s.face_bitmap = (HBITMAP)GetWindowLongA(hwnd, g_face_picture_window_long_offset);
    s.owns_face_bitmap = GetWindowLongA(hwnd, g_face_picture_ownership_window_long_offset);
    if (s.face_bitmap != (HBITMAP)0 && s.owns_face_bitmap == 0)
    {
      delete_and_close_object(s.face_bitmap);
    }

    s.face_bitmap = (HBITMAP)wparam;
    s.owns_face_bitmap = lparam;
    SetWindowLongA(hwnd, g_face_picture_window_long_offset, s.face_bitmap);
    SetWindowLongA(hwnd, g_face_picture_ownership_window_long_offset, s.owns_face_bitmap);
    InvalidateRect(hwnd, (RECT *)0, 0);
    return 0;

  case 0x438:
    s.face_bitmap = (HBITMAP)GetWindowLongA(hwnd, g_face_picture_window_long_offset);
    return (LRESULT)s.face_bitmap;

  case WM_COMMAND:
    switch ((UINT)wparam & 0xffff)
    {
    case 102:
      s.command_player = g_duel_player_face_window_hwnd == hwnd ? 0 : 1;
      unk_00715fb0 = 0;
      post_face_directive_action(s.command_player);
      break;
    case 100:
      set_player_directive_value(g_duel_player_face_window_hwnd != hwnd, 0);
      break;
    case 101:
      s.help_context = 0xbdf;
      strcpy(s.help_path, global_base_directory);
      strcat(s.help_path, s__duel_hlp_0056fb14);
      WinHelpA(g_duel_window_hwnd, s.help_path, HELP_CONTEXT, s.help_context);
      break;
    default:
      break;
    }
    return 0;

  case WM_CREATE:
    s.face_bitmap = (HBITMAP)0;
    SetWindowLongA(hwnd, g_face_picture_window_long_offset, s.face_bitmap);
    s.owns_face_bitmap = 0;
    SetWindowLongA(hwnd, g_face_picture_ownership_window_long_offset, s.owns_face_bitmap);
    return 0;

  case WM_DESTROY:
    s.face_bitmap = (HBITMAP)GetWindowLongA(hwnd, g_face_picture_window_long_offset);
    s.owns_face_bitmap = GetWindowLongA(hwnd, g_face_picture_ownership_window_long_offset);
    if (s.face_bitmap != (HBITMAP)0 && s.owns_face_bitmap == 0)
    {
      delete_and_close_object(s.face_bitmap);
    }
    return 0;

  case WM_LBUTTONDOWN:
    s.click_player = g_duel_player_face_window_hwnd == hwnd ? 0 : 1;
    if (g_duel_modal_action_active != 0)
    {
      Sleep(GetDoubleClickTime());
      unk_00715fb0 = PeekMessageA(&s.peek_msg, hwnd, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, 0);
      post_face_directive_action(s.click_player);
    }
    return 0;

  case WM_PAINT:
    s.face_bitmap = (HBITMAP)GetWindowLongA(hwnd, g_face_picture_window_long_offset);
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetClientRect(hwnd, &s.client_rect);
      if (s.face_bitmap != (HBITMAP)0)
      {
        draw_duel_face_window(s.paint_dc, &s.client_rect, g_duel_player_face_window_hwnd != hwnd);
      }
      else
      {
        FillRect(s.paint_dc, &s.client_rect, GetStockObject(4));
      }
      EndPaint(hwnd, &s.paint);
    }
    return 0;

  case WM_RBUTTONDOWN:
    s.menu_point.x = lparam & 0xffff;
    s.menu_point.y = HIWORD(lparam);
    ClientToScreen(hwnd, &s.menu_point);
    SetRect(&s.popup_rect, s.menu_point.x, s.menu_point.y, s.menu_point.x + 1, s.menu_point.y + 1);
    TrackPopupMenu(g_face_popup_menu, TPM_RIGHTBUTTON, s.menu_point.x, s.menu_point.y, 0, hwnd, &s.popup_rect);
    return 0;

  case WM_INITMENU:
    s.menu_player = g_duel_life_status_window_1_hwnd == hwnd ? 0 : 1;
    if (g_duel_modal_action_active != 0 && is_face_directive_action_enabled(s.menu_player) != 0)
    {
      if (s.menu_player == 1)
      {
        copy_opponent_name_prefix(s.opponent_name);
        sprintf(s.directive_text, g_face_menu_directive_format, s.opponent_name);
      }
      else
      {
        strcpy(s.directive_text, g_face_menu_directive_self_text);
      }
      AppendMenuA(g_face_popup_menu, MF_ENABLED, 0x66, s.directive_text);
    }

    if (0 < GetMenuItemCount(g_face_popup_menu))
    {
      AppendMenuA(g_face_popup_menu, MF_SEPARATOR, 0, (LPCSTR)0);
    }
    AppendMenuA(g_face_popup_menu, MF_ENABLED, 100, g_face_menu_end_directive_text);
    AppendMenuA(g_face_popup_menu, MF_ENABLED, 0x65, g_face_menu_help_text);
    return 0;

  case WM_MENUSELECT:
    if (HIWORD(wparam) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_face_popup_menu);
      while (s.menu_item_count-- != 0)
      {
        DeleteMenu(g_face_popup_menu, 0, MF_BYPOSITION);
      }
    }
    return 0;

  case WM_SETCURSOR:
    return handle_duel_inactive_cursor(hwnd, msg, wparam, lparam);

  case 0x30f:
  case 0x310:
  case 0x311:
    return FUN_10025b5e((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00464209
void draw_duel_face_window(HDC dc, RECT *rect, int player)
{
  struct
  {
    BITMAP bitmap;
    int saved_dc;
    RECT bitmap_rect;
    HBITMAP background_bitmap;
    POINT text_origin;
    int text_saved_dc;
    HBITMAP face_bitmap;
    char player_name[52];
    HWND face_window;
  } s;

  if (player == 0)
  {
    s.face_window = g_duel_player_face_window_hwnd;
  }
  else
  {
    s.face_window = g_duel_opponent_face_window_hwnd;
  }

  s.face_bitmap = (HBITMAP)GetWindowLongA(s.face_window, g_face_picture_window_long_offset);
  if (player == 0)
  {
    s.background_bitmap = g_face_background_pics[g_last_duel_player_primary_color];
  }
  else
  {
    s.background_bitmap = g_face_background_pics[g_last_duel_enemy_primary_color];
  }

  if (s.background_bitmap != (HBITMAP)0)
  {
    DrawBitmapToRect(dc, rect, s.background_bitmap);
  }
  else
  {
    FillRect(dc, rect, GetStockObject(4));
  }

  if (s.face_bitmap != (HBITMAP)0)
  {
    GetObjectA(s.face_bitmap, 0x18, &s.bitmap);
    s.bitmap.bmWidth = s.bitmap.bmWidth / 2;
    s.saved_dc = SaveDC(dc);
    SetMapMode(dc, MM_ISOTROPIC);
    SetWindowOrgEx(dc, s.bitmap.bmWidth / 2, s.bitmap.bmHeight / 2, (LPPOINT)0);
    SetViewportOrgEx(dc, rect->left + (rect->right - rect->left) / 2, rect->top + (rect->bottom - rect->top) / 2, (LPPOINT)0);
    SetWindowExtEx(dc, s.bitmap.bmWidth, s.bitmap.bmHeight, (LPSIZE)0);
    SetViewportExtEx(dc, rect->right - rect->left, rect->bottom - rect->top, (LPSIZE)0);
    SetRect(&s.bitmap_rect, 0, 0, s.bitmap.bmWidth, s.bitmap.bmHeight);
    draw_masked_bitmap_left_half_to_rect(dc, &s.bitmap_rect, s.face_bitmap);
    RestoreDC(dc, s.saved_dc);
  }

  if (player == 1)
  {
    copy_opponent_name_prefix(s.player_name);
  }
  else
  {
    strcpy(s.player_name, g_player_name);
  }

  if (strlen(s.player_name) != 0)
  {
    s.text_saved_dc = SaveDC(dc);
    SetMapMode(dc, MM_ANISOTROPIC);
    SetWindowExtEx(dc, rect->right - rect->left, 100, (LPSIZE)0);
    SetViewportExtEx(dc, rect->right - rect->left, rect->bottom - rect->top, (LPSIZE)0);
    SelectObject(dc, g_face_name_font);
    SetBkMode(dc, TRANSPARENT);
    SetTextAlign(dc, 0xe);
    s.text_origin.x = rect->left + (rect->right - rect->left) / 2;
    s.text_origin.y = rect->bottom;
    DPtoLP(dc, &s.text_origin, 1);
    SetTextColor(dc, g_face_name_shadow_color);
    TextOutA(dc, s.text_origin.x + 1, s.text_origin.y + 1, s.player_name, strlen(s.player_name));
    SetTextColor(dc, g_face_name_text_color);
    TextOutA(dc, s.text_origin.x, s.text_origin.y, s.player_name, strlen(s.player_name));
    RestoreDC(dc, s.text_saved_dc);
  }
}

// FUNCTION: MAGIC 0x0046452a
void set_player_directive_value(int player, int value)
{
  struct
  {
    HWND life_window;
    HWND face_window;
    HWND graveyard_window;
    HWND mana_summary_window;
    HWND library_window;
  } s;

  if (player == 0)
  {
    s.face_window = g_duel_player_face_window_hwnd;
  }
  else
  {
    s.face_window = g_duel_opponent_face_window_hwnd;
  }

  if (player == 0)
  {
    s.life_window = g_duel_life_status_window_1_hwnd;
  }
  else
  {
    s.life_window = g_duel_life_status_window_2_hwnd;
  }

  if (player == 0)
  {
    s.library_window = DAT_0091ce30;
  }
  else
  {
    s.library_window = DAT_0092680c;
  }

  if (player == 0)
  {
    s.graveyard_window = g_duel_player_graveyard_window_hwnd;
  }
  else
  {
    s.graveyard_window = g_phasebar_your_untap_window_hwnd;
  }

  if (player == 0)
  {
    s.mana_summary_window = unk_008ce534;
  }
  else
  {
    s.mana_summary_window = unk_00939344;
  }

  if (value != 0)
  {
    ShowWindow(s.face_window, SW_SHOW);
    BringWindowToTop(s.face_window);
    ShowWindow(s.life_window, SW_HIDE);
    if (g_duel_interface_options.layout != 2)
    {
      ShowWindow(s.library_window, SW_HIDE);
      ShowWindow(s.graveyard_window, SW_HIDE);
      ShowWindow(s.mana_summary_window, SW_HIDE);
    }
  }
  else
  {
    ShowWindow(s.life_window, SW_SHOW);
    ShowWindow(s.library_window, SW_SHOW);
    ShowWindow(s.graveyard_window, SW_SHOW);
    ShowWindow(s.mana_summary_window, SW_SHOW);
    ShowWindow(s.face_window, SW_HIDE);
  }
}

// FUNCTION: MAGIC 0x00464671
void post_face_directive_action(int player)
{
  g_face_directive_packet[0] = 0;
  g_face_directive_packet[1] = player;
  g_face_directive_packet[2] = -1;
  PostMessageA(g_duel_window_hwnd, 0x464, 0, (LPARAM)g_face_directive_packet);
}

// FUNCTION: MAGIC 0x004646b0
int is_face_directive_action_enabled(int player)
{
  if ((player == 1 && g_opponent_face_directive_action_enabled != 0) ||
      (player == 0 && g_player_face_directive_action_enabled != 0))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: MAGIC 0x004941ee
int draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HANDLE bitmap)
{
  struct
  {
    int result;
    BITMAP bitmap_info;
    int height;
    int half_width;
    int unused_14;
    int unused_10;
    int unused_c;
    int width;
  } s;

  GetObjectA(bitmap, 0x18, &s.bitmap_info);
  s.half_width = s.bitmap_info.bmWidth / 2;
  s.height = s.bitmap_info.bmHeight;
  s.unused_10 = 0;
  s.unused_c = 0;
  s.width = s.half_width;
  s.unused_14 = 0;
  s.result = DrawMaskedBitmapToRect(dc, rect, bitmap, s.half_width, s.height, s.unused_10, s.unused_c, s.width, s.unused_14);
  return s.result;
}

// FUNCTION: MAGIC 0x0049545f
LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg)
  {
  case WM_SETCURSOR:
    if (g_duel_modal_action_active == 0)
    {
      SetCursor(LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f8a));
      return 0;
    }
    else
    {
      return DefWindowProcA(hwnd, msg, wparam, lparam);
    }

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: MAGIC 0x004ecbd4
int load_text_with_tab_escapes(char *filename, char *section_name)
{
  int loaded_count;
  int line_index;
  int line_count;
  size_t line_length;
  int out_index;
  int char_index;

  if (g_duel_ai_mode_state == 1)
  {
    loaded_count = 0;
  }
  else
  {
    loaded_count = load_text(filename, section_name);
    for (line_index = 0; line_count = abs(loaded_count), line_index < line_count; line_index++)
    {
      line_length = strlen(text_lines[line_index]);
      out_index = 0;
      for (char_index = 0; char_index < (int)line_length; char_index++)
      {
        if (text_lines[line_index][char_index] == '\\' && text_lines[line_index][char_index + 1] == 't')
        {
          text_lines[line_index][out_index] = '\t';
          char_index++;
        }
        else
        {
          text_lines[line_index][out_index] = text_lines[line_index][char_index];
        }
        out_index++;
      }
      text_lines[line_index][out_index] = '\0';
    }
  }
  return loaded_count;
}
