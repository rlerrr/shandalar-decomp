#ifndef __DRAWCARDLIB_H__
#define __DRAWCARDLIB_H__

#include <windows.h>
#include "defs.h"
#include "mystdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllimport) void DrawSmallCardTitle(HDC dc, RECT *rect, char* name, int param_4, int param_5);
    __declspec(dllimport) void DrawSmallCard(HDC dc,RECT *rect, card_ptr_t *card, uint param_4, int param_5);
    __declspec(dllimport) uint CalcDrawManaText(HDC dc,RECT *rect,char* text);
    __declspec(dllimport) uint DrawManaText(HDC dc,RECT *param_2,char *text,int param_4);
    __declspec(dllimport) void DrawCardBack(HDC dc,RECT *rect);
    __declspec(dllimport) void DestroyAllCardBackgrounds(void);
    __declspec(dllimport) uint DrawFullCard(HDC dc, RECT *rect, card_ptr_t *card, uint version, uint param_5, bool expanded_text, LPCSTR param_7);

#ifdef __cplusplus
}
#endif

#endif
