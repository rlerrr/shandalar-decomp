#include <windows.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "magic/src/duel_engine.h"
#include "magic/src/global_other.h"
#include "magic/src/global_strings.h"

BOOL DrawMaskedBitmapToRect(HDC dc, RECT *rect, HANDLE bitmap, int width, int height, int src_x, int src_y, int mask_x, int mask_y);
int draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HANDLE bitmap);
int handle_button_palette_message(int hwnd, int msg, void *wparam, int lparam);
void checked_DeleteDC_DeleteObject(HDC dc, HGDIOBJ obj);

#define SCROLLBAR_OFFSCREEN_DC_PTR (&g_scrollbar_offscreen_dc)
#define SCROLLBAR_OFFSCREEN_BITMAP_PTR (&g_scrollbar_offscreen_bitmap)
#define SCROLLBAR_OFFSCREEN_DC g_scrollbar_offscreen_dc
#define SCROLLBAR_LAST_MOUSE_X g_scrollbar_last_mouse_x
#define SCROLLBAR_LAST_MOUSE_Y g_scrollbar_last_mouse_y
#define SCROLLBAR_OFFSCREEN_BITMAP g_scrollbar_offscreen_bitmap

// GLOBAL: MAGIC 0x00638628
// GLOBAL: SHANDALAR 0x00653188
HDC g_scrollbar_offscreen_dc;

// GLOBAL: MAGIC 0x00638630
// GLOBAL: SHANDALAR 0x00653190
int g_scrollbar_last_mouse_x;

// GLOBAL: MAGIC 0x00638634
// GLOBAL: SHANDALAR 0x00653194
int g_scrollbar_last_mouse_y;

// GLOBAL: MAGIC 0x00638638
// GLOBAL: SHANDALAR 0x00653198
HBITMAP g_scrollbar_offscreen_bitmap;

// GLOBAL: MAGIC 0x0055e048
// GLOBAL: SHANDALAR 0x0057f140
int g_scrollbar_position_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e04c
// GLOBAL: SHANDALAR 0x0057f144
int g_scrollbar_min_position_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e050
// GLOBAL: SHANDALAR 0x0057f148
int g_scrollbar_max_position_window_long_offset = 8;

// GLOBAL: MAGIC 0x0055e054
// GLOBAL: SHANDALAR 0x0057f14c
int g_scrollbar_direction_window_long_offset = 0xc;

// GLOBAL: MAGIC 0x0055e058
// GLOBAL: SHANDALAR 0x0057f150
int g_scrollbar_track_bitmap_window_long_offset = 0x10;

// GLOBAL: MAGIC 0x0055e05c
// GLOBAL: SHANDALAR 0x0057f154
int g_scrollbar_track_bitmap_layout_window_long_offset = 0x14;

// GLOBAL: MAGIC 0x0055e060
// GLOBAL: SHANDALAR 0x0057f158
int g_scrollbar_thumb_bitmap_window_long_offset = 0x18;

// GLOBAL: MAGIC 0x0055e064
// GLOBAL: SHANDALAR 0x0057f15c
int g_scrollbar_thumb_bitmap_frame_count_window_long_offset = 0x1c;

// GLOBAL: MAGIC 0x0055e068
// GLOBAL: SHANDALAR 0x0057f160
int g_scrollbar_thumb_position_window_long_offset = 0x20;

// GLOBAL: MAGIC 0x0055e06c
// GLOBAL: SHANDALAR 0x0057f164
int g_scrollbar_thumb_visible_window_long_offset = 0x24;

// GLOBAL: MAGIC 0x0055e070
// GLOBAL: SHANDALAR 0x0057f168
int g_scrollbar_window_extra_bytes = 0x28;

// FUNCTION: MAGIC 0x00490900
// FUNCTION: SHANDALAR 0x005562d0
int register_MAGICGAME_ScrollbarClass(LPCSTR class_name)
{
  struct
  {
    int registered;
    WNDCLASSA wndclass;
  } s;

  s.registered = 1;
  SET_DUEL_WNDCLASS(s.wndclass, 1, wndproc_MAGICGAME_ScrollbarClass,
                    g_scrollbar_window_extra_bytes,
                    (HICON)0, (HBRUSH)0x6, class_name);
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.registered = 0;
  }

  s.registered &= CreateOffscreen32bppDibSection(1000,
                                                 GetSystemMetrics(SM_CYHSCROLL) * 5,
                                                 SCROLLBAR_OFFSCREEN_DC_PTR,
                                                 (BITMAPINFO *)0,
                                                 SCROLLBAR_OFFSCREEN_BITMAP_PTR,
                                                 (HGDIOBJ *)0,
                                                 (void **)0);
  return s.registered;
}

// FUNCTION: MAGIC 0x004909b5
// FUNCTION: SHANDALAR 0x00556385
void destroy_MAGICGAME_ScrollbarClass(LPCSTR class_name)
{
  checked_DeleteDC_DeleteObject(SCROLLBAR_OFFSCREEN_DC, SCROLLBAR_OFFSCREEN_BITMAP);
  SCROLLBAR_OFFSCREEN_DC = (HDC)0;
  SCROLLBAR_OFFSCREEN_BITMAP = (HBITMAP)0;
}

// FUNCTION: MAGIC 0x00491be4
// FUNCTION: SHANDALAR 0x005575b3
int scrollbar_client_x_to_position(HWND hwnd, int client_x)
{
  struct
  {
    int result;
    LONG min_position;
    RECT client_rect;
    int max_position;
  } s;

  if (hwnd == (HWND)0)
  {
    return 0;
  }

  s.min_position = GetWindowLongA(hwnd, g_scrollbar_min_position_window_long_offset);
  s.max_position = GetWindowLongA(hwnd, g_scrollbar_max_position_window_long_offset);
  GetClientRect(hwnd, &s.client_rect);
  if (client_x < s.client_rect.left)
  {
    client_x = s.client_rect.left;
  }
  if (s.client_rect.right < client_x)
  {
    client_x = s.client_rect.right;
  }
  s.result = s.min_position + (((s.max_position - s.min_position) + 1) * client_x) / s.client_rect.right;
  return s.result;
}

// FUNCTION: MAGIC 0x00491c7a
// FUNCTION: SHANDALAR 0x00557649
int scrollbar_position_to_client_x(HWND hwnd, int position)
{
  struct
  {
    LONG min_position;
    int result;
    RECT client_rect;
    int max_position;
  } s;

  if (hwnd == (HWND)0)
  {
    return 0;
  }

  s.min_position = GetWindowLongA(hwnd, g_scrollbar_min_position_window_long_offset);
  s.max_position = GetWindowLongA(hwnd, g_scrollbar_max_position_window_long_offset);
  GetClientRect(hwnd, &s.client_rect);
  if (position < s.min_position)
  {
    position = s.min_position;
  }
  if (s.max_position < position)
  {
    position = s.max_position;
  }
  s.result = (s.client_rect.right * position) / ((s.max_position - s.min_position) + 1);
  return s.result;
}

// FUNCTION: MAGIC 0x00491d0d
// FUNCTION: SHANDALAR 0x005576dc
void get_scrollbar_thumb_rect(HWND hwnd, LPRECT rect)
{
  struct
  {
    LONG frame_count;
    BITMAP bitmap;
    int height;
    int width;
    LONG position;
    HANDLE thumb_bitmap;
    RECT client_rect;
  } s;

  if (hwnd != (HWND)0 && rect != (LPRECT)0)
  {
    s.position = GetWindowLongA(hwnd, g_scrollbar_thumb_position_window_long_offset);
    s.thumb_bitmap = (HANDLE)GetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_window_long_offset);
    s.frame_count = GetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_frame_count_window_long_offset);
    GetClientRect(hwnd, &s.client_rect);
    if (s.thumb_bitmap != (HANDLE)0 && s.frame_count != 0)
    {
      GetObjectA(s.thumb_bitmap, 0x18, &s.bitmap);
      s.height = s.client_rect.bottom;
      s.width = ((s.bitmap.bmWidth / 2) * s.client_rect.bottom) / (s.bitmap.bmHeight / s.frame_count);
      SetRect(rect, s.position - s.width / 2, 0,
              (s.position - s.width / 2) + s.width, s.client_rect.bottom);
    }
    else
    {
      SetRect(rect, s.position - s.client_rect.right / 20, 0,
              s.position + s.client_rect.right / 20, s.client_rect.bottom);
    }
    if (rect->left < s.client_rect.left)
    {
      OffsetRect(rect, s.client_rect.left - rect->left, 0);
    }
    else if (s.client_rect.right < rect->right)
    {
      OffsetRect(rect, s.client_rect.right - rect->right, 0);
    }
  }
}

// FUNCTION: MAGIC 0x004909e8
// FUNCTION: SHANDALAR 0x005563b8
LRESULT CALLBACK wndproc_MAGICGAME_ScrollbarClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int flipped_rect_edge;
    BITMAP thumb_bitmap_info;
    int thumb_source_height;
    int thumb_source_width;
    int thumb_mask_y;
    int thumb_frame;
    int thumb_source_x;
    int thumb_source_y;
    int thumb_mask_x;
    int track_source_height;
    int track_source_width;
    RECT bitmap_rect;
    HDC paint_dc;
    BITMAP bitmap;
    int client_height;
    PAINTSTRUCT paint;
    int scaled_width;
    RECT tile_rect;
    int tile_y;
    int saved_dc;
    int tile_x;
    RECT thumb_rect;
    RECT window_rect;
    RECT client_rect;
    int button_up_mouse_x;
    int button_up_mouse_y;
    RECT button_up_client_rect;
    int button_down_mouse_x;
    int button_down_mouse_y;
    RECT button_down_hit_rect;
    int clamped_x;
    int move_mouse_x;
    int move_mouse_y;
    RECT move_client_rect;
    int cue_mouse_x;
    int cue_mouse_y;
    RECT cue_hit_rect;
    int scroll_position;
    LONG thumb_frame_count;
    unsigned int min_position;
    unsigned int thumb_visible;
    HANDLE track_bitmap;
    int direction;
    LONG thumb_position;
    HANDLE thumb_bitmap;
    LONG max_position;
    LONG track_bitmap_layout;
  } s;

  switch (msg)
  {
  case 0x437:
    s.thumb_position = GetWindowLongA(hwnd, g_scrollbar_thumb_position_window_long_offset);
    s.thumb_visible = GetWindowLongA(hwnd, g_scrollbar_thumb_visible_window_long_offset);
    s.cue_mouse_x = (int)(short)lparam;
    s.cue_mouse_y = (int)(short)((unsigned int)lparam >> 16);
    get_scrollbar_thumb_rect(hwnd, &s.cue_hit_rect);
    if (s.thumb_visible != 0 && s.cue_hit_rect.left <= s.cue_mouse_x && s.cue_mouse_x <= s.cue_hit_rect.right)
    {
      strcpy((char *)wparam, gs_cuecard_scroll_thumb_0093a810);
    }
    else
    {
      strcpy((char *)wparam, gs_cuecard_scrollbar_008b2900);
    }
    return 1;

  case 0x432:
    InvalidateRect(hwnd, (RECT *)0, 1);
    return 0;

  case SBM_SETRANGE:
    s.min_position = GetWindowLongA(hwnd, g_scrollbar_min_position_window_long_offset);
    s.max_position = GetWindowLongA(hwnd, g_scrollbar_max_position_window_long_offset);
    if (s.min_position != wparam || s.max_position != lparam)
    {
      s.min_position = wparam;
      s.max_position = lparam;
      SetWindowLongA(hwnd, g_scrollbar_min_position_window_long_offset, s.min_position);
      SetWindowLongA(hwnd, g_scrollbar_max_position_window_long_offset, s.max_position);
      InvalidateRect(hwnd, (RECT *)0, 1);
    }
    return 0;

  case SBM_GETRANGE:
    s.min_position = GetWindowLongA(hwnd, g_scrollbar_min_position_window_long_offset);
    s.max_position = GetWindowLongA(hwnd, g_scrollbar_max_position_window_long_offset);
    if (wparam != 0)
    {
      *(unsigned int *)wparam = s.min_position;
    }
    if (lparam != 0)
    {
      *(LONG *)lparam = s.max_position;
    }
    return (s.max_position << 16) | (s.min_position & 0xffff);

  case SBM_SETPOS:
    s.scroll_position = GetWindowLongA(hwnd, g_scrollbar_position_window_long_offset);
    if (s.scroll_position != wparam)
    {
      s.scroll_position = wparam;
      SetWindowLongA(hwnd, g_scrollbar_position_window_long_offset, s.scroll_position);
      InvalidateRect(hwnd, (RECT *)0, 1);
    }
    return 0;

  case SBM_GETPOS:
    s.scroll_position = GetWindowLongA(hwnd, g_scrollbar_position_window_long_offset);
    return s.scroll_position;

  case 0x464:
    s.track_bitmap = (HANDLE)GetWindowLongA(hwnd, g_scrollbar_track_bitmap_window_long_offset);
    if (s.track_bitmap != (HANDLE)wparam)
    {
      s.track_bitmap = (HANDLE)wparam;
      s.track_bitmap_layout = lparam;
      SetWindowLongA(hwnd, g_scrollbar_track_bitmap_window_long_offset, (LONG)s.track_bitmap);
      SetWindowLongA(hwnd, g_scrollbar_track_bitmap_layout_window_long_offset, s.track_bitmap_layout);
      InvalidateRect(hwnd, (RECT *)0, 1);
    }
    return 0;

  case 0x465:
    s.track_bitmap = (HANDLE)GetWindowLongA(hwnd, g_scrollbar_track_bitmap_window_long_offset);
    if (wparam != 0)
    {
      *(HANDLE *)wparam = s.track_bitmap;
    }
    return (LRESULT)s.track_bitmap;

  case 0x466:
    s.thumb_bitmap = (HANDLE)GetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_window_long_offset);
    s.thumb_frame_count = GetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_frame_count_window_long_offset);
    if ((HANDLE)wparam != s.thumb_bitmap)
    {
      s.thumb_bitmap = (HANDLE)wparam;
      s.thumb_frame_count = lparam;
      SetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_window_long_offset, (LONG)s.thumb_bitmap);
      SetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_frame_count_window_long_offset, s.thumb_frame_count);
      InvalidateRect(hwnd, (RECT *)0, 1);
    }
    return 0;

  case 0x467:
    s.thumb_bitmap = (HANDLE)GetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_window_long_offset);
    s.thumb_frame_count = GetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_frame_count_window_long_offset);
    if (wparam != 0)
    {
      *(HANDLE *)wparam = s.thumb_bitmap;
    }
    if (lparam != 0)
    {
      *(LONG *)lparam = s.thumb_frame_count;
    }
    return (LRESULT)s.thumb_bitmap;

  case 0x468:
    s.thumb_visible = GetWindowLongA(hwnd, g_scrollbar_thumb_visible_window_long_offset);
    if (wparam != s.thumb_visible)
    {
      s.thumb_visible = wparam;
      SetWindowLongA(hwnd, g_scrollbar_thumb_visible_window_long_offset, s.thumb_visible);
      InvalidateRect(hwnd, (RECT *)0, 1);
    }
    return 0;

  case WM_CREATE:
    s.scroll_position = 0;
    s.min_position = 0;
    s.max_position = 0;
    SetWindowLongA(hwnd, g_scrollbar_position_window_long_offset, s.scroll_position);
    SetWindowLongA(hwnd, g_scrollbar_min_position_window_long_offset, s.min_position);
    SetWindowLongA(hwnd, g_scrollbar_max_position_window_long_offset, s.max_position);
    s.direction = 1;
    SetWindowLongA(hwnd, g_scrollbar_direction_window_long_offset, s.direction);
    s.track_bitmap = (HANDLE)0;
    s.track_bitmap_layout = 0;
    SetWindowLongA(hwnd, g_scrollbar_track_bitmap_window_long_offset, (LONG)s.track_bitmap);
    SetWindowLongA(hwnd, g_scrollbar_track_bitmap_layout_window_long_offset, s.track_bitmap_layout);
    s.thumb_bitmap = (HANDLE)0;
    s.thumb_frame_count = 0;
    SetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_window_long_offset, (LONG)s.thumb_bitmap);
    SetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_frame_count_window_long_offset, s.thumb_frame_count);
    s.thumb_position = 0;
    SetWindowLongA(hwnd, g_scrollbar_thumb_position_window_long_offset, s.thumb_position);
    s.thumb_visible = 0;
    SetWindowLongA(hwnd, g_scrollbar_thumb_visible_window_long_offset, s.thumb_visible);
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_MOUSEMOVE:
    if (GetCapture() == hwnd)
    {
      s.direction = GetWindowLongA(hwnd, g_scrollbar_direction_window_long_offset);
      s.scroll_position = GetWindowLongA(hwnd, g_scrollbar_position_window_long_offset);
      s.min_position = GetWindowLongA(hwnd, g_scrollbar_min_position_window_long_offset);
      s.max_position = GetWindowLongA(hwnd, g_scrollbar_max_position_window_long_offset);
      s.thumb_position = GetWindowLongA(hwnd, g_scrollbar_position_window_long_offset);
      s.move_mouse_x = (int)(short)lparam;
      s.move_mouse_y = (int)(short)((unsigned int)lparam >> 16);
      GetClientRect(hwnd, &s.move_client_rect);
      if (s.move_mouse_x < 0)
      {
        s.move_mouse_x = 0;
      }
      if (s.move_client_rect.right < s.move_mouse_x)
      {
        s.move_mouse_x = s.move_client_rect.right;
      }
      if (s.move_mouse_y < 0)
      {
        s.move_mouse_y = 0;
      }
      if (s.move_client_rect.bottom < s.move_mouse_y)
      {
        s.move_mouse_y = s.move_client_rect.bottom;
      }
      if (SCROLLBAR_LAST_MOUSE_X < s.move_mouse_x)
      {
        if (s.direction == 0)
        {
          s.direction = 1;
          SetWindowLongA(hwnd, g_scrollbar_direction_window_long_offset, s.direction);
          InvalidateRect(hwnd, (RECT *)0, 1);
        }
      }
      else if (s.move_mouse_x < SCROLLBAR_LAST_MOUSE_X && s.direction != 0)
      {
        s.direction = 0;
        SetWindowLongA(hwnd, g_scrollbar_direction_window_long_offset, s.direction);
        InvalidateRect(hwnd, (RECT *)0, 1);
      }
      s.clamped_x = s.move_mouse_x;
      if (s.clamped_x < (int)s.min_position)
      {
        s.clamped_x = s.min_position;
      }
      else if (s.max_position < s.clamped_x)
      {
        s.clamped_x = s.max_position;
      }
      if (s.clamped_x != s.thumb_position)
      {
        s.thumb_position = s.clamped_x;
        SetWindowLongA(hwnd, g_scrollbar_thumb_position_window_long_offset, s.thumb_position);
        InvalidateRect(hwnd, (RECT *)0, 1);
      }
      if (scrollbar_client_x_to_position(hwnd, s.move_mouse_x) != s.scroll_position)
      {
        SendMessageA(GetParent(hwnd), WM_HSCROLL,
                     MAKELONG(5, scrollbar_client_x_to_position(hwnd, s.move_mouse_x)), (LPARAM)hwnd);
      }
      UpdateWindow(hwnd);
      SCROLLBAR_LAST_MOUSE_X = s.move_mouse_x;
      SCROLLBAR_LAST_MOUSE_Y = s.move_mouse_y;
    }
    return 0;

  case WM_LBUTTONDOWN:
    UpdateWindow(hwnd);
    s.thumb_position = GetWindowLongA(hwnd, g_scrollbar_thumb_position_window_long_offset);
    s.direction = GetWindowLongA(hwnd, g_scrollbar_direction_window_long_offset);
    s.thumb_visible = GetWindowLongA(hwnd, g_scrollbar_thumb_visible_window_long_offset);
    s.button_down_mouse_x = (int)(short)lparam;
    s.button_down_mouse_y = (int)(short)((unsigned int)lparam >> 16);
    if (s.thumb_visible == 0)
    {
      return 0;
    }
    get_scrollbar_thumb_rect(hwnd, &s.button_down_hit_rect);
    if (s.button_down_hit_rect.left <= s.button_down_mouse_x &&
        s.button_down_mouse_x <= s.button_down_hit_rect.right)
    {
      SetCapture(hwnd);
      SCROLLBAR_LAST_MOUSE_X = s.button_down_mouse_x;
      SCROLLBAR_LAST_MOUSE_Y = s.button_down_mouse_y;
    }
    else if (s.button_down_hit_rect.right < s.button_down_mouse_x)
    {
      if (s.direction == 0)
      {
        s.direction = 1;
        SetWindowLongA(hwnd, g_scrollbar_direction_window_long_offset, s.direction);
        InvalidateRect(hwnd, (RECT *)0, 1);
      }
      SendMessageA(GetParent(hwnd), WM_HSCROLL, 1, (LPARAM)hwnd);
    }
    else if (s.button_down_mouse_x < s.button_down_hit_rect.left)
    {
      if (s.direction != 0)
      {
        s.direction = 0;
        SetWindowLongA(hwnd, g_scrollbar_direction_window_long_offset, s.direction);
        InvalidateRect(hwnd, (RECT *)0, 1);
      }
      SendMessageA(GetParent(hwnd), WM_HSCROLL, 0, (LPARAM)hwnd);
    }
    SCROLLBAR_LAST_MOUSE_X = s.button_down_mouse_x;
    SCROLLBAR_LAST_MOUSE_Y = s.button_down_mouse_y;
    return 0;

  case WM_LBUTTONUP:
    if (GetCapture() == hwnd)
    {
      ReleaseCapture();
      s.thumb_position = GetWindowLongA(hwnd, g_scrollbar_thumb_position_window_long_offset);
      s.button_up_mouse_x = (int)(short)lparam;
      s.button_up_mouse_y = (int)(short)((unsigned int)lparam >> 16);
      GetClientRect(hwnd, &s.button_up_client_rect);
      if (s.button_up_mouse_x < 0)
      {
        s.button_up_mouse_x = 0;
      }
      if (s.button_up_client_rect.right < s.button_up_mouse_x)
      {
        s.button_up_mouse_x = s.button_up_client_rect.right;
      }
      if (s.button_up_mouse_y < 0)
      {
        s.button_up_mouse_y = 0;
      }
      if (s.button_up_client_rect.bottom < s.button_up_mouse_y)
      {
        s.button_up_mouse_y = s.button_up_client_rect.bottom;
      }
      s.thumb_position = s.button_up_mouse_x;
      SetWindowLongA(hwnd, g_scrollbar_thumb_position_window_long_offset, s.thumb_position);
      InvalidateRect(hwnd, (RECT *)0, 1);
      SendMessageA(GetParent(hwnd), WM_HSCROLL,
                   MAKELONG(4, scrollbar_client_x_to_position(hwnd, s.button_up_mouse_x)), (LPARAM)hwnd);
    }
    return 0;

  case WM_PAINT:
    s.scroll_position = GetWindowLongA(hwnd, g_scrollbar_position_window_long_offset);
    s.thumb_position = GetWindowLongA(hwnd, g_scrollbar_thumb_position_window_long_offset);
    s.direction = GetWindowLongA(hwnd, g_scrollbar_direction_window_long_offset);
    s.track_bitmap = (HANDLE)GetWindowLongA(hwnd, g_scrollbar_track_bitmap_window_long_offset);
    s.track_bitmap_layout = GetWindowLongA(hwnd, g_scrollbar_track_bitmap_layout_window_long_offset);
    s.thumb_bitmap = (HANDLE)GetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_window_long_offset);
    s.thumb_frame_count = GetWindowLongA(hwnd, g_scrollbar_thumb_bitmap_frame_count_window_long_offset);
    s.thumb_visible = GetWindowLongA(hwnd, g_scrollbar_thumb_visible_window_long_offset);
    s.min_position = GetWindowLongA(hwnd, g_scrollbar_min_position_window_long_offset);
    s.max_position = GetWindowLongA(hwnd, g_scrollbar_max_position_window_long_offset);
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetClientRect(hwnd, &s.client_rect);
      s.saved_dc = SaveDC(SCROLLBAR_OFFSCREEN_DC);
      IntersectClipRect(SCROLLBAR_OFFSCREEN_DC, 0, 0, s.client_rect.right, s.client_rect.bottom);
      GetWindowRect(hwnd, &s.window_rect);
      MapWindowPoints((HWND)0, GetParent(hwnd), (LPPOINT)&s.window_rect, 2);
      OffsetViewportOrgEx(SCROLLBAR_OFFSCREEN_DC, -s.window_rect.left, -s.window_rect.top, (LPPOINT)0);
      SendMessageA(GetParent(hwnd), WM_ERASEBKGND, (WPARAM)SCROLLBAR_OFFSCREEN_DC, 0);
      OffsetViewportOrgEx(SCROLLBAR_OFFSCREEN_DC, s.window_rect.left, s.window_rect.top, (LPPOINT)0);
      if (s.track_bitmap != (HANDLE)0)
      {
        CopyRect(&s.bitmap_rect, &s.client_rect);
        GetObjectA(s.track_bitmap, 0x18, &s.bitmap);
        s.bitmap_rect.left = 0 - (s.scroll_position % s.bitmap.bmWidth);
        s.client_height = s.client_rect.bottom;
        if (s.track_bitmap_layout != 0)
        {
          s.scaled_width = (s.bitmap.bmWidth * s.client_height) / (s.bitmap.bmHeight / 2);
          s.track_source_width = s.bitmap.bmWidth;
          s.track_source_height = s.bitmap.bmHeight / 2;
        }
        else
        {
          s.scaled_width = ((s.bitmap.bmWidth / 2) * s.client_height) / s.bitmap.bmHeight;
          s.track_source_width = s.bitmap.bmWidth / 2;
          s.track_source_height = s.bitmap.bmHeight;
        }
        for (s.tile_x = s.bitmap_rect.left;
             s.tile_x < s.bitmap_rect.right;
             s.tile_x += s.track_source_width)
        {
          for (s.tile_y = s.bitmap_rect.top;
               s.tile_y < s.bitmap_rect.bottom;
               s.tile_y += s.track_source_height)
          {
            SetRect(&s.tile_rect, s.tile_x, s.tile_y,
                    s.scaled_width + s.tile_x, s.tile_y + s.client_height);
            if (s.track_bitmap_layout != 0)
            {
              DrawMaskedBitmapToRect(SCROLLBAR_OFFSCREEN_DC, &s.tile_rect, s.track_bitmap,
                                     s.bitmap.bmWidth, s.bitmap.bmHeight / 2,
                                     0, 0, 0, s.bitmap.bmHeight / 2);
            }
            else
            {
              draw_masked_bitmap_left_half_to_rect(SCROLLBAR_OFFSCREEN_DC, &s.tile_rect, s.track_bitmap);
            }
          }
        }
      }
      else
      {
        FillRect(SCROLLBAR_OFFSCREEN_DC, &s.client_rect, GetStockObject(GRAY_BRUSH));
      }
      if (s.thumb_visible != 0)
      {
        s.thumb_frame = s.thumb_position % s.thumb_frame_count;
        if (scrollbar_client_x_to_position(hwnd, s.thumb_position) != s.scroll_position)
        {
          s.thumb_position = scrollbar_position_to_client_x(hwnd, s.scroll_position);
          SetWindowLongA(hwnd, g_scrollbar_thumb_position_window_long_offset, s.thumb_position);
        }
        if (s.thumb_bitmap != (HANDLE)0 && s.thumb_frame_count != 0)
        {
          GetObjectA(s.thumb_bitmap, 0x18, &s.thumb_bitmap_info);
          s.thumb_source_width = s.thumb_bitmap_info.bmWidth / 2;
          s.thumb_source_height = s.thumb_bitmap_info.bmHeight / s.thumb_frame_count;
          s.thumb_source_x = 0;
          s.thumb_source_y = s.thumb_source_height * s.thumb_frame;
          s.thumb_mask_x = s.thumb_source_width;
          s.thumb_mask_y = s.thumb_source_y;
          get_scrollbar_thumb_rect(hwnd, &s.thumb_rect);
          if (s.direction == 0)
          {
            SetMapMode(SCROLLBAR_OFFSCREEN_DC, MM_ANISOTROPIC);
            SetWindowExtEx(SCROLLBAR_OFFSCREEN_DC, 1, 1, (LPSIZE)0);
            SetViewportExtEx(SCROLLBAR_OFFSCREEN_DC, -1, 1, (LPSIZE)0);
            SetWindowOrgEx(SCROLLBAR_OFFSCREEN_DC, 0, 0, (LPPOINT)0);
            SetViewportOrgEx(SCROLLBAR_OFFSCREEN_DC, s.client_rect.right, 0, (LPPOINT)0);
            s.thumb_rect.left = s.client_rect.right - s.thumb_rect.left;
            s.thumb_rect.right = s.client_rect.right - s.thumb_rect.right;
            s.flipped_rect_edge = s.thumb_rect.left;
            s.thumb_rect.left = s.thumb_rect.right;
            s.thumb_rect.right = s.flipped_rect_edge;
          }
          DrawMaskedBitmapToRect(SCROLLBAR_OFFSCREEN_DC, &s.thumb_rect, s.thumb_bitmap,
                                 s.thumb_source_width, s.thumb_source_height,
                                 s.thumb_source_x, s.thumb_source_y, s.thumb_mask_x, s.thumb_mask_y);
          if (s.direction == 0)
          {
            SetMapMode(SCROLLBAR_OFFSCREEN_DC, MM_TEXT);
            SetWindowOrgEx(SCROLLBAR_OFFSCREEN_DC, 0, 0, (LPPOINT)0);
            SetViewportOrgEx(SCROLLBAR_OFFSCREEN_DC, 0, 0, (LPPOINT)0);
          }
        }
        else
        {
          SetRect(&s.thumb_rect,
                  s.thumb_position - s.client_rect.right / 20,
                  0,
                  s.thumb_position + s.client_rect.right / 20,
                  s.client_rect.bottom);
          FillRect(SCROLLBAR_OFFSCREEN_DC, &s.thumb_rect, GetStockObject(BLACK_BRUSH));
        }
      }
      RestoreDC(SCROLLBAR_OFFSCREEN_DC, s.saved_dc);
      BitBlt(s.paint_dc, 0, 0, s.client_rect.right, s.client_rect.bottom,
             SCROLLBAR_OFFSCREEN_DC, 0, 0, SRCCOPY);
      EndPaint(hwnd, &s.paint);
    }
    return 0;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (void *)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}
