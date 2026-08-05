#ifndef SHANDALAR_DRAWCARDLIB_AUTOPROTO_H
#define SHANDALAR_DRAWCARDLIB_AUTOPROTO_H

/* Auto-generated from src/drawcardlib/src/main.c; do not edit by hand.
 * Intentionally uses non-prototype declarations (empty parameter lists) to
 * avoid argument-count/type mismatches in decompiler output while still
 * preventing implicit-int return-type assumptions. */

uint BitStream_ReadBits();
uint BitStream_ReadBit();
undefined4 prepare_fonts_and_imgs();
LOGFONTA * LoadFontFromIni();
void DestroyAllResources();
void DestroyAllCardBackgrounds();
void DrawCardBack();
undefined4 DrawFullCard();
HBITMAP * GetCardBackgroundPointer();
BOOL LoadCardBackground();
void DrawCardSet();
void DrawCastingCost();
int BuildCostString();
int GetNextManaSymbol();
void DrawManaSymbol(HDC dc, char symbol, int left, int top, int width, int height);
undefined4 CalcDrawManaText();
uint DrawManaText();
void DrawSmallCard();
void DrawCardBackground();
void DrawSmallCardTitle();
undefined4 DrawMaskedBitmapToRect();
void DeleteAndCloseObject();
COLORREF GetPaletteColor(int index);
void ReplaceSubstring();
int read_cfg();
void DrawCardLib_Shutdown();
#endif
