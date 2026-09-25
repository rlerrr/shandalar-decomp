#include "magic_shell_dialogs.h"
#include <stdio.h>
#include <string.h>
#include "cardartlib/src/palette.h"
#include "deckdll/src/shared_resources.h"
#include "game_support.h"
#include "shared_startup.h"

extern HINSTANCE g_app_instance;
extern char g_shell_art_directory[];
extern CRITICAL_SECTION g_card_render_lock;
extern HDC g_shared_offscreen_dc;
void shell_set_map_mode(HDC dc, HWND hwnd, HBITMAP background);
int handle_button_palette_message(int hwnd, unsigned int message,
                                  int wparam, int lparam);

typedef struct shell_credits_dialog_context_t
{
  const char *text;
  RECT window_rect;
  HBITMAP background;
  COLORREF background_color;
  COLORREF text_color;
  HFONT normal_font;
  HFONT bold_font;
  RECT text_rect;
  int dialog_result;
} shell_credits_dialog_context_t;

// GLOBAL: MAGIC 0x00708a50
static RECT g_shell_credits_device_text_rect;
// GLOBAL: MAGIC 0x00708a60
static BITMAP g_shell_credits_background_bitmap;
// GLOBAL: MAGIC 0x00708a78
static HDC g_shell_credits_bitmap_dc;
// GLOBAL: MAGIC 0x00708a7c
static int g_shell_credits_scroll_offset;
// GLOBAL: MAGIC 0x00708a80
static shell_credits_dialog_context_t *g_shell_credits_context;
// GLOBAL: MAGIC 0x00708a84
static int g_shell_credits_text_height;

// FUNCTION: MAGIC 0x0055230f
static void shell_initialize_credits_resources(HBITMAP *background,
                                               RECT *window_rect,
                                               COLORREF *background_color,
                                               COLORREF *text_color,
                                               HFONT *normal_font,
                                               HFONT *bold_font)
{
  struct
  {
    char path[264];
    BITMAP bitmap;
    LOGFONTA *font_description;
  } s;

  if (g_display_color_depth == 8)
  {
    sprintf(s.path, "%s\\WINBK_Credits.pic", g_shell_art_directory);
    *background = load_pic(s.path);
    *background_color = 0x1000037;
    *text_color = 0x1000001;
  }
  else
  {
    sprintf(s.path, "%s\\WINBK_Credits16.bmp", g_shell_art_directory);
    *background = shell_load_bitmap_file(s.path, NULL, 0);
    *background_color = 0x4777b2;
    *text_color = 0x10101;
  }
  s.font_description = LoadFontFromIni("ShellCredits", 0);
  if (*background != NULL)
  {
    GetObjectA(*background, sizeof(s.bitmap), &s.bitmap);
    s.font_description->lfHeight =
        ((window_rect->bottom - window_rect->top) *
         s.font_description->lfHeight) / s.bitmap.bmHeight;
  }
  *normal_font = CreateFontIndirectA(s.font_description);
  s.font_description->lfWeight = FW_BOLD;
  *bold_font = CreateFontIndirectA(s.font_description);
}

// FUNCTION: MAGIC 0x00552429
static void shell_release_credits_resources(HBITMAP background,
                                            HFONT normal_font, HFONT bold_font)
{
  if (background != NULL)
    delete_and_close_object(background);
  if (normal_font != NULL)
    DeleteObject(normal_font);
  if (bold_font != NULL)
    DeleteObject(bold_font);
}

// FUNCTION: MAGIC 0x00552472
static BOOL CALLBACK shell_credits_dialog_proc(HWND hwnd, UINT message,
                                               WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int scroll_step;
    int line_height;
    HDC dialog_paint_dc;
    HDC offscreen_dc;
    int line_length;
    PAINTSTRUCT paint;
    int saved_dc;
    int text_top;
    int text_left;
    RECT update_rect;
    RECT client_rect;
    SIZE text_size;
    const char *text_cursor;
    HDC dialog_dc;
    RECT measure_rect;
  } s;
  switch (message)
  {
  case WM_INITDIALOG:
  {
    ShowWindow(GetDlgItem(hwnd, 0x740), SW_HIDE);
    g_shell_credits_context =
        (shell_credits_dialog_context_t *)lparam;
    MoveWindow(hwnd,
               g_shell_credits_context->window_rect.left,
               g_shell_credits_context->window_rect.top,
               g_shell_credits_context->window_rect.right -
                   g_shell_credits_context->window_rect.left,
               g_shell_credits_context->window_rect.bottom -
                   g_shell_credits_context->window_rect.top,
               TRUE);
    s.dialog_dc = GetDC(hwnd);
    shell_set_map_mode(s.dialog_dc, hwnd,
                       g_shell_credits_context->background);
    CopyRect(&g_shell_credits_device_text_rect,
             &g_shell_credits_context->text_rect);
    LPtoDP(s.dialog_dc,
           (LPPOINT)&g_shell_credits_device_text_rect, 2);
    SetMapMode(s.dialog_dc, MM_TEXT);
    SelectObject(s.dialog_dc,
                 g_shell_credits_context->normal_font);
    CopyRect(&s.measure_rect,
             &g_shell_credits_device_text_rect);
    g_shell_credits_text_height =
        DrawTextA(s.dialog_dc, g_shell_credits_context->text,
                  -1, &s.measure_rect, DT_CALCRECT | DT_WORDBREAK);
    g_shell_credits_bitmap_dc = CreateCompatibleDC(s.dialog_dc);
    ApplyCardArtPaletteToDc(g_shell_credits_bitmap_dc);
    SelectObject(g_shell_credits_bitmap_dc,
                 g_shell_credits_context->background);
    ReleaseDC(hwnd, s.dialog_dc);
    GetObjectA(g_shell_credits_context->background,
               sizeof(BITMAP), &g_shell_credits_background_bitmap);
    g_shell_credits_scroll_offset = 0;
    SetTimer(hwnd, 1, 100, NULL);
    return TRUE;
  }
  case WM_DESTROY:
  {
    DeleteDC(g_shell_credits_bitmap_dc);
    return FALSE;
  }
  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, message,
                                         (int)wparam, (int)lparam);
  case WM_ERASEBKGND:
    return TRUE;
  case WM_PAINT:
  {
    EnterCriticalSection(&g_card_render_lock);
    s.offscreen_dc = g_shared_offscreen_dc;
    s.saved_dc = SaveDC(g_shared_offscreen_dc);
    ApplyCardArtPaletteToDc(s.offscreen_dc);
    GetClientRect(hwnd, &s.client_rect);
    GetUpdateRect(hwnd, &s.update_rect, FALSE);
    IntersectClipRect(s.offscreen_dc, s.update_rect.left, s.update_rect.top,
                      s.update_rect.right, s.update_rect.bottom);
    s.dialog_paint_dc = BeginPaint(hwnd, &s.paint);
    SetMapMode(s.offscreen_dc, MM_TEXT);
    if (g_shell_credits_context->background != NULL)
    {
      StretchBlt(s.offscreen_dc, s.client_rect.left, s.client_rect.top,
                 s.client_rect.right - s.client_rect.left,
                 s.client_rect.bottom - s.client_rect.top,
                 g_shell_credits_bitmap_dc, 0, 0,
                 g_shell_credits_background_bitmap.bmWidth,
                 g_shell_credits_background_bitmap.bmHeight, SRCCOPY);
    }
    else
      FillRect(s.offscreen_dc, &s.client_rect,
               GetStockObject(GRAY_BRUSH));
    IntersectClipRect(s.offscreen_dc,
                      g_shell_credits_device_text_rect.left,
                      g_shell_credits_device_text_rect.top,
                      g_shell_credits_device_text_rect.right,
                      g_shell_credits_device_text_rect.bottom);
    OffsetViewportOrgEx(s.offscreen_dc, 0,
                        -g_shell_credits_scroll_offset, NULL);
    SetBkMode(s.offscreen_dc, TRANSPARENT);
    SelectObject(s.offscreen_dc,
                 g_shell_credits_context->normal_font);
    SetTextColor(s.offscreen_dc,
                 g_shell_credits_context->background_color);
    s.text_cursor = g_shell_credits_context->text;
    s.text_left = g_shell_credits_device_text_rect.left;
    s.text_top = g_shell_credits_device_text_rect.top;
    GetTextExtentPoint32A(s.offscreen_dc, "xyz", 3, &s.text_size);
    s.line_height = s.text_size.cy;
    while (*s.text_cursor != '\0' && *s.text_cursor != '@')
    {
      if (*s.text_cursor == '\r')
      {
        while (*s.text_cursor != '\0' &&
               (*s.text_cursor == '\r' || *s.text_cursor == '\n' ||
                *s.text_cursor == ' '))
        {
          if (*s.text_cursor == '\r')
            s.text_top += s.line_height;
          ++s.text_cursor;
        }
      }
      else
      {
        s.line_length = 0;
        while (s.text_cursor[s.line_length] != '\0' &&
               s.text_cursor[s.line_length] != '\r')
          ++s.line_length;
        if (*s.text_cursor == '#')
        {
          SelectObject(s.offscreen_dc,
                       g_shell_credits_context->bold_font);
          TextOutA(s.offscreen_dc, s.text_left, s.text_top,
                   s.text_cursor + 1, s.line_length - 1);
          SelectObject(s.offscreen_dc,
                       g_shell_credits_context->normal_font);
        }
        else
          TextOutA(s.offscreen_dc, s.text_left, s.text_top,
                   s.text_cursor, s.line_length);
        s.text_cursor += s.line_length;
        if (*s.text_cursor == '\r')
          ++s.text_cursor;
        if (*s.text_cursor == '\n')
          ++s.text_cursor;
        s.text_top += s.line_height;
      }
    }
    RestoreDC(g_shared_offscreen_dc, s.saved_dc);
    s.offscreen_dc = s.dialog_paint_dc;
    ApplyCardArtPaletteToDc(s.offscreen_dc);
    BitBlt(s.offscreen_dc, s.update_rect.left, s.update_rect.top,
           s.update_rect.right - s.update_rect.left,
           s.update_rect.bottom - s.update_rect.top, g_shared_offscreen_dc,
           s.update_rect.left, s.update_rect.top, SRCCOPY);
    EndPaint(hwnd, &s.paint);
    LeaveCriticalSection(&g_card_render_lock);
    return TRUE;
  }
  case WM_TIMER:
  {
    if (g_shell_credits_text_height <
        g_shell_credits_scroll_offset)
      SendMessageA(hwnd, WM_COMMAND, 1, 0);
    else
    {
      for (s.scroll_step = 0; s.scroll_step < 8; ++s.scroll_step)
      {
        g_shell_credits_scroll_offset += 2;
        InvalidateRect(hwnd,
                       &g_shell_credits_device_text_rect, FALSE);
        UpdateWindow(hwnd);
      }
    }
    return TRUE;
  }
  case WM_KEYDOWN:
  case WM_COMMAND:
  {
    EndDialog(hwnd, 0);
    return TRUE;
  }
  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
  {
    EndDialog(hwnd, 0);
    return TRUE;
  }
  default:
    return FALSE;
  }
}

// FUNCTION: MAGIC 0x00552260
BOOL shell_show_credits(HWND hwnd, void *text, RECT *rect)
{
  struct
  {
    void *text;
    RECT window_rect;
    HBITMAP background;
    COLORREF background_color;
    COLORREF text_color;
    HFONT normal_font;
    HFONT bold_font;
    RECT dialog_rect;
    int dialog_result;
  } s;

  s.text = text;
  CopyRect(&s.window_rect, rect);
  shell_initialize_credits_resources(
      &s.background, &s.window_rect, &s.background_color, &s.text_color,
      &s.normal_font, &s.bold_font);
  SetRect(&s.dialog_rect, 0x2f, 0x28, 0x23a, 0x2c0);
  s.dialog_result = DialogBoxParamA(g_app_instance, (LPCSTR)0xfe, hwnd,
                                    shell_credits_dialog_proc, (LPARAM)&s.text);
  shell_release_credits_resources(s.background, s.normal_font, s.bold_font);
  if (s.dialog_result == -1)
    return FALSE;
  else
    return TRUE;
}
