#include <stddef.h>
#include <windows.h>
#include "mystdbool.h"
#include "defs.h"
#include "deckdll.h"
#include "resources.h"
#include "cardartlib/src/palette.h"
#include "sidlib/pic.h"

typedef ptrdiff_t INT_PTR;

extern char text_lines[500][128];

extern int global_edited_deck_num_entries;
extern DeckEntry global_edited_deck[300];
extern char global_deckname[32];
extern const card_ptr_t *cards_ptr;

extern COLORREF global_colorref_darkgrey;
extern COLORREF global_colorref_flesh;
extern COLORREF global_colorref_lavender;
extern COLORREF global_colorref_lightgrey;
extern COLORREF global_colorref_white;

extern HFONT global_font_32;
extern char global_base_directory[];

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
  int rval = DT_UNKNOWN;

  bool any_duplicate_nonbasics = false;
  int i;
  for (i = 0; i < global_edited_deck_num_entries; ++i)
    if (!check_basic(global_edited_deck[i].DeckEntry_csvid))
    {
      if (global_edited_deck[i].DeckEntry_Amount > 1)
        any_duplicate_nonbasics = true;
      if (check_ante(global_edited_deck[i].DeckEntry_csvid))
        rval |= DT_HAS_ANTE;
    }

  if (!any_duplicate_nonbasics)
    return rval | DT_HIGHLANDER;

  for (i = 0; i < global_edited_deck_num_entries; ++i)
  {
    csvid_t csvid;
    int num;

    csvid = global_edited_deck[i].DeckEntry_csvid;
    if (check_basic(csvid))
      continue;

    if (check_banned(csvid))
    {
      rval |= DT_UNRESTRICTED;
      continue;
    }

    num = global_edited_deck[i].DeckEntry_Amount;
    if (num > 4)
    {
      rval |= DT_UNRESTRICTED;
      continue;
    }

    if (!check_restricted(csvid))
      rval |= DT_TOURNAMENT_T1_5;
    else if (num <= 1)
      rval |= DT_RESTRICTED_T1;
    else
      rval |= DT_WILD;
  }

  return rval;
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

// FUNCTION: DECKDLL 0x1002e9ee
static void
build_deck_stats_table(void)
{
  int i;
  int j;
  int k;
  int row;
  int col;
  int csvid;
  int amt;
  int num_five_color_lands;

  num_five_color_lands = 0;
  for (i = 0; i < 9; ++i)
    for (j = 0; j < 7; ++j)
      global_deck_stats[i][j] = 0;

  for (i = 0; i < global_edited_deck_num_entries; ++i)
  {
    csvid = global_edited_deck[i].DeckEntry_csvid;
    amt = global_edited_deck[i].DeckEntry_Amount;

    if (cards_ptr[csvid].card_type == CP_TYPE_CREATURE)
      row = 1;
    else if (cards_ptr[csvid].card_type == CP_TYPE_ENCHANTMENT)
      row = 2;
    else if (cards_ptr[csvid].card_type == CP_TYPE_SORCERY)
      row = 3;
    else if (cards_ptr[csvid].card_type == CP_TYPE_INSTANT)
      row = 4;
    else if (cards_ptr[csvid].card_type == CP_TYPE_INTERRUPT)
      row = 5;
    else if (cards_ptr[csvid].card_type == CP_TYPE_LAND)
      row = 6;
    else if (cards_ptr[csvid].card_type == CP_TYPE_ARTIFACT)
      row = 7;
    else
      continue;

    if (cards_ptr[csvid].color == CP_COLOR_BLACK)
      col = 0;
    else if (cards_ptr[csvid].color == CP_COLOR_BLUE)
      col = 1;
    else if (cards_ptr[csvid].color == CP_COLOR_GREEN)
      col = 2;
    else if (cards_ptr[csvid].color == CP_COLOR_RED)
      col = 3;
    else if (cards_ptr[csvid].color == CP_COLOR_WHITE)
      col = 4;
    else
      col = -1;

    if (row == 6)
    {
      if (csvid == 0xa4)
        col = 3;
      else if (csvid == 0x5b)
        col = 2;
      else if (csvid == 0x7e)
        col = 1;
      else if (csvid == 0xbc)
        col = 4;
      else if (csvid == 0xef)
        col = 0;
      else
        col = 5;
    }

    if (row == 7)
    {
      col = 5;
      if (cards_ptr[csvid].subtype1 == 0x2c && cards_ptr[csvid].subtype2 == 0)
        row = 1;
    }

    if (col == -1)
    {
      MessageBox(NULL, "Color is -1", "Stats Error", 0);
      continue;
    }

    if (cards_ptr[csvid].db_card_type_2 == 10)
    {
      if (csvid == 0xf || csvid == 0x11 || csvid == 0x193 || csvid == 0x1b9)
      {
        num_five_color_lands += amt;
        global_deck_stats[0][0] += amt;
        global_deck_stats[0][1] += amt;
        global_deck_stats[0][2] += amt;
        global_deck_stats[0][3] += amt;
        global_deck_stats[0][4] += amt;
      }
      else if (csvid == 0xa5)
        global_deck_stats[0][2] += amt;
      else if (csvid == 0xa6)
        global_deck_stats[0][0] += amt;
      else if (csvid == 0xa7)
        global_deck_stats[0][4] += amt;
      else if (csvid == 0xa8)
        global_deck_stats[0][3] += amt;
      else if (csvid == 0xa9)
        global_deck_stats[0][1] += amt;
      else if (csvid == 9)
      {
        global_deck_stats[0][0] += amt;
        global_deck_stats[0][3] += amt;
      }
      else if (csvid == 0xc)
      {
        global_deck_stats[0][0] += amt;
        global_deck_stats[0][2] += amt;
      }
      else if (csvid == 0xbd)
      {
        global_deck_stats[0][3] += amt;
        global_deck_stats[0][4] += amt;
      }
      else if (csvid == 0xd4)
      {
        global_deck_stats[0][4] += amt;
        global_deck_stats[0][2] += amt;
      }
      else if (csvid == 0xd8)
      {
        global_deck_stats[0][4] += amt;
        global_deck_stats[0][0] += amt;
      }
      else if (csvid == 0xf1)
      {
        global_deck_stats[0][3] += amt;
        global_deck_stats[0][2] += amt;
      }
      else if (csvid == 0xfc)
      {
        global_deck_stats[0][1] += amt;
        global_deck_stats[0][2] += amt;
      }
      else if (csvid == 0xfe)
      {
        global_deck_stats[0][4] += amt;
        global_deck_stats[0][1] += amt;
      }
      else if (csvid == 0x102)
      {
        global_deck_stats[0][1] += amt;
        global_deck_stats[0][0] += amt;
      }
      else if (csvid == 0x10a)
      {
        global_deck_stats[0][1] += amt;
        global_deck_stats[0][3] += amt;
      }
      else
        global_deck_stats[0][col] += amt;
    }

    global_deck_stats[row][col] += amt;
  }

  for (i = 0; i < 8; ++i)
    for (j = 0; j < 6; ++j)
      global_deck_stats[i][6] += global_deck_stats[i][j];

  global_deck_stats[0][6] += num_five_color_lands * -4;

  for (j = 0; j < 6; ++j)
    for (k = 0; k < 8; ++k)
      if (k != 0)
        global_deck_stats[8][j] += global_deck_stats[k][j];

  for (k = 0; k < 8; ++k)
    if (k != 0)
      global_deck_stats[8][6] += global_deck_stats[k][6];
}

// FUNCTION: DECKDLL 0x1002e3fd
static void
show_stats(HDC hdc, int word_width, int word_height, int *stepx, int *stepy, int *posx, int *starty)
{
  char buf[264];
  int x;
  int y;
  int u;
  int len;

  load_text("Menus", "STATSSCREEN");
  SetTextColor(hdc, global_colorref_stats_lightgrey);

  x = 20;
  y = 20;
  wsprintf(buf, text_lines[0]);
  len = strlen(buf);
  TextOut(hdc, x, y, buf, len);

  u = 1000 - (x + word_width);
  *stepx = (u + (((unsigned int)u >> 31) & 7)) >> 3;
  x = x + word_width + 80;
  *posx = x;

  wsprintf(buf, text_lines[1]);
  len = strlen(buf);
  TextOut(hdc, x, y, buf, len);
  x += *stepx;
  wsprintf(buf, text_lines[2]);
  len = strlen(buf);
  TextOut(hdc, x, y, buf, len);
  x += *stepx;
  wsprintf(buf, text_lines[3]);
  len = strlen(buf);
  TextOut(hdc, x, y, buf, len);
  x += *stepx;
  wsprintf(buf, text_lines[4]);
  len = strlen(buf);
  TextOut(hdc, x, y, buf, len);
  x += *stepx;
  wsprintf(buf, text_lines[5]);
  len = strlen(buf);
  TextOut(hdc, x, y, buf, len);
  x += *stepx;
  wsprintf(buf, text_lines[6]);
  len = strlen(buf);
  TextOut(hdc, x, y, buf, len);
  x += *stepx;
  wsprintf(buf, text_lines[7]);
  len = strlen(buf);
  TextOut(hdc, x, y, buf, len);

  *stepy = 65;
  *starty = *stepy + 20;
  SetTextColor(hdc, global_colorref_stats_lavender);
  x = 20;
  y = *starty;
  wsprintf(buf, text_lines[8]);
  len = strlen(buf);
  TextOut(hdc, x, y, buf, len);

  SetTextColor(hdc, global_colorref_stats_lightgrey);
  y = y + word_height / 2 + *stepy;
  for (u = 9; u < 16; ++u)
  {
    wsprintf(buf, text_lines[u]);
    len = strlen(buf);
    TextOut(hdc, x, y, buf, len);
    y += *stepy;
  }
  y += word_height;
  wsprintf(buf, text_lines[16]);
  len = strlen(buf);
  TextOut(hdc, x, y, buf, len);
  y += *stepy;
  wsprintf(buf, text_lines[7]);
  len = strlen(buf);
  TextOut(hdc, x, y, buf, len);
}

// FUNCTION: DECKDLL 0x1002e096
static void
show_stats_values(HDC hdc, int unused_word_width, int word_height, int stepx, int stepy, int posx, int starty)
{
  char buf[264];
  int c;
  int l;
  int percent;
  int x;
  int y;

  y = starty;
  SetTextColor(hdc, global_colorref_stats_flesh);
  for (l = 0; l < 9; ++l)
  {
    x = posx;
    if (l == 1)
      y += word_height / 2;

    for (c = 0; c < 7; ++c)
    {
      if (l == 0)
        SetTextColor(hdc, global_colorref_stats_lavender);
      else
        SetTextColor(hdc, global_colorref_stats_flesh);

      if (global_deck_stats[l][c] == 0)
      {
        percent = 0;
        wsprintf(buf, "  -", global_deck_stats[l][c], percent);
      }
      else if (l == 8 || c == 6)
      {
        if (global_deck_stats[l][c] == 0 || global_deck_stats[8][6] == 0)
          percent = 0;
        else
          percent = (global_deck_stats[l][c] * 100 + (global_deck_stats[8][6] >> 1)) / global_deck_stats[8][6];

        wsprintf(buf, "(%d) %d%%", global_deck_stats[l][c], percent);
      }
      else
      {
        if (global_deck_stats[l][c] == 0 || global_deck_stats[l][6] == 0)
          percent = 0;
        else
          percent = (global_deck_stats[l][c] * 100 + (global_deck_stats[l][6] >> 1)) / global_deck_stats[l][6];

        wsprintf(buf, "(%d) %d%%", global_deck_stats[l][c], percent);
      }

      TextOut(hdc, x, y, buf, strlen(buf));
      x += stepx;
    }

    if (l == 7)
      y += word_height + stepy;
    else
      y += stepy;
  }
}

// FUNCTION: DECKDLL 0x1002dfd0
static void
fill_stats_window(HDC hdc, RECT r, HFONT font)
{
  SIZE sz;
  int stepx;
  int stepy;
  int posx;
  int starty;

  build_deck_stats_table();
  SetMapMode(hdc, MM_ANISOTROPIC);
  SetWindowExtEx(hdc, 1000, 750, NULL);
  SetViewportExtEx(hdc, r.right - r.left, r.bottom - r.top, NULL);
  SelectObject(hdc, font);
  SetBkMode(hdc, TRANSPARENT);

  GetTextExtentPoint32(hdc, "Enchantments", strlen("Enchantments"), &sz);

  show_stats(hdc, sz.cx, sz.cy, &stepx, &stepy, &posx, &starty);
  show_stats_values(hdc, sz.cx, sz.cy, stepx, stepy, posx, starty);
}

// FUNCTION: DECKDLL 0x1002db30
INT_PTR CALLBACK
dlgproc_DeckStats(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  HDC hdc;
  HDC chdc;
  RECT r;
  BITMAP bmp;
  char txt[264];
  int x;
  int y;
  unsigned int decktype;
  static const LOGFONT stats_logfont_template = {
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

  switch (msg)
  {
  case WM_INITDIALOG:
    load_text("menus", "STATSDIALOG");
    strcpy(txt, text_lines[0]);
    strcat(txt, ": ");
    strcat(txt, global_deckname);
    strcat(txt, "  - ");
    decktype = check_deck_type();
    load_text("MP_UIStrings.txt", "SHELLPAGE_MULTIDUEL");
    if (decktype & DT_UNRESTRICTED)
      strcat(txt, text_lines[1]);
    else if (decktype & DT_WILD)
      strcat(txt, text_lines[2]);
    else if (decktype & DT_RESTRICTED_T1)
      strcat(txt, text_lines[3]);
    else if (decktype & DT_TOURNAMENT_T1_5)
      strcat(txt, text_lines[4]);
    else if (decktype & DT_HIGHLANDER)
      strcat(txt, text_lines[5]);
    else
      strcat(txt, "Unknown");

    if (decktype & DT_HAS_ANTE)
    {
      strcat(txt, " / ");
      strcat(txt, text_lines[7] + 1);
    }

    SetWindowTextA(hdlg, txt);
    sprintf(txt, "%s\\GAUN_Options.pic", global_base_directory);
    global_stats_pic = (HANDLE)load_pic(txt);
    memcpy(&global_stats_font_desc, &stats_logfont_template, sizeof(LOGFONT));
    global_stats_font_desc.lfHeight = 30;
    strcpy(global_stats_font_desc.lfFaceName, "Cheltenham ITC Bold BT");
    global_stats_font = CreateFontIndirectA(&global_stats_font_desc);
    global_colorref_stats_lavender = PALETTERGB(0xae, 0xb2, 0xef);
    global_colorref_stats_lightgrey = PALETTERGB(0xf7, 0xf7, 0xf6);
    global_colorref_stats_flesh = PALETTERGB(0xcd, 0xb0, 0x8f);
    return 0;

  case WM_ERASEBKGND:
    hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(hdc);
    GetClientRect(hdlg, &r);
    chdc = CreateCompatibleDC(hdc);
    ApplyCardArtPaletteToDc(chdc);
    SelectObject(chdc, global_stats_pic);
    GetObjectA(global_stats_pic, sizeof(BITMAP), &bmp);
    for (x = 0; x < r.right; x += bmp.bmWidth)
      for (y = 0; y < r.bottom; y += bmp.bmHeight)
        BitBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, chdc, 0, 0, SRCCOPY);

    fill_stats_window(hdc, r, global_stats_font);
    DeleteDC(chdc);
    return 1;

  case WM_COMMAND:
    if (LOWORD(wparam) == RES_BUTTON_OK || LOWORD(wparam) == RES_BUTTON_CANCEL)
    {
      DeleteObject(global_stats_font);
      if (global_stats_pic)
        DeleteObject(global_stats_pic);
      EndDialog(hdlg, 0);
    }
    return 1;

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    hdc = (HDC)wparam;
    ApplyCardArtPaletteToDc(hdc);
    (void)lparam;
    SetBkMode(hdc, TRANSPARENT);
    return (INT_PTR)GetStockObject(HOLLOW_BRUSH);
  }

  return 0;
}
