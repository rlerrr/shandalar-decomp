// DeckDll: standalone and integrated deckbuilder.
// deckdll.c: primary interface to Magic.exe/Shandalar.exe

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <direct.h>

#include "deckdll.h"
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

typedef ptrdiff_t INT_PTR;

typedef int (*Int_fn_int)(int);

#define HORZLIST_LASTPIC_INDEX 0
#define HORZLIST_ADDR_INDEX 4
#define HORZLIST_CURRSEL_INDEX 8
#define HORZLIST_LEFTPIC_INDEX 12
#define HORZLIST_WIDTHPLUSSPACE_INDEX 16
#define HORZLIST_PICSINLINE_INDEX 20
#define HORZLIST_SIZE_INDEX 24

#define CARD_CSVID_INDEX 0
#define CARD_AMOUNT_INDEX 4

#define FULLCARD_CSVID_INDEX 0
#define FULLCARD_EXPANDTEXT_INDEX 4

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
static char global_dbart_path[MAX_PATH + 15];
// GLOBAL: DECKDLL 0x101bbf50
static char global_duel_dat_path[MAX_PATH + 15];

// GLOBAL: DECKDLL 0x10143c4c
int global_DIB_debug = 0;

// GLOBAL: DECKDLL 0x10143fbc
DBFlags global_db_flags_1 = DBFLAGS_0;
// GLOBAL: DECKDLL 0x1019f00c
static DBFlags global_db_flags_2 = DBFLAGS_0;
// GLOBAL: DECKDLL 0x101bc0c4
static bool global_cfg_consolidate = false;
// GLOBAL: DECKDLL 0x101a8c70
static signed char global_cfg_consolidate_from_registry = 0;
// GLOBAL: DECKDLL 0x101a8c72
signed char global_cfg_effects = 0;
// GLOBAL: DECKDLL 0x101a8d13
static signed char global_cfg_expand_text = 0;
// GLOBAL: DECKDLL 0x101a8c71
static signed char global_cfg_music = 0;
static bool global_cfg_no_frame = false;
static bool global_cfg_read_by_name = false;
bool global_cfg_view_all = false;
#define MAX_NAMELEN 80

// GLOBAL: DECKDLL 0x101a8c73
static char global_cfg_player_name[MAX_NAMELEN];

// GLOBAL: DECKDLL 0x101a8cc3;
static char global_cfg_email[MAX_NAMELEN];
static char global_cfg_skin_name[MAX_PATH + 1];

// GLOBAL: DECKDLL 0x1012e898
static int global_current_deck = 0;

// GLOBAL: DECKDLL 0x10144370
static int *global_external_current_deck = NULL;

// GLOBAL: DECKDLL 0x101f1e80
int *deck = NULL;

// GLOBAL: DECKDLL 0x1012ed80
GlobalDeckEntry global_deck[1000];
// GLOBAL: DECKDLL 0x101a0ce0
static int global_deck_num_cards = 0;

// GLOBAL: DECKDLL 0x101a9694
int global_deck_num_entries = 0;
// GLOBAL: DECKDLL 0x1012e88c
static int global_deck_num_entries_copy = 0;

// GLOBAL: DECKDLL 0x1019fed0
DeckEntry global_edited_deck[300];
// GLOBAL: DECKDLL 0x101a0ce4
int global_edited_deck_num_entries = 0;
// GLOBAL: DECKDLL 0x10144710
static DeckEntry global_excessive_cards[201];

// GLOBAL: DECKDLL 0x1012ea0c
static bool global_deck_was_edited = false;
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
char global_deckname[32];

// GLOBAL: DECKDLL 0x10142814
static char *global_external_deckname = NULL;

// GLOBAL: DECKDLL 0x1012dcd4
static char global_deck_author[81];
// GLOBAL: DECKDLL 0x1012dda0
static char global_deck_comments[404];

// GLOBAL: DECKDLL 0x1012dd76
static char global_deck_creation_date[22];
// GLOBAL: DECKDLL 0x1012dcbf
static char global_deck_description[21];
// GLOBAL: DECKDLL 0x1012dd90
static char global_deck_edition[16];

// GLOBAL: DECKDLL 0x1012dd25
static char global_deck_email[81];

// GLOBAL: DECKDLL 0x101a8a70
static char global_deck_filename[(MAX_PATH + 15) + 32 + 5];

// GLOBAL: DECKDLL 0x1012dd8c
static int global_deck_revision = 1;

// GLOBAL: DECKDLL 0x101bc604
int global_dlg_parameter = 0;
// GLOBAL: DECKDLL 0x101423d0
static int global_dlg_result = 0;
// GLOBAL: DECKDLL 0x10143ae0
static char global_ask_x_dlg_title[128];
// GLOBAL: DECKDLL 0x101127e0
static char global_loaddeck_dlg_filename[2 * MAX_PATH + 15 + 10];

static HBRUSH global_brush_gold1 = NULL;
static HBRUSH global_brush_gold2 = NULL;
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
static HBRUSH global_create_brush_5 = NULL;
// GLOBAL: DECKDLL 0x10132c3c
static HBRUSH global_create_brush_6 = NULL;
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

COLORREF global_colorref_darkgrey = 0;
COLORREF global_colorref_flesh = 0;
COLORREF global_colorref_lavender = 0;
COLORREF global_colorref_lightgrey = 0;
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

static HPALETTE global_palette = NULL;

// GLOBAL: DECKDLL 0x10113cec
static int global_smallcard_normal_width = 0;
// GLOBAL: DECKDLL 0x1012e4f4
static int global_smallcard_normal_height = 0;

// GLOBAL: DECKDLL 0x101bc448
static int global_smallcard_smaller_width = 0;
// GLOBAL: DECKDLL 0x10113cf0
static int global_smallcard_smaller_height = 0;

// GLOBAL: DECKDLL 0x10143fc0
static int global_smallcard_smallest_width = 0;
// GLOBAL: DECKDLL 0x1014532c
static int global_smallcard_smallest_height = 0;

static int global_smallcard_piclist_height = 0;
static int global_smallcard_piclist_width = 0;

static LOGFONT global_logfont_template =
    {
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
            'T', 'r', 'e', 'b', 'u', 'c', 'h', 'e', 't', ' ', 'M', 'S', '\0',
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'} // lfFaceName[LF_FACESIZE]
};
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
// GLOBAL: DECKDLL 0x1011279c
static WNDPROC global_wndproc_std_ButtonClass;
// GLOBAL: DECKDLL 0x101436b4
static HWND global_cuecard_hwnd;
// GLOBAL: DECKDLL 0x101a8a6c
static HWND global_decksurface_hwnd;
// GLOBAL: DECKDLL 0x100f2768
static HMENU global_decksurface_popup;
// GLOBAL: DECKDLL 0x100f2840
static HMENU global_tradesurface_popup;

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
static HWND global_cardlistfilter_hwnd;
// GLOBAL: DECKDLL 0x101052c8
static HMENU global_horzlist_popup;

// GLOBAL: DECKDLL 0x101a8c64
HWND global_listbox_hwnd;

// GLOBAL: DECKDLL 0x10144ef4
HWND global_main_hwnd;
static int global_main_window_width = 0;
static int global_main_window_height = 0;

static WNDPROC global_wndproc_std_SearchClass;

// GLOBAL: DECKDLL 0x10144eec
static int global_smallcard_height = 0;
// GLOBAL: DECKDLL 0x1013a17c
static int global_smallcard_width = 0;

// GLOBAL: DECKDLL 0x10141b4c
static HMENU global_smallcard_popup;

// GLOBAL: DECKDLL 0x10146df0
static HWND global_title_hwnd;

// GLOBAL: DECKDLL 0x10125a3c
static HWND global_unknown_10125a3c;

// GLOBAL: DECKDLL 0x10144b04
static HWND global_unknown_10144b04;

// GLOBAL: DECKDLL 0x1012566c
HDC global_hdc;

// GLOBAL: DECKDLL 0x10105738
static POINT global_horzlist_mouse_pt;

// GLOBAL: DECKDLL 0x10105748
static POINT global_main_mouse_pt;

static char global_search_string[264];

// GLOBAL: DECKDLL 0x101915a0
static Packs global_packs[PACK1_MAX + 1][PACK2_MAX + 1];
// GLOBAL: DECKDLL 0x101e74f0
static Packs global_packs_copy[PACK1_MAX + 1][PACK2_MAX + 1];

// This is really [225][128] ending at 0x10139CD0
// TODO: Looks like [250][300] in magic.exe
// GLOBAL: DECKDLL 0x10132c50
char text_lines[225][128];

// GLOBAL: DECKDLL 0x101427b4
int global_available_slots = 0;
int global_num_expansions = 0;
int global_expansion_size = 0;
const card_ptr_t *cards_ptr;

// GLOBAL: DECKDLL 0x10147220
card_ptr_t global_raw_cards_storage[1000];
card_ptr_t *global_raw_cards_ptr = global_raw_cards_storage;

// GLOBAL: DECKDLL 0x1012df3c
static char *global_base_txt;
static char *global_raw_dbinfo;
char *global_raw_rarities;

// GLOBAL: DECKDLL 0x10145330
OrigRarities global_origrarities[1000];
static bool global_db_read;

static bool global_using_main_db = false;
static char *global_deckbuilder_csv_raw;
static int (*global_check_card_count_fn)(const DeckEntry *, int, int);
int (*global_is_valid_card_fn)(int);
static bool (*global_colors_match_fn)(iid_t, color_test_t);
static int (*global_check_colors_inout_edited_deck_fn)(const GlobalDeckEntry *, int, bool);

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

static void popup(const char *fmt, ...)
{
  char buf[8000];
  va_list args;
  va_start(args, fmt);
  _vsnprintf(buf, 8000, fmt, args);
  va_end(args);

  MessageBox(NULL, buf, 0, MB_SYSTEMMODAL | MB_ICONWARNING);
}

static void logf(const char *fmt, ...)
{
  char buf[8000];
  va_list args;
  va_start(args, fmt);
  _vsnprintf(buf, 8000, fmt, args);
  va_end(args);

#if 0
  if (!deck_dll_log)
    deck_dll_log = new File("deck_dll.log", "w");

  deck_dll_log->printf("%s\n", buf);
  deck_dll_log->flush();
#endif

  fprintf(stderr, "%s\n", buf);
  fflush(stderr);
}

char *readline(FILE *file, char *dest, int sz)
{
  int l;
  char rest_of_line[512];

  dest[0] = dest[sz - 1] = 0;
  if (!fgets(dest, sz, file))
    return NULL;

  if (!dest[0]) /* eof or other error */
    return NULL;

  l = strlen(dest);
  if (dest[l - 1] == '\n')
  {
    dest[l - 1] = 0;
    return dest;
  }

  /* Line in file too long; read until eol */
  do
  {
    rest_of_line[510] = rest_of_line[511] = 0;
    fgets(rest_of_line, 512, file);
  } while (rest_of_line[0] && rest_of_line[510] && rest_of_line[510] != '\n');
  return dest;
}

// FUNCTION: DECKDLL 0x10028350
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

static __inline void textout(HDC hdc, int x, int y, const char *txt)
{
  TextOut(hdc, x, y, txt, strlen(txt));
}

static __inline void textoutf(HDC hdc, int x, int y, const char *fmt, ...)
{
  char buf[512];
  int len;

  va_list args;
  va_start(args, fmt);
  len = _vsnprintf(buf, 512, fmt, args);
  va_end(args);
  TextOut(hdc, x, y, buf, len);
}

static __inline int
popup_loaded(HWND hwnd, const char *title, const char *msg, int msgbox_params)
{
  char txt[128];
  load_text("Menus", msg);
  strcpy(txt, text_lines[0]);

  load_text("Menus", title);
  return MessageBox(hwnd, txt, text_lines[0], msgbox_params);
}

static __inline int
popup_loaded_with_args(HWND hwnd, const char *title, const char *msg, const char *sprintf_arg, int msgbox_params)
{
  int buffer_size;
  char *txt;
  int result;

  buffer_size = 128 + strlen(sprintf_arg) + 1;
  txt = (char *)malloc(buffer_size);
  if (!txt)
    fatal("popup_loaded_with_args: out of memory");

  load_text("Menus", msg);
  sprintf(txt, text_lines[0], sprintf_arg);

  load_text("Menus", title);
  result = MessageBox(hwnd, txt, text_lines[0], msgbox_params);
  free(txt);
  return result;
}

#define set_dlg_text(hdlg, resource, txt) SetWindowText(GetDlgItem(hdlg, resource), txt)

static void
set_dlg_text_limited(HWND hdlg, int resource, const char *txt, int limit)
{
  HWND item = GetDlgItem(hdlg, resource);
  SetWindowText(item, txt);
  SendMessage(item, EM_SETLIMITTEXT, limit, 0);
}

static __inline void
translate_backslash_n_to_newline(char *str)
{
  char *p;
  for (p = str;; ++p, ++str)
    switch (*str)
    {
    case '\0':
      *p = 0;
      return;

    case '\\':
      if (*(str + 1) == 'n' || *(str + 1) == 'N')
      {
        *p = '\n';
        ++str;
        break;
      }
      // else fall through

    default:
      *p = *str;
      break;
    }
}

// GLOBAL: DECKDLL 0x100355d8
static char read_db_empty_rules[1];
// GLOBAL: DECKDLL 0x100355ec
static char read_db_empty_flavor[1];

// GLOBAL: DECKDLL 0x10036210
const char s__08x_DestroyDIBSection__file_map_10036210[] = "  %08x DestroyDIBSection (file mapping: %08x)\n";

// GLOBAL: DECKDLL 0x1003628c
const char s_Button_1003628c[] = "Button";

// GLOBAL: DECKDLL 0x1019c020
const char read_db_artist_names[100][100];

// GLOBAL: DECKDLL 0x10033e48
const char *const_db_artist_names[] = {
    "None",
    "Amy Weber",
    "Andi Rusu",
    "Anson Maddocks",
    "Anthony Waters",
    "Brian Snoddy",
    "Bryon Wackwitz",
    "Christopher Rush",
    "Cornelius Brudi",
    "Dameon Willich",
    "Dan Frazier",
    "Daniel Gelon",
    "Dennis Detwiller",
    "Douglas Shuler",
    "Drew Tucker",
    "Edward Beard Jr.",
    "Fay Jones",
    "Frank Frazier",
    "Harold McNeill",
    "Heather Hudson",
    "Jeff A. Menges",
    "Jesper Myrfors",
    "Julie Baroh",
    "Justin Hampton",
    "Kaja & Phil Foglio",
    "Kaja Foglio",
    "Ken Meyer Jr.",
    "Kerstin Kaman",
    "Kev Brockschmidt",
    "Kristen Bishop",
    "Liz Danforth",
    "Margaret Organ-Kean",
    "Mark Poole",
    "Mark Tedin",
    "Melissa Benson",
    "Michael Whelan",
    "Mike Kimble",
    "Nicola Leonard",
    "Nene Thomas",
    "Pat Morrissey",
    "Pete Venters",
    "Phil Foglio",
    "Quinton Hoover",
    "Randy Asplund-Faith",
    "Richard Kane-Ferguson",
    "Richard Thomas",
    "Rob Alexander",
    "Ron Spencer",
    "Sandra Everingham",
    "Scott Kirschner",
    "Susan Van Camp",
    "Tom Wanerstrand",
    "Unknown",
};

// FUNCTION: DECKDLL 0x1001a940
static int read_db_guts(char *cards_dat_filename)
{
  struct read_db_guts_locals
  {
    int init_idx;               /* local_808 */
    int flavor_out_idx;         /* local_804 */
    int flavor_card_idx;        /* local_800 */
    char flavor_text_buf[1000]; /* local_7fc */
    char *flavor_in;            /* local_414 */
    int rules_out_idx;          /* local_410 */
    int rules_card_idx;         /* local_40c */
    char rules_text_buf[1000];  /* local_408 */
    char *rules_in;             /* local_20 */
    int req_idx;                /* local_1c */
    int exp_idx;                /* local_18 */
    card_ptr_t *cp;
    size_t record_size;
    int card_idx; /* local_c */
    FILE *cards_dat;
  } locals;

  locals.cards_dat = fopen(cards_dat_filename, "rb");
  if (locals.cards_dat == NULL)
    return 0;

  fread(&global_available_slots, 4, 1, locals.cards_dat);
  fread(&locals.record_size, 4, 1, locals.cards_dat);

  global_base_txt = (char *)malloc(locals.record_size);
  if (global_base_txt == NULL)
  {
    fclose(locals.cards_dat);
    return 0;
  }

  fread(global_raw_cards_storage, 0x98, global_available_slots, locals.cards_dat);
  fread(global_base_txt, 1, locals.record_size, locals.cards_dat);
  fclose(locals.cards_dat);

  for (locals.card_idx = 0; locals.card_idx < global_available_slots; locals.card_idx += 1)
  {
    *(int *)&global_raw_cards_storage[locals.card_idx].full_name += (int)global_base_txt;
    *(int *)&global_raw_cards_storage[locals.card_idx].name += (int)global_base_txt;
    *(int *)&global_raw_cards_storage[locals.card_idx].type_text += (int)global_base_txt;
    *(int *)&global_raw_cards_storage[locals.card_idx].rules_text += (int)global_base_txt;
    *(int *)&global_raw_cards_storage[locals.card_idx].flavor_text += (int)global_base_txt;

    if (_strcmpi(global_raw_cards_storage[locals.card_idx].rules_text, "None") == 0)
      global_raw_cards_storage[locals.card_idx].rules_text = read_db_empty_rules;

    if ((_strcmpi(global_raw_cards_storage[locals.card_idx].flavor_text, "None") == 0) ||
        (_strcmpi(global_raw_cards_storage[locals.card_idx].flavor_text, "Blank") == 0))
      global_raw_cards_storage[locals.card_idx].flavor_text = read_db_empty_flavor;

    global_raw_cards_storage[locals.card_idx].artist =
        const_db_artist_names[(int)global_raw_cards_storage[locals.card_idx].artist];

    if (global_raw_cards_storage[locals.card_idx].num_pics == 0)
      global_raw_cards_storage[locals.card_idx].num_pics = 1;
  }

  for (locals.exp_idx = 0; locals.exp_idx < global_available_slots; locals.exp_idx += 1)
    if (global_raw_cards_storage[locals.exp_idx].expansion & 0x40)
      global_raw_cards_storage[locals.exp_idx].expansion = 0x80;

  for (locals.req_idx = 0; locals.req_idx < global_available_slots; locals.req_idx += 1)
    if ((signed char)global_raw_cards_storage[locals.req_idx].req.req_colorless == 0x11)
      global_raw_cards_storage[locals.req_idx].req.req_colorless = 10;

  for (locals.rules_card_idx = 0; locals.rules_card_idx < global_available_slots;
       locals.rules_card_idx += 1)
  {
    locals.rules_out_idx = 0;
    for (locals.rules_in = global_raw_cards_storage[locals.rules_card_idx].rules_text;
         *locals.rules_in;)
    {
      if (((*locals.rules_in == '\\') && (locals.rules_in[1] == '\\')) ||
          ((*locals.rules_in == '\\') && (locals.rules_in[1] == 'n')))
      {
        locals.rules_in += 1;
        locals.rules_text_buf[locals.rules_out_idx] = '\n';
        locals.rules_out_idx += 1;
      }
      else
      {
        locals.rules_text_buf[locals.rules_out_idx] = *locals.rules_in;
        locals.rules_out_idx += 1;
      }

      locals.rules_in += 1;
    }

    locals.rules_text_buf[locals.rules_out_idx] = '\0';
    strcpy(global_raw_cards_storage[locals.rules_card_idx].rules_text, locals.rules_text_buf);
  }

  for (locals.flavor_card_idx = 0; locals.flavor_card_idx < global_available_slots;
       locals.flavor_card_idx += 1)
  {
    locals.flavor_out_idx = 0;
    for (locals.flavor_in = global_raw_cards_storage[locals.flavor_card_idx].flavor_text;
         *locals.flavor_in;)
    {
      if (((*locals.flavor_in == '\\') && (locals.flavor_in[1] == '\\')) ||
          ((*locals.flavor_in == '\\') && (locals.flavor_in[1] == 'n')))
      {
        locals.flavor_in += 1;
        locals.flavor_text_buf[locals.flavor_out_idx] = '\n';
        locals.flavor_out_idx += 1;
      }
      else
      {
        locals.flavor_text_buf[locals.flavor_out_idx] = *locals.flavor_in;
        locals.flavor_out_idx += 1;
      }

      locals.flavor_in += 1;
    }

    locals.flavor_text_buf[locals.flavor_out_idx] = '\0';
    strcpy(global_raw_cards_storage[locals.flavor_card_idx].flavor_text, locals.flavor_text_buf);
  }

#define SET_HACK(idx, col) \
  *(int *)&global_raw_cards_storage[(idx)].hack_colors = (col)

  for (locals.init_idx = 0; locals.init_idx < global_available_slots; locals.init_idx += 1)
  {
    global_raw_cards_storage[locals.init_idx].sleight_color = 0;
    SET_HACK(locals.init_idx, 0);
  }

  global_raw_cards_storage[0x227].sleight_color = 0x28;
  global_raw_cards_storage[8].sleight_color = 2;
  global_raw_cards_storage[0x10].sleight_color = 0x20;
  global_raw_cards_storage[0x13].sleight_color = 2;
  global_raw_cards_storage[0x16].sleight_color = 0x10;
  global_raw_cards_storage[0x17].sleight_color = 4;
  global_raw_cards_storage[0x20].sleight_color = 0x10;
  global_raw_cards_storage[0x21].sleight_color = 2;
  global_raw_cards_storage[0x22].sleight_color = 4;
  global_raw_cards_storage[0x23].sleight_color = 8;
  global_raw_cards_storage[0x24].sleight_color = 0x10;
  global_raw_cards_storage[0x25].sleight_color = 0x20;
  global_raw_cards_storage[0x33].sleight_color = 0x20;
  global_raw_cards_storage[0x34].sleight_color = 4;
  global_raw_cards_storage[0x3a].sleight_color = 8;
  global_raw_cards_storage[0x3b].sleight_color = 2;
  global_raw_cards_storage[0x51].sleight_color = 2;
  global_raw_cards_storage[100].sleight_color = 0x20;
  global_raw_cards_storage[0x69].sleight_color = 8;
  global_raw_cards_storage[0x7b].sleight_color = 0x10;
  global_raw_cards_storage[0x80].sleight_color = 0x20;
  global_raw_cards_storage[0x8e].sleight_color = 2;
  global_raw_cards_storage[0x8f].sleight_color = 8;
  global_raw_cards_storage[0x1b7].sleight_color = 4;
  global_raw_cards_storage[0x367].sleight_color = 2;
  global_raw_cards_storage[0xaf].sleight_color = 2;
  global_raw_cards_storage[0xc4].sleight_color = 0x20;
  global_raw_cards_storage[199].sleight_color = 4;
  global_raw_cards_storage[200].sleight_color = 0x10;
  global_raw_cards_storage[0x30d].sleight_color = 0x20;
  global_raw_cards_storage[0xee].sleight_color = 0x30;
  global_raw_cards_storage[0x16c].sleight_color = 4;
  global_raw_cards_storage[0xf2].sleight_color = 2;
  global_raw_cards_storage[0xf5].sleight_color = 4;
  global_raw_cards_storage[0xf6].sleight_color = 2;
  global_raw_cards_storage[0x353].sleight_color = 2;
  global_raw_cards_storage[0x11b].sleight_color = 2;
  global_raw_cards_storage[0x11c].sleight_color = 0x20;
  global_raw_cards_storage[0x120].sleight_color = 8;

  SET_HACK(0x125, 2);
  SET_HACK(7, 8);
  SET_HACK(0x18, 2);
  SET_HACK(0x1a, 0x10);
  SET_HACK(0x132, 0x10);
  SET_HACK(0x2d, 0x30);
  SET_HACK(0x4d, 2);
  SET_HACK(0x56, 0x20);
  SET_HACK(0x5f, 8);
  SET_HACK(0x66, 0x10);
  SET_HACK(0x3a9, 0x10);
  SET_HACK(0x1ab, 4);
  SET_HACK(0x7f, 4);
  SET_HACK(0x86, 2);
  SET_HACK(0x88, 2);
  SET_HACK(0x154, 4);
  SET_HACK(0x90, 8);
  SET_HACK(0x93, 8);
  SET_HACK(0x96, 4);
  SET_HACK(0x2cf, 2);
  SET_HACK(0x367, 2);
  SET_HACK(0xae, 2);
  SET_HACK(0xba, 4);
  SET_HACK(0xda, 4);
  SET_HACK(0x30e, 4);
  SET_HACK(0xde, 8);
  SET_HACK(900, 2);
  SET_HACK(0xfd, 4);
  SET_HACK(0x109, 0x10);
  SET_HACK(0x123, 2);

  global_raw_cards_storage[0x1c2].sleight_color = 0x10;
  global_raw_cards_storage[0x225].sleight_color = 2;
  global_raw_cards_storage[0xd2].sleight_color = 2;
  global_raw_cards_storage[0x60].sleight_color = 0x10;

  SET_HACK(0x196, 4);
  SET_HACK(0x1a0, 8);
  SET_HACK(0x1b5, 8);
  SET_HACK(0x1b8, 4);
  SET_HACK(0x1c5, 4);
  SET_HACK(0x1c7, 4);
  SET_HACK(9, 0x12);
  SET_HACK(0xc, 10);
  SET_HACK(0xbd, 0x30);
  SET_HACK(0xd4, 0x28);
  SET_HACK(0xd8, 0x22);
  SET_HACK(0xdb, 2);
  SET_HACK(0xf1, 0x18);
  SET_HACK(0xfc, 0xc);
  SET_HACK(0xfe, 0x24);
  SET_HACK(0x102, 6);
  SET_HACK(0x10a, 0x14);
  SET_HACK(0x60, 0x10);

#undef SET_HACK

  return global_available_slots;
}

static void
free_db(void)
{
  FREEZ(global_raw_rarities);
  FREEZ(global_raw_dbinfo);

  if (!global_using_main_db)
  {
    FREEZ(global_base_txt);
  }

  global_raw_cards_ptr = global_raw_cards_storage;
  cards_ptr = NULL;
}

static int
read_db(void)
{
  int rval = read_db_guts("Cards.dat");
  if (rval)
    global_db_read = true;
  if (!rval)
    free_db();
  return rval;
}

// FUNCTION: DECKDLL 0x1001b510
static char *
CsvParseNextField(char **txt)
{
  struct
  {
    char *cr;
    char *next;
    char *comma;
    char *p;
    char *field;
  } s;

  s.field = *txt;

  if (*s.field == 0x22)
  {
    s.field += 1;
    s.p = strchr(s.field, 0x22);
    *s.p = '\0';
    s.p++;
    if (*s.p == 0x2c)
      s.next = s.p + 1;
    else
      s.next = s.p + 2;
  }
  else
  {
    s.comma = strchr(s.field, 0x2c);
    s.cr = strchr(s.field, 0xd);
    if (s.comma != NULL && s.cr > s.comma)
    {
      *s.comma = '\0';
      s.next = s.comma + 1;
    }
    else
    {
      *s.cr = '\0';
      s.next = s.cr + 2;
    }
  }

  *txt = s.field;
  return s.next;
}

// FUNCTION: DECKDLL 0x1001b832
static int
make_orig_rarities(const char *filename, OrigRarities *orig_rarities)
{
  struct
  {
    HANDLE hfile;      /* ebp - 0x8c */
    int rval;          /* ebp - 0x88 */
    DWORD bytes_read;  /* ebp - 0x84 */
    DWORD size;        /* ebp - 0x80 */
    char *next;        /* ebp - 0x7c */
    char set_id;       /* ebp - 0x78 */
    char pad_set_id[3];
    int i;             /* ebp - 0x74 */
    char *line;        /* ebp - 0x70 */
  } s;
  char set_names[7][15] = {
      "Antiquities",
      "Arabian",
      "Astral",
      "Dark",
      "Legends",
      "Promo",
      "Unlimited",
  };

  s.rval = 0;
  for (s.i = 0;; s.i++)
  {
    if (global_available_slots <= s.i)
      break;
    ((unsigned char *)orig_rarities)[s.i * 6] = SET_INVALID;
  }

  s.hfile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  if (s.hfile != INVALID_HANDLE_VALUE)
  {
    s.size = GetFileSize(s.hfile, NULL);
    global_deckbuilder_csv_raw = (char *)malloc(s.size + 1);
    if (global_deckbuilder_csv_raw)
    {
      ReadFile(s.hfile, global_deckbuilder_csv_raw, s.size, &s.bytes_read, NULL);
      s.line = global_deckbuilder_csv_raw;
      s.line = strchr(s.line, '\n') + 1;
      s.line = strchr(s.line, '\n') + 1;

      for (s.i = 0;; s.i = s.i + 1)
      {
        if (global_available_slots <= s.i)
          break;
        s.next = s.line;
        s.next = CsvParseNextField(&s.line);
        s.line = s.next;
        s.next = CsvParseNextField(&s.line);
        s.line = s.next;
        s.next = CsvParseNextField(&s.line);

        s.set_id = '\0';
        while (s.set_id < '\a' && strcmp(set_names[s.set_id], s.line))
          s.set_id = s.set_id + '\x01';

        if (s.set_id == '\a')
        {
          ((unsigned char *)orig_rarities)[s.i * 6] = SET_INVALID;
          ((char *)orig_rarities)[s.i * 6 + 2] = '-';
          ((char *)orig_rarities)[s.i * 6 + 3] = '-';
          ((char *)orig_rarities)[s.i * 6 + 4] = '-';
          ((char *)orig_rarities)[s.i * 6 + 5] = '-';
        }
        else
        {
          ((char *)orig_rarities)[s.i * 6] = s.set_id;

          s.line = s.next;
          s.next = CsvParseNextField(&s.line);
          ((char *)orig_rarities)[s.i * 6 + 1] = *s.line;

          s.line = s.next;
          s.next = CsvParseNextField(&s.line);
          ((char *)orig_rarities)[s.i * 6 + 2] = *s.line;

          s.line = s.next;
          s.next = CsvParseNextField(&s.line);
          ((char *)orig_rarities)[s.i * 6 + 3] = *s.line;

          s.line = s.next;
          s.next = CsvParseNextField(&s.line);
          ((char *)orig_rarities)[s.i * 6 + 4] = *s.line;

          s.line = s.next;
          s.next = CsvParseNextField(&s.line);
          ((char *)orig_rarities)[s.i * 6 + 5] = *s.line;
        }

        s.line = s.next;
        s.line = strchr(s.next, '\n') + 1;
      }

      s.rval = 1;
    }

    CloseHandle(s.hfile);
  }

  return s.rval;
}

static int
config_get_str(int size, const char *keyname, const char *deflt, char *rval)
{
  return GetPrivateProfileString("DeckBuilder", keyname, deflt, rval, size, global_manalink_ini_path);
}

static int
config_get_int(int deflt, const char *keyname)
{
  return GetPrivateProfileInt("DeckBuilder", keyname, deflt, global_manalink_ini_path);
}

// FUNCTION: DECKDLL 0x1000d9f1
static void
read_manalink_ini(void)
{
  struct
  {
    HKEY key; /* ebp - 0x6c */
    BYTE buf[100]; /* ebp - 0x68 */
    DWORD buflen; /* ebp - 0x4 */
  } s;

  if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\MicroProse\\Magic: The Gathering\\DeckBuilderOptions", 0, KEY_QUERY_VALUE, &s.key) == ERROR_SUCCESS)
  {
    s.buflen = 10;
    if (RegQueryValueEx(s.key, "Consolidate", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      sscanf((char *)s.buf, "%d", (int *)&global_cfg_consolidate_from_registry);
    else
      global_cfg_consolidate_from_registry = 1;

    if (RegQueryValueEx(s.key, "Music", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      sscanf((char *)s.buf, "%d", (int *)((char *)&global_cfg_consolidate_from_registry + 1));
    else
      global_cfg_music = 1;

    if (RegQueryValueEx(s.key, "Effects", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      sscanf((char *)s.buf, "%d", (int *)((char *)&global_cfg_consolidate_from_registry + 2));
    else
      global_cfg_effects = 1;

    RegCloseKey(s.key);
  }
  else
  {
    global_cfg_consolidate_from_registry = 1;
    global_cfg_music = 1;
    global_cfg_effects = 1;
  }

  if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\MicroProse\\Magic: The Gathering", 0, KEY_QUERY_VALUE, &s.key) == ERROR_SUCCESS)
  {
    s.buflen = 100;
    if (RegQueryValueEx(s.key, "Name", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      strcpy(global_cfg_player_name, (char *)s.buf);
    else
      strcpy(global_cfg_player_name, "User");

    s.buflen = 100;
    if (RegQueryValueEx(s.key, "Email", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      strcpy(global_cfg_email, (char *)s.buf);
    else
      strcpy(global_cfg_email, "User E-Mail");

    RegCloseKey(s.key);
  }
  else
    strcpy(global_cfg_email, "User E-Mail");

  if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\MicroProse\\Magic: The Gathering\\DuelOptions", 0, KEY_QUERY_VALUE, &s.key) == ERROR_SUCCESS)
  {
    s.buflen = 100;
    if (RegQueryValueEx(s.key, "ExpandTextOnBigCard", NULL, NULL, s.buf, &s.buflen) == ERROR_SUCCESS)
      sscanf((char *)s.buf, "%d", &global_cfg_expand_text);
    else
      global_cfg_expand_text = 0;
    RegCloseKey(s.key);
  }
  else
    global_cfg_expand_text = 0;
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
    wsprintf((char *)buf, "%d", (int)(signed char)global_cfg_consolidate_from_registry);
    RegSetValueEx(key, "Consolidate", 0, 1, buf, strlen((char *)buf) + 1);

    wsprintf((char *)buf, "%d", (int)(signed char)global_cfg_music);
    RegSetValueEx(key, "Music", 0, 1, buf, strlen((char *)buf) + 1);

    wsprintf((char *)buf, "%d", (int)(signed char)global_cfg_effects);
    RegSetValueEx(key, "Effects", 0, 1, buf, strlen((char *)buf) + 1);

    RegFlushKey(key);
    RegCloseKey(key);
  }

  if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\MicroProse\\Magic: The Gathering\\DuelOptions",
                     0, NULL, 0, 0xF003F, NULL, &key, &disposition) == ERROR_SUCCESS)
  {
    wsprintf((char *)buf, "%d", (int)(signed char)global_cfg_expand_text);
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
static LOGFONT *
LoadFontFromIni(char *name, int italic)
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
static COLORREF
GetPaletteColor(int index)
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
  return ((BYTE)rgb.g << 8) | (BYTE)rgb.r | ((BYTE)rgb.b << 16) | 0x02000000;
}

void delete_and_close_object(HANDLE obj);
static void destroy_create_fonts_resources(void);

// FUNCTION: DECKDLL 0x10011680
static bool
create_fonts(void)
{
  LOGFONT *font;
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

  sprintf(s.path, "%s\\Damage.pic", global_base_directory);
  global_damage_pic = load_pic(s.path);
  sprintf(s.path, "%s\\CardCounters.pic", global_base_directory);
  global_card_counters_pic = load_pic(s.path);
  sprintf(s.path, "%s\\ManaSymbols.pic", global_base_directory);
  global_mana_symbols_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Abilities.pic", global_base_directory);
  global_abilities_pic = load_pic(s.path);
  sprintf(s.path, "%s\\ManaStripes.pic", global_base_directory);
  global_mana_stripes_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Summon.pic", global_base_directory);
  global_summon_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Dying.pic", global_base_directory);
  global_dying_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Target.pic", global_base_directory);
  global_target_pic = load_pic(s.path);
  sprintf(s.path, "%s\\CantTarget.pic", global_base_directory);
  global_cant_target_pic = load_pic(s.path);
  sprintf(s.path, "%s\\WillUntap.pic", global_base_directory);
  global_will_untap_pic = load_pic(s.path);

  font = LoadFontFromIni("SmallCardPT", 0);
  global_smallcard_pt_font = CreateFontIndirectA(font);
  font = LoadFontFromIni("Damage", 0);
  global_damage_font = CreateFontIndirectA(font);
  font = LoadFontFromIni("IDTag", 0);
  global_idtag_font = CreateFontIndirectA(font);

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

  s.local_8 = 1;
  for (s.local_c = 0; s.local_c < 10; ++s.local_c)
  {
    global_debug_pens[s.local_c] =
        CreatePen(6, 3,
                  ((s.local_c * 0x14 & 0xffU) << 8) |
                      (unsigned int)(BYTE)((char)s.local_c * '\n') |
                      ((unsigned int)(BYTE)((char)s.local_c * 'K') << 16));
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
static void
destroy_create_fonts_resources(void)
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
static bool
process_cue_cards(MSG *msg)
{
  struct
  {
    POINT screen_point; /* ebp - 0x18 */
    LRESULT msgnum; /* ebp - 0x10 */
    HWND hwnd; /* ebp - 0xc */
    POINT client_point; /* ebp - 0x8 */
  } s;
  static int last_cursor_x = 0;
  static int last_cursor_y = 0;
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
                           (((s.client_point.y & 0xffff) << 16) | (s.client_point.x & 0xffff)));
    if (s.msgnum != 0)
      SendMessage(global_cuecard_hwnd,
                  0x400,
                  ((((s.screen_point.y + 0xf) & 0xffff) << 16) | ((s.screen_point.x + 5) & 0xffff)),
                  (LPARAM)text_lines[s.msgnum - 1]);
    return true;
  }
  else if (msg->message == WM_MOUSEMOVE)
  {
    GetCursorPos(&s.screen_point);
    if (abs(last_cursor_y - s.screen_point.y) + abs(last_cursor_x - s.screen_point.x) < 2)
      return false;
    else
    {
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
  }
  else if ((unsigned int)msg->message < WM_MOUSEMOVE || (unsigned int)msg->message > WM_MBUTTONDBLCLK)
    return false;
  else
  {
    KillTimer(global_main_hwnd, 0);
    ShowWindow(global_cuecard_hwnd, SW_HIDE);
    return false;
  }
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
    if (global_db_flags_1 & DBFLAGS_NOCARDCOUNTCHECK)
      goto check_external_functions;

    if (!(global_db_flags_1 & DBFLAGS_GAUNTLET))
      goto check_external_functions_done;

  check_external_functions:
    if (!CardIDFromType || !CardTypeFromID || !CardInDeck || !SetCardInDeck || !Gold || !global_external_deckname)
    {
      fatal_err("Deck Builder WinMain: Couldn't find External Functions.", 0);
      return 0;
    }

  check_external_functions_done:
    ;
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
  global_cfg_consolidate = (int)(signed char)global_cfg_consolidate_from_registry;
  global_deck_was_edited = 0;
  global_deckname_set = 0;
  global_deckbuilder_unknown_1019f010 = 0;
  global_current_deck = 0;

  GetModuleFileNameA(0, global_base_directory, 0x105);
  s.pcVar3 = strrchr(global_base_directory, '\\');
  *s.pcVar3 = 0;

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
  struct {
    char path[0x50];
    int i;
  } s;

  if (global_db_flags_1 & DBFLAGS_SHANDALAR)
    return;

  // Very expensive srand()
  for (s.i = 0; (clock() % 1024) > s.i;s.i++)
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
  HDC hdc;
  RECT r;
  char buf[261];
  char *p;
  char *q;

  switch (msg)
  {
  case WM_INITDIALOG:
    load_text("Menus", "TITLEDIALOG");
    SetWindowText(hdlg, text_lines[0]);
    set_dlg_text(hdlg, RES_DECKINFO_DECKTITLE, text_lines[1]);
    set_dlg_text(hdlg, RES_DECKINFO_DESCRIPTION, text_lines[2]);
    set_dlg_text(hdlg, RES_DECKINFO_NAME, text_lines[3]);
    set_dlg_text(hdlg, RES_DECKINFO_EMAIL, text_lines[4]);
    set_dlg_text(hdlg, RES_DECKINFO_DATE, text_lines[5]);
    set_dlg_text(hdlg, RES_DECKINFO_COMMENT, text_lines[7]);
    set_dlg_text(hdlg, RES_DECKINFO_VERSION, text_lines[8]);

    load_text("Menus", "OKCANCEL");
    set_dlg_text(hdlg, RES_BUTTON_OK, text_lines[0]);
    set_dlg_text(hdlg, RES_BUTTON_CANCEL, text_lines[1]);
    set_dlg_text_limited(hdlg, RES_DECKINFO_DECKTITLE_EDITTEXT, global_deckname, 28);
    set_dlg_text_limited(hdlg, RES_DECKINFO_DESCRIPTION_EDITTEXT, global_deck_description, 18);
    set_dlg_text_limited(hdlg, RES_DECKINFO_NAME_EDITTEXT, global_deck_author, 78);
    set_dlg_text_limited(hdlg, RES_DECKINFO_EMAIL_EDITTEXT, global_deck_email, 78);
    set_dlg_text_limited(hdlg, RES_DECKINFO_DATE_EDITTEXT, global_deck_creation_date, 19);
    set_dlg_text_limited(hdlg, RES_DECKINFO_VERSION_EDITTEXT, "4th Edition", 13);
    set_dlg_text_limited(hdlg, RES_DECKINFO_COMMENT_EDITTEXT, global_deck_comments, 398);
    return 0;

  case WM_ERASEBKGND:
  {
    hdc = wparam;
    ApplyCardArtPaletteToDc(hdc);

    GetClientRect(hdlg, &r);
    DrawBitmapToRect(hdc, &r, global_deckinfo_pic);
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
      EndDialog(hdlg, 0);
    else if (LOWORD(wparam) == RES_BUTTON_OK)
    {
      GetWindowText(GetDlgItem(hdlg, RES_DECKINFO_DECKTITLE_EDITTEXT), buf, 261);

      for (p = buf, q = buf; (*p = *q); ++p, ++q)
        if (*p == '.')
          --p;

      if (strcmp(buf, global_deckname))
      {
        global_deck_was_edited = global_deckname_set = true;
        strcpy(global_deckname, buf);
      }

      GetWindowText(GetDlgItem(hdlg, RES_DECKINFO_DESCRIPTION_EDITTEXT), global_deck_description, 21);
      GetWindowText(GetDlgItem(hdlg, RES_DECKINFO_NAME_EDITTEXT), global_deck_author, 81);
      GetWindowText(GetDlgItem(hdlg, RES_DECKINFO_EMAIL_EDITTEXT), global_deck_email, 81);
      GetWindowText(GetDlgItem(hdlg, RES_DECKINFO_DATE_EDITTEXT), global_deck_creation_date, 22);
      global_deck_revision = 1;
      global_deck_edition[GetWindowText(GetDlgItem(hdlg, RES_DECKINFO_VERSION_EDITTEXT), global_deck_edition, 16)] = 0;
      GetWindowText(GetDlgItem(hdlg, RES_DECKINFO_COMMENT_EDITTEXT), global_deck_comments, 401);

      sprintf(global_deck_filename, "%s%s.dck", global_playdeck_path, global_deckname);

      EndDialog(hdlg, 1);
    }
    return 1;
  }

  default:
    return 0;
  }
}

// FUNCTION: DECKDLL 0x10006df3
static bool show_dialog_deckinfo(void)
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
INT_PTR CALLBACK dlgproc_GroupMove(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  HDC hdc;
  RECT r;

  switch (msg)
  {
  case WM_INITDIALOG:
  {
    load_text("Menus", "GROUPMOVE");
    SetWindowText(hdlg, text_lines[0]);
    set_dlg_text(hdlg, RES_GROUPMOVE_BUTTON_BLACK, text_lines[1]);
    set_dlg_text(hdlg, RES_GROUPMOVE_BUTTON_BLUE, text_lines[2]);
    set_dlg_text(hdlg, RES_GROUPMOVE_BUTTON_GREEN, text_lines[3]);
    set_dlg_text(hdlg, RES_GROUPMOVE_BUTTON_RED, text_lines[4]);
    set_dlg_text(hdlg, RES_GROUPMOVE_BUTTON_WHITE, text_lines[5]);
    set_dlg_text(hdlg, RES_GROUPMOVE_BUTTON_ARTIFACT, text_lines[6]);

    load_text("Menus", "OKCANCEL");
    set_dlg_text(hdlg, RES_BUTTON_OK, text_lines[0]);
    set_dlg_text(hdlg, RES_BUTTON_CANCEL, text_lines[1]);

    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_BLACK), !!(global_dlg_parameter & 1));
    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_BLUE), !!(global_dlg_parameter & 2));
    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_GREEN), !!(global_dlg_parameter & 4));
    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_RED), !!(global_dlg_parameter & 8));
    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_WHITE), !!(global_dlg_parameter & 0x10));
    EnableWindow(GetDlgItem(hdlg, RES_GROUPMOVE_BUTTON_ARTIFACT), !!(global_dlg_parameter & 0x20));
    return 0;
  }

  case WM_ERASEBKGND:
  {
    hdc = wparam;
    ApplyCardArtPaletteToDc(hdc);

    GetClientRect(hdlg, &r);
    DrawBitmapToRect(hdc, &r, global_groupmove_pic);
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
    switch (LOWORD(wparam))
    {
    case RES_BUTTON_OK:
      global_dlg_result = 0;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_BLACK))
        global_dlg_result |= COLOR_TEST_BLACK;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_BLUE))
        global_dlg_result |= COLOR_TEST_BLUE;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_GREEN))
        global_dlg_result |= COLOR_TEST_GREEN;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_RED))
        global_dlg_result |= COLOR_TEST_RED;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_WHITE))
        global_dlg_result |= COLOR_TEST_WHITE;
      if (IsDlgButtonChecked(hdlg, RES_GROUPMOVE_BUTTON_ARTIFACT))
        global_dlg_result |= COLOR_TEST_ARTIFACT;
      global_dlg_result >>= 1;
      EndDialog(hdlg, 1);
      break;

    case RES_BUTTON_CANCEL:
      global_dlg_result = 0;
      EndDialog(hdlg, 0);
      break;
    }
    return 1;
  }

  default:
    return 0;
  }
}

// FUNCTION: DECKDLL 0x10007b21
static bool show_dialog_groupmove(void)
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
    int pad0;
    UINT msg_copy;
    RECT client_rect;
    LPARAM lparam_copy;
    HDC hdc_copy;
    int text_len;
    char path[264];
    char edit_txt[12];
    RECT rect_main;
    RECT rect_dlg;
  } s;

  s.msg_copy = msg;
  switch (s.msg_copy)
  {
  case WM_INITDIALOG:
  {
    sprintf(s.edit_txt, "%d", global_dlg_parameter);
    SetWindowTextA(hdlg, global_ask_x_dlg_title);
    SetWindowTextA(GetDlgItem(hdlg, 1000), s.edit_txt);

    load_text("menus", "OKCANCEL");
    SetWindowTextA(GetDlgItem(hdlg, 1), text_lines[0]);
    SetWindowTextA(GetDlgItem(hdlg, 2), text_lines[1]);

    GetWindowRect(global_main_hwnd, &s.rect_main);
    GetWindowRect(hdlg, &s.rect_dlg);
    SetWindowPos(hdlg, (HWND)0,
                 s.rect_main.left +
                     ((s.rect_main.right - s.rect_main.left) - (s.rect_dlg.right - s.rect_dlg.left)) / 2,
                 s.rect_main.top +
                     ((s.rect_main.bottom - s.rect_main.top) - (s.rect_dlg.bottom - s.rect_dlg.top)) / 2,
                 0, 0, 5);

    sprintf(s.path, "%s\\GAUN_Options.pic", global_duelart_path);
    global_gaun_options_pic = load_pic(s.path);
    return 0;
  }

  case WM_ERASEBKGND:
  {
    ApplyCardArtPaletteToDc((HDC)wparam);
    GetClientRect(hdlg, &s.client_rect);
    if (global_gaun_options_pic == (HANDLE)0)
    {
      FillRect((HDC)wparam, &s.client_rect, (HBRUSH)GetStockObject(2));
    }
    else
    {
      DrawBitmapToRect((HDC)wparam, &s.client_rect, global_gaun_options_pic);
    }
    return 1;
  }

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
  {
    s.hdc_copy = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc_copy);
    s.lparam_copy = lparam;
    SetBkMode(s.hdc_copy, 1);
    return (INT_PTR)GetStockObject(5);
  }

  case WM_COMMAND:
  {
    if (LOWORD(wparam) == 1)
    {
      s.text_len = GetWindowTextA(GetDlgItem(hdlg, 1000), s.edit_txt, 10);
      s.edit_txt[s.text_len] = '\0';
      global_dlg_result = atoi(s.edit_txt);
      EndDialog(hdlg, 1);
      if (global_gaun_options_pic != (HANDLE)0)
      {
        delete_and_close_object(global_gaun_options_pic);
      }
    }
    else if (LOWORD(wparam) == 2)
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
  }
  return 0;
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
    int tmp;              /* [ebp-0x138] */
    RECT r;               /* [ebp-0x134] */
    LPARAM lparam_copy;   /* [ebp-0x124] */
    HDC hdc;              /* [ebp-0x120] */
    unsigned int pad_11c; /* [ebp-0x11c] */
    HGDIOBJ hobj;         /* [ebp-0x118] */
    char local_118[264];  /* [ebp-0x114] */
    int j;                /* [ebp-0x0c] */
    int idx;              /* [ebp-0x08] */
    int done;             /* [ebp-0x04] */
  } s;

  {
    UINT msg_copy;
    msg_copy = msg;
    switch (msg_copy)
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
        if (((int *)global_excessive_cards)[s.idx * 3] == -1)
        {
          s.done = 1;
        }
        else
        {
          s.tmp = s.idx * 3;
          ++s.idx;
          SendDlgItemMessageA(hdlg, RES_INFOBOX_LIST, LB_ADDSTRING, 0,
                              ((LPARAM *)global_excessive_cards)[s.tmp + 2]);
        }
      }

      sprintf(s.local_118, "%s\\GAUN_Results.pic", global_duelart_path);
      global_gaun_results_pic = load_pic(s.local_118);
      return 0;

    case WM_ERASEBKGND:
      s.hdc = (HDC)wparam;
      ApplyCardArtPaletteToDc(s.hdc);
      GetClientRect(hdlg, &s.r);

      if (global_gaun_results_pic == (HANDLE)0)
      {
        FillRect(s.hdc, &s.r, (HBRUSH)GetStockObject(2));
      }
      else
      {
        DrawBitmapToRect(s.hdc, &s.r, global_gaun_results_pic);
      }
      return 1;

    case WM_COMMAND:
      if (LOWORD(wparam) == RES_BUTTON_OK)
      {
        s.done = 0;
        s.idx = s.done;
        while (s.done == 0)
        {
          if (((int *)global_excessive_cards)[s.idx * 3] == -1)
          {
            s.done = 1;
          }
          else
          {
            for (s.j = 0; s.j < global_deck_num_entries; ++s.j)
            {
              if ((global_deck[s.j].GDE_csvid == ((int *)global_excessive_cards)[s.idx * 3]) &&
                  (global_deck[s.j].GDE_Available == 0) &&
                  (0 < ((int *)global_excessive_cards)[s.idx * 3 + 1]))
              {
                global_deck[s.j].GDE_Available = 1;
                global_deck[s.j].GDE_DecksBits &= ~(1 << ((unsigned char)global_current_deck & 0x1f));
                ((int *)global_excessive_cards)[s.idx * 3 + 1] = ((int *)global_excessive_cards)[s.idx * 3 + 1] + -1;
              }
            }
            ++s.idx;
          }
        }

        EndDialog(hdlg, 1);
        if (global_gaun_results_pic != (HANDLE)0)
          delete_and_close_object(global_gaun_results_pic);
      }
      else if (LOWORD(wparam) == RES_BUTTON_CANCEL)
      {
        if (global_gaun_results_pic != (HANDLE)0)
          delete_and_close_object(global_gaun_results_pic);
        EndDialog(hdlg, 0);
      }
      return 1;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC:
      s.hdc = (HDC)wparam;
      ApplyCardArtPaletteToDc(s.hdc);
      s.lparam_copy = lparam;
      SetBkMode(s.hdc, 1);
      s.hobj = GetStockObject(5);
      return (INT_PTR)s.hobj;

    default:
      return 0;
    }
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
      SetWindowTextA(GetDlgItem(hdlg, 0xffff), text_lines[1]);

      load_text("menus", "OKCANCEL");
      SetWindowTextA(GetDlgItem(hdlg, 1), text_lines[0]);
      SetWindowTextA(GetDlgItem(hdlg, 2), text_lines[1]);

      s.hwnd_list = CreateWindowExA(0, "LISTBOX", "",
                                    0x40a00003,
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
      if ((wparam & 0xFFFF) == 1)
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
      else if ((wparam & 0xFFFF) == 2)
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
      SetBkMode(s.hdc_copy, 1);
      return s.pad_500 = GetStockObject(5);

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
static void
clear_packs(void)
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
static void
clear_packs_copy(void)
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
static void
count_packs(void)
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
    s.csvid = global_edited_deck[s.l].DeckEntry_csvid;
    s.amt = global_edited_deck[s.l].DeckEntry_Amount;

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
  HDC paintdc;
  HFONT font;
  HDC hdc;

  switch (msg)
  {
  case WM_PAINT:
  {
    PAINTSTRUCT paint;

    if (paintdc = BeginPaint(hwnd, &paint))
    {
      HFONT cue_font = GetWindowLong(hwnd, CUECARD_FONT_INDEX);
      RECT r;
      HBRUSH brush1;
      HBRUSH brush2;
      char txt[100];

      ApplyCardArtPaletteToDc(paintdc);

      GetClientRect(hwnd, &r);

      brush1 = CreateSolidBrush(PALETTERGB(127, 127, 127));
      FillRect(paintdc, &r, brush1);

      r.right -= 2;
      r.bottom -= 2;
      brush2 = CreateSolidBrush(PALETTERGB(210, 190, 150));
      FillRect(paintdc, &r, brush2);

      SetTextColor(paintdc, PALETTERGB(0, 0, 0));
      SetBkMode(paintdc, TRANSPARENT);
      GetWindowText(hwnd, txt, 100);
      SelectObject(paintdc, cue_font);
      DrawText(paintdc, txt, -1, &r, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

      EndPaint(hwnd, &paint);

      DELETE_OBJ(brush1);
      DELETE_OBJ(brush2);
    }
    return 0;
  }

  case WM_CREATE:
  {
    if (font = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                          OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana"))
    {
      SetWindowLong(hwnd, CUECARD_FONT_INDEX, font);
      return 0;
    }
    else
      return -1;
  }

  case WM_DESTROY:
    if (font = GetWindowLong(hwnd, CUECARD_FONT_INDEX))
      DeleteObject(font);
    return 0;

  case WM_KEYDOWN:
    SetFocus(global_main_hwnd);
    PostMessage(GetFocus(), WM_KEYDOWN, wparam, lparam);
    return 0;

  case 0x8400:
  {
    int x;
    int y;
    char *str;
    HDC hdc_local;
    SIZE sz;
    int width;
    int height;
    RECT r_local;

    font = GetWindowLong(hwnd, CUECARD_FONT_INDEX);

    x = LOWORD(wparam);
    y = HIWORD(wparam);
    str = lparam;

    if (hdc_local = GetDC(hwnd))
    {
      ApplyCardArtPaletteToDc(hdc_local);
      SelectObject(hdc_local, font);

      GetTextExtentPoint32(hdc_local, str, strlen(str), &sz);
      width = sz.cx + 10;
      height = sz.cy + 6;

      ReleaseDC(hwnd, hdc_local);

      GetClientRect(GetParent(hwnd), &r_local);
      if (x > r_local.right - width)
        x = r_local.right - width;

      MoveWindow(hwnd, x, y, width, height, TRUE);
      SetWindowText(hwnd, str);
      ShowWindow(hwnd, SW_SHOW);
      InvalidateRect(hwnd, NULL, TRUE);
    }
    return 0;
  }

  default:
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: DECKDLL 0x1002764d
LRESULT CALLBACK
wndproc_TitleClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  HDC hdc;
  HDC chdc;
  RECT r;
  BITMAP bmp;

  switch (msg)
  {
  case WM_PAINT:
  {
    PAINTSTRUCT paint;
    HDC paintdc;
    RECT rect;

    paintdc = BeginPaint(hwnd, &paint);
    ApplyCardArtPaletteToDc(paintdc);
    GetClientRect(hwnd, &rect);
    InflateRect(&rect, -5, -5);

    if (strlen(global_deckname) >= 20)
      SelectObject(paintdc, global_font_28percent);
    else
      SelectObject(paintdc, global_font_40percent);

    SetBkMode(paintdc, TRANSPARENT);
    SetMapMode(paintdc, MM_ANISOTROPIC);

    SetWindowExtEx(paintdc, RECT_WIDTH(rect), RECT_HEIGHT(rect), NULL);
    SetViewportExtEx(paintdc, RECT_WIDTH(rect), RECT_HEIGHT(rect), NULL);

    rect.left += 3;
    rect.top += 3;
    SetTextColor(paintdc, PALETTERGB(20, 46, 77));
    DrawText(paintdc, global_deckname, strlen(global_deckname), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

    rect.left -= 3;
    rect.top -= 3;
    SetTextColor(paintdc, PALETTERGB(243, 209, 175));
    DrawText(paintdc, global_deckname, strlen(global_deckname), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

    EndPaint(hwnd, &paint);
    return 0;
  }

  case WM_CREATE:
  case WM_DESTROY:
    return 0;

  case WM_ERASEBKGND:
  {
    hdc = wparam;
    ApplyCardArtPaletteToDc(hdc);

    chdc = CreateCompatibleDC(hdc);
    ApplyCardArtPaletteToDc(chdc);

    SelectObject(chdc, global_pic_dektit1);

    GetClientRect(hwnd, &r);

    GetObject(global_pic_dektit1, sizeof(BITMAP), &bmp);

    StretchBlt(hdc, 0, 0, r.right, r.bottom, chdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

    DELETE_DC(chdc);
    return 1;
  }

  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
    if (!(global_db_flags_1 & (DBFLAGS_EDITDECK | DBFLAGS_SHANDALAR)))
    {
      show_dialog_deckinfo();
      InvalidateRect(hwnd, NULL, TRUE);
    }
    return 0;

  default:
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }
}

static void
fill_back(HWND hwnd, HDC hdc)
{
  int x;
  int y;
  RECT rect;
  BITMAP bmp;
  HDC chdc;

  chdc = CreateCompatibleDC(hdc);
  ApplyCardArtPaletteToDc(chdc);
  SelectObject(chdc, global_pic_dektile4);

  GetClientRect(hwnd, &rect);
  GetObject(global_pic_dektile4, sizeof(BITMAP), &bmp);

  for (x = 0; x < rect.right; x += bmp.bmWidth)
    for (y = rect.bottom; y > -29; y -= bmp.bmHeight)
      BitBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, chdc, 0, 0, SRCCOPY);

  DELETE_DC(chdc);
}

// FUNCTION: DECKDLL 0x10010f75
LRESULT CALLBACK
wndproc_FullCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  int card_to_draw, expanded_text, wanted_card;
  switch (msg)
  {
  case WM_PAINT:
  {
    PAINTSTRUCT paint;
    HDC paintdc;
    RECT rect;

    card_to_draw = GetWindowLong(hwnd, FULLCARD_CSVID_INDEX);
    expanded_text = GetWindowLong(hwnd, FULLCARD_EXPANDTEXT_INDEX);

    paintdc = BeginPaint(hwnd, &paint);

    GetClientRect(hwnd, &rect);

    ApplyCardArtPaletteToDc(paintdc);

    if (card_to_draw >= 0)
      DrawFullCard(paintdc, &rect, &cards_ptr[card_to_draw], 0, 1, expanded_text, 0);
    else
      DrawCardBack(hwnd, paintdc);

    EndPaint(hwnd, &paint);
    return 0;
  }

  case WM_CREATE:
    global_fullcard_popup = CreatePopupMenu();
    load_text("Menus", "FULLCARD");
    AppendMenu(global_fullcard_popup, MF_STRING, RES_FULLCARDMENU_EXPAND, text_lines[0]);
    SetWindowLong(hwnd, FULLCARD_CSVID_INDEX, -1);
    SetWindowLong(hwnd, FULLCARD_EXPANDTEXT_INDEX, global_cfg_expand_text);
    CHECKMENU_IF(global_fullcard_popup, RES_FULLCARDMENU_EXPAND, global_cfg_expand_text);
    return 0;

  case WM_DESTROY:
    global_cfg_expand_text = GetWindowLong(hwnd, FULLCARD_EXPANDTEXT_INDEX);
    DESTROY_MENU(global_fullcard_popup);
    return 0;

  case WM_ERASEBKGND:
  {
    HDC hdc;

    hdc = wparam;
    ApplyCardArtPaletteToDc(hdc);
    DrawCardBack(hwnd, hdc);
    return 1;
  }

  case WM_RBUTTONDOWN:
  {
    POINT point;

    point.x = GET_X_LPARAM(lparam);
    point.y = GET_Y_LPARAM(lparam);
    ClientToScreen(hwnd, &point);

    TrackPopupMenu(global_fullcard_popup, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
    return 0;
  }

  case WM_COMMAND:
    if (LOWORD(wparam) == RES_FULLCARDMENU_EXPAND)
      SendMessage(hwnd, 0x8401, SendMessage(hwnd, 0x8402, 0, 0) ? 0 : 1, 0);
    return 0;

  case 0x8400:
    card_to_draw = wparam;
    if (GetWindowLong(hwnd, FULLCARD_CSVID_INDEX) != card_to_draw)
    {
      if (card_to_draw < 0 && global_edited_deck[0].DeckEntry_Amount > 0)
        card_to_draw = global_edited_deck[0].DeckEntry_csvid;
      SetWindowLong(hwnd, FULLCARD_CSVID_INDEX, card_to_draw);
      InvalidateRect(hwnd, NULL, FALSE);
    }
    return 0;

  case 0x8401:
    expanded_text = wparam;
    SetWindowLong(hwnd, FULLCARD_EXPANDTEXT_INDEX, expanded_text);
    InvalidateRect(hwnd, NULL, FALSE);
    CHECKMENU_IF(global_fullcard_popup, RES_FULLCARDMENU_EXPAND, expanded_text);
    return expanded_text;

  case 0x8402:
    return GetWindowLong(hwnd, FULLCARD_EXPANDTEXT_INDEX);

  case 0x8466:
    wanted_card = wparam;
    if (wanted_card == GetWindowLong(hwnd, FULLCARD_CSVID_INDEX))
      InvalidateRect(hwnd, NULL, FALSE);
    return 0;

  default:
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }
}

static void
update_scroll_range(void)
{
  int r = GetWindowLong(global_horzlist_hwnd, HORZLIST_LASTPIC_INDEX) - GetWindowLong(global_horzlist_hwnd, HORZLIST_PICSINLINE_INDEX);
  if (r < 0)
    r = 0;
  SetScrollRange(global_horzlist_hwnd, 0, 0, r, 0);
}

#define MAKELONG2(a, b) ((LONG)(((a) & 0xffff) | ((DWORD)((WORD)(b))) << 16))

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

static void
make_new_search(HWND hwnd)
{
  char *p;

  global_search_string[256] = 0;
  global_search_string[0] = '*';

  SendMessage(hwnd, WM_GETTEXT, 255, &global_search_string[1]);

  p = &global_search_string[strlen(global_search_string)];
  if (p == global_search_string)
    strcpy(global_search_string, "*");
  else if (*(p - 1) != '*')
    strcpy(p, "*");

  SendMessage(global_horzlist_hwnd, 0x8007, 0, 0);
  filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);
}

LRESULT CALLBACK
wndproc_SearchClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  if (msg == WM_KILLFOCUS)
    make_new_search(hwnd);
  else if (msg == WM_CHAR && wparam == '\r')
  {
    make_new_search(hwnd);
    SetFocus(global_horzlist_hwnd);
    return 0;
  }
  return global_wndproc_std_SearchClass(hwnd, msg, wparam, lparam);
}

// FUNCTION: DECKDLL 0x1002b8f0
static void
insert_cards_into_deck(csvid_t csvid, int num, DeckEntry *tgt_deck)
{
  struct
  {
    int i;     /* ebp - 0x8 */
    int found; /* ebp - 0x4 */
  } s;

  s.i = 0;
  s.found = 0;
  while (s.i < *(int *)((char *)tgt_deck + 0xE14) && s.found == 0)
  {
    if (tgt_deck[s.i].DeckEntry_csvid == csvid)
    {
      tgt_deck[s.i].DeckEntry_Amount += num;
      *(int *)((char *)tgt_deck + 0xE10) += num;
      s.found = 1;
    }
    s.i++;
  }

  if (s.found == 0)
  {
    tgt_deck[*(int *)((char *)tgt_deck + 0xE14)].DeckEntry_csvid = csvid;
    tgt_deck[*(int *)((char *)tgt_deck + 0xE14)].DeckEntry_Amount = num;
    tgt_deck[*(int *)((char *)tgt_deck + 0xE14)].DeckEntry_FullName = global_raw_cards_storage[csvid].full_name;
    *(int *)((char *)tgt_deck + 0xE14) += 1;
    *(int *)((char *)tgt_deck + 0xE10) += num;
  }
}

// FUNCTION: DECKDLL 0x1002bcde
static void remove_cards_from_deck(csvid_t csvid, int num, DeckEntry *tgt_deck)
{
  struct
  {
    int local_10; /* ebp - 0xc */
    int i;        /* ebp - 0x8 */
    int found;    /* ebp - 0x4 */
  } s;

  s.i = 0;
  s.found = 0;
  for (; s.i < *(int *)((char *)tgt_deck + 0xE14) && s.found == 0; s.i++)
  {
    if (tgt_deck[s.i].DeckEntry_csvid == csvid)
    {
      s.found = 1;
      if (tgt_deck[s.i].DeckEntry_Amount == num)
      {
        *(int *)((char *)tgt_deck + 0xE10) -= num;
        for (s.local_10 = s.i; s.local_10 < *(int *)((char *)tgt_deck + 0xE14) - 1; s.local_10++)
        {
          tgt_deck[s.local_10].DeckEntry_csvid = tgt_deck[s.local_10 + 1].DeckEntry_csvid;
          tgt_deck[s.local_10].DeckEntry_Amount = tgt_deck[s.local_10 + 1].DeckEntry_Amount;
          tgt_deck[s.local_10].DeckEntry_FullName = tgt_deck[s.local_10 + 1].DeckEntry_FullName;
        }
        --*(int *)((char *)tgt_deck + 0xE14);
      }
      else if (num < tgt_deck[s.i].DeckEntry_Amount)
      {
        tgt_deck[s.i].DeckEntry_Amount -= num;
        *(int *)((char *)tgt_deck + 0xE10) -= num;
      }
      else
      {
        *(int *)((char *)tgt_deck + 0xE10) -= num;
        num -= tgt_deck[s.i].DeckEntry_Amount;
        s.found = 0;
        for (s.local_10 = s.i; s.local_10 < *(int *)((char *)tgt_deck + 0xE14) - 1; s.local_10++)
        {
          tgt_deck[s.local_10].DeckEntry_csvid = tgt_deck[s.local_10 + 1].DeckEntry_csvid;
          tgt_deck[s.local_10].DeckEntry_Amount = tgt_deck[s.local_10 + 1].DeckEntry_Amount;
          tgt_deck[s.local_10].DeckEntry_FullName = tgt_deck[s.local_10 + 1].DeckEntry_FullName;
        }
        --*(int *)((char *)tgt_deck + 0xE14);
      }
    }
  }
}

// FUNCTION: DECKDLL 0x1002bed0
static void __cdecl
FUN_1002bed0(int which, int csvid, int num, int deck_base)
{
  struct
  {
    int base;       /* ebp - 0x14 */
    int idx;        /* ebp - 0x10 */
    int i;          /* ebp - 0xc */
    int *count_ptr; /* ebp - 0x8 */
    int found;      /* ebp - 0x4 */
  } s;

  if (which == 0)
  {
    s.base = deck_base + 0xE18;
    s.count_ptr = (int *)(deck_base + 0xE90);
  }
  else if (which == 1)
  {
    s.base = deck_base + 0xE94;
    s.count_ptr = (int *)(deck_base + 0xF0C);
  }
  else if (which == 2)
  {
    s.base = deck_base + 0xF10;
    s.count_ptr = (int *)(deck_base + 0xF88);
  }
  else if (which == 5)
  {
    s.base = deck_base + 0x1084;
    s.count_ptr = (int *)(deck_base + 0x10FC);
  }
  else if (which == 4)
  {
    s.base = deck_base + 0x1008;
    s.count_ptr = (int *)(deck_base + 0x1080);
  }
  else
  {
    if (which != 3)
      return;

    s.base = deck_base + 0xF8C;
    s.count_ptr = (int *)(deck_base + 0x1004);
  }

  s.i = 0;
  s.found = 0;
  while (s.i < *s.count_ptr && !s.found)
  {
    if (((int *)s.base)[s.i * 3] == csvid)
    {
      s.found = 1;

      if (((int *)s.base)[s.i * 3 + 1] == num)
      {
        for (s.idx = s.i; s.idx < *s.count_ptr - 1; s.idx++)
        {
          ((int *)s.base)[s.idx * 3] = ((int *)s.base)[s.idx * 3 + 3];
          ((int *)s.base)[s.idx * 3 + 1] = ((int *)s.base)[s.idx * 3 + 4];
          ((int *)s.base)[s.idx * 3 + 2] = ((int *)s.base)[s.idx * 3 + 5];
        }
        *s.count_ptr = *s.count_ptr + -1;
      }
      else if (num < ((int *)s.base)[s.i * 3 + 1])
      {
        ((int *)s.base)[s.i * 3 + 1] -= num;
      }
      else
      {
        num -= ((int *)s.base)[s.i * 3 + 1];
        s.found = 0;

        for (s.idx = s.i; s.idx < *s.count_ptr - 1; s.idx++)
        {
          ((int *)s.base)[s.idx * 3] = ((int *)s.base)[s.idx * 3 + 3];
          ((int *)s.base)[s.idx * 3 + 1] = ((int *)s.base)[s.idx * 3 + 4];
          ((int *)s.base)[s.idx * 3 + 2] = ((int *)s.base)[s.idx * 3 + 5];
        }
        *s.count_ptr = *s.count_ptr + -1;
      }
    }
    s.i++;
  }
}

// FUNCTION: DECKDLL 0x1002c161
static void __cdecl
FUN_1002c161(int which, int csvid, int num, int deck_base)
{
  struct
  {
    int base;       /* ebp - 0x14 */
    int idx;        /* ebp - 0x10 */
    int i;          /* ebp - 0xc */
    int *count_ptr; /* ebp - 0x8 */
    int done;       /* ebp - 0x4 */
  } s;

  if (which == 0)
  {
    s.base = deck_base + 0x117C;
    s.count_ptr = (int *)(deck_base + 0x11F4);
  }
  else if (which == 1)
  {
    s.base = deck_base + 0x11F8;
    s.count_ptr = (int *)(deck_base + 0x1270);
  }
  else if (which == 2)
  {
    s.base = deck_base + 0x1274;
    s.count_ptr = (int *)(deck_base + 0x12EC);
  }
  else if (which == 5)
  {
    s.base = deck_base + 0x13E8;
    s.count_ptr = (int *)(deck_base + 0x1460);
  }
  else if (which == 4)
  {
    s.base = deck_base + 0x136C;
    s.count_ptr = (int *)(deck_base + 0x13E4);
  }
  else if (which == 3)
  {
    s.base = deck_base + 0x12F0;
    s.count_ptr = (int *)(deck_base + 0x1368);
  }
  else
  {
    if (which != 6)
      return;

    s.base = deck_base + 0x1100;
    s.count_ptr = (int *)(deck_base + 0x1178);
  }

  s.i = 0;
  s.done = 0;
  while (s.i < *s.count_ptr && !s.done)
  {
    if (((int *)s.base)[s.i * 3] == csvid)
    {
      for (s.idx = s.i; s.idx < *s.count_ptr - 1; s.idx++)
      {
        ((int *)s.base)[s.idx * 3] = ((int *)s.base)[s.idx * 3 + 3];
        ((int *)s.base)[s.idx * 3 + 1] = ((int *)s.base)[s.idx * 3 + 4];
        ((int *)s.base)[s.idx * 3 + 2] = ((int *)s.base)[s.idx * 3 + 5];
      }
      *s.count_ptr = *s.count_ptr + -1;

      num = num + -1;
      if (num < 1)
        s.done = 1;
    }
    s.i++;
  }
}

// FUNCTION: DECKDLL 0x100092c5
static int delete_card_from_global_deck(csvid_t csvid, int num)
{
  struct
  {
    int idx; // ebp - 0xc
    int i; // ebp - 0x8
    int found; // ebp - 0x4
  } s;

  s.i = 0;
  s.found = 0;
  for (;s.i < global_deck_num_entries && s.found == 0; s.i++)
  {
    if (global_deck[s.i].GDE_csvid == csvid && global_deck[s.i].GDE_Available == num)
    {
      s.idx = s.i;
      s.found = 1;
      if (num == 0)
        remove_cards_from_deck(csvid, 1, global_edited_deck);
    }
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
static void
move_colors_fromto_global_deck(int move_into, unsigned char dlgbits)
{
  int i;
  if (move_into == 1)
  {
    for (i = 0; i < global_deck_num_entries; ++i)
    {
      if (global_deck[i].GDE_Available == 1)
      {
        if ((((dlgbits & 1) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].color != CP_COLOR_BLACK) &&
             ((dlgbits & 2) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].color != CP_COLOR_BLUE) &&
             ((dlgbits & 4) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].color != CP_COLOR_GREEN) &&
             ((dlgbits & 8) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].color != CP_COLOR_RED) &&
             ((dlgbits & 0x10) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].color != CP_COLOR_WHITE) &&
             ((dlgbits & 0x20) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].card_type != CP_TYPE_ARTIFACT)))
        {
          if ((((dlgbits & 1) != 0 && global_deck[i].GDE_iid == 0) ||   // swamp
               ((dlgbits & 2) != 0 && global_deck[i].GDE_iid == 1) ||   // island
               ((dlgbits & 4) != 0 && global_deck[i].GDE_iid == 2) ||   // forest
               ((dlgbits & 8) != 0 && global_deck[i].GDE_iid == 3) ||   // mountain
               ((dlgbits & 0x10) != 0 && global_deck[i].GDE_iid == 4))) // plains
          {
            global_deck[i].GDE_Available = 0;
            global_deck[i].GDE_DecksBits |= 1 << global_current_deck;
            insert_cards_into_deck(global_deck[i].GDE_csvid, 1, global_edited_deck);
          }
        }
        else
        {
          global_deck[i].GDE_Available = 0;
          global_deck[i].GDE_DecksBits |= 1 << global_current_deck;
          insert_cards_into_deck(global_deck[i].GDE_csvid, 1, global_edited_deck);
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
        if ((((dlgbits & 1) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].color != CP_COLOR_BLACK) &&
             ((dlgbits & 2) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].color != CP_COLOR_BLUE) &&
             ((dlgbits & 4) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].color != CP_COLOR_GREEN) &&
             ((dlgbits & 8) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].color != CP_COLOR_RED) &&
             ((dlgbits & 0x10) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].color != CP_COLOR_WHITE) &&
             ((dlgbits & 0x20) == 0 || global_raw_cards_storage[global_deck[i].GDE_csvid].card_type != CP_TYPE_ARTIFACT)))
        {
          if ((((dlgbits & 1) != 0 && global_deck[i].GDE_iid == 0) ||   // swamp
               ((dlgbits & 2) != 0 && global_deck[i].GDE_iid == 1) ||   // island
               ((dlgbits & 4) != 0 && global_deck[i].GDE_iid == 2) ||   // forest
               ((dlgbits & 8) != 0 && global_deck[i].GDE_iid == 3) ||   // mountain
               ((dlgbits & 0x10) != 0 && global_deck[i].GDE_iid == 4))) // plains
          {
            global_deck[i].GDE_Available = 1;
            global_deck[i].GDE_DecksBits &= ~(1 << global_current_deck);
            remove_cards_from_deck(global_deck[i].GDE_csvid, 1, global_edited_deck);
          }
        }
        else
        {
          global_deck[i].GDE_Available = 1;
          global_deck[i].GDE_DecksBits &= ~(1 << global_current_deck);
          remove_cards_from_deck(global_deck[i].GDE_csvid, 1, global_edited_deck);
        }
      }
    }
  }
}

// FUNCTION: DECKDLL 0x1002cb92
static void
set_smallcard_dimensions(void)
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
static int
check_colors_inout_edited_deck(bool currently_in)
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
    if (global_deck[i].GDE_csvid == csvid && !(global_deck[i].GDE_DecksBits & (1 << global_current_deck)))
      ++count;

  return count;
}

static __inline void
draw_small_amount(int amount, RECT *rect)
{
  int old_dc = SaveDC(global_hdc);

  SetMapMode(global_hdc, MM_ANISOTROPIC);
  SetWindowExtEx(global_hdc, 200, 280, 0);
  SetViewportExtEx(global_hdc, rect->right - rect->left, rect->bottom - rect->top, NULL);
  SetViewportOrgEx(global_hdc, rect->left, rect->top, NULL);

  SetBkMode(global_hdc, TRANSPARENT);

  SelectObject(global_hdc, global_font_42);
  SetTextAlign(global_hdc, TA_RIGHT);

  SetTextColor(global_hdc, RGB(0, 0, 0));
  textoutf(global_hdc, 195, 3, "%d", amount);

  SetTextColor(global_hdc, PALETTERGB(0xFF, 0xD9, 0x27));
  textoutf(global_hdc, 192, 0, "%d", amount);

  RestoreDC(global_hdc, old_dc);
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

  for (s.i = 0;; ++s.i)
  {
    if (global_deck_num_entries <= s.i)
      break;
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

  // GLOBAL: DECKDLL 0x10030040
  static int DAT_10030040 = 10;
  // GLOBAL: DECKDLL 0x1003003c
  static int DAT_1003003c = 8;
  // GLOBAL: DECKDLL 0x10030044
  static int DAT_10030044 = 12;

  w = GetWindowWord(hwnd, DAT_10030040);
  lft = GetWindowWord(hwnd, DAT_1003003c);
  pics = GetWindowWord(hwnd, DAT_10030044);
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
  s.tmp = 0;
  s.i = 0;
  s.done = 0;
  while ((int)s.i < s.count && s.done == 0)
  {
    if (SendMessageA(hwnd_listbox, 0x199, s.i, 0) == (LRESULT)s.data)
    {
      s.tmp = s.i;
      s.done = 1;
    }
    s.i = s.i + 1;
  }

  SendMessageA(hwnd_listbox, 0x186, s.tmp, 0);
  SendMessageA(hwnd_horzlist, 0x186, s.tmp, 0);
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

      s.local_c = 0;
      while (1)
      {
        if (s.local_8 <= s.local_c)
          break;
        change_global_deck_card_availability(csvid, 1);
        ++s.local_c;
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
static void
refresh_numofcards_text(void)
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
    WPARAM wParam;       /* ebp - 0x30 */
    int amt;             /* ebp - 0x2c */
    int i;               /* ebp - 0x28 */
    int xoff;            /* ebp - 0x24 */
    RECT r;              /* ebp - 0x20 */
    HANDLE itemdata;     /* ebp - 0x10 */
    HWND card_hwnd;      /* ebp - 0xc */
    HWND target_hwnd;    /* ebp - 0x8 */
    int yoff;            /* ebp - 0x4 */
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
    for (;;)
    {
      if (s.target_hwnd == (HWND)0)
        break;
      if (s.target_hwnd == global_main_hwnd)
        break;
      if (s.target_hwnd == global_decksurface_hwnd)
        break;
      if (s.target_hwnd == global_unknown_10144b04)
        break;
      if (s.target_hwnd == global_unknown_10125a3c)
        break;
      if (s.target_hwnd == global_horzlist_hwnd)
        break;
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
    if (global_cfg_effects != 0)
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
    if ((global_db_flags_1 & 0x20) == 0)
    {
      SendMessageA(global_fullcard_hwnd, 0x400, (WPARAM)s.itemdata, 0);
      SendMessageA(global_horzlist_hwnd, 0x186, s.wParam, 0);
      SendMessageA(global_listbox_hwnd, 0x186, s.wParam, 0);
    }
    else
    {
      SendMessageA(global_horzlist_hwnd, 0x186, 0, 0);
      SendMessageA(global_listbox_hwnd, 0x186, 0, 0);
    }

    if ((global_db_flags_1 & 0x41) != 0)
       SendMessageA(global_horzlist_hwnd, 0x466, (WPARAM)s.itemdata, 0);
  }
  else if ((s.target_hwnd == global_unknown_10125a3c) || (s.target_hwnd == global_unknown_10144b04))
  {
    s.amt = ask_movexcards((int)s.itemdata, shifted);
    if (global_cfg_effects != 0)
      play_sound(2, 400, 0, 0);

    global_deck_was_edited = 1;
    ScreenToClient(s.target_hwnd, &global_main_mouse_pt);
    for (s.i = 0; s.i < s.amt; s.i = s.i + 1)
    {
      if (SendMessageA(s.target_hwnd, 0x4c8, (WPARAM)s.itemdata,
                       MAKELONG(global_main_mouse_pt.x, global_main_mouse_pt.y)) == 0)
        MessageBeep(0);
    }

    if ((global_db_flags_1 & 0x20) == 0)
    {
      SendMessageA(global_fullcard_hwnd, 0x400, (WPARAM)s.itemdata, 0);
      SendMessageA(global_horzlist_hwnd, 0x186, s.wParam, 0);
      SendMessageA(global_listbox_hwnd, 0x186, s.wParam, 0);
    }
    else
    {
      SendMessageA(global_horzlist_hwnd, 0x186, 0, 0);
      SendMessageA(global_listbox_hwnd, 0x186, 0, 0);
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
LRESULT CALLBACK
wndproc_HorzListClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  int i;
  static HDC comphdc = NULL;
  // GLOBAL: DECKDLL 0x1003a7cc
  static int sellprice = 0;
  // GLOBAL: DECKDLL 0x10105310
  static csvid_t curr_csvid = -1;

  // common vars
  char buf[300];
  HDC hdc;
  HDC hdc_local;
  RECT r;
  POINT p, p2;
  int last_pic_index, left_pic_index, pics_inline, currsel;
  int *horz_list_addr;

  switch (msg)
  {
  case WM_PAINT:
  {
    BITMAP bmp;
    HGDIOBJ h;
    int max_index;
    PAINTSTRUCT paint;

    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    horz_list_addr = GetWindowLong(hwnd, HORZLIST_ADDR_INDEX);
    left_pic_index = GetWindowLong(hwnd, HORZLIST_LEFTPIC_INDEX);
    pics_inline = GetWindowLong(hwnd, HORZLIST_PICSINLINE_INDEX);

    hdc = BeginPaint(hwnd, &paint);
    ApplyCardArtPaletteToDc(hdc);

    GetClientRect(hwnd, &r);
    GetObject(global_pic_dekbar1, sizeof(BITMAP), &bmp);

    h = SelectObject(comphdc, global_pic_dekbar1);
    StretchBlt(hdc, 0, 0, r.right, r.bottom, comphdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
    SelectObject(comphdc, h);

    max_index = MIN(left_pic_index + pics_inline, last_pic_index);

    for (i = left_pic_index; i < max_index; ++i)
    {
      RECT r2;
      csvid_t csvid;
      int amt;

      horzlist_prep_rectangle(hwnd, i, &r);
      SetRect(&r2, 0, 0, global_smallcard_piclist_width, global_smallcard_piclist_height);
      csvid = (csvid_t)horz_list_addr[i];
      amt = count_card_amount_outside_edited_deck(csvid);
      DrawSmallCard(global_hdc, &r2, &cards_ptr[csvid], 0, 1, 0, 0);
      if (amt > 1 && (global_db_flags_1 & (DBFLAGS_EDITDECK | DBFLAGS_SHANDALAR)))
        draw_small_amount(amt, &r2);

      r.left += (r.right - r.left - global_smallcard_piclist_width) / 2;
      r.top += (r.bottom - r.top - global_smallcard_piclist_height) / 2;
      BitBlt(hdc, r.left, r.top, global_smallcard_piclist_width, global_smallcard_piclist_height, global_hdc, 0, 0, SRCCOPY);
    }
    EndPaint(hwnd, &paint);
    return 0;
  }

  case WM_CREATE:
    pics_inline = 7;

    GetClientRect(hwnd, &r);
    if (global_smallcard_piclist_width > 0 && r.right)
    {
      pics_inline = (r.right - 4) / global_smallcard_piclist_width;
      if ((r.right - 4) % global_smallcard_piclist_width >= global_smallcard_piclist_width / 4)
        ++pics_inline;
    }

    SetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX, 0);
    SetWindowLong(hwnd, HORZLIST_ADDR_INDEX, malloc(sizeof(int)));
    SetWindowLong(hwnd, HORZLIST_CURRSEL_INDEX, -1);
    SetWindowLong(hwnd, HORZLIST_LEFTPIC_INDEX, 0);
    SetWindowLong(hwnd, HORZLIST_WIDTHPLUSSPACE_INDEX, global_smallcard_piclist_width);
    SetWindowLong(hwnd, HORZLIST_PICSINLINE_INDEX, pics_inline);

    update_scroll_range();

    SetScrollPos(hwnd, SB_HORZ, -1, TRUE);

    global_horzlist_popup = CreatePopupMenu();

    hdc_local = GetDC(hwnd);
    ApplyCardArtPaletteToDc(hdc_local);
    comphdc = CreateCompatibleDC(hdc_local);
    ApplyCardArtPaletteToDc(comphdc);
    ReleaseDC(hwnd, hdc_local);
    return 0;

  case WM_DESTROY:
    free(GetWindowLong(hwnd, HORZLIST_ADDR_INDEX));
    DESTROY_MENU(global_horzlist_popup);
    DELETE_DC(comphdc);
    return 0;

  case WM_MOUSEMOVE:
  {
    static POINT last_mousepos = {0, 0};
    int dx;
    int dy;

    p.x = GET_X_LPARAM(lparam);
    p.y = GET_Y_LPARAM(lparam);

    dx = abs(last_mousepos.x - p.x);
    dy = abs(last_mousepos.y - p.y);
    if (dx + dy < 2)
      return 0;

    last_mousepos = p;

    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    left_pic_index = GetWindowLong(hwnd, HORZLIST_LEFTPIC_INDEX);
    if (last_pic_index <= 0 || left_pic_index < 0)
      return 0;

    pics_inline = GetWindowLong(hwnd, HORZLIST_PICSINLINE_INDEX);

    for (i = left_pic_index; i < left_pic_index + pics_inline; ++i)
    {
      horzlist_prep_rectangle(hwnd, i, &r);
      if (PtInRect(&r, p))
      {
        if (SendMessage(hwnd, 0x8003, 0, 0) != i)
        {
          SendMessage(hwnd, 0x8007, i, 0);
          SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(hwnd), 1), hwnd);
        }
        break;
      }
    }

    return 0;
  }

  case WM_MOUSEWHEEL:
    SendMessage(hwnd, WM_HSCROLL, SHIWORD(wparam) < 0 ? 1 : 0, 0);
    return 0;

  case WM_LBUTTONDOWN:
  case WM_LBUTTONDBLCLK:
    if (!(global_db_flags_2 & DBFLAGS_SHANDALAR))
      return 0;

    GetCursorPos(&p);
    p2 = p;
    ScreenToClient(global_main_hwnd, &p);
    ScreenToClient(global_horzlist_hwnd, &p2);

    if (msg == WM_LBUTTONDOWN)
    {
      MSG m;
      Sleep(GetDoubleClickTime());
      if (PeekMessage(&m, hwnd, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, PM_NOREMOVE))
        return 0;
    }

    SetFocus(hwnd);
    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    if (last_pic_index <= 0)
      return 0;

    SendMessage(hwnd, WM_MOUSEMOVE, wparam & ~MK_LBUTTON, lparam);

    global_main_mouse_pt = p;
    global_horzlist_mouse_pt = p2;
    if (!handle_card_list_click_or_drag(hwnd, msg == WM_LBUTTONDOWN, wparam & MK_SHIFT))
      MessageBeep(0);

    return 0;

  case WM_RBUTTONDOWN:
    if (!(global_db_flags_2 & DBFLAGS_STANDALONE))
      return 0;

    SetFocus(hwnd);
    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    if (last_pic_index <= 0)
      return 0;

    SendMessage(hwnd, WM_MOUSEMOVE, wparam & ~MK_RBUTTON, lparam);

    p.x = GET_X_LPARAM(lparam);
    p.y = GET_Y_LPARAM(lparam);

    currsel = SendMessage(hwnd, 0x8003, 0, 0);
    horzlist_prep_rectangle(global_horzlist_hwnd, currsel, &r);
    if (!PtInRect(&r, p))
      return 0;

    ScreenToClient(global_main_hwnd, &p);

    {
      iid_t iid;
      curr_csvid = SendMessage(hwnd, 0x8004, currsel, 0);
      iid = get_iid_from_global_deck_card(curr_csvid);
      if (!(iid >= 0))
        return 0;

      sellprice = SellPrice(iid);
    }

    load_text("Menus", "SELLCARD");
    sprintf(buf, text_lines[0], sellprice);
    AppendMenu(global_horzlist_popup, MF_ENABLED, RES_SELLMENU_SELLCARDFORX, buf);

    ClientToScreen(hwnd, &p);
    TrackPopupMenu(global_horzlist_popup, TPM_RIGHTBUTTON, p.x + 10, p.y + 10, 0, hwnd, NULL);
    DeleteMenu(global_horzlist_popup, RES_SELLMENU_SELLCARDFORX, MF_BYCOMMAND);
    return 0;

  case WM_HSCROLL:
  {
    int scrollpos;
    int minpos;
    int maxpos;
    int tgt;

    pics_inline = GetWindowLong(hwnd, HORZLIST_PICSINLINE_INDEX);
    scrollpos = GetScrollPos(hwnd, 0);
    GetScrollRange(hwnd, SB_HORZ, &minpos, &maxpos);

    switch (LOWORD(wparam))
    {
    case SB_LINELEFT:
      tgt = scrollpos - 1;
      break;
    case SB_LINERIGHT:
      tgt = scrollpos + 1;
      break;
    case SB_PAGELEFT:
      tgt = scrollpos - pics_inline;
      break;
    case SB_PAGERIGHT:
      tgt = scrollpos + pics_inline;
      break;
    case SB_LEFT:
      tgt = minpos;
      break;
    case SB_RIGHT:
      tgt = maxpos;
      break;
    case SB_THUMBPOSITION:
      tgt = HIWORD(wparam);
      break;
    default:
      tgt = scrollpos;
      break;
    }

    tgt = MAX(minpos, tgt);
    tgt = MIN(maxpos, tgt);
    if (tgt != scrollpos)
    {
      left_pic_index = tgt;
      SetWindowLong(hwnd, HORZLIST_LEFTPIC_INDEX, tgt);
      UpdateWindow(hwnd);
      ScrollWindow(hwnd, GetWindowLong(hwnd, HORZLIST_WIDTHPLUSSPACE_INDEX) * (scrollpos - left_pic_index), 0, NULL, NULL);
      SetScrollPos(hwnd, SB_HORZ, left_pic_index, TRUE);
      InvalidateRect(hwnd, NULL, FALSE);
    }

    return 0;
  }

  case WM_KEYDOWN:
    switch (wparam)
    {
    case VK_UP:
    case VK_LEFT:
      SendMessage(hwnd, WM_HSCROLL, SB_LINELEFT, 0);
      break;
    case VK_DOWN:
    case VK_RIGHT:
      SendMessage(hwnd, WM_HSCROLL, SB_LINERIGHT, 0);
      break;
    case VK_PRIOR:
      SendMessage(hwnd, WM_HSCROLL, SB_PAGELEFT, 0);
      break;
    case VK_NEXT:
      SendMessage(hwnd, WM_HSCROLL, SB_PAGERIGHT, 0);
      break;
    case VK_HOME:
      SendMessage(hwnd, WM_HSCROLL, SB_LEFT, 0);
      break;
    case VK_END:
      SendMessage(hwnd, WM_HSCROLL, SB_RIGHT, 0);
      break;
    }
    return 0;

  case WM_CHAR:
    SendMessage(global_listbox_hwnd, WM_CHAR, wparam, lparam);
    currsel = SendMessage(global_listbox_hwnd, LB_GETCURSEL, 0, 0);
    SendMessage(hwnd, WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, currsel), 0);
    SendMessage(hwnd, 0x8007, currsel, 0);
    SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(hwnd), 1), hwnd);
    return 0;

  case WM_COMMAND:
  {
    int amt;
    int move_into_flag;

    switch (LOWORD(wparam))
    {
    case 1:
      amt = count_card_amount_outside_edited_deck(curr_csvid);

      global_dlg_parameter = 1;
      global_dlg_result = 1;
      if ((amt > 1 && show_dialog_sellxcards() == 0) || !global_dlg_result)
        return 0;

      global_dlg_result = MIN(global_dlg_result, amt);

      for (i = 0; i < global_dlg_result; ++i)
        if (!delete_card_from_global_deck(curr_csvid, 1))
          break;

      *Gold += global_dlg_result * sellprice;

      load_text("Menus", "PRICE");
      sprintf(buf, text_lines[0], *Gold);
      strncpy(global_deckname, buf, 12);

      InvalidateRect(global_title_hwnd, NULL, TRUE);
      TENTATIVE_remove_selected_from_horzlist(global_listbox_hwnd, global_horzlist_hwnd);
      InvalidateRect(global_horzlist_hwnd, NULL, FALSE);
      return 0;

    case 2:
    case 3:
      move_into_flag = LOWORD(wparam) == 2;

      global_dlg_parameter = check_colors_inout_edited_deck(move_into_flag);
      show_dialog_groupmove();
      move_colors_fromto_global_deck(move_into_flag, global_dlg_result);

      TENTATIVE_remove_selected_from_horzlist(global_listbox_hwnd, global_horzlist_hwnd);
      InvalidateRect(global_horzlist_hwnd, 0, TRUE);

      SendMessage(global_decksurface_hwnd, 0x8401, 0, 0);
      InvalidateRect(global_decksurface_hwnd, NULL, TRUE);
      return 0;

    default:
      return 0;
    }
  }

  case 0x8001:
    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    horz_list_addr = GetWindowLong(hwnd, HORZLIST_ADDR_INDEX);
    horz_list_addr = realloc(horz_list_addr, 4 * last_pic_index + 4);
    if (!horz_list_addr)
      return 0;

    horz_list_addr[last_pic_index++] = lparam;
    SetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX, last_pic_index);
    SetWindowLong(hwnd, HORZLIST_ADDR_INDEX, horz_list_addr);
    update_scroll_range();
    InvalidateRect(hwnd, NULL, TRUE);
    return last_pic_index;

  case 0x8002:
    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    if (wparam >= last_pic_index)
      return -1;

    horz_list_addr = GetWindowLong(hwnd, HORZLIST_ADDR_INDEX);
    currsel = GetWindowLong(hwnd, HORZLIST_CURRSEL_INDEX);
    left_pic_index = GetWindowLong(hwnd, HORZLIST_LEFTPIC_INDEX);

    {
      DELETEITEMSTRUCT del;
      del.CtlType = ODT_LISTBOX;
      del.itemID = wparam;
      del.hwndItem = hwnd;
      del.itemData = horz_list_addr[wparam];
      del.CtlID = GetDlgCtrlID(hwnd);

      SendMessage(GetParent(hwnd), WM_DELETEITEM, del.CtlID, &del);
    }

    for (i = wparam; i < last_pic_index - 1; ++i)
      horz_list_addr[i] = horz_list_addr[i + 1];

    SetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX, --last_pic_index);

    if (last_pic_index - 1 < currsel)
      SetWindowLong(hwnd, HORZLIST_CURRSEL_INDEX, --currsel);

    if (last_pic_index - 1 < left_pic_index)
      SetWindowLong(hwnd, HORZLIST_LEFTPIC_INDEX, --left_pic_index);

    update_scroll_range();
    InvalidateRect(hwnd, NULL, TRUE);

    if (!last_pic_index)
    {
      SetWindowLong(hwnd, HORZLIST_CURRSEL_INDEX, -1);
      SetWindowLong(hwnd, HORZLIST_LEFTPIC_INDEX, 0);
    }

    return last_pic_index;

  case 0x8003:
    return GetWindowLong(hwnd, HORZLIST_CURRSEL_INDEX);

  case 0x8004:
    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    if (wparam < 0 || wparam >= last_pic_index)
      return -1;

    horz_list_addr = GetWindowLong(hwnd, HORZLIST_ADDR_INDEX);
    return horz_list_addr[wparam];

  case 0x8005:
  {
    int insert_index;
    int idx;

    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    insert_index = wparam;
    if (insert_index >= last_pic_index)
      return SendMessage(hwnd, 0x8001, 0, lparam);

    horz_list_addr = GetWindowLong(hwnd, HORZLIST_ADDR_INDEX);
    horz_list_addr = realloc(horz_list_addr, 4 * last_pic_index + 4);

    if (!horz_list_addr)
      return -2;

    for (idx = last_pic_index; idx > 0 && idx != insert_index; --idx)
      horz_list_addr[idx] = horz_list_addr[idx - 1];

    horz_list_addr[idx] = lparam;
    SetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX, last_pic_index + 1);
    SetWindowLong(hwnd, HORZLIST_ADDR_INDEX, horz_list_addr);
    update_scroll_range();
    InvalidateRect(hwnd, NULL, TRUE);
    return insert_index;
  }

  case 0x8006:
    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    horz_list_addr = GetWindowLong(hwnd, HORZLIST_ADDR_INDEX);

    for (i = 0; i < last_pic_index; ++i)
    {
      DELETEITEMSTRUCT del;
      del.CtlType = ODT_LISTBOX;
      del.itemID = i;
      del.hwndItem = hwnd;
      del.itemData = horz_list_addr[i];
      del.CtlID = GetDlgCtrlID(hwnd);

      SendMessage(GetParent(hwnd), WM_DELETEITEM, del.CtlID, &del);
    }

    horz_list_addr = realloc(horz_list_addr, 4);
    SetWindowLong(hwnd, HORZLIST_ADDR_INDEX, horz_list_addr);
    SetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX, 0);
    SetWindowLong(hwnd, HORZLIST_CURRSEL_INDEX, -1);
    SetWindowLong(hwnd, HORZLIST_LEFTPIC_INDEX, 0);
    SetScrollRange(hwnd, SB_HORZ, 0, -1, FALSE);
    SetScrollPos(hwnd, SB_HORZ, 0, TRUE);
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;

  case 0x8007:
    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    currsel = wparam;
    if (currsel >= last_pic_index)
      return -1;

    pics_inline = GetWindowLong(hwnd, HORZLIST_PICSINLINE_INDEX);
    left_pic_index = GetWindowLong(hwnd, HORZLIST_LEFTPIC_INDEX);
    SetWindowLong(hwnd, HORZLIST_CURRSEL_INDEX, currsel);

    if (currsel < left_pic_index)
      SendMessage(hwnd, WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, currsel), 0);
    else if (currsel >= left_pic_index + pics_inline)
      SendMessage(hwnd, WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, currsel - pics_inline + 1), 0);

    return 0;

  case 0x8008:
  {
    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    pics_inline = GetWindowLong(hwnd, HORZLIST_PICSINLINE_INDEX);
    SetScrollRange(hwnd, SB_HORZ, 0, MAX(0, last_pic_index - pics_inline), TRUE);
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;
  }

  case 0x8464:
    SendMessage(global_main_hwnd, 0x8466, wparam, hwnd);
    return 0;

  case 0x8466:
  {
    int max_idx;
    int wanted_card;

    last_pic_index = GetWindowLong(hwnd, HORZLIST_LASTPIC_INDEX);
    horz_list_addr = GetWindowLong(hwnd, HORZLIST_ADDR_INDEX);
    left_pic_index = GetWindowLong(hwnd, HORZLIST_LEFTPIC_INDEX);
    pics_inline = GetWindowLong(hwnd, HORZLIST_PICSINLINE_INDEX);

    max_idx = MIN(last_pic_index, left_pic_index + pics_inline);
    wanted_card = wparam;

    for (i = left_pic_index; i < max_idx; ++i)
      if (horz_list_addr[i] == wanted_card)
      {
        horzlist_prep_rectangle(hwnd, i, &r);
        InvalidateRect(hwnd, &r, FALSE);
      }

    return 0;
  }

  default:
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: DECKDLL 0x100057bc
static void
reset_dim_and_pos_all_cards(HWND hwnd, RECT *rect)
{
  struct
  {
    HWND last; /* ebp - 0x18 */
    int offset_between_cards_y; /* ebp - 0x14 */
    HWND w; /* ebp - 0x10 */
    int offset_from_sides; /* ebp - 0xc */
    int y; /* ebp - 8 */
    int x; /* ebp - 4 */
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
static int ask_about_saving_deck(void)
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
        global_deck_revision = 1;
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
        sprintf(global_deckname, text_lines[0]);

        global_deck_comments[0] = 0;
        global_deck_description[0] = 0;

        strcpy(global_deck_author, global_cfg_player_name);
        strcpy(global_deck_email, global_cfg_email);

        GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, NULL, "dd/MM/yyyy", global_deck_creation_date, 22);

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
        insert_cards_into_deck(csvid, 1, global_edited_deck);
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
          insert_cards_into_deck(csvid, 1, global_edited_deck);
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

// FUNCTION: DECKDLL 0x10008abf
static void
TENTATIVE_move_from_owned_cards(HWND hwnd, int x, int y, int shift2_unshift1, int singleclick)
{
  struct
  {
    unsigned int amt32; /* ebp - 0x38 */
    POINT pt;          /* ebp - 0x34 */
    int ctrl_id;       /* ebp - 0x2c */
    int i;             /* ebp - 0x28 */
    HWND new_hwnd;     /* ebp - 0x24 */
    unsigned int num;  /* ebp - 0x20 (low word) */
    HWND parent;       /* ebp - 0x1c */
    RECT r;            /* ebp - 0x18 */
    csvid_t csvid;     /* ebp - 0x8 */
    HWND target;       /* ebp - 0x4 */
  } s;

  s.csvid = (csvid_t)GetWindowLongA(hwnd, 0);
  *(WORD *)&s.amt32 = GetWindowWord(hwnd, 4);
  s.parent = GetParent(hwnd);
  s.ctrl_id = GetDlgCtrlID(hwnd);
  GetWindowRect(hwnd, &s.r);

  if ((s.amt32 & 0xFFFF) >= 2)
  {
    s.pt.x = s.r.left;
    s.pt.y = s.r.top;
    SendMessageA(hwnd, 0x401, 1, 0);
    MapWindowPoints(NULL, s.parent, &s.pt, 1);

    s.new_hwnd = CreateWindowExA(0, "MAGICDECK_CardClass", "", 0x54000000, s.pt.x, s.pt.y, global_smallcard_width,
                                 global_smallcard_height, s.parent, (HMENU)1, global_hinstance, (LPVOID)s.csvid);
    SendMessageA(s.new_hwnd, 0x401, (s.amt32 & 0xFFFF) - 1, 0);
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
    for (s.target = WindowFromPoint(s.pt);
         s.target != NULL && s.target != global_main_hwnd && s.target != global_decksurface_hwnd &&
         s.target != global_unknown_10144b04 && s.target != global_unknown_10125a3c && s.target != global_horzlist_hwnd;
         s.target = GetParent(s.target))
    {
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
    if (shift2_unshift1 < 2)
    {
      SendMessageA(s.new_hwnd, 0x401, (s.amt32 & 0xFFFF) - 1, 0);
      *(WORD *)&s.num = 1;
    }
    else
    {
      global_dlg_parameter = (unsigned int)(s.amt32 & 0xFFFF);
      if (!show_dialog_movexcards())
        global_dlg_result = 0;

      if (global_dlg_result < 1)
      {
        SendMessageA(s.new_hwnd, 0x401, (unsigned int)(s.amt32 & 0xFFFF), 0);
        s.num &= 0xFFFF0000;
      }
      else if (global_dlg_result < (int)(unsigned int)(s.amt32 & 0xFFFF))
      {
        SendMessageA(s.new_hwnd, 0x401, (unsigned int)(s.amt32 & 0xFFFF) - global_dlg_result, 0);
        *(WORD *)&s.num = (WORD)global_dlg_result;
      }
      else
      {
        DestroyWindow(s.new_hwnd);
        *(WORD *)&s.num = (WORD)(s.amt32 & 0xFFFF);
      }
    }

    for (s.i = 0; s.i < (int)(s.num & 0xFFFF); s.i++)
      change_global_deck_card_availability(s.csvid, 0);

    remove_cards_from_deck(s.csvid, s.num & 0xFFFF, global_edited_deck);
    count_packs();
    refresh_numofcards_text();
  }
  else if (global_unknown_10144b04 == s.parent)
  {
    FUN_1002bed0(s.ctrl_id, (int)s.csvid, 1, (int)global_edited_deck);
  }
  else
  {
    FUN_1002c161(s.ctrl_id, (int)s.csvid, 1, (int)global_edited_deck);
  }

  if (s.target == global_decksurface_hwnd)
  {
    if (s.target != s.parent)
      global_deck_was_edited = 1;

    ScreenToClient(s.target, &s.pt);
    s.pt.x = s.pt.x - x;
    s.pt.y = s.pt.y - y;

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
      s.num &= 0xFFFF0000;

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
      if (global_cfg_effects != 0)
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
    s.pt.x = s.pt.x - x;
    s.pt.y = s.pt.y - y;

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
  int i;
  RECT r;
  csvid_t csvid;
  int amt;
  static int sellprice = 0;

  switch (msg)
  {
  case WM_PAINT:
  {
    PAINTSTRUCT paint;
    HDC paintdc;

    amt = GetWindowLong(hwnd, CARD_AMOUNT_INDEX);
    csvid = GetWindowLong(hwnd, CARD_CSVID_INDEX);
    paintdc = BeginPaint(hwnd, &paint);
    GetClientRect(hwnd, &r);
    ApplyCardArtPaletteToDc(paintdc);
    DrawSmallCard(global_hdc, &r, &cards_ptr[csvid], 0, 1, -1, -1);
    if (amt > 1)
      draw_small_amount(amt, &r);
    BitBlt(paintdc, 0, 0, r.right, r.bottom, global_hdc, 0, 0, SRCCOPY);
    EndPaint(hwnd, &paint);
    return 0;
  }

  case WM_CREATE:
    csvid = *(int *)(lparam);
    SetWindowLong(hwnd, CARD_CSVID_INDEX, csvid);
    SetWindowLong(hwnd, CARD_AMOUNT_INDEX, 1);
    return (!(csvid >= 0) || csvid > global_available_slots) ? -1 : 0;

  case WM_DESTROY:
    return 0;

  case WM_SIZE:
    GetWindowRect(hwnd, &r);
    global_smallcard_width = RECT_WIDTH(r);
    global_smallcard_height = RECT_HEIGHT(r);
    return 0;

  case WM_MOUSEMOVE:
    csvid = GetWindowLong(hwnd, CARD_CSVID_INDEX);
    SendMessage(global_fullcard_hwnd, 0x8400, csvid, 0);
    return 0;

  case WM_COMMAND:
  {
    char buf[500];

    if (LOWORD(wparam) != RES_SELLMENU_SELLCARDFORX)
      return 0;

    csvid = GetWindowLong(hwnd, CARD_CSVID_INDEX);
    amt = GetWindowLong(hwnd, CARD_AMOUNT_INDEX);

    global_dlg_result = 1;
    global_dlg_parameter = 1;

    if ((amt > 1 && !show_dialog_sellxcards()) || global_dlg_result == 0)
      return 0;

    if (global_dlg_result > amt)
      global_dlg_result = amt;

    for (i = 0; i < global_dlg_result; ++i)
      if (!delete_card_from_global_deck(csvid, 0))
        break;

    *Gold += global_dlg_result * sellprice;

    count_packs();

    load_text("Menus", "PRICE");
    sprintf(buf, text_lines[0], *Gold);
    strncpy(global_deckname, buf, 12); // That's peculiar.
    InvalidateRect(global_title_hwnd, NULL, TRUE);

    if (global_dlg_result >= amt)
      DestroyWindow(hwnd);
    else
    {
      SetWindowLong(hwnd, CARD_AMOUNT_INDEX, amt - global_dlg_result);
      InvalidateRect(hwnd, NULL, FALSE);
    }

    GetClientRect(global_decksurface_hwnd, &r);
    LockWindowUpdate(global_main_hwnd);
    reset_dim_and_pos_all_cards(global_decksurface_hwnd, &r);
    LockWindowUpdate(NULL);
    refresh_numofcards_text();
    return 0;
  }

  case WM_LBUTTONDOWN:
  {
    MSG message;

    if (!(global_db_flags_2 & DBFLAGS_SHANDALAR))
      return 0;

    Sleep(GetDoubleClickTime());

    if (PeekMessage(&message, hwnd, WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK, PM_NOREMOVE))
      return 0;

    TENTATIVE_move_from_owned_cards(hwnd, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), (wparam & MK_SHIFT) ? 2 : 1, 1);

    SendMessage(global_decksurface_hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    return 0;
  }

  case WM_LBUTTONDBLCLK:
  {
    if (!(global_db_flags_2 & DBFLAGS_SHANDALAR))
      return 0;

    TENTATIVE_move_from_owned_cards(hwnd, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), (wparam & MK_SHIFT) ? 2 : 1, 0);

    SendMessage(global_decksurface_hwnd, WM_COMMAND, RES_MENU_REFRESH, 0);
    return 0;
  }

  case WM_RBUTTONDOWN:
  {
    POINT p;
    p.x = GET_X_LPARAM(lparam);
    p.y = GET_Y_LPARAM(lparam);

    if (global_db_flags_1 & DBFLAGS_STANDALONE)
    {
      HWND parent = GetParent(hwnd);
      ClientToScreen(hwnd, &p);
      ScreenToClient(parent, &p);
      SendMessage(parent, WM_RBUTTONDOWN, wparam, MAKELONG(p.x, p.y));
    }
    else if (global_db_flags_2 & DBFLAGS_STANDALONE)
    {
      iid_t iid;
      csvid = GetWindowLong(hwnd, CARD_CSVID_INDEX);
      iid = get_iid_from_global_deck_card(csvid);
      if (iid >= 0)
      {
        char itemtxt[500];
        sellprice = SellPrice(iid);
        load_text("Menus", "SELLCARD");
        sprintf(itemtxt, text_lines[0], sellprice);
        AppendMenu(global_smallcard_popup, MF_ENABLED, RES_SELLMENU_SELLCARDFORX, itemtxt);
        ClientToScreen(hwnd, &p);
        TrackPopupMenu(global_smallcard_popup, TPM_RIGHTBUTTON, p.x + 10, p.y + 10, 0, hwnd, NULL);
        DeleteMenu(global_smallcard_popup, 0, MF_BYPOSITION);
      }
    }
    return 0;
  }

  case 0x8400:
    return GetWindowLong(hwnd, CARD_CSVID_INDEX);

  case 0x8401:
    SetWindowLong(hwnd, CARD_AMOUNT_INDEX, LOWORD(wparam));
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;

  case 0x8402:
    return GetWindowLong(hwnd, CARD_AMOUNT_INDEX);

  default:
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: DECKDLL 0x100256c8
static int is_buttonclass(HWND hwnd)
{
  char classname[0x64];

  if (hwnd == (HWND)0)
    return 0;

  GetClassNameA(hwnd, classname, 0x64);
  if (!_strcmpi(classname, s_Button_1003628c))
    return 1;
  else
    return 0;
}

// FUNCTION: DECKDLL 0x10025593
LRESULT CALLBACK
wndproc_ButtonClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  if (msg == WM_KILLFOCUS)
    lparam = hwnd;
  else if (msg == WM_SETFOCUS)
  {
    wparam = hwnd;
    lparam = wparam;
  }
  else
    return CallWindowProc(global_wndproc_std_ButtonClass, hwnd, msg, wparam, lparam);

  if (!is_buttonclass(wparam))
    wparam = 0;
  if (!is_buttonclass(lparam))
    lparam = 0;

  SendMessage(GetParent(hwnd), 0x84c8, wparam, lparam);

  return 0;
}

// FUNCTION: DECKDLL 0x10025552
BOOL CALLBACK
enumfunc_change_buttonclass_wndproc(HWND hwnd, LPARAM lparam)
{
  if (is_buttonclass(hwnd))
    global_wndproc_std_ButtonClass = SetWindowLong(hwnd, GWL_WNDPROC, wndproc_ButtonClass);
  return TRUE;
}

// FUNCTION: DECKDLL 0x10025536
void change_buttonclass_wndproc(HWND hwnd)
{
  EnumChildWindows(hwnd, enumfunc_change_buttonclass_wndproc, 0);
}

// FUNCTION: DECKDLL 0x10019d9f
static void copy_deck_to_edit(void)
{
  struct
  {
    int i;      /* ebp - 0x8 */
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
      insert_cards_into_deck(global_deck[s.i].GDE_csvid, 1, global_edited_deck);
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
    FILE *f; /* ebp - 0xc */
    int i; /* ebp - 0x8 */
    int pad; /* ebp - 0x4 */
  } s;

  s.pad = 0;
  s.f = fopen(filename, "wt");
  if (s.f != 0)
  {
    fprintf(s.f, ";%s\n", global_deckname);
    fprintf(s.f, ";%s\n", global_deckname + 0x1f);
    fprintf(s.f, ";%s\n", global_deckname + 0x34);
    fprintf(s.f, ";%s\n", global_deckname + 0x85);
    fprintf(s.f, ";%s\n", global_deckname + 0xd6);
    fprintf(s.f, ";%d\n", global_deck_revision);
    fprintf(s.f, ";%s\n", global_deckname + 0xf0);
    fprintf(s.f, ";%s\n", global_deckname + 0x100);
    fprintf(s.f, "\n");

    for (s.i = 0; s.i < global_edited_deck_num_entries; ++s.i)
      fprintf(s.f, ".%d\t%d\t%s\n",
              global_edited_deck[s.i].DeckEntry_csvid,
              global_edited_deck[s.i].DeckEntry_Amount,
              global_edited_deck[s.i].DeckEntry_FullName);

    fclose(s.f);
    return true;
  }

  return false;
}

// FUNCTION: DECKDLL 0x1000e90f
static int
load_deck(char *filename)
{
  struct
  {
    size_t lens[8];
    int pad_before_f;
    FILE *f;
    char txt[500];
    int tail_pad;
    int num;
    csvid_t csvid;
  } s;

  s.pad_before_f = 0;
  s.f = fopen(filename, "rt");
  if (s.f == 0)
    return 0;

  memset(global_edited_deck, 0, 0x1464);

  if (global_db_flags_1 & DBFLAGS_GAUNTLET)
  {
    global_deck_num_entries_copy = 0;
    global_deck_num_entries = global_deck_num_entries_copy;
  }

  fgets(s.txt, 0x1f, s.f);
  if (s.txt[0] != ';')
    return 0;
  s.lens[0] = strlen(s.txt);
  s.txt[s.lens[0] - 1] = 0;
  strncpy(global_deckname, &s.txt[1], 0x1f);

  fgets(s.txt, 0x15, s.f);
  if (s.txt[0] != ';')
    return 0;
  s.lens[1] = strlen(s.txt);
  s.txt[s.lens[1] - 1] = 0;
  strncpy(global_deckname + 0x1f, &s.txt[1], 0x15);

  fgets(s.txt, 0x51, s.f);
  if (s.txt[0] != ';')
    return 0;
  s.lens[2] = strlen(s.txt);
  s.txt[s.lens[2] - 1] = 0;
  strncpy(global_deckname + 0x34, &s.txt[1], 0x51);

  fgets(s.txt, 0x51, s.f);
  if (s.txt[0] != ';')
    return 0;
  s.lens[3] = strlen(s.txt);
  s.txt[s.lens[3] - 1] = 0;
  strncpy(global_deckname + 0x85, &s.txt[1], 0x51);

  fgets(s.txt, 0x16, s.f);
  if (s.txt[0] != ';')
    return 0;
  s.lens[4] = strlen(s.txt);
  s.txt[s.lens[4] - 1] = 0;
  strncpy(global_deckname + 0xd6, &s.txt[1], 0x16);

  fgets(s.txt, 0x10, s.f);
  s.lens[5] = strlen(s.txt);
  s.txt[s.lens[5] - 1] = 0;
  if (s.txt[0] != ';')
    return 0;
  global_deck_revision = atoi(&s.txt[1]);

  fgets(s.txt, 0x10, s.f);
  if (s.txt[0] != ';')
    return 0;
  s.lens[6] = strlen(s.txt);
  s.txt[s.lens[6] - 1] = 0;
  strncpy(global_deckname + 0xf0, &s.txt[1], 0x10);

  fgets(s.txt, 0x191, s.f);
  if (s.txt[0] != ';')
    return 0;
  s.lens[7] = strlen(s.txt);
  s.txt[s.lens[7] - 1] = 0;
  strncpy(global_deckname + 0x100, &s.txt[1], 0x191);

  while (1)
  {
    if (((int (*)(FILE *, char *, int))readline)(s.f, s.txt, 0x200) == -1)
      break;
    if (s.txt[0] == 'v')
      break;

    if (s.txt[0] == '.')
    {
      if (s.txt[1] == 'v')
        break;
      sscanf(s.txt, ".%d %d", &s.csvid, &s.num);
      if (s.num == 0)
        insert_cards_into_deck(s.csvid, 0, global_edited_deck);
      else
        insert_cards_into_deck(s.csvid, s.num, global_edited_deck);
    }
  }

  fclose(s.f);

  count_packs();
  return SendMessage(global_decksurface_hwnd, 0x401, 0, 0);
}

// FUNCTION: DECKDLL 0x1002b7bb
static int
check_card_count(int csvid)
{
  int limit;
  int i;
  limit = Scards[5].worldmagic_city == 0;

  if (global_deck_num_cards < 59)
  {
    for (i = 0; i < global_edited_deck_num_entries; ++i)
      if (global_edited_deck[i].DeckEntry_csvid == csvid && CardTypeFromID(csvid) > 4)
        return global_edited_deck[i].DeckEntry_Amount - (limit + 3);
  }
  else
  {
    for (i = 0; i < global_edited_deck_num_entries; ++i)
      if (global_edited_deck[i].DeckEntry_csvid == csvid && CardTypeFromID(csvid) > 4)
        return global_edited_deck[i].DeckEntry_Amount - (limit * 99 + 4);
  }

  return 0;
}

static __inline void
draw_lines(HDC hdc, RECT *r, HGDIOBJ pen1, HPEN pen2, HPEN pen3)
{
  // top and left border
  SelectObject(hdc, pen1);
  MoveToEx(hdc, 0, 0, NULL);
  LineTo(hdc, r->right, 0);
  MoveToEx(hdc, 0, 0, NULL);
  LineTo(hdc, 0, r->bottom);

  // top and left border, moved inward by 1 px
  SelectObject(hdc, pen2);
  MoveToEx(hdc, 1, 1, NULL);
  LineTo(hdc, r->right - 1, 1);
  MoveToEx(hdc, 1, 1, NULL);
  LineTo(hdc, 1, r->bottom - 1);

  // bottom and right border
  SelectObject(hdc, pen3);
  MoveToEx(hdc, r->right - 1, 1, NULL);
  LineTo(hdc, r->right - 1, r->bottom);
  MoveToEx(hdc, 1, r->bottom - 1, NULL);
  LineTo(hdc, r->right, r->bottom - 1);
}

// FUNCTION: DECKDLL 0x10023972
BOOL TileBitmapIntoRect(HDC hdc, RECT *r, HBITMAP bmp)
{
  struct
  {
    BITMAP bm;
    RECT dst;
    int y;
    int saved;
    int x;
  } s;

  if (!hdc || !r || !bmp)
    return FALSE;

  s.saved = SaveDC(hdc);
  IntersectClipRect(hdc, r->left, r->top, r->right, r->bottom);

  GetObjectA(bmp, 0x18, &s.bm);

  for (s.x = r->left; s.x < r->right; s.x += s.bm.bmWidth)
  {
    for (s.y = r->top; s.y < r->bottom; s.y += s.bm.bmHeight)
    {
      SetRect(&s.dst, s.x, s.y, s.x - 1, s.y - 1);
      DrawBitmapSubrectToRect(hdc, &s.dst, bmp, 0, 0, s.bm.bmWidth, s.bm.bmHeight);
    }
  }

  RestoreDC(hdc, s.saved);
  return TRUE;
}

// FUNCTION: DECKDLL 0x10024ed5
static void
draw_item(DRAWITEMSTRUCT *item, HBRUSH brush, HANDLE hbmp_bkgrd, HPEN pen1, HPEN pen2, COLORREF col, int do_focus, UINT format)
{
  struct
  {
    HDC hdc;       /* ebp - 0x64 */
    RECT focus;    /* ebp - 0x60 */
    HGDIOBJ hfont; /* ebp - 0x50 */
    RECT r;        /* ebp - 0x4c */
    char txt[52];  /* ebp - 0x3c */
    SIZE sz;       /* ebp - 0xc */
  } s;

  s.hdc = item->hDC;
  CopyRect(&s.r, &item->rcItem);
  GetWindowText(item->hwndItem, s.txt, 0x32);
  ApplyCardArtPaletteToDc(s.hdc);

  OffsetRect(&s.r, -item->rcItem.left, -item->rcItem.top);

  if (item->itemState & ODS_SELECTED)
  {
    if (brush)
      FillRect(s.hdc, &s.r, brush);
    else if (hbmp_bkgrd)
      TileBitmapIntoRect(s.hdc, &s.r, hbmp_bkgrd);

    SelectObject(s.hdc, GetStockObject(BLACK_PEN));
    MoveToEx(s.hdc, 0, 0, NULL);
    LineTo(s.hdc, s.r.right, 0);
    MoveToEx(s.hdc, 0, 0, NULL);
    LineTo(s.hdc, 0, s.r.bottom);

    SelectObject(s.hdc, pen2);
    MoveToEx(s.hdc, 1, 1, NULL);
    LineTo(s.hdc, s.r.right - 1, 1);
    MoveToEx(s.hdc, 1, 1, NULL);
    LineTo(s.hdc, 1, s.r.bottom - 1);

    SelectObject(s.hdc, pen1);
    MoveToEx(s.hdc, s.r.right - 1, 1, NULL);
    LineTo(s.hdc, s.r.right - 1, s.r.bottom);
    MoveToEx(s.hdc, 1, s.r.bottom - 1, NULL);
    LineTo(s.hdc, s.r.right, s.r.bottom - 1);

    OffsetRect(&s.r, 2, 2);
  }
  else
  {
    if (brush)
      FillRect(s.hdc, &s.r, brush);
    else if (hbmp_bkgrd)
      TileBitmapIntoRect(s.hdc, &s.r, hbmp_bkgrd);

    SelectObject(s.hdc, pen1);
    MoveToEx(s.hdc, 0, 0, NULL);
    LineTo(s.hdc, s.r.right, 0);
    MoveToEx(s.hdc, 0, 0, NULL);
    LineTo(s.hdc, 0, s.r.bottom);
    MoveToEx(s.hdc, 1, 1, NULL);
    LineTo(s.hdc, s.r.right - 1, 1);
    MoveToEx(s.hdc, 1, 1, NULL);
    LineTo(s.hdc, 1, s.r.bottom - 1);

    SelectObject(s.hdc, pen2);
    MoveToEx(s.hdc, s.r.right - 1, 1, NULL);
    LineTo(s.hdc, s.r.right - 1, s.r.bottom);
    MoveToEx(s.hdc, 1, s.r.bottom - 1, NULL);
    LineTo(s.hdc, s.r.right, s.r.bottom - 1);
    MoveToEx(s.hdc, s.r.right - 2, 2, NULL);
    LineTo(s.hdc, s.r.right - 2, s.r.bottom - 1);
    MoveToEx(s.hdc, 2, s.r.bottom - 2, NULL);
    LineTo(s.hdc, s.r.right - 1, s.r.bottom - 2);
  }

  SetBkMode(s.hdc, TRANSPARENT);
  SetTextColor(s.hdc, col);
  s.hfont = (HGDIOBJ)SendMessageA(item->hwndItem, WM_GETFONT, 0, 0);
  SelectObject(s.hdc, s.hfont);
  DrawTextA(s.hdc, s.txt, -1, &s.r, format);

  if (do_focus && (item->itemState & ODS_FOCUS))
  {
    GetTextExtentPoint32A(s.hdc, s.txt, strlen(s.txt), &s.sz);

    s.focus.left = (s.r.right - s.r.left) / 2 - s.sz.cx / 2 - 3;
    s.focus.top = (s.r.bottom - s.r.top) / 2 - s.sz.cy / 2 - 3;
    s.focus.right = s.sz.cx + s.focus.left + 6;
    s.focus.bottom = s.sz.cy + s.focus.top + 6;
    DrawFocusRect(s.hdc, &s.focus);
  }
}

// FUNCTION: DECKDLL 0x10016e4e
LRESULT CALLBACK
wndproc_MainClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  int i;
  static HPEN pen_ltgrey = NULL;
  static HPEN pen_medgrey = NULL;

  // GLOBAL: DECKDLL 0x101a8c24
  static HWND button_deckinfo = NULL;
  // GLOBAL: DECKDLL 0x1019e9d0
  static HWND button_exit = NULL;
  // GLOBAL: DECKDLL 0x1014263c
  static HWND button_deck1 = NULL;
  // GLOBAL: DECKDLL 0x1014250c
  static HWND button_deck2 = NULL;
  // GLOBAL: DECKDLL 0x101423cc
  static HWND button_deck3 = NULL;
  // GLOBAL: DECKDLL 0x1012e2d8
  static HWND cardlistfilter_hwnd = NULL;
  static HWND search_hwnd = NULL;
  // GLOBAL: DECKDLL 0x10104b8c
  static HMENU popup = NULL;

  HDC hdc;

  switch (msg)
  {
  case WM_CREATE:
    pen_ltgrey = CreatePen(PS_SOLID, 0, PALETTEINDEX(193)); // 203, 203, 203
    pen_medgrey = CreatePen(PS_SOLID, 0, PALETTEINDEX(22)); // 79, 78, 78
    // memcpy(&unk_4150C8, &global_logfont_template, 0x3Cu);	unused

    if (!(global_db_flags_1 & DBFLAGS_SHANDALAR))
      init_sound_dll(hwnd, 0, 0);

    init_sounds_and_music();

    sprintf(global_deck_filename, "%sNew.dck", global_playdeck_path);

    if (global_db_flags_1 & DBFLAGS_SHANDALAR)
    {
      char buf[160];
      load_text("Menus", "GOLDTITLE");
      sprintf(buf, text_lines[0], *Gold);
      strncpy(global_deckname, buf, 12);
    }
    else if (global_db_flags_1 & DBFLAGS_EDITDECK)
      strcpy(global_deckname, " ");
    else
    {
      load_text("Menus", "NEWDECK");
      sprintf(global_deckname, text_lines[0]);
    }

    global_deck_comments[0] = 0;
    global_deck_description[0] = 0;
    global_deck_revision = 1;
    strcpy(global_deck_author, global_cfg_player_name);
    strcpy(global_deck_email, global_cfg_email);

    GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, NULL, "dd/MM/yyyy", global_deck_creation_date, 22);

    if (global_db_flags_1 & (DBFLAGS_EDITDECK | DBFLAGS_GAUNTLET | DBFLAGS_SHANDALAR))
      copy_deck_to_edit();

    global_title_hwnd = CreateWindowEx(0, "TitleClass", "",
                                       WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                       0, 0, 0, 0,
                                       hwnd,
                                       1,
                                       global_hinstance, 0);

    global_fullcard_hwnd = CreateWindowEx(0, "FullCardClass", "",
                                          WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                          0, 0, 0, 0,
                                          hwnd,
                                          2,
                                          global_hinstance, 0);

    global_decksurface_hwnd = CreateWindowEx(0, "DeckSurfaceClass", "",
                                             WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER,
                                             0, 0, 0, 0,
                                             hwnd,
                                             7,
                                             global_hinstance, 0);

    global_button_stats_hwnd = CreateWindowEx(0, "BUTTON", "",
                                              WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                              0, 0, 0, 0,
                                              hwnd,
                                              RES_MAINMENU_BUTTON_STATS,
                                              global_hinstance, 0);

    button_deckinfo = CreateWindowEx(0, "BUTTON", "",
                                     WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                     0, 0, 0, 0,
                                     hwnd,
                                     RES_MAINMENU_BUTTON_DECKINFO,
                                     global_hinstance, 0);

    button_exit = CreateWindowEx(0, "BUTTON", "",
                                 WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                 0, 0, 0, 0,
                                 hwnd,
                                 RES_MAINMENU_BUTTON_EXIT,
                                 global_hinstance, 0);

    button_deck1 = CreateWindowEx(0, "BUTTON", "",
                                  WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                  0, 0, 0, 0,
                                  hwnd,
                                  RES_MAINMENU_BUTTON_DECK_1,
                                  global_hinstance, 0);

    button_deck2 = CreateWindowEx(0, "BUTTON", "",
                                  WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                  0, 0, 0, 0,
                                  hwnd,
                                  RES_MAINMENU_BUTTON_DECK_2,
                                  global_hinstance, 0);

    button_deck3 = CreateWindowEx(0, "BUTTON", "",
                                  WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_OWNERDRAW,
                                  0, 0, 0, 0,
                                  hwnd,
                                  RES_MAINMENU_BUTTON_DECK_3,
                                  global_hinstance, 0);

    global_listbox_hwnd = CreateWindowEx(0, "LISTBOX", "",
                                         (WS_CHILDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | WS_THICKFRAME | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED | LBS_SORT),
                                         0, 0, 0, 0,
                                         hwnd,
                                         4,
                                         global_hinstance, 0);

    global_horzlist_hwnd = CreateWindowEx(0, "HorzListClass", "",
                                          WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL,
                                          0, 0, 0, 0,
                                          hwnd,
                                          5,
                                          global_hinstance, 0);

    cardlistfilter_hwnd = CreateWindowEx(0, "CardListFilterClass", "",
                                         WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                         0, 0, 0, 0,
                                         hwnd,
                                         3,
                                         global_hinstance, 0);

    popup = CreatePopupMenu();
    global_smallcard_popup = CreatePopupMenu();

    search_hwnd = CreateWindowEx(0, "EDIT", "",
                                 WS_CHILDWINDOW | WS_VISIBLE | ES_AUTOHSCROLL,
                                 0, 0, 0, 0,
                                 hwnd,
                                 8,
                                 global_hinstance, 0);
    if (!search_hwnd || !global_title_hwnd || !global_fullcard_hwnd || !global_listbox_hwnd || !global_horzlist_hwnd || !cardlistfilter_hwnd || !global_decksurface_hwnd || !popup)
    {
      DESTROY_MENU(popup);
      DESTROY_MENU(global_smallcard_popup);
      return -1;
    }

    global_wndproc_std_SearchClass = SetWindowLong(search_hwnd, GWL_WNDPROC, wndproc_SearchClass);

    SendMessage(global_listbox_hwnd, LB_SETTOPINDEX, 0, 0);
    SendMessage(global_listbox_hwnd, LB_SETCURSEL, 0, 0);
    SendMessage(global_horzlist_hwnd, 0x8007, 0, 0);

    {
      csvid_t csvid = SendMessage(global_horzlist_hwnd, 0x8004, 0, 0);
      if (!(csvid >= 0) && global_deck_num_entries)
        csvid = global_deck[0].GDE_csvid;
      SendMessage(global_fullcard_hwnd, 0x8400, csvid, 0);
    }

    if (global_db_flags_1 & DBFLAGS_STANDALONE)
    {
      load_text("Menus", "MAINMENU_STANDALONE");
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
      load_text("Menus", "MAINMENU_GAUNTLET");
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
      load_text("Menus", "MAINMENU_ADVENTURE");
      AppendMenu(popup, MF_ENABLED, RES_FULLCARDMENU_EXPAND, text_lines[0]);
      AppendMenu(popup, MF_ENABLED, RES_MENU_CONSOLIDATE, text_lines[1]);
      AppendMenu(popup, MF_SEPARATOR, 0, 0);
      AppendMenu(popup, MF_ENABLED, RES_MAINMENU_EXIT, text_lines[2]);
    }

    SetFocus(global_horzlist_hwnd);
    load_text("Menus", "DECKINFO");
    SetWindowText(button_deckinfo, text_lines[0]);
    change_buttonclass_wndproc(hwnd);
    refresh_numofcards_text();
    if (!(global_db_flags_1 & DBFLAGS_SHANDALAR))
    {
      load_text("Menus", "EXIT");
      SetWindowText(button_exit, text_lines[1]);
    }
    else
    {
      load_text("Menus", "EXIT");
      SetWindowText(button_exit, text_lines[0]);

      load_text("Menus", "DECKNUMBERS");
      SetWindowText(button_deck1, text_lines[0]);
      SetWindowText(button_deck2, text_lines[1]);
      SetWindowText(button_deck3, text_lines[2]);

      switch (global_current_deck)
      {
      case 0:
        SetWindowText(button_deck1, text_lines[3]);
        break;
      case 1:
        SetWindowText(button_deck2, text_lines[4]);
        break;
      case 2:
        SetWindowText(button_deck3, text_lines[5]);
        break;
      }
    }
    return 0;

  case WM_INITMENU:
    CHECKMENU_IF(popup, RES_FULLCARDMENU_EXPAND, SendMessage(global_fullcard_hwnd, 0x8402, 0, 0));
    CHECKMENU_IF(popup, RES_MAINMENU_EFFECTS, global_cfg_effects);
    CHECKMENU_IF(popup, RES_MAINMENU_MUSIC, global_cfg_music);
    CHECKMENU_IF(popup, RES_MENU_CONSOLIDATE, global_cfg_consolidate);
    return 0;

  case WM_CLOSE:
    if (global_db_flags_1 & (DBFLAGS_GAUNTLET | DBFLAGS_STANDALONE | DBFLAGS_NOCARDCOUNTCHECK))
    {
      if (!global_deck_was_edited || ask_about_saving_deck() != 2)
        DestroyWindow(global_main_hwnd);
    }
    else
    {
      int num_failed = 0;
      for (i = 0; i < global_edited_deck_num_entries; ++i)
      {
        int excess = check_card_count(global_edited_deck[i].DeckEntry_csvid);
        if (excess > 0)
        {
          global_excessive_cards[num_failed].DeckEntry_csvid = global_edited_deck[i].DeckEntry_csvid;
          global_excessive_cards[num_failed].DeckEntry_Amount = excess;
          global_excessive_cards[num_failed].DeckEntry_FullName = global_edited_deck[i].DeckEntry_FullName;
          ++num_failed;
        }
      }
      global_excessive_cards[num_failed].DeckEntry_csvid = -1;

      if (!num_failed || show_dialog_infobox())
        DestroyWindow(global_main_hwnd);
    }
    return 0;

  case WM_DESTROY:
    DELETE_OBJ(pen_ltgrey);
    DELETE_OBJ(pen_medgrey);
    DESTROY_MENU(popup);
    DESTROY_MENU(global_smallcard_popup);
    if (!(global_db_flags_1 & DBFLAGS_SHANDALAR))
      free_sounds_and_music();
    PostQuitMessage(0);
    return 0;

  case WM_QUERYENDSESSION:
    return (!global_deck_was_edited || ask_about_saving_deck() != 2) ? 1 : 0;

  case WM_SETFOCUS:
    SetFocus(global_horzlist_hwnd);
    return 0;

  case WM_SIZE:
  {
    int pics_inline;
    LOGFONT font;
    int hgt6;
    int hgt33;
    HDC hdc_local;
    SIZE sz;
    RECT rect_title, rect_fullcard, rect_horzlist, rect_cardlistfilter, rect_decksurface, rect_stats;

    global_main_window_width = LOWORD(lparam);
    global_main_window_height = HIWORD(lparam);

    LockWindowUpdate(hwnd);

    global_smallcard_width =
        global_smallcard_normal_width =
            global_smallcard_piclist_width =
                global_smallcard_height =
                    global_smallcard_normal_width =
                        global_smallcard_piclist_height = global_main_window_height * 0.18;

    pics_inline = global_main_window_width / (global_smallcard_width * 1.1);
    if (pics_inline <= 0)
      pics_inline = 1;
    SetWindowLong(global_horzlist_hwnd, HORZLIST_PICSINLINE_INDEX, pics_inline);
    SetWindowLong(global_horzlist_hwnd, HORZLIST_WIDTHPLUSSPACE_INDEX, global_main_window_width / pics_inline);

    global_smallcard_smaller_width = global_smallcard_smaller_height = global_smallcard_width * 0.8;
    global_smallcard_smallest_width = global_smallcard_smallest_height = global_smallcard_width * 0.7;

    memcpy(&font, &global_logfont_template, sizeof(LOGFONT));

    DELETE_OBJ(global_font_28percent);
    font.lfHeight = 280 * global_main_window_height / 10000;
    global_font_28percent = CreateFontIndirect(&font);

    DELETE_OBJ(global_font_40percent);
    font.lfHeight = 400 * global_main_window_height / 10000;
    global_font_40percent = CreateFontIndirect(&font);

    hgt6 = 6 * global_main_window_height / 100;
    hgt33 = 33 * global_main_window_height / 100;

    hdc_local = GetDC(hwnd);
    ApplyCardArtPaletteToDc(hdc_local);

    GetTextExtentPoint32(hdc_local, "Load new deck", strlen("Load new deck"), &sz);
    sz.cx += 10;
    sz.cy += sz.cy / 2;

    ReleaseDC(hwnd, hdc_local);

#define MOVEWINDOW_TL_BR(hwnd, rect) MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, FALSE)
#define MOVEWINDOW_TL_WH(hwnd, rect) MoveWindow(hwnd, rect.left, rect.top, rect.right, rect.bottom, FALSE)

    SetRect(&rect_title,
            0, 0,
            hgt33, 10 * global_main_window_height / 100);

    SetRect(&rect_fullcard,
            0, rect_title.bottom,
            hgt33, rect_title.bottom + 5900 * global_main_window_height / 10000);

    SetRect(&rect_horzlist,
            0, global_main_window_height - 22 * global_main_window_height / 100,
            global_main_window_width, global_main_window_height);

    SetRect(&rect_cardlistfilter,
            rect_horzlist.left, rect_horzlist.top - hgt6,
            rect_horzlist.right, rect_horzlist.top);

    SetRect(&rect_decksurface,
            rect_fullcard.right, 0,
            global_main_window_width, rect_fullcard.bottom);

    MOVEWINDOW_TL_BR(global_title_hwnd, rect_title);
    MOVEWINDOW_TL_BR(global_fullcard_hwnd, rect_fullcard);
    MOVEWINDOW_TL_BR(global_decksurface_hwnd, rect_decksurface);

    rect_horzlist.top = rect_horzlist.bottom - global_smallcard_piclist_height - 8 - GetSystemMetrics(SM_CYHSCROLL);
    MOVEWINDOW_TL_BR(global_horzlist_hwnd, rect_horzlist);

    rect_cardlistfilter.top = rect_horzlist.top - hgt6;
    rect_cardlistfilter.bottom = rect_horzlist.top;
    MOVEWINDOW_TL_BR(cardlistfilter_hwnd, rect_cardlistfilter);

    SetRect(&rect_stats, rect_decksurface.left, rect_decksurface.bottom, (rect_decksurface.right - rect_decksurface.left) / 3, rect_cardlistfilter.top - rect_decksurface.bottom - 3);

    MOVEWINDOW_TL_WH(global_button_stats_hwnd, rect_stats);
    MoveWindow(button_exit, rect_stats.left + 2 * rect_stats.right, rect_stats.top, rect_stats.right, rect_stats.bottom, 0);

#undef MOVEWINDOW_TL_BR
#undef MOVEWINDOW_TL_WH

    MoveWindow(search_hwnd,
               rect_title.left + ((rect_title.right - (600 * rect_title.right / 1000)) / 2),
               rect_stats.top,
               600 * rect_title.right / 1000,
               700 * MAX(rect_stats.bottom, 0) / 1000,
               FALSE);

    SendMessage(search_hwnd, WM_SETFONT, global_font_28percent, 0);

    if (global_db_flags_1 & DBFLAGS_SHANDALAR)
    {
      int x = rect_stats.left + rect_stats.right;
      MoveWindow(button_deck1,
                 x, rect_stats.top,
                 rect_stats.right / 3, rect_stats.bottom,
                 FALSE);
      MoveWindow(button_deck2,
                 x + rect_stats.right / 3, rect_stats.top,
                 rect_stats.right / 3, rect_stats.bottom,
                 FALSE);
      MoveWindow(button_deck3,
                 x + 2 * rect_stats.right / 3, rect_stats.top,
                 rect_stats.right / 3, rect_stats.bottom,
                 FALSE);
    }
    else
      MoveWindow(button_deckinfo,
                 rect_stats.left + rect_stats.right, rect_stats.top,
                 rect_stats.right, rect_stats.bottom,
                 FALSE);

    SendMessage(global_horzlist_hwnd, 0x8008, 0, global_smallcard_piclist_width);
    SendMessage(cardlistfilter_hwnd, 0x8402,
                MAKELONG(rect_cardlistfilter.left, rect_cardlistfilter.right),
                MAKELONG(rect_cardlistfilter.top, rect_cardlistfilter.bottom));

#if 0 // ...doesn't actually do anything.
	int lastpic, leftpic, pics_inline;
        lastpic = GetWindowLong(global_horzlist_hwnd, HORZLIST_LASTPIC_INDEX);
        leftpic = GetWindowLong(global_horzlist_hwnd, HORZLIST_LEFTPIC_INDEX);
        pics_inline = GetWindowLong(global_horzlist_hwnd, HORZLIST_PICSINLINE_INDEX);

        for (i = leftpic; ; ++i)
	  {
	    int rightpic = MIN(lastpic, leftpic + pics_inline);
	    if (i >= rightpic)
	      break;
	  }
#endif

    InvalidateRect(hwnd, NULL, TRUE);

    BringWindowToTop(global_title_hwnd);
    BringWindowToTop(cardlistfilter_hwnd);
    BringWindowToTop(global_horzlist_hwnd);
    BringWindowToTop(global_button_stats_hwnd);
    BringWindowToTop(button_deckinfo);
    BringWindowToTop(button_exit);

    if (!(global_db_flags_1 & DBFLAGS_SHANDALAR) && global_cfg_music)
      play_music(1, 400, 0);

    return 0;
  }

  case WM_DISPLAYCHANGE:
    SelectObject(global_hdc, global_old_bmp_obj);
    DELETE_OBJ(global_hbmp);

    hdc = GetDC(hwnd);
    ApplyCardArtPaletteToDc(hdc);

    global_hbmp = CreateCompatibleBitmap(hdc, LOWORD(lparam), HIWORD(lparam));

    ReleaseDC(hwnd, hdc);

    SelectObject(global_hdc, global_hbmp);

    if (global_hbmp)
      ShowWindow(hwnd, SW_SHOW);
    else
    {
      MessageBox(hwnd,
                 ("Not enough system memory to run at this screen resolution.\n"
                  "\n"
                  "Change it back to a smaller size before continuing.\n"
                  "(The game will hide itself until then)"),
                 "Deck Builder",
                 MB_ICONWARNING);
      ShowWindow(hwnd, SW_HIDE);
    }

    MoveWindow(hwnd, 0, 0, LOWORD(lparam), HIWORD(lparam), TRUE);
    return 0;

  case WM_ERASEBKGND:
    fill_back(hwnd, wparam);
    return 1;

  case WM_DRAWITEM:
  {
    DRAWITEMSTRUCT *item;
    item = (DRAWITEMSTRUCT *)lparam;
    draw_item(item, global_brush_mediumgrey, global_pic_all_butn, pen_ltgrey, pen_medgrey,
              GetFocus() == item->hwndItem && (item->itemState & ODS_SELECTED) ? global_colorref_white : RGB(0, 0, 0),
              GetFocus() == item->hwndItem,
              DT_SINGLELINE | DT_VCENTER | DT_CENTER);
    return 1;
  }

  case WM_MEASUREITEM:
  {
    MEASUREITEMSTRUCT *measure;
    measure = (MEASUREITEMSTRUCT *)lparam;
    if (measure->CtlID == 4)
    {
      RECT r;
      GetClientRect(GetDlgItem(hwnd, wparam), &r);
      measure->itemWidth = r.right;
      measure->itemHeight = r.bottom / 8;
    }
    return 0;
  }

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
  {
    HDC hdc_local = (HDC)wparam;
    hdc = hdc_local;
    ApplyCardArtPaletteToDc(hdc);
    SetTextColor(hdc, GetFocus() == lparam ? global_colorref_white : global_colorref_darkgrey);
    SetBkMode(hdc, TRANSPARENT);
    return GetStockObject(HOLLOW_BRUSH);
  }

  case WM_CTLCOLORLISTBOX:
  {
    HDC hdc_local = (HDC)wparam;
    hdc = hdc_local;
    ApplyCardArtPaletteToDc(hdc);
    SetTextColor(hdc, RGB(0, 0, 0));
    return global_listbox_hwnd == lparam ? global_brush_gold1 : global_brush_gold2;
  }

  case WM_CTLCOLOREDIT:
  {
    HDC hdc_local = (HDC)wparam;
    hdc = hdc_local;
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkColor(hdc, RGB(176, 176, 176));
    return global_brush_mediumgrey;
  }

  case WM_RBUTTONDOWN:
  {
    POINT p;
    p.x = GET_X_LPARAM(lparam);
    p.y = GET_Y_LPARAM(lparam);
    ClientToScreen(hwnd, &p);
    TrackPopupMenu(popup, TPM_RIGHTBUTTON, p.x, p.y, 0, hwnd, NULL);
    return 0;
  }

  case WM_COMMAND:
  {
    switch (LOWORD(wparam))
    {
    case RES_FULLCARDMENU_EXPAND:
      SendMessage(global_fullcard_hwnd, WM_COMMAND, wparam, lparam);
      break;

    case 4:
    case 5:
    {
      HWND w = (HWND)lparam;
      HWND tgt = LOWORD(wparam) == 4 ? global_horzlist_hwnd : global_listbox_hwnd;
      int cursel;
      int csvidraw;
      if (HIWORD(wparam) != 1)
        break;

      cursel = SendMessage(w, w == global_horzlist_hwnd ? 0x8003 : LB_GETCURSEL, 0, 0);
      csvidraw = SendMessage(w, w == global_horzlist_hwnd ? 0x8004 : LB_GETITEMDATA, cursel, 0);
      SendMessage(global_fullcard_hwnd, 0x8400, csvidraw, 0);

      cursel = SendMessage(w, w == global_horzlist_hwnd ? 0x8003 : LB_GETCURSEL, 0, 0);
      SendMessage(tgt, w == global_horzlist_hwnd ? 0x8007 : LB_SETCURSEL, cursel, 0);
      break;
    }

    case RES_MAINMENU_BUTTON_STATS:
      DialogBoxParam(global_hinstance, MAKEINTRESOURCE(RES_DIALOG_DECKSTATS), global_main_hwnd, dlgproc_DeckStats, 0);
      break;

    case RES_MENU_CONSOLIDATE:
      SendMessage(global_decksurface_hwnd, WM_COMMAND, wparam, lparam);
      break;

    case RES_MAINMENU_NEWDECK:
      SendMessage(global_decksurface_hwnd, WM_COMMAND, RES_MENU_NEWDECK, 0);
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
          popup_loaded_with_args(hwnd, "DECKBUILDER", "DECKLOADERROR", global_deck_filename, MB_ICONINFORMATION);

          load_text("Menus", "NEWDECK");
          sprintf(global_deckname, text_lines[0]);

          sprintf(global_deck_filename, "%sNew.dck", global_playdeck_path);
        }
      }
      break;

    case RES_MAINMENU_SAVEDECK:
      load_text("Menus", "NEWDECK");
      if (!strcmp(global_deckname, text_lines[0]) || !global_deckname[0])
      {
        popup_loaded(hwnd, "DECKBUILDER", "NAMEYOURDECK", MB_ICONINFORMATION);
        show_dialog_deckinfo();
        return 2;
      }

      if (global_deck_num_cards < 40)
      {
        popup_loaded(hwnd, "DECKBUILDER", "TOOFEWCARDS", MB_ICONINFORMATION);
        return 2;
      }

      if (global_deck_num_cards > 500 || global_edited_deck_num_entries > 200)
        popup_loaded(hwnd, "DECKBUILDER", "TOOMANYCARDS", MB_ICONINFORMATION);

      sprintf(global_deck_filename, "%s%s.dck", global_playdeck_path, global_deckname);

      if (global_deckname_set)
      {
        FILE *f = fopen(global_deck_filename, "rt");
        if (f != NULL)
        {
          int rval;

          fclose(f);

          rval = popup_loaded_with_args(hwnd, "DECKBUILDER", "DECKEXISTS", global_deckname, MB_ICONQUESTION | MB_YESNOCANCEL);
          if (rval == IDNO || rval == IDCANCEL)
            return 2;
        }
      }

      global_deckname_set = false;
      global_deck_was_edited = false;

      if (save_deck(global_deck_filename))
        popup_loaded_with_args(hwnd, "DECKBUILDER", "SAVED", global_deckname, MB_ICONINFORMATION);
      else
        popup_loaded_with_args(hwnd, "DECKBUILDER", "DECKSAVEERROR", global_deckname, MB_ICONINFORMATION);

      if (global_db_flags_1 & (DBFLAGS_EDITDECK | DBFLAGS_GAUNTLET | DBFLAGS_NOCARDCOUNTCHECK))
        strcpy(global_external_deckname, global_deckname);

      return 6;

    case RES_MAINMENU_EFFECTS:
      global_cfg_effects = !global_cfg_effects;
      break;

    case RES_MAINMENU_MUSIC:
      if ((global_cfg_music = !global_cfg_music))
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

    case RES_MAINMENU_BUTTON_DECKINFO:
      show_dialog_deckinfo();
      InvalidateRect(global_title_hwnd, 0, TRUE);
      break;

    case RES_MAINMENU_BUTTON_DECK_1:
    case RES_MAINMENU_BUTTON_DECK_2:
    case RES_MAINMENU_BUTTON_DECK_3:
      if (!(global_db_flags_2 & DBFLAGS_SHANDALAR))
        break;

      load_text("Menus", "DECKNUMBERS");
      SetWindowText(button_deck1, text_lines[0]);
      SetWindowText(button_deck2, text_lines[1]);
      SetWindowText(button_deck3, text_lines[2]);

      global_current_deck = LOWORD(wparam) - RES_MAINMENU_BUTTON_DECK_1;
      global_deck_num_cards = 0;
      global_edited_deck_num_entries = 0;

      for (i = 0; i < global_deck_num_entries; ++i)
        if (global_deck[i].GDE_DecksBits & (1 << global_current_deck))
        {
          global_deck[i].GDE_Available = 0;
          insert_cards_into_deck(global_deck[i].GDE_csvid, 1, global_edited_deck);
        }
        else
          global_deck[i].GDE_Available = 1;

      count_packs();

      SendMessage(global_decksurface_hwnd, 0x8401, 0, 0);

      filter_cards_in_lists(global_listbox_hwnd, global_horzlist_hwnd);

      switch (global_current_deck)
      {
      case 0:
        SetWindowText(button_deck1, text_lines[3]);
        break;
      case 1:
        SetWindowText(button_deck2, text_lines[4]);
        break;
      case 2:
        SetWindowText(button_deck3, text_lines[5]);
        break;
      }

      SetFocus(global_horzlist_hwnd);

      break;
    }
    return 0;
  }

  case 0x8466:
    if (lparam != global_fullcard_hwnd)
      SendMessage(global_fullcard_hwnd, 0x8466, wparam, 0);
    return 0;

  case 0x84c8:
  {
    HWND hwnd1, hwnd2;
    hwnd1 = wparam;
    hwnd2 = lparam;
    if (GetDlgItem(hwnd, RES_MAINMENU_BUTTON_STATS) == hwnd1 || GetDlgItem(hwnd, RES_MAINMENU_BUTTON_DECKINFO) == hwnd1 || GetDlgItem(hwnd, RES_MAINMENU_BUTTON_EXIT) == hwnd1 || GetDlgItem(hwnd, RES_MAINMENU_BUTTON_DECK_1) == hwnd1 || GetDlgItem(hwnd, RES_MAINMENU_BUTTON_DECK_2) == hwnd1 || GetDlgItem(hwnd, RES_MAINMENU_BUTTON_DECK_3) == hwnd1)
      SendMessage(hwnd, 0x8401, GetDlgCtrlID(hwnd1), 0);
    else
      SendMessage(hwnd, 0x8401, 1, 0);

    if (hwnd1)
      InvalidateRect(hwnd1, NULL, TRUE);

    if (hwnd2)
      InvalidateRect(hwnd2, NULL, TRUE);

    return 0;
  }

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
}

// FUNCTION: DECKDLL 0x10002b0f
LRESULT CALLBACK
wndproc_SideboardSurfaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
}

// FUNCTION: DECKDLL 0x10003f6a
LRESULT CALLBACK
wndproc_TradeSurfaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
}
