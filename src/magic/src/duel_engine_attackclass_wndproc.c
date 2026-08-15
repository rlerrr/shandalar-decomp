#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "cardartlib/src/palette.h"
#include "drawcardlib/Drawcardlib.h"
#include "drawcardlib/src/pic.h"
#include "magic/src/duel_engine.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"

#define ATTACK_MAX_GROUPS 100
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

BOOL TileBitmapIntoRect(HDC hdc, RECT *rect, HBITMAP bitmap);
int draw_masked_bitmap_left_half_to_rect(HDC dc, RECT *rect, HANDLE bitmap);
int load_text_with_tab_escapes(char *filename, char *section_name);
void delete_and_close_object(HANDLE obj);
LRESULT handle_duel_inactive_cursor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void get_current_duel_selection(int *selected_player, int *selected_card);
int get_displayed_card_blocking(int player, int card);
int get_displayed_card_internal_id(int player, int card);
int get_displayed_card_zone(int player, int card);
unsigned int get_displayed_card_display_flags(int player, int card);
unsigned int get_displayed_card_ui_flags(int player, int card);
unsigned int get_displayed_card_state(int player, int card);
void get_displayed_card_attachment(int *player_and_card, int player, int card);
int find_attack_phase_card_window(HWND hwnd, int *player_and_card, int *unused1, HWND *child_hwnd, int *unused2);
int find_battlefield_card_window(HWND hwnd, int *player_and_card, int *unused, HWND *child_hwnd);
int card_window_matches_player_and_card(HWND hwnd, int *player_and_card);
int card_window_matches_card_id(HWND hwnd, card_id_t card_id);
card_id_t get_card_window_displayed_card_id(HWND hwnd);
LONG get_card_window_hidden_flag(HWND hwnd);
void layout_attack_phase_window(HWND hwnd);
int handle_button_palette_message(int hwnd, int msg, void *wparam, int lparam);
void restack_duel_child_windows(void);

extern char global_base_directory[];
extern char global_duelart_path[];
extern char global_ui_strings_filename[];
extern int g_showlist_smallcard_width;
extern int g_showlist_smallcard_height;
extern int DAT_008a8dec;
extern int DAT_00939508;
extern int g_duel_window_userdata_card_offset;
extern int g_duel_window_userdata_player_offset;

// GLOBAL: MAGIC 0x0055e114
// GLOBAL: SHANDALAR 0x0057f014
int g_attackclass_data_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e118
// GLOBAL: SHANDALAR 0x0057f018
int g_attackclass_count_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e11c
// GLOBAL: SHANDALAR 0x0057f01c
int g_attackclass_window_extra_bytes = 8;

// GLOBAL: MAGIC 0x00579ab4
// GLOBAL: SHANDALAR 0x00585734
int DAT_00579ab4 = 6;

// GLOBAL: MAGIC 0x00579be0
// GLOBAL: SHANDALAR 0x00585860
char DAT_00579be0[4] = "";

// GLOBAL: MAGIC 0x00579c00
// GLOBAL: SHANDALAR 0x00585880
char DAT_00579c00[4] = "";

// GLOBAL: MAGIC 0x00579c18
// GLOBAL: SHANDALAR 0x00585898
char DAT_00579c18[4] = "";

// GLOBAL: MAGIC 0x0069c610
// GLOBAL: SHANDALAR 0x005a8fd8
HBITMAP DAT_0069c610;

// GLOBAL: MAGIC 0x0069c614
// GLOBAL: SHANDALAR 0x005a8fdc
HBITMAP DAT_0069c614;

// GLOBAL: MAGIC 0x0069c61c
// GLOBAL: SHANDALAR 0x005a8fe4
HPEN DAT_0069c61c;

// GLOBAL: MAGIC 0x0069c620
// GLOBAL: SHANDALAR 0x005a8fe8
HWND DAT_0069c620;

// GLOBAL: MAGIC 0x0069c628
// GLOBAL: SHANDALAR 0x005a8ff0
char DAT_0069c628[0x1c];

// GLOBAL: MAGIC 0x0069c644
// GLOBAL: SHANDALAR 0x005a900c
HBITMAP DAT_0069c644;

// GLOBAL: MAGIC 0x0069c648
// GLOBAL: SHANDALAR 0x005a9010
HPEN DAT_0069c648;

// GLOBAL: MAGIC 0x0069c64c
// GLOBAL: SHANDALAR 0x005a9014
COLORREF DAT_0069c64c;

// GLOBAL: MAGIC 0x0069c650
// GLOBAL: SHANDALAR 0x005a9018
HPEN DAT_0069c650;

// GLOBAL: MAGIC 0x0069c654
// GLOBAL: SHANDALAR 0x005a901c
int DAT_0069c654;

// GLOBAL: MAGIC 0x0069c658
// GLOBAL: SHANDALAR 0x005a9020
char DAT_0069c658[0x20];

// GLOBAL: MAGIC 0x0069c678
// GLOBAL: SHANDALAR 0x005a9040
char g_attack_minimized_menu_help_text[0x1c];

// GLOBAL: MAGIC 0x0069c694
// GLOBAL: SHANDALAR 0x005a905c
HBRUSH g_attack_defender_brush;

// GLOBAL: MAGIC 0x0069c698
// GLOBAL: SHANDALAR 0x005a9060
HMENU g_attack_minimized_popup_menu;

// GLOBAL: MAGIC 0x0069c6a0
// GLOBAL: SHANDALAR 0x005a9068
HBITMAP DAT_0069c6a0;

// GLOBAL: MAGIC 0x0069c6a4
// GLOBAL: SHANDALAR 0x005a906c
HPEN DAT_0069c6a4;

// GLOBAL: MAGIC 0x0069c6a8
// GLOBAL: SHANDALAR 0x005a9070
HPEN DAT_0069c6a8;

// GLOBAL: MAGIC 0x0069c6b4
// GLOBAL: SHANDALAR 0x005a907c
HBRUSH DAT_0069c6b4;

// GLOBAL: MAGIC 0x0069c6b8
// GLOBAL: SHANDALAR 0x005a9080
HBITMAP g_attack_minimized_background_bitmap;

// GLOBAL: MAGIC 0x0069c6bc
// GLOBAL: SHANDALAR 0x005a9084
HPEN DAT_0069c6bc;

// GLOBAL: MAGIC 0x0069c6c0
// GLOBAL: SHANDALAR 0x005a9088
int DAT_0069c6c0;

// GLOBAL: MAGIC 0x0069c6c8
// GLOBAL: SHANDALAR 0x005a9090
char g_attack_minimized_menu_restore_text[0x1c];

// GLOBAL: MAGIC 0x0069c6e4
// GLOBAL: SHANDALAR 0x005a90ac
COLORREF g_attack_text_shadow_color;

// GLOBAL: MAGIC 0x0069c6e8
// GLOBAL: SHANDALAR 0x005a90b0
HBITMAP DAT_0069c6e8;

// FUNCTION: MAGIC 0x004d2ac0
// FUNCTION: SHANDALAR 0x0045c730
int register_MAGICGAME_AttackClass(LPCSTR class_name)
{
  struct
  {
    char pic_path[264];
    int registered;
    WNDCLASSA wndclass;
  } s;

  s.registered = 1;
  SET_DUEL_WNDCLASS(s.wndclass, 0x800, wndproc_MAGICGAME_AttackClass,
                    g_attackclass_window_extra_bytes,
                    (HICON)0, (HBRUSH)0x6, class_name);
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.registered = 0;
  }

  SET_DUEL_WNDCLASS(s.wndclass, 0x801, wndproc_AttackSwordShield, 0,
                    (HICON)0, (HBRUSH)0x6, CLASS_ATTACK_SWORD_SHIELD);
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.registered = 0;
  }

  SET_DUEL_WNDCLASS(s.wndclass, 3, wndproc_AttackMinimized, 0,
                    (HICON)0, (HBRUSH)0x6, CLASS_ATTACK_MINIMIZED);
  if (RegisterClassA(&s.wndclass) == 0)
  {
    s.registered = 0;
  }

  g_attack_minimized_popup_menu = CreatePopupMenu();
  load_text_with_tab_escapes(global_ui_strings_filename, "MENU_ATTACK");
  strcpy(DAT_0069c628, g_text_lines[0]);
  strcpy(DAT_0069c658, g_text_lines[1]);
  load_text_with_tab_escapes(global_ui_strings_filename, "MENU_MINIMIZEDATTACK");
  strcpy(g_attack_minimized_menu_restore_text, g_text_lines[0]);
  strcpy(g_attack_minimized_menu_help_text, g_text_lines[1]);

  strcpy(s.pic_path, global_duelart_path);
  strcat(s.pic_path, "\\WINBK_Attack.pic");
  DAT_0069c6e8 = load_pic(s.pic_path);
  strcpy(s.pic_path, global_duelart_path);
  strcat(s.pic_path, "\\WINBK_AttackSword.pic");
  DAT_0069c614 = load_pic(s.pic_path);
  strcpy(s.pic_path, global_duelart_path);
  strcat(s.pic_path, "\\WINBK_AttackShield.pic");
  DAT_0069c6a0 = load_pic(s.pic_path);
  strcpy(s.pic_path, global_duelart_path);
  strcat(s.pic_path, "\\WINBK_AttackBones.pic");
  DAT_0069c644 = load_pic(s.pic_path);
  strcpy(s.pic_path, global_duelart_path);
  strcat(s.pic_path, "\\WINBK_AttackRats.pic");
  DAT_0069c610 = load_pic(s.pic_path);
  DAT_00579ab4 = 6;
  strcpy(s.pic_path, global_duelart_path);
  strcat(s.pic_path, "\\WINBK_AttackMin.pic");
  g_attack_minimized_background_bitmap = load_pic(s.pic_path);

  DAT_0069c650 = CreatePen(PS_SOLID, 0, 0x10000b4);
  DAT_0069c6bc = CreatePen(PS_SOLID, 0, 0x100007c);
  DAT_0069c61c = CreatePen(PS_SOLID, 0, 0x1000050);
  g_attack_defender_brush = CreateSolidBrush(0x1000076);
  DAT_0069c6a4 = CreatePen(PS_SOLID, 0, 0x10000d3);
  DAT_0069c6a8 = CreatePen(PS_SOLID, 0, 0x100002f);
  DAT_0069c648 = CreatePen(PS_SOLID, 0, 0x10000d7);
  DAT_0069c6b4 = CreateSolidBrush(0x100003a);
  DAT_0069c64c = 0x10000bf;
  g_attack_text_shadow_color = 0x10000c9;

  if (DAT_0069c650 == (HPEN)0 || DAT_0069c6bc == (HPEN)0 ||
      DAT_0069c61c == (HPEN)0 || g_attack_defender_brush == (HBRUSH)0 ||
      DAT_0069c6a4 == (HPEN)0 || DAT_0069c6a8 == (HPEN)0 ||
      DAT_0069c648 == (HPEN)0 || DAT_0069c6b4 == (HBRUSH)0)
  {
    s.registered = 0;
  }

  return s.registered;
}

// FUNCTION: MAGIC 0x004d2f34
// FUNCTION: SHANDALAR 0x0045cba4
void destroy_MAGICGAME_AttackClass(LPCSTR class_name)
{
  (void)class_name;

  if (g_attack_minimized_popup_menu != (HMENU)0)
  {
    DestroyMenu(g_attack_minimized_popup_menu);
  }
  g_attack_minimized_popup_menu = (HMENU)0;

  if (DAT_0069c6e8 != (HBITMAP)0)
  {
    delete_and_close_object(DAT_0069c6e8);
  }

  if (DAT_0069c614 != (HBITMAP)0)
  {
    delete_and_close_object(DAT_0069c614);
  }

  if (DAT_0069c6a0 != (HBITMAP)0)
  {
    delete_and_close_object(DAT_0069c6a0);
  }

  if (DAT_0069c644 != (HBITMAP)0)
  {
    delete_and_close_object(DAT_0069c644);
  }

  if (DAT_0069c610 != (HBITMAP)0)
  {
    delete_and_close_object(DAT_0069c610);
  }

  if (g_attack_minimized_background_bitmap != (HBITMAP)0)
  {
    delete_and_close_object(g_attack_minimized_background_bitmap);
  }

  if (DAT_0069c650 != (HPEN)0)
  {
    DeleteObject(DAT_0069c650);
  }
  if (DAT_0069c6bc != (HPEN)0)
  {
    DeleteObject(DAT_0069c6bc);
  }
  if (DAT_0069c61c != (HPEN)0)
  {
    DeleteObject(DAT_0069c61c);
  }
  if (g_attack_defender_brush != (HBRUSH)0)
  {
    DeleteObject(g_attack_defender_brush);
  }
  if (DAT_0069c6a4 != (HPEN)0)
  {
    DeleteObject(DAT_0069c6a4);
  }
  if (DAT_0069c6a8 != (HPEN)0)
  {
    DeleteObject(DAT_0069c6a8);
  }
  if (DAT_0069c648 != (HPEN)0)
  {
    DeleteObject(DAT_0069c648);
  }
  if (DAT_0069c6b4 != (HBRUSH)0)
  {
    DeleteObject(DAT_0069c6b4);
  }

  DAT_0069c6e8 = (HBITMAP)0;
  DAT_0069c614 = (HBITMAP)0;
  DAT_0069c6a0 = (HBITMAP)0;
  DAT_0069c644 = (HBITMAP)0;
  DAT_0069c610 = (HBITMAP)0;
  g_attack_minimized_background_bitmap = (HBITMAP)0;
  DAT_0069c650 = (HPEN)0;
  DAT_0069c6bc = (HPEN)0;
  DAT_0069c61c = (HPEN)0;
  g_attack_defender_brush = (HBRUSH)0;
  DAT_0069c6a4 = (HPEN)0;
  DAT_0069c6a8 = (HPEN)0;
  DAT_0069c648 = (HPEN)0;
  DAT_0069c6b4 = (HBRUSH)0;
}

// FUNCTION: MAGIC 0x004d6b78
// FUNCTION: SHANDALAR 0x004607dd
int destroy_attack_window_tree(HWND hwnd, HWND *windows, int window_count)
{
  int destroyed;
  int i;

  destroyed = 0;
  for (i = 0; i < window_count; i++)
  {
    if (windows[i] != (HWND)0 && (HWND)get_card_window_hidden_flag(windows[i]) == hwnd)
    {
      destroy_attack_window_tree(windows[i], windows, window_count);
      DestroyWindow(windows[i]);
      windows[i] = (HWND)0;
    }
  }

  for (i = 0; i < window_count; i++)
  {
    if (windows[i] == hwnd)
    {
      DestroyWindow(windows[i]);
      windows[i] = (HWND)0;
      destroyed = 1;
    }
  }

  return destroyed;
}

// FUNCTION: MAGIC 0x004e9751
// FUNCTION: SHANDALAR 0x0050755e
void propagate_attack_window_selection(HWND parent_hwnd, HWND source_hwnd)
{
  struct
  {
    int player_and_card[2];
    LONG window_count;
    HWND attack_window;
    int i;
    HWND *windows;
  } s;

  s.window_count = GetWindowLongA(parent_hwnd, g_duel_window_userdata_card_offset);
  s.windows = (HWND *)GetWindowLongA(parent_hwnd, g_duel_window_userdata_player_offset);
  SendMessageA(source_hwnd, 0x401, (WPARAM)s.player_and_card, 0);
  if (find_attack_phase_card_window(g_duel_attack_phase_window_hwnd,
                                    s.player_and_card, (int *)0,
                                    &s.attack_window, (int *)0) == 0)
  {
    return;
  }

  for (s.i = 0; s.i < s.window_count; s.i++)
  {
    if ((HWND)get_card_window_hidden_flag(s.windows[s.i]) == source_hwnd)
    {
      SendMessageA(s.windows[s.i], 0x401, (WPARAM)s.player_and_card, 0);
      SendMessageA(g_duel_attack_phase_window_hwnd, 0x406,
                   (WPARAM)s.player_and_card, (LPARAM)s.attack_window);
      propagate_attack_window_selection(parent_hwnd, s.windows[s.i]);
    }
  }
}

// FUNCTION: MAGIC 0x004d3158
// FUNCTION: SHANDALAR 0x0045cdc8
LRESULT CALLBACK wndproc_MAGICGAME_AttackClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int select_menu_item_count;
    POINT popup_point;
    RECT popup_rect;
    int sycommand_masked_wparam;
    LRESULT sycommand_result;
    int ncpaint_frame_width;
    HPEN ncpaint_dark_pen;
    RECT ncpaint_client_rect;
    HBITMAP ncpaint_close_bitmap;
    char ncpaint_caption[100];
    HBRUSH ncpaint_caption_brush;
    HDC ncpaint_dc;
    msvc_bitmap_t ncpaint_bitmap;
    int ncpaint_bitmap_height;
    int ncpaint_bitmap_width;
    int ncpaint_loop_i;
    int ncpaint_frame_y;
    int ncpaint_frame_height;
    int ncpaint_frame_x;
    LRESULT ncpaint_result;
    RECT ncpaint_close_rect;
    RECT ncpaint_caption_rect;
    HPEN ncpaint_light_pen;
    RECT ncpaint_window_rect;
    int ncpaint_selected_player;
    HPEN ncpaint_shade_pen;
    WPARAM nclbutton_wparam;
    LRESULT nclbutton_result;
    int nchittest_width;
    LRESULT nchittest_result;
    RECT nchittest_rect;
    int nccalc_saved_left;
    LRESULT nccalc_result;
    int *nccalc_lparam;
    int hscroll_page_width;
    int hscroll_current_pos;
    int hscroll_min_pos;
    int hscroll_smallcard_width;
    RECT hscroll_rect2;
    RECT hscroll_rect;
    HWND hscroll_scrollbar_hwnd;
    int hscroll_max_pos;
    int hscroll_new_pos;
    char erase_bitmap_path[264];
    HDC erase_dc;
    msvc_bitmap_t erase_bitmap;
    RECT erase_tile_rect;
    RECT erase_client_rect;

    int erase_scroll_pos;
    HWND create_scrollbar_hwnd;
    char help_path[264];

    int help_context;
    int minimize_height;
    msvc_bitmap_t minimize_bitmap;
    int minimize_width;
    int minimize_top;
    int minimize_left;
    RECT minimize_rect;
    HWND case412_attached_hwnd;
    int case412_attached_player_and_card[2];
    HWND case412_child_hwnd;
    int case412_player_and_card[2];
    int case412_card_index;
    unsigned int case412_state_flags;
    WPARAM case412_wparam;
    int case412_child_side;
    int case412_card_zone;
    int case412_internal_id;
    int case412_changed;
    unsigned int case412_display_flags;
    int case412_player_index;
    unsigned int case412_ui_flags;
    HWND case412_battlefield_hwnd;
    int case404_count;
    int case404_j;
    int case404_i;
    WPARAM case404_wparam;
    LPARAM case404_lparam;
    int case405_j;
    int case405_i;
    WPARAM case405_wparam;
    LRESULT case405_result;
    int case405_found;
    int case40e_j;
    int case40e_i;
    WPARAM case40e_wparam;
    LRESULT case40e_result;
    int case40e_found;
    int case40c_j;
    int case40c_i;
    WPARAM case402_wparam;
    int case402_compact_index;
    int case402_compact_j;
    int case402_j;
    int case402_i;
    HWND case402_child_hwnd;
    int case402_found;
    int case410_y_step;
    int case410_x_offset;
    int case410_j;
    int case410_i;
    int case410_top;
    int case410_left;
    WPARAM case410_wparam;
    RECT case410_rect;
    HWND case410_child_hwnd;
    int case406_j;
    int case406_i;
    int case406_found;
    HWND case406_child_hwnd;
    WPARAM case406_wparam;
    int case406_child_side;
    int case406_group_index;
    LPARAM case406_lparam;
    int case400_group_index;
    HWND case400_child_hwnd;
    WPARAM case400_wparam;
    int case400_child_side;
    int case400_group_card;
    int case400_found;
    int case411_found;
    int case411_i;
    LPARAM case436_lparam;
    WPARAM case436_wparam;
    int case436_j;
    int case436_i;
    int case436_found;
    int case435_j;
    int case435_i;
    int case432_j;
    int case432_i;
    WPARAM case433_wparam;
    int case433_j;
    int case433_i;
    int case464_max_pos;
    POINT case464_point;
    int case464_j;
    int case464_i;
    int case464_group_card;
    int case464_min_pos;
    RECT case464_rect;
    int case464_found;
    HWND case464_scrollbar_hwnd;
    attack_window_group_t *groups;
    LONG group_count;
  } s;

  if (msg == 0x464)
  {
    s.case464_group_card = (int)wparam;
    s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
    s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
    s.case464_scrollbar_hwnd = GetDlgItem(hwnd, 0);
    s.case464_found = 0;
    for (s.case464_i = 0; s.case464_i < s.group_count && s.case464_found == 0; s.case464_i++)
    {
      if (s.groups[s.case464_i].group_card == s.case464_group_card)
      {
        s.case464_found = 1;
        s.case464_min_pos = 5000;
        s.case464_max_pos = -5000;
        for (s.case464_j = 0; s.case464_j < s.groups[s.case464_i].attacker_count; s.case464_j++)
        {
          GetWindowRect(s.groups[s.case464_i].attackers[s.case464_j], &s.case464_rect);
          if (s.case464_rect.left < s.case464_min_pos)
          {
            s.case464_min_pos = s.case464_rect.left;
          }
          if (s.case464_max_pos < s.case464_rect.right)
          {
            s.case464_max_pos = s.case464_rect.right;
          }
        }
        for (s.case464_j = 0; s.case464_j < s.groups[s.case464_i].blocker_count; s.case464_j++)
        {
          GetWindowRect(s.groups[s.case464_i].blockers[s.case464_j], &s.case464_rect);
          if (s.case464_rect.left < s.case464_min_pos)
          {
            s.case464_min_pos = s.case464_rect.left;
          }
          if (s.case464_max_pos < s.case464_rect.right)
          {
            s.case464_max_pos = s.case464_rect.right;
          }
        }
      }
    }
    if (s.case464_found != 0)
    {
      GetWindowRect(hwnd, &s.case464_rect);
      if (s.case464_min_pos < s.case464_rect.left)
      {
        s.case464_point.x = s.case464_min_pos;
        s.case464_point.y = 0;
        ScreenToClient(hwnd, &s.case464_point);
      }
      else if (s.case464_rect.right < s.case464_max_pos)
      {
        s.case464_point.x = s.case464_min_pos;
        s.case464_point.y = 0;
        ScreenToClient(hwnd, &s.case464_point);
      }
    }
    return 0;
  }
  else
  {
    switch (msg)
    {
    case 0x437:
      return 0;

    case 0x433:
    case 0x434:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      s.case433_wparam = wparam;
      for (s.case433_i = 0; s.case433_i < s.group_count; s.case433_i++)
      {
        for (s.case433_j = 0; s.case433_j < s.groups[s.case433_i].attacker_count; s.case433_j++)
        {
          if (card_window_matches_card_id(s.groups[s.case433_i].attackers[s.case433_j],
                                          (int)s.case433_wparam) != 0)
          {
            InvalidateRect(s.groups[s.case433_i].attackers[s.case433_j], (RECT *)0, 0);
          }
        }
        for (s.case433_j = 0; s.case433_j < s.groups[s.case433_i].blocker_count; s.case433_j++)
        {
          if (card_window_matches_card_id(s.groups[s.case433_i].blockers[s.case433_j],
                                          (int)s.case433_wparam) != 0)
          {
            InvalidateRect(s.groups[s.case433_i].blockers[s.case433_j], (RECT *)0, 0);
          }
        }
      }
      return 0;

    case 0x432:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      for (s.case432_i = 0; s.case432_i < s.group_count; s.case432_i++)
      {
        for (s.case432_j = 0; s.case432_j < s.groups[s.case432_i].attacker_count; s.case432_j++)
        {
          SendMessageA(s.groups[s.case432_i].attackers[s.case432_j], 0x432, 0, 0);
        }
        for (s.case432_j = 0; s.case432_j < s.groups[s.case432_i].blocker_count; s.case432_j++)
        {
          SendMessageA(s.groups[s.case432_i].blockers[s.case432_j], 0x432, 0, 0);
        }
      }
      return 0;

    case 0x435:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      for (s.case435_i = 0; s.case435_i < s.group_count; s.case435_i++)
      {
        for (s.case435_j = 0; s.case435_j < s.groups[s.case435_i].attacker_count; s.case435_j++)
        {
          InvalidateRect(s.groups[s.case435_i].attackers[s.case435_j], (RECT *)0, 0);
        }
        for (s.case435_j = 0; s.case435_j < s.groups[s.case435_i].blocker_count; s.case435_j++)
        {
          InvalidateRect(s.groups[s.case435_i].blockers[s.case435_j], (RECT *)0, 0);
        }
      }
      return 0;

    case 0x436:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      s.case436_wparam = wparam;
      s.case436_lparam = lparam;
      if (s.case436_wparam == 0)
      {
        return 0;
      }
      s.case436_found = 0;
      for (s.case436_i = 0; s.case436_i < s.group_count; s.case436_i++)
      {
        for (s.case436_j = 0; s.case436_j < s.groups[s.case436_i].attacker_count; s.case436_j++)
        {
          if (card_window_matches_player_and_card(s.groups[s.case436_i].attackers[s.case436_j],
                                                  (int *)s.case436_wparam) != 0)
          {
            s.case436_found = 1;
            if (s.case436_lparam != 0)
            {
              SendMessageA(s.groups[s.case436_i].attackers[s.case436_j], 0x432, 0, 0);
            }
            else
            {
              InvalidateRect(s.groups[s.case436_i].attackers[s.case436_j], (RECT *)0, 0);
            }
          }
        }
        for (s.case436_j = 0; s.case436_j < s.groups[s.case436_i].blocker_count; s.case436_j++)
        {
          if (card_window_matches_player_and_card(s.groups[s.case436_i].blockers[s.case436_j],
                                                  (int *)s.case436_wparam) != 0)
          {
            s.case436_found = 1;
            if (s.case436_lparam != 0)
            {
              SendMessageA(s.groups[s.case436_i].blockers[s.case436_j], 0x432, 0, 0);
            }
            else
            {
              InvalidateRect(s.groups[s.case436_i].blockers[s.case436_j], (RECT *)0, 0);
            }
          }
        }
      }
      return 0;

    case 0x411:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      s.case411_found = 0;
      for (s.case411_i = 0; s.case411_i < s.group_count; s.case411_i++)
      {
        if (s.groups[s.case411_i].attacker_count != 0)
        {
          s.case411_found++;
        }
      }
      return s.case411_found;

    case 0x400:
    case 0x401:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      s.case400_wparam = wparam;
      if (s.case400_wparam == 0)
      {
        return 0;
      }
      find_attack_phase_card_window(hwnd, (int *)s.case400_wparam, (int *)0,
                                    &s.case400_child_hwnd, &s.case400_child_side);
      if (s.case400_child_hwnd != (HWND)0 &&
          ((s.case400_child_side != 0 && msg == 0x400) ||
           (s.case400_child_side == 0 && msg == 0x401)))
      {
        s.case400_found = 1;
      }
      else
      {
        s.case400_found = 0;
      }
      if (s.case400_found == 0)
      {
        s.case400_child_hwnd = CreateWindowExA(0, CLASS_MAGICGAME_CARD, "Card in attack",
                                               WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, 0, 0,
                                               hwnd, (HMENU)1, g_app_instance, (LPVOID)s.case400_wparam);
        if (s.case400_child_hwnd == (HWND)0)
        {
          return 0;
        }
        if (msg == 0x400)
        {
          s.case400_group_card = get_displayed_card_blocking(((int *)s.case400_wparam)[0],
                                                             ((int *)s.case400_wparam)[1]);
          if (s.case400_group_card == -1)
          {
            s.case400_group_card = ((int *)s.case400_wparam)[1];
          }
        }
        else
        {
          s.case400_group_card = get_displayed_card_blocking(((int *)s.case400_wparam)[0],
                                                             ((int *)s.case400_wparam)[1]);
        }
        s.case400_group_index = 0;
        s.case400_found = 0;
        for (; s.case400_group_index < s.group_count && s.case400_found == 0; s.case400_group_index++)
        {
          if (s.groups[s.case400_group_index].group_card == s.case400_group_card)
          {
            s.case400_found = 1;
            if (msg == 0x400 && s.groups[s.case400_group_index].attacker_count < ATTACK_MAX_CARDS_PER_GROUP)
            {
              s.groups[s.case400_group_index].attackers[s.groups[s.case400_group_index].attacker_count] =
                  s.case400_child_hwnd;
              s.groups[s.case400_group_index].attacker_count++;
            }
            else if (msg == 0x401 && s.groups[s.case400_group_index].blocker_count < ATTACK_MAX_CARDS_PER_GROUP)
            {
              s.groups[s.case400_group_index].blockers[s.groups[s.case400_group_index].blocker_count] =
                  s.case400_child_hwnd;
              s.groups[s.case400_group_index].blocker_count++;
            }
            else
            {
              DestroyWindow(s.case400_child_hwnd);
              return 0;
            }
          }
        }
        if (s.case400_found == 0)
        {
          if (s.group_count < ATTACK_MAX_GROUPS)
          {
            s.groups[s.group_count].group_card = s.case400_group_card;
            if (msg == 0x400)
            {
              s.groups[s.group_count].attackers[0] = s.case400_child_hwnd;
              s.groups[s.group_count].attacker_count = 1;
              s.groups[s.group_count].blocker_count = 0;
            }
            else
            {
              s.groups[s.group_count].blockers[0] = s.case400_child_hwnd;
              s.groups[s.group_count].blocker_count = 1;
              s.groups[s.group_count].attacker_count = 0;
            }
            s.group_count++;
            SetWindowLongA(hwnd, g_attackclass_count_window_long_offset, s.group_count);
          }
          else
          {
            DestroyWindow(s.case400_child_hwnd);
            return 0;
          }
        }
        BringWindowToTop(s.case400_child_hwnd);
      }
      return 1;

    case 0x406:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      s.case406_wparam = wparam;
      s.case406_lparam = lparam;
      if (s.case406_wparam == 0 || s.case406_lparam == 0 ||
          GetParent((HWND)s.case406_lparam) != hwnd)
      {
        return 0;
      }
      if (find_attack_phase_card_window(hwnd, (int *)s.case406_wparam, (int *)0,
                                        (HWND *)0, (int *)0) != 0)
      {
        return 1;
      }
      s.case406_found = 0;
      s.case406_i = 0;
      for (; s.case406_i < s.group_count && s.case406_found == 0; s.case406_i++)
      {
        for (s.case406_j = 0;
             s.case406_j < s.groups[s.case406_i].attacker_count && s.case406_found == 0;
             s.case406_j++)
        {
          if (s.groups[s.case406_i].attackers[s.case406_j] == (HWND)s.case406_lparam)
          {
            s.case406_found = 1;
            s.case406_group_index = s.case406_i;
            s.case406_child_side = 1;
          }
        }
        for (s.case406_j = 0;
             s.case406_j < s.groups[s.case406_i].blocker_count && s.case406_found == 0;
             s.case406_j++)
        {
          if (s.groups[s.case406_i].blockers[s.case406_j] == (HWND)s.case406_lparam)
          {
            s.case406_found = 1;
            s.case406_group_index = s.case406_i;
            s.case406_child_side = 0;
          }
        }
      }
      if (s.case406_found == 0)
      {
        return 0;
      }
      s.case406_child_hwnd = CreateWindowExA(0, CLASS_MAGICGAME_CARD, "Card in attack",
                                             WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, 0, 0,
                                             hwnd, (HMENU)1, g_app_instance, (LPVOID)s.case406_wparam);
      if (s.case406_child_hwnd == (HWND)0)
      {
        return 0;
      }
      SendMessageA(s.case406_child_hwnd, 0x402, s.case406_lparam, 0);
      if (s.case406_child_side != 0 && s.groups[s.case406_group_index].attacker_count < ATTACK_MAX_CARDS_PER_GROUP)
      {
        s.groups[s.case406_group_index].attackers[s.groups[s.case406_group_index].attacker_count] =
            s.case406_child_hwnd;
        s.groups[s.case406_group_index].attacker_count++;
      }
      else if (s.case406_child_side != 0 && s.groups[s.case406_group_index].blocker_count < ATTACK_MAX_CARDS_PER_GROUP)
      {
        s.groups[s.case406_group_index].blockers[s.groups[s.case406_group_index].blocker_count] =
            s.case406_child_hwnd;
        s.groups[s.case406_group_index].blocker_count++;
      }
      else
      {
        DestroyWindow(s.case406_child_hwnd);
        return 0;
      }
      return 1;

    case 0x410:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      s.case410_wparam = wparam;
      if (s.case410_wparam == 0)
      {
        return 0;
      }
      s.case410_x_offset = 5;
      s.case410_y_step = DAT_00939508;
      GetWindowRect((HWND)s.case410_wparam, &s.case410_rect);
      MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.case410_rect, 2);
      s.case410_left = s.case410_rect.left + s.case410_x_offset;
      s.case410_top = s.case410_rect.top - s.case410_y_step;
      s.case410_child_hwnd = (HWND)s.case410_wparam;
      for (s.case410_i = 0; s.case410_i < s.group_count; s.case410_i++)
      {
        for (s.case410_j = 0; s.case410_j < s.groups[s.case410_i].attacker_count; s.case410_j++)
        {
          if ((HWND)get_card_window_hidden_flag(s.groups[s.case410_i].attackers[s.case410_j]) ==
              (HWND)s.case410_wparam)
          {
            SetWindowPos(s.groups[s.case410_i].attackers[s.case410_j], s.case410_child_hwnd,
                         s.case410_left, s.case410_top,
                         g_showlist_smallcard_width, g_showlist_smallcard_height, 0);
            s.case410_top -= s.case410_y_step;
            s.case410_child_hwnd = s.groups[s.case410_i].attackers[s.case410_j];
          }
        }
        for (s.case410_j = 0; s.case410_j < s.groups[s.case410_i].blocker_count; s.case410_j++)
        {
          if ((HWND)get_card_window_hidden_flag(s.groups[s.case410_i].blockers[s.case410_j]) ==
              (HWND)s.case410_wparam)
          {
            SetWindowPos(s.groups[s.case410_i].blockers[s.case410_j], s.case410_child_hwnd,
                         s.case410_left, s.case410_top,
                         g_showlist_smallcard_width, g_showlist_smallcard_height, 0);
            s.case410_top -= s.case410_y_step;
            s.case410_child_hwnd = s.groups[s.case410_i].blockers[s.case410_j];
          }
        }
      }
      return 0;

    case 0x402:
    case 0x403:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      s.case402_wparam = wparam;
      if (s.case402_wparam != 0)
      {
        find_attack_phase_card_window(hwnd, (int *)s.case402_wparam, (int *)0,
                                      &s.case402_child_hwnd, (int *)0);
      }
      else
      {
        s.case402_child_hwnd = (HWND)lparam;
      }
      if (s.case402_child_hwnd == (HWND)0 || GetParent(s.case402_child_hwnd) != hwnd)
      {
        return 0;
      }
      for (s.case402_i = 0; s.case402_i < s.group_count; s.case402_i++)
      {
        s.case402_found = 0;
        for (s.case402_j = 0; s.case402_j < s.groups[s.case402_i].attacker_count && s.case402_found == 0; s.case402_j++)
        {
          if (msg == 0x403 && s.groups[s.case402_i].attackers[s.case402_j] == s.case402_child_hwnd)
          {
            s.case402_found = 1;
            destroy_attack_window_tree(s.case402_child_hwnd, s.groups[s.case402_i].attackers,
                                       s.groups[s.case402_i].attacker_count);
            s.case402_compact_index = 0;
            for (s.case402_compact_j = 0; s.case402_compact_j < s.groups[s.case402_i].attacker_count; s.case402_compact_j++)
            {
              if (s.groups[s.case402_i].attackers[s.case402_compact_j] != (HWND)0)
              {
                s.groups[s.case402_i].attackers[s.case402_compact_index] =
                    s.groups[s.case402_i].attackers[s.case402_compact_j];
                s.case402_compact_index++;
              }
            }
            s.groups[s.case402_i].attacker_count = s.case402_compact_index;
          }
          else if (msg == 0x402 && s.groups[s.case402_i].blockers[s.case402_j] == s.case402_child_hwnd)
          {
            s.case402_found = 1;
            destroy_attack_window_tree(s.case402_child_hwnd, s.groups[s.case402_i].blockers,
                                       s.groups[s.case402_i].blocker_count);
            s.case402_compact_index = 0;
            for (s.case402_compact_j = 0; s.case402_compact_j < s.groups[s.case402_i].blocker_count; s.case402_compact_j++)
            {
              if (s.groups[s.case402_i].blockers[s.case402_compact_j] != (HWND)0)
              {
                s.groups[s.case402_i].blockers[s.case402_compact_index] =
                    s.groups[s.case402_i].blockers[s.case402_compact_j];
                s.case402_compact_index++;
              }
            }
            s.groups[s.case402_i].blocker_count = s.case402_compact_index;
          }
        }
      }
      return s.case402_found;

    case 0x40c:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      ShowWindow(hwnd, SW_HIDE);
      ShowWindow(DAT_0069c620, SW_HIDE);
      for (s.case40c_i = 0; s.case40c_i < s.group_count; s.case40c_i++)
      {
        for (s.case40c_j = 0; s.case40c_j < s.groups[s.case40c_i].attacker_count; s.case40c_j++)
        {
          DestroyWindow(s.groups[s.case40c_i].attackers[s.case40c_j]);
        }
        s.groups[s.case40c_i].attacker_count = 0;
        for (s.case40c_j = 0; s.case40c_j < s.groups[s.case40c_i].blocker_count; s.case40c_j++)
        {
          DestroyWindow(s.groups[s.case40c_i].blockers[s.case40c_j]);
        }
        s.groups[s.case40c_i].blocker_count = 0;
      }
      s.group_count = 0;
      SetWindowLongA(hwnd, g_attackclass_count_window_long_offset, s.group_count);
      SendMessageA(GetDlgItem(hwnd, 0), 0xe0, 0, 1);
      return 0;

    case 0x40e:
    case 0x40f:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      s.case40e_wparam = wparam;
      if (s.case40e_wparam == 0)
      {
        s.case40e_found = 0;
      }
      else
      {
        s.case40e_found = 0;
        for (s.case40e_i = 0; s.case40e_i < s.group_count; s.case40e_i++)
        {
          for (s.case40e_j = 0;
               s.case40e_j < s.groups[s.case40e_i].attacker_count && s.case40e_found == 0;
               s.case40e_j++)
          {
            if (card_window_matches_player_and_card(s.groups[s.case40e_i].attackers[s.case40e_j],
                                                    (int *)s.case40e_wparam) != 0)
            {
              s.case40e_found = 1;
              if (msg == 0x40e)
              {
                s.case40e_result =
                    get_card_window_displayed_card_id(s.groups[s.case40e_i].attackers[s.case40e_j]);
              }
              else
              {
                s.case40e_result = (LRESULT)s.groups[s.case40e_i].attackers[s.case40e_j];
              }
            }
          }
          for (s.case40e_j = 0;
               s.case40e_j < s.groups[s.case40e_i].blocker_count && s.case40e_found == 0;
               s.case40e_j++)
          {
            if (card_window_matches_player_and_card(s.groups[s.case40e_i].blockers[s.case40e_j],
                                                    (int *)s.case40e_wparam) != 0)
            {
              s.case40e_found = 1;
              if (msg == 0x40e)
              {
                s.case40e_result =
                    get_card_window_displayed_card_id(s.groups[s.case40e_i].blockers[s.case40e_j]);
              }
              else
              {
                s.case40e_result = (LRESULT)s.groups[s.case40e_i].blockers[s.case40e_j];
              }
            }
          }
        }
      }

      if (s.case40e_found != 0)
      {
        return s.case40e_result;
      }
      else
      {
        return msg == 0x40e ? -1 : 0;
      }

    case 0x405:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      s.case405_wparam = wparam;
      if (s.case405_wparam == 0 || GetParent((HWND)s.case405_wparam) != hwnd)
      {
        return -1;
      }
      s.case405_found = 0;
      for (s.case405_i = 0; s.case405_i < s.group_count; s.case405_i++)
      {
        for (s.case405_j = 0;
             s.case405_j < s.groups[s.case405_i].attacker_count && s.case405_found == 0;
             s.case405_j++)
        {
          if (s.groups[s.case405_i].attackers[s.case405_j] == (HWND)s.case405_wparam)
          {
            s.case405_found = 1;
            s.case405_result = 1;
          }
        }
        for (s.case405_j = 0;
             s.case405_j < s.groups[s.case405_i].blocker_count && s.case405_found == 0;
             s.case405_j++)
        {
          if (s.groups[s.case405_i].blockers[s.case405_j] == (HWND)s.case405_wparam)
          {
            s.case405_found = 1;
            s.case405_result = 0;
          }
        }
      }

      if (s.case405_found != 0)
      {
        return s.case405_result;
      }
      else
      {
        return -1;
      }

    case 0x404:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      s.group_count = GetWindowLongA(hwnd, g_attackclass_count_window_long_offset);
      s.case404_wparam = wparam;
      s.case404_lparam = lparam;
      if (s.case404_lparam == 0 || s.case404_wparam == 0xffffffff)
      {
        return 0;
      }
      s.case404_count = 0;
      for (s.case404_i = 0; s.case404_i < s.group_count; s.case404_i++)
      {
        if (s.groups[s.case404_i].group_card == (int)s.case404_wparam)
        {
          for (s.case404_j = 0; s.case404_j < s.groups[s.case404_i].attacker_count; s.case404_j++)
          {
            if (get_card_window_hidden_flag(s.groups[s.case404_i].attackers[s.case404_j]) == 0)
            {
              ((HWND *)s.case404_lparam)[s.case404_count] =
                  s.groups[s.case404_i].attackers[s.case404_j];
              s.case404_count++;
            }
          }
        }
      }
      return s.case404_count;

    case 0x412:
      s.case412_wparam = wparam;
      s.case412_changed = 0;
      for (s.case412_player_index = 0; s.case412_player_index < 2; s.case412_player_index++)
      {
        for (s.case412_card_index = 0; s.case412_card_index < 0x96; s.case412_card_index++)
        {
          s.case412_player_and_card[0] = s.case412_player_index;
          s.case412_player_and_card[1] = s.case412_card_index;
          s.case412_internal_id = get_displayed_card_internal_id(s.case412_player_index, s.case412_card_index);
          s.case412_card_zone = get_displayed_card_zone(s.case412_player_index, s.case412_card_index);
          s.case412_display_flags = get_displayed_card_display_flags(s.case412_player_index, s.case412_card_index);
          find_attack_phase_card_window(hwnd, s.case412_player_and_card, (int *)0,
                                        &s.case412_child_hwnd, &s.case412_child_side);
          if (find_battlefield_card_window(g_duel_player_battlefield_window_hwnd,
                                           s.case412_player_and_card, (int *)0,
                                           &s.case412_battlefield_hwnd) == 0)
          {
            find_battlefield_card_window(g_duel_help_owner_hwnd, s.case412_player_and_card,
                                         (int *)0, &s.case412_battlefield_hwnd);
          }

          if (s.case412_internal_id == g_stack_proxy_internal_card_id || s.case412_card_zone == 2)
          {
          }
          else if (s.case412_internal_id == -1 || s.case412_card_zone != 1 ||
                   ((s.case412_display_flags & 0x10000) != 0 &&
                    g_duel_interface_options.show_invisible_effect_cards == 0))
          {
            if (s.case412_child_hwnd != (HWND)0)
            {
              if (s.case412_child_side != 0)
              {
                s.case412_changed |= SendMessageA(hwnd, 0x403, 0, (LPARAM)s.case412_child_hwnd);
              }
              else
              {
                s.case412_changed |= SendMessageA(hwnd, 0x402, 0, (LPARAM)s.case412_child_hwnd);
              }
            }
            SendMessageA(GetParent(s.case412_battlefield_hwnd), 0x402,
                         (WPARAM)s.case412_battlefield_hwnd, 1);
          }
          else
          {
            s.case412_ui_flags = get_displayed_card_ui_flags(s.case412_player_index, s.case412_card_index);
            s.case412_state_flags = get_displayed_card_state(s.case412_player_index, s.case412_card_index);
            if ((s.case412_ui_flags & 0x10) == 0)
            {
              if ((((s.case412_ui_flags & 8) != 0) && ((s.case412_state_flags & 4) == 0) &&
                   ((s.case412_state_flags & 0x40) == 0)) ||
                  ((s.case412_state_flags & 8) != 0))
              {
                if (s.case412_child_hwnd == (HWND)0)
                {
                  SendMessageA(hwnd, 0x401, (WPARAM)s.case412_player_and_card, 0);
                  propagate_attack_window_selection(GetParent(s.case412_battlefield_hwnd), s.case412_battlefield_hwnd);
                  s.case412_changed = 1;
                  SendMessageA(GetParent(s.case412_battlefield_hwnd), 0x402,
                               (WPARAM)s.case412_battlefield_hwnd, 0);
                }
              }
              else if (s.case412_child_hwnd != (HWND)0 && s.case412_child_side == 0)
              {
                s.case412_changed |= SendMessageA(hwnd, 0x402, 0, (LPARAM)s.case412_child_hwnd);
                SendMessageA(GetParent(s.case412_battlefield_hwnd), 0x402,
                             (WPARAM)s.case412_battlefield_hwnd, 1);
              }
              if (((s.case412_ui_flags & 4) != 0) || ((s.case412_state_flags & 4) != 0) ||
                  ((s.case412_state_flags & 0x40) != 0))
              {
                if (s.case412_child_hwnd == (HWND)0)
                {
                  SendMessageA(hwnd, 0x400, (WPARAM)s.case412_player_and_card, 0);
                  propagate_attack_window_selection(GetParent(s.case412_battlefield_hwnd), s.case412_battlefield_hwnd);
                  s.case412_changed = 1;
                  SendMessageA(GetParent(s.case412_battlefield_hwnd), 0x402,
                               (WPARAM)s.case412_battlefield_hwnd, 0);
                }
              }
              else if (s.case412_child_hwnd != (HWND)0 && s.case412_child_side != 0)
              {
                s.case412_changed |= SendMessageA(hwnd, 0x403, 0, (LPARAM)s.case412_child_hwnd);
                SendMessageA(GetParent(s.case412_battlefield_hwnd), 0x402,
                             (WPARAM)s.case412_battlefield_hwnd, 1);
              }
            }
            else
            {
              if (find_attack_phase_card_window(hwnd, s.case412_player_and_card,
                                                (int *)0, (HWND *)0, (int *)0) != 0)
              {
              }
              else
              {
                get_displayed_card_attachment(s.case412_attached_player_and_card,
                                              s.case412_player_index, s.case412_card_index);
                if (find_attack_phase_card_window(hwnd, s.case412_attached_player_and_card,
                                                  (int *)0, &s.case412_attached_hwnd, (int *)0) != 0)
                {
                  SendMessageA(hwnd, 0x406, (WPARAM)s.case412_player_and_card,
                               (LPARAM)s.case412_attached_hwnd);
                  s.case412_changed = 1;
                }
              }
            }
          }
        }
      }
      if (s.case412_changed != 0 || s.case412_wparam != 0)
      {
        layout_attack_phase_window(hwnd);
      }
      return 0;

    case WM_ACTIVATE:
      if ((wparam & 0xffff) == WA_ACTIVE || (wparam & 0xffff) == WA_CLICKACTIVE)
      {
        SendMessageA(DAT_0094ca30, WM_NCACTIVATE, 1, 0);
      }
      else
      {
        SendMessageA(DAT_0094ca30, WM_NCACTIVATE, 0, 0);
      }
      return DefWindowProcA(hwnd, msg, wparam, lparam);

    case WM_CLOSE:
      ShowWindow(hwnd, SW_HIDE);
      ShowWindow(DAT_0069c620, SW_HIDE);
      return 0;

    case WM_COMMAND:
      switch (wparam & 0xffff)
      {
      case 0x65:
        ShowWindow(hwnd, SW_HIDE);
        UpdateWindow(g_duel_help_owner_hwnd);
        GetWindowRect((HWND)DAT_008a8dec, &s.minimize_rect);
        s.minimize_left = s.minimize_rect.left;
        s.minimize_width = s.minimize_rect.right - s.minimize_rect.left;
        if (g_attack_minimized_background_bitmap != (HBITMAP)0)
        {
          GetObjectA(g_attack_minimized_background_bitmap, sizeof(s.minimize_bitmap), &s.minimize_bitmap);
          s.minimize_height = (s.minimize_bitmap.bmHeight * s.minimize_width) / s.minimize_bitmap.bmWidth;
        }
        else
        {
          s.minimize_height = s.minimize_width * 2;
        }
        s.minimize_top = (s.minimize_rect.bottom - s.minimize_rect.top) / 2 + s.minimize_height / 2 + 2;
        MoveWindow(DAT_0069c620, s.minimize_left, s.minimize_top, s.minimize_width, s.minimize_height, 1);
        ShowWindow(DAT_0069c620, SW_SHOW);
        BringWindowToTop(DAT_0069c620);
        SendMessageA(g_duel_prompt_context_hwnd, 0x403, 0, 0);
        break;

      case 0x66:
        ShowWindow(DAT_0069c620, SW_HIDE);
        ShowWindow(hwnd, SW_SHOW);
        SendMessageA(g_duel_prompt_context_hwnd, 0x403, 0, 0);
        restack_duel_child_windows();
        break;

      case 100:
        s.help_context = 0xbbc;
        strcpy(s.help_path, global_base_directory);
        strcat(s.help_path, "\\duel.hlp");
        WinHelpA(g_duel_window_hwnd, s.help_path, HELP_CONTEXT, s.help_context);
        break;

      default:
        break;
      }
      return 0;

    case WM_CREATE:
      s.group_count = 0;
      SetWindowLongA(hwnd, g_attackclass_count_window_long_offset, s.group_count);
      s.groups = (attack_window_group_t *)malloc(sizeof(attack_window_group_t) * ATTACK_MAX_GROUPS);
      SetWindowLongA(hwnd, g_attackclass_data_window_long_offset, (LONG)s.groups);
      s.create_scrollbar_hwnd = CreateWindowExA(0, CLASS_MAGICGAME_SCROLLBAR, DAT_00579be0,
                                                WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
                                                hwnd, (HMENU)0, g_app_instance, (LPVOID)0);
      if (s.create_scrollbar_hwnd != (HWND)0)
      {
        SendMessageA(s.create_scrollbar_hwnd, 0x464, (WPARAM)DAT_0069c644, 1);
        SendMessageA(s.create_scrollbar_hwnd, 0x466, (WPARAM)DAT_0069c610, DAT_00579ab4);
      }
      DAT_0094ca30 = CreateWindowExA(0, CLASS_ATTACK_SWORD_SHIELD, DAT_00579c00,
                                     WS_POPUP | WS_BORDER | WS_CAPTION, 0, 0, 0, 0,
                                     GetParent(hwnd), (HMENU)0, g_app_instance, (LPVOID)0);
      DAT_0069c620 = CreateWindowExA(0, CLASS_ATTACK_MINIMIZED, DAT_00579c18,
                                     WS_POPUP, 0, 0, 0, 0, hwnd, (HMENU)0,
                                     g_app_instance, (LPVOID)0);
      if (s.groups == (attack_window_group_t *)0 || s.create_scrollbar_hwnd == (HWND)0 ||
          DAT_0094ca30 == (HWND)0 || DAT_0069c620 == (HWND)0)
      {
        if (s.groups != (attack_window_group_t *)0)
        {
          free(s.groups);
        }
        return -1;
      }
      return 0;

    case WM_DESTROY:
      s.groups = (attack_window_group_t *)GetWindowLongA(hwnd, g_attackclass_data_window_long_offset);
      free(s.groups);
      return 0;

    case WM_ERASEBKGND:
      s.erase_dc = (HDC)wparam;
      ApplyCardArtPaletteToDc(s.erase_dc);
      GetClientRect(hwnd, &s.erase_client_rect);
      s.erase_scroll_pos = SendDlgItemMessageA(hwnd, 0, 0xe1, 0, 0);
      if (DAT_0069c6e8 == (HBITMAP)0)
      {
        strcpy(s.erase_bitmap_path, global_duelart_path);
        strcat(s.erase_bitmap_path, "\\WINBK_Attack.pic");
        DAT_0069c6e8 = load_pic(s.erase_bitmap_path);
      }
      if (DAT_0069c6e8 != (HBITMAP)0)
      {
        CopyRect(&s.erase_tile_rect, &s.erase_client_rect);
        GetObjectA(DAT_0069c6e8, sizeof(s.erase_bitmap), &s.erase_bitmap);
        s.erase_tile_rect.left = 0 - (s.erase_scroll_pos % s.erase_bitmap.bmWidth);
        TileBitmapIntoRect(s.erase_dc, &s.erase_tile_rect, DAT_0069c6e8);
      }
      else
      {
        FillRect(s.erase_dc, &s.erase_client_rect, GetStockObject(BLACK_BRUSH));
      }
      return 1;

    case WM_HSCROLL:
      s.hscroll_scrollbar_hwnd = GetDlgItem(hwnd, 0);
      SendMessageA(s.hscroll_scrollbar_hwnd, 0xe3,
                   (WPARAM)&s.hscroll_min_pos, (LPARAM)&s.hscroll_max_pos);
      s.hscroll_current_pos = SendMessageA(s.hscroll_scrollbar_hwnd, 0xe1, 0, 0);
      s.hscroll_smallcard_width = g_showlist_smallcard_width;
      GetClientRect(hwnd, &s.hscroll_rect);
      s.hscroll_page_width = s.hscroll_rect.right;
      switch (wparam & 0xffff)
      {
      case 6:
        s.hscroll_new_pos = s.hscroll_min_pos;
        break;
      case 0:
        s.hscroll_new_pos = s.hscroll_current_pos - s.hscroll_smallcard_width;
        break;

      case 2:
        s.hscroll_new_pos = s.hscroll_current_pos - s.hscroll_page_width;
        break;
      case 4:
      case 5:
        s.hscroll_new_pos = HIWORD(wparam);
        break;

      case 3:
        s.hscroll_new_pos = s.hscroll_current_pos + s.hscroll_page_width;
        break;

      case 1:
        s.hscroll_new_pos = s.hscroll_current_pos + s.hscroll_smallcard_width;
        break;

      case 7:
        s.hscroll_new_pos = s.hscroll_max_pos;
        break;
      default:
        s.hscroll_new_pos = s.hscroll_current_pos;
        break;
      }
      if (s.hscroll_new_pos < s.hscroll_min_pos)
      {
        s.hscroll_new_pos = s.hscroll_min_pos;
      }
      if (s.hscroll_max_pos < s.hscroll_new_pos)
      {
        s.hscroll_new_pos = s.hscroll_max_pos;
      }
      if (s.hscroll_new_pos != s.hscroll_current_pos)
      {
        SendMessageA(s.hscroll_scrollbar_hwnd, 0xe0, s.hscroll_new_pos, 1);
        GetWindowRect(s.hscroll_scrollbar_hwnd, &s.hscroll_rect2);
        MapWindowPoints((HWND)0, hwnd, (LPPOINT)&s.hscroll_rect2, 2);
        ScrollWindow(hwnd, s.hscroll_current_pos - s.hscroll_new_pos, 0, (RECT *)0, (RECT *)0);
        MoveWindow(s.hscroll_scrollbar_hwnd, s.hscroll_rect2.left, s.hscroll_rect2.top,
                   s.hscroll_rect2.right - s.hscroll_rect2.left,
                   s.hscroll_rect2.bottom - s.hscroll_rect2.top, 0);
        UpdateWindow(hwnd);
      }
      return 0;

    case WM_LBUTTONDOWN:
      return 0;

    case WM_NCCALCSIZE:
      s.nccalc_lparam = (int *)lparam;
      s.nccalc_saved_left = *s.nccalc_lparam;
      s.nccalc_result = DefWindowProcA(hwnd, msg, wparam, lparam);
      *s.nccalc_lparam = s.nccalc_saved_left;
      return s.nccalc_result;

    case WM_NCHITTEST:
      s.nchittest_result = DefWindowProcA(hwnd, msg, wparam, lparam);
      if (s.nchittest_result == HTCAPTION)
      {
        s.nchittest_width = GetSystemMetrics(SM_CXSIZE);
        GetClientRect(hwnd, &s.nchittest_rect);
        MapWindowPoints(hwnd, (HWND)0, (LPPOINT)&s.nchittest_rect, 2);
        s.nchittest_result = HTMINBUTTON;
      }
      return s.nchittest_result;

    case WM_NCLBUTTONDOWN:
      s.nclbutton_wparam = wparam;
      if (s.nclbutton_wparam == HTMINBUTTON)
      {
        SendMessageA(hwnd, WM_COMMAND, 0x65, 0);
        s.nclbutton_result = 0;
      }
      else
      {
        s.nclbutton_result = DefWindowProcA(hwnd, msg, wparam, lparam);
      }
      return s.nclbutton_result;

    case WM_NCPAINT:
    case WM_NCACTIVATE:
      GetWindowRect(hwnd, &s.ncpaint_window_rect);
      OffsetRect(&s.ncpaint_window_rect, -s.ncpaint_window_rect.left, -s.ncpaint_window_rect.top);
      if (s.ncpaint_window_rect.right - s.ncpaint_window_rect.left == 0 ||
          s.ncpaint_window_rect.bottom - s.ncpaint_window_rect.top == 0)
      {
        return DefWindowProcA(hwnd, msg, wparam, lparam);
      }

      if (msg == WM_NCPAINT)
      {
        s.ncpaint_result = 0;
      }
      else
      {
        s.ncpaint_result = 1;
      }
      s.ncpaint_dc = GetWindowDC(hwnd);
      if (s.ncpaint_dc != (HDC)0)
      {
        ApplyCardArtPaletteToDc(s.ncpaint_dc);
        GetWindowRect(hwnd, &s.ncpaint_window_rect);
        GetClientRect(hwnd, &s.ncpaint_client_rect);
        MapWindowPoints(hwnd, (HWND)0, (LPPOINT)&s.ncpaint_client_rect, 2);
        OffsetRect(&s.ncpaint_client_rect, -s.ncpaint_window_rect.left, -s.ncpaint_window_rect.top);
        OffsetRect(&s.ncpaint_window_rect, -s.ncpaint_window_rect.left, -s.ncpaint_window_rect.top);
        GetWindowTextA(hwnd, s.ncpaint_caption, 100);
        s.ncpaint_frame_width = s.ncpaint_window_rect.right - s.ncpaint_client_rect.right;
        s.ncpaint_frame_height = s.ncpaint_window_rect.bottom - s.ncpaint_client_rect.bottom;
        get_current_duel_selection(&s.ncpaint_selected_player, (int *)0);
        if (s.ncpaint_selected_player == 0)
        {
          s.ncpaint_light_pen = DAT_0069c650;
          s.ncpaint_dark_pen = DAT_0069c6bc;
          s.ncpaint_shade_pen = DAT_0069c61c;
          s.ncpaint_caption_brush = g_attack_defender_brush;
        }
        else
        {
          s.ncpaint_light_pen = DAT_0069c6a4;
          s.ncpaint_dark_pen = DAT_0069c6a8;
          s.ncpaint_shade_pen = DAT_0069c648;
          s.ncpaint_caption_brush = DAT_0069c6b4;
        }
        SelectObject(s.ncpaint_dc, s.ncpaint_dark_pen);
        s.ncpaint_frame_y = 0;
        MoveToEx(s.ncpaint_dc, 0, s.ncpaint_frame_y, (LPPOINT)0);
        LineTo(s.ncpaint_dc, s.ncpaint_window_rect.right - 1, s.ncpaint_frame_y);
        SelectObject(s.ncpaint_dc, s.ncpaint_light_pen);
        for (s.ncpaint_loop_i = 1, s.ncpaint_frame_y = 1;
             s.ncpaint_loop_i <= s.ncpaint_frame_height - 2;
             s.ncpaint_loop_i++, s.ncpaint_frame_y++)
        {
          MoveToEx(s.ncpaint_dc, 0, s.ncpaint_frame_y, (LPPOINT)0);
          LineTo(s.ncpaint_dc,
                 (s.ncpaint_window_rect.right - s.ncpaint_frame_width) + 1,
                 s.ncpaint_frame_y);
        }
        SelectObject(s.ncpaint_dc, s.ncpaint_dark_pen);
        s.ncpaint_frame_y = s.ncpaint_frame_height - 1;
        MoveToEx(s.ncpaint_dc, 0, s.ncpaint_frame_y, (LPPOINT)0);
        LineTo(s.ncpaint_dc, s.ncpaint_client_rect.right + 1, s.ncpaint_frame_y);
        SelectObject(s.ncpaint_dc, GetStockObject(BLACK_PEN));
        s.ncpaint_frame_x = s.ncpaint_window_rect.right - 1;
        MoveToEx(s.ncpaint_dc, s.ncpaint_frame_x, 0, (LPPOINT)0);
        LineTo(s.ncpaint_dc, s.ncpaint_frame_x, s.ncpaint_window_rect.bottom);
        SelectObject(s.ncpaint_dc, s.ncpaint_shade_pen);
        for (s.ncpaint_loop_i = 1, s.ncpaint_frame_x = s.ncpaint_window_rect.right - 2;
             s.ncpaint_loop_i <= s.ncpaint_frame_width - 2;
             s.ncpaint_loop_i++, s.ncpaint_frame_x--)
        {
          MoveToEx(s.ncpaint_dc, s.ncpaint_frame_x, 1, (LPPOINT)0);
          LineTo(s.ncpaint_dc, s.ncpaint_frame_x, s.ncpaint_window_rect.bottom - 1);
        }
        SelectObject(s.ncpaint_dc, s.ncpaint_dark_pen);
        s.ncpaint_frame_x = s.ncpaint_client_rect.right;
        MoveToEx(s.ncpaint_dc, s.ncpaint_frame_x,
                 s.ncpaint_frame_height - 1, (LPPOINT)0);
        LineTo(s.ncpaint_dc, s.ncpaint_frame_x, s.ncpaint_client_rect.bottom + 1);
        SelectObject(s.ncpaint_dc, GetStockObject(BLACK_PEN));
        s.ncpaint_frame_y = s.ncpaint_window_rect.bottom - 1;
        MoveToEx(s.ncpaint_dc, 0, s.ncpaint_frame_y, (LPPOINT)0);
        LineTo(s.ncpaint_dc, s.ncpaint_window_rect.right, s.ncpaint_frame_y);
        SelectObject(s.ncpaint_dc, s.ncpaint_shade_pen);
        s.ncpaint_frame_y = s.ncpaint_window_rect.bottom - 2;
        for (s.ncpaint_loop_i = 1;
             s.ncpaint_loop_i <= s.ncpaint_frame_height - 2;
             s.ncpaint_frame_y--, s.ncpaint_loop_i++)
        {
          MoveToEx(s.ncpaint_dc, 0, s.ncpaint_frame_y, (LPPOINT)0);
          LineTo(s.ncpaint_dc, s.ncpaint_window_rect.right - 1, s.ncpaint_frame_y);
        }
        SelectObject(s.ncpaint_dc, s.ncpaint_dark_pen);
        s.ncpaint_frame_y = s.ncpaint_window_rect.bottom - s.ncpaint_frame_height;
        MoveToEx(s.ncpaint_dc, 0, s.ncpaint_frame_y, (LPPOINT)0);
        LineTo(s.ncpaint_dc, s.ncpaint_window_rect.right - 2, s.ncpaint_frame_y);
        SelectObject(s.ncpaint_dc, s.ncpaint_dark_pen);
        s.ncpaint_frame_y = s.ncpaint_client_rect.top - 1;
        MoveToEx(s.ncpaint_dc, 0, s.ncpaint_frame_y, (LPPOINT)0);
        LineTo(s.ncpaint_dc, s.ncpaint_client_rect.right + 1, s.ncpaint_frame_y);
        SetRect(&s.ncpaint_caption_rect, 0, s.ncpaint_frame_height,
                s.ncpaint_client_rect.right, s.ncpaint_client_rect.top - 1);
        FillRect(s.ncpaint_dc, &s.ncpaint_caption_rect, s.ncpaint_caption_brush);
        SetBkMode(s.ncpaint_dc, TRANSPARENT);
        OffsetRect(&s.ncpaint_caption_rect, 1, 1);
        SetTextColor(s.ncpaint_dc, g_attack_text_shadow_color);
        DrawTextA(s.ncpaint_dc, s.ncpaint_caption, -1, &s.ncpaint_caption_rect,
                  DT_SINGLELINE | DT_VCENTER);
        OffsetRect(&s.ncpaint_caption_rect, -1, -1);
        SetTextColor(s.ncpaint_dc, DAT_0069c64c);
        DrawTextA(s.ncpaint_dc, s.ncpaint_caption, -1, &s.ncpaint_caption_rect,
                  DT_SINGLELINE | DT_VCENTER);
        s.ncpaint_close_bitmap = LoadBitmapA((HINSTANCE)0, (LPCSTR)0x7fed);
        GetObjectA(s.ncpaint_close_bitmap, sizeof(s.ncpaint_bitmap), &s.ncpaint_bitmap);
        s.ncpaint_bitmap_width = s.ncpaint_bitmap.bmWidth;
        s.ncpaint_bitmap_height = s.ncpaint_bitmap.bmHeight;
        SetRect(&s.ncpaint_close_rect, s.ncpaint_client_rect.right - s.ncpaint_bitmap_width,
                s.ncpaint_client_rect.top - s.ncpaint_bitmap_height,
                s.ncpaint_client_rect.right, s.ncpaint_client_rect.top);
        DrawBitmapToRect(s.ncpaint_dc, &s.ncpaint_close_rect, g_attack_minimized_background_bitmap);
        DeleteObject(s.ncpaint_close_bitmap);
        ReleaseDC(hwnd, s.ncpaint_dc);
      }
      return s.ncpaint_result;

    case WM_SETCURSOR:
      return handle_duel_inactive_cursor(hwnd, msg, wparam, lparam);

    case WM_SHOWWINDOW:
      if (wparam != 0)
      {
        ShowWindow(DAT_0094ca30, SW_SHOW);
      }
      else
      {
        ShowWindow(DAT_0094ca30, SW_HIDE);
      }
      PostMessageA(g_duel_prompt_context_hwnd, 0x403, 0, 0);
      return DefWindowProcA(hwnd, msg, wparam, lparam);

    case WM_SYSCOMMAND:
      s.sycommand_masked_wparam = wparam & 0xfff0;
      if (s.sycommand_masked_wparam == SC_MOVE)
      {
        s.sycommand_result = 0;
      }
      else
      {
        s.sycommand_result = DefWindowProcA(hwnd, msg, wparam, lparam);
      }
      return s.sycommand_result;

    case WM_RBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
      s.popup_point.x = lparam & 0xffff;
      s.popup_point.y = HIWORD(lparam);
      if (msg == WM_RBUTTONDOWN)
      {
        ClientToScreen(hwnd, &s.popup_point);
      }
      SetRect(&s.popup_rect, s.popup_point.x, s.popup_point.y,
              s.popup_point.x + 1, s.popup_point.y + 1);
      TrackPopupMenu(g_attack_minimized_popup_menu, TPM_RIGHTBUTTON,
                     s.popup_point.x, s.popup_point.y, 0, hwnd, &s.popup_rect);
      return 0;

    case WM_INITMENU:
      AppendMenuA(g_attack_minimized_popup_menu, 0, 0x65, DAT_0069c628);
      AppendMenuA(g_attack_minimized_popup_menu, 0, 100, DAT_0069c658);
      return 0;

    case WM_MENUSELECT:
      if (HIWORD(wparam) == 0xffff && lparam == 0)
      {
        s.select_menu_item_count = GetMenuItemCount(g_attack_minimized_popup_menu);
        while (s.select_menu_item_count-- != 0)
        {
          DeleteMenu(g_attack_minimized_popup_menu, 0, MF_BYPOSITION);
        }
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
  return 0;
}

// FUNCTION: MAGIC 0x004d79ae
// FUNCTION: SHANDALAR 0x0046160c
LRESULT CALLBACK wndproc_AttackSwordShield(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int stack_padding[2];
    char pic_path[264];
    RECT bitmap_rect;
    HDC paint_dc;
    msvc_bitmap_t bitmap;
    int card_height;
    int paint_margin;
    PAINTSTRUCT paint_struct;
    int save_dc;
    RECT scrollbar_rect;
    RECT client_rect;
    int paint_selected_player;
    int frame_width;
    HPEN dark_pen;
    RECT client_window_rect;
    char caption[100];
    HBRUSH caption_brush;
    HDC dc;
    int margin;
    int frame_y;
    int frame_height;
    int frame_x;
    unsigned int nonclient_result;
    RECT caption_rect;
    HPEN light_pen;
    RECT window_rect;
    int border_selected_player;
    HPEN shade_pen;
    LRESULT nccalc_result;
    int top;
    RECT *nccalc_rect;
    LRESULT result;
  } s;

  switch (msg)
  {
  case WM_ACTIVATE:
    s.result = DefWindowProcA(hwnd, msg, wparam, lparam);
    if (wparam != 0)
    {
      SetActiveWindow(g_duel_attack_phase_window_hwnd);
    }
    return s.result;

  case WM_ERASEBKGND:
    return 1;

  case WM_NCCALCSIZE:
    s.nccalc_rect = (RECT *)lparam;
    s.top = s.nccalc_rect->right;
    s.nccalc_result = DefWindowProcA(hwnd, msg, wparam, lparam);
    s.nccalc_rect->right = s.top;
    return s.nccalc_result;

  case WM_NCPAINT:
  case WM_NCACTIVATE:
    GetWindowRect(hwnd, &s.window_rect);
    OffsetRect(&s.window_rect, -s.window_rect.left, -s.window_rect.top);
    if (s.window_rect.right - s.window_rect.left == 0 ||
        s.window_rect.bottom - s.window_rect.top == 0)
    {
      return DefWindowProcA(hwnd, msg, wparam, lparam);
    }
    if (msg == WM_NCPAINT)
    {
      s.nonclient_result = 0;
    }
    else
    {
      s.nonclient_result = 1;
    }
    s.dc = GetWindowDC(hwnd);
    if (s.dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.dc);
      GetWindowRect(hwnd, &s.window_rect);
      GetClientRect(hwnd, &s.client_window_rect);
      MapWindowPoints(hwnd, (HWND)0, (LPPOINT)&s.client_window_rect, 2);
      OffsetRect(&s.client_window_rect, -s.window_rect.left, -s.window_rect.top);
      OffsetRect(&s.window_rect, -s.window_rect.left, -s.window_rect.top);
      GetWindowTextA(hwnd, s.caption, 100);
      s.frame_width = s.client_window_rect.left - s.window_rect.left;
      s.frame_height = s.window_rect.bottom - s.client_window_rect.bottom;
      get_current_duel_selection(&s.border_selected_player, (int *)0);
      if (s.border_selected_player == 0)
      {
        s.light_pen = DAT_0069c650;
        s.dark_pen = DAT_0069c6bc;
        s.shade_pen = DAT_0069c61c;
        s.caption_brush = g_attack_defender_brush;
      }
      else
      {
        s.light_pen = DAT_0069c6a4;
        s.dark_pen = DAT_0069c6a8;
        s.shade_pen = DAT_0069c648;
        s.caption_brush = DAT_0069c6b4;
      }
      SelectObject(s.dc, s.dark_pen);
      s.frame_y = 0;
      MoveToEx(s.dc, 0, s.frame_y, (LPPOINT)0);
      LineTo(s.dc, s.window_rect.right, s.frame_y);
      SelectObject(s.dc, s.light_pen);
      for (s.margin = 1, s.frame_y = 1; s.margin <= s.frame_height - 2; s.margin++, s.frame_y++)
      {
        MoveToEx(s.dc, 1, s.frame_y, (LPPOINT)0);
        LineTo(s.dc, s.window_rect.right, s.frame_y);
      }
      SelectObject(s.dc, s.dark_pen);
      s.frame_y = s.frame_height - 1;
      MoveToEx(s.dc, s.frame_width - 1, s.frame_y, (LPPOINT)0);
      LineTo(s.dc, s.client_window_rect.right, s.frame_y);
      SelectObject(s.dc, s.dark_pen);
      s.frame_x = 0;
      MoveToEx(s.dc, s.frame_x, 0, (LPPOINT)0);
      LineTo(s.dc, s.frame_x, s.window_rect.bottom - 1);
      SelectObject(s.dc, s.light_pen);
      for (s.margin = 1, s.frame_x = 1; s.margin <= s.frame_width - 2; s.margin++, s.frame_x++)
      {
        MoveToEx(s.dc, s.frame_x, 1, (LPPOINT)0);
        LineTo(s.dc, s.frame_x, s.window_rect.bottom - 1);
      }
      SelectObject(s.dc, s.dark_pen);
      s.frame_x = s.client_window_rect.left - 1;
      MoveToEx(s.dc, s.frame_x, s.frame_height - 1, (LPPOINT)0);
      LineTo(s.dc, s.frame_x, s.client_window_rect.bottom + 1);
      SelectObject(s.dc, GetStockObject(BLACK_PEN));
      s.frame_y = s.window_rect.bottom - 1;
      MoveToEx(s.dc, 0, s.frame_y, (LPPOINT)0);
      LineTo(s.dc, s.window_rect.right, s.frame_y);
      SelectObject(s.dc, s.shade_pen);
      s.frame_y = s.window_rect.bottom - 2;
      for (s.margin = 1; s.margin <= s.frame_height - 2; s.margin++, s.frame_y--)
      {
        MoveToEx(s.dc, 1, s.frame_y, (LPPOINT)0);
        LineTo(s.dc, s.window_rect.right, s.frame_y);
      }
      SelectObject(s.dc, s.dark_pen);
      s.frame_y = s.window_rect.bottom - s.frame_height;
      MoveToEx(s.dc, s.frame_width - 1, s.frame_y, (LPPOINT)0);
      LineTo(s.dc, s.window_rect.right, s.frame_y);
      SelectObject(s.dc, s.dark_pen);
      s.frame_y = s.client_window_rect.top - 1;
      MoveToEx(s.dc, s.client_window_rect.left, s.frame_y, (LPPOINT)0);
      LineTo(s.dc, s.window_rect.right, s.frame_y);
      SetRect(&s.caption_rect, s.client_window_rect.left, s.frame_height,
              s.client_window_rect.right, s.client_window_rect.top - 1);
      FillRect(s.dc, &s.caption_rect, s.caption_brush);
      SetTextColor(s.dc, DAT_0069c64c);
      SetBkMode(s.dc, TRANSPARENT);
      DrawTextA(s.dc, s.caption, -1, &s.caption_rect, DT_SINGLELINE | DT_VCENTER);
      ReleaseDC(hwnd, s.dc);
    }
    return s.nonclient_result;

  case WM_PAINT:
    if (DAT_0069c614 == (HBITMAP)0)
    {
      strcpy(s.pic_path, global_duelart_path);
      strcat(s.pic_path, "\\WINBK_AttackSword.pic");
      DAT_0069c614 = load_pic(s.pic_path);
    }
    if (DAT_0069c6a0 == (HBITMAP)0)
    {
      strcpy(s.pic_path, global_duelart_path);
      strcat(s.pic_path, "\\WINBK_AttackShield.pic");
      DAT_0069c6a0 = load_pic(s.pic_path);
    }
    get_current_duel_selection(&s.paint_selected_player, (int *)0);
    s.paint_dc = BeginPaint(hwnd, &s.paint_struct);
    if (s.paint_dc != (HDC)0)
    {
      ApplyCardArtPaletteToDc(s.paint_dc);
      GetClientRect(hwnd, &s.client_rect);
      SendMessageA(g_duel_attack_phase_window_hwnd, WM_ERASEBKGND, (WPARAM)s.paint_dc, 0);
      GetWindowRect(GetDlgItem(g_duel_attack_phase_window_hwnd, 0), &s.scrollbar_rect);
      MapWindowPoints((HWND)0, g_duel_attack_phase_window_hwnd, (LPPOINT)&s.scrollbar_rect, 2);
      s.card_height = g_showlist_smallcard_height;
      s.paint_margin = (g_showlist_smallcard_height * 5) / 100;
      if (DAT_0069c614 != (HBITMAP)0)
      {
        s.save_dc = SaveDC(s.paint_dc);
        GetObjectA(DAT_0069c614, sizeof(s.bitmap), &s.bitmap);
        s.bitmap.bmWidth /= 2;
        if (s.paint_selected_player == 1)
        {
          s.bitmap_rect.top = s.client_rect.top + DAT_0069c6c0;
          s.bitmap_rect.bottom = s.bitmap_rect.top + s.card_height;
        }
        else
        {
          s.bitmap_rect.top = s.scrollbar_rect.bottom + s.paint_margin;
          s.bitmap_rect.bottom = s.bitmap_rect.top + s.card_height;
          SetMapMode(s.paint_dc, MM_ANISOTROPIC);
          SetViewportExtEx(s.paint_dc, 1, -1, (LPSIZE)0);
          SetWindowExtEx(s.paint_dc, 1, 1, (LPSIZE)0);
          SetViewportOrgEx(s.paint_dc, 0, s.client_rect.bottom - DAT_0069c6c0, (LPPOINT)0);
          SetWindowOrgEx(s.paint_dc, 0, s.bitmap_rect.top, (LPPOINT)0);
        }
        s.bitmap_rect.left = 5;
        s.bitmap_rect.right = ((s.bitmap_rect.bottom - s.bitmap_rect.top) *
                               s.bitmap.bmWidth) /
                                  s.bitmap.bmHeight +
                              s.bitmap_rect.left;
        draw_masked_bitmap_left_half_to_rect(s.paint_dc, &s.bitmap_rect, DAT_0069c614);
        RestoreDC(s.paint_dc, s.save_dc);
      }
      if (DAT_0069c6a0 != (HBITMAP)0)
      {
        GetObjectA(DAT_0069c6a0, sizeof(s.bitmap), &s.bitmap);
        s.bitmap.bmWidth /= 2;
        if (s.paint_selected_player == 0)
        {
          s.bitmap_rect.top = s.client_rect.top + DAT_0069c6c0;
          s.bitmap_rect.bottom = s.bitmap_rect.top + s.card_height;
        }
        else
        {
          s.bitmap_rect.bottom = s.client_rect.bottom - DAT_0069c6c0;
          s.bitmap_rect.top = s.bitmap_rect.bottom - s.card_height;
        }
        s.bitmap_rect.left = 5;
        s.bitmap_rect.right = ((s.bitmap_rect.bottom - s.bitmap_rect.top) *
                               s.bitmap.bmWidth) /
                                  s.bitmap.bmHeight +
                              s.bitmap_rect.left;
        draw_masked_bitmap_left_half_to_rect(s.paint_dc, &s.bitmap_rect, DAT_0069c6a0);
      }
      EndPaint(hwnd, &s.paint_struct);
    }
    return 0;
  case WM_SETCURSOR:

    return handle_duel_inactive_cursor(hwnd, msg, wparam, lparam);
  case WM_SYSCOMMAND:

    s.stack_padding[0] = wparam & 0xfff0;
    if (s.stack_padding[0] == SC_MOVE)
    {
      s.stack_padding[1] = 0;
    }
    else
    {
      s.stack_padding[1] = DefWindowProcA(hwnd, msg, wparam, lparam);
    }
    return s.stack_padding[1];
  case 0x30f:
  case 0x310:
  case 0x311:
    return handle_button_palette_message((int)hwnd, msg, (void *)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004d84e0
// FUNCTION: SHANDALAR 0x0046213c
LRESULT CALLBACK wndproc_AttackMinimized(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int menu_item_count;
    POINT popup_point;
    RECT popup_rect;
    char background_path[264];
    HDC erase_dc;
    RECT client_rect;
  } s;

  switch (msg)
  {
  case 0x437:
    strcpy((char *)wparam, gs_cuecard_minimized_attack_window_008b4280);
    return 1;

  case WM_CLOSE:
    ShowWindow(hwnd, SW_HIDE);
    ShowWindow(g_duel_attack_phase_window_hwnd, SW_HIDE);
    return 0;

  case WM_COMMAND:
    return SendMessageA(g_duel_attack_phase_window_hwnd, msg, wparam, lparam);

  case WM_ERASEBKGND:
    s.erase_dc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.erase_dc);
    GetClientRect(hwnd, &s.client_rect);
    IntersectClipRect(s.erase_dc, 0, 0, s.client_rect.right, s.client_rect.bottom);
    if (g_attack_minimized_background_bitmap == (HBITMAP)0)
    {
      strcpy(s.background_path, global_duelart_path);
      strcat(s.background_path, "\\WINBK_AttackMin.pic");
      g_attack_minimized_background_bitmap = load_pic(s.background_path);
    }
    if (g_attack_minimized_background_bitmap != (HBITMAP)0)
    {
      DrawBitmapToRect(s.erase_dc, &s.client_rect, g_attack_minimized_background_bitmap);
    }
    else
    {
      FillRect(s.erase_dc, &s.client_rect, GetStockObject(BLACK_BRUSH));
    }
    return 1;

  case WM_LBUTTONDOWN:
    SendMessageA(g_duel_attack_phase_window_hwnd, WM_COMMAND, 0x66, 0);
    return 0;

  case WM_RBUTTONDOWN:
    s.popup_point.x = (unsigned int)lparam & 0xffff;
    s.popup_point.y = HIWORD(lparam);
    ClientToScreen(hwnd, &s.popup_point);
    SetRect(&s.popup_rect, s.popup_point.x, s.popup_point.y,
            s.popup_point.x + 1, s.popup_point.y + 1);
    TrackPopupMenu(g_attack_minimized_popup_menu, TPM_RIGHTBUTTON, s.popup_point.x,
                   s.popup_point.y, 0, hwnd, &s.popup_rect);
    return 0;

  case WM_INITMENU:
    AppendMenuA(g_attack_minimized_popup_menu, 0, 0x66, g_attack_minimized_menu_restore_text);
    AppendMenuA(g_attack_minimized_popup_menu, 0, 100, g_attack_minimized_menu_help_text);
    return 0;

  case WM_MENUSELECT:
    if (HIWORD(wparam) == 0xffff && lparam == 0)
    {
      s.menu_item_count = GetMenuItemCount(g_attack_minimized_popup_menu);
      while (s.menu_item_count-- != 0)
      {
        DeleteMenu(g_attack_minimized_popup_menu, 0, MF_BYPOSITION);
      }
    }
    return 0;

  case WM_QUERYNEWPALETTE:
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    return handle_button_palette_message((int)hwnd, msg, (void *)wparam, lparam);

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}
