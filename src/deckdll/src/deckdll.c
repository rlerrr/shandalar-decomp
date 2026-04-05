// DeckDll: standalone and integrated deckbuilder.
// deckdll.c: primary interface to Magic.exe/Shandalar.exe

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <direct.h>

#include "deckdll.h"
#include "full_deck.h"
#include "mystdbool.h"
#include "sidlib/pic.h"
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "magsnd.h"
#include "drawcardlib/Drawcardlib.h"

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif

#define MAKELONG2(a, b) ((LONG)(((a) & 0xffff) | ((DWORD)((WORD)(b))) << 16))
#define MAKELONG3(a, b) ((LONG)(((a) & 0xffff) | ((DWORD)((b) & 0xffff)) << 16))

typedef ptrdiff_t INT_PTR;

typedef int (*Int_fn_int)(int);
void change_buttonclass_wndproc(HWND hwnd);
extern int global_num_artists;

// GLOBAL: DECKDLL 0x10030030
int HORZLIST_LASTPIC_INDEX = 0;
// GLOBAL: DECKDLL 0x10030034
int HORZLIST_ADDR_INDEX = 2;
// GLOBAL: DECKDLL 0x10030038
int HORZLIST_CURRSEL_INDEX = 6;
// GLOBAL: DECKDLL 0x1003003c
int HORZLIST_LEFTPIC_INDEX = 8;
// GLOBAL: DECKDLL 0x10030040
int HORZLIST_WIDTHPLUSSPACE_INDEX = 10;
// GLOBAL: DECKDLL 0x10030044
int HORZLIST_PICSINLINE_INDEX = 12;

// GLOBAL: DECKDLL 0x10030000
static int DAT_10030000 = 0;
// GLOBAL: DECKDLL 0x10030004
static int DAT_10030004 = 4;

#define CARD_CSVID_INDEX 0
#define CARD_AMOUNT_INDEX 4

#define CUECARD_FONT_INDEX 0

// GLOBAL: DECKDLL 0x101a88f4
HINSTANCE global_hinstance = NULL;

// GLOBAL: DECKDLL 0x101a9710
static CRITICAL_SECTION global_critical_section_for_unknown;

// GLOBAL: DECKDLL 0x10146bb0
static char global_previous_directory[MAX_PATH + 1];
// GLOBAL: DECKDLL 0x101bc0d0
static char global_manalink_ini_path[MAX_PATH + 15];
// GLOBAL: DECKDLL 0x101a9240
static char global_playdeck_path[MAX_PATH + 15];
// GLOBAL: DECKDLL 0x101140c0
static char global_cardart_path[MAX_PATH + 15];
// GLOBAL: DECKDLL 0x10144f50
char global_duelart_path[MAX_PATH + 15];
// GLOBAL: DECKDLL 0x10143d80
static char global_dbart_path[264];
// GLOBAL: DECKDLL 0x101bbf50
static char global_duel_dat_path[MAX_PATH + 15];

// GLOBAL: DECKDLL 0x10143c4c
int global_DIB_debug = 0;

// GLOBAL: DECKDLL 0x10143fbc
DBFlags global_db_flags_1 = DBFLAGS_0;
// GLOBAL: DECKDLL 0x1019f00c
DBFlags global_db_flags_2 = DBFLAGS_0;
// GLOBAL: DECKDLL 0x101bc0c4
bool global_cfg_consolidate = false;

// GLOBAL: DECKDLL 0x101a8a70
static char global_deck_filename[(MAX_PATH + 15) + 32 + 5];

// GLOBAL: DECKDLL 0x101a8c64
HWND global_listbox_hwnd;

// GLOBAL: DECKDLL 0x101a8c70
struct GlobalConfig_t global_cfg;

static bool global_cfg_no_frame = false;
static bool global_cfg_read_by_name = false;
bool global_cfg_view_all = false;

// GLOBAL: DECKDLL 0x1012e898
static int global_current_deck = 0;

// GLOBAL: DECKDLL 0x10144370
static int *global_external_current_deck = NULL;

// GLOBAL: DECKDLL 0x101f1e80
int *deck = NULL;

// GLOBAL: DECKDLL 0x1012ed80
GlobalDeckEntry global_deck[1000];
// GLOBAL: DECKDLL 0x101a0ce0
int global_deck_num_cards = 0;

// GLOBAL: DECKDLL 0x101a9694
int global_deck_num_entries = 0;
// GLOBAL: DECKDLL 0x1012e88c
static int global_deck_num_entries_copy = 0;

// STATIC_ASSERT(sizeof(FullDeck) == 0x1464, full_deck_wrong_size);

// GLOBAL: DECKDLL 0x1019fed0
FullDeck global_edited_deck;

// GLOBAL: DECKDLL 0x101a0ce4
int global_edited_deck_num_entries = 0;
// GLOBAL: DECKDLL 0x10144710
static DeckEntry global_excessive_cards[84];

// GLOBAL: DECKDLL 0x1012ea0c
bool global_deck_was_edited = false;
// GLOBAL: DECKDLL 0x1014436c
static bool global_deckname_set = false;
// GLOBAL: DECKDLL 0x1019f010
static int global_deckbuilder_unknown_1019f010 = 0;

// GLOBAL: DECKDLL 0x1012ea10
static char global_ui_strings_filename[MAX_PATH + 1];

// GLOBAL: DECKDLL 0x101255a0
static char global_artistline[128];

// GLOBAL: DECKDLL 0x1012df44
static int global_unknown_1012df44 = 0;

// GLOBAL: DECKDLL 0x1012df34
static int global_supports_palette = 0;

// GLOBAL: DECKDLL 0x101bbefc
static int global_color_depth = 0;

// GLOBAL: DECKDLL 0x1012dca0
GlobalDeckInfoBlob global_deckinfo;

// GLOBAL: DECKDLL 0x10142814
static char *global_external_deckname = NULL;

// GLOBAL: DECKDLL 0x101bc604
int global_dlg_parameter = 0;
// GLOBAL: DECKDLL 0x101423d0
int global_dlg_result = 0;
// GLOBAL: DECKDLL 0x10143ae0
static char global_ask_x_dlg_title[52];
// GLOBAL: DECKDLL 0x101127e0
static char global_loaddeck_dlg_filename[MAX_PATH + 15 + 10];

static HBRUSH global_brush_gold1 = NULL;
static HBRUSH global_brush_gold2 = NULL;
// GLOBAL: DECKDLL 0x10104c94
static HBRUSH global_brush_mediumgrey = NULL;
// GLOBAL: DECKDLL 0x101a9654
static HBRUSH global_create_brush_0 = NULL;
// GLOBAL: DECKDLL 0x10144c40
static HBRUSH global_create_brush_1 = NULL;
// GLOBAL: DECKDLL 0x1012d68c
static HBRUSH global_create_brush_2 = NULL;
// GLOBAL: DECKDLL 0x10139f14
static HBRUSH global_create_brush_3 = NULL;
// GLOBAL: DECKDLL 0x10142f00
static HBRUSH global_create_brush_4 = NULL;
// GLOBAL: DECKDLL 0x1012e894
HBRUSH global_create_brush_5 = NULL;
// GLOBAL: DECKDLL 0x10132c3c
HBRUSH global_create_brush_6 = NULL;
// GLOBAL: DECKDLL 0x1012e890
static HANDLE global_delete_brush_handle_0 = NULL;
// GLOBAL: DECKDLL 0x10142efc
static HANDLE global_delete_brush_handle_1 = NULL;
// GLOBAL: DECKDLL 0x101bbf00
static HANDLE global_delete_brush_handle_2 = NULL;
// GLOBAL: DECKDLL 0x10139e2c
static HANDLE global_delete_brush_handle_3 = NULL;
// GLOBAL: DECKDLL 0x1014470c
static HANDLE global_delete_brush_handle_4 = NULL;
// GLOBAL: DECKDLL 0x1012e62c
static HANDLE global_delete_brush_handle_5 = NULL;
// GLOBAL: DECKDLL 0x101a9708
static HANDLE global_delete_brush_handle_6 = NULL;

// GLOBAL: DECKDLL 0x10104d74
COLORREF global_colorref_darkgrey = 0;
// GLOBAL: DECKDLL 0x10104c90
COLORREF global_colorref_flesh = 0;
COLORREF global_colorref_lavender = 0;
// GLOBAL: DECKDLL 0x10104d70
COLORREF global_colorref_lightgrey = 0;
// GLOBAL: DECKDLL 0x10104ca0
COLORREF global_colorref_white = 0;

// GLOBAL: DECKDLL 0x10113f6c
HANDLE global_pic_ability = NULL;
// GLOBAL: DECKDLL 0x1014422c
HANDLE global_pic_antiquit = NULL;
// GLOBAL: DECKDLL 0x10143b14
HANDLE global_pic_arabnite = NULL;
// GLOBAL: DECKDLL 0x10125ba4
HANDLE global_pic_artifact = NULL;
// GLOBAL: DECKDLL 0x10146df4
HANDLE global_pic_astral = NULL;
// GLOBAL: DECKDLL 0x1012d7d4
HANDLE global_pic_grey = NULL;
// GLOBAL: DECKDLL 0x1012d7bc
HANDLE global_pic_bldr01c = NULL;
// GLOBAL: DECKDLL 0x1012d7c0
HANDLE global_pic_bldr02c = NULL;
// GLOBAL: DECKDLL 0x1012d7c8
HANDLE global_pic_bldr03c = NULL;
// GLOBAL: DECKDLL 0x1012d7cc
HANDLE global_pic_bldr04c = NULL;
// GLOBAL: DECKDLL 0x1012d7c4
HANDLE global_pic_bldr05c = NULL;
// GLOBAL: DECKDLL 0x1019f74c
HANDLE global_pic_blue = NULL;
// GLOBAL: DECKDLL 0x10144ef0
HANDLE global_pic_castcost = NULL;
// GLOBAL: DECKDLL 0x1012e2d4
HANDLE global_pic_creature = NULL;
// GLOBAL: DECKDLL 0x10141b84
HANDLE global_pic_dark = NULL;
// GLOBAL: DECKDLL 0x1012e89c
HANDLE global_pic_dekbar1 = NULL;
// GLOBAL: DECKDLL 0x101bc43c
HANDLE global_pic_dektile4 = NULL;
// GLOBAL: DECKDLL 0x101bc484
HANDLE global_pic_dektit1 = NULL;
// GLOBAL: DECKDLL 0x10144230
HANDLE global_pic_enchant = NULL;
// GLOBAL: DECKDLL 0x100f2854
HANDLE global_groupmove_pic = NULL;
// GLOBAL: DECKDLL 0x1019e9d4
HANDLE global_pic_fourth = NULL;
// GLOBAL: DECKDLL 0x10132c34
HANDLE global_pic_yellow = NULL;
// GLOBAL: DECKDLL 0x10125668
HANDLE global_pic_green = NULL;
// GLOBAL: DECKDLL 0x100f284c
HANDLE global_deckinfo_pic = NULL;
// GLOBAL: DECKDLL 0x1012dc54
HANDLE global_pic_instant = NULL;
// GLOBAL: DECKDLL 0x1012df38
HANDLE global_pic_interrupt = NULL;
// GLOBAL: DECKDLL 0x101a970c
HANDLE global_pic_land = NULL;
// GLOBAL: DECKDLL 0x1014294c
HANDLE global_pic_legends = NULL;
// GLOBAL: DECKDLL 0x1019eedc
HANDLE global_pic_power = NULL;
// GLOBAL: DECKDLL 0x10113e38
HANDLE global_pic_rarity = NULL;
// GLOBAL: DECKDLL 0x10139cd0
HANDLE global_pic_red = NULL;
// GLOBAL: DECKDLL 0x101140b4
HANDLE global_pic_sorcery = NULL;
// GLOBAL: DECKDLL 0x1019f828
HANDLE global_pic_statbak1 = NULL;
// GLOBAL: DECKDLL 0x10144c3c
HANDLE global_pic_tough = NULL;
// GLOBAL: DECKDLL 0x10146b60
HANDLE global_pic_artist = NULL;
// GLOBAL: DECKDLL 0x1019e9cc
HANDLE global_pic_all_butn = NULL;

// GLOBAL: DECKDLL 0x101a9704
HBITMAP global_hbmp = NULL;

// GLOBAL: DECKDLL 0x101bc5fc
static HGDIOBJ global_old_bmp_obj = NULL;

// GLOBAL: DECKDLL 0x10125e0c
static HFONT DAT_10125e0c = NULL;

// GLOBAL: DECKDLL 0x10113cec
int global_smallcard_normal_width = 0;
// GLOBAL: DECKDLL 0x1012e4f4
int global_smallcard_normal_height = 0;

// GLOBAL: DECKDLL 0x101bc448
int global_smallcard_smaller_width = 0;
// GLOBAL: DECKDLL 0x10113cf0
int global_smallcard_smaller_height = 0;

// GLOBAL: DECKDLL 0x10143fc0
int global_smallcard_smallest_width = 0;
// GLOBAL: DECKDLL 0x1014532c
int global_smallcard_smallest_height = 0;

// GLOBAL: DECKDLL 0x10132c40
int global_smallcard_piclist_height = 0;
// GLOBAL: DECKDLL 0x10143e88
int global_smallcard_piclist_width = 0;

HFONT global_font_42 = NULL;
HFONT global_font_42unused = NULL;
HFONT global_font_40percent = NULL;
HFONT global_font_28percent = NULL;
HFONT global_font_32 = NULL;

// GLOBAL: DECKDLL 0x10031948
static int global_fonts_init_state = 0;

// GLOBAL: DECKDLL 0x10104abc
static HBITMAP global_damage_pic = NULL;
// GLOBAL: DECKDLL 0x101048b0
static HBITMAP global_card_counters_pic = NULL;
// GLOBAL: DECKDLL 0x10104ac4
static HBITMAP global_mana_symbols_pic = NULL;
// GLOBAL: DECKDLL 0x10104aac
static HBITMAP global_abilities_pic = NULL;
// GLOBAL: DECKDLL 0x10104718
static HBITMAP global_mana_stripes_pic = NULL;
// GLOBAL: DECKDLL 0x101048b4
static HBITMAP global_summon_pic = NULL;
// GLOBAL: DECKDLL 0x10104564
static HBITMAP global_dying_pic = NULL;
// GLOBAL: DECKDLL 0x10104714
static HBITMAP global_target_pic = NULL;
// GLOBAL: DECKDLL 0x10104ab8
static HBITMAP global_cant_target_pic = NULL;
// GLOBAL: DECKDLL 0x1010456c
static HBITMAP global_will_untap_pic = NULL;

// GLOBAL: DECKDLL 0x10104710
static HFONT global_smallcard_pt_font = NULL;
// GLOBAL: DECKDLL 0x1010452c
static HFONT global_damage_font = NULL;
// GLOBAL: DECKDLL 0x101048b8
static HFONT global_idtag_font = NULL;

// GLOBAL: DECKDLL 0x1010470c
static COLORREF global_palette_col_c9 = 0;
// GLOBAL: DECKDLL 0x10104ac0
static COLORREF global_palette_col_9e_a = 0;
// GLOBAL: DECKDLL 0x10104700
static COLORREF global_palette_col_9e_b = 0;
// GLOBAL: DECKDLL 0x10104528
static COLORREF global_palette_col_7c = 0;
// GLOBAL: DECKDLL 0x10104704
static COLORREF global_palette_col_2f = 0;
// GLOBAL: DECKDLL 0x10104708
static COLORREF global_palette_col_bf = 0;
// GLOBAL: DECKDLL 0x10104568
static COLORREF global_palette_col_5d = 0;
// GLOBAL: DECKDLL 0x10104ab4
static COLORREF global_palette_col_1f = 0;

// GLOBAL: DECKDLL 0x10104ab0
static HPEN global_pen_palette_5d = NULL;
// GLOBAL: DECKDLL 0x10104afc
static HPEN global_pen_palette_1f = NULL;
// GLOBAL: DECKDLL 0x10104a50
static HPEN global_debug_pens[10] = {0};

static LOGFONT global_font_template_ini = {
    0,                           // lfHeight
    0,                           // lfWidth
    0,                           // lfEscapement
    0,                           // lfOrientation
    FW_NORMAL,                   // lfWeight
    FALSE,                       // lfItalic
    FALSE,                       // lfUnderline
    FALSE,                       // lfStrikeOut
    ANSI_CHARSET,                // lfCharSet
    OUT_DEFAULT_PRECIS,          // lfOutPrecision
    CLIP_DEFAULT_PRECIS,         // lfClipPrecision
    DEFAULT_QUALITY,             // lfQuality
    DEFAULT_PITCH | FF_DONTCARE, // lfPitchAndFamily
    {
        'M', 'S', ' ', 'S', 'a', 'n', 's', ' ', 'S', 'e', 'r', 'i', 'f', '\0',
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
        '\0', '\0', '\0', '\0', '\0', '\0'} // lfFaceName[LF_FACESIZE]
};
static LOGFONT global_font_from_ini;

// GLOBAL: DECKDLL 0x101bbc28
static HWND global_button_stats_hwnd;
// GLOBAL: DECKDLL 0x101436b4
static HWND global_cuecard_hwnd;

extern HWND global_decksurface_hwnd;
extern HPALETTE global_cart_art_hpalette;

// GLOBAL: DECKDLL 0x100f2848
static HANDLE global_gaun_options_pic;

// GLOBAL: DECKDLL 0x100f2850
static HANDLE global_gaun_results_pic;

// GLOBAL: DECKDLL 0x10104b4c
static HANDLE global_loaddeck_pic;

// GLOBAL: DECKDLL 0x10132c38
static HWND global_fullcard_hwnd;
// GLOBAL: DECKDLL 0x10104520
static HMENU global_fullcard_popup;

// GLOBAL: DECKDLL 0x101bc444
HWND global_horzlist_hwnd;
// GLOBAL: DECKDLL 0x101052c0
static POINT DAT_101052c0;
// GLOBAL: DECKDLL 0x101052c8
static HMENU global_horzlist_popup;

// GLOBAL: DECKDLL 0x1012e2d8
HWND cardlistfilter_hwnd;

// GLOBAL: DECKDLL 0x10144ef4
HWND global_main_hwnd;
static int global_main_window_width = 0;
static int global_main_window_height = 0;

// GLOBAL: DECKDLL 0x10144eec
int global_smallcard_height = 0;
// GLOBAL: DECKDLL 0x1013a17c
int global_smallcard_width = 0;

// GLOBAL: DECKDLL 0x10141b4c
static HMENU global_smallcard_popup;

// GLOBAL: DECKDLL 0x10146df0
HWND global_title_hwnd;

// GLOBAL: DECKDLL 0x10125a3c
HWND global_unknown_10125a3c;

// GLOBAL: DECKDLL 0x10144b04
HWND global_unknown_10144b04;

// GLOBAL: DECKDLL 0x1012566c
HDC global_hdc;

// GLOBAL: DECKDLL 0x10105738
static POINT global_horzlist_mouse_pt;

// GLOBAL: DECKDLL 0x10105748
static POINT global_main_mouse_pt;

static char global_search_string[264];

// GLOBAL: DECKDLL 0x101915a0
Packs global_packs[PACK1_MAX + 1][PACK2_MAX + 1];
// GLOBAL: DECKDLL 0x101e74f0
Packs global_packs_copy[PACK1_MAX + 1][PACK2_MAX + 1];

// Different sizes in magic.exe/shandalar.exe vs deckdll.dll for some reason
#ifdef DECKDLL
// GLOBAL: DECKDLL 0x10132c50
char text_lines[225][128];
#else
// GLOBAL: SHANDALAR 0x008aa920
char text_lines[250][300];
#endif

extern int global_available_slots;

extern card_ptr_t global_raw_cards_storage[2000];

// GLOBAL: DECKDLL 0x10145330
OrigRarities global_origrarities[1000];
// GLOBAL: DECKDLL 0x101bc440
int *Gold;
// GLOBAL: DECKDLL 0x10146aa0
shandalar_worldmagic_t *Scards;
// Why are these exported from here?  And with illegal names, no less?
int _PlayerFace;
int _OpponFace;

// GLOBAL: DECKDLL 0x1019f824
static Int_fn_int CardIDFromType;
// GLOBAL: DECKDLL 0x10142818
static Int_fn_int CardTypeFromID;
// GLOBAL: DECKDLL 0x101a8934
static Int_fn_int CardInDeck;
// GLOBAL: DECKDLL 0x10143350
static Int_fn_int SetCardInDeck;
// GLOBAL: DECKDLL 0x10125eb4
static Int_fn_int SellPrice;

bool check_filters(csvid_t csvid);
int InitLicenseSecretsFromRegistry(void);

/* Defined in filtermenus.c (original binary uses these in wndproc_MainClass). */
extern int check_filters_debug_log_enabled;
extern FILE *check_filters_debug_log_file;

LRESULT CALLBACK wndproc_CardClass(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wndproc_CardListFilterClass(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wndproc_CueCardClass(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wndproc_DeckSurfaceClass(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wndproc_FullCardClass(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wndproc_HorzListClass(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wndproc_MainClass(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wndproc_TitleClass(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wndproc_SideboardSurfaceClass(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wndproc_TradeSurfaceClass(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK dlgproc_DeckStats(HWND, UINT, WPARAM, LPARAM);

// FUNCTION: DECKDLL 0x1000d9a3
static int fatal_err(const char *text, HWND hwnd)
{
  MessageBox(hwnd, text, 0, MB_SYSTEMMODAL | MB_ICONERROR);

  if (hwnd)
    SendMessage(hwnd, WM_CLOSE, 0, 0);
  else
    PostQuitMessage(1);
  return 0;
}

void fatal(const char *fmt, ...)
{
  char buf[8000];
  va_list args;
  va_start(args, fmt);
  _vsnprintf(buf, 8000, fmt, args);
  va_end(args);

  fatal_err(buf, 0);
  exit(1);
}

// FUNCTION: DECKDLL 0x10028350
// FUNCTION: SHANDALAR 0x0056ca10
int load_text(const char *file_name, const char *section_name)
{
  struct
  {
    FILE *f;
    int i;
    char line[80];
    char path[128];
    char section_line[80]; // ebp - 0x54
    unsigned int num_text;
  } s;

  strcpy(s.section_line, "@");
  strcat(s.section_line, section_name);
  strcat(s.section_line, "\n");

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\");
  strcat(s.path, file_name);
  if (!strchr(file_name, '.'))
    strcat(s.path, ".txt");

  s.f = fopen(s.path, "rt");
  if (s.f == NULL)
    return -1;

  while (strcmp(s.section_line, s.line))
  {
    if (!fgets(s.line, 80, s.f))
    {
      fclose(s.f);
      return -1;
    }
  }

  fscanf(s.f, "%d", &s.num_text);
  fgets(s.line, 80, s.f);
  if (s.num_text > 225)
  {
    fclose(s.f);
    return -1;
  }

  for (s.i = 0; s.i < (int)s.num_text; s.i++)
  {
    if (!fgets(text_lines[s.i], 80, s.f))
    {
      fclose(s.f);
      return -1;
    }
    text_lines[s.i][strlen(text_lines[s.i]) - 1] = 0;
  }

  fclose(s.f);
  return s.num_text;
}

// GLOBAL: DECKDLL 0x10036210
const char s__08x_DestroyDIBSection__file_map_10036210[] = "  %08x DestroyDIBSection (file mapping: %08x)\n";

// GLOBAL: DECKDLL 0x1003181c
static const char s_PRICE_1003181c[8] = "PRICE";
// GLOBAL: DECKDLL 0x10031824
static const char s_menus_10031824[8] = "menus";
// GLOBAL: DECKDLL 0x1003182c
static const char DAT_1003182c[4] = "%d";
// GLOBAL: DECKDLL 0x10031830
static const char s_SELLCARD_10031830[12] = "SELLCARD";
// GLOBAL: DECKDLL 0x1003183c
static const char s_menus_1003183c[8] = "menus";

// GLOBAL: DECKDLL 0x1019c020
char read_db_artist_names[100][100];

// FUNCTION: DECKDLL 0x1000d9f1
static void
read_manalink_ini(void)
{
  struct
  {
    HKEY key;      /* ebp - 0x6c */
    BYTE buf[100]; /* ebp - 0x68 */
    DWORD buflen;  /* ebp - 0x4 */
  } s;

  if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\MicroProse\\Magic: The Gathering\\DeckBuilderOptions", 0, KEY_QUERY_VALUE, &s.key) == ERROR_SUCCESS)
  {
    s.buflen = 10;
    if (RegQueryValueEx(s.key, "Consolidate", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      sscanf((char *)s.buf, "%d", (int *)&global_cfg.consolidate_from_registry);
    else
      global_cfg.consolidate_from_registry = 1;

    if (RegQueryValueEx(s.key, "Music", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      sscanf((char *)s.buf, "%d", (int *)((char *)&global_cfg.consolidate_from_registry + 1));
    else
      global_cfg.music = 1;

    if (RegQueryValueEx(s.key, "Effects", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      sscanf((char *)s.buf, "%d", (int *)((char *)&global_cfg.consolidate_from_registry + 2));
    else
      global_cfg.effects = 1;

    RegCloseKey(s.key);
  }
  else
  {
    global_cfg.consolidate_from_registry = 1;
    global_cfg.music = 1;
    global_cfg.effects = 1;
  }

  if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\MicroProse\\Magic: The Gathering", 0, KEY_QUERY_VALUE, &s.key) == ERROR_SUCCESS)
  {
    s.buflen = 100;
    if (RegQueryValueEx(s.key, "Name", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      strcpy(global_cfg.player_name, (char *)s.buf);
    else
      strcpy(global_cfg.player_name, "User");

    s.buflen = 100;
    if (RegQueryValueEx(s.key, "Email", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      strcpy(global_cfg.email, (char *)s.buf);
    else
      strcpy(global_cfg.email, "User E-Mail");

    RegCloseKey(s.key);
  }
  else
    strcpy(global_cfg.email, "User E-Mail");

  if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\MicroProse\\Magic: The Gathering\\DuelOptions", 0, KEY_QUERY_VALUE, &s.key) == ERROR_SUCCESS)
  {
    s.buflen = 100;
    if (RegQueryValueEx(s.key, "ExpandTextOnBigCard", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      sscanf((char *)s.buf, "%d", &global_cfg.expand_text);
    else
      global_cfg.expand_text = 0;
    RegCloseKey(s.key);
  }
  else
    global_cfg.expand_text = 0;
}

static void
cfg_write_str(const char *str, const char *keyname)
{
  WritePrivateProfileString("DeckBuilder", keyname, str, global_manalink_ini_path);
}

static void
cfg_write_int(int val, const char *keyname)
{
  char buf[256];
  sprintf(buf, "%d", val);
  cfg_write_str(buf, keyname);
}

// FUNCTION: DECKDLL 0x1000dca4
static void
write_manalink_ini(void)
{
  HKEY key;
  BYTE buf[12];
  DWORD disposition;

  if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\MicroProse\\Magic: The Gathering\\DeckBuilderOptions",
                     0, NULL, 0, 0xF003F, NULL, &key, &disposition) == ERROR_SUCCESS)
  {
    wsprintf((char *)buf, "%d", (int)(signed char)global_cfg.consolidate_from_registry);
    RegSetValueEx(key, "Consolidate", 0, 1, buf, strlen((char *)buf) + 1);

    wsprintf((char *)buf, "%d", (int)(signed char)global_cfg.music);
    RegSetValueEx(key, "Music", 0, 1, buf, strlen((char *)buf) + 1);

    wsprintf((char *)buf, "%d", (int)(signed char)global_cfg.effects);
    RegSetValueEx(key, "Effects", 0, 1, buf, strlen((char *)buf) + 1);

    RegFlushKey(key);
    RegCloseKey(key);
  }

  if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\MicroProse\\Magic: The Gathering\\DuelOptions",
                     0, NULL, 0, 0xF003F, NULL, &key, &disposition) == ERROR_SUCCESS)
  {
    wsprintf((char *)buf, "%d", (int)(signed char)global_cfg.expand_text);
    RegSetValueEx(key, "ExpandTextOnBigCard", 0, 1, buf, strlen((char *)buf) + 1);

    RegFlushKey(key);
    RegCloseKey(key);
  }
}

void delete_and_close_object(HANDLE obj);

// FUNCTION: DECKDLL 0x1000d741
static void
delete_brushes(void)
{
  delete_and_close_object(global_delete_brush_handle_0);
  delete_and_close_object(global_delete_brush_handle_1);
  delete_and_close_object(global_delete_brush_handle_2);
  delete_and_close_object(global_delete_brush_handle_3);
  delete_and_close_object(global_delete_brush_handle_4);
  delete_and_close_object(global_delete_brush_handle_5);
  delete_and_close_object(global_delete_brush_handle_6);
}

// FUNCTION: DECKDLL 0x1000d7ae
static bool
create_brushes(void)
{
  global_create_brush_0 = CreateSolidBrush(0x24a3230);
  global_create_brush_1 = CreateSolidBrush(0x2508080);
  global_create_brush_2 = CreateSolidBrush(0x2508080);
  global_create_brush_3 = CreateSolidBrush(0x2508080);
  global_create_brush_4 = CreateSolidBrush(0x2508080);
  global_create_brush_5 = CreateSolidBrush(0x2508080);
  global_create_brush_6 = CreateSolidBrush(0x2508080);

  if (!global_create_brush_0 || !global_create_brush_1 || !global_create_brush_2 ||
      !global_create_brush_3 || !global_create_brush_4 || !global_create_brush_5 ||
      !global_create_brush_6)
  {
    if (global_create_brush_0)
      DeleteObject(global_create_brush_0);
    if (global_create_brush_1)
      DeleteObject(global_create_brush_1);
    if (global_create_brush_2)
      DeleteObject(global_create_brush_2);
    if (global_create_brush_3)
      DeleteObject(global_create_brush_3);
    if (global_create_brush_4)
      DeleteObject(global_create_brush_4);
    if (global_create_brush_5)
      DeleteObject(global_create_brush_5);
    if (global_create_brush_6)
      DeleteObject(global_create_brush_6);
    return false;
  }

  return true;
}

// FUNCTION: DECKDLL 0x1000d944
static void
delete_fonts(void)
{
  DeleteObject(global_create_brush_0);
  DeleteObject(global_create_brush_1);
  DeleteObject(global_create_brush_2);
  DeleteObject(global_create_brush_3);
  DeleteObject(global_create_brush_4);
  DeleteObject(global_create_brush_5);
  DeleteObject(global_create_brush_6);
}

// FUNCTION: DECKDLL 0x10025727
// FUNCTION: MAGIC 0x00495e95
static LOGFONT *LoadFontFromIni(char *name, int italic)
{
  struct
  {
    char key[100]; /* ebp - 0x68 */
    UINT bold;     /* ebp - 0x4 */
  } s;

  memcpy(&global_font_from_ini, &global_font_template_ini, sizeof(LOGFONT));

  strcpy(s.key, "size");
  strcat(s.key, name);
  global_font_from_ini.lfHeight = GetPrivateProfileIntA("Fonts", s.key, 0x14, global_duel_dat_path);

  strcpy(s.key, "bold");
  strcat(s.key, name);
  s.bold = GetPrivateProfileIntA("Fonts", s.key, 0, global_duel_dat_path);
  if (s.bold != 0)
    global_font_from_ini.lfWeight = FW_BOLD;

  if (italic != 0)
    global_font_from_ini.lfItalic = TRUE;

  strcpy(s.key, "font");
  strcat(s.key, name);
  GetPrivateProfileStringA("Fonts", s.key, "MS Sans Serif",
                           global_font_from_ini.lfFaceName, 0x20, global_duel_dat_path);
  return &global_font_from_ini;
}

// FUNCTION: DECKDLL 0x10025b00
// FUNCTION: MAGIC 0x0049626e
static COLORREF GetPaletteColor(int index)
{
  struct
  {
    int g;
    int r;
    int b;
  } rgb;

  rgb.r = g_cardArtPalette[index].rgbRed;
  rgb.g = g_cardArtPalette[index].rgbGreen;
  rgb.b = g_cardArtPalette[index].rgbBlue;
  return (COLORREF)(0x02000000 | (DWORD)MAKEWORD((BYTE)rgb.r, (BYTE)rgb.g) | ((DWORD)(BYTE)rgb.b << 16));
}

void delete_and_close_object(HANDLE obj);
static void destroy_create_fonts_resources(void);
void draw_item(DRAWITEMSTRUCT *item, HBRUSH brush, HANDLE hbmp_bkgrd, HPEN pen1, HPEN pen2, COLORREF col, int do_focus, UINT format);

// FUNCTION: DECKDLL 0x10011680
// FUNCTION: MAGIC 0x00558fb0
static bool create_fonts(void)
{
  struct
  {
    char path[MAX_PATH + 4]; /* big buffer first */
    int local_c;             /* ebp - 0x8 */
    int local_8;             /* ebp - 0x4 */
  } s;

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0530m_.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Magis___.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0127m_.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0085m_.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0298m_.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0299m_.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0300m_.TTF");
  AddFontResourceA(s.path);

  global_fonts_init_state = 3;

  sprintf(s.path, "%s\\Damage.pic", global_cardart_path);
  global_damage_pic = load_pic(s.path);
  sprintf(s.path, "%s\\CardCounters.pic", global_cardart_path);
  global_card_counters_pic = load_pic(s.path);
  sprintf(s.path, "%s\\ManaSymbols.pic", global_cardart_path);
  global_mana_symbols_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Abilities.pic", global_cardart_path);
  global_abilities_pic = load_pic(s.path);
  sprintf(s.path, "%s\\ManaStripes.pic", global_cardart_path);
  global_mana_stripes_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Summon.pic", global_cardart_path);
  global_summon_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Dying.pic", global_cardart_path);
  global_dying_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Target.pic", global_cardart_path);
  global_target_pic = load_pic(s.path);
  sprintf(s.path, "%s\\CantTarget.pic", global_cardart_path);
  global_cant_target_pic = load_pic(s.path);
  sprintf(s.path, "%s\\WillUntap.pic", global_cardart_path);
  global_will_untap_pic = load_pic(s.path);

  global_smallcard_pt_font = CreateFontIndirectA(LoadFontFromIni("SmallCardPT", 0));
  global_damage_font = CreateFontIndirectA(LoadFontFromIni("Damage", 0));
  global_idtag_font = CreateFontIndirectA(LoadFontFromIni("IDTag", 0));

  global_palette_col_c9 = GetPaletteColor(0xc9);
  global_palette_col_9e_a = GetPaletteColor(0x9e);
  global_palette_col_9e_b = GetPaletteColor(0x9e);
  global_palette_col_7c = GetPaletteColor(0x7c);
  global_palette_col_2f = GetPaletteColor(0x2f);
  global_palette_col_bf = GetPaletteColor(0xbf);
  global_palette_col_5d = GetPaletteColor(0x5d);
  global_palette_col_1f = GetPaletteColor(0x1f);
  global_pen_palette_5d = CreatePen(6, 2, global_palette_col_5d);
  global_pen_palette_1f = CreatePen(6, 2, global_palette_col_1f);

  for (s.local_c = 0, s.local_8 = 1; s.local_c < 10; ++s.local_c)
  {
    global_debug_pens[s.local_c] =
        CreatePen(6, 3,
                  ((unsigned int)(BYTE)(s.local_c * 75) << 16) |
                      ((unsigned int)(BYTE)(s.local_c * 20) << 8) |
                      (unsigned int)(BYTE)(s.local_c * 10));
    if (global_debug_pens[s.local_c] == (HPEN)0)
      s.local_8 = 0;
  }

  if (!global_damage_pic || !global_card_counters_pic || !global_mana_symbols_pic ||
      !global_abilities_pic || !global_mana_stripes_pic || !global_summon_pic ||
      !global_dying_pic || !global_target_pic || !global_cant_target_pic ||
      !global_will_untap_pic || !global_smallcard_pt_font || !global_damage_font ||
      !global_idtag_font || !global_pen_palette_5d || !global_pen_palette_1f || !s.local_8)
  {
    destroy_create_fonts_resources();
    return false;
  }

  return true;
}

// FUNCTION: DECKDLL 0x10011c41
// FUNCTION: MAGIC 0x00559571
static void destroy_create_fonts_resources(void)
{
  struct
  {
    char path[MAX_PATH + 4];
    int i;
  } s;

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Magim___.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0530m_.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Magis___.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0127m_.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0085m_.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0298m_.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0299m_.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0300m_.TTF");
  RemoveFontResourceA(s.path);

  if (global_damage_pic)
  {
    delete_and_close_object(global_damage_pic);
    global_damage_pic = NULL;
  }
  if (global_card_counters_pic)
  {
    delete_and_close_object(global_card_counters_pic);
    global_card_counters_pic = NULL;
  }
  if (global_mana_symbols_pic)
  {
    delete_and_close_object(global_mana_symbols_pic);
    global_mana_symbols_pic = NULL;
  }
  if (global_abilities_pic)
  {
    delete_and_close_object(global_abilities_pic);
    global_abilities_pic = NULL;
  }
  if (global_mana_stripes_pic)
  {
    delete_and_close_object(global_mana_stripes_pic);
    global_mana_stripes_pic = NULL;
  }
  if (global_summon_pic)
  {
    delete_and_close_object(global_summon_pic);
    global_summon_pic = NULL;
  }
  if (global_dying_pic)
  {
    delete_and_close_object(global_dying_pic);
    global_dying_pic = NULL;
  }
  if (global_target_pic)
  {
    delete_and_close_object(global_target_pic);
    global_target_pic = NULL;
  }
  if (global_cant_target_pic)
  {
    delete_and_close_object(global_cant_target_pic);
    global_cant_target_pic = NULL;
  }
  if (global_will_untap_pic)
  {
    delete_and_close_object(global_will_untap_pic);
    global_will_untap_pic = NULL;
  }

  if (global_smallcard_pt_font)
    DeleteObject(global_smallcard_pt_font);
  global_smallcard_pt_font = NULL;
  if (global_damage_font)
    DeleteObject(global_damage_font);
  global_damage_font = NULL;
  if (global_idtag_font)
    DeleteObject(global_idtag_font);
  global_idtag_font = NULL;
  if (global_pen_palette_5d)
    DeleteObject(global_pen_palette_5d);
  global_pen_palette_5d = NULL;
  if (global_pen_palette_1f)
    DeleteObject(global_pen_palette_1f);
  global_pen_palette_1f = NULL;

  for (s.i = 0; s.i < 10; s.i++)
    if (global_debug_pens[s.i])
    {
      DeleteObject(global_debug_pens[s.i]);
      global_debug_pens[s.i] = NULL;
    }
}

// GLOBAL: DECKDLL 0x10035b3c
static int global_deleteobj_platform_id = -1;

#pragma intrinsic(memset)

// FUNCTION: DECKDLL 0x1002417b
// FUNCTION: MAGIC 0x004948e1
void delete_and_close_object(HANDLE obj)
{
  struct
  {
    char dbg_buf[500];
    OSVERSIONINFOA ver;
    HANDLE section_handle;
    DIBSECTION dib_section;
    char *bits_pointer;
  } s;

  if (global_deleteobj_platform_id == -1)
  {
    s.ver.dwOSVersionInfoSize = 0x94;
    memset(&s.ver.dwMajorVersion, 0, 0x90);
    GetVersionExA(&s.ver);
    if (s.ver.dwPlatformId == 1)
      global_deleteobj_platform_id = 1;
    else
      global_deleteobj_platform_id = 0;
  }

  if (obj != (HANDLE)0)
  {
    GetObjectA(obj, 0x54, &s.dib_section);
    s.section_handle = s.dib_section.dshSection;
    s.bits_pointer = (char *)s.dib_section.dsBm.bmBits;
    s.bits_pointer += s.dib_section.dsOffset;
    DeleteObject(obj);
    if (s.section_handle != (HANDLE)0 && global_deleteobj_platform_id != 0)
      CloseHandle(s.section_handle);
  }

  if (global_DIB_debug != 0)
  {
    sprintf(s.dbg_buf, s__08x_DestroyDIBSection__file_map_10036210, obj, s.section_handle);
    OutputDebugStringA(s.dbg_buf);
  }
}

#pragma function(memset)

#define DELETE_PIC_NOZERO(pic)      \
  if ((pic) != (HANDLE)0)           \
  {                                 \
    delete_and_close_object((pic)); \
  }
#define DELETE_PIC(pic)             \
  if ((pic) != (HANDLE)0)           \
  {                                 \
    delete_and_close_object((pic)); \
    (pic) = (HANDLE)0;              \
  }

// FUNCTION: DECKDLL 0x1000d0ae
static void
delete_pics(void)
{
  DELETE_PIC(global_pic_artist);
  DELETE_PIC(global_pic_rarity);
  DELETE_PIC(global_pic_ability);
  DELETE_PIC(global_pic_all_butn);
  DELETE_PIC(global_pic_astral);
  DELETE_PIC(global_pic_fourth);
  DELETE_PIC(global_pic_green);
  DELETE_PIC(global_pic_yellow);
  DELETE_PIC(global_pic_blue);
  DELETE_PIC(global_pic_grey);
  DELETE_PIC(global_pic_red);
  DELETE_PIC(global_pic_artifact);
  DELETE_PIC(global_pic_castcost);
  DELETE_PIC(global_pic_creature);
  DELETE_PIC(global_pic_enchant);
  DELETE_PIC(global_pic_instant);
  DELETE_PIC(global_pic_interrupt);
  DELETE_PIC(global_pic_land);
  DELETE_PIC(global_pic_power);
  DELETE_PIC(global_pic_sorcery);
  DELETE_PIC(global_pic_tough);
  DELETE_PIC(global_pic_arabnite);
  DELETE_PIC(global_pic_antiquit);
  DELETE_PIC(global_pic_legends);
  DELETE_PIC(global_pic_dark);
  DELETE_PIC_NOZERO(global_pic_dektile4);
  DELETE_PIC_NOZERO(global_pic_dekbar1);
  DELETE_PIC_NOZERO(global_pic_dektit1);
  DELETE_PIC_NOZERO(global_pic_statbak1);
  DELETE_PIC_NOZERO(global_pic_bldr01c);
  DELETE_PIC_NOZERO(global_pic_bldr02c);
  DELETE_PIC_NOZERO(global_pic_bldr03c);
  DELETE_PIC_NOZERO(global_pic_bldr04c);
  DELETE_PIC_NOZERO(global_pic_bldr05c);
}

// FUNCTION: DECKDLL 0x1000c51f
static bool
load_pics(void)
{
  char path[MAX_PATH + 4];

  global_fonts_init_state = 3;

  sprintf(path, "%s\\Artifact.pic", global_dbart_path);
  global_pic_artifact = load_pic(path);
  sprintf(path, "%s\\CastCost.pic", global_dbart_path);
  global_pic_castcost = load_pic(path);
  sprintf(path, "%s\\Creature.pic", global_dbart_path);
  global_pic_creature = load_pic(path);
  sprintf(path, "%s\\Enchant.pic", global_dbart_path);
  global_pic_enchant = load_pic(path);
  sprintf(path, "%s\\Instant.pic", global_dbart_path);
  global_pic_instant = load_pic(path);
  sprintf(path, "%s\\Interrupt.pic", global_dbart_path);
  global_pic_interrupt = load_pic(path);
  sprintf(path, "%s\\Land.pic", global_dbart_path);
  global_pic_land = load_pic(path);
  sprintf(path, "%s\\Power.pic", global_dbart_path);
  global_pic_power = load_pic(path);
  sprintf(path, "%s\\Sorcery.pic", global_dbart_path);
  global_pic_sorcery = load_pic(path);
  sprintf(path, "%s\\Tough.pic", global_dbart_path);
  global_pic_tough = load_pic(path);
  sprintf(path, "%s\\Artist.pic", global_dbart_path);
  global_pic_artist = load_pic(path);
  sprintf(path, "%s\\Rarity.pic", global_dbart_path);
  global_pic_rarity = load_pic(path);
  sprintf(path, "%s\\Ability.pic", global_dbart_path);
  global_pic_ability = load_pic(path);
  sprintf(path, "%s\\All-Butn.pic", global_dbart_path);
  global_pic_all_butn = load_pic(path);
  sprintf(path, "%s\\Astral.pic", global_dbart_path);
  global_pic_astral = load_pic(path);
  sprintf(path, "%s\\Arabnite.pic", global_dbart_path);
  global_pic_arabnite = load_pic(path);
  sprintf(path, "%s\\Antiquit.pic", global_dbart_path);
  global_pic_antiquit = load_pic(path);
  sprintf(path, "%s\\Legends.pic", global_dbart_path);
  global_pic_legends = load_pic(path);
  sprintf(path, "%s\\Dark.pic", global_dbart_path);
  global_pic_dark = load_pic(path);
  sprintf(path, "%s\\Green.pic", global_dbart_path);
  global_pic_green = load_pic(path);
  sprintf(path, "%s\\Yellow.pic", global_dbart_path);
  global_pic_yellow = load_pic(path);
  sprintf(path, "%s\\Blue.pic", global_dbart_path);
  global_pic_blue = load_pic(path);
  sprintf(path, "%s\\Grey.pic", global_dbart_path);
  global_pic_grey = load_pic(path);
  sprintf(path, "%s\\Red.pic", global_dbart_path);
  global_pic_red = load_pic(path);
  sprintf(path, "%s\\dektile4.pic", global_dbart_path);
  global_pic_dektile4 = load_pic(path);
  sprintf(path, "%s\\dekbar1.pic", global_dbart_path);
  global_pic_dekbar1 = load_pic(path);
  sprintf(path, "%s\\dektit1.pic", global_dbart_path);
  global_pic_dektit1 = load_pic(path);
  sprintf(path, "%s\\statbak1.pic", global_dbart_path);
  global_pic_statbak1 = load_pic(path);
  sprintf(path, "%s\\bldr01c.pic", global_dbart_path);
  global_pic_bldr01c = load_pic(path);
  sprintf(path, "%s\\bldr02c.pic", global_dbart_path);
  global_pic_bldr02c = load_pic(path);
  sprintf(path, "%s\\bldr03c.pic", global_dbart_path);
  global_pic_bldr03c = load_pic(path);
  sprintf(path, "%s\\bldr04c.pic", global_dbart_path);
  global_pic_bldr04c = load_pic(path);
  sprintf(path, "%s\\bldr05c.pic", global_dbart_path);
  global_pic_bldr05c = load_pic(path);

  if (!global_pic_dektile4 || !global_pic_dekbar1 || !global_pic_dektit1 || !global_pic_statbak1 ||
      !global_pic_bldr01c || !global_pic_bldr02c || !global_pic_bldr03c || !global_pic_bldr04c ||
      !global_pic_bldr05c || !global_pic_artist || !global_pic_rarity || !global_pic_ability ||
      !global_pic_artifact || !global_pic_castcost || !global_pic_creature || !global_pic_enchant ||
      !global_pic_instant || !global_pic_interrupt || !global_pic_land || !global_pic_power ||
      !global_pic_sorcery || !global_pic_tough || !global_pic_all_butn || !global_pic_astral ||
      !global_pic_green || !global_pic_yellow || !global_pic_blue || !global_pic_grey ||
      !global_pic_red)
  {

    DELETE_PIC_NOZERO(global_pic_dektile4);
    DELETE_PIC_NOZERO(global_pic_dekbar1);
    DELETE_PIC_NOZERO(global_pic_dektit1);
    DELETE_PIC_NOZERO(global_pic_statbak1);
    DELETE_PIC_NOZERO(global_pic_bldr01c);
    DELETE_PIC_NOZERO(global_pic_bldr02c);
    DELETE_PIC_NOZERO(global_pic_bldr03c);
    DELETE_PIC_NOZERO(global_pic_bldr04c);
    DELETE_PIC_NOZERO(global_pic_bldr05c);

    DELETE_PIC(global_pic_artist);
    DELETE_PIC(global_pic_rarity);
    DELETE_PIC(global_pic_ability);
    DELETE_PIC(global_pic_all_butn);
    DELETE_PIC(global_pic_astral);
    DELETE_PIC(global_pic_fourth);
    DELETE_PIC(global_pic_green);
    DELETE_PIC(global_pic_yellow);
    DELETE_PIC(global_pic_blue);
    DELETE_PIC(global_pic_grey);
    DELETE_PIC(global_pic_red);
    DELETE_PIC(global_pic_artifact);
    DELETE_PIC(global_pic_castcost);
    DELETE_PIC(global_pic_creature);
    DELETE_PIC(global_pic_enchant);
    DELETE_PIC(global_pic_instant);
    DELETE_PIC(global_pic_interrupt);
    DELETE_PIC(global_pic_land);
    DELETE_PIC(global_pic_power);
    DELETE_PIC(global_pic_sorcery);
    DELETE_PIC(global_pic_tough);

    // Conspicuously missing
    /*
    DELETE_PIC(global_pic_arabnite);
    DELETE_PIC(global_pic_antiquit);
    DELETE_PIC(global_pic_legends);
    DELETE_PIC(global_pic_dark);
    */

    return false;
  }

  return true;
}

#undef DELETE_PIC_NOZERO
#undef DELETE_PIC

// FUNCTION: DECKDLL 0x1000bbec
static int
delete_resources(void)
{
  unsigned int v1;
  int i;

  write_manalink_ini();

  if (global_db_flags_1 & (DBFLAGS_EDITDECK | DBFLAGS_SHANDALAR))
  {
    for (i = 0; i < global_deck_num_entries; ++i)
    {
      v1 = 0;
      if (global_deck[i].GDE_Available)
        v1 |= 0x4000;
      v1 |= global_deck[i].GDE_DecksBits << 16;
      deck[i] = global_deck[i].GDE_iid | v1;
    }

    for (i = global_deck_num_entries; i < 500; ++i)
      deck[i] = -1;

    *global_external_current_deck = global_current_deck;
  }

  delete_pics();

  delete_brushes();
  DestroyCardArtPalette();
  delete_fonts();
  DeleteDC(global_hdc);
  DeleteObject(global_hbmp);
  chdir(global_previous_directory);
  return 0;
}

// FUNCTION: DECKDLL 0x1000c337
bool process_cue_cards(MSG *msg)
{
  struct
  {
    POINT screen_point; /* ebp - 0x18 */
    LRESULT msgnum;     /* ebp - 0x10 */
    HWND hwnd;          /* ebp - 0xc */
    POINT client_point; /* ebp - 0x8 */
  } s;

  // GLOBAL: DECKDLL 0x100327d0
  static int last_cursor_x = 0;
  // GLOBAL: DECKDLL 0x100327d4
  static int last_cursor_y = 0;
  // GLOBAL: DECKDLL 0x100f30a0
  static HWND last_hwnd = NULL;

  if (msg->message == WM_TIMER && msg->hwnd == global_main_hwnd && msg->wParam == 0)
  {
    GetCursorPos(&s.screen_point);
    KillTimer(global_main_hwnd, 0);

    s.hwnd = WindowFromPoint(s.screen_point);
    s.client_point.x = s.screen_point.x;
    s.client_point.y = s.screen_point.y;
    ScreenToClient(s.hwnd, &s.client_point);
    s.msgnum = SendMessage(s.hwnd,
                           0x465,
                           0,
                           MAKELONG2(s.client_point.x, s.client_point.y));
    if (s.msgnum != 0)
      SendMessage(global_cuecard_hwnd,
                  0x400,
                  MAKELONG3(s.screen_point.x + 5, s.screen_point.y + 0xf),
                  (LPARAM)text_lines[s.msgnum - 1]);
    return true;
  }

  if (msg->message == WM_MOUSEMOVE)
  {
    GetCursorPos(&s.screen_point);
    if (abs(last_cursor_y - s.screen_point.y) + abs(last_cursor_x - s.screen_point.x) < 2)
      return false;

    last_cursor_x = s.screen_point.x;
    last_cursor_y = s.screen_point.y;
    SetTimer(global_main_hwnd, 0, 500, NULL);
    if (last_hwnd != msg->hwnd)
    {
      last_hwnd = msg->hwnd;
      ShowWindow(global_cuecard_hwnd, SW_HIDE);
    }
    return false;
  }

  if ((unsigned int)msg->message >= WM_MOUSEMOVE && (unsigned int)msg->message <= WM_MBUTTONDBLCLK)
  {
    KillTimer(global_main_hwnd, 0);
    ShowWindow(global_cuecard_hwnd, SW_HIDE);
    return false;
  }

  return false;
}

/* stdcall version exported without stdcall suffix; used by assembly and external callers. */
// FUNCTION: DECKDLL 0x1000bcfd
WPARAM WINAPI
DeckBuilderMain(HWND parent_hwnd, int db_flags_1, int db_flags_2)
{
  struct locals
  {
    int local_30;
    int local_2c;
    MSG msg;
    int local_0c;
    HDC desktop_hdc;
    HACCEL accel;
  } s;

  s.local_0c = 0;

  global_db_flags_1 = db_flags_1;
  global_db_flags_2 = db_flags_2;

  InitializeCriticalSection(&global_critical_section_for_unknown);

  _getcwd(global_previous_directory, 0x105);
  _chdir(global_base_directory);

  CardIDFromType = (Int_fn_int)GetProcAddress(GetModuleHandleA(NULL), "CardIDFromType");
  CardTypeFromID = (Int_fn_int)GetProcAddress(GetModuleHandleA(NULL), "CardTypeFromID");
  CardInDeck = (Int_fn_int)GetProcAddress(GetModuleHandleA(NULL), "CardInDeck");
  SetCardInDeck = (Int_fn_int)GetProcAddress(GetModuleHandleA(NULL), "SetCardInDeck");
  SellPrice = (Int_fn_int)GetProcAddress(GetModuleHandleA(NULL), "SellPrice");
  deck = (int *)GetProcAddress(GetModuleHandleA(NULL), "deck");
  Gold = (int *)GetProcAddress(GetModuleHandleA(NULL), "Gold");
  Scards = (shandalar_worldmagic_t *)GetProcAddress(GetModuleHandleA(NULL), "Scards");
  global_external_deckname = (char *)GetProcAddress(GetModuleHandleA(NULL), "szDeckName");
  global_external_current_deck = (int *)GetProcAddress(GetModuleHandleA(NULL), "_currentDeck");

  if (global_db_flags_1 & (DBFLAGS_EDITDECK | DBFLAGS_SHANDALAR))
  {
    if (!CardIDFromType || !CardInDeck || !SetCardInDeck || !SellPrice || !deck || !Gold || !Scards || !global_external_current_deck)
    {
      fatal_err("Deck Builder WinMain: Couldn't find External Functions.", 0);
      return 0;
    }
  }
  else
  {
    if (global_db_flags_1 & DBFLAGS_NOCARDCOUNTCHECK || global_db_flags_1 & DBFLAGS_GAUNTLET)
    {
      if (!CardIDFromType || !CardTypeFromID || !CardInDeck || !SetCardInDeck || !Gold || !global_external_deckname)
      {
        fatal_err("Deck Builder WinMain: Couldn't find External Functions.", 0);
        return 0;
      }
    }
  }

  if (!SetupDuelPalette())
  {
    fatal_err("WinMain: Couldn't create the palette", 0);
    return 0;
  }

  if (global_db_flags_1 & (DBFLAGS_EDITDECK | DBFLAGS_SHANDALAR))
    global_current_deck = *global_external_current_deck;

  s.desktop_hdc = GetDC(GetDesktopWindow());

  global_hdc = CreateCompatibleDC(s.desktop_hdc);
  global_hbmp = CreateCompatibleBitmap(s.desktop_hdc, 800, 800);
  if (!global_hdc || !global_hbmp)
    fatal_err("WinMain: Couldn't create the app-wide memory DC or bitmap", 0);

  ApplyCardArtPaletteToDc(global_hdc);

  global_old_bmp_obj = SelectObject(global_hdc, global_hbmp);

  ReleaseDC(GetDesktopWindow(), s.desktop_hdc);

  if (!InitCardArtGdiResources())
  {
    fatal_err("WinMain: Couldn't load deck builder bitmaps", 0);
    return 0;
  }

  if (!load_pics())
  {
    fatal_err("WinMain: Couldn't load deck builder bitmaps", 0);
    return 0;
  }

  if (!create_fonts())
  {
    fatal_err("WinMain: Couldn't load the card background bitmaps", 0);
    return 0;
  }

  if (!create_brushes())
  {
    fatal_err("WinMain: Couldn't create the window background bitmaps", 0);
    return 0;
  }

  make_orig_rarities("DeckBuilder.csv", global_origrarities);

  s.local_30 = GetSystemMetrics(0);
  s.local_2c = GetSystemMetrics(1);

  global_main_hwnd = CreateWindowEx(0, "MAGICDECK_MainClass", "Deck Maker",
                                    0x90040000,
                                    0, 0, s.local_30, s.local_2c,
                                    parent_hwnd,
                                    0,
                                    global_hinstance, 0);
  if (!global_main_hwnd)
  {
    fatal_err("WinMain: Couldn't create the main window", 0);
    return 0;
  }

  global_cuecard_hwnd = CreateWindowEx(0, "CueCardClass", "",
                                       WS_POPUP,
                                       0, 0, 0, 0,
                                       global_main_hwnd,
                                       0,
                                       global_hinstance, 0);
  if (!global_cuecard_hwnd)
  {
    fatal_err("WinMain: Couldn't create the cue card window", global_main_hwnd);
    return 0;
  }

  s.accel = LoadAccelerators(global_hinstance, MAKEINTRESOURCE(RES_ACCEL));
  if (!s.accel)
  {
    fatal_err("WinMain: Couldn't load the accelerator table", global_main_hwnd);
    return 0;
  }

  ShowWindow(global_main_hwnd, SW_SHOWNORMAL);
  SetTimer(global_main_hwnd, 0, 500, NULL);
  SetForegroundWindow(global_main_hwnd);

  while (GetMessage(&s.msg, NULL, 0, 0))
    if (!TranslateAccelerator(global_main_hwnd, s.accel, &s.msg) && !process_cue_cards(&s.msg))
    {
      TranslateMessage(&s.msg);
      DispatchMessage(&s.msg);
    }

  delete_resources();
  ShutdownCardArtGdiResources();
  destroy_create_fonts_resources();
  DeleteCriticalSection(&global_critical_section_for_unknown);

  return s.msg.wParam;
}

WPARAM
deckbuilder_main(HWND parent_hwnd, int db_flags_1, int db_flags_2)
{
  return DeckBuilderMain(parent_hwnd, db_flags_1, db_flags_2);
}

// TODO: call to RegisterClass seems to think it returns an int in orig?
#define REGISTER_AND_RETURN_CLASS(classname, wndproc, style, extra_size, icon, cursor)                    \
  WNDCLASS hwnd = {style, wndproc, 0, extra_size, global_hinstance, icon, cursor, NULL, NULL, classname}; \
  if (!(RegisterClass(&hwnd) & 0x0000ffff))                                                               \
    return 0;                                                                                             \
  return 1;

// FUNCTION: DECKDLL 0x10001140
bool register_DeckSurfaceClass(void)
{
  WNDCLASS hwnd;

  hwnd.style = 0;
  hwnd.lpfnWndProc = wndproc_DeckSurfaceClass;
  hwnd.cbClsExtra = 0;
  hwnd.cbWndExtra = 0;
  hwnd.hInstance = global_hinstance;
  hwnd.hIcon = LoadIcon(0, IDI_APPLICATION);
  hwnd.hCursor = LoadCursor(0, IDC_ARROW);
  hwnd.hbrBackground = (HBRUSH)0x0;
  hwnd.lpszMenuName = (LPCSTR)0x0;
  hwnd.lpszClassName = "MAGICDECK_DeckSurfaceClass";

  if (!(RegisterClassA(&hwnd) & 0x0000ffff))
    return 0;

  hwnd.style = 0;
  hwnd.lpfnWndProc = wndproc_SideboardSurfaceClass;
  hwnd.cbClsExtra = 0;
  hwnd.cbWndExtra = 0;
  hwnd.hInstance = global_hinstance;
  hwnd.hIcon = LoadIcon(0, IDI_APPLICATION);
  hwnd.hCursor = LoadCursor(0, IDC_ARROW);
  hwnd.hbrBackground = (HBRUSH)0x0;
  hwnd.lpszMenuName = (LPCSTR)0x0;
  hwnd.lpszClassName = "MAGICDECK_SideboardSurfaceClass";

  if (!(RegisterClassA(&hwnd) & 0x0000ffff))
    return 0;

  hwnd.style = 0;
  hwnd.lpfnWndProc = wndproc_TradeSurfaceClass;
  hwnd.cbClsExtra = 0;
  hwnd.cbWndExtra = 0;
  hwnd.hInstance = global_hinstance;
  hwnd.hIcon = LoadIconA((HINSTANCE)0x0, (LPCSTR)0x7f00);
  hwnd.hCursor = LoadCursorA((HINSTANCE)0x0, (LPCSTR)0x7f00);
  hwnd.hbrBackground = (HBRUSH)0x0;
  hwnd.lpszMenuName = (LPCSTR)0x0;
  hwnd.lpszClassName = "MAGICDECK_TradeSurfaceClass";

  if (!(RegisterClass(&hwnd) & 0x0000ffff))
    return 0;

  return 1;
}

// FUNCTION: DECKDLL 0x10008080
bool register_CardClass(void)
{
  // GLOBAL: DECKDLL 0x10030008
  static int DAT_10030008 = 6;
  REGISTER_AND_RETURN_CLASS("MAGICDECK_CardClass",
                            wndproc_CardClass,
                            CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
                            DAT_10030008,
                            LoadIcon(0, IDI_APPLICATION),
                            LoadCursor(0, IDC_ARROW));
}

// FUNCTION: DECKDLL 0x10010EF0
bool register_FullCardClass(void)
{
  // GLOBAL: DECKDLL 0x1003001c
  static int DAT_1003001c = 0xe;
  REGISTER_AND_RETURN_CLASS("MagicFullCardClass",
                            wndproc_FullCardClass,
                            CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
                            DAT_1003001c,
                            0,
                            LoadCursor(0, IDC_ARROW));
}

// FUNCTION: DECKDLL 0x10016DC0
bool register_MainClass(void)
{
  REGISTER_AND_RETURN_CLASS("MAGICDECK_MainClass",
                            wndproc_MainClass,
                            CS_OWNDC,
                            0,
                            LoadIcon(global_hinstance, MAKEINTRESOURCE(RES_ICON)),
                            LoadCursor(0, IDC_ARROW));
}

// FUNCTION: DECKDLL 0x1001BC50
bool register_CardListFilterClass(void)
{
  // GLOBAL: DECKDLL 0x10030024
  static int DAT_10030024 = 0xe;
  REGISTER_AND_RETURN_CLASS("MAGICDECK_CardListFiltersClass",
                            wndproc_CardListFilterClass,
                            CS_HREDRAW | CS_VREDRAW,
                            DAT_10030024,
                            0,
                            LoadCursor(0, IDC_ARROW));
}

// FUNCTION: DECKDLL 0x10021360
bool register_CueCardClass(void)
{
  // GLOBAL: DECKDLL 0x1003002c
  static int DAT_1003002c = 4;
  REGISTER_AND_RETURN_CLASS("CueCardClass",
                            wndproc_CueCardClass,
                            CS_SAVEBITS,
                            DAT_1003002c,
                            LoadIcon(0, IDI_APPLICATION),
                            LoadCursor(0, IDC_ARROW));
}

// FUNCTION: DECKDLL 0x100275C0
bool register_TitleClass(void)
{
  REGISTER_AND_RETURN_CLASS("MAGICDECK_TitleClass",
                            wndproc_TitleClass,
                            0,
                            0,
                            LoadIcon(0, IDI_APPLICATION),
                            LoadCursor(0, IDC_ARROW));
}

// FUNCTION: DECKDLL 0x100287F9
bool register_HorzListClass(void)
{
  // GLOBAL: DECKDLL 0x10030048
  static int DAT_10030048 = 0xe;
  REGISTER_AND_RETURN_CLASS("MAGICDECK_HorzListClass",
                            wndproc_HorzListClass,
                            CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
                            DAT_10030048,
                            0,
                            LoadCursor(0, IDC_ARROW));
}

// FUNCTION: DECKDLL 0x1000c27a
static bool
register_classes(void)
{
  bool result = 1;
  if (!register_MainClass())
    result = 0;
  if (!register_DeckSurfaceClass())
    result = 0;
  if (!register_CardClass())
    result = 0;
  if (!register_FullCardClass())
    result = 0;
  if (!register_HorzListClass())
    result = 0;
  if (!register_CardListFilterClass())
    result = 0;
  if (!register_CueCardClass())
    result = 0;
  if (!register_TitleClass())
    result = 0;
  return result;
}

// GLOBAL: DECKDLL 0x10104d78
static int global_unknown_10104d78 = 0;

// FUNCTION: DECKDLL 0x1000f190
static int __stdcall
FUN_1000f190(void)
{
  return 1;
}

// FUNCTION: DECKDLL 0x1001b5e7
static int
FUN_1001b5e7(const char *newdata_csv_path, int available_slots)
{
  (void)newdata_csv_path;
  (void)available_slots;
  global_unknown_10104d78 = 0;
  return 1;
}

// FUNCTION: DECKDLL 0x1000b8df
static int
init_deckbuilder(HINSTANCE dll, int unused)
{
  struct init_deckbuilder_locals
  {
    char *pcVar3;         /* [ebp-0x110] */
    HDC hdc;              /* [ebp-0x10c] */
    char path_buf[0x108]; /* [ebp-0x108] */
  } s;

  global_hinstance = dll;
  if (InitLicenseSecretsFromRegistry())
    return 0;

  read_manalink_ini();
  global_cfg_consolidate = (int)(signed char)global_cfg.consolidate_from_registry;
  global_deck_was_edited = 0;
  global_deckname_set = 0;
  global_deckbuilder_unknown_1019f010 = 0;
  global_current_deck = 0;

#ifdef _DEBUG
  // Allow debugging directly from output directory
  if (getcwd(global_base_directory, sizeof(global_base_directory)) == NULL)
  {
    return 0;
  }
#else
  GetModuleFileNameA(0, global_base_directory, 0x105);
  s.pcVar3 = strrchr(global_base_directory, '\\');
  *s.pcVar3 = 0;
#endif

  strcpy(global_manalink_ini_path, global_base_directory);
  strcpy(global_playdeck_path, global_base_directory);
  strcat(global_playdeck_path, "\\PlayDeck");
  strcpy(global_cardart_path, global_base_directory);
  strcat(global_cardart_path, "\\CARDART");
  strcpy(global_duelart_path, global_base_directory);
  strcat(global_duelart_path, "\\DUELART");
  strcpy(global_dbart_path, global_base_directory);
  strcat(global_dbart_path, "\\DBART");
  strcpy(global_duel_dat_path, global_duelart_path);
  strcat(global_duel_dat_path, "\\Duel.dat");

  strcpy(global_ui_strings_filename, "UIStrings.txt");
  load_text(global_ui_strings_filename, "ARTISTLINE");
  strcpy(global_artistline, text_lines[0]);
  global_unknown_1012df44 = 1;

  strcpy(s.path_buf, global_base_directory);
  strcat(s.path_buf, "\\CARDS.DAT");
  global_available_slots = read_db_guts(s.path_buf);
  if (global_available_slots == 0)
    fatal_err("WinMain: Couldn't find CARDS.DAT", (HWND)0);

  strcpy(s.path_buf, global_base_directory);
  strcat(s.path_buf, "\\NEWDATA.CSV");
  if (!FUN_1001b5e7(s.path_buf, global_available_slots))
    fatal_err("WinMain: Couldn't find CARDS.DAT", (HWND)0);

  s.hdc = GetDC(GetDesktopWindow());
  global_supports_palette = GetDeviceCaps(s.hdc, RASTERCAPS) & RC_PALETTE;
  global_color_depth = GetDeviceCaps(s.hdc, BITSPIXEL) * GetDeviceCaps(s.hdc, PLANES);
  ReleaseDC(GetDesktopWindow(), s.hdc);

  global_fonts_init_state = 3;
  InitCommonControls();

  if (!FUN_1000f190())
  {
    MessageBoxA((HWND)0, "Deckbuilder.dat is either missing or currupt!", "Magic: The Gathering", 0x10);
    return 0;
  }

  if (!register_classes())
  {
    fatal_err("WinMain: Couldn't register the classes", (HWND)0);
  }

  return 0;
}

// FUNCTION: DECKDLL 0x1000b860
BOOL WINAPI DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved)
{
  int result;

  switch (reason)
  {
  case DLL_PROCESS_ATTACH:
    result = 1;
    init_deckbuilder(dll, 0);
    return result;
  case DLL_THREAD_ATTACH:
    return 1;
  case DLL_THREAD_DETACH:
    return 1;
  case DLL_PROCESS_DETACH:
    return 1;
  }
}

BOOL WINAPI DllEntryPoint(HINSTANCE dll, DWORD reason, LPVOID reserved) // Stupidly enough, there's a direct reference to this alternate name.
{
  return DllMain(dll, reason, reserved);
}

// FUNCTION: DECKDLL 0x1001a8ca
static void
sound_init(const char *path, int num)
{
  Sound snd;

  if (global_db_flags_1 & DBFLAGS_SHANDALAR)
    return;

  memset(&snd, 0, sizeof(Sound));
  snd.flags = (int)snd.flags | 4;
  snd.volume = 400;
  snd.sampleRate = 0;
  snd.pan = 0;

  sound_load(path, num, &snd);

  set_sound_loop(num, 1);
}

// FUNCTION: DECKDLL 0x1001a62a
static void
init_sounds_and_music(void)
{
  struct
  {
    char path[0x50];
    int i;
  } s;

  if (global_db_flags_1 & DBFLAGS_SHANDALAR)
    return;

  // Very expensive srand()
  for (s.i = 0; (clock() % 1024) > s.i; s.i++)
  {
    rand();
  }

  s.i = (rand() % 0x13) + 1;
  sprintf(s.path, "sound\\locmus%d.wav", s.i);
  sound_init(s.path, 1);
  sprintf(s.path, "DuelSounds\\discard.wav");
  sound_load(s.path, 3, 0);
  sprintf(s.path, "DuelSounds\\draw.wav");
  sound_load(s.path, 2, 0);
  sprintf(s.path, "DuelSounds\\button.wav");
  sound_load(s.path, 4, 0);
  sprintf(s.path, "DuelSounds\\cancel.wav");
  sound_load(s.path, 5, 0);
}

// FUNCTION: DECKDLL 0x1001a741
static void
free_sounds_and_music(void)
{
  if (global_db_flags_1 & 1)
    return;

  sound_unload(1);
  sound_unload(2);
  sound_unload(3);
  sound_unload(4);
  sound_unload(5);
  sound_close();
}

// FUNCTION: DECKDLL 0x1001a795
void play_sound(int a1, int a2, int a3, int a4)
{
  Sound snd;

  if (global_db_flags_1 & DBFLAGS_SHANDALAR)
    return;

  memset(&snd, 0, sizeof(Sound));
  snd.volume = 4 * a2;
  snd.sampleRate = 22050 * a3 / 100;
  snd.pan = 4 * a4;
  snd.flags &= ~1;
  snd.flags &= ~0x10;

  sound_play(a1, &snd);
}

// FUNCTION: DECKDLL 0x1001a80f
static void
play_music(int a1, int a2, int a3)
{
  Sound snd;

  if (global_db_flags_1 & DBFLAGS_SHANDALAR)
    return;

  memset(&snd, 0, sizeof(Sound));
  snd.volume = 4 * a2;
  snd.sampleRate = 22050;
  snd.pan = 4 * a3;
  snd.flags = (int)snd.flags | 1;

  sound_play(a1, &snd);
}

// FUNCTION: DECKDLL 0x10006e7f
INT_PTR CALLBACK
dlgproc_DeckInfo(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  /* Stack layout is extremely sensitive; keep locals grouped. */
  struct
  {
    HDC hdc2;           /* [ebp-0x134] */
    RECT r;             /* [ebp-0x130] */
    LPARAM lparam_copy; /* [ebp-0x120] */
    HDC hdc;            /* [ebp-0x11c] */
    HGDIOBJ local_10;   /* [ebp-0x118] */
    int local_114;      /* [ebp-0x114] */
    char buf[264];      /* [ebp-0x110] */
    int local_c;        /* [ebp-0x08] */
    char *p;            /* [ebp-0x04] */
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    load_text("menus", "TITLEDIALOG");
    SetWindowTextA(hdlg, text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_DECKTITLE), text_lines[1]);
    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_DESCRIPTION), text_lines[2]);
    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_NAME), text_lines[3]);
    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_EMAIL), text_lines[4]);
    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_DATE), text_lines[5]);
    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_DECKFACES), text_lines[6]);
    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_COMMENT), text_lines[7]);
    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_VERSION), text_lines[8]);

    load_text("menus", "OKCANCEL");
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_OK), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_CANCEL), text_lines[1]);

    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_DECKTITLE_EDITTEXT), global_deckinfo.deckname);
    SendMessageA(GetDlgItem(hdlg, RES_DECKINFO_DECKTITLE_EDITTEXT), EM_LIMITTEXT, 0x1c, 0);

    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_DESCRIPTION_EDITTEXT), global_deckinfo.description);
    SendMessageA(GetDlgItem(hdlg, RES_DECKINFO_DESCRIPTION_EDITTEXT), EM_LIMITTEXT, 0x12, 0);

    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_NAME_EDITTEXT), global_deckinfo.author);
    SendMessageA(GetDlgItem(hdlg, RES_DECKINFO_NAME_EDITTEXT), EM_LIMITTEXT, 0x4e, 0);

    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_EMAIL_EDITTEXT), global_deckinfo.email);
    SendMessageA(GetDlgItem(hdlg, RES_DECKINFO_EMAIL_EDITTEXT), EM_LIMITTEXT, 0x4e, 0);

    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_DATE_EDITTEXT), global_deckinfo.creation_date);
    SendMessageA(GetDlgItem(hdlg, RES_DECKINFO_DATE_EDITTEXT), EM_LIMITTEXT, 0x13, 0);

    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_VERSION_EDITTEXT), "4th Edition");
    SendMessageA(GetDlgItem(hdlg, RES_DECKINFO_VERSION_EDITTEXT), EM_LIMITTEXT, 0xd, 0);

    SetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_COMMENT_EDITTEXT), global_deckinfo.comments);
    SendMessageA(GetDlgItem(hdlg, RES_DECKINFO_COMMENT_EDITTEXT), EM_LIMITTEXT, 0x18e, 0);

    s.local_c = load_text("menus", "DECKFACES");
    for (s.local_114 = 0; s.local_114 < s.local_c; s.local_114 = s.local_114 + 1)
      SendMessageA(GetDlgItem(hdlg, RES_DECKINFO_DECKFACES_COMBOBOX), LB_ADDSTRING, 0, (LPARAM)(text_lines + s.local_114));
    SendMessageA(GetDlgItem(hdlg, RES_DECKINFO_DECKFACES_COMBOBOX), LB_SETCURSEL, global_deckinfo.revision - 1, 0);

    sprintf(s.buf, "%s\\GAUN_Results.pic", global_duelart_path);
    global_deckinfo_pic = load_pic(s.buf);
    return 0;

  case WM_COMMAND:
  {
    if ((((unsigned int)wparam) & 0xffff) == RES_BUTTON_OK)
    {
      s.local_114 = GetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_DECKTITLE_EDITTEXT), s.buf, 0x105);
      s.p = s.buf;
      while (*s.p != '\0')
      {
        if (*s.p == '.')
          strcpy(s.p, s.p + 1);
        else
          s.p = s.p + 1;
      }
      s.buf[s.local_114] = '\0';
      if (strcmp(s.buf, global_deckinfo.deckname) != 0)
      {
        global_deck_was_edited = 1;
        global_deckname_set = 1;
      }
      strcpy(global_deckinfo.deckname, s.buf);
      s.local_114 = 0;

      s.local_114 = GetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_DESCRIPTION_EDITTEXT), global_deckinfo.description, 0x15);
      global_deckinfo.description[s.local_114] = '\0';
      s.local_114 = 0;

      s.local_114 = GetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_NAME_EDITTEXT), global_deckinfo.author, 0x51);
      global_deckinfo.author[s.local_114] = '\0';
      s.local_114 = 0;

      s.local_114 = GetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_EMAIL_EDITTEXT), global_deckinfo.email, 0x51);
      global_deckinfo.email[s.local_114] = '\0';
      s.local_114 = 0;

      s.local_114 = GetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_DATE_EDITTEXT), global_deckinfo.creation_date, 0x16);
      global_deckinfo.creation_date[s.local_114] = '\0';

      global_deckinfo.revision = SendMessageA(GetDlgItem(hdlg, RES_DECKINFO_DECKFACES_COMBOBOX), LB_GETCURSEL, 0, 0) + 1;

      GetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_VERSION_EDITTEXT), global_deckinfo.edition, 0x10);
      s.local_114 = 0;

      s.local_114 = GetWindowTextA(GetDlgItem(hdlg, RES_DECKINFO_COMMENT_EDITTEXT), global_deckinfo.comments, 0x191);
      global_deckinfo.comments[s.local_114] = '\0';

      strcpy(global_deck_filename, global_playdeck_path);
      strcat(global_deck_filename, "\\");
      strcat(global_deck_filename, global_deckinfo.deckname);
      strcat(global_deck_filename, ".dck");
      EndDialog(hdlg, 1);
      if (global_deckinfo_pic != (HANDLE)0)
        delete_and_close_object(global_deckinfo_pic);
    }
    else if ((((unsigned int)wparam) & 0xffff) == RES_BUTTON_CANCEL)
    {
      if (global_deckinfo_pic != (HANDLE)0)
        delete_and_close_object(global_deckinfo_pic);
      EndDialog(hdlg, 0);
    }
    return 1;
  }

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
  {
    s.hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc);
    s.lparam_copy = lparam;
    SetBkMode(s.hdc, TRANSPARENT);
    return (INT_PTR)s.local_10 = GetStockObject(HOLLOW_BRUSH);
  }

  case WM_ERASEBKGND:
  {
    s.hdc2 = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc2);
    GetClientRect(hdlg, &s.r);
    if (global_deckinfo_pic != (HANDLE)0)
    {
      DrawBitmapToRect(s.hdc2, &s.r, global_deckinfo_pic);
    }
    else
    {
      FillRect(s.hdc2, &s.r, (HBRUSH)GetStockObject(GRAY_BRUSH));
    }
    return 1;
  }

  default:
    return 0;
  }
}

// FUNCTION: DECKDLL 0x10006df3
bool show_dialog_deckinfo(void)
{
  struct
  {
    int result;
    INT_PTR dlg_result;
  } s;

  s.dlg_result = DialogBoxParam(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_DECKINFO), global_main_hwnd, dlgproc_DeckInfo, 0);
  if (s.dlg_result == -1)
  {
    MessageBox(global_main_hwnd, "Couldn't bring up the title dialog box", "", 0);
    s.result = 0;
  }
  else if (s.dlg_result == 0)
  {
    s.result = 0;
  }
  else if (s.dlg_result == 1)
  {
    s.result = 1;
  }
  return s.result;
}

// FUNCTION: DECKDLL 0x10007bad
INT_PTR CALLBACK
dlgproc_GroupMove(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  /* Stack layout is extremely sensitive; keep locals grouped. */
  struct
  {
    HDC hdc_erase;      /* [ebp-0x128] */
    RECT r;             /* [ebp-0x124] */
    LPARAM lparam_copy; /* [ebp-0x114] */
    HDC hdc;            /* [ebp-0x110] */
    HGDIOBJ pv;         /* [ebp-0x10c] */
    char path[264];     /* [ebp-0x108] */
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
  {
    load_text("menus", "GROUPMOVE");
    SetWindowTextA(hdlg, text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_BLACK), text_lines[1]);
    SetWindowTextA(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_BLUE), text_lines[2]);
    SetWindowTextA(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_GREEN), text_lines[3]);
    SetWindowTextA(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_RED), text_lines[4]);
    SetWindowTextA(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_WHITE), text_lines[5]);
    SetWindowTextA(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_ARTIFACT), text_lines[6]);

    load_text("menus", "OKCANCEL");
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_OK), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_CANCEL), text_lines[1]);

    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_BLACK), global_dlg_parameter & 1);
    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_BLUE), (global_dlg_parameter & 2U) >> 1);
    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_GREEN), (global_dlg_parameter & 4U) >> 2);
    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_RED), (global_dlg_parameter & 8U) >> 3);
    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_WHITE), (global_dlg_parameter & 0x10U) >> 4);
    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_ARTIFACT), (global_dlg_parameter & 0x20U) >> 5);

    sprintf(s.path, "%s\\GAUN_Options.pic", global_duelart_path);
    global_groupmove_pic = load_pic(s.path);
    return 0;
  }

  case WM_COMMAND:
  {
    if ((((unsigned int)wparam) & 0xffff) == RES_BUTTON_OK)
    {
      global_dlg_result = 0;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_BLACK))
        global_dlg_result |= 1;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_BLUE))
        global_dlg_result |= 2;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_GREEN))
        global_dlg_result |= 4;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_RED))
        global_dlg_result |= 8;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_WHITE))
        global_dlg_result |= 0x10;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_ARTIFACT))
        global_dlg_result |= 0x20;
      if (global_groupmove_pic != (HANDLE)0)
        delete_and_close_object(global_groupmove_pic);
      EndDialog(hdlg, 1);
    }
    else if ((((unsigned int)wparam) & 0xffff) == RES_BUTTON_CANCEL)
    {
      if (global_groupmove_pic != (HANDLE)0)
        delete_and_close_object(global_groupmove_pic);
      global_dlg_result = 0;
      EndDialog(hdlg, 0);
    }
    return 1;
  }

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
  {
    s.hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc);
    s.lparam_copy = lparam;
    SetBkMode(s.hdc, TRANSPARENT);
    s.pv = GetStockObject(HOLLOW_BRUSH);
    return (INT_PTR)s.pv;
  }

  case WM_ERASEBKGND:
  {
    s.hdc_erase = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc_erase);

    GetClientRect(hdlg, &s.r);
    if (global_groupmove_pic != (HANDLE)0)
    {
      DrawBitmapToRect(s.hdc_erase, &s.r, global_groupmove_pic);
    }
    else
    {
      FillRect(s.hdc_erase, &s.r, (HBRUSH)GetStockObject(GRAY_BRUSH));
    }
    return 1;
  }

  default:
    return 0;
  }
}

// FUNCTION: DECKDLL 0x10007b21
bool show_dialog_groupmove(void)
{
  struct
  {
    int result;
    INT_PTR dlg_result;
  } s;

  s.dlg_result = DialogBoxParam(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_GROUPMOVE), global_main_hwnd, dlgproc_GroupMove, 0);
  if (s.dlg_result == -1)
  {
    MessageBox(global_main_hwnd, "Couldn't bring up the filter dialog box", "", 0);
    s.result = 0;
  }
  else if (s.dlg_result == 0)
  {
    s.result = 0;
  }
  else if (s.dlg_result == 1)
  {
    s.result = 1;
  }
  return s.result;
}

// FUNCTION: DECKDLL 0x10006aba
INT_PTR CALLBACK dlgproc_AskX(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC hdc_erase;      /* [ebp-0x158] */
    RECT client_rect;   /* [ebp-0x154] */
    LPARAM lparam_copy; /* [ebp-0x144] */
    HDC hdc_copy;       /* [ebp-0x140] */
    HGDIOBJ pv;         /* [ebp-0x13c] */
    int text_len;       /* [ebp-0x138] */
    char path[264];     /* [ebp-0x134] */
    char edit_txt[12];  /* [ebp-0x2c] */
    RECT rect_main;     /* [ebp-0x20] */
    RECT rect_dlg;      /* [ebp-0x10] */
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
  {
    sprintf(s.edit_txt, "%d", global_dlg_parameter);
    SetWindowTextA(hdlg, global_ask_x_dlg_title);
    SetWindowTextA(GetDlgItem(hdlg, RES_ASKVALUE_EDITTEXT), s.edit_txt);

    load_text("menus", "OKCANCEL");
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_OK), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_CANCEL), text_lines[1]);

    GetWindowRect(global_main_hwnd, &s.rect_main);
    GetWindowRect(hdlg, &s.rect_dlg);
    SetWindowPos(hdlg, (HWND)0,
                 s.rect_main.left +
                     ((s.rect_main.right - s.rect_main.left) - (s.rect_dlg.right - s.rect_dlg.left)) / 2,
                 s.rect_main.top +
                     ((s.rect_main.bottom - s.rect_main.top) - (s.rect_dlg.bottom - s.rect_dlg.top)) / 2,
                 0, 0, (SWP_NOSIZE | SWP_NOZORDER));

    sprintf(s.path, "%s\\GAUN_Options.pic", global_duelart_path);
    global_gaun_options_pic = load_pic(s.path);
    return 0;
  }

  case WM_COMMAND:
  {
    if ((((unsigned int)wparam) & 0xffff) == RES_BUTTON_OK)
    {
      s.text_len = GetWindowTextA(GetDlgItem(hdlg, RES_ASKVALUE_EDITTEXT), s.edit_txt, 10);
      s.edit_txt[s.text_len] = '\0';
      global_dlg_result = atoi(s.edit_txt);
      EndDialog(hdlg, 1);
      if (global_gaun_options_pic != (HANDLE)0)
      {
        delete_and_close_object(global_gaun_options_pic);
      }
    }
    else if ((((unsigned int)wparam) & 0xffff) == RES_BUTTON_CANCEL)
    {
      if (global_gaun_options_pic != (HANDLE)0)
      {
        delete_and_close_object(global_gaun_options_pic);
      }
      global_dlg_result = 0;
      EndDialog(hdlg, 0);
    }
    return 1;
  }

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.hdc_copy = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc_copy);
    s.lparam_copy = lparam;
    SetBkMode(s.hdc_copy, TRANSPARENT);
    s.pv = GetStockObject(HOLLOW_BRUSH);
    return (INT_PTR)s.pv;

  case WM_ERASEBKGND:
    s.hdc_erase = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc_erase);
    GetClientRect(hdlg, &s.client_rect);
    if (global_gaun_options_pic != (HANDLE)0)
    {
      DrawBitmapToRect(s.hdc_erase, &s.client_rect, global_gaun_options_pic);
    }
    else
    {
      FillRect(s.hdc_erase, &s.client_rect, (HBRUSH)GetStockObject(GRAY_BRUSH));
    }
    return 1;

  default:
    return 0;
  }
}

// FUNCTION: DECKDLL 0x10006960
static bool show_dialog_sellxcards(void)
{
  struct
  {
    int result;
    INT_PTR dlg_result;
  } s;

  load_text("menus", "NUMBERTOSELL");
  strcpy(global_ask_x_dlg_title, text_lines[0]);
  s.dlg_result = DialogBoxParam(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_ASK_VALUE), global_main_hwnd, dlgproc_AskX, 0);
  if (s.dlg_result == -1)
  {
    MessageBox(global_main_hwnd, "Couldn't bring up the filter dialog box", "", MB_OK);
    s.result = 0;
  }
  else if (s.dlg_result == 0)
  {
    s.result = 0;
  }
  else if (s.dlg_result == 1)
  {
    s.result = 1;
  }
  return s.result;
}

// FUNCTION: DECKDLL 0x10006a0d
static bool show_dialog_movexcards(void)
{
  struct
  {
    int result;
    INT_PTR dlg_result;
  } s;

  load_text("menus", "NUMBERTOMOVE");
  strcpy(global_ask_x_dlg_title, text_lines[0]);
  s.dlg_result = DialogBoxParam(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_ASK_VALUE), global_main_hwnd, dlgproc_AskX, 0);
  if (s.dlg_result == -1)
  {
    MessageBox(global_main_hwnd, "Couldn't bring up the filter dialog box", "", MB_OK);
    s.result = 0;
  }
  else if (s.dlg_result == 0)
  {
    s.result = 0;
  }
  else if (s.dlg_result == 1)
  {
    s.result = 1;
  }
  return s.result;
}

// FUNCTION: DECKDLL 0x1000774f
INT_PTR CALLBACK
dlgproc_InfoBox(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  /* Stack layout is extremely sensitive; keep locals grouped. */
  struct
  {
    int tmp;             /* [ebp-0x138] */
    HDC hdc_erase;       /* [ebp-0x134] */
    RECT r;              /* [ebp-0x130] */
    LPARAM lparam_copy;  /* [ebp-0x120] */
    HDC hdc_ctl;         /* [ebp-0x11c] */
    HGDIOBJ hobj;        /* [ebp-0x118] */
    char local_118[264]; /* [ebp-0x114] */
    int j;               /* [ebp-0x0c] */
    int idx;             /* [ebp-0x08] */
    int done;            /* [ebp-0x04] */
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    load_text("menus", "EXTRACARDSDIALOG");
    SetWindowTextA(hdlg, text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, RES_INFOBOX_DESCRIPTION), text_lines[1]);
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_OK), text_lines[2]);
    SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_CANCEL), text_lines[3]);

    s.done = 0;
    s.idx = s.done;
    while (s.done == 0)
    {
      if (((int *)global_excessive_cards)[s.idx * 3] != -1)
      {
        s.tmp = s.idx;
        s.idx++;
        SendDlgItemMessageA(hdlg, RES_INFOBOX_LIST, LB_ADDSTRING, 0, ((LPARAM *)global_excessive_cards)[s.tmp * 3 + 2]);
      }
      else
      {
        s.done = 1;
      }
    }

    sprintf(s.local_118, "%s\\GAUN_Results.pic", global_duelart_path);
    global_gaun_results_pic = load_pic(s.local_118);
    return 0;

  case WM_COMMAND:
    if ((((unsigned int)wparam) & 0xffff) == RES_BUTTON_OK)
    {
      s.done = 0;
      s.idx = s.done;
      while (s.done == 0)
      {
        if (global_excessive_cards[s.idx].DeckEntry_csvid != -1)
        {
          for (s.j = 0; s.j < global_deck_num_entries; ++s.j)
          {
            if ((global_deck[s.j].GDE_csvid == global_excessive_cards[s.idx].DeckEntry_csvid) &&
                (global_deck[s.j].GDE_Available == 0) &&
                (0 < global_excessive_cards[s.idx].DeckEntry_Amount))
            {
              global_deck[s.j].GDE_Available = 1;
              global_deck[s.j].GDE_DecksBits &= ~(1 << ((unsigned char)global_current_deck));
              global_excessive_cards[s.idx].DeckEntry_Amount--;
            }
          }
          ++s.idx;
        }
        else
        {
          s.done = 1;
        }
      }

      EndDialog(hdlg, 1);
      if (global_gaun_results_pic != (HANDLE)0)
        delete_and_close_object(global_gaun_results_pic);
    }
    else if ((((unsigned int)wparam) & 0xffff) == RES_BUTTON_CANCEL)
    {
      if (global_gaun_results_pic != (HANDLE)0)
        delete_and_close_object(global_gaun_results_pic);
      EndDialog(hdlg, 0);
    }
    return 1;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.hdc_ctl = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc_ctl);
    s.lparam_copy = lparam;
    SetBkMode(s.hdc_ctl, TRANSPARENT);
    s.hobj = GetStockObject(HOLLOW_BRUSH);
    return (INT_PTR)s.hobj;

  case WM_ERASEBKGND:
    s.hdc_erase = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc_erase);
    GetClientRect(hdlg, &s.r);

    if (global_gaun_results_pic != (HANDLE)0)
    {
      DrawBitmapToRect(s.hdc_erase, &s.r, global_gaun_results_pic);
    }
    else
    {
      FillRect(s.hdc_erase, &s.r, (HBRUSH)GetStockObject(GRAY_BRUSH));
    }
    return 1;

  default:
    return 0;
  }
}

// FUNCTION: DECKDLL 0x100076c3
static bool show_dialog_infobox(void)
{
  struct
  {
    int result;
    INT_PTR dlg_result;
  } s;

  s.dlg_result = DialogBoxParam(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_INFOBOX), global_main_hwnd, dlgproc_InfoBox, 0);
  if (s.dlg_result == -1)
  {
    MessageBox(global_main_hwnd, "Couldn't bring up the title dialog box", "", 0);
    s.result = 0;
  }
  else if (s.dlg_result == 0)
  {
    s.result = 0;
  }
  else if (s.dlg_result == 1)
  {
    s.result = 1;
  }
  return s.result;
}

// FUNCTION: DECKDLL 0x10019fe9
INT_PTR CALLBACK
dlgproc_LoadDeck(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  /* Stack layout is extremely sensitive; keep locals grouped. */
  struct
  {
    int pad;
    RECT client_rect;        /* [ebp-0x53c] */
    LPARAM lparam_copy;      /* [ebp-0x530] */
    HDC hdc_copy;            /* [ebp-0x52c] */
    int pad_500;             /* [ebp-0x524] */
    WPARAM sel;              /* [ebp-0x520] */
    char filename[32];       /* [ebp-0x51c] */
    char path[0x104];        /* [ebp-0x4fc] */
    int pad_3f8;             /* [ebp-0x3f8] */
    char listbox_buf[0x104]; /* [ebp-0x3f4] */
    int pad_2f0;             /* [ebp-0x2f0] */
    HWND hwnd_list;          /* [ebp-0x2ec] */
    int count;               /* [ebp-0x2e8] */
    char deckname[200];      /* [ebp-0x2e4] */
    char prev_dir[0x104];    /* [ebp-0x21c] */
    int pad_118;             /* [ebp-0x118] */
    WPARAM i;                /* [ebp-0x114] */
    FILE *fp;                /* [ebp-0x110] */
    char *p;                 /* [ebp-0x10c] */
    char gaun_path[0x100];   /* [ebp-0x108] */
    char pad_end[8];         /* [ebp-0x008] */
  } s;

  {
    switch (msg)
    {
    case WM_INITDIALOG:
      load_text("menus", "LOADDECKDIALOG");
      SetWindowTextA(hdlg, text_lines[0]);
      SetWindowTextA(GetDlgItem(hdlg, RES_LOADDECK_PLAYERDECK), text_lines[1]);

      load_text("menus", "OKCANCEL");
      SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_OK), text_lines[0]);
      SetWindowTextA(GetDlgItem(hdlg, RES_BUTTON_CANCEL), text_lines[1]);

      s.hwnd_list = CreateWindowExA(0, "LISTBOX", "",
                                    (WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | LBS_SORT),
                                    0, 0, 0, 0,
                                    hdlg, (HMENU)0, global_hinstance, (LPVOID)0);
      if (s.hwnd_list == (HWND)0)
      {
        EndDialog(hdlg, -1);
        return 1;
      }

      GetCurrentDirectoryA(0x104, s.prev_dir);
      _chdir(global_base_directory);

      strcpy(s.listbox_buf, "playdeck\\*.DCK");
      SendMessageA(s.hwnd_list, LB_DIR, 0, (LPARAM)s.listbox_buf);
      s.count = SendMessageA(s.hwnd_list, LB_GETCOUNT, 0, 0);

      for (s.i = 0; (int)s.i < s.count; s.i = s.i + 1)
      {
        SendMessageA(s.hwnd_list, LB_GETTEXT, s.i, (LPARAM)s.listbox_buf);
        strcpy(s.path, "playdeck\\");
        strcat(s.path, s.listbox_buf);

        s.fp = fopen(s.path, "r");
        if (s.fp != (FILE *)0)
        {
          if (fgetc(s.fp) != ';')
          {
            fclose(s.fp);
            continue;
          }
          s.deckname[0] = '\0';
          s.p = (char *)&s.deckname + strlen(s.deckname);
          while ((*s.p = (char)fgetc(s.fp)) != '\n')
          {
            s.p = s.p + 1;
          }
          *s.p = '\0';
          fclose(s.fp);

          strcpy(s.path, "playdeck\\");
          strcat(s.path, s.deckname);
          strcat(s.path, ".dck");
          s.fp = fopen(s.path, "r");
          if (s.fp != (FILE *)0)
          {
            SendDlgItemMessageA(hdlg, RES_LOADDECK_DECKLIST, CB_ADDSTRING, 0, (LPARAM)s.deckname);
            fclose(s.fp);
          }
        }
      }

      SendDlgItemMessageA(hdlg, RES_LOADDECK_DECKLIST, CB_SETCURSEL, 0, 0);
      _chdir(s.prev_dir);

      sprintf(s.gaun_path, "%s\\GAUN_Results.pic", global_duelart_path);
      global_loaddeck_pic = load_pic(s.gaun_path);
      return 0;

    case WM_COMMAND:
      if ((wparam & 0xFFFF) == RES_BUTTON_OK)
      {
        s.sel = SendDlgItemMessageA(hdlg, RES_LOADDECK_DECKLIST, CB_GETCURSEL, 0, 0);
        SendDlgItemMessageA(hdlg, RES_LOADDECK_DECKLIST, CB_GETLBTEXT, s.sel, (LPARAM)s.filename);
        sprintf(global_loaddeck_dlg_filename, "%s\\%s.dck", global_playdeck_path, s.filename);
        if (global_loaddeck_pic != (HANDLE)0)
        {
          delete_and_close_object(global_loaddeck_pic);
        }
        EndDialog(hdlg, 1);
      }
      else if ((wparam & 0xFFFF) == RES_BUTTON_CANCEL)
      {
        if (global_loaddeck_pic != (HANDLE)0)
        {
          delete_and_close_object(global_loaddeck_pic);
        }
        EndDialog(hdlg, 0);
      }
      return 1;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC:
      s.hdc_copy = (HDC)wparam;
      ApplyCardArtPaletteToDc(s.hdc_copy);
      s.lparam_copy = lparam;
      SetBkMode(s.hdc_copy, TRANSPARENT);
      return (INT_PTR)(s.pad_500 = (int)GetStockObject(HOLLOW_BRUSH));

    case WM_ERASEBKGND:
      s.pad = (HDC)wparam;
      ApplyCardArtPaletteToDc(s.pad);
      GetClientRect(hdlg, &s.client_rect);
      if (global_loaddeck_pic != (HANDLE)0)
      {
        DrawBitmapToRect(s.pad, &s.client_rect, global_loaddeck_pic);
      }
      else
      {
        FillRect(s.pad, &s.client_rect, (HBRUSH)GetStockObject(2));
      }
      return 1;

    default:
      return 0;
    }
  }
}

// FUNCTION: DECKDLL 0x10019f4c
static bool show_dialog_loaddeck(char *dest)
{
  struct
  {
    int ret;
    INT_PTR dlg_result;
  } s;

  s.dlg_result = DialogBoxParam(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_LOADDECK), global_main_hwnd,
                                dlgproc_LoadDeck, 0);
  if (s.dlg_result == -1)
  {
    MessageBox(global_main_hwnd, "Couldn't bring up the Load Deck dialog", "", MB_OK);
    s.ret = 0;
  }
  else if (s.dlg_result == 0)
  {
    s.ret = 0;
  }
  else if (s.dlg_result == 1)
  {
    strcpy(dest, global_loaddeck_dlg_filename);
    s.ret = 1;
  }

  return s.ret;
}

// FUNCTION: DECKDLL 0x1000679f
void clear_packs(void)
{
  int i;

  for (i = 0; i < 6; ++i)
  {
    global_packs[i][0].num = 0;
    global_packs[i][1].num = 0;
    global_packs[i][2].num = 0;
    global_packs[i][3].num = 0;
    global_packs[i][4].num = 0;
    global_packs[i][5].num = 0;
  }
}

// FUNCTION: DECKDLL 0x1000687f
void clear_packs_copy(void)
{
  int i;

  for (i = 0; i < 6; ++i)
  {
    global_packs_copy[i][0].num = 0;
    global_packs_copy[i][1].num = 0;
    global_packs_copy[i][2].num = 0;
    global_packs_copy[i][3].num = 0;
    global_packs_copy[i][4].num = 0;
    global_packs_copy[i][5].num = 0;
  }
}

// FUNCTION: DECKDLL 0x10005a9e
void count_packs(void)
{
  struct
  {
    int amt;
    int l;
    unsigned int csvid;
  } s;
#define STORE_PACK(p1, p2)                                                                                                  \
  do                                                                                                                        \
  {                                                                                                                         \
    *(unsigned int *)&global_packs[(p1)][(p2)].table[global_packs[(p1)][(p2)].num] = ((unsigned int)s.amt << 16) | s.csvid; \
    ++global_packs[(p1)][(p2)].num;                                                                                         \
  } while (0)

  for (s.l = 0; s.l < 6; ++s.l)
  {
    global_packs[s.l][0].num = 0;
    global_packs[s.l][1].num = 0;
    global_packs[s.l][2].num = 0;
    global_packs[s.l][3].num = 0;
    global_packs[s.l][4].num = 0;
    global_packs[s.l][5].num = 0;
  }

  for (s.l = 0; s.l < global_edited_deck_num_entries; ++s.l)
  {
    s.csvid = global_edited_deck.entries[s.l].DeckEntry_csvid;
    s.amt = global_edited_deck.entries[s.l].DeckEntry_Amount;

    if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_LAND)
    {
      if (strcmp(global_raw_cards_storage[s.csvid].full_name, "Mountain") == 0)
        STORE_PACK(PACK1_RED, PACK2_LAND);
      else if (strcmp(global_raw_cards_storage[s.csvid].full_name, "Plains") == 0)
        STORE_PACK(PACK1_WHITE, PACK2_LAND);
      else if (strcmp(global_raw_cards_storage[s.csvid].full_name, "Forest") == 0)
        STORE_PACK(PACK1_GREEN, PACK2_LAND);
      else if (strcmp(global_raw_cards_storage[s.csvid].full_name, "Swamp") == 0)
        STORE_PACK(PACK1_BLACK, PACK2_LAND);
      else if (strcmp(global_raw_cards_storage[s.csvid].full_name, "Island") == 0)
        STORE_PACK(PACK1_BLUE, PACK2_LAND);
      else
        STORE_PACK(PACK1_OTHER, PACK2_LAND);
    }
    else if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_CREATURE)
    {
      if (global_raw_cards_storage[s.csvid].color == CP_COLOR_RED)
        STORE_PACK(PACK1_RED, PACK2_CREATURE);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLACK)
        STORE_PACK(PACK1_BLACK, PACK2_CREATURE);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLUE)
        STORE_PACK(PACK1_BLUE, PACK2_CREATURE);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_GREEN)
        STORE_PACK(PACK1_GREEN, PACK2_CREATURE);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_WHITE)
        STORE_PACK(PACK1_WHITE, PACK2_CREATURE);
      else
        STORE_PACK(PACK1_OTHER, PACK2_CREATURE);
    }
    else if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_ENCHANTMENT)
    {
      if (global_raw_cards_storage[s.csvid].color == CP_COLOR_RED)
        STORE_PACK(PACK1_RED, PACK2_ENCHANTMENT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLACK)
        STORE_PACK(PACK1_BLACK, PACK2_ENCHANTMENT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLUE)
        STORE_PACK(PACK1_BLUE, PACK2_ENCHANTMENT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_GREEN)
        STORE_PACK(PACK1_GREEN, PACK2_ENCHANTMENT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_WHITE)
        STORE_PACK(PACK1_WHITE, PACK2_ENCHANTMENT);
      else
        STORE_PACK(PACK1_OTHER, PACK2_ENCHANTMENT);
    }
    else if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_SORCERY)
    {
      if (global_raw_cards_storage[s.csvid].color == CP_COLOR_RED)
        STORE_PACK(PACK1_RED, PACK2_SORCERY);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLACK)
        STORE_PACK(PACK1_BLACK, PACK2_SORCERY);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLUE)
        STORE_PACK(PACK1_BLUE, PACK2_SORCERY);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_GREEN)
        STORE_PACK(PACK1_GREEN, PACK2_SORCERY);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_WHITE)
        STORE_PACK(PACK1_WHITE, PACK2_SORCERY);
      else
        STORE_PACK(PACK1_OTHER, PACK2_SORCERY);
    }
    else if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_INTERRUPT)
    {
      if (global_raw_cards_storage[s.csvid].color == CP_COLOR_RED)
        STORE_PACK(PACK1_RED, PACK2_INTERRUPT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLACK)
        STORE_PACK(PACK1_BLACK, PACK2_INTERRUPT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLUE)
        STORE_PACK(PACK1_BLUE, PACK2_INTERRUPT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_GREEN)
        STORE_PACK(PACK1_GREEN, PACK2_INTERRUPT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_WHITE)
        STORE_PACK(PACK1_WHITE, PACK2_INTERRUPT);
      else
        STORE_PACK(PACK1_OTHER, PACK2_INTERRUPT);
    }
    else if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_INSTANT)
    {
      if (global_raw_cards_storage[s.csvid].color == CP_COLOR_RED)
        STORE_PACK(PACK1_RED, PACK2_INSTANT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLACK)
        STORE_PACK(PACK1_BLACK, PACK2_INSTANT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLUE)
        STORE_PACK(PACK1_BLUE, PACK2_INSTANT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_GREEN)
        STORE_PACK(PACK1_GREEN, PACK2_INSTANT);
      else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_WHITE)
        STORE_PACK(PACK1_WHITE, PACK2_INSTANT);
      else
        STORE_PACK(PACK1_OTHER, PACK2_INSTANT);
    }
    else
      STORE_PACK(PACK1_OTHER, PACK2_INSTANT);
  }
#undef STORE_PACK
}

// FUNCTION: DECKDLL 0x100213ee
LRESULT CALLBACK
wndproc_CueCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  // GLOBAL: DECKDLL 0x10030028
  static int DAT_10030028 = 0;

  struct
  {
    char txt[100]; /* matches [ebp-0x104] */
    HDC paintdc;
    PAINTSTRUCT paint;
    HBRUSH brush_inner;
    RECT rect_inner;
    HBRUSH brush_outer;
    RECT rect_client;
    int height;
    HDC hdc_local;
    int width;
    UINT y;
    UINT x;
    LPCSTR str;
    SIZE sz;
    HFONT font;
    RECT rect_parent;
  } s;

  switch (msg)
  {
  case 0x400:
    s.font = (HFONT)GetWindowLongA(hwnd, DAT_10030028);

    s.x = (UINT)wparam & 0xFFFF;
    s.y = ((UINT)wparam >> 16) & 0xFFFF;
    s.str = (LPCSTR)lparam;

    s.hdc_local = GetDC(hwnd);
    if (s.hdc_local != 0)
    {
      ApplyCardArtPaletteToDc(s.hdc_local);
      SelectObject(s.hdc_local, s.font);

      GetTextExtentPointA(s.hdc_local, s.str, lstrlenA(s.str), &s.sz);

      s.width = s.sz.cx + 10;
      s.height = s.sz.cy + 6;

      ReleaseDC(hwnd, s.hdc_local);

      GetClientRect(GetParent(hwnd), &s.rect_parent);
      if (s.rect_parent.right < s.width + (int)s.x)
        s.x = s.rect_parent.right - s.width;

      MoveWindow(hwnd, s.x, s.y, s.width, s.height, TRUE);
      SetWindowTextA(hwnd, s.str);
      ShowWindow(hwnd, SW_SHOW);
      InvalidateRect(hwnd, NULL, TRUE);
    }
    return 0;

  case WM_CREATE:
    s.font = CreateFontA(12, 0, 0, 0, 400, 0, 0, 0, 0, 4, 0, 0, 0, "Arial");
    if (s.font == 0)
      return -1;
    SetWindowLongA(hwnd, DAT_10030028, (LONG)s.font);
    return 0;

  case WM_DESTROY:

#ifdef MODERN_FIXES
    s.font = (HFONT)GetWindowLong(hwnd, DAT_10030028);
#else
    s.font = (HFONT)GetWindowWord(hwnd, DAT_10030028);
#endif

    if (s.font != 0)
      DeleteObject(s.font);
    return 0;

  case WM_KEYDOWN:
    SetFocus(global_main_hwnd);
    PostMessageA(GetFocus(), msg, wparam, lparam);
    return 0;

  case WM_PAINT:

#ifdef MODERN_FIXES
    s.font = (HFONT)GetWindowLong(hwnd, DAT_10030028);
#else
    s.font = (HFONT)GetWindowWord(hwnd, DAT_10030028);
#endif

    s.paintdc = BeginPaint(hwnd, &s.paint);
    if (s.paintdc != 0)
    {

      ApplyCardArtPaletteToDc(s.paintdc);

      GetClientRect(hwnd, &s.rect_client);

      s.brush_inner = CreateSolidBrush(PALETTERGB(210, 190, 150));
      s.brush_outer = CreateSolidBrush(PALETTERGB(127, 127, 127));

      SetRect(&s.rect_inner,
              s.rect_client.left,
              s.rect_client.top,
              s.rect_client.right - 2,
              s.rect_client.bottom - 2);
      FillRect(s.paintdc, &s.rect_client, s.brush_outer);
      FillRect(s.paintdc, &s.rect_inner, s.brush_inner);

      SetTextColor(s.paintdc, PALETTERGB(0, 0, 0));
      SetBkMode(s.paintdc, TRANSPARENT);
      GetWindowTextA(hwnd, s.txt, 100);
      SelectObject(s.paintdc, s.font);
      DrawTextA(s.paintdc,
                s.txt,
                -1,
                &s.rect_inner,
                DT_SINGLELINE | DT_VCENTER | DT_CENTER);

      EndPaint(hwnd, &s.paint);

      DeleteObject(s.brush_inner);
      DeleteObject(s.brush_outer);
    }
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: DECKDLL 0x1002764d
LRESULT CALLBACK
wndproc_TitleClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HDC chdc; // ebp - 0x80
    HDC hdc;  // ebp - 0x7c
    BITMAP bmp;
    PAINTSTRUCT paint;
    RECT rect2; // ebp - 0x20
    RECT rect;  // ebp - 0x10
  } s;

  // TitleClass fonts.
  // GLOBAL: DECKDLL 0x10105280
  static LOGFONT DAT_10105280;
  // GLOBAL: DECKDLL 0x10105278
  static HFONT DAT_10105278;
  // GLOBAL: DECKDLL 0x101052bc
  static HFONT DAT_101052bc;
  // GLOBAL: DECKDLL 0x100362d8
  static LOGFONT titleClass_fontTemplate = {
      0,        // lfHeight
      0,        // lfWidth
      0,        // lfEscapement
      0,        // lfOrientation
      FW_NORMAL // lfWeight
  };

  switch (msg)
  {
  case WM_CREATE:
    memcpy(&DAT_10105280, &titleClass_fontTemplate, 0x3c);
    strcpy(DAT_10105280.lfFaceName, "Cheltenham ITC Bold BT");
    DAT_10105280.lfHeight = 0x32;
    DAT_10105278 = CreateFontIndirect(&DAT_10105280);
    DAT_10105280.lfHeight = 0x1e;
    DAT_101052bc = CreateFontIndirect(&DAT_10105280);
    return 0;

  case WM_DESTROY:
    DeleteObject(DAT_10105278);
    DeleteObject(DAT_101052bc);
    return 0;

  case WM_ERASEBKGND:
  {
    s.hdc = wparam;
    ApplyCardArtPaletteToDc(s.hdc);

    s.chdc = CreateCompatibleDC(s.hdc);
    ApplyCardArtPaletteToDc(s.chdc);

    SelectObject(s.chdc, global_pic_dektit1);

    GetClientRect(hwnd, &s.rect);

    GetObject(global_pic_dektit1, sizeof(BITMAP), &s.bmp);

    StretchBlt(s.hdc, 0, 0, s.rect.right, s.rect.bottom, s.chdc, 0, 0, s.bmp.bmWidth, s.bmp.bmHeight, SRCCOPY);

    DeleteDC(s.chdc);
    return 1;
  }

  case WM_PAINT:
    s.hdc = BeginPaint(hwnd, &s.paint);
    ApplyCardArtPaletteToDc(s.hdc);
    GetClientRect(hwnd, &s.rect);
    InflateRect(&s.rect, -5, -5);
    CopyRect(&s.rect2, &s.rect);

    if (strlen(global_deckinfo.deckname) < 12)
      SelectObject(s.hdc, DAT_10105278);
    else
      SelectObject(s.hdc, DAT_101052bc);

    SetBkMode(s.hdc, TRANSPARENT);
    SetMapMode(s.hdc, MM_ANISOTROPIC);

    SetWindowExtEx(s.hdc, s.rect2.right - s.rect2.left, 0x46, NULL);
    SetViewportExtEx(s.hdc, s.rect2.right - s.rect2.left, s.rect2.bottom - s.rect2.top, NULL);

    s.rect2.left = s.rect2.left + 3;
    s.rect2.top = s.rect2.top + 3;
    SetTextColor(s.hdc, PALETTERGB(20, 46, 77));
    DrawTextA(s.hdc, global_deckinfo.deckname, strlen(global_deckinfo.deckname), &s.rect2, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

    s.rect2.left = s.rect2.left + -3;
    s.rect2.top = s.rect2.top + -3;
    SetTextColor(s.hdc, PALETTERGB(243, 209, 175));
    DrawTextA(s.hdc, global_deckinfo.deckname, strlen(global_deckinfo.deckname), &s.rect2, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

    EndPaint(hwnd, &s.paint);
    return 0;

  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
    if (global_db_flags_1 & (DBFLAGS_EDITDECK | DBFLAGS_SHANDALAR))
      return 0;

    show_dialog_deckinfo();
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: DECKDLL 0x10010f75
LRESULT CALLBACK wndproc_FullCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  // GLOBAL: DECKDLL 0x10030010
  static int FULLCARD_CSVID_INDEX = 4;
  // GLOBAL: DECKDLL 0x10030014
  static int FULLCARD_EXPANDTEXT_INDEX = 8;

  int card_to_draw;
  uint expanded_text;
  int wanted_card;
  switch (msg)
  {

  case 0x466:
    wanted_card = wparam;
    card_to_draw = GetWindowLongA(hwnd, FULLCARD_CSVID_INDEX);
    if (wanted_card == card_to_draw)
      InvalidateRect(hwnd, NULL, 0);
    return 0;

  case 0x400:
    card_to_draw = wparam;
    if (GetWindowLongA(hwnd, FULLCARD_CSVID_INDEX) != card_to_draw)
    {
      if (card_to_draw < 0 && global_edited_deck.entries[0].DeckEntry_Amount > 0)
        card_to_draw = global_edited_deck.entries[0].DeckEntry_csvid;
      SetWindowLongA(hwnd, FULLCARD_CSVID_INDEX, card_to_draw);
      InvalidateRect(hwnd, NULL, 0);
    }
    return 0;

  case 0x402:
    expanded_text = (WORD)GetWindowWord(hwnd, FULLCARD_EXPANDTEXT_INDEX);
    return expanded_text;

  case 0x401:
    expanded_text = wparam;
    SetWindowWord(hwnd, FULLCARD_EXPANDTEXT_INDEX, (WORD)expanded_text);
    InvalidateRect(hwnd, NULL, 0);
    CheckMenuItem(global_fullcard_popup, RES_FULLCARDMENU_EXPAND,
                  (expanded_text >= 1) ? MF_CHECKED : 0);
    return expanded_text;

  case WM_COMMAND:
    if ((wparam & 0xffff) == RES_FULLCARDMENU_EXPAND)
    {
      if (!SendMessageA(hwnd, 0x402, 0, 0))
        SendMessageA(hwnd, 0x401, 1, 0);
      else
        SendMessageA(hwnd, 0x401, 0, 0);
    }
    return 0;

  case WM_CREATE:
    global_fullcard_popup = CreatePopupMenu();
    load_text("menus", "FULLCARD");
    AppendMenuA(global_fullcard_popup, 0, RES_FULLCARDMENU_EXPAND, text_lines[0]);
    card_to_draw = -1;
    SetWindowLongA(hwnd, FULLCARD_CSVID_INDEX, card_to_draw);
    expanded_text = global_cfg.expand_text;
    SetWindowWord(hwnd, FULLCARD_EXPANDTEXT_INDEX, (WORD)expanded_text);
    CheckMenuItem(global_fullcard_popup, RES_FULLCARDMENU_EXPAND,
                  (expanded_text >= 1) ? MF_CHECKED : 0);
    return 0;

  case WM_DESTROY:
    global_cfg.expand_text = (char)GetWindowWord(hwnd, FULLCARD_EXPANDTEXT_INDEX);
    DestroyMenu(global_fullcard_popup);
    return 0;

  case WM_PAINT:
  {
    struct
    {
      HDC hdc;           // ebp - 0x60
      PAINTSTRUCT paint; // ebp - 0x5c
      RECT rect;         // ebp - 0x1c
    } s;

    card_to_draw = GetWindowLong(hwnd, FULLCARD_CSVID_INDEX);
    expanded_text = GetWindowWord(hwnd, FULLCARD_EXPANDTEXT_INDEX);

    s.hdc = BeginPaint(hwnd, &s.paint);

    GetClientRect(hwnd, &s.rect);

    ApplyCardArtPaletteToDc(s.hdc);

    if (card_to_draw < 0)
    {
      DrawCardBack(s.hdc, &s.rect);
    }
    else
    {
      uint cardid;
      FillRect(global_hdc, &s.rect, global_create_brush_0);
      cardid = DrawFullCard(global_hdc, &s.rect, &global_raw_cards_storage[card_to_draw], 0, 1, expanded_text, global_artistline);
      BitBlt(s.hdc, 0, 0, s.rect.right, s.rect.bottom, global_hdc, 0, 0, SRCCOPY);
    }

    EndPaint(hwnd, &s.paint);
    return 0;
  }

  case WM_RBUTTONDOWN:
  {
    POINT pt;
    pt.x = (int)(short)LOWORD(lparam);
    pt.y = (int)(short)HIWORD(lparam);
    ClientToScreen(hwnd, &pt);
    TrackPopupMenu(global_fullcard_popup, 2, pt.x, pt.y, 0, hwnd, NULL);
    return 0;
  }

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: DECKDLL 0x1002ab15
void filter_cards_in_lists(HWND hwnd_listbox, HWND hwnd_horzlist)
{
  struct
  {
    int newsel;           // ebp - 0x1c
    int idx;              // ebp - 0x18
    int pad;              // ebp - 0x14
    int data;             // ebp - 0x10
    int count;            // ebp - 0xc
    const card_ptr_t *cp; // ebp - 0x8
    csvid_t csvid;        // ebp - 0x4
  } s;

  s.pad = 0;
  s.idx = SendMessage(hwnd_listbox, LB_GETCURSEL, 0, 0);
  s.data = SendMessage(hwnd_listbox, LB_GETITEMDATA, s.idx, 0);

  SendMessage(hwnd_listbox, LB_RESETCONTENT, 0, 0);
  SendMessage(hwnd_horzlist, LB_RESETCONTENT, 0, 0);

  UpdateWindow(hwnd_listbox);
  UpdateWindow(hwnd_horzlist);

  s.newsel = -1;
  s.csvid = 0;
  s.cp = &global_raw_cards_storage[0];
  for (; s.csvid < global_available_slots; ++s.csvid, ++s.cp)
    if (check_filters(s.csvid))
    {
      s.idx = SendMessage(hwnd_listbox, LB_ADDSTRING, 0, s.cp->full_name);
      SendMessage(hwnd_listbox, LB_SETITEMDATA, s.idx, s.csvid);
      if (s.csvid == s.data)
        s.newsel = s.idx;
    }

  if (s.newsel == -1)
    s.newsel = 0;

  SendMessage(hwnd_listbox, LB_SETCURSEL, s.newsel, 0);

  SendMessage(GetParent(hwnd_listbox), WM_COMMAND, MAKELONG2(GetDlgCtrlID(hwnd_listbox), 1), hwnd_listbox);
  UpdateWindow(hwnd_listbox);

  s.count = SendMessage(hwnd_listbox, LB_GETCOUNT, 0, 0);
  for (s.idx = 0; s.idx < s.count; ++s.idx)
  {
    s.csvid = SendMessage(hwnd_listbox, LB_GETITEMDATA, s.idx, 0);
    SendMessage(hwnd_horzlist, LB_ADDSTRING, 0, s.csvid);
  }

  SendMessage(hwnd_horzlist, LB_SETCURSEL, 0, 0);

  SendMessage(GetParent(hwnd_horzlist), WM_COMMAND, MAKELONG2(GetDlgCtrlID(hwnd_horzlist), 1), hwnd_horzlist);
}

// FUNCTION: DECKDLL 0x1002b8f0
void insert_cards_into_deck(csvid_t csvid, int num, FullDeck *tgt_deck)
{
  struct
  {
    int i;     /* ebp - 0x8 */
    int found; /* ebp - 0x4 */
  } s;

  s.i = 0;
  s.found = 0;
  for (; s.i < tgt_deck->total && s.found == 0; s.i++)
  {
    if (tgt_deck->entries[s.i].DeckEntry_csvid == csvid)
    {
      tgt_deck->entries[s.i].DeckEntry_Amount += num;
      tgt_deck->cards += num;
      s.found = 1;
    }
  }

  if (s.found == 0)
  {
    tgt_deck->entries[tgt_deck->total].DeckEntry_csvid = csvid;
    tgt_deck->entries[tgt_deck->total].DeckEntry_Amount = num;
    tgt_deck->entries[tgt_deck->total].DeckEntry_FullName = global_raw_cards_storage[csvid].full_name;
    tgt_deck->total += 1;
    tgt_deck->cards += num;
  }
}

// FUNCTION: DECKDLL 0x1002b9dc
void add_or_increment_sideboard_bucket_entry(int which, csvid_t csvid, int num, FullDeck *tgt_deck)
{
  struct
  {
    DeckBucket *bucket;
    int i;
    int *count_ptr;
    int found;
  } s;

  if (which == 0)
  {
    s.bucket = &tgt_deck->sideboard[0];
    s.count_ptr = &tgt_deck->sideboard[0].total;
  }
  else if (which == 1)
  {
    s.bucket = &tgt_deck->sideboard[1];
    s.count_ptr = &tgt_deck->sideboard[1].total;
  }
  else if (which == 2)
  {
    s.bucket = &tgt_deck->sideboard[2];
    s.count_ptr = &tgt_deck->sideboard[2].total;
  }
  else if (which == 5)
  {
    s.bucket = &tgt_deck->sideboard[5];
    s.count_ptr = &tgt_deck->sideboard[5].total;
  }
  else if (which == 4)
  {
    s.bucket = &tgt_deck->sideboard[4];
    s.count_ptr = &tgt_deck->sideboard[4].total;
  }
  else if (which == 3)
  {
    s.bucket = &tgt_deck->sideboard[3];
    s.count_ptr = &tgt_deck->sideboard[3].total;
  }
  else
  {

    return;
  }

  s.i = 0;
  s.found = 0;
  for (; (s.i < *s.count_ptr) && (!s.found); s.i++)
    if (s.bucket->entries[s.i].DeckEntry_csvid == csvid)
    {
      s.bucket->entries[s.i].DeckEntry_Amount += num;
      s.found = 1;
    }

  if (!s.found)
  {
    s.bucket->entries[*s.count_ptr].DeckEntry_csvid = csvid;
    s.bucket->entries[*s.count_ptr].DeckEntry_Amount = num;
    s.bucket->entries[*s.count_ptr].DeckEntry_FullName = global_raw_cards_storage[csvid].full_name;
    *s.count_ptr = *s.count_ptr + 1;
  }
}

// FUNCTION: DECKDLL 0x1002bb7f
void append_trade_bucket_entry(int which, int csvid, int num, FullDeck *deck_base)
{
  struct
  {
    DeckBucket *bucket;
    int *count_ptr;
  } s;

  if (which == 0)
  {
    s.bucket = &deck_base->trade[1];
    s.count_ptr = &deck_base->trade[1].total;
  }
  else if (which == 1)
  {
    s.bucket = &deck_base->trade[2];
    s.count_ptr = &deck_base->trade[2].total;
  }
  else if (which == 2)
  {
    s.bucket = &deck_base->trade[3];
    s.count_ptr = &deck_base->trade[3].total;
  }
  else if (which == 5)
  {
    s.bucket = &deck_base->trade[6];
    s.count_ptr = &deck_base->trade[6].total;
  }
  else if (which == 4)
  {
    s.bucket = &deck_base->trade[5];
    s.count_ptr = &deck_base->trade[5].total;
  }
  else if (which == 3)
  {
    s.bucket = &deck_base->trade[4];
    s.count_ptr = &deck_base->trade[4].total;
  }

  else if (which == 6)
  {
    s.bucket = &deck_base->trade[0];
    s.count_ptr = &deck_base->trade[0].total;
  }
  else
  {
    return;
  }

  s.bucket->entries[*s.count_ptr].DeckEntry_csvid = csvid;
  s.bucket->entries[*s.count_ptr].DeckEntry_Amount = num;
  s.bucket->entries[*s.count_ptr].DeckEntry_FullName = global_raw_cards_storage[csvid].full_name;
  *s.count_ptr = *s.count_ptr + 1;
}

// FUNCTION: DECKDLL 0x1002bcde
static void remove_cards_from_deck(csvid_t csvid, int num, FullDeck *tgt_deck)
{
  struct
  {
    int j;     /* ebp - 0xc */
    int i;     /* ebp - 0x8 */
    int found; /* ebp - 0x4 */
  } s;

  s.i = 0;
  s.found = 0;
  for (; s.i < tgt_deck->total && s.found == 0; s.i++)
  {
    if (tgt_deck->entries[s.i].DeckEntry_csvid == csvid)
    {
      s.found = 1;
      if (tgt_deck->entries[s.i].DeckEntry_Amount == num)
      {
        tgt_deck->cards -= num;
        for (s.j = s.i; s.j < tgt_deck->total - 1; s.j++)
        {
          tgt_deck->entries[s.j].DeckEntry_csvid = tgt_deck->entries[s.j + 1].DeckEntry_csvid;
          tgt_deck->entries[s.j].DeckEntry_Amount = tgt_deck->entries[s.j + 1].DeckEntry_Amount;
          tgt_deck->entries[s.j].DeckEntry_FullName = tgt_deck->entries[s.j + 1].DeckEntry_FullName;
        }
        --tgt_deck->total;
      }
      else if (num < tgt_deck->entries[s.i].DeckEntry_Amount)
      {
        tgt_deck->entries[s.i].DeckEntry_Amount -= num;
        tgt_deck->cards -= num;
      }
      else
      {
        tgt_deck->cards -= num;
        num -= tgt_deck->entries[s.i].DeckEntry_Amount;
        s.found = 0;
        for (s.j = s.i; s.j < tgt_deck->total - 1; s.j++)
        {
          tgt_deck->entries[s.j].DeckEntry_csvid = tgt_deck->entries[s.j + 1].DeckEntry_csvid;
          tgt_deck->entries[s.j].DeckEntry_Amount = tgt_deck->entries[s.j + 1].DeckEntry_Amount;
          tgt_deck->entries[s.j].DeckEntry_FullName = tgt_deck->entries[s.j + 1].DeckEntry_FullName;
        }
        --tgt_deck->total;
      }
    }
  }
}

// FUNCTION: DECKDLL 0x1002bed0
static void remove_or_decrement_sideboard_bucket_entry(int which, int csvid, int num, FullDeck *deck_base)
{
  struct
  {
    DeckBucket *bucket; /* ebp - 0x14 */
    int idx;            /* ebp - 0x10 */
    int i;              /* ebp - 0xc */
    int *count_ptr;     /* ebp - 0x8 */
    int found;          /* ebp - 0x4 */
  } s;

  if (which == 0)
  {
    s.bucket = &deck_base->sideboard[0];
    s.count_ptr = &deck_base->sideboard[0].total;
  }
  else if (which == 1)
  {
    s.bucket = &deck_base->sideboard[1];
    s.count_ptr = &deck_base->sideboard[1].total;
  }
  else if (which == 2)
  {
    s.bucket = &deck_base->sideboard[2];
    s.count_ptr = &deck_base->sideboard[2].total;
  }
  else if (which == 5)
  {
    s.bucket = &deck_base->sideboard[5];
    s.count_ptr = &deck_base->sideboard[5].total;
  }
  else if (which == 4)
  {
    s.bucket = &deck_base->sideboard[4];
    s.count_ptr = &deck_base->sideboard[4].total;
  }
  else if (which == 3)
  {
    s.bucket = &deck_base->sideboard[3];
    s.count_ptr = &deck_base->sideboard[3].total;
  }
  else
  {
    return;
  }

  s.i = 0;
  s.found = 0;
  for (; s.i < *s.count_ptr && !s.found; s.i++)
  {
    if (s.bucket->entries[s.i].DeckEntry_csvid == csvid)
    {
      s.found = 1;

      if (s.bucket->entries[s.i].DeckEntry_Amount == num)
      {
        for (s.idx = s.i; s.idx < *s.count_ptr - 1; s.idx++)
        {
          s.bucket->entries[s.idx].DeckEntry_csvid = s.bucket->entries[s.idx + 1].DeckEntry_csvid;
          s.bucket->entries[s.idx].DeckEntry_Amount = s.bucket->entries[s.idx + 1].DeckEntry_Amount;
          s.bucket->entries[s.idx].DeckEntry_FullName = s.bucket->entries[s.idx + 1].DeckEntry_FullName;
        }
        *s.count_ptr = *s.count_ptr - 1;
      }
      else if (num < s.bucket->entries[s.i].DeckEntry_Amount)
      {
        s.bucket->entries[s.i].DeckEntry_Amount -= num;
      }
      else
      {
        num -= s.bucket->entries[s.i].DeckEntry_Amount;
        s.found = 0;

        for (s.idx = s.i; s.idx < *s.count_ptr - 1; s.idx++)
        {
          s.bucket->entries[s.idx].DeckEntry_csvid = s.bucket->entries[s.idx + 1].DeckEntry_csvid;
          s.bucket->entries[s.idx].DeckEntry_Amount = s.bucket->entries[s.idx + 1].DeckEntry_Amount;
          s.bucket->entries[s.idx].DeckEntry_FullName = s.bucket->entries[s.idx + 1].DeckEntry_FullName;
        }
        *s.count_ptr = *s.count_ptr - 1;
      }
    }
  }
}

// FUNCTION: DECKDLL 0x1002c161
static void remove_trade_bucket_entries(int which, int csvid, int num, FullDeck *deck_base)
{
  struct
  {
    DeckBucket *bucket; /* ebp - 0x14 */
    int idx;            /* ebp - 0x10 */
    int i;              /* ebp - 0xc */
    int *count_ptr;     /* ebp - 0x8 */
    int done;           /* ebp - 0x4 */
  } s;

  if (which == 0)
  {
    s.bucket = &deck_base->trade[1];
    s.count_ptr = &deck_base->trade[1].total;
  }
  else if (which == 1)
  {
    s.bucket = &deck_base->trade[2];
    s.count_ptr = &deck_base->trade[2].total;
  }
  else if (which == 2)
  {
    s.bucket = &deck_base->trade[3];
    s.count_ptr = &deck_base->trade[3].total;
  }
  else if (which == 5)
  {
    s.bucket = &deck_base->trade[6];
    s.count_ptr = &deck_base->trade[6].total;
  }
  else if (which == 4)
  {
    s.bucket = &deck_base->trade[5];
    s.count_ptr = &deck_base->trade[5].total;
  }
  else if (which == 3)
  {
    s.bucket = &deck_base->trade[4];
    s.count_ptr = &deck_base->trade[4].total;
  }
  else if (which == 6)
  {
    s.bucket = &deck_base->trade[0];
    s.count_ptr = &deck_base->trade[0].total;
  }
  else
  {
    return;
  }

  s.i = 0;
  s.done = 0;
  for (; s.i < *s.count_ptr && !s.done; s.i++)
  {
    if (s.bucket->entries[s.i].DeckEntry_csvid == csvid)
    {
      for (s.idx = s.i; s.idx < *s.count_ptr - 1; s.idx++)
      {
        s.bucket->entries[s.idx].DeckEntry_csvid = s.bucket->entries[s.idx + 1].DeckEntry_csvid;
        s.bucket->entries[s.idx].DeckEntry_Amount = s.bucket->entries[s.idx + 1].DeckEntry_Amount;
        s.bucket->entries[s.idx].DeckEntry_FullName = s.bucket->entries[s.idx + 1].DeckEntry_FullName;
      }
      *s.count_ptr = *s.count_ptr - 1;

      num = num + -1;
      if (num <= 0)
        s.done = 1;
    }
  }
}

// FUNCTION: DECKDLL 0x100092c5
static int delete_card_from_global_deck(csvid_t csvid, int num)
{
  struct
  {
    int idx;   // ebp - 0xc
    int i;     // ebp - 0x8
    int found; // ebp - 0x4
  } s;

  s.i = 0;
  s.found = 0;
  for (; s.i < global_deck_num_entries && s.found == 0; s.i++)
    if (global_deck[s.i].GDE_csvid == csvid && global_deck[s.i].GDE_Available == num)
    {
      s.idx = s.i;
      s.found = 1;
      if (num == 0)
        remove_cards_from_deck(csvid, 1, &global_edited_deck);
    }

  if (!s.found)
    return 0;

  for (s.i = s.idx + 1; s.i < global_deck_num_entries; s.i++)
  {
    global_deck[s.i - 1] = global_deck[s.i];
  }
  global_deck_num_entries = global_deck_num_entries + -1;
  return 1;
}

// FUNCTION: DECKDLL 0x1002c6b5
void move_colors_fromto_global_deck(int move_into, unsigned int dlgbits)
{
  int i;
  if (move_into == 1)
  {
    for (i = 0; i < global_deck_num_entries; ++i)
    {
      if (global_deck[i].GDE_Available == 1)
      {
        if ((((dlgbits & 1) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].color == CP_COLOR_BLACK) ||
             ((dlgbits & 2) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].color == CP_COLOR_BLUE) ||
             ((dlgbits & 4) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].color == CP_COLOR_GREEN) ||
             ((dlgbits & 8) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].color == CP_COLOR_RED) ||
             ((dlgbits & 0x10) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].color == CP_COLOR_WHITE) ||
             ((dlgbits & 0x20) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].card_type == CP_TYPE_ARTIFACT)))
        {
          global_deck[i].GDE_Available = 0;
          global_deck[i].GDE_DecksBits |= 1 << global_current_deck;
          insert_cards_into_deck(global_deck[i].GDE_csvid, 1, &global_edited_deck);
        }
        else if ((((dlgbits & 1) != 0 && global_deck[i].GDE_iid == 0) ||   // swamp
                  ((dlgbits & 2) != 0 && global_deck[i].GDE_iid == 1) ||   // island
                  ((dlgbits & 4) != 0 && global_deck[i].GDE_iid == 2) ||   // forest
                  ((dlgbits & 8) != 0 && global_deck[i].GDE_iid == 3) ||   // mountain
                  ((dlgbits & 0x10) != 0 && global_deck[i].GDE_iid == 4))) // plains
        {
          global_deck[i].GDE_Available = 0;
          global_deck[i].GDE_DecksBits |= 1 << global_current_deck;
          insert_cards_into_deck(global_deck[i].GDE_csvid, 1, &global_edited_deck);
        }
      }
    }
  }
  else
  {
    for (i = 0; i < global_deck_num_entries; ++i)
    {
      if (global_deck[i].GDE_Available == 0)
      {
        if ((((dlgbits & 1) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].color == CP_COLOR_BLACK) ||
             ((dlgbits & 2) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].color == CP_COLOR_BLUE) ||
             ((dlgbits & 4) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].color == CP_COLOR_GREEN) ||
             ((dlgbits & 8) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].color == CP_COLOR_RED) ||
             ((dlgbits & 0x10) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].color == CP_COLOR_WHITE) ||
             ((dlgbits & 0x20) != 0 && global_raw_cards_storage[global_deck[i].GDE_csvid].card_type == CP_TYPE_ARTIFACT)))
        {
          global_deck[i].GDE_Available = 1;
          global_deck[i].GDE_DecksBits &= ~(1 << global_current_deck);
          remove_cards_from_deck(global_deck[i].GDE_csvid, 1, &global_edited_deck);
        }
        else
        {
          if ((((dlgbits & 1) != 0 && global_deck[i].GDE_iid == 0) ||   // swamp
               ((dlgbits & 2) != 0 && global_deck[i].GDE_iid == 1) ||   // island
               ((dlgbits & 4) != 0 && global_deck[i].GDE_iid == 2) ||   // forest
               ((dlgbits & 8) != 0 && global_deck[i].GDE_iid == 3) ||   // mountain
               ((dlgbits & 0x10) != 0 && global_deck[i].GDE_iid == 4))) // plains
          {
            global_deck[i].GDE_Available = 1;
            global_deck[i].GDE_DecksBits &= ~(1 << global_current_deck);
            remove_cards_from_deck(global_deck[i].GDE_csvid, 1, &global_edited_deck);
          }
        }
      }
    }
  }
}

// FUNCTION: DECKDLL 0x1002cb92
void set_smallcard_dimensions(void)
{
  if (global_cfg_consolidate)
  {
    if (global_edited_deck_num_entries < 75)
    {
      global_smallcard_width = global_smallcard_normal_width;
      global_smallcard_height = global_smallcard_normal_height;
    }
    else if (global_edited_deck_num_entries < 132)
    {
      global_smallcard_width = global_smallcard_smaller_width;
      global_smallcard_height = global_smallcard_smaller_height;
    }
    else
    {
      global_smallcard_width = global_smallcard_smallest_width;
      global_smallcard_height = global_smallcard_smallest_height;
    }
  }
  else
  {
    if (global_deck_num_cards < 75)
    {
      global_smallcard_width = global_smallcard_normal_width;
      global_smallcard_height = global_smallcard_normal_height;
    }
    else if (global_deck_num_cards < 132)
    {
      global_smallcard_width = global_smallcard_smaller_width;
      global_smallcard_height = global_smallcard_smaller_height;
    }
    else
    {
      global_smallcard_width = global_smallcard_smallest_width;
      global_smallcard_height = global_smallcard_smallest_height;
    }
  }
}

// FUNCTION: DECKDLL 0x1002c349
int check_colors_inout_edited_deck(bool currently_in)
{
  struct
  {
    int i;
    int dlgbits;
  } s;

  s.dlgbits = 0;
  if (currently_in == 1)
  {
    for (s.i = 0; s.i < global_deck_num_entries; ++s.i)
    {
      if (global_deck[s.i].GDE_Available == 0)
      {
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].color == CP_COLOR_BLACK)
          s.dlgbits |= 1;
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].color == CP_COLOR_BLUE)
          s.dlgbits |= 2;
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].color == CP_COLOR_GREEN)
          s.dlgbits |= 4;
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].color == CP_COLOR_RED)
          s.dlgbits |= 8;
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].color == CP_COLOR_WHITE)
          s.dlgbits |= 0x10;
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].card_type == CP_TYPE_ARTIFACT)
          s.dlgbits |= 0x20;
        if (global_deck[s.i].GDE_iid == 0)
          s.dlgbits |= 1;
        else if (global_deck[s.i].GDE_iid == 1)
          s.dlgbits |= 2;
        else if (global_deck[s.i].GDE_iid == 2)
          s.dlgbits |= 4;
        else if (global_deck[s.i].GDE_iid == 3)
          s.dlgbits |= 8;
        else if (global_deck[s.i].GDE_iid == 4)
          s.dlgbits |= 0x10;
      }
    }
  }
  else
  {
    for (s.i = 0; s.i < global_deck_num_entries; ++s.i)
    {
      if (global_deck[s.i].GDE_Available == 1)
      {
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].color == CP_COLOR_BLACK)
          s.dlgbits |= 1;
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].color == CP_COLOR_BLUE)
          s.dlgbits |= 2;
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].color == CP_COLOR_GREEN)
          s.dlgbits |= 4;
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].color == CP_COLOR_RED)
          s.dlgbits |= 8;
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].color == CP_COLOR_WHITE)
          s.dlgbits |= 0x10;
        if (global_raw_cards_storage[global_deck[s.i].GDE_csvid].card_type == CP_TYPE_ARTIFACT)
          s.dlgbits |= 0x20;
        if (global_deck[s.i].GDE_iid == 0)
          s.dlgbits |= 1;
        else if (global_deck[s.i].GDE_iid == 1)
          s.dlgbits |= 2;
        else if (global_deck[s.i].GDE_iid == 2)
          s.dlgbits |= 4;
        else if (global_deck[s.i].GDE_iid == 3)
          s.dlgbits |= 8;
        else if (global_deck[s.i].GDE_iid == 4)
          s.dlgbits |= 0x10;
      }
    }
  }

  return s.dlgbits;
}

// FUNCTION: DECKDLL 0x1002a9b5
static int
count_card_amount_outside_edited_deck(csvid_t csvid)
{
  int i;
  int count = 0;

  for (i = 0; i < global_deck_num_entries; ++i)
    if (global_deck[i].GDE_csvid == csvid && !(global_deck[i].GDE_DecksBits & (1 << (unsigned char)global_current_deck)))
      ++count;

  return count;
}

// FUNCTION: DECKDLL 0x100093cd
static iid_t
get_iid_from_global_deck_card(csvid_t csvid)
{
  int i;
  for (i = 0; i < global_deck_num_entries; ++i)
    if (global_deck[i].GDE_csvid == csvid)
      return global_deck[i].GDE_iid;
  return -1;
}

// FUNCTION: DECKDLL 0x10009187
static bool
change_global_deck_card_availability(csvid_t csvid, int mode)
{
  struct
  {
    int old_avail; /* ebp - 0xc */
    int new_avail; /* ebp - 0x8 */
    int i;         /* ebp - 0x4 */
  } s;
  if (mode == 0)
  {
    s.old_avail = 0;
    s.new_avail = 2;
  }
  else if (mode == 1)
  {
    s.old_avail = 1;
    s.new_avail = 2;
  }
  else if (mode == 2)
  {
    s.old_avail = 2;
    s.new_avail = 0;
  }
  else if (mode == 3)
  {
    s.old_avail = 2;
    s.new_avail = 1;
  }
  else
    return false;

  for (s.i = 0; global_deck_num_entries > s.i; ++s.i)
  {
    if (global_deck[s.i].GDE_csvid == csvid && global_deck[s.i].GDE_Available == s.old_avail)
    {
      global_deck[s.i].GDE_Available = s.new_avail;

      if (s.new_avail == 0)
        global_deck[s.i].GDE_DecksBits |= 1 << global_current_deck;
      if (s.new_avail == 1)
        global_deck[s.i].GDE_DecksBits &= ~(1 << global_current_deck);

      return true;
    }
  }

  return false;
}

// FUNCTION: DECKDLL 0x1002ad25
static void
TENTATIVE_remove_selected_from_horzlist(HWND hwnd_listbox, HWND hwnd_horzlist)
{
  struct
  {
    int cursel;
    int n;
    int count;
    int idk;
  } s;

  s.cursel = SendMessage(hwnd_listbox, LB_GETCURSEL, 0, 0);
  s.count = SendMessage(hwnd_listbox, LB_GETCOUNT, 0, 0);

  for (s.n = 0; s.n < s.count;)
    if (check_filters(s.idk = SendMessage(hwnd_listbox, LB_GETITEMDATA, s.n, 0)))
      ++s.n;
    else
    {
      SendMessage(hwnd_listbox, LB_DELETESTRING, s.n, 0);
      SendMessage(hwnd_horzlist, LB_DELETESTRING, s.n, 0);
      --s.count;

      if (s.n < s.cursel)
        --s.cursel;
      else if (s.n == s.cursel)
        s.cursel = 0;
    }

  if (s.count = SendMessage(hwnd_listbox, LB_GETCOUNT, 0, 0))
  {
    SendMessage(hwnd_listbox, LB_SETCURSEL, s.cursel, 0);
    SendMessage(hwnd_horzlist, LB_SETCURSEL, s.cursel, 0);
    SendMessage(GetParent(hwnd_listbox), WM_COMMAND, MAKELONG2(GetDlgCtrlID(hwnd_listbox), 1), hwnd_listbox);
  }
}

// FUNCTION: DECKDLL 0x1002aa2b
int horzlist_prep_rectangle(HWND hwnd, int idx, RECT *rect)
{
  RECT r;
  WORD w;
  WORD lft;
  WORD pics;

  w = GetWindowWord(hwnd, HORZLIST_WIDTHPLUSSPACE_INDEX);
  lft = GetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX);
  pics = GetWindowWord(hwnd, HORZLIST_PICSINLINE_INDEX);
  GetClientRect(hwnd, &r);

  SetRect(rect, (idx - lft) * w + r.left, r.top,
          ((idx - lft) + 1) * w + r.left, r.bottom);

  if (idx >= lft && (lft + pics) >= idx)
    return 1;
  else
    return 0;
}

// FUNCTION: DECKDLL 0x1002ae78
static void
TENTATIVE_scroll(HWND hwnd_listbox, HWND hwnd_horzlist)
{
  struct
  {
    LRESULT tmp;   /* ebp - 0x1f58 */
    WPARAM sel;    /* ebp - 0x1f54 */
    LRESULT count; /* ebp - 0x1f50 */
    LRESULT data;  /* ebp - 0x1f4c */
    WPARAM i;      /* ebp - 0x1f48 */
    int done;      /* ebp - 0x1f44 */
    int present[2000];
  } s;

  s.sel = SendMessageA(hwnd_listbox, 0x188, 0, 0);
  s.data = SendMessageA(hwnd_listbox, 0x199, s.sel, 0);
  s.count = SendMessageA(hwnd_listbox, 0x18b, 0, 0);

  memset(s.present, 0, 8000);
  for (s.i = 0; (int)s.i < s.count; s.i = s.i + 1)
  {
    s.tmp = SendMessageA(hwnd_listbox, 0x199, s.i, 0);
    s.present[s.tmp] = 1;
  }

  for (s.i = 0; (int)s.i < global_available_slots; s.i = s.i + 1)
  {
    s.tmp = global_raw_cards_storage[s.i].id;
    if (s.present[s.tmp] == 0)
    {
      if (check_filters(s.tmp))
      {
        s.sel = SendMessageA(hwnd_listbox, 0x180, 0, (LPARAM)global_raw_cards_storage[s.i].full_name);
        SendMessageA(hwnd_listbox, 0x19a, s.sel, s.tmp);
        SendMessageA(hwnd_horzlist, 0x181, s.sel, s.tmp);
      }
    }
  }

  s.count = SendMessageA(hwnd_listbox, 0x18b, 0, 0);
  s.sel = 0;
  s.i = 0;
  s.done = 0;
  for (; (int)s.i < s.count && s.done == 0; s.i++)
  {
    if (SendMessageA(hwnd_listbox, 0x199, s.i, 0) == (LRESULT)s.data)
    {
      s.sel = s.i;
      s.done = 1;
    }
  }

  SendMessageA(hwnd_listbox, 0x186, s.sel, 0);
  SendMessageA(hwnd_horzlist, 0x186, s.sel, 0);
  SendMessageA(GetParent(hwnd_listbox), 0x111, (GetDlgCtrlID(hwnd_listbox) & 0xffff) | 0x10000,
               (LPARAM)hwnd_listbox);
}

// FUNCTION: DECKDLL 0x1002b69d
static int
ask_movexcards(csvid_t csvid, bool shifted)
{
  struct
  {
    int local_8; /* ebp - 0x8 */
    int local_c; /* ebp - 0x4 */
  } s;

  s.local_8 = 0;

  if (shifted == 1)
  {
    if ((global_db_flags_1 & 0x61) != 0)
    {
      s.local_8 = count_card_amount_outside_edited_deck(csvid);
      global_dlg_parameter = s.local_8;
      if (show_dialog_movexcards() == 0)
        global_dlg_result = 0;
      if (s.local_8 > global_dlg_result)
        s.local_8 = global_dlg_result;

      for (s.local_c = 0; s.local_8 > s.local_c; s.local_c++)
      {
        change_global_deck_card_availability(csvid, 1);
      }

      TENTATIVE_remove_selected_from_horzlist(global_listbox_hwnd, global_horzlist_hwnd);
    }
    else
    {
      if (show_dialog_movexcards() == 0)
        s.local_8 = 0;
      s.local_8 = global_dlg_result;
    }
  }
  else
  {
    s.local_8 = 1;
    if ((global_db_flags_1 & 0x61) != 0)
      if (change_global_deck_card_availability(csvid, 1) != 0)
        TENTATIVE_remove_selected_from_horzlist(global_listbox_hwnd, global_horzlist_hwnd);
  }

  return s.local_8;
}

// FUNCTION: DECKDLL 0x10019c42
void refresh_numofcards_text(void)
{
  char buf[80];

  load_text("menus", "STATS");
  sprintf(buf, text_lines[0], global_deck_num_cards);
  SetWindowText(global_button_stats_hwnd, buf);
  InvalidateRect(global_button_stats_hwnd, NULL, FALSE);
}

// FUNCTION: DECKDLL 0x1002b126
static bool handle_card_list_click_or_drag(HWND hwnd, int singleclick, int shifted)
{
  struct
  {
    WPARAM wParam;    /* ebp - 0x30 */
    int amt;          /* ebp - 0x2c */
    int i;            /* ebp - 0x28 */
    int xoff;         /* ebp - 0x24 */
    RECT r;           /* ebp - 0x20 */
    HANDLE itemdata;  /* ebp - 0x10 */
    HWND card_hwnd;   /* ebp - 0xc */
    HWND target_hwnd; /* ebp - 0x8 */
    int yoff;         /* ebp - 0x4 */
  } s;

  s.wParam = SendMessageA(hwnd, 0x188, 0, 0);
  s.itemdata = (HANDLE)SendMessageA(hwnd, 0x199, s.wParam, 0);

  if ((((s.itemdata == (HANDLE)0x38e) || (s.itemdata == (HANDLE)0x37a)) || (s.itemdata == (HANDLE)0x375)) ||
      (s.itemdata == (HANDLE)0x384) || (s.itemdata == (HANDLE)0x37b))
  {
    return 0;
  }

  horzlist_prep_rectangle(global_horzlist_hwnd, s.wParam, &s.r);
  if (PtInRect(&s.r, global_horzlist_mouse_pt) == 0)
    return 0;

  MapWindowPoints(global_horzlist_hwnd, global_main_hwnd, (LPPOINT)&s.r, 2);

  if (singleclick == 1)
  {
    s.card_hwnd = CreateWindowExA(0, "MAGICDECK_CardClass", "Card", 0x54000000,
                                  s.r.left, s.r.top, global_smallcard_width, global_smallcard_height,
                                  global_main_hwnd, (HMENU)1, global_hinstance, (LPVOID)s.itemdata);
    if (s.card_hwnd == (HWND)0)
      return 0;

    BringWindowToTop(s.card_hwnd);
    UpdateWindow(s.card_hwnd);

    s.xoff = global_main_mouse_pt.x - s.r.left;
    s.yoff = global_main_mouse_pt.y - s.r.top;

    SendMessageA(s.card_hwnd, 0x112, 0xf012, 0);
    DestroyWindow(s.card_hwnd);

    GetCursorPos(&global_main_mouse_pt);

    s.target_hwnd = WindowFromPoint(global_main_mouse_pt);
    while (s.target_hwnd != (HWND)0 &&
           s.target_hwnd != global_main_hwnd &&
           s.target_hwnd != global_decksurface_hwnd &&
           s.target_hwnd != global_unknown_10144b04 &&
           s.target_hwnd != global_unknown_10125a3c &&
           s.target_hwnd != global_horzlist_hwnd)
    {
      s.target_hwnd = GetParent(s.target_hwnd);
    }
  }
  else
  {
    s.target_hwnd = global_decksurface_hwnd;
  }

  if (s.target_hwnd == global_decksurface_hwnd)
  {
    s.amt = ask_movexcards((int)s.itemdata, shifted);
    set_smallcard_dimensions();
    if (global_cfg.effects != 0)
      play_sound(2, 400, 0, 0);

    global_deck_was_edited = 1;
    ScreenToClient(s.target_hwnd, &global_main_mouse_pt);
    global_main_mouse_pt.x = global_main_mouse_pt.x - s.xoff;
    global_main_mouse_pt.y = global_main_mouse_pt.y - s.yoff;

    for (s.i = 0; s.i < s.amt; s.i = s.i + 1)
    {
      if (SendMessageA(s.target_hwnd, 0x4c8, (WPARAM)s.itemdata,
                       MAKELONG(global_main_mouse_pt.x, global_main_mouse_pt.y)) == 0)
        MessageBeep(0);
      change_global_deck_card_availability((int)s.itemdata, 2);
    }

    count_packs();
    if ((global_db_flags_1 & 0x20) != 0)
    {
      SendMessageA(global_horzlist_hwnd, 0x186, 0, 0);
      SendMessageA(global_listbox_hwnd, 0x186, 0, 0);
    }
    else
    {
      SendMessageA(global_fullcard_hwnd, 0x400, (WPARAM)s.itemdata, 0);
      SendMessageA(global_horzlist_hwnd, 0x186, s.wParam, 0);
      SendMessageA(global_listbox_hwnd, 0x186, s.wParam, 0);
    }

    if ((global_db_flags_1 & 0x41) != 0)
      SendMessageA(global_horzlist_hwnd, 0x466, (WPARAM)s.itemdata, 0);
  }
  else if ((s.target_hwnd == global_unknown_10125a3c) || (s.target_hwnd == global_unknown_10144b04))
  {
    s.amt = ask_movexcards((int)s.itemdata, shifted);
    if (global_cfg.effects != 0)
      play_sound(2, 400, 0, 0);

    global_deck_was_edited = 1;
    ScreenToClient(s.target_hwnd, &global_main_mouse_pt);
    for (s.i = 0; s.i < s.amt; s.i = s.i + 1)
    {
      if (SendMessageA(s.target_hwnd, 0x4c8, (WPARAM)s.itemdata,
                       MAKELONG(global_main_mouse_pt.x, global_main_mouse_pt.y)) == 0)
        MessageBeep(0);
    }

    if ((global_db_flags_1 & 0x20) != 0)
    {
      SendMessageA(global_horzlist_hwnd, 0x186, 0, 0);
      SendMessageA(global_listbox_hwnd, 0x186, 0, 0);
    }
    else
    {
      SendMessageA(global_fullcard_hwnd, 0x400, (WPARAM)s.itemdata, 0);
      SendMessageA(global_horzlist_hwnd, 0x186, s.wParam, 0);
      SendMessageA(global_listbox_hwnd, 0x186, s.wParam, 0);
    }
  }
  else
  {
    TENTATIVE_scroll(global_listbox_hwnd, global_horzlist_hwnd);
    SendMessageA(global_fullcard_hwnd, 0x400, (WPARAM)s.itemdata, 0);
    SendMessageA(global_horzlist_hwnd, 0x186, s.wParam, 0);
    SendMessageA(global_listbox_hwnd, 0x186, s.wParam, 0);
    if ((global_db_flags_1 & 0x41) != 0)
      SendMessageA(global_horzlist_hwnd, 0x466, (WPARAM)s.itemdata, 0);
  }

  refresh_numofcards_text();
  return 1;
}

// FUNCTION: DECKDLL 0x1002887e
LRESULT CALLBACK wndproc_HorzListClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  // GLOBAL: DECKDLL 0x10105740
  static HDC DAT_10105740 = NULL;
  // GLOBAL: DECKDLL 0x101052d0
  static LOGFONT DAT_101052d0;
  // GLOBAL: DECKDLL 0x1010530c
  static HFONT DAT_1010530c = NULL;

  // GLOBAL: DECKDLL 0x1003a7cc
  static int sellprice;
  // GLOBAL: DECKDLL 0x10105310
  static csvid_t curr_csvid;
  // GLOBAL: DECKDLL 0x1003a790
  static LOGFONT horzListClass_fontTemplate = {
      0,        // lfHeight
      0,        // lfWidth
      0,        // lfEscapement
      0,        // lfOrientation
      FW_NORMAL // lfWeight
  };

  struct
  {
    HRGN hrgn;         /* ebp - 0x22c */
    HGDIOBJ h;         /* ebp - 0x228 */
    RECT r_small;      /* ebp - 0x224 */
    int paint_amt;     /* ebp - 0x214 */
    HDC hdc;           /* ebp - 0x210 */
    BITMAP bmp;        /* ebp - 0x20c */
    PAINTSTRUCT paint; /* ebp - 0x1f4 */

    int paint_idx; /* ebp - 0x1b4 */
    int save;      /* ebp - 0x1b0 */

    RECT paint_r;    /* ebp - 0x1ac */
    int paint_csvid; /* ebp - 0x19c */

    POINT mm_pt;    /* ebp - 0x198 */
    int mm_i;       /* ebp - 0x190 */
    int mm_hit_idx; /* ebp - 0x18c */
    RECT mm_r;      /* ebp - 0x188 */
    int mm_hit;     /* ebp - 0x178 */

    int scrollpos;    /* ebp - 0x174 */
    int minpos;       /* ebp - 0x170 */
    int page_amt;     /* ebp - 0x16c */
    WORD scroll_code; /* ebp - 0x168 */
    int maxpos;       /* ebp - 0x164 */
    uint wLast_int;   /* ebp - 0x160 */
    int tgt;          /* ebp - 0x15c */

    HDC hdc_local; /* ebp - 0x158 */
    RECT create_r; /* ebp - 0x154 */
    int pad_144;   /* ebp - 0x144 */

    DELETEITEMSTRUCT del2; /* ebp - 0x140 */
    int last_pic_index;    /* ebp - 0x12c */
    int left_pic_index;    /* ebp - 0x128 */
    int pad_124;           /* ebp - 0x124 */

    RECT r6;              /* ebp - 0x120 */
    int scrollMinPos;     /* ebp - 0x110 */
    int scrollMaxPos;     /* ebp - 0x10c */
    DELETEITEMSTRUCT del; /* ebp - 0x108 */
    int oldidx;           /* ebp - 0xf4 */
    int getInx;           /* ebp - 0xf0 */
    int insert_index;     /* ebp - 0xec */
    int insert_loop_idx;  /* ebp - 0xe8 */
    RECT r5;              /* ebp - 0xe4 */
    int item_idx;         /* ebp - 0xd4 */
    RECT r4;              /* ebp - 0xd0 */
    UINT local_c0;        /* ebp - 0xc0 */
    RECT r7;              /* ebp - 0xb0 */

    WORD wOldSel;    /* ebp - 0xac */
    WORD wOldSelPad; /* ebp - 0xaa */
    RECT r2;         /* ebp - 0xa8 */

    unsigned int wanted_card; /* ebp - 0x98 */
    int idx;                  /* ebp - 0x94 */
    RECT r3;                  /* ebp - 0x90 */
    WORD wPics;               /* ebp - 0x80 */
    WORD wPicsPad;            /* ebp - 0x7e */
    int *horz_list_addr;      /* ebp - 0x7c */

    MSG m;          /* ebp - 0x78 */
    int amt;        /* ebp - 0x5c */
    int currsel;    /* ebp - 0x58 */
    POINT p;        /* ebp - 0x54 */
    int j;          /* ebp - 0x4c */
    WORD wWidth;    /* ebp - 0x48 */
    WORD wWidthPad; /* ebp - 0x46 */
    iid_t iid;      /* ebp - 0x44 */
    char buf[32];   /* ebp - 0x40 */

    int i;           /* ebp - 0x20 */
    WORD wNewSel;    /* ebp - 0x1c */
    WORD wNewSelPad; /* ebp - 0x1a */
    WORD wLeft;      /* ebp - 0x18 */
    RECT r;          /* ebp - 0x14 */
    WORD wLast;      /* ebp - 4 */
    WORD wLastPad;   /* ebp - 2 */
  } s;

  switch (msg)
  {
  case 0x464:
    SendMessageA(global_main_hwnd, 0x466, (WPARAM)wparam, (LPARAM)hwnd);
    return 0;

  case 0x466:
  {

    s.wanted_card = (unsigned int)wparam;
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    s.horz_list_addr = (int *)GetWindowLongA(hwnd, HORZLIST_ADDR_INDEX);
    s.wLeft = GetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX);
    s.wPics = GetWindowWord(hwnd, HORZLIST_PICSINLINE_INDEX);

    for (s.idx = s.wLeft; MIN(s.wLast, s.wPics + s.wLeft) > s.idx; s.idx++)
    {
      if ((unsigned int)s.horz_list_addr[s.idx] == s.wanted_card)
      {
        horzlist_prep_rectangle(hwnd, (int)s.idx, &s.r3);
        InvalidateRect(hwnd, &s.r3, 0);
      }
    }
    return 0;
  }

  case LB_GETCURSEL:
    return s.wNewSel = GetWindowWord(hwnd, HORZLIST_CURRSEL_INDEX);

  case LB_SETCURSEL:
  {
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    s.horz_list_addr = (int *)GetWindowLongA(hwnd, HORZLIST_ADDR_INDEX);
    s.wPics = GetWindowWord(hwnd, HORZLIST_PICSINLINE_INDEX);
    s.wOldSel = GetWindowWord(hwnd, HORZLIST_CURRSEL_INDEX);
    s.wLeft = GetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX);

    s.wNewSel = (WORD)wparam;
    if (s.wNewSel >= 0 && s.wNewSel < s.wLast)
    {
      SetWindowWord(hwnd, HORZLIST_CURRSEL_INDEX, s.wNewSel);

      if (s.wOldSel >= 0 && s.wOldSel < s.wLast)
      {
        horzlist_prep_rectangle(hwnd, s.wOldSel, &s.r2);
        // InvalidateRect(hwnd, &s.r2, 0);
      }
      horzlist_prep_rectangle(hwnd, s.wNewSel, &s.r2);
      // InvalidateRect(hwnd, &s.r2, 0);
      UpdateWindow(hwnd);

      if (s.wNewSel < s.wLeft)
      {
        SendMessageA(hwnd, WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, s.wNewSel), 0);
      }

      if ((s.wPics + s.wLeft - 1) < s.wNewSel)
      {
        SendMessageA(hwnd, WM_HSCROLL, MAKELONG3(SB_THUMBPOSITION, s.wNewSel - s.wPics + 1), 0);
      }
      return 0;
    }
    else
      return -1;
  }

  case LB_SETITEMHEIGHT:
  {
    s.local_c0 = (UINT)lparam & 0xffff;

    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);

    s.wWidth = (WORD)(s.local_c0 + 8);

    SetWindowWord(hwnd, HORZLIST_WIDTHPLUSSPACE_INDEX, s.wWidth);

    GetClientRect(hwnd, &s.r7);

    s.wPics = (s.r7.right - 4) / s.wWidth;
    if (((s.r7.right - 4) % s.wWidth) >= (s.wWidth >> 2))
      ++s.wPics;

    SetWindowWord(hwnd, HORZLIST_PICSINLINE_INDEX, s.wPics);

    SetScrollRange(hwnd, SB_HORZ, 0, s.wLast - s.wPics + 1, 0);
    InvalidateRect(hwnd, 0, 1);
    return 0;
  }

  case LB_ADDSTRING:
  {
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    s.horz_list_addr = (int *)GetWindowLongA(hwnd, HORZLIST_ADDR_INDEX);
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    s.wPics = GetWindowWord(hwnd, HORZLIST_PICSINLINE_INDEX);

    s.item_idx = s.wLast;
    s.horz_list_addr = (int *)realloc(s.horz_list_addr, (s.wLast + 1) * 4);
    if (s.horz_list_addr)
    {

      s.horz_list_addr[s.wLast] = (int)lparam;
      SetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX, ++s.wLast);
      SetWindowLongA(hwnd, HORZLIST_ADDR_INDEX, (LONG)s.horz_list_addr);

      SetScrollRange(hwnd, SB_HORZ, 0, s.wLast - s.wPics + 1, 0);
      horzlist_prep_rectangle(hwnd, s.item_idx, &s.r4);
      InvalidateRect(hwnd, &s.r4, 1);

      return s.item_idx;
    }
    else
      return -2;
  }

  case LB_INSERTSTRING:
  {
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    s.horz_list_addr = (int *)GetWindowLongA(hwnd, HORZLIST_ADDR_INDEX);
    s.wPics = GetWindowWord(hwnd, HORZLIST_PICSINLINE_INDEX);

    s.insert_index = (int)wparam;
    if (s.insert_index == -1)
    {
      return SendMessageA(hwnd, LB_ADDSTRING, 0, lparam);
    }

    if (s.insert_index >= (int)(unsigned int)s.wLast)
    {
      return SendMessageA(hwnd, LB_ADDSTRING, 0, lparam);
    }

    if (s.insert_index < -1 || s.insert_index >= s.wLast)
    {
      return -1;
    }

    s.horz_list_addr = (int *)realloc(s.horz_list_addr, (s.wLast + 1) * 4);
    if (s.horz_list_addr)
    {

      for (s.insert_loop_idx = s.wLast; s.insert_index < s.insert_loop_idx; s.insert_loop_idx--)
      {
        s.horz_list_addr[s.insert_loop_idx] = s.horz_list_addr[s.insert_loop_idx - 1];
      }
      s.horz_list_addr[s.insert_index] = (int)lparam;

      SetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX, ++s.wLast);
      SetWindowLongA(hwnd, HORZLIST_ADDR_INDEX, (LONG)s.horz_list_addr);
      SetScrollRange(hwnd, SB_HORZ, 0, s.wLast - s.wPics + 1, 0);

      horzlist_prep_rectangle(hwnd, s.insert_index, &s.r5);
      s.r5.right = 2000;
      InvalidateRect(hwnd, &s.r5, 1);

      return s.insert_index;
    }
    else
      return -2;
  }

  case LB_GETITEMDATA:
  {
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    s.horz_list_addr = (int *)GetWindowLongA(hwnd, HORZLIST_ADDR_INDEX);
    s.getInx = wparam;
    if (s.getInx >= 0 && s.getInx < s.wLast)
      return s.horz_list_addr[s.getInx];
    else
      return -1;
  }

  case LB_RESETCONTENT:
  {
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    s.horz_list_addr = (int *)GetWindowLongA(hwnd, HORZLIST_ADDR_INDEX);

    s.oldidx = 0;
    for (; s.wLast > s.oldidx; s.oldidx++)
    {
      s.del.CtlType = ODT_LISTBOX;
      s.del.CtlID = GetDlgCtrlID(hwnd);
      s.del.itemID = s.oldidx;
      s.del.hwndItem = hwnd;
      s.del.itemData = (unsigned int)s.horz_list_addr[s.oldidx];

      SendMessageA(GetParent(hwnd), WM_DELETEITEM, (WPARAM)GetDlgCtrlID(hwnd), (LPARAM)&s.del);
    }

    s.horz_list_addr = (int *)realloc(s.horz_list_addr, 4);

    s.wLast = 0;
    s.wNewSel = (WORD)-1;
    s.wLeft = 0;

    SetWindowLongA(hwnd, HORZLIST_ADDR_INDEX, (LONG)s.horz_list_addr);
    SetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX, s.wLast);
    SetWindowWord(hwnd, HORZLIST_CURRSEL_INDEX, s.wNewSel);
    SetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX, s.wLeft);

    SetScrollRange(hwnd, SB_HORZ, 0, -1, 0);
    SetScrollPos(hwnd, SB_HORZ, s.wNewSel, 1);
    InvalidateRect(hwnd, 0, 1);

    return 0;
  }

  case LB_GETTOPINDEX:
    s.wLeft = GetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX);
    return (LRESULT)(s.wLeft);

  case LB_SETTOPINDEX:
    s.wLeft = (WORD)wparam;
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);

    if (s.wLeft >= 0 && s.wLast > s.wLeft)
    {
      SetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX, s.wLeft);
      SendMessageA(hwnd, 0x114, (WPARAM)MAKELONG(SB_THUMBPOSITION, s.wLeft), 0);
      return 0;
    }
    else
      return -1;

  case LB_DELETESTRING:
  {
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    // if ((int)(unsigned int)wparam >= s.last_pic_index)
    //   return -1;

    s.horz_list_addr = (int *)GetWindowLongA(hwnd, HORZLIST_ADDR_INDEX);
    s.wNewSel = (int)(short)GetWindowWord(hwnd, HORZLIST_CURRSEL_INDEX);
    s.wLeft = (int)(short)GetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX);
    s.last_pic_index = (int)wparam;
    if (s.last_pic_index >= 0 && s.last_pic_index < s.wLast)
    {
      s.del2.CtlType = ODT_LISTBOX;
      s.del2.CtlID = GetDlgCtrlID(hwnd);
      s.del2.itemID = s.last_pic_index;
      s.del2.hwndItem = hwnd;
      s.del2.itemData = (unsigned int)s.horz_list_addr[s.last_pic_index];

      SendMessageA(GetParent(hwnd), WM_DELETEITEM, (WPARAM)GetDlgCtrlID(hwnd), (LPARAM)&s.del2);

      for (s.left_pic_index = s.last_pic_index; s.left_pic_index < s.wLast - 1; ++s.left_pic_index)
        s.horz_list_addr[s.left_pic_index] = s.horz_list_addr[s.left_pic_index + 1];

      SetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX, --s.wLast);

      if (s.wLast - 1 < s.wNewSel)
      {
        SetWindowWord(hwnd, HORZLIST_CURRSEL_INDEX, --s.wNewSel);
      }

      if (s.wLeft > s.wLast - 1)
      {
        SetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX, --s.wLeft);
      }

      // s.wPics = GetWindowWord(hwnd, HORZLIST_PICSINLINE_INDEX);
      // s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
      GetScrollRange(hwnd, 0, &s.scrollMinPos, &s.scrollMaxPos);
      SetScrollRange(hwnd, 0, s.scrollMinPos, s.scrollMaxPos - 1, 1);

      horzlist_prep_rectangle(hwnd, s.last_pic_index, &s.r6);
      s.r6.right = GetWindowWord(hwnd, HORZLIST_WIDTHPLUSSPACE_INDEX) * (s.wLast + 1);
      s.r6.left = MAX(s.r6.left, 0);
      InvalidateRect(hwnd, &s.r6, 1);

      if (!s.wLast)
      {
        s.wNewSel = -1;
        s.wLeft = 0;
        SetWindowWord(hwnd, HORZLIST_CURRSEL_INDEX, s.wNewSel);
        SetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX, s.wLeft);
      }

      return s.wLast;
    }
    else
      return -1;
  }

  case WM_CHAR:
    if (wparam == 27)
    {
      SendMessageA(global_main_hwnd, 0x10, 0, 0);
      return 0;
    }

    SendMessageA(global_listbox_hwnd, msg, wparam, lparam);
    s.pad_144 = (int)SendMessageA(global_listbox_hwnd, LB_GETCURSEL, 0, 0);
    SendMessageA(hwnd, WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, s.pad_144), 0);
    SendMessageA(hwnd, LB_SETCURSEL, s.pad_144, 0);
    SendMessageA(GetParent(hwnd), WM_COMMAND, (GetDlgCtrlID(hwnd) & 0xffff) | 0x10000, (LPARAM)hwnd);
    return 0;

  case WM_COMMAND:
  {
    switch ((unsigned int)wparam & 0xffff)
    {
    case 1:
      *(WORD *)&s.amt = (WORD)count_card_amount_outside_edited_deck(curr_csvid);

      global_dlg_parameter = 1;
      global_dlg_result = 1;

      if ((s.amt & 0xffff) > 1)
        if (show_dialog_sellxcards() == 0)
          return 0;

      if (!global_dlg_result)
        return 0;
      else if ((s.amt & 0xffff) < global_dlg_result)
        global_dlg_result = s.amt & 0xffff;

      for (s.i = 0, s.j = 0; s.j < global_dlg_result && !s.i; s.j++)
        if (!delete_card_from_global_deck(curr_csvid, 1))
          s.i = 1;

      *Gold += global_dlg_result * sellprice;

      load_text("menus", "PRICE");
      sprintf(s.buf, text_lines[0], *Gold);
      strncpy(global_deckinfo.deckname, s.buf, 12);

      InvalidateRect(global_title_hwnd, NULL, 1);
      TENTATIVE_remove_selected_from_horzlist(global_listbox_hwnd, global_horzlist_hwnd);
      InvalidateRect(global_horzlist_hwnd, 0, 0);
      break;
    case 2:
      global_dlg_parameter = check_colors_inout_edited_deck(1);
      show_dialog_groupmove();
      move_colors_fromto_global_deck(1, global_dlg_result);
      TENTATIVE_remove_selected_from_horzlist(global_listbox_hwnd, global_horzlist_hwnd);
      InvalidateRect(global_horzlist_hwnd, 0, 1);

      SendMessage(global_decksurface_hwnd, 0x401, 0, 0);
      InvalidateRect(global_decksurface_hwnd, 0, 1);
      break;
    case 3:
      global_dlg_parameter = check_colors_inout_edited_deck(0);
      show_dialog_groupmove();
      move_colors_fromto_global_deck(0, global_dlg_result);
      TENTATIVE_remove_selected_from_horzlist(global_listbox_hwnd, global_horzlist_hwnd);
      InvalidateRect(global_horzlist_hwnd, 0, 1);

      SendMessage(global_decksurface_hwnd, 0x401, 0, 0);
      InvalidateRect(global_decksurface_hwnd, 0, 1);
      break;
    }
    return 0;
  }

  case WM_CREATE:
  {
    GetClientRect(hwnd, &s.create_r);

    s.horz_list_addr = (int *)malloc(4);

    s.wLast = 0;
    s.wNewSel = (WORD)-1;
    s.wLeft = 0;

    s.wWidth = (WORD)global_smallcard_piclist_width;
    if ((int)(unsigned int)s.wWidth > 0)
      s.wPics = (WORD)((s.create_r.right - 4) / s.wWidth);
    else
      s.wPics = 0;

    SetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX, s.wLast);
    SetWindowLongA(hwnd, HORZLIST_ADDR_INDEX, (LONG)s.horz_list_addr);
    SetWindowWord(hwnd, HORZLIST_CURRSEL_INDEX, s.wNewSel);
    SetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX, s.wLeft);

    SetWindowWord(hwnd, HORZLIST_WIDTHPLUSSPACE_INDEX, s.wWidth);
    SetWindowWord(hwnd, HORZLIST_PICSINLINE_INDEX, s.wPics);

    SetScrollRange(hwnd, SB_HORZ, 0, s.wLast - s.wPics + 1, 0);
    SetScrollPos(hwnd, SB_HORZ, s.wNewSel, 1);

    global_horzlist_popup = CreatePopupMenu();

    s.hdc_local = GetDC(hwnd);
    ApplyCardArtPaletteToDc(s.hdc_local);
    DAT_10105740 = CreateCompatibleDC(s.hdc_local);
    ApplyCardArtPaletteToDc(DAT_10105740);
    ReleaseDC(hwnd, s.hdc_local);

    if (!DAT_1010530c)
    {
      memcpy(&DAT_101052d0, &horzListClass_fontTemplate, sizeof(LOGFONT));
      DAT_101052d0.lfHeight = 0x2a;
      DAT_101052d0.lfWeight = 0x1f4;
      strcpy(DAT_101052d0.lfFaceName, "Kudos Condensed SSi");
      DAT_1010530c = CreateFontIndirectA(&DAT_101052d0);
    }

    return 0;
  }

  case WM_DESTROY:
  {
    s.horz_list_addr = (int *)GetWindowLongA(hwnd, HORZLIST_ADDR_INDEX);
    free(s.horz_list_addr);

    DestroyMenu(global_horzlist_popup);
    DeleteDC(DAT_10105740);
    if (DAT_1010530c)
      DeleteObject(DAT_1010530c);
    DAT_1010530c = 0;
    return 0;
  }

  case WM_HSCROLL:
  {
    s.wWidth = GetWindowWord(hwnd, HORZLIST_WIDTHPLUSSPACE_INDEX);
    s.wPics = GetWindowWord(hwnd, HORZLIST_PICSINLINE_INDEX);

    s.scrollpos = GetScrollPos(hwnd, SB_HORZ);
    GetScrollRange(hwnd, SB_HORZ, &s.minpos, &s.maxpos);

    s.page_amt = s.wPics;
    s.wLast_int = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    s.scroll_code = (WORD)wparam;

    if (s.scroll_code == SB_TOP)
      s.tgt = s.minpos;
    else if (s.scroll_code == SB_LINEUP)
      s.tgt = s.scrollpos - 1;
    else if (s.scroll_code == SB_PAGEUP)
      s.tgt = s.scrollpos - s.page_amt;
    else if (s.scroll_code == SB_THUMBPOSITION)
      s.tgt = GET_Y_LPARAM(wparam);
    else if (s.scroll_code == SB_PAGEDOWN)
      s.tgt = s.page_amt + s.scrollpos;
    else if (s.scroll_code == SB_LINEDOWN)
      s.tgt = s.scrollpos + 1;
    else if (s.scroll_code == SB_BOTTOM)
      s.tgt = s.maxpos;
    else
      s.tgt = s.scrollpos;

    if (s.tgt < s.minpos)
      s.tgt = s.minpos;
    if (s.tgt > s.maxpos)
      s.tgt = s.maxpos;

    if (s.scrollpos != s.tgt)
    {
      s.wLeft = (WORD)s.tgt;
      SetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX, s.wLeft);
      UpdateWindow(hwnd);

      ScrollWindow(hwnd, (s.scrollpos - s.tgt) * s.wWidth, 0, 0, 0);
      SetScrollPos(hwnd, SB_HORZ, s.tgt, 1);
      InvalidateRect(hwnd, NULL, 0);
    }
    return 0;
  }

  case WM_KEYDOWN:
    if (wparam == VK_UP || wparam == VK_LEFT)
    {
      SendMessageA(hwnd, WM_HSCROLL, SB_LINELEFT, 0);
    }
    else if (wparam == VK_DOWN || wparam == VK_RIGHT)
    {
      SendMessageA(hwnd, WM_HSCROLL, SB_LINERIGHT, 0);
    }
    else if (wparam == VK_PRIOR)
    {
      SendMessageA(hwnd, WM_HSCROLL, SB_PAGELEFT, 0);
    }
    else if (wparam == VK_NEXT)
    {
      SendMessageA(hwnd, WM_HSCROLL, SB_PAGERIGHT, 0);
    }
    else if (wparam == VK_HOME)
    {
      SendMessageA(hwnd, WM_HSCROLL, SB_LEFT, 0);
    }
    else if (wparam == VK_END)
    {
      SendMessageA(hwnd, WM_HSCROLL, SB_RIGHT, 0);
    }
    return 0;

  case WM_LBUTTONDBLCLK:
  {
    if ((global_db_flags_2 & 1) == 0)
      return 0;

    GetCursorPos(&s.p);
    GetCursorPos((LPPOINT)&global_horzlist_mouse_pt);
    ScreenToClient(global_main_hwnd, &s.p);
    ScreenToClient(global_horzlist_hwnd, (LPPOINT)&global_horzlist_mouse_pt);

    SetFocus(hwnd);
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    if (s.wLast > 0)
    {
      SendMessageA(hwnd, WM_MOUSEMOVE, wparam & ~MK_LBUTTON, lparam);

      if ((wparam & MK_SHIFT))
      {
        if (!handle_card_list_click_or_drag(hwnd, 0, 1))
        {
          MessageBeep(0);
          return 0;
        }
      }
      else if (!handle_card_list_click_or_drag(hwnd, 0, 0))
      {
        MessageBeep(0);
        return 0;
      }
    }

    return 0;
  }

  case WM_LBUTTONDOWN:
  {
    if ((global_db_flags_2 & 1) == 0)
      return 0;

    GetCursorPos(&s.p);
    GetCursorPos((LPPOINT)&global_horzlist_mouse_pt);
    ScreenToClient(global_main_hwnd, &s.p);
    ScreenToClient(global_horzlist_hwnd, (LPPOINT)&global_horzlist_mouse_pt);

    s.j = GetDoubleClickTime();
    Sleep(s.j);
    if (PeekMessageA(&s.m, hwnd, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, 0))
      return 0;

    SetFocus(hwnd);
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    if (s.wLast > 0)
    {
      SendMessageA(hwnd, WM_MOUSEMOVE, wparam & ~MK_LBUTTON, lparam);

      if ((wparam & MK_SHIFT))
      {
        if (!handle_card_list_click_or_drag(hwnd, 1, 1))
        {
          MessageBeep(0);
          return 0;
        }
      }
      else if (!handle_card_list_click_or_drag(hwnd, 1, 0))
      {
        MessageBeep(0);
        return 0;
      }
    }
    return 0;
  }

  case WM_MOUSEMOVE:
  {
    if (abs(DAT_101052c0.y - GET_Y_LPARAM(lparam)) +
            abs(DAT_101052c0.x - GET_X_LPARAM(lparam)) <
        2)
      return 0;

    DAT_101052c0.x = GET_X_LPARAM(lparam);
    DAT_101052c0.y = GET_Y_LPARAM(lparam);

    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    s.wLeft = GetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX);
    s.wWidth = GetWindowWord(hwnd, HORZLIST_WIDTHPLUSSPACE_INDEX);
    s.wPics = GetWindowWord(hwnd, HORZLIST_PICSINLINE_INDEX);

    if (s.wLast > 0)
    {
      s.mm_pt.x = GET_X_LPARAM(lparam);
      s.mm_pt.y = GET_Y_LPARAM(lparam);

      s.mm_i = s.wLeft;
      s.mm_hit = 0;
      for (; s.wPics + s.wLeft >= s.mm_i; s.mm_i++)
      {
        if (s.mm_hit == 0)
        {
          horzlist_prep_rectangle(hwnd, s.mm_i, &s.mm_r);
          if (PtInRect(&s.mm_r, s.mm_pt))
          {
            s.mm_hit = 1;
            s.mm_hit_idx = s.mm_i;
          }
        }
      }

      if (s.mm_hit != 0)
      {
        if (SendMessageA(hwnd, LB_GETCURSEL, 0, 0) != s.mm_hit_idx)
        {
          SendMessageA(hwnd, LB_SETCURSEL, (WPARAM)s.mm_hit_idx, 0);
          SendMessageA(GetParent(hwnd), WM_COMMAND, (GetDlgCtrlID(hwnd) & 0xffff) | 0x10000, (LPARAM)hwnd);
        }
      }
    }

    return 0;
  }

  case WM_PAINT:
  {
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    s.horz_list_addr = (int *)GetWindowLongA(hwnd, HORZLIST_ADDR_INDEX);
    s.wNewSel = GetWindowWord(hwnd, HORZLIST_CURRSEL_INDEX);
    s.wLeft = GetWindowWord(hwnd, HORZLIST_LEFTPIC_INDEX);
    s.wWidth = GetWindowWord(hwnd, HORZLIST_WIDTHPLUSSPACE_INDEX);
    s.wPics = GetWindowWord(hwnd, HORZLIST_PICSINLINE_INDEX);

    s.hdc = BeginPaint(hwnd, &s.paint);
    ApplyCardArtPaletteToDc(s.hdc);

    s.hrgn = CreateRectRgn(0, 0, global_smallcard_piclist_width, global_smallcard_piclist_height);

    GetClientRect(hwnd, &s.paint_r);
    GetObjectA(global_pic_dekbar1, sizeof(BITMAP), &s.bmp);

    s.h = SelectObject(DAT_10105740, global_pic_dekbar1);
    StretchBlt(s.hdc, 0, 0, s.paint_r.right, s.paint_r.bottom, DAT_10105740, 0, 0, s.bmp.bmWidth, s.bmp.bmHeight, SRCCOPY);
    SelectObject(DAT_10105740, s.h);

    for (s.paint_idx = s.wLeft;
         MIN(s.wLast, s.wPics + s.wLeft) > s.paint_idx;
         s.paint_idx++)
    {
      horzlist_prep_rectangle(hwnd, (int)s.paint_idx, &s.paint_r);
      s.paint_csvid = s.horz_list_addr[s.paint_idx];

      SetRect(&s.r_small, 0, 0, global_smallcard_piclist_width, global_smallcard_piclist_height);

      SetRectRgn(s.hrgn, s.r_small.left, s.r_small.top, s.r_small.right, s.r_small.bottom);
      SelectClipRgn(global_hdc, s.hrgn);

      s.paint_amt = count_card_amount_outside_edited_deck((csvid_t)s.paint_csvid);
      DrawSmallCard(global_hdc, &s.r_small, &global_raw_cards_storage[s.paint_csvid], 1, 1);

      if ((global_db_flags_1 & 0x41) != 0 && (1 < s.paint_amt))
      {
        sprintf(s.buf, "%d", s.paint_amt);

        s.save = SaveDC(global_hdc);
        SetMapMode(global_hdc, 8);
        SetWindowExtEx(global_hdc, 200, 0x118, 0);
        SetViewportExtEx(global_hdc, s.r_small.right - s.r_small.left, s.r_small.bottom - s.r_small.top, 0);
        SetViewportOrgEx(global_hdc, s.r_small.left, s.r_small.top, 0);

        SetBkMode(global_hdc, 1);
        SelectObject(global_hdc, DAT_1010530c);
        SetTextAlign(global_hdc, 0);

        SetTextColor(global_hdc, 0x010000C9);

        TextOutA(global_hdc, 0xc8 - (int)(strlen(s.buf) * 0x10) + 3, 3, s.buf, (int)strlen(s.buf));

        SetTextColor(global_hdc, 0x02FEFEFE);
        SetBkMode(global_hdc, 1);

        TextOutA(global_hdc, 0xc8 - (int)(strlen(s.buf) * 0x10), 0, s.buf, (int)strlen(s.buf));

        RestoreDC(global_hdc, s.save);
      }

      SelectClipRgn(global_hdc, 0);

      s.paint_r.left += (s.paint_r.right - s.paint_r.left - global_smallcard_piclist_width) / 2;
      s.paint_r.top += (s.paint_r.bottom - s.paint_r.top - global_smallcard_piclist_height) / 2;

      BitBlt(s.hdc, s.paint_r.left, s.paint_r.top, global_smallcard_piclist_width, global_smallcard_piclist_height,
             global_hdc, 0, 0, SRCCOPY);
    }

    EndPaint(hwnd, &s.paint);
    DeleteObject(s.hrgn);
    return 0;
  }

  case WM_RBUTTONDOWN:
  {
    if ((global_db_flags_2 & 2) == 0)
      return 0;

    SetFocus(hwnd);
    s.wLast = GetWindowWord(hwnd, HORZLIST_LASTPIC_INDEX);
    if (s.wLast > 0)
    {
      SendMessageA(hwnd, WM_MOUSEMOVE, wparam & ~MK_RBUTTON, lparam);

      s.p.x = GET_X_LPARAM(lparam);
      s.p.y = GET_Y_LPARAM(lparam);

      s.currsel = (int)SendMessageA(hwnd, LB_GETCURSEL, 0, 0);
      horzlist_prep_rectangle(global_horzlist_hwnd, s.currsel, &s.r);
      if (!PtInRect(&s.r, s.p))
        return 0;

      ScreenToClient(global_main_hwnd, &s.p);

      curr_csvid = (csvid_t)SendMessageA(hwnd, LB_GETITEMDATA, s.currsel, 0);
      s.iid = get_iid_from_global_deck_card(curr_csvid);
      if (s.iid == -1)
        return 0;

      sellprice = SellPrice(s.iid);

      load_text("menus", "SELLCARD");
      sprintf(s.buf, text_lines[0], sellprice);

      AppendMenuA(global_horzlist_popup, 0, 1, s.buf);

      ClientToScreen(hwnd, &s.p);
      TrackPopupMenu(global_horzlist_popup, 2, s.p.x + 10, s.p.y + 10, 0, hwnd, 0);
      DeleteMenu(global_horzlist_popup, 1, 0);
    }
    return 0;
  }

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: DECKDLL 0x100057bc
void reset_dim_and_pos_all_cards(HWND hwnd, RECT *rect)
{
  struct
  {
    HWND last;                  /* ebp - 0x18 */
    int offset_between_cards_y; /* ebp - 0x14 */
    HWND w;                     /* ebp - 0x10 */
    int offset_from_sides;      /* ebp - 0xc */
    int y;                      /* ebp - 8 */
    int x;                      /* ebp - 4 */
  } s;

  s.offset_between_cards_y = (global_smallcard_height * 18) / 100;

  s.offset_from_sides = global_smallcard_height / 10;

  s.x = rect->left + s.offset_from_sides;
  s.y = rect->top + s.offset_from_sides;

  s.last = 0;
  for (s.w = GetTopWindow(hwnd); s.w != 0; s.w = GetWindow(s.w, GW_HWNDNEXT))
    s.last = s.w;

  for (s.w = s.last; s.w != 0; s.w = GetWindow(s.w, GW_HWNDPREV))
  {
    if (rect->bottom < s.y + global_smallcard_height)
    {
      s.x += global_smallcard_width + 8;
      s.y = rect->top + s.offset_from_sides;
    }

    SetWindowPos(s.w, 0, s.x, s.y, global_smallcard_width, global_smallcard_height, 4);
    s.y += s.offset_between_cards_y;
  }
}

// FUNCTION: DECKDLL 0x1001a58e
int ask_about_saving_deck(void)
{
  int rval;
  char txt[128];

  load_text("menus", "SAVE");
  sprintf(txt, text_lines[0], global_deck_filename);
  load_text("menus", "DECKBUILDER");
  rval = MessageBox(global_main_hwnd, txt, text_lines[0], MB_ICONQUESTION | MB_YESNOCANCEL);
  if (rval == IDYES)
    rval = SendMessage(global_main_hwnd, WM_COMMAND, RES_MAINMENU_SAVEDECK, 0);
  return rval;
}

// FUNCTION: DECKDLL 0x10008abf
static void
TENTATIVE_move_from_owned_cards(HWND hwnd, POINT point, int shift2_unshift1, int singleclick)
{
  struct
  {
    WORD amt32;    /* ebp - 0x38 */
    POINT pt;      /* ebp - 0x34 */
    int ctrl_id;   /* ebp - 0x2c */
    int i;         /* ebp - 0x28 */
    HWND new_hwnd; /* ebp - 0x24 */
    int num;       /* ebp - 0x20 (low word) */
    HWND parent;   /* ebp - 0x1c */
    RECT r;        /* ebp - 0x18 */
    csvid_t csvid; /* ebp - 0x8 */
    HWND target;   /* ebp - 0x4 */
  } s;

  s.csvid = (csvid_t)GetWindowLongA(hwnd, DAT_10030000);
  s.amt32 = GetWindowWord(hwnd, DAT_10030004);
  s.parent = GetParent(hwnd);
  s.ctrl_id = GetDlgCtrlID(hwnd);
  GetWindowRect(hwnd, &s.r);

  if (s.amt32 >= 2)
  {
    s.pt.x = s.r.left;
    s.pt.y = s.r.top;
    SendMessageA(hwnd, 0x401, 1, 0);
    MapWindowPoints(NULL, s.parent, &s.pt, 1);

    s.new_hwnd = CreateWindowExA(0, "MAGICDECK_CardClass", "", 0x54000000, s.pt.x, s.pt.y, global_smallcard_width,
                                 global_smallcard_height, s.parent, (HMENU)1, global_hinstance, (LPVOID)s.csvid);
    SendMessageA(s.new_hwnd, 0x401, s.amt32 - 1, 0);
    BringWindowToTop(s.new_hwnd);
  }

  if (singleclick == 1)
  {
    LockWindowUpdate(global_main_hwnd);
    MapWindowPoints(NULL, global_main_hwnd, (LPPOINT)&s.r, 2);
    MoveWindow(hwnd, s.r.left, s.r.top, 0, 0, 1);
    SetParent(hwnd, global_main_hwnd);
    MoveWindow(hwnd, s.r.left, s.r.top, s.r.right - s.r.left, s.r.bottom - s.r.top, 1);
    BringWindowToTop(hwnd);
    LockWindowUpdate(NULL);
    SendMessageA(hwnd, 0x112, 0xF012, 0);
    DestroyWindow(hwnd);

    GetCursorPos(&s.pt);
    s.target = WindowFromPoint(s.pt);
    while (
        s.target != NULL &&
        s.target != global_main_hwnd &&
        s.target != global_decksurface_hwnd &&
        s.target != global_unknown_10144b04 &&
        s.target != global_unknown_10125a3c &&
        s.target != global_horzlist_hwnd)
    {
      s.target = GetParent(s.target);
    }
  }
  else
  {
    s.target = global_horzlist_hwnd;
    DestroyWindow(hwnd);
    SetFocus(global_horzlist_hwnd);
  }

  *(WORD *)&s.num = 1;
  if (global_decksurface_hwnd == s.parent)
  {
    if (shift2_unshift1 > 1)
    {
      global_dlg_parameter = s.amt32;
      if (!show_dialog_movexcards())
        global_dlg_result = 0;

      if (global_dlg_result <= 0)
      {
        SendMessageA(s.new_hwnd, 0x401, s.amt32, 0);
        *(WORD *)&s.num = 0;
      }
      else if (global_dlg_result >= s.amt32)
      {
        DestroyWindow(s.new_hwnd);
        *(WORD *)&s.num = s.amt32;
      }
      else
      {
        SendMessageA(s.new_hwnd, 0x401, s.amt32 - global_dlg_result, 0);
        *(WORD *)&s.num = (WORD)global_dlg_result;
      }
    }
    else
    {
      SendMessageA(s.new_hwnd, 0x401, s.amt32 - 1, 0);
      *(WORD *)&s.num = 1;
    }

    for (s.i = 0; s.i < (int)(s.num & 0xFFFF); s.i++)
      change_global_deck_card_availability(s.csvid, 0);

    remove_cards_from_deck(s.csvid, s.num & 0xFFFF, &global_edited_deck);
    count_packs();
    refresh_numofcards_text();
  }
  else if (global_unknown_10144b04 == s.parent)
  {
    remove_or_decrement_sideboard_bucket_entry(s.ctrl_id, (int)s.csvid, s.num & 0xffff, &global_edited_deck);
  }
  else
  {
    remove_trade_bucket_entries(s.ctrl_id, (int)s.csvid, s.num & 0xffff, &global_edited_deck);
  }

  if (s.target == global_decksurface_hwnd)
  {
    if (s.target != s.parent)
      global_deck_was_edited = 1;

    ScreenToClient(s.target, &s.pt);
    s.pt.x = s.pt.x - point.x;
    s.pt.y = s.pt.y - point.y;

    for (s.i = 0; s.i < (int)(s.num & 0xFFFF); s.i++)
    {
      if (!SendMessageA(s.target, 0x4C8, (WPARAM)s.csvid, MAKELONG(s.pt.x, s.pt.y)))
        MessageBeep(0);

      change_global_deck_card_availability(s.csvid, 2);
    }
    count_packs();
  }
  else if (s.target == global_unknown_10125a3c || s.target == global_unknown_10144b04)
  {
    if (s.target != s.parent)
      global_deck_was_edited = 1;

    ScreenToClient(s.target, &s.pt);
    if (s.target == global_unknown_10125a3c)
      *(short *)&s.num = 0;

    for (s.i = 0; s.i < (int)(s.num & 0xFFFF); s.i++)
      if (!SendMessageA(s.target, 0x4C8, (WPARAM)s.csvid, MAKELONG(s.pt.x, s.pt.y)))
        MessageBeep(0);
  }
  else if (s.target == global_horzlist_hwnd)
  {
    set_smallcard_dimensions();
    if (s.target != s.parent)
    {
      global_deck_was_edited = 1;
      if (global_cfg.effects != 0)
        play_sound(3, 400, 0, 0);
    }

    for (s.i = 0; s.i < (int)(s.num & 0xFFFF); s.i++)
      if (change_global_deck_card_availability(s.csvid, 3))
        TENTATIVE_scroll(global_listbox_hwnd, global_horzlist_hwnd);

    if ((global_db_flags_1 & 0x41) != 0)
      SendMessageA(global_horzlist_hwnd, 0x466, (WPARAM)s.csvid, 0);
  }
  else
  {
    ScreenToClient(s.target, &s.pt);
    s.pt.x = s.pt.x - point.x;
    s.pt.y = s.pt.y - point.y;

    change_global_deck_card_availability(s.csvid, 2);
    for (s.i = 0; s.i < (int)(s.num & 0xFFFF); s.i++)
      if (!SendMessageA(global_decksurface_hwnd, 0x4C8, (WPARAM)s.csvid, MAKELONG(s.pt.x, s.pt.y)))
        MessageBeep(0);

    count_packs();
  }

  if (global_edited_deck_num_entries == 0)
    global_deck_was_edited = 0;
}

// FUNCTION: DECKDLL 0x1000810e
LRESULT CALLBACK
wndproc_CardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  // GLOBAL: DECKDLL 0x100f2858
  static POINT DAT_100f2858;
  // GLOBAL: DECKDLL 0x100f2860
  static int DAT_100f2860;

  struct
  {
    RECT local_d4;        /* ebp - 0xd4 */
    HDC local_c4;         /* ebp - 0xc4 */
    PAINTSTRUCT local_c0; /* ebp - 0xc0 */
    int local_84;         /* ebp - 0x84 */
    RECT local_80;        /* ebp - 0x80 */
    MSG local_70;         /* ebp - 0x70 */
    WORD local_54;        /* ebp - 0x54 */
    POINT local_50;       /* ebp - 0x50 */
    UINT local_48;        /* ebp - 0x48 */
    char local_44[32];    /* ebp - 0x44 */
    int local_24;         /* ebp - 0x24 */
    int local_20;         /* ebp - 0x20 */
    WPARAM local_1c;      /* ebp - 0x18 */
    RECT local_18;        /* ebp - 0x14 */
    LONG local_8;         /* ebp - 0x4 */
  } s;

  switch (msg)
  {
  case WM_USER:
    return s.local_1c = GetWindowLongA(hwnd, DAT_10030000);

  case WM_USER + 1:
    s.local_54 = (WORD)wparam;
    SetWindowWord(hwnd, DAT_10030004, s.local_54);
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;

  case WM_USER + 2:
    s.local_54 = GetWindowWord(hwnd, DAT_10030004);
    return s.local_54 & 0xffff;

  case WM_COMMAND:
    switch (wparam & 0xffff)
    {
    case RES_SELLMENU_SELLCARDFORX:
      s.local_8 = GetWindowLongA(hwnd, DAT_10030000);
      s.local_54 = GetWindowWord(hwnd, DAT_10030004);
      global_dlg_result = 1;
      global_dlg_parameter = 1;

      if ((1 < (WORD)s.local_54) && (show_dialog_sellxcards() == 0))
        return 0;
      if (global_dlg_result == 0)
        return 0;

      if (GetWindowWord(hwnd, DAT_10030004) < global_dlg_result)
      {
        global_dlg_result = GetWindowWord(hwnd, DAT_10030004);
      }

      s.local_20 = 0;
      s.local_48 = 0;
      for (; ((int)s.local_48 < global_dlg_result) && (s.local_20 == 0); s.local_48++)
      {
        if (delete_card_from_global_deck(s.local_8, 0) == 0)
          s.local_20 = 1;
      }
      *Gold = *Gold + DAT_100f2860 * global_dlg_result;

      count_packs();

      load_text(s_menus_10031824, s_PRICE_1003181c);
      sprintf(s.local_44, text_lines[0], *Gold);
      strncpy(global_deckinfo.deckname, s.local_44, 0xC);
      InvalidateRect(global_title_hwnd, NULL, TRUE);

      if (global_dlg_result < GetWindowWord(hwnd, DAT_10030004))
      {
        SetWindowWord(hwnd, DAT_10030004, s.local_54 - global_dlg_result);
        InvalidateRect(hwnd, NULL, FALSE);
      }
      else
        DestroyWindow(hwnd);

      GetClientRect(global_decksurface_hwnd, &s.local_18);
      LockWindowUpdate(global_main_hwnd);
      reset_dim_and_pos_all_cards(global_decksurface_hwnd, &s.local_18);
      LockWindowUpdate(NULL);
      refresh_numofcards_text();
    }

    return 0;
  case WM_CREATE:
    s.local_1c = *(LONG *)lparam;
    SetWindowLongA(hwnd, DAT_10030000, s.local_1c);
    s.local_54 = 1;
    SetWindowWord(hwnd, DAT_10030004, s.local_54);
    if ((int)s.local_1c < 0 || ((int)s.local_1c > global_available_slots))
      return -1;
    return 0;

  case WM_DESTROY:
    return 0;

  case WM_MOUSEMOVE:
    s.local_1c = GetWindowLongA(hwnd, DAT_10030000);
    SendMessageA(global_fullcard_hwnd, WM_USER, s.local_1c, 0);
    return 0;

  case WM_LBUTTONDBLCLK:
  {
    if ((global_db_flags_2 & 1) == 0)
      return 0;

    DAT_100f2858.x = GET_X_LPARAM(lparam);
    DAT_100f2858.y = GET_Y_LPARAM(lparam);
    if ((wparam & MK_SHIFT) != 0)
      TENTATIVE_move_from_owned_cards(hwnd, DAT_100f2858, 2, 0);
    else
      TENTATIVE_move_from_owned_cards(hwnd, DAT_100f2858, 1, 0);
    SendMessageA(global_decksurface_hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    return 0;
  }

  case WM_LBUTTONDOWN:
  {
    if ((global_db_flags_2 & 1) == 0)
      return 0;

    s.local_48 = GetDoubleClickTime();
    Sleep(s.local_48);
    if (PeekMessageA(&s.local_70, hwnd, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, 0) != 0)
      return 0;

    DAT_100f2858.x = GET_X_LPARAM(lparam);
    DAT_100f2858.y = GET_Y_LPARAM(lparam);
    if ((wparam & MK_SHIFT) != 0)
      TENTATIVE_move_from_owned_cards(hwnd, DAT_100f2858, 2, 1);
    else
      TENTATIVE_move_from_owned_cards(hwnd, DAT_100f2858, 1, 1);
    SendMessageA(global_decksurface_hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    return 0;
  }

  case WM_PAINT:
  {
    s.local_54 = GetWindowWord(hwnd, DAT_10030004);
    s.local_1c = GetWindowLongA(hwnd, DAT_10030000);
    s.local_c4 = BeginPaint(hwnd, &s.local_c0);
    GetClientRect(hwnd, &s.local_80);
    ApplyCardArtPaletteToDc(s.local_c4);
    FillRect(global_hdc, &s.local_80, global_create_brush_0);
    DrawSmallCard(global_hdc, &s.local_80, global_raw_cards_storage + (int)s.local_1c, 1, 1);
    if (1 < (int)(s.local_54 & 0xFFFF))
    {
      sprintf(s.local_44, DAT_1003182c, (unsigned int)(s.local_54 & 0xFFFF));
      s.local_84 = SaveDC(global_hdc);
      SetMapMode(global_hdc, MM_ANISOTROPIC);
      SetWindowExtEx(global_hdc, 200, 0x118, NULL);
      SetViewportExtEx(global_hdc,
                       s.local_80.right - s.local_80.left,
                       s.local_80.bottom - s.local_80.top,
                       NULL);
      SetViewportOrgEx(global_hdc, s.local_80.left, s.local_80.top, NULL);
      SetBkMode(global_hdc, TRANSPARENT);
      SelectObject(global_hdc, DAT_10125e0c);
      SetTextAlign(global_hdc, 0);
      SetTextColor(global_hdc, PALETTERGB(0x43, 0x43, 0x43));

      TextOutA(global_hdc, 200 - (int)strlen(s.local_44) * 0x10 + 3, 3, s.local_44, (int)strlen(s.local_44));
      SetTextColor(global_hdc, PALETTERGB(0xF1, 0xD9, 0x27));
      SetBkMode(global_hdc, TRANSPARENT);

      TextOutA(global_hdc, 200 - (int)strlen(s.local_44) * 0x10, 0, s.local_44, (int)strlen(s.local_44));
      RestoreDC(global_hdc, s.local_84);
    }
    BitBlt(s.local_c4, 0, 0, s.local_80.right, s.local_80.bottom, global_hdc, 0, 0, SRCCOPY);
    EndPaint(hwnd, &s.local_c0);
    return 0;
  }

  case WM_RBUTTONDOWN:
  {
    s.local_50.x = GET_X_LPARAM(lparam);
    s.local_50.y = GET_Y_LPARAM(lparam);
    if ((global_db_flags_1 & DBFLAGS_STANDALONE) != 0)
    {
      ClientToScreen(hwnd, &s.local_50);
      ScreenToClient(GetParent(hwnd), &s.local_50);
      SendMessageA(GetParent(hwnd), msg, wparam,
                   (LPARAM)((unsigned short)s.local_50.y << 16) | (unsigned short)s.local_50.x);
      return 0;
    }
    else if ((global_db_flags_2 & DBFLAGS_STANDALONE) == 0)
      return 0;

    s.local_8 = GetWindowLongA(hwnd, DAT_10030000);
    s.local_24 = get_iid_from_global_deck_card(s.local_8);
    if (s.local_24 == -1)
      return 0;
    DAT_100f2860 = SellPrice(s.local_24);
    load_text(s_menus_1003183c, s_SELLCARD_10031830);
    sprintf(s.local_44, text_lines[0], DAT_100f2860);
    AppendMenuA(global_smallcard_popup, 0, RES_SELLMENU_SELLCARDFORX, s.local_44);
    ClientToScreen(hwnd, &s.local_50);
    TrackPopupMenu(global_smallcard_popup, TPM_RIGHTBUTTON, s.local_50.x + 10, s.local_50.y + 10, 0, hwnd, NULL);
    DeleteMenu(global_smallcard_popup, 0, MF_BYPOSITION);

    return 0;
  }

  case WM_SIZE:
    GetWindowRect(hwnd, &s.local_d4);
    global_smallcard_width = s.local_d4.right - s.local_d4.left;
    global_smallcard_height = s.local_d4.bottom - s.local_d4.top;
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}

// FUNCTION: DECKDLL 0x10019d9f
static void copy_deck_to_edit(void)
{
  struct
  {
    int i;         /* ebp - 0x8 */
    csvid_t csvid; /* ebp - 0x4 */
  } s;
  global_deck_num_entries = 0;
  global_deck_num_entries_copy = global_deck_num_entries;
  s.i = global_deck_num_entries_copy;
  global_deck_num_cards = 0;
  global_edited_deck_num_entries = 0;
  while (s.i < 0x1f4)
  {
    s.csvid = CardIDFromType(deck[s.i]);
    if (s.csvid == -1)
    {
      count_packs();
      return;
    }

    global_deck[s.i].GDE_csvid = s.csvid;
    global_deck[s.i].GDE_iid = deck[global_deck_num_entries] & 0xFFF;
    global_deck[s.i].GDE_Available = 1;
    global_deck[s.i].GDE_DecksBits = (deck[global_deck_num_entries] & 0x70000) >> 16;

    if ((deck[global_deck_num_entries] & (1 << (global_current_deck + 0x10))) != 0)
    {
      insert_cards_into_deck(global_deck[s.i].GDE_csvid, 1, &global_edited_deck);
      global_deck[s.i].GDE_Available = 0;
    }

    ++global_deck_num_entries;
    ++s.i;
  }
}

// FUNCTION: DECKDLL 0x1000e770
static bool
save_deck(const char *filename)
{
  struct
  {
    FILE *f;    /* ebp - 0xc */
    int i;      /* ebp - 0x8 */
    int result; /* ebp - 0x4 */
  } s;

  s.f = fopen(filename, "wt");
  if (s.f != 0)
  {
    fprintf(s.f, ";%s\n", global_deckinfo.deckname);
    fprintf(s.f, ";%s\n", global_deckinfo.description);
    fprintf(s.f, ";%s\n", global_deckinfo.author);
    fprintf(s.f, ";%s\n", global_deckinfo.email);
    fprintf(s.f, ";%s\n", global_deckinfo.creation_date);
    fprintf(s.f, ";%d\n", global_deckinfo.revision);
    fprintf(s.f, ";%s\n", global_deckinfo.edition);
    fprintf(s.f, ";%s\n", global_deckinfo.comments);
    fprintf(s.f, "\n");

    for (s.i = 0; s.i < global_edited_deck_num_entries; ++s.i)
      fprintf(s.f, ".%d\t%d\t%s\n",
              global_edited_deck.entries[s.i].DeckEntry_csvid,
              global_edited_deck.entries[s.i].DeckEntry_Amount,
              global_edited_deck.entries[s.i].DeckEntry_FullName);

    fclose(s.f);
    s.result = 1;
  }
  else
    s.result = 0;

  return s.result;
}

// FUNCTION: DECKDLL 0x1000e90f
static int
load_deck(char *filename)
{
  struct
  {
    size_t len_comments;  /* ebp - 0x224 */
    size_t len_edition;   /* ebp - 0x220 */
    size_t len_revision;  /* ebp - 0x21c */
    size_t len_date;      /* ebp - 0x218 */
    size_t len_email;     /* ebp - 0x214 */
    size_t len_author;    /* ebp - 0x210 */
    size_t len_deckname2; /* ebp - 0x20c */
    size_t len_deckname1; /* ebp - 0x208 */
    FILE *f;              /* ebp - 0x204 */
    char txt[500];        /* ebp - 0x200 */
    int ok;               /* ebp - 0xc */
    int num;              /* ebp - 0x8 */
    csvid_t csvid;        /* ebp - 0x4 */
  } s;

  s.f = fopen(filename, "rt");
  if (s.f != 0)
  {
    memset(&global_edited_deck, 0, 0x1464);

    if (global_db_flags_1 & DBFLAGS_GAUNTLET)
    {
      global_deck_num_entries_copy = 0;
      global_deck_num_entries = global_deck_num_entries_copy;
    }

    fgets(s.txt, 0x1f, s.f);
    if (s.txt[0] != ';')
      return 0;
    s.len_deckname1 = strlen(s.txt);
    s.txt[s.len_deckname1 - 1] = 0;
    strncpy(global_deckinfo.deckname, &s.txt[1], 0x1f);

    fgets(s.txt, 0x15, s.f);
    if (s.txt[0] != ';')
      return 0;
    s.len_deckname2 = strlen(s.txt);
    s.txt[s.len_deckname2 - 1] = 0;
    strncpy(global_deckinfo.description, &s.txt[1], 0x15);

    fgets(s.txt, 0x51, s.f);
    if (s.txt[0] != ';')
      return 0;
    s.len_author = strlen(s.txt);
    s.txt[s.len_author - 1] = 0;
    strncpy(global_deckinfo.author, &s.txt[1], 0x51);

    fgets(s.txt, 0x51, s.f);
    if (s.txt[0] != ';')
      return 0;
    s.len_email = strlen(s.txt);
    s.txt[s.len_email - 1] = 0;
    strncpy(global_deckinfo.email, &s.txt[1], 0x51);

    fgets(s.txt, 0x16, s.f);
    if (s.txt[0] != ';')
      return 0;
    s.len_date = strlen(s.txt);
    s.txt[s.len_date - 1] = 0;
    strncpy(global_deckinfo.creation_date, &s.txt[1], 0x16);

    fgets(s.txt, 0x10, s.f);
    s.len_revision = strlen(s.txt);
    s.txt[s.len_revision - 1] = 0;
    if (s.txt[0] != ';')
      return 0;
    global_deckinfo.revision = atoi(&s.txt[1]);

    fgets(s.txt, 0x10, s.f);
    if (s.txt[0] != ';')
      return 0;
    s.len_edition = strlen(s.txt);
    s.txt[s.len_edition - 1] = 0;
    strncpy(global_deckinfo.edition, &s.txt[1], 0x10);

    fgets(s.txt, 0x191, s.f);
    if (s.txt[0] != ';')
      return 0;
    s.len_comments = strlen(s.txt);
    s.txt[s.len_comments - 1] = 0;
    strncpy(global_deckinfo.comments, &s.txt[1], 0x191);

    while (1 && (int)FUN_1000edea(s.f, s.txt) != -1 && FUN_1000ee98(s.txt) == 0)
    {
      if (FUN_1000ee5d(s.txt))
      {
        sscanf(s.txt, ".%d %d", &s.csvid, &s.num);
        if (s.num != 0)
          insert_cards_into_deck(s.csvid, s.num, &global_edited_deck);
        else
          append_trade_bucket_entry(6, s.csvid, s.num, &global_edited_deck);
      }
    }

    fclose(s.f);
    s.ok = 1;
  }
  else
  {
    s.ok = 0;
  }

  if (s.ok != 0)
  {
    count_packs();
    s.ok = SendMessage(global_decksurface_hwnd, 0x401, 0, 0);
  }
  return s.ok;
}

// FUNCTION: DECKDLL 0x1000edea
static int FUN_1000edea(FILE *file, char *dest)
{
  int len;
  int ch;

  for (len = 0; (ch = fgetc(file)) != '\n' && ch != -1;)
  {
    *dest = (char)ch;
    dest++;
    len++;
  }

  *dest = 0;
  if (ch == -1)
    return -1;
  else
    return len;
}

// FUNCTION: DECKDLL 0x1000ee5d
static int FUN_1000ee5d(char *line)
{
  if (*line == '.' && line[1] != 'v')
    return 1;
  return 0;
}

// FUNCTION: DECKDLL 0x1000ee98
static int FUN_1000ee98(char *line)
{
  if (*line == 'v')
    return 1;
  if (*line == '.' && line[1] == 'v')
    return 1;
  return 0;
}

// FUNCTION: DECKDLL 0x1002b7bb
static int
check_card_count(int csvid)
{
  struct
  {
    int i;
    unsigned int limit;
  } s;

  s.limit = Scards[5].worldmagic_city == 0;

  if (global_deck_num_cards < 59)
  {
    for (s.i = 0; s.i < global_edited_deck_num_entries; ++s.i)
    {
      if (global_edited_deck.entries[s.i].DeckEntry_csvid == csvid && CardTypeFromID(csvid) > 4)
        return global_edited_deck.entries[s.i].DeckEntry_Amount - (s.limit + 3);
    }
  }
  else
  {
    for (s.i = 0; s.i < global_edited_deck_num_entries; ++s.i)
    {
      if (global_edited_deck.entries[s.i].DeckEntry_csvid == csvid && CardTypeFromID(csvid) > 4)
        return global_edited_deck.entries[s.i].DeckEntry_Amount - (s.limit * 99 + 4);
    }
  }

  return 0;
}

// FUNCTION: DECKDLL 0x10024e48
void FUN_10024e48(DRAWITEMSTRUCT *item, HBRUSH brush, HPEN pen1, HPEN pen2, COLORREF col, int do_focus)
{
  draw_item(item, brush, (HANDLE)0x0, pen1, pen2, col, do_focus, 0x25);
}

// FUNCTION: DECKDLL 0x1000942e
static void FUN_1000942e(HWND hwnd, int csvid)
{
  (void)hwnd;
  (void)csvid;
}

struct WndprocMainClassLocals
{
  int border_adjust;     /* ebp - 0xb40 */
  int margin8;           /* ebp - 0xb3c */
  int fullcard_height;   /* ebp - 0xb38 */
  int horzlist_hgt6;     /* ebp - 0xb34 */
  int decksurface_x;     /* ebp - 0xb30 */
  LONG horzlist_style;   /* ebp - 0xb2c */
  int title_height;      /* ebp - 0xb28 */
  RECT rect_load_button; /* ebp - 0xb24 */
  int decksurface_top;   /* ebp - 0xb14 */

  int main_h; /* ebp - 0xb10 */
  int ten;    /* ebp - 0xb0c */

  RECT rect_cardlistfilter; /* ebp - 0xb08 */
  HDC pad_b0c;              /* ebp - 0xb04 */
  int main_w;               /* ebp - 0xaf4 */
  int cardlistfilter_hgt;   /* ebp - 0xaf0 */

  RECT rect_title;    /* ebp - 0xaec */
  RECT rect_pad_adc;  /* ebp - 0xadc */
  RECT rect_fullcard; /* ebp - 0xacc */
  int third_w;        /* ebp - 0xabc */

  SIZE sz;               /* ebp - 0xab8 */
  RECT rect_horzlist;    /* ebp - 0xab0 */
  RECT rect_decksurface; /* ebp - 0xaa0 */
  POINT pad_a90;         /* ebp - 0xa90 */

  HWND hwnd_a8c; /* ebp - 0xa8c */
  MEASUREITEMSTRUCT *pad_a8c;
  HWND pad_a88;

  RECT rect_stats; /* ebp - 0xa84 */
  BITMAP bmp_a6c;  /* ebp - 0xa6c */
  int fill_y;      /* ebp - 0xa54 */
  int pics_inline; /* ebp - 0xa50 */

  RECT r;     /* ebp - 0xa4c */
  int fill_x; /* ebp - 0xa3c */

  COLORREF item_col; /* ebp - 0xa38 */
  HWND tgt;          /* ebp - 0xa38 */
  int cursel;        /* ebp - 0xa34 */
  HDC pad_a30;

  HWND hwnd2; /* ebp - 0xa28 */
  HWND hwnd1; /* ebp - 0xa24 */

  HDC pad_a20;
  HGDIOBJ pad_a1c;
  HWND pad_a18;
  HDC pad_a14;
  WPARAM pad_a10;

  int csvidraw;     /* ebp - 0xa0c */
  int csvid;        /* ebp - 0xa08 */
  int unique_count; /* ebp - 0xa04 */

  int local_a00[100];   /* ebp - 0xa00 */
  int pad_870;          /* ebp - 0x870 */
  char local_86c[2000]; /* ebp - 0x86c */

  HWND cmd_hwnd;        /* ebp - 0x9c */
  HWND cmd_tgt;         /* ebp - 0x98 */
  int cmd_cursel;       /* ebp - 0x94 */
  LRESULT cmd_itemdata; /* ebp - 0x90 */

  HDC hdc_local;    /* ebp - 0x8c */
  HDC hdc;          /* ebp - 0x88 */
  WPARAM wparam_84; /* ebp - 0x84 */

  LPARAM lparam_copy; /* ebp - 0x80 */
  WPARAM wparam_copy; /* ebp - 0x7c */

  WORD wPicsInline; /* ebp - 0x78 */
  WORD wPicsInlinePad;
  FILE *f;               /* ebp - 0x74 */
  int *horz_list_addr;   /* ebp - 0x70 */
  int i;                 /* ebp - 0x6c */
  unsigned int artist_i; /* ebp - 0x68 */

  HDC fill_chdc; /* ebp - 0x64 */
  char buf[76];  /* ebp - 0x60 */

  unsigned int pad_14;

  WORD wLeft;       /* ebp - 0x10 */
  WORD wLeftPad;    /* ebp - 0xe */
  int excess;       /* ebp - 0xc */
  int check_excess; /* ebp - 8 */
  WORD wLast;       /* ebp - 4 */
  WORD wLastPad;    /* ebp - 2 */
};

// FUNCTION: DECKDLL 0x10019ed5
static void
FUN_10019ed5(int csvid, int *list, int *count)
{
  int i;

  for (i = 0; i < *count; ++i)
  {
    if (list[i] == csvid)
      return;
  }

  list[*count] = csvid;
  *count = *count + 1;
}

// FUNCTION: DECKDLL 0x10016e4e
LRESULT CALLBACK
wndproc_MainClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct WndprocMainClassLocals s;
  // GLOBAL: DECKDLL 0x10104c98
  static HPEN pen_ltgrey = NULL;
  // GLOBAL: DECKDLL 0x10104c9c
  static HPEN pen_medgrey = NULL;
  // GLOBAL: DECKDLL 0x10104ca4
  static HGDIOBJ mainclass_destroy_obj = NULL;
  // wndproc_MainClass statics
  // GLOBAL: DECKDLL 0x101a8c24
  static HWND button_deckinfo;
  // GLOBAL: DECKDLL 0x1019e9d0
  static HWND button_exit;
  // GLOBAL: DECKDLL 0x1014263c
  static HWND button_deck1;
  // GLOBAL: DECKDLL 0x1014250c
  static HWND button_deck2;
  // GLOBAL: DECKDLL 0x101423cc
  static HWND button_deck3;
  // GLOBAL: DECKDLL 0x10104b8c
  static HMENU popup;
  // GLOBAL: DECKDLL 0x10035208
  static const char wndname_title[] = "";
  // GLOBAL: DECKDLL 0x10035224
  static const char wndname_fullcard[] = "";
  // GLOBAL: DECKDLL 0x1003523c
  static const char wndname_decksurface[] = "";
  // GLOBAL: DECKDLL 0x1003525c
  static const char wndname_button_stats[] = "";
  // GLOBAL: DECKDLL 0x10035268
  static const char wndname_button_deckinfo[] = "";
  // GLOBAL: DECKDLL 0x10035274
  static const char wndname_button_exit[] = "";
  // GLOBAL: DECKDLL 0x10035280
  static const char wndname_button_deck1[] = "";
  // GLOBAL: DECKDLL 0x1003528c
  static const char wndname_button_deck2[] = "";
  // GLOBAL: DECKDLL 0x10035298
  static const char wndname_button_deck3[] = "";
  // GLOBAL: DECKDLL 0x100352a4
  static const char wndname_listbox[] = "";
  // GLOBAL: DECKDLL 0x100352b0
  static const char wndname_horzlist[] = "";
  // GLOBAL: DECKDLL 0x100352cc
  static const char wndname_cardlistfilter[] = "";

  // GLOBAL: DECKDLL 0x10034f08
  static LOGFONT mainClass_fontTemplate = {
      0,         // lfHeight
      0,         // lfWidth
      0,         // lfEscapement
      0,         // lfOrientation
      FW_NORMAL, // lfWeight
  };
  // GLOBAL: DECKDLL 0x10104b50
  static LOGFONT DAT_10104b50;

  switch (msg)
  {
  case 0x466:
    s.wparam_copy = wparam;
    s.lparam_copy = lparam;
    if ((HWND)s.lparam_copy != global_fullcard_hwnd)
      SendMessage(global_fullcard_hwnd, 0x466, s.wparam_copy, 0);
    return 0;

  case WM_PALETTECHANGED:
    s.wparam_84 = wparam;
    if (hwnd != (HWND)s.wparam_84)
    {
      if (global_main_hwnd != (HWND)s.wparam_84)
      {
        s.hdc = GetDC(hwnd);
        SelectPalette(s.hdc, global_cart_art_hpalette, TRUE);
        if (RealizePalette(s.hdc))
          InvalidateRect(hwnd, NULL, FALSE);
        ReleaseDC(hwnd, s.hdc);
      }
    }
    return 0;

  case WM_QUERYNEWPALETTE:
    s.hdc_local = GetDC(hwnd);
    SelectPalette(s.hdc_local, global_cart_art_hpalette, FALSE);
    if (RealizePalette(s.hdc_local))
      InvalidateRect(hwnd, NULL, FALSE);
    ReleaseDC(hwnd, s.hdc_local);
    return 1;

  case WM_CLOSE:
    if ((global_db_flags_1 & 0x0a) || (global_db_flags_1 & 4))
    {
      if (!global_deck_was_edited || ask_about_saving_deck() != 2)
        DestroyWindow(global_main_hwnd);
    }
    else
    {
      s.artist_i = 0;
      s.i = 0;
      s.excess = 0;
      for (; global_edited_deck_num_entries > (int)s.artist_i; ++s.artist_i)
      {
        s.check_excess = check_card_count(global_edited_deck.entries[s.artist_i].DeckEntry_csvid);
        if (s.check_excess > 0)
        {
          global_excessive_cards[s.i].DeckEntry_csvid = global_edited_deck.entries[s.artist_i].DeckEntry_csvid;
          global_excessive_cards[s.i].DeckEntry_Amount = s.check_excess;
          global_excessive_cards[s.i].DeckEntry_FullName =
              global_raw_cards_storage[global_edited_deck.entries[s.artist_i].DeckEntry_csvid].full_name;
          ++s.i;
          s.excess = 1;
        }
      }
      global_excessive_cards[s.i].DeckEntry_csvid = -1;

      if (s.excess)
      {
        if (show_dialog_infobox())
          DestroyWindow(global_main_hwnd);
      }
      else
        DestroyWindow(global_main_hwnd);
    }
    return 0;

  case WM_COMMAND:
  {
    switch ((int)wparam & 0xffff)
    {
    case RES_UNK:
      if (global_deckbuilder_unknown_1019f010 != 0)
        global_deckbuilder_unknown_1019f010 = 0;
      else
        global_deckbuilder_unknown_1019f010 = 1;
      break;

    case 4:
    case 5:
    {
      s.cmd_hwnd = (HWND)lparam;
      if (((unsigned int)wparam & 0xffff) == 4)
        s.cmd_tgt = global_horzlist_hwnd;
      else
        s.cmd_tgt = global_listbox_hwnd;

      if ((((unsigned int)wparam >> 16) & 0xffff) == 1)
      {
        s.cmd_cursel = SendMessage(s.cmd_hwnd, LB_GETCURSEL, 0, 0);
        s.cmd_itemdata = SendMessage(s.cmd_hwnd, LB_GETITEMDATA, s.cmd_cursel, 0);
        SendMessage(global_fullcard_hwnd, 0x400, s.cmd_itemdata, 0);

        s.cmd_cursel = SendMessage(s.cmd_hwnd, LB_GETCURSEL, 0, 0);
        SendMessage(s.cmd_tgt, LB_SETCURSEL, s.cmd_cursel, 0);
      }
      break;
    }

    case 10:
    {
      s.unique_count = 0;

      for (s.local_a00[0] = 0; global_edited_deck_num_entries > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);

      for (s.local_a00[0] = 0; global_edited_deck.trade[0].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.trade[0].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);
      for (s.local_a00[0] = 0; global_edited_deck.sideboard[0].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.sideboard[0].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);

      for (s.local_a00[0] = 0; global_edited_deck.trade[1].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.trade[1].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);
      for (s.local_a00[0] = 0; global_edited_deck.sideboard[1].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.sideboard[1].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);

      for (s.local_a00[0] = 0; global_edited_deck.trade[2].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.trade[2].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);
      for (s.local_a00[0] = 0; global_edited_deck.sideboard[2].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.sideboard[2].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);

      for (s.local_a00[0] = 0; global_edited_deck.trade[3].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.trade[3].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);
      for (s.local_a00[0] = 0; global_edited_deck.sideboard[3].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.sideboard[3].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);

      for (s.local_a00[0] = 0; global_edited_deck.trade[4].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.trade[4].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);
      for (s.local_a00[0] = 0; global_edited_deck.sideboard[5].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.sideboard[5].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);

      for (s.local_a00[0] = 0; global_edited_deck.trade[5].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.trade[5].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);
      for (s.local_a00[0] = 0; global_edited_deck.sideboard[4].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.sideboard[4].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);

      for (s.local_a00[0] = 0; global_edited_deck.trade[6].total > s.local_a00[0]; ++s.local_a00[0])
        FUN_10019ed5(global_edited_deck.trade[6].entries[s.local_a00[0]].DeckEntry_csvid, &s.local_a00[1], &s.unique_count);

      s.csvid = 0;
      strcpy(s.local_86c, "The following cards are not in 4th edition:\n");

      for (s.local_a00[0] = 0; s.local_a00[0] < s.unique_count; ++s.local_a00[0])
      {
        if (!check_filters(s.local_a00[s.local_a00[0] + 1]))
        {
          strcat(s.local_86c, global_raw_cards_storage[s.local_a00[s.local_a00[0] + 1]].full_name);
          strcat(s.local_86c, "\n");
          ++s.csvid;
        }
      }

      if (s.csvid)
        MessageBoxA(hwnd, s.local_86c, "Checking deck for legal cards", 0);
      else
        MessageBoxA(hwnd, "All cards are in 4th edition", "Checking deck for legal cards", 0);
      break;
    }

    case RES_MAINMENU_NEWDECK:
      SendMessage(global_decksurface_hwnd, RES_MENU_NEWDECK, 0, 0);
      break;

    case RES_MAINMENU_LOADDECK:
      if ((!global_deck_was_edited || ask_about_saving_deck() != 2) && show_dialog_loaddeck(global_deck_filename))
      {
        if (load_deck(global_deck_filename))
        {
          InvalidateRect(global_title_hwnd, NULL, TRUE);
          global_deck_was_edited = false;
        }
        else
        {
          load_text("menus", "DECKLOADERROR");
          sprintf(s.buf, text_lines[0], global_deck_filename);

          load_text("menus", "DECKBUILDER");
          MessageBoxA(hwnd, s.buf, text_lines[0], MB_ICONHAND);

          load_text("menus", "NEWDECK");
          sprintf(global_deckinfo.deckname, text_lines[0]);

          strcpy(global_deck_filename, global_playdeck_path);
          strcat(global_deck_filename, "\\new.dck");
        }
      }
      break;

    case RES_MAINMENU_SAVEDECK:
      load_text("menus", "NEWDECK");
      if (!strcmp(global_deckinfo.deckname, text_lines[0]) || !global_deckinfo.deckname[0])
      {
        load_text("menus", "NAMEYOURDECK");
        strcpy(s.buf, text_lines[0]);
        load_text("menus", "DECKBUILDER");
        MessageBoxA(hwnd, s.buf, text_lines[0], MB_ICONINFORMATION);
        show_dialog_deckinfo();
        return 2;
      }

      if (global_deck_num_cards < 40)
      {
        load_text("menus", "TOOFEWCARDS");
        strcpy(s.buf, text_lines[0]);
        load_text("menus", "DECKBUILDER");
        MessageBoxA(hwnd, s.buf, text_lines[0], MB_ICONINFORMATION);
        return 2;
      }

      if (global_deck_num_cards > 500 || global_edited_deck_num_entries > 200)
      {
        load_text("menus", "TOOMANYCARDS");
        strcpy(s.buf, text_lines[0]);
        load_text("menus", "DECKBUILDER");
        MessageBoxA(hwnd, s.buf, text_lines[0], MB_ICONINFORMATION);
      }

      strcpy(global_deck_filename, global_playdeck_path);
      strcat(global_deck_filename, "\\");
      strcat(global_deck_filename, global_deckinfo.deckname);
      strcat(global_deck_filename, ".dck");

      if (global_deckname_set)
      {
        s.f = fopen(global_deck_filename, "rt");
        if (s.f != NULL)
        {
          fclose(s.f);

          load_text("menus", "DECKEXISTS");
          sprintf(s.buf, text_lines[0], global_deckinfo.deckname);
          load_text("menus", "DECKBUILDER");
          s.artist_i = MessageBoxA(hwnd, s.buf, text_lines[0], MB_ICONQUESTION | MB_YESNOCANCEL);
          if (s.artist_i == IDCANCEL || s.artist_i == IDNO)
            return 2;
        }
      }

      global_deckname_set = false;
      global_deck_was_edited = false;

      if (save_deck(global_deck_filename))
      {
        load_text("menus", "SAVED");
        sprintf(s.buf, text_lines[0], global_deckinfo.deckname);
        load_text("menus", "DECKBUILDER");
        MessageBoxA(hwnd, s.buf, text_lines[0], MB_ICONINFORMATION);
      }
      else
      {
        load_text("menus", "DECKSAVEERROR");
        sprintf(s.buf, text_lines[0], global_deckinfo.deckname);
        load_text("menus", "DECKBUILDER");
        MessageBoxA(hwnd, s.buf, text_lines[0], MB_ICONINFORMATION);
      }

      if (global_db_flags_1 & (DBFLAGS_EDITDECK | DBFLAGS_GAUNTLET | DBFLAGS_NOCARDCOUNTCHECK))
        strcpy(global_external_deckname, global_deckinfo.deckname);

      return 6;

    case RES_FULLCARDMENU_EXPAND:
      SendMessage(global_fullcard_hwnd, msg, wparam, lparam);
      break;

    case RES_MENU_CONSOLIDATE:
      SendMessage(global_decksurface_hwnd, msg, wparam, lparam);
      break;

    case RES_MAINMENU_EFFECTS:
      global_cfg.effects ^= 1;
      break;

    case RES_MAINMENU_MUSIC:
      global_cfg.music ^= 1;
      if (global_cfg.music)
        play_music(1, 400, 0);
      else
        sound_stop(1);
      break;

    case RES_MAINMENU_MINIMIZE:
      SendMessage(global_main_hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
      break;

    case RES_MAINMENU_EXIT:
    case RES_MAINMENU_BUTTON_EXIT:
      SendMessage(global_main_hwnd, WM_CLOSE, 0, 0);
      break;

    case RES_MAINMENU_BUTTON_STATS:
      DialogBoxParam(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_DECKSTATS), global_main_hwnd, dlgproc_DeckStats, 0);
      break;

    case RES_MAINMENU_BUTTON_DECKINFO:
      show_dialog_deckinfo();
      InvalidateRect(global_title_hwnd, 0, TRUE);
      break;

    case RES_MAINMENU_BUTTON_DECK_1:
    case RES_MAINMENU_BUTTON_DECK_2:
    case RES_MAINMENU_BUTTON_DECK_3:
      if (!(global_db_flags_2 & DBFLAGS_SHANDALAR))
        break;

      load_text("menus", "DECKNUMBERS");
      SetWindowText(button_deck1, text_lines[0]);
      SetWindowText(button_deck2, text_lines[1]);
      SetWindowText(button_deck3, text_lines[2]);

      global_current_deck = (((unsigned int)wparam & 0xffff) - RES_MAINMENU_BUTTON_DECK_1);
      global_deck_num_cards = 0;
      global_edited_deck_num_entries = 0;

      for (s.artist_i = 0; global_deck_num_entries > (int)s.artist_i; ++s.artist_i)
        if (global_deck[s.artist_i].GDE_DecksBits & (1 << global_current_deck))
        {
          global_deck[s.artist_i].GDE_Available = 0;
          insert_cards_into_deck(global_deck[s.artist_i].GDE_csvid, 1, &global_edited_deck);
        }
        else
          global_deck[s.artist_i].GDE_Available = 1;

      count_packs();

      SendMessage(global_decksurface_hwnd, 0x401, 0, 0);

      filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);

      if (global_current_deck == 0)
        SetWindowText(button_deck1, text_lines[3]);
      else if (global_current_deck == 1)
        SetWindowText(button_deck2, text_lines[4]);
      else if (global_current_deck == 2)
        SetWindowText(button_deck3, text_lines[5]);

      SetFocus(global_horzlist_hwnd);

      break;
    default:
      break;
    }
    return 0;
  }

  case WM_CREATE:
    if (check_filters_debug_log_enabled != 0)
    {
      check_filters_debug_log_file = fopen("CardIDs.TXT", "wt");
      if (check_filters_debug_log_file == NULL)
      {
        check_filters_debug_log_enabled = 0;
        MessageBoxA(hwnd, "Could not create CARDID.TXT", "Deck Builder Error", MB_ICONHAND);
      }
    }

    global_colorref_lightgrey = PALETTEINDEX(7);
    global_colorref_darkgrey = PALETTEINDEX(1);
    global_brush_mediumgrey = CreateSolidBrush(PALETTEINDEX(0x86));
    pen_ltgrey = CreatePen(0, 0, PALETTEINDEX(0xC1));
    pen_medgrey = CreatePen(0, 0, PALETTEINDEX(0x16));
    global_colorref_flesh = PALETTEINDEX(1);
    global_colorref_white = PALETTEINDEX(0xBF);

    memcpy(&DAT_10104b50, &mainClass_fontTemplate, sizeof(LOGFONT));
    DAT_10104b50.lfHeight = 0x2a;
    DAT_10104b50.lfWeight = 500;
    strcpy(DAT_10104b50.lfFaceName, "Kudos Condensed SSi");
    DAT_10125e0c = CreateFontIndirectA(&DAT_10104b50);

    if (!(global_db_flags_1 & DBFLAGS_SHANDALAR))
      init_sound_dll(hwnd, 0, 0);

    init_sounds_and_music();

    strcpy(global_deck_filename, global_playdeck_path);
    strcat(global_deck_filename, "\\new.dck");

    global_num_artists = load_text("menus", "ARTISTNAMES");
    for (s.artist_i = 0; global_num_artists > (int)s.artist_i; s.artist_i = s.artist_i + 1)
      strcpy(read_db_artist_names[s.artist_i], text_lines[s.artist_i]);

    if (global_db_flags_1 & DBFLAGS_SHANDALAR)
    {
      load_text("menus", "GOLDTITLE");
      sprintf(s.buf, text_lines[0], *Gold);
      strncpy(global_deckinfo.deckname, s.buf, 12);
    }
    else if (global_db_flags_1 & DBFLAGS_EDITDECK)
    {
      global_deckinfo.deckname[0] = ' ';
      global_deckinfo.deckname[1] = '\0';
    }
    else
    {
      load_text("menus", "NEWDECK");
      sprintf(global_deckinfo.deckname, text_lines[0]);
    }

    global_deckinfo.description[0] = global_deckinfo.comments[0] = 0;
    global_deckinfo.revision = 1;
    strcpy(global_deckinfo.author, global_cfg.player_name);
    strcpy(global_deckinfo.email, global_cfg.email);

    GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "MMMM dd',' yyyy", global_deckinfo.creation_date, 22);

    if (global_db_flags_1 & (DBFLAGS_EDITDECK | DBFLAGS_GAUNTLET | DBFLAGS_SHANDALAR))
      copy_deck_to_edit();

    global_title_hwnd = CreateWindowEx(0, "MAGICDECK_TitleClass", wndname_title,
                                       WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                       0, 0, 0, 0,
                                       hwnd,
                                       1,
                                       global_hinstance, 0);

    global_fullcard_hwnd = CreateWindowEx(0, "MagicFullCardClass", wndname_fullcard,
                                          WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                          0, 0, 0, 0,
                                          hwnd,
                                          2,
                                          global_hinstance, 0);

    global_decksurface_hwnd = CreateWindowEx(0, "MAGICDECK_DeckSurfaceClass", wndname_decksurface,
                                             WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER,
                                             0, 0, 0, 0,
                                             hwnd,
                                             7,
                                             global_hinstance, 0);

    global_button_stats_hwnd = CreateWindowEx(0, "BUTTON", wndname_button_stats,
                                              WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                              0, 0, 0, 0,
                                              hwnd,
                                              RES_MAINMENU_BUTTON_STATS,
                                              global_hinstance, 0);

    button_deckinfo = CreateWindowEx(0, "BUTTON", wndname_button_deckinfo,
                                     WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                     0, 0, 0, 0,
                                     hwnd,
                                     RES_MAINMENU_BUTTON_DECKINFO,
                                     global_hinstance, 0);

    button_exit = CreateWindowEx(0, "BUTTON", wndname_button_exit,
                                 WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                 0, 0, 0, 0,
                                 hwnd,
                                 RES_MAINMENU_BUTTON_EXIT,
                                 global_hinstance, 0);

    button_deck1 = CreateWindowEx(0, "BUTTON", wndname_button_deck1,
                                  WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                  0, 0, 0, 0,
                                  hwnd,
                                  RES_MAINMENU_BUTTON_DECK_1,
                                  global_hinstance, 0);

    button_deck2 = CreateWindowEx(0, "BUTTON", wndname_button_deck2,
                                  WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                  0, 0, 0, 0,
                                  hwnd,
                                  RES_MAINMENU_BUTTON_DECK_2,
                                  global_hinstance, 0);

    button_deck3 = CreateWindowEx(0, "BUTTON", wndname_button_deck3,
                                  WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                  0, 0, 0, 0,
                                  hwnd,
                                  RES_MAINMENU_BUTTON_DECK_3,
                                  global_hinstance, 0);

    global_listbox_hwnd = CreateWindowEx(0, "LISTBOX", wndname_listbox,
                                         (WS_CHILDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | WS_THICKFRAME | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED | LBS_SORT),
                                         0, 0, 0, 0,
                                         hwnd,
                                         4,
                                         global_hinstance, 0);

    global_horzlist_hwnd = CreateWindowEx(0, "MAGICDECK_HorzListClass", wndname_horzlist,
                                          WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL,
                                          0, 0, 0, 0,
                                          hwnd,
                                          5,
                                          global_hinstance, 0);

    cardlistfilter_hwnd = CreateWindowEx(0, "MAGICDECK_CardListFiltersClass", wndname_cardlistfilter,
                                         WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                         0, 0, 0, 0,
                                         hwnd,
                                         3,
                                         global_hinstance, 0);

    popup = CreatePopupMenu();
    global_smallcard_popup = CreatePopupMenu();

    if (!global_title_hwnd || !global_fullcard_hwnd || !global_listbox_hwnd || !global_horzlist_hwnd || !cardlistfilter_hwnd || !global_decksurface_hwnd || !popup)
    {
      if (popup)
        DestroyMenu(popup);
      if (global_smallcard_popup)
        DestroyMenu(global_smallcard_popup);
      return -1;
    }

    SendMessage(global_listbox_hwnd, LB_SETTOPINDEX, 0, 0);
    SendMessage(global_listbox_hwnd, LB_SETCURSEL, 0, 0);
    SendMessage(global_horzlist_hwnd, LB_SETCURSEL, 0, 0);

    {
      s.csvidraw = SendMessage(global_horzlist_hwnd, LB_GETITEMDATA, 0, 0);
      if (s.csvidraw == -1 && global_deck_num_entries)
        s.csvidraw = global_deck[0].GDE_csvid;
      SendMessage(global_fullcard_hwnd, 0x400, s.csvidraw, 0);
    }

    if (global_db_flags_1 & DBFLAGS_STANDALONE)
    {
      load_text("menus", "MAINMENU_STANDALONE");
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_NEWDECK, text_lines[0]);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_LOADDECK, text_lines[1]);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_SAVEDECK, text_lines[2]);
      AppendMenu(popup, MF_SEPARATOR, 0, 0);
      AppendMenu(popup, MF_ENABLED, RES_FULLCARDMENU_EXPAND, text_lines[3]);
      AppendMenu(popup, MF_ENABLED, RES_MENU_CONSOLIDATE, text_lines[4]);
      AppendMenu(popup, MF_SEPARATOR, 0, 0);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_MINIMIZE, text_lines[5]);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_MUSIC, text_lines[6]);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_EFFECTS, text_lines[7]);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_EXIT, text_lines[8]);
    }
    else if (global_db_flags_1 & DBFLAGS_GAUNTLET)
    {
      load_text("menus", "MAINMENU_GAUNTLET");
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_NEWDECK, text_lines[0]);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_LOADDECK, text_lines[1]);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_SAVEDECK, text_lines[2]);
      AppendMenu(popup, MF_SEPARATOR, 0, 0);
      AppendMenu(popup, MF_ENABLED, RES_FULLCARDMENU_EXPAND, text_lines[3]);
      AppendMenu(popup, MF_ENABLED, RES_MENU_CONSOLIDATE, text_lines[4]);
      AppendMenu(popup, MF_SEPARATOR, 0, 0);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_MUSIC, text_lines[5]);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_EFFECTS, text_lines[6]);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_EXIT, text_lines[7]);
    }
    else
    {
      load_text("menus", "MAINMENU_ADVENTURE");
      AppendMenu(popup, MF_ENABLED, RES_FULLCARDMENU_EXPAND, text_lines[0]);
      AppendMenu(popup, MF_ENABLED, RES_MENU_CONSOLIDATE, text_lines[1]);
      AppendMenu(popup, MF_SEPARATOR, 0, 0);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_EXIT, text_lines[2]);
    }

    SetFocus(global_horzlist_hwnd);
    load_text("menus", "DECKINFO");
    SetWindowText(button_deckinfo, text_lines[0]);
    change_buttonclass_wndproc(hwnd);
    refresh_numofcards_text();
    if ((global_db_flags_1 & DBFLAGS_SHANDALAR))
    {
      load_text("menus", "EXIT");
      SetWindowText(button_exit, text_lines[0]);

      load_text("menus", "DECKNUMBERS");
      SetWindowText(button_deck1, text_lines[0]);
      SetWindowText(button_deck2, text_lines[1]);
      SetWindowText(button_deck3, text_lines[2]);
      if (global_current_deck == 0)
      {
        SetWindowText(button_deck1, text_lines[3]);
      }
      else if (global_current_deck == 1)
      {
        SetWindowText(button_deck2, text_lines[4]);
      }
      else if (global_current_deck == 2)
      {
        SetWindowText(button_deck3, text_lines[5]);
      }
    }
    else
    {
      load_text("menus", "EXIT");
      SetWindowText(button_exit, text_lines[1]);
    }
    return 0;

  case WM_CTLCOLORLISTBOX:
  {
    s.pad_a10 = wparam;
    ApplyCardArtPaletteToDc((HDC)s.pad_a10);
    s.pad_a14 = (HDC)lparam;
    SetTextColor((HDC)s.pad_a10, RGB(0, 0, 0));
    if (global_listbox_hwnd == (HWND)s.pad_a14)
      return global_create_brush_3;
    else
      return global_create_brush_4;
  }

  case WM_DESTROY:
    if (global_brush_mediumgrey)
      DeleteObject(global_brush_mediumgrey);
    if (pen_ltgrey)
      DeleteObject(pen_ltgrey);
    if (pen_medgrey)
      DeleteObject(pen_medgrey);

    s.pad_a18 = SendMessageA(global_title_hwnd, 0x31, 0, 0);
    SendMessageA(global_title_hwnd, 0x30, 0, 0);
    if (s.pad_a18)
      DeleteObject(s.pad_a18);

    if (DAT_10125e0c)
    {
      DeleteObject(DAT_10125e0c);
    }
    DAT_10125e0c = 0;

    if (popup)
      DestroyMenu(popup);
    if (global_smallcard_popup)
      DestroyMenu(global_smallcard_popup);
    if (mainclass_destroy_obj)
      DeleteObject(mainclass_destroy_obj);
    if (!(global_db_flags_1 & DBFLAGS_SHANDALAR))
      free_sounds_and_music();
    if (check_filters_debug_log_file != NULL)
    {
      fclose(check_filters_debug_log_file);
      check_filters_debug_log_file = NULL;
    }
    PostQuitMessage(0);
    return 0;

  case WM_DISPLAYCHANGE:
    global_color_depth = (int)wparam;
    SelectObject(global_hdc, global_old_bmp_obj);
    DeleteObject(global_hbmp);

    s.pad_a1c = GetDC(hwnd);
    ApplyCardArtPaletteToDc(s.pad_a1c);

    global_hbmp = CreateCompatibleBitmap(s.pad_a1c, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));

    ReleaseDC(hwnd, s.pad_a1c);

    SelectObject(global_hdc, global_hbmp);

    if (!global_hbmp)
    {
      MessageBox(hwnd,
                 ("Not enough system memory to run at this screen resolution.\n"
                  "\n"
                  "Change it back to a smaller size before continuing (the game will hide itself until then)."),
                 "Magic: The Gathering",
                 MB_ICONWARNING);
      ShowWindow(hwnd, SW_HIDE);
    }
    else
      ShowWindow(hwnd, SW_SHOW);

    MoveWindow(hwnd, 0, 0, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), TRUE);
    return 0;

  case 0x4c8:
  {
    s.pad_a20 = (HDC)wparam;
    s.hwnd1 = (HWND)lparam;
    if (GetDlgItem(hwnd, RES_MAINMENU_BUTTON_STATS) == (HWND)s.pad_a20 || GetDlgItem(hwnd, RES_MAINMENU_BUTTON_DECKINFO) == (HWND)s.pad_a20 || GetDlgItem(hwnd, RES_MAINMENU_BUTTON_EXIT) == (HWND)s.pad_a20 || GetDlgItem(hwnd, RES_MAINMENU_BUTTON_DECK_1) == (HWND)s.pad_a20 || GetDlgItem(hwnd, RES_MAINMENU_BUTTON_DECK_2) == (HWND)s.pad_a20 || GetDlgItem(hwnd, RES_MAINMENU_BUTTON_DECK_3) == (HWND)s.pad_a20)
      SendMessage(hwnd, 0x401, GetDlgCtrlID((HWND)s.pad_a20), 0);
    else
      SendMessage(hwnd, 0x401, 1, 0);

    if (s.pad_a20)
      InvalidateRect((HWND)s.pad_a20, NULL, TRUE);

    if (s.hwnd1)
      InvalidateRect(s.hwnd1, NULL, TRUE);

    return 0;
  }

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
  {
    s.pad_a30 = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.pad_a30);
    s.tgt = (HWND)lparam;
    s.cursel = GetDlgCtrlID(s.tgt);
    if (GetFocus() == s.tgt)
      SetTextColor(s.pad_a30, global_colorref_white);
    else
      SetTextColor(s.pad_a30, global_colorref_darkgrey);
    SetBkMode(s.pad_a30, TRANSPARENT);
    s.hwnd2 = (HWND)GetStockObject(HOLLOW_BRUSH);
    return (HBRUSH)s.hwnd2;
  }

  case WM_DRAWITEM:
  {
    s.item_col = (COLORREF)lparam;

    if (GetFocus() == ((DRAWITEMSTRUCT *)s.item_col)->hwndItem)
      s.fill_x = global_colorref_white;
    else
    {
      if (((DRAWITEMSTRUCT *)s.item_col)->itemState & 2)
        s.fill_x = 0x10000c6;
      else
        s.fill_x = global_colorref_flesh;
    }

    FUN_10024e48((DRAWITEMSTRUCT *)s.item_col, global_brush_mediumgrey, pen_ltgrey, pen_medgrey, s.fill_x, 0);
    return 1;
  }

  case WM_ERASEBKGND:
    s.rect_stats.bottom = (LONG)wparam;
    ApplyCardArtPaletteToDc((HDC)s.rect_stats.bottom);
    s.fill_chdc = CreateCompatibleDC((HDC)s.rect_stats.bottom);
    ApplyCardArtPaletteToDc(s.fill_chdc);
    SelectObject(s.fill_chdc, global_pic_dektile4);

    GetClientRect(hwnd, &s.r);
    GetObjectA(global_pic_dektile4, sizeof(BITMAP), &s.bmp_a6c);

    for (s.pics_inline = 0; s.r.right > s.pics_inline; s.pics_inline += 0x20)
      for (s.fill_y = s.r.bottom; s.fill_y > -0x1d; s.fill_y -= 0x1d)
        BitBlt((HDC)s.rect_stats.bottom, s.pics_inline, s.fill_y, s.bmp_a6c.bmWidth, s.bmp_a6c.bmHeight, s.fill_chdc, 0, 0, SRCCOPY);

    DeleteDC(s.fill_chdc);
    return 1;

  case WM_INITMENU:
    CHECKMENU_IF(popup, RES_FULLCARDMENU_EXPAND, SendMessage(global_fullcard_hwnd, 0x402, 0, 0));
    CHECKMENU_IF(popup, RES_MAINMENU_EFFECTS, global_cfg.effects);
    CHECKMENU_IF(popup, RES_MAINMENU_MUSIC, global_cfg.music);
    CHECKMENU_IF(popup, RES_MENU_CONSOLIDATE, global_cfg_consolidate);
    return 0;

  case WM_MEASUREITEM:
  {
    s.pad_a8c = (MEASUREITEMSTRUCT *)lparam;
    s.hwnd_a8c = GetDlgItem(hwnd, wparam);
    GetClientRect(s.hwnd_a8c, (RECT *)&s.pad_a88);

    if (s.pad_a8c->CtlID == 4)
    {
      s.pad_a8c->itemWidth = s.rect_stats.top;
      s.pad_a8c->itemHeight = s.rect_stats.right / 8;
    }
    return 0;
  }

  case WM_RBUTTONDOWN:
  {
    s.pad_a90.x = GET_X_LPARAM(lparam);
    s.pad_a90.y = GET_Y_LPARAM(lparam);
    ClientToScreen(hwnd, &s.pad_a90);
    TrackPopupMenu(popup, TPM_RIGHTBUTTON, s.pad_a90.x, s.pad_a90.y, 0, hwnd, NULL);
    return 0;
  }

  case WM_SETFOCUS:
    SetFocus(global_horzlist_hwnd);
    return 0;

  case WM_SIZE:
  {
    s.main_w = (int)((unsigned int)lparam & 0xffff);
    s.main_h = (int)(((unsigned int)lparam >> 16) & 0xffff);
    // global_main_window_width = s.main_w;
    // global_main_window_height = s.main_h;

    LockWindowUpdate(hwnd);

    s.margin8 = 8;
    s.ten = 0xa;

    s.third_w = s.main_w / 3 - 0x35;
    s.fullcard_height = ((s.main_h - s.ten * 4) * 64) / 100 - 0xd;
    s.horzlist_hgt6 = (s.main_h - s.ten * 4) * 11 * 2 / 100;
    s.title_height = 10 * (s.main_h - s.ten * 4) / 100;
    s.cardlistfilter_hgt = 6 * (s.main_h - s.ten * 4) / 100;

    s.pad_b0c = (LONG)GetDC(hwnd);
    ApplyCardArtPaletteToDc(s.pad_b0c);
    GetTextExtentPointA(s.pad_b0c, "Load new deck", 0xd, &s.sz);
    s.sz.cx += 0xa;
    s.sz.cy += s.sz.cy / 2;
    ReleaseDC(hwnd, s.pad_b0c);

    SetRect(&s.rect_title,
            s.margin8, s.ten,
            s.third_w + s.margin8, s.title_height + s.ten);

    SetRect(&s.rect_fullcard,
            s.margin8, s.rect_title.bottom + s.ten,
            s.third_w + s.margin8,
            (s.rect_title.bottom - s.ten) + s.fullcard_height);

    SetRect(&s.rect_horzlist,
            s.margin8, (s.main_h - s.ten) - s.horzlist_hgt6,
            s.main_w - s.margin8, s.main_h - s.ten);

    SetRect(&s.rect_cardlistfilter,
            s.rect_horzlist.left, s.rect_horzlist.top - s.cardlistfilter_hgt,
            s.rect_horzlist.right, s.rect_horzlist.top);

    SetRect(&s.rect_load_button,
            s.main_w - s.margin8 - s.sz.cx,
            s.ten,
            s.main_w - s.margin8,
            s.sz.cy * 2 + s.ten / 2 + s.ten);

    SetRect(&s.rect_decksurface,
            s.rect_fullcard.right + s.margin8, 0,
            s.main_w, s.rect_fullcard.bottom);

    MoveWindow(global_title_hwnd,
               s.rect_title.left, s.rect_title.top,
               s.rect_title.right - s.rect_title.left,
               s.rect_title.bottom - s.rect_title.top,
               TRUE);

    MoveWindow(global_fullcard_hwnd,
               s.rect_fullcard.left, s.rect_fullcard.top,
               s.rect_fullcard.right - s.rect_fullcard.left,
               s.rect_fullcard.bottom - s.rect_fullcard.top,
               TRUE);

    s.decksurface_top = s.rect_decksurface.top;
    s.decksurface_x = s.rect_decksurface.left;

    MoveWindow(global_decksurface_hwnd,
               s.rect_decksurface.left, s.rect_decksurface.top,
               s.rect_decksurface.right - s.rect_decksurface.left,
               s.rect_decksurface.bottom - s.rect_decksurface.top,
               TRUE);

    s.horzlist_style = GetWindowLongA(global_horzlist_hwnd, GWL_STYLE);
    if (s.horzlist_style & WS_BORDER)
      s.border_adjust = GetSystemMetrics(SM_CXBORDER) * 2;
    else
      s.border_adjust = 0;

    s.rect_horzlist.top = s.rect_horzlist.bottom - global_smallcard_piclist_height - 8 -
                          GetSystemMetrics(SM_CYHSCROLL) - s.border_adjust;

    s.rect_cardlistfilter.bottom = s.rect_horzlist.top;
    s.rect_cardlistfilter.top = s.rect_cardlistfilter.bottom - s.cardlistfilter_hgt;

    MoveWindow(cardlistfilter_hwnd,
               s.rect_cardlistfilter.left, s.rect_cardlistfilter.top,
               s.rect_cardlistfilter.right - s.rect_cardlistfilter.left,
               s.rect_cardlistfilter.bottom - s.rect_cardlistfilter.top,
               TRUE);

    MoveWindow(global_horzlist_hwnd,
               s.rect_horzlist.left, s.rect_horzlist.top,
               s.rect_horzlist.right - s.rect_horzlist.left,
               s.rect_horzlist.bottom - s.rect_horzlist.top,
               TRUE);

    // TODO : huh?
    if (false)
    {
      s.rect_fullcard.bottom = s.rect_cardlistfilter.top + 4;
      s.rect_decksurface.bottom = s.rect_cardlistfilter.top + 4;

      MoveWindow(global_fullcard_hwnd,
                 s.rect_fullcard.left, s.rect_fullcard.top,
                 s.rect_fullcard.right - s.rect_fullcard.left,
                 s.rect_fullcard.bottom - s.rect_fullcard.top,
                 TRUE);

      s.decksurface_top = s.rect_decksurface.top + GetSystemMetrics(SM_CYCAPTION) * 2;
      s.decksurface_x = s.rect_decksurface.left;

      MoveWindow(global_decksurface_hwnd,
                 s.decksurface_x, s.decksurface_top,
                 s.rect_decksurface.right - s.decksurface_x,
                 s.rect_decksurface.bottom - s.decksurface_top,
                 TRUE);

      s.decksurface_x += 0x2a;
      s.decksurface_top -= GetSystemMetrics(SM_CYCAPTION);
      MoveWindow(global_unknown_10125a3c,
                 s.decksurface_x, s.decksurface_top,
                 s.rect_decksurface.right - s.decksurface_x,
                 s.rect_decksurface.bottom - s.decksurface_top,
                 TRUE);

      s.decksurface_x += 0x2a;
      s.decksurface_top -= GetSystemMetrics(SM_CYCAPTION);
      MoveWindow(global_unknown_10144b04,
                 s.decksurface_x, s.decksurface_top,
                 s.rect_decksurface.right - s.decksurface_x,
                 s.rect_decksurface.bottom - s.decksurface_top,
                 TRUE);
    }
    SetRect(&s.rect_pad_adc,
            s.rect_decksurface.left, s.rect_decksurface.bottom,
            (s.rect_decksurface.right - s.rect_decksurface.left) / 3,
            s.rect_cardlistfilter.top - s.rect_decksurface.bottom - 3);

    MoveWindow(global_button_stats_hwnd,
               s.rect_pad_adc.left, s.rect_pad_adc.top,
               s.rect_pad_adc.right, s.rect_pad_adc.bottom,
               TRUE);

    MoveWindow(button_exit,
               s.rect_pad_adc.left + s.rect_pad_adc.right * 2, s.rect_pad_adc.top,
               s.rect_pad_adc.right, s.rect_pad_adc.bottom,
               TRUE);

    if (global_db_flags_1 & DBFLAGS_SHANDALAR)
    {
      MoveWindow(button_deck1,
                 s.rect_pad_adc.left + s.rect_pad_adc.right, s.rect_pad_adc.top,
                 s.rect_pad_adc.right / 3, s.rect_pad_adc.bottom,
                 TRUE);
      MoveWindow(button_deck2,
                 s.rect_pad_adc.left + s.rect_pad_adc.right + s.rect_pad_adc.right / 3, s.rect_pad_adc.top,
                 s.rect_pad_adc.right / 3, s.rect_pad_adc.bottom,
                 TRUE);
      MoveWindow(button_deck3,
                 s.rect_pad_adc.left + s.rect_pad_adc.right + 2 * s.rect_pad_adc.right / 3, s.rect_pad_adc.top,
                 s.rect_pad_adc.right / 3, s.rect_pad_adc.bottom,
                 TRUE);
    }
    else
      MoveWindow(button_deckinfo,
                 s.rect_pad_adc.left + s.rect_pad_adc.right, s.rect_pad_adc.top,
                 s.rect_pad_adc.right, s.rect_pad_adc.bottom,
                 TRUE);

    SendMessageA(global_horzlist_hwnd, 0x1a0, 0, (LPARAM)(unsigned short)global_smallcard_piclist_width);
    SendMessage(cardlistfilter_hwnd, 0x402,
                MAKELONG(s.rect_cardlistfilter.left, s.rect_cardlistfilter.right),
                MAKELONG(s.rect_cardlistfilter.top, s.rect_cardlistfilter.bottom));

    s.wLast = GetWindowWord(global_horzlist_hwnd, HORZLIST_LASTPIC_INDEX);
    s.horz_list_addr = (int *)GetWindowLongA(global_horzlist_hwnd, HORZLIST_ADDR_INDEX);
    s.wLeft = GetWindowWord(global_horzlist_hwnd, HORZLIST_LEFTPIC_INDEX);
    s.wPicsInline = GetWindowWord(global_horzlist_hwnd, HORZLIST_PICSINLINE_INDEX);

    for (s.artist_i = s.wLeft;
         MIN(s.wPicsInline + s.wLeft, s.wLast) > (int)s.artist_i;
         ++s.artist_i)
      FUN_1000942e(global_horzlist_hwnd, s.horz_list_addr[s.artist_i]);

    BringWindowToTop(global_title_hwnd);
    BringWindowToTop(cardlistfilter_hwnd);
    BringWindowToTop(global_horzlist_hwnd);
    BringWindowToTop(global_button_stats_hwnd);
    BringWindowToTop(button_deckinfo);
    BringWindowToTop(button_exit);
    LockWindowUpdate(0);

    if (!(global_db_flags_1 & DBFLAGS_SHANDALAR) && global_cfg.music)
      play_music(1, 400, 0);

    return 0;
  }

  case WM_QUERYENDSESSION:
    if (!global_deck_was_edited || ask_about_saving_deck() != 2)
      return 1;

    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}
