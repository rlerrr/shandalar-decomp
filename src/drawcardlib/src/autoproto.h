#ifndef SHANDALAR_DRAWCARDLIB_AUTOPROTO_H
#define SHANDALAR_DRAWCARDLIB_AUTOPROTO_H

/* Function prototypes for src/drawcardlib/src/main.c and drawing.c. */

uint BitStream_ReadBits(uint bit_count);
uint BitStream_ReadBit(void);
undefined4 prepare_fonts_and_imgs(void);
LOGFONTA *LoadFontFromIni(char *fontKeySuffix, int italic);
void DestroyAllResources(void);
void DestroyAllCardBackgrounds(void);
void DrawCardBack(HDC dc, RECT *rect);
undefined4 DrawFullCard(HDC dc, RECT *rect, card_ptr_t *card, undefined4 version, uint drawFlags, bool expanded_text, LPCSTR artistFormat);
HBITMAP *GetCardBackgroundPointer(card_ptr_t *card);
BOOL LoadCardBackground(HBITMAP *outBitmap);
void DrawCardSet(int dc, RECT *rect, uint expansion);
void DrawCastingCost(HDC dc, RECT *rect, casting_cost_t *castingCost);
int BuildCostString(int *costReqs, char *costStringOut);
int GetNextManaSymbol(char **textCursor);
void DrawManaSymbol(HDC dc, char symbol, int left, int top, int width, int height);
undefined4 CalcDrawManaText(HDC dc, RECT *rect, char *text);
uint DrawManaText(HDC dc, RECT *rect, char *text, int drawSymbols);
void DrawSmallCard(HDC dc, RECT *rect, card_ptr_t *card, undefined4 version, int reloadIfWrongSize);
void DrawCardBackground(HDC dc, RECT *rect, card_ptr_t *card);
void DrawSmallCardTitle(HDC dc, RECT *rect, char *name, int colorMode, int transparentBackground);
BOOL DrawMaskedBitmapToRect(HDC dc, RECT *rect, HANDLE bitmap, int wSrc, int hSrc, int srcX, int srcY, int maskX, int maskY);
void DeleteAndCloseObject(HANDLE handle);
COLORREF GetPaletteColor(int index);
void ReplaceSubstring(char *inOutStr, char *needle, int caseSensitive, char *replacement);
int read_cfg(undefined4 hinst);
void DrawCardLib_Shutdown(void);
#endif
