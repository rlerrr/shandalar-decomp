#include <windows.h>
#include <wingdi.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "inttypes.h"
#include "mystdbool.h"
#include "defs.h"
#include "assert.h"
#include "palette.h"

typedef struct OctNode OctNode;

// SIZE 0x30
typedef struct OctNode {
    unsigned int      flags;          /* +0 : low byte == 1 => leaf */
    unsigned int      palette_idx;    /* +4 */
    OctNode           *children[8];   /* +8 */
    byte              *list;          /* +0x28 */
    int               list_count;     /* +0x2C */
};
STATIC_ASSERT(sizeof(OctNode) == 0x30, OctNode_wrong_size);

STATIC_ASSERT(sizeof(PaletteLog) == 0x404, PaletteLog_wrong_size);

/* Forward declarations for globals referenced before their definitions. */
void checked_DeleteDC_DeleteObject(HDC dc,HGDIOBJ obj);
void DestroyCardArtPalette(void);
PaletteLog * ReadPalette(char *palette_text_path,char *palette_binary_path);
void DestroyPaletteOctree(void);
BOOL CreateOffscreen32bppDibSection(int width,int height,HDC *out_dc,BITMAPINFO *bmi_optional,
                                           HBITMAP *out_bmp_optional,HGDIOBJ *obj_to_select_optional,void **out_bits_optional);
int Octree_Destroy(OctNode *node);
undefined4 Octree_InsertPathString(OctNode *node,char *path_str,unsigned int palette_index);
int OctreeNode_FinalizeSubtree(OctNode *node);
undefined4 InitOctreeBitTables(void);
bool InitDiffSquaredLookupTable(void);

extern int g_diff_squared_table[0x200];
extern undefined1 global_RedPathBitsTable[0x800];
extern undefined1 global_GreenPathBitsTable[0x800];
extern undefined1 global_BluePathBitsTable[0x800];

// GLOBAL: CARDARTLIB 0x1001d0e4
// GLOBAL: DRAWCARDLIB 0x10022500
// GLOBAL: DECKDLL 0x10035b38
// GLOBAL: MAGIC 0x00571d18
// GLOBAL: SHANDALAR 0x00585920
HDC global_screen_dc = (HDC)0x0;

// GLOBAL: CARDARTLIB 0x1001d240
// GLOBAL: DRAWCARDLIB 0x10021030
// GLOBAL: DECKDLL 0x10031930
// GLOBAL: SHANDALAR 0x00581ea0
int g_octree_depth_max = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d244
// GLOBAL: DRAWCARDLIB 0x10021034
// GLOBAL: DECKDLL 0x10031934
// GLOBAL: SHANDALAR 0x00581ea4
int * g_diff_squared_table_mid_ptr = &g_diff_squared_table[0xFF];

// GLOBAL: CARDARTLIB 0x1001d248
// GLOBAL: DRAWCARDLIB 0x10021038
// GLOBAL: DECKDLL 0x10031938
// GLOBAL: SHANDALAR 0x00581ea8
undefined4 global_diff_squared_initialized = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d24c
// GLOBAL: DRAWCARDLIB 0x1002103c
// GLOBAL: DECKDLL 0x1003193c
// GLOBAL: SHANDALAR 0x00581eac
undefined1 * global_PTR_BluePathBitsTable = global_BluePathBitsTable;

// GLOBAL: CARDARTLIB 0x1001d250
// GLOBAL: DRAWCARDLIB 0x10021040
// GLOBAL: DECKDLL 0x10031940
// GLOBAL: SHANDALAR 0x00581eb0
undefined1 * global_PTR_GreenPathBitsTable = global_GreenPathBitsTable;

// GLOBAL: CARDARTLIB 0x1001d254
// GLOBAL: DRAWCARDLIB 0x10021044
// GLOBAL: DECKDLL 0x10031944
// GLOBAL: SHANDALAR 0x00581eb4
undefined1 * global_PTR_RedPathBitsTable = global_RedPathBitsTable;

// GLOBAL: CARDARTLIB 0x100209e0
// GLOBAL: DRAWCARDLIB 0x100f1f70
// GLOBAL: DECKDLL 0x10113ce8
// GLOBAL: MAGIC 0x00777844
// GLOBAL: SHANDALAR 0x0078e5d0
HPALETTE global_cart_art_hpalette;

// GLOBAL: CARDARTLIB 0x100209e4
// GLOBAL: DRAWCARDLIB 0x100f1f74
// GLOBAL: DECKDLL 0x10104df0
// GLOBAL: MAGIC 0x00638640
// GLOBAL: SHANDALAR 0x005a90b8
HBITMAP g_offscreen_bitmap;

// GLOBAL: CARDARTLIB 0x100209e8
// GLOBAL: DRAWCARDLIB 0x100f1f78
// GLOBAL: DECKDLL 0x10142f50
// GLOBAL: MAGIC 0x008b49d0
// GLOBAL: SHANDALAR 0x008c8b50
RGBQUAD g_cardArtPalette[0x100];

// GLOBAL: CARDARTLIB 0x10020de8
// GLOBAL: DRAWCARDLIB 0x100f2378
// GLOBAL: DECKDLL 0x10104e38
// GLOBAL: MAGIC 0x00638688
// GLOBAL: SHANDALAR 0x005a9100
CRITICAL_SECTION global_critical_section_for_drawing;

// GLOBAL: CARDARTLIB 0x10116cf0
// GLOBAL: DRAWCARDLIB 0x10152020
// GLOBAL: DECKDLL 0x101e6a60
// GLOBAL: SHANDALAR 0x0097bdd0
PaletteLog g_palette_log;

// GLOBAL: CARDARTLIB 0x10031ea4
// GLOBAL: DRAWCARDLIB 0x10028ef4
// GLOBAL: DECKDLL 0x100f2c6c
// GLOBAL: SHANDALAR 0x005a86f4
OctNode* g_paletteOctreeRoot;

// GLOBAL: CARDARTLIB 0x10031eb0
// GLOBAL: DRAWCARDLIB 0x10028f00
// GLOBAL: DECKDLL 0x100f2c78
undefined1 g_octree_path_tmp_alt[8];

// GLOBAL: CARDARTLIB 0x10031eb8
// GLOBAL: DRAWCARDLIB 0x10028f08
// GLOBAL: DECKDLL 0x100f2c80
// GLOBAL: SHANDALAR 0x005a8708
int g_octree_leaf_count;

// GLOBAL: CARDARTLIB 0x10031ec0
// GLOBAL: DRAWCARDLIB 0x10028f10
// GLOBAL: DECKDLL 0x100f2c88
// GLOBAL: SHANDALAR 0x005a8710
uint g_paletteRgbTable[0x100];

// GLOBAL: CARDARTLIB 0x100322c0
// GLOBAL: DRAWCARDLIB 0x10029310
// GLOBAL: DECKDLL 0x100f3088
// GLOBAL: SHANDALAR 0x005a8b10
undefined1 g_octree_path_tmp[8];

// GLOBAL: CARDARTLIB 0x100322d4
// GLOBAL: DRAWCARDLIB 0x10029324
// GLOBAL: DECKDLL 0x100f309c
// GLOBAL: SHANDALAR 0x005a8b24
int g_octree_depth_current;

// GLOBAL: CARDARTLIB 0x100ecb10
// GLOBAL: DRAWCARDLIB 0x10127e40
// GLOBAL: DECKDLL 0x101bc880
// GLOBAL: SHANDALAR 0x00951bf0
undefined1 global_RedPathBitsTable[0x800];

// GLOBAL: CARDARTLIB 0x100ed310
// GLOBAL: DRAWCARDLIB 0x10128640
// GLOBAL: DECKDLL 0x101bd080
// GLOBAL: SHANDALAR 0x009523f0
int g_diff_squared_table[0x200];

// GLOBAL: CARDARTLIB 0x10115cf0
// GLOBAL: DRAWCARDLIB 0x10151020
// GLOBAL: DECKDLL 0x101e5a60
// GLOBAL: SHANDALAR 0x0097add0
undefined1 global_GreenPathBitsTable[0x800];

// GLOBAL: CARDARTLIB 0x101164f0
// GLOBAL: DRAWCARDLIB 0x10151820
// GLOBAL: DECKDLL 0x101e6260
// GLOBAL: SHANDALAR 0x0097b5d0
undefined1 global_BluePathBitsTable[0x800];

// MATCHING
// FUNCTION: SHANDALAR 0x00462920
// FUNCTION: CARDARTLIB 0x10001220
// FUNCTION: DRAWCARDLIB 0x1000a5a0
// FUNCTION: DECKDLL 0x100217b0
// FUNCTION: MAGIC 0x00491e80
BOOL InitCardArtGdiResources(void)
{
#if defined(CARDARTLIB) || defined(DRAWCARDLIB)
  BOOL result = 1;
  
  if (!SetupDuelPalette())
    result = 0;
#endif

  if (global_screen_dc == 0) {
    CreateOffscreen32bppDibSection(10,10,(HDC *)&global_screen_dc,(BITMAPINFO *)0x0,
                                  (HBITMAP *)&g_offscreen_bitmap,(HGDIOBJ *)0x0,(void **)0x0);
    InitializeCriticalSection(&global_critical_section_for_drawing);
  }

#if defined(CARDARTLIB) || defined(DRAWCARDLIB)
  if (global_screen_dc == 0)
    result=0;
    
  if (result == 0)
    ShutdownCardArtGdiResources();

  return result;
#else  
  if (global_screen_dc == 0)
    return 0;
  
  return 1;
#endif
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100012a8
// FUNCTION: DRAWCARDLIB 0x1000a628
// FUNCTION: DECKDLL 0x1002180d
// FUNCTION: MAGIC 0x00491edd
// FUNCTION: SHANDALAR 0x0046297d
void ShutdownCardArtGdiResources(void)
{
  if (global_screen_dc != (HDC)0x0) {
    checked_DeleteDC_DeleteObject(global_screen_dc,(HGDIOBJ)g_offscreen_bitmap);
    global_screen_dc = (HDC)0x0;
    DeleteCriticalSection(&global_critical_section_for_drawing);
  }
#if defined(CARDARTLIB) || defined(DRAWCARDLIB)
  if (global_cart_art_hpalette != 0) {
    DestroyCardArtPalette();
  }
#endif
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100012fb
// FUNCTION: DRAWCARDLIB 0x1000a67b
// FUNCTION: DECKDLL 0x1002349a
// FUNCTION: MAGIC 0x00493bfa
// FUNCTION: SHANDALAR 0x0046469a
void ApplyCardArtPaletteToDc(HDC hdc)
{
  SelectPalette(hdc,global_cart_art_hpalette,0);
  RealizePalette(hdc);
  GdiFlush();
  SetDIBColorTable(hdc,0,0x100,g_cardArtPalette);
  SetStretchBltMode(hdc,COLORONCOLOR);
}

// MATCHING
// FUNCTION: SHANDALAR 0x00464703
// FUNCTION: CARDARTLIB 0x1000134a
// FUNCTION: DRAWCARDLIB 0x1000a6ca
// FUNCTION: DECKDLL 0x10023503
// FUNCTION: MAGIC 0x00493c63
BOOL CreateOffscreen32bppDibSection(int width,int height,HDC *out_dc,BITMAPINFO *bmi_optional,
                                          HBITMAP *out_bitmap,HGDIOBJ *out_prev_object,void **out_bits)
{
  //Stack layout won't behave
  struct {
    HBITMAP bitmap;
    HDC screen_dc;
    HDC memory_dc;
    BITMAPINFO bmi;
    HGDIOBJ prev_object;
    void *bits;
  } s;

  s.memory_dc = (HDC)0x0;
  s.bitmap = (HBITMAP)0x0;
  s.bits = (void *)0x0;
  if ((out_dc == (HDC *)0x0) || (out_bitmap == (HBITMAP *)0x0)) {
    return 0;
  }
 
  if (bmi_optional == (BITMAPINFO *)0x0) {
    bmi_optional = &s.bmi;
  }
  s.screen_dc = GetDC((HWND)0x0);
  if (s.screen_dc != (HDC)0x0) {
    ApplyCardArtPaletteToDc(s.screen_dc);
    s.memory_dc = CreateCompatibleDC(s.screen_dc);
    if (s.memory_dc != (HDC)0x0) {
      InitBitmapInfo24bppTopDown(bmi_optional,width,height);
      s.bmi.bmiHeader.biBitCount = 0x20;
      s.bitmap = CreateDIBSection(s.screen_dc,bmi_optional,0,&s.bits,(HANDLE)0x0,0);
      s.prev_object = SelectObject(s.memory_dc,s.bitmap);
      ApplyCardArtPaletteToDc(s.memory_dc);
    }
    ReleaseDC((HWND)0x0,s.screen_dc);
  }
  if (((s.memory_dc == (HDC)0x0) || (s.bitmap == (HBITMAP)0x0)) || (s.bits == (void *)0x0)) {
    if (s.memory_dc != (HDC)0x0) {
      DeleteDC(s.memory_dc);
    }
    if (s.bitmap != (HBITMAP)0x0) {
      DeleteObject(s.bitmap);
    }
    return 0;
  }
  else {
    if (out_dc != 0)
      *out_dc = s.memory_dc;

    if (out_bitmap != 0)
      *out_bitmap = s.bitmap;
    if (out_prev_object != (HGDIOBJ *)0x0) {
      *out_prev_object = s.prev_object;
    }
    if (out_bits != (void **)0x0) {
      *out_bits = s.bits;
    }
    return 1;
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100014d2
// FUNCTION: DRAWCARDLIB 0x1000a852
// FUNCTION: DECKDLL 0x1002368b
// FUNCTION: MAGIC 0x00493deb
// FUNCTION: SHANDALAR 0x0046488b
void checked_DeleteDC_DeleteObject(HDC dc,HGDIOBJ obj)
{
  if (dc != (HDC)0x0) {
    DeleteDC(dc);
  }
  if (obj != (HGDIOBJ)0x0) {
    DeleteObject(obj);
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001505
// FUNCTION: DRAWCARDLIB 0x1000a885
// FUNCTION: DECKDLL 0x100237c0
// FUNCTION: MAGIC 0x00493f24
// FUNCTION: SHANDALAR 0x004649c0
BOOL DrawBitmapToRect(HDC dst_dc,const RECT *dst_rect,HBITMAP bitmap)
{
  BITMAP bm;
  
  if (((dst_dc == (HDC)0x0) || (dst_rect == (RECT *)0x0)) || (bitmap == (HBITMAP)0x0)) {
    return 0;
  }
  GetObjectA(bitmap,sizeof(bm),&bm);
  return DrawBitmapSubrectToRect(dst_dc,dst_rect,bitmap,0,0,bm.bmWidth,bm.bmHeight);
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000156d
// FUNCTION: DRAWCARDLIB 0x1000a8ed
// FUNCTION: DECKDLL 0x10023828
// FUNCTION: MAGIC 0x00493f8c
// FUNCTION: SHANDALAR 0x00464a28
BOOL DrawBitmapSubrectToRect(HDC dst_dc,const RECT *dst_rect,HBITMAP bitmap,int src_x,int src_y,
                                   int src_width,int src_height)
{
  //Stack layout won't behave
  struct {
    HGDIOBJ h;
    int dst_height;
    BITMAP bm;
    int dst_width;
    int dst_top;
    int dst_left;
  } v;
  
  if (((dst_dc == (HDC)0x0) || (dst_rect == (RECT *)0x0)) || (bitmap == (HBITMAP)0x0)) {
    return 0;
  }

  EnterCriticalSection(&global_critical_section_for_drawing);
  v.h = SelectObject(global_screen_dc,bitmap);
  GetObjectA(bitmap,sizeof(v.bm),&v.bm);

  v.dst_left = dst_rect->left;
  v.dst_top = dst_rect->top;
  v.dst_width =  (dst_rect->right < dst_rect->left) ? v.bm.bmWidth : dst_rect->right - dst_rect->left;

  v.dst_height = (dst_rect->bottom < dst_rect->top) ? v.bm.bmHeight : dst_rect->bottom - dst_rect->top;

  ApplyCardArtPaletteToDc(global_screen_dc);

  StretchBlt(dst_dc,v.dst_left,v.dst_top,v.dst_width,v.dst_height,global_screen_dc,src_x,src_y,
    src_width <= v.bm.bmWidth ? src_width : v.bm.bmWidth,
    src_height <= v.bm.bmHeight ? src_height : v.bm.bmHeight, 0xcc0020);
  SelectObject(global_screen_dc,v.h);
  LeaveCriticalSection(&global_critical_section_for_drawing);
  return 1;
}

// MATCHING
// FUNCTION: SHANDALAR 0x00465473
// FUNCTION: CARDARTLIB 0x100016b7
// FUNCTION: DRAWCARDLIB 0x1000acf5
// FUNCTION: DECKDLL 0x10024273
// FUNCTION: MAGIC 0x004949d9
BOOL SetupDuelPalette(void)
{
  //Stack layout won't behave
  struct {
    PALETTEENTRY entry;
    uint palette_text_path[66];
    undefined4 success;
    UINT entry_count;
    UINT i;
    uint palette_bin_path[66];
    LOGPALETTE *log_palette;
    PALETTEENTRY entries[256];
  } s;
  
  s.success = 1;
  strcpy((char *)s.palette_text_path,global_base_directory);
  strcat((char *)s.palette_text_path,"\\DUELPALall.TR");
  strcpy((char *)s.palette_bin_path,global_base_directory);
  strcat((char *)s.palette_bin_path,"\\DUEL.plogpal");
  s.log_palette = (LOGPALETTE *)ReadPalette((char *)s.palette_text_path,(char *)s.palette_bin_path);
  if (s.log_palette != (LOGPALETTE *)0x0) {
    for (s.i = 1; (int)s.i < 0xff; s.i = s.i + 1) {
      s.log_palette->palPalEntry[s.i].peFlags = '\x04';
    }
    global_cart_art_hpalette = CreatePalette(s.log_palette);
    if (global_cart_art_hpalette != (HPALETTE)0x0) {
      s.entry.peRed = 0xff;
      s.entry.peGreen = 0xff;
      s.entry.peBlue = 0xff;
      s.entry.peFlags = '\0';
      SetPaletteEntries(global_cart_art_hpalette,0xff,1,&s.entry);
      s.entry.peRed = 0xfe;
      s.entry.peGreen = 0xfe;
      s.entry.peBlue = 0xfe;
      s.entry.peFlags = '\x04';
      SetPaletteEntries(global_cart_art_hpalette,0xbf,1,&s.entry);
      for (s.i = 0xec; (int)s.i < 0xff; s.i = s.i + 1) {
        s.entry.peRed = '\x01';
        s.entry.peGreen = '\x01';
        s.entry.peBlue = '\x01';
        s.entry.peFlags = '\x04';
        SetPaletteEntries(global_cart_art_hpalette,s.i,1,&s.entry);
      }
      s.entry_count = GetPaletteEntries(global_cart_art_hpalette,0,0x100,s.entries);
      for (s.i = 0; (int)s.i < (int)s.entry_count; s.i = s.i + 1) {
        g_cardArtPalette[s.i].rgbBlue = s.entries[s.i].peBlue;
        g_cardArtPalette[s.i].rgbGreen = s.entries[s.i].peGreen;
        g_cardArtPalette[s.i].rgbRed = s.entries[s.i].peRed;
        g_cardArtPalette[s.i].rgbReserved = 0;
      }
      
      for (s.i = s.entry_count; (int)s.i < 0x100; s.i++) {
        g_cardArtPalette[s.i].rgbBlue = 0;
        g_cardArtPalette[s.i].rgbGreen = 0;
        g_cardArtPalette[s.i].rgbRed = 0;
        g_cardArtPalette[s.i].rgbReserved = 0;
      }
    }
    else
      s.success=0;
  }
  else
    s.success=0;

  return (BOOL)s.success;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100019a8
// FUNCTION: DRAWCARDLIB 0x1000afe6
// FUNCTION: DECKDLL 0x10024564
// FUNCTION: MAGIC 0x00494cca
// FUNCTION: SHANDALAR 0x00465764
void DestroyCardArtPalette(void)
{
  DeleteObject(global_cart_art_hpalette);
  global_cart_art_hpalette = (HGDIOBJ)0x0;
  DestroyPaletteOctree();
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003c70
// FUNCTION: DRAWCARDLIB 0x1000a520
// FUNCTION: DECKDLL 0x10016200
// FUNCTION: MAGIC 0x004ecd40
// FUNCTION: SHANDALAR 0x004a3110
void InitBitmapInfo24bppTopDown(BITMAPINFO *bmi,int width,int height)
{
  bmi->bmiHeader.biSize = sizeof(bmi->bmiHeader);
  bmi->bmiHeader.biWidth = width;
  bmi->bmiHeader.biHeight = -height;
  bmi->bmiHeader.biPlanes = 1;
  bmi->bmiHeader.biBitCount = 0x18;
  bmi->bmiHeader.biCompression = 0;
  bmi->bmiHeader.biSizeImage = 0;
  bmi->bmiHeader.biXPelsPerMeter = 0;
  bmi->bmiHeader.biYPelsPerMeter = 0;
  bmi->bmiHeader.biClrUsed = 0x100;
  bmi->bmiHeader.biClrImportant = 0x100;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10004a50
// FUNCTION: DRAWCARDLIB 0x10001000
// FUNCTION: DECKDLL 0x100099b0
// FUNCTION: MAGIC 0x004b9c20
// FUNCTION: SHANDALAR 0x0042df50
void * OctreeNode_Create(void)
{
  void *_Dst;
  
  _Dst = malloc(0x30);
  memset(_Dst,0,0x30);
  return _Dst;
}

// FUNCTION: CARDARTLIB 0x10004a83
// FUNCTION: DRAWCARDLIB 0x10001033
// FUNCTION: DECKDLL 0x100099e4
// FUNCTION: MAGIC 0x004b9c54
// FUNCTION: SHANDALAR 0x0042df84
PaletteLog * ReadPalette(char *palette_text_path,char *palette_binary_path)
{
  struct {
    uint rgb_color;
    char line[256];
    int palette_index;
    int green;
    FILE *palette_file;
    char *path_start;
    int red;
    uint blue;
    PaletteLog *palette_data;
  } s;
  
  s.palette_index = 0;
  s.palette_data = &g_palette_log;
  s.palette_data->palVersion = 0x300;
  s.palette_file = fopen(palette_text_path,"rt");
  if (s.palette_file == (FILE *)0x0) {
    return 0;
  }

  if (g_paletteOctreeRoot != (OctNode *)0x0) {
    Octree_Destroy(g_paletteOctreeRoot);
  }
  g_paletteOctreeRoot = OctreeNode_Create();
#ifdef MODERN_FIXES
  while (fgets(s.line,0xff,s.palette_file) != (char *)0x0) {
#else
  fgets(s.line,0xff,s.palette_file);
  while ((s.palette_file->_flag & 0x10) == 0) {
#endif
    sscanf(s.line,"%d - %d %d %d",&s.palette_index,&s.red,&s.green,&s.blue);
    s.path_start = strchr(s.line,0x2d) + 1;
    s.path_start = strchr(s.path_start,0x2d) + 1;

    Octree_InsertPathString(g_paletteOctreeRoot,s.path_start,s.palette_index);
    s.rgb_color = s.red << 0x10 | s.green << 8 | s.blue;
    ((uint *)g_paletteRgbTable)[s.palette_index] = s.rgb_color;
    s.palette_data->palPalEntry[s.palette_index].peRed = (BYTE)s.red;
    s.palette_data->palPalEntry[s.palette_index].peGreen = (BYTE)s.green;
    s.palette_data->palPalEntry[s.palette_index].peBlue = (BYTE)s.blue;
    if ((s.palette_index == 0) || (s.palette_index == 0xff)) {
      s.palette_data->palPalEntry[s.palette_index].peFlags = 0;
    }
    else {
      s.palette_data->palPalEntry[s.palette_index].peFlags = 1;
    }
#ifndef MODERN_FIXES
    fgets(s.line,0xff,s.palette_file);
#endif
  }
  g_octree_depth_current = 0;
  g_octree_depth_max = g_octree_depth_current;
  g_octree_leaf_count = OctreeNode_FinalizeSubtree(g_paletteOctreeRoot);
  g_octree_depth_max = g_octree_depth_max + -1;
  s.palette_data->palNumEntries = 0x100;
  fclose(s.palette_file);
  s.palette_file = (FILE *)0x0;
  if (palette_binary_path != (char *)0x0) {
    s.palette_file = fopen(palette_binary_path,"rb");
  }
  if (s.palette_file != (FILE *)0x0) {
    fread(&g_palette_log,0x404,1,s.palette_file);
    fclose(s.palette_file);
  }
  s.palette_data->palPalEntry[0xff].peFlags = 0;
  s.palette_data->palPalEntry[0].peFlags = s.palette_data->palPalEntry[0xff].peFlags;
  InitOctreeBitTables();
  InitDiffSquaredLookupTable();

  return s.palette_data;
}

// FUNCTION: CARDARTLIB 0x10004cd7
// FUNCTION: DRAWCARDLIB 0x10001287
// FUNCTION: DECKDLL 0x10009c42
// FUNCTION: MAGIC 0x004b9eb2
// FUNCTION: SHANDALAR 0x0042e1e2
bool InitDiffSquaredLookupTable(void)
{
  int diff;
  int i;
  
  if (global_diff_squared_initialized != 0) 
    return 0;

  for (i = 0, diff = -0xff; i < 0x200; i++, diff++) {
    g_diff_squared_table[i] = diff * diff;
  }
  global_diff_squared_initialized = 1;

  return 1;
}

// FUNCTION: CARDARTLIB 0x10004d49
// FUNCTION: DRAWCARDLIB 0x100012f9
// FUNCTION: DECKDLL 0x10009cb4
// FUNCTION: MAGIC 0x004b9f24
// FUNCTION: SHANDALAR 0x0042e254
void OctreeNode_CollectLeafIndices(OctNode *node,int out_base,int *out_index)
{
  int i;
  
  if (node->flags != 0) {
    *(char *)(*out_index + out_base) = (char)node->palette_idx;
    *out_index = *out_index + 1;   
  }
  else {
     for (i = 0; i < 8; i = i + 1) {
      if (node->children[i] != 0) {
        OctreeNode_CollectLeafIndices(node->children[i],out_base,out_index);
      }
    }
  }
}

// FUNCTION: CARDARTLIB 0x10004dc8
// FUNCTION: DRAWCARDLIB 0x10001378
// FUNCTION: DECKDLL 0x10009d33
// FUNCTION: MAGIC 0x004b9fa3
// FUNCTION: SHANDALAR 0x0042e2d3
int OctreeNode_FinalizeSubtree(OctNode *node)
{
  struct {
    size_t list_count;
    int i;
    int leaf_total;
    undefined1 leaf_list[1024];
  } s;
  
  s.leaf_total = 0;
  s.list_count = 0;
  g_octree_depth_current = g_octree_depth_current + 1;
  if (g_octree_depth_max < g_octree_depth_current) {
    g_octree_depth_max = g_octree_depth_current;
  }
  if (node->flags != 0) {
    g_octree_depth_current--;
    return 1;
  }

  for (s.i = 0; s.i < 8; s.i = s.i + 1) {
    if (node->children[s.i] != 0) {
      s.leaf_total = s.leaf_total + OctreeNode_FinalizeSubtree(node->children[s.i]);
      s.list_count = s.list_count + 1;
    }
  }
  if (s.list_count != 0) {
    s.list_count = 0;
    OctreeNode_CollectLeafIndices(node,(int)s.leaf_list,(int *)&s.list_count);
    node->list = malloc(s.list_count);
    node->list_count = s.list_count;
    memcpy(node->list,s.leaf_list,s.list_count);
  }
  g_octree_depth_current = g_octree_depth_current + -1;

  return s.leaf_total;
}

// FUNCTION: CARDARTLIB 0x10004f07
// FUNCTION: DRAWCARDLIB 0x100014b7
// FUNCTION: DECKDLL 0x10009e73
// FUNCTION: SHANDALAR 0x0042e413
undefined4 Octree_InsertPathString(OctNode *node,char *path_str,unsigned int palette_index)
{
  int child_index;

  path_str += strspn(path_str," \n");
  while (*path_str != '\0') {
    child_index = atoi(path_str);
    if (node->children[child_index] == 0) {
      node->children[child_index] = OctreeNode_Create();
    }
    node = node->children[child_index];

    path_str += strspn(path_str +=  
        strcspn(path_str + strspn(path_str," \n")," \n") + 
        strspn(path_str," \n")
      ," \n");
  }
  node->flags = 1;
  node->palette_idx = palette_index;
  return 0;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10004fe1
// FUNCTION: DRAWCARDLIB 0x10001591
// FUNCTION: DECKDLL 0x10009f53
// FUNCTION: MAGIC 0x004ba1c3
// FUNCTION: SHANDALAR 0x0042e4f3
int Octree_Destroy(OctNode *node)
{
  int i;
  int result;
  
  result = 0;
  if (node->flags != 0) {   
      free(node);
      return 1;
  }

  for (i = 0; i < 8; i = i + 1) {
    if (node->children[i] != 0) {
      result = result + Octree_Destroy(node->children[i]);
    }
  }
  if (node->list != 0) {
    free(node->list);
  }
  free(node);

  return result;
}

// FUNCTION: CARDARTLIB 0x1000508d
// FUNCTION: DRAWCARDLIB 0x1000163d
// FUNCTION: DECKDLL 0x1000a002
// FUNCTION: SHANDALAR 0x0042e5a2
void Octree_BuildPathBytesFromRgb(uint rgb_color,undefined8 *out_path_words)
{
  struct {
    int green;
    int blue;
    int red;
  } s;
  
  s.blue = (rgb_color & 0xff);
  s.green = ((RGBQUAD *)&rgb_color)->rgbGreen;
  s.red = ((rgb_color & 0xff0000) >> 0x10);

  *out_path_words = ((undefined8 *)global_RedPathBitsTable)[s.red] | 
    ((undefined8 *)global_GreenPathBitsTable)[s.green] | 
    ((undefined8 *)global_BluePathBitsTable)[s.blue];
}

// FUNCTION: CARDARTLIB 0x100050f1
// FUNCTION: DRAWCARDLIB 0x100016a1
// FUNCTION: DECKDLL 0x1000a066
// FUNCTION: SHANDALAR 0x0042e606
undefined4 InitOctreeBitTables(void)
{
  struct {
    int bit_mask; /* ebp - 0xc */
    int bit;      /* ebp - 0x8 */
    int value;    /* ebp - 0x4 */
  } s;
  
  for (s.value = 0; s.value < 0x100; s.value++) {
    for (s.bit = 0, s.bit_mask = 0x80; s.bit < 8; s.bit++, s.bit_mask >>= 1) {
      global_PTR_BluePathBitsTable[s.bit + s.value * 8] = (s.bit_mask & s.value) != 0 ? 4 : 0;

      global_PTR_GreenPathBitsTable[s.bit + s.value * 8] = (s.bit_mask & s.value) != 0 ? 2 : 0;

      global_PTR_RedPathBitsTable[s.bit + s.value * 8] = (s.bit_mask & s.value) != 0 ? 1 : 0;
    }
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x100051f5
// FUNCTION: DRAWCARDLIB 0x100017a5
// FUNCTION: DECKDLL 0x1000a169
// FUNCTION: MAGIC 0x004ba3da
// FUNCTION: SHANDALAR 0x0042e709
undefined4 Octree_FindNearestColor(uint rgb_color)
{
  struct {
    int pal_r;        /* ebp - 0x34 */
    int best_distance;/* ebp - 0x30 */
    byte *index_list; /* ebp - 0x2c */
    uint best_index;  /* ebp - 0x28 */
    int i;            /* ebp - 0x24 */
    int green;        /* ebp - 0x20 */
    int distance;     /* ebp - 0x1c */
    int pal_g;        /* ebp - 0x18 */
    int blue;         /* ebp - 0x14 */
    int red;          /* ebp - 0x10 */
    int pal_b;        /* ebp - 0xc */
    OctNode *node;    /* ebp - 8 */
    byte *path;       /* ebp - 4 */
  } s;

  s.path = (byte *)&g_octree_path_tmp;
  s.node = g_paletteOctreeRoot;

  Octree_BuildPathBytesFromRgb(rgb_color,(undefined8 *)&g_octree_path_tmp);

#ifdef MODERN_FIXES
  while (1) {
    OctNode *child = s.node->children[*s.path];
    s.path++;
    if (child == 0) {
      break;
    }
    s.node = child;
    if ((byte)s.node->flags == 1) {
      return g_paletteRgbTable[s.node->palette_idx];
    }
  }
#else
  __asm {
    mov eax, g_paletteOctreeRoot
    mov ebx, offset g_octree_path_tmp
    xor ecx, ecx
  loop_start:
    mov edx, eax
    mov cl, byte ptr [ebx]
    mov eax, dword ptr [eax + ecx*4 + 8]
    add ebx, 1
    cmp eax, 0
    je loop_end
    cmp byte ptr [eax], 1
    jne loop_start
    mov ebx, dword ptr [eax + 4]
    mov edi, offset g_paletteRgbTable
    mov eax, dword ptr [edi + ebx*4]
    jmp end
  loop_end:
    mov s.node, edx
  }
#endif
  
  if (s.node->flags == 0) {
    s.index_list = s.node->list;
    s.best_distance = 0x7fffffff;
    s.blue = (int)(rgb_color & 0xff);
    s.green = (int)((byte *)&rgb_color)[1];
    s.red = (int)((rgb_color & 0xff0000) >> 0x10);

    for (s.i = 0; s.i < s.node->list_count; s.i = s.i + 1) {
      s.pal_b = g_paletteRgbTable[s.index_list[s.i]] & 0xff;

      s.pal_g = ((RGBQUAD *)&g_paletteRgbTable[s.index_list[s.i]])->rgbGreen;

      s.pal_r = (g_paletteRgbTable[s.index_list[s.i]] & 0xff0000) >> 0x10;

      s.distance = g_diff_squared_table_mid_ptr[s.green - s.pal_g] +
                g_diff_squared_table_mid_ptr[s.blue - s.pal_b] +
                g_diff_squared_table_mid_ptr[s.red - s.pal_r];

      if (s.distance < s.best_distance) {
        s.best_index = s.index_list[s.i];
        s.best_distance = s.distance;
      }
    }
    return g_paletteRgbTable[s.best_index];
  }
  else {
    return g_paletteRgbTable[s.node->palette_idx];
  }

#ifndef MODERN_FIXES
end:  ;
#endif
}

// FUNCTION: CARDARTLIB 0x10005383
// FUNCTION: DRAWCARDLIB 0x10001933
// FUNCTION: DECKDLL 0x1000a2f7
// FUNCTION: MAGIC 0x004ba568
uint Octree_FindNearestPaletteIndex(uint rgb_color)
{
  struct {
    int pal_blue;      // edp - 0x34
    int best_distance; // edp - 0x30
    byte *index_list;  // edp - 0x2c
    uint best_index;   // edp - 0x28
    int i;             // ebp - 0x24
    int pal_g;         // ebp - 0x20
    int distance;      // ebp - 0x1c
    int pal_green;     // ebp - 0x18
    int pal_b;         // ebp - 0x14
    int pal_r;         // ebp - 0x10
    int pal_red;       // edp - 0xc
    OctNode *node;     // edp - 8
    byte *path;        // edp - 4
  } s;

  s.path = g_octree_path_tmp_alt;
  s.node = g_paletteOctreeRoot;
  
  Octree_BuildPathBytesFromRgb(rgb_color,(undefined8 *)&g_octree_path_tmp_alt);

#ifdef MODERN_FIXES
  while (1) {
    OctNode *child = s.node->children[*s.path];
    s.path++;
    if (child == 0) {
      break;
    }
    s.node = child;
    if ((byte)s.node->flags == 1) {
      return s.node->palette_idx;
    }
  }
#else
  __asm {
    mov eax, g_paletteOctreeRoot
    mov ebx, offset g_octree_path_tmp_alt
    xor ecx, ecx
  loop_start:
    mov edx, eax
    mov cl, byte ptr [ebx]
    mov eax, dword ptr [eax + ecx*4 + 8]
    add ebx, 1
    cmp eax, 0
    je loop_end
    cmp byte ptr [eax], 1
    jne loop_start
    mov eax, dword ptr [eax + 4]
    jmp end
  loop_end:
    mov s.node, edx
  }
#endif
  
  if (s.node->flags == 0) {
    s.index_list = s.node->list;
    s.best_distance = 0x7fffffff;
    s.pal_b = rgb_color & 0xff;
    s.pal_g = ((RGBQUAD *)&rgb_color)->rgbGreen;
    s.pal_r = (rgb_color & 0xff0000) >> 0x10;
    for (s.i = 0; s.i < s.node->list_count; s.i = s.i + 1) {
      s.pal_blue = g_paletteRgbTable[s.index_list[s.i]] & 0xff;
      s.pal_green = ((RGBQUAD *)&g_paletteRgbTable[s.index_list[s.i]])->rgbGreen;
      s.pal_red = (g_paletteRgbTable[s.index_list[s.i]] & 0xff0000) >> 0x10;

      s.distance = g_diff_squared_table_mid_ptr[s.pal_b - s.pal_red] +
        g_diff_squared_table_mid_ptr[s.pal_r - s.pal_blue] +
        g_diff_squared_table_mid_ptr[s.pal_g - s.pal_green];

      if (s.distance < s.best_distance) {
        s.best_index = (uint)*(byte *)(s.i + s.index_list);
        s.best_distance = s.distance;
      }
    }
    return s.best_index;
  }
  else
    return s.node->palette_idx;

#ifndef MODERN_FIXES
  end: ;
#endif
}

// FUNCTION: CARDARTLIB 0x100054fb
// FUNCTION: DRAWCARDLIB 0x10001aab
// FUNCTION: DECKDLL 0x1000a46f
// FUNCTION: MAGIC 0x004ba6e0
int Octree_FlattenLeafValues(int *node,int *out_values)
{
  int i;
  int result = 0;
  if (*node != 0) {
    *out_values = node[1];
    return 1;
  }
  
  for (i = 0; i < 8; i++) {
    if (node[i + 2] != 0) {
      int count = Octree_FlattenLeafValues((int *)node[i + 2],out_values);
      result = result + count;
      out_values = out_values + count;
    }
  }
  return result;
}

// This maybe shouldn't be in palette, it's not in DECKDLL
// FUNCTION: CARDARTLIB 0x10005591
// FUNCTION: DRAWCARDLIB 0x10001b41
// FUNCTION: SHANDALAR 0x0042eaa5
undefined4 QuantizeBgr24ToNearestPaletteColorInPlace(uint *bgr24,int height,int width,int row_padding)
{
  struct {
    uint cur_color;  /* [ebp-0x14] */
    uint arg_color;  /* [ebp-0x10] */
    int y;           /* [ebp-0x0c] */
    int byte_off;    /* [ebp-0x08] */
    int width3;      /* [ebp-0x04] */
  } s;

  s.width3 = width * 3;
  for (s.y = 0; height > s.y; s.y += 1, *(int *)&bgr24 += row_padding + s.width3) {
    s.byte_off = 0;
    s.cur_color = *bgr24;
    for (; s.byte_off < s.width3; s.byte_off = s.byte_off + 3) {
      s.arg_color = s.cur_color;
      s.cur_color = *(uint *)(s.byte_off + 3 + (int)bgr24);
      *(undefined4 *)(s.byte_off + (int)bgr24) = Octree_FindNearestColor(s.arg_color);
    }
  }
  return 0;
}

// This maybe shouldn't be in palette, it's not in DECKDLL
// FUNCTION: CARDARTLIB 0x10005629
// FUNCTION: DRAWCARDLIB 0x10001bd9
undefined4 QuantizeBgr24ToPaletteIndicesInPlace(uint *bgr24,int height,int width,int row_padding)
{
  struct {
    uint cur_color;     /* [ebp-0x1c] */
    uint masked_color;  /* [ebp-0x18] */
    int y;              /* [ebp-0x14] */
    int byte_off;       /* [ebp-0x10] */
    char *out_ptr;      /* [ebp-0x0c] */
    int width3;         /* [ebp-0x08] */
    int out_idx;        /* [ebp-0x04] */
  } s;

  s.width3 = width * 3;
  s.out_ptr = (char *)bgr24;

  for (s.y = 0; height > s.y;
      s.y = s.y + 1, *(int *)&bgr24 += row_padding + s.width3,
      *(int *)&s.out_ptr += row_padding + width) {
    s.out_idx = 0;
    s.byte_off = s.out_idx;
    s.cur_color = *bgr24;
    for (; s.byte_off < s.width3; s.byte_off = s.byte_off + 3, s.out_idx = s.out_idx + 1) {
      s.masked_color = s.cur_color & 0xffffff;
      s.cur_color = *(uint *)(s.byte_off + 3 + (int)bgr24);

      s.out_ptr[s.out_idx] = (char)Octree_FindNearestPaletteIndex(s.masked_color);

      if (g_paletteRgbTable[(byte)s.out_ptr[s.out_idx]] != s.masked_color) {
        g_paletteRgbTable[(byte)s.out_ptr[s.out_idx]] = 0;
      }
    }
  }
  return 0;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10006809
// FUNCTION: DRAWCARDLIB 0x10002db9
// FUNCTION: DECKDLL 0x1000b77c
// FUNCTION: MAGIC 0x004bb9f2
// FUNCTION: SHANDALAR 0x0042fd1d
void DestroyPaletteOctree(void)
{
  Octree_Destroy(g_paletteOctreeRoot);
  g_paletteOctreeRoot = (OctNode *)0x0;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000682c
// FUNCTION: DRAWCARDLIB 0x10002ddc
// FUNCTION: DECKDLL 0x1000b79f
// FUNCTION: SHANDALAR 0x0042fd40
int Palette_FindNearestEntryIndex(int target_r,int target_g,int target_b,byte *palette_bytes)
{
  struct {
    int best_distance; // edp - 0x10
    int best_index; // ebp - 0xc
    int i; // ebp - 8
    int distance; // edp - 4
  } s;
  
  s.best_distance = 0x7fffffff;
  for (s.i = 0; s.i < 0x100; s.i++, palette_bytes += 3) {
    s.distance = *(int *)(g_diff_squared_table_mid_ptr + (target_b - (uint)palette_bytes[2]) ) +
            *(int *)(g_diff_squared_table_mid_ptr + (target_g - (uint)palette_bytes[1]) ) +
            *(int *)(g_diff_squared_table_mid_ptr + (target_r - (uint)*palette_bytes) );


    if (s.distance < s.best_distance) {
      s.best_distance = s.distance;
      s.best_index = s.i;
    }
  }
  return s.best_index;
}

// FUNCTION: DRAWCARDLIB 0x1000b00c
// FUNCTION: DECKDLL 0x10025b00
// FUNCTION: MAGIC 0x0049626e
// FUNCTION: SHANDALAR 0x00466d01
COLORREF GetPaletteColor(int index)
{
  struct
  {
    int g;
    int r;
    int b;
  } rgb;

  rgb.r = g_cardArtPalette[index].rgbRed;
  rgb.g = g_cardArtPalette[index].rgbGreen;
  rgb.b = g_cardArtPalette[index].rgbBlue;
  return PALETTERGB(rgb.r, rgb.g, rgb.b);
}
