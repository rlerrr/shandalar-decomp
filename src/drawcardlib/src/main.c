#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include "mystdbool.h"
#include "defs.h"
#include "pcxw.h"
#include "pic.h"
#include "cardartlib/CardArtLib.h"
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "cardartlib/src/catalog.h"
#include "cardartlib/src/haar.h"

/* Function prototypes (auto-generated). */
#include "autoproto.h"

#pragma intrinsic(memset)

typedef unsigned short ushort;
typedef BOOL(WINAPI code)(HINSTANCE, DWORD, LPVOID);

#define CONCAT12(x, y) ((undefined4)(((undefined1)(x)) | ((undefined4)(undefined2)(y) << 8)))

// GLOBAL: DRAWCARDLIB 0x10021fe0
LOGFONTA global_template_font = {0, 0, 0, 0, 400};

// GLOBAL: DRAWCARDLIB 0x1003a048
LOGFONTA global_temp_font;

// GLOBAL: DRAWCARDLIB 0x1003a090
char global_dual_dat_filename[260];

// GLOBAL: DRAWCARDLIB 0x100f36c0
char global_cardart_directory[0x105];

// GLOBAL: DRAWCARDLIB 0x100223ac
undefined4 g_emptyStringStorage = 0;

// GLOBAL: DRAWCARDLIB 0x10022504
undefined4 global_dwPlatformId = 0xFFFFFFFF;

// GLOBAL: DRAWCARDLIB 0x10039fc8
HBITMAP global_CARDBK_LegendsLand;

// GLOBAL: DRAWCARDLIB 0x10039fcc
HBITMAP global_CARDBK_Green;

// GLOBAL: DRAWCARDLIB 0x10039fd0
HBITMAP global_CARDBK_FallenEmpiresLand;

// GLOBAL: DRAWCARDLIB 0x10039fd4
HBITMAP global_CARDBK_White;

// GLOBAL: DRAWCARDLIB 0x10039fd8
HBITMAP global_CARDBK_GreenLand;

// GLOBAL: DRAWCARDLIB 0x10039fdc
HFONT g_bigCardSubtitleFont;

// GLOBAL: DRAWCARDLIB 0x10039fe0
COLORREF g_smallCardTitleColor;

// GLOBAL: DRAWCARDLIB 0x10039fe4
HBITMAP global_CardSets;

// GLOBAL: DRAWCARDLIB 0x10039fe8
HBITMAP global_CARDBK_BlackLand;

// GLOBAL: DRAWCARDLIB 0x10039fec
COLORREF g_cardTextShadowColor;

// GLOBAL: DRAWCARDLIB 0x10039ff0
HBITMAP global_CARDBK_WhiteLand;

// GLOBAL: DRAWCARDLIB 0x10039ff4
HBITMAP global_CARDBK_Gold;

// GLOBAL: DRAWCARDLIB 0x10039ff8
HFONT g_bigCardPowerToughnessFont;

// GLOBAL: DRAWCARDLIB 0x10039ffc
COLORREF g_defaultCardFrameColor;

// GLOBAL: DRAWCARDLIB 0x1003a000
HFONT g_bigCardTextFont;

// GLOBAL: DRAWCARDLIB 0x1003a004
HBITMAP global_CARDBK_BlueLand;

// GLOBAL: DRAWCARDLIB 0x1003a008
HFONT g_bigCardTitleFont;

// GLOBAL: DRAWCARDLIB 0x1003a00c
HBITMAP global_CARDBK_RedLand;

// GLOBAL: DRAWCARDLIB 0x1003a010
HBITMAP global_CardBack;

// GLOBAL: DRAWCARDLIB 0x1003a014
HBITMAP global_CARDBK_DarklandsLand;

// GLOBAL: DRAWCARDLIB 0x1003a018
HFONT g_smallCardTitleFont;

// GLOBAL: DRAWCARDLIB 0x1003a01c
HPEN g_cardBorderPen;

// GLOBAL: DRAWCARDLIB 0x1003a020
HBITMAP global_CARDBK_Special;

// GLOBAL: DRAWCARDLIB 0x1003a024
HBITMAP global_CARDBK_Artifact;

// GLOBAL: DRAWCARDLIB 0x1003a028
HBITMAP global_CARDBK_ArabianNightsLand;

// GLOBAL: DRAWCARDLIB 0x1003a02c
COLORREF g_alternateCardFrameColor;

// GLOBAL: DRAWCARDLIB 0x1003a030
HBRUSH g_defaultCardFrameBrush;

// GLOBAL: DRAWCARDLIB 0x1003a034
HBITMAP global_CARDBK_Blue;

// GLOBAL: DRAWCARDLIB 0x1003a038
HBRUSH g_alternateCardFrameBrush;

// GLOBAL: DRAWCARDLIB 0x1003a03c
HBITMAP global_CARDBK_Red;

// GLOBAL: DRAWCARDLIB 0x1003a040
HFONT g_bigCardTextItalicFont;

// GLOBAL: DRAWCARDLIB 0x1003a084
HBITMAP global_CARDBK_AntiquitiesLand;

// GLOBAL: DRAWCARDLIB 0x1003a088
HBITMAP global_CARDBK_Black;

// GLOBAL: DRAWCARDLIB 0x1003a08c
COLORREF g_smallCardTitleAltColor;

// GLOBAL: DRAWCARDLIB 0x1003a198
HBITMAP global_ManaSymbols;

// GLOBAL: DRAWCARDLIB 0x1003a19c
COLORREF g_smallCardTitleFallbackColor;

// GLOBAL: DRAWCARDLIB 0x1003a1a0
COLORREF g_rulesTextColor;

// GLOBAL: DRAWCARDLIB 0x1003a1a4
COLORREF g_cardBorderColor;

// GLOBAL: DRAWCARDLIB 0x100f37d0
char g_cardNames[100 * 1024];

// GLOBAL: DRAWCARDLIB 0x100f3833
char g_cardNameSlotTerminators[100 * 1024];

// GLOBAL: DRAWCARDLIB 0x10124510
HINSTANCE g_drawCardLibInstance;

// GLOBAL: DRAWCARDLIB 0x1012655c
COLORREF g_smallCardTitleBackgroundColor;

// FUNCTION: DRAWCARDLIB 0x10004970
undefined4 prepare_fonts_and_imgs(void)
{
  CHAR resourcePath[264];

  strcpy(global_dual_dat_filename, global_base_directory);
  strcat(global_dual_dat_filename, "\\DuelArt");
  strcat(global_dual_dat_filename, "\\Duel.dat");
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0530m_.TTF");
  AddFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Magis___.TTF");
  AddFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0127m_.TTF");
  AddFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0085m_.TTF");
  AddFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0298m_.TTF");
  AddFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0299m_.TTF");
  AddFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0300m_.TTF");
  AddFontResourceA(resourcePath);
  sprintf(resourcePath, "%s\\ManaSymbols.pic", &global_cardart_directory);
  global_ManaSymbols = load_pic(resourcePath);
  sprintf(resourcePath, "%s\\CardSets.pic", &global_cardart_directory);
  global_CardSets = load_pic(resourcePath);
  sprintf(resourcePath, "%s\\CardBack.pic", &global_cardart_directory);
  global_CardBack = load_pic(resourcePath);

  g_bigCardTitleFont = CreateFontIndirectA(LoadFontFromIni("BigCardTitle", 0));
  g_bigCardSubtitleFont = CreateFontIndirectA(LoadFontFromIni("BigCardSubtitle", 0));
  g_bigCardPowerToughnessFont = CreateFontIndirectA(LoadFontFromIni("BigCardPT", 0));
  g_bigCardTextFont = CreateFontIndirectA(LoadFontFromIni("BigCardText", 0));
  g_bigCardTextItalicFont = CreateFontIndirectA(LoadFontFromIni("BigCardText", 1));
  g_smallCardTitleFont = CreateFontIndirectA(LoadFontFromIni("SmallCardTitle", 0));
  g_defaultCardFrameColor = GetPaletteColor(0xbe);
  g_alternateCardFrameColor = GetPaletteColor(0xca);
  g_cardTextShadowColor = GetPaletteColor(0xc9);
  g_smallCardTitleColor = GetPaletteColor(0xbf);
  g_smallCardTitleAltColor = GetPaletteColor(0xd8);
  g_smallCardTitleFallbackColor = GetPaletteColor(0x31);
  g_rulesTextColor = GetPaletteColor(0xc9);
  g_smallCardTitleBackgroundColor = GetPaletteColor(0xc4);
  g_cardBorderColor = GetPaletteColor(0xc4);
  g_cardBorderPen = CreatePen(0, 0, g_cardBorderColor);
  g_defaultCardFrameBrush = CreateSolidBrush(g_defaultCardFrameColor);
  g_alternateCardFrameBrush = CreateSolidBrush(g_alternateCardFrameColor);
  global_CARDBK_Artifact = 0;
  global_CARDBK_Gold = global_CARDBK_Artifact;
  global_CARDBK_Red = global_CARDBK_Gold;
  global_CARDBK_Black = global_CARDBK_Red;
  global_CARDBK_Blue = global_CARDBK_Black;
  global_CARDBK_White = global_CARDBK_Blue;
  global_CARDBK_Green = global_CARDBK_White;
  global_CARDBK_ArabianNightsLand = 0;
  global_CARDBK_LegendsLand = global_CARDBK_ArabianNightsLand;
  global_CARDBK_AntiquitiesLand = global_CARDBK_LegendsLand;
  global_CARDBK_FallenEmpiresLand = global_CARDBK_AntiquitiesLand;
  global_CARDBK_DarklandsLand = global_CARDBK_FallenEmpiresLand;
  global_CARDBK_RedLand = global_CARDBK_DarklandsLand;
  global_CARDBK_BlackLand = global_CARDBK_RedLand;
  global_CARDBK_BlueLand = global_CARDBK_BlackLand;
  global_CARDBK_WhiteLand = global_CARDBK_BlueLand;
  global_CARDBK_GreenLand = global_CARDBK_WhiteLand;
  global_CARDBK_Special = 0;
  if ((((((global_ManaSymbols == (HBITMAP)0x0) || (global_CardSets == (HBITMAP)0x0)) || (global_CardBack == (HBITMAP)0x0)) ||
        ((g_bigCardTitleFont == (HFONT)0x0 || (g_bigCardSubtitleFont == (HFONT)0x0)))) ||
       ((g_bigCardTextFont == (HFONT)0x0 || ((g_bigCardTextItalicFont == (HFONT)0x0 || (g_smallCardTitleFont == (HFONT)0x0)))))) ||
      (((HPEN)g_cardBorderPen == (HPEN)0x0 ||
        (((HBRUSH)g_defaultCardFrameBrush == (HBRUSH)0x0 || ((HBRUSH)g_alternateCardFrameBrush == (HBRUSH)0x0))))))
  {
    DestroyAllResources();
    return 0;
  }

  return 1;
}

// FUNCTION: DRAWCARDLIB 0x10004e82
LOGFONTA *LoadFontFromIni(char *fontKeySuffix, int italic)
{
  CHAR key[100];
  UINT boldSetting;

  memcpy(&global_temp_font, &global_template_font, sizeof(LOGFONTA));
  strcpy(key, "size");
  strcat(key, fontKeySuffix);
  global_temp_font.lfHeight = GetPrivateProfileIntA("Fonts", key, 0x14, global_dual_dat_filename);

  strcpy(key, "bold");
  strcat(key, fontKeySuffix);
  boldSetting = GetPrivateProfileIntA("Fonts", key, 0, global_dual_dat_filename);
  if (boldSetting != 0)
  {
    global_temp_font.lfWeight = FW_BOLD;
  }
  if (italic != 0)
  {
    global_temp_font.lfItalic = TRUE;
  }

  strcpy(key, "font");
  strcat(key, fontKeySuffix);
  GetPrivateProfileStringA("Fonts", key, "MS Sans Serif", global_temp_font.lfFaceName, 0x20, global_dual_dat_filename);
  return &global_temp_font;
}

void __inline DestroyAllCardBackgrounds_impl(void)
{
  if (global_CARDBK_Green != 0)
  {
    DeleteAndCloseObject(global_CARDBK_Green);
    global_CARDBK_Green = 0;
  }
  if (global_CARDBK_White != 0)
  {
    DeleteAndCloseObject(global_CARDBK_White);
    global_CARDBK_White = 0;
  }
  if (global_CARDBK_Blue != 0)
  {
    DeleteAndCloseObject(global_CARDBK_Blue);
    global_CARDBK_Blue = 0;
  }
  if (global_CARDBK_Black != 0)
  {
    DeleteAndCloseObject(global_CARDBK_Black);
    global_CARDBK_Black = 0;
  }
  if (global_CARDBK_Red != 0)
  {
    DeleteAndCloseObject(global_CARDBK_Red);
    global_CARDBK_Red = 0;
  }
  if (global_CARDBK_Gold != 0)
  {
    DeleteAndCloseObject(global_CARDBK_Gold);
    global_CARDBK_Gold = 0;
  }
  if (global_CARDBK_Artifact != 0)
  {
    DeleteAndCloseObject(global_CARDBK_Artifact);
    global_CARDBK_Artifact = 0;
  }
  if (global_CARDBK_GreenLand != 0)
  {
    DeleteAndCloseObject(global_CARDBK_GreenLand);
    global_CARDBK_GreenLand = 0;
  }
  if (global_CARDBK_WhiteLand != 0)
  {
    DeleteAndCloseObject(global_CARDBK_WhiteLand);
    global_CARDBK_WhiteLand = 0;
  }
  if (global_CARDBK_BlueLand != 0)
  {
    DeleteAndCloseObject(global_CARDBK_BlueLand);
    global_CARDBK_BlueLand = 0;
  }
  if (global_CARDBK_BlackLand != 0)
  {
    DeleteAndCloseObject(global_CARDBK_BlackLand);
    global_CARDBK_BlackLand = 0;
  }
  if (global_CARDBK_RedLand != 0)
  {
    DeleteAndCloseObject(global_CARDBK_RedLand);
    global_CARDBK_RedLand = 0;
  }
  if (global_CARDBK_DarklandsLand != 0)
  {
    DeleteAndCloseObject(global_CARDBK_DarklandsLand);
    global_CARDBK_DarklandsLand = 0;
  }
  if (global_CARDBK_FallenEmpiresLand != 0)
  {
    DeleteAndCloseObject(global_CARDBK_FallenEmpiresLand);
    global_CARDBK_FallenEmpiresLand = 0;
  }
  if (global_CARDBK_AntiquitiesLand != 0)
  {
    DeleteAndCloseObject(global_CARDBK_AntiquitiesLand);
    global_CARDBK_AntiquitiesLand = 0;
  }
  if (global_CARDBK_LegendsLand != 0)
  {
    DeleteAndCloseObject(global_CARDBK_LegendsLand);
    global_CARDBK_LegendsLand = 0;
  }
  if (global_CARDBK_ArabianNightsLand != 0)
  {
    DeleteAndCloseObject(global_CARDBK_ArabianNightsLand);
    global_CARDBK_ArabianNightsLand = 0;
  }
  if (global_CARDBK_Special != 0)
  {
    DeleteAndCloseObject(global_CARDBK_Special);
    global_CARDBK_Special = 0;
  }
}

// FUNCTION: DRAWCARDLIB 0x10004f8e
void DestroyAllResources(void)
{
  CHAR resourcePath[264];

  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Magim___.TTF");
  RemoveFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0530m_.TTF");
  RemoveFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Magis___.TTF");
  RemoveFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0127m_.TTF");
  RemoveFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0085m_.TTF");
  RemoveFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0298m_.TTF");
  RemoveFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0299m_.TTF");
  RemoveFontResourceA(resourcePath);
  strcpy(resourcePath, global_base_directory);
  strcat(resourcePath, "\\Tt0300m_.TTF");
  RemoveFontResourceA(resourcePath);
  if (global_ManaSymbols != 0)
  {
    DeleteAndCloseObject(global_ManaSymbols);
    global_ManaSymbols = 0;
  }
  if (global_CardSets != 0)
  {
    DeleteAndCloseObject(global_CardSets);
    global_CardSets = 0;
  }
  if (global_CardBack != 0)
  {
    DeleteAndCloseObject(global_CardBack);
    global_CardBack = 0;
  }

  DestroyAllCardBackgrounds_impl();

  if (g_bigCardTitleFont != (HGDIOBJ)0x0)
  {
    DeleteObject(g_bigCardTitleFont);
  }
  g_bigCardTitleFont = (HGDIOBJ)0x0;
  if (g_bigCardSubtitleFont != (HGDIOBJ)0x0)
  {
    DeleteObject(g_bigCardSubtitleFont);
  }
  g_bigCardSubtitleFont = (HGDIOBJ)0x0;
  if (g_bigCardPowerToughnessFont != (HGDIOBJ)0x0)
  {
    DeleteObject(g_bigCardPowerToughnessFont);
  }
  g_bigCardPowerToughnessFont = (HGDIOBJ)0x0;
  if (g_bigCardTextFont != (HGDIOBJ)0x0)
  {
    DeleteObject(g_bigCardTextFont);
  }
  g_bigCardTextFont = (HGDIOBJ)0x0;
  if (g_bigCardTextItalicFont != (HGDIOBJ)0x0)
  {
    DeleteObject(g_bigCardTextItalicFont);
  }
  g_bigCardTextItalicFont = (HGDIOBJ)0x0;
  if (g_smallCardTitleFont != (HGDIOBJ)0x0)
  {
    DeleteObject(g_smallCardTitleFont);
  }
  g_smallCardTitleFont = (HGDIOBJ)0x0;
  if (g_cardBorderPen != (HPEN)0x0)
  {
    DeleteObject(g_cardBorderPen);
  }
  g_cardBorderPen = (HPEN)0x0;
  if (g_defaultCardFrameBrush != (HBRUSH)0x0)
  {
    DeleteObject(g_defaultCardFrameBrush);
  }
  g_defaultCardFrameBrush = (HBRUSH)0x0;
  if (g_alternateCardFrameBrush != (HBRUSH)0x0)
  {
    DeleteObject(g_alternateCardFrameBrush);
  }
  g_alternateCardFrameBrush = (HBRUSH)0x0;
}

// FUNCTION: DRAWCARDLIB 0x1000558b
void DestroyAllCardBackgrounds(void)
{
  DestroyAllCardBackgrounds_impl();
}

// FUNCTION: DRAWCARDLIB 0x10005830
void DrawCardBack(HDC dc, RECT *rect)
{
  struct
  {
    int xInset;
    int yInset;
    RECT innerRect;
  } s;

  if (dc == NULL || rect == NULL)
  {
    return;
  }

  if (global_CardBack != 0)
  {
    s.xInset = max(((rect->right - rect->left) * 3) / 100, 1);
    s.yInset = max(((rect->bottom - rect->top) * 2) / 100, 1);

    FillRect(dc, rect, GetStockObject(4));
    SetRect(&s.innerRect, rect->left + s.xInset, rect->top + s.yInset, rect->right - s.xInset,
            rect->bottom - s.yInset);
    DrawBitmapToRect(dc, &s.innerRect, global_CardBack);
  }
  else
  {
    FillRect(dc, rect, GetStockObject(4));
  }
}

// FUNCTION: DRAWCARDLIB 0x1000592c
undefined4 DrawFullCard(HDC dc, RECT *rect, card_ptr_t *card, undefined4 version, uint drawFlags, bool expanded_text, LPCSTR artistFormat)
{
  struct
  {
    int backgroundCropLeftPermille;
    int backgroundCropTopPermille;
    BITMAP backgroundBitmapInfo;
    int rulesBackgroundTopInset;
    int backgroundCropHeightPermille;
    int backgroundCropWidthPermille;
    TEXTMETRICA textMetrics;
    RECT flavorRect;
    int textBlockHeight;
    uint manaTextHeight;
    RECT artDeviceRect;
    LPCSTR typeText;
    int defaultPowerToughness;
    HBITMAP *backgroundPointer;
    RECT cardBackgroundRect;
    HBRUSH frameBrush;
    RECT artRect;
    RECT powerToughnessRect;
    int shadowOffset;
    COLORREF textColor;
    char rules_text[1000];
    int cardWidth;
    RECT artistRect;
    char tempText[52];
    RECT rulesBackgroundRect;
    undefined4 result;
    HANDLE backgroundBitmap;
    RECT typeLineRect;
    RECT rulesTextRect;
    int cornerWidth;
    RECT nameRect;
    int nSavedDC;
    int cardHeight;
    int cornerHeight;
    RECT castingCostRect;
    RECT setIconRect;
  } s;

  if (((dc == (HDC)0x0) || (rect == (RECT *)0x0)) || (card == (card_ptr_t *)0x0))
  {
    return 0;
  }

  s.nSavedDC = SaveDC(dc);
  s.cardWidth = 200;
  s.cardHeight = 300;
  SetMapMode(dc, 7);
  SetWindowExtEx(dc, s.cardWidth, s.cardHeight, (LPSIZE)0x0);
  SetViewportExtEx(dc, rect->right - rect->left, rect->bottom - rect->top, (LPSIZE)0x0);
  SetWindowOrgEx(dc, s.cardWidth / 2, s.cardHeight / 2, (LPPOINT)0x0);
  SetViewportOrgEx(dc, rect->left + (rect->right - rect->left) / 2,
                   rect->top + (rect->bottom - rect->top) / 2, (LPPOINT)0x0);
  s.cornerWidth = 6;
  s.cornerHeight = 6;
  SetRect(&s.nameRect, 10, 5, 0xbc, 0x15);
  SetRect(&s.castingCostRect, 0xc, 8, 0xbe, 0x14);
  SetRect(&s.artRect, 0x15, 0x19, 0xb5, 0xa4);
  SetRect(&s.typeLineRect, 0xc, 0xa8, 0xba, 0xb3);
  SetRect(&s.setIconRect, 0xc, 0xa9, 0xba, 0xb4);
  SetRect(&s.rulesTextRect, 0x1c, 0xb9, 0xae, 0x10c);
  SetRect(&s.rulesBackgroundRect, 0x14, 0xb4, 0xb6, 0x10f);
  SetRect(&s.artistRect, 0xc, 0x114, 0xbc, 0x124);
  SetRect(&s.powerToughnessRect, 0xc, 0x114, 0xbc, 0x124);
  if (card->expansion == -1 || (card->expansion & 0x10) != 0 || (card->expansion & 0x80) != 0)
  {
    s.frameBrush = CreateSolidBrush(g_defaultCardFrameColor);
    SelectObject(dc, s.frameBrush);
  }
  else
  {
    s.frameBrush = CreateSolidBrush(g_alternateCardFrameColor);
    SelectObject(dc, s.frameBrush);
  }
  SelectObject(dc, GetStockObject(8));
  RoundRect(dc, 0, 0, s.cardWidth, s.cardHeight, s.cornerWidth / 2, s.cornerHeight / 2);
  SelectObject(dc, GetStockObject(4));
  if (s.frameBrush != (HBRUSH)0x0)
  {
    DeleteObject(s.frameBrush);
  }
  s.backgroundPointer = GetCardBackgroundPointer(card);
  LoadCardBackground(s.backgroundPointer);
  SetRect(&s.cardBackgroundRect, s.cornerWidth, s.cornerHeight,
          // Well these are weird
          s.cornerWidth + s.cardWidth - (s.cornerWidth << 1),
          s.cornerHeight + s.cardHeight - (s.cornerHeight << 1));
  if (*s.backgroundPointer != 0)
  {
    DrawBitmapToRect(dc, &s.cardBackgroundRect, *s.backgroundPointer);
  }
  else
  {
    FillRect(dc, &s.cardBackgroundRect, GetStockObject(0));
  }
  s.backgroundBitmap = *s.backgroundPointer;
  s.shadowOffset = 1;
  s.textColor = GetPaletteColor(0xbf);
  SelectObject(dc, g_bigCardTitleFont);
  SetBkMode(dc, 1);
  SetTextColor(dc, g_cardTextShadowColor);
  OffsetRect(&s.nameRect, s.shadowOffset, s.shadowOffset);
  DrawTextA(dc, card->full_name, -1, &s.nameRect, 0x824);
  OffsetRect(&s.nameRect, -s.shadowOffset, -s.shadowOffset);
  SetTextColor(dc, s.textColor);
  DrawTextA(dc, card->full_name, -1, &s.nameRect, 0x824);
  s.defaultPowerToughness = 0x64;
  SelectObject(dc, g_bigCardPowerToughnessFont);
  if ((card->power != 0) || (card->toughness != 0))
  {
    s.tempText[0] = '\0';
    if (card->power == s.defaultPowerToughness)
    {
      strcat(s.tempText, "*");
    }
    else if (s.defaultPowerToughness < card->power)
    {
      wsprintfA((char *)((ptrdiff_t)&s.tempText + strlen(s.tempText)), "%d+*", card->power - s.defaultPowerToughness);
    }
    else
    {
      wsprintfA((char *)((ptrdiff_t)&s.tempText + strlen(s.tempText)), "%d", card->power);
    }
    strcat(s.tempText, "/");
    if (card->toughness == s.defaultPowerToughness)
    {
      strcat(s.tempText, "*");
    }
    else if (s.defaultPowerToughness < card->toughness)
    {
      wsprintfA((char *)((ptrdiff_t)&s.tempText + strlen(s.tempText)), "%d+*", card->toughness - s.defaultPowerToughness);
    }
    else
    {
      wsprintfA((char *)((ptrdiff_t)&s.tempText + strlen(s.tempText)), "%d", card->toughness);
    }
    SetTextColor(dc, g_cardTextShadowColor);
    OffsetRect(&s.powerToughnessRect, s.shadowOffset, s.shadowOffset);
    DrawTextA(dc, s.tempText, -1, &s.powerToughnessRect, 0x26);
    OffsetRect(&s.powerToughnessRect, -s.shadowOffset, -s.shadowOffset);
    SetTextColor(dc, s.textColor);
    DrawTextA(dc, s.tempText, -1, &s.powerToughnessRect, 0x26);
  }
  SelectObject(dc, g_bigCardSubtitleFont);
  SetBkMode(dc, 1);
  if ((card->card_type != -1) && (card->card_type != 0))
  {
    s.typeText = card->type_text;
    SetTextColor(dc, g_cardTextShadowColor);
    OffsetRect(&s.typeLineRect, s.shadowOffset, s.shadowOffset);
    DrawTextA(dc, s.typeText, -1, &s.typeLineRect, 0x24);
    OffsetRect(&s.typeLineRect, -s.shadowOffset, -s.shadowOffset);
    SetTextColor(dc, s.textColor);
    DrawTextA(dc, s.typeText, -1, &s.typeLineRect, 0x24);
  }
  if (card->artist != 0)
  {
    wsprintfA(s.tempText, artistFormat, card->artist);
    SetTextColor(dc, g_cardTextShadowColor);
    OffsetRect(&s.artistRect, s.shadowOffset, s.shadowOffset);
    DrawTextA(dc, s.tempText, -1, &s.artistRect, 0x824);
    OffsetRect(&s.artistRect, -s.shadowOffset, -s.shadowOffset);
    SetTextColor(dc, s.textColor);
    DrawTextA(dc, s.tempText, -1, &s.artistRect, 0x824);
  }
  if (((card->card_type != 5) && (card->card_type != 8)) && (card->card_type != 0))
  {
    DrawCastingCost(dc, &s.castingCostRect, &card->req);
  }
  if (card->expansion != -1)
  {
    DrawCardSet(dc, &s.setIconRect, card->expansion);
  }
  CopyRect(&s.artDeviceRect, &s.artRect);
  LPtoDP(dc, (LPPOINT)&s.artDeviceRect, 2);

  // HMMM
  switch (drawFlags & 0xf)
  {
  case 0:
    if (IsBigArtIn(card->id, version) != 0)
    {
      DrawBigArt(dc, &s.artRect, card->id, version);
    }
    else
    {
      DrawSmallArt(dc, &s.artRect, card->id, version);
    }
    break;

  case 1:
    if (IsBigArtIn(card->id, version) == 0)
    {
      if (((drawFlags & 0x10) != 0) && (IsSmallArtIn(card->id, version) != 0))
      {
        DrawSmallArt(dc, &s.artRect, card->id, version);
      }
      LoadBigArt(card->id, version, s.artDeviceRect.right - s.artDeviceRect.left,
                 s.artDeviceRect.bottom - s.artDeviceRect.top);
    }
    DrawBigArt(dc, &s.artRect, card->id, version);
    break;
  default:
    if (((IsBigArtIn(card->id, version) == 0) && ((drawFlags & 0x10) != 0)) &&
        (IsSmallArtIn(card->id, version) != 0))
    {
      DrawSmallArt(dc, &s.artRect, card->id, version);
    }
    if (LoadBigArt(card->id, version, s.artDeviceRect.right - s.artDeviceRect.left,
                   s.artDeviceRect.bottom - s.artDeviceRect.top) != 0)
    {
      DrawBigArt(dc, &s.artRect, card->id, version);
    }
    else
    {
      DrawSmallArt(dc, &s.artRect, card->id, version);
    }
    break;
  }
  s.result = IsBigArtRightSize(card->id, version, s.artDeviceRect.right - s.artDeviceRect.left,
                               s.artDeviceRect.bottom - s.artDeviceRect.top);
  strcpy(s.rules_text, card->rules_text);
  ReplaceSubstring(s.rules_text, "|H1", 1, "");
  ReplaceSubstring(s.rules_text, "|H2", 1, "");
  ReplaceSubstring(s.rules_text, "|H3", 1, "");
  ReplaceSubstring(s.rules_text, "|H4", 1, "");
  ReplaceSubstring(s.rules_text, "|H", 1, "");
  ReplaceSubstring(s.rules_text, "|S1", 1, "");
  ReplaceSubstring(s.rules_text, "|S2", 1, "");
  ReplaceSubstring(s.rules_text, "|S3", 1, "");
  ReplaceSubstring(s.rules_text, "|S4", 1, "");
  ReplaceSubstring(s.rules_text, "|S", 1, "");
  SetTextColor(dc, g_rulesTextColor);
  SetBkMode(dc, 1);
  if (expanded_text != 0)
  {
    SelectObject(dc, g_bigCardTextFont);
    s.manaTextHeight = (unsigned short)((CalcDrawManaText(dc, &s.rulesTextRect, s.rules_text) >> 0x10) & 0xffff);
    GetTextMetricsA(dc, &s.textMetrics);
    CopyRect(&s.flavorRect, &s.rulesTextRect);
    s.flavorRect.top += s.manaTextHeight;
    s.flavorRect.top += s.textMetrics.tmHeight / 2;
    SelectObject(dc, g_bigCardTextItalicFont);
    DrawTextA(dc, card->flavor_text, -1, &s.flavorRect, 0x410);
    s.textBlockHeight = s.flavorRect.bottom - s.rulesTextRect.top;
    if (s.rulesTextRect.bottom - s.rulesTextRect.top < s.textBlockHeight)
    {
      s.rulesBackgroundTopInset = s.rulesTextRect.top - s.rulesBackgroundRect.top;
      s.rulesTextRect.top = s.rulesTextRect.bottom - s.textBlockHeight;
      s.rulesBackgroundRect.top = s.rulesTextRect.top - s.rulesBackgroundTopInset;
      if (s.backgroundBitmap != 0)
      {
        GetObjectA(s.backgroundBitmap, sizeof(s.backgroundBitmapInfo), &s.backgroundBitmapInfo);
        s.backgroundCropLeftPermille = 0x49;
        s.backgroundCropTopPermille = 0x25d;
        s.backgroundCropWidthPermille = 0x359;
        s.backgroundCropHeightPermille = 0x140;
        DrawBitmapSubrectToRect(dc, &s.rulesBackgroundRect, s.backgroundBitmap,
                                (s.backgroundBitmapInfo.bmWidth * s.backgroundCropLeftPermille) / 1000,
                                (s.backgroundBitmapInfo.bmHeight * s.backgroundCropTopPermille) / 1000,
                                (s.backgroundBitmapInfo.bmWidth * s.backgroundCropWidthPermille) / 1000,
                                (s.backgroundBitmapInfo.bmHeight * s.backgroundCropHeightPermille) / 1000);
      }
      else
      {
        FillRect(dc, &s.rulesBackgroundRect, GetStockObject(0));
      }
    }
  }
  SelectObject(dc, g_bigCardTextFont);
  s.manaTextHeight = (unsigned short)((DrawManaText(dc, &s.rulesTextRect, s.rules_text, 1) >> 0x10) & 0xffff);
  GetTextMetricsA(dc, &s.textMetrics);
  CopyRect(&s.flavorRect, &s.rulesTextRect);
  s.flavorRect.top += s.manaTextHeight;
  s.flavorRect.top += s.textMetrics.tmHeight / 3;
  SelectObject(dc, g_bigCardTextItalicFont);
  DrawTextA(dc, card->flavor_text, -1, &s.flavorRect, 0x10);
  RestoreDC(dc, s.nSavedDC);
  return s.result;
}

// FUNCTION: DRAWCARDLIB 0x100067c7
HBITMAP *GetCardBackgroundPointer(card_ptr_t *card)
{
  HBITMAP *result;

  if (card->color == 1)
  {
    result = &global_CARDBK_Black;
  }
  else if (card->color == 8)
  {
    result = &global_CARDBK_White;
  }
  else if (card->color == 7)
  {
    result = &global_CARDBK_Red;
  }
  else if (card->color == 5)
  {
    result = &global_CARDBK_Green;
  }
  else if (card->color == 2)
  {
    result = &global_CARDBK_Blue;
  }
  else if (card->color == 4)
  {
    result = &global_CARDBK_Gold;
  }
  else if (card->color == 0)
  {
    result = &global_CARDBK_Artifact;
  }
  else if (card->color == 3)
  {
    result = &global_CARDBK_Artifact;
  }
  else if (card->color == 6)
  {
    if ((card->expansion & 2) != 0)
    {
      result = &global_CARDBK_AntiquitiesLand;
    }
    else
    {
      if ((card->expansion & 4) != 0)
      {
        result = &global_CARDBK_ArabianNightsLand;
      }
      else
      {
        if ((card->expansion & 0x20) != 0)
        {
          result = &global_CARDBK_DarklandsLand;
        }
        else
        {
          if ((card->expansion & 0x100) != 0)
          {
            result = &global_CARDBK_LegendsLand;
          }
          else
          {
            if ((card->expansion & 8) != 0)
            {
              result = &global_CARDBK_AntiquitiesLand;
            }
            else
            {
              if (strcmp(card->full_name, g_cardNames + 0x5d5c) == 0)
              {
                result = &global_CARDBK_BlackLand;
              }
              else if (strcmp(card->full_name, g_cardNames + 0x4970) == 0)
              {
                result = &global_CARDBK_WhiteLand;
              }
              else if (strcmp(card->full_name, g_cardNames + 0x4010) == 0)
              {
                result = &global_CARDBK_RedLand;
              }
              else if (strcmp(card->full_name, g_cardNames + 0x238c) == 0)
              {
                result = &global_CARDBK_GreenLand;
              }
              else if (strcmp(card->full_name, g_cardNames + 0x3138) == 0)
              {
                result = &global_CARDBK_BlueLand;
              }
              else
              {
                result = &global_CARDBK_AntiquitiesLand;
              }
            }
          }
        }
      }
    }
  }
  else if (card->color == -1)
  {
    result = &global_CARDBK_Special;
  }
  else
  {
    result = &global_CARDBK_Artifact;
  }
  return result;
}

// FUNCTION: DRAWCARDLIB 0x10006a41
BOOL LoadCardBackground(HBITMAP *outBitmap)
{
  struct
  {
    char filepath[264];
    char filename[100];
  } s;

  if (outBitmap == (HBITMAP *)0x0)
  {
    return 0;
  }
  if (*outBitmap != 0)
  {
    return 1;
  }
  if (outBitmap == &global_CARDBK_Green)
  {
    strcpy(s.filename, "CARDBK_Green");
  }
  else if (outBitmap == &global_CARDBK_White)
  {
    strcpy(s.filename, "CARDBK_White");
  }
  else if (outBitmap == &global_CARDBK_Blue)
  {
    strcpy(s.filename, "CARDBK_Blue");
  }
  else if (outBitmap == &global_CARDBK_Black)
  {
    strcpy(s.filename, "CARDBK_Black");
  }
  else if (outBitmap == &global_CARDBK_Red)
  {
    strcpy(s.filename, "CARDBK_Red");
  }
  else if (outBitmap == &global_CARDBK_Gold)
  {
    strcpy(s.filename, "CARDBK_Gold");
  }
  else if (outBitmap == &global_CARDBK_Artifact)
  {
    strcpy(s.filename, "CARDBK_Artifact");
  }
  else if (outBitmap == &global_CARDBK_GreenLand)
  {
    strcpy(s.filename, "CARDBK_GreenLand");
  }
  else if (outBitmap == &global_CARDBK_WhiteLand)
  {
    strcpy(s.filename, "CARDBK_WhiteLand");
  }
  else if (outBitmap == &global_CARDBK_BlueLand)
  {
    strcpy(s.filename, "CARDBK_BlueLand");
  }
  else if (outBitmap == &global_CARDBK_BlackLand)
  {
    strcpy(s.filename, "CARDBK_BlackLand");
  }
  else if (outBitmap == &global_CARDBK_RedLand)
  {
    strcpy(s.filename, "CARDBK_RedLand");
  }
  else if (outBitmap == &global_CARDBK_DarklandsLand)
  {
    strcpy(s.filename, "CARDBK_DarklandsLand");
  }
  else if (outBitmap == &global_CARDBK_FallenEmpiresLand)
  {
    strcpy(s.filename, "CARDBK_FallenEmpiresLand");
  }
  else if (outBitmap == &global_CARDBK_AntiquitiesLand)
  {
    strcpy(s.filename, "CARDBK_AntiquitiesLand");
  }
  else if (outBitmap == &global_CARDBK_LegendsLand)
  {
    strcpy(s.filename, "CARDBK_LegendsLand");
  }
  else if (outBitmap == &global_CARDBK_ArabianNightsLand)
  {
    strcpy(s.filename, "CARDBK_ArabianNightsLand");
  }
  else if (outBitmap == &global_CARDBK_Special)
  {
    strcpy(s.filename, "CARDBK_Special");
  }
  else
  {
    strcpy(s.filename, (char *)&g_emptyStringStorage);
  }
  if (s.filename[0] != '\0')
  {
    sprintf(s.filepath, "%s\\%s\x2Epic", &global_cardart_directory, s.filename);
    *outBitmap = load_pic(s.filepath);
  }
  if (*outBitmap != 0)
  {
    return 1;
  }
  else
    return 0;
}

// FUNCTION: DRAWCARDLIB 0x10006d5f
void DrawCardSet(HDC dc, RECT *rect, uint expansion)
{
  struct
  {
    BITMAP bm; // ebp - 0x40
    int cellH; // ebp - 0x24

    int dstW;     // ebp - 0x24
    int cellW;    // ebp - 0x20
    int srcX;     // ebp - 0x1c
    RECT dstRect; // ebp - 0x18
    int srcX2;    // ebp - 8
    int dstH;
  } s;

  if (dc == 0 || rect == (RECT *)0x0)
  {
    return;
  }
  if ((expansion & 0x800) != 0)
  {
    return;
  }
  if (expansion == 0xffffffff || (expansion & 0x10) != 0 || (expansion & 0x80) != 0)
  {
    return;
  }
  if (global_CardSets != NULL)
  {
    if (((expansion & 0x2e) != 0) || ((expansion & 0x100) != 0))
    {
      GetObjectA(global_CardSets, sizeof(s.bm), &s.bm);
      s.cellW = s.bm.bmWidth / 10;
      s.cellH = s.bm.bmHeight;
      if ((expansion & 0x20) != 0)
      {
        s.srcX = 0;
      }
      else if ((expansion & 0x100) != 0)
      {
        s.srcX = s.cellW * 2;
      }
      else if ((expansion & 4) != 0)
      {
        s.srcX = s.cellW << 2;
      }
      else if ((expansion & 2) != 0)
      {
        s.srcX = s.cellW * 6;
      }
      else if ((expansion & 8) != 0)
      {
        s.srcX = s.cellW << 3;
      }

      s.srcX2 = s.cellW + s.srcX;
      s.dstH = rect->bottom - rect->top;
      s.dstW = (s.dstH * s.cellW) / s.cellH;
      SetRect(&s.dstRect, rect->right - s.dstW, rect->top, rect->right, rect->top + s.dstH);
      DrawMaskedBitmapToRect(dc, &s.dstRect, global_CardSets, s.cellW, s.cellH, s.srcX, 0, s.srcX2, 0);
    }
  }
}

// FUNCTION: DRAWCARDLIB 0x10006eed
void DrawCastingCost(HDC dc, RECT *rect, casting_cost_t *castingCost)
{
  struct
  {
    int width;
    char castingCostString[20];
    int top;
    char symbol;
    int left;
    char *stringPtr;
    int height;
    int costReqs[7];
  } s;

  if (dc == 0 || rect == 0 || castingCost == NULL)
  {
  }
  else
  {
    s.costReqs[0] = (int)(char)castingCost->req_colorless;
    s.costReqs[1] = (int)(char)castingCost->req_black;
    s.costReqs[5] = (int)(char)castingCost->req_white;
    s.costReqs[3] = (int)(char)castingCost->req_green;
    s.costReqs[4] = (int)(char)castingCost->req_red;
    s.costReqs[2] = (int)(char)castingCost->req_blue;
    BuildCostString(s.costReqs, s.castingCostString);
    s.height = rect->bottom - rect->top;
    s.width = s.height;
    s.left = rect->right - (strlen(s.castingCostString) >> 1) * s.width;
    s.top = rect->top;
    s.stringPtr = s.castingCostString;
    while (*s.stringPtr != '\0')
    {
      s.symbol = GetNextManaSymbol(&s.stringPtr);
      DrawManaSymbol(dc, s.symbol, s.left, s.top, s.width, s.height);
      s.left += s.width;
    }
  }
}

// FUNCTION: DRAWCARDLIB 0x10006ff3
int BuildCostString(int *costReqs, char *costStringOut)
{
  struct
  {
    int req_black;
    int req_blue;
    int length;
    char result[52];
    int req_colorless;
    int req_red;
    int req_white;
    int done;
    int req_green;
  } s;

  if ((costReqs == (int *)0x0) || (costStringOut == 0))
  {
    return 0;
  }

  s.req_white = costReqs[5];
  s.req_green = costReqs[3];
  s.req_red = costReqs[4];
  s.req_black = costReqs[1];
  s.req_blue = costReqs[2];
  s.req_colorless = costReqs[0];
  s.length = 0;
  if (s.req_colorless != 0)
  {
    if (s.req_colorless == -1)
    {
      s.result[s.length] = '|';
      s.length++;
      s.result[s.length] = 'X';
      s.length++;
    }
    else if (s.req_colorless == 0x48)
    {
      s.result[s.length] = '|';
      s.length++;
      s.result[s.length] = 'X';
      s.length++;
    }
    else if (s.req_colorless == 0x28)
    {
      s.result[s.length] = '|';
      s.length++;
      s.result[s.length] = 'X';
      s.length++;
    }
    else if ((s.req_colorless >= 1) && (9 >= s.req_colorless))
    {
      s.result[s.length] = '|';
      s.length++;
      s.result[s.length] = (char)(s.req_colorless + '0');
      s.length++;
    }
    else if (s.req_colorless == 10)
    {
      s.result[s.length] = '|';
      s.length++;
      s.result[s.length] = '1';
      s.length++;
      s.result[s.length] = '0';
      s.length++;
    }
  }
  s.done = 0;
  while (s.done == 0)
  {
    if (s.req_white != 0)
    {
      s.result[s.length] = '|';
      s.length++;
      s.result[s.length] = 'W';
      s.length++;
      s.req_white--;
    }
    else if (s.req_green != 0)
    {
      s.result[s.length] = '|';
      s.length++;
      s.result[s.length] = 'G';
      s.length++;
      s.req_green--;
    }
    else if (s.req_red != 0)
    {
      s.result[s.length] = '|';
      s.length++;
      s.result[s.length] = 'R';
      s.length++;
      s.req_red--;
    }
    else if (s.req_black != 0)
    {
      s.result[s.length] = '|';
      s.length++;
      s.result[s.length] = 'B';
      s.length++;
      --s.req_black;
    }
    else if (s.req_blue != 0)
    {
      s.result[s.length] = '|';
      s.length++;
      s.result[s.length] = 'U';
      s.length++;
      --s.req_blue;
    }
    else
    {
      s.result[s.length] = '\0';
      s.done = 1;
    }
  }
  if ((((s.req_colorless == 0) && (s.length == 0)) && (s.req_white == 0)) &&
      (((s.req_green == 0 && (s.req_red == 0)) && ((s.req_black == 0 && (s.req_blue == 0))))))
  {
    // 0 cost
    strcpy(s.result, "|0");
  }
  if (costStringOut != 0)
  {
    strcpy(costStringOut, s.result);
  }
  return s.length;
}

// FUNCTION: DRAWCARDLIB 0x100074b2
void DrawManaSymbol(HDC dc, char symbol, int left, int top, int width, int height)
{
  struct
  {
    BITMAP bm;
    int hSrc;
    int wSrc;
    int symbolX;
    RECT rect;
    int maskX;
  } s;

  if (dc == (HDC)0x0)
  {
    return;
  }
  if (symbol < -0x12 || -1 < symbol)
  {
    return;
  }
  GetObjectA(global_ManaSymbols, sizeof(s.bm), &s.bm);
  s.wSrc = s.bm.bmHeight;
  s.hSrc = s.wSrc;
  s.maskX = s.bm.bmWidth - s.wSrc;
  if (symbol == -0x10)
  {
    s.symbolX = 0;
  }
  else if (symbol == -0xf)
  {
    s.symbolX = s.wSrc;
  }
  else if (symbol == -0xe)
  {
    s.symbolX = s.wSrc * 2;
  }
  else if (symbol == -0xd)
  {
    s.symbolX = s.wSrc * 3;
  }
  else if (symbol == -0xc)
  {
    s.symbolX = s.wSrc << 2;
  }
  else if (symbol == -0xb)
  {
    s.symbolX = s.wSrc * 5;
  }
  else if (symbol == -10)
  {
    s.symbolX = (s.wSrc * 3) * 2;
  }
  else if (symbol == -9)
  {
    s.symbolX = s.wSrc * 7;
  }
  else if (symbol == -8)
  {
    s.symbolX = s.wSrc << 3;
  }
  else if (symbol == -7)
  {
    s.symbolX = s.wSrc * 9;
  }
  else if (symbol == -6)
  {
    s.symbolX = s.wSrc * 10;
  }
  else if (symbol == -0x11)
  {
    s.symbolX = s.wSrc * 0xb;
  }
  else if (symbol == -5)
  {
    s.symbolX = s.wSrc * 0xc;
  }
  else if (symbol == -4)
  {
    s.symbolX = s.wSrc * 0xd;
  }
  else if (symbol == -3)
  {
    s.symbolX = s.wSrc * 0xe;
  }
  else if (symbol == -2)
  {
    s.symbolX = s.wSrc * 0xf;
  }
  else if (symbol == -1)
  {
    s.symbolX = s.wSrc << 4;
  }
  else if (symbol == -0x12)
  {
    s.symbolX = s.wSrc * 17;
  }
  SetRect(&s.rect, left, top, width + left, height + top);
  DrawMaskedBitmapToRect(dc, &s.rect, global_ManaSymbols, s.wSrc, s.hSrc, s.symbolX, 0, s.maskX, 0);
}

// FUNCTION: DRAWCARDLIB 0x1000775c
undefined4 CalcDrawManaText(HDC dc, RECT *rect, char *text)
{
  struct
  {
    int nSavedDC;
    undefined4 result;
  } s;

  /* 0x775c  1  CalcDrawManaText */
  if (((dc == (HDC)0x0) || (rect == (RECT *)0x0)) || (text == (char *)0x0))
  {
    return 0;
  }
  s.nSavedDC = SaveDC(dc);
  IntersectClipRect(dc, 0, 0, 1, 1);
  s.result = DrawManaText(dc, rect, text, 0);
  RestoreDC(dc, s.nSavedDC);
  return s.result;
}

// FUNCTION: DRAWCARDLIB 0x100077dd
uint DrawManaText(HDC dc, RECT *rect, char *text, int drawSymbols)
{
  struct
  {
    int line_height;
    int symbol_step;
    char buf[52];
    int symbol_width;
    int font_height;
    int temp;
    int nSavedDC;
    int top;
    int idx;
    char symbol;
    int max_width;            // ebp - 0x60
    int left;                 // ebp - 0x5c
    TEXTMETRICA text_metrics; // ebp - 0x58
    int symbol_height;        // ebp - 0x20
    RECT rc;                  // ebp - 0x1c
    SIZE size;                // ebp - 0xc
    int height;               // ebp - 0x4
  } s;

  if (((dc == (HDC)0x0) || (rect == (RECT *)0x0)) || (text == (char *)0x0))
  {
    return 0;
  }
  if (*text == '\0')
  {
    return 0;
  }
  s.nSavedDC = SaveDC(dc);
  s.max_width = 0;
  s.height = 0;
  GetTextMetricsA(dc, &s.text_metrics);
  s.line_height = s.text_metrics.tmExternalLeading + s.text_metrics.tmHeight;
  SetRect(&s.rc, 0, 0, 0, s.text_metrics.tmHeight);
  LPtoDP(dc, (LPPOINT)&s.rc, 2);
  SetRect(&s.rc, 0, 0, s.rc.bottom - s.rc.top, 0);
  DPtoLP(dc, (LPPOINT)&s.rc, 2);
  s.symbol_width = ((s.rc.right - s.rc.left) * 0x4b) / 100;
  s.symbol_step = ((s.rc.right - s.rc.left) * 0x55) / 100;
  s.symbol_height = (s.text_metrics.tmHeight * 0x4b) / 100;
  s.font_height = s.text_metrics.tmHeight;
  IntersectClipRect(dc, rect->left, rect->top, rect->right + 1, rect->bottom);
  SelectObject(dc, GetStockObject(4));
  SelectObject(dc, GetStockObject(8));
  s.left = rect->left;
  s.top = rect->top;
  while (*text != '\0')
  {
    if (*text == ' ')
    {
      s.idx = 0;
      s.buf[s.idx] = *text;
      text = text + 1;
      s.idx = s.idx + 1;
      while (*text != '\0' && *text == ' ')
      {
        s.buf[s.idx] = *text;
        text = text + 1;
        s.idx = s.idx + 1;
      }
      s.buf[s.idx] = '\0';
      GetTextExtentPoint32A(dc, s.buf, s.idx, &s.size);
      s.left = s.left + s.size.cx;
    }
    else if (*text != '\0' && *text == '\n')
    {
      text = text + 1;
      s.top = s.top + s.line_height + s.line_height / 3;
      s.left = rect->left;
    }
    else
    {
      s.symbol = GetNextManaSymbol(&text);
      if (s.symbol != '\0')
      {
        s.idx = 0;
        s.buf[s.idx] = s.symbol;
        s.idx = s.idx + 1;
        while (*text != '\0' && (s.symbol = GetNextManaSymbol(&text)) != '\0')
        {
          s.buf[s.idx++] = s.symbol;
        }
        s.buf[s.idx] = '\0';
        s.size.cx = s.idx * s.symbol_width;
        if (rect->right < s.size.cx + s.left)
        {
          s.max_width = max(s.max_width, s.left - rect->left);
          s.top = s.top + s.line_height;
          s.left = rect->left;
        }
        s.temp = strlen(s.buf);
        for (s.idx = 0; s.temp > s.idx; s.idx = s.idx + 1)
        {
          if (drawSymbols != 0)
          {
            DrawManaSymbol(dc, s.buf[s.idx], s.left + (s.symbol_step - s.symbol_width) / 2,
                           s.top + (s.font_height - s.symbol_height) / 2, s.symbol_width, s.symbol_height);
          }
          else
          {
            Ellipse(dc, s.left + (s.symbol_step - s.symbol_width) / 2,
                    s.top + (s.font_height - s.symbol_height) / 2,
                    s.symbol_width + (s.symbol_step - s.symbol_width) / 2 + s.left,
                    s.symbol_height + (s.font_height - s.symbol_height) / 2 + s.top);
          }
          s.left = s.left + s.symbol_step;
        }
        if ((*text == ':') || (*text == ','))
        {
          s.idx = 0;
          s.buf[s.idx] = *text;
          text = text + 1;
          s.idx = s.idx + 1;
          s.buf[s.idx] = '\0';
          GetTextExtentPoint32A(dc, s.buf, s.idx, &s.size);
          TextOutA(dc, s.left, s.top, s.buf, strlen(s.buf));
          s.left = s.left + s.size.cx;
        }
      }
      else
      {
        s.idx = 0;
        s.buf[s.idx] = *text;
        text = text + 1;
        s.idx = s.idx + 1;
        while (*text != '\0' && *text != ' ' && *text != '\n' && *text != '|')
        {
          s.buf[s.idx] = *text;
          text = text + 1;
          s.idx = s.idx + 1;
        }
        s.buf[s.idx] = '\0';
        GetTextExtentPoint32A(dc, s.buf, s.idx, &s.size);
        if (rect->right < s.size.cx + s.left)
        {
          s.max_width = max(s.max_width, s.left - rect->left);
          s.top = s.top + s.line_height;
          s.left = rect->left;
        }
        TextOutA(dc, s.left, s.top, s.buf, strlen(s.buf));
        s.left = s.left + s.size.cx;
      }
    }
  }
  s.max_width = max(s.max_width, s.left - rect->left);
  s.height = (s.line_height + s.top) - rect->top;
  RestoreDC(dc, s.nSavedDC);
  return ((unsigned short)s.height << 0x10) | (unsigned short)s.max_width;
}

// FUNCTION: DRAWCARDLIB 0x10007dd7
void DrawSmallCard(HDC dc, RECT *rect, card_ptr_t *card, undefined4 version, int reloadIfWrongSize)
{
  struct
  {
    RECT artRect;   // ebp - 0x30
    int artTop;     // ebp - 0x20
    int artLeft;    // ebp - 0x1c
    int cardHeight; // ebp - 0x18
    int cardWidth;  // ebp - 0x14
    int artHeight;  // ebp - 0x10
    int status;     // ebp - 0xc
    int artWidth;   // ebp - 0x8
    int nSavedDC;   // ebp - 0x4
  } s;

  if (dc == NULL || rect == NULL || card == NULL)
  {
    return;
  }

  s.nSavedDC = SaveDC(dc);
  SetMapMode(dc, 8);
  SetWindowExtEx(dc, 200, 0x118, (LPSIZE)0x0);
  SetViewportExtEx(dc, rect->right - rect->left, rect->bottom - rect->top, (LPSIZE)0x0);
  SetWindowOrgEx(dc, 0, 0, (LPPOINT)0x0);
  SetViewportOrgEx(dc, rect->left, rect->top, (LPPOINT)0x0);
  DrawCardBackground(dc, rect, card);
  DrawSmallCardTitle(dc, rect, card->name, 0, 1);
  SelectObject(dc, GetStockObject(5));
  SelectObject(dc, g_cardBorderPen);
  Rectangle(dc, 0, 0, 200, 0x118);
  SetMapMode(dc, 1);
  s.cardWidth = rect->right - rect->left;
  s.cardHeight = rect->bottom - rect->top;
  s.artLeft = (s.cardWidth * 0x12) / 0xe4;
  s.artTop = (s.cardHeight * 8) / 100 + (s.cardHeight * 0xb) / 100 + -2;
  s.artWidth = ((s.cardWidth * 0xd3) / 0xe4 - s.artLeft) + 1;
  s.artHeight = (s.cardHeight * 0xb2) / 0xbf - s.artTop;
  SetRect(&s.artRect, s.artLeft, s.artTop, s.artWidth + s.artLeft, s.artHeight + s.artTop);
  if (IsSmallArtIn(card->id, version) == 0)
  {
    LoadSmallArt(card->id, version, s.artWidth, s.artHeight);
  }
  else if (reloadIfWrongSize != 0)
  {
    ReloadSmallArtIfWrongSize(card->id, version, s.artWidth, s.artHeight);
  }
  s.status = DrawSmallArt(dc, &s.artRect, card->id, version);
  if (s.status == 0)
  {
    DrawBigArt(dc, &s.artRect, card->id, version);
  }
  RestoreDC(dc, s.nSavedDC);
}

// FUNCTION: DRAWCARDLIB 0x10008065
void DrawCardBackground(HDC dc, RECT *rect, card_ptr_t *card)
{
  struct
  {
    int scaledHeight;
    BITMAP bm;
    HBITMAP *backgroundPointer;
    RECT dstRect;
    int sectionTop;
    int sectionHeight;
  } s;

  if (dc == NULL || rect == NULL || card == NULL)
  {
    return;
  }

  s.backgroundPointer = GetCardBackgroundPointer(card);
  LoadCardBackground(s.backgroundPointer);
  if (*s.backgroundPointer != 0)
  {
    GetObjectA(*s.backgroundPointer, sizeof(s.bm), &s.bm);
    SetRect(&s.dstRect, 0, 0, 200, 0x118);
    DrawBitmapSubrectToRect(dc, &s.dstRect, *s.backgroundPointer, 0, 0, s.bm.bmWidth,
                            (s.bm.bmHeight * 0x3b) / 100);
    s.scaledHeight = (((s.bm.bmHeight * 0x3b) / 100) * 0xb) / 100;
    s.sectionHeight = 0x1e;
    s.sectionTop = 0x16;
    SetRect(&s.dstRect, 0, s.sectionTop, 200, s.sectionHeight + s.sectionTop);
    DrawBitmapSubrectToRect(dc, &s.dstRect, *s.backgroundPointer, 0, 2, s.bm.bmWidth,
                            s.scaledHeight + 2);
  }
  else
  {
    FillRect(dc, rect, GetStockObject(0));
  }
}

// FUNCTION: DRAWCARDLIB 0x100081b2
void DrawSmallCardTitle(HDC dc, RECT *rect, char *name, int colorMode, int transparentBackground)
{
  struct
  {
    COLORREF titleColor;
    char titleText[100];
    int nSavedDC;
  } s;

  /* 0x81b2  7  DrawSmallCardTitle */
  if (((dc == (HDC)0x0) || (rect == (RECT *)0x0)) || (name == 0))
  {
    return;
  }
  strcpy(s.titleText, name);
  s.nSavedDC = SaveDC(dc);
  SetMapMode(dc, 8);
  SetWindowExtEx(dc, 200, 0x118, (LPSIZE)0x0);
  SetViewportExtEx(dc, rect->right - rect->left, rect->bottom - rect->top, (LPSIZE)0x0);
  SetWindowOrgEx(dc, 0, 0, (LPPOINT)0x0);
  SetViewportOrgEx(dc, rect->left, rect->top, (LPPOINT)0x0);
  if (colorMode == 0)
  {
    s.titleColor = g_smallCardTitleColor;
  }
  else if (colorMode == 2)
  {
    s.titleColor = g_smallCardTitleAltColor;
  }
  else
  {
    s.titleColor = g_smallCardTitleFallbackColor;
  }
  SelectObject(dc, g_smallCardTitleFont);
  SetTextAlign(dc, 0);
  if (transparentBackground == 0)
  {
    SetBkMode(dc, 2);
    SetBkColor(dc, g_smallCardTitleBackgroundColor);
  }
  else
  {
    SetBkMode(dc, 1);
  }
  SetTextColor(dc, g_cardTextShadowColor);
  TextOutA(dc, 5, 1, s.titleText, strlen(s.titleText));
  SetTextColor(dc, s.titleColor);
  SetBkMode(dc, 1);
  TextOutA(dc, 2, -2, s.titleText, strlen(s.titleText));
  RestoreDC(dc, s.nSavedDC);
}

// FUNCTION: DRAWCARDLIB 0x1000ac34
void DeleteAndCloseObject(HANDLE handle)
{
  struct
  {
    OSVERSIONINFOA ver;
    HANDLE sectionHandle;
    DIBSECTION dibSection;
    char *bitsPointer;
  } s;

  if (global_dwPlatformId == -1)
  {
    s.ver.dwOSVersionInfoSize = 0x94;
    memset(&s.ver.dwMajorVersion, 0, 0x90);
    GetVersionExA(&s.ver);
    if (s.ver.dwPlatformId == 1)
    {
      global_dwPlatformId = 1;
    }
    else
    {
      global_dwPlatformId = 0;
    }
  }
  if (handle != (HANDLE)0x0)
  {
    GetObjectA(handle, sizeof(s.dibSection), &s.dibSection);
    s.sectionHandle = s.dibSection.dshSection;
    s.bitsPointer = (char *)s.dibSection.dsBm.bmBits;
    s.bitsPointer += s.dibSection.dsOffset;
    DeleteObject(handle);
    if (s.sectionHandle != (HANDLE)0x0)
    {
      if (global_dwPlatformId != 0)
      {
        CloseHandle(s.sectionHandle);
      }
    }
  }
}

// FUNCTION: DRAWCARDLIB 0x1000b06a
void ReplaceSubstring(char *inOutStr, char *needle, int caseSensitive, char *replacement)
{
  struct
  {
    int outLen;
    char outStr[1000];
    size_t needleLen;
    int matched;
    char *cur;
  } s;

  if (((inOutStr == (char *)0x0) || (needle == (char *)0x0)) || (replacement == (char *)0x0))
  {
    return;
  }

  if (strlen(inOutStr) == 0 || strlen(needle) == 0)
  {
    return;
  }

  s.needleLen = strlen(needle);
  s.cur = inOutStr;
  s.outStr[0] = '\0';
  s.outLen = 0;
  while (*s.cur != '\0')
  {
    s.matched = 0;
    if ((caseSensitive != 0 && strncmp(s.cur, needle, s.needleLen) == 0) || (caseSensitive == 0 && _strnicmp(s.cur, needle, s.needleLen) == 0))
    {
      s.matched = 1;
    }
    if (s.matched != 0)
    {
      strcat(s.outStr, replacement);
      s.outLen += strlen(replacement);
      s.cur += s.needleLen;
    }
    else
    {
      s.outStr[s.outLen] = *s.cur;
      s.cur++;
      s.outLen++;
      s.outStr[s.outLen] = '\0';
    }
  }
  strcpy(inOutStr, s.outStr);
}

// FUNCTION: DRAWCARDLIB 0x1000bd90
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
  undefined4 result;

  (void)reserved;
  switch (reason)
  {
  case 1:
    result = 1;
    result &= read_cfg(hinst);
    result &= prepare_fonts_and_imgs();
    return (BOOL)result;
  case 2:
    return (BOOL)1;
  case 3:
    return (BOOL)1;
  case 0:
    DestroyAllResources();
    DrawCardLib_Shutdown();
    return (BOOL)1;
  }
}

// FUNCTION: DRAWCARDLIB 0x1000be22
int read_cfg(HINSTANCE hinst)
{
  struct
  {
    char *lastSlash;
    void *recordBuf;
    char cards_dat_filename[264];
    size_t global_available_slots;
    size_t record_size;
    int slotIndex;
    FILE *cards_dat;
    int record_table[76000];
    int result;
  } s;

  s.result = 1;
  g_drawCardLibInstance = hinst;
  InitializeCriticalSection(&global_critical_section_for_catalog);

#ifdef _DEBUG
  // Allow debugging directly from output directory
  if (getcwd(global_base_directory, sizeof(global_base_directory)) == NULL)
  {
    return 0;
  }
#else
  GetModuleFileNameA((HMODULE)0x0, global_base_directory, 0x105);
  s.lastSlash = strrchr(global_base_directory, '\\');
  *s.lastSlash = '\0';
#endif

  strcpy(global_cardart_directory, global_base_directory);
  strcat(global_cardart_directory, "\\CARDART");
  global_dither_kernel_id = 3;
  global_color_depth = 0x10;
  InitCardArtGdiResources();
  strcpy(s.cards_dat_filename, global_base_directory);
  strcat(s.cards_dat_filename, "\\CARDS.DAT");
  s.cards_dat = fopen(s.cards_dat_filename, "rb");
  if (s.cards_dat != (FILE *)0x0)
  {
    fread(&s.global_available_slots, 4, 1, s.cards_dat);
    fread(&s.record_size, 4, 1, s.cards_dat);
    fread(s.record_table, 0x98, s.global_available_slots, s.cards_dat);
    s.recordBuf = malloc(s.record_size);
    if (s.recordBuf != (void *)0x0)
    {
      fread(s.recordBuf, 1, s.record_size, s.cards_dat);
      for (s.slotIndex = 0; (int)s.global_available_slots > s.slotIndex;
           s.slotIndex = s.slotIndex + 1)
      {
        s.record_table[s.slotIndex * 0x26 + 1] = s.record_table[s.slotIndex * 0x26 + 1] + (int)s.recordBuf;
        strncpy(g_cardNames + s.slotIndex * 100, (char *)s.record_table[s.slotIndex * 0x26 + 1], 100);
        g_cardNameSlotTerminators[s.slotIndex * 100] = 0;
      }
      free(s.recordBuf);
    }
    else
    {
      s.result = 0;
    }
    fclose(s.cards_dat);
  }
  else
  {
    s.result = 0;
  }
  if (s.result == 0)
  {
    for (s.slotIndex = 0; (int)s.global_available_slots > s.slotIndex;
         s.slotIndex = s.slotIndex + 1)
    {
      strcpy(g_cardNames + s.slotIndex * 100, "");
    }
  }
  return s.result;
}

// FUNCTION: DRAWCARDLIB 0x1000c0ca
void DrawCardLib_Shutdown(void)
{
  ShutdownCardArtGdiResources();
  DeleteCriticalSection(&global_critical_section_for_catalog);
}
