#ifndef SHANDALAR_SIDLIB_PCXW_H
#define SHANDALAR_SIDLIB_PCXW_H

#include <stdio.h>
#include "inttypes.h"
#include "mystdbool.h"

/* Globals shared with callers (kept for decomp matching). */
extern undefined4 gImageWidth;
extern undefined4 gImageHeight;
extern FILE *gPcxInFile;
extern char *gPcxPath;

/* PCXW APIs */
undefined1 * PcxLoad8bppImage(char *path, undefined1 *imagePixels, void *paletteOut);
bool PcxReadHeaderAndPaletteFromPath(char *path, void *paletteOut);
undefined4 PcxReadHeaderAndPalette(void *paletteOut);
undefined4 PcxReadScanlineRle(char *dstScanline);
undefined4 PcxSave8bppImage(undefined *srcPixels, char *path, void *palette, undefined4 unused_param_4,
                            undefined4 unused_param_5, int width, int height);
undefined4 PcxWriteHeader(undefined4 width, short height);
undefined4 PcxWriteScanlineRle(char *srcScanline, int widthBytes);
void PcxWriteRleByte(char value);
int CountRepeats(char value, char *buffer, int maxCount);
undefined4 PcxWritePalette256(void *palette);

#endif
