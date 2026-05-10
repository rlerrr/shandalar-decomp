#ifndef SHANDALAR_SIDLIB_PIC_H
#define SHANDALAR_SIDLIB_PIC_H

#include <windows.h>

typedef struct DIBSurface
{
  HANDLE hMapping;         // 0x00
  HDC hTempDC;             // 0x04 (used only during creation)
  HBITMAP hBitmap;         // 0x08
  HGDIOBJ hPreviousBitmap; // 0x0c
  BITMAPINFO *pBitmapInfo; // 0x10
  HPALETTE hPalette;       // 0x14
  void *pBits;             // 0x18
  DWORD imageSizeBytes;    // 0x1C
  int width;               // 0x20
  int height;              // 0x24
  int bitsPerPixel;        // 0x28
  int rowPadding;          // 0x2C
} DIBSurface;

HBITMAP load_pic(char *filename);
BITMAPINFO *CreateBitmapInfo(int width, int height, int bitsPerPixel);

#endif
