#include "deckdll/src/shared_resources.h"
#include "magic_shell_dialogs.h"
#include "magic_shell_screen_name.h"
#include "game_support.h"
#include "global_other.h"
#include "global_strings.h"
#include "shared_startup.h"
#include "cardartlib/src/palette.h"
#include "manalinkinterface/manalinkinterface.h"
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commdlg.h>

extern HWND global_main_hwnd;
extern char global_base_directory[];

// FUNCTION: MAGIC 0x0048c879
BOOL CALLBACK shell_screen_name_dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    char portrait_path[264];
    char portrait_name[100];
    HBITMAP portrait_bitmap;
    BITMAP portrait_info;
    RECT portrait_rect;
    RECT portrait_reserved;
    WPARAM portrait_selection;
    HDC paint_dc;
    PAINTSTRUCT paint;
    int clear_index;
    LOGFONTA resize_font;
    HFONT replacement_font;
    int control_index;
    int base_font_height;
    HWND controls[50];
    LOGFONTA * font_description;
    int control_count;
    RECT shell_rect;
    HFONT old_font;
    HDC erase_dc;
    RECT erase_rect;
    COLORREF button_color;
    DRAWITEMSTRUCT * draw_item;
    HWND color_control;
    int color_id;
    HDC color_dc;
    HBRUSH color_brush;
    HWND previous_focus;
    HWND focus_control;
    UINT notify_id;
    WPARAM selected_portrait;
    WPARAM previous_portrait;
    LRESULT portrait_count;
    WPARAM next_portrait;
    int quote_reserved;
    int closing_quote;
    UINT command_id;
    char input_text[1024];
    UINT notification;
    WPARAM profile_selection;
    RECT invalidate_rect;
    char * input_cursor;
    MSG pending_message;
    int destroy_index;
    char listed_portrait_name[100];
    WPARAM portrait_index;
    HANDLE find_handle;
    WIN32_FIND_DATAA find_data;
    char filename[264];
    char previous_directory[264];
    int init_index;
    WPARAM init_font;
    RECT * initial_rect;
    LOGFONTA * init_font_description;
    int listed_portrait_count;
    LONG edit_style;
  } s;
  switch (message)
  {
  case WM_INITDIALOG:

    if (FamInterface_HasOpponent() != 0)
    {
      EnableWindow(GetDlgItem(hwnd, 0x742), 0);
      EnableWindow(GetDlgItem(hwnd, 0x717), 0);
      EnableWindow(GetDlgItem(hwnd, 0x741), 0);
      ShowWindow(GetDlgItem(hwnd, 0x742), 0);
      ShowWindow(GetDlgItem(hwnd, 0x717), 0);
      ShowWindow(GetDlgItem(hwnd, 0x741), 0);
    }
    else
    {
      EnableWindow(GetDlgItem(hwnd, 0x742), 1);
      EnableWindow(GetDlgItem(hwnd, 0x717), 1);
      EnableWindow(GetDlgItem(hwnd, 0x741), 1);
      ShowWindow(GetDlgItem(hwnd, 0x742), 5);
      ShowWindow(GetDlgItem(hwnd, 0x717), 5);
      ShowWindow(GetDlgItem(hwnd, 0x741), 5);
    }
    s.edit_style = GetWindowLongA(GetDlgItem(hwnd, 0x6f0), GWL_STYLE);
    s.edit_style |= ES_READONLY;
    SetWindowLongA(GetDlgItem(hwnd, 0x6f0), GWL_STYLE, s.edit_style);
    for (s.init_index = 0; s.init_index < 200; s.init_index = s.init_index + 1)
    {
      g_screen_name_playface_resources.playfaces[s.init_index] = NULL;
    }

    ShowWindow(GetDlgItem(hwnd, 0x715), 0);
    load_text("MP_UIStrings.txt", "SKILLLEVELNAMES");
    for (s.init_index = 0; s.init_index < 10; s.init_index = s.init_index + 1)
    {
      strcpy(g_screen_name_skill_names[s.init_index], g_text_lines[s.init_index]);
    }
    s.init_font_description = LoadFontFromIni("ShellDialog", 0);
    g_screen_name_text_resources.font = CreateFontIndirectA((LOGFONTA *)s.init_font_description);
    s.init_font_description = LoadFontFromIni("ShellDialog_lores", 0);
    g_screen_name_unknown_resources.font = CreateFontIndirectA((LOGFONTA *)s.init_font_description);
    s.init_font = (WPARAM)g_screen_name_text_resources.font;
    SendDlgItemMessageA(hwnd, 0x70c, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x713, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x70d, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6f0, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x70e, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6f3, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x70f, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6f1, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x716, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x717, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6f7, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x6f6, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x711, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x736, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x737, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x738, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x739, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x73a, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x73b, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x73c, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x73d, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x73e, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x73f, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x742, 0x30, s.init_font, 0);
    SendDlgItemMessageA(hwnd, 0x741, 0x30, s.init_font, 0);
    shell_populate_screen_names(hwnd);
    GetCurrentDirectoryA(0x104, s.previous_directory);
    SetCurrentDirectoryA(global_base_directory);
    strcpy(s.filename, "PlayFace\\*.*");
    SendDlgItemMessageA(hwnd, 0x711, 0x145, 0, (LPARAM)s.filename);
    s.listed_portrait_count = SendDlgItemMessageA(hwnd, 0x711, 0x146, 0, 0);
    for (s.portrait_index = 0; (int)s.portrait_index < s.listed_portrait_count; s.portrait_index = s.portrait_index + 1)
    {
      SendDlgItemMessageA(hwnd, 0x711, 0x148, s.portrait_index, (LPARAM)s.listed_portrait_name);
      sprintf(s.filename, "PlayFace\\%s", s.listed_portrait_name);
      s.find_handle = FindFirstFileA(s.filename, &s.find_data);
      GetFileTitleA(s.find_data.cFileName, s.filename, 0x104);
      if (_stricmp(s.filename + (strlen(s.filename) - 4), ".PIC") == 0)
      {
        s.filename[strlen(s.filename) - 4] = '\0';
      }
      FindClose(s.find_handle);
      SendDlgItemMessageA(hwnd, 0x711, 0x144, s.portrait_index, 0);
      SendDlgItemMessageA(hwnd, 0x711, 0x14a, s.portrait_index, (LPARAM)s.filename);
    }
    SetCurrentDirectoryA(s.previous_directory);
    SendDlgItemMessageA(hwnd, 0x6f0, 0xc5, 0xd, 0);
    SendDlgItemMessageA(hwnd, 0x6f3, 0xc5, 0xff, 0);
    SendDlgItemMessageA(hwnd, 0x6f1, 0xc5, 0x3ff, 0);
    load_text("MP_UIStrings.txt", "SHELLPAGE_SCREENNAME");
    SetDlgItemTextA(hwnd, 0x70c, g_text_lines[0]);
    SetDlgItemTextA(hwnd, 0x70d, g_text_lines[1]);
    strcpy(g_screen_name_playface_resources.default_name, g_text_lines[2]);
    SetDlgItemTextA(hwnd, 0x70e, g_text_lines[3]);
    SetDlgItemTextA(hwnd, 0x70f, g_text_lines[4]);
    SetDlgItemTextA(hwnd, 0x738, g_text_lines[5]);
    SetDlgItemTextA(hwnd, 0x736, g_text_lines[6]);
    strcpy(g_screen_name_text_resources.date_format, g_text_lines[7]);
    strcpy(g_screen_name_dci_record_label, g_text_lines[8]);
    strcpy(g_screen_name_multiplayer_record_label, g_text_lines[9]);
    strcpy(g_screen_name_record_label, g_text_lines[10]);
    strcpy(g_screen_name_rank_format, g_text_lines[0xb]);
    strcpy(g_screen_name_skill_format, g_text_lines[0xc]);
    strcpy(g_screen_name_concede_format, g_text_lines[0xd]);
    strcpy(g_screen_name_disconnect_format, g_text_lines[0xe]);
    SetDlgItemTextA(hwnd, 0x717, g_text_lines[0xf]);
    SetDlgItemTextA(hwnd, 0x742, g_text_lines[0x10]);
    strcpy(g_screen_name_unknown_resources.unknown_date, g_text_lines[0x11]);
    load_active_screen_name_profile();
    shell_apply_screen_name_profile(hwnd);
    if ((char)g_screen_name_profile.has_profile_stats != 0)
    {
      ShowWindow(GetDlgItem(hwnd, 0x743), 5);
    }
    else
    {
      ShowWindow(GetDlgItem(hwnd, 0x743), 0);
    }

    ShowWindow(GetDlgItem(hwnd, 0x713), 0);
    ShowWindow(GetDlgItem(hwnd, 0x714), 0);
    ShowWindow(GetDlgItem(hwnd, 0x70c), 0);
    SendMessageA(hwnd, 0x401, 1, 0);
    change_buttonclass_wndproc(hwnd);
    s.initial_rect = (RECT *)lparam;
    MoveWindow(hwnd, s.initial_rect->left, s.initial_rect->top, s.initial_rect->right - s.initial_rect->left,
        s.initial_rect->bottom - s.initial_rect->top, 1);
    g_screen_name_profile_changed = 0;
    g_screen_name_worker_running = 1;
    _beginthread(shell_screen_name_network_worker, 0, hwnd);
    return FALSE;

  case WM_DESTROY:

    if (g_screen_name_profile_changed != 0)
    {
      g_screen_name_profile.unk_744 = 1;
    }
    shell_save_match_screen_name_profile();
    if (PeekMessageA(&s.pending_message, NULL, 0x12, 0x12, 0) == 0 && g_screen_name_profile_changed != 0)
    {
      FamInterface_UpdateScreenName();
    }
    g_screen_name_worker_running = 0;
    for (s.destroy_index = 0; s.destroy_index < 200; s.destroy_index = s.destroy_index + 1)
    {
      if (*(int *)(g_screen_name_playface_resources.default_name + s.destroy_index * 4 + 0x38) != 0)
      {
        delete_and_close_object(g_screen_name_playface_resources.playfaces[s.destroy_index]);
      }
    }
    SendDlgItemMessageA(hwnd, 0x70c, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x713, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x70d, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6f0, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x70e, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6f3, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x70f, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6f1, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x716, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x717, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6f7, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x6f6, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x711, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x736, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x737, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x738, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x739, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x73a, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x73b, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x73c, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x73d, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x73e, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x73f, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x742, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x741, 0x30, 0, 0);
    DeleteObject((HGDIOBJ)g_screen_name_unknown_resources.font);
    DeleteObject((HGDIOBJ)g_screen_name_text_resources.font);
    return FALSE;

  case WM_COMMAND:

    s.command_id = (unsigned int)wparam & 0xffff;
    s.notification = HIWORD(wparam);
    switch(s.command_id) {      case 0x717:
      if (strcmp(g_screen_name_profile.screen_name, g_screen_name_playface_resources.default_name) != 0)
      {
        if ((char)g_screen_name_profile.has_profile_stats != 0)
        {
          load_text("MP_UIStrings.txt", "DELETESCREENNAME");
          if (MessageBoxA(hwnd, g_text_lines[1], g_text_lines[0], 0x21) == 1)
          {
            sprintf(s.input_text, "ScreenNames\\%s.scn", g_screen_name_profile.screen_name);
            DeleteFileA(s.input_text);
            initialize_screen_name_profile(&g_screen_name_profile, 1);
            shell_apply_screen_name_profile(hwnd);
            shell_get_playface_rect(hwnd, &s.invalidate_rect);
            InvalidateRect(hwnd, &s.invalidate_rect, 1);
            shell_save_match_screen_name_profile();
            shell_populate_screen_names(hwnd);
          }
        }
        else
        {
          sprintf(s.input_text, "ScreenNames\\%s.scn", g_screen_name_profile.screen_name);
          DeleteFileA(s.input_text);
          initialize_screen_name_profile(&g_screen_name_profile, 1);
          shell_apply_screen_name_profile(hwnd);
          shell_get_playface_rect(hwnd, &s.invalidate_rect);
          InvalidateRect(hwnd, &s.invalidate_rect, 1);
          shell_save_match_screen_name_profile();
          shell_populate_screen_names(hwnd);
        }
      }
      g_screen_name_profile_changed = 1;
      break;
    case 0x742:
      initialize_screen_name_profile(&g_screen_name_profile, 1);
      DialogBoxParamA(g_app_instance, "ENTERSCREENNAME", global_main_hwnd, shell_enter_screen_name_dialog_proc, 0);
      shell_save_match_screen_name_profile();
      shell_populate_screen_names(hwnd);
      shell_apply_screen_name_profile(hwnd);
      if ((char)g_screen_name_profile.has_profile_stats != 0)
      {
        ShowWindow(GetDlgItem(hwnd, 0x743), 5);
      }
      else
      {
        ShowWindow(GetDlgItem(hwnd, 0x743), 5);
      }
      shell_get_playface_rect(hwnd, &s.invalidate_rect);
      InvalidateRect(hwnd, &s.invalidate_rect, 1);
      SetFocus(GetDlgItem(hwnd, 0x6f3));
      g_screen_name_profile_changed = 1;
      break;
    case 0x741:
      if (s.notification == 9)
      {
        s.profile_selection = SendDlgItemMessageA(hwnd, 0x741, 0x147, 0, 0);
        SendDlgItemMessageA(hwnd, 0x741, 0x148, s.profile_selection, (LPARAM)s.input_text);
        if (strcmp(g_screen_name_profile.screen_name, s.input_text) != 0)
        {
          strcpy(g_screen_name_profile.screen_name, s.input_text);
          shell_save_active_screen_name();
          load_active_screen_name_profile();
          shell_apply_screen_name_profile(hwnd);
          if ((char)g_screen_name_profile.has_profile_stats != 0)
          {
            ShowWindow(GetDlgItem(hwnd, 0x743), 5);
          }
          else
          {
            ShowWindow(GetDlgItem(hwnd, 0x743), 0);
          }
          shell_get_playface_rect(hwnd, &s.invalidate_rect);
          InvalidateRect(hwnd, &s.invalidate_rect, 1);
          g_screen_name_profile_changed = 1;
        }
      }
      break;
    case 0x739:
      if (s.notification == 0x200)
      {
        GetDlgItemTextA(hwnd, s.command_id, s.input_text, 0x100);
        strcpy(g_screen_name_profile.email, s.input_text);
        shell_save_match_screen_name_profile();
        shell_get_playface_rect(hwnd, &s.invalidate_rect);
        InvalidateRect(hwnd, &s.invalidate_rect, 1);
        g_screen_name_profile_changed = 1;
      }
      break;
    case 0x6f3:
      if (s.notification == 0x200)
      {
        s.closing_quote = 0;
        GetDlgItemTextA(hwnd, s.command_id, s.input_text, 0x100);
        for (s.input_cursor = s.input_text; *s.input_cursor != '\0'; s.input_cursor = s.input_cursor + 1)
        {
          if (*s.input_cursor == '\"')
          {
            if (s.closing_quote == 0)
            {
              *s.input_cursor = -0x6e;
            }
            else
            {
              *s.input_cursor = -0x6d;
            }
            s.closing_quote = s.closing_quote == 0;
          }
        }
        SetDlgItemTextA(hwnd, s.command_id, s.input_text);
        strcpy(g_screen_name_profile.real_name, s.input_text);
        shell_save_match_screen_name_profile();
        g_screen_name_profile_changed = 1;
      }
      break;

    case 0x6f1:
      if (s.notification == 0x200)
      {
        s.quote_reserved = 0;
        GetDlgItemTextA(hwnd, s.command_id, s.input_text, 0x400);
        strcpy(g_screen_name_profile.personal_quote, s.input_text);
        shell_save_match_screen_name_profile();
        g_screen_name_profile_changed = 1;
      }
      break;
    case 0x6f6:
    case 0x6f7:
      s.previous_portrait = SendDlgItemMessageA(hwnd, 0x711, 0x147, 0, 0);
      s.portrait_count = SendDlgItemMessageA(hwnd, 0x711, 0x146, 0, 0);
      s.next_portrait = ((int)s.previous_portrait + (s.command_id == 0x6f7 ? -1 : 1)) % s.portrait_count;
      if ((int)s.next_portrait < 0)
      {
        s.next_portrait = s.portrait_count - 1;
      }
      if (s.next_portrait != s.previous_portrait)
      {
        s.previous_portrait = s.next_portrait;
        SendDlgItemMessageA(hwnd, 0x711, 0x14e, s.next_portrait, 0);
        shell_get_playface_rect(hwnd, &s.invalidate_rect);
        InvalidateRect(hwnd, &s.invalidate_rect, 0);
        SendDlgItemMessageA(hwnd, 0x711, 0x148, s.previous_portrait, (LPARAM)g_screen_name_profile.playface_name);
        shell_save_match_screen_name_profile();
      }
      g_screen_name_profile_changed = 1;
      break;
    case 0x711:
      if (s.notification == 9)
      {
        s.selected_portrait = SendDlgItemMessageA(hwnd, 0x711, 0x147, 0, 0);
        SendDlgItemMessageA(hwnd, 0x711, 0x148, s.selected_portrait, (LPARAM)g_screen_name_profile.playface_name);
        shell_get_playface_rect(hwnd, &s.invalidate_rect);
        InvalidateRect(hwnd, &s.invalidate_rect, 1);
        shell_save_match_screen_name_profile();
        g_screen_name_profile_changed = 1;
      }
      break;
    }
    return TRUE;

  case WM_NOTIFY:

    s.notify_id = (UINT)wparam;
    if (s.notify_id == 0x714)
      PostMessageA(hwnd, WM_COMMAND, 0x2000713, (LPARAM)GetDlgItem(hwnd, 0x713));
    return FALSE;

  case 0x4c8:

    s.focus_control = (HWND)wparam;
    s.previous_focus = (HWND)lparam;
    SendMessageA(hwnd, 0x401, 1, 0);
    if (s.focus_control != NULL)
    {
      InvalidateRect((HWND)s.focus_control, NULL, 1);
    }
    if (s.previous_focus != NULL)
    {
      InvalidateRect(s.previous_focus, NULL, 1);
    }
    return FALSE;

  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:

    s.color_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.color_dc);
    s.color_control = (HWND)lparam;
    s.color_id = GetDlgCtrlID(s.color_control);
    if (s.color_id == 0x713 || s.color_id == 0x6f0 || s.color_id == 0x6f3 || s.color_id == 0x6f1 ||
        s.color_id == 0x711 || s.color_id == 0x737 || s.color_id == 0x739 || s.color_id == 0x741)
    {
      SetTextColor(s.color_dc, shell_draw_resources.background_color);
      SetBkMode(s.color_dc, OPAQUE);
      SetBkColor(s.color_dc, shell_draw_resources.panel_color);
      s.color_brush = shell_draw_resources.panel_brush;
    }
    else if (s.color_id == 0x73a || s.color_id == 0x73b || s.color_id == 0x73e || s.color_id == 0x73f)
    {
      SetTextColor(s.color_dc, shell_draw_resources.button_text_color);
      SetBkColor(s.color_dc, 0x6f6a52);
      SetBkMode(s.color_dc, OPAQUE);
      s.color_brush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    else if (s.color_id == 0x73c || s.color_id == 0x73d)
    {
      SetTextColor(s.color_dc, shell_draw_resources.button_text_color);
      SetBkColor(s.color_dc, 0x8e6f11);
      SetBkMode(s.color_dc, OPAQUE);
      s.color_brush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    else if (s.color_id == 0x716)
    {
      SetTextColor(s.color_dc, shell_draw_resources.button_text_color);
      SetBkColor(s.color_dc, 0x8b736e);
      SetBkMode(s.color_dc, OPAQUE);
      s.color_brush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    else
    {
      if (GetFocus() == s.color_control)
        SetTextColor(s.color_dc, shell_draw_resources.selected_button_color);
      else
        SetTextColor(s.color_dc, shell_draw_resources.button_text_color);
      SetBkMode(s.color_dc, TRANSPARENT);
      s.color_brush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }

    return (BOOL)s.color_brush;
  case WM_DRAWITEM:

    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
      s.button_color = shell_draw_resources.selected_button_color;
    else if (s.draw_item->itemState & ODS_GRAYED)
      s.button_color = 0x10000c6;
    else
      s.button_color = shell_draw_resources.regular_button_color;
    if (shell_draw_resources.button_face != NULL)
      shell_draw_bitmap_button(s.draw_item, shell_draw_resources.button_face, shell_draw_resources.light_pen,
        shell_draw_resources.dark_pen, s.button_color, 0);
    else
      draw_owner_draw_button_centered(s.draw_item, (HBRUSH)GetStockObject(GRAY_BRUSH),
        shell_draw_resources.light_pen, shell_draw_resources.dark_pen, s.button_color, 0);
    return TRUE;

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, message, (int)wparam, (long)lparam);
  case WM_ERASEBKGND:

    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.erase_rect);
    MapWindowPoints(hwnd, GetParent(hwnd), (LPPOINT)&s.erase_rect, 2);
    SendMessageA(GetParent(hwnd), 0x498, (WPARAM)s.erase_dc, (LPARAM)&s.erase_rect);
    return TRUE;

  case WM_SIZE:

    s.control_count = 0;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x70c);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x713);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x714);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x70d);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x6f0);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x70e);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x6f3);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x70f);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x6f1);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x716);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x717);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x6f7);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x6f6);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x711);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x736);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x737);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x738);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x739);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x73a);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x73c);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x73d);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x73b);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x73e);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x73f);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x742);
    s.control_count = s.control_count + 1;
    s.controls[s.control_count] = GetDlgItem(hwnd, 0x741);
    s.control_count = s.control_count + 1;
    LockWindowUpdate(hwnd);
    GetClientRect(global_main_hwnd, &s.shell_rect);
    if ((s.shell_rect.right < 0x2ee) || (s.shell_rect.bottom < 0x226))
    {
      s.font_description = LoadFontFromIni("ShellDialog_lores", 0);
      s.old_font = (HANDLE)g_screen_name_unknown_resources.font;
    }
    else
    {
      s.font_description = LoadFontFromIni("ShellDialog", 0);
      s.old_font = (HANDLE)g_screen_name_text_resources.font;
    }
    GetObjectA(s.old_font, 0x3c, &s.resize_font);
    s.base_font_height = s.font_description->lfHeight;
    s.resize_font.lfHeight = (s.shell_rect.bottom - s.shell_rect.top) / s.base_font_height;
    if (s.resize_font.lfHeight % 2 != 0)
    {
      s.resize_font.lfHeight = s.resize_font.lfHeight + 1;
    }
    s.replacement_font = CreateFontIndirectA(&s.resize_font);
    if (s.old_font == (HANDLE)g_screen_name_unknown_resources.font)
      g_screen_name_unknown_resources.font = s.replacement_font;
    else
      g_screen_name_text_resources.font = s.replacement_font;
    for (s.control_index = 0; s.control_index < s.control_count; s.control_index = s.control_index + 1)
    {
      SendMessageA((HWND)s.controls[s.control_index], 0x30, (WPARAM)s.replacement_font, 0);
    }
    shell_layout_screen_name_dialog(hwnd);
    DeleteObject(s.old_font);
    LockWindowUpdate(NULL);
    return TRUE;

  case 0x499:

    for (s.clear_index = 0; s.clear_index < 200; s.clear_index = s.clear_index + 1)
    {
      if (*(int *)(g_screen_name_playface_resources.default_name + s.clear_index * 4 + 0x38) != 0)
      {
        delete_and_close_object(g_screen_name_playface_resources.playfaces[s.clear_index]);
      }
    }
    return FALSE;

  case WM_PAINT:

    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != NULL)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetWindowRect(GetDlgItem(hwnd, 0x715), &s.portrait_rect);
      MapWindowPoints(NULL, hwnd, (LPPOINT)&s.portrait_rect, 2);
      SelectObject(s.paint_dc, shell_draw_resources.light_pen);
      MoveToEx(s.paint_dc, s.portrait_rect.right + -3, s.portrait_rect.top + 1, NULL);
      LineTo(s.paint_dc, s.portrait_rect.left + 1, s.portrait_rect.top + 1);
      LineTo(s.paint_dc, s.portrait_rect.left + 1, s.portrait_rect.bottom + -2);
      MoveToEx(s.paint_dc, s.portrait_rect.right + -1, s.portrait_rect.top, NULL);
      LineTo(s.paint_dc, s.portrait_rect.right + -1, s.portrait_rect.bottom + -1);
      LineTo(s.paint_dc, s.portrait_rect.left + -1, s.portrait_rect.bottom + -1);
      SelectObject(s.paint_dc, shell_draw_resources.dark_pen);
      SelectObject(s.paint_dc, GetStockObject(5));
      Rectangle(s.paint_dc, s.portrait_rect.left, s.portrait_rect.top, s.portrait_rect.right + -1,
          s.portrait_rect.bottom + -1);
      InflateRect(&s.portrait_rect, -2, -2);
      s.portrait_selection = SendDlgItemMessageA(hwnd, 0x711, 0x147, 0, 0);
      if (s.portrait_selection == 0xffffffff)
      {
        s.portrait_bitmap = NULL;
      }
      else
      {
        if (*(int *)(g_screen_name_playface_resources.default_name + s.portrait_selection * 4 + 0x38) == 0)
        {
          SendDlgItemMessageA(hwnd, 0x711, 0x148, s.portrait_selection, (LPARAM)s.portrait_name);
          sprintf(s.portrait_path, "%s\\PlayFace\\%s.PIC", global_base_directory, s.portrait_name);
          g_screen_name_playface_resources.playfaces[s.portrait_selection] = load_pic(s.portrait_path);
        }
        s.portrait_bitmap = *(HANDLE *)(g_screen_name_playface_resources.default_name + s.portrait_selection * 4 + 0x38);
      }
      if (s.portrait_bitmap != NULL)
      {
        GetObjectA(s.portrait_bitmap, sizeof(BITMAP), &s.portrait_info);
        DrawBitmapSubrectToRect
        (s.paint_dc, &s.portrait_rect, s.portrait_bitmap, 0, 0, s.portrait_info.bmWidth / 2,
            s.portrait_info.bmHeight);
      }
      else
      {
        FillRect(s.paint_dc, &s.portrait_rect, GetStockObject(4));
      }
      EndPaint(hwnd, &s.paint);
    }
    return TRUE;

  default:
    return FALSE;
  }
}
