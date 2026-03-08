#include <STDDEF.H>
#include "deckdll.h"
#include "mystdbool.h"
#include "cardartlib/src/palette.h"
#include "sidlib/pic.h"

typedef ptrdiff_t INT_PTR;

extern char text_lines[500][128];
extern HANDLE *global_pics;
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

int load_text(const char *file_name, const char *section_name);
void filter_cards_in_lists(HWND hwnd_listbox, HWND hwnd_horzlist);
void play_sound(int a1, int a2, int a3, int a4);
unsigned int FUN_100271e3(unsigned char param_1);

//Forward declarations
static bool show_dialog_filter_gle(int textline);
static bool show_dialog_filter_subtype(void);

static bool global_filter_subtype_dlg_mode = false;
// GLOBAL: DECKDLL 0x10104db0
static char global_filter_dlg_title[64];
// GLOBAL: DECKDLL 0x10104d80
static int global_filter_gle_dlg_value = 0;

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
static bool global_filtermenu_gold_enabled = true; // And this one isn't even used except for setting it true.

// GLOBAL: DECKDLL 0x101a9190
FilterExpansions global_filter_expansions = FE_0;
// GLOBAL: DECKDLL 0x101a9192
FilterColors global_filter_colors = FC_0;
// GLOBAL: DECKDLL 0x101a9194
FilterSets global_filter_cardsets = FS_0;
FilterAbilities global_filter_abilities = FA_0;
// GLOBAL: DECKDLL 0x101a9198
FilterTypes global_filter_cardtypes = FT_0;
// GLOBAL: DECKDLL 0x10113d04
FilterCardSetsFlags global_filter_cardsets_flags = FCSF_0;
// GLOBAL: DECKDLL 0x101a91b8
FilterNum global_filter_casting_cost = FN_0;
// GLOBAL: DECKDLL 0x101a91ba
int global_filter_casting_cost_value = 0;

// GLOBAL: DECKDLL 0x101a91bc
FilterNum global_filter_power = FN_0;
// GLOBAL: DECKDLL 0x101a91be
int global_filter_power_value = 0;
// GLOBAL: DECKDLL 0x101a91c0
FilterNum global_filter_toughness = FN_0;
// GLOBAL: DECKDLL 0x101a91c2
int global_filter_toughness_value = 0;

// GLOBAL: DECKDLL 0x101a91c6
FilterRarities global_filter_rarity = FR_0;



#define CREATURE_LIST_SIZE 10
// 1 bit per creature type, so 10 means a maximum of 320.  The highest used is currently 0xEA, for SUBTYPE_MOLE.
STATIC_ASSERT((CREATURE_LIST_SIZE * 32) > SUBTYPE_MAX_USED_CREATURE_SUBTYPE, Too_Many_Creature_Types);

// GLOBAL: DECKDLL 0x101a919c
uint32_t global_filter_creature_list[CREATURE_LIST_SIZE] = {0};

#define EXPANSION_LIST_SIZE 8
// 1 bit per expansion, so 8 means a maximum of 256.  We currently have 159, including 8 "Format" expansions at the start and 8 "Future Expansion" at the end.
// GLOBAL: DECKDLL 0x101a91c8
uint32_t global_filter_expansion_list[EXPANSION_LIST_SIZE] = {0};

// GLOBAL: DECKDLL 0x10104d9c
static HANDLE global_filter_subtype_background_pic;

// And for both, they're limited by constants in dlgproc_FilterSubtype() to 320 entries.
#define MAX_FILTER_SUBTYPE_SIZE 320
#if ((CREATURE_LIST_SIZE * 32) > MAX_FILTER_SUBTYPE_SIZE)
#error "sizeof global_filter_creature_list > MAX_FILTER_SUBTYPE_SIZE"
#endif
#if ((EXPANSION_LIST_SIZE * 32) > MAX_FILTER_SUBTYPE_SIZE)
#error "sizeof global_filter_expansion_list > MAX_FILTER_SUBTYPE_SIZE"
#endif

// FUNCTION: DECKDLL 0x1001d67f
static int
filterbuttons_setcoords(const RECT *r, int button_number, RECT *rval)
{
  RECT temp_rect;
  int w;

  if (((global_db_flags_1 & DBFLAGS_SHANDALAR) && button_number < 10) ||
      (!FUN_100271e3(2) && (button_number == 7 || button_number == 8)) ||
      (!FUN_100271e3(4) && (button_number == 9 || button_number == 10)))
  {
    SetRect(rval, -100, -100, -100, -100);
    return 1;
  }

  w = r->right - r->left - 2;
  SetRect(&temp_rect, r->left + 1, r->top + 1, r->left + w + 1, r->bottom - 2);
  OffsetRect(&temp_rect, w * (button_number - 5), 0);

  if (button_number > 10)
  {
    temp_rect.left += 6;
    temp_rect.right += 6;
  }

  if (button_number >= 18)
  {
    temp_rect.left += 6;
    temp_rect.right += 6;
  }

  if (button_number > 22)
  {
    temp_rect.left += 8;
    temp_rect.right += 8;
  }

  CopyRect(rval, &temp_rect);
  return 1;
}

static __inline bool
point_in_filterbutton(int button_number, RECT *r, POINT p)
{
  RECT r2;
  filterbuttons_setcoords(r, button_number, &r2);
  return PtInRect(&r2, p);
}

// FUNCTION: DECKDLL 0x1001eaa5
static int
get_filter_button_state(HWND hwnd, POINT p)
{
#define CHECK_BUTTON(button_number, txthdr, var, bit) \
  if (point_in_filterbutton(button_number, &r, p))    \
  {                                                   \
    if (load_text("CueCards", txthdr) == -1)          \
      return 0;                                       \
    return (var & bit) ? 1 : 2;                       \
  }

#define CHECK_BUTTON_CONDITIONAL(button_number, txthdr, var, bit, additional_condition) \
  if (additional_condition point_in_filterbutton(button_number, &r, p))                 \
  {                                                                                     \
    if (load_text("CueCards", txthdr) == -1)                                            \
      return 0;                                                                         \
    return (var & bit) ? 1 : 2;                                                         \
  }

  RECT r;
  GetClientRect(hwnd, &r);

  CHECK_BUTTON(11, "WHITE", global_filter_colors, FC_WHITE)
  else CHECK_BUTTON(15, "GREEN", global_filter_colors, FC_GREEN) else CHECK_BUTTON(14, "RED", global_filter_colors, FC_RED) else CHECK_BUTTON(13, "BLACK", global_filter_colors, FC_BLACK) else CHECK_BUTTON(16, "GOLD", global_filter_colors, FC_GOLD) else CHECK_BUTTON(12, "BLUE", global_filter_colors, FC_BLUE) else CHECK_BUTTON_CONDITIONAL(10, "DARK", global_filter_cardsets, FS_THE_DARK, (global_filter_cardsets_flags & FCSF_THE_DARK) &&) else CHECK_BUTTON_CONDITIONAL(7, "ARABIAN", global_filter_cardsets, FS_ARABIAN_NIGHTS, (global_filter_cardsets_flags & FCSF_ARABIAN_NIGHTS) &&) else CHECK_BUTTON_CONDITIONAL(8, "ANTIQUITIES", global_filter_cardsets, FS_ANTIQUITIES, (global_filter_cardsets_flags & FCSF_ANTIQUITIES) &&) else CHECK_BUTTON(6, "ASTRAL", global_filter_cardsets, FS_ASTRAL) else CHECK_BUTTON_CONDITIONAL(9, "LEGENDS", global_filter_cardsets, FS_LEGENDS, (global_filter_cardsets_flags & FCSF_LEGENDS) &&) else CHECK_BUTTON(5, "FOURTH", global_filter_cardsets, FS_4TH_EDITION) else CHECK_BUTTON(4, "EIGHT", global_filter_cardsets, FS_OTHER) else CHECK_BUTTON(17, "LAND", global_filter_cardtypes, FT_LAND) else CHECK_BUTTON(18, "ARTIFACT", global_filter_cardtypes, FT_ARTIFACT) else CHECK_BUTTON(19, "CREATURE", global_filter_cardtypes, FT_CREATURE) else CHECK_BUTTON(20, "ENCHANTMENT", global_filter_cardtypes, FT_ENCHANTMENT) else CHECK_BUTTON(21, "INSTANT", global_filter_cardtypes, FT_INSTANT) else CHECK_BUTTON(22, "INTERRUPT", global_filter_cardtypes, FT_INTERRUPT) else CHECK_BUTTON(23, "SORCERY", global_filter_cardtypes, FT_SORCERY) else CHECK_BUTTON(24, "CASTCOST", global_filter_casting_cost, FN_ENABLE) else CHECK_BUTTON(25, "POWER", global_filter_power, FN_ENABLE) else CHECK_BUTTON(26, "TOUGHNESS", global_filter_toughness, FN_ENABLE) else CHECK_BUTTON(27, "ABILITY", global_filter_abilities, FA_ENABLE) else CHECK_BUTTON(28, "RARITY", global_filter_rarity, FR_ENABLE) else return 0;
#undef CHECK_BUTTON
}

// FUNCTION: DECKDLL 0x1001f6f4
static HMENU
select_filter_menu(HWND hwnd, POINT p)
{
#define CHECK_BUTTON(button_number, menu)          \
  if (point_in_filterbutton(button_number, &r, p)) \
    return menu;

  RECT r;
  GetClientRect(hwnd, &r);

  CHECK_BUTTON(4, global_filtermenu_newexp)
  else CHECK_BUTTON(16, global_filtermenu_gold) else CHECK_BUTTON(5, global_filtermenu_fourth) else CHECK_BUTTON(17, global_filtermenu_land) else CHECK_BUTTON(18, global_filtermenu_artifact) else CHECK_BUTTON(19, global_filtermenu_creature) else CHECK_BUTTON(20, global_filtermenu_enchantment) else CHECK_BUTTON(24, global_filtermenu_castcost) else CHECK_BUTTON(25, global_filtermenu_power) else CHECK_BUTTON(26, global_filtermenu_toughness) else CHECK_BUTTON(27, global_filtermenu_ability) else CHECK_BUTTON(28, global_filtermenu_rarity) else return global_filtermenu_default;
#undef CHECK_BUTTON
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
    SetRect(r3, r2->left + 3, r2->top + 3, r2->right, r2->bottom);
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
    SetRect(r3, r2->left + 1, r2->top + 1, r2->right - 2, r2->bottom - 2);
  }
  FillRect(hdc, r3, GetStockObject(LTGRAY_BRUSH));
}

// FUNCTION: DECKDLL 0x1001da6f
static void
draw_filter_button_pic(HDC hdc, const RECT *r, int button_number, int pic_number, bool pushed)
{
  RECT r2, r3;
  HANDLE hbmp;
  BITMAP bmp;

  filterbuttons_setcoords(r, button_number, &r2);
  draw_filter_button_3d(hdc, &r2, &r3, pushed);
  InflateRect(&r3, -1, -1);

  hbmp = global_pics[pic_number];
  SelectObject(global_hdc, hbmp);
  GetObject(hbmp, sizeof(BITMAP), &bmp);
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
  HDC chdc = CreateCompatibleDC(hdc);
  BITMAP bmp;
  ApplyCardArtPaletteToDc(chdc);

  SelectObject(chdc, global_pics[15]);
  GetObject(global_pics[15], sizeof(BITMAP), &bmp);

  StretchBlt(hdc, 0, 0, r->right, r->bottom, chdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
  DELETE_DC(chdc);

  draw_filter_button_pic(hdc, r, 11, 35, global_filter_colors & FC_WHITE);
  draw_filter_button_pic(hdc, r, 14, 31, global_filter_colors & FC_RED);
  draw_filter_button_pic(hdc, r, 12, 11, global_filter_colors & FC_BLUE);
  draw_filter_button_pic(hdc, r, 15, 22, global_filter_colors & FC_GREEN);
  draw_filter_button_pic(hdc, r, 13, 5, global_filter_colors & FC_BLACK);
  draw_filter_button_pic(hdc, r, 16, 21, global_filter_colors & FC_GOLD);

  draw_filter_button_pic(hdc, r, 17, 26, global_filter_cardtypes & FT_LAND);
  draw_filter_button_pic(hdc, r, 18, 3, global_filter_cardtypes & FT_ARTIFACT);
  draw_filter_button_pic(hdc, r, 19, 13, global_filter_cardtypes & FT_CREATURE);
  draw_filter_button_pic(hdc, r, 20, 18, global_filter_cardtypes & FT_ENCHANTMENT);
  draw_filter_button_pic(hdc, r, 21, 24, global_filter_cardtypes & FT_INSTANT);
  draw_filter_button_pic(hdc, r, 22, 25, global_filter_cardtypes & FT_INTERRUPT);
  draw_filter_button_pic(hdc, r, 23, 32, global_filter_cardtypes & FT_SORCERY);

  if (global_filter_cardsets_flags & FCSF_Q_ENABLE)
  {
    draw_filter_button_pic(hdc, r, 5, 20, global_filter_cardsets & FS_4TH_EDITION);
    draw_filter_button_pic(hdc, r, 6, 4, global_filter_cardsets & FS_ASTRAL);
    draw_filter_button_pic(hdc, r, 8, 1, global_filter_cardsets & FS_ANTIQUITIES);
    draw_filter_button_pic(hdc, r, 10, 14, global_filter_cardsets & FS_THE_DARK);
    draw_filter_button_pic(hdc, r, 7, 2, global_filter_cardsets & FS_ARABIAN_NIGHTS);
    draw_filter_button_pic(hdc, r, 9, 27, global_filter_cardsets & FS_LEGENDS);
    draw_filter_button_pic(hdc, r, 4, 28, global_filter_cardsets & FS_OTHER);
  }

  if (global_filtermenu_castcost_enabled)
    draw_filter_button_pic(hdc, r, 24, 12, global_filter_casting_cost & FN_ENABLE);
  if (global_filtermenu_power_enabled)
    draw_filter_button_pic(hdc, r, 25, 29, global_filter_power & FN_ENABLE);
  if (global_filtermenu_toughness_enabled)
    draw_filter_button_pic(hdc, r, 26, 34, global_filter_toughness & FN_ENABLE);
  if (global_filtermenu_ability_enabled)
    draw_filter_button_pic(hdc, r, 27, 0, global_filter_abilities & FA_ENABLE);
  if (global_filtermenu_rarity_enabled)
    draw_filter_button_pic(hdc, r, 28, 30, global_filter_rarity & FR_ENABLE);
}

// FUNCTION: DECKDLL 0x1001e4b6
static bool
toggle_filterbutton(int n)
{
  int rval;

  switch (n)
  {
  case 1:
    global_filter_cardsets ^= 0x800;
    return (global_filter_cardsets & 0x800) ? true : false;
  case 2:
    global_filter_cardsets ^= 0x200;
    return (global_filter_cardsets & 0x200) ? true : false;
  case 3:
    global_filter_cardsets ^= 0x400;
    return (global_filter_cardsets & 0x400) ? true : false;
  case 4:
    global_filter_cardsets ^= FS_OTHER;
    return (global_filter_cardsets & FS_OTHER) ? true : false;
  case 5:
    global_filter_cardsets ^= FS_4TH_EDITION;
    return (global_filter_cardsets & FS_4TH_EDITION) ? true : false;
  case 6:
    global_filter_cardsets ^= FS_ASTRAL;
    return (global_filter_cardsets & FS_ASTRAL) ? true : false;
  case 7:
    if (!(global_filter_cardsets_flags & FCSF_ARABIAN_NIGHTS))
      return false;
    global_filter_cardsets ^= FS_ARABIAN_NIGHTS;
    return (global_filter_cardsets & FS_ARABIAN_NIGHTS) ? true : false;
  case 8:
    if (!(global_filter_cardsets_flags & FCSF_ANTIQUITIES))
      return false;
    global_filter_cardsets ^= FS_ANTIQUITIES;
    return (global_filter_cardsets & FS_ANTIQUITIES) ? true : false;
  case 9:
    if (!(global_filter_cardsets_flags & FCSF_LEGENDS))
      return false;
    global_filter_cardsets ^= FS_LEGENDS;
    return (global_filter_cardsets & FS_LEGENDS) ? true : false;
  case 10:
    if (!(global_filter_cardsets_flags & FCSF_THE_DARK))
      return false;
    global_filter_cardsets ^= FS_THE_DARK;
    return (global_filter_cardsets & FS_THE_DARK) ? true : false;

  case 11:
    global_filter_colors ^= FC_WHITE;
    return (global_filter_colors & FC_WHITE) ? true : false;
  case 12:
    global_filter_colors ^= FC_BLUE;
    return (global_filter_colors & FC_BLUE) ? true : false;
  case 13:
    global_filter_colors ^= FC_BLACK;
    return (global_filter_colors & FC_BLACK) ? true : false;
  case 14:
    global_filter_colors ^= FC_RED;
    return (global_filter_colors & FC_RED) ? true : false;
  case 15:
    global_filter_colors ^= FC_GREEN;
    return (global_filter_colors & FC_GREEN) ? true : false;
  case 16:
    global_filter_cardtypes ^= FT_LAND;
    return (global_filter_cardtypes & FT_LAND) ? true : false;

  case 17:
    global_filter_cardtypes ^= FT_ARTIFACT;
    return (global_filter_cardtypes & FT_ARTIFACT) ? true : false;
  case 18:
    global_filter_cardtypes ^= FT_CREATURE;
    return (global_filter_cardtypes & FT_CREATURE) ? true : false;
  case 19:
    global_filter_cardtypes ^= FT_ENCHANTMENT;
    return (global_filter_cardtypes & FT_ENCHANTMENT) ? true : false;
  case 20:
    global_filter_cardtypes ^= FT_ENCHANTMENT_PERMANENT;
    return (global_filter_cardtypes & FT_ENCHANTMENT_PERMANENT) ? true : false;
  case 21:
    global_filter_cardtypes ^= FT_ENCHANTMENT_PLAYER;
    return (global_filter_cardtypes & FT_ENCHANTMENT_PLAYER) ? true : false;
  case 22:
    global_filter_cardtypes ^= FT_ENCHANTMENT_INSTANT;
    return (global_filter_cardtypes & FT_ENCHANTMENT_INSTANT) ? true : false;
  case 23:
    global_filter_casting_cost ^= FN_ENABLE;
    return (global_filter_casting_cost & FN_ENABLE) ? false : true;

  case 24:
    global_filter_power ^= FN_ENABLE;
    return (global_filter_power & FN_ENABLE) ? false : true;
  case 25:
    global_filter_toughness ^= FN_ENABLE;
    return (global_filter_toughness & FN_ENABLE) ? false : true;
  case 26:
    global_filter_abilities ^= FA_ENABLE;
    return (global_filter_abilities & FA_ENABLE) ? false : true;
  case 27:
    global_filter_rarity ^= FR_ENABLE;
    return (global_filter_rarity & FR_ENABLE) ? false : true;
  case 28:
    rval = ((unsigned char *)&global_filter_rarity)[1];
    rval ^= 1;
    ((unsigned char *)&global_filter_rarity)[1] = rval;
    return (rval & 1) ? false : true;

  default:
    return false;
  }
}

// FUNCTION: DECKDLL 0x1001f97e
static void
create_filter_menus(void)
{
#define FILTER_MENU_NO_CONDITION
#define FILTER_MENU_NO_CODA

  int i;
  int n;

#define CREATE_FILTER_MENU(menu, name, base, condition, coda)      \
  if ((n = load_text("Menus", (name))) != -1)                      \
  {                                                                \
    (menu) = CreatePopupMenu();                                    \
    for (i = 0; i < n; ++i)                                    \
      condition                                                    \
      {                                                            \
        AppendMenu((menu), MF_ENABLED, i + (base), text_lines[i]); \
      }                                                            \
    coda;                                                          \
  }

  CREATE_FILTER_MENU(global_filtermenu_default, "FILTERS", RES_FILTERMENU_MAINMENUBUTTONS_ON, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);
  CREATE_FILTER_MENU(global_filtermenu_fourth, "FOURTH", RES_FILTERMENU_FOURTH_UNLIMITED, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);
  CREATE_FILTER_MENU(global_filtermenu_gold, "GOLD", RES_FILTERMENU_GOLD_ALL, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);
  CREATE_FILTER_MENU(global_filtermenu_land, "LAND", RES_FILTERMENU_LAND_LANDANDMANA, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);
  CREATE_FILTER_MENU(global_filtermenu_artifact, "ARTIFACT", RES_FILTERMENU_ARTIFACT_CREATURES, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);

  CREATE_FILTER_MENU(global_filtermenu_creature, "CREATURE", RES_FILTERMENU_CREATURE_CREATURE,
                     if ((global_db_flags_1 & DBFLAGS_STANDALONE) || i != 1),
                     InsertMenu(global_filtermenu_creature, (global_db_flags_1 & DBFLAGS_STANDALONE) ? n - 1 : n - 2, MF_BYPOSITION | MF_SEPARATOR, 0, NULL));

  CREATE_FILTER_MENU(global_filtermenu_enchantment, "ENCHANTMENT", RES_FILTERMENU_ENCHANTMENT_ENCHANTMENTS, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);

  if (global_filtermenu_castcost_enabled)
    CREATE_FILTER_MENU(global_filtermenu_castcost, "CASTCOST", RES_FILTERMENU_COST_GREATER, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);

  if (global_filtermenu_power_enabled)
    CREATE_FILTER_MENU(global_filtermenu_power, "POWER", RES_FILTERMENU_POWER_GREATER, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);

  if (global_filtermenu_toughness_enabled)
    CREATE_FILTER_MENU(global_filtermenu_toughness, "TOUGHNESS", RES_FILTERMENU_TOUGHNESS_GREATER, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);

  if (global_filtermenu_ability_enabled)
    CREATE_FILTER_MENU(global_filtermenu_ability, "ABILITY", RES_FILTERMENU_ABILITY_NATIVE,
                       FILTER_MENU_NO_CONDITION,
                       InsertMenu(global_filtermenu_ability, 2, MF_BYPOSITION | MF_SEPARATOR, 0, NULL));

  if (global_filtermenu_rarity_enabled)
    CREATE_FILTER_MENU(global_filtermenu_rarity, "RARITY", RES_FILTERMENU_RARITY_COMMON, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);

  if (global_filtermenu_artist_enabled)
    CREATE_FILTER_MENU(global_filtermenu_artist, "ARTIST", RES_FILTERMENU_RARITY_RARE, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);

  //CREATE_FILTER_MENU(global_filtermenu_newexp, "NEWEXP", RES_FILTERMENU_EXPANSIONLIST, FILTER_MENU_NO_CONDITION, FILTER_MENU_NO_CODA);

#undef CREATE_FILTER_MENU
#undef FILTER_MENU_NO_CONDITION
#undef FILTER_MENU_NO_CODA
}

// FUNCTION: DECKDLL 0x10020007
static void
destroy_filter_menus(void)
{
  DESTROY_MENU(global_filtermenu_default);
  DESTROY_MENU(global_filtermenu_fourth);
  DESTROY_MENU(global_filtermenu_gold);
  DESTROY_MENU(global_filtermenu_land);
  DESTROY_MENU(global_filtermenu_artifact);
  DESTROY_MENU(global_filtermenu_creature);
  DESTROY_MENU(global_filtermenu_enchantment);
  DESTROY_MENU(global_filtermenu_castcost);
  DESTROY_MENU(global_filtermenu_power);
  DESTROY_MENU(global_filtermenu_toughness);
  DESTROY_MENU(global_filtermenu_ability);
  DESTROY_MENU(global_filtermenu_rarity);
  DESTROY_MENU(global_filtermenu_newexp);
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
    global_filter_colors = FC_WHITE | FC_GREEN | FC_RED | FC_BLACK | FC_BLUE | FC_GOLD | FC_GOLD_ALL;

    global_filter_expansions = FE_4TH_EDITION | FE_REVISED | FE_UNLIMITED | 0x0F00;

    global_filter_cardtypes = (FT_LAND | FT_ARTIFACT | FT_CREATURE | FT_ENCHANTMENT | FT_INSTANT | FT_INTERRUPT | FT_SORCERY | FT_ARTIFACT_NON_CREATURE | FT_ARTIFACT_CREATURE | FT_LAND_LAND_AND_MANA | FT_LAND_LAND_ONLY | FT_CREATURE_CREATURE | FT_CREATURE_TOKEN | FT_CREATURE_ARTIFACT | (FT_ENCHANTMENT_ENCHANTMENTS | FT_ENCHANTMENT_WORLD | FT_ENCHANTMENT_LAND | FT_ENCHANTMENT_CREATURE | FT_ENCHANTMENT_ARTIFACT | FT_ENCHANTMENT_ENCHANT | FT_ENCHANTMENT_PERMANENT | FT_ENCHANTMENT_PLAYER | FT_ENCHANTMENT_INSTANT));

    global_filter_casting_cost = FN_GT;
    global_filter_casting_cost_value = 0;

    global_filter_power = FN_GT;
    global_filter_power_value = 0;

    global_filter_toughness = FN_GT;
    global_filter_toughness_value = 0;

    global_filter_abilities = (FA_NATIVE | FA_GRANTS | FA_FLYING | FA_FIRSTSTRIKE | FA_TRAMPLE | FA_REGENERATION | FA_BANDING | FA_PROTECTION | FA_LANDWALK | FA_INFECT | FA_RAMPAGE | FA_REACH | FA_DEATHTOUCH | FA_VIGILANCE | FA_HASTE);

    global_filter_rarity = FR_COMMON | FR_UNCOMMON | FR_RARE | FR_RESTRICTED | FR_BANNED;

    memset(global_filter_creature_list, -1, sizeof global_filter_creature_list);
    memset(global_filter_expansion_list, -1, sizeof global_filter_expansion_list);

    if (global_db_flags_1 & (DBFLAGS_STANDALONE | DBFLAGS_NOCARDCOUNTCHECK | DBFLAGS_GAUNTLET | DBFLAGS_EDITDECK))
      global_filter_cardsets_flags = (FCSF_Q_OTHER | FCSF_LEGENDS | FCSF_THE_DARK | FCSF_ANTIQUITIES | FCSF_ARABIAN_NIGHTS | FCSF_Q_ASTRAL | FCSF_Q_ENABLE);
    else
      global_filter_cardsets_flags = FCSF_0;

    global_filter_cardsets = (FS_OTHER | FS_LEGENDS | FS_THE_DARK | FS_ANTIQUITIES | FS_ARABIAN_NIGHTS | FS_ASTRAL | FS_4TH_EDITION);

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
      CHECKMENU_IF(global_filtermenu_newexp, RES_FILTERMENU_EXPANSIONLIST, global_filter_expansions & FE_EXPANSIONLIST);
    else if (menu == global_filtermenu_fourth)
    {
      CHECKMENU_IF(global_filtermenu_fourth, RES_FILTERMENU_FOURTH_UNLIMITED, global_filter_expansions & FE_UNLIMITED);
      CHECKMENU_IF(global_filtermenu_fourth, RES_FILTERMENU_FOURTH_REVISED, global_filter_expansions & FE_REVISED);
      CHECKMENU_IF(global_filtermenu_fourth, RES_FILTERMENU_FOURTH_FOURTH, global_filter_expansions & FE_4TH_EDITION);
    }
    else if (menu == global_filtermenu_gold)
    {
      CHECKMENU_IF(global_filtermenu_gold, RES_FILTERMENU_GOLD_ALL, global_filter_colors & FC_GOLD_ALL);
      CHECKMENU_IF(global_filtermenu_gold, RES_FILTERMENU_GOLD_MATCHINGALL, global_filter_colors & FC_GOLD_ALLSELECTED);
      CHECKMENU_IF(global_filtermenu_gold, RES_FILTERMENU_GOLD_MATCHINGANY, global_filter_colors & FC_GOLD_ANYSELECTED);
    }
    else if (menu == global_filtermenu_land)
    {
      CHECKMENU_IF(global_filtermenu_land, RES_FILTERMENU_LAND_LANDANDMANA, global_filter_cardtypes & FT_LAND_LAND_AND_MANA);
      CHECKMENU_IF(global_filtermenu_land, RES_FILTERMENU_LAND_LANDONLY, global_filter_cardtypes & FT_LAND_LAND_ONLY);
      CHECKMENU_IF(global_filtermenu_land, RES_FILTERMENU_LAND_MANAONLY, global_filter_cardtypes & FT_LAND_MANA_ONLY);
    }
    else if (menu == global_filtermenu_artifact)
    {
      CHECKMENU_IF(global_filtermenu_artifact, RES_FILTERMENU_ARTIFACT_CREATURES, global_filter_cardtypes & FT_ARTIFACT_CREATURE);
      CHECKMENU_IF(global_filtermenu_artifact, RES_FILTERMENU_ARTIFACT_NONCREATURES, global_filter_cardtypes & FT_ARTIFACT_NON_CREATURE);
    }
    else if (menu == global_filtermenu_creature)
    {
      CHECKMENU_IF(global_filtermenu_creature, RES_FILTERMENU_CREATURE_CREATURE, global_filter_cardtypes & FT_CREATURE_CREATURE);
      CHECKMENU_IF(global_filtermenu_creature, RES_FILTERMENU_CREATURE_TOKEN, global_filter_cardtypes & FT_CREATURE_TOKEN);
      CHECKMENU_IF(global_filtermenu_creature, RES_FILTERMENU_CREATURE_ARTIFACT, global_filter_cardtypes & FT_CREATURE_ARTIFACT);
      CHECKMENU_IF(global_filtermenu_creature, RES_FILTERMENU_CREATURE_LIST, global_filter_cardtypes & FT_CREATURE_LIST);
    }
    else if (menu == global_filtermenu_enchantment)
    {
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_ENCHANTMENTS, global_filter_cardtypes & FT_ENCHANTMENT_ENCHANTMENTS);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_WORLD, global_filter_cardtypes & FT_ENCHANTMENT_WORLD);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_LAND, global_filter_cardtypes & FT_ENCHANTMENT_LAND);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_CREATURE, global_filter_cardtypes & FT_ENCHANTMENT_CREATURE);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_ARTIFACT, global_filter_cardtypes & FT_ENCHANTMENT_ARTIFACT);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_ENCHANT, global_filter_cardtypes & FT_ENCHANTMENT_ENCHANT);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_PERMANENT, global_filter_cardtypes & FT_ENCHANTMENT_PERMANENT);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_PLAYER, global_filter_cardtypes & FT_ENCHANTMENT_PLAYER);
      CHECKMENU_IF(global_filtermenu_enchantment, RES_FILTERMENU_ENCHANTMENT_INSTANT, global_filter_cardtypes & FT_ENCHANTMENT_INSTANT);
    }
    else if (menu == global_filtermenu_castcost)
    {
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_GREATER, global_filter_casting_cost & FN_GT);
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_LESSER, global_filter_casting_cost & FN_LT);
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_EQUAL, global_filter_casting_cost & FN_EQ);
      CHECKMENU_IF(global_filtermenu_castcost, RES_FILTERMENU_COST_X, global_filter_casting_cost & FN_CC_X);
    }
    else if (menu == global_filtermenu_power)
    {
      CHECKMENU_IF(global_filtermenu_power, RES_FILTERMENU_POWER_GREATER, global_filter_power & FN_GT);
      CHECKMENU_IF(global_filtermenu_power, RES_FILTERMENU_POWER_LESSER, global_filter_power & FN_LT);
      CHECKMENU_IF(global_filtermenu_power, RES_FILTERMENU_POWER_EQUAL, global_filter_power & FN_EQ);
    }
    else if (menu == global_filtermenu_toughness)
    {
      CHECKMENU_IF(global_filtermenu_toughness, RES_FILTERMENU_TOUGHNESS_GREATER, global_filter_toughness & FN_GT);
      CHECKMENU_IF(global_filtermenu_toughness, RES_FILTERMENU_TOUGHNESS_LESSER, global_filter_toughness & FN_LT);
      CHECKMENU_IF(global_filtermenu_toughness, RES_FILTERMENU_TOUGHNESS_EQUAL, global_filter_toughness & FN_EQ);
    }
    else if (menu == global_filtermenu_ability)
    {
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_NATIVE, global_filter_abilities & FA_NATIVE);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_GIVES, global_filter_abilities & FA_GRANTS);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_FLYING, global_filter_abilities & FA_FLYING);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_FIRSTSTRIKE, global_filter_abilities & FA_FIRSTSTRIKE);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_TRAMPLE, global_filter_abilities & FA_TRAMPLE);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_REGENERATION, global_filter_abilities & FA_REGENERATION);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_BANDING, global_filter_abilities & FA_BANDING);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_COLORWARD, global_filter_abilities & FA_PROTECTION);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_LANDWALK, global_filter_abilities & FA_LANDWALK);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_POISON, global_filter_abilities & FA_INFECT);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_RAMPAGE, global_filter_abilities & FA_RAMPAGE);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_REACH, global_filter_abilities & FA_REACH);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_STONING, global_filter_abilities & FA_DEATHTOUCH);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_VIGILANCE, global_filter_abilities & FA_VIGILANCE);
      CHECKMENU_IF(global_filtermenu_ability, RES_FILTERMENU_ABILITY_HASTE, global_filter_abilities & FA_HASTE);
    }
    else if (menu == global_filtermenu_rarity)
    {
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_COMMON, global_filter_rarity & FR_COMMON);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_UNCOMMON, global_filter_rarity & FR_UNCOMMON);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_RARE, global_filter_rarity & FR_RARE);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_RESTRICTED, global_filter_rarity & FR_RESTRICTED);
      CHECKMENU_IF(global_filtermenu_rarity, RES_FILTERMENU_RARITY_BANNED, global_filter_rarity & FR_BANNED);
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
  if (!(global_filter_expansions & tgt) || (global_filter_expansions & (other1 | other2))) \
    global_filter_expansions ^= tgt;                                                       \
  if (global_filter_cardsets & FS_4TH_EDITION)                                             \
  refresh_filters = true

    case RES_FILTERMENU_GOLD_ALL:
    case RES_FILTERMENU_GOLD_MATCHINGALL:
    case RES_FILTERMENU_GOLD_MATCHINGANY:
    {
      int old_fc;

      old_fc = global_filter_colors;
      global_filter_colors &= ~(FC_GOLD_ALL | FC_GOLD_ALLSELECTED | FC_GOLD_ANYSELECTED);
      if (cmd == RES_FILTERMENU_GOLD_ALL)
        global_filter_colors |= FC_GOLD_ALL;
      if (cmd == RES_FILTERMENU_GOLD_MATCHINGALL)
        global_filter_colors |= FC_GOLD_ALLSELECTED;
      if (cmd == RES_FILTERMENU_GOLD_MATCHINGANY)
        global_filter_colors |= FC_GOLD_ANYSELECTED;
      if (old_fc != global_filter_colors)
        refresh_filters = true;
      break;
    }

    case TOGGLE_FILTER(RES_FILTERMENU_LAND_LANDANDMANA, global_filter_cardtypes, FT_LAND_LAND_AND_MANA, FT_LAND); break;
        case TOGGLE_FILTER(RES_FILTERMENU_LAND_LANDONLY, global_filter_cardtypes, FT_LAND_LAND_ONLY, FT_LAND); break;
        case TOGGLE_FILTER(RES_FILTERMENU_LAND_MANAONLY, global_filter_cardtypes, FT_LAND_MANA_ONLY, FT_LAND); break;

        case TOGGLE_FILTER(RES_FILTERMENU_ARTIFACT_CREATURES, global_filter_cardtypes, FT_ARTIFACT_CREATURE, FT_ARTIFACT); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ARTIFACT_NONCREATURES, global_filter_cardtypes, FT_ARTIFACT_NON_CREATURE, FT_ARTIFACT); break;

        case TOGGLE_FILTER_ADD(RES_FILTERMENU_CREATURE_CREATURE, global_filter_cardtypes, FT_CREATURE_CREATURE, FT_CREATURE,
                               global_filter_cardtypes &= ~FT_CREATURE_LIST);
        break;
        case TOGGLE_FILTER_ADD(RES_FILTERMENU_CREATURE_TOKEN, global_filter_cardtypes, FT_CREATURE_TOKEN, FT_CREATURE,
                               global_filter_cardtypes &= ~FT_CREATURE_LIST);
        break;
        case TOGGLE_FILTER_ADD(RES_FILTERMENU_CREATURE_ARTIFACT, global_filter_cardtypes, FT_CREATURE_ARTIFACT, FT_CREATURE,
                               global_filter_cardtypes &= ~FT_CREATURE_LIST);
        break;

        case RES_FILTERMENU_CREATURE_LIST:
      global_filter_subtype_dlg_mode = false;
      if (!show_dialog_filter_subtype())
        break;

      if (!(global_filter_cardtypes & FT_CREATURE_LIST))
      {
        global_filter_cardtypes |= stored_creature_filters;
        stored_creature_filters = FT_0;
      }
      else if (!stored_creature_filters)
      {
        stored_creature_filters = global_filter_cardtypes & (FT_CREATURE_CREATURE | FT_CREATURE_TOKEN | FT_CREATURE_ARTIFACT);
        global_filter_cardtypes &= ~(FT_CREATURE_CREATURE | FT_CREATURE_TOKEN | FT_CREATURE_ARTIFACT);
      }

      if (global_filter_cardtypes & FT_CREATURE)
        refresh_filters = true;
      break;

    case TOGGLE_FILTER(RES_FILTERMENU_ENCHANTMENT_ENCHANTMENTS, global_filter_cardtypes, FT_ENCHANTMENT_ENCHANTMENTS, FT_ENCHANTMENT); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ENCHANTMENT_WORLD, global_filter_cardtypes, FT_ENCHANTMENT_WORLD, FT_ENCHANTMENT); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ENCHANTMENT_LAND, global_filter_cardtypes, FT_ENCHANTMENT_LAND, FT_ENCHANTMENT); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ENCHANTMENT_CREATURE, global_filter_cardtypes, FT_ENCHANTMENT_CREATURE, FT_ENCHANTMENT); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ENCHANTMENT_ARTIFACT, global_filter_cardtypes, FT_ENCHANTMENT_ARTIFACT, FT_ENCHANTMENT); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ENCHANTMENT_ENCHANT, global_filter_cardtypes, FT_ENCHANTMENT_ENCHANT, FT_ENCHANTMENT); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ENCHANTMENT_PERMANENT, global_filter_cardtypes, FT_ENCHANTMENT_PERMANENT, FT_ENCHANTMENT); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ENCHANTMENT_PLAYER, global_filter_cardtypes, FT_ENCHANTMENT_PLAYER, FT_ENCHANTMENT); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ENCHANTMENT_INSTANT, global_filter_cardtypes, FT_ENCHANTMENT_INSTANT, FT_ENCHANTMENT); break;

        case GLE_FILTER(RES_FILTERMENU_COST, global_filter_casting_cost, 0); break;

        case RES_FILTERMENU_COST_X:
      global_filter_casting_cost &= FN_ENABLE;
      global_filter_casting_cost |= FN_CC_X;
      if (global_filter_casting_cost & FN_ENABLE)
        refresh_filters = true;
      break;

    case GLE_FILTER(RES_FILTERMENU_POWER, global_filter_power, 1); break;
        case GLE_FILTER(RES_FILTERMENU_TOUGHNESS, global_filter_toughness, 2); break;

        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_NATIVE, global_filter_abilities, FA_NATIVE, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_GIVES, global_filter_abilities, FA_GRANTS, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_FLYING, global_filter_abilities, FA_FLYING, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_FIRSTSTRIKE, global_filter_abilities, FA_FIRSTSTRIKE, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_TRAMPLE, global_filter_abilities, FA_TRAMPLE, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_REGENERATION, global_filter_abilities, FA_REGENERATION, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_BANDING, global_filter_abilities, FA_BANDING, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_COLORWARD, global_filter_abilities, FA_PROTECTION, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_LANDWALK, global_filter_abilities, FA_LANDWALK, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_POISON, global_filter_abilities, FA_INFECT, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_RAMPAGE, global_filter_abilities, FA_RAMPAGE, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_REACH, global_filter_abilities, FA_REACH, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_STONING, global_filter_abilities, FA_DEATHTOUCH, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_VIGILANCE, global_filter_abilities, FA_VIGILANCE, FA_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_ABILITY_HASTE, global_filter_abilities, FA_HASTE, FA_ENABLE); break;

        case TOGGLE_FILTER(RES_FILTERMENU_RARITY_COMMON, global_filter_rarity, FR_COMMON, FR_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_RARITY_UNCOMMON, global_filter_rarity, FR_UNCOMMON, FR_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_RARITY_RARE, global_filter_rarity, FR_RARE, FR_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_RARITY_RESTRICTED, global_filter_rarity, FR_RESTRICTED, FR_ENABLE); break;
        case TOGGLE_FILTER(RES_FILTERMENU_RARITY_BANNED, global_filter_rarity, FR_BANNED, FR_ENABLE); break;

        case RES_FILTERMENU_MAINMENUBUTTONS_ON:
      global_filter_colors |= (FC_WHITE | FC_GREEN | FC_RED | FC_BLACK | FC_BLUE | FC_GOLD);
      global_filter_cardsets |= (FS_4TH_EDITION | FS_ASTRAL | FS_ARABIAN_NIGHTS | FS_ANTIQUITIES | FS_THE_DARK | FS_LEGENDS | FS_OTHER);
      global_filter_cardtypes |= (FT_LAND | FT_ARTIFACT | FT_CREATURE | FT_ENCHANTMENT | FT_INSTANT | FT_INTERRUPT | FT_SORCERY);
      refresh_filters = true;
      break;

    case RES_FILTERMENU_MAINMENUBUTTONS_OFF:
      global_filter_colors &= ~(FC_WHITE | FC_GREEN | FC_RED | FC_BLACK | FC_BLUE | FC_GOLD);
      global_filter_cardsets = FS_0;
      global_filter_cardtypes &= ~(FT_LAND | FT_ARTIFACT | FT_CREATURE | FT_ENCHANTMENT | FT_INSTANT | FT_INTERRUPT | FT_SORCERY);
      refresh_filters = true;
      break;

    case TOGGLE_3SETS_FILTER(RES_FILTERMENU_FOURTH_UNLIMITED, FE_UNLIMITED, FE_REVISED, FE_4TH_EDITION); break;
        case TOGGLE_3SETS_FILTER(RES_FILTERMENU_FOURTH_REVISED, FE_REVISED, FE_UNLIMITED, FE_4TH_EDITION); break;
        case TOGGLE_3SETS_FILTER(RES_FILTERMENU_FOURTH_FOURTH, FE_4TH_EDITION, FE_REVISED, FE_UNLIMITED); break;

        case RES_FILTERMENU_EXPANSIONLIST:
      global_filter_subtype_dlg_mode = true;
      if (show_dialog_filter_subtype())
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
    p.x = GET_X_LPARAM(lparam);
    p.y = GET_Y_LPARAM(lparam);
    return get_filter_button_state(hwnd, p);

  default:
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }
}

#define set_dlg_text(hdlg, resource, txt)  SetWindowText(GetDlgItem(hdlg, resource), txt)

// FUNCTION: DECKDLL 0x10020273
INT_PTR CALLBACK
dlgproc_FilterGLE(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  HDC hdc;

  switch (msg)
  {
  case WM_INITDIALOG:
  {
    char txt[24];

    sprintf(txt, "%d", global_dlg_parameter);
    SetWindowText(GetDlgItem(hdlg, RES_ASKVALUE_EDITTEXT), txt);

    load_text("Menus", "OKCANCEL");
    set_dlg_text(hdlg, RES_BUTTON_OK, text_lines[0]);
    set_dlg_text(hdlg, RES_BUTTON_CANCEL, text_lines[1]);
    SetWindowText(hdlg, global_filter_dlg_title);
    return 0;
  }

  case WM_ERASEBKGND:
  {
    RECT r;

    hdc = wparam;
    ApplyCardArtPaletteToDc(hdc);

    GetClientRect(hdlg, &r);
    DrawBitmapToRect(hdc, &r, global_pics[19]);
    return 1;
  }

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
  {
    hdc = wparam;
    ApplyCardArtPaletteToDc(hdc);
    SetBkMode(hdc, TRANSPARENT);
    return GetStockObject(HOLLOW_BRUSH);
  }

  case WM_COMMAND:
  {
    if (LOWORD(wparam) == RES_BUTTON_CANCEL)
    {
      EndDialog(hdlg, 0);
    }
    else if (LOWORD(wparam) == RES_BUTTON_OK)
    {
      char val[262];

      GetWindowText(GetDlgItem(hdlg, RES_ASKVALUE_EDITTEXT), val, 261);
      val[261] = 0;
      global_filter_gle_dlg_value = atoi(val);
      EndDialog(hdlg, 1);
    }
    return 1;
  }

  default:
    return 0;
  }
}

// FUNCTION: DECKDLL 0x10020157
static bool show_dialog_filter_gle(int textline)
{
  INT_PTR dialog_result;
  int rval;

  load_text("Menus", "POWERTOUGHNESSCC");
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
INT_PTR CALLBACK
dlgproc_FilterSubtype(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  HDC hdc;
  RECT r;
  int i;
  int num;
  int selected[0x34];
  char path[264];
  uint32_t mask_lo;
  uint32_t mask_hi;

  switch (msg)
  {
  case WM_INITDIALOG:
    SetWindowText(hdlg, global_filter_dlg_title);

    load_text("menus", "LONGLIST");
    SetWindowText(GetDlgItem(hdlg, RES_FILTERLIST_ENABLEFILTER), text_lines[0]);
    SetWindowText(GetDlgItem(hdlg, RES_FILTERLIST_SELECTALL), text_lines[1]);
    SetWindowText(GetDlgItem(hdlg, RES_FILTERLIST_CLEARALL), text_lines[2]);

    load_text("menus", "OKCANCEL");
    SetWindowText(GetDlgItem(hdlg, RES_BUTTON_OK), text_lines[0]);
    SetWindowText(GetDlgItem(hdlg, RES_BUTTON_CANCEL), text_lines[1]);

    num = load_text("menus", "ARTISTNAMES");
    SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, FALSE, 0);
    ShowWindow(GetDlgItem(hdlg, RES_FILTERLIST_ENABLEFILTER), SW_HIDE);

    if (num != -1)
      for (i = 0; i < num; ++i)
        SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, LB_ADDSTRING, 0, (LPARAM)text_lines[i]);

    for (i = 0; i < 0x34; ++i)
    {
      mask_lo = (uint32_t)1 << (byte)i;
      mask_hi = (i >= 32) ? ((uint32_t)1 << (byte)(i - 32)) : 0;

      if (!(global_filter_expansion_list[1] & mask_hi) && !(global_filter_expansion_list[0] & mask_lo))
        SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, FALSE, i);
      else
        SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, TRUE, i);
    }

    SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, LB_SETCARETINDEX, 0, 0);
    SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, TRUE, 0);
    sprintf(path, "%s\\GAUN_Options.pic", global_base_directory);
    global_filter_subtype_background_pic = (HANDLE)load_pic(path);
    return 0;

  case WM_ERASEBKGND:
    hdc = wparam;
    ApplyCardArtPaletteToDc(hdc);
    GetClientRect(hdlg, &r);

    if (!global_filter_subtype_background_pic)
      FillRect(hdc, &r, GetStockObject(GRAY_BRUSH));
    else
      DrawBitmapToRect(hdc, &r, global_filter_subtype_background_pic);
    return 1;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    hdc = wparam;
    ApplyCardArtPaletteToDc(hdc);
    (void)lparam;
    SetBkMode(hdc, TRANSPARENT);
    return GetStockObject(HOLLOW_BRUSH);

  case WM_COMMAND:
    if (LOWORD(wparam) == RES_FILTERLIST_SELECTALL)
    {
      SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, FALSE, 0);
      for (i = 0x32; i >= 0; --i)
        SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, TRUE, i);
      SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, TRUE, 0);
      return 1;
    }

    if (LOWORD(wparam) == RES_BUTTON_OK)
    {
      for (i = 0; i < 0x34; ++i)
        selected[i] = -1;

      SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, LB_GETSELITEMS, 0x33, (LPARAM)selected);
      global_filter_expansion_list[0] = 0;
      global_filter_expansion_list[1] = 0;

      for (i = 0; i < 0x34; ++i)
        if (selected[i] != -1)
        {
          mask_lo = (uint32_t)1 << (byte)selected[i];
          mask_hi = (selected[i] >= 32) ? ((uint32_t)1 << (byte)(selected[i] - 32)) : 0;
          global_filter_expansion_list[0] |= mask_lo;
          global_filter_expansion_list[1] |= mask_hi;
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
      SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, FALSE, 0);
      for (i = 0; i < 0x33; ++i)
        SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, LB_SETSEL, FALSE, i);
      SendDlgItemMessage(hdlg, RES_FILTERLIST_LISTBOX, WM_SETREDRAW, TRUE, 0);
      return 1;
    }

    return 1;

  default:
    return 0;
  }
}

// FUNCTION: DECKDLL 0x10020508
static bool show_dialog_filter_subtype(void)
{
  load_text("Menus", "FILTERDIALOGTITLE");
  strcpy(global_filter_dlg_title, text_lines[global_filter_subtype_dlg_mode ? 0 : 1]);

  switch (DialogBoxParam(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_FILTER_LIST), global_main_hwnd, dlgproc_FilterSubtype, 0))
  {
  case 1:
    return true;

  case -1:
    MessageBox(global_main_hwnd, "Couldn't bring up the filter dialog box", "", MB_OK);
    return false;

  default:
    return false;
  }
}
