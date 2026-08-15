#include <STDDEF.H>
#include "filtermenus.h"
#include "mystdbool.h"
#include "cardartlib/src/palette.h"
#include "drawcardlib/src/pic.h"

bool check_restricted(csvid_t csvid);
bool check_banned(csvid_t csvid);

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
extern GlobalConfig global_cfg;

extern HDC global_hdc;
extern HINSTANCE global_hinstance;
extern HBITMAP global_hbmp;
extern HBRUSH global_create_brush_5;

extern HWND global_main_hwnd;
extern HWND global_listbox_hwnd;
extern HWND global_horzlist_hwnd;
extern HWND cardlistfilter_hwnd;
extern int global_dlg_parameter;
extern char global_base_directory[];
extern char global_duelart_path[];

extern HWND global_main_hwnd;

extern int global_deck_num_entries;
extern GlobalDeckEntry global_deck[];
extern int global_available_slots;
extern card_ptr_t global_raw_cards_storage[2000];
extern char read_db_artist_names[][100];
extern DBFlags global_db_flags_1;

extern OrigRarities global_origrarities[];

extern struct global_filters_t global_filters;

extern int (*global_is_valid_card_fn)(int);

int IsCardAvailable(csvid_t csvid, int expansion);

int load_text(const char *file_name, const char *section_name);
void filter_cards_in_lists(HWND hwnd_listbox, HWND hwnd_horzlist);
void play_sound(int a1, int a2, int a3, int a4);
unsigned int HasExpansion(unsigned char expansion_mask);

static void rotatem();

// Forward declarations
static bool show_dialog_filter_gle(int textline);
static int show_dialog_filter_subtype(int mode);
// GLOBAL: DECKDLL 0x10104d80
static int global_filter_gle_dlg_value;

// GLOBAL: DECKDLL 0x10104d84
static HANDLE global_filter_creature_background_pic;

// GLOBAL: DECKDLL 0x10104d88
static HDC global_filter_buttons_hdc;
/* Padding (unnamed in the original). */
static char pad_10104d8c[0x0c];
// GLOBAL: DECKDLL 0x10104d98
static HFONT global_filter_buttons_font;
// GLOBAL: DECKDLL 0x10104da0
static RECT global_filter_buttons_rect;

struct filter_dlg_title_t
{
  char title[52];
  int pad_34;
  HBITMAP bmp;
};

// GLOBAL: DECKDLL 0x10104db0
static struct filter_dlg_title_t global_filter_dlg_title;

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
static bool global_filtermenu_castcost_enabled;
// GLOBAL: DECKDLL 0x10113d10
static bool global_filtermenu_power_enabled;
// GLOBAL: DECKDLL 0x10113d14
static bool global_filtermenu_toughness_enabled;
// GLOBAL: DECKDLL 0x10113d18
static bool global_filtermenu_ability_enabled;
// GLOBAL: DECKDLL 0x10113d1c
static bool global_filtermenu_rarity_enabled;
// GLOBAL: DECKDLL 0x10113d20
static bool global_filtermenu_artist_enabled;
// GLOBAL: DECKDLL 0x10113d08
static bool global_filtermenu_gold_enabled;

// GLOBAL: DECKDLL 0x101a9190
struct global_filters_t global_filters;

// GLOBAL: DECKDLL 0x10113d04
short global_filter_cardsets_flags;

// GLOBAL: DECKDLL 0x10104d9c
static HANDLE global_filter_subtype_background_pic;

// GLOBAL: DECKDLL 0x10104de4
static HANDLE global_filter_gle_background_pic;

// FUNCTION: DECKDLL 0x1001d67f
static int filterbuttons_setcoords(const RECT *r, int button_number, RECT *rval)
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
static HMENU select_filter_menu(HWND hwnd, POINT p)
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
static void draw_filter_button_3d(HDC hdc, RECT *r2, RECT *r3, bool pushed)
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
  case 11:
    SelectObject(global_hdc, global_pic_yellow);
    break;
  case 15:
    SelectObject(global_hdc, global_pic_green);
    break;
  case 14:
    SelectObject(global_hdc, global_pic_red);
    break;
  case 13:
    SelectObject(global_hdc, global_pic_grey);
    break;
  case 12:
    SelectObject(global_hdc, global_pic_blue);
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
  case 6:
    SelectObject(global_hdc, global_pic_astral);
    break;
  case 5:
    SelectObject(global_hdc, global_pic_all_butn);
    break;
  case 7:
    SelectObject(global_hdc, global_pic_arabnite);
    break;
  case 8:
    SelectObject(global_hdc, global_pic_antiquit);
    break;
  case 1:
    SelectObject(global_hdc, global_pic_fourth);
    break;
  case 9:
    SelectObject(global_hdc, global_pic_legends);
    break;
  case 10:
    SelectObject(global_hdc, global_pic_dark);
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
static void draw_filter_buttons(HDC hdc, RECT r)
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

  StretchBlt(hdc, 0, 0, r.right, r.bottom, s.chdc, 0, 0, s.bmp.bmWidth, s.bmp.bmHeight, SRCCOPY);
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
  // TODO: why are the RES_ consts here fucked up?

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
      AppendMenu(global_filtermenu_enchantment, MF_ENABLED, s.i + RES_FILTERMENU_ENCHANTMENT_LAND, text_lines[s.i]);
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
        AppendMenu(global_filtermenu_artist, MF_ENABLED, s.i + RES_FILTERMENU_ARTIST, text_lines[s.i]);
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
  struct
  {
    HDC hdc2;
    PAINTSTRUCT paint; // ebp - 0xa4
    POINT pt2;
    HMENU menu;           // ebp - 0x5c
    POINT pt;             // ebp - 0x58
    int i;                // ebp - 0x50
    RECT r;               // ebp - 0x4c
    RECT r2;              // ebp - 0x3c
    UINT refresh_filters; // ebp - 0x2c
    HDC hdc;              // ebp - 0x28
    int cmd;              // ebp - 0x24
    RECT r3;              // ebp - 0x20
    RECT r4;              // ebp - 0x10
  } s;

  // GLOBAL: DECKDLL 0x10035684
  static int stored_creature_filters = FT_0;

  switch (msg)
  {
  case 0x402:
    GetClientRect(cardlistfilter_hwnd, &global_filter_buttons_rect);

    if (global_filter_buttons_hdc)
      DeleteDC(global_filter_buttons_hdc);
    s.hdc = GetDC(hwnd);
    ApplyCardArtPaletteToDc(s.hdc);

    global_filter_buttons_hdc = CreateCompatibleDC(s.hdc);
    ApplyCardArtPaletteToDc(global_filter_buttons_hdc);

    global_filter_dlg_title.bmp =
        CreateCompatibleBitmap(s.hdc, global_filter_buttons_rect.right - global_filter_buttons_rect.left, global_filter_buttons_rect.bottom - global_filter_buttons_rect.top);
    SelectObject(global_filter_buttons_hdc, global_filter_dlg_title.bmp);

    global_filter_buttons_font = CreateFontA(global_filter_buttons_rect.bottom - 6, 0, 0, 0, 400, 0, 0, 0, 0, 4, 0, 0, 0x40, NULL);
    SelectObject(global_filter_buttons_hdc, global_filter_buttons_font);

    ReleaseDC(hwnd, s.hdc);

    FillRect(global_filter_buttons_hdc, &global_filter_buttons_rect, global_create_brush_5);
    draw_filter_buttons(global_filter_buttons_hdc, global_filter_buttons_rect);
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;

  case 0x465:
    return get_filter_button_state(hwnd, wparam, lparam);

  case WM_COMMAND:
  {
    s.refresh_filters = 0;
    // This is the part that breaks decompilation.  Starting at 0x40852a, change "FF 24 85 14 8a 40 00" to "E9 02 00 00 00 90 90" to force case 1.
    // s.cmd = ;
    switch (LOWORD(wparam))
    {
    case RES_FILTERMENU_MAINMENUBUTTONS_ON:
      global_filters.colors |= (FC_WHITE | FC_GREEN | FC_RED | FC_BLACK | FC_BLUE);
      global_filters.cardsets |= (FS_4TH_EDITION | FS_ASTRAL | FS_ARABIAN_NIGHTS | FS_ANTIQUITIES | FS_THE_DARK | FS_LEGENDS);
      global_filters.cardtypes |= (FT_LAND | FT_ARTIFACT | FT_CREATURE | FT_ENCHANTMENT | FT_INSTANT | FT_INTERRUPT | FT_SORCERY);
      s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_MAINMENUBUTTONS_OFF:
      global_filters.colors &= ~(FC_WHITE | FC_GREEN | FC_RED | FC_BLACK | FC_BLUE);
      global_filters.cardsets = FS_0;
      global_filters.cardtypes &= ~(FT_LAND | FT_ARTIFACT | FT_CREATURE | FT_ENCHANTMENT | FT_INSTANT | FT_INTERRUPT | FT_SORCERY);
      s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_FOURTH_UNLIMITED:
      if (!(global_filters.expansions & FE_UNLIMITED))
      {
        global_filters.expansions |= FE_UNLIMITED;
      }
      else if ((global_filters.expansions & (FE_REVISED | FE_4TH_EDITION)))
      {
        global_filters.expansions ^= FE_UNLIMITED;
      }
      if (global_filters.cardsets & FS_4TH_EDITION)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_FOURTH_REVISED:
      if (!(global_filters.expansions & FE_REVISED))
      {
        global_filters.expansions |= FE_REVISED;
      }
      else if (global_filters.expansions & (FE_UNLIMITED | FE_4TH_EDITION))
      {
        global_filters.expansions ^= FE_REVISED;
      }
      if (global_filters.cardsets & FS_4TH_EDITION)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_FOURTH_FOURTH:
      if (!(global_filters.expansions & FE_4TH_EDITION))
      {
        global_filters.expansions |= FE_4TH_EDITION;
      }
      else if (global_filters.expansions & (FE_REVISED | FE_UNLIMITED))
      {
        global_filters.expansions ^= FE_4TH_EDITION;
      }
      if (global_filters.cardsets & FS_4TH_EDITION)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_GOLD_ALL:
    case RES_FILTERMENU_GOLD_MATCHINGALL:
    case RES_FILTERMENU_GOLD_MATCHINGANY:
    {
      global_filters.colors &= (unsigned short)(~(FC_GOLD_ALL | FC_GOLD_ALLSELECTED | FC_GOLD_ANYSELECTED | FC_GOLD_ALLUNKNOWN));
      if (LOWORD(wparam) == RES_FILTERMENU_GOLD_ALL)
        global_filters.colors |= FC_GOLD_ALL;
      if (LOWORD(wparam) == RES_FILTERMENU_GOLD_MATCHINGALL)
        global_filters.colors |= FC_GOLD_ALLSELECTED;
      if (LOWORD(wparam) == RES_FILTERMENU_GOLD_MATCHINGANY)
        global_filters.colors |= FC_GOLD_ANYSELECTED;

      break;
    }

    case RES_FILTERMENU_LAND_LANDANDMANA:
      global_filters.cardtypes ^= FT_LAND_LAND_AND_MANA;
      if (global_filters.cardtypes & FT_LAND)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_LAND_LANDONLY:
      global_filters.cardtypes ^= FT_LAND_LAND_ONLY;
      if (global_filters.cardtypes & FT_LAND)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_LAND_MANAONLY:
      global_filters.cardtypes ^= FT_LAND_MANA_ONLY;
      if (global_filters.cardtypes & FT_LAND)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_ARTIFACT_CREATURES:
      global_filters.cardtypes ^= FT_ARTIFACT_CREATURE;
      if (global_filters.cardtypes & FT_ARTIFACT)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ARTIFACT_NONCREATURES:
      global_filters.cardtypes ^= FT_ARTIFACT_NON_CREATURE;
      if (global_filters.cardtypes & FT_ARTIFACT)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_CREATURE_CREATURE:
      global_filters.cardtypes ^= FT_CREATURE_CREATURE;
      if (global_filters.cardtypes & FT_CREATURE_LIST)
        global_filters.cardtypes ^= FT_CREATURE_LIST;
      if (global_filters.cardtypes & FT_CREATURE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_CREATURE_TOKEN:
      global_filters.cardtypes ^= FT_CREATURE_TOKEN;
      if (global_filters.cardtypes & FT_CREATURE_LIST)
        global_filters.cardtypes ^= FT_CREATURE_LIST;
      if (global_filters.cardtypes & FT_CREATURE)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_CREATURE_ARTIFACT:
      global_filters.cardtypes ^= FT_CREATURE_ARTIFACT;
      if (global_filters.cardtypes & FT_CREATURE_LIST)
        global_filters.cardtypes ^= FT_CREATURE_LIST;
      if (global_filters.cardtypes & FT_CREATURE)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_CREATURE_LIST:
      if (!show_dialog_filter_subtype(1))
        break;

      if (global_filters.cardtypes & FT_CREATURE_LIST)
      {
        if (!stored_creature_filters)
        {
          stored_creature_filters = global_filters.cardtypes & (FT_CREATURE_CREATURE | FT_CREATURE_TOKEN | FT_CREATURE_ARTIFACT);

          global_filters.cardtypes = (uint)global_filters.cardtypes & ~(FT_CREATURE_CREATURE | FT_CREATURE_TOKEN | FT_CREATURE_ARTIFACT);
        }
      }
      else
      {
        global_filters.cardtypes |= stored_creature_filters;
        stored_creature_filters = FT_0;
      }

      if (global_filters.cardtypes & FT_CREATURE)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_ENCHANTMENT_LAND:
      global_filters.cardtypes ^= FT_ENCHANTMENT_LAND;
      if (global_filters.cardtypes & FT_ENCHANTMENT)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_ENCHANTMENT_CREATURE:
      global_filters.cardtypes ^= FT_ENCHANTMENT_CREATURE;
      if (global_filters.cardtypes & FT_ENCHANTMENT)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_ENCHANTMENT_ARTIFACT:
      global_filters.cardtypes ^= FT_ENCHANTMENT_ARTIFACT;
      if (global_filters.cardtypes & FT_ENCHANTMENT)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_ENCHANTMENT_ENCHANTMENTS:
      global_filters.cardtypes ^= FT_ENCHANTMENT_ENCHANT;
      if (global_filters.cardtypes & FT_ENCHANTMENT)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_ENCHANTMENT_ENCHANT:
      global_filters.cardtypes ^= FT_ENCHANTMENT_PERMANENT;
      if (global_filters.cardtypes & FT_ENCHANTMENT)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_ENCHANTMENT_WORLD:
      global_filters.cardtypes ^= FT_ENCHANTMENT_PLAYER;
      if (global_filters.cardtypes & FT_ENCHANTMENT)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_COST_GREATER:
    case RES_FILTERMENU_COST_LESSER:
    case RES_FILTERMENU_COST_EQUAL:
      global_dlg_parameter = global_filters.casting_cost_value;
      if (!show_dialog_filter_gle(0x17))
        break;
      global_filters.casting_cost_value = (short)global_filter_gle_dlg_value;
      global_filters.casting_cost &= FN_ENABLE;
      if (LOWORD(wparam) == RES_FILTERMENU_COST_GREATER)
        global_filters.casting_cost |= FN_GT;
      if (LOWORD(wparam) == RES_FILTERMENU_COST_LESSER)
        global_filters.casting_cost |= FN_LT;
      if (LOWORD(wparam) == RES_FILTERMENU_COST_EQUAL)
        global_filters.casting_cost |= FN_EQ;
      if (global_filters.casting_cost & FN_ENABLE)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_COST_X:
      global_filters.casting_cost &= FN_ENABLE;
      global_filters.casting_cost |= FN_CC_X;
      if (global_filters.casting_cost & FN_ENABLE)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_POWER_GREATER:
    case RES_FILTERMENU_POWER_LESSER:
    case RES_FILTERMENU_POWER_EQUAL:
      global_dlg_parameter = global_filters.power_value;
      if (!show_dialog_filter_gle(0x18))
        break;
      global_filters.power_value = (short)global_filter_gle_dlg_value;
      global_filters.power &= FN_ENABLE;
      if (LOWORD(wparam) == RES_FILTERMENU_POWER_GREATER)
        global_filters.power |= FN_GT;
      if (LOWORD(wparam) == RES_FILTERMENU_POWER_LESSER)
        global_filters.power |= FN_LT;
      if (LOWORD(wparam) == RES_FILTERMENU_POWER_EQUAL)
        global_filters.power |= FN_EQ;
      if (global_filters.power & FN_ENABLE)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_TOUGHNESS_GREATER:
    case RES_FILTERMENU_TOUGHNESS_LESSER:
    case RES_FILTERMENU_TOUGHNESS_EQUAL:
      global_dlg_parameter = global_filters.toughness_value;
      if (!show_dialog_filter_gle(0x19))
        break;
      global_filters.toughness_value = (short)global_filter_gle_dlg_value;
      global_filters.toughness &= FN_ENABLE;
      if (LOWORD(wparam) == RES_FILTERMENU_TOUGHNESS_GREATER)
        global_filters.toughness |= FN_GT;
      if (LOWORD(wparam) == RES_FILTERMENU_TOUGHNESS_LESSER)
        global_filters.toughness |= FN_LT;
      if (LOWORD(wparam) == RES_FILTERMENU_TOUGHNESS_EQUAL)
        global_filters.toughness |= FN_EQ;
      if (global_filters.toughness & FN_ENABLE)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_ABILITY_NATIVE:
      global_filters.abilities ^= FA_NATIVE;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_GIVES:
      global_filters.abilities ^= FA_GRANTS;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_FLYING:
      global_filters.abilities ^= FA_FLYING;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_FIRSTSTRIKE:
      global_filters.abilities ^= FA_FIRSTSTRIKE;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_TRAMPLE:
      global_filters.abilities ^= FA_TRAMPLE;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_REGENERATION:
      global_filters.abilities ^= FA_REGENERATION;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_BANDING:
      global_filters.abilities ^= FA_BANDING;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_COLORWARD:
      global_filters.abilities ^= FA_PROTECTION;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_LANDWALK:
      global_filters.abilities ^= FA_LANDWALK;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_POISON:
      global_filters.abilities ^= FA_INFECT;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_RAMPAGE:
      global_filters.abilities ^= FA_RAMPAGE;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_REACH:
      global_filters.abilities ^= FA_REACH;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_STONING:
      global_filters.abilities ^= FA_DEATHTOUCH;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_VIGILANCE:
      global_filters.abilities ^= FA_VIGILANCE;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_ABILITY_HASTE:
      global_filters.abilities ^= FA_HASTE;
      if (global_filters.abilities & FA_ENABLE)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_RARITY_COMMON:
      global_filters.rarity ^= FR_COMMON;
      if (global_filters.rarity & FR_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_RARITY_UNCOMMON:
      global_filters.rarity ^= FR_UNCOMMON;
      if (global_filters.rarity & FR_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_RARITY_RARE:
      global_filters.rarity ^= FR_RARE;
      if (global_filters.rarity & FR_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_RARITY_RESTRICTED:
      global_filters.rarity ^= FR_RESTRICTED;
      if (global_filters.rarity & FR_ENABLE)
        s.refresh_filters = 1;
      break;
    case RES_FILTERMENU_RARITY_BANNED:
      global_filters.rarity ^= FR_BANNED;
      if (global_filters.rarity & FR_ENABLE)
        s.refresh_filters = 1;
      break;

    case RES_FILTERMENU_ARTIST:
      if (show_dialog_filter_subtype(0))
      {
        if (global_filters.artist & 1)
          s.refresh_filters = 1;
      }
      break;

    default:
      return 0;
    }

    s.cmd = 1;
    if (s.cmd)
    {
      GetClientRect(hwnd, &s.r3);
      filterbuttons_setcoords(&s.r3, s.cmd, &s.r4);
      FillRect(global_filter_buttons_hdc, &s.r3, global_create_brush_5);

      draw_filter_buttons(global_filter_buttons_hdc, s.r3);
      InvalidateRect(hwnd, NULL, FALSE);
    }

    if (s.refresh_filters == 1)
    {
      SendMessage(global_horzlist_hwnd, 0x186, 0, 0);
      filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);
    }
    return 0;
  }

  case WM_CREATE:
    global_filters.colors = FC_WHITE | FC_GREEN | FC_RED | FC_BLACK | FC_BLUE;

    global_filters.expansions = FE_4TH_EDITION | FE_REVISED | FE_UNLIMITED;

    global_filters.cardtypes = (FT_LAND | FT_ARTIFACT | FT_CREATURE | FT_ENCHANTMENT | FT_INSTANT | FT_INTERRUPT | FT_SORCERY);

    global_filters.cardtypes |= FT_ARTIFACT_CREATURE | FT_ARTIFACT_NON_CREATURE;
    global_filters.cardtypes |= FT_LAND_LAND_AND_MANA | FT_LAND_LAND_ONLY;
    global_filters.cardtypes |= FT_CREATURE_CREATURE;
    global_filters.cardtypes |= FT_ENCHANTMENT_LAND | FT_ENCHANTMENT_CREATURE | FT_ENCHANTMENT_ARTIFACT | FT_ENCHANTMENT_ENCHANT |
                                FT_ENCHANTMENT_PERMANENT | FT_ENCHANTMENT_PLAYER;

    global_filters.casting_cost = FN_GT;
    global_filters.casting_cost_value = 0;

    global_filters.power = FN_GT;
    global_filters.power_value = 0;

    global_filters.toughness = FN_GT;
    global_filters.toughness_value = 0;

    global_filters.abilities = -2;

    global_filters.rarity = FR_COMMON | FR_UNCOMMON | FR_RARE | FR_RESTRICTED | FR_BANNED;
    global_filters.artist = 0;

    global_filters.artists_list = 0x7ffffffffffff;

    for (s.cmd = 0; s.cmd < CREATURE_LIST_SIZE; s.cmd++)
      global_filters.creature_list[s.cmd] = -1;

    if ((global_db_flags_1 & DBFLAGS_STANDALONE) || (global_db_flags_1 & 0x4c))
    {
      global_filter_cardsets_flags = (FCSF_Q_ENABLE | FCSF_Q_ASTRAL);
      global_filters.cardsets = (FS_4TH_EDITION | FS_ASTRAL | FS_ARABIAN_NIGHTS | FS_ANTIQUITIES);

      if (HasExpansion(2))
      {
        global_filter_cardsets_flags |= (FCSF_ARABIAN_NIGHTS | FCSF_ANTIQUITIES);
        global_filters.cardsets |= (FS_ARABIAN_NIGHTS | FS_ANTIQUITIES);
      }

      if (HasExpansion(4))
      {
        global_filter_cardsets_flags |= (FCSF_THE_DARK | FCSF_LEGENDS);
        global_filters.cardsets |= (FS_THE_DARK | FS_LEGENDS);
      }
    }
    else
    {
      global_filter_cardsets_flags = FCSF_0;
      global_filters.cardsets =
          (FS_4TH_EDITION | FS_ASTRAL | FS_ARABIAN_NIGHTS | FS_ANTIQUITIES | FS_THE_DARK | FS_LEGENDS);
    }

    global_filtermenu_castcost_enabled = true;
    global_filtermenu_power_enabled = true;
    global_filtermenu_toughness_enabled = true;
    global_filtermenu_ability_enabled = true;
    global_filtermenu_rarity_enabled = true;
    global_filtermenu_artist_enabled = true;

    filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);

    create_filter_menus();
    return 0;

  case WM_DESTROY:
    if (global_filter_buttons_hdc)
      DeleteDC(global_filter_buttons_hdc);
    DeleteObject((HGDIOBJ)global_filter_dlg_title.bmp);
    DeleteObject(global_filter_buttons_font);
    destroy_filter_menus();
    KillTimer(hwnd, 1);
    return 0;

  case WM_INITMENU:
    if ((HMENU)wparam == global_filtermenu_fourth)
    {
      CheckMenuItem(global_filtermenu_fourth, RES_FILTERMENU_FOURTH_UNLIMITED,
                    ((int)global_filters.expansions & FE_UNLIMITED) >> 9);
      CheckMenuItem(global_filtermenu_fourth, RES_FILTERMENU_FOURTH_REVISED,
                    ((int)global_filters.expansions & FE_REVISED) >> 2);
      CheckMenuItem(global_filtermenu_fourth, RES_FILTERMENU_FOURTH_FOURTH,
                    ((int)global_filters.expansions & FE_4TH_EDITION) << 2);
    }
    else if ((HMENU)wparam == global_filtermenu_gold)
    {
      CheckMenuItem(global_filtermenu_gold, RES_FILTERMENU_GOLD_ALL, ((int)global_filters.colors & FC_GOLD_ALL) >> 5);
      CheckMenuItem(global_filtermenu_gold, RES_FILTERMENU_GOLD_MATCHINGALL,
                    ((int)global_filters.colors & FC_GOLD_ALLSELECTED) >> 6);
      CheckMenuItem(global_filtermenu_gold, RES_FILTERMENU_GOLD_MATCHINGANY,
                    ((int)global_filters.colors & FC_GOLD_ANYSELECTED) >> 7);
    }
    else if ((HMENU)wparam == global_filtermenu_land)
    {
      CheckMenuItem(global_filtermenu_land, RES_FILTERMENU_LAND_LANDANDMANA,
                    (global_filters.cardtypes & FT_LAND_LAND_AND_MANA) << 2);
      CheckMenuItem(global_filtermenu_land, RES_FILTERMENU_LAND_LANDONLY,
                    (global_filters.cardtypes & FT_LAND_LAND_ONLY) * 2);
      CheckMenuItem(global_filtermenu_land, RES_FILTERMENU_LAND_MANAONLY,
                    (global_filters.cardtypes & FT_LAND_MANA_ONLY));
    }
    else if ((HMENU)wparam == global_filtermenu_artifact)
    {
      CheckMenuItem(global_filtermenu_artifact, RES_FILTERMENU_ARTIFACT_CREATURES,
                    (global_filters.cardtypes & FT_ARTIFACT_CREATURE) >> 2);
      CheckMenuItem(global_filtermenu_artifact, RES_FILTERMENU_ARTIFACT_NONCREATURES,
                    (global_filters.cardtypes & FT_ARTIFACT_NON_CREATURE) >> 3);
    }
    else if ((HMENU)wparam == global_filtermenu_creature)
    {
      CheckMenuItem(global_filtermenu_creature, RES_FILTERMENU_CREATURE_CREATURE,
                    (global_filters.cardtypes & FT_CREATURE_CREATURE) >> 5);
      CheckMenuItem(global_filtermenu_creature, RES_FILTERMENU_CREATURE_TOKEN,
                    (global_filters.cardtypes & FT_CREATURE_TOKEN) >> 6);
      CheckMenuItem(global_filtermenu_creature, RES_FILTERMENU_CREATURE_ARTIFACT,
                    (global_filters.cardtypes & FT_CREATURE_ARTIFACT) >> 7);
      CheckMenuItem(global_filtermenu_creature, RES_FILTERMENU_CREATURE_LIST,
                    (global_filters.cardtypes & FT_CREATURE_LIST) >> 8);
    }
    else if ((HMENU)wparam == global_filtermenu_enchantment)
    {
      CheckMenuItem(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_LAND,
                    (global_filters.cardtypes & FT_ENCHANTMENT_LAND) >> 10);
      CheckMenuItem(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_CREATURE,
                    (global_filters.cardtypes & FT_ENCHANTMENT_CREATURE) >> 11);
      CheckMenuItem(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_ARTIFACT,
                    (global_filters.cardtypes & FT_ENCHANTMENT_ARTIFACT) >> 12);

      CheckMenuItem(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_ENCHANTMENTS,
                    (global_filters.cardtypes & FT_ENCHANTMENT_ENCHANT) >> 13);
      CheckMenuItem(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_ENCHANT,
                    (global_filters.cardtypes & FT_ENCHANTMENT_PERMANENT) >> 14);
      CheckMenuItem(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_WORLD,
                    (global_filters.cardtypes & FT_ENCHANTMENT_PLAYER) >> 15);
    }
    else if ((HMENU)wparam == global_filtermenu_castcost)
    {
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_GREATER, global_filters.casting_cost & FN_GT);
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_LESSER, global_filters.casting_cost & FN_LT);
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_EQUAL, global_filters.casting_cost & FN_EQ);
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_X, global_filters.casting_cost & FN_CC_X);
    }
    else if ((HMENU)wparam == global_filtermenu_power)
    {
      CHECKMENU_IF(global_filtermenu_power, RES_FILTERMENU_POWER_GREATER, global_filters.power & FN_GT);
      CHECKMENU_IF(global_filtermenu_power, RES_FILTERMENU_POWER_LESSER, global_filters.power & FN_LT);
      CHECKMENU_IF(global_filtermenu_power, RES_FILTERMENU_POWER_EQUAL, global_filters.power & FN_EQ);
    }
    else if ((HMENU)wparam == global_filtermenu_toughness)
    {
      CHECKMENU_IF(global_filtermenu_toughness, RES_FILTERMENU_TOUGHNESS_GREATER, global_filters.toughness & FN_GT);
      CHECKMENU_IF(global_filtermenu_toughness, RES_FILTERMENU_TOUGHNESS_LESSER, global_filters.toughness & FN_LT);
      CHECKMENU_IF(global_filtermenu_toughness, RES_FILTERMENU_TOUGHNESS_EQUAL, global_filters.toughness & FN_EQ);
    }
    else if ((HMENU)wparam == global_filtermenu_ability)
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
    else if ((HMENU)wparam == global_filtermenu_rarity)
    {
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_COMMON, global_filters.rarity & FR_COMMON);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_UNCOMMON, global_filters.rarity & FR_UNCOMMON);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_RARE, global_filters.rarity & FR_RARE);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_RESTRICTED, global_filters.rarity & FR_RESTRICTED);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_BANNED, global_filters.rarity & FR_BANNED);
    }
    return 0;

  case WM_LBUTTONDOWN:
  {
    s.pt.x = lparam & 0xffff;
    s.pt.y = GET_Y_LPARAM(lparam);

    GetClientRect(hwnd, &s.r);
    for (s.i = 0; s.i < 35; ++s.i)
    {
      filterbuttons_setcoords(&s.r, s.i, &s.r2);
      if (PtInRect(&s.r2, s.pt))
      {
        FillRect(global_filter_buttons_hdc, &s.r, global_create_brush_5);

        if (toggle_filterbutton(s.i))
        {
          if (global_cfg.effects)
            play_sound(4, 400, 0, 0);
        }
        else if (global_cfg.effects)
        {
          play_sound(5, 400, 0, 0);
        }

        SendMessage(global_horzlist_hwnd, 0x186, 0, 0);
        filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);
        draw_filter_buttons(global_filter_buttons_hdc, s.r);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
      }
    }
    return 0;
  }

  case WM_RBUTTONDOWN:
    s.pt2.x = lparam & 0xffff;
    s.pt2.y = GET_Y_LPARAM(lparam);
    s.menu = select_filter_menu(hwnd, s.pt2);
    if (s.menu)
    {
      ClientToScreen(hwnd, &s.pt2);
      TrackPopupMenu(s.menu, TPM_RIGHTBUTTON, s.pt2.x, s.pt2.y, 0, hwnd, NULL);
    }
    return 0;

  case WM_PAINT:
    s.hdc2 = BeginPaint(hwnd, &s.paint);
    ApplyCardArtPaletteToDc(s.hdc2);
    BitBlt(s.hdc2, global_filter_buttons_rect.left, global_filter_buttons_rect.top, global_filter_buttons_rect.right - global_filter_buttons_rect.left,
           global_filter_buttons_rect.bottom - global_filter_buttons_rect.top, global_filter_buttons_hdc, 0, 0, SRCCOPY);
    EndPaint(hwnd, &s.paint);
    return 0;

  default:
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }

  return 0;
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
    SetWindowTextA(hdlg, global_filter_dlg_title.title);

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
    SetBkMode(s.hdc2, TRANSPARENT);
    s.pad = (INT_PTR)GetStockObject(HOLLOW_BRUSH);
    return s.pad;
  case WM_ERASEBKGND:
    s.hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc);
    GetClientRect(hdlg, &s.r);
    if (global_filter_gle_background_pic != (HANDLE)0)
      DrawBitmapToRect(s.hdc, &s.r, global_filter_gle_background_pic);
    else
      FillRect(s.hdc, &s.r, (HBRUSH)GetStockObject(GRAY_BRUSH));
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
    strcpy(global_filter_dlg_title.title, text_lines[0]);
  else if (textline == 0x18)
    strcpy(global_filter_dlg_title.title, text_lines[1]);
  else if (textline == 0x19)
    strcpy(global_filter_dlg_title.title, text_lines[2]);
  else
    strcpy(global_filter_dlg_title.title, "Filter Dialog");

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
    HDC hdc2; // ebp - 0x204
    RECT r;
    unsigned int ctl_hwnd;
    HDC hdc;           // ebp - 0x1ec
    HGDIOBJ hollow_brush; // ebp - 0x1e8
    int selected_idx;

    int selected[0x33];
    char path[264];

    unsigned int artist_count;
    unsigned __int64 qmask;
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    SetWindowTextA(hdlg, global_filter_dlg_title.title);

    load_text("menus", "LONGLIST");
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_ENABLEFILTER), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_SELECTALL), text_lines[1]);
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_CLEARALL), text_lines[2]);

    load_text("menus", "OKCANCEL");
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_OK), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_CANCEL), text_lines[1]);

    s.artist_count = (unsigned int)load_text("menus", "ARTISTNAMES");
    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);
    ShowWindow(GetDlgItem(hdlg, RES_FILTERLIST_ENABLEFILTER), 0);

    if (s.artist_count != 0xffffffff)
    {
      s.qmask = 0;
      while (s.qmask < (signed __int64)(int)s.artist_count)
      {
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_ADDSTRING, 0,
                            (LPARAM)(text_lines[0] + (int)(s.qmask * 0x80)));
        s.qmask = s.qmask + 1;
      }
    }

    s.qmask = 0;
    while (s.qmask < 0x34)
    {
      if ((((__int64)1 << (int)s.qmask) & global_filters.artists_list) != 0)
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 1, (LPARAM)(int)s.qmask);
      else
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 0, (LPARAM)(int)s.qmask);

      s.qmask = s.qmask + 1;
    }

    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETCARETINDEX, 0, 0);
    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);
    sprintf(s.path, "%s\\GAUN_Options.pic", global_duelart_path);
    global_filter_subtype_background_pic = (HANDLE)load_pic(s.path);
    return 0;

  case WM_COMMAND:
    switch (LOWORD(wparam))
    {
    case RES_BUTTON_OK:
      for (s.selected_idx = 0; s.selected_idx < 0x34; s.selected_idx = s.selected_idx + 1)
        s.selected[s.selected_idx] = -1;
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_GETSELITEMS, 0x33, (LPARAM)s.selected);
      global_filters.artists_list = 0;

      for (s.selected_idx = 0; s.selected_idx < 0x34; s.selected_idx = s.selected_idx + 1)
        if (s.selected[s.selected_idx] != -1)
        {
          s.qmask = (unsigned __int64)1;
          s.qmask = s.qmask << (unsigned char)s.selected[s.selected_idx];
          global_filters.artists_list |= (unsigned int)s.qmask;
        }

      EndDialog(hdlg, 1);
      return 1;
    case RES_BUTTON_CANCEL:
      EndDialog(hdlg, 0);
      return 1;
    case RES_FILTERLIST_SELECTALL:
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);
      for (s.selected_idx = 0x32; -1 < s.selected_idx; s.selected_idx = s.selected_idx + -1)
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 1, s.selected_idx);
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);
      return 1;
    case RES_FILTERLIST_CLEARALL:
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);
      for (s.selected_idx = 0; s.selected_idx < 0x33; s.selected_idx = s.selected_idx + 1)
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 0, s.selected_idx);
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);
      return 1;
    default:
      return 1;
    }

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc);
    s.ctl_hwnd = (unsigned int)lparam;
    SetBkMode(s.hdc, TRANSPARENT);
    return (INT_PTR)(s.hollow_brush = GetStockObject(HOLLOW_BRUSH));

  case WM_ERASEBKGND:
    s.hdc2 = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc2);
    GetClientRect(hdlg, &s.r);

    if (global_filter_subtype_background_pic)
      DrawBitmapToRect(s.hdc2, &s.r, global_filter_subtype_background_pic);
    else
      FillRect(s.hdc2, &s.r, (HBRUSH)GetStockObject(GRAY_BRUSH));
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
    HDC hdc2; // ebp - 0x4bc
    RECT r;
    unsigned int ctl_hwnd;
    HDC hdc;           // ebp - 0x4a4
    HGDIOBJ hollow_brush; // ebp - 0x4a0
    int word_idx;
    int selected_idx;
    int selected[0xe0];
    char path[264];
    int list_word_idx;
    int item_idx;          // ebp - 0x8
    unsigned int creature_name_count; // ebp - 0x4
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    SetWindowTextA(hdlg, global_filter_dlg_title.title);

    load_text("menus", "LONGLIST");
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_ENABLEFILTER), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_SELECTALL), text_lines[1]);
    SetWindowTextA(GetDlgItem(hdlg, RES_FILTERLIST_CLEARALL), text_lines[2]);

    load_text("menus", "OKCANCEL");
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_OK), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_CANCEL), text_lines[1]);

    s.creature_name_count = (unsigned int)load_text("menus", "CREATURENAMES");
    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);

    if (global_filters.cardtypes & FT_CREATURE_LIST)
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_ENABLEFILTER, BM_SETCHECK, 1, 0);

    if (s.creature_name_count != 0xffffffff)
      for (s.item_idx = 0; s.item_idx < (int)s.creature_name_count; s.item_idx = s.item_idx + 1)
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_INSERTSTRING, 0xffffffff, (LPARAM)(text_lines + s.item_idx));

    for (s.list_word_idx = 0; s.list_word_idx < 7; s.list_word_idx = s.list_word_idx + 1)
      for (s.item_idx = 0; s.item_idx < 0x20; s.item_idx = s.item_idx + 1)
      {
        if (global_filters.creature_list[s.list_word_idx] & (1U << (unsigned char)s.item_idx))
          SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 1, s.list_word_idx * 0x20 + s.item_idx);
        else
          SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 0, s.list_word_idx * 0x20 + s.item_idx);
      }

    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETCARETINDEX, 0, 0);
    SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);

    sprintf(s.path, "%s\\GAUN_Options.pic", global_duelart_path);
    global_filter_creature_background_pic = (HANDLE)load_pic(s.path);
    return 0;

  case WM_COMMAND:
    switch (LOWORD(wparam))
    {
    case RES_BUTTON_OK:
      for (s.selected_idx = 0; s.selected_idx < 0xe0; s.selected_idx = s.selected_idx + 1)
        s.selected[s.selected_idx] = -1;

      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_GETSELITEMS, 0xdd, (LPARAM)s.selected);
      for (s.selected_idx = 0; s.selected_idx < 7; s.selected_idx = s.selected_idx + 1)
        global_filters.creature_list[s.selected_idx] = 0;

      for (s.word_idx = 0; s.word_idx < 7; s.word_idx = s.word_idx + 1)
        for (s.selected_idx = 0; s.selected_idx < 0xdd; s.selected_idx = s.selected_idx + 1)
          if (((s.selected[s.selected_idx] != -1) && (s.word_idx * 0x20 <= s.selected[s.selected_idx])) &&
              (s.selected[s.selected_idx] < (s.word_idx + 1) * 0x20))
          {
            global_filters.creature_list[s.word_idx] |=
                1U << (s.selected[s.selected_idx] % 32);
          }

      if (global_filter_creature_background_pic)
        delete_and_close_object(global_filter_creature_background_pic);
      EndDialog(hdlg, 1);
      return 1;

    case RES_BUTTON_CANCEL:
      if (global_filter_creature_background_pic)
        delete_and_close_object(global_filter_creature_background_pic);
      EndDialog(hdlg, 0);
      return 1;

    case RES_FILTERLIST_SELECTALL:
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);
      for (s.item_idx = 0xdc; -1 < s.item_idx; s.item_idx = s.item_idx + -1)
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 1, s.item_idx);
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);
      return 1;

    case RES_FILTERLIST_CLEARALL:
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 0, 0);
      for (s.item_idx = 0; s.item_idx < 0xdd; s.item_idx = s.item_idx + 1)
        SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, 0, s.item_idx);
      SendDlgItemMessageA(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, 1, 0);
      return 1;

    case RES_FILTERLIST_ENABLEFILTER:
      global_filters.cardtypes ^= FT_CREATURE_LIST;
      return 1;

    default:
      return 1;
    }

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc);
    s.ctl_hwnd = (unsigned int)lparam;
    SetBkMode(s.hdc, TRANSPARENT);
    return (INT_PTR)(s.hollow_brush = GetStockObject(HOLLOW_BRUSH));

  case WM_ERASEBKGND:
    s.hdc2 = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc2);
    GetClientRect(hdlg, &s.r);

    if (global_filter_creature_background_pic)
      DrawBitmapToRect(s.hdc2, &s.r, global_filter_creature_background_pic);
    else
      FillRect(s.hdc2, &s.r, (HBRUSH)GetStockObject(GRAY_BRUSH));
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
    strcpy(global_filter_dlg_title.title, text_lines[0]);
  else if (mode == 1)
    strcpy(global_filter_dlg_title.title, text_lines[1]);
  else
    strcpy(global_filter_dlg_title.title, text_lines[2]);

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

// FUNCTION: DECKDLL 0x1000f719
static bool check_card_global_deck_availability(csvid_t csvid)
{
  int i;
  for (i = 0; i < global_deck_num_entries; ++i)
    if (global_deck[i].GDE_csvid == csvid && global_deck[i].GDE_Available == 1)
      return true;

  return false;
}

// FUNCTION: DECKDLL 0x10010c57
static bool
check_set_availability(csvid_t csvid, unsigned int allowed_sets)
{
  struct
  {
    int b0;
    int b1;
    int b2;
    int b3;
    int b4;
    int b5;
    int b6;
    int b7;
    int b8;
  } bits;

  bits.b5 = 0;
  bits.b8 = bits.b5;
  bits.b6 = bits.b8;
  bits.b2 = bits.b6;
  bits.b0 = bits.b2;
  bits.b7 = bits.b0;
  bits.b1 = bits.b7;
  bits.b4 = bits.b1;
  bits.b3 = bits.b4;

  if (IsCardAvailable(csvid, 0) == 0 && IsCardAvailable(csvid, 1) == 0 && IsCardAvailable(csvid, 2) == 0)
    return false;

  if (global_origrarities[csvid].set == -1)
    return false;

  if (allowed_sets & 0x1)
    bits.b3 = (global_origrarities[csvid].exp_rarities[1] != '-') ? 1 : 0;

  if (allowed_sets & 0x4)
    bits.b4 = (global_origrarities[csvid].exp_rarities[0] != '-') ? 1 : 0;

  if (allowed_sets & 0x2)
    bits.b1 = (global_origrarities[csvid].set == SET_UNLIMITED) ? 1 : 0;

  if (allowed_sets & 0x8)
    bits.b7 = (global_origrarities[csvid].set == SET_ARABIAN_NIGHTS) ? 1 : 0;

  if (allowed_sets & 0x10)
    bits.b0 = (global_origrarities[csvid].set == SET_ANTIQUITIES) ? 1 : 0;

  if (allowed_sets & 0x20)
  {
    bits.b2 = (global_origrarities[csvid].set == SET_ASTRAL);
    bits.b6 = (global_origrarities[csvid].set == SET_PROMO) ? 1 : 0;
  }

  if (allowed_sets & 0x40)
    bits.b8 = (global_origrarities[csvid].set == SET_LEGENDS) ? 1 : 0;

  if (allowed_sets & 0x80)
    bits.b5 = (global_origrarities[csvid].set == SET_THE_DARK) ? 1 : 0;

  return bits.b5 | bits.b8 | bits.b6 | bits.b2 | bits.b0 | bits.b7 | bits.b1 | bits.b3 | bits.b4;
}

// FUNCTION: DECKDLL 0x1000fa34
static bool
check_lands(int cardtype, int db_card_type_2)
{
  struct
  {
    int rval;
  } s;

  s.rval = 0;
  if ((global_filters.cardtypes & FT_LAND) == 0)
    return s.rval;

  if ((global_filters.cardtypes & FT_LAND_LAND_AND_MANA) != 0 && cardtype == CP_TYPE_LAND && db_card_type_2 == 10)
    s.rval = 1;
  if ((global_filters.cardtypes & FT_LAND_LAND_ONLY) != 0 && cardtype == CP_TYPE_LAND && db_card_type_2 != 10)
    s.rval = 1;
  if ((global_filters.cardtypes & FT_LAND_MANA_ONLY) != 0 && cardtype != CP_TYPE_LAND && db_card_type_2 == 10)
    s.rval = 1;

  return s.rval;
}

// FUNCTION: DECKDLL 0x1000fade
static bool
check_artifacts(int cardtype, int subtype1)
{
  int result = 0;
  if (cardtype != CP_TYPE_ARTIFACT || (global_filters.cardtypes & FT_ARTIFACT) == 0)
  {
    return result;
  }

  if ((global_filters.cardtypes & FT_ARTIFACT_CREATURE) != 0 &&
      subtype1 == HARDCODED_SUBTYPE_ARTIFACT_CREATURE_OR_AURA_MOSTLY_WITH_ENCHANT_CREATURE)
    result = 1;

  if ((global_filters.cardtypes & FT_ARTIFACT_NON_CREATURE) != 0 &&
      subtype1 != HARDCODED_SUBTYPE_ARTIFACT_CREATURE_OR_AURA_MOSTLY_WITH_ENCHANT_CREATURE)
    result = 1;

  return result;
}

// FUNCTION: DECKDLL 0x10010bcf
static bool
check_creature_list_filter(int subtype1)
{
  struct
  {
    int i;
    int j;
  } s;

  for (s.i = 0; s.i < 7; s.i++)
  {
    for (s.j = 0; s.j < 0x20; s.j++)
    {
      if ((global_filters.creature_list[s.i] & (1 << (byte)s.j)) != 0)
        if (s.i * 0x20 + s.j + 1 == subtype1)
          return true;
    }
  }

  return false;
}

// FUNCTION: DECKDLL 0x1000fb56
static bool
check_creatures(int cardtype, int subtype1)
{

  int result = 0;
  if ((global_filters.cardtypes & FT_CREATURE) == 0)
  {
    return result;
  }

  if ((global_filters.cardtypes & 0x100) != 0 && cardtype == CP_TYPE_CREATURE)
    result = 1;

  if ((global_filters.cardtypes & 0x200) != 0 && cardtype == CP_TYPE_TOKEN)
    result = 1;

  if ((global_filters.cardtypes & 0x400) != 0 &&
      cardtype == CP_TYPE_ARTIFACT &&
      subtype1 == HARDCODED_SUBTYPE_ARTIFACT_CREATURE_OR_AURA_MOSTLY_WITH_ENCHANT_CREATURE)
    result = 1;

  if ((global_filters.cardtypes & 0x800) != 0)
  {
    if (check_creature_list_filter(subtype1) != 0)
      result = 1;
  }

  return result;
}

// FUNCTION: DECKDLL 0x1000fc14
static bool
check_enchantments(int cardtype, int subtype1)
{
  int result = 0;
  if ((global_filters.cardtypes & 0x1000) == 0 || cardtype != CP_TYPE_ENCHANTMENT)
  {
    return result;
  }

  if ((global_filters.cardtypes & 0x2000) != 0 && subtype1 == 0xd3)
    result = 1;
  if ((global_filters.cardtypes & 0x4000) != 0 && subtype1 == 0xcd)
    result = 1;
  if ((global_filters.cardtypes & 0x8000) != 0 && subtype1 == 0x6c)
    result = 1;
  if ((global_filters.cardtypes & 0x10000) != 0 && subtype1 == 0x2c)
    result = 1;
  if ((global_filters.cardtypes & 0x20000) != 0 && subtype1 == 0x0b)
    result = 1;
  if ((global_filters.cardtypes & 0x40000) != 0 && subtype1 == 0x44)
    result = 1;
  if (subtype1 == 0xc5)
    result = 1;

  return result;
}

// FUNCTION: DECKDLL 0x1000fd1e
static bool
check_casting_cost(csvid_t csvid, char *entry)
{
  int cmc;
  int result = 0;

  if (entry == NULL)
  {
    char txt[80];
    sprintf(txt, "Card Number %d does not have a valid cast cost!", csvid);
    MessageBox(global_main_hwnd, txt, "Card Error", MB_ICONSTOP);
    return 0;
  }

  if ((global_filters.casting_cost & FN_ENABLE) == 0)
  {
    return 1;
  }

  if (*entry == '(')
  {
    if ((global_filters.casting_cost & FN_CC_X) != 0)
      return 1;
    else
      return 0;
  }

  cmc = (int)entry[2] + (int)entry[5] + (int)entry[7] + (int)entry[8] + (int)entry[1] + (int)entry[0];

  if ((global_filters.casting_cost & FN_GT) && global_filters.casting_cost_value <= cmc)
    result = 1;
  if ((global_filters.casting_cost & FN_LT) && cmc <= global_filters.casting_cost_value)
    result = 1;
  if ((global_filters.casting_cost & FN_EQ) && global_filters.casting_cost_value == cmc)
    result = 1;

  return result;
}

// FUNCTION: DECKDLL 0x1000fe6e
static bool
check_power(int cp_power)
{
  int rval = 0;

  if ((global_filters.power & FN_ENABLE) == 0)
    return 1;

  if ((global_filters.power & FN_GT) && global_filters.power_value <= cp_power)
    rval = 1;
  if ((global_filters.power & FN_LT) && cp_power <= global_filters.power_value)
    rval = 1;
  if ((global_filters.power & FN_EQ) && global_filters.power_value == cp_power)
    rval = 1;

  return rval;
}

// FUNCTION: DECKDLL 0x1000ff16
static bool
check_toughness(int cp_toughness)
{
  int rval = 0;

  if ((global_filters.toughness & FN_ENABLE) == 0)
    return 1;

  if ((global_filters.toughness & FN_GT) && global_filters.toughness_value <= cp_toughness)
    rval = 1;
  if ((global_filters.toughness & FN_LT) && cp_toughness <= global_filters.toughness_value)
    rval = 1;
  if ((global_filters.toughness & FN_EQ) && global_filters.toughness_value == cp_toughness)
    rval = 1;

  return rval;
}

// FUNCTION: DECKDLL 0x1000ffbe
static bool
check_rarity(csvid_t csvid, int cp_rarity)
{
  int result = 0;
  if (cp_rarity >= 5)
  {
    char txt[80];
    sprintf(txt, "Card Number %d does not have a valid rarity value!", csvid);
    MessageBox(global_main_hwnd, txt, "Card Error", MB_ICONERROR);
    return 0;
  }

  if ((global_filters.rarity & FR_ENABLE) == 0)
    return 1;

  if ((global_filters.rarity & FR_COMMON) && cp_rarity <= 1)
    result = 1;
  if ((global_filters.rarity & FR_UNCOMMON) && cp_rarity == 4)
    result = 1;
  if ((global_filters.rarity & FR_RARE) && cp_rarity == 2)
    result = 1;
  if ((global_filters.rarity & FR_RESTRICTED) && check_restricted(csvid))
    result = 1;
  if ((global_filters.rarity & FR_BANNED) && check_banned(csvid))
    result = 1;

  return result;
}

// FUNCTION: DECKDLL 0x10010260
static bool
check_abilities(csvid_t csvid, int num_abils, char *abils)
{
  struct
  {
    char txt[80];
    int native; // ebp - 0xc
    int grants; // ebp - 0x8
    int i;      // ebp - 0x4
  } s;

  s.native = 0;
  s.grants = 0;

  if (!abils)
  {
    sprintf(s.txt, "Card Number %d does not have a valid abilities value!", csvid);
    MessageBox(global_main_hwnd, s.txt, "Card Error", MB_ICONERROR);
    return false;
  }

  if (!(global_filters.abilities & FA_ENABLE))
    return true;

  if (global_filters.abilities & FA_NATIVE)
    s.native = 1;
  if (global_filters.abilities & FA_GRANTS)
    s.grants = 1;

  if (!s.native && !s.grants)
    return false;

  for (s.i = 0; s.i < num_abils; ++s.i)
  {
    if ((global_filters.abilities & FA_FLYING) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_FLYING && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_FLYING && s.grants)
        return true;
    }

    if ((global_filters.abilities & FA_FIRSTSTRIKE) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_FIRSTSTRIKE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_FIRSTSTRIKE && s.grants)
        return true;
    }

    if ((global_filters.abilities & FA_TRAMPLE) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_TRAMPLE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_TRAMPLE && s.grants)
        return true;
    }

    if ((global_filters.abilities & FA_REGENERATION) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_REGENERATION && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_REGENERATION && s.grants)
        return true;
    }

    if ((global_filters.abilities & FA_BANDING) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_BANDING && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_BANDING && s.grants)
        return true;
    }

    if ((global_filters.abilities & FA_PROTECTION) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_PROTECTION_FROM_BLACK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_PROTECTION_FROM_RED && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_PROTECTION_FROM_WHITE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_BLACK && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_RED && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_WHITE && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_BLUE && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_GREEN && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PROTECTION_FROM_ARTIFACTS && s.grants)
        return true;
    }

    if ((global_filters.abilities & FA_LANDWALK) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_DESERTWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_FORESTWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_ISLANDWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_LEGENDARY_LANDWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_MOUNTAINWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_PLAINSWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_NATIVE_SWAMPWALK && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_FORESTWALK && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_ISLANDWALK && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_MOUNTAINWALK && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_PLAINSWALK && s.grants)
        return true;
      if (abils[s.i] == ABIL_GRANTS_SWAMPWALK && s.grants)
        return true;
    }

    if ((global_filters.abilities & FA_INFECT) != 0 && abils[s.i] == ABIL_NATIVE_INFECT && s.native)
      return true;

    if ((global_filters.abilities & FA_RAMPAGE) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_RAMPAGE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_RAMPAGE && s.grants)
        return true;
    }

    if ((global_filters.abilities & FA_REACH) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_REACH && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_REACH && s.grants)
        return true;
    }

    if ((global_filters.abilities & FA_DEATHTOUCH) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_DEATHTOUCH && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_DEATHTOUCH && s.grants)
        return true;
    }

    if ((global_filters.abilities & FA_VIGILANCE) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_VIGILANCE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_VIGILANCE && s.grants)
        return true;
    }

    if (((int)global_filters.abilities & FA_HASTE) != 0)
    {
      if (abils[s.i] == ABIL_NATIVE_HASTE && s.native)
        return true;
      if (abils[s.i] == ABIL_GRANTS_HASTE && s.grants)
        return true;
    }
  }

  return false;
}

// GLOBAL: DECKDLL 0x1012e75c
int global_num_artists;

// GLOBAL: DECKDLL 0x101bc600
int check_filters_debug_log_enabled;
// GLOBAL: DECKDLL 0x10142640
FILE *check_filters_debug_log_file;

// FUNCTION: DECKDLL 0x10010b1e
static int
find_artist_name_idx(const char *artist, int num_artists)
{
  struct
  {
    int l; // ebp - 0xc
    int m; // ebp - 0x8
    int r; // ebp - 0x4
  } s;

  s.l = 0;
  s.r = num_artists - 1;
  while (s.r >= s.l)
  {
    s.m = (s.r + s.l) / 2;
    if (strcmp(read_db_artist_names[s.m], artist) < 0)
    {
      s.l = s.m + 1;
    }
    else if (strcmp(read_db_artist_names[s.m], artist) > 0)
    {
      s.r = s.m - 1;
    }
    else
    {
      return s.m;
    }
  }
  return -1;
}

// Don't touch this, code is super weird and this will eventually match!
// FUNCTION: DECKDLL 0x10010a72
static int check_artist_filter(csvid_t csvid, const char *artist)
{
  char txt[80];         /* ebp - 0x5c */
  unsigned __int64 idx; /* ebp - 0xc */
  int result;           /* ebp - 0x4 */

  result = 0;

  if (artist == NULL)
  {
    sprintf(txt, "Card Number %d does not have a valid artist name!", csvid);
    return 0;
  }

  if ((global_filters.artist & 0x1) == 0)
    return 1;

  /* Uses global_filters.expansion_list[0..1] as a 64-bit bitfield, but accessed via its position after global_filters.expansions. */
  if ((*(unsigned __int64 *)&global_filters.artists_list & ((unsigned __int64)1 << (idx = find_artist_name_idx(artist, global_num_artists)))) != 0)
    return 1;

  return result;
}

// FUNCTION: DECKDLL 0x1000f1a5
bool check_filters(csvid_t csvid)
{
  struct
  {
    unsigned int setmask; // ebp - 0x88
    int set_ok;
    unsigned int power;     // ebp - 0x80
    int i;                  // ebp - 0x7c
    int subtypes;           // ebp - 0x78
    bool type_ok;           // ebp - 0x74
    bool color_ok;          // ebp - 0x70
    int abils_ok;           // ebp - 0x6c
    int artist_ok;          // ebp - 0x68
    unsigned int color;     // ebp - 0x64
    unsigned int toughness; // ebp - 0x60
    unsigned int rarity;    // ebp - 0x5c
    char *abils;
    char txt[52];                // ebp - 0x54
    unsigned int rarity_ok;      // ebp - 0x20
    unsigned int db_card_type_2; // ebp - 0x1c
    const char *artist;          // ebp - 0x18
    casting_cost_t *req;         // ebp - 0x14
    unsigned int toughness_ok;   // ebp - 0x10
    unsigned int cardtype;       // ebp - 0xc
    int result;                  // ebp - 0x8
    unsigned int power_ok;       // ebp - 0x4
  } s;

  if (strcmp(global_raw_cards_storage[csvid].full_name, "Blank") == 0)
    return 0;
  if ((global_db_flags_1 & 0x61) != 0 && check_card_global_deck_availability(csvid) == 0)
    return 0;

  s.color = global_raw_cards_storage[csvid].color;
  s.cardtype = global_raw_cards_storage[csvid].card_type;
  s.subtypes = *(int *)&global_raw_cards_storage[csvid].subtype;
  s.db_card_type_2 = global_raw_cards_storage[csvid].db_card_type_2;
  s.req = &global_raw_cards_storage[csvid].req;
  s.power = global_raw_cards_storage[csvid].power;
  s.toughness = global_raw_cards_storage[csvid].toughness;
  s.abils = global_raw_cards_storage[csvid].abilities;
  s.artist = global_raw_cards_storage[csvid].artist;
  s.rarity = global_raw_cards_storage[csvid].rarity;

  if (s.color == CP_COLOR_LESS)
    s.color_ok = true;
  else if (((((s.color == CP_COLOR_WHITE) && ((global_filters.colors & FC_WHITE) != 0)) ||
             ((s.color == CP_COLOR_GREEN) && ((global_filters.colors & FC_GREEN) != 0))) ||
            ((s.color == CP_COLOR_RED) && ((global_filters.colors & FC_RED) != 0))) ||
           (((s.color == CP_COLOR_BLACK) && ((global_filters.colors & FC_BLACK) != 0)) ||
            (((s.color == CP_COLOR_BLUE) && ((global_filters.colors & FC_BLUE) != 0)) ||
             s.color == CP_COLOR_LAND || s.color == CP_COLOR_ARTIFACT)))
    s.color_ok = true;
  else
    s.color_ok = false;

  s.setmask = 0;
  if ((global_filters.cardsets & FS_4TH_EDITION) != 0)
  {
    if ((global_filters.expansions & FE_4TH_EDITION) != 0)
      s.setmask |= 1;
    if ((global_filters.expansions & FE_UNLIMITED) != 0)
      s.setmask |= 2;
    if ((global_filters.expansions & FE_REVISED) != 0)
      s.setmask |= 4;
  }
  if ((global_filters.cardsets & FS_ASTRAL) != 0)
    s.setmask |= 0x20;
  if ((global_filters.cardsets & FS_ARABIAN_NIGHTS) != 0)
    s.setmask |= 8;
  if ((global_filters.cardsets & FS_ANTIQUITIES) != 0)
    s.setmask |= 0x10;
  if ((global_filters.cardsets & FS_LEGENDS) != 0)
    s.setmask |= 0x40;
  if ((global_filters.cardsets & FS_THE_DARK) != 0)
    s.setmask |= 0x80;
  s.set_ok = check_set_availability(csvid, s.setmask) ? 1 : 0;

  if (check_lands(s.cardtype, s.db_card_type_2) ||
      check_artifacts(s.cardtype, s.subtypes) ||
      check_creatures(s.cardtype, s.subtypes) ||
      check_enchantments(s.cardtype, s.subtypes) ||
      (s.cardtype == CP_TYPE_INSTANT && (global_filters.cardtypes & 0x80000) != 0) ||
      (s.cardtype == CP_TYPE_INTERRUPT && (global_filters.cardtypes & 0x100000) != 0) ||
      (s.cardtype == CP_TYPE_SORCERY && (global_filters.cardtypes & 0x200000) != 0))
    s.type_ok = true;
  else
    s.type_ok = false;

  s.i = check_casting_cost(csvid, (char *)s.req) ? 1 : 0;
  s.power_ok = check_power(s.power & 0xfff) ? 1 : 0;
  s.toughness_ok = check_toughness(s.toughness & 0xfff) ? 1 : 0;
  s.rarity_ok = check_rarity(csvid, s.rarity) ? 1 : 0;
  s.abils_ok = check_abilities(csvid, 4, s.abils) ? 1 : 0;
  s.artist_ok = check_artist_filter(csvid, s.artist) ? 1 : 0;

  if (s.color_ok && s.set_ok != 0 && s.type_ok &&
      s.i != 0 && s.power_ok != 0 && s.toughness_ok != 0 && s.rarity_ok != 0 &&
      s.abils_ok != 0 && s.artist_ok != 0)
    s.result = 1;
  else
    s.result = 0;

  if (check_filters_debug_log_enabled != 0 && s.result != 0)
  {
    sprintf(s.txt, "%d\n", csvid);
    fprintf(check_filters_debug_log_file, s.txt);
  }

  return s.result;
}
