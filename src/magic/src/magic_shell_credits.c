#include "magic_shell_dialogs.h"
#include <stdio.h>
#include "deckdll/src/shared_resources.h"
#include "game_support.h"
#include "shared_startup.h"

extern HINSTANCE g_app_instance;
extern char g_shell_art_directory[];

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
  (void)hwnd;
  (void)message;
  (void)wparam;
  (void)lparam;
  return FALSE;
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
