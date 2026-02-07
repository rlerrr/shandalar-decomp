#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <windows.h>

void ApplyCardArtPaletteToDc(HDC hdc);
void ShutdownCardArtGdiResources(void);
BOOL InitCardArtGdiResources(void);
BOOL DrawBitmapToRect(HDC dst_dc,const RECT *dst_rect,HBITMAP bitmap);

#endif