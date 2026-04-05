#include "deckdll.h"
#include "full_deck.h"
#include "mystdbool.h"

extern HINSTANCE global_hinstance;
extern DBFlags global_db_flags_1;
extern DBFlags global_db_flags_2;
extern GlobalConfig global_cfg;
extern bool global_cfg_consolidate;

extern GlobalConfig global_cfg;
extern HWND global_main_hwnd;
extern HWND global_title_hwnd;
extern HWND global_horzlist_hwnd;
extern HWND global_listbox_hwnd;
extern HWND global_unknown_10125a3c;
extern HWND global_unknown_10144b04;
extern int global_deck_num_cards;
extern int global_edited_deck_num_entries;
extern bool global_deck_was_edited;
extern GlobalDeckInfoBlob global_deckinfo;
extern int global_dlg_parameter;
extern int global_dlg_result;

extern int global_smallcard_width;
extern int global_smallcard_height;
extern int global_smallcard_normal_width;
extern int global_smallcard_normal_height;
extern int global_smallcard_smaller_width;
extern int global_smallcard_smaller_height;
extern int global_smallcard_smallest_width;
extern int global_smallcard_smallest_height;
extern int global_smallcard_piclist_height;
extern int global_smallcard_piclist_width;

extern HBRUSH global_create_brush_6;
extern HANDLE global_pic_bldr01c;
extern HANDLE global_pic_bldr02c;
extern HANDLE global_pic_bldr03c;
extern HANDLE global_pic_bldr04c;
extern HANDLE global_pic_bldr05c;
extern FullDeck global_edited_deck;
extern Packs global_packs[PACK1_MAX + 1][PACK2_MAX + 1];
extern Packs global_packs_copy[PACK1_MAX + 1][PACK2_MAX + 1];
extern char text_lines[225][128];

extern int load_text(const char *file_name, const char *section_name);
extern void clear_packs(void);
extern void clear_packs_copy(void);
extern void count_packs(void);
extern void filter_cards_in_lists(HWND hwnd_listbox, HWND hwnd_horzlist);
extern void move_colors_fromto_global_deck(int move_into, unsigned char dlgbits);
extern int check_colors_inout_edited_deck(bool currently_in);
extern void set_smallcard_dimensions(void);
extern void reset_dim_and_pos_all_cards(HWND hwnd, RECT *rect);
extern int ask_about_saving_deck(void);
extern bool show_dialog_deckinfo(void);
extern bool show_dialog_groupmove(void);
extern void refresh_numofcards_text(void);
extern void insert_cards_into_deck(csvid_t csvid, int num, FullDeck *tgt_deck);
extern void __cdecl add_or_increment_sideboard_bucket_entry(int which, int csvid, int num, FullDeck *deck_base);
extern void __cdecl append_trade_bucket_entry(int which, int csvid, int num, FullDeck *deck_base);
extern void ApplyCardArtPaletteToDc(HDC hdc);

// GLOBAL: DECKDLL 0x101a8a6c
HWND global_decksurface_hwnd;
// GLOBAL: DECKDLL 0x100f2768
static HMENU global_decksurface_popup;

// GLOBAL: DECKDLL 0x100f2770
static RECT trade_group_rects[7];
// GLOBAL: DECKDLL 0x100f27e0
static RECT sideboard_group_rects[6];
// GLOBAL: DECKDLL 0x100f2840
static HMENU global_tradesurface_popup;
// GLOBAL: DECKDLL 0x100f2844
static HMENU global_sideboardsurface_popup;

// Sideboard surface strings (class names, menu strings, labels).
// GLOBAL: DECKDLL 0x10031214
static const char s_Empty_10031214[4] = "";
// GLOBAL: DECKDLL 0x10031218
static const char s_MAGICDECK_CardClass_10031218[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003122c
static const char s_Empty_1003122c[4] = "";
// GLOBAL: DECKDLL 0x10031230
static const char s_MAGICDECK_CardClass_10031230[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x10031244
static const char s_Empty_10031244[4] = "";
// GLOBAL: DECKDLL 0x10031248
static const char s_MAGICDECK_CardClass_10031248[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003125c
static const char s_Empty_1003125c[4] = "";
// GLOBAL: DECKDLL 0x10031260
static const char s_MAGICDECK_CardClass_10031260[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x10031274
static const char s_Empty_10031274[4] = "";
// GLOBAL: DECKDLL 0x10031278
static const char s_MAGICDECK_CardClass_10031278[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003128c
static const char s_Empty_1003128c[4] = "";
// GLOBAL: DECKDLL 0x10031290
static const char s_MAGICDECK_CardClass_10031290[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100312a4
static const char s_Empty_100312a4[4] = "";
// GLOBAL: DECKDLL 0x100312a8
static const char s_MAGICDECK_CardClass_100312a8[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100312bc
static const char s_Empty_100312bc[4] = "";
// GLOBAL: DECKDLL 0x100312c0
static const char s_MAGICDECK_CardClass_100312c0[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100312d4
static const char s_Empty_100312d4[4] = "";
// GLOBAL: DECKDLL 0x100312d8
static const char s_MAGICDECK_CardClass_100312d8[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100312ec
static const char s_Empty_100312ec[4] = "";
// GLOBAL: DECKDLL 0x100312f0
static const char s_MAGICDECK_CardClass_100312f0[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x10031304
static const char s_Empty_10031304[4] = "";
// GLOBAL: DECKDLL 0x10031308
static const char s_MAGICDECK_CardClass_10031308[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003131c
static const char s_Empty_1003131c[4] = "";
// GLOBAL: DECKDLL 0x10031320
static const char s_MAGICDECK_CardClass_10031320[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x10031334
static const char s_Empty_10031334[4] = "";
// GLOBAL: DECKDLL 0x10031338
static const char s_MAGICDECK_CardClass_10031338[20] = "MAGICDECK_CardClass";

// GLOBAL: DECKDLL 0x1003134c
static const char s_C_lear_sideboard_1003134c[20] = "C&lear sideboard";
// GLOBAL: DECKDLL 0x10031360
static const char s__Restore_sideboard_10031360[20] = "&Restore sideboard";
// GLOBAL: DECKDLL 0x10031374
static const char s_C_lear_sideboard_10031374[20] = "C&lear sideboard";
// GLOBAL: DECKDLL 0x10031388
static const char s_Sideboard_10031388[12] = "Sideboard";
// GLOBAL: DECKDLL 0x10031394
static const char s__New_deck_10031394[12] = "&New deck";
// GLOBAL: DECKDLL 0x100313a0
static const char s__Load_deck_100313a0[12] = "&Load deck";
// GLOBAL: DECKDLL 0x100313ac
static const char s__Save_deck_100313ac[12] = "&Save deck";
// GLOBAL: DECKDLL 0x100313b8
static const char s__Consolidate_duplicate_cards_sid_100313b8[36] = "&Consolidate duplicate cards side";
// GLOBAL: DECKDLL 0x100313dc
static const char s_C_lear_Sideboard_100313dc[20] = "C&lear Sideboard";
// GLOBAL: DECKDLL 0x100313f0
static const char s_M_inimize_100313f0[12] = "M&inimize";
// GLOBAL: DECKDLL 0x100313fc
static const char s__Music_100313fc[8] = "&Music";
// GLOBAL: DECKDLL 0x10031404
static const char s_Sound__Effects_10031404[16] = "Sound &Effects";
// GLOBAL: DECKDLL 0x10031414
static const char s_E_xit_deck_builder_10031414[20] = "E&xit deck builder";

// GLOBAL: DECKDLL 0x10031428
static const char s_None_10031428[8] = "None";
// GLOBAL: DECKDLL 0x10031430
static const char s_Black_10031430[8] = "Black";
// GLOBAL: DECKDLL 0x10031438
static const char s_Blue_10031438[8] = "Blue";
// GLOBAL: DECKDLL 0x10031440
static const char s_Red_10031440[4] = "Red";
// GLOBAL: DECKDLL 0x10031444
static const char s_Green_10031444[8] = "Green";
// GLOBAL: DECKDLL 0x1003144c
static const char s_White_1003144c[8] = "White";

// Trade surface strings (class names, localization keys, labels).
// GLOBAL: DECKDLL 0x10031454
static const char s_Empty_10031454[4] = "";
// GLOBAL: DECKDLL 0x10031458
static const char s_MAGICDECK_CardClass_10031458[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003146c
static const char s_Empty_1003146c[4] = "";
// GLOBAL: DECKDLL 0x10031470
static const char s_MAGICDECK_CardClass_10031470[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x10031484
static const char s_Empty_10031484[4] = "";
// GLOBAL: DECKDLL 0x10031488
static const char s_MAGICDECK_CardClass_10031488[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003149c
static const char s_Empty_1003149c[4] = "";
// GLOBAL: DECKDLL 0x100314a0
static const char s_MAGICDECK_CardClass_100314a0[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100314b4
static const char s_Empty_100314b4[4] = "";
// GLOBAL: DECKDLL 0x100314b8
static const char s_MAGICDECK_CardClass_100314b8[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100314cc
static const char s_Empty_100314cc[4] = "";
// GLOBAL: DECKDLL 0x100314d0
static const char s_MAGICDECK_CardClass_100314d0[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100314e4
static const char s_Empty_100314e4[4] = "";
// GLOBAL: DECKDLL 0x100314e8
static const char s_MAGICDECK_CardClass_100314e8[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100314fc
static const char s_Empty_100314fc[4] = "";
// GLOBAL: DECKDLL 0x10031500
static const char s_MAGICDECK_CardClass_10031500[20] = "MAGICDECK_CardClass";

// GLOBAL: DECKDLL 0x10031514
static const char s_DECKSURFACE_TRADE_10031514[20] = "DECKSURFACE_TRADE";
// GLOBAL: DECKDLL 0x10031528
static const char s_menus_10031528[8] = "menus";
// GLOBAL: DECKDLL 0x10031530
static const char s_DECKSURFACE_TRADE_10031530[20] = "DECKSURFACE_TRADE";
// GLOBAL: DECKDLL 0x10031544
static const char s_menus_10031544[8] = "menus";
// GLOBAL: DECKDLL 0x1003154c
static const char s_DECKSURFACE_TRADE_1003154c[20] = "DECKSURFACE_TRADE";
// GLOBAL: DECKDLL 0x10031560
static const char s_menus_10031560[8] = "menus";
// GLOBAL: DECKDLL 0x10031568
static const char s_DECKSURFACE_TRADE_10031568[20] = "DECKSURFACE_TRADE";
// GLOBAL: DECKDLL 0x1003157c
static const char s_menus_1003157c[8] = "menus";

// GLOBAL: DECKDLL 0x10031584
static const char s_Deck_10031584[8] = "Deck";
// GLOBAL: DECKDLL 0x1003158c
static const char s_None_1003158c[8] = "None";
// GLOBAL: DECKDLL 0x10031594
static const char s_Black_10031594[8] = "Black";
// GLOBAL: DECKDLL 0x1003159c
static const char s_Blue_1003159c[8] = "Blue";
// GLOBAL: DECKDLL 0x100315a4
static const char s_Red_100315a4[4] = "Red";
// GLOBAL: DECKDLL 0x100315a8
static const char s_Green_100315a8[8] = "Green";
// GLOBAL: DECKDLL 0x100315b0
static const char s_White_100315b0[8] = "White";

// FUNCTION: DECKDLL 0x10005051
static void __cdecl
DrawGroupBoxWithLabel(HDC hdc, RECT *rect, const char *label)
{
  struct
  {
    SIZE local_24;
    RECT local_1c;
    int local_c;
    HPEN local_8;
    HPEN local_4;
  } s;

  CopyRect(&s.local_1c, rect);
  GetTextExtentPointA(hdc, label, strlen(label), &s.local_24);

  s.local_4 = CreatePen(0, 1, GetSysColor(0x10));
  s.local_8 = CreatePen(0, 1, GetSysColor(0x16));

  s.local_c = SaveDC(hdc);
  SetBkMode(hdc, 1);

  OffsetRect(&s.local_1c, 1, 1);
  SetTextColor(hdc, GetSysColor(0x16));
  SelectObject(hdc, s.local_8);
  MoveToEx(hdc, s.local_1c.left, s.local_1c.top + s.local_24.cy / 2, (LPPOINT)0x0);
  LineTo(hdc, s.local_1c.left + 10, s.local_1c.top + s.local_24.cy / 2);
  TextOutA(hdc, s.local_1c.left + 0xe, s.local_1c.top, label, strlen(label));
  MoveToEx(hdc, s.local_1c.left + s.local_24.cx + 0x12, s.local_1c.top + s.local_24.cy / 2, (LPPOINT)0x0);
  LineTo(hdc, s.local_1c.right, s.local_1c.top + s.local_24.cy / 2);
  LineTo(hdc, s.local_1c.right, s.local_1c.bottom);
  LineTo(hdc, s.local_1c.left, s.local_1c.bottom);
  LineTo(hdc, s.local_1c.left, s.local_1c.top + s.local_24.cy / 2);

  OffsetRect(&s.local_1c, -1, -1);
  SetTextColor(hdc, GetSysColor(0x10));
  SelectObject(hdc, s.local_4);
  MoveToEx(hdc, s.local_1c.left, s.local_1c.top + s.local_24.cy / 2, (LPPOINT)0x0);
  LineTo(hdc, s.local_1c.left + 10, s.local_1c.top + s.local_24.cy / 2);
  TextOutA(hdc, s.local_1c.left + 0xe, s.local_1c.top, label, strlen(label));
  MoveToEx(hdc, s.local_1c.left + s.local_24.cx + 0x12, s.local_1c.top + s.local_24.cy / 2, (LPPOINT)0x0);
  LineTo(hdc, s.local_1c.right, s.local_1c.top + s.local_24.cy / 2);
  LineTo(hdc, s.local_1c.right, s.local_1c.bottom);
  LineTo(hdc, s.local_1c.left, s.local_1c.bottom);
  LineTo(hdc, s.local_1c.left, s.local_1c.top + s.local_24.cy / 2);

  RestoreDC(hdc, s.local_c);
  DeleteObject(s.local_4);
  DeleteObject(s.local_8);
}

// FUNCTION: DECKDLL 0x10005317
static int pick_group_id_from_point(RECT *group_rects, int group_count, POINT *pt)
{
  struct
  {
    int best_xdist;
    int best_ydist;
    int group_i;
    int best_group;
    int dist_xy[50];
    int hit_found;
  } s;

  s.group_i = 0;
  s.hit_found = 0;
  for (; (s.group_i < group_count) && (s.hit_found == 0); s.group_i++)
  {
    if (PtInRect(&group_rects[s.group_i], *pt) != 0)
    {
      s.best_group = s.group_i;
      s.hit_found = 1;
    }
    else
    {
      s.dist_xy[s.group_i * 2] = MAX(
          abs(pt->x - group_rects[s.group_i].right),
          abs(pt->x - group_rects[s.group_i].left));

      s.dist_xy[s.group_i * 2 + 1] = MAX(
          abs(pt->y - group_rects[s.group_i].bottom),
          abs(pt->y - group_rects[s.group_i].top));
    }
  }

  if (s.hit_found == 0)
  {
    s.best_xdist = 1000;
    for (s.group_i = 0; s.group_i < group_count; s.group_i = s.group_i + 1)
    {
      if (s.dist_xy[s.group_i * 2] < s.best_xdist)
      {
        s.best_xdist = s.dist_xy[s.group_i * 2];
        s.best_group = s.group_i;
        s.best_ydist = s.dist_xy[s.best_group * 2 + 1];
      }
      else if ((s.dist_xy[s.group_i * 2] == s.best_xdist) &&
               (s.dist_xy[s.group_i * 2 + 1] < s.best_ydist))
      {
        s.best_ydist = s.dist_xy[s.group_i * 2 + 1];
        s.best_group = s.group_i;
      }
    }
  }
  return s.best_group;
}

// FUNCTION: DECKDLL 0x100055cc
static HWND
find_wanted_window(HWND hwnd, csvid_t csvid)
{
  HWND result = 0;
  HWND w = GetTopWindow(hwnd);
  for (; w; w = GetWindow(w, GW_HWNDNEXT))
  {
    if (SendMessage(w, 0x400, 0, 0) == csvid)
      result = w;
  }
  return result;
}

// FUNCTION: DECKDLL 0x1000563b
static HWND __cdecl
find_wanted_window_in_group(HWND hwnd_parent, int group_id, int csvid)
{
  HWND w;
  HWND result;

  result = (HWND)0x0;
  for (w = GetTopWindow(hwnd_parent); w != (HWND)0x0; w = GetWindow(w, GW_HWNDNEXT))
  {
    if (GetDlgCtrlID(w) == group_id)
    {
      if (SendMessageA(w, 0x400, 0, 0) == csvid)
        result = w;
    }
  }
  return result;
}

// FUNCTION: DECKDLL 0x10006450
static int add_smallcard_window(HWND hwnd_parent, csvid_t csvid, int num)
{
  HWND hwnd;

  hwnd = CreateWindowEx(0, "MAGICDECK_CardClass", "",
                        WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                        0, 0, global_smallcard_width, global_smallcard_height,
                        hwnd_parent,
                        1,
                        global_hinstance, csvid);
  if (hwnd)
  {
    BringWindowToTop(hwnd);
    SendMessage(hwnd, 0x401, num, 0);
  }
  else
    return 0;

  return 1;
}

// FUNCTION: DECKDLL 0x100056bd
static void invalidate_wanted_window(HWND hwnd, WPARAM wparam)
{
  int result = 0;
  HWND w;
  for (w = GetTopWindow(hwnd); w; w = GetWindow(w, GW_HWNDNEXT))
    if (SendMessage(w, 0x400, 0, 0) == wparam)
      InvalidateRect(w, NULL, TRUE);
}

// FUNCTION: DECKDLL 0x1000572c
void destroy_child_windows(HWND hwnd)
{
  HWND w;
  while (w = GetTopWindow(hwnd))
    DestroyWindow(w);
}

// FUNCTION: DECKDLL 0x100058c0
static void __cdecl
layout_child_smallcard_windows(HWND hwnd_parent, RECT *group_rects, int group_count)
{
  struct
  {
    HWND last_child;
    int y_step;
    HWND child;
    int group_i;
    int group_xy[40];
  } s;

  s.y_step = (global_smallcard_height * 0x12) / 100;
  for (s.group_i = 0; s.group_i < group_count; s.group_i = s.group_i + 1)
  {
    s.group_xy[s.group_i * 2] =
        group_rects[s.group_i].left +
        (((group_rects[s.group_i].right - group_rects[s.group_i].left) - global_smallcard_width) / 2);
    s.group_xy[s.group_i * 2 + 1] =
        group_rects[s.group_i].top + global_smallcard_height / 5;
  }

  s.last_child = (HWND)0x0;
  for (s.child = GetTopWindow(hwnd_parent); s.child != (HWND)0x0; s.child = GetWindow(s.child, GW_HWNDNEXT))
    s.last_child = s.child;

  for (s.child = s.last_child; s.child != (HWND)0x0; s.child = GetWindow(s.child, GW_HWNDPREV))
  {
    for (s.group_i = 0; s.group_i < group_count; s.group_i = s.group_i + 1)
    {
      if (GetDlgCtrlID(s.child) == s.group_i)
      {
        SetWindowPos(s.child, (HWND)0x0, s.group_xy[s.group_i * 2], s.group_xy[s.group_i * 2 + 1],
                     global_smallcard_width, global_smallcard_height, SWP_NOZORDER);
        s.group_xy[s.group_i * 2 + 1] = s.group_xy[s.group_i * 2 + 1] + s.y_step;
      }
    }
  }
}

// FUNCTION: DECKDLL 0x1000663b
void restore_global_packs()
{
  int pack1;

  for (pack1 = 0; pack1 < PACK1_MAX + 1; pack1++)
  {
    global_packs[pack1][PACK2_LAND].num = global_packs_copy[pack1][PACK2_LAND].num;
    global_packs[pack1][PACK2_CREATURE].num = global_packs_copy[pack1][PACK2_CREATURE].num;
    global_packs[pack1][PACK2_ENCHANTMENT].num = global_packs_copy[pack1][PACK2_ENCHANTMENT].num;
    global_packs[pack1][PACK2_SORCERY].num = global_packs_copy[pack1][PACK2_SORCERY].num;
    global_packs[pack1][PACK2_INTERRUPT].num = global_packs_copy[pack1][PACK2_INTERRUPT].num;
    global_packs[pack1][PACK2_INSTANT].num = global_packs_copy[pack1][PACK2_INSTANT].num;
  }
}

// FUNCTION: DECKDLL 0x100064d7
void copy_global_packs()
{
  int pack1;

  for (pack1 = 0; pack1 < PACK1_MAX + 1; pack1++)
  {
    global_packs_copy[pack1][PACK2_LAND].num = global_packs[pack1][PACK2_LAND].num;
    global_packs_copy[pack1][PACK2_CREATURE].num = global_packs[pack1][PACK2_CREATURE].num;
    global_packs_copy[pack1][PACK2_ENCHANTMENT].num = global_packs[pack1][PACK2_ENCHANTMENT].num;
    global_packs_copy[pack1][PACK2_SORCERY].num = global_packs[pack1][PACK2_SORCERY].num;
    global_packs_copy[pack1][PACK2_INTERRUPT].num = global_packs[pack1][PACK2_INTERRUPT].num;
    global_packs_copy[pack1][PACK2_INSTANT].num = global_packs[pack1][PACK2_INSTANT].num;
  }
}

// FUNCTION: DECKDLL 0x10005760
void resize_child_smallcard_windows(HWND hwnd_parent)
{
  HWND child = GetTopWindow(hwnd_parent);

  for (; child; child = GetWindow(child, GW_HWNDNEXT))
  {
    SetWindowPos(child, NULL, 0, 0, global_smallcard_width, global_smallcard_height, SWP_NOZORDER | SWP_NOMOVE);
  }
}

/*
 * wndproc_DeckSurfaceClass has a large stack frame in the original binary.
 * Use a single locals struct to keep MSVC 4.20's stack layout stable.
 */
typedef struct WndprocDeckSurfaceLocals_t
{
  int mosaic_inx;/* [ebp-0xa4] */
  POINT popup_pt; /* [ebp-0xa0] */
  RECT tile_rect; /* [ebp-0x98] */
  BITMAP bmp;     /* [ebp-0x88] */
  int pad;        /* [ebp-0x70] */
  int mosaic_x;   /* [ebp-0x6c] */
  int mosaic_y;   /* [ebp-0x68] */
  RECT r2;        /* [ebp-0x64] */
  RECT refresh_rect;
  int pad2;     /* [ebp-0x44] */
  char *pcVar1; /* [ebp-0x40] */
  int x;        /* [ebp-0x3c] */
  int y;        /* [ebp-0x38] */
  int tile;     /* [ebp-0x34] */

  HWND hwnd_card; /* [ebp-0x30] */
  csvid_t csvid;  /* [ebp-0x2c] */

  HDC hdc0; /* [ebp-0x28] */

  int idx_i; /* [ebp-0x24] */
  int idx_d; /* [ebp-0x20] */
  int idx_c; /* [ebp-0x1c] */

  int mosaics[5]; /* [ebp-0x18] */

  LRESULT result; /* [ebp-0x4] */
} WndprocDeckSurfaceLocals;

// 2 stack slots are from switch() statements so substract 8 bytes
STATIC_ASSERT(sizeof(WndprocDeckSurfaceLocals) == 0xa4, WndprocDeckSurfaceLocals_wrong_size);

// FUNCTION: DECKDLL 0x100012b7
LRESULT CALLBACK
wndproc_DeckSurfaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  WndprocDeckSurfaceLocals s;
  // GLOBAL: DECKDLL 0x1003105c
  static int cleared_global_deck_num_entries = 0;
  // GLOBAL: DECKDLL 0x10031060
  static int cleared_global_deck_num_cards = 0;
  // GLOBAL: DECKDLL 0x101f1e40
  static char mosaic_order[50];

  s.result = 1;

  switch (msg)
  {
  case 0x401:
  {
    destroy_child_windows(hwnd);

    set_smallcard_dimensions();

    LockWindowUpdate(global_main_hwnd);

    if (global_cfg_consolidate)
    {
      for (s.idx_i = 0; s.idx_i < PACK1_MAX + 1; s.idx_i = s.idx_i + 1)
      {
        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_LAND].num; s.idx_c = s.idx_c + 1)
        {
          add_smallcard_window(
              hwnd,
              (csvid_t)(global_packs[s.idx_i][PACK2_LAND].table[s.idx_c].val & 0xffff),
              global_packs[s.idx_i][PACK2_LAND].table[s.idx_c].val >> 0x10);
        }
      }

      for (s.idx_i = 0; s.idx_i < PACK1_MAX + 1; s.idx_i = s.idx_i + 1)
      {
        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_CREATURE].num; s.idx_c = s.idx_c + 1)
        {
          add_smallcard_window(
              hwnd,
              (csvid_t)(global_packs[s.idx_i][PACK2_CREATURE].table[s.idx_c].val & 0xffff),
              global_packs[s.idx_i][PACK2_CREATURE].table[s.idx_c].val >> 0x10);
        }

        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_ENCHANTMENT].num; s.idx_c = s.idx_c + 1)
        {
          add_smallcard_window(
              hwnd,
              (csvid_t)(global_packs[s.idx_i][PACK2_ENCHANTMENT].table[s.idx_c].val & 0xffff),
              global_packs[s.idx_i][PACK2_ENCHANTMENT].table[s.idx_c].val >> 0x10);
        }

        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_SORCERY].num; s.idx_c = s.idx_c + 1)
        {
          add_smallcard_window(
              hwnd,
              (csvid_t)(global_packs[s.idx_i][PACK2_SORCERY].table[s.idx_c].val & 0xffff),
              global_packs[s.idx_i][PACK2_SORCERY].table[s.idx_c].val >> 0x10);
        }

        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_INTERRUPT].num; s.idx_c = s.idx_c + 1)
        {
          add_smallcard_window(
              hwnd,
              (csvid_t)(global_packs[s.idx_i][PACK2_INTERRUPT].table[s.idx_c].val & 0xffff),
              global_packs[s.idx_i][PACK2_INTERRUPT].table[s.idx_c].val >> 0x10);
        }

        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_INSTANT].num; s.idx_c = s.idx_c + 1)
        {
          add_smallcard_window(
              hwnd,
              (csvid_t)(global_packs[s.idx_i][PACK2_INSTANT].table[s.idx_c].val & 0xffff),
              global_packs[s.idx_i][PACK2_INSTANT].table[s.idx_c].val >> 0x10);
        }
      }
    }
    else
    {
      for (s.idx_i = 0; s.idx_i < PACK1_MAX + 1; s.idx_i = s.idx_i + 1)
      {
        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_LAND].num; s.idx_c = s.idx_c + 1)
        {
          for (s.idx_d = 0;
               s.idx_d < (global_packs[s.idx_i][PACK2_LAND].table[s.idx_c].val >> 0x10);
               s.idx_d = s.idx_d + 1)
          {
            add_smallcard_window(
                hwnd,
                (csvid_t)(global_packs[s.idx_i][PACK2_LAND].table[s.idx_c].val & 0xffff),
                1);
          }
        }
      }

      for (s.idx_i = 0; s.idx_i < PACK1_MAX + 1; s.idx_i = s.idx_i + 1)
      {
        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_CREATURE].num; s.idx_c = s.idx_c + 1)
        {
          for (s.idx_d = 0;
               s.idx_d < (global_packs[s.idx_i][PACK2_CREATURE].table[s.idx_c].val >> 0x10);
               s.idx_d = s.idx_d + 1)
          {
            add_smallcard_window(
                hwnd,
                (csvid_t)(global_packs[s.idx_i][PACK2_CREATURE].table[s.idx_c].val & 0xffff),
                1);
          }
        }

        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_ENCHANTMENT].num; s.idx_c = s.idx_c + 1)
        {
          for (s.idx_d = 0;
               s.idx_d < (global_packs[s.idx_i][PACK2_ENCHANTMENT].table[s.idx_c].val >> 0x10);
               s.idx_d = s.idx_d + 1)
          {
            add_smallcard_window(
                hwnd,
                (csvid_t)(global_packs[s.idx_i][PACK2_ENCHANTMENT].table[s.idx_c].val & 0xffff),
                1);
          }
        }

        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_SORCERY].num; s.idx_c = s.idx_c + 1)
        {
          for (s.idx_d = 0;
               s.idx_d < (global_packs[s.idx_i][PACK2_SORCERY].table[s.idx_c].val >> 0x10);
               s.idx_d = s.idx_d + 1)
          {
            add_smallcard_window(
                hwnd,
                (csvid_t)(global_packs[s.idx_i][PACK2_SORCERY].table[s.idx_c].val & 0xffff),
                1);
          }
        }

        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_INTERRUPT].num; s.idx_c = s.idx_c + 1)
        {
          for (s.idx_d = 0;
               s.idx_d < (global_packs[s.idx_i][PACK2_INTERRUPT].table[s.idx_c].val >> 0x10);
               s.idx_d = s.idx_d + 1)
          {
            add_smallcard_window(
                hwnd,
                (csvid_t)(global_packs[s.idx_i][PACK2_INTERRUPT].table[s.idx_c].val & 0xffff),
                1);
          }
        }

        for (s.idx_c = 0; s.idx_c < global_packs[s.idx_i][PACK2_INSTANT].num; s.idx_c = s.idx_c + 1)
        {
          for (s.idx_d = 0;
               s.idx_d < (global_packs[s.idx_i][PACK2_INSTANT].table[s.idx_c].val >> 0x10);
               s.idx_d = s.idx_d + 1)
          {
            add_smallcard_window(
                hwnd,
                (csvid_t)(global_packs[s.idx_i][PACK2_INSTANT].table[s.idx_c].val & 0xffff),
                1);
          }
        }
      }
    }

    LockWindowUpdate(0);

    SendMessage(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    refresh_numofcards_text();
    return s.result;
  }

  case 0x4c8:
  {
    s.tile = 1;
    s.csvid = wparam;
    s.x = lparam & 0xffff;
    s.y = (lparam >> 16) & 0xffff;

    if (global_cfg_consolidate && (s.hwnd_card = find_wanted_window(hwnd, s.csvid)))
    {
      insert_cards_into_deck(s.csvid, 1, &global_edited_deck);
      s.pcVar1 = SendMessage(s.hwnd_card, 0x402, 0, 0);
      SendMessage(s.hwnd_card, 0x401, 1 + s.pcVar1, 0);
    }
    else
    {
      s.hwnd_card = CreateWindowEx(0, "MAGICDECK_CardClass", "",
                                   WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                   s.x, s.y, global_smallcard_width, global_smallcard_height,
                                   hwnd,
                                   1,
                                   global_hinstance, s.csvid);
      if (s.hwnd_card)
      {
        BringWindowToTop(s.hwnd_card);
        insert_cards_into_deck(s.csvid, 1, &global_edited_deck);
        SendMessage(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
      }
      else
        s.tile = 0;
    }

    if (cleared_global_deck_num_entries)
    {
      load_text("menus", "DECKCLEAR_RESTORE");
      InsertMenu(global_decksurface_popup, RES_MENU_SORTDECK, MF_BYCOMMAND | MF_ENABLED, RES_MENU_CLEARDECK, text_lines[0]);
      RemoveMenu(global_decksurface_popup, RES_MENU_RESTOREDECK, MF_BYCOMMAND);
      cleared_global_deck_num_cards = 0;
      cleared_global_deck_num_entries = cleared_global_deck_num_cards;
    }
    return s.tile;
  }

  case 0x466:
  {
    s.pad2 = wparam;
    invalidate_wanted_window(hwnd, s.pad2);
    return 0;
  }

  case WM_COMMAND:
    switch (wparam & 0xffff)
    {
    case RES_MENU_CONSOLIDATE:
    {
      global_cfg_consolidate = !global_cfg_consolidate;
      SendMessage(global_decksurface_hwnd, 0x401, 0, 0);
      SendMessageA(global_unknown_10144b04, 0x401, 0, 0);
      SendMessageA(global_unknown_10125a3c, 0x401, 0, 0);
      break;
    }

    case RES_MENU_REFRESH:
    {
      GetClientRect(hwnd, &s.refresh_rect);
      InflateRect(&s.refresh_rect, -5, -5);
      LockWindowUpdate(global_main_hwnd);
      reset_dim_and_pos_all_cards(hwnd, &s.refresh_rect);
      LockWindowUpdate(NULL);
      break;
    }

    case RES_MENU_CLEARDECK:
    {
      if (global_edited_deck_num_entries)
      {
        global_deck_was_edited = false;
        copy_global_packs();
        clear_packs();

        cleared_global_deck_num_entries = global_edited_deck_num_entries;

        cleared_global_deck_num_cards = global_deck_num_cards;
        global_edited_deck_num_entries = global_deck_num_cards = 0;

        RemoveMenu(global_decksurface_popup, RES_MENU_CLEARDECK, MF_BYCOMMAND);
        load_text("menus", "DECKCLEAR_RESTORE");
        InsertMenu(global_decksurface_popup, RES_MENU_SORTDECK, MF_BYCOMMAND | MF_ENABLED, RES_MENU_RESTOREDECK, text_lines[1]);
        SendMessage(global_decksurface_hwnd, 0x401, 0, 0);
      }
      break;
    }

    case RES_MENU_RESTOREDECK:
    {
      restore_global_packs();
      clear_packs_copy();
      global_deck_was_edited = true;

      global_edited_deck_num_entries = cleared_global_deck_num_entries;

      global_deck_num_cards = cleared_global_deck_num_cards;
      cleared_global_deck_num_entries = cleared_global_deck_num_cards = 0;

      load_text("menus", "DECKCLEAR_RESTORE");
      InsertMenu(global_decksurface_popup, RES_MENU_SORTDECK, MF_BYCOMMAND | MF_ENABLED, RES_MENU_CLEARDECK, text_lines[0]);
      DeleteMenu(global_decksurface_popup, RES_MENU_RESTOREDECK, MF_BYCOMMAND);
      SendMessage(global_decksurface_hwnd, 0x401, 0, 0);
      break;
    }

    case RES_MENU_NEWDECK:
    {
      // TODO: this is probably a good idea
      // if (!global_deck_was_edited || ask_about_saving_deck())

      global_deckinfo.revision = 1;
      global_edited_deck_num_entries = global_deck_num_cards = 0;

      InvalidateRect(global_title_hwnd, NULL, TRUE);

      clear_packs();
      clear_packs_copy();

      if (DeleteMenu(global_decksurface_popup, RES_MENU_RESTOREDECK, MF_BYCOMMAND))
      {
        load_text("menus", "DECKCLEAR_RESTORE");
        InsertMenu(global_decksurface_popup, RES_MENU_SORTDECK, MF_BYCOMMAND | MF_ENABLED, RES_MENU_CLEARDECK, text_lines[0]);
      }
      SendMessage(global_decksurface_hwnd, 0x401, 0, 0);

      load_text("menus", "NEWDECK");
      sprintf(global_deckinfo.deckname, text_lines[0]);

      global_deckinfo.description[0] = global_deckinfo.comments[0] = 0;

      strcpy(global_deckinfo.author, global_cfg.player_name);
      strcpy(global_deckinfo.email, global_cfg.email);

      GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, NULL, "MMMM dd',' yyyy", global_deckinfo.creation_date, 22);

      show_dialog_deckinfo();

      break;
    }

    case RES_MENU_LOADDECK:
    {
      if (DeleteMenu(global_decksurface_popup, RES_MENU_RESTOREDECK, MF_BYCOMMAND))
      {
        load_text("menus", "DECKCLEAR_RESTORE");
        InsertMenu(global_decksurface_popup, RES_MENU_SORTDECK, MF_BYCOMMAND | MF_ENABLED, RES_MENU_CLEARDECK, text_lines[0]);
      }
      SendMessage(global_main_hwnd, WM_COMMAND, RES_MAINMENU_LOADDECK, 0);
      break;
    }

    case RES_MENU_SAVEDECK:
    {
      SendMessage(global_main_hwnd, WM_COMMAND, RES_MAINMENU_SAVEDECK, 0);
      break;
    }

    case RES_MENU_EXIT:
    {
      SendMessage(global_main_hwnd, WM_CLOSE, 0, 0);
      break;
    }

    case RES_MENU_SORTDECK:
    {
      SendMessage(global_decksurface_hwnd, 0x401, 0, 0);
      break;
    }

    case RES_MENU_MUSIC:
    {
      SendMessage(global_main_hwnd, WM_COMMAND, RES_MAINMENU_MUSIC, 0);
      break;
    }

    case RES_MENU_EFFECTS:
    {
      SendMessage(global_main_hwnd, WM_COMMAND, RES_MAINMENU_EFFECTS, 0);
      break;
    }

    case RES_MENU_MINIMIZE:
    {
      SendMessage(global_main_hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
      SendMessage(GetParent(global_main_hwnd), WM_SYSCOMMAND, SC_MINIMIZE, 0);
      break;
    }

    case RES_MENU_COLORINTODECK:
      global_dlg_parameter = check_colors_inout_edited_deck(0);
      if (show_dialog_groupmove())
      {
        move_colors_fromto_global_deck(1, global_dlg_result);
        count_packs();
        SendMessage(global_decksurface_hwnd, 0x401, 0, 0);
        filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);
      }
      break;

    case RES_MENU_COLOROUTOFDECK:
      global_dlg_parameter = check_colors_inout_edited_deck(1);
      if (show_dialog_groupmove())
      {
        move_colors_fromto_global_deck(0, global_dlg_result);
        count_packs();
        SendMessage(global_decksurface_hwnd, 0x401, 0, 0);
        filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);
      }

    default:
      break;
    }
    return 0;
  case WM_CREATE:
  {
    if ((global_db_flags_1 & DBFLAGS_GAUNTLET) || (global_db_flags_1 & DBFLAGS_NOCARDCOUNTCHECK))
    {
      clear_packs();
      clear_packs_copy();
    }

    for (s.idx_c = 0; s.idx_c < 50; s.idx_c++)
    {
      mosaic_order[s.idx_c] = (char)(((unsigned char)rand()) % 5);
    }

    global_decksurface_popup = CreatePopupMenu();

    if (global_db_flags_1 & DBFLAGS_STANDALONE)
    {
      load_text("menus", "DECKSURFACE_STANDALONE");

      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_NEWDECK, text_lines[0]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_LOADDECK, text_lines[1]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_SAVEDECK, text_lines[2]);

      AppendMenu(global_decksurface_popup, MF_SEPARATOR, 0, NULL);

      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_CONSOLIDATE, text_lines[3]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_CLEARDECK, text_lines[4]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_SORTDECK, text_lines[5]);

      AppendMenu(global_decksurface_popup, MF_SEPARATOR, 0, NULL);

      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_MINIMIZE, text_lines[6]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_MUSIC, text_lines[7]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_EFFECTS, text_lines[8]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_EXIT, text_lines[9]);
    }
    else if (global_db_flags_1 & DBFLAGS_GAUNTLET)
    {
      load_text("menus", "DECKSURFACE_GAUNTLET");

      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_NEWDECK, text_lines[0]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_LOADDECK, text_lines[1]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_SAVEDECK, text_lines[2]);

      AppendMenu(global_decksurface_popup, MF_SEPARATOR, 0, NULL);

      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_CONSOLIDATE, text_lines[3]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_CLEARDECK, text_lines[4]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_SORTDECK, text_lines[5]);

      AppendMenu(global_decksurface_popup, MF_SEPARATOR, 0, NULL);

      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_MUSIC, text_lines[6]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_EFFECTS, text_lines[7]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_EXIT, text_lines[8]);
    }
    else
    {
      load_text("menus", "DECKSURFACE_ADVENTURE");
      if (global_db_flags_2 & DBFLAGS_SHANDALAR)
      {
        AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_COLORINTODECK, text_lines[0]);
        AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_COLOROUTOFDECK, text_lines[1]);

        AppendMenu(global_decksurface_popup, MF_SEPARATOR, 0, NULL);
      }
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_CONSOLIDATE, text_lines[2]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_SORTDECK, text_lines[3]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_MUSIC, text_lines[4]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_EFFECTS, text_lines[5]);
      AppendMenu(global_decksurface_popup, MF_ENABLED, RES_MENU_EXIT, text_lines[6]);
    }

    return 0;
  }

  case WM_DESTROY:
    DestroyMenu(global_decksurface_popup);
    return 0;

  case WM_ERASEBKGND:
  {
    s.hdc0 = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc0);

    for (s.pad = 0; s.pad < 5; s.pad++)
    {
      s.mosaics[s.pad] = CreateCompatibleDC(s.hdc0);
      ApplyCardArtPaletteToDc(s.mosaics[s.pad]);
    }

    SelectObject(s.mosaics[0], global_pic_bldr01c);
    SelectObject(s.mosaics[1], global_pic_bldr02c);
    SelectObject(s.mosaics[2], global_pic_bldr03c);
    SelectObject(s.mosaics[3], global_pic_bldr04c);
    SelectObject(s.mosaics[4], global_pic_bldr05c);

    GetClientRect(hwnd, &s.r2);
    GetClientRect(hwnd, &s.tile_rect);
    GetObjectA(global_pic_bldr01c, 0x18, &s.bmp);

    s.tile_rect.right = s.r2.right / 6;
    s.tile_rect.bottom = s.r2.bottom / 5;

    s.pad = 0;
    for (s.mosaic_y = 0; s.mosaic_y < s.r2.right; s.mosaic_y += s.tile_rect.right)
    {
      for (s.mosaic_x = 0; s.mosaic_x < s.r2.bottom; s.mosaic_x += s.tile_rect.bottom)
      {
        s.mosaic_inx = s.pad++;
        StretchBlt(s.hdc0, s.mosaic_y, s.mosaic_x, s.tile_rect.right, s.tile_rect.bottom, s.mosaics[mosaic_order[s.mosaic_inx]], 0, 0, s.bmp.bmWidth,
                   s.bmp.bmHeight, SRCCOPY);
      }
    }

    ReleaseDC(hwnd, s.hdc0);
    for (s.pad = 0; s.pad < 5; s.pad++)
      DeleteDC(s.mosaics[s.pad]);

    return 1;
  }

  case WM_INITMENU:
    CHECKMENU_IF(global_decksurface_popup, RES_MENU_CONSOLIDATE, global_cfg_consolidate);
    CHECKMENU_IF(global_decksurface_popup, RES_MENU_EFFECTS, global_cfg.effects);
    CHECKMENU_IF(global_decksurface_popup, RES_MENU_MUSIC, global_cfg.music);
    return 0;

  case WM_LBUTTONDOWN:
  {
    BringWindowToTop(hwnd);
    SetFocus(global_main_hwnd);
    return 0;
  }

  case WM_RBUTTONDOWN:
  {
    s.popup_pt.x = (int)((unsigned)lparam & 0xFFFF);
    s.popup_pt.y = (int)((unsigned)lparam >> 16) & 0xFFFF;
    ClientToScreen(hwnd, &s.popup_pt);
    TrackPopupMenu(global_decksurface_popup, TPM_RIGHTBUTTON, s.popup_pt.x, s.popup_pt.y, 0, hwnd, NULL);
    return 0;
  }

  case WM_SIZE:
  {
    global_smallcard_height = ((lparam & 0xFFFF) * 19) / 100;
    global_smallcard_width = global_smallcard_height;

    global_smallcard_normal_height = ((lparam & 0xFFFF) * 19) / 100;
    global_smallcard_normal_width = global_smallcard_normal_height;
    
    global_smallcard_smaller_height = (lparam & 0xFFFF) / 7;
    global_smallcard_smaller_width = global_smallcard_smaller_height;

    global_smallcard_smallest_height = (lparam & 0xFFFF) / 8;
    global_smallcard_smallest_width = global_smallcard_smallest_height;

    global_smallcard_piclist_height = ((lparam & 0xFFFF) * 19) / 100;
    global_smallcard_piclist_width = global_smallcard_piclist_height;

    resize_child_smallcard_windows(hwnd);

    if (global_decksurface_hwnd)
      SendMessage(global_decksurface_hwnd, 0x401, 0, 0);

    return 0;
  }

  default:
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: DECKDLL 0x10002b0f
LRESULT CALLBACK wndproc_SideboardSurfaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  // Sideboard "clear/restore" saved totals.
  // GLOBAL: DECKDLL 0x10031064
  static int saved_sideboard_total_none = 0;
  // GLOBAL: DECKDLL 0x10031068
  static int saved_sideboard_total_black = 0;
  // GLOBAL: DECKDLL 0x1003106c
  static int saved_sideboard_total_blue = 0;
  // GLOBAL: DECKDLL 0x10031070
  static int saved_sideboard_total_green = 0;
  // GLOBAL: DECKDLL 0x10031074
  static int saved_sideboard_total_white = 0;
  // GLOBAL: DECKDLL 0x10031078
  static int saved_sideboard_total_red = 0;

  struct
  {
    int pad0; /* matches [ebp-0xb0] */
    int pad1;
    int iVar1;
    int iVar2;
    int iVar3;
    int iVar4;
    POINT local_94;
    HDC local_8c;
    PAINTSTRUCT local_88;
    HDC local_48;
    RECT local_44;
    HDC local_34;
    LRESULT local_30;
    POINT local_2c;
    LRESULT local_24;
    HWND local_20;
    HMENU local_1c;
    HDC local_18;
    int local_14;
    int local_10;
    LRESULT local_c;
    HWND local_8;
  } s;

  switch (msg)
  {
  case 0x401:
    s.local_c = 1;
    destroy_child_windows(hwnd);
    if (global_cfg_consolidate != 0)
    {
      for (s.local_10 = 0; global_edited_deck.sideboard[0].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031218, s_Empty_10031214,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x0, global_hinstance,
                                    (LPVOID)global_edited_deck.sideboard[0].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, global_edited_deck.sideboard[0].entries[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
      for (s.local_10 = 0; global_edited_deck.sideboard[1].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031230, s_Empty_1003122c,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x1, global_hinstance,
                                    (LPVOID)global_edited_deck.sideboard[1].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, global_edited_deck.sideboard[1].entries[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
      for (s.local_10 = 0; global_edited_deck.sideboard[2].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031248, s_Empty_10031244,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x2, global_hinstance,
                                    (LPVOID)global_edited_deck.sideboard[2].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, global_edited_deck.sideboard[2].entries[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
      for (s.local_10 = 0; global_edited_deck.sideboard[3].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031260, s_Empty_1003125c,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x3, global_hinstance,
                                    (LPVOID)global_edited_deck.sideboard[3].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, global_edited_deck.sideboard[3].entries[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
      for (s.local_10 = 0; global_edited_deck.sideboard[5].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031278, s_Empty_10031274,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x5, global_hinstance,
                                    (LPVOID)global_edited_deck.sideboard[5].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, global_edited_deck.sideboard[5].entries[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
      for (s.local_10 = 0; global_edited_deck.sideboard[4].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031290, s_Empty_1003128c,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x4, global_hinstance,
                                    (LPVOID)global_edited_deck.sideboard[4].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, global_edited_deck.sideboard[4].entries[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
    }
    else
    {
      for (s.local_10 = 0; global_edited_deck.sideboard[0].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; global_edited_deck.sideboard[0].entries[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100312a8, s_Empty_100312a4,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x0, global_hinstance,
                                            (LPVOID)global_edited_deck.sideboard[0].entries[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
      for (s.local_10 = 0; global_edited_deck.sideboard[1].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; global_edited_deck.sideboard[1].entries[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100312c0, s_Empty_100312bc,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x1, global_hinstance,
                                            (LPVOID)global_edited_deck.sideboard[1].entries[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
      for (s.local_10 = 0; global_edited_deck.sideboard[2].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; global_edited_deck.sideboard[2].entries[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100312d8, s_Empty_100312d4,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x2, global_hinstance,
                                            (LPVOID)global_edited_deck.sideboard[2].entries[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
      for (s.local_10 = 0; global_edited_deck.sideboard[3].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; global_edited_deck.sideboard[3].entries[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100312f0, s_Empty_100312ec,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x3, global_hinstance,
                                            (LPVOID)global_edited_deck.sideboard[3].entries[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
      for (s.local_10 = 0; global_edited_deck.sideboard[5].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; global_edited_deck.sideboard[5].entries[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031308, s_Empty_10031304,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x5, global_hinstance,
                                            (LPVOID)global_edited_deck.sideboard[5].entries[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
      for (s.local_10 = 0; global_edited_deck.sideboard[4].total > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; global_edited_deck.sideboard[4].entries[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031320, s_Empty_1003131c,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x4, global_hinstance,
                                            (LPVOID)global_edited_deck.sideboard[4].entries[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
    }
    SendMessageA(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    return s.local_c;

  case 0x4c8:
    s.local_24 = 1;
    s.local_18 = (HDC)wparam;
    s.local_2c.x = (int)((unsigned)lparam & 0xFFFF);
    s.local_2c.y = (int)((unsigned)lparam >> 16);
    s.local_1c = (HMENU)pick_group_id_from_point(&sideboard_group_rects[0], 6, &s.local_2c);
    if ((global_cfg_consolidate != 0) && ((s.local_20 = find_wanted_window_in_group(hwnd, (int)s.local_1c, (int)s.local_18)) != NULL))
    {
      add_or_increment_sideboard_bucket_entry((int)s.local_1c, (int)s.local_18, 1, &global_edited_deck);
      s.local_30 = SendMessageA(s.local_20, 0x402, 0, 0);
      SendMessageA(s.local_20, 0x401, s.local_30 + 1, 0);
    }
    else
    {
      s.local_20 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031338, s_Empty_10031334,
                                   WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                   s.local_2c.x, s.local_2c.y, global_smallcard_width,
                                   global_smallcard_height, hwnd, s.local_1c, global_hinstance,
                                   (LPVOID)s.local_18);
      if (s.local_20 == NULL)
      {
        s.local_24 = 0;
      }
      else
      {
        BringWindowToTop(s.local_20);
        add_or_increment_sideboard_bucket_entry((int)s.local_1c, (int)s.local_18, 1, &global_edited_deck);
        SendMessageA(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
      }
    }

    if ((((saved_sideboard_total_none == 0) && (saved_sideboard_total_black == 0)) && (saved_sideboard_total_blue == 0)) &&
        (((saved_sideboard_total_green == 0 && (saved_sideboard_total_white == 0)) && (saved_sideboard_total_red == 0))))
      return s.local_24;

    AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_CLEARDECK, s_C_lear_sideboard_1003134c);
    DeleteMenu(global_sideboardsurface_popup, RES_MENU_RESTOREDECK, 0);

    saved_sideboard_total_none = 0;
    saved_sideboard_total_black = 0;
    saved_sideboard_total_blue = 0;
    saved_sideboard_total_green = 0;
    saved_sideboard_total_white = 0;
    saved_sideboard_total_red = 0;
    return s.local_24;

  case 0x466:
    s.local_34 = (HDC)wparam;
    invalidate_wanted_window(hwnd, wparam);
    return 0;

  case WM_COMMAND:
    switch ((unsigned)wparam & 0xFFFF)
    {
    case RES_MENU_CONSOLIDATE:
      global_cfg_consolidate = (int)(global_cfg_consolidate == 0);
      SendMessageA(global_decksurface_hwnd, 0x401, 0, 0);
      SendMessageA(global_unknown_10144b04, 0x401, 0, 0);
      SendMessageA(global_unknown_10125a3c, 0x401, 0, 0);
      break;

    case RES_MENU_REFRESH:
      layout_child_smallcard_windows(hwnd, &sideboard_group_rects[0], 6);
      break;

    case RES_MENU_CLEARDECK:
      if (((((global_edited_deck.sideboard[0].total != 0) || (global_edited_deck.sideboard[1].total != 0)) || (global_edited_deck.sideboard[2].total != 0)) ||
           ((global_edited_deck.sideboard[3].total != 0 || (global_edited_deck.sideboard[4].total != 0)))) ||
          (global_edited_deck.sideboard[5].total != 0))
      {
        saved_sideboard_total_none = global_edited_deck.sideboard[0].total;
        saved_sideboard_total_black = global_edited_deck.sideboard[1].total;
        saved_sideboard_total_blue = global_edited_deck.sideboard[2].total;
        saved_sideboard_total_green = global_edited_deck.sideboard[3].total;
        saved_sideboard_total_white = global_edited_deck.sideboard[4].total;
        saved_sideboard_total_red = global_edited_deck.sideboard[5].total;

        global_edited_deck.sideboard[2].total = 0;
        global_edited_deck.sideboard[1].total = 0;
        global_edited_deck.sideboard[0].total = 0;
        global_edited_deck.sideboard[5].total = 0;
        global_edited_deck.sideboard[4].total = 0;
        global_edited_deck.sideboard[3].total = 0;

        DeleteMenu(global_sideboardsurface_popup, RES_MENU_CLEARDECK, 0);
        AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_RESTOREDECK, s__Restore_sideboard_10031360);
        SendMessageA(global_unknown_10144b04, 0x401, 0, 0);
      }
      break;

    case RES_MENU_RESTOREDECK:
      global_edited_deck.sideboard[0].total = saved_sideboard_total_none;
      global_edited_deck.sideboard[1].total = saved_sideboard_total_black;
      global_edited_deck.sideboard[2].total = saved_sideboard_total_blue;
      global_edited_deck.sideboard[3].total = saved_sideboard_total_green;
      global_edited_deck.sideboard[4].total = saved_sideboard_total_white;
      global_edited_deck.sideboard[5].total = saved_sideboard_total_red;

      saved_sideboard_total_blue = 0;
      saved_sideboard_total_black = 0;
      saved_sideboard_total_none = 0;
      saved_sideboard_total_red = 0;
      saved_sideboard_total_white = 0;
      saved_sideboard_total_green = 0;

      AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_CLEARDECK, s_C_lear_sideboard_10031374);
      DeleteMenu(global_sideboardsurface_popup, RES_MENU_RESTOREDECK, 0);
      SendMessageA(global_unknown_10144b04, 0x401, 0, 0);
      break;

    case RES_MENU_LOADDECK:
      SendMessageA(global_main_hwnd, WM_COMMAND, RES_MAINMENU_LOADDECK, 0);
      break;

    case RES_MENU_SAVEDECK:
      SendMessageA(global_main_hwnd, WM_COMMAND, RES_MAINMENU_SAVEDECK, 0);
      break;

    case RES_MENU_EXIT:
      SendMessageA(global_main_hwnd, WM_CLOSE, 0, 0);
      break;

    case RES_MENU_MUSIC:
      SendMessageA(global_main_hwnd, WM_COMMAND, RES_MAINMENU_MUSIC, 0);
      break;

    case RES_MENU_EFFECTS:
      SendMessageA(global_main_hwnd, WM_COMMAND, RES_MAINMENU_EFFECTS, 0);
      break;

    case RES_MENU_MINIMIZE:
      SendMessageA(global_main_hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
      break;
    }
    return 0;

  case WM_CREATE:
    SetWindowTextA(hwnd, s_Sideboard_10031388);
    global_sideboardsurface_popup = CreatePopupMenu();
    AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_NEWDECK, s__New_deck_10031394);
    AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_LOADDECK, s__Load_deck_100313a0);
    AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_SAVEDECK, s__Save_deck_100313ac);
    AppendMenuA(global_sideboardsurface_popup, MF_SEPARATOR, 0, NULL);
    AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_CONSOLIDATE, s__Consolidate_duplicate_cards_sid_100313b8);
    AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_CLEARDECK, s_C_lear_Sideboard_100313dc);
    AppendMenuA(global_sideboardsurface_popup, MF_SEPARATOR, 0, NULL);
    AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_MINIMIZE, s_M_inimize_100313f0);
    AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_MUSIC, s__Music_100313fc);
    AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_EFFECTS, s_Sound__Effects_10031404);
    AppendMenuA(global_sideboardsurface_popup, MF_ENABLED, RES_MENU_EXIT, s_E_xit_deck_builder_10031414);
    return 0;

  case WM_DESTROY:
    DestroyMenu(global_sideboardsurface_popup);
    return 0;

  case WM_ERASEBKGND:
    s.local_48 = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.local_48);
    GetClientRect(hwnd, &s.local_44);
    FillRect(s.local_48, &s.local_44, global_create_brush_6);
    return 1;

  case WM_INITMENU:
    CheckMenuItem(global_sideboardsurface_popup, RES_MENU_CONSOLIDATE, ((unsigned int)global_cfg_consolidate >= 1) ? MF_CHECKED : 0);
    CheckMenuItem(global_sideboardsurface_popup, RES_MENU_EFFECTS, ((unsigned int)global_cfg.effects >= 1) ? MF_CHECKED : 0);
    CheckMenuItem(global_sideboardsurface_popup, RES_MENU_MUSIC, ((unsigned int)global_cfg.music >= 1) ? MF_CHECKED : 0);
    return 0;

  case WM_LBUTTONDOWN:
    BringWindowToTop(hwnd);
    SetFocus(global_main_hwnd);
    return 0;

  case WM_PAINT:
    s.local_8c = BeginPaint(hwnd, &s.local_88);
    ApplyCardArtPaletteToDc(s.local_8c);
    DrawGroupBoxWithLabel(s.local_8c, &sideboard_group_rects[0], s_None_10031428);
    DrawGroupBoxWithLabel(s.local_8c, &sideboard_group_rects[1], s_Black_10031430);
    DrawGroupBoxWithLabel(s.local_8c, &sideboard_group_rects[2], s_Blue_10031438);
    DrawGroupBoxWithLabel(s.local_8c, &sideboard_group_rects[5], s_Red_10031440);
    DrawGroupBoxWithLabel(s.local_8c, &sideboard_group_rects[3], s_Green_10031444);
    DrawGroupBoxWithLabel(s.local_8c, &sideboard_group_rects[4], s_White_1003144c);
    EndPaint(hwnd, &s.local_88);
    return 0;

  case WM_RBUTTONDOWN:
    s.local_94.x = (int)((unsigned)lparam & 0xFFFF);
    s.local_94.y = (int)((unsigned)lparam >> 16);
    ClientToScreen(hwnd, &s.local_94);
    TrackPopupMenu(global_sideboardsurface_popup, TPM_RIGHTBUTTON, s.local_94.x, s.local_94.y, 0, hwnd, NULL);
    return 0;

  case WM_SIZE:
    s.iVar3 = (int)(((unsigned)lparam & 0xFFFF) - 0x28) / 3;
    s.iVar4 = (int)(((unsigned)lparam >> 16) - 0x1e) / 2;

    SetRect(&sideboard_group_rects[0], 10, 10, s.iVar3 + 10, s.iVar4 + 10);
    s.iVar1 = s.iVar3 + 0x14;
    SetRect(&sideboard_group_rects[1], s.iVar1, 10, s.iVar3 + s.iVar1, s.iVar4 + 10);
    s.iVar1 = s.iVar1 + s.iVar3 + 10;
    SetRect(&sideboard_group_rects[4], s.iVar1, 10, s.iVar3 + s.iVar1, s.iVar4 + 10);

    s.iVar1 = s.iVar4 + 0x14;
    SetRect(&sideboard_group_rects[2], 10, s.iVar1, s.iVar3 + 10, s.iVar1 + s.iVar4);
    s.iVar2 = s.iVar3 + 0x14;
    SetRect(&sideboard_group_rects[3], s.iVar2, s.iVar1, s.iVar3 + s.iVar2, s.iVar1 + s.iVar4);
    s.iVar2 = s.iVar2 + s.iVar3 + 10;
    SetRect(&sideboard_group_rects[5], s.iVar2, s.iVar1, s.iVar3 + s.iVar2, s.iVar1 + s.iVar4);

    SendMessageA(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: DECKDLL 0x10003f6a
LRESULT CALLBACK wndproc_TradeSurfaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  // Trade "clear/restore" saved totals.
  // GLOBAL: DECKDLL 0x1003107c
  static int saved_trade_total_deck = 0;
  // GLOBAL: DECKDLL 0x10031080
  static int saved_trade_total_none = 0;
  // GLOBAL: DECKDLL 0x10031084
  static int saved_trade_total_black = 0;
  // GLOBAL: DECKDLL 0x10031088
  static int saved_trade_total_blue = 0;
  // GLOBAL: DECKDLL 0x1003108c
  static int saved_trade_total_green = 0;
  // GLOBAL: DECKDLL 0x10031090
  static int saved_trade_total_white = 0;
  // GLOBAL: DECKDLL 0x10031094
  static int saved_trade_total_red = 0;

  struct
  {
    int pad0; /* matches [ebp-0xa8] */
    int pad1;
    int iVar1;
    int iVar2;
    int iVar3;
    int iVar4;
    POINT local_8c;
    HDC local_84;
    PAINTSTRUCT local_80;
    HDC local_40;
    RECT local_3c;
    HDC local_2c;
    POINT local_28;
    LRESULT local_20;
    HWND local_1c;
    HMENU local_18;
    HDC local_14;
    int local_10;
    int local_c;
    HWND local_8;
  } s;

  switch (msg)
  {
  case 0x401:
    s.local_c = 1;
    destroy_child_windows(hwnd);
    for (s.local_10 = 0; s.local_10 < global_edited_deck.trade[0].total; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031458, s_Empty_10031454,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x6, global_hinstance,
                                    (LPVOID)global_edited_deck.trade[0].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < global_edited_deck.trade[1].total; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031470, s_Empty_1003146c,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x0, global_hinstance,
                                    (LPVOID)global_edited_deck.trade[1].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < global_edited_deck.trade[2].total; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031488, s_Empty_10031484,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x1, global_hinstance,
                                    (LPVOID)global_edited_deck.trade[2].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < global_edited_deck.trade[3].total; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100314a0, s_Empty_1003149c,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x2, global_hinstance,
                                    (LPVOID)global_edited_deck.trade[3].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < global_edited_deck.trade[4].total; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100314b8, s_Empty_100314b4,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x3, global_hinstance,
                                    (LPVOID)global_edited_deck.trade[4].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < global_edited_deck.trade[6].total; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100314d0, s_Empty_100314cc,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x5, global_hinstance,
                                    (LPVOID)global_edited_deck.trade[6].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < global_edited_deck.trade[5].total; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100314e8, s_Empty_100314e4,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x4, global_hinstance,
                                    (LPVOID)global_edited_deck.trade[5].entries[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }

    SendMessageA(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    return s.local_c;

  case 0x4c8:
    s.local_20 = 1;
    s.local_14 = (HDC)wparam;
    s.local_28.x = (int)((unsigned)lparam & 0xFFFF);
    s.local_28.y = (int)((unsigned)lparam >> 16) & 0xFFFF;
    s.local_18 = (HMENU)pick_group_id_from_point(&trade_group_rects[0], 7, &s.local_28);
    s.local_1c = CreateWindowExA(0, s_MAGICDECK_CardClass_10031500, s_Empty_100314fc,
                                  WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                  s.local_28.x, s.local_28.y, global_smallcard_width,
                                  global_smallcard_height, hwnd, s.local_18, global_hinstance,
                                  (LPVOID)s.local_14);
    if (s.local_1c != NULL)
    {
      BringWindowToTop(s.local_1c);
      append_trade_bucket_entry((int)s.local_18, (int)s.local_14, 1, &global_edited_deck);
      SendMessageA(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    }
    else
    {
      s.local_20 = 0;
    }

    if ((((saved_trade_total_deck == 0) && (saved_trade_total_none == 0)) && (saved_trade_total_black == 0)) &&
        (((saved_trade_total_blue == 0 && (saved_trade_total_green == 0)) &&
          ((saved_trade_total_white == 0 && (saved_trade_total_red == 0))))))
      return s.local_20;

    load_text(s_menus_10031528, s_DECKSURFACE_TRADE_10031514);
    AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_CLEARDECK, text_lines[1]);
    DeleteMenu(global_tradesurface_popup, RES_MENU_RESTOREDECK, 0);

    saved_trade_total_deck = 0;
    saved_trade_total_none = 0;
    saved_trade_total_black = 0;
    saved_trade_total_blue = 0;
    saved_trade_total_green = 0;
    saved_trade_total_white = 0;
    saved_trade_total_red = 0;
    return s.local_20;

  case WM_COMMAND:
    switch ((unsigned)wparam & 0xFFFF)
    {
    case RES_MENU_CONSOLIDATE:
      global_cfg_consolidate = (int)(global_cfg_consolidate == 0);
      SendMessageA(global_decksurface_hwnd, 0x401, 0, 0);
      SendMessageA(global_unknown_10144b04, 0x401, 0, 0);
      SendMessageA(global_unknown_10125a3c, 0x401, 0, 0);
      break;

    case RES_MENU_REFRESH:
      layout_child_smallcard_windows(hwnd, &trade_group_rects[0], 7);
      break;

    case RES_MENU_CLEARDECK:
      if (((((global_edited_deck.trade[0].total != 0) || (global_edited_deck.trade[1].total != 0)) || (global_edited_deck.trade[2].total != 0)) ||
           ((global_edited_deck.trade[3].total != 0 || (global_edited_deck.trade[4].total != 0)))) ||
          ((global_edited_deck.trade[5].total != 0 || (global_edited_deck.trade[6].total != 0))))
      {
        saved_trade_total_deck = global_edited_deck.trade[0].total;
        saved_trade_total_none = global_edited_deck.trade[1].total;
        saved_trade_total_black = global_edited_deck.trade[2].total;
        saved_trade_total_blue = global_edited_deck.trade[3].total;
        saved_trade_total_green = global_edited_deck.trade[4].total;
        saved_trade_total_white = global_edited_deck.trade[5].total;
        saved_trade_total_red = global_edited_deck.trade[6].total;

        global_edited_deck.trade[3].total = 0;
        global_edited_deck.trade[2].total = 0;
        global_edited_deck.trade[1].total = 0;
        global_edited_deck.trade[0].total = 0;
        global_edited_deck.trade[6].total = 0;
        global_edited_deck.trade[5].total = 0;
        global_edited_deck.trade[4].total = 0;

        load_text(s_menus_10031544, s_DECKSURFACE_TRADE_10031530);
        DeleteMenu(global_tradesurface_popup, RES_MENU_CLEARDECK, 0);
        AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_RESTOREDECK, text_lines[2]);
        SendMessageA(global_unknown_10125a3c, 0x401, 0, 0);
      }
      break;

    case RES_MENU_RESTOREDECK:
      global_edited_deck.trade[0].total = saved_trade_total_deck;
      global_edited_deck.trade[1].total = saved_trade_total_none;
      global_edited_deck.trade[2].total = saved_trade_total_black;
      global_edited_deck.trade[3].total = saved_trade_total_blue;
      global_edited_deck.trade[4].total = saved_trade_total_green;
      global_edited_deck.trade[5].total = saved_trade_total_white;
      global_edited_deck.trade[6].total = saved_trade_total_red;

      saved_trade_total_blue = 0;
      saved_trade_total_black = 0;
      saved_trade_total_none = 0;
      saved_trade_total_deck = 0;
      saved_trade_total_red = 0;
      saved_trade_total_white = 0;
      saved_trade_total_green = 0;

      load_text(s_menus_10031560, s_DECKSURFACE_TRADE_1003154c);
      AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_CLEARDECK, text_lines[1]);
      DeleteMenu(global_tradesurface_popup, RES_MENU_RESTOREDECK, 0);
      SendMessageA(global_unknown_10125a3c, 0x401, 0, 0);
      break;

    case RES_MENU_LOADDECK:
      SendMessageA(global_main_hwnd, WM_COMMAND, RES_MAINMENU_LOADDECK, 0);
      break;

    case RES_MENU_SAVEDECK:
      SendMessageA(global_main_hwnd, WM_COMMAND, RES_MAINMENU_SAVEDECK, 0);
      break;

    case RES_MENU_EXIT:
      SendMessageA(global_main_hwnd, WM_CLOSE, 0, 0);
      break;

    case RES_MENU_MUSIC:
      SendMessageA(global_main_hwnd, WM_COMMAND, RES_MAINMENU_MUSIC, 0);
      break;

    case RES_MENU_EFFECTS:
      SendMessageA(global_main_hwnd, WM_COMMAND, RES_MAINMENU_EFFECTS, 0);
      break;

    case RES_MENU_MINIMIZE:
      SendMessageA(global_main_hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
      break;
    }
    return 0;

  case WM_CREATE:
    load_text(s_menus_1003157c, s_DECKSURFACE_TRADE_10031568);
    SetWindowTextA(hwnd, text_lines[0]);
    global_tradesurface_popup = CreatePopupMenu();
    AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_NEWDECK, text_lines[3]);
    AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_LOADDECK, text_lines[4]);
    AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_SAVEDECK, text_lines[5]);
    AppendMenuA(global_tradesurface_popup, MF_SEPARATOR, 0, NULL);
    AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_CONSOLIDATE, text_lines[6]);
    AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_CLEARDECK, text_lines[1]);
    AppendMenuA(global_tradesurface_popup, MF_SEPARATOR, 0, NULL);
    AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_MINIMIZE, text_lines[7]);
    AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_MUSIC, text_lines[8]);
    AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_EFFECTS, text_lines[9]);
    AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_EXIT, text_lines[10]);
    return 0;

  case WM_DESTROY:
    DestroyMenu(global_tradesurface_popup);
    return 0;

  case WM_ERASEBKGND:
    s.local_40 = (HDC)wparam;
    ApplyCardArtPaletteToDc((HDC)wparam);
    GetClientRect(hwnd, &s.local_3c);
    FillRect(s.local_40, &s.local_3c, global_create_brush_6);
    return 1;

  case WM_INITMENU:
    CheckMenuItem(global_tradesurface_popup, RES_MENU_CONSOLIDATE, ((global_cfg_consolidate == 0) - 1) & MF_CHECKED);
    CheckMenuItem(global_tradesurface_popup, RES_MENU_EFFECTS, ((global_cfg.effects == '\0') - 1) & MF_CHECKED);
    CheckMenuItem(global_tradesurface_popup, RES_MENU_MUSIC, ((global_cfg.music == '\0') - 1) & MF_CHECKED);
    return 0;

  case WM_LBUTTONDOWN:
    BringWindowToTop(hwnd);
    SetFocus(global_main_hwnd);
    return 0;

  case WM_PAINT:
    s.local_84 = BeginPaint(hwnd, &s.local_80);
    ApplyCardArtPaletteToDc(s.local_84);
    DrawGroupBoxWithLabel(s.local_84, &trade_group_rects[6], s_Deck_10031584);
    DrawGroupBoxWithLabel(s.local_84, &trade_group_rects[0], s_None_1003158c);
    DrawGroupBoxWithLabel(s.local_84, &trade_group_rects[1], s_Black_10031594);
    DrawGroupBoxWithLabel(s.local_84, &trade_group_rects[2], s_Blue_1003159c);
    DrawGroupBoxWithLabel(s.local_84, &trade_group_rects[5], s_Red_100315a4);
    DrawGroupBoxWithLabel(s.local_84, &trade_group_rects[3], s_Green_100315a8);
    DrawGroupBoxWithLabel(s.local_84, &trade_group_rects[4], s_White_100315b0);
    EndPaint(hwnd, &s.local_80);
    return 0;

  case WM_RBUTTONDOWN:
    s.local_8c.x = (int)((unsigned)lparam & 0xFFFF);
    s.local_8c.y = (int)((unsigned)lparam >> 16);
    ClientToScreen(hwnd, &s.local_8c);
    TrackPopupMenu(global_tradesurface_popup, TPM_RIGHTBUTTON, s.local_8c.x, s.local_8c.y, 0, hwnd, NULL);
    return 0;

  case 0x466:
    s.local_2c = (HDC)wparam;
    invalidate_wanted_window(hwnd, wparam);
    return 0;

  case WM_SIZE:
    s.iVar1 = ((int)((unsigned)lparam & 0xFFFF) - 0x32);
    s.iVar2 = (int)(s.iVar1 + ((unsigned)s.iVar1 >> 31 & 3U)) >> 2;
    s.iVar3 = (int)(((unsigned)lparam >> 16) - 0x1e) / 2;

    SetRect(&trade_group_rects[6], 10, 10, s.iVar2 + 10, s.iVar3 + 10);
    s.iVar1 = s.iVar2 + 0x14;
    SetRect(&trade_group_rects[0], s.iVar1, 10, s.iVar1 + s.iVar2, s.iVar3 + 10);
    s.iVar1 = s.iVar1 + s.iVar2 + 10;
    SetRect(&trade_group_rects[1], s.iVar1, 10, s.iVar1 + s.iVar2, s.iVar3 + 10);
    s.iVar1 = s.iVar1 + s.iVar2 + 10;
    SetRect(&trade_group_rects[4], s.iVar1, 10, s.iVar1 + s.iVar2, s.iVar3 + 10);

    s.iVar4 = s.iVar2 + 0x14;
    s.iVar1 = s.iVar3 + 0x14;
    SetRect(&trade_group_rects[2], s.iVar4, s.iVar1, s.iVar4 + s.iVar2, s.iVar1 + s.iVar3);
    s.iVar4 = s.iVar4 + s.iVar2 + 10;
    SetRect(&trade_group_rects[3], s.iVar4, s.iVar1, s.iVar4 + s.iVar2, s.iVar1 + s.iVar3);
    s.iVar4 = s.iVar4 + s.iVar2 + 10;
    SetRect(&trade_group_rects[5], s.iVar4, s.iVar1, s.iVar4 + s.iVar2, s.iVar1 + s.iVar3);

    SendMessageA(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}
