#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <windows.h>

typedef struct PaletteLog {
    WORD palVersion;
    WORD palNumEntries;
    PALETTEENTRY palPalEntry[0x100];
} PaletteLog;

void InitBitmapInfo24bppTopDown(BITMAPINFO *bmi, int width, int height);
void ApplyCardArtPaletteToDc(HDC hdc);
void ShutdownCardArtGdiResources(void);
BOOL InitCardArtGdiResources(void);
BOOL DrawBitmapToRect(HDC dst_dc, const RECT *dst_rect, HBITMAP bitmap);
BOOL DrawBitmapSubrectToRect(HDC dst_dc, const RECT *dst_rect, HBITMAP bitmap, int src_x, int src_y,
                             int src_width, int src_height);
BOOL CreateOffscreen32bppDibSection(int width, int height, HDC *out_dc, BITMAPINFO *bmi_optional,
                                    HBITMAP *out_bitmap, HGDIOBJ *out_prev_object, void **out_bits);
BOOL SetupDuelPalette(void);
COLORREF GetPaletteColor(int index);

extern CRITICAL_SECTION global_critical_section_for_drawing;
extern HDC global_screen_dc;
extern RGBQUAD g_cardArtPalette[0x100];

#endif
