#include <stddef.h>
#include <windows.h>
#include "mystdbool.h"
#include "defs.h"
#include "deckdll.h"
#include "full_deck.h"
#include "resources.h"
#include "cardartlib/src/palette.h"
#include "drawcardlib/src/pic.h"

typedef ptrdiff_t INT_PTR;

extern char text_lines[500][128];

extern FullDeck global_edited_deck;
#define global_edited_deck_num_entries (global_edited_deck.total)

extern GlobalDeckInfoBlob global_deckinfo;
;
extern const card_ptr_t *cards_ptr;
extern card_ptr_t global_raw_cards_storage[1000];

extern COLORREF global_colorref_darkgrey;
extern COLORREF global_colorref_flesh;
extern COLORREF global_colorref_lavender;
extern COLORREF global_colorref_lightgrey;
extern COLORREF global_colorref_white;

extern HFONT global_font_32;
extern char global_base_directory[];
extern char global_duelart_path[];
extern HANDLE global_pic_statbak1;

bool check_restricted(csvid_t csvid);
bool check_banned(csvid_t csvid);
bool check_ante(csvid_t csvid);
bool check_basic(csvid_t csvid);
int load_text(const char *file_name, const char *section_name);

typedef enum
{
  DT_UNKNOWN = 0,
  DT_UNRESTRICTED = 0x1,
  DT_WILD = 0x2,
  DT_RESTRICTED_T1 = 0x4,
  DT_TOURNAMENT_T1_5 = 0x8,
  DT_HIGHLANDER = 0x10,
  DT_HAS_ANTE = 0x20,
} DeckType;

#define DASH "\227"

// FUNCTION: DECKDLL 0x1002f0ec
static int
check_deck_type(void)
{
  struct
  {
    int all_unique_nonbasics;
    int i;
    int rval;
    int num;
    int csvid;
  } s;

  s.rval = DT_UNKNOWN;
  s.all_unique_nonbasics = 1;

  for (s.i = 0; s.i < global_edited_deck_num_entries; ++s.i)
    if (!check_basic(global_edited_deck.entries[s.i].DeckEntry_csvid))
    {
      if (global_edited_deck.entries[s.i].DeckEntry_Amount > 1)
        s.all_unique_nonbasics = 0;
      if (check_ante(global_edited_deck.entries[s.i].DeckEntry_csvid))
        s.rval |= DT_HAS_ANTE;
    }

  if (s.all_unique_nonbasics)
    return s.rval | DT_HIGHLANDER;

  for (s.i = 0; s.i < global_edited_deck_num_entries; ++s.i)
  {
    s.csvid = global_edited_deck.entries[s.i].DeckEntry_csvid;
    s.num = global_edited_deck.entries[s.i].DeckEntry_Amount;
    if (!check_basic(s.csvid))
    {
      if (check_banned(s.csvid))
        s.rval |= DT_UNRESTRICTED;
      else if (check_restricted(s.csvid))
      {
        if (s.num > 4)
          s.rval |= DT_UNRESTRICTED;
        else if (s.num > 1)
          s.rval |= DT_WILD;
        else
          s.rval |= DT_RESTRICTED_T1;
      }
      else if (s.num > 4)
        s.rval |= DT_UNRESTRICTED;
      else
        s.rval |= DT_TOURNAMENT_T1_5;
    }
  }

  return s.rval;
}

// GLOBAL: DECKDLL 0x101053a8
static int global_deck_stats[9][7];

// GLOBAL: DECKDLL 0x10105368
static LOGFONT global_stats_font_desc;

// GLOBAL: DECKDLL 0x101054a4
static HFONT global_stats_font;

// GLOBAL: DECKDLL 0x101054a8
static HANDLE global_stats_pic;

// GLOBAL: DECKDLL 0x101054ac
static COLORREF global_colorref_stats_lightgrey;

// GLOBAL: DECKDLL 0x101054b0
static COLORREF global_colorref_stats_lavender;

// GLOBAL: DECKDLL 0x101054b4
static COLORREF global_colorref_stats_flesh;

// GLOBAL: DECKDLL 0x1003a878
static const LOGFONT stats_logfont_template = {
    0,         /* lfHeight */
    0,         /* lfWidth */
    0,         /* lfEscapement */
    0,         /* lfOrientation */
    FW_NORMAL, /* lfWeight (0x190) */
    0,         /* lfItalic */
    0,         /* lfUnderline */
    0,         /* lfStrikeOut */
    0,         /* lfCharSet */
    0,         /* lfOutPrecision */
    0,         /* lfClipPrecision */
    0,         /* lfQuality */
    0,         /* lfPitchAndFamily */
    {0}        /* lfFaceName */
};

// FUNCTION: DECKDLL 0x1002e9ee
static void
build_deck_stats_table(void)
{
  struct
  {
    int j;
    int i;
    int col;
    int amt;
    int num_five_color_lands;
    int row;
    int csvid;
  } s;

  s.num_five_color_lands = 0;
  for (s.j = 0; s.j < 9; ++s.j)
    for (s.i = 0; s.i < 7; ++s.i)
      global_deck_stats[s.j][s.i] = 0;

  for (s.i = 0; s.i < global_edited_deck_num_entries; ++s.i)
  {
    s.csvid = global_edited_deck.entries[s.i].DeckEntry_csvid;
    s.amt = global_edited_deck.entries[s.i].DeckEntry_Amount;

    if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_CREATURE)
      s.row = 1;
    else if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_ENCHANTMENT)
      s.row = 2;
    else if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_SORCERY)
      s.row = 3;
    else if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_INSTANT)
      s.row = 4;
    else if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_INTERRUPT)
      s.row = 5;
    else if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_ARTIFACT)
      s.row = 7;
    else if (global_raw_cards_storage[s.csvid].card_type == CP_TYPE_LAND)
      s.row = 6;
    else
      continue;

    if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLACK)
      s.col = 0;
    else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_BLUE)
      s.col = 1;
    else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_GREEN)
      s.col = 2;
    else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_RED)
      s.col = 3;
    else if (global_raw_cards_storage[s.csvid].color == CP_COLOR_WHITE)
      s.col = 4;
    else
      s.col = -1;

    if (s.row == 6)
    {
      if (s.csvid == 0xa4)
        s.col = 3;
      else if (s.csvid == 0x5b)
        s.col = 2;
      else if (s.csvid == 0x7e)
        s.col = 1;
      else if (s.csvid == 0xbc)
        s.col = 4;
      else if (s.csvid == 0xef)
        s.col = 0;
      else
        s.col = 5;
    }

    if (s.row == 7)
    {
      s.col = 5;
      if (global_raw_cards_storage[s.csvid].subtype == 0x2c)
        s.row = 1;
    }

    if (s.col == -1)
    {
      MessageBox(NULL, "Color is -1", "Stats Error", 0);
      continue;
    }

    if (global_raw_cards_storage[s.csvid].db_card_type_2 == 10)
    {
      if (s.csvid == 0xf || s.csvid == 0x11 || s.csvid == 0x193 || s.csvid == 0x1b9)
      {
        s.num_five_color_lands += s.amt;
        global_deck_stats[0][0] += s.amt;
        global_deck_stats[0][1] += s.amt;
        global_deck_stats[0][2] += s.amt;
        global_deck_stats[0][3] += s.amt;
        global_deck_stats[0][4] += s.amt;
      }
      else if (s.csvid == 0xa5)
        global_deck_stats[0][2] += s.amt;
      else if (s.csvid == 0xa6)
        global_deck_stats[0][0] += s.amt;
      else if (s.csvid == 0xa7)
        global_deck_stats[0][4] += s.amt;
      else if (s.csvid == 0xa8)
        global_deck_stats[0][3] += s.amt;
      else if (s.csvid == 0xa9)
        global_deck_stats[0][1] += s.amt;
      else if (s.csvid == 9)
      {
        global_deck_stats[0][0] += s.amt;
        global_deck_stats[0][3] += s.amt;
      }
      else if (s.csvid == 0xc)
      {
        global_deck_stats[0][0] += s.amt;
        global_deck_stats[0][2] += s.amt;
      }
      else if (s.csvid == 0xbd)
      {
        global_deck_stats[0][3] += s.amt;
        global_deck_stats[0][4] += s.amt;
      }
      else if (s.csvid == 0xd4)
      {
        global_deck_stats[0][4] += s.amt;
        global_deck_stats[0][2] += s.amt;
      }
      else if (s.csvid == 0xd8)
      {
        global_deck_stats[0][4] += s.amt;
        global_deck_stats[0][0] += s.amt;
      }
      else if (s.csvid == 0xf1)
      {
        global_deck_stats[0][3] += s.amt;
        global_deck_stats[0][2] += s.amt;
      }
      else if (s.csvid == 0xfc)
      {
        global_deck_stats[0][1] += s.amt;
        global_deck_stats[0][2] += s.amt;
      }
      else if (s.csvid == 0xfe)
      {
        global_deck_stats[0][4] += s.amt;
        global_deck_stats[0][1] += s.amt;
      }
      else if (s.csvid == 0x102)
      {
        global_deck_stats[0][1] += s.amt;
        global_deck_stats[0][0] += s.amt;
      }
      else if (s.csvid == 0x10a)
      {
        global_deck_stats[0][1] += s.amt;
        global_deck_stats[0][3] += s.amt;
      }
      else
        global_deck_stats[0][s.col] += s.amt;
    }

    global_deck_stats[s.row][s.col] += s.amt;
  }

  for (s.j = 0; s.j < 8; ++s.j)
    for (s.i = 0; s.i < 6; ++s.i)
      global_deck_stats[s.j][6] += global_deck_stats[s.j][s.i];

  global_deck_stats[0][6] -= s.num_five_color_lands * 4;

  for (s.i = 0; s.i < 6; ++s.i)
    for (s.j = 0; s.j < 8; ++s.j)
      if (s.j != 0)
        global_deck_stats[8][s.i] += global_deck_stats[s.j][s.i];

  for (s.i = 0; s.i < 8; ++s.i)
    if (s.i != 0)
      global_deck_stats[8][6] += global_deck_stats[s.i][6];
}

// FUNCTION: DECKDLL 0x1002e3fd
static void
show_stats(HDC hdc, SIZE word_size, int *stepx, int *stepy, int *posx, int *starty)
{
  struct
  {
    char buf[260];
    int u;
    int y;
    int x;
  } s;

  load_text("menus", "STATSSCREEN");
  SetTextColor(hdc, global_colorref_stats_lightgrey);

  s.x = 20;
  s.y = 20;
  wsprintf(s.buf, text_lines[0]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));

  s.x += word_size.cx;
  *stepx = (1000 - s.x) / 8;
  s.x += 0x50;
  *posx = s.x;

  wsprintf(s.buf, text_lines[1]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.x += *stepx;
  wsprintf(s.buf, text_lines[2]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.x += *stepx;
  wsprintf(s.buf, text_lines[3]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.x += *stepx;
  wsprintf(s.buf, text_lines[4]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.x += *stepx;
  wsprintf(s.buf, text_lines[5]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.x += *stepx;
  wsprintf(s.buf, text_lines[6]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.x += *stepx;
  wsprintf(s.buf, text_lines[7]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));

  *stepy = 65;
  *starty = *stepy + 20;
  SetTextColor(hdc, global_colorref_stats_lavender);
  s.x = 20;
  s.y = *starty;
  wsprintf(s.buf, text_lines[8]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));

  SetTextColor(hdc, global_colorref_stats_lightgrey);
  s.y += word_size.cy / 2;
  s.y += *stepy;
  wsprintf(s.buf, text_lines[9]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.y += *stepy;
  wsprintf(s.buf, text_lines[10]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.y += *stepy;
  wsprintf(s.buf, text_lines[11]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.y += *stepy;
  wsprintf(s.buf, text_lines[12]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.y += *stepy;
  wsprintf(s.buf, text_lines[13]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.y += *stepy;
  wsprintf(s.buf, text_lines[14]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.y += *stepy;
  wsprintf(s.buf, text_lines[15]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  //  s.y += *stepy;
  s.y += word_size.cy;
  wsprintf(s.buf, text_lines[16]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
  s.y += *stepy;
  wsprintf(s.buf, text_lines[7]);
  TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
}

// FUNCTION: DECKDLL 0x1002e096
static void
show_stats_values(HDC hdc, SIZE word_size, int stepx, int stepy, int posx, int starty)
{
  struct
  {
    char buf[264]; // ebp - 0x118
    int l;         // ebp - 0x10
    int y;         // ebp - 0xc
    int c;         // ebp - 8
    int x;         // ebp - 4
  } s;

  s.y = starty;
  SetTextColor(hdc, global_colorref_stats_flesh);
  for (s.l = 0; s.l < 9; ++s.l)
  {
    s.x = posx;
    if (s.l == 1)
      s.y += word_size.cy / 2;

    for (s.c = 0; s.c < 7; ++s.c)
    {
      if (s.l == 0)
        SetTextColor(hdc, global_colorref_stats_lavender);
      else
        SetTextColor(hdc, global_colorref_stats_flesh);

      if (global_deck_stats[s.l][s.c] != 0)
      {
        if (s.l == 8 || s.c == 6)
        {
          int percent;
          if (global_deck_stats[s.l][s.c] == 0 || global_deck_stats[8][6] == 0)
            percent = 0;
          else
            percent = (global_deck_stats[s.l][s.c] * 100 + (global_deck_stats[8][6] >> 1)) / global_deck_stats[8][6];

          wsprintf(s.buf, "(%d) %d%%", global_deck_stats[s.l][s.c], percent);
        }
        else
        {
          int percent;
          if (global_deck_stats[s.l][s.c] == 0 || global_deck_stats[s.l][6] == 0)
            percent = 0;
          else
            percent = (global_deck_stats[s.l][s.c] * 100 + (global_deck_stats[s.l][6] >> 1)) / global_deck_stats[s.l][6];

          wsprintf(s.buf, "(%d) %d%%", global_deck_stats[s.l][s.c], percent);
        }
      }
      else
      {
        int percent;
        if (global_deck_stats[s.l][s.c] == 0 || global_deck_stats[s.l][6] == 0)
          percent = 0;
        else
          percent = (global_deck_stats[s.l][s.c] * 100 + (global_deck_stats[s.l][6] >> 1)) / global_deck_stats[s.l][6];

        wsprintf(s.buf, "  -", global_deck_stats[s.l][s.c], percent);
      }

      TextOut(hdc, s.x, s.y, s.buf, strlen(s.buf));
      s.x += stepx;
    }

    if (s.l != 7)
      s.y += stepy;
    else
      s.y += word_size.cy + stepy;
  }
}

// FUNCTION: DECKDLL 0x1002dfd0
static void
fill_stats_window(HDC hdc, LONG left, LONG top, LONG right, LONG bottom, HFONT font)
{
  struct
  {
    int posx;  // ebp - 0x18
    int stepy; // ebp - 0x14
    SIZE sz;
    int starty; // ebp - 0x8
    int stepx;  // ebp - 0x4
  } s;

  build_deck_stats_table();
  SetMapMode(hdc, MM_ANISOTROPIC);
  SetWindowExtEx(hdc, 1000, 750, NULL);
  SetViewportExtEx(hdc, right - left, bottom - top, NULL);
  SelectObject(hdc, font);
  SetBkMode(hdc, TRANSPARENT);

  GetTextExtentPoint32(hdc, "Enchantments", 0xc, &s.sz);

  show_stats(hdc, s.sz, &s.stepx, &s.stepy, &s.posx, &s.starty);
  show_stats_values(hdc, s.sz, s.stepx, s.stepy, s.posx, s.starty);
}

// FUNCTION: DECKDLL 0x1002db30
INT_PTR CALLBACK
dlgproc_DeckStats(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  /* Stack layout is extremely sensitive; keep locals grouped. */
  struct
  {
    HDC hdc_erase;         /* [ebp-0x150] */
    RECT client;           /* [ebp-0x14c] */
    LPARAM lparam_copy;    /* [ebp-0x13c] */
    HDC hdc_ctl;           /* [ebp-0x138] */
    HGDIOBJ brush;         /* [ebp-0x134] */
    HDC chdc;              /* [ebp-0x130] */
    char txt[264];         /* [ebp-0x12c] */
    BITMAP bmp;            /* [ebp-0x24] */
    int y;                 /* [ebp-0x0c] */
    int x;                 /* [ebp-0x08] */
    unsigned int decktype; /* [ebp-0x04] */
  } s;

  switch (msg)
  {
  case WM_INITDIALOG:
    load_text("menus", "STATSDIALOG");
    strcpy(s.txt, text_lines[0]);
    strcat(s.txt, ": ");
    strcat(s.txt, global_deckinfo.deckname);
    strcat(s.txt, "  - ");
    s.decktype = check_deck_type();
    load_text("MP_UIStrings.txt", "SHELLPAGE_MULTIDUEL");
    if (s.decktype & DT_UNRESTRICTED)
      strcat(s.txt, text_lines[1]);
    else if (s.decktype & DT_WILD)
      strcat(s.txt, text_lines[2]);
    else if (s.decktype & DT_RESTRICTED_T1)
      strcat(s.txt, text_lines[3]);
    else if (s.decktype & DT_TOURNAMENT_T1_5)
      strcat(s.txt, text_lines[4]);
    else if (s.decktype & DT_HIGHLANDER)
      strcat(s.txt, text_lines[5]);
    else
      strcat(s.txt, "Unknown");

    if (s.decktype & DT_HAS_ANTE)
    {
      strcat(s.txt, " / ");
      strcat(s.txt, text_lines[7] + 1);
    }

    SetWindowTextA(hdlg, s.txt);
    sprintf(s.txt, "%s\\GAUN_Options.pic", global_duelart_path);
    global_stats_pic = (HANDLE)load_pic(s.txt);
    memcpy(&global_stats_font_desc, &stats_logfont_template, sizeof(LOGFONT));
    global_stats_font_desc.lfHeight = 30;
    strcpy(global_stats_font_desc.lfFaceName, "Cheltenham ITC Bold BT");
    global_stats_font = CreateFontIndirectA(&global_stats_font_desc);
    global_colorref_stats_lavender = 0x02efb2ae;
    global_colorref_stats_lightgrey = 0x02f6f7f7;
    global_colorref_stats_flesh = 0x028fb0cd;
    return 0;

  case WM_COMMAND:
    if (((((unsigned int)wparam) & 0xffff) == RES_BUTTON_OK) || ((((unsigned int)wparam) & 0xffff) == RES_BUTTON_CANCEL))
    {
      DeleteObject(global_stats_font);
      if (global_stats_pic)
        delete_and_close_object(global_stats_pic);
      EndDialog(hdlg, 0);
    }
    return 1;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    s.hdc_ctl = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc_ctl);
    s.lparam_copy = lparam;
    SetBkMode(s.hdc_ctl, TRANSPARENT);
    s.brush = GetStockObject(HOLLOW_BRUSH);
    return (INT_PTR)s.brush;

  case WM_ERASEBKGND:
    s.hdc_erase = (HDC)wparam;
    ApplyCardArtPaletteToDc(s.hdc_erase);
    GetClientRect(hdlg, &s.client);
    s.chdc = CreateCompatibleDC(s.hdc_erase);
    ApplyCardArtPaletteToDc(s.chdc);
    SelectObject(s.chdc, (HGDIOBJ)global_pic_statbak1);
    GetObjectA((HGDIOBJ)global_pic_statbak1, sizeof(BITMAP), &s.bmp);
    for (s.x = 0; s.client.right > s.x; s.x = s.x + s.bmp.bmWidth)
      for (s.y = 0; s.client.bottom > s.y; s.y = s.y + s.bmp.bmHeight)
        BitBlt(s.hdc_erase, s.x, s.y, s.bmp.bmWidth, s.bmp.bmHeight, s.chdc, 0, 0, SRCCOPY);

    fill_stats_window(s.hdc_erase, s.client.left, s.client.top, s.client.right, s.client.bottom, global_stats_font);
    DeleteDC(s.chdc);
    return 1;

  default:
    return 0;
  }
}
