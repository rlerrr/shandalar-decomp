#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cardartlib/src/palette.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/card_db.h"
#include "magic_sealed_ladder.h"
#include "magic_sealed_duel.h"
#include "magic_sealed_player.h"
#include "magic_sealed_build.h"
#include "magic_shell_dialogs.h"
#include "magic_shell.h"
#include "game_support.h"
#include "global_other.h"
#include "global_state.h"
#include "global_strings.h"
#include "shared_startup.h"

extern HWND global_main_hwnd;
extern char g_exp1_art_path[];
extern int g_showlist_smallcard_width;
extern int g_showlist_smallcard_height;
void prepare_duel_video_mode_transition(void);
void shell_save_sealed_duel(HWND hwnd);
void TENTATIVE_savegame(int autosave_slot);
BOOL DrawMaskedBitmapToRect(HDC dc, RECT *rect, HANDLE bitmap, int width, int height, int src_x, int src_y,
    int mask_x, int mask_y);
void draw_owner_draw_button_centered(DRAWITEMSTRUCT *item, HBRUSH brush, HPEN light_pen, HPEN dark_pen,
    COLORREF color, int focus);

static void shell_load_sealed_ladder_resources(HBITMAP *background, COLORREF *title_color,
    COLORREF *shadow_color, COLORREF *text_color, COLORREF *selected_color, HBITMAP *bars, HBITMAP *winner,
    HBITMAP *button_face, HPEN *light_pen, HPEN *dark_pen, COLORREF *button_color, COLORREF *focus_color);
static void shell_release_sealed_ladder_resources(HBITMAP background, HBITMAP bars, HBITMAP winner,
    HBITMAP button_face, HPEN light_pen, HPEN dark_pen);
#define LADDER_POINT_IN_RECT(rect, x, y) \
  ((BOOL(WINAPI *)(const RECT *, LONG, LONG))PtInRect)((rect), (x), (y))

static void shell_layout_sealed_ladder_buttons(HWND hwnd);
static int shell_sealed_bracket_round_start(int count, int round, int *last);
static int shell_sealed_ladder_entry_rects(HWND hwnd, HFONT font, sealed_deck_tournament_t *tournament,
    int round, int entry, RECT *first, RECT *second);
static sealed_deck_player_t *shell_sealed_ladder_hit_test(HWND hwnd, RECT *rects,
    sealed_deck_tournament_t *tournament, LONG x, LONG y, RECT *hit_rect);
static void shell_show_sealed_player(HWND hwnd, sealed_deck_player_t *player);
static int shell_play_sealed_ladder_round(HWND hwnd, sealed_deck_tournament_t *tournament, int round);
static unsigned int __stdcall shell_run_sealed_opponent_matches(sealed_deck_tournament_t *tournament);
static int shell_next_sealed_opponent_match(sealed_deck_tournament_t *tournament, int round, int *winner_entry);
static void shell_pick_sealed_free_lands(sealed_deck_tournament_t *tournament, HWND hwnd);

// GLOBAL: MAGIC 0x0064fbb8
static HFONT g_ladder_title_font;
// GLOBAL: MAGIC 0x0064fa50
static HFONT g_ladder_entry_font;
// GLOBAL: MAGIC 0x0064faec
static sealed_deck_tournament_t *g_ladder_tournament;
// GLOBAL: MAGIC 0x0069aa24
static HBITMAP g_ladder_background;
// GLOBAL: MAGIC 0x0069b3b4
static HBITMAP g_ladder_bars;
// GLOBAL: MAGIC 0x0069afec
static HBITMAP g_ladder_winner;
// GLOBAL: MAGIC 0x0069af4c
static HBITMAP g_ladder_button_face;
// GLOBAL: MAGIC 0x0069b3b0
static HPEN g_ladder_light_pen;
// GLOBAL: MAGIC 0x0069b390
static HPEN g_ladder_dark_pen;
// GLOBAL: MAGIC 0x0069b39c
static COLORREF g_ladder_button_color;
// GLOBAL: MAGIC 0x0069aee0
static COLORREF g_ladder_focus_color;
// GLOBAL: MAGIC 0x0064fa54
static COLORREF g_ladder_title_color;
// GLOBAL: MAGIC 0x0069aed8
static COLORREF g_ladder_shadow_color;
// GLOBAL: MAGIC 0x0064f928
static COLORREF g_ladder_text_color;
// GLOBAL: MAGIC 0x0069aff4
static COLORREF g_ladder_selected_color;
// GLOBAL: MAGIC 0x0069aeb4
static HDC g_ladder_background_dc;
// GLOBAL: MAGIC 0x0069b320
static int g_ladder_background_width;
// GLOBAL: MAGIC 0x0069b3ac
static int g_ladder_background_height;
// GLOBAL: MAGIC 0x0069af18
static int g_ladder_show_bracket;
// GLOBAL: MAGIC 0x0069b318
static int g_ladder_blink;
// GLOBAL: MAGIC 0x0069b398
static int g_ladder_blink_rect_count;
// GLOBAL: MAGIC 0x0069aa30
static RECT g_ladder_entry_rects[64];
// GLOBAL: MAGIC 0x0069aff8
static RECT g_ladder_blink_rects[2];
// GLOBAL: MAGIC 0x0069af38
static RECT g_ladder_hover_rect;
// GLOBAL: MAGIC 0x0069b2b0
static char g_ladder_title[100];
// GLOBAL: MAGIC 0x0064fa60
static char g_ladder_opponents_building[100];
// GLOBAL: MAGIC 0x0064f930
static char g_ladder_decks_building[100];
// GLOBAL: MAGIC 0x0069af50
static char g_ladder_play_caption[100];
// GLOBAL: MAGIC 0x0064fac8
static char g_ladder_finished_caption[100];

// FUNCTION: MAGIC 0x004c81ff
BOOL CALLBACK shell_sealed_ladder_dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int winner_second_index;
    int winner_first_index;
    int right_second_index;
    int right_first_index;
    int left_second_index;
    int left_first_index;
    int blink_rect_index;
    int resize_rect_index;
    int resize_rounds;
    int resize_player_count;
    int resize_entry;
    int resize_round;
    LOGFONTA resize_font_description;
    HFONT new_font;
    int button_index;
    HWND buttons [20];
    int button_count;
    RECT resize_rect;
    HFONT old_font;
    COLORREF button_color;
    DRAWITEMSTRUCT *draw_item;
    HWND colored_control;
    int control_id;
    HDC control_dc;
    HBRUSH control_brush;
    HWND old_focus;
    HWND new_focus;
    int blink_index;
    int paint_rounds;
    int paint_rect_index;
    int winner_width;
    int paint_player_count;
    int bracket_index;
    BITMAP art_bitmap;
    int winner_text_y;
    int paint_entry;
    int text_x;
    int mask_x;
    int bar_height;
    int winner_height;
    RECT first_rect;
    RECT second_rect;
    int bar_width;
    int entry_width;
    sealed_deck_player_t *paint_player;
    int paint_round;
    int paint_player_index;
    int mask_y;
    SIZE text_size;
    int source_y;
    int text_height;
    int first_text_y;
    int second_text_y;
    int source_x;
    HDC paint_dc;
    HDC offscreen_dc;
    PAINTSTRUCT paint;
    int saved_dc;
    HRGN update_region;
    RECT update_rect;
    RECT client_rect;
    sealed_deck_player_t *clicked_player;
    uint click_x;
    uint click_y;
    RECT moved_rect;
    sealed_deck_player_t *hovered_player;
    RECT invalidate_rect;
    uint hover_x;
    uint hover_y;
    RECT hit_rect;
    HANDLE closing_thread;
    char message_text [300];
    uint match_thread_id;
    int match_result;
    HANDLE reset_thread;
    int reset_index;
    int reset_start;
    int command_id;
    uint notification;
    HWND command_control;
    HGDIOBJ button_font;
    uint init_thread_id;
    uint window_style;
    LOGFONTA font_description;
    BITMAP background_bitmap;
    HFONT dialog_font;
    RECT owner_rect;

  } s;

  switch (message)
  {
  case 0x496:

    if (wparam != 0)
    {
      ShowWindow(hwnd, 5);
    }
    else
    {
      ShowWindow(hwnd, 0);
    }
    return TRUE;

  case WM_INITDIALOG:

    SetWindowTextA(hwnd, gs_magic_the_gathering_title_00789460);
    g_ladder_blink = 0;
    g_ladder_blink_rect_count = 0;
    SetTimer(hwnd, 3, 0x5dc, NULL);
    GetClientRect(hwnd, &s.owner_rect);
    s.blink_rect_index = g_ladder_blink_rect_count;
    g_ladder_blink_rect_count = g_ladder_blink_rect_count + 1;
    SetRect(&g_ladder_blink_rects[s.blink_rect_index], s.owner_rect.left,
        s.owner_rect.top + ((s.owner_rect.bottom - s.owner_rect.top) * 3) / 100, s.owner_rect.right,
        s.owner_rect.top + ((s.owner_rect.bottom - s.owner_rect.top) * 0x14) / 100);
    g_ladder_blink_rect_count = g_ladder_blink_rect_count + 1;
    s.window_style = GetWindowLongA(hwnd, -0x10);
    s.window_style = s.window_style | 0x2000000;
    SetWindowLongA(hwnd, -0x10, s.window_style);
    shell_load_sealed_ladder_resources(&g_ladder_background, &g_ladder_title_color, &g_ladder_shadow_color,
        &g_ladder_text_color, &g_ladder_selected_color, &g_ladder_bars, &g_ladder_winner,
        &g_ladder_button_face, &g_ladder_light_pen, &g_ladder_dark_pen, &g_ladder_button_color,
        &g_ladder_focus_color);
    if (g_ladder_background != NULL)
    {
      g_ladder_background_dc = CreateCompatibleDC(NULL);
      GetObjectA(g_ladder_background, sizeof(BITMAP), &s.background_bitmap);
      g_ladder_background_width = s.background_bitmap.bmWidth;
      g_ladder_background_height = s.background_bitmap.bmHeight;
      SelectObject(g_ladder_background_dc, g_ladder_background);
    }
    s.dialog_font = (HFONT)SendMessageA(hwnd, 0x31, 0, 0);
    GetObjectA(s.dialog_font, 0x3c, &s.font_description);
    g_ladder_title_font = CreateFontIndirectA(&s.font_description);
    GetObjectA(s.dialog_font, 0x3c, &s.font_description);
    g_ladder_entry_font = CreateFontIndirectA(&s.font_description);
    GetObjectA(s.dialog_font, 0x3c, &s.font_description);
    s.dialog_font = CreateFontIndirectA(&s.font_description);
    SendDlgItemMessageA(hwnd, 0x718, 0x30, (WPARAM)s.dialog_font, 0);
    SendDlgItemMessageA(hwnd, 0x693, 0x30, (WPARAM)s.dialog_font, 0);
    SendDlgItemMessageA(hwnd, 0x691, 0x30, (WPARAM)s.dialog_font, 0);
    SendDlgItemMessageA(hwnd, 1, 0x30, (WPARAM)s.dialog_font, 0);
    SendDlgItemMessageA(hwnd, 0x692, 0x30, (WPARAM)s.dialog_font, 0);
    g_ladder_tournament = (sealed_deck_tournament_t *)lparam;
    g_ladder_tournament->opponent_match_thread = NULL;
    SetTimer(hwnd, 1, 1000, NULL);
    g_ladder_show_bracket = 1;
    ShowWindow(GetDlgItem(hwnd, 0x69c), 0);
    ShowWindow(GetDlgItem(hwnd, 0x69d), 0);
    ShowWindow(GetDlgItem(hwnd, 0x69b), 0);
    ShowWindow(GetDlgItem(hwnd, 0x69a), 0);
    load_text(global_ui_strings_filename, "SEALEDDECK_LADDERSCREEN");
    strcpy(g_ladder_title, g_text_lines[0]);
    SetDlgItemTextA(hwnd, 0x693, g_text_lines[1]);
    SetDlgItemTextA(hwnd, 0x691, g_text_lines[2]);
    SetDlgItemTextA(hwnd, 0x692, g_text_lines[3]);
    SetDlgItemTextA(hwnd, 0x718, g_text_lines[4]);
    SetDlgItemTextA(hwnd, 1, g_text_lines[5]);
    strcpy(g_ladder_play_caption, g_text_lines[5]);
    strcpy(g_ladder_finished_caption, g_text_lines[8]);
    load_text(global_ui_strings_filename, "LADDER_OPPONBUILDING");
    strcpy(g_ladder_decks_building, g_text_lines[0]);
    strcpy(g_ladder_opponents_building, g_text_lines[1]);
    SetFocus(GetDlgItem(hwnd, 1));
    SendMessageA(hwnd, 0x401, 1, 0);
    change_buttonclass_wndproc(hwnd);
    GetWindowRect(GetParent(hwnd), &s.owner_rect);
    MoveWindow(hwnd, s.owner_rect.left, s.owner_rect.top, s.owner_rect.right - s.owner_rect.left,
        s.owner_rect.bottom - s.owner_rect.top, 1);
    if (g_ladder_tournament->players[0].deck_built == 0)
    {
      SetTimer(hwnd, 2, 100, NULL);
    }
    else if (g_ladder_tournament->duel_in_progress != 0)
    {
      SendMessageA(hwnd, 0x111, 1, (LPARAM)GetDlgItem(hwnd, 1));
    }
    else
    {
      if (g_ladder_tournament->opponents_in_progress != 0)
      {
        EnableWindow(GetDlgItem(hwnd, 1), 0);
        g_ladder_tournament->opponent_match_thread = (HANDLE)_beginthreadex((void *)0x0, 0,
            (unsigned int (__stdcall *)(void *))shell_run_sealed_opponent_matches, g_ladder_tournament, 0, &s.init_thread_id);
        SetThreadPriority((HANDLE)g_ladder_tournament->opponent_match_thread, -1);
      }
    }
    return FALSE;

  case WM_DESTROY:

    DeleteDC(g_ladder_background_dc);
    shell_release_sealed_ladder_resources(g_ladder_background, g_ladder_bars, g_ladder_winner,
        g_ladder_button_face, g_ladder_light_pen, g_ladder_dark_pen);
    DeleteObject(g_ladder_title_font);
    DeleteObject(g_ladder_entry_font);
    s.button_font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 0x718, 0x31, 0, 0);
    SendDlgItemMessageA(hwnd, 0x718, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x693, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x691, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 1, 0x30, 0, 0);
    SendDlgItemMessageA(hwnd, 0x692, 0x30, 0, 0);
    DeleteObject(s.button_font);
    KillTimer(hwnd, 1);
    return FALSE;

  case WM_COMMAND:

    s.command_id = (uint)wparam & 0xffff;
    s.notification = (WORD)(((uint)wparam >> 16) & 0xffff);
    s.command_control = (HWND)lparam;

    switch (s.command_id)
    {
    case 0x691:

      prepare_duel_video_mode_transition();
      sound_close();
      shell_edit_sealed_deck(hwnd, &g_ladder_tournament->players[0]);
      init_sound_dll(global_main_hwnd, 0, 0);

      break;
    case 0x693:

      if (g_ladder_tournament->deck_builder_thread != 0)
      {
        SuspendThread((HANDLE)g_ladder_tournament->deck_builder_thread);
      }
      if (g_ladder_tournament->opponent_match_thread != 0)
      {
        SuspendThread((HANDLE)g_ladder_tournament->opponent_match_thread);
      }
      EnterCriticalSection(&g_ladder_tournament->critical_section);
      shell_save_sealed_duel(hwnd);
      LeaveCriticalSection(&g_ladder_tournament->critical_section);
      if (g_ladder_tournament->deck_builder_thread != 0)
      {
        ResumeThread((HANDLE)g_ladder_tournament->deck_builder_thread);
      }
      if (g_ladder_tournament->opponent_match_thread != 0)
      {
        ResumeThread((HANDLE)g_ladder_tournament->opponent_match_thread);
      }

      break;
    case 0x718:
      if (
          ((g_ladder_tournament->deck_builder_thread == 0 ||
          (WaitForSingleObject((HANDLE)g_ladder_tournament->deck_builder_thread, 0) != 0x102)
          )))
      {
        if (g_ladder_tournament->opponent_match_thread != 0)
        {
          s.reset_thread = (HANDLE)g_ladder_tournament->opponent_match_thread;
          g_ladder_tournament->opponent_match_thread = 0;
          WaitForSingleObject(s.reset_thread, 0xffffffff);
          CloseHandle(s.reset_thread);
        }
        EnterCriticalSection(&g_ladder_tournament->critical_section);
        s.reset_start = shell_sealed_bracket_round_start(1 << (byte)g_ladder_tournament->rounds, 2, NULL);
        for (s.reset_index = s.reset_start; s.reset_index < 0xff; s.reset_index = s.reset_index + 1)
        {
          g_ladder_tournament->bracket[s.reset_index] = -1;
        }
        LeaveCriticalSection(&g_ladder_tournament->critical_section);
        g_ladder_tournament->decks_changed = 1;
        InvalidateRect(hwnd, NULL, 1);
        g_ladder_tournament->current_round = 1;
        g_ladder_tournament->duel_in_progress = 0;
        g_ladder_tournament->opponents_in_progress = 0;
        SetDlgItemTextA(hwnd, 1, g_ladder_play_caption);
        EnableWindow(GetDlgItem(hwnd, 1), 1);
        ShowWindow(GetDlgItem(hwnd, 0x692), 5);

      }
      break;
    case 1:

      if (g_ladder_tournament->rounds < g_ladder_tournament->current_round)
      {
        EndDialog(hwnd, 0);
      }
      else if ((g_ladder_tournament->deck_builder_thread != NULL) &&
          (WaitForSingleObject(g_ladder_tournament->deck_builder_thread, 0) == WAIT_TIMEOUT))
      {
        load_text(global_ui_strings_filename, "SEALEDDECK_LADDERSCREEN");
        MessageBoxA(hwnd, g_text_lines[6], gs_magic_the_gathering_title_00789460, 0x10);
      }
      else
      {
        if (g_ladder_tournament->players[0].deck_count < g_ladder_tournament->minimum_deck_size)
        {
          load_text(global_ui_strings_filename, "SEALEDDECK_LADDERSCREEN");
          sprintf(s.message_text, g_text_lines[7], g_ladder_tournament->minimum_deck_size);
          MessageBoxA(hwnd, s.message_text, gs_magic_the_gathering_title_00789460, 0x10);
        }
        else
        {
          EnableWindow(s.command_control, 0);
          s.match_result = shell_play_sealed_ladder_round(hwnd, g_ladder_tournament,
              g_ladder_tournament->current_round);
          g_ladder_tournament->opponents_in_progress = 1;
          g_ladder_tournament->opponent_match_thread = (HANDLE)_beginthreadex((void *)0x0, 0,
              (unsigned int (__stdcall *)(void *))shell_run_sealed_opponent_matches, g_ladder_tournament, 0, &s.match_thread_id);
          SetThreadPriority((HANDLE)g_ladder_tournament->opponent_match_thread, -1);
          if ((g_ladder_tournament->current_round == g_ladder_tournament->rounds) && (s.match_result == 1))
          {
            UpdateWindow(hwnd);
            shell_play_movie(hwnd, "WinSealedTournament.AVI", g_exp1_art_path, NULL);
          }
        }
      }

      break;
    case 0x692:

      ShowWindow(GetDlgItem(hwnd, 0x718), 0);
      ShowWindow(GetDlgItem(hwnd, 0x691), 0);
      ShowWindow(GetDlgItem(hwnd, 0x693), 0);
      ShowWindow(GetDlgItem(hwnd, 0x692), 0);
      ShowWindow(GetDlgItem(hwnd, 1), 0);
      UpdateWindow(hwnd);
      if (g_ladder_tournament->opponent_match_thread != 0)
      {
        s.closing_thread = (HANDLE)g_ladder_tournament->opponent_match_thread;
        g_ladder_tournament->opponent_match_thread = 0;
        WaitForSingleObject(s.closing_thread, 0xffffffff);
        CloseHandle(s.closing_thread);
      }
      if (g_ladder_tournament->deck_builder_thread != 0)
      {
        s.closing_thread = (HANDLE)g_ladder_tournament->deck_builder_thread;
        g_ladder_tournament->deck_builder_thread = 0;
        WaitForSingleObject(s.closing_thread, 0xffffffff);
        CloseHandle(s.closing_thread);
      }
      EndDialog(hwnd, 0);

      break;
      default: break;
    }
    return TRUE;

  case WM_MOUSEMOVE:

    s.hover_x = (uint)lparam & 0xffff;
    s.hover_y = (WORD)(((uint)lparam >> 16) & 0xffff);
    s.hovered_player = shell_sealed_ladder_hit_test(hwnd, g_ladder_entry_rects, g_ladder_tournament,
        s.hover_x, s.hover_y, &s.hit_rect);
    if (EqualRect(&s.hit_rect, (RECT *)&g_ladder_hover_rect) == 0)
    {
      if (IsRectEmpty((RECT *)&g_ladder_hover_rect) == 0)
      {
        CopyRect(&s.invalidate_rect, (RECT *)&g_ladder_hover_rect);
        InflateRect(&s.invalidate_rect, 0x32, 0);
        InvalidateRect(hwnd, &s.invalidate_rect, 0);
        SetRectEmpty((LPRECT)&g_ladder_hover_rect);
      }
      if (s.hovered_player != NULL)
      {
        CopyRect((LPRECT)&g_ladder_hover_rect, &s.hit_rect);
        if (IsRectEmpty((RECT *)&g_ladder_hover_rect) == 0)
        {
          CopyRect(&s.invalidate_rect, (RECT *)&g_ladder_hover_rect);
          InflateRect(&s.invalidate_rect, 0x32, 0);
          InvalidateRect(hwnd, &s.invalidate_rect, 0);
        }
      }
    }
    return TRUE;

  case WM_LBUTTONDOWN:

    s.click_x = (uint)lparam & 0xffff;
    s.click_y = (WORD)(((uint)lparam >> 16) & 0xffff);
    s.clicked_player = shell_sealed_ladder_hit_test(hwnd, g_ladder_entry_rects, g_ladder_tournament,
        s.click_x, s.click_y, NULL);
    if (s.clicked_player != NULL)
    {
      shell_show_sealed_player(hwnd, s.clicked_player);
    }
    else
    {
      SendMessageA(hwnd, 0x112, 0xf012, 0);
      GetWindowRect(hwnd, &s.moved_rect);
      SetWindowPos(global_main_hwnd, NULL, s.moved_rect.left, s.moved_rect.top, 0, 0, 5);
    }
    return TRUE;

  case WM_PAINT:

    s.update_region = CreateRectRgn(0, 0, 1, 1);
    GetUpdateRgn(hwnd, s.update_region, 0);
    EnterCriticalSection((LPCRITICAL_SECTION)&g_card_render_lock);
    s.offscreen_dc = g_shared_offscreen_dc;
    s.saved_dc = SaveDC(g_shared_offscreen_dc);
    ApplyCardArtPaletteToDc(s.offscreen_dc);
    GetClientRect(hwnd, &s.client_rect);
    GetUpdateRect(hwnd, &s.update_rect, 0);
    IntersectClipRect(s.offscreen_dc, s.update_rect.left, s.update_rect.top, s.update_rect.right,
        s.update_rect.bottom);
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (g_ladder_background != NULL)
    {
      StretchBlt(s.offscreen_dc, s.client_rect.left, s.client_rect.top,
          s.client_rect.right - s.client_rect.left, s.client_rect.bottom - s.client_rect.top,
          g_ladder_background_dc, 0, 0, g_ladder_background_width, g_ladder_background_height, 0xcc0020);
    }
    else
    {
      FillRect(s.offscreen_dc, &s.client_rect, GetStockObject(GRAY_BRUSH));
    }
    if (g_ladder_show_bracket != 0)
    {
      SelectObject(s.offscreen_dc, g_ladder_title_font);
      GetTextExtentPoint32A(s.offscreen_dc, "fun", 3, &s.text_size);
      GetClientRect(hwnd, &s.client_rect);
      s.client_rect.bottom = s.text_size.cy;
      OffsetRect(&s.client_rect, 0, 5);
      SetBkMode(s.offscreen_dc, 1);
      SetTextColor(s.offscreen_dc, g_ladder_shadow_color);
      DrawTextA(s.offscreen_dc, g_ladder_title, -1, &s.client_rect, 0x25);
      OffsetRect(&s.client_rect, -2, -2);
      SetTextColor(s.offscreen_dc, g_ladder_title_color);
      DrawTextA(s.offscreen_dc, g_ladder_title, -1, &s.client_rect, 0x25);
      if ((g_ladder_tournament->deck_builder_thread != NULL) &&
          (WaitForSingleObject(g_ladder_tournament->deck_builder_thread, 0) == WAIT_TIMEOUT))
      {
        if (g_ladder_blink != 0)
        {
          OffsetRect(&s.client_rect, 0, s.text_size.cy);
          SelectObject(s.offscreen_dc, g_ladder_entry_font);
          SetBkMode(s.offscreen_dc, 1);
          SetTextColor(s.offscreen_dc, g_ladder_shadow_color);
          DrawTextA(s.offscreen_dc, g_ladder_decks_building, -1, &s.client_rect, 0x25);
          OffsetRect(&s.client_rect, -1, -1);
          SetTextColor(s.offscreen_dc, g_ladder_selected_color);
          DrawTextA(s.offscreen_dc, g_ladder_decks_building, -1, &s.client_rect, 0x25);
        }
      }
      else
      {
        if ((g_ladder_tournament->opponent_match_thread != 0) &&
            ((WaitForSingleObject(g_ladder_tournament->opponent_match_thread, 0) == 0x102
            && (g_ladder_blink != 0))))
        {
          OffsetRect(&s.client_rect, 0, s.text_size.cy);
          SelectObject(s.offscreen_dc, g_ladder_entry_font);
          SetBkMode(s.offscreen_dc, 1);
          SetTextColor(s.offscreen_dc, g_ladder_shadow_color);
          DrawTextA(s.offscreen_dc, g_ladder_opponents_building, -1, &s.client_rect, 0x25);
          OffsetRect(&s.client_rect, -1, -1);
          SetTextColor(s.offscreen_dc, g_ladder_selected_color);
          DrawTextA(s.offscreen_dc, g_ladder_opponents_building, -1, &s.client_rect, 0x25);
        }
      }
      SelectObject(s.offscreen_dc, g_ladder_entry_font);
      GetTextExtentPoint32A(s.offscreen_dc, "fun", 3, &s.text_size);
      s.text_height = s.text_size.cy;
      SetTextColor(s.offscreen_dc, g_ladder_text_color);
      SetBkMode(s.offscreen_dc, 1);
      GetObjectA(g_ladder_bars, sizeof(BITMAP), &s.art_bitmap);
      s.bar_width = s.art_bitmap.bmWidth;
      s.bar_height = s.art_bitmap.bmHeight / 6;
      GetObjectA(g_ladder_winner, sizeof(BITMAP), &s.art_bitmap);
      s.winner_width = s.art_bitmap.bmWidth;
      s.winner_height = s.art_bitmap.bmHeight / 2;
      s.paint_player_count = min(g_ladder_tournament->player_count, 32);
      s.paint_rounds = g_ladder_tournament->rounds;
      s.paint_rect_index = 0;
      EnterCriticalSection(&g_ladder_tournament->critical_section);
      for (s.paint_round = 1; s.paint_round <= s.paint_rounds; s.paint_round = s.paint_round + 1)
      {
        s.bracket_index = shell_sealed_bracket_round_start(g_ladder_tournament->player_count, s.paint_round, NULL);
        s.source_x = 0;
        s.source_y = (s.paint_round + -1) * s.bar_height;
        s.mask_x = 0;
        s.mask_y = s.bar_height * 5;
        SetTextAlign(s.offscreen_dc, 8);
        for (s.paint_entry = 0; s.paint_entry < s.paint_player_count / 2; s.paint_entry = s.paint_entry + 2)
        {
          s.left_first_index = s.paint_rect_index++;
          CopyRect(&s.first_rect, &g_ladder_entry_rects[s.left_first_index]);
          s.left_second_index = s.paint_rect_index++;
          CopyRect(&s.second_rect, &g_ladder_entry_rects[s.left_second_index]);
          if (RectInRegion(s.update_region, &s.first_rect) != 0)
          {
            DrawMaskedBitmapToRect
            (s.offscreen_dc, (RECT *)&s.first_rect, g_ladder_bars, s.bar_width, s.bar_height, s.source_x,
                s.source_y, s.mask_x, s.mask_y);
          }
          if (RectInRegion(s.update_region, &s.second_rect) != 0)
          {
            DrawMaskedBitmapToRect
            (s.offscreen_dc, (RECT *)&s.second_rect, g_ladder_bars, s.bar_width, s.bar_height, s.source_x,
                s.source_y, s.mask_x, s.mask_y);
          }
          s.text_x = s.first_rect.left + ((s.first_rect.right - s.first_rect.left) * 8) / 100;
          s.entry_width = s.first_rect.right - s.first_rect.left;
          s.first_text_y = s.first_rect.bottom - ((s.first_rect.bottom - s.first_rect.top) - s.text_height) / 2;
          s.second_text_y = s.second_rect.bottom - ((s.first_rect.bottom - s.first_rect.top) - s.text_height) / 2;
          s.paint_player_index = g_ladder_tournament->bracket[s.bracket_index];
          s.bracket_index = s.bracket_index + 1;
          if ((s.paint_player_index != -1) && (g_ladder_tournament->players[s.paint_player_index].deck_built != 0))
          {
            s.paint_player = &g_ladder_tournament->players[s.paint_player_index];
            SetTextColor(s.offscreen_dc, g_ladder_shadow_color);
            TextOutA(s.offscreen_dc, s.text_x, s.first_text_y, s.paint_player->name, strlen(s.paint_player->name));
            if (EqualRect(&s.first_rect, (RECT *)&g_ladder_hover_rect) != 0)
            {
              SetTextColor(s.offscreen_dc, g_ladder_selected_color);
            }
            else
            {
              SetTextColor(s.offscreen_dc, g_ladder_text_color);
            }
            TextOutA(s.offscreen_dc, s.text_x + -1, s.first_text_y + -1, s.paint_player->name,
                strlen(s.paint_player->name));
          }
          if (s.second_rect.right - s.second_rect.left > 0)
          {
            s.paint_player_index = g_ladder_tournament->bracket[s.bracket_index];
            s.bracket_index = s.bracket_index + 1;
            if ((s.paint_player_index != -1) && (g_ladder_tournament->players[s.paint_player_index].deck_built != 0))
            {
              s.paint_player = &g_ladder_tournament->players[s.paint_player_index];
              SetTextColor(s.offscreen_dc, g_ladder_shadow_color);
              TextOutA(s.offscreen_dc, s.text_x + 2, s.second_text_y, s.paint_player->name,
                  strlen(s.paint_player->name));
              if (EqualRect(&s.second_rect, (RECT *)&g_ladder_hover_rect) != 0)
              {
                SetTextColor(s.offscreen_dc, g_ladder_selected_color);
              }
              else
              {
                SetTextColor(s.offscreen_dc, g_ladder_text_color);
              }
              TextOutA(s.offscreen_dc, s.text_x + 1, s.second_text_y + -1, s.paint_player->name,
                  strlen(s.paint_player->name));
            }
          }
        }
        SetTextAlign(s.offscreen_dc, 10);
        for (s.paint_entry = 0; s.paint_entry < s.paint_player_count / 2; s.paint_entry = s.paint_entry + 2)
        {
          s.right_first_index = s.paint_rect_index++;
          CopyRect(&s.first_rect, &g_ladder_entry_rects[s.right_first_index]);
          s.right_second_index = s.paint_rect_index++;
          CopyRect(&s.second_rect, &g_ladder_entry_rects[s.right_second_index]);
          if (RectInRegion(s.update_region, &s.first_rect) != 0)
          {
            DrawMaskedBitmapToRect
            (s.offscreen_dc, (RECT *)&s.first_rect, g_ladder_bars, s.bar_width, s.bar_height, s.source_x,
                s.source_y, s.mask_x, s.mask_y);
          }
          if (RectInRegion(s.update_region, &s.second_rect) != 0)
          {
            DrawMaskedBitmapToRect
            (s.offscreen_dc, (RECT *)&s.second_rect, g_ladder_bars, s.bar_width, s.bar_height, s.source_x,
                s.source_y, s.mask_x, s.mask_y);
          }
          s.first_text_y = s.first_rect.bottom - ((s.first_rect.bottom - s.first_rect.top) - s.text_height) / 2;
          s.second_text_y = s.second_rect.bottom - ((s.first_rect.bottom - s.first_rect.top) - s.text_height) / 2;
          s.text_x = s.first_rect.right - ((s.first_rect.right - s.first_rect.left) * 8) / 100;
          s.entry_width = s.first_rect.right - s.first_rect.left;
          s.paint_player_index = g_ladder_tournament->bracket[s.bracket_index];
          s.bracket_index = s.bracket_index + 1;
          if ((s.paint_player_index != -1) && (g_ladder_tournament->players[s.paint_player_index].deck_built != 0))
          {
            s.paint_player = &g_ladder_tournament->players[s.paint_player_index];
            SetTextColor(s.offscreen_dc, g_ladder_shadow_color);
            TextOutA(s.offscreen_dc, s.text_x + -2, s.first_text_y, s.paint_player->name,
                strlen(s.paint_player->name));
            if (EqualRect(&s.first_rect, (RECT *)&g_ladder_hover_rect) != 0)
            {
              SetTextColor(s.offscreen_dc, g_ladder_selected_color);
            }
            else
            {
              SetTextColor(s.offscreen_dc, g_ladder_text_color);
            }
            TextOutA(s.offscreen_dc, s.text_x + -3, s.first_text_y + -1, s.paint_player->name,
                strlen(s.paint_player->name));
          }
          if (s.second_rect.right - s.second_rect.left > 0)
          {
            s.paint_player_index = g_ladder_tournament->bracket[s.bracket_index];
            s.bracket_index = s.bracket_index + 1;
            if ((s.paint_player_index != -1) && (g_ladder_tournament->players[s.paint_player_index].deck_built != 0))
            {
              s.paint_player = &g_ladder_tournament->players[s.paint_player_index];
              SetTextColor(s.offscreen_dc, g_ladder_shadow_color);
              TextOutA(s.offscreen_dc, s.text_x, s.second_text_y, s.paint_player->name,
                  strlen(s.paint_player->name));
              if (EqualRect(&s.second_rect, (RECT *)&g_ladder_hover_rect) != 0)
              {
                SetTextColor(s.offscreen_dc, g_ladder_selected_color);
              }
              else
              {
                SetTextColor(s.offscreen_dc, g_ladder_text_color);
              }
              TextOutA(s.offscreen_dc, s.text_x + -1, s.second_text_y + -1, s.paint_player->name,
                  strlen(s.paint_player->name));
            }
          }
        }
        s.paint_player_count = s.paint_player_count / 2;
      }
      s.winner_first_index = s.paint_rect_index++;
      CopyRect(&s.first_rect, &g_ladder_entry_rects[s.winner_first_index]);
      s.winner_second_index = s.paint_rect_index++;
      CopyRect(&s.second_rect, &g_ladder_entry_rects[s.winner_second_index]);
      s.source_x = 0;
      s.source_y = 0;
      s.mask_x = 0;
      s.mask_y = s.winner_height;
      DrawMaskedBitmapToRect
      (s.offscreen_dc, (RECT *)&s.first_rect, g_ladder_winner, s.winner_width, s.winner_height, 0, 0, 0,
          s.winner_height)
      ;
      s.text_x = s.first_rect.left + (s.first_rect.right - s.first_rect.left) / 2;
      s.winner_text_y = s.first_rect.bottom - ((s.first_rect.bottom - s.first_rect.top) - s.text_height) / 2;
      s.entry_width = s.first_rect.right - s.first_rect.left;
      SetTextAlign(s.offscreen_dc, 0xe);
      s.paint_player_index = g_ladder_tournament->bracket[s.bracket_index];
      s.bracket_index = s.bracket_index + 1;
      if ((s.paint_player_index != -1) && (g_ladder_tournament->players[s.paint_player_index].deck_built != 0))
      {
        s.paint_player = &g_ladder_tournament->players[s.paint_player_index];
        SetTextColor(s.offscreen_dc, g_ladder_shadow_color);
        TextOutA(s.offscreen_dc, s.text_x, s.winner_text_y, s.paint_player->name, strlen(s.paint_player->name));
        if (EqualRect(&s.first_rect, (RECT *)&g_ladder_hover_rect) != 0)
        {
          SetTextColor(s.offscreen_dc, g_ladder_selected_color);
        }
        else
        {
          SetTextColor(s.offscreen_dc, g_ladder_text_color);
        }
        TextOutA(s.offscreen_dc, s.text_x + -1, s.winner_text_y + -1, s.paint_player->name,
            strlen(s.paint_player->name));
      }
      LeaveCriticalSection(&g_ladder_tournament->critical_section);
    }
    RestoreDC(g_shared_offscreen_dc, s.saved_dc);
    s.offscreen_dc = s.paint_dc;
    ApplyCardArtPaletteToDc(s.offscreen_dc);
    GetClientRect(hwnd, &s.client_rect);
    BitBlt(s.offscreen_dc, 0, 0, s.client_rect.right, s.client_rect.bottom, g_shared_offscreen_dc, 0, 0, 0xcc0020);
    LeaveCriticalSection((LPCRITICAL_SECTION)&g_card_render_lock);
    EndPaint(hwnd, &s.paint);
    DeleteObject(s.update_region);
    return TRUE;

  case WM_ERASEBKGND:

    return TRUE;

  case WM_TIMER:

    if (wparam == 0x1)
    {
      EnterCriticalSection(&g_ladder_tournament->critical_section);
      if (g_ladder_tournament->decks_changed != 0)
      {
        InvalidateRect(hwnd, NULL, 0);
        g_ladder_tournament->decks_changed = 0;
      }
      LeaveCriticalSection(&g_ladder_tournament->critical_section);
      if ((g_ladder_tournament->opponent_match_thread != 0) &&
          (WaitForSingleObject((HANDLE)g_ladder_tournament->opponent_match_thread, 0) == 0))
      {
        CloseHandle((HANDLE)g_ladder_tournament->opponent_match_thread);
        g_ladder_tournament->opponent_match_thread = 0;
        g_ladder_tournament->opponents_in_progress = 0;
        g_ladder_tournament->current_round = g_ladder_tournament->current_round + 1;
        EnableWindow(GetDlgItem(hwnd, 1), 1);
        if (g_ladder_tournament->rounds < g_ladder_tournament->current_round)
        {
          SetDlgItemTextA(hwnd, 1, g_ladder_finished_caption);
          ShowWindow(GetDlgItem(hwnd, 0x692), 0);
        }
        else
        {
          TENTATIVE_savegame(0);
        }
      }
      return TRUE;
    }
    if (wparam == 0x2)
    {
      KillTimer(hwnd, 2);
      prepare_duel_video_mode_transition();
      sound_close();
      g_ladder_tournament->players[0].deck_count = 0;
      shell_edit_sealed_deck(hwnd, &g_ladder_tournament->players[0]);
      init_sound_dll(global_main_hwnd, 0, 0);
      if (0 < g_ladder_tournament->free_lands)
      {
        shell_pick_sealed_free_lands(g_ladder_tournament, hwnd);
      }
      g_ladder_tournament->players[0].deck_built = 1;
      InvalidateRect(hwnd, NULL, 0);
      TENTATIVE_savegame(0);
      return TRUE;
    }
    if (wparam == 0x3)
    {
      g_ladder_blink = (uint)(g_ladder_blink == 0);
      for (s.blink_index = 0; s.blink_index < g_ladder_blink_rect_count; s.blink_index = s.blink_index + 1)
      {
        InvalidateRect(hwnd, &g_ladder_blink_rects[s.blink_index * 0x10 / 0x10], 0);
        UpdateWindow(hwnd);
      }
      return TRUE;
    }
    return FALSE;

  case 0x4c8:

    s.new_focus = (HWND)wparam;
    s.old_focus = (HWND)lparam;
    SendMessageA(hwnd, 0x401, GetDlgCtrlID((HWND)wparam), 0);
    if (s.new_focus != NULL)
    {
      InvalidateRect(s.new_focus, NULL, 1);
    }
    if (s.old_focus != NULL)
    {
      InvalidateRect(s.old_focus, NULL, 1);
    }
    return FALSE;

  case WM_CTLCOLORSTATIC:
  case WM_CTLCOLORBTN:

    s.control_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.control_dc);
    s.colored_control = (HWND)lparam;
    s.control_id = GetDlgCtrlID(s.colored_control);
    if (GetFocus() == s.colored_control)
    {
      SetTextColor((HDC)s.control_dc, g_ladder_focus_color);
    }
    else
    {
      SetTextColor((HDC)s.control_dc, g_ladder_text_color);
    }
    SetBkMode((HDC)s.control_dc, 1);
    s.control_brush = GetStockObject(NULL_BRUSH);
    return (BOOL)s.control_brush;

  case WM_DRAWITEM:

    s.draw_item = (DRAWITEMSTRUCT *)lparam;
    if (GetFocus() == s.draw_item->hwndItem)
    {
      s.button_color = shell_draw_resources.selected_button_color;
    }
    else if ((s.draw_item->itemState & 2) != 0)
    {
      s.button_color = 0x10000c6;
    }
    else
    {
      s.button_color = shell_draw_resources.regular_button_color;
    }
    if (shell_draw_resources.button_face != NULL)
    {
      shell_draw_bitmap_button(s.draw_item, shell_draw_resources.button_face, shell_draw_resources.light_pen,
          shell_draw_resources.dark_pen, s.button_color, 0);
    }
    else
    {
      draw_owner_draw_button_centered(s.draw_item, GetStockObject(GRAY_BRUSH), shell_draw_resources.light_pen,
          shell_draw_resources.dark_pen, s.button_color, 0);
    }
    return TRUE;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, message, (int)wparam, (long)lparam);

  case WM_SIZE:

    GetClientRect(hwnd, &s.resize_rect);
    s.old_font = g_ladder_title_font;
    GetObjectA(g_ladder_title_font, 0x3c, &s.resize_font_description);
    s.resize_font_description.lfHeight = (s.resize_rect.bottom - s.resize_rect.top) / 0x1c;
    if (s.resize_font_description.lfHeight % 2 != 0)
    {
      s.resize_font_description.lfHeight = s.resize_font_description.lfHeight + 1;
    }
    g_ladder_title_font = CreateFontIndirectA(&s.resize_font_description);
    s.new_font = g_ladder_title_font;
    DeleteObject(s.old_font);
    s.old_font = g_ladder_entry_font;
    GetObjectA(g_ladder_entry_font, 0x3c, &s.resize_font_description);
    s.resize_font_description.lfHeight = (s.resize_rect.bottom - s.resize_rect.top) / 0x30;
    if (s.resize_font_description.lfHeight % 2 != 0)
    {
      s.resize_font_description.lfHeight = s.resize_font_description.lfHeight + 1;
    }
    g_ladder_entry_font = CreateFontIndirectA(&s.resize_font_description);
    s.new_font = g_ladder_entry_font;
    DeleteObject(s.old_font);
    s.button_count = 0;
    s.buttons[s.button_count] = GetDlgItem(hwnd, 0x718);
    s.button_count = s.button_count + 1;
    s.buttons[s.button_count] = GetDlgItem(hwnd, 0x693);
    s.button_count = s.button_count + 1;
    s.buttons[s.button_count] = GetDlgItem(hwnd, 0x691);
    s.button_count = s.button_count + 1;
    s.buttons[s.button_count] = GetDlgItem(hwnd, 1);
    s.button_count = s.button_count + 1;
    s.buttons[s.button_count] = GetDlgItem(hwnd, 0x692);
    s.button_count = s.button_count + 1;
    LockWindowUpdate(hwnd);
    s.old_font = (HFONT)SendMessageA(s.buttons[0], 0x31, 0, 0);
    GetObjectA(s.old_font, 0x3c, &s.resize_font_description);
    GetClientRect(hwnd, &s.resize_rect);
    s.resize_font_description.lfHeight = (s.resize_rect.bottom - s.resize_rect.top) / 0x28;
    if (s.resize_font_description.lfHeight % 2 != 0)
    {
      s.resize_font_description.lfHeight = s.resize_font_description.lfHeight + 1;
    }
    s.new_font = CreateFontIndirectA(&s.resize_font_description);
    for (s.button_index = 0; s.button_index < s.button_count; s.button_index = s.button_index + 1)
    {
      SendMessageA(s.buttons[s.button_index], 0x30, (WPARAM)s.new_font, 0);
    }
    shell_layout_sealed_ladder_buttons(hwnd);
    DeleteObject(s.old_font);
    s.resize_rounds = g_ladder_tournament->rounds;
    s.resize_player_count = min(g_ladder_tournament->player_count, 32);
    s.resize_rect_index = 0;
    for (s.resize_round = 1; s.resize_round <= s.resize_rounds; s.resize_round = s.resize_round + 1)
    {
      for (s.resize_entry = 0; s.resize_entry < s.resize_player_count / 2; s.resize_entry = s.resize_entry + 2)
      {
        shell_sealed_ladder_entry_rects(hwnd, g_ladder_entry_font, g_ladder_tournament, s.resize_round,
            s.resize_entry, &g_ladder_entry_rects[s.resize_rect_index],
            &g_ladder_entry_rects[(s.resize_rect_index + 1)]);
        s.resize_rect_index = s.resize_rect_index + 2;
      }
      for (s.resize_entry = 0; s.resize_entry < s.resize_player_count / 2; s.resize_entry = s.resize_entry + 2)
      {
        shell_sealed_ladder_entry_rects(hwnd, g_ladder_entry_font, g_ladder_tournament, s.resize_round,
            s.resize_entry + s.resize_player_count / 2, &g_ladder_entry_rects[s.resize_rect_index],
            &g_ladder_entry_rects[(s.resize_rect_index + 1)]);
        s.resize_rect_index = s.resize_rect_index + 2;
      }
      s.resize_player_count = s.resize_player_count / 2;
    }
    shell_sealed_ladder_entry_rects(hwnd, g_ladder_entry_font, g_ladder_tournament, s.resize_round, 0,
        &g_ladder_entry_rects[s.resize_rect_index], &g_ladder_entry_rects[(s.resize_rect_index + 1)]);
    InvalidateRect(hwnd, NULL, 1);
    LockWindowUpdate(NULL);
    GetWindowRect(hwnd, &s.resize_rect);
    MoveWindow(global_main_hwnd, s.resize_rect.left, s.resize_rect.top,
        s.resize_rect.right - s.resize_rect.left, s.resize_rect.bottom - s.resize_rect.top, 0);
    return TRUE;

  case WM_DISPLAYCHANGE:

    shell_release_sealed_ladder_resources(g_ladder_background, g_ladder_bars, g_ladder_winner,
        g_ladder_button_face, g_ladder_light_pen, g_ladder_dark_pen);
    shell_load_sealed_ladder_resources(&g_ladder_background, &g_ladder_title_color, &g_ladder_shadow_color,
        &g_ladder_text_color, &g_ladder_selected_color, &g_ladder_bars, &g_ladder_winner,
        &g_ladder_button_face, &g_ladder_light_pen, &g_ladder_dark_pen, &g_ladder_button_color,
        &g_ladder_focus_color);
    return FALSE;

    default: return FALSE;

  }
}

// FUNCTION: MAGIC 0x004cad39
static void shell_load_sealed_ladder_resources(HBITMAP *background, COLORREF *title_color,
    COLORREF *shadow_color, COLORREF *text_color, COLORREF *selected_color, HBITMAP *bars, HBITMAP *winner,
    HBITMAP *button_face, HPEN *light_pen, HPEN *dark_pen, COLORREF *button_color, COLORREF *focus_color)
{
  char path[264];
  if (g_display_color_depth == 8)
  {
    sprintf(path, "%s\\WINBK_Ladder.pic", g_exp1_art_path);
    *background = load_pic(path);
    *title_color = 0x10000be;
    *text_color = 0x10000be;
    *shadow_color = 0x1000002;
    *selected_color = 0x1000049;
    sprintf(path, "%s\\LADDER_Bars.pic", g_exp1_art_path);
    *bars = load_pic(path);
    sprintf(path, "%s\\LADDER_Winner.pic", g_exp1_art_path);
    *winner = load_pic(path);
    *light_pen = CreatePen(0, 0, 0x1000040);
    *dark_pen = CreatePen(0, 0, 0x1000004);
    *button_color = 0x100000f;
    *focus_color = 0x10000b9;
  }
  else
  {
    sprintf(path, "%s\\WINBK_Ladder16.bmp", g_exp1_art_path);
    *background = shell_load_bitmap_file(path, NULL, 0);
    *title_color = 0xf6f7f7;
    *text_color = 0xf6f7f7;
    *shadow_color = 0x121212;
    *selected_color = 0x17f1ce;
    sprintf(path, "%s\\LADDER_Bars16.bmp", g_exp1_art_path);
    *bars = shell_load_bitmap_file(path, NULL, 0);
    sprintf(path, "%s\\LADDER_Winner16.bmp", g_exp1_art_path);
    *winner = shell_load_bitmap_file(path, NULL, 0);
    *light_pen = CreatePen(0, 0, 0x71adcf);
    *dark_pen = CreatePen(0, 0, 0x162a4f);
    *button_color = 0x4a3230;
    *focus_color = 0xd0f0f7;
  }
  sprintf(path, "%s\\LADDER_ButtonFace.bmp", g_exp1_art_path);
  *button_face = shell_load_bitmap_file(path, NULL, 0);
  if (*light_pen == 0)
  {
    *light_pen = GetStockObject(6);
  }
  if (*dark_pen == 0)
  {
    *dark_pen = GetStockObject(7);
  }
}

// FUNCTION: MAGIC 0x004caf9c
static void shell_release_sealed_ladder_resources(HBITMAP background, HBITMAP bars, HBITMAP winner,
    HBITMAP button_face, HPEN light_pen, HPEN dark_pen)
{
  if (background != NULL) delete_and_close_object(background);
  if (bars != NULL) delete_and_close_object(bars);
  if (winner != NULL) delete_and_close_object(winner);
  if (button_face != NULL) delete_and_close_object(button_face);
  if (light_pen != NULL) DeleteObject(light_pen);
  if (dark_pen != NULL) DeleteObject(dark_pen);
}
// FUNCTION: MAGIC 0x004cb027

static void shell_layout_sealed_ladder_buttons(HWND hwnd)
{
  struct
  {
    CHAR button_text [100];
    HWND button;
    HDC dc;
    int width;
    int center_x;
    int y;
    int x;
    HGDIOBJ font;
    int middle_width;
    RECT rect;
    SIZE text_size;
    int height;
    int margin;
  } s;

  GetClientRect(hwnd, &s.rect);
  s.rect.left = s.rect.left + 0x14;
  s.rect.right = s.rect.right + -0x14;
  s.rect.top = s.rect.top + 0x14;
  s.rect.bottom = s.rect.bottom + -0x14;
  s.font = (HGDIOBJ)SendDlgItemMessageA(hwnd, 1, 0x31, 0, 0);
  s.dc = GetDC(hwnd);
  SelectObject(s.dc, s.font);
  GetTextExtentPoint32A(s.dc, "fun", 3, &s.text_size);
  s.height = (s.text_size.cy * 3) / 2;
  s.margin = (s.text_size.cy * 3) / 2;
  s.y = s.rect.bottom - s.height;
  s.center_x = s.rect.left + (s.rect.right - s.rect.left) / 2;
  s.x = s.center_x;
  s.button = GetDlgItem(hwnd, 0x693);
  GetWindowTextA(s.button, s.button_text, 100);
  GetTextExtentPoint32A(s.dc, s.button_text, strlen(s.button_text), &s.text_size);
  s.middle_width = s.text_size.cx + s.margin;
  s.width = s.middle_width;
  SetWindowPos(s.button, NULL, s.x - s.width / 2, s.y, s.width, s.height, 4);
  s.x = (s.center_x - s.middle_width / 2) - s.margin;
  s.button = GetDlgItem(hwnd, 0x718);
  GetWindowTextA(s.button, s.button_text, 100);
  GetTextExtentPoint32A(s.dc, s.button_text, strlen(s.button_text), &s.text_size);
  s.width = s.text_size.cx + s.margin;
  SetWindowPos(s.button, NULL, s.x - s.width, s.y, s.width, s.height, 4);
  s.x = s.center_x + s.middle_width / 2 + s.margin;
  s.button = GetDlgItem(hwnd, 0x692);
  GetWindowTextA(s.button, s.button_text, 100);
  GetTextExtentPoint32A(s.dc, s.button_text, strlen(s.button_text), &s.text_size);
  s.width = s.text_size.cx + s.margin;
  SetWindowPos(s.button, NULL, s.x, s.y, s.width, s.height, 4);
  s.x = s.center_x;
  s.y = s.y - (s.height + (s.height * 2) / 3);
  s.button = GetDlgItem(hwnd, 1);
  GetWindowTextA(s.button, s.button_text, 100);
  GetTextExtentPoint32A(s.dc, s.button_text, strlen(s.button_text), &s.text_size);
  s.width = s.text_size.cx + s.margin;
  SetWindowPos(s.button, NULL, s.x - s.width / 2, s.y, s.width, s.height, 4);
  s.y = s.y - (s.height + 10);
  s.button = GetDlgItem(hwnd, 0x691);
  GetWindowTextA(s.button, s.button_text, 100);
  GetTextExtentPoint32A(s.dc, s.button_text, strlen(s.button_text), &s.text_size);
  s.width = s.text_size.cx + s.margin;
  SetWindowPos(s.button, NULL, s.x - s.width / 2, s.y, s.width, s.height, 4);
  ReleaseDC(hwnd, s.dc);
}

// FUNCTION: MAGIC 0x004cb39e

static int shell_sealed_bracket_round_start(int count, int round, int *last)
{
  struct
  {
    int round_index;
    int start;
  } s;

  if ((count <= 0) || (round <= 0))
  {
    return -1;
  }
  s.start = 0;
  for (s.round_index = 1; s.round_index < round; s.round_index = s.round_index + 1)
  {
    s.start += count;
    count = count / 2;
  }
  if (last != NULL)
  {
    *last = s.start + count + -1;
  }
  return s.start;
}

// FUNCTION: MAGIC 0x004cb421

static int shell_sealed_ladder_entry_rects(HWND hwnd, HFONT font, sealed_deck_tournament_t *tournament, int round,
    int entry, LPRECT first, LPRECT second)
{
  struct
  {
    int winner_height;
    int winner_width_padding;
    int round_count;
    int player_count;
    int column_offsets [6];
    HDC dc;
    int entry;
    int row_step;
    int y;
    int text_height;
    int top_margin;
    int x;
    uint has_pair;
    RECT first_rect;
    RECT second_rect;
    int row_scale;
    int entry_width;
    int round_index;
    RECT client_rect;
    SIZE text_size;
    int entry_margin;
    int found;
    int entry_height;
    int row_height;
    int first_y;
    int second_y;
  } s;

  if ((hwnd == NULL) || (tournament == 0))
  {
    return 0;
  }
  GetClientRect(hwnd, &s.client_rect);
  s.dc = GetDC(hwnd);
  SelectObject(s.dc, font);
  GetTextExtentPoint32A(s.dc, "fun", 3, &s.text_size);
  s.entry_height = (s.text_size.cy * 125) / 100;
  s.client_rect.bottom = s.client_rect.bottom - s.entry_height;
  s.round_count = 0;
  s.player_count = min(tournament->player_count / 2, 16);
  while (s.player_count > 0)
  {
    s.round_count = s.round_count + 1;
    s.player_count = s.player_count / 2;
  }
  s.entry_width = (s.client_rect.right - s.client_rect.left) / (s.round_count * 2 + 1);
  s.text_height = s.entry_height;
  s.top_margin = s.entry_height;
  s.player_count = min(tournament->player_count, 32);
  s.row_height = (s.client_rect.bottom - s.client_rect.top) / (s.player_count / 2);
  s.row_scale = 1;
  s.entry_width = ((s.client_rect.right - s.client_rect.left) * 0x10) / 100;
  if (s.round_count == 3)
  {
    s.column_offsets[1] = max(((s.client_rect.right - s.client_rect.left) * 2) / 100, 20);
    s.column_offsets[2] = ((s.client_rect.right - s.client_rect.left) * 0x10) / 100;
    s.column_offsets[3] = ((s.client_rect.right - s.client_rect.left) * 0x1b) / 100;
  }
  else if (s.round_count == 4)
  {
    s.column_offsets[1] = max(((s.client_rect.right - s.client_rect.left) * 2) / 100, 20);
    s.column_offsets[2] = ((s.client_rect.right - s.client_rect.left) * 0xe) / 100;
    s.column_offsets[3] = ((s.client_rect.right - s.client_rect.left) * 0x18) / 100;
    s.column_offsets[4] = ((s.client_rect.right - s.client_rect.left) * 0x20) / 100;
  }
  else
  {
    s.column_offsets[1] = max(((s.client_rect.right - s.client_rect.left) * 2) / 100, 20);
    s.column_offsets[2] = ((s.client_rect.right - s.client_rect.left) * 0xc) / 100;
    s.column_offsets[3] = ((s.client_rect.right - s.client_rect.left) * 0x16) / 100;
    s.column_offsets[4] = ((s.client_rect.right - s.client_rect.left) * 0x1c) / 100;
    s.column_offsets[5] = ((s.client_rect.right - s.client_rect.left) * 0x24) / 100;
  }
  EnterCriticalSection(&tournament->critical_section);
  s.found = 0;
  for (s.round_index = 1; s.round_index <= s.round_count && s.found == 0; ++s.round_index)
  {
    if (s.round_count > s.round_index)
    {
      s.has_pair = 1;
    }
    else
    {
      s.has_pair = 0;
    }

    s.row_step = s.row_scale * s.row_height;
    s.entry_margin = 0;
    s.x = s.column_offsets[s.round_index] + s.client_rect.left;
    s.y = s.client_rect.top + s.row_step / 2 + s.top_margin;
    for (s.entry = 0; s.entry < s.player_count / 2 && s.found == 0; s.entry += 2)
    {
      s.first_y = s.y;
      s.second_y = s.y + s.row_step;
      if ((round == s.round_index) && (entry == s.entry))
      {
        SetRect(&s.first_rect, s.x - s.entry_margin, s.first_y - s.entry_height,
            s.x + s.entry_margin + s.entry_width, s.first_y);
        if (s.has_pair != 0)
        {
          SetRect(&s.second_rect, s.x - s.entry_margin, s.second_y - s.entry_height,
              s.x + s.entry_margin + s.entry_width, s.second_y);
        }
        else
        {
          SetRect(&s.second_rect, 0, 0, 0, 0);
        }
        s.found = 1;
      }
      if (s.has_pair != 0)
      {
        s.y = s.y + s.row_step * 2;
      }
    }
    s.x = s.client_rect.right - s.column_offsets[s.round_index];
    s.y = s.client_rect.top + s.row_step / 2 + s.top_margin;
    for (s.entry = 0; s.entry < s.player_count / 2 && s.found == 0; s.entry += 2)
    {
      s.first_y = s.y;
      s.second_y = s.y + s.row_step;
      if ((round == s.round_index) && (s.entry + s.player_count / 2 == entry))
      {
        SetRect(&s.first_rect, (s.x - s.entry_width) - s.entry_margin, s.first_y - s.entry_height,
            s.x + s.entry_margin
            , s.first_y);
        if (s.has_pair != 0)
        {
          SetRect(&s.second_rect, (s.x - s.entry_width) - s.entry_margin, s.second_y - s.entry_height,
              s.x + s.entry_margin, s.second_y);
        }
        else
        {
          SetRect(&s.second_rect, 0, 0, 0, 0);
        }
        s.found = 1;
      }
      if (s.has_pair != 0)
      {
        s.y = s.y + s.row_step * 2;
      }
    }
    s.player_count = s.player_count / 2;
    s.row_scale = s.row_scale << 1;
  }
  s.x = s.client_rect.left + (s.client_rect.right - s.client_rect.left) / 2;
  s.y = s.client_rect.top + (s.client_rect.bottom - s.client_rect.top) / 2 + s.entry_height * 4;
  if (s.round_count + 1 == round)
  {
    s.winner_height = (s.entry_height * 115) / 100;
    s.winner_width_padding = 10;
    SetRect(&s.first_rect, (s.x - s.entry_width / 2) - s.winner_width_padding / 2, s.y - s.winner_height / 2,
        s.entry_width / 2 + s.winner_width_padding / 2 + s.x, s.entry_height + s.winner_height / 2 + s.y);
    SetRect(&s.second_rect, 0, 0, 0, 0);
    s.found = 1;
  }
  LeaveCriticalSection(&tournament->critical_section);
  ReleaseDC(hwnd, s.dc);
  if (s.found != 0)
  {
    if ((s.round_count == round) &&
        ((s.client_rect.right - s.column_offsets[s.round_count]) - s.entry_width <
        s.column_offsets[s.round_count] + s.client_rect.left + s.entry_width))
    {
      if (entry == 0)
      {
        OffsetRect(&s.first_rect, 0, -s.entry_height);
      }
      else
      {
        OffsetRect(&s.first_rect, 0, s.entry_height);
      }
    }
    if (first != NULL)
    {
      CopyRect(first, &s.first_rect);
    }
    if (second != NULL)
    {
      CopyRect(second, &s.second_rect);
    }
  }
  return s.found;
}

// FUNCTION: MAGIC 0x004cbb4f
static sealed_deck_player_t *shell_sealed_ladder_hit_test(HWND hwnd, RECT *rects,
    sealed_deck_tournament_t *tournament, LONG x, LONG y, RECT *hit_rect)
{
  struct
  {
    int second_rect_index;
    int first_rect_index;
    int player_index;
    int bracket_index;
    int rect_index;
    sealed_deck_player_t *player;
    int entries;
    RECT hit_rect;
    RECT first_rect;
    RECT second_rect;
    int round_start;
    int round;
    int entry;
    int found;
  } s;

  if (hwnd == NULL || tournament == NULL)
    return NULL;
  s.player = NULL;
  SetRectEmpty(&s.hit_rect);
  s.found = 0;
  s.entries = 1 << (byte)tournament->rounds;
  s.rect_index = 0;
  for (s.round = 1; s.round <= tournament->rounds + 1 && s.found == 0; ++s.round)
  {
    for (s.entry = 0; s.entry < s.entries && s.found == 0; s.entry += 2)
    {
      s.first_rect_index = s.rect_index++;
      CopyRect(&s.first_rect, &rects[s.first_rect_index]);
      s.second_rect_index = s.rect_index++;
      CopyRect(&s.second_rect, &rects[s.second_rect_index]);
      if (LADDER_POINT_IN_RECT(&s.first_rect, x, y) != 0)
      {
        s.found = 1;
        s.round_start = shell_sealed_bracket_round_start(tournament->player_count, s.round, NULL);
        s.bracket_index = s.entry + s.round_start;
        CopyRect(&s.hit_rect, &s.first_rect);
      }
      else if (LADDER_POINT_IN_RECT(&s.second_rect, x, y) != 0)
      {
        s.found = 1;
        s.round_start = shell_sealed_bracket_round_start(tournament->player_count, s.round, NULL);
        s.bracket_index = s.entry + s.round_start + 1;
        CopyRect(&s.hit_rect, &s.second_rect);
      }
      if (tournament->rounds == s.round)
        --s.entry;
    }
    s.entries /= 2;
  }
  if (hit_rect != NULL)
    CopyRect(hit_rect, &s.hit_rect);
  if (s.found != 0)
  {
    EnterCriticalSection(&tournament->critical_section);
    s.player_index = tournament->bracket[s.bracket_index];
    LeaveCriticalSection(&tournament->critical_section);
    if (s.player_index == -1)
      s.player = NULL;
    else
      s.player = &tournament->players[s.player_index];
  }
  return s.player;
}

// FUNCTION: MAGIC 0x004cc7bf
static int shell_play_sealed_ladder_round(HWND hwnd, sealed_deck_tournament_t *tournament, int round)
{
  struct
  {
    int rounds;
    int last_entry;
    int match_entry;
    int winner;
    int winner_entry;
    int second_player;
    int round_start;
    int first_player;
    int entry;
    int opponent;
    int result;
    int found;
    int resume;
  } s;

  s.rounds = tournament->rounds;
  if (round < 1 || s.rounds < round)
    return -2;
  s.round_start = shell_sealed_bracket_round_start(1 << (byte)s.rounds, round, &s.last_entry);
  if (tournament->duel_in_progress != 0)
  {
    tournament->duel_in_progress = 0;
    s.resume = 1;
  }
  else
    s.resume = 0;
  s.found = 0;
  for (s.entry = s.round_start; s.entry < s.last_entry && s.found == 0; s.entry += 2)
  {
    EnterCriticalSection(&tournament->critical_section);
    s.first_player = tournament->bracket[s.entry];
    s.second_player = tournament->bracket[s.entry + 1];
    LeaveCriticalSection(&tournament->critical_section);
    if (s.first_player == 0 || s.second_player == 0)
    {
      s.found = 1;
      s.match_entry = s.entry;
    }
  }
  if (s.found != 0)
  {
    if (s.first_player == 0)
      s.opponent = s.second_player;
    else
      s.opponent = s.first_player;
    tournament->duel_in_progress = 1;
    s.result = shell_run_sealed_player_match(s.resume, hwnd, &tournament->players[0],
        &tournament->players[s.opponent], tournament->ante, tournament->minimum_deck_size);
    if (s.result == 1)
      s.winner = 0;
    else if (s.result == 0)
      s.winner = s.opponent;
    else
      s.winner = -1;
    if (s.winner != -1)
    {
      s.winner_entry = shell_sealed_bracket_round_start(1 << (byte)s.rounds, round + 1, NULL);
      s.winner_entry = s.winner_entry + (s.match_entry - s.round_start) / 2;
      EnterCriticalSection(&tournament->critical_section);
      tournament->bracket[s.winner_entry] = s.winner;
      tournament->decks_changed = 1;
      LeaveCriticalSection(&tournament->critical_section);
    }
    tournament->duel_in_progress = 0;
  }
  return s.result;
}

// FUNCTION: MAGIC 0x004cca05
static unsigned int __stdcall shell_run_sealed_opponent_matches(sealed_deck_tournament_t *tournament)
{
  struct
  {
    int rounds;
    int match_entry;
    int second_value;
    int winner;
    int first_value;
    int second_difficulty;
    int first_difficulty;
    int winner_entry;
    int second_player;
    int first_player;
    int round;
  } s;

  s.rounds = tournament->rounds;
  s.round = tournament->current_round;
  if (s.round < 1 || s.round > s.rounds)
    return 0;
  while (tournament->opponent_match_thread != NULL &&
      (s.match_entry = shell_next_sealed_opponent_match(tournament, s.round, &s.winner_entry)) != -1)
  {
    EnterCriticalSection(&tournament->critical_section);
    s.first_player = tournament->bracket[s.match_entry];
    s.second_player = tournament->bracket[s.match_entry + 1];
    LeaveCriticalSection(&tournament->critical_section);
    Sleep(rand() % 4500 + 500);
    s.first_difficulty = tournament->players[s.first_player].difficulty;
    s.first_value = shell_evaluate_sealed_deck(tournament->players[s.first_player].deck,
        tournament->players[s.first_player].deck_count, NULL, NULL);
    s.first_value += ((s.first_value * 20) / 100) * s.first_difficulty;
    s.second_difficulty = tournament->players[s.second_player].difficulty;
    s.second_value = shell_evaluate_sealed_deck(tournament->players[s.second_player].deck,
        tournament->players[s.second_player].deck_count, NULL, NULL);
    s.second_value += ((s.second_value * 20) / 100) * s.second_difficulty;
    if (s.first_value > s.second_value)
      s.winner = s.first_player;
    else
      s.winner = s.second_player;
    EnterCriticalSection(&tournament->critical_section);
    tournament->bracket[s.winner_entry] = s.winner;
    tournament->decks_changed = 1;
    LeaveCriticalSection(&tournament->critical_section);
    MessageBeep(0);
  }
  return 0;
}

// FUNCTION: MAGIC 0x004ccc3c
static int shell_next_sealed_opponent_match(sealed_deck_tournament_t *tournament, int round,
    int *winner_entry)
{
  struct
  {
    int initial_entry;
    int rounds;
    int entry;
    int last_entry;
    int winner;
    int winner_entry;
    int second_player;
    int round_start;
    int first_player;
    int exhausted;
    int found;
  } s;

  s.rounds = tournament->rounds;
  s.round_start = shell_sealed_bracket_round_start(1 << (byte)s.rounds, round, &s.last_entry);
  s.entry = s.round_start + rand() % (s.last_entry - s.round_start + 1);
  if (s.entry % 2 != 0)
  {
    if (s.entry == s.last_entry)
      --s.entry;
    else
      ++s.entry;
  }
  s.initial_entry = s.entry;
  s.exhausted = 0;
  s.found = 0;
  do
  {
    s.winner_entry = shell_sealed_bracket_round_start(1 << (byte)s.rounds, round + 1, NULL);
    s.winner_entry = s.winner_entry + (s.entry - s.round_start) / 2;
    EnterCriticalSection(&tournament->critical_section);
    s.first_player = tournament->bracket[s.entry];
    s.second_player = tournament->bracket[s.entry + 1];
    s.winner = tournament->bracket[s.winner_entry];
    LeaveCriticalSection(&tournament->critical_section);
    if (s.first_player > 0 && s.second_player > 0 && s.winner == -1)
      s.found = 1;
    if (s.found == 0)
    {
      s.entry += 2;
      if (s.entry > s.last_entry)
        s.entry = s.round_start;
      if (s.entry == s.initial_entry)
        s.exhausted = 1;
    }
  } while (s.found == 0 && s.exhausted == 0);
  if (s.exhausted != 0 && s.found == 0)
    return -1;
  else
  {
    if (winner_entry != NULL)
      *winner_entry = s.winner_entry;
    return s.entry;
  }
}

// FUNCTION: MAGIC 0x004cbd95
static void shell_show_sealed_player(HWND hwnd, sealed_deck_player_t *player)
{
  sealed_player_dialog_context_t s;

  s.name = player->name;
  s.description = player->description;
  s.face = NULL;
  s.face_path = player->face_path;
  s.build_thread = NULL;
  s.keep_open_under_cursor = 1;
  DialogBoxParamA(g_app_instance, "Rogue", hwnd, shell_sealed_player_dialog_proc, (LPARAM)&s);
}

// FUNCTION: MAGIC 0x004c8013
static void shell_pick_sealed_free_lands(sealed_deck_tournament_t *tournament, HWND hwnd)
{
  struct
  {
    int picked;
    int available[5];
    char title[152];
    int card_types[5];
    int selection;
    int cancelled;
    RECT rect;
  } s;

  GetClientRect(hwnd, &s.rect);
  g_showlist_smallcard_height = (s.rect.right - s.rect.left) / 5;
  g_showlist_smallcard_width = g_showlist_smallcard_height;
  s.card_types[0] = 0;
  s.card_types[1] = 1;
  s.card_types[2] = 2;
  s.card_types[3] = 3;
  s.card_types[4] = 4;
  s.available[0] = s.available[1] = s.available[2] = s.available[3] = s.available[4] = 1;
  s.cancelled = 0;
  s.picked = 0;
  while (s.cancelled == 0 && s.picked < tournament->free_lands)
  {
    load_text(global_ui_strings_filename, "SEALEDDECK_PICKFREELANDS");
    sprintf(s.title, g_text_lines[0], tournament->free_lands - s.picked);
    g_duel_window_hwnd = hwnd;
    s.selection = show_cardlist(s.card_types, NULL, s.available, 5, s.title, 0, g_text_lines[1]);
    g_duel_window_hwnd = NULL;
    if (s.selection >= 0 && s.selection <= 4)
    {
      tournament->players[0].deck[tournament->players[0].deck_count] =
      CardIDFromType(s.card_types[s.selection]);
      ++tournament->players[0].deck_count;
      tournament->players[0].card_pool[tournament->players[0].card_pool_count] =
      CardIDFromType(s.card_types[s.selection]);
      ++tournament->players[0].card_pool_count;
      ++s.picked;
    }
    else
    {
      s.cancelled = 1;
    }
  }
}
