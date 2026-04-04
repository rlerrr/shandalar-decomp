#include "deckdll.h"
#include "full_deck.h"
#include "mystdbool.h"

extern HINSTANCE global_hinstance;
extern DBFlags global_db_flags_1;
extern DBFlags global_db_flags_2;
extern bool global_cfg_consolidate;
extern signed char global_cfg_effects;
extern signed char global_cfg_music;
extern char global_cfg_player_name[];
extern char global_cfg_email[];
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
extern void __cdecl add_or_increment_sideboard_bucket_entry(int param_1, int param_2, int param_3, FullDeck *param_4);
extern void __cdecl append_trade_bucket_entry(int which, int csvid, int num, FullDeck *deck_base);
extern void ApplyCardArtPaletteToDc(HDC hdc);

// Sideboard and trade groups are stored in global_edited_deck.
#define DAT_101a0ce8 (global_edited_deck.sideboard[0].entries)
#define DAT_101a0d60 (global_edited_deck.sideboard[0].total)
#define DAT_101a0d64 (global_edited_deck.sideboard[1].entries)
#define DAT_101a0ddc (global_edited_deck.sideboard[1].total)
#define DAT_101a0de0 (global_edited_deck.sideboard[2].entries)
#define DAT_101a0e58 (global_edited_deck.sideboard[2].total)
#define DAT_101a0e5c (global_edited_deck.sideboard[3].entries)
#define DAT_101a0ed4 (global_edited_deck.sideboard[3].total)
#define DAT_101a0ed8 (global_edited_deck.sideboard[4].entries)
#define DAT_101a0f50 (global_edited_deck.sideboard[4].total)
#define DAT_101a0f54 (global_edited_deck.sideboard[5].entries)
#define DAT_101a0fcc (global_edited_deck.sideboard[5].total)

#define DAT_101a0fd0 (global_edited_deck.trade[0].entries)
#define DAT_101a1048 (global_edited_deck.trade[0].total)
#define DAT_101a104c (global_edited_deck.trade[1].entries)
#define DAT_101a10c4 (global_edited_deck.trade[1].total)
#define DAT_101a10c8 (global_edited_deck.trade[2].entries)
#define DAT_101a1140 (global_edited_deck.trade[2].total)
#define DAT_101a1144 (global_edited_deck.trade[3].entries)
#define DAT_101a11bc (global_edited_deck.trade[3].total)
#define DAT_101a11c0 (global_edited_deck.trade[4].entries)
#define DAT_101a1238 (global_edited_deck.trade[4].total)
#define DAT_101a123c (global_edited_deck.trade[5].entries)
#define DAT_101a12b4 (global_edited_deck.trade[5].total)
#define DAT_101a12b8 (global_edited_deck.trade[6].entries)
#define DAT_101a1330 (global_edited_deck.trade[6].total)

// GLOBAL: DECKDLL 0x101a8a6c
HWND global_decksurface_hwnd;
// GLOBAL: DECKDLL 0x100f2768
static HMENU global_decksurface_popup;

// GLOBAL: DECKDLL 0x100f276c
static int DAT_100f276c;

// GLOBAL: DECKDLL 0x100f2770
static RECT DAT_100f2770;
// GLOBAL: DECKDLL 0x100f2780
static RECT DAT_100f2780;
// GLOBAL: DECKDLL 0x100f2790
static RECT DAT_100f2790;
// GLOBAL: DECKDLL 0x100f27a0
static RECT DAT_100f27a0;
// GLOBAL: DECKDLL 0x100f27b0
static RECT DAT_100f27b0;
// GLOBAL: DECKDLL 0x100f27c0
static RECT DAT_100f27c0;
// GLOBAL: DECKDLL 0x100f27d0
static RECT DAT_100f27d0;
// GLOBAL: DECKDLL 0x100f27e0
static RECT DAT_100f27e0;
// GLOBAL: DECKDLL 0x100f27f0
static RECT DAT_100f27f0;
// GLOBAL: DECKDLL 0x100f2800
static RECT DAT_100f2800;
// GLOBAL: DECKDLL 0x100f2810
static RECT DAT_100f2810;
// GLOBAL: DECKDLL 0x100f2820
static RECT DAT_100f2820;
// GLOBAL: DECKDLL 0x100f2830
static RECT DAT_100f2830;
// GLOBAL: DECKDLL 0x100f2840
static HMENU global_tradesurface_popup;
// GLOBAL: DECKDLL 0x100f2844
static HMENU DAT_100f2844;


// Sideboard surface strings (class names, menu strings, labels).
// GLOBAL: DECKDLL 0x10031214
static const char DAT_10031214[4] = "";
// GLOBAL: DECKDLL 0x10031218
static const char s_MAGICDECK_CardClass_10031218[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003122c
static const char DAT_1003122c[4] = "";
// GLOBAL: DECKDLL 0x10031230
static const char s_MAGICDECK_CardClass_10031230[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x10031244
static const char DAT_10031244[4] = "";
// GLOBAL: DECKDLL 0x10031248
static const char s_MAGICDECK_CardClass_10031248[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003125c
static const char DAT_1003125c[4] = "";
// GLOBAL: DECKDLL 0x10031260
static const char s_MAGICDECK_CardClass_10031260[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x10031274
static const char DAT_10031274[4] = "";
// GLOBAL: DECKDLL 0x10031278
static const char s_MAGICDECK_CardClass_10031278[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003128c
static const char DAT_1003128c[4] = "";
// GLOBAL: DECKDLL 0x10031290
static const char s_MAGICDECK_CardClass_10031290[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100312a4
static const char DAT_100312a4[4] = "";
// GLOBAL: DECKDLL 0x100312a8
static const char s_MAGICDECK_CardClass_100312a8[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100312bc
static const char DAT_100312bc[4] = "";
// GLOBAL: DECKDLL 0x100312c0
static const char s_MAGICDECK_CardClass_100312c0[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100312d4
static const char DAT_100312d4[4] = "";
// GLOBAL: DECKDLL 0x100312d8
static const char s_MAGICDECK_CardClass_100312d8[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100312ec
static const char DAT_100312ec[4] = "";
// GLOBAL: DECKDLL 0x100312f0
static const char s_MAGICDECK_CardClass_100312f0[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x10031304
static const char DAT_10031304[4] = "";
// GLOBAL: DECKDLL 0x10031308
static const char s_MAGICDECK_CardClass_10031308[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003131c
static const char DAT_1003131c[4] = "";
// GLOBAL: DECKDLL 0x10031320
static const char s_MAGICDECK_CardClass_10031320[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x10031334
static const char DAT_10031334[4] = "";
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
static const char DAT_10031454[4] = "";
// GLOBAL: DECKDLL 0x10031458
static const char s_MAGICDECK_CardClass_10031458[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003146c
static const char DAT_1003146c[4] = "";
// GLOBAL: DECKDLL 0x10031470
static const char s_MAGICDECK_CardClass_10031470[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x10031484
static const char DAT_10031484[4] = "";
// GLOBAL: DECKDLL 0x10031488
static const char s_MAGICDECK_CardClass_10031488[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x1003149c
static const char DAT_1003149c[4] = "";
// GLOBAL: DECKDLL 0x100314a0
static const char s_MAGICDECK_CardClass_100314a0[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100314b4
static const char DAT_100314b4[4] = "";
// GLOBAL: DECKDLL 0x100314b8
static const char s_MAGICDECK_CardClass_100314b8[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100314cc
static const char DAT_100314cc[4] = "";
// GLOBAL: DECKDLL 0x100314d0
static const char s_MAGICDECK_CardClass_100314d0[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100314e4
static const char DAT_100314e4[4] = "";
// GLOBAL: DECKDLL 0x100314e8
static const char s_MAGICDECK_CardClass_100314e8[20] = "MAGICDECK_CardClass";
// GLOBAL: DECKDLL 0x100314fc
static const char DAT_100314fc[4] = "";
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
static const char DAT_10031584[8] = "Deck";
// GLOBAL: DECKDLL 0x1003158c
static const char DAT_1003158c[8] = "None";
// GLOBAL: DECKDLL 0x10031594
static const char s_Black_10031594[8] = "Black";
// GLOBAL: DECKDLL 0x1003159c
static const char DAT_1003159c[8] = "Blue";
// GLOBAL: DECKDLL 0x100315a4
static const char DAT_100315a4[4] = "Red";
// GLOBAL: DECKDLL 0x100315a8
static const char s_Green_100315a8[8] = "Green";
// GLOBAL: DECKDLL 0x100315b0
static const char s_White_100315b0[8] = "White";



// FUNCTION: DECKDLL 0x10005051
static void __cdecl
DrawGroupBoxWithLabel(HDC param_1, RECT *param_2, char *param_3)
{
  size_t sVar1;
  DWORD DVar2;
  SIZE *lpsz;
  SIZE local_28;
  RECT local_20;
  int local_10;
  HPEN local_c;
  HPEN local_8;

  CopyRect(&local_20, param_2);
  lpsz = &local_28;
  sVar1 = strlen(param_3);
  GetTextExtentPointA(param_1, param_3, sVar1, lpsz);
  DVar2 = GetSysColor(0x10);
  local_8 = CreatePen(0, 1, DVar2);
  DVar2 = GetSysColor(0x16);
  local_c = CreatePen(0, 1, DVar2);
  local_10 = SaveDC(param_1);
  SetBkMode(param_1, 1);
  OffsetRect(&local_20, 1, 1);
  DVar2 = GetSysColor(0x16);
  SetTextColor(param_1, DVar2);
  SelectObject(param_1, local_c);
  MoveToEx(param_1, local_20.left, local_20.top + local_28.cy / 2, (LPPOINT)0x0);
  LineTo(param_1, local_20.left + 10, local_20.top + local_28.cy / 2);
  sVar1 = strlen(param_3);
  TextOutA(param_1, local_20.left + 0xe, local_20.top, param_3, sVar1);
  MoveToEx(param_1, local_20.left + local_28.cx + 0x12, local_20.top + local_28.cy / 2, (LPPOINT)0x0);
  LineTo(param_1, local_20.right, local_20.top + local_28.cy / 2);
  LineTo(param_1, local_20.right, local_20.bottom);
  LineTo(param_1, local_20.left, local_20.bottom);
  LineTo(param_1, local_20.left, local_20.top + local_28.cy / 2);
  OffsetRect(&local_20, -1, -1);
  DVar2 = GetSysColor(0x10);
  SetTextColor(param_1, DVar2);
  SelectObject(param_1, local_8);
  MoveToEx(param_1, local_20.left, local_20.top + local_28.cy / 2, (LPPOINT)0x0);
  LineTo(param_1, local_20.left + 10, local_20.top + local_28.cy / 2);
  sVar1 = strlen(param_3);
  TextOutA(param_1, local_20.left + 0xe, local_20.top, param_3, sVar1);
  MoveToEx(param_1, local_20.left + local_28.cx + 0x12, local_20.top + local_28.cy / 2, (LPPOINT)0x0);
  LineTo(param_1, local_20.right, local_20.top + local_28.cy / 2);
  LineTo(param_1, local_20.right, local_20.bottom);
  LineTo(param_1, local_20.left, local_20.bottom);
  LineTo(param_1, local_20.left, local_20.top + local_28.cy / 2);
  RestoreDC(param_1, local_10);
  DeleteObject(local_8);
  DeleteObject(local_c);
}

// FUNCTION: DECKDLL 0x10005317
static int __cdecl
FUN_10005317(int param_1, int param_2, POINT *param_3)
{
  struct
  {
    int local_e0;
    int local_dc;
    int local_d8;
    int local_d4;
    int aiStack_d0[50];
    int local_8;
  } s;

  s.local_d8 = 0;
  s.local_8 = 0;
  while ((s.local_d8 < param_2) && (s.local_8 == 0))
  {
    if (PtInRect((RECT *)(s.local_d8 * 0x10 + param_1), *param_3) == 0)
    {
      if (abs(param_3->x - *(int *)(s.local_d8 * 0x10 + param_1)) <
          abs(param_3->x - *(int *)(s.local_d8 * 0x10 + 8 + param_1)))
        s.aiStack_d0[s.local_d8 * 2] = abs(param_3->x - *(int *)(s.local_d8 * 0x10 + param_1));
      else
        s.aiStack_d0[s.local_d8 * 2] = abs(param_3->x - *(int *)(s.local_d8 * 0x10 + 8 + param_1));

      if (abs(param_3->y - *(int *)(s.local_d8 * 0x10 + 4 + param_1)) <
          abs(param_3->y - *(int *)(s.local_d8 * 0x10 + 0xc + param_1)))
        s.aiStack_d0[s.local_d8 * 2 + 1] = abs(param_3->y - *(int *)(s.local_d8 * 0x10 + 4 + param_1));
      else
        s.aiStack_d0[s.local_d8 * 2 + 1] = abs(param_3->y - *(int *)(s.local_d8 * 0x10 + 0xc + param_1));
    }
    else
    {
      s.local_d4 = s.local_d8;
      s.local_8 = 1;
    }
    s.local_d8 = s.local_d8 + 1;
  }

  if (s.local_8 == 0)
  {
    s.local_e0 = 1000;
    for (s.local_d8 = 0; s.local_d8 < param_2; s.local_d8 = s.local_d8 + 1)
    {
      if (s.aiStack_d0[s.local_d8 * 2] < s.local_e0)
      {
        s.local_e0 = s.aiStack_d0[s.local_d8 * 2];
        s.local_d4 = s.local_d8;
        s.local_dc = s.aiStack_d0[s.local_d8 * 2 + 1];
      }
      else if ((s.aiStack_d0[s.local_d8 * 2] == s.local_e0) &&
               (s.aiStack_d0[s.local_d8 * 2 + 1] < s.local_dc))
      {
        s.local_dc = s.aiStack_d0[s.local_d8 * 2 + 1];
        s.local_d4 = s.local_d8;
      }
    }
  }
  return s.local_d4;
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
FUN_1000563b(HWND param_1, int param_2, int param_3)
{
  int iVar1;
  LRESULT LVar2;
  HWND local_c;
  HWND local_8;

  local_8 = (HWND)0x0;
  for (local_c = GetTopWindow(param_1); local_c != (HWND)0x0; local_c = GetWindow(local_c, 2))
  {
    iVar1 = GetDlgCtrlID(local_c);
    if (iVar1 == param_2)
    {
      LVar2 = SendMessageA(local_c, 0x400, 0, 0);
      if (LVar2 == param_3)
        local_8 = local_c;
    }
  }
  return local_8;
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
FUN_100058c0(HWND param_1, int param_2, int param_3)
{
  int iVar1;
  int iVar2;
  HWND local_b4;
  HWND local_ac;
  int local_a8;
  int aiStack_a4[40];

  iVar1 = global_smallcard_height * 0x12;
  for (local_a8 = 0; local_a8 < param_3; local_a8 = local_a8 + 1)
  {
    aiStack_a4[local_a8 * 2] =
        *(int *)(local_a8 * 0x10 + param_2) +
        ((*(int *)(local_a8 * 0x10 + 8 + param_2) - *(int *)(local_a8 * 0x10 + param_2)) -
         global_smallcard_width) /
            2;
    aiStack_a4[local_a8 * 2 + 1] =
        *(int *)(local_a8 * 0x10 + 4 + param_2) + global_smallcard_height / 5;
  }

  local_b4 = (HWND)0x0;
  for (local_ac = GetTopWindow(param_1); local_ac != (HWND)0x0; local_ac = GetWindow(local_ac, 2))
    local_b4 = local_ac;

  for (local_ac = local_b4; local_ac != (HWND)0x0; local_ac = GetWindow(local_ac, 3))
  {
    for (local_a8 = 0; local_a8 < param_3; local_a8 = local_a8 + 1)
    {
      iVar2 = GetDlgCtrlID(local_ac);
      if (iVar2 == local_a8)
      {
        SetWindowPos(local_ac, (HWND)0x0, aiStack_a4[local_a8 * 2], aiStack_a4[local_a8 * 2 + 1],
                     global_smallcard_width, global_smallcard_height, 4);
        aiStack_a4[local_a8 * 2 + 1] = aiStack_a4[local_a8 * 2 + 1] + iVar1 / 100;
      }
    }
  }
}

// FUNCTION: DECKDLL 0x100012b7
LRESULT CALLBACK
wndproc_DeckSurfaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  int c;
  int d;
  int i;
  int j;
  int mosx;
  int mosy;
  int x;
  int y;
  int dx;
  int dy;
  int tgt_dx;
  RECT r;
  HDC hdc;
  HDC hdc_local;
  // GLOBAL: DECKDLL 0x1003105c
  static int cleared_global_deck_num_entries = 0;
  // GLOBAL: DECKDLL 0x10031060
  static int cleared_global_deck_num_cards = 0;
  HDC mosaics[5];
  BITMAP bmp;

  switch (msg)
  {
  case WM_CREATE:
    if (global_db_flags_1 & (DBFLAGS_GAUNTLET | DBFLAGS_NOCARDCOUNTCHECK))
    {
      clear_packs();
      clear_packs_copy();
    }

    global_decksurface_popup = CreatePopupMenu();

    if (global_db_flags_1 & DBFLAGS_STANDALONE)
    {
      load_text("Menus", "DECKSURFACE_STANDALONE");

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
      load_text("Menus", "DECKSURFACE_GAUNTLET");

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
      load_text("Menus", "DECKSURFACE_ADVENTURE");
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

  case WM_INITMENU:
    CHECKMENU_IF(global_decksurface_popup, RES_MENU_CONSOLIDATE, global_cfg_consolidate);
    CHECKMENU_IF(global_decksurface_popup, RES_MENU_EFFECTS, global_cfg_effects);
    CHECKMENU_IF(global_decksurface_popup, RES_MENU_MUSIC, global_cfg_music);
    return 0;

  case WM_COMMAND:
    switch (LOWORD(wparam))
    {
    case RES_MENU_CONSOLIDATE:
    {
      global_cfg_consolidate = !global_cfg_consolidate;
      SendMessage(global_decksurface_hwnd, 0x8401, 0, 0);
      return 0;
    }

    case RES_MENU_REFRESH:
    {
      RECT r;
      GetClientRect(hwnd, &r);
      InflateRect(&r, -5, -5);
      LockWindowUpdate(global_main_hwnd);
      reset_dim_and_pos_all_cards(hwnd, &r);
      LockWindowUpdate(NULL);
      return 0;
    }

    case RES_MENU_CLEARDECK:
    {
      if (global_edited_deck_num_entries)
      {
        global_deck_was_edited = false;
        memcpy(&global_packs_copy[0][0], &global_packs[0][0], sizeof global_packs);
        clear_packs();

        cleared_global_deck_num_entries = global_edited_deck_num_entries;
        global_edited_deck_num_entries = 0;

        cleared_global_deck_num_cards = global_deck_num_cards;
        global_deck_num_cards = 0;

        RemoveMenu(global_decksurface_popup, RES_MENU_CLEARDECK, MF_BYCOMMAND);
        load_text("Menus", "DECKCLEAR_RESTORE");
        InsertMenu(global_decksurface_popup, RES_MENU_SORTDECK, MF_BYCOMMAND | MF_ENABLED, RES_MENU_RESTOREDECK, text_lines[1]);
        SendMessage(global_decksurface_hwnd, 0x8401, 0, 0);
      }
      return 0;
    }

    case RES_MENU_RESTOREDECK:
    {
      memcpy(&global_packs[0][0], &global_packs_copy[0][0], sizeof global_packs);
      clear_packs_copy();
      global_deck_was_edited = true;

      global_edited_deck_num_entries = cleared_global_deck_num_entries;
      cleared_global_deck_num_entries = 0;

      global_deck_num_cards = cleared_global_deck_num_cards;
      cleared_global_deck_num_cards = 0;

      load_text("Menus", "DECKCLEAR_RESTORE");
      InsertMenu(global_decksurface_popup, RES_MENU_SORTDECK, MF_BYCOMMAND | MF_ENABLED, RES_MENU_CLEARDECK, text_lines[0]);
      DeleteMenu(global_decksurface_popup, RES_MENU_RESTOREDECK, MF_BYCOMMAND);
      SendMessage(global_decksurface_hwnd, 0x8401, 0, 0);
      return 0;
    }

    case RES_MENU_NEWDECK:
    {
      if (!global_deck_was_edited || ask_about_saving_deck())
      {
        global_deckinfo.revision = 1;
        global_deck_num_cards = 0;
        global_edited_deck_num_entries = 0;

        InvalidateRect(global_title_hwnd, NULL, TRUE);

        clear_packs();
        clear_packs_copy();

        if (DeleteMenu(global_decksurface_popup, RES_MENU_RESTOREDECK, MF_BYCOMMAND))
        {
          load_text("Menus", "DECKCLEAR_RESTORE");
          InsertMenu(global_decksurface_popup, RES_MENU_SORTDECK, MF_BYCOMMAND | MF_ENABLED, RES_MENU_CLEARDECK, text_lines[0]);
        }
        SendMessage(global_decksurface_hwnd, 0x8401, 0, 0);

        load_text("Menus", "NEWDECK");
        sprintf(global_deckinfo.deckname, text_lines[0]);

        global_deckinfo.comments[0] = 0;
        global_deckinfo.description[0] = 0;

        strcpy(global_deckinfo.author, global_cfg_player_name);
        strcpy(global_deckinfo.email, global_cfg_email);

        GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, NULL, "dd/MM/yyyy", global_deckinfo.creation_date, 22);

        show_dialog_deckinfo();
      }
      return 0;
    }

    case RES_MENU_LOADDECK:
    {
      if (DeleteMenu(global_decksurface_popup, RES_MENU_RESTOREDECK, MF_BYCOMMAND))
      {
        load_text("Menus", "DECKCLEAR_RESTORE");
        InsertMenu(global_decksurface_popup, RES_MENU_SORTDECK, MF_BYCOMMAND | MF_ENABLED, RES_MENU_CLEARDECK, text_lines[0]);
      }
      SendMessage(global_main_hwnd, WM_COMMAND, RES_MAINMENU_LOADDECK, 0);
      return 0;
    }

    case RES_MENU_SAVEDECK:
    {
      SendMessage(global_main_hwnd, WM_COMMAND, RES_MAINMENU_SAVEDECK, 0);
      return 0;
    }

    case RES_MENU_EXIT:
    {
      SendMessage(global_main_hwnd, WM_CLOSE, 0, 0);
      return 0;
    }

    case RES_MENU_SORTDECK:
    {
      SendMessage(global_decksurface_hwnd, 0x8401, 0, 0);
      return 0;
    }

    case RES_MENU_MUSIC:
    {
      SendMessage(global_main_hwnd, WM_COMMAND, RES_MAINMENU_MUSIC, 0);
      return 0;
    }

    case RES_MENU_EFFECTS:
    {
      SendMessage(global_main_hwnd, WM_COMMAND, RES_MAINMENU_EFFECTS, 0);
      return 0;
    }

    case RES_MENU_MINIMIZE:
    {
      SendMessage(global_main_hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
      SendMessage(GetParent(global_main_hwnd), WM_SYSCOMMAND, SC_MINIMIZE, 0);
      return 0;
    }

    case RES_MENU_COLORINTODECK:
    case RES_MENU_COLOROUTOFDECK:
    {
      global_dlg_parameter = check_colors_inout_edited_deck(LOWORD(wparam) == RES_MENU_COLOROUTOFDECK);
      if (show_dialog_groupmove())
      {
        move_colors_fromto_global_deck(LOWORD(wparam) == RES_MENU_COLORINTODECK, global_dlg_result);
        count_packs();
        SendMessage(global_decksurface_hwnd, 0x8401, 0, 0);
        filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);
      }
      return 0;
    }

    default:
      return 0;
    }

  case WM_DESTROY:
    DESTROY_MENU(global_decksurface_popup);
    return 0;

  case WM_SIZE:
  {
    HWND w = GetTopWindow(hwnd);

    while (w)
    {
      SetWindowPos(w, NULL, 0, 0, global_smallcard_width, global_smallcard_height, SWP_NOZORDER | SWP_NOMOVE);
      w = GetWindow(w, GW_HWNDNEXT);
    }

    if (global_decksurface_hwnd)
      SendMessage(global_decksurface_hwnd, 0x8401, 0, 0);

    return 0;
  }

  case WM_ERASEBKGND:
  {
    HANDLE mosaic_pics[5];
    hdc = wparam;
    ApplyCardArtPaletteToDc(hdc);

    mosaic_pics[0] = global_pic_bldr01c;
    mosaic_pics[1] = global_pic_bldr02c;
    mosaic_pics[2] = global_pic_bldr03c;
    mosaic_pics[3] = global_pic_bldr04c;
    mosaic_pics[4] = global_pic_bldr05c;

    for (i = 0; i < 5; ++i)
      ApplyCardArtPaletteToDc((mosaics[i] = CreateCompatibleDC(hdc)));

    for (i = 0; i < 5; ++i)
      SelectObject(mosaics[i], mosaic_pics[i]);

    GetClientRect(hwnd, &r);
    GetObject(global_pic_bldr01c, sizeof(BITMAP), &bmp);

    dy = r.bottom / 5;
    tgt_dx = bmp.bmWidth * r.bottom / (bmp.bmHeight * 5); // = (width/height) * dy, after reordering for rounding
    for (i = 1; i < 100; ++i)
    {
      dx = r.right / i;
      if (dx < tgt_dx)
      {
        if (i != 1) // already as wide as it can go
        {
          int old_dx = r.right / (i - 1);
          if (abs(tgt_dx - old_dx) < abs(tgt_dx - dx)) // overshot?
            dx = old_dx;
        }
        break;
      }
    }

    for (mosx = 0, x = 0; x < r.right; x += dx, ++mosx)
      for (y = 0, mosy = mosx; y < r.bottom; y += dy, ++mosy)
        StretchBlt(hdc,
                   x, y,
                   dx, dy,
                   mosaics[mosy % 5],
                   0, 0,
                   bmp.bmWidth, bmp.bmHeight,
                   SRCCOPY);

    ReleaseDC(hwnd, hdc);
    for (i = 0; i < 5; ++i)
      DELETE_DC(mosaics[i]);

    return 1;
  }

  case WM_LBUTTONDOWN:
  {
    BringWindowToTop(hwnd);
    SetFocus(global_main_hwnd);
    return 0;
  }

  case WM_RBUTTONDOWN:
  {
    POINT p;
    p.x = GET_X_LPARAM(lparam);
    p.y = GET_Y_LPARAM(lparam);
    ClientToScreen(hwnd, &p);
    TrackPopupMenu(global_decksurface_popup, TPM_RIGHTBUTTON, p.x, p.y, 0, hwnd, NULL);
    return 0;
  }

  case 0x8401:
  {
    destroy_child_windows(hwnd);

    set_smallcard_dimensions();

    LockWindowUpdate(global_main_hwnd);

    if (global_cfg_consolidate)
    {
      for (i = 0; i <= PACK1_MAX; ++i)
        for (j = 0; j <= PACK2_MAX; ++j)
          for (c = 0; c < global_packs[i][j].num; ++c)
            add_smallcard_window(hwnd, global_packs[i][j].table[c].csvid, global_packs[i][j].table[c].amt);
    }
    else
    {
      for (i = 0; i <= PACK1_MAX; ++i)
        for (j = 0; j <= PACK2_MAX; ++j)
          for (c = 0; c < global_packs[i][j].num; ++c)
            for (d = 0; d < global_packs[i][j].table[c].amt; ++d)
              add_smallcard_window(hwnd, global_packs[i][j].table[c].csvid, 1);
    }

    LockWindowUpdate(0);

    SendMessage(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    refresh_numofcards_text();
    return 1;
  }

  case 0x8466:
  {
    invalidate_wanted_window(hwnd, wparam);
    return 0;
  }

  case 0x84c8:
  {
    int rval;
    rval = 1;

    {
      csvid_t csvid = (csvid_t)wparam;

      HWND hwnd_card;

      if (global_cfg_consolidate && (hwnd_card = find_wanted_window(hwnd, csvid)))
      {
        insert_cards_into_deck(csvid, 1, &global_edited_deck);
        SendMessage(hwnd_card, 0x8401, 1 + SendMessage(hwnd_card, 0x8402, 0, 0), 0);
      }
      else
      {
        hwnd_card = CreateWindowEx(0, "CardClass", "",
                                   WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                   GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), global_smallcard_width, global_smallcard_height,
                                   hwnd,
                                   1,
                                   global_hinstance, csvid);
        if (hwnd_card)
        {
          BringWindowToTop(hwnd_card);
          insert_cards_into_deck(csvid, 1, &global_edited_deck);
          SendMessage(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
        }
        else
          rval = 0;
      }
    }

    if (cleared_global_deck_num_entries)
    {
      load_text("Menus", "DECKCLEAR_RESTORE");
      InsertMenu(global_decksurface_popup, RES_MENU_SORTDECK, MF_BYCOMMAND | MF_ENABLED, RES_MENU_CLEARDECK, text_lines[0]);
      RemoveMenu(global_decksurface_popup, RES_MENU_RESTOREDECK, MF_BYCOMMAND);
      cleared_global_deck_num_cards = 0;
      cleared_global_deck_num_entries = 0;
    }
    return rval;
  }

  default:
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }
}


// FUNCTION: DECKDLL 0x10002b0f
LRESULT CALLBACK wndproc_SideboardSurfaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  // GLOBAL: DECKDLL 0x10031064
  static int DAT_10031064;
  // GLOBAL: DECKDLL 0x10031068
  static int DAT_10031068;
  // GLOBAL: DECKDLL 0x1003106c
  static int DAT_1003106c;
  // GLOBAL: DECKDLL 0x10031070
  static int DAT_10031070;
  // GLOBAL: DECKDLL 0x10031074
  static int DAT_10031074;
  // GLOBAL: DECKDLL 0x10031078
  static int DAT_10031078;

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
      for (s.local_10 = 0; DAT_101a0d60 > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031218, DAT_10031214,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x0, global_hinstance,
                                    (LPVOID)DAT_101a0ce8[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, DAT_101a0ce8[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
      for (s.local_10 = 0; DAT_101a0ddc > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031230, DAT_1003122c,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x1, global_hinstance,
                                    (LPVOID)DAT_101a0d64[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, DAT_101a0d64[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
      for (s.local_10 = 0; DAT_101a0e58 > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031248, DAT_10031244,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x2, global_hinstance,
                                    (LPVOID)DAT_101a0de0[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, DAT_101a0de0[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
      for (s.local_10 = 0; DAT_101a0ed4 > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031260, DAT_1003125c,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x3, global_hinstance,
                                    (LPVOID)DAT_101a0e5c[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, DAT_101a0e5c[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
      for (s.local_10 = 0; DAT_101a0fcc > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031278, DAT_10031274,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x5, global_hinstance,
                                    (LPVOID)DAT_101a0f54[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, DAT_101a0f54[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
      for (s.local_10 = 0; DAT_101a0f50 > s.local_10; s.local_10 = s.local_10 + 1)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031290, DAT_1003128c,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x4, global_hinstance,
                                    (LPVOID)DAT_101a0ed8[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          SendMessageA(s.local_8, 0x401, DAT_101a0ed8[s.local_10].DeckEntry_Amount, 0);
        else
          s.local_c = 0;
      }
    }
    else
    {
      for (s.local_10 = 0; DAT_101a0d60 > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; DAT_101a0ce8[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100312a8, DAT_100312a4,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x0, global_hinstance,
                                            (LPVOID)DAT_101a0ce8[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
      for (s.local_10 = 0; DAT_101a0ddc > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; DAT_101a0d64[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100312c0, DAT_100312bc,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x1, global_hinstance,
                                            (LPVOID)DAT_101a0d64[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
      for (s.local_10 = 0; DAT_101a0e58 > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; DAT_101a0de0[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100312d8, DAT_100312d4,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x2, global_hinstance,
                                            (LPVOID)DAT_101a0de0[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
      for (s.local_10 = 0; DAT_101a0ed4 > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; DAT_101a0e5c[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100312f0, DAT_100312ec,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x3, global_hinstance,
                                            (LPVOID)DAT_101a0e5c[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
      for (s.local_10 = 0; DAT_101a0fcc > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; DAT_101a0f54[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031308, DAT_10031304,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x5, global_hinstance,
                                            (LPVOID)DAT_101a0f54[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
      for (s.local_10 = 0; DAT_101a0f50 > s.local_10; s.local_10 = s.local_10 + 1)
      {
        for (s.local_14 = 0; DAT_101a0ed8[s.local_10].DeckEntry_Amount > s.local_14; s.local_14 = s.local_14 + 1)
        {
          if ((s.local_c == 0) ||
              ((s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031320, DAT_1003131c,
                                            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                            0, 0, global_smallcard_width, global_smallcard_height,
                                            hwnd, (HMENU)0x4, global_hinstance,
                                            (LPVOID)DAT_101a0ed8[s.local_10].DeckEntry_csvid)) == NULL))
            s.local_c = 0;
          else
            s.local_c = 1;
        }
      }
    }
    SendMessageA(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    return s.local_c;

  case WM_PAINT:
    s.local_8c = BeginPaint(hwnd, &s.local_88);
    ApplyCardArtPaletteToDc(s.local_8c);
    DrawGroupBoxWithLabel(s.local_8c, (RECT *)&DAT_100f27e0, s_None_10031428);
    DrawGroupBoxWithLabel(s.local_8c, (RECT *)&DAT_100f27f0, s_Black_10031430);
    DrawGroupBoxWithLabel(s.local_8c, (RECT *)&DAT_100f2800, s_Blue_10031438);
    DrawGroupBoxWithLabel(s.local_8c, (RECT *)&DAT_100f2830, s_Red_10031440);
    DrawGroupBoxWithLabel(s.local_8c, (RECT *)&DAT_100f2810, s_Green_10031444);
    DrawGroupBoxWithLabel(s.local_8c, (RECT *)&DAT_100f2820, s_White_1003144c);
    EndPaint(hwnd, &s.local_88);
    return 0;

  case WM_CREATE:
    SetWindowTextA(hwnd, s_Sideboard_10031388);
    DAT_100f2844 = CreatePopupMenu();
    AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_NEWDECK, s__New_deck_10031394);
    AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_LOADDECK, s__Load_deck_100313a0);
    AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_SAVEDECK, s__Save_deck_100313ac);
    AppendMenuA(DAT_100f2844, MF_SEPARATOR, 0, NULL);
    AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_CONSOLIDATE, s__Consolidate_duplicate_cards_sid_100313b8);
    AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_CLEARDECK, s_C_lear_Sideboard_100313dc);
    AppendMenuA(DAT_100f2844, MF_SEPARATOR, 0, NULL);
    AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_MINIMIZE, s_M_inimize_100313f0);
    AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_MUSIC, s__Music_100313fc);
    AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_EFFECTS, s_Sound__Effects_10031404);
    AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_EXIT, s_E_xit_deck_builder_10031414);
    return 0;

  case WM_DESTROY:
    DestroyMenu(DAT_100f2844);
    return 0;

  case WM_SIZE:
    s.iVar3 = (int)(((unsigned)lparam & 0xFFFF) - 0x28) / 3;
    s.iVar4 = (int)(((unsigned)lparam >> 16) - 0x1e) / 2;

    SetRect((LPRECT)&DAT_100f27e0, 10, 10, s.iVar3 + 10, s.iVar4 + 10);
    s.iVar1 = s.iVar3 + 0x14;
    SetRect((LPRECT)&DAT_100f27f0, s.iVar1, 10, s.iVar3 + s.iVar1, s.iVar4 + 10);
    s.iVar1 = s.iVar1 + s.iVar3 + 10;
    SetRect((LPRECT)&DAT_100f2820, s.iVar1, 10, s.iVar3 + s.iVar1, s.iVar4 + 10);

    s.iVar1 = s.iVar4 + 0x14;
    SetRect((LPRECT)&DAT_100f2800, 10, s.iVar1, s.iVar3 + 10, s.iVar1 + s.iVar4);
    s.iVar2 = s.iVar3 + 0x14;
    SetRect((LPRECT)&DAT_100f2810, s.iVar2, s.iVar1, s.iVar3 + s.iVar2, s.iVar1 + s.iVar4);
    s.iVar2 = s.iVar2 + s.iVar3 + 10;
    SetRect((LPRECT)&DAT_100f2830, s.iVar2, s.iVar1, s.iVar3 + s.iVar2, s.iVar1 + s.iVar4);

    SendMessageA(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
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
      FUN_100058c0(hwnd, (int)&DAT_100f27e0, 6);
      break;

    case RES_MENU_CLEARDECK:
      if (((((DAT_101a0d60 != 0) || (DAT_101a0ddc != 0)) || (DAT_101a0e58 != 0)) ||
           ((DAT_101a0ed4 != 0 || (DAT_101a0f50 != 0)))) ||
          (DAT_101a0fcc != 0))
      {
        DAT_10031064 = DAT_101a0d60;
        DAT_10031068 = DAT_101a0ddc;
        DAT_1003106c = DAT_101a0e58;
        DAT_10031070 = DAT_101a0ed4;
        DAT_10031074 = DAT_101a0f50;
        DAT_10031078 = DAT_101a0fcc;

        DAT_101a0e58 = 0;
        DAT_101a0ddc = DAT_101a0e58;
        DAT_101a0d60 = DAT_101a0ddc;
        DAT_101a0fcc = 0;
        DAT_101a0f50 = DAT_101a0fcc;
        DAT_101a0ed4 = DAT_101a0f50;

        DeleteMenu(DAT_100f2844, RES_MENU_CLEARDECK, 0);
        AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_RESTOREDECK, s__Restore_sideboard_10031360);
        SendMessageA(global_unknown_10144b04, 0x401, 0, 0);
      }
      break;

    case RES_MENU_RESTOREDECK:
      DAT_101a0d60 = DAT_10031064;
      DAT_101a0ddc = DAT_10031068;
      DAT_101a0e58 = DAT_1003106c;
      DAT_101a0ed4 = DAT_10031070;
      DAT_101a0f50 = DAT_10031074;
      DAT_101a0fcc = DAT_10031078;

      DAT_1003106c = 0;
      DAT_10031068 = DAT_1003106c;
      DAT_10031064 = DAT_10031068;
      DAT_10031078 = 0;
      DAT_10031074 = DAT_10031078;
      DAT_10031070 = DAT_10031074;

      AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_CLEARDECK, s_C_lear_sideboard_10031374);
      DeleteMenu(DAT_100f2844, RES_MENU_RESTOREDECK, 0);
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

  case WM_ERASEBKGND:
    s.local_48 = (HDC)wparam;
    ApplyCardArtPaletteToDc((HDC)wparam);
    GetClientRect(hwnd, &s.local_44);
    FillRect(s.local_48, &s.local_44, global_create_brush_6);
    return 1;

  case WM_LBUTTONDOWN:
    BringWindowToTop(hwnd);
    SetFocus(global_main_hwnd);
    return 0;

  case WM_INITMENU:
    CheckMenuItem(DAT_100f2844, RES_MENU_CONSOLIDATE, ((global_cfg_consolidate == 0) - 1) & MF_CHECKED);
    CheckMenuItem(DAT_100f2844, RES_MENU_EFFECTS, ((global_cfg_effects == '\0') - 1) & MF_CHECKED);
    CheckMenuItem(DAT_100f2844, RES_MENU_MUSIC, ((global_cfg_music == '\0') - 1) & MF_CHECKED);
    return 0;

  case WM_RBUTTONDOWN:
    s.local_94.x = (int)((unsigned)lparam & 0xFFFF);
    s.local_94.y = (int)((unsigned)lparam >> 16);
    ClientToScreen(hwnd, &s.local_94);
    TrackPopupMenu(DAT_100f2844, TPM_RIGHTBUTTON, s.local_94.x, s.local_94.y, 0, hwnd, NULL);
    return 0;

  case 0x466:
    invalidate_wanted_window(hwnd, wparam);
    return 0;

  case 0x4c8:
    s.local_24 = 1;
    s.local_18 = (HDC)wparam;
    s.local_2c.x = (int)((unsigned)lparam & 0xFFFF);
    s.local_2c.y = (int)((unsigned)lparam >> 16);
    s.local_1c = (HMENU)FUN_10005317((int)&DAT_100f27e0, 6, &s.local_2c);
    if ((global_cfg_consolidate == 0) ||
        ((s.local_20 = FUN_1000563b(hwnd, (int)s.local_1c, (int)s.local_18)) == NULL))
    {
      s.local_20 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031338, DAT_10031334,
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
    else
    {
      add_or_increment_sideboard_bucket_entry((int)s.local_1c, (int)s.local_18, 1, &global_edited_deck);
      s.local_30 = SendMessageA(s.local_20, 0x402, 0, 0);
      SendMessageA(s.local_20, 0x401, s.local_30 + 1, 0);
    }

    if ((((DAT_10031064 == 0) && (DAT_10031068 == 0)) && (DAT_1003106c == 0)) &&
        (((DAT_10031070 == 0 && (DAT_10031074 == 0)) && (DAT_10031078 == 0))))
      return s.local_24;

    AppendMenuA(DAT_100f2844, MF_ENABLED, RES_MENU_CLEARDECK, s_C_lear_sideboard_1003134c);
    DeleteMenu(DAT_100f2844, RES_MENU_RESTOREDECK, 0);

    DAT_1003106c = 0;
    DAT_10031078 = 0;
    DAT_10031068 = DAT_1003106c;
    DAT_10031064 = DAT_10031068;
    DAT_10031074 = DAT_10031078;
    DAT_10031070 = DAT_10031074;
    return s.local_24;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: DECKDLL 0x10003f6a
LRESULT CALLBACK wndproc_TradeSurfaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  // GLOBAL: DECKDLL 0x1003107c
  static int DAT_1003107c;
  // GLOBAL: DECKDLL 0x10031080
  static int DAT_10031080;
  // GLOBAL: DECKDLL 0x10031084
  static int DAT_10031084;
  // GLOBAL: DECKDLL 0x10031088
  static int DAT_10031088;
  // GLOBAL: DECKDLL 0x1003108c
  static int DAT_1003108c;
  // GLOBAL: DECKDLL 0x10031090
  static int DAT_10031090;
  // GLOBAL: DECKDLL 0x10031094
  static int DAT_10031094;

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
    for (s.local_10 = 0; s.local_10 < DAT_101a1048; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031458, DAT_10031454,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x6, global_hinstance,
                                    (LPVOID)DAT_101a0fd0[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < DAT_101a10c4; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031470, DAT_1003146c,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x0, global_hinstance,
                                    (LPVOID)DAT_101a104c[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < DAT_101a1140; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_10031488, DAT_10031484,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x1, global_hinstance,
                                    (LPVOID)DAT_101a10c8[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < DAT_101a11bc; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100314a0, DAT_1003149c,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x2, global_hinstance,
                                    (LPVOID)DAT_101a1144[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < DAT_101a1238; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100314b8, DAT_100314b4,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x3, global_hinstance,
                                    (LPVOID)DAT_101a11c0[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < DAT_101a1330; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100314d0, DAT_100314cc,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x5, global_hinstance,
                                    (LPVOID)DAT_101a12b8[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }
    for (s.local_10 = 0; s.local_10 < DAT_101a12b4; s.local_10 = s.local_10 + 1)
    {
      if (s.local_c != 0)
      {
        s.local_8 = CreateWindowExA(0, s_MAGICDECK_CardClass_100314e8, DAT_100314e4,
                                    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                    0, 0, global_smallcard_width, global_smallcard_height,
                                    hwnd, (HMENU)0x4, global_hinstance,
                                    (LPVOID)DAT_101a123c[s.local_10].DeckEntry_csvid);
        if (s.local_8 != NULL)
          s.local_c = 1;
        else
          s.local_c = 0;
      }
    }

    SendMessageA(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    return s.local_c;

  case WM_PAINT:
    s.local_84 = BeginPaint(hwnd, &s.local_80);
    ApplyCardArtPaletteToDc(s.local_84);
    DrawGroupBoxWithLabel(s.local_84, (RECT *)&DAT_100f27d0, DAT_10031584);
    DrawGroupBoxWithLabel(s.local_84, (RECT *)&DAT_100f2770, DAT_1003158c);
    DrawGroupBoxWithLabel(s.local_84, (RECT *)&DAT_100f2780, s_Black_10031594);
    DrawGroupBoxWithLabel(s.local_84, (RECT *)&DAT_100f2790, DAT_1003159c);
    DrawGroupBoxWithLabel(s.local_84, (RECT *)&DAT_100f27c0, DAT_100315a4);
    DrawGroupBoxWithLabel(s.local_84, (RECT *)&DAT_100f27a0, s_Green_100315a8);
    DrawGroupBoxWithLabel(s.local_84, (RECT *)&DAT_100f27b0, s_White_100315b0);
    EndPaint(hwnd, &s.local_80);
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

  case WM_SIZE:
    s.iVar1 = ((int)((unsigned)lparam & 0xFFFF) - 0x32);
    s.iVar2 = (int)(s.iVar1 + ((unsigned)s.iVar1 >> 31 & 3U)) >> 2;
    s.iVar3 = (int)(((unsigned)lparam >> 16) - 0x1e) / 2;

    SetRect((LPRECT)&DAT_100f27d0, 10, 10, s.iVar2 + 10, s.iVar3 + 10);
    s.iVar1 = s.iVar2 + 0x14;
    SetRect((LPRECT)&DAT_100f2770, s.iVar1, 10, s.iVar1 + s.iVar2, s.iVar3 + 10);
    s.iVar1 = s.iVar1 + s.iVar2 + 10;
    SetRect((LPRECT)&DAT_100f2780, s.iVar1, 10, s.iVar1 + s.iVar2, s.iVar3 + 10);
    s.iVar1 = s.iVar1 + s.iVar2 + 10;
    SetRect((LPRECT)&DAT_100f27b0, s.iVar1, 10, s.iVar1 + s.iVar2, s.iVar3 + 10);

    s.iVar4 = s.iVar2 + 0x14;
    s.iVar1 = s.iVar3 + 0x14;
    SetRect((LPRECT)&DAT_100f2790, s.iVar4, s.iVar1, s.iVar4 + s.iVar2, s.iVar1 + s.iVar3);
    s.iVar4 = s.iVar4 + s.iVar2 + 10;
    SetRect((LPRECT)&DAT_100f27a0, s.iVar4, s.iVar1, s.iVar4 + s.iVar2, s.iVar1 + s.iVar3);
    s.iVar4 = s.iVar4 + s.iVar2 + 10;
    SetRect((LPRECT)&DAT_100f27c0, s.iVar4, s.iVar1, s.iVar4 + s.iVar2, s.iVar1 + s.iVar3);

    SendMessageA(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
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
      FUN_100058c0(hwnd, (int)&DAT_100f2770, 7);
      break;

    case RES_MENU_CLEARDECK:
      if (((((DAT_101a1048 != 0) || (DAT_101a10c4 != 0)) || (DAT_101a1140 != 0)) ||
           ((DAT_101a11bc != 0 || (DAT_101a1238 != 0)))) ||
          ((DAT_101a12b4 != 0 || (DAT_101a1330 != 0))))
      {
        DAT_1003107c = DAT_101a1048;
        DAT_10031080 = DAT_101a10c4;
        DAT_10031084 = DAT_101a1140;
        DAT_10031088 = DAT_101a11bc;
        DAT_1003108c = DAT_101a1238;
        DAT_10031090 = DAT_101a12b4;
        DAT_10031094 = DAT_101a1330;

        DAT_101a11bc = 0;
        DAT_101a1140 = DAT_101a11bc;
        DAT_101a10c4 = DAT_101a1140;
        DAT_101a1048 = DAT_101a10c4;
        DAT_101a1330 = 0;
        DAT_101a12b4 = DAT_101a1330;
        DAT_101a1238 = DAT_101a12b4;

        load_text(s_menus_10031544, s_DECKSURFACE_TRADE_10031530);
        DeleteMenu(global_tradesurface_popup, RES_MENU_CLEARDECK, 0);
        AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_RESTOREDECK, text_lines[2]);
        SendMessageA(global_unknown_10125a3c, 0x401, 0, 0);
      }
      break;

    case RES_MENU_RESTOREDECK:
      DAT_101a1048 = DAT_1003107c;
      DAT_101a10c4 = DAT_10031080;
      DAT_101a1140 = DAT_10031084;
      DAT_101a11bc = DAT_10031088;
      DAT_101a1238 = DAT_1003108c;
      DAT_101a12b4 = DAT_10031090;
      DAT_101a1330 = DAT_10031094;

      DAT_10031088 = 0;
      DAT_10031084 = DAT_10031088;
      DAT_10031080 = DAT_10031084;
      DAT_1003107c = DAT_10031080;
      DAT_10031094 = 0;
      DAT_10031090 = DAT_10031094;
      DAT_1003108c = DAT_10031090;

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

  case WM_ERASEBKGND:
    s.local_40 = (HDC)wparam;
    ApplyCardArtPaletteToDc((HDC)wparam);
    GetClientRect(hwnd, &s.local_3c);
    FillRect(s.local_40, &s.local_3c, global_create_brush_6);
    return 1;

  case WM_LBUTTONDOWN:
    BringWindowToTop(hwnd);
    SetFocus(global_main_hwnd);
    return 0;

  case WM_INITMENU:
    CheckMenuItem(global_tradesurface_popup, RES_MENU_CONSOLIDATE, ((global_cfg_consolidate == 0) - 1) & MF_CHECKED);
    CheckMenuItem(global_tradesurface_popup, RES_MENU_EFFECTS, ((global_cfg_effects == '\0') - 1) & MF_CHECKED);
    CheckMenuItem(global_tradesurface_popup, RES_MENU_MUSIC, ((global_cfg_music == '\0') - 1) & MF_CHECKED);
    return 0;

  case WM_RBUTTONDOWN:
    s.local_8c.x = (int)((unsigned)lparam & 0xFFFF);
    s.local_8c.y = (int)((unsigned)lparam >> 16);
    ClientToScreen(hwnd, &s.local_8c);
    TrackPopupMenu(global_tradesurface_popup, TPM_RIGHTBUTTON, s.local_8c.x, s.local_8c.y, 0, hwnd, NULL);
    return 0;

  case 0x466:
    invalidate_wanted_window(hwnd, wparam);
    return 0;

  case 0x4c8:
    s.local_20 = 1;
    s.local_14 = (HDC)wparam;
    s.local_28.x = (int)((unsigned)lparam & 0xFFFF);
    s.local_28.y = (int)((unsigned)lparam >> 16);
    s.local_18 = (HMENU)FUN_10005317((int)&DAT_100f2770, 7, &s.local_28);
    s.local_1c = CreateWindowExA(0, s_MAGICDECK_CardClass_10031500, DAT_100314fc,
                                 WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
                                 s.local_28.x, s.local_28.y, global_smallcard_width,
                                 global_smallcard_height, hwnd, s.local_18, global_hinstance,
                                 (LPVOID)s.local_14);
    if (s.local_1c == NULL)
    {
      s.local_20 = 0;
    }
    else
    {
      BringWindowToTop(s.local_1c);
      append_trade_bucket_entry((int)s.local_18, (int)s.local_14, 1, &global_edited_deck);
      SendMessageA(hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    }

    if ((((DAT_1003107c == 0) && (DAT_10031080 == 0)) && (DAT_10031084 == 0)) &&
        (((DAT_10031088 == 0 && (DAT_1003108c == 0)) &&
          ((DAT_10031090 == 0 && (DAT_10031094 == 0))))))
      return s.local_20;

    load_text(s_menus_10031528, s_DECKSURFACE_TRADE_10031514);
    AppendMenuA(global_tradesurface_popup, MF_ENABLED, RES_MENU_CLEARDECK, text_lines[1]);
    DeleteMenu(global_tradesurface_popup, RES_MENU_RESTOREDECK, 0);

    DAT_10031088 = 0;
    DAT_10031084 = DAT_10031088;
    DAT_10031080 = DAT_10031084;
    DAT_1003107c = DAT_10031080;
    DAT_10031094 = 0;
    DAT_10031090 = DAT_10031094;
    DAT_1003108c = DAT_10031090;
    return s.local_20;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}
