#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

#include "cardartlib/src/palette.h"
#include "drawcardlib/Drawcardlib.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/network.h"
#include "magic/src/shared_startup.h"

#define FIREBALL_CARD_DISPLAY 0x4ce
#define FIREBALL_MANA_SPIN 0x4cf
#define FIREBALL_MANA_NOTIFY 0x4d0
#define FIREBALL_MAX_MANA_LABEL 0x4d1
#define FIREBALL_TARGETS_NOTIFY 0x4d2
#define FIREBALL_TARGETS_SPIN 0x4d3
#define FIREBALL_MAX_TARGETS_LABEL 0x4d4
#define FIREBALL_DAMAGE_PER_TARGET 0x4d5
#define FIREBALL_MANA_TO_PAY 0x4d6
#define FIREBALL_EXTRA_TARGET_COST 0x4d7
#define WM_DUEL_BUTTON_FOCUS 0x4c8

typedef struct
{
  int maximum_mana;
  int maximum_targets;
  int mana_to_pay;
  int number_of_targets;
  int damage_per_target;
  int card_id;
  int dialog_result;
} FireballOptionsDialogContext;

extern HINSTANCE g_app_instance;
extern card_ptr_t global_raw_cards_storage[2000];

HBITMAP load_pic(char *filename);
void delete_and_close_object(HANDLE obj);
void change_buttonclass_wndproc(HWND hwnd);
void draw_owner_draw_button_centered(DRAWITEMSTRUCT *draw_item, HBRUSH brush,
                                     HPEN pen1, HPEN pen2, COLORREF color, int draw_focus);
int handle_button_palette_message(int hwnd, unsigned int msg, int wparam, int lparam);

BOOL WINAPI dlgproc_fireball_options(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void setup_fireball_dialog_resources(HBITMAP *background, COLORREF *label_text_color,
                                     HBRUSH *button_brush, HPEN *button_pen1, HPEN *button_pen2,
                                     COLORREF *button_unfocus_text_color,
                                     COLORREF *button_focus_text_color);
void cleanup_fireball_dialog_resources(HBITMAP background, HBRUSH button_brush,
                                       HPEN button_pen1, HPEN button_pen2);
int calculate_fireball_damage_per_target(int mana_to_pay, int number_of_targets);

// GLOBAL: MAGIC 0x00638b2c
// GLOBAL: SHANDALAR 0x00650274
HBRUSH g_fireball_dialog_button_brush;

// GLOBAL: MAGIC 0x00638b54
// GLOBAL: SHANDALAR 0x0065029c
int g_fireball_dialog_card_id;

// GLOBAL: MAGIC 0x00638ba8
// GLOBAL: SHANDALAR 0x006502f0
COLORREF g_fireball_dialog_button_unfocus_text_color;

// GLOBAL: MAGIC 0x00638bd0
// GLOBAL: SHANDALAR 0x00650318
HPEN g_fireball_dialog_button_pen1;

// GLOBAL: MAGIC 0x00638c28
// GLOBAL: SHANDALAR 0x00650370
HPEN g_fireball_dialog_button_pen2;

// GLOBAL: MAGIC 0x00638c34
// GLOBAL: SHANDALAR 0x0065037c
FireballOptionsDialogContext *g_fireball_dialog_context;

// GLOBAL: MAGIC 0x00638c48
// GLOBAL: SHANDALAR 0x00650390
HBITMAP g_fireball_dialog_background;

// GLOBAL: MAGIC 0x00638c5c
// GLOBAL: SHANDALAR 0x006503a4
COLORREF g_fireball_dialog_label_text_color;

// GLOBAL: MAGIC 0x00638c94
// GLOBAL: SHANDALAR 0x006503dc
COLORREF g_fireball_dialog_button_focus_text_color;

// FUNCTION: MAGIC 0x004a44fa
// FUNCTION: SHANDALAR 0x0053fe87
int choose_fireball_options(int player, int internal_card_id, int maximum_mana, int maximum_targets,
                            int *mana_to_pay, int *number_of_targets, int *damage_per_target)
{
  struct
  {
    int maximum_mana;
    int maximum_targets;
    int mana_to_pay;
    int number_of_targets;
    int damage_per_target;
    int card_id;
    int dialog_result;
  } s;

  if (player == g_other_player && (g_duel_network_flags & 2) == 0)
  {
    return 0;
  }
  if (player == -1 || internal_card_id == -1 || mana_to_pay == NULL ||
      number_of_targets == NULL || damage_per_target == NULL)
  {
    return 0;
  }

  s.maximum_mana = maximum_mana;
  s.maximum_targets = maximum_targets;
  s.mana_to_pay = *mana_to_pay;
  s.number_of_targets = *number_of_targets;
  s.card_id = CardIDFromType(internal_card_id);

  if ((g_duel_network_flags & 2) != 0 && player == g_other_player)
  {
    TENTATIVE_wait_for_network_result(player, 0x15);
    s.dialog_result = g_fireball_options_network_packet.dialog_result;
    s.mana_to_pay = g_fireball_options_network_packet.mana_to_pay;
    s.number_of_targets = g_fireball_options_network_packet.number_of_targets;
    s.damage_per_target = g_fireball_options_network_packet.damage_per_target;
  }
  else
  {
    s.dialog_result = DialogBoxParamA(g_app_instance, (LPCSTR)0xef, g_duel_window_hwnd,
                                      dlgproc_fireball_options, (LPARAM)&s);
    if ((g_duel_network_flags & 2) != 0)
    {
      g_fireball_options_network_packet.dialog_result = s.dialog_result;
      g_fireball_options_network_packet.mana_to_pay = s.mana_to_pay;
      g_fireball_options_network_packet.number_of_targets = s.number_of_targets;
      g_fireball_options_network_packet.damage_per_target = s.damage_per_target;
      g_fireball_options_network_packet.packet_type = 0x15;
      TENTATIVE_send_network_result(player, 0x15);
    }
  }

  if (s.dialog_result == -1)
  {
    return 0;
  }
  else
  {
    if (s.dialog_result == -2)
    {
      return 0;
    }
    else
    {
      *mana_to_pay = s.mana_to_pay;
      *number_of_targets = s.number_of_targets;
      *damage_per_target = s.damage_per_target;
      return 1;
    }
  }
}

// FUNCTION: MAGIC 0x004a4691
// FUNCTION: SHANDALAR 0x0054001c
BOOL WINAPI dlgproc_fireball_options(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    char pad_168[8];
    PAINTSTRUCT paint;
    RECT card_rect;
    unsigned int mouse_x;
    unsigned int mouse_y;
    RECT preview_rect;
    HDC erase_dc;
    RECT erase_rect;
    COLORREF button_text_color;
    DRAWITEMSTRUCT *draw_item;
    HWND ctl_hwnd;
    int ctl_id;
    HDC ctl_hdc;
    int pad_d0;
    HWND focus_lost;
    HWND focus_gained;
    HDC notify_dc;
    int valid;
    unsigned int command_id;
    int target_count;
    unsigned int spin_result;
    unsigned int notify_code;
    unsigned int spin_error;
    int mana_value;
    HWND button_hwnd;
    char formatted_text[100];
    unsigned int damage;
    char format_text[52];
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    g_fireball_dialog_context = (FireballOptionsDialogContext *)lparam;
    g_fireball_dialog_card_id = g_fireball_dialog_context->card_id;
    ShowWindow(GetDlgItem(hwnd, FIREBALL_CARD_DISPLAY), SW_HIDE);
    setup_fireball_dialog_resources(&g_fireball_dialog_background,
                                    &g_fireball_dialog_label_text_color,
                                    &g_fireball_dialog_button_brush,
                                    &g_fireball_dialog_button_pen1,
                                    &g_fireball_dialog_button_pen2,
                                    &g_fireball_dialog_button_unfocus_text_color,
                                    &g_fireball_dialog_button_focus_text_color);
    load_text(global_ui_strings_filename, "DIALOG_FIREBALL");
    SetDlgItemTextA(hwnd, 0x4d8, g_text_lines[0]);
    SetDlgItemTextA(hwnd, FIREBALL_MAX_MANA_LABEL, g_text_lines[1]);
    SetDlgItemTextA(hwnd, 0x4d9, g_text_lines[2]);
    SetDlgItemTextA(hwnd, 0x4da, g_text_lines[3]);
    SetDlgItemTextA(hwnd, 0x4db, g_text_lines[4]);
    SetDlgItemTextA(hwnd, FIREBALL_MAX_TARGETS_LABEL, g_text_lines[5]);
    SetDlgItemTextA(hwnd, 0x4dc, g_text_lines[6]);
    SetDlgItemTextA(hwnd, IDOK, gs_done_008b40e0);
    SetDlgItemTextA(hwnd, IDCANCEL, gs_cancel_008a8c20);
    GetDlgItemTextA(hwnd, FIREBALL_MAX_MANA_LABEL, s.format_text, 0x32);
    sprintf(s.formatted_text, s.format_text, g_fireball_dialog_context->maximum_mana);
    SetDlgItemTextA(hwnd, FIREBALL_MAX_MANA_LABEL, s.formatted_text);
    GetDlgItemTextA(hwnd, FIREBALL_MAX_TARGETS_LABEL, s.format_text, 0x32);
    sprintf(s.formatted_text, s.format_text, g_fireball_dialog_context->maximum_targets);
    SetDlgItemTextA(hwnd, FIREBALL_MAX_TARGETS_LABEL, s.formatted_text);
    SendDlgItemMessageA(hwnd, FIREBALL_MANA_SPIN, UDM_SETRANGE, 0,
                        (unsigned int)(unsigned short)g_fireball_dialog_context->maximum_mana);
    SendDlgItemMessageA(hwnd, FIREBALL_TARGETS_SPIN, UDM_SETRANGE, 0,
                        (unsigned int)(unsigned short)g_fireball_dialog_context->maximum_targets);
    SendDlgItemMessageA(hwnd, FIREBALL_MANA_SPIN, UDM_SETPOS, 0,
                        (unsigned int)(unsigned short)g_fireball_dialog_context->mana_to_pay);
    SendDlgItemMessageA(hwnd, FIREBALL_TARGETS_SPIN, UDM_SETPOS, 0,
                        (unsigned int)(unsigned short)g_fireball_dialog_context->number_of_targets);
    s.damage = calculate_fireball_damage_per_target(g_fireball_dialog_context->mana_to_pay,
                                                    g_fireball_dialog_context->number_of_targets);
    SetDlgItemInt(hwnd, FIREBALL_DAMAGE_PER_TARGET, s.damage, 1);
    SetFocus(GetDlgItem(hwnd, IDOK));
    SendMessageA(hwnd, 0x401, IDOK, 0);
    s.button_hwnd = GetDlgItem(hwnd, IDOK);
    SetWindowLongA(s.button_hwnd, GWL_STYLE,
                   GetWindowLongA(s.button_hwnd, GWL_STYLE) | 0x800000);
    change_buttonclass_wndproc(hwnd);
    return 0;

  case WM_COMMAND:
    s.command_id = LOWORD(wparam);
    s.notify_code = HIWORD(wparam);
    if (s.command_id == FIREBALL_MANA_NOTIFY)
    {
      if (s.notify_code == 0x200)
      {
        s.spin_result = SendDlgItemMessageA(hwnd, FIREBALL_MANA_SPIN, UDM_GETPOS, 0, 0);
        s.spin_error = HIWORD(s.spin_result) != 0;
        if (s.spin_error == 0)
        {
          s.mana_value = SendDlgItemMessageA(hwnd, FIREBALL_MANA_SPIN, UDM_GETPOS, 0, 0);
          s.target_count = SendDlgItemMessageA(hwnd, FIREBALL_TARGETS_SPIN, UDM_GETPOS, 0, 0);
          SetDlgItemInt(hwnd, FIREBALL_MANA_TO_PAY,
                        s.mana_value - (s.target_count - 1), 1);
          SetDlgItemInt(hwnd, FIREBALL_EXTRA_TARGET_COST, s.target_count - 1, 1);
          SetDlgItemInt(hwnd, FIREBALL_DAMAGE_PER_TARGET,
                        calculate_fireball_damage_per_target(s.mana_value, s.target_count), 1);
        }
        else
        {
          SendDlgItemMessageA(hwnd, FIREBALL_MANA_SPIN, UDM_SETPOS, 0,
                              (unsigned int)(unsigned short)g_fireball_dialog_context->mana_to_pay);
        }
      }
    }
    else if (s.command_id == IDOK)
    {
      s.valid = 1;
      s.spin_result = SendDlgItemMessageA(hwnd, FIREBALL_MANA_SPIN, UDM_GETPOS, 0, 0);
      s.spin_error = HIWORD(s.spin_result) != 0;
      if (s.spin_error != 0)
      {
        SendDlgItemMessageA(hwnd, FIREBALL_MANA_SPIN, UDM_SETPOS, 0,
                            (unsigned int)(unsigned short)g_fireball_dialog_context->mana_to_pay);
        s.valid = 0;
      }
      s.spin_result = SendDlgItemMessageA(hwnd, FIREBALL_TARGETS_SPIN, UDM_GETPOS, 0, 0);
      s.spin_error = HIWORD(s.spin_result) != 0;
      if (s.spin_error != 0)
      {
        SendDlgItemMessageA(hwnd, FIREBALL_TARGETS_SPIN, UDM_SETPOS, 0,
                            (unsigned int)(unsigned short)g_fireball_dialog_context->number_of_targets);
        s.valid = 0;
      }
      if (s.valid != 0)
      {
        g_fireball_dialog_context->mana_to_pay =
            SendDlgItemMessageA(hwnd, FIREBALL_MANA_SPIN, UDM_GETPOS, 0, 0);
        s.mana_value = g_fireball_dialog_context->mana_to_pay;
        g_fireball_dialog_context->number_of_targets =
            SendDlgItemMessageA(hwnd, FIREBALL_TARGETS_SPIN, UDM_GETPOS, 0, 0);
        s.target_count = g_fireball_dialog_context->number_of_targets;
        g_fireball_dialog_context->damage_per_target =
            calculate_fireball_damage_per_target(s.mana_value, s.target_count);
        cleanup_fireball_dialog_resources(g_fireball_dialog_background,
                                          g_fireball_dialog_button_brush,
                                          g_fireball_dialog_button_pen1,
                                          g_fireball_dialog_button_pen2);
        EndDialog(hwnd, 1);
      }
    }
    else if (s.command_id == IDCANCEL)
    {
      cleanup_fireball_dialog_resources(g_fireball_dialog_background,
                                        g_fireball_dialog_button_brush,
                                        g_fireball_dialog_button_pen1,
                                        g_fireball_dialog_button_pen2);
      EndDialog(hwnd, -2);
    }
    else if ((s.command_id == FIREBALL_TARGETS_NOTIFY) && (s.notify_code == 0x200))
    {
      s.spin_result = SendDlgItemMessageA(hwnd, FIREBALL_TARGETS_SPIN, UDM_GETPOS, 0, 0);
      s.spin_error = HIWORD(s.spin_result) != 0;
      if (s.spin_error == 0)
      {
        s.mana_value = SendDlgItemMessageA(hwnd, FIREBALL_MANA_SPIN, UDM_GETPOS, 0, 0);
        s.target_count = SendDlgItemMessageA(hwnd, FIREBALL_TARGETS_SPIN, UDM_GETPOS, 0, 0);
        SetDlgItemInt(hwnd, FIREBALL_MANA_TO_PAY,
                      s.mana_value - (s.target_count - 1), 1);
        SetDlgItemInt(hwnd, FIREBALL_EXTRA_TARGET_COST, s.target_count - 1, 1);
        SetDlgItemInt(hwnd, FIREBALL_DAMAGE_PER_TARGET,
                      calculate_fireball_damage_per_target(s.mana_value, s.target_count), 1);
      }
      else
      {
        SendDlgItemMessageA(hwnd, FIREBALL_TARGETS_SPIN, UDM_SETPOS, 0,
                            (unsigned int)(unsigned short)g_fireball_dialog_context->number_of_targets);
      }
    }
    return 1;

  case WM_NOTIFY:
    s.notify_dc = (HDC)wparam;
    PostMessageA(hwnd, WM_COMMAND, 0x020004d0,
                 (LPARAM)GetDlgItem(hwnd, FIREBALL_MANA_NOTIFY));
    return 0;

  case WM_DRAWITEM:
    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
      s.button_text_color = g_fireball_dialog_button_focus_text_color;
    else
      s.button_text_color = g_fireball_dialog_button_unfocus_text_color;
    draw_owner_draw_button_centered(s.draw_item,
                                    g_fireball_dialog_button_brush,
                                    g_fireball_dialog_button_pen1,
                                    g_fireball_dialog_button_pen2,
                                    s.button_text_color,
                                    0);
    return 1;

  case WM_CTLCOLORSTATIC:
    s.ctl_hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.ctl_hdc);
    s.ctl_hwnd = (HWND)lparam;
    s.ctl_id = GetDlgCtrlID(s.ctl_hwnd);
    if (GetFocus() == s.ctl_hwnd)
      SetTextColor(s.ctl_hdc, g_fireball_dialog_button_focus_text_color);
    else
      SetTextColor(s.ctl_hdc, g_fireball_dialog_label_text_color);
    if ((s.ctl_id != FIREBALL_DAMAGE_PER_TARGET) &&
        (s.ctl_id != FIREBALL_MANA_TO_PAY) &&
        (s.ctl_id != FIREBALL_EXTRA_TARGET_COST))
    {
      SetBkMode(s.ctl_hdc, TRANSPARENT);
      return (BOOL)GetStockObject(NULL_BRUSH);
    }
    SetBkMode(s.ctl_hdc, TRANSPARENT);
    return (BOOL)g_fireball_dialog_button_brush;

  case WM_PAINT:
    s.erase_dc = BeginPaint(hwnd, &s.paint);
    if (s.erase_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.erase_dc);
      if (g_fireball_dialog_card_id != -1)
      {
        GetWindowRect(GetDlgItem(hwnd, FIREBALL_CARD_DISPLAY), &s.card_rect);
        MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.card_rect, 2);
        DrawFullCard(s.erase_dc, &s.card_rect,
                     global_raw_cards_storage + g_fireball_dialog_card_id,
                     0, 0x11, 0, gs_illus_00789130);
      }
    }
    EndPaint(hwnd, &s.paint);
    return 0;

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    if (g_fireball_dialog_background == (HBITMAP)0)
      FillRect(s.erase_dc, &s.erase_rect, (HBRUSH)GetStockObject(3));
    else
      DrawBitmapToRect(s.erase_dc, &s.erase_rect, g_fireball_dialog_background);
    return 1;

  case WM_MOUSEMOVE:
  case WM_RBUTTONDOWN:
    s.mouse_x = LOWORD(lparam);
    s.mouse_y = HIWORD(lparam);
    if (((msg == WM_MOUSEMOVE) && (g_duel_interface_options.layout != 2)) ||
        ((msg == WM_RBUTTONDOWN) && (g_duel_interface_options.layout == 2)))
    {
      GetWindowRect(GetDlgItem(hwnd, FIREBALL_CARD_DISPLAY), &s.preview_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.preview_rect, 2);
      s.erase_rect.left = s.mouse_x;
      s.erase_rect.top = s.mouse_y;
      if ((g_fireball_dialog_card_id != -1) &&
          PtInRect(&s.preview_rect, *(POINT *)&s.erase_rect))
      {
        SendMessageA(g_duel_card_preview_window_hwnd, 0x401,
                     g_fireball_dialog_card_id, 0);
      }
    }
    return 0;

  case WM_LBUTTONDOWN:
    SendMessageA(hwnd, WM_SYSCOMMAND, 0xf012, 0);
    return 0;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  case WM_DUEL_BUTTON_FOCUS:
    s.focus_gained = (HWND)wparam;
    s.focus_lost = (HWND)lparam;
    if (GetDlgItem(hwnd, IDCANCEL) == s.focus_gained)
      SendMessageA(hwnd, 0x401, IDCANCEL, 0);
    else
      SendMessageA(hwnd, 0x401, IDOK, 0);
    if (s.focus_gained != (HWND)0)
      InvalidateRect(s.focus_gained, (RECT *)0, 1);
    if (s.focus_lost != (HWND)0)
      InvalidateRect(s.focus_lost, (RECT *)0, 1);
    return 0;

  default:
    return 0;
  }
}

// FUNCTION: MAGIC 0x004a543d
// FUNCTION: SHANDALAR 0x00540dae
void setup_fireball_dialog_resources(HBITMAP *background, COLORREF *label_text_color,
                                     HBRUSH *button_brush, HPEN *button_pen1, HPEN *button_pen2,
                                     COLORREF *button_unfocus_text_color,
                                     COLORREF *button_focus_text_color)
{
  struct
  {
    char filename[264];
  } s;

  sprintf(s.filename, "%s\\WINBK_Fireball.pic", global_duelart_path);
  *background = load_pic(s.filename);
  *label_text_color = 0x10000b6;
  *button_brush = CreateSolidBrush(0x10000e5);
  *button_pen1 = CreatePen(PS_SOLID, 0, 0x1000025);
  *button_pen2 = CreatePen(PS_SOLID, 0, 0x1000002);
  *button_unfocus_text_color = 0x1000001;
  *button_focus_text_color = 0x10000bf;
  if (*button_brush == (HBRUSH)0)
    *button_brush = (HBRUSH)GetStockObject(2);
  if (*button_pen1 == (HPEN)0)
    *button_pen1 = (HPEN)GetStockObject(6);
  if (*button_pen2 == (HPEN)0)
    *button_pen2 = (HPEN)GetStockObject(7);
}

// FUNCTION: MAGIC 0x004a551a
// FUNCTION: SHANDALAR 0x00540e8b
void cleanup_fireball_dialog_resources(HBITMAP background, HBRUSH button_brush,
                                       HPEN button_pen1, HPEN button_pen2)
{
  if (background != (HBITMAP)0)
    delete_and_close_object(background);
  if (button_brush != (HBRUSH)0)
    DeleteObject(button_brush);
  if (button_pen1 != (HPEN)0)
    DeleteObject(button_pen1);
  if (button_pen2 != (HPEN)0)
    DeleteObject(button_pen2);
}

// FUNCTION: MAGIC 0x004a5577
// FUNCTION: SHANDALAR 0x00540ee8
int calculate_fireball_damage_per_target(int mana_to_pay, int number_of_targets)
{
  int result;

  if (mana_to_pay == 0 || number_of_targets == 0)
  {
    result = 0;
  }
  else
  {
    result = (mana_to_pay - (number_of_targets - 1)) / number_of_targets;
    if (result <= 0)
      result = 0;
  }
  return result;
}
