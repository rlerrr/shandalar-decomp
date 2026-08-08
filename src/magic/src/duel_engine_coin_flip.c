#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vfw.h>

#include "cardartlib/src/palette.h"
#include "drawcardlib/Drawcardlib.h"
#include "drawcardlib/src/pic.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/network.h"
#include "magic/src/shared_startup.h"

#ifndef MCIWNDM_PLAY
#define MCIWNDM_PLAY 0x806
#endif
#ifndef MCIWNDM_STOP
#define MCIWNDM_STOP 0x808
#endif

extern HINSTANCE g_app_instance;
extern int g_manalink_is_host;
extern int DAT_00638c08;
extern HPALETTE global_cart_art_hpalette;
extern card_ptr_t global_raw_cards_storage[2000];
#ifndef SHANDALAR
extern HANDLE global_mutex_GameInit;
#endif

void delete_and_close_object(HANDLE obj);
void resize_duel_hand_window(HWND hwnd);
void set_player_directive_value(int player, int value);
void refresh_duel_window(HWND hwnd);
void change_buttonclass_wndproc(HWND hwnd);
void draw_duel_dialog_bitmap_button(DRAWITEMSTRUCT *draw_item,
                                    HBITMAP normal_bitmap,
                                    HBITMAP pressed_bitmap,
                                    HBITMAP disabled_bitmap,
                                    COLORREF text_color,
                                    int draw_focus);
BOOL CALLBACK dlgproc_duel_coin_flip_animation(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void load_coin_flip_animation_dialog_assets(HBRUSH *brush, COLORREF *text_color);
void cleanup_coin_flip_animation_dialog_assets(HGDIOBJ brush);
void load_redraw_ante_dialog_assets(HBITMAP *background,
                                    int *text_color,
                                    HBITMAP *normal_button_bitmap,
                                    HBITMAP *depressed_button_bitmap,
                                    HBITMAP *disabled_button_bitmap,
                                    COLORREF *normal_color,
                                    COLORREF *focus_color);
void cleanup_redraw_ante_dialog_assets(HANDLE background,
                                       HANDLE normal_button_bitmap,
                                       HANDLE depressed_button_bitmap,
                                       HANDLE disabled_button_bitmap);
void __cdecl redraw_ante_wait_for_opponent_done_thread(void *hwnd);
void __cdecl redraw_ante_wait_for_opponent_mulligan_thread(void *hwnd);

typedef struct DuelCoinFlipAnimationDialogContext
{
  char title[100];
  int coin_result;
  int show_coin_flips;
} DuelCoinFlipAnimationDialogContext;

// GLOBAL: MAGIC 0x00638c74
HBITMAP g_coin_flip_dialog_background;

// GLOBAL: MAGIC 0x00638c68
int g_coin_flip_dialog_text_color;

// GLOBAL: MAGIC 0x00638bac
HBITMAP g_coin_flip_dialog_button_normal_bitmap;

// GLOBAL: MAGIC 0x00638c58
HBITMAP g_coin_flip_dialog_button_depressed_bitmap;

// GLOBAL: MAGIC 0x00638be0
COLORREF g_coin_flip_dialog_button_normal_color;

// GLOBAL: MAGIC 0x00638ba0
COLORREF g_coin_flip_dialog_button_focus_color;

// GLOBAL: MAGIC 0x005726a0
HWND g_coin_flip_animation_mci_hwnd = (HWND)0;

// GLOBAL: MAGIC 0x00638b7c
HBRUSH g_coin_flip_animation_dialog_brush;

// GLOBAL: MAGIC 0x00638c60
COLORREF g_coin_flip_animation_dialog_text_color;

// GLOBAL: MAGIC 0x00638c64
DuelCoinFlipAnimationDialogContext *g_coin_flip_animation_dialog_context;

typedef struct
{
  int coin_winner;
  int was_random_starting_player;
  int play_draw_choice;
  int coin_flip_result;
} DuelCoinFlipDialogContext;

typedef struct
{
  int starting_player;
  int opponent_ante_card;
  int player_ante_card;
  int redraw_result;
  int ante_result;
  int ante_info;
  int mulligan_accepted;
  int opponent_mulligan_accepted;
} DuelRedrawAnteDialogContext;

// GLOBAL: MAGIC 0x00638c10
HBITMAP g_redraw_ante_dialog_background;

// GLOBAL: MAGIC 0x00638c88
int g_redraw_ante_dialog_text_color;

// GLOBAL: MAGIC 0x00638c3c
HBITMAP g_redraw_ante_dialog_button_normal_bitmap;

// GLOBAL: MAGIC 0x00638b4c
HBITMAP g_redraw_ante_dialog_button_depressed_bitmap;

// GLOBAL: MAGIC 0x00638b40
HBITMAP g_redraw_ante_dialog_button_disabled_bitmap;

// GLOBAL: MAGIC 0x00638c04
COLORREF g_redraw_ante_dialog_button_normal_color;

// GLOBAL: MAGIC 0x00638bf0
COLORREF g_redraw_ante_dialog_button_focus_color;

// GLOBAL: MAGIC 0x00638b74
int g_redraw_ante_waiting_for_both_network_choices;

// GLOBAL: MAGIC 0x00638c0c
char g_redraw_ante_opponent_choice_received;

// FUNCTION: MAGIC 0x004a3b82
// FUNCTION: SHANDALAR 0x0053f510
int coin_flip(int player, char *dialog_title, int show_dialog_if_animation_is_off)
{
  struct
  {
    int palette_index;           // ebp - 0x884
    LOGPALETTE log_palette[128]; // ebp - 0x880
    int pad;
    PALETTEENTRY palette_entries[256];
    HPALETTE coin_flip_palette;
    HPALETTE old_palette;
    DuelCoinFlipAnimationDialogContext animation_context;
    int dialog_result;
    int show_dialog;
  } s;

  KillTimer(g_duel_window_hwnd, g_duel_timer_id);
  if ((g_duel_network_flags & 2) != 0)
  {
    if (g_manalink_is_host != 0)
    {
      g_network_result_packet_type = 1;
      s.animation_context.coin_result = rand() % 2;
      g_network_result_value = s.animation_context.coin_result;
      TENTATIVE_send_network_result(player, 1);
    }
    else
    {
      TENTATIVE_wait_for_network_result(player, 1);
      s.animation_context.coin_result = g_network_result_value;
    }
  }
  else
  {
    s.animation_context.coin_result = rand() % 2;
  }
  s.animation_context.show_coin_flips = g_duel_interface_options.show_coin_flips;
  if (g_duel_interface_options.show_coin_flips != 0)
  {
    s.show_dialog = 1;
  }
  else if (show_dialog_if_animation_is_off != 0)
  {
    s.show_dialog = 1;
  }
  else
  {
    s.show_dialog = 0;
  }

  if (g_duel_ai_mode_state == 1)
  {
    s.show_dialog = 0;
  }
  if (s.show_dialog != 0)
  {
    strcpy(s.animation_context.title, dialog_title);
    GetPaletteEntries(global_cart_art_hpalette, 0, 0x100, s.palette_entries);
    for (s.palette_index = 0; s.palette_index < 0x100; ++s.palette_index)
    {
      s.palette_entries[s.palette_index].peFlags = 0;
    }
    s.log_palette[0].palVersion = 0x300;
    s.log_palette[0].palNumEntries = 0x100;
    memcpy(s.log_palette[0].palPalEntry, s.palette_entries, 0x400);
    s.coin_flip_palette = CreatePalette(s.log_palette);
    s.old_palette = global_cart_art_hpalette;
    global_cart_art_hpalette = s.coin_flip_palette;
    s.dialog_result = DialogBoxParamA(g_app_instance,
                                      (LPCSTR)0xf0,
                                      g_duel_window_hwnd,
                                      dlgproc_duel_coin_flip_animation,
                                      (LPARAM)&s.animation_context);
    InvalidateRect(g_duel_player_battlefield_window_hwnd, (RECT *)0, 1);
    InvalidateRect(g_duel_help_owner_hwnd, (RECT *)0, 1);
    global_cart_art_hpalette = s.old_palette;
    ApplyCardArtPaletteToDc(g_shared_offscreen_dc);
    DeleteObject(s.coin_flip_palette);
  }
  return s.animation_context.coin_result;
}

// FUNCTION: MAGIC 0x004a44b7
void load_coin_flip_animation_dialog_assets(HBRUSH *brush, COLORREF *text_color)
{
  *brush = CreateSolidBrush(0x100000d);
  *text_color = 0x10000bf;
}

// FUNCTION: MAGIC 0x004a44db
void cleanup_coin_flip_animation_dialog_assets(HGDIOBJ brush)
{
  if (brush != (HGDIOBJ)0)
  {
    DeleteObject(brush);
  }
}

// FUNCTION: MAGIC 0x004a3d9c
BOOL CALLBACK dlgproc_duel_coin_flip_animation(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC erase_dc;
    RECT erase_rect;
    HWND ctl_hwnd;
    int ctl_id;
    HDC ctl_dc;
    char avi_path[264];
    HDC measure_dc;
    HGDIOBJ prompt_font;
    RECT rect;
    SIZE prompt_size;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    g_coin_flip_animation_dialog_context = (DuelCoinFlipAnimationDialogContext *)lparam;
    load_coin_flip_animation_dialog_assets(&g_coin_flip_animation_dialog_brush,
                                           &g_coin_flip_animation_dialog_text_color);
    load_text("UIStrings.txt", "DIALOG_COINFLIP");
    if (g_coin_flip_animation_dialog_context->coin_result != 0)
    {
      SetDlgItemTextA(hwnd, 0x41c, text_lines[1]);
    }
    else
    {
      SetDlgItemTextA(hwnd, 0x41c, text_lines[0]);
    }
    ShowWindow(GetDlgItem(hwnd, 0x41c), 0);
    SetDlgItemTextA(hwnd, 0x41b, g_coin_flip_animation_dialog_context->title);
    s.prompt_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x41b, WM_GETFONT, 0, 0);
    s.measure_dc = GetDC(hwnd);
    SelectObject(s.measure_dc, s.prompt_font);
    GetTextExtentPoint32A(s.measure_dc,
                          g_coin_flip_animation_dialog_context->title,
                          strlen(g_coin_flip_animation_dialog_context->title),
                          &s.prompt_size);
    ReleaseDC(hwnd, s.measure_dc);
    GetClientRect(hwnd, &s.rect);
    s.prompt_size.cx += s.prompt_size.cy;
    s.prompt_size.cy += 5;
    MoveWindow(GetDlgItem(hwnd, 0x41b),
               (s.rect.right - s.prompt_size.cx) / 2,
               0x14,
               s.prompt_size.cx,
               s.prompt_size.cy,
               1);
    if (g_coin_flip_animation_dialog_context->show_coin_flips != 0)
    {
      if (g_coin_flip_animation_dialog_context->coin_result != 0)
      {
        sprintf(s.avi_path, "%s\\COINTOSS_Tails.AVI", global_duelart_path);
      }
      else
      {
        sprintf(s.avi_path, "%s\\COINTOSS_Heads.AVI", global_duelart_path);
      }
      g_coin_flip_animation_mci_hwnd = MCIWndCreateA(hwnd, g_app_instance, 0x50000102, s.avi_path);
      SendMessageA(g_coin_flip_animation_mci_hwnd, MCIWNDM_SETPALETTE, (WPARAM)global_cart_art_hpalette, 0);
      GetWindowRect(g_coin_flip_animation_mci_hwnd, &s.rect);
      AdjustWindowRect(&s.rect, GetWindowLongA(hwnd, GWL_STYLE), 0);
      MoveWindow(hwnd, s.rect.left, s.rect.top, s.rect.right - s.rect.left, s.rect.bottom - s.rect.top, 1);
    }
    else
    {
      if (g_coin_flip_animation_dialog_context->coin_result != 0)
      {
        sprintf(s.avi_path, "%s\\COINTOSS_Heads.AVI", global_duelart_path);
      }
      else
      {
        sprintf(s.avi_path, "%s\\COINTOSS_Tails.AVI", global_duelart_path);
      }
      g_coin_flip_animation_mci_hwnd = MCIWndCreateA(hwnd, g_app_instance, 0x50000102, s.avi_path);
      SendMessageA(g_coin_flip_animation_mci_hwnd, MCIWNDM_SETPALETTE, (WPARAM)global_cart_art_hpalette, 0);
    }
    if (g_coin_flip_animation_dialog_context->show_coin_flips != 0)
    {
      if (SetTimer(hwnd, 1, 10, (TIMERPROC)0) == 0)
      {
        PostMessageA(hwnd, WM_TIMER, 1, 0);
      }
    }
    else
    {
      if (SetTimer(hwnd, 1, 1000, (TIMERPROC)0) == 0)
      {
        PostMessageA(hwnd, WM_TIMER, 1, 0);
      }
    }
    SetTimer(hwnd, 2, 15000, (TIMERPROC)0);
    SetFocus(hwnd);
    return 0;

  case WM_DESTROY:
    cleanup_coin_flip_animation_dialog_assets(g_coin_flip_animation_dialog_brush);
    return 0;

  case WM_CTLCOLORSTATIC:
    s.ctl_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.ctl_dc);
    s.ctl_hwnd = (HWND)lparam;
    s.ctl_id = GetDlgCtrlID(s.ctl_hwnd);
    SetBkMode(s.ctl_dc, TRANSPARENT);
    SetTextColor(s.ctl_dc, g_coin_flip_animation_dialog_text_color);
    return (BOOL)g_coin_flip_animation_dialog_brush;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    FillRect(s.erase_dc, &s.erase_rect, g_coin_flip_animation_dialog_brush);
    return 1;

  case WM_CLOSE:
  case WM_CHAR:
    KillTimer(hwnd, 2);
    if (g_coin_flip_animation_dialog_context->show_coin_flips != 0)
    {
      SendMessageA(g_coin_flip_animation_mci_hwnd, MCIWNDM_STOP, 0, 0);
    }
    SendMessageA(g_coin_flip_animation_mci_hwnd, WM_CLOSE, 0, 0);
    EndDialog(hwnd, 0);
    return 1;

  case WM_PARENTNOTIFY:
    if (((unsigned int)wparam & 0xffff) == WM_LBUTTONDOWN || ((unsigned int)wparam & 0xffff) == WM_RBUTTONDOWN)
    {
      KillTimer(hwnd, 2);
      if (g_coin_flip_animation_dialog_context->show_coin_flips != 0)
      {
        SendMessageA(g_coin_flip_animation_mci_hwnd, MCIWNDM_STOP, 0, 0);
      }
      SendMessageA(g_coin_flip_animation_mci_hwnd, WM_CLOSE, 0, 0);
      EndDialog(hwnd, 0);
    }
    return 1;

  case WM_KEYDOWN:
    if (lparam == 0x20d)
    {
      KillTimer(hwnd, 2);
      if (g_coin_flip_animation_dialog_context->show_coin_flips != 0)
      {
        SendMessageA(g_coin_flip_animation_mci_hwnd, MCIWNDM_STOP, 0, 0);
      }
      SendMessageA(g_coin_flip_animation_mci_hwnd, WM_CLOSE, 0, 0);
      EndDialog(hwnd, 0);
    }
    return 1;

  case WM_TIMER:
    KillTimer(hwnd, (UINT)wparam);
    if (wparam == 1)
    {
      if (g_coin_flip_animation_dialog_context->show_coin_flips != 0)
      {
        SendMessageA(g_coin_flip_animation_mci_hwnd, MCIWNDM_PLAY, 0, 0);
        play_sound_effect(0x3b);
      }
      else
      {
        ShowWindow(GetDlgItem(hwnd, 0x41c), 1);
      }
      SetFocus(hwnd);
    }
    else
    {
      if (g_coin_flip_animation_dialog_context->show_coin_flips != 0)
      {
        SendMessageA(g_coin_flip_animation_mci_hwnd, MCIWNDM_STOP, 0, 0);
      }
      SendMessageA(g_coin_flip_animation_mci_hwnd, WM_CLOSE, 0, 0);
      EndDialog(hwnd, 0);
    }
    return 1;

  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x0049a6c8
BOOL CALLBACK dlgproc_duel_redraw_ante(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC paint_dc;
    PAINTSTRUCT paint;
    RECT card_rect;
    POINT mouse_point;
    RECT player_card_rect;
    RECT opponent_card_rect;
    HDC erase_dc;
    RECT erase_rect;
    int draw_focus;
    COLORREF draw_color;
    DRAWITEMSTRUCT *draw_item;
    HWND ctl_hwnd;
    int ctl_id;
    HDC ctl_dc;
    HWND invalidate_b;
    HWND invalidate_a;
    RECT dialog_rect;
    HDC measure_dc;
    int button_height;
    int button_width;
    HGDIOBJ button_font;
    RECT button_rect;
    SIZE text_size;
    int command_id;
    char text[500];
    int ante_info;
    char player_name[100];
    DuelRedrawAnteDialogContext *context;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    g_redraw_ante_waiting_for_both_network_choices = 0;
    BringWindowToTop(g_duel_full_card_window_hwnd);
    s.context = (DuelRedrawAnteDialogContext *)lparam;
    SetWindowLongA(hwnd, DWL_USER, (LONG)s.context);
    ShowWindow(GetDlgItem(hwnd, 0x448), SW_HIDE);
    ShowWindow(GetDlgItem(hwnd, 0x449), SW_HIDE);
    DAT_00638c08 = 0;
    load_redraw_ante_dialog_assets(&g_redraw_ante_dialog_background,
                                   &g_redraw_ante_dialog_text_color,
                                   &g_redraw_ante_dialog_button_normal_bitmap,
                                   &g_redraw_ante_dialog_button_depressed_bitmap,
                                   &g_redraw_ante_dialog_button_disabled_bitmap,
                                   &g_redraw_ante_dialog_button_normal_color,
                                   &g_redraw_ante_dialog_button_focus_color);
    copy_opponent_name_prefix(s.player_name);
    load_text(global_ui_strings_filename, "DIALOG_MULLIGAN");
    if (s.context->starting_player == 1)
    {
      sprintf(s.text, text_lines[0], s.player_name);
      SetDlgItemTextA(hwnd, 0x442, s.text);
    }
    else
    {
      SetDlgItemTextA(hwnd, 0x442, text_lines[1]);
    }
    sprintf(s.text, text_lines[2], s.player_name);
    SetDlgItemTextA(hwnd, 0x446, s.text);
    SetDlgItemTextA(hwnd, 0x447, text_lines[3]);
    if ((g_duel_network_flags & 2) == 0)
    {
      if (s.context->ante_result != 0)
      {
        if (s.context->ante_result == 1)
        {
          sprintf(s.text, text_lines[4], s.player_name);
        }
        else if (s.context->ante_result == 2)
        {
          sprintf(s.text, text_lines[5], s.player_name);
        }
        else
        {
          sprintf(s.text, text_lines[6], s.player_name);
        }
        SetDlgItemTextA(hwnd, 0x443, s.text);
      }
      else
      {
        sprintf(s.text, text_lines[7], s.player_name);
        SetDlgItemTextA(hwnd, 0x443, s.text);
      }
      if (s.context->ante_result != 0)
      {
        SetDlgItemTextA(hwnd, 0x444, text_lines[10]);
        ShowWindow(GetDlgItem(hwnd, 0x444), SW_SHOW);
        SetFocus(GetDlgItem(hwnd, 0x444));
        SendMessageA(hwnd, 0x401, 0x444, 0);
      }
      else
      {
        if (s.context->redraw_result != 0)
        {
          if (s.context->ante_result == 0)
          {
            ShowWindow(GetDlgItem(hwnd, 0x443), SW_HIDE);
          }
          SetDlgItemTextA(hwnd, 0x444, text_lines[10]);
          ShowWindow(GetDlgItem(hwnd, 0x444), SW_SHOW);
          SetFocus(GetDlgItem(hwnd, 0x444));
          SendMessageA(hwnd, 0x401, 0x444, 0);
        }
        else
        {
          ShowWindow(GetDlgItem(hwnd, 0x444), SW_HIDE);
          SetFocus(GetDlgItem(hwnd, IDOK));
          SendMessageA(hwnd, 0x401, IDOK, 0);
        }
      }
    }
    else
    {
      g_redraw_ante_opponent_choice_received = 0;
      if (s.context->redraw_result != 0 && s.context->ante_result != 0)
      {
        g_redraw_ante_waiting_for_both_network_choices = 1;
      }
      ShowWindow(GetDlgItem(hwnd, IDOK), SW_HIDE);
      if (s.context->redraw_result != 0 && (s.context->ante_result == 0 || g_manalink_is_host != 0))
      {
        SetDlgItemTextA(hwnd, 0x444, text_lines[10]);
        ShowWindow(GetDlgItem(hwnd, 0x444), SW_SHOW);
        SetFocus(GetDlgItem(hwnd, 0x444));
        SendMessageA(hwnd, 0x401, 0x444, 0);
        ShowWindow(GetDlgItem(hwnd, IDOK), SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, 0x443), SW_HIDE);
      }
      else
      {
        if (s.context->ante_result != 0)
        {
          ShowWindow(GetDlgItem(hwnd, 0x444), SW_HIDE);
          _beginthread(redraw_ante_wait_for_opponent_done_thread, 0, hwnd);
        }
        else
        {
          ShowWindow(GetDlgItem(hwnd, 0x443), SW_HIDE);
          ShowWindow(GetDlgItem(hwnd, 0x444), SW_HIDE);
          ShowWindow(GetDlgItem(hwnd, IDOK), SW_SHOW);
          SetFocus(GetDlgItem(hwnd, IDOK));
          SendMessageA(hwnd, 0x401, IDOK, 0);
        }
      }
    }
    s.context->opponent_mulligan_accepted = 0;
    if (s.context->ante_info != 0)
    {
      sprintf(s.text, text_lines[8], s.player_name);
    }
    else
    {
      sprintf(s.text, text_lines[9], s.player_name);
    }
    SetDlgItemTextA(hwnd, 0x445, s.text);
    ShowWindow(GetDlgItem(hwnd, 0x445), SW_HIDE);
    s.context->mulligan_accepted = 0;
    SetDlgItemTextA(hwnd, IDOK, text_lines[11]);

    s.measure_dc = GetDC(hwnd);
    ApplyCardArtPaletteToDc(s.measure_dc);
    s.button_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x444, WM_GETFONT, 0, 0);
    SelectObject(s.measure_dc, s.button_font);
    GetWindowRect(GetDlgItem(hwnd, 0x444), &s.button_rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.button_rect, 2);
    GetDlgItemTextA(hwnd, 0x444, s.text, 200);
    GetTextExtentPoint32A(s.measure_dc, s.text, strlen(s.text), &s.text_size);
    s.button_width = s.text_size.cx + s.text_size.cy * 2;
    s.button_height = (s.text_size.cy * 5) / 2;
    s.button_rect.left = s.button_rect.left + ((s.button_rect.right - s.button_rect.left) / 2 - s.button_width / 2);
    MoveWindow(GetDlgItem(hwnd, 0x444),
               s.button_rect.left,
               s.button_rect.top,
               s.button_width,
               s.button_height,
               1);

    GetWindowRect(GetDlgItem(hwnd, IDOK), &s.button_rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.button_rect, 2);
    GetDlgItemTextA(hwnd, IDOK, s.text, 200);
    GetTextExtentPoint32A(s.measure_dc, s.text, strlen(s.text), &s.text_size);
    s.button_width = s.text_size.cx + s.text_size.cy * 2;
    s.button_height = (s.text_size.cy * 5) / 2;
    s.button_rect.left = s.button_rect.left + ((s.button_rect.right - s.button_rect.left) / 2 - s.button_width / 2);
    MoveWindow(GetDlgItem(hwnd, IDOK),
               s.button_rect.left,
               s.button_rect.top,
               s.button_width,
               s.button_height,
               1);

    if (s.context->opponent_ante_card == -1)
    {
      ShowWindow(GetDlgItem(hwnd, 0x448), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, 0x446), SW_HIDE);
    }
    if (s.context->player_ante_card == -1)
    {
      ShowWindow(GetDlgItem(hwnd, 0x449), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, 0x447), SW_HIDE);
    }
    ReleaseDC(hwnd, s.measure_dc);
    change_buttonclass_wndproc(hwnd);
    if (((g_duel_network_flags & 2) == 0 && s.context->ante_result != 0) || s.context->redraw_result != 0)
    {
      GetWindowRect(hwnd, &s.dialog_rect);
      SetWindowPos(hwnd, (HWND)0, 5, s.dialog_rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
    return 0;

  case WM_COMMAND:
    s.command_id = (unsigned int)wparam & 0xffff;
    switch (s.command_id)
    {
    case IDOK:
    case IDCANCEL:
      s.context = (DuelRedrawAnteDialogContext *)GetWindowLongA(hwnd, DWL_USER);
      if ((g_duel_network_flags & 2) != 0)
      {
        if (s.context->redraw_result != 0 || s.context->ante_result != 0)
        {
          g_network_result_packet_type = 6;
          g_network_result_value = 0;
          TENTATIVE_send_network_result(0, 6);
        }
        EnableWindow(GetDlgItem(hwnd, IDOK), 0);
        ShowWindow(GetDlgItem(hwnd, 0x444), SW_HIDE);
        if (s.context->redraw_result != 0 && g_redraw_ante_opponent_choice_received == 0)
        {
          _beginthread(redraw_ante_wait_for_opponent_mulligan_thread, 0, hwnd);
        }
        else
        {
          SetTimer(hwnd, 1, 2000, (TIMERPROC)0);
        }
      }
      if ((g_duel_network_flags & 2) == 0)
      {
        cleanup_redraw_ante_dialog_assets(g_redraw_ante_dialog_background,
                                          g_redraw_ante_dialog_button_normal_bitmap,
                                          g_redraw_ante_dialog_button_depressed_bitmap,
                                          g_redraw_ante_dialog_button_disabled_bitmap);
        EndDialog(hwnd, s.context->mulligan_accepted);
      }
      break;

    case 0x444:
      s.context = (DuelRedrawAnteDialogContext *)GetWindowLongA(hwnd, DWL_USER);
      s.context->mulligan_accepted = 1;
      EnableWindow(GetDlgItem(hwnd, 0x444), 0);
      ShowWindow(GetDlgItem(hwnd, IDOK), SW_HIDE);
      if ((g_duel_network_flags & 2) != 0)
      {
        g_network_result_packet_type = 6;
        g_network_result_value = 1;
        TENTATIVE_send_network_result(0, 6);
        if (s.context->redraw_result != 0 && g_redraw_ante_opponent_choice_received == 0)
        {
          _beginthread(redraw_ante_wait_for_opponent_mulligan_thread, 0, hwnd);
        }
        else
        {
          SetTimer(hwnd, 1, 2000, (TIMERPROC)0);
        }
      }
      if ((g_duel_network_flags & 2) == 0)
      {
        if (s.context->ante_result == 0)
        {
          Sleep(500);
          ShowWindow(GetDlgItem(hwnd, 0x445), SW_SHOW);
          if (s.context->ante_info != 0)
          {
            SendMessageA(g_duel_life_window_hwnd, 0x40c, 0, 0);
          }
        }
        SetTimer(hwnd, 1, 2000, (TIMERPROC)0);
      }
      break;
    case 0x402:
      s.context = (DuelRedrawAnteDialogContext *)GetWindowLongA(hwnd, DWL_USER);
      s.ante_info = s.context->ante_result;
      s.context->opponent_mulligan_accepted = lparam;
      g_redraw_ante_opponent_choice_received = 1;
      if ((s.context->redraw_result == 0 || g_redraw_ante_waiting_for_both_network_choices != 0) &&
          (g_redraw_ante_waiting_for_both_network_choices != 0 ||
           (lparam != 0 && s.context->mulligan_accepted == 0)))
      {
        g_redraw_ante_waiting_for_both_network_choices = 0;
        load_text(global_ui_strings_filename, "DIALOG_MULLIGAN");
        copy_opponent_name_prefix(s.player_name);
        if (s.context->opponent_mulligan_accepted != 0)
        {
          if (s.ante_info == 1)
          {
            sprintf(s.text, text_lines[4], s.player_name);
          }
          else if (s.ante_info == 2)
          {
            sprintf(s.text, text_lines[5], s.player_name);
          }
          else
          {
            sprintf(s.text, text_lines[6], s.player_name);
          }
          SetDlgItemTextA(hwnd, 0x443, s.text);
          ShowWindow(GetDlgItem(hwnd, 0x443), SW_SHOW);
        }
        SetDlgItemTextA(hwnd, 0x444, text_lines[10]);
        ShowWindow(GetDlgItem(hwnd, 0x444), SW_SHOW);
        SetFocus(GetDlgItem(hwnd, 0x444));
        SendMessageA(hwnd, 0x401, 0x444, 0);
        if (s.context->opponent_mulligan_accepted != 0)
        {
          g_duel_surface_reset_state = 1;
          resize_duel_hand_window(g_duel_life_window_hwnd);
        }
      }
      ShowWindow(GetDlgItem(hwnd, IDOK), SW_SHOW);
      break;

    case 0x401:
      s.context = (DuelRedrawAnteDialogContext *)GetWindowLongA(hwnd, DWL_USER);
      s.context->opponent_mulligan_accepted = lparam;
      if (s.context->opponent_mulligan_accepted != 0)
      {
        load_text(global_ui_strings_filename, "DIALOG_MULLIGAN");
        copy_opponent_name_prefix(s.player_name);
        if (s.context->opponent_mulligan_accepted != 0)
        {
          sprintf(s.text, text_lines[8], s.player_name);
        }
        else
        {
          sprintf(s.text, text_lines[9], s.player_name);
        }
        SetDlgItemTextA(hwnd, 0x445, s.text);
        ShowWindow(GetDlgItem(hwnd, 0x445), SW_SHOW);
      }
      SetTimer(hwnd, 1, 2000, (TIMERPROC)0);
      break;
    }
    return 1;

  case 0x4c8:
    s.invalidate_a = (HWND)wparam;
    s.invalidate_b = (HWND)lparam;
    if (s.invalidate_a != (HWND)0)
    {
      SendMessageA(hwnd, 0x401, GetDlgCtrlID(s.invalidate_a), 0);
    }
    if (s.invalidate_a != (HWND)0)
    {
      InvalidateRect(s.invalidate_a, (RECT *)0, 1);
    }
    if (s.invalidate_b != (HWND)0)
    {
      InvalidateRect(s.invalidate_b, (RECT *)0, 1);
    }
    return 0;

  case WM_CTLCOLORSTATIC:
    s.ctl_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.ctl_dc);
    s.ctl_hwnd = (HWND)lparam;
    s.ctl_id = GetDlgCtrlID(s.ctl_hwnd);
    SetBkMode(s.ctl_dc, TRANSPARENT);
    SetTextColor(s.ctl_dc, g_redraw_ante_dialog_text_color);
    return (BOOL)GetStockObject(HOLLOW_BRUSH);

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
    {
      s.draw_color = g_redraw_ante_dialog_button_focus_color;
    }
    else
    {
      s.draw_color = g_redraw_ante_dialog_button_normal_color;
    }
    s.draw_focus = 0;
    if (IsWindowVisible(GetDlgItem(hwnd, 0x444)) == 0)
    {
      s.draw_color = g_redraw_ante_dialog_button_normal_color;
      s.draw_focus = 1;
    }
    draw_duel_dialog_bitmap_button(s.draw_item,
                                   g_redraw_ante_dialog_button_normal_bitmap,
                                   g_redraw_ante_dialog_button_depressed_bitmap,
                                   g_redraw_ante_dialog_button_disabled_bitmap,
                                   s.draw_color,
                                   s.draw_focus);
    return 1;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    if (g_redraw_ante_dialog_background != (HBITMAP)0)
    {
      DrawBitmapToRect(s.erase_dc, &s.erase_rect, g_redraw_ante_dialog_background);
    }
    else
    {
      FillRect(s.erase_dc, &s.erase_rect, GetStockObject(2));
    }
    return 1;

  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    return 1;

  case WM_MOUSEMOVE:
  case WM_RBUTTONDOWN:
    s.mouse_point.x = (unsigned int)lparam & 0xffff;
    s.mouse_point.y = (unsigned short)(((unsigned int)lparam >> 0x10) & 0xffff);
    s.context = (DuelRedrawAnteDialogContext *)GetWindowLongA(hwnd, DWL_USER);
    if ((msg == WM_MOUSEMOVE && g_duel_interface_options.layout != 2) ||
        (msg == WM_RBUTTONDOWN && g_duel_interface_options.layout == 2))
    {
      GetWindowRect(GetDlgItem(hwnd, 0x448), &s.opponent_card_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.opponent_card_rect, 2);
      GetWindowRect(GetDlgItem(hwnd, 0x449), &s.player_card_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.player_card_rect, 2);
      if (s.context->opponent_ante_card != -1 &&
          PtInRect(&s.opponent_card_rect, s.mouse_point) != 0)
      {
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401, s.context->opponent_ante_card, 0);
      }
      else if (s.context->player_ante_card != -1 &&
               PtInRect(&s.player_card_rect, s.mouse_point) != 0)
      {
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401, s.context->player_ante_card, 0);
      }
    }
    return 0;

  case WM_PAINT:
    UpdateWindow(GetDlgItem(hwnd, 0x442));
    UpdateWindow(GetDlgItem(hwnd, 0x447));
    UpdateWindow(GetDlgItem(hwnd, 0x446));
    s.context = (DuelRedrawAnteDialogContext *)GetWindowLongA(hwnd, DWL_USER);
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      if (s.context->player_ante_card != -1)
      {
        GetWindowRect(GetDlgItem(hwnd, 0x449), &s.card_rect);
        MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.card_rect, 2);
        DrawFullCard(s.paint_dc, &s.card_rect, global_raw_cards_storage + s.context->player_ante_card, 0, 2, 0, gs_illus_00789130);
      }
      if (s.context->opponent_ante_card != -1)
      {
        GetWindowRect(GetDlgItem(hwnd, 0x448), &s.card_rect);
        MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.card_rect, 2);
        DrawFullCard(s.paint_dc, &s.card_rect, global_raw_cards_storage + s.context->opponent_ante_card, 0, 2, 0, gs_illus_00789130);
      }
      EndPaint(hwnd, &s.paint);
    }
    return 1;

  case WM_TIMER:
    s.context = (DuelRedrawAnteDialogContext *)GetWindowLongA(hwnd, DWL_USER);
    s.command_id = wparam;
    if (s.command_id == 1)
    {
      cleanup_redraw_ante_dialog_assets(g_redraw_ante_dialog_background,
                                        g_redraw_ante_dialog_button_normal_bitmap,
                                        g_redraw_ante_dialog_button_depressed_bitmap,
                                        g_redraw_ante_dialog_button_disabled_bitmap);
      EndDialog(hwnd, s.context->mulligan_accepted);
    }
    else if (s.command_id == 2)
    {
      ShowWindow(GetDlgItem(hwnd, IDOK), SW_SHOW);
      SetFocus(GetDlgItem(hwnd, IDOK));
      SendMessageA(hwnd, 0x401, IDOK, 0);
    }
    return 1;

  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x0049bc58
void __cdecl redraw_ante_wait_for_opponent_mulligan_thread(void *hwnd)
{
#ifndef SHANDALAR
  struct
  {
    int network_choice;
    HWND thread_hwnd;
  } s;

  s.thread_hwnd = (HWND)hwnd;
  do
  {
  } while (WaitForSingleObject(global_mutex_GameInit, 0xffffffff) != 0);
  TENTATIVE_wait_for_network_result(1, 6);
  s.network_choice = g_network_result_value;
  if (s.thread_hwnd != (HWND)0)
  {
    SendMessageA(s.thread_hwnd, WM_COMMAND, 0x401, s.network_choice);
  }
  ReleaseMutex(global_mutex_GameInit);
  _endthread();
#else
  (void)hwnd;
#endif
}

// FUNCTION: MAGIC 0x0049bccf
void __cdecl redraw_ante_wait_for_opponent_done_thread(void *hwnd)
{
#ifndef SHANDALAR
  struct
  {
    int network_choice;
    HWND thread_hwnd;
  } s;

  s.thread_hwnd = (HWND)hwnd;
  do
  {
  } while (WaitForSingleObject(global_mutex_GameInit, 0xffffffff) != 0);
  TENTATIVE_wait_for_network_result(1, 6);
  s.network_choice = g_network_result_value;
  if (s.thread_hwnd != (HWND)0)
  {
    SendMessageA(s.thread_hwnd, WM_COMMAND, 0x402, s.network_choice);
  }
  ReleaseMutex(global_mutex_GameInit);
  _endthread();
#else
  (void)hwnd;
#endif
}

// FUNCTION: MAGIC 0x0049bd46
void load_redraw_ante_dialog_assets(HBITMAP *background,
                                    int *text_color,
                                    HBITMAP *normal_button_bitmap,
                                    HBITMAP *depressed_button_bitmap,
                                    HBITMAP *disabled_button_bitmap,
                                    COLORREF *normal_color,
                                    COLORREF *focus_color)
{
  struct
  {
    char filename[264];
  } s;

  sprintf(s.filename, "%s\\WINBK_StartDuel.pic", global_duelart_path);
  *background = load_pic(s.filename);
  *text_color = 0;
  sprintf(s.filename, "%s\\WINBK_StartDuelButtonNormal.pic", global_duelart_path);
  *normal_button_bitmap = load_pic(s.filename);
  sprintf(s.filename, "%s\\WINBK_StartDuelButtonDepressed.pic", global_duelart_path);
  *depressed_button_bitmap = load_pic(s.filename);
  sprintf(s.filename, "%s\\WINBK_StartDuelButtonDisabled.pic", global_duelart_path);
  *disabled_button_bitmap = load_pic(s.filename);
  *normal_color = 0x1000001;
  *focus_color = 0x10000bf;
}

// FUNCTION: MAGIC 0x0049be2a
void cleanup_redraw_ante_dialog_assets(HANDLE background,
                                       HANDLE normal_button_bitmap,
                                       HANDLE depressed_button_bitmap,
                                       HANDLE disabled_button_bitmap)
{
  if (background != 0)
  {
    delete_and_close_object(background);
  }
  if (normal_button_bitmap != 0)
  {
    delete_and_close_object(normal_button_bitmap);
  }
  if (depressed_button_bitmap != 0)
  {
    delete_and_close_object(depressed_button_bitmap);
  }
  if (disabled_button_bitmap != 0)
  {
    delete_and_close_object(disabled_button_bitmap);
  }
}

// FUNCTION: MAGIC 0x0049a5c5
void load_coin_flip_dialog_assets(HBITMAP *background,
                                  int *text_color,
                                  HBITMAP *normal_button_bitmap,
                                  HBITMAP *depressed_button_bitmap,
                                  COLORREF *normal_color,
                                  COLORREF *focus_color)
{
  struct
  {
    char filename[264];
  } s;

  sprintf(s.filename, "%s\\WINBK_StartDuel2.pic", global_duelart_path);
  *background = load_pic(s.filename);
  *text_color = 0;
  sprintf(s.filename, "%s\\WINBK_StartDuelButtonNormal.pic", global_duelart_path);
  *normal_button_bitmap = load_pic(s.filename);
  sprintf(s.filename, "%s\\WINBK_StartDuelButtonDepressed.pic", global_duelart_path);
  *depressed_button_bitmap = load_pic(s.filename);
  *normal_color = 0x1000001;
  *focus_color = 0x10000bf;
}

// FUNCTION: MAGIC 0x0049a67b
void cleanup_coin_flip_dialog_assets(HANDLE background, HANDLE normal_button_bitmap, HANDLE depressed_button_bitmap)
{
  if (background != 0)
  {
    delete_and_close_object(background);
  }
  if (normal_button_bitmap != 0)
  {
    delete_and_close_object(normal_button_bitmap);
  }
  if (depressed_button_bitmap != 0)
  {
    delete_and_close_object(depressed_button_bitmap);
  }
}

// FUNCTION: MAGIC 0x00495a69
void draw_duel_dialog_bitmap_button(DRAWITEMSTRUCT *draw_item,
                                    HBITMAP normal_bitmap,
                                    HBITMAP pressed_bitmap,
                                    HBITMAP disabled_bitmap,
                                    COLORREF text_color,
                                    int draw_focus)
{
  struct
  {
    HDC hdc;
    RECT focus_rect;
    HGDIOBJ font;
    RECT draw_rect;
    char text[200];
    SIZE text_size;
  } s;

  s.hdc = draw_item->hDC;
  CopyRect(&s.draw_rect, &draw_item->rcItem);
  GetWindowTextA(draw_item->hwndItem, s.text, 200);
  ApplyCardArtPaletteToDc(s.hdc);
  OffsetRect(&s.draw_rect, -draw_item->rcItem.left, -draw_item->rcItem.top);
  if ((draw_item->itemState & ODS_SELECTED) != 0)
  {
    DrawBitmapToRect(s.hdc, &s.draw_rect, pressed_bitmap);
    OffsetRect(&s.draw_rect, 2, 2);
  }
  else
  {
    if ((draw_item->itemState & ODS_DISABLED) != 0 || (draw_item->itemState & ODS_GRAYED) != 0)
    {
      DrawBitmapToRect(s.hdc, &s.draw_rect, disabled_bitmap);
    }
    else
    {
      DrawBitmapToRect(s.hdc, &s.draw_rect, normal_bitmap);
    }
  }
  SetBkMode(s.hdc, TRANSPARENT);
  SetTextColor(s.hdc, text_color);
  s.font = (HGDIOBJ)SendMessageA(draw_item->hwndItem, WM_GETFONT, 0, 0);
  SelectObject(s.hdc, s.font);
  DrawTextA(s.hdc, s.text, -1, &s.draw_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  if (draw_focus != 0 && (draw_item->itemState & ODS_FOCUS) != 0)
  {
    GetTextExtentPoint32A(s.hdc, s.text, strlen(s.text), &s.text_size);
    s.focus_rect.left = (s.draw_rect.right - s.draw_rect.left) / 2 - s.text_size.cx / 2 - 3;
    s.focus_rect.right = s.focus_rect.left + s.text_size.cx + 6;
    s.focus_rect.top = (s.draw_rect.bottom - s.draw_rect.top) / 2 - s.text_size.cy / 2 - 3;
    s.focus_rect.bottom = s.focus_rect.top + s.text_size.cy + 6;
    DrawFocusRect(s.hdc, &s.focus_rect);
  }
}

// FUNCTION: MAGIC 0x0049a558
void __cdecl coin_flip_wait_for_network_choice_thread(void *hwnd)
{
#ifndef SHANDALAR
  struct
  {
    int network_choice;
    HWND thread_hwnd;
  } s;

  s.thread_hwnd = (HWND)hwnd;
  do
  {
  } while (WaitForSingleObject(global_mutex_GameInit, 0xffffffff) != 0);
  TENTATIVE_wait_for_network_result(1, 5);
  s.network_choice = g_network_result_value;
  SendMessageA(s.thread_hwnd, WM_COMMAND, 0x401, s.network_choice);
  ReleaseMutex(global_mutex_GameInit);
  _endthread();
#else
  (void)hwnd;
#endif
}

// FUNCTION: MAGIC 0x00499984
BOOL CALLBACK dlgproc_duel_coin_flip(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    UINT timer_id;             // ebp - 0x1ac
    HDC hdc;                   // ebp - 0x1a8
    RECT erase_rect;           // ebp - 0x1a4
    COLORREF draw_color;       // ebp - 0x1a0
    DRAWITEMSTRUCT *draw_item; // ebp - 0x190
    HWND ctl_hwnd;             // ebp - 0x18c
    int ctl_id;                // ebp - 0x188
    HDC hdc_a;                 // ebp - 0x184
    HWND invalidate_b;         // ebp - 0x180
    HWND custom_hwnd;          // ebp - 0x17c
    int command_id;            // ebp - 0x178
    HDC dialog_dc;             // ebp - 0x174
    int button_height;         // ebp - 0x170
    int button_width;          // ebp - 0x16c
    int button_y;              // ebp - 0x168
    int button_x;              // ebp - 0x164
    HGDIOBJ button_font;       // ebp - 0x160
    RECT client_rect;
    SIZE text_size;
    RECT button_rect;
    int random_choice;
    char text[200];
    char player_name[100];
    DuelCoinFlipDialogContext *context;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    s.context = (DuelCoinFlipDialogContext *)lparam;
    SetWindowLongA(hwnd, DWL_USER, (LONG)s.context);
    load_coin_flip_dialog_assets(&g_coin_flip_dialog_background,
                                 &g_coin_flip_dialog_text_color,
                                 &g_coin_flip_dialog_button_normal_bitmap,
                                 &g_coin_flip_dialog_button_depressed_bitmap,
                                 &g_coin_flip_dialog_button_normal_color,
                                 &g_coin_flip_dialog_button_focus_color);
    copy_opponent_name_prefix(s.player_name);
    if ((g_duel_network_flags & 2) != 0)
    {
      s.context->play_draw_choice = -1;
    }
    load_text(global_ui_strings_filename, "DIALOG_PLAYORDRAW");
    if (s.context->coin_winner == 1)
    {
      if ((g_duel_network_flags & 2) != 0)
      {
        sprintf(s.text, text_lines[0], s.player_name);
        SetDlgItemTextA(hwnd, 0x4c4, s.text);
        ShowWindow(GetDlgItem(hwnd, 0x4c5), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, 0x4c6), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, 0x4c7), SW_HIDE);
        SetFocus(hwnd);
        _beginthread(coin_flip_wait_for_network_choice_thread, 0, hwnd);
      }
      else
      {
        sprintf(s.text, text_lines[0], s.player_name);
        SetDlgItemTextA(hwnd, 0x4c4, s.text);
        s.random_choice = internal_rand(2);
        s.context->play_draw_choice = s.random_choice;
        if (s.context->was_random_starting_player)
        {
          sprintf(s.text, text_lines[0], s.player_name);
          SetDlgItemTextA(hwnd, 0x4c4, s.text);
          if (s.random_choice)
          {
            SetDlgItemTextA(hwnd, 0x4c7, text_lines[1]);
          }
          else
          {
            SetDlgItemTextA(hwnd, 0x4c7, text_lines[2]);
          }
          ShowWindow(GetDlgItem(hwnd, 0x4c7), SW_HIDE);
          SetTimer(hwnd, 1, 1000, (TIMERPROC)0);
        }
        else
        {
          ShowWindow(GetDlgItem(hwnd, 0x4c4), SW_HIDE);
          if (s.random_choice)
          {
            sprintf(s.text, text_lines[7], s.player_name);
          }
          else
          {
            sprintf(s.text, text_lines[8], s.player_name);
          }
          SetDlgItemTextA(hwnd, 0x4c7, s.text);
        }
        ShowWindow(GetDlgItem(hwnd, 0x4c5), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, 0x4c6), SW_HIDE);
        SetFocus(hwnd);
        SetTimer(hwnd, 1, 1000, (TIMERPROC)0);
      }
    }
    else
    {
      if (s.context->was_random_starting_player)
      {
        SetDlgItemTextA(hwnd, 0x4c4, text_lines[3]);
      }
      else
      {
        ShowWindow(GetDlgItem(hwnd, 0x4c4), SW_HIDE);
      }
      SetDlgItemTextA(hwnd, 0x4c7, text_lines[4]);
      SetDlgItemTextA(hwnd, 0x4c5, text_lines[5]);
      SetDlgItemTextA(hwnd, 0x4c6, text_lines[6]);
      SetFocus(GetDlgItem(hwnd, 0x4c5));
      SendMessageA(hwnd, 0x401, 0x4c5, 0);
    }

    s.dialog_dc = GetDC(hwnd);
    ApplyCardArtPaletteToDc(s.dialog_dc);
    s.button_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x4c5, WM_GETFONT, 0, 0);
    SelectObject(s.dialog_dc, s.button_font);
    GetDlgItemTextA(hwnd, 0x4c5, s.text, 200);
    GetTextExtentPoint32A(s.dialog_dc, s.text, strlen(s.text), &s.text_size);
    s.button_width = s.text_size.cx;
    s.button_height = (s.text_size.cy * 5) / 2;
    GetDlgItemTextA(hwnd, 0x4c6, s.text, 200);
    GetTextExtentPoint32A(s.dialog_dc, s.text, strlen(s.text), &s.text_size);
    s.button_width = MAX(s.button_width, s.text_size.cx);
    s.button_width += s.text_size.cy * 2;
    GetWindowRect(GetDlgItem(hwnd, 0x4c5), &s.button_rect);
    MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.button_rect, 2);
    s.button_y = s.button_rect.top;
    GetClientRect(hwnd, &s.client_rect);
    s.button_x = s.client_rect.left + ((s.client_rect.right - s.client_rect.left) - s.button_width) / 2;
    SetWindowPos(GetDlgItem(hwnd, 0x4c5), (HWND)0, s.button_x, s.button_y, s.button_width, s.button_height, SWP_NOZORDER);
    s.button_y = s.button_y + s.button_height + s.button_height / 5;
    SetWindowPos(GetDlgItem(hwnd, 0x4c6), (HWND)0, s.button_x, s.button_y, s.button_width, s.button_height, SWP_NOZORDER);
    ReleaseDC(hwnd, s.dialog_dc);
    change_buttonclass_wndproc(hwnd);
    return 0;

  case WM_DESTROY:
    cleanup_coin_flip_dialog_assets(g_coin_flip_dialog_background,
                                    g_coin_flip_dialog_button_normal_bitmap,
                                    g_coin_flip_dialog_button_depressed_bitmap);
    return 0;

  case WM_COMMAND:
    switch (s.command_id = (unsigned int)wparam & 0xffff)
    {
    case 0x4c5:
    case 0x4c6:
      s.context = (DuelCoinFlipDialogContext *)GetWindowLongA(hwnd, DWL_USER);
      if (s.command_id == 0x4c5)
      {
        s.context->play_draw_choice = 1;
      }
      else
      {
        s.context->play_draw_choice = 0;
      }
      EndDialog(hwnd, 1);
      break;
    case 0x401:
      s.context = (DuelCoinFlipDialogContext *)GetWindowLongA(hwnd, DWL_USER);
      s.context->play_draw_choice = lparam;
      load_text(global_ui_strings_filename, "DIALOG_PLAYORDRAW");
      if (s.context->play_draw_choice != 0)
      {
        SetDlgItemTextA(hwnd, 0x4c7, text_lines[1]);
      }
      else
      {
        SetDlgItemTextA(hwnd, 0x4c7, text_lines[2]);
      }
      ShowWindow(GetDlgItem(hwnd, 0x4c7), SW_SHOW);
      SetTimer(hwnd, 2, 3000, (TIMERPROC)0);
      break;
    }
    return 1;

  case 0x4c8:
    s.custom_hwnd = (HWND)wparam;
    s.invalidate_b = (HWND)lparam;
    if (s.custom_hwnd != (HWND)0)
    {
      SendMessageA(hwnd, 0x401, GetDlgCtrlID(s.custom_hwnd), 0);
    }
    if (s.custom_hwnd != (HWND)0)
    {
      InvalidateRect(s.custom_hwnd, (RECT *)0, 1);
    }
    if (s.invalidate_b != (HWND)0)
    {
      InvalidateRect(s.invalidate_b, (RECT *)0, 1);
    }
    return 0;

  case WM_CTLCOLORSTATIC:
    s.hdc_a = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc_a);
    s.ctl_hwnd = (HWND)lparam;
    s.ctl_id = GetDlgCtrlID(s.ctl_hwnd);
    SetBkMode(s.hdc_a, TRANSPARENT);
    SetTextColor(s.hdc_a, g_coin_flip_dialog_text_color);
    return (BOOL)GetStockObject(HOLLOW_BRUSH);

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
    {
      s.draw_color = g_coin_flip_dialog_button_focus_color;
    }
    else
    {
      s.draw_color = g_coin_flip_dialog_button_normal_color;
    }
    draw_duel_dialog_bitmap_button(s.draw_item,
                                   g_coin_flip_dialog_button_normal_bitmap,
                                   g_coin_flip_dialog_button_depressed_bitmap,
                                   g_coin_flip_dialog_button_normal_bitmap,
                                   s.draw_color,
                                   0);
    return 1;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_ERASEBKGND:
    s.hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc);
    GetClientRect(hwnd, &s.erase_rect);
    if (g_coin_flip_dialog_background != (HBITMAP)0)
    {
      DrawBitmapToRect(s.hdc, &s.erase_rect, g_coin_flip_dialog_background);
    }
    else
    {
      FillRect(s.hdc, &s.erase_rect, GetStockObject(2));
    }
    return 1;

  case WM_TIMER:
    s.timer_id = wparam;
    if (s.timer_id == 1)
    {
      KillTimer(hwnd, 1);
      ShowWindow(GetDlgItem(hwnd, 0x4c7), SW_SHOW);
      SetTimer(hwnd, 2, 3000, (TIMERPROC)0);
    }
    else if (s.timer_id == 2)
    {
      KillTimer(hwnd, 2);
      EndDialog(hwnd, 1);
    }
    return 0;

  case WM_CLOSE:
    if (IsWindowVisible(GetDlgItem(hwnd, 0x4c5)) == 0)
    {
      KillTimer(hwnd, 2);
      EndDialog(hwnd, 1);
      return 1;
    }
    else
    {
      return 1;
    }

  case WM_CHAR:
  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
    if (g_duel_network_flags & 2)
    {
      s.context = (DuelCoinFlipDialogContext *)GetWindowLongA(hwnd, DWL_USER);
      if (s.context->play_draw_choice == -1)
      {
        return 0;
      }
    }
    if (IsWindowVisible(GetDlgItem(hwnd, 0x4c5)) == 0)
    {
      KillTimer(hwnd, 2);
      EndDialog(hwnd, 1);
      return 1;
    }
    else
    {
      return 0;
    }
  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x004996d0
// FUNCTION: SHANDALAR 0x00535090
int run_duel_coin_flip_dialogs(unsigned int *starting_player,
                               unsigned int *mulligan_accepted,
                               unsigned int starting_player_value,
                               int random_starting_player,
                               int opponent_ante_card,
                               int player_ante_card,
                               int redraw_result,
                               int ante_result,
                               int ante_info,
                               int *out_flag)
{
  struct
  {
    unsigned int mulligan_result;
    DuelRedrawAnteDialogContext redraw_ante_context;
    DuelCoinFlipDialogContext coin_flip_context;
  } s;

  if (starting_player == (unsigned int *)0 || mulligan_accepted == (unsigned int *)0)
  {
    return 0;
  }

  load_text(global_ui_strings_filename, "DIALOG_STARTCOINFLIP");
  if (random_starting_player != 0)
  {
    if ((g_duel_network_flags & 2) != 0)
    {
      s.coin_flip_context.coin_flip_result = coin_flip(starting_player_value, text_lines[0], 0);
      if (g_manalink_is_host != 0)
      {
        s.coin_flip_context.coin_winner = 1 - s.coin_flip_context.coin_flip_result;
      }
      else
      {
        s.coin_flip_context.coin_winner = s.coin_flip_context.coin_flip_result;
      }
    }
    else
    {
      if (coin_flip(starting_player_value, text_lines[0], 0) == 0)
      {
        s.coin_flip_context.coin_winner = 1;
      }
      else
      {
        s.coin_flip_context.coin_winner = 0;
      }
    }
    s.coin_flip_context.was_random_starting_player = 1;
  }
  else
  {
    s.coin_flip_context.coin_winner = starting_player_value;
    s.coin_flip_context.was_random_starting_player = 0;
  }

  if (starting_player_was_random != 0)
  {
    s.coin_flip_context.coin_flip_result =
        DialogBoxParamA(g_app_instance, (LPCSTR)0xf4, g_duel_window_hwnd, dlgproc_duel_coin_flip, (LPARAM)&s.coin_flip_context);
    if ((g_duel_network_flags & 2) != 0 && s.coin_flip_context.coin_winner == 0)
    {
      g_network_result_packet_type = 5;
      g_network_result_value = s.coin_flip_context.play_draw_choice;
      TENTATIVE_send_network_result(0, 5);
    }
    set_player_directive_value(1, 0);
  }
  else
  {
    s.coin_flip_context.play_draw_choice = 1;
  }

  shuffle_duel_library(0, 0);
  shuffle_duel_library(1, 1);
  TENTATIVE_reassess_all_cards(0, 0x30);
  if ((s.coin_flip_context.coin_winner == 1 && s.coin_flip_context.play_draw_choice != 0) ||
      (s.coin_flip_context.coin_winner == 0 && s.coin_flip_context.play_draw_choice == 0))
  {
    s.redraw_ante_context.starting_player = 1;
  }
  else
  {
    s.redraw_ante_context.starting_player = 0;
  }

  s.redraw_ante_context.opponent_ante_card = CardIDFromType(opponent_ante_card);
  s.redraw_ante_context.player_ante_card = CardIDFromType(player_ante_card);
  s.redraw_ante_context.redraw_result = redraw_result;
  s.redraw_ante_context.ante_result = ante_result;
  s.redraw_ante_context.ante_info = ante_info;
  if (ante_result != 0 && (g_duel_network_flags & 2) == 0)
  {
    g_duel_surface_reset_state = 1;
    resize_duel_hand_window(g_duel_life_window_hwnd);
  }

  s.coin_flip_context.coin_flip_result =
      DialogBoxParamA(g_app_instance, (LPCSTR)0xe3, g_duel_window_hwnd, dlgproc_duel_redraw_ante, (LPARAM)&s.redraw_ante_context);
  s.mulligan_result = (s.coin_flip_context.coin_flip_result != 0);
  *starting_player = s.redraw_ante_context.starting_player;
  *mulligan_accepted = s.mulligan_result;
  *out_flag = s.redraw_ante_context.opponent_mulligan_accepted;
  g_duel_surface_reset_state = 0;
  resize_duel_hand_window(g_duel_life_window_hwnd);
  if (s.mulligan_result != 0)
  {
    PostMessageA(g_duel_full_card_window_hwnd, 0x40c, 0, 0);
  }
  UpdateWindow(g_duel_window_hwnd);
  return 1;
}
