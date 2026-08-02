#ifndef __DRAWCARDLIB_H__
#define __DRAWCARDLIB_H__

#include <windows.h>
#include "defs.h"
#include "mystdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllimport) void DrawSmallCardTitle(HDC dc, RECT *rect, char* name, int colorMode, int transparentBackground);
    __declspec(dllimport) void DrawSmallCard(HDC dc,RECT *rect, card_ptr_t *card, uint version, int reloadIfWrongSize);
    __declspec(dllimport) uint CalcDrawManaText(HDC dc,RECT *rect,char* text);
    __declspec(dllimport) uint DrawManaText(HDC dc,RECT *rect,char *text,int drawSymbols);
    __declspec(dllimport) void DrawCardBack(HDC dc,RECT *rect);
    __declspec(dllimport) void DestroyAllCardBackgrounds(void);
    __declspec(dllimport) uint DrawFullCard(HDC dc, RECT *rect, card_ptr_t *card, uint version, uint drawFlags, bool expanded_text, LPCSTR artistFormat);

#ifdef __cplusplus
}
#endif

#endif
