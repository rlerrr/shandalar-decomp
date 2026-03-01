#include <stddef.h>
#include <windows.h>
#include "mystdbool.h"
#include "defs.h"
#include "deckdll.h"
#include "resources.h"
#include "cardartlib/src/palette.h"

typedef ptrdiff_t INT_PTR;

extern char text_lines[500][128];
extern HANDLE *global_pics;

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

// FUNCTION: DECKDLL 0x1002e096
static void
show_stats(HDC hdc, int word_width, int word_height)
{
  enum Stat1
  {
    STAT1_MANASOURCE = 0,
    STAT1_CREATURE = 1,
    STAT1_ENCHANTMENT = 2,
    STAT1_SORCERY = 3,
    STAT1_INSTANT = 4,
    STAT1_INTERRUPT = 5,
    STAT1_LAND = 6,
    STAT1_ARTIFACT = 7,
    STAT1_TOTAL = 8,

    STAT1_0 = 0,
    STAT1_MAX = STAT1_TOTAL,
  };

  enum Stat2
  {
    STAT2_BLACK = 0,
    STAT2_BLUE = 1,
    STAT2_GREEN = 2,
    STAT2_RED = 3,
    STAT2_WHITE = 4,
    STAT2_COLORLESS = 5,
    STAT2_MULTI = 6,
    STAT2_TOTAL = STAT2_MULTI,

    STAT2_0 = 0,
    STAT2_MAX = STAT2_TOTAL,
  };

  int stats[STAT1_MAX + 1][STAT2_MAX + 1];
  int i;
  int j;
  int c;
  int l;
  int stepx;
  int stepy;
  int x;
  int y;
  int posx;
  int textline;
  int div;

  memset(stats, 0, sizeof stats);

  for (i = 0; i < global_edited_deck_num_entries; ++i)
  {
    csvid_t csvid = global_edited_deck[i].DeckEntry_csvid;
    int amt = global_edited_deck[i].DeckEntry_Amount;
    const card_ptr_t *cp = &cards_ptr[csvid];

    int stat1;
    int stat2;
    switch (cp->card_type)
    {
    case CP_TYPE_CREATURE:
      stat1 = STAT1_CREATURE;
      break;
    case CP_TYPE_ENCHANTMENT:
      stat1 = STAT1_ENCHANTMENT;
      break;
    case CP_TYPE_SORCERY:
      stat1 = STAT1_SORCERY;
      break;
    case CP_TYPE_INSTANT:
      stat1 = STAT1_INSTANT;
      break;
    case CP_TYPE_INTERRUPT:
      stat1 = STAT1_INTERRUPT;
      break;
    case CP_TYPE_LAND:
      stat1 = STAT1_LAND;
      break;
    case CP_TYPE_ARTIFACT:
      stat1 = STAT1_ARTIFACT;
      for (j = 0; j < 7; ++j)
        if (cp->types[j] == SUBTYPE_CREATURE)
        {
          stat1 = STAT1_CREATURE;
          break;
        }
      break;
    default:
      continue;
    }

    if (cp->mana_source_colors & (COLOR_TEST_ANY_COLORED | COLOR_TEST_COLORLESS | COLOR_TEST_ARTIFACT))
    {
      int add[STAT2_MAX + 1] = {0};
      if (cp->mana_source_colors & (COLOR_TEST_COLORLESS | COLOR_TEST_ARTIFACT))
        add[STAT2_COLORLESS] += amt;
      if (cp->mana_source_colors & COLOR_TEST_BLACK)
        add[STAT2_BLACK] += amt;
      if (cp->mana_source_colors & COLOR_TEST_BLUE)
        add[STAT2_BLUE] += amt;
      if (cp->mana_source_colors & COLOR_TEST_GREEN)
        add[STAT2_GREEN] += amt;
      if (cp->mana_source_colors & COLOR_TEST_RED)
        add[STAT2_RED] += amt;
      if (cp->mana_source_colors & COLOR_TEST_WHITE)
        add[STAT2_WHITE] += amt;

      add[STAT2_TOTAL] += amt;

      for (j = STAT2_0; j <= STAT2_MAX; ++j)
        stats[STAT1_MANASOURCE][j] += add[j];

      if (stat1 == STAT1_LAND)
      {
        for (j = STAT2_0; j <= STAT2_MAX; ++j)
          stats[STAT1_LAND][j] += add[j];
        continue;
      }
    }

    switch (cp->color)
    {
    case CP_COLOR_BLACK:
      stat2 = STAT2_BLACK;
      break;
    case CP_COLOR_BLUE:
      stat2 = STAT2_BLUE;
      break;
    case CP_COLOR_GREEN:
      stat2 = STAT2_GREEN;
      break;
    case CP_COLOR_RED:
      stat2 = STAT2_RED;
      break;
    case CP_COLOR_WHITE:
      stat2 = STAT2_WHITE;
      break;
    case CP_COLOR_MULTI:
      stat2 = STAT2_MULTI;
      if (cp->req.req_black || (cp->req.req_hybrid && (cp->req.hybrid_type & COLOR_TEST_BLACK)))
        stats[stat1][STAT2_BLACK] += amt;
      if (cp->req.req_blue || (cp->req.req_hybrid && (cp->req.hybrid_type & COLOR_TEST_BLUE)))
        stats[stat1][STAT2_BLUE] += amt;
      if (cp->req.req_green || (cp->req.req_hybrid && (cp->req.hybrid_type & COLOR_TEST_GREEN)))
        stats[stat1][STAT2_GREEN] += amt;
      if (cp->req.req_red || (cp->req.req_hybrid && (cp->req.hybrid_type & COLOR_TEST_RED)))
        stats[stat1][STAT2_RED] += amt;
      if (cp->req.req_white || (cp->req.req_hybrid && (cp->req.hybrid_type & COLOR_TEST_WHITE)))
        stats[stat1][STAT2_WHITE] += amt;
      break;
    default:
      stat2 = STAT2_COLORLESS;
      break;
    }
    if (stat2 != STAT2_MULTI)
      stats[stat1][stat2] += amt;

    stats[stat1][STAT2_TOTAL] += amt;
  }

  for (i = STAT1_0 + 1; i < STAT1_MAX; ++i)
    for (j = STAT2_0; j <= STAT2_MAX; ++j)
      stats[STAT1_TOTAL][j] += stats[i][j];

  load_text("Menus", "STATSSCREEN");
  SetTextColor(hdc, global_colorref_lightgrey);

  // Column headers
  x = 40;
  y = 40;
  textout(hdc, x, y, text_lines[0]);

  x += word_width;
  stepx = (1000 - x) / 8;
  x += 80;
  posx = x;
  for (c = 1; c <= 7; ++c)
  {
    textout(hdc, x, y, text_lines[c]);
    x += stepx;
  }

  stepy = 65;
  y = 105;
  for (l = STAT1_0; l <= STAT1_TOTAL; ++l, y += stepy)
  {
    // Row header
    x = 40;
    textline = l == STAT1_TOTAL ? 7 : (8 + l);
    SetTextColor(hdc, l == STAT1_MANASOURCE ? global_colorref_lavender : global_colorref_lightgrey);
    textout(hdc, x, y, text_lines[textline]);

    // Extra word below "Non-creature".
    if (l == STAT1_ARTIFACT)
      textout(hdc, x, y + word_height, text_lines[textline + 1]);

    // Values
    x = posx;
    if (l != STAT1_MANASOURCE)
      SetTextColor(hdc, global_colorref_flesh);
    for (c = STAT2_0; c <= STAT2_TOTAL; ++c, x += stepx)
      if (stats[l][c] == 0)
        textout(hdc, x, y, " " DASH);
      else
      {
        div = 0;
        if (c == STAT2_TOTAL)
          div = stats[STAT1_TOTAL][STAT2_TOTAL];
        else
          div = stats[l][STAT2_TOTAL];

        if (div)
          div = (100 * stats[l][c] + div / 2) / div;

        textoutf(hdc, x, y, "(%d) %d%%", stats[l][c], div);
      }

    // Extra space below "Mana Sources"
    if (l == STAT1_MANASOURCE)
      y += word_height / 2;

    // Extra space below "Non-creature".
    if (l == STAT1_ARTIFACT)
      y += word_height;
  }
}

// FUNCTION: DECKDLL 0x1002dfd0
static void
fill_stats_window(HDC hdc, RECT r, HFONT font)
{
  SIZE sz;

  SetMapMode(hdc, MM_ANISOTROPIC);
  SetWindowExtEx(hdc, 1000, 750, NULL);
  SetViewportExtEx(hdc, r.right - r.left, r.bottom - r.top, NULL);
  SelectObject(hdc, font);
  SetBkMode(hdc, TRANSPARENT);

  GetTextExtentPoint32(hdc, "Enchantments", strlen("Enchantments"), &sz);

  show_stats(hdc, sz.cx, sz.cy);
}

// FUNCTION: DECKDLL 0x1002db30
INT_PTR CALLBACK
dlgproc_DeckStats(HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
  int x;
  int y;
  HDC hdc;
  HDC chdc;
  RECT r;
  BITMAP bmp;
  switch (msg)
  {
  case WM_INITDIALOG:
  {
    char txt[512]; // 128 + 2 + 32 + 128 + 3 + 127 = 420, and might as well be a bit wider just in case
    char *p;
    DeckType decktype;
    const char *decktxt;

    load_text("Menus", "STATSDIALOG");
    p = txt + sprintf(txt, "%s: %s " DASH " ", text_lines[0], global_deckname);

    load_text("Menus", "MULTIDECKTYPES");
    decktype = check_deck_type();
    if (decktype & DT_UNRESTRICTED)
      decktxt = text_lines[1];
    else if (decktype & DT_WILD)
      decktxt = text_lines[2];
    else if (decktype & DT_RESTRICTED_T1)
      decktxt = text_lines[3];
    else if (decktype & DT_TOURNAMENT_T1_5)
      decktxt = text_lines[4];
    else if (decktype & DT_HIGHLANDER)
      decktxt = text_lines[5];
    else
      decktxt = text_lines[0];

    p += sprintf(p, "%s", decktxt);

    if (decktype & DT_HAS_ANTE)
      p += sprintf(p, " / %s", &text_lines[7][1]);

    SetWindowText(hdlg, txt);

    return 0;
  }

  case WM_ERASEBKGND:
  {
    hdc = wparam;
    ApplyCardArtPaletteToDc(hdc);

    chdc = CreateCompatibleDC(hdc);
    ApplyCardArtPaletteToDc(chdc);

    SelectObject(chdc, global_pics[33]);

    GetClientRect(hdlg, &r);

    GetObject(global_pics[33], sizeof(BITMAP), &bmp);

    for (x = 0; x < r.right; x += bmp.bmWidth)
      for (y = 0; y < r.bottom; y += bmp.bmHeight)
        BitBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, chdc, 0, 0, SRCCOPY);

    fill_stats_window(hdc, r, global_font_32);

    DeleteDC(chdc);

    return 1;
  }

  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC:
    hdc = wparam;
    ApplyCardArtPaletteToDc(hdc);
    SetBkMode(hdc, TRANSPARENT);
    return GetStockObject(HOLLOW_BRUSH);

  case WM_COMMAND:
  {
    if (LOWORD(wparam) == RES_BUTTON_OK || LOWORD(wparam) == RES_BUTTON_CANCEL)
      EndDialog(hdlg, 0);
    return 1;
  }

  default:
    return 0;
  }
}
