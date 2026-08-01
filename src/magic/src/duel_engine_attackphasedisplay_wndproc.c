#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "drawcardlib/src/pic.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"

#define ATTACK_PHASE_DISPLAY_SELECTED_PHASE_OFFSET 0
#define ATTACK_PHASE_DISPLAY_COMMAND_HELP 0x64
#define ATTACK_PHASE_DISPLAY_COMMAND_STOP_BASE 0x96
#define ATTACK_PHASE_DISPLAY_COMMAND_TOGGLE_BASE 0xc8
#define ATTACK_PHASE_DISPLAY_COMMAND_HELP_BASE 0xfa
#define ATTACK_PHASE_DISPLAY_BITMAP_WIDTH 0x2f8

#define ATTACK_PHASE_DISPLAY_PIC g_magicgame_attack_phase_display_pic
#define ATTACK_WINDOW_TITLE_HEIGHT DAT_0069c654

#define ATTACK_MAX_CARDS_PER_GROUP 50

typedef struct attack_window_group
{
  int group_card;
  HWND attackers[ATTACK_MAX_CARDS_PER_GROUP];
  int attacker_count;
  HWND blockers[ATTACK_MAX_CARDS_PER_GROUP];
  int blocker_count;
} attack_window_group_t;

typedef struct msvc_bitmap
{
  LONG bmType;
  LONG bmWidth;
  LONG bmHeight;
  LONG bmWidthBytes;
  WORD bmPlanes;
  WORD bmBitsPixel;
  LPVOID bmBits;
} msvc_bitmap_t;

// GLOBAL: MAGIC 0x00571d20
int DAT_00571d20 = -1;

// GLOBAL: MAGIC 0x0069c618
int DAT_0069c618;

// GLOBAL: MAGIC 0x0069c69c
int DAT_0069c69c;

// GLOBAL: MAGIC 0x0069c6ac
int DAT_0069c6ac;

// GLOBAL: MAGIC 0x0069c6b0
int DAT_0069c6b0;

// GLOBAL: MAGIC 0x00708828
// GLOBAL: SHANDALAR 0x00669660
int g_attack_phase_display_click_packet[3];

// GLOBAL: MAGIC 0x00708838
// GLOBAL: SHANDALAR 0x00669670
int g_attack_phase_display_menu_packet[3];

extern HWND DAT_0069c620;
extern char DAT_0069c628[0x1c];
extern HBITMAP DAT_0069c644;
extern int DAT_0069c654;
extern int DAT_0069c6c0;
extern HWND DAT_008a8d78;
extern HWND DAT_008a8dec;
extern HWND DAT_0094ca30;
extern int DAT_00939508;
extern char global_base_directory[];
extern char global_duelart_path[];
extern int g_duel_modal_action_active;
extern int g_battlefield_draw_placeholder_x[2];
extern int g_battlefield_draw_placeholder_y[2];
extern int g_showlist_smallcard_width;
extern int g_showlist_smallcard_height;
extern int g_attackclass_data_window_long_offset;
extern int g_attackclass_count_window_long_offset;

extern char g_phase_display_menu_phase_help_text[0x38];
extern char g_phase_display_menu_toggle_text[0x38];
extern char g_phase_display_menu_help_text[0x1c];
extern HMENU g_phase_display_menu;
extern char g_phase_display_menu_stop_text[0x38];
extern int g_phase_display_window_extra_bytes;

void copy_phase_stop_flags(int *phase_flags, int player);
void get_current_duel_selection(int *selected_player, int *selected_card);
void get_phase_display_action_selection(int *player, int *phase);
void noop_phase_display_user_message(HWND hwnd, WPARAM wparam, LPARAM lparam);
void delete_and_close_object(HANDLE obj);
unsigned int refresh_duel_display_cache(void);
LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LONG get_card_window_hidden_flag(HWND hwnd);
int card_window_matches_player_and_card(HWND hwnd, int *player_and_card);
card_id_t get_card_window_displayed_card_id(HWND hwnd);
int is_attack_phase_window_enabled(void);
void copy_opponent_name_prefix(char *name);
void restack_duel_child_windows(void);

static void hit_test_attack_phase_display(POINT *point, RECT *client_rect, int *phase);
static void get_attack_phase_display_phase_rect(RECT *rect, int phase, int width, int height);
static void draw_attack_phase_display_status_markers(HDC dc, RECT *client_rect);
static int count_attack_phase_hidden_descendants(HWND hwnd, HWND parent_card_hwnd);
static void set_attack_phase_window_title(HWND hwnd);

// FUNCTION: MAGIC 0x004954ef
int FUN_004954ef(char *text, COLORREF color, HBRUSH brush)
{
  struct
  {
    HDC dc;
    RECT rect;
  } s;

  SetRect(&s.rect, 0, 0x23f, 0x8c, 0x26c);
  if (DAT_00571d20 != -1)
  {

    s.dc = CreateDCA("DISPLAY", (LPCSTR)0, (LPCSTR)0, (DEVMODEA *)0);
    SetTextColor(s.dc, color);
    SetBkMode(s.dc, TRANSPARENT);
    FillRect(s.dc, &s.rect, brush);
    TextOutA(s.dc, s.rect.left + 5, s.rect.top + 5, text, strlen(text));
    DeleteDC(s.dc);
    Sleep(DAT_00571d20);
  }
  return 1;
}

// FUNCTION: MAGIC 0x00538854
// FUNCTION: SHANDALAR 0x0055d078
static void hit_test_attack_phase_display(POINT *point, RECT *client_rect, int *phase)
{
  struct
  {
    POINT points[7];
    RECT client_copy;
    RECT phase_rect;
    int found_phase;
    int index;
  } s;

  CopyRect(&s.client_copy, client_rect);
  for (s.index = 0; s.index < 7; s.index++)
    s.points[s.index] = *point;
  s.found_phase = -1;

  get_attack_phase_display_phase_rect(&s.phase_rect, 0x15, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, s.points[0]) != 0)
    s.found_phase = 0x15;
  get_attack_phase_display_phase_rect(&s.phase_rect, 0x16, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, s.points[1]) != 0)
    s.found_phase = 0x16;
  get_attack_phase_display_phase_rect(&s.phase_rect, 0x17, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, s.points[2]) != 0)
    s.found_phase = 0x17;
  get_attack_phase_display_phase_rect(&s.phase_rect, 0x18, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, s.points[3]) != 0)
    s.found_phase = 0x18;
  get_attack_phase_display_phase_rect(&s.phase_rect, 0x19, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, s.points[4]) != 0)
    s.found_phase = 0x19;
  get_attack_phase_display_phase_rect(&s.phase_rect, 0x1b, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, s.points[5]) != 0)
    s.found_phase = 0x1b;
  get_attack_phase_display_phase_rect(&s.phase_rect, 0x1e, s.client_copy.right, s.client_copy.bottom);
  if (PtInRect(&s.phase_rect, s.points[6]) != 0)
    s.found_phase = 0x1e;

  *phase = s.found_phase;
}

// FUNCTION: MAGIC 0x00538a11
// FUNCTION: SHANDALAR 0x0055d235
static void get_attack_phase_display_phase_rect(RECT *rect, int phase, int width, int height)
{
  int top;

  if (phase == -1)
  {
    SetRect(rect, 0, 0, 0, 0);
    return;
  }

  if (phase == 0x15)
    top = (height * 2) / ATTACK_PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0x16)
    top = (height * 0x2b) / ATTACK_PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0x17)
    top = (height * 0x54) / ATTACK_PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0x18)
    top = (height * 0x7d) / ATTACK_PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0x19)
    top = (height * 0xa6) / ATTACK_PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0x1a)
    top = (height * 0xcf) / ATTACK_PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0x1b)
    top = (height * 0xcf) / ATTACK_PHASE_DISPLAY_BITMAP_WIDTH;
  else if (phase == 0x1e)
    top = (height * 0x121) / ATTACK_PHASE_DISPLAY_BITMAP_WIDTH;
  else
    top = -1;

  if (top == -1)
    SetRect(rect, 0, 0, 0, 0);
  else
    SetRect(rect, 0, top, width, (height * 0x28) / ATTACK_PHASE_DISPLAY_BITMAP_WIDTH + top);
}

// FUNCTION: MAGIC 0x00538bd2
// FUNCTION: SHANDALAR 0x0055d3f6
static void draw_attack_phase_display_status_markers(HDC dc, RECT *client_rect)
{
  struct
  {
    int selected_player;
    int selected_phase;
    int current_player;
    int phase;
    int phase_flags[38];
    HBRUSH brush;
    HGDIOBJ old_brush;
    RECT phase_rect;
    RECT marker_rect;
  } s;

  get_phase_display_action_selection(&s.selected_player, &s.selected_phase);
  if (s.selected_player == -1 || s.selected_phase == -1)
  {
    s.brush = CreateSolidBrush(0xff);
    s.old_brush = SelectObject(dc, s.brush);
    get_current_duel_selection(&s.current_player, (int *)0);
    copy_phase_stop_flags(s.phase_flags, s.current_player);
    for (s.phase = 0x15; s.phase < 0x1f; s.phase++)
    {
      if (s.phase_flags[s.phase] != 0)
      {
        get_attack_phase_display_phase_rect(&s.phase_rect, s.phase, client_rect->right, client_rect->bottom);
        if (IsRectEmpty(&s.phase_rect) == 0)
        {
          CopyRect(&s.marker_rect, &s.phase_rect);
          s.marker_rect.left = s.marker_rect.right - (s.phase_rect.right - s.phase_rect.left) / 3;
          s.marker_rect.top = s.marker_rect.bottom -
                              ((s.phase_rect.bottom - s.phase_rect.top) *
                               (s.marker_rect.right - s.marker_rect.left)) /
                                  (s.phase_rect.right - s.phase_rect.left);
          Ellipse(dc, s.marker_rect.left, s.marker_rect.top, s.marker_rect.right, s.marker_rect.bottom);
        }
      }
    }
    SelectObject(dc, s.old_brush);
    DeleteObject(s.brush);
  }

  if (s.selected_player != -1 && s.selected_phase != -1)
  {
    s.brush = CreateSolidBrush(0xff00);
    s.old_brush = SelectObject(dc, s.brush);
    for (s.phase = 0x15; s.phase < 0x1f; s.phase++)
    {
      if (s.selected_phase == s.phase)
      {
        get_attack_phase_display_phase_rect(&s.phase_rect, s.phase, client_rect->right, client_rect->bottom);
        if (IsRectEmpty(&s.phase_rect) == 0)
        {
          CopyRect(&s.marker_rect, &s.phase_rect);
          s.marker_rect.right = s.marker_rect.left + (s.phase_rect.right - s.phase_rect.left) / 3;
          s.marker_rect.top = s.marker_rect.bottom -
                              ((s.phase_rect.bottom - s.phase_rect.top) *
                               (s.marker_rect.right - s.marker_rect.left)) /
                                  (s.phase_rect.right - s.phase_rect.left);
          Ellipse(dc, s.marker_rect.left, s.marker_rect.top, s.marker_rect.right, s.marker_rect.bottom);
        }
      }
    }
    SelectObject(dc, s.old_brush);
    DeleteObject(s.brush);
  }
}

// FUNCTION: MAGIC 0x004d8b29
static int count_attack_phase_hidden_descendants(HWND hwnd, HWND parent_card_hwnd)
{
  attack_window_group_t *groups;
  int group_count;
  int result;
  int group_index;
  int card_index;

  groups = (attack_window_group_t *)GetWindowLongA(hwnd, 0);
  group_count = GetWindowLongA(hwnd, 4);
  result = 0;
  for (group_index = 0; group_index < group_count; group_index++)
  {
    for (card_index = 0; card_index < groups[group_index].attacker_count; card_index++)
    {
      if ((HWND)get_card_window_hidden_flag(groups[group_index].attackers[card_index]) == parent_card_hwnd)
        result += count_attack_phase_hidden_descendants(hwnd, groups[group_index].attackers[card_index]) + 1;
    }
    for (card_index = 0; card_index < groups[group_index].blocker_count; card_index++)
    {
      if ((HWND)get_card_window_hidden_flag(groups[group_index].blockers[card_index]) == parent_card_hwnd)
        result += count_attack_phase_hidden_descendants(hwnd, groups[group_index].blockers[card_index]) + 1;
    }
  }
  return result;
}

// FUNCTION: MAGIC 0x004d8468
static void set_attack_phase_window_title(HWND hwnd)
{
  struct
  {
    char title[100];
    char opponent_name[100];
    int player;
  } s;

  get_current_duel_selection(&s.player, (int *)0);
  if (s.player == 0)
  {
    strcpy(s.title, gs_window_title_your_attack_00926010);
  }
  else
  {
    copy_opponent_name_prefix(s.opponent_name);
    sprintf(s.title, gs_window_title_opponent_attack_009267a0, s.opponent_name);
  }
  SetWindowTextA(hwnd, s.title);
}

// FUNCTION: MAGIC 0x004d8879
// FUNCTION: SHANDALAR 0x004624cf
int find_attack_phase_card_window(HWND hwnd, int *player_and_card, int *unused1, HWND *child_hwnd, int *unused2)
{
  struct
  {
    attack_window_group_t *groups;
    card_id_t found_card_id;
    int group_count;
    int card_index;
    int group_index;
    HWND found_hwnd;
    int found_is_attacker;
    int found;
  } s;

  if (hwnd == (HWND)0 || player_and_card == (int *)0)
  {
    s.found = 0;
  }
  else
  {
    s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
    s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
    s.found = 0;
    for (s.group_index = 0; s.group_index < s.group_count; s.group_index++)
    {
      s.card_index = 0;
      while (s.card_index < s.groups[s.group_index].attacker_count && s.found == 0)
      {
        if (card_window_matches_player_and_card(s.groups[s.group_index].attackers[s.card_index], player_and_card) != 0)
        {
          s.found = 1;
          s.found_card_id = get_card_window_displayed_card_id(s.groups[s.group_index].attackers[s.card_index]);
          s.found_hwnd = s.groups[s.group_index].attackers[s.card_index];
          s.found_is_attacker = 1;
        }
        s.card_index++;
      }
      s.card_index = 0;
      while (s.card_index < s.groups[s.group_index].blocker_count && s.found == 0)
      {
        if (card_window_matches_player_and_card(s.groups[s.group_index].blockers[s.card_index], player_and_card) != 0)
        {
          s.found = 1;
          s.found_card_id = get_card_window_displayed_card_id(s.groups[s.group_index].blockers[s.card_index]);
          s.found_hwnd = s.groups[s.group_index].blockers[s.card_index];
          s.found_is_attacker = 0;
        }
        s.card_index++;
      }
    }
  }

  if (unused1 != (int *)0)
  {
    if (s.found != 0)
      *unused1 = s.found_card_id;
    else
      *unused1 = -1;
  }
  if (child_hwnd != (HWND *)0)
  {
    if (s.found != 0)
      *child_hwnd = s.found_hwnd;
    else
      *child_hwnd = (HWND)0;
  }
  if (unused2 != (int *)0)
  {
    if (s.found != 0)
      *unused2 = s.found_is_attacker;
    else
      *unused2 = 0;
  }
  return s.found;
}

// FUNCTION: MAGIC 0x004d6c6d
// FUNCTION: SHANDALAR 0x004608d2
void layout_attack_phase_window(HWND hwnd)
{
  struct
  {
    DWORD style;
    int width;
    BOOL visible;
    int value;
    int scratch_padding1;
    int scratch_padding2;
    int scratch_padding3;
    int scratch_padding4;
    RECT adjusted_rect;
    RECT battlefield_rect;
    int visible_card_count;
    int layout_card_index;
    int attacker_y;
    int layout_group_index;
    int card_x;
    int blocker_y;
    int attacker_x;
    int blocker_x;
    int visible_attacker_descendants;
    int hidden_count;
    int count_card_index;
    int count_group_index;
    int visible_blocker_descendants;
    msvc_bitmap_t bitmap;
    LONG groups_long;
    int scrollbar_height;
    LONG group_count;
    int current_phase;
    int minimum_rows;
    int visible_attacker_rows;
    RECT content_rect;
    int current_player;
    HWND scrollbar_hwnd;
    int visible_blocker_rows;
    int blocker_spacing;
    int attacker_spacing;
    int scroll_pos;
  } s;

  s.groups_long = GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
  s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
  s.scrollbar_hwnd = GetDlgItem(hwnd, 0);
  s.scroll_pos = SendMessageA(s.scrollbar_hwnd, SBM_GETPOS, 0, 0);
  get_current_duel_selection(&s.current_player, &s.current_phase);
  if (s.current_phase <= 0x14 || s.current_phase >= 0x1e ||
      is_attack_phase_window_enabled() == 0 ||
      (s.current_player == 1 && s.group_count == 0 && (g_duel_network_flags & 2) == 0))
  {
    ShowWindow(DAT_008a8d78, SW_HIDE);
    ShowWindow(DAT_008a8dec, SW_SHOW);
    ShowWindow(hwnd, SW_HIDE);
    ShowWindow(DAT_0069c620, SW_HIDE);
    UpdateWindow(g_duel_help_owner_hwnd);
    SendMessageA(s.scrollbar_hwnd, 0x468, 0, 0);
    SendMessageA(g_duel_attack_phase_window_hwnd, 0x40c, 0, 0);
    SendMessageA(g_duel_player_battlefield_window_hwnd, 0x401, 0, 0);
    SendMessageA(g_duel_help_owner_hwnd, 0x401, 0, 0);
    g_battlefield_draw_placeholder_x[1] = 5;
    g_battlefield_draw_placeholder_x[0] = g_battlefield_draw_placeholder_x[1];
    g_battlefield_draw_placeholder_y[1] = g_showlist_smallcard_height / 2;
    g_battlefield_draw_placeholder_y[0] = g_battlefield_draw_placeholder_y[1];
    return;
  }

  FUN_004954ef("LayoutAttackCards", 0xff00ff, GetStockObject(LTGRAY_BRUSH));
  DAT_0069c654 = 10;
  DAT_0069c6c0 = 10;
  DAT_0069c69c = (g_showlist_smallcard_width * 0xf) / 100;
  DAT_0069c618 = 5;
  s.minimum_rows = 2;
  if (DAT_0069c644 != (HBITMAP)0)
  {
    GetObjectA(DAT_0069c644, sizeof(s.bitmap), &s.bitmap);
    if (GetSystemMetrics(SM_CYHSCROLL) * 2 < s.bitmap.bmHeight / 2)
      s.scrollbar_height = s.bitmap.bmHeight / 2;
    else
      s.scrollbar_height = GetSystemMetrics(SM_CYHSCROLL) * 2;
  }
  else
  {
    s.scrollbar_height = GetSystemMetrics(SM_CYHSCROLL) * 2;
  }

#define ATTACK_GROUPS ((attack_window_group_t *)s.groups_long)
  s.visible_attacker_descendants = 0;
  s.visible_blocker_descendants = 0;
  for (s.count_group_index = 0; s.count_group_index < s.group_count; s.count_group_index++)
  {
    for (s.count_card_index = 0;
         s.count_card_index < ATTACK_GROUPS[s.count_group_index].attacker_count;
         s.count_card_index++)
    {
      if (get_card_window_hidden_flag(ATTACK_GROUPS[s.count_group_index].attackers[s.count_card_index]) == 0)
      {
        s.hidden_count = count_attack_phase_hidden_descendants(hwnd,
                                                               ATTACK_GROUPS[s.count_group_index].attackers[s.count_card_index]);
        if (s.hidden_count > s.visible_attacker_descendants)
          s.visible_attacker_descendants = s.hidden_count;
      }
    }
    for (s.count_card_index = 0;
         s.count_card_index < ATTACK_GROUPS[s.count_group_index].blocker_count;
         s.count_card_index++)
    {
      if (get_card_window_hidden_flag(ATTACK_GROUPS[s.count_group_index].blockers[s.count_card_index]) == 0)
      {
        s.hidden_count = count_attack_phase_hidden_descendants(hwnd,
                                                               ATTACK_GROUPS[s.count_group_index].blockers[s.count_card_index]);
        if (s.hidden_count > s.visible_blocker_descendants)
          s.visible_blocker_descendants = s.hidden_count;
      }
    }
  }

  if (s.current_player == 0)
  {
    s.visible_attacker_rows = s.visible_attacker_descendants;
    s.visible_blocker_rows = s.visible_blocker_descendants;
  }
  else
  {
    s.visible_blocker_rows = s.visible_attacker_descendants;
    s.visible_attacker_rows = s.visible_blocker_descendants;
  }

  DAT_0069c6ac = (s.visible_blocker_rows <= s.minimum_rows ? s.minimum_rows : s.visible_blocker_rows) *
                     DAT_00939508 +
                 DAT_0069c6c0;

  DAT_0069c6b0 = (s.visible_attacker_rows <= s.minimum_rows ? s.minimum_rows : s.visible_attacker_rows) *
                     DAT_00939508 +
                 g_showlist_smallcard_height +
                 DAT_0069c6ac + s.scrollbar_height + DAT_0069c618 + DAT_0069c618;

  s.attacker_spacing = g_showlist_smallcard_width / 3;
  s.blocker_spacing = g_showlist_smallcard_width / 3;
  SetWindowPos(s.scrollbar_hwnd, (HWND)0, 0, g_showlist_smallcard_height + DAT_0069c6ac + DAT_0069c618,
               0, 0, SWP_NOSIZE | SWP_NOZORDER);

  s.card_x = ATTACK_WINDOW_TITLE_HEIGHT - s.scroll_pos;
  s.content_rect.left = s.card_x;
  s.content_rect.right = s.card_x;
  s.content_rect.top = DAT_0069c6ac -
                       (s.visible_blocker_rows <= s.minimum_rows ? s.minimum_rows : s.visible_blocker_rows) *
                           DAT_00939508;
  s.content_rect.bottom = g_showlist_smallcard_height + DAT_0069c6b0;
  if (s.current_player == 0)
  {
    s.attacker_y = DAT_0069c6b0;
    s.blocker_y = DAT_0069c6ac;
  }
  else
  {
    s.attacker_y = DAT_0069c6ac;
    s.blocker_y = DAT_0069c6b0;
  }

  for (s.layout_group_index = 0; s.layout_group_index < s.group_count; s.layout_group_index++)
  {
    s.blocker_x = s.card_x;
    s.attacker_x = s.card_x;
    s.visible_card_count = 0;
    for (s.layout_card_index = 0;
         s.layout_card_index < ATTACK_GROUPS[s.layout_group_index].attacker_count;
         s.layout_card_index++)
    {
      if (get_card_window_hidden_flag(ATTACK_GROUPS[s.layout_group_index].attackers[s.layout_card_index]) == 0)
      {
        s.attacker_x = s.attacker_spacing * s.visible_card_count + s.card_x;
        MoveWindow(ATTACK_GROUPS[s.layout_group_index].attackers[s.layout_card_index], s.attacker_x, s.attacker_y,
                   g_showlist_smallcard_width, g_showlist_smallcard_height, 1);
        s.visible_card_count++;
        if (s.content_rect.right < s.attacker_x + g_showlist_smallcard_width)
          s.content_rect.right = s.attacker_x + g_showlist_smallcard_width;
      }
    }
    s.visible_card_count = 0;
    for (s.layout_card_index = 0;
         s.layout_card_index < ATTACK_GROUPS[s.layout_group_index].blocker_count;
         s.layout_card_index++)
    {
      if (get_card_window_hidden_flag(ATTACK_GROUPS[s.layout_group_index].blockers[s.layout_card_index]) == 0)
      {
        s.blocker_x = s.blocker_spacing * s.visible_card_count + s.card_x;
        MoveWindow(ATTACK_GROUPS[s.layout_group_index].blockers[s.layout_card_index], s.blocker_x, s.blocker_y,
                   g_showlist_smallcard_width, g_showlist_smallcard_height, 1);
        s.visible_card_count++;
        if (s.content_rect.right < s.blocker_x + g_showlist_smallcard_width)
          s.content_rect.right = s.blocker_x + g_showlist_smallcard_width;
      }
    }
    s.card_x = (s.blocker_x <= s.attacker_x ? s.attacker_x : s.blocker_x) +
               g_showlist_smallcard_width + DAT_0069c69c;
  }
#undef ATTACK_GROUPS

  s.scratch_padding3 = g_showlist_smallcard_width / 3;
  s.content_rect.left -= ATTACK_WINDOW_TITLE_HEIGHT;
  s.content_rect.right += ATTACK_WINDOW_TITLE_HEIGHT;
  s.content_rect.top -= DAT_0069c6c0;
  s.content_rect.bottom += DAT_0069c6c0;
  s.width = g_showlist_smallcard_width + ATTACK_WINDOW_TITLE_HEIGHT * 2;
  if (s.content_rect.right - s.content_rect.left < s.width)
    s.content_rect.right = s.content_rect.left + s.width;

  s.style = GetWindowLongA(hwnd, GWL_STYLE);
  CopyRect(&s.adjusted_rect, &s.content_rect);
  AdjustWindowRect(&s.adjusted_rect, s.style, 0);
  GetWindowRect(g_duel_player_battlefield_window_hwnd, &s.battlefield_rect);
  s.width = s.battlefield_rect.top - (s.adjusted_rect.bottom - s.adjusted_rect.top);
  if (s.width < 6)
    s.width = 5;
  s.hidden_count = (s.battlefield_rect.right - s.battlefield_rect.left) - s.scratch_padding3;
  if (s.adjusted_rect.right - s.adjusted_rect.left <= s.hidden_count)
    s.hidden_count = s.adjusted_rect.right - s.adjusted_rect.left;

  MoveWindow(DAT_0094ca30, s.battlefield_rect.left, s.width, s.scratch_padding3,
             s.adjusted_rect.bottom - s.adjusted_rect.top, 1);
  MoveWindow(hwnd, s.scratch_padding3 + s.battlefield_rect.left, s.width, s.hidden_count,
             s.adjusted_rect.bottom - s.adjusted_rect.top, 1);
  GetClientRect(hwnd, &s.adjusted_rect);
  SetWindowPos(s.scrollbar_hwnd, (HWND)0, 0, 0, s.adjusted_rect.right, s.scrollbar_height,
               SWP_NOMOVE | SWP_NOZORDER);
  UpdateWindow(g_duel_help_owner_hwnd);
  UpdateWindow(g_duel_player_battlefield_window_hwnd);
  GetClientRect(hwnd, &s.adjusted_rect);
  if (s.adjusted_rect.right < s.content_rect.right - s.content_rect.left)
  {
    s.value = (s.content_rect.right - s.content_rect.left) - s.adjusted_rect.right;
    SendMessageA(s.scrollbar_hwnd, SBM_SETRANGE, 0, s.value);
    SendMessageA(s.scrollbar_hwnd, 0x468, 1, 0);
    if (s.scroll_pos < 0)
      s.scroll_pos = 0;
    if (s.value < s.scroll_pos)
      s.scroll_pos = s.value;
    SendMessageA(hwnd, WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, s.scroll_pos), (LPARAM)s.scrollbar_hwnd);
  }
  else
  {
    SendMessageA(s.scrollbar_hwnd, 0x468, 0, 0);
    s.scroll_pos = 0;
    SendMessageA(hwnd, WM_HSCROLL, SB_THUMBPOSITION, (LPARAM)s.scrollbar_hwnd);
  }

  if (IsWindowVisible(hwnd) == 0 && IsWindowVisible(DAT_0069c620) == 0)
  {
    set_attack_phase_window_title(hwnd);
    ShowWindow(DAT_008a8d78, SW_SHOW);
    ShowWindow(DAT_008a8dec, SW_HIDE);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(DAT_0094ca30);
    UpdateWindow(hwnd);
    UpdateWindow(DAT_008a8d78);
    restack_duel_child_windows();
  }
  UpdateWindow(hwnd);
  UpdateWindow(g_duel_help_owner_hwnd);
}

// FUNCTION: MAGIC 0x0053582a
// FUNCTION: SHANDALAR 0x0055a06a
int register_MAGICGAME_AttackPhaseDisplayClass(LPCSTR class_name)
{
  struct
  {
    char pic_path[264];
    int registered;
    WNDCLASSA wndclass;
  } s;

  s.registered = 1;
  SET_DUEL_WNDCLASS(s.wndclass, 0xb, wndproc_MAGICGAME_AttackPhaseDisplayClass, g_phase_display_window_extra_bytes,
                    LoadIconA((HINSTANCE)0, (LPCSTR)0x7f00), (HBRUSH)0x6, class_name);
  if (RegisterClassA(&s.wndclass) == 0)
    s.registered = 0;
  strcpy(s.pic_path, global_duelart_path);
  strcat(s.pic_path, "\\WINBK_PhaseCombat.pic");
  ATTACK_PHASE_DISPLAY_PIC = load_pic(s.pic_path);
  return s.registered;
}

// FUNCTION: MAGIC 0x005358fb
// FUNCTION: SHANDALAR 0x0055a13b
void destroy_MAGICGAME_AttackPhaseDisplayClass(LPCSTR class_name)
{
  (void)class_name;
  if (ATTACK_PHASE_DISPLAY_PIC != (HBITMAP)0)
    delete_and_close_object(ATTACK_PHASE_DISPLAY_PIC);

  ATTACK_PHASE_DISPLAY_PIC = (HANDLE)0;
}

// FUNCTION: MAGIC 0x00537760
// FUNCTION: SHANDALAR 0x0055bf91
LRESULT CALLBACK wndproc_MAGICGAME_AttackPhaseDisplayClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int phase_index;
    int selected_phase;
    int menu_count;
    POINT menu_point;
    unsigned int phase_help_command;
    int menu_phase_index;
    unsigned int stop_command;
    unsigned int toggle_command;
    RECT menu_client_rect;
    int menu_player;
    POINT popup_point;
    RECT popup_rect;
    char pic_path[264];
    HRGN paint_clip_region;
    HDC paint_dc;
    int paint_selected_phase;
    msvc_bitmap_t paint_bitmap;
    PAINTSTRUCT paint;
    int paint_saved_dc;
    RECT paint_phase_rect;
    int paint_player;
    RECT paint_client_rect;
    int click_player;
    MSG peek_message;
    int peek_result;
    POINT click_point;
    RECT click_client_rect;
    int click_phase;
    char help_path[264];
    unsigned int help_context;
    int toggle_phase;
    int toggle_player;
    RECT toggle_client_rect;
    RECT toggle_phase_rect;
    int stop_phase_code;
    int command_phase;
    int command_base;
    int command_index;
    int stop_player;
    char general_help_path[264];
    unsigned int general_help_context;
    int refresh_phase;
    char cue_text[100];
    int cue_phase;
    POINT cue_point;
    int cue_valid;
    RECT cue_client_rect;
    int cached_phase;
    int phase_flags[38];
  } s;
  unsigned int command;

  switch (msg)
  {
  case 0x437:
    s.cue_point.x = lparam & 0xffff;
    s.cue_point.y = HIWORD(lparam);
    GetClientRect(hwnd, &s.cue_client_rect);
    hit_test_attack_phase_display(&s.cue_point, &s.cue_client_rect, &s.cue_phase);
    s.cue_valid = 1;
    if (s.cue_phase == 0x15)
      strcpy(s.cue_text, gs_phasebar_choose_attackers_008966e0);
    else if (s.cue_phase == 0x16)
      strcpy(s.cue_text, gs_phasebar_attacker_fast_effects_0093a7c0);
    else if (s.cue_phase == 0x17)
      strcpy(s.cue_text, gs_phasebar_assign_defenders_00925a90);
    else if (s.cue_phase == 0x18)
      strcpy(s.cue_text, gs_phasebar_blocker_fast_effects_00925bc0);
    else if (s.cue_phase == 0x19)
      strcpy(s.cue_text, gs_phasebar_resolve_first_strike_damage_0091d220);
    else if (s.cue_phase == 0x1a || s.cue_phase == 0x1b)
      strcpy(s.cue_text, gs_phasebar_resolve_normal_damage_008cec80);
    else if (s.cue_phase == 0x1e)
      strcpy(s.cue_text, gs_phasebar_main_phase_postcombat_00789870);
    else
      s.cue_valid = 0;
    if (s.cue_valid == 0)
      return 0;
    strcpy((char *)wparam, s.cue_text);
    return s.cue_valid;

  case 0x432:
    s.cached_phase = GetWindowLongA(hwnd, ATTACK_PHASE_DISPLAY_SELECTED_PHASE_OFFSET);
    get_current_duel_selection((int *)0, &s.refresh_phase);
    if (s.cached_phase != s.refresh_phase)
      InvalidateRect(hwnd, (RECT *)0, 0);
    return 0;

  case 0x400:
    noop_phase_display_user_message(hwnd, wparam, lparam);
    return 0;

  case WM_COMMAND:
    if (((unsigned int)wparam & 0xffff) == ATTACK_PHASE_DISPLAY_COMMAND_HELP)
    {
      s.general_help_context = 0x7e5;
      strcpy(s.general_help_path, global_base_directory);
      strcat(s.general_help_path, "\\duel.hlp");
      WinHelpA(g_duel_window_hwnd, s.general_help_path, HELP_CONTEXT, s.general_help_context);
    }
    else
    {
      command = (unsigned int)wparam & 0xffff;
      if (command < ATTACK_PHASE_DISPLAY_COMMAND_HELP_BASE)
      {
        if (command < ATTACK_PHASE_DISPLAY_COMMAND_TOGGLE_BASE)
          s.command_base = ATTACK_PHASE_DISPLAY_COMMAND_STOP_BASE;
        else
          s.command_base = ATTACK_PHASE_DISPLAY_COMMAND_TOGGLE_BASE;
      }
      else
      {
        s.command_base = ATTACK_PHASE_DISPLAY_COMMAND_HELP_BASE;
      }
      s.command_phase = command - s.command_base;
      s.command_index = s.command_phase;

      if (s.command_base == ATTACK_PHASE_DISPLAY_COMMAND_STOP_BASE)
      {
        if ((g_duel_network_flags & 2) == 0)
        {
          if (s.command_phase == 0)
            s.stop_phase_code = 0x15;
          else if (s.command_phase == 1)
            s.stop_phase_code = 0x16;
          else if (s.command_phase == 2)
            s.stop_phase_code = 0x17;
          else if (s.command_phase == 3)
            s.stop_phase_code = 0x18;
          else if (s.command_phase == 4)
            s.stop_phase_code = 0x19;
          else if (s.command_phase == 5)
            s.stop_phase_code = 0x1b;
          else if (s.command_phase == 6)
            s.stop_phase_code = 0x1e;
          get_current_duel_selection(&s.stop_player, (int *)0);
          stop_phase_player = s.stop_player;
          stop_phase = s.stop_phase_code;
          unk_00715fb0 = 0;
          g_attack_phase_display_menu_packet[0] = -2;
          g_attack_phase_display_menu_packet[1] = -1;
          g_attack_phase_display_menu_packet[2] = -1;
          PostMessageA(g_duel_window_hwnd, 0x464, 0, (LPARAM)g_attack_phase_display_menu_packet);
        }
      }
      else if (s.command_base == ATTACK_PHASE_DISPLAY_COMMAND_TOGGLE_BASE)
      {
        if (s.command_phase == 0)
          s.toggle_phase = 0x15;
        else if (s.command_phase == 1)
          s.toggle_phase = 0x16;
        else if (s.command_phase == 2)
          s.toggle_phase = 0x17;
        else if (s.command_phase == 3)
          s.toggle_phase = 0x18;
        else if (s.command_phase == 4)
          s.toggle_phase = 0x19;
        else if (s.command_phase == 5)
          s.toggle_phase = 0x1b;
        else if (s.command_phase == 6)
          s.toggle_phase = 0x1e;
        get_current_duel_selection(&s.toggle_player, (int *)0);
        if ((g_duel_phase_stop_settings[s.toggle_player].phase_flags[s.toggle_phase] & PHASE_STOP_ENABLED) == 0)
          g_duel_phase_stop_settings[s.toggle_player].phase_flags[s.toggle_phase] =
              (unsigned char)(g_duel_phase_stop_settings[s.toggle_player].phase_flags[s.toggle_phase] | PHASE_STOP_ENABLED);
        else
          g_duel_phase_stop_settings[s.toggle_player].phase_flags[s.toggle_phase] =
              (unsigned char)(g_duel_phase_stop_settings[s.toggle_player].phase_flags[s.toggle_phase] & ~PHASE_STOP_ENABLED);
        refresh_duel_display_cache();
        GetClientRect(hwnd, &s.toggle_client_rect);
        get_attack_phase_display_phase_rect(&s.toggle_phase_rect, s.toggle_phase,
                                            s.toggle_client_rect.right, s.toggle_client_rect.bottom);
        InvalidateRect(hwnd, &s.toggle_phase_rect, 0);
      }
      else if (s.command_base == ATTACK_PHASE_DISPLAY_COMMAND_HELP_BASE)
      {
        if (s.command_phase == 0)
          s.help_context = 0x7dd;
        else if (s.command_phase == 1)
          s.help_context = 0x7dd;
        else if (s.command_phase == 2)
          s.help_context = 0x7dd;
        else if (s.command_phase == 3)
          s.help_context = 0x7dd;
        else if (s.command_phase == 4)
          s.help_context = 0x7dd;
        else if (s.command_phase == 5)
          s.help_context = 0x7dd;
        else if (s.command_phase == 6)
          s.help_context = 0x7dd;
        strcpy(s.help_path, global_base_directory);
        strcat(s.help_path, "\\duel.hlp");
        WinHelpA(g_duel_window_hwnd, s.help_path, HELP_CONTEXT, s.help_context);
      }
    }
    return 0;

  case WM_CREATE:
    s.cached_phase = 0;
    SetWindowLongA(hwnd, ATTACK_PHASE_DISPLAY_SELECTED_PHASE_OFFSET, s.cached_phase);
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case WM_LBUTTONDOWN:
    Sleep(GetDoubleClickTime());
    s.peek_result = PeekMessageA(&s.peek_message, hwnd, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, 0);
    s.click_point.x = lparam & 0xffff;
    s.click_point.y = HIWORD(lparam);
    GetClientRect(hwnd, &s.click_client_rect);
    hit_test_attack_phase_display(&s.click_point, &s.click_client_rect, &s.click_phase);
    if ((g_duel_network_flags & 2) == 0 && s.click_phase != -1 && g_duel_modal_action_active != 0)
    {
      get_current_duel_selection(&s.click_player, (int *)0);
      stop_phase_player = s.click_player;
      stop_phase = s.click_phase;
      unk_00715fb0 = s.peek_result;
      g_attack_phase_display_click_packet[0] = -2;
      g_attack_phase_display_click_packet[1] = -1;
      g_attack_phase_display_click_packet[2] = -1;
      PostMessageA(g_duel_window_hwnd, 0x464, 0, (LPARAM)g_attack_phase_display_click_packet);
    }
    return 0;

  case WM_PAINT:
    s.cached_phase = GetWindowLongA(hwnd, ATTACK_PHASE_DISPLAY_SELECTED_PHASE_OFFSET);
    get_current_duel_selection(&s.paint_player, &s.paint_selected_phase);
    if (ATTACK_PHASE_DISPLAY_PIC == (HBITMAP)0)
    {
      strcpy(s.pic_path, global_duelart_path);
      strcat(s.pic_path, "\\WINBK_PhaseCombat.pic");
      ATTACK_PHASE_DISPLAY_PIC = load_pic(s.pic_path);
    }
    if (s.paint_selected_phase != s.cached_phase)
      InvalidateRect(hwnd, (RECT *)0, 0);

    EnterCriticalSection(&g_card_render_lock);
    s.paint_dc = g_shared_offscreen_dc;
    s.paint_saved_dc = SaveDC(g_shared_offscreen_dc);
    GetClientRect(hwnd, &s.paint_client_rect);
    get_current_duel_selection(&s.paint_player, (int *)0);
    if (ATTACK_PHASE_DISPLAY_PIC == (HBITMAP)0)
    {
      FillRect(s.paint_dc, &s.paint_client_rect, GetStockObject(GRAY_BRUSH));
    }
    else
    {
      GetObjectA(ATTACK_PHASE_DISPLAY_PIC, sizeof(s.paint_bitmap), &s.paint_bitmap);
      if (s.paint_player == 1)
        s.phase_index = 0;
      else
        s.phase_index = s.paint_bitmap.bmWidth / 2;
      DrawBitmapSubrectToRect(s.paint_dc, &s.paint_client_rect, ATTACK_PHASE_DISPLAY_PIC,
                              s.phase_index, 0,
                              (s.paint_bitmap.bmWidth + (s.paint_bitmap.bmWidth >> 31 & 3)) >> 2,
                              s.paint_bitmap.bmHeight);
    }
    if (s.paint_selected_phase != -1)
    {
      GetClientRect(hwnd, &s.paint_client_rect);
      get_attack_phase_display_phase_rect(&s.paint_phase_rect, s.paint_selected_phase,
                                          s.paint_client_rect.right, s.paint_client_rect.bottom);
      s.paint_clip_region = CreateRectRgnIndirect(&s.paint_phase_rect);
      SelectClipRgn(s.paint_dc, s.paint_clip_region);
      if (ATTACK_PHASE_DISPLAY_PIC == (HBITMAP)0)
      {
        FillRect(s.paint_dc, &s.paint_client_rect, GetStockObject(BLACK_BRUSH));
      }
      else
      {
        GetObjectA(ATTACK_PHASE_DISPLAY_PIC, sizeof(s.paint_bitmap), &s.paint_bitmap);
        if (s.paint_player == 1)
          s.phase_index = s.paint_bitmap.bmWidth + (s.paint_bitmap.bmWidth >> 31 & 3);
        else
          s.phase_index = s.paint_bitmap.bmWidth * 3 + (s.paint_bitmap.bmWidth * 3 >> 31 & 3);
        s.phase_index >>= 2;
        DrawBitmapSubrectToRect(s.paint_dc, &s.paint_client_rect, ATTACK_PHASE_DISPLAY_PIC,
                                s.phase_index, 0,
                                (s.paint_bitmap.bmWidth + (s.paint_bitmap.bmWidth >> 31 & 3)) >> 2,
                                s.paint_bitmap.bmHeight);
      }
      SelectClipRgn(s.paint_dc, (HRGN)0);
      DeleteObject(s.paint_clip_region);
    }

    RestoreDC(g_shared_offscreen_dc, s.paint_saved_dc);
    draw_attack_phase_display_status_markers(s.paint_dc, &s.paint_client_rect);
    s.paint_dc = BeginPaint(hwnd, &s.paint);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetClientRect(hwnd, &s.paint_client_rect);
      BitBlt(s.paint_dc, 0, 0, s.paint_client_rect.right, s.paint_client_rect.bottom,
             g_shared_offscreen_dc, 0, 0, SRCCOPY);
      EndPaint(hwnd, &s.paint);
      s.cached_phase = s.paint_selected_phase;
      SetWindowLongA(hwnd, ATTACK_PHASE_DISPLAY_SELECTED_PHASE_OFFSET, s.paint_selected_phase);
    }
    LeaveCriticalSection(&g_card_render_lock);
    return 0;

  case WM_RBUTTONDOWN:
    s.popup_point.x = lparam & 0xffff;
    s.popup_point.y = HIWORD(lparam);
    if (g_duel_modal_action_active != 0)
    {
      ClientToScreen(hwnd, &s.popup_point);
      SetRect(&s.popup_rect, s.popup_point.x, s.popup_point.y,
              s.popup_point.x + 1, s.popup_point.y + 1);
      TrackPopupMenu(g_phase_display_menu, TPM_RIGHTBUTTON, s.popup_point.x, s.popup_point.y,
                     0, hwnd, &s.popup_rect);
    }
    return 0;

  case WM_INITMENU:
    GetCursorPos(&s.menu_point);
    ScreenToClient(hwnd, &s.menu_point);
    GetClientRect(hwnd, &s.menu_client_rect);
    hit_test_attack_phase_display(&s.menu_point, &s.menu_client_rect, &s.selected_phase);
    get_current_duel_selection(&s.menu_player, (int *)0);
    if (s.selected_phase == 0x15)
      s.menu_phase_index = 0;
    else if (s.selected_phase == 0x16)
      s.menu_phase_index = 1;
    else if (s.selected_phase == 0x17)
      s.menu_phase_index = 2;
    else if (s.selected_phase == 0x18)
      s.menu_phase_index = 3;
    else if (s.selected_phase == 0x19)
      s.menu_phase_index = 4;
    else if (s.selected_phase == 0x1b)
      s.menu_phase_index = 5;
    else if (s.selected_phase == 0x1e)
      s.menu_phase_index = 6;
    else
      s.menu_phase_index = -1;
    if (s.selected_phase != -1)
    {
      if (g_duel_modal_action_active != 0)
      {
        s.stop_command = s.menu_phase_index + ATTACK_PHASE_DISPLAY_COMMAND_STOP_BASE;
        if ((g_duel_network_flags & 2) == 0)
          AppendMenuA(g_phase_display_menu, 0, s.stop_command, g_phase_display_menu_stop_text);
      }
      if (GetMenuItemCount(g_phase_display_menu) != 0)
        AppendMenuA(g_phase_display_menu, MF_SEPARATOR, 0, (LPCSTR)0);
      s.toggle_command = s.menu_phase_index + ATTACK_PHASE_DISPLAY_COMMAND_TOGGLE_BASE;
      AppendMenuA(g_phase_display_menu, 0, s.toggle_command, g_phase_display_menu_toggle_text);
      copy_phase_stop_flags(s.phase_flags, s.menu_player);
      if (s.phase_flags[s.selected_phase] != 0)
        CheckMenuItem(g_phase_display_menu, s.toggle_command, MF_CHECKED);
      s.phase_help_command = s.menu_phase_index + ATTACK_PHASE_DISPLAY_COMMAND_HELP_BASE;
      AppendMenuA(g_phase_display_menu, 0, s.phase_help_command, g_phase_display_menu_phase_help_text);
    }
    AppendMenuA(g_phase_display_menu, 0, ATTACK_PHASE_DISPLAY_COMMAND_HELP, g_phase_display_menu_help_text);
    return 0;

  case WM_MENUSELECT:
    if (HIWORD(wparam) == 0xffff && lparam == 0)
    {
      s.menu_count = GetMenuItemCount(g_phase_display_menu);
      while (s.menu_count != 0)
      {
        DeleteMenu(g_phase_display_menu, 0, MF_BYPOSITION);
        s.menu_count--;
      }
    }
    return 0;

  case WM_SETCURSOR:
    return handle_duel_inactive_cursor(hwnd, WM_SETCURSOR, wparam, lparam);

  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (int)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}
