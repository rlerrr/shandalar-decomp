#include <STDDEF.H>
#include "filtermenus.h"
#include "mystdbool.h"
#include "cardartlib/src/palette.h"
#include "sidlib/pic.h"

typedef ptrdiff_t INT_PTR;

extern char text_lines[500][128];
extern HANDLE global_pic_ability;
extern HANDLE global_pic_antiquit;
extern HANDLE global_pic_arabnite;
extern HANDLE global_pic_artifact;
extern HANDLE global_pic_astral;
extern HANDLE global_pic_grey;
extern HANDLE global_pic_blue;
extern HANDLE global_pic_castcost;
extern HANDLE global_pic_creature;
extern HANDLE global_pic_dark;
extern HANDLE global_pic_dekbar1;
extern HANDLE global_pic_enchant;
extern HANDLE global_pic_fourth;
extern HANDLE global_pic_yellow;
extern HANDLE global_pic_green;
extern HANDLE global_pic_instant;
extern HANDLE global_pic_interrupt;
extern HANDLE global_pic_land;
extern HANDLE global_pic_legends;
extern HANDLE global_pic_power;
extern HANDLE global_pic_rarity;
extern HANDLE global_pic_red;
extern HANDLE global_pic_sorcery;
extern HANDLE global_pic_statbak1;
extern HANDLE global_pic_tough;
extern HANDLE global_pic_artist;
extern HANDLE global_pic_all_butn;
extern DBFlags global_db_flags_1;
extern bool global_cfg_effects;

extern HDC global_hdc;
extern HINSTANCE global_hinstance;
extern HBITMAP global_hbmp;

extern HWND global_main_hwnd;
extern HWND global_listbox_hwnd;
extern HWND global_horzlist_hwnd;
extern int global_dlg_parameter;
extern char global_base_directory[];
extern char global_duelart_path[];

int load_text(const char *file_name, const char *section_name);
void filter_cards_in_lists(HWND hwnd_listbox, HWND hwnd_horzlist);
void play_sound(int a1, int a2, int a3, int a4);
unsigned int HasExpansion(unsigned char param_1);

static void rotatem();

// Forward declarations
static bool show_dialog_filter_gle(int textline);
static int show_dialog_filter_subtype(int mode);
// GLOBAL: DECKDLL 0x10104db0
static char global_filter_dlg_title[64];
// GLOBAL: DECKDLL 0x10104d80
static int global_filter_gle_dlg_value = 0;

// GLOBAL: DECKDLL 0x10104d84
static HANDLE global_filter_creature_background_pic;

// GLOBAL: DECKDLL 0x101127a8
static HMENU global_filtermenu_default;
// GLOBAL: DECKDLL 0x101127d8
static HMENU global_filtermenu_fourth;
// GLOBAL: DECKDLL 0x101127b4
static HMENU global_filtermenu_gold;
// GLOBAL: DECKDLL 0x101127b8
static HMENU global_filtermenu_land;
// GLOBAL: DECKDLL 0x101127c8
static HMENU global_filtermenu_artifact;
// GLOBAL: DECKDLL 0x101127ac
static HMENU global_filtermenu_creature;
// GLOBAL: DECKDLL 0x101127c4
static HMENU global_filtermenu_enchantment;
// GLOBAL: DECKDLL 0x101127cc
static HMENU global_filtermenu_castcost;
// GLOBAL: DECKDLL 0x101127c0
static HMENU global_filtermenu_power;
// GLOBAL: DECKDLL 0x101127d0
static HMENU global_filtermenu_toughness;
// GLOBAL: DECKDLL 0x101127bc
static HMENU global_filtermenu_ability;
// GLOBAL: DECKDLL 0x101127d4
static HMENU global_filtermenu_rarity;
// GLOBAL: DECKDLL 0x101127b0
static HMENU global_filtermenu_artist;

// TODO: this is fake
static HMENU global_filtermenu_newexp;

// These are never set false.
// GLOBAL: DECKDLL 0x10113d0c
static bool global_filtermenu_castcost_enabled = true;
// GLOBAL: DECKDLL 0x10113d10
static bool global_filtermenu_power_enabled = true;
// GLOBAL: DECKDLL 0x10113d14
static bool global_filtermenu_toughness_enabled = true;
// GLOBAL: DECKDLL 0x10113d18
static bool global_filtermenu_ability_enabled = true;
// GLOBAL: DECKDLL 0x10113d1c
static bool global_filtermenu_rarity_enabled = true;
// GLOBAL: DECKDLL 0x10113d20
static bool global_filtermenu_artist_enabled = NULL;
// GLOBAL: DECKDLL 0x10113d08
static bool global_filtermenu_gold_enabled = true; // And this one isn't even used except for setting it true.

// GLOBAL: DECKDLL 0x101a9190
struct global_filters_t global_filters;

// GLOBAL: DECKDLL 0x10113d04
short global_filter_cardsets_flags = FCSF_0;

// GLOBAL: DECKDLL 0x10104d9c
static HANDLE global_filter_subtype_background_pic;

// GLOBAL: DECKDLL 0x10104de4
static HANDLE global_filter_gle_background_pic;

// FUNCTION: DECKDLL 0x1001d67f
static int
filterbuttons_setcoords(const RECT *r, int button_number, RECT *rval)
{
  struct
  {
    int idk;
    RECT temp_rect;
    int height;
  } s;

  if (((global_db_flags_1 & DBFLAGS_SHANDALAR) && button_number < 10) ||
      (!HasExpansion(2) && (button_number == 7 || button_number == 8)) ||
      (!HasExpansion(4) && (button_number == 9 || button_number == 10)))
  {
    SetRect(rval, -100, -100, -100, -100);
    return 1;
  }

  s.height = r->bottom - r->top - 2;
  s.idk = 0;
  SetRect(&s.temp_rect, r->left + 1, r->top + 1, r->left + s.height + 1, r->bottom - 2);
  OffsetRect(&s.temp_rect, (s.idk + s.height) * (button_number - 5), 0);

  if (button_number > 10)
  {
    s.temp_rect.left += 6;
    s.temp_rect.right += 6;
  }

  if (button_number >= 18)
  {
    s.temp_rect.left += 6;
    s.temp_rect.right += 6;
  }

  if (button_number > 22)
  {
    s.temp_rect.left += 8;
    s.temp_rect.right += 8;
  }

  CopyRect(rval, &s.temp_rect);
  return 1;
}

// FUNCTION: DECKDLL 0x1001eaa5
static int
get_filter_button_state(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    POINT p; // ebp - 0x2c
    int pad; // ebp - 0x24
    RECT r;  // ebp - 0x20
    RECT r2; // ebp - 0x10
  } s;

  s.p.x = lparam & 0xffff;
  s.p.y = (lparam >> 0x10) & 0xffff;

  GetClientRect(hwnd, &s.r);

#define CHECK_BUTTON(button_number, txthdr, var, bit)  \
  filterbuttons_setcoords(&s.r, button_number, &s.r2); \
  if (PtInRect(&s.r2, s.p))                            \
  {                                                    \
    if ((s.pad = load_text("cuecards", txthdr)) == -1) \
      return 0;                                        \
    return (var & bit) ? 1 : 2;                        \
  }

  CHECK_BUTTON(11, "WHITE", global_filters.colors, FC_WHITE);
  CHECK_BUTTON(15, "GREEN", global_filters.colors, FC_GREEN);
  CHECK_BUTTON(14, "RED", global_filters.colors, FC_RED);
  CHECK_BUTTON(13, "BLACK", global_filters.colors, FC_BLACK);
  CHECK_BUTTON(12, "BLUE", global_filters.colors, FC_BLUE);
  if (global_filter_cardsets_flags & FCSF_THE_DARK)
  {
    CHECK_BUTTON(10, "DARK", global_filters.cardsets, FS_THE_DARK);
  }
  if (global_filter_cardsets_flags & FCSF_ARABIAN_NIGHTS)
  {
    CHECK_BUTTON(7, "ARABIAN", global_filters.cardsets, FS_ARABIAN_NIGHTS);
  }
  if (global_filter_cardsets_flags & FCSF_ANTIQUITIES)
  {
    CHECK_BUTTON(8, "ANTIQUITIES", global_filters.cardsets, FS_ANTIQUITIES);
  }
  CHECK_BUTTON(6, "ASTRAL", global_filters.cardsets, FS_ASTRAL);
  if (global_filter_cardsets_flags & FCSF_LEGENDS)
  {
    CHECK_BUTTON(9, "LEGENDS", global_filters.cardsets, FS_LEGENDS);
  }
  CHECK_BUTTON(5, "FOURTH", global_filters.cardsets, FS_4TH_EDITION);
  CHECK_BUTTON(3, "PROMO", global_filters.cardsets, FS_UNK2);
  CHECK_BUTTON(1, "RESTRICTED", global_filters.cardsets, FS_UNK3);
  CHECK_BUTTON(16, "LAND", global_filters.cardtypes, FT_LAND);
  CHECK_BUTTON(17, "ARTIFACT", global_filters.cardtypes, FT_ARTIFACT);
  CHECK_BUTTON(18, "CREATURE", global_filters.cardtypes, FT_CREATURE);
  CHECK_BUTTON(19, "ENCHANTMENT", global_filters.cardtypes, FT_ENCHANTMENT);
  CHECK_BUTTON(20, "INSTANT", global_filters.cardtypes, FT_INSTANT);
  CHECK_BUTTON(21, "INTERRUPT", global_filters.cardtypes, FT_INTERRUPT);
  CHECK_BUTTON(22, "SORCERY", global_filters.cardtypes, FT_SORCERY);
  CHECK_BUTTON(23, "CASTCOST", global_filters.casting_cost, FN_ENABLE);
  CHECK_BUTTON(24, "POWER", global_filters.power, FN_ENABLE);
  CHECK_BUTTON(25, "TOUGHNESS", global_filters.toughness, FN_ENABLE);
  CHECK_BUTTON(26, "ABILITY", global_filters.abilities, FA_ENABLE);
  CHECK_BUTTON(27, "RARITY", global_filters.rarity, FR_ENABLE);
  CHECK_BUTTON(28, "ARTIST", global_filters.artist, 1);

  return 0;
#undef CHECK_BUTTON
}

// FUNCTION: DECKDLL 0x1001f6f4
static HMENU
select_filter_menu(HWND hwnd, POINT p)
{
  RECT r, r2;
  GetClientRect(hwnd, &r);
  filterbuttons_setcoords(&r, 5, &r2);
  if (PtInRect(&r2, p))
    return global_filtermenu_fourth;

  filterbuttons_setcoords(&r, 16, &r2);
  filterbuttons_setcoords(&r, 16, &r2);
  if (PtInRect(&r2, p))
    return global_filtermenu_land;

  filterbuttons_setcoords(&r, 17, &r2);
  if (PtInRect(&r2, p))
    return global_filtermenu_artifact;

  filterbuttons_setcoords(&r, 18, &r2);
  if (PtInRect(&r2, p))
    return global_filtermenu_creature;

  filterbuttons_setcoords(&r, 19, &r2);
  if (PtInRect(&r2, p))
    return global_filtermenu_enchantment;

  filterbuttons_setcoords(&r, 23, &r2);
  if (PtInRect(&r2, p))
    return global_filtermenu_castcost;

  filterbuttons_setcoords(&r, 24, &r2);
  if (PtInRect(&r2, p))
    return global_filtermenu_power;

  filterbuttons_setcoords(&r, 25, &r2);
  if (PtInRect(&r2, p))
    return global_filtermenu_toughness;

  filterbuttons_setcoords(&r, 26, &r2);
  if (PtInRect(&r2, p))
    return global_filtermenu_ability;

  filterbuttons_setcoords(&r, 27, &r2);
  if (PtInRect(&r2, p))
    return global_filtermenu_rarity;

  filterbuttons_setcoords(&r, 28, &r2);
  if (PtInRect(&r2, p))
    return global_filtermenu_artist;

  return global_filtermenu_default;
}

// FUNCTION: DECKDLL 0x1001d7bc
static void
draw_filter_button_3d(HDC hdc, RECT *r2, RECT *r3, bool pushed)
{
  RECT r4;
  if (pushed)
  {
    SetRect(&r4, r2->left + 1, r2->top + 1, r2->right, r2->top + 3);
    FillRect(hdc, &r4, GetStockObject(DKGRAY_BRUSH));
    SetRect(&r4, r2->left + 1, r2->top + 1, r2->left + 2, r2->bottom);
    FillRect(hdc, &r4, GetStockObject(DKGRAY_BRUSH));
    if (r3)
      SetRect(r3, r2->left + 3, r2->top + 3, r2->right, r2->bottom);

    FillRect(hdc, r3, GetStockObject(LTGRAY_BRUSH));
  }
  else
  {
    SetRect(&r4, r2->left, r2->top, r2->right, r2->top + 1);
    FillRect(hdc, &r4, GetStockObject(WHITE_BRUSH));
    SetRect(&r4, r2->left, r2->top, r2->left + 1, r2->bottom);
    FillRect(hdc, &r4, GetStockObject(WHITE_BRUSH));
    SetRect(&r4, r2->right - 2, r2->top + 1, r2->right - 1, r2->bottom);
    FillRect(hdc, &r4, GetStockObject(DKGRAY_BRUSH));
    SetRect(&r4, r2->right - 1, r2->top + 2, r2->right, r2->bottom);
    FillRect(hdc, &r4, GetStockObject(DKGRAY_BRUSH));
    SetRect(&r4, r2->left + 1, r2->bottom - 2, r2->right, r2->bottom - 1);
    FillRect(hdc, &r4, GetStockObject(DKGRAY_BRUSH));
    SetRect(&r4, r2->left + 1, r2->bottom - 1, r2->right, r2->bottom);
    FillRect(hdc, &r4, GetStockObject(DKGRAY_BRUSH));
    if (r3)
      SetRect(r3, r2->left + 1, r2->top + 1, r2->right - 2, r2->bottom - 2);

    FillRect(hdc, r3, GetStockObject(LTGRAY_BRUSH));
  }
}

// FUNCTION: DECKDLL 0x1001da6f
static void
draw_filter_button_pic(HDC hdc, RECT *r, int button_number, int pushed)
{
  RECT r3;
  BITMAP bmp;

  draw_filter_button_3d(hdc, r, &r3, pushed);
  InflateRect(&r3, -1, -1);

  GetObject(global_pic_yellow, sizeof(BITMAP), &bmp);
  switch (button_number)
  {
  case 1:
    SelectObject(global_hdc, global_pic_fourth);
    break;
  case 2:
  case 3:
  case 4:
    SelectObject(global_hdc, global_pic_red);
    break;
  case 5:
    SelectObject(global_hdc, global_pic_all_butn);
    break;
  case 6:
    SelectObject(global_hdc, global_pic_astral);
    break;
  case 7:
    SelectObject(global_hdc, global_pic_arabnite);
    break;
  case 8:
    SelectObject(global_hdc, global_pic_antiquit);
    break;
  case 9:
    SelectObject(global_hdc, global_pic_legends);
    break;
  case 10:
    SelectObject(global_hdc, global_pic_dark);
    break;
  case 11:
    SelectObject(global_hdc, global_pic_yellow);
    break;
  case 12:
    SelectObject(global_hdc, global_pic_blue);
    break;
  case 13:
    SelectObject(global_hdc, global_pic_grey);
    break;
  case 14:
    SelectObject(global_hdc, global_pic_red);
    break;
  case 15:
    SelectObject(global_hdc, global_pic_green);
    break;
  case 16:
    SelectObject(global_hdc, global_pic_land);
    break;
  case 17:
    SelectObject(global_hdc, global_pic_artifact);
    break;
  case 18:
    SelectObject(global_hdc, global_pic_creature);
    break;
  case 19:
    SelectObject(global_hdc, global_pic_enchant);
    break;
  case 20:
    SelectObject(global_hdc, global_pic_instant);
    break;
  case 21:
    SelectObject(global_hdc, global_pic_interrupt);
    break;
  case 22:
    SelectObject(global_hdc, global_pic_sorcery);
    break;
  case 23:
    SelectObject(global_hdc, global_pic_castcost);
    break;
  case 24:
    SelectObject(global_hdc, global_pic_power);
    break;
  case 25:
    SelectObject(global_hdc, global_pic_tough);
    break;
  case 26:
    SelectObject(global_hdc, global_pic_ability);
    break;
  case 27:
    SelectObject(global_hdc, global_pic_rarity);
    break;
  case 28:
    SelectObject(global_hdc, global_pic_artist);
    break;
  default:
    SelectObject(global_hdc, global_pic_red);
    break;
  }
  StretchBlt(hdc,
             r3.left, r3.top,
             r3.right - r3.left,
             r3.bottom - r3.top,
             global_hdc,
             0, 0,
             bmp.bmWidth, bmp.bmHeight,
             SRCCOPY);
  SelectObject(global_hdc, global_hbmp);
}

// FUNCTION: DECKDLL 0x1001ddeb
static void
draw_filter_buttons(HDC hdc, const RECT *r)
{
  struct
  {
    HDC chdc;
    BITMAP bmp;
    RECT r2;
  } s;

  s.chdc = CreateCompatibleDC(hdc);
  ApplyCardArtPaletteToDc(s.chdc);

  SelectObject(s.chdc, global_pic_dekbar1);
  GetObject(global_pic_dekbar1, sizeof(BITMAP), &s.bmp);

  StretchBlt(hdc, 0, 0, r->right, r->bottom, s.chdc, 0, 0, s.bmp.bmWidth, s.bmp.bmHeight, SRCCOPY);
  DeleteDC(s.chdc);

#define DRAW_FILTER_BUTTON(btn, state)                  \
  do                                                    \
  {                                                     \
    filterbuttons_setcoords(&r, (btn), &s.r2);          \
    draw_filter_button_pic(hdc, &s.r2, (btn), (state)); \
  } while (false)

#define DRAW_FILTER_BUTTON_CONDITIONAL(btn, state, cond) \
  if (cond)                                              \
  {                                                      \
    DRAW_FILTER_BUTTON(btn, state);                      \
  }

  DRAW_FILTER_BUTTON(11, global_filters.colors & FC_WHITE);
  DRAW_FILTER_BUTTON(14, global_filters.colors & FC_RED);
  DRAW_FILTER_BUTTON(12, global_filters.colors & FC_BLUE);
  DRAW_FILTER_BUTTON(15, global_filters.colors & FC_GREEN);
  DRAW_FILTER_BUTTON(13, global_filters.colors & FC_BLACK);

  DRAW_FILTER_BUTTON_CONDITIONAL(16, global_filters.colors & FC_GOLD, global_filtermenu_gold_enabled);

  DRAW_FILTER_BUTTON(16, global_filters.cardtypes & FT_LAND);
  DRAW_FILTER_BUTTON(17, global_filters.cardtypes & FT_ARTIFACT);
  DRAW_FILTER_BUTTON(18, global_filters.cardtypes & FT_CREATURE);
  DRAW_FILTER_BUTTON(19, global_filters.cardtypes & FT_ENCHANTMENT);

  DRAW_FILTER_BUTTON(20, global_filters.cardtypes & FT_INSTANT);
  DRAW_FILTER_BUTTON(21, global_filters.cardtypes & FT_INTERRUPT);
  DRAW_FILTER_BUTTON(22, global_filters.cardtypes & FT_SORCERY);

  DRAW_FILTER_BUTTON_CONDITIONAL(5, global_filters.cardsets & FS_4TH_EDITION, global_filter_cardsets_flags & FCSF_Q_ENABLE);
  DRAW_FILTER_BUTTON_CONDITIONAL(6, global_filters.cardsets & FS_ASTRAL, global_filter_cardsets_flags & FCSF_Q_ASTRAL);
  DRAW_FILTER_BUTTON_CONDITIONAL(8, global_filters.cardsets & FS_ANTIQUITIES, global_filter_cardsets_flags & FCSF_ANTIQUITIES);
  DRAW_FILTER_BUTTON_CONDITIONAL(10, global_filters.cardsets & FS_THE_DARK, global_filter_cardsets_flags & FCSF_THE_DARK);
  DRAW_FILTER_BUTTON_CONDITIONAL(7, global_filters.cardsets & FS_ARABIAN_NIGHTS, global_filter_cardsets_flags & FCSF_ARABIAN_NIGHTS);
  DRAW_FILTER_BUTTON_CONDITIONAL(9, global_filters.cardsets & FS_LEGENDS, global_filter_cardsets_flags & FCSF_LEGENDS);

  DRAW_FILTER_BUTTON_CONDITIONAL(2, global_filters.cardsets & FS_UNK1, global_filter_cardsets_flags & FCSF_UNK1);
  DRAW_FILTER_BUTTON_CONDITIONAL(3, global_filters.cardsets & FS_UNK2, global_filter_cardsets_flags & FCSF_UNK2);
  DRAW_FILTER_BUTTON_CONDITIONAL(1, global_filters.cardsets & FS_UNK3, global_filter_cardsets_flags & FCSF_UNK3);

  DRAW_FILTER_BUTTON_CONDITIONAL(4, global_filters.cardsets & FS_OTHER, global_filter_cardsets_flags & FCSF_Q_OTHER);

  DRAW_FILTER_BUTTON_CONDITIONAL(23, global_filters.casting_cost & FN_ENABLE, global_filtermenu_castcost_enabled);
  DRAW_FILTER_BUTTON_CONDITIONAL(24, global_filters.power & FN_ENABLE, global_filtermenu_power_enabled);
  DRAW_FILTER_BUTTON_CONDITIONAL(25, global_filters.toughness & FN_ENABLE, global_filtermenu_toughness_enabled);
  DRAW_FILTER_BUTTON_CONDITIONAL(26, global_filters.abilities & FA_ENABLE, global_filtermenu_ability_enabled);
  DRAW_FILTER_BUTTON_CONDITIONAL(27, global_filters.rarity & FR_ENABLE, global_filtermenu_rarity_enabled);

  DRAW_FILTER_BUTTON_CONDITIONAL(28, global_filters.artist & FR_ENABLE, global_filtermenu_artist_enabled);

#undef DRAW_FILTER_BUTTON
#undef DRAW_FILTER_BUTTON_CONDITIONAL
}

// FUNCTION: DECKDLL 0x1001e4b6
static bool
toggle_filterbutton(int n)
{
  switch (n)
  {
  case 11:
    global_filters.colors ^= FC_WHITE;
    if (global_filters.colors & FC_WHITE)
      return true;
    return false;
  case 15:
    global_filters.colors ^= FC_GREEN;
    if (global_filters.colors & FC_GREEN)
      return true;
    return false;
  case 14:
    global_filters.colors ^= FC_RED;
    if (global_filters.colors & FC_RED)
      return true;
    return false;
  case 13:
    global_filters.colors ^= FC_BLACK;
    if (global_filters.colors & FC_BLACK)
      return true;
    return false;
  case 12:
    global_filters.colors ^= FC_BLUE;
    if (global_filters.colors & FC_BLUE)
      return true;
    return false;
  case 4:
    global_filters.cardsets ^= FS_OTHER;
    if (global_filters.cardsets & FS_OTHER)
      return true;
    return false;
  case 7:
    if (global_filter_cardsets_flags & FCSF_ARABIAN_NIGHTS)
    {
      global_filters.cardsets ^= FS_ARABIAN_NIGHTS;
      if (global_filters.cardsets & FS_ARABIAN_NIGHTS)
        return true;
    }
    return false;
  case 8:
    if (global_filter_cardsets_flags & FCSF_ANTIQUITIES)
    {
      global_filters.cardsets ^= FS_ANTIQUITIES;
      if (global_filters.cardsets & FS_ANTIQUITIES)
        return true;
    }
    return false;
  case 10:
    if (global_filter_cardsets_flags & FCSF_THE_DARK)
    {
      global_filters.cardsets ^= FS_THE_DARK;
      if (global_filters.cardsets & FS_THE_DARK)
        return true;
    }
    return false;
  case 6:
    global_filters.cardsets ^= FS_ASTRAL;
    if (global_filters.cardsets & FS_ASTRAL)
      return true;
    return false;
  case 5:
    global_filters.cardsets ^= FS_4TH_EDITION;
    if (global_filters.cardsets & FS_4TH_EDITION)
      return true;
    return false;
  case 9:
    if (global_filter_cardsets_flags & FCSF_LEGENDS)
    {
      global_filters.cardsets ^= FS_LEGENDS;
      if (global_filters.cardsets & FS_LEGENDS)
        return true;
    }
    return false;
  case 3:
    global_filters.cardsets ^= FS_UNK2;
    if (global_filters.cardsets & FS_UNK2)
      return true;
    return false;
  case 1:
    global_filters.cardsets ^= FS_UNK3;
    if (global_filters.cardsets & FS_UNK3)
      return true;
    return false;
  case 2:
    global_filters.cardsets ^= FS_UNK1;
    if (global_filters.cardsets & FS_UNK1)
      return true;
    return false;
  case 16:
    global_filters.cardtypes ^= FT_LAND;
    if (global_filters.cardtypes & FT_LAND)
      return true;
    return false;
  case 17:
    global_filters.cardtypes ^= FT_ARTIFACT;
    if (global_filters.cardtypes & FT_ARTIFACT)
      return true;
    return false;
  case 18:
    global_filters.cardtypes ^= FT_CREATURE;
    if (global_filters.cardtypes & FT_CREATURE)
      return true;
    return false;
  case 19:
    global_filters.cardtypes ^= FT_ENCHANTMENT;
    if (global_filters.cardtypes & FT_ENCHANTMENT)
      return true;
    return false;
  case 20:
    global_filters.cardtypes ^= FT_INSTANT;
    if (global_filters.cardtypes & FT_INSTANT)
      return true;
    return false;
  case 21:
    global_filters.cardtypes ^= FT_INTERRUPT;
    if (global_filters.cardtypes & FT_INTERRUPT)
      return true;
    return false;
  case 22:
    global_filters.cardtypes ^= FT_SORCERY;
    if (global_filters.cardtypes & FT_SORCERY)
      return true;
    return false;
  case 23:
    global_filters.casting_cost ^= FN_ENABLE;
    if (global_filters.casting_cost & FN_ENABLE)
      return false;
    return true;
  case 24:
    global_filters.power ^= FN_ENABLE;
    if (global_filters.power & FN_ENABLE)
      return false;
    return true;
  case 25:
    global_filters.toughness ^= FN_ENABLE;
    if (global_filters.toughness & FN_ENABLE)
      return false;
    return true;
  case 26:
    global_filters.abilities ^= FA_ENABLE;
    if (global_filters.abilities & FA_ENABLE)
      return false;
    return true;
  case 27:
    global_filters.rarity ^= FR_ENABLE;
    if (global_filters.rarity & FR_ENABLE)
      return false;
    return true;
  case 28:
    global_filters.artist ^= 1;
    if (global_filters.artist & 1)
      return false;
    return true;
  default:
    return false;
  }
}

// FUNCTION: DECKDLL 0x1001f97e
static void create_filter_menus(void)
{
  //TODO: why are the RES_ consts here fucked up?

  struct
  {
    int i;
    int n;
  } s;

  s.n = load_text("menus", "FILTERS");
  if (s.n != -1)
  {
    global_filtermenu_default = CreatePopupMenu();
    for (s.i = 0; s.i < s.n; ++s.i)
      AppendMenu(global_filtermenu_default, MF_ENABLED, s.i + RES_FILTERMENU_MAINMENUBUTTONS_ON, text_lines[s.i]);
  }

  s.n = load_text("menusx1", "FOURTH");
  if (s.n != -1)
  {
    global_filtermenu_fourth = CreatePopupMenu();
    for (s.i = 0; s.i < s.n; ++s.i)
      AppendMenu(global_filtermenu_fourth, MF_ENABLED, s.i + RES_FILTERMENU_FOURTH_UNLIMITED, text_lines[s.i]);
  }

  if (global_filtermenu_gold_enabled & 0x40)
  {
    s.n = load_text("menus", "GOLD");
    if (s.n != -1)
    {
      global_filtermenu_gold = CreatePopupMenu();
      for (s.i = 0; s.i < s.n; ++s.i)
        AppendMenu(global_filtermenu_gold, MF_ENABLED, s.i + RES_FILTERMENU_GOLD_ALL, text_lines[s.i]);
    }
  }

  s.n = load_text("menus", "LAND");
  if (s.n != -1)
  {
    global_filtermenu_land = CreatePopupMenu();
    for (s.i = 0; s.i < s.n; ++s.i)
      AppendMenu(global_filtermenu_land, MF_ENABLED, s.i + RES_FILTERMENU_LAND_LANDANDMANA, text_lines[s.i]);
  }

  s.n = load_text("menus", "ARTIFACT");
  if (s.n != -1)
  {
    global_filtermenu_artifact = CreatePopupMenu();
    for (s.i = 0; s.i < s.n; ++s.i)
      AppendMenu(global_filtermenu_artifact, MF_ENABLED, s.i + RES_FILTERMENU_ARTIFACT_CREATURES, text_lines[s.i]);
  }

  s.n = load_text("menus", "CREATURE");
  if (s.n != -1)
  {
    global_filtermenu_creature = CreatePopupMenu();
    if (global_db_flags_1 & DBFLAGS_STANDALONE)
    {
      for (s.i = 0; s.i < s.n; ++s.i)
        AppendMenu(global_filtermenu_creature, MF_ENABLED, s.i + RES_FILTERMENU_CREATURE_CREATURE, text_lines[s.i]);
      InsertMenu(global_filtermenu_creature, s.i - 1, MF_BYPOSITION, MF_SEPARATOR, NULL);
    }
    else
    {
      for (s.i = 0; s.i < s.n; ++s.i)
        if (s.i != 1)
          AppendMenu(global_filtermenu_creature, MF_ENABLED, s.i + RES_FILTERMENU_CREATURE_CREATURE, text_lines[s.i]);
      InsertMenu(global_filtermenu_creature, s.i - 2, MF_BYPOSITION, MF_SEPARATOR, NULL);
    }
  }

  s.n = load_text("menus", "ENCHANTMENT");
  if (s.n != -1)
  {
    global_filtermenu_enchantment = CreatePopupMenu();
    for (s.i = 0; s.i < s.n; ++s.i)
      AppendMenu(global_filtermenu_enchantment, MF_ENABLED, s.i + RES_FILTERMENU_ENCHANTMENT_ENCHANTMENTS, text_lines[s.i]);
  }

  if (global_filtermenu_castcost_enabled & 1)
  {
    s.n = load_text("menus", "CASTCOST");
    if (s.n != -1)
    {
      global_filtermenu_castcost = CreatePopupMenu();
      for (s.i = 0; s.i < s.n; ++s.i)
        AppendMenu(global_filtermenu_castcost, MF_ENABLED, s.i + RES_FILTERMENU_COST_GREATER, text_lines[s.i]);
    }
  }

  if (global_filtermenu_power_enabled & 1)
  {
    s.n = load_text("menus", "POWER");
    if (s.n != -1)
    {
      global_filtermenu_power = CreatePopupMenu();
      for (s.i = 0; s.i < s.n; ++s.i)
        AppendMenu(global_filtermenu_power, MF_ENABLED, s.i + RES_FILTERMENU_POWER_GREATER, text_lines[s.i]);
    }
  }

  if (global_filtermenu_toughness_enabled & 1)
  {
    s.n = load_text("menus", "TOUGHNESS");
    if (s.n != -1)
    {
      global_filtermenu_toughness = CreatePopupMenu();
      for (s.i = 0; s.i < s.n; ++s.i)
        AppendMenu(global_filtermenu_toughness, MF_ENABLED, s.i + RES_FILTERMENU_TOUGHNESS_GREATER, text_lines[s.i]);
    }
  }

  if (global_filtermenu_ability_enabled & 1)
  {
    s.n = load_text("menus", "ABILITY");
    if (s.n != -1)
    {
      global_filtermenu_ability = CreatePopupMenu();
      for (s.i = 0; s.i < s.n; ++s.i)
        AppendMenu(global_filtermenu_ability, MF_ENABLED, s.i + RES_FILTERMENU_ABILITY_NATIVE, text_lines[s.i]);
    }
    InsertMenu(global_filtermenu_ability, 2, MF_BYPOSITION, MF_SEPARATOR, NULL);
  }

  if (global_filtermenu_rarity_enabled & 1)
  {
    s.n = load_text("menus", "RARITY");
    if (s.n != -1)
    {
      global_filtermenu_rarity = CreatePopupMenu();
      for (s.i = 0; s.i < s.n; ++s.i)
        AppendMenu(global_filtermenu_rarity, MF_ENABLED, s.i + RES_FILTERMENU_RARITY_COMMON, text_lines[s.i]);
    }
  }

  if (global_filtermenu_artist_enabled & 1)
  {
    s.n = load_text("menus", "ARTIST");
    if (s.n != -1)
    {
      global_filtermenu_artist = CreatePopupMenu();
      for (s.i = 0; s.i < s.n; ++s.i)
        AppendMenu(global_filtermenu_artist, MF_ENABLED, s.i + RES_FILTERMENU_RARITY_RARE, text_lines[s.i]);
    }
  }
}

// FUNCTION: DECKDLL 0x10020007
static void
destroy_filter_menus(void)
{
  if (global_filtermenu_default)
    DestroyMenu(global_filtermenu_default);
  if (global_filtermenu_fourth)
    DestroyMenu(global_filtermenu_fourth);
  if (global_filtermenu_gold)
    DestroyMenu(global_filtermenu_gold);
  if (global_filtermenu_land)
    DestroyMenu(global_filtermenu_land);
  if (global_filtermenu_artifact)
    DestroyMenu(global_filtermenu_artifact);
  if (global_filtermenu_creature)
    DestroyMenu(global_filtermenu_creature);
  if (global_filtermenu_enchantment)
    DestroyMenu(global_filtermenu_enchantment);
  if (global_filtermenu_castcost)
    DestroyMenu(global_filtermenu_castcost);
  if (global_filtermenu_power)
    DestroyMenu(global_filtermenu_power);
  if (global_filtermenu_toughness)
    DestroyMenu(global_filtermenu_toughness);
  if (global_filtermenu_ability)
    DestroyMenu(global_filtermenu_ability);
  if (global_filtermenu_rarity)
    DestroyMenu(global_filtermenu_rarity);
  if (global_filtermenu_artist)
    DestroyMenu(global_filtermenu_artist);
}

// FUNCTION: DECKDLL 0x1001bcd5
LRESULT CALLBACK
wndproc_CardListFilterClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  int i;
  static RECT rect = {0, 0, 0, 0};
  static HDC chdc = NULL;
  static HBITMAP bmp = NULL;
  static int stored_creature_filters = FT_0;

  HDC hdc;
  HMENU menu;
  POINT p;

  switch (msg)
  {
  case WM_PAINT:
  {
    PAINTSTRUCT paint;
    hdc = BeginPaint(hwnd, &paint);
    ApplyCardArtPaletteToDc(hdc);
    BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, chdc, 0, 0, SRCCOPY);
    EndPaint(hwnd, &paint);
    return 0;
  }

  case WM_CREATE:
    global_filters.colors = FC_WHITE | FC_GREEN | FC_RED | FC_BLACK | FC_BLUE | FC_GOLD | FC_GOLD_ALL;

    global_filters.expansions = FE_4TH_EDITION | FE_REVISED | FE_UNLIMITED | 0x0F00;

    global_filters.cardtypes = (FT_LAND | FT_ARTIFACT | FT_CREATURE | FT_ENCHANTMENT | FT_INSTANT | FT_INTERRUPT | FT_SORCERY | FT_ARTIFACT_NON_CREATURE | FT_ARTIFACT_CREATURE | FT_LAND_LAND_AND_MANA | FT_LAND_LAND_ONLY | FT_CREATURE_CREATURE | FT_CREATURE_TOKEN | FT_CREATURE_ARTIFACT);

    global_filters.casting_cost = FN_GT;
    global_filters.casting_cost_value = 0;

    global_filters.power = FN_GT;
    global_filters.power_value = 0;

    global_filters.toughness = FN_GT;
    global_filters.toughness_value = 0;

    global_filters.abilities = (FA_NATIVE | FA_GRANTS | FA_FLYING | FA_FIRSTSTRIKE | FA_TRAMPLE | FA_REGENERATION | FA_BANDING | FA_PROTECTION | FA_LANDWALK | FA_INFECT | FA_RAMPAGE | FA_REACH | FA_DEATHTOUCH | FA_VIGILANCE | FA_HASTE);

    global_filters.rarity = FR_COMMON | FR_UNCOMMON | FR_RARE | FR_RESTRICTED | FR_BANNED;

    memset(global_filters.creature_list, -1, sizeof global_filters.creature_list);
    memset(global_filters.expansion_list, -1, sizeof global_filters.expansion_list);

    if (global_db_flags_1 & (DBFLAGS_STANDALONE | DBFLAGS_NOCARDCOUNTCHECK | DBFLAGS_GAUNTLET | DBFLAGS_EDITDECK))
      global_filter_cardsets_flags = (FCSF_Q_OTHER | FCSF_LEGENDS | FCSF_THE_DARK | FCSF_ANTIQUITIES | FCSF_ARABIAN_NIGHTS | FCSF_Q_ASTRAL | FCSF_Q_ENABLE);
    else
      global_filter_cardsets_flags = FCSF_0;

    global_filters.cardsets = (FS_OTHER | FS_LEGENDS | FS_THE_DARK | FS_ANTIQUITIES | FS_ARABIAN_NIGHTS | FS_ASTRAL | FS_4TH_EDITION);

    global_filtermenu_castcost_enabled = true;
    global_filtermenu_power_enabled = true;
    global_filtermenu_toughness_enabled = true;
    global_filtermenu_ability_enabled = true;
    global_filtermenu_rarity_enabled = true;
    global_filtermenu_gold_enabled = true;

    filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);

    create_filter_menus();
    return 0;

  case WM_INITMENU:
    menu = wparam;
    if (menu == global_filtermenu_newexp)
      CHECKMENU_IF(global_filtermenu_newexp, RES_FILTERMENU_EXPANSIONLIST, global_filters.expansions & FE_EXPANSIONLIST);
    else if (menu == global_filtermenu_fourth)
    {
      CHECKMENU_IF(global_filtermenu_fourth, RES_FILTERMENU_FOURTH_UNLIMITED, global_filters.expansions & FE_UNLIMITED);
      CHECKMENU_IF(global_filtermenu_fourth, RES_FILTERMENU_FOURTH_REVISED, global_filters.expansions & FE_REVISED);
      CHECKMENU_IF(global_filtermenu_fourth, RES_FILTERMENU_FOURTH_FOURTH, global_filters.expansions & FE_4TH_EDITION);
    }
    else if (menu == global_filtermenu_gold)
    {
      CHECKMENU_IF(global_filtermenu_gold, RES_FILTERMENU_GOLD_ALL, global_filters.colors & FC_GOLD_ALL);
      CHECKMENU_IF(global_filtermenu_gold, RES_FILTERMENU_GOLD_MATCHINGALL, global_filters.colors & FC_GOLD_ALLSELECTED);
      CHECKMENU_IF(global_filtermenu_gold, RES_FILTERMENU_GOLD_MATCHINGANY, global_filters.colors & FC_GOLD_ANYSELECTED);
    }
    else if (menu == global_filtermenu_land)
    {
      CHECKMENU_IF(global_filtermenu_land, RES_FILTERMENU_LAND_LANDANDMANA, global_filters.cardtypes & FT_LAND_LAND_AND_MANA);
      CHECKMENU_IF(global_filtermenu_land, RES_FILTERMENU_LAND_LANDONLY, global_filters.cardtypes & FT_LAND_LAND_ONLY);
      CHECKMENU_IF(global_filtermenu_land, RES_FILTERMENU_LAND_MANAONLY, global_filters.cardtypes & FT_LAND_MANA_ONLY);
    }
    else if (menu == global_filtermenu_artifact)
    {
      CHECKMENU_IF(global_filtermenu_artifact, RES_FILTERMENU_ARTIFACT_CREATURES, global_filters.cardtypes & FT_ARTIFACT_CREATURE);
      CHECKMENU_IF(global_filtermenu_artifact, RES_FILTERMENU_ARTIFACT_NONCREATURES, global_filters.cardtypes & FT_ARTIFACT_NON_CREATURE);
    }
    else if (menu == global_filtermenu_creature)
    {
      CHECKMENU_IF(global_filtermenu_creature, RES_FILTERMENU_CREATURE_CREATURE, global_filters.cardtypes & FT_CREATURE_CREATURE);
      CHECKMENU_IF(global_filtermenu_creature, RES_FILTERMENU_CREATURE_TOKEN, global_filters.cardtypes & FT_CREATURE_TOKEN);
      CHECKMENU_IF(global_filtermenu_creature, RES_FILTERMENU_CREATURE_ARTIFACT, global_filters.cardtypes & FT_CREATURE_ARTIFACT);
      CHECKMENU_IF(global_filtermenu_creature, RES_FILTERMENU_CREATURE_LIST, global_filters.cardtypes & FT_CREATURE_LIST);
    }
    /*
    else if (menu == global_filtermenu_enchantment)
    {
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_ENCHANTMENTS, global_filters.cardtypes & FT_ENCHANTMENT_ENCHANTMENTS);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_WORLD, global_filters.cardtypes & FT_ENCHANTMENT_WORLD);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_LAND, global_filters.cardtypes & FT_ENCHANTMENT_LAND);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_CREATURE, global_filters.cardtypes & FT_ENCHANTMENT_CREATURE);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_ARTIFACT, global_filters.cardtypes & FT_ENCHANTMENT_ARTIFACT);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_ENCHANT, global_filters.cardtypes & FT_ENCHANTMENT_ENCHANT);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_PERMANENT, global_filters.cardtypes & FT_ENCHANTMENT_PERMANENT);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_PLAYER, global_filters.cardtypes & FT_ENCHANTMENT_PLAYER);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_INSTANT, global_filters.cardtypes & FT_ENCHANTMENT_INSTANT);
    }
    */
    else if (menu == global_filtermenu_castcost)
    {
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_GREATER, global_filters.casting_cost & FN_GT);
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_LESSER, global_filters.casting_cost & FN_LT);
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_EQUAL, global_filters.casting_cost & FN_EQ);
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_X, global_filters.casting_cost & FN_CC_X);
    }
    else if (menu == global_filtermenu_power)
    {
      CHECKMENU_IF(global_filtermenu_power, RES_FILTERMENU_POWER_GREATER, global_filters.power & FN_GT);
      CHECKMENU_IF(global_filtermenu_power, RES_FILTERMENU_POWER_LESSER, global_filters.power & FN_LT);
      CHECKMENU_IF(global_filtermenu_power, RES_FILTERMENU_POWER_EQUAL, global_filters.power & FN_EQ);
    }
    else if (menu == global_filtermenu_toughness)
    {
      CHECKMENU_IF(global_filtermenu_toughness, RES_FILTERMENU_TOUGHNESS_GREATER, global_filters.toughness & FN_GT);
      CHECKMENU_IF(global_filtermenu_toughness, RES_FILTERMENU_TOUGHNESS_LESSER, global_filters.toughness & FN_LT);
      CHECKMENU_IF(global_filtermenu_toughness, RES_FILTERMENU_TOUGHNESS_EQUAL, global_filters.toughness & FN_EQ);
    }
    else if (menu == global_filtermenu_ability)
    {
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_NATIVE, global_filters.abilities & FA_NATIVE);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_GIVES, global_filters.abilities & FA_GRANTS);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_FLYING, global_filters.abilities & FA_FLYING);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_FIRSTSTRIKE, global_filters.abilities & FA_FIRSTSTRIKE);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_TRAMPLE, global_filters.abilities & FA_TRAMPLE);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_REGENERATION, global_filters.abilities & FA_REGENERATION);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_BANDING, global_filters.abilities & FA_BANDING);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_COLORWARD, global_filters.abilities & FA_PROTECTION);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_LANDWALK, global_filters.abilities & FA_LANDWALK);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_POISON, global_filters.abilities & FA_INFECT);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_RAMPAGE, global_filters.abilities & FA_RAMPAGE);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_REACH, global_filters.abilities & FA_REACH);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_STONING, global_filters.abilities & FA_DEATHTOUCH);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_VIGILANCE, global_filters.abilities & FA_VIGILANCE);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_HASTE, global_filters.abilities & FA_HASTE);
    }
    else if (menu == global_filtermenu_rarity)
    {
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_COMMON, global_filters.rarity & FR_COMMON);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_UNCOMMON, global_filters.rarity & FR_UNCOMMON);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_RARE, global_filters.rarity & FR_RARE);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_RESTRICTED, global_filters.rarity & FR_RESTRICTED);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_BANNED, global_filters.rarity & FR_BANNED);
    }
    return 0;

  case WM_DESTROY:
    DELETE_DC(chdc);
    DELETE_OBJ(bmp);
    destroy_filter_menus();
    KillTimer(hwnd, 1);
    return 0;

  case WM_LBUTTONDOWN:
  {
    RECT r, r2;
    p.x = GET_X_LPARAM(lparam);
    p.y = GET_Y_LPARAM(lparam);

    GetClientRect(hwnd, &r);
    for (i = 0; i < 35; ++i)
    {
      filterbuttons_setcoords(&r, i, &r2);
      if (PtInRect(&r2, p))
      {
        if (global_cfg_effects)
          play_sound(toggle_filterbutton(i) ? 4 : 5, 400, 0, 0);

        SendMessage(global_horzlist_hwnd, 0x8007, 0, 0);
        filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);
        draw_filter_buttons(chdc, &r);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
      }
    }
    return 0;
  }

  case WM_RBUTTONDOWN:
    p.x = GET_X_LPARAM(lparam);
    p.y = GET_Y_LPARAM(lparam);
    if ((menu = select_filter_menu(hwnd, p)))
    {
      ClientToScreen(hwnd, &p);
      TrackPopupMenu(menu, TPM_RIGHTBUTTON, p.x, p.y, 0, hwnd, NULL);
    }
    return 0;

  case WM_COMMAND:
  {
    bool refresh_filters = false;
    RECT lRect2;
    // This is the part that breaks decompilation.  Starting at 0x40852a, change "FF 24 85 14 8a 40 00" to "E9 02 00 00 00 90 90" to force case 1.
    int cmd;
    switch (cmd = LOWORD(wparam))
    {
#define TOGGLE_FILTER_NO_INSERT
#define TOGGLE_FILTER_ADD(button, val, bit, base, insert1) \
  button:                                                  \
  val ^= bit;                                              \
  insert1;                                                 \
  if (val & base)                                          \
  refresh_filters = true

#define TOGGLE_FILTER(button, val, bit, base) TOGGLE_FILTER_ADD(button, val, bit, base, TOGGLE_FILTER_NO_INSERT)

#define GLE_FILTER(buttonbase, valbase, dlgcode)   \
  buttonbase##_GREATER : case buttonbase##_LESSER: \
  case buttonbase##_EQUAL:                         \
  global_dlg_parameter = valbase##_value;          \
  if (!show_dialog_filter_gle(dlgcode)) break;     \
  valbase##_value = global_filter_gle_dlg_value;   \
  valbase &= FN_ENABLE;                            \
  if (cmd == buttonbase##_GREATER)                 \
    valbase |= FN_GT;                              \
  if (cmd == buttonbase##_LESSER)                  \
    valbase |= FN_LT;                              \
  if (cmd == buttonbase##_EQUAL)                   \
    valbase |= FN_EQ;                              \
  if (valbase & FN_ENABLE)                         \
  refresh_filters = true

#define TOGGLE_3SETS_FILTER(button, tgt, other1, other2)                                   \
  button:                                                                                  \
  if (!(global_filters.expansions & tgt) || (global_filters.expansions & (other1 | other2))) \
    global_filters.expansions ^= tgt;                                                       \
  if (global_filters.cardsets & FS_4TH_EDITION)                                             \
  refresh_filters = true

    case RES_FILTERMENU_GOLD_ALL:
    case RES_FILTERMENU_GOLD_MATCHINGALL:
    case RES_FILTERMENU_GOLD_MATCHINGANY:
    {
      int old_fc;

      old_fc = global_filters.colors;
      global_filters.colors &= ~(FC_GOLD_ALL | FC_GOLD_ALLSELECTED | FC_GOLD_ANYSELECTED);
      if (cmd == RES_FILTERMENU_GOLD_ALL)
        global_filters.colors |= FC_GOLD_ALL;
      if (cmd == RES_FILTERMENU_GOLD_MATCHINGALL)
        global_filters.colors |= FC_GOLD_ALLSELECTED;
      if (cmd == RES_FILTERMENU_GOLD_MATCHINGANY)
        global_filters.colors |= FC_GOLD_ANYSELECTED;
      if (old_fc != global_filters.colors)
        refresh_filters = true;
      break;
    }

    case TOGGLE_FILTER(RES_FILTERMENU_LAND_LANDANDMANA, global_filters.cardtypes, FT_LAND_LAND_AND_MANA, FT_LAND); break;
        case TOGGLE_FILTER(RES_FILTERMENU_LAND_LANDONLY, global_filters.cardtypes, FT_LAND_LAND_ONLY, FT_LAND); break;
        case TOGGLE_FILTER(RES_FILTERMENU_LAND_MANAONLY, global_filters.cardtypes, FT_LAND_MANA_ONLY, FT_LAND); break;

        case TOGGLE_FILTER(RES_FILTERMENU_ARTIFACT_CREATURES, global_filters.cardtypes, FT_ARTIFACT_CREATURE, FT_ARTIFACT); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ARTIFACT_NONCREATURES, global_filters.cardtypes, FT_ARTIFACT_NON_CREATURE, FT_ARTIFACT); break;

        case TOGGLE_FILTER_ADD(RES_FILTERMENU_CREATURE_CREATURE, global_filters.cardtypes, FT_CREATURE_CREATURE, FT_CREATURE,
                               global_filters.cardtypes &= ~FT_CREATURE_LIST);
        break;
        case TOGGLE_FILTER_ADD(RES_FILTERMENU_CREATURE_TOKEN, global_filters.cardtypes, FT_CREATURE_TOKEN, FT_CREATURE,
                               global_filters.cardtypes &= ~FT_CREATURE_LIST);
        break;
        case TOGGLE_FILTER_ADD(RES_FILTERMENU_CREATURE_ARTIFACT, global_filters.cardtypes, FT_CREATURE_ARTIFACT, FT_CREATURE,
                               global_filters.cardtypes &= ~FT_CREATURE_LIST);
        break;

        case RES_FILTERMENU_CREATURE_LIST:
      if (!show_dialog_filter_subtype(1))
        break;

      if (!(global_filters.cardtypes & FT_CREATURE_LIST))
      {
        global_filters.cardtypes |= stored_creature_filters;
        stored_creature_filters = FT_0;
      }
      else if (!stored_creature_filters)
      {
        stored_creature_filters = global_filters.cardtypes & (FT_CREATURE_CREATURE | FT_CREATURE_TOKEN | FT_CREATURE_ARTIFACT);
        global_filters.cardtypes &= ~(FT_CREATURE_CREATURE | FT_CREATURE_TOKEN | FT_CREATURE_ARTIFACT);
      }

      if (global_filters.cardtypes & FT_CREATURE)
        refresh_filters = true;
      break;

    case GLE_FILTER(RES_FILTERMENU_COST, global_filters.casting_cost, 0); break;

        case RES_FILTERMENU_COST_X:
      global_filters.casting_cost &= FN_ENABLE;
      global_filters.casting_cost |= FN_CC_X;
      if (global_filters.casting_cost & FN_ENABLE)
        refresh_filters = true;
      break;

    case GLE_FILTER(RES_FILTERMENU_POWER, global_filters.power, 1); break;
        case GLE_FILTER(RES_FILTERMENU_TOUGHNESS, global_filters.toughness, 2); break;

        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_NATIVE, global_filters.abilities, FA_NATIVE, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_GIVES, global_filters.abilities, FA_GRANTS, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_FLYING, global_filters.abilities, FA_FLYING, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_FIRSTSTRIKE, global_filters.abilities, FA_FIRSTSTRIKE, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_TRAMPLE, global_filters.abilities, FA_TRAMPLE, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_REGENERATION, global_filters.abilities, FA_REGENERATION, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_BANDING, global_filters.abilities, FA_BANDING, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_COLORWARD, global_filters.abilities, FA_PROTECTION, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_LANDWALK, global_filters.abilities, FA_LANDWALK, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_POISON, global_filters.abilities, FA_INFECT, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_RAMPAGE, global_filters.abilities, FA_RAMPAGE, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_REACH, global_filters.abilities, FA_REACH, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_STONING, global_filters.abilities, FA_DEATHTOUCH, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_VIGILANCE, global_filters.abilities, FA_VIGILANCE, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_HASTE, global_filters.abilities, FA_HASTE, FA_ENABLE); break;

        case TOGGLE_FILTER(RES_FILTERMENU_RARITY_COMMON, global_filters.rarity, FR_COMMON, FR_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_RARITY_UNCOMMON, global_filters.rarity, FR_UNCOMMON, FR_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_RARITY_RARE, global_filters.rarity, FR_RARE, FR_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_RARITY_RESTRICTED, global_filters.rarity, FR_RESTRICTED, FR_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_RARITY_BANNED, global_filters.rarity, FR_BANNED, FR_ENABLE); break;

        case RES_FILTERMENU_MAINMENUBUTTONS_ON:
      global_filters.colors |= (FC_WHITE | FC_GREEN | FC_RED | FC_BLACK | FC_BLUE | FC_GOLD);
      global_filters.cardsets |= (FS_4TH_EDITION | FS_ASTRAL | FS_ARABIAN_NIGHTS | FS_ANTIQUITIES | FS_THE_DARK | FS_LEGENDS | FS_OTHER);
      global_filters.cardtypes |= (FT_LAND | FT_ARTIFACT | FT_CREATURE | FT_ENCHANTMENT | FT_INSTANT | FT_INTERRUPT | FT_SORCERY);
      refresh_filters = true;
      break;

    case RES_FILTERMENU_MAINMENUBUTTONS_OFF:
      global_filters.colors &= ~(FC_WHITE | FC_GREEN | FC_RED | FC_BLACK | FC_BLUE | FC_GOLD);
      global_filters.cardsets = FS_0;
      global_filters.cardtypes &= ~(FT_LAND | FT_ARTIFACT | FT_CREATURE | FT_ENCHANTMENT | FT_INSTANT | FT_INTERRUPT | FT_SORCERY);
      refresh_filters = true;
      break;

    case TOGGLE_3SETS_FILTER(RES_FILTERMENU_FOURTH_UNLIMITED, FE_UNLIMITED, FE_REVISED, FE_4TH_EDITION); break;
        case TOGGLE_3SETS_FILTER(RES_FILTERMENU_FOURTH_REVISED, FE_REVISED, FE_UNLIMITED, FE_4TH_EDITION); break;
        case TOGGLE_3SETS_FILTER(RES_FILTERMENU_FOURTH_FOURTH, FE_4TH_EDITION, FE_REVISED, FE_UNLIMITED); break;

        case RES_FILTERMENU_EXPANSIONLIST:
      if (show_dialog_filter_subtype(0))
        refresh_filters = true;
      break;

    default:
      return 0;
#undef TOGGLE_FILTER_ADD
#undef TOGGLE_FILTER
#undef TOGGLE_FILTER_NO_INSERT
#undef GLE_FILTER
#undef TOGGLE_3SETS_FILTER
    }
    GetClientRect(hwnd, &lRect2);
    // filterbuttons_setcoords(&lRect2, 1, &lRect1);
    draw_filter_buttons(chdc, &lRect2);
    InvalidateRect(hwnd, NULL, FALSE);
    if (refresh_filters)
    {
      SendMessage(global_horzlist_hwnd, 0x8007, 0, 0);
      filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);
    }
    return 0;
  }

  case 0x8402:
    GetClientRect(hwnd, &rect);

    DELETE_DC(chdc);
    hdc = GetDC(hwnd);
    ApplyCardArtPaletteToDc(hdc);

    chdc = CreateCompatibleDC(hdc);
    ApplyCardArtPaletteToDc(chdc);

    bmp = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
    SelectObject(chdc, bmp);
    ReleaseDC(hwnd, hdc);
    draw_filter_buttons(chdc, &rect);
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;

  case 0x8465:
    return get_filter_button_state(hwnd, wparam, lparam);

  default:
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }
}

#define set_dlg_text(hdlg, resource, txt) SetWindowText(GetDlgItem(hdlg, resource), txt)

// FUNCTION: DECKDLL 0x10020273
INT_PTR CALLBACK dlgproc_FilterGLE(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC hdc;
    RECT r;
    LPARAM lparam_copy; // ebp - 0x114
    HDC hdc2;           // ebp - 0x110
    INT_PTR pad;
    char buf[264];
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
  {
    sprintf(s.buf, "%d", global_dlg_parameter);
    SetWindowTextA(GetDlgItem(hdlg, RES_ASKVALUE_EDITTEXT), s.buf);

    load_text("menus", "OKCANCEL");
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_OK), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_CANCEL), text_lines[1]);
    SetWindowTextA(hdlg, global_filter_dlg_title);

    sprintf(s.buf, "%s\\GAUN_Options.pic", global_duelart_path);
    global_filter_gle_background_pic = (HANDLE)load_pic(s.buf);
    return 0;
  }

  case WM_COMMAND:
    if (((unsigned int)wparam & 0xffff) == RES_BUTTON_OK)
    {
      GetWindowTextA(GetDlgItem(hdlg, RES_ASKVALUE_EDITTEXT), s.buf, 0x105);
      global_filter_gle_dlg_value = atoi(s.buf);
      EndDialog(hdlg, 1);
    }
    else if (((unsigned int)wparam & 0xffff) == RES_BUTTON_CANCEL)
    {
      EndDialog(hdlg, 0);
    }
    return 1;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.hdc2 = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc2);
    s.lparam_copy = lparam;
    SetBkMode(s.hdc2, 1);
    s.pad = (INT_PTR)GetStockObject(5);
    return s.pad;
  case WM_ERASEBKGND:
    s.hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc);
    GetClientRect(hdlg, &s.r);
    if (global_filter_gle_background_pic != (HANDLE)0)
      DrawBitmapToRect(s.hdc, &s.r, global_filter_gle_background_pic);
    else
      FillRect(s.hdc, &s.r, (HBRUSH)GetStockObject(2));
    return 1;

  default:
    return 0;
  }
}

// FUNCTION: DECKDLL 0x10020157
static bool show_dialog_filter_gle(int textline)
{
  INT_PTR dialog_result;
  int rval;

  load_text("menus", "POWERTOUGHNESSCC");
  if (textline == 0x17)
    strcpy(global_filter_dlg_title, text_lines[0]);
  else if (textline == 0x18)
    strcpy(global_filter_dlg_title, text_lines[1]);
  else if (textline == 0x19)
    strcpy(global_filter_dlg_title, text_lines[2]);
  else
    strcpy(global_filter_dlg_title, "Filter Dialog");

  dialog_result = DialogBoxParam(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_ASK_VALUE),
                                 global_main_hwnd, dlgproc_FilterGLE, 0);
  if (dialog_result == -1)
  {
    MessageBox(global_main_hwnd, "Couldn't bring up the filter dialog box", "", MB_OK);
    rval = 0;
  }
  else if (dialog_result == 0)
    rval = 0;
  else if (dialog_result == 1)
    rval = 1;

  return rval;
}

// FUNCTION: DECKDLL 0x10020636
INT_PTR CALLBACK dlgproc_FilterSubtype(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  /* Stack layout is extremely sensitive; keep locals grouped. */
  struct
  {
    RECT r;
    unsigned int local_1f4;
    HDC local_1f0;
    unsigned int local_1ec;
    int local_1e8;
    union
    {
      struct
      {
        int selected[0x33];
        char path[264];
      } sp;
      int selected_overflow[0x34]; /* intentional 4-byte overflow into path */
    } u;
    unsigned int pad_after_union;
    unsigned int local_10;
    unsigned int local_8;
    unsigned int local_4;
  } s;

#define QMASK (*(unsigned __int64 *)&s.local_8)

  switch (msg)
  {
  case WM_INITDIALOG:
    SetWindowTextA(hdlg, global_filter_dlg_title);

    load_text("menus", "LONGLIST");
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_ENABLEFILTER), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_SELECTALL), text_lines[1]);
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_CLEARALL), text_lines[2]);

    load_text("menus", "OKCANCEL");
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_OK), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_CANCEL), text_lines[1]);

    s.local_10 = (unsigned int)load_text("menus", "ARTISTNAMES");
    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);
    ShowWindow(GetDlgItem(hdlg, RES_FILTERLIST_ENABLEFILTER), 0);

    if (s.local_10 != 0xffffffff)
    {
      QMASK = 0;
      while ((signed __int64)QMASK < (signed __int64)(int)s.local_10)
      {
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_ADDSTRING, 0,
                            (LPARAM)(text_lines[0] + (int)(QMASK * 0x80)));
        QMASK = QMASK + 1;
      }
    }

    for (s.local_1e8 = 0; s.local_1e8 < 0x34; s.local_1e8 = s.local_1e8 + 1)
    {
      QMASK = (unsigned __int64)1;
      QMASK = QMASK << (unsigned char)s.local_1e8;

      if ((global_filters.expansion_list[1] & (unsigned int)(QMASK >> 0x20)) || (global_filters.expansion_list[0] & (unsigned int)QMASK))
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 1, s.local_1e8);
      else
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 0, s.local_1e8);
    }

    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETCARETINDEX, 0, 0);
    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);
    sprintf(s.u.sp.path, "%s\\GAUN_Options.pic", global_duelart_path);
    global_filter_subtype_background_pic = (HANDLE)load_pic(s.u.sp.path);
    return 0;

  case WM_ERASEBKGND:
    s.local_1f0 = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.local_1f0);
    GetClientRect(hdlg, &s.r);

    if (global_filter_subtype_background_pic)
      DrawBitmapToRect(s.local_1f0, &s.r, global_filter_subtype_background_pic);
    else
      FillRect(s.local_1f0, &s.r, (HBRUSH)GetStockObject(2));
    return 1;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.local_1f0 = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.local_1f0);
    s.local_1f4 = (unsigned int)lparam;
    SetBkMode(s.local_1f0, 1);
    return (INT_PTR)GetStockObject(5);

  case WM_COMMAND:
    if (LOWORD(wparam) == RES_FILTERLIST_SELECTALL)
    {
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);
      for (s.local_1e8 = 0x32; -1 < s.local_1e8; s.local_1e8 = s.local_1e8 + -1)
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 1, s.local_1e8);
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);
      return 1;
    }

    if (LOWORD(wparam) == RES_BUTTON_OK)
    {
      for (s.local_1e8 = 0; s.local_1e8 < 0x34; s.local_1e8 = s.local_1e8 + 1)
        s.u.selected_overflow[s.local_1e8] = -1;
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_GETSELITEMS, 0x33, (LPARAM)s.u.sp.selected);
      global_filters.expansion_list[0] = 0;
      global_filters.expansion_list[1] = 0;

      for (s.local_1e8 = 0; s.local_1e8 < 0x34; s.local_1e8 = s.local_1e8 + 1)
        if (s.u.selected_overflow[s.local_1e8] != -1)
        {
          QMASK = (unsigned __int64)1;
          QMASK = QMASK << (unsigned char)s.u.selected_overflow[s.local_1e8];
          global_filters.expansion_list[0] |= (unsigned int)QMASK;
          global_filters.expansion_list[1] |= (unsigned int)(QMASK >> 0x20);
        }

      EndDialog(hdlg, 1);
      return 1;
    }

    if (LOWORD(wparam) == RES_BUTTON_CANCEL)
    {
      EndDialog(hdlg, 0);
      return 1;
    }

    if (LOWORD(wparam) == RES_FILTERLIST_CLEARALL)
    {
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);
      for (s.local_1e8 = 0; s.local_1e8 < 0x33; s.local_1e8 = s.local_1e8 + 1)
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 0, s.local_1e8);
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);
      return 1;
    }

    return 1;

  default:
    return 0;
  }
#undef QMASK
}

// FUNCTION: DECKDLL 0x10020c7a
INT_PTR CALLBACK dlgproc_FilterCreatureList(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  /* Stack layout is extremely sensitive; keep locals grouped. */
  struct
  {
    RECT r;
    unsigned int local_4ac;
    HDC local_4a8;
    unsigned int local_4a4;
    int local_4a0;
    int local_49c;
    int selected[0xe0];
    char path[264];
    unsigned int pad_after_path;
    int local_10;
    int local_c;
    unsigned int local_8;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    SetWindowTextA(hdlg, global_filter_dlg_title);

    load_text("menus", "LONGLIST");
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_ENABLEFILTER), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_SELECTALL), text_lines[1]);
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_CLEARALL), text_lines[2]);

    load_text("menus", "OKCANCEL");
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_OK), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_CANCEL), text_lines[1]);

    s.local_8 = (unsigned int)load_text("menus", "CREATURENAMES");
    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);

    if (global_filters.cardtypes & FT_CREATURE_LIST)
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_ENABLEFILTER, BM_SETCHECK, 1, 0);

    if (s.local_8 != 0xffffffff)
      for (s.local_c = 0; s.local_c < (int)s.local_8; s.local_c = s.local_c + 1)
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_INSERTSTRING, 0xffffffff, (LPARAM)(text_lines + s.local_c));

    for (s.local_10 = 0; s.local_10 < 7; s.local_10 = s.local_10 + 1)
      for (s.local_c = 0; s.local_c < 0x20; s.local_c = s.local_c + 1)
      {
        if (global_filters.creature_list[s.local_10] & (1U << ((unsigned char)s.local_c & 0x1f)))
          SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 1, s.local_10 * 0x20 + s.local_c);
        else
          SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 0, s.local_10 * 0x20 + s.local_c);
      }

    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETCARETINDEX, 0, 0);
    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);

    sprintf(s.path, "%s\\GAUN_Options.pic", global_duelart_path);
    global_filter_creature_background_pic = (HANDLE)load_pic(s.path);
    return 0;

  case WM_ERASEBKGND:
    s.local_4a8 = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.local_4a8);
    GetClientRect(hdlg, &s.r);

    if (global_filter_creature_background_pic)
      DrawBitmapToRect(s.local_4a8, &s.r, global_filter_creature_background_pic);
    else
      FillRect(s.local_4a8, &s.r, (HBRUSH)GetStockObject(2));
    return 1;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.local_4a8 = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.local_4a8);
    s.local_4ac = (unsigned int)lparam;
    SetBkMode(s.local_4a8, 1);
    return (INT_PTR)GetStockObject(5);

  case WM_COMMAND:
    if (LOWORD(wparam) == RES_FILTERLIST_SELECTALL)
    {
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);
      for (s.local_c = 0xdc; -1 < s.local_c; s.local_c = s.local_c + -1)
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 1, s.local_c);
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);
      return 1;
    }

    if (LOWORD(wparam) == RES_BUTTON_OK)
    {
      for (s.local_49c = 0; s.local_49c < 0xe0; s.local_49c = s.local_49c + 1)
        s.selected[s.local_49c] = -1;

      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_GETSELITEMS, 0xdd, (LPARAM)s.selected);
      for (s.local_49c = 0; s.local_49c < 7; s.local_49c = s.local_49c + 1)
        global_filters.creature_list[s.local_49c] = 0;

      for (s.local_4a0 = 0; s.local_4a0 < 7; s.local_4a0 = s.local_4a0 + 1)
        for (s.local_49c = 0; s.local_49c < 0xdd; s.local_49c = s.local_49c + 1)
          if (((s.selected[s.local_49c] != -1) && (s.local_4a0 * 0x20 <= s.selected[s.local_49c])) &&
              (s.selected[s.local_49c] < (s.local_4a0 + 1) * 0x20))
          {
            global_filters.creature_list[s.local_4a0] |=
                1U << ((((unsigned char)s.selected[s.local_49c] ^ (unsigned char)(s.selected[s.local_49c] >> 0x1f)) - (unsigned char)(s.selected[s.local_49c] >> 0x1f) & 0x1f ^
                        (unsigned char)(s.selected[s.local_49c] >> 0x1f)) -
                           (unsigned char)(s.selected[s.local_49c] >> 0x1f) &
                       0x1f);
          }

      if (global_filter_creature_background_pic)
        delete_and_close_object(global_filter_creature_background_pic);
      EndDialog(hdlg, 1);
      return 1;
    }

    if (LOWORD(wparam) == RES_BUTTON_CANCEL)
    {
      if (global_filter_creature_background_pic)
        delete_and_close_object(global_filter_creature_background_pic);
      EndDialog(hdlg, 0);
      return 1;
    }

    if (LOWORD(wparam) == RES_FILTERLIST_CLEARALL)
    {
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);
      for (s.local_c = 0; s.local_c < 0xdd; s.local_c = s.local_c + 1)
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 0, s.local_c);
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);
      return 1;
    }

    if (LOWORD(wparam) == RES_FILTERLIST_ENABLEFILTER)
    {
      global_filters.cardtypes ^= FT_CREATURE_LIST;
      return 1;
    }

    return 1;

  default:
    return 0;
  }
}

// FUNCTION: DECKDLL 0x10020508
static int show_dialog_filter_subtype(int mode)
{
  int dialog_result;
  int rval;

  load_text("menus", "FILTERDIALOGTITLE");
  if (mode == 0)
    strcpy(global_filter_dlg_title, text_lines[0]);
  else if (mode == 1)
    strcpy(global_filter_dlg_title, text_lines[1]);
  else
    strcpy(global_filter_dlg_title, text_lines[2]);

  if (mode == 0)
    dialog_result = DialogBoxParamA(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_FILTER_LIST), global_main_hwnd, dlgproc_FilterSubtype, 0);
  else
    dialog_result = DialogBoxParamA(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_FILTER_LIST), global_main_hwnd, dlgproc_FilterCreatureList, 0);

  if (dialog_result == -1)
  {
    MessageBoxA(global_main_hwnd, "Couldn't bring up the filter dialog box", "", MB_OK);
    rval = 0;
  }
  else if (dialog_result == 0)
    rval = 0;
  else if (dialog_result == 1)
    rval = 1;

  return rval;
}
