#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <windows.h>

void InitBitmapInfo24bppTopDown(BITMAPINFO *bmi,int width,int height);
void ApplyCardArtPaletteToDc(HDC hdc);
void ShutdownCardArtGdiResources(void);
BOOL InitCardArtGdiResources(void);
BOOL DrawBitmapToRect(HDC dst_dc,const RECT *dst_rect,HBITMAP bitmap);
BOOL DrawBitmapSubrectToRect(HDC dst_dc,const RECT *dst_rect,HBITMAP bitmap,int src_x,int src_y,
                                   int src_width,int src_height);
BOOL SetupDuelPalette(void);

extern CRITICAL_SECTION global_critical_section_for_drawing;
extern HDC global_screen_dc;
extern RGBQUAD g_cardArtPalette[0x100];

#endif