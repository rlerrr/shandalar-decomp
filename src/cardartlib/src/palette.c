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

/* Forward declarations for globals referenced before their definitions. */
void checked_DeleteDC_DeleteObject(HDC param_1,HGDIOBJ param_2);
void DestroyCardArtPalette(void);
undefined2 * ReadPalette(char *param_1,char *param_2);
void DestroyPaletteOctree(void);
BOOL CreateOffscreen32bppDibSection(int width,int height,HDC *out_dc,BITMAPINFO *bmi_optional,
                                           HBITMAP *out_bmp_optional,HGDIOBJ *obj_to_select_optional,void **out_bits_optional);
int Octree_Destroy(OctNode *rootPtr);
undefined4 Octree_InsertPathString(OctNode *param_1,char *param_2,unsigned int param_3);
int OctreeNode_FinalizeSubtree(OctNode *param_1);
undefined4 InitOctreeBitTables(void);
bool InitDiffSquaredLookupTable(void);

undefined4 DAT_100ed70c[0x100];
extern undefined1 global_RedPathBitsTable[0x800];
extern undefined1 global_GreenPathBitsTable[0x800];
extern undefined1 global_BluePathBitsTable[0x800];

// GLOBAL: CARDARTLIB 0x1001d0e4
// GLOBAL: DRAWCARDLIB 0x10022500
// GLOBAL: DECKDLL 0x10035b38
HDC global_screen_dc = (HDC)0x0;

// GLOBAL: CARDARTLIB 0x1001d0e8
// GLOBAL: DRAWCARDLIB 0x10022508
// GLOBAL: DECKDLL 0x10036240
char s__DUELPALall_TR_1001d0e8[] = "\\DUELPALall.TR";

// GLOBAL: CARDARTLIB 0x1001d0f8
// GLOBAL: DRAWCARDLIB 0x10022518
// GLOBAL: DECKDLL 0x10036250
char s__DUEL_plogpal_1001d0f8[] = "\\DUEL.plogpal";

// GLOBAL: CARDARTLIB 0x1001d240
// GLOBAL: DRAWCARDLIB 0x10021030
// GLOBAL: DECKDLL 0x10031930
int DAT_1001d240 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d244
// GLOBAL: DRAWCARDLIB 0x10021034
// GLOBAL: DECKDLL 0x10031934
undefined4 * PTR_DAT_1001d244 = DAT_100ed70c;

// GLOBAL: CARDARTLIB 0x1001d248
// GLOBAL: DRAWCARDLIB 0x10021038
// GLOBAL: DECKDLL 0x10031938
undefined4 global_diff_squared_initialized = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d24c
// GLOBAL: DRAWCARDLIB 0x1002103c
// GLOBAL: DECKDLL 0x1003193c
undefined1 * global_PTR_BluePathBitsTable = global_BluePathBitsTable;

// GLOBAL: CARDARTLIB 0x1001d250
// GLOBAL: DRAWCARDLIB 0x10021040
// GLOBAL: DECKDLL 0x10031940
undefined1 * global_PTR_GreenPathBitsTable = global_GreenPathBitsTable;

// GLOBAL: CARDARTLIB 0x1001d254
// GLOBAL: DRAWCARDLIB 0x10021044
// GLOBAL: DECKDLL 0x10031944
undefined1 * global_PTR_RedPathBitsTable = global_RedPathBitsTable;

// GLOBAL: CARDARTLIB 0x1001e064
// GLOBAL: DRAWCARDLIB 0x10021e54
// GLOBAL: DECKDLL 0x10032754
char s__rt_1001e064[] = "rt";

// GLOBAL: CARDARTLIB 0x1001e068
// GLOBAL: DRAWCARDLIB 0x10021e58
// GLOBAL: DECKDLL 0x10032758
char s__d____d__d__d_1001e068[] = "%d - %d %d %d";

// GLOBAL: CARDARTLIB 0x1001e078
// GLOBAL: DRAWCARDLIB 0x10021e68
// GLOBAL: DECKDLL 0x10032768
char s__rb_1001e078[] = "rb";

// GLOBAL: CARDARTLIB 0x1001e07c
// GLOBAL: DRAWCARDLIB 0x10021e6c
// GLOBAL: DECKDLL 0x1003276c
char s__sp_lf_1001e07c[] = " \n";

// GLOBAL: CARDARTLIB 0x1001e080
// GLOBAL: DRAWCARDLIB 0x10021e70
// GLOBAL: DECKDLL 0x10032770
char s__sp_lf__1001e080[] = " \n";

// GLOBAL: CARDARTLIB 0x1001e084
// GLOBAL: DRAWCARDLIB 0x10021e74
// GLOBAL: DECKDLL 0x10032774
char s__sp_lf__1001e084[] = " \n";

// GLOBAL: CARDARTLIB 0x1001e088
// GLOBAL: DRAWCARDLIB 0x10021e78
// GLOBAL: DECKDLL 0x10032778
char s__sp_lf__1001e088[] = " \n";

// GLOBAL: CARDARTLIB 0x1001e08c
// GLOBAL: DRAWCARDLIB 0x10021e7c
// GLOBAL: DECKDLL 0x1003277c
char s__sp_lf__1001e08c[] = " \n";

// GLOBAL: CARDARTLIB 0x100209e0
// GLOBAL: DRAWCARDLIB 0x100f1f70
// GLOBAL: DECKDLL 0x10113ce8
HPALETTE global_cart_art_hpalette;

// GLOBAL: CARDARTLIB 0x100209e4
// GLOBAL: DRAWCARDLIB 0x100f1f74
// GLOBAL: DECKDLL 0x10104df0
undefined4 DAT_100209e4;

// GLOBAL: CARDARTLIB 0x100209e8
// GLOBAL: DRAWCARDLIB 0x100f1f78
// GLOBAL: DECKDLL 0x10142f50
RGBQUAD g_cardArtPalette[0x100];

// GLOBAL: CARDARTLIB 0x10020de8
// GLOBAL: DRAWCARDLIB 0x100f2378
// GLOBAL: DECKDLL 0x10104e38
CRITICAL_SECTION global_critical_section_for_drawing;

// GLOBAL: CARDARTLIB 0x10116cf0
// GLOBAL: DRAWCARDLIB 0x10152020
// GLOBAL: DECKDLL 0x101e6a60
undefined2 DAT_10116cf0;

// GLOBAL: CARDARTLIB 0x10031ea4
// GLOBAL: DRAWCARDLIB 0x10028ef4
// GLOBAL: DECKDLL 0x100f2c6c
OctNode* g_paletteOctreeRoot;

// GLOBAL: CARDARTLIB 0x10031eb0
// GLOBAL: DRAWCARDLIB 0x10028f00
// GLOBAL: DECKDLL 0x100f2c78
undefined1 DAT_10031eb0;

// GLOBAL: CARDARTLIB 0x10031eb8
// GLOBAL: DRAWCARDLIB 0x10028f08
// GLOBAL: DECKDLL 0x100f2c80
undefined4 _DAT_10031eb8;

// GLOBAL: CARDARTLIB 0x10031ec0
// GLOBAL: DRAWCARDLIB 0x10028f10
// GLOBAL: DECKDLL 0x100f2c88
uint g_paletteRgbTable[0x100];

// GLOBAL: CARDARTLIB 0x100322c0
// GLOBAL: DRAWCARDLIB 0x10029310
// GLOBAL: DECKDLL 0x100f3088
undefined1 *g_octPathTmp;

// GLOBAL: CARDARTLIB 0x100322d4
// GLOBAL: DRAWCARDLIB 0x10029324
// GLOBAL: DECKDLL 0x100f309c
int DAT_100322d4;

// GLOBAL: CARDARTLIB 0x100ecb10
// GLOBAL: DRAWCARDLIB 0x10127e40
// GLOBAL: DECKDLL 0x101bc880
undefined1 global_RedPathBitsTable[0x800];

// GLOBAL: CARDARTLIB 0x100ed310
// GLOBAL: DRAWCARDLIB 0x10128640
// GLOBAL: DECKDLL 0x101bd080
int DAT_100ed310[0x200 / 4];

// GLOBAL: CARDARTLIB 0x100ed70c
// GLOBAL: DRAWCARDLIB 0x10128a3c
// GLOBAL: DECKDLL 0x101bd47c
undefined4 DAT_100ed70c[0x100];

// GLOBAL: CARDARTLIB 0x10115cf0
// GLOBAL: DRAWCARDLIB 0x10151020
// GLOBAL: DECKDLL 0x101e5a60
undefined1 global_GreenPathBitsTable[0x800];

// GLOBAL: CARDARTLIB 0x101164f0
// GLOBAL: DRAWCARDLIB 0x10151820
// GLOBAL: DECKDLL 0x101e6260
undefined1 global_BluePathBitsTable[0x800];

// MATCHING
// FUNCTION: CARDARTLIB 0x10001220
// FUNCTION: DRAWCARDLIB 0x1000a5a0
// FUNCTION: DECKDLL 0x100217b0
BOOL InitCardArtGdiResources(void)
{
#ifndef DECKDLL
  BOOL result = 1;
  
  if (!SetupDuelPalette())
    result = 0;
#endif

  if (global_screen_dc == 0) {
    CreateOffscreen32bppDibSection(10,10,(HDC *)&global_screen_dc,(BITMAPINFO *)0x0,
                                  (HBITMAP *)&DAT_100209e4,(HGDIOBJ *)0x0,(void **)0x0);
    InitializeCriticalSection(&global_critical_section_for_drawing);
  }

#ifndef DECKDLL
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
void ShutdownCardArtGdiResources(void)
{
  if (global_screen_dc != (HDC)0x0) {
    checked_DeleteDC_DeleteObject(global_screen_dc,(HGDIOBJ)DAT_100209e4);
    global_screen_dc = (HDC)0x0;
    DeleteCriticalSection(&global_critical_section_for_drawing);
  }
#ifndef DECKDLL
  if (global_cart_art_hpalette != 0) {
    DestroyCardArtPalette();
  }
#endif
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100012fb
// FUNCTION: DRAWCARDLIB 0x1000a67b
// FUNCTION: DECKDLL 0x1002349a
void ApplyCardArtPaletteToDc(HDC hdc)
{
  SelectPalette(hdc,global_cart_art_hpalette,0);
  RealizePalette(hdc);
  GdiFlush();
  SetDIBColorTable(hdc,0,0x100,g_cardArtPalette);
  SetStretchBltMode(hdc,COLORONCOLOR);
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000134a
// FUNCTION: DRAWCARDLIB 0x1000a6ca
// FUNCTION: DECKDLL 0x10023503
static BOOL CreateOffscreen32bppDibSection(int width,int height,HDC *out_dc,BITMAPINFO *bmi_optional,
                                          HBITMAP *out_bitmap,HGDIOBJ *out_prev_object,void **out_bits)
{
  //Stack layout won't behave
  struct {
    HBITMAP local_44;
    HDC hdc;
    HDC local_3c;
    BITMAPINFO local_38;
    HGDIOBJ local_c;
    void *bits;
  } s;

  s.local_3c = (HDC)0x0;
  s.local_44 = (HBITMAP)0x0;
  s.bits = (void *)0x0;
  if ((out_dc == (HDC *)0x0) || (out_bitmap == (HBITMAP *)0x0)) {
    return 0;
  }
 
  if (bmi_optional == (BITMAPINFO *)0x0) {
    bmi_optional = &s.local_38;
  }
  s.hdc = GetDC((HWND)0x0);
  if (s.hdc != (HDC)0x0) {
    ApplyCardArtPaletteToDc(s.hdc);
    s.local_3c = CreateCompatibleDC(s.hdc);
    if (s.local_3c != (HDC)0x0) {
      InitBitmapInfo24bppTopDown(bmi_optional,width,height);
      s.local_38.bmiHeader.biBitCount = 0x20;
      s.local_44 = CreateDIBSection(s.hdc,bmi_optional,0,&s.bits,(HANDLE)0x0,0);
      s.local_c = SelectObject(s.local_3c,s.local_44);
      ApplyCardArtPaletteToDc(s.local_3c);
    }
    ReleaseDC((HWND)0x0,s.hdc);
  }
  if (((s.local_3c == (HDC)0x0) || (s.local_44 == (HBITMAP)0x0)) || (s.bits == (void *)0x0)) {
    if (s.local_3c != (HDC)0x0) {
      DeleteDC(s.local_3c);
    }
    if (s.local_44 != (HBITMAP)0x0) {
      DeleteObject(s.local_44);
    }
    return 0;
  }
  else {
    if (out_dc != 0)
      *out_dc = s.local_3c;

    if (out_bitmap != 0)
      *out_bitmap = s.local_44;
    if (out_prev_object != (HGDIOBJ *)0x0) {
      *out_prev_object = s.local_c;
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
void checked_DeleteDC_DeleteObject(HDC param_1,HGDIOBJ param_2)
{
  if (param_1 != (HDC)0x0) {
    DeleteDC(param_1);
  }
  if (param_2 != (HGDIOBJ)0x0) {
    DeleteObject(param_2);
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001505
// FUNCTION: DRAWCARDLIB 0x1000a885
// FUNCTION: DECKDLL 0x100237c0
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
// FUNCTION: CARDARTLIB 0x100016b7
// FUNCTION: DRAWCARDLIB 0x1000acf5
// FUNCTION: DECKDLL 0x10024273
BOOL SetupDuelPalette(void)
{
  //Stack layout won't behave
  struct {
    PALETTEENTRY local_628;
    uint local_624 [66];
    undefined4 local_51c;
    UINT UVar1;
    UINT local_514;
    uint local_510 [66];
    LOGPALETTE *local_408;
    PALETTEENTRY local_404 [256];
  } s;
  
  s.local_51c = 1;
  strcpy((char *)s.local_624,global_base_directory);
  strcat((char *)s.local_624,s__DUELPALall_TR_1001d0e8);
  strcpy((char *)s.local_510,global_base_directory);
  strcat((char *)s.local_510,s__DUEL_plogpal_1001d0f8);
  s.local_408 = (LOGPALETTE *)ReadPalette((char *)s.local_624,(char *)s.local_510);
  if (s.local_408 != (LOGPALETTE *)0x0) {
    for (s.local_514 = 1; (int)s.local_514 < 0xff; s.local_514 = s.local_514 + 1) {
      s.local_408->palPalEntry[s.local_514].peFlags = '\x04';
    }
    global_cart_art_hpalette = CreatePalette(s.local_408);
    if (global_cart_art_hpalette != (HPALETTE)0x0) {
      s.local_628.peRed = 0xff;
      s.local_628.peGreen = 0xff;
      s.local_628.peBlue = 0xff;
      s.local_628.peFlags = '\0';
      SetPaletteEntries(global_cart_art_hpalette,0xff,1,&s.local_628);
      s.local_628.peRed = 0xfe;
      s.local_628.peGreen = 0xfe;
      s.local_628.peBlue = 0xfe;
      s.local_628.peFlags = '\x04';
      SetPaletteEntries(global_cart_art_hpalette,0xbf,1,&s.local_628);
      for (s.local_514 = 0xec; (int)s.local_514 < 0xff; s.local_514 = s.local_514 + 1) {
        s.local_628.peRed = '\x01';
        s.local_628.peGreen = '\x01';
        s.local_628.peBlue = '\x01';
        s.local_628.peFlags = '\x04';
        SetPaletteEntries(global_cart_art_hpalette,s.local_514,1,&s.local_628);
      }
      s.UVar1 = GetPaletteEntries(global_cart_art_hpalette,0,0x100,s.local_404);
      for (s.local_514 = 0; (int)s.local_514 < (int)s.UVar1; s.local_514 = s.local_514 + 1) {
        g_cardArtPalette[s.local_514].rgbBlue = s.local_404[s.local_514].peBlue;
        g_cardArtPalette[s.local_514].rgbGreen = s.local_404[s.local_514].peGreen;
        g_cardArtPalette[s.local_514].rgbRed = s.local_404[s.local_514].peRed;
        g_cardArtPalette[s.local_514].rgbReserved = 0;
      }
      
      for (s.local_514 = s.UVar1; (int)s.local_514 < 0x100; s.local_514++) {
        g_cardArtPalette[s.local_514].rgbBlue = 0;
        g_cardArtPalette[s.local_514].rgbGreen = 0;
        g_cardArtPalette[s.local_514].rgbRed = 0;
        g_cardArtPalette[s.local_514].rgbReserved = 0;
      }
    }
    else
      s.local_51c=0;
  }
  else
    s.local_51c=0;

  return (BOOL)s.local_51c;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100019a8
// FUNCTION: DRAWCARDLIB 0x1000afe6
// FUNCTION: DECKDLL 0x10024564
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
undefined2 * ReadPalette(char *param_1,char *param_2)
{
  struct {
    uint rgbTemp;
    char local_120 [256];
    int local_20;
    int local_1c;
    FILE *local_18;
    char *local_14;
    int local_10;
    uint local_c;
    undefined2 *local_8;
  } s;
  
  s.local_20 = 0;
  s.local_8 = &DAT_10116cf0;
  *s.local_8 = 0x300;
  s.local_18 = fopen(param_1,s__rt_1001e064);
  if (s.local_18 == (FILE *)0x0) {
    return 0;
  }

  if (g_paletteOctreeRoot != (OctNode *)0x0) {
    Octree_Destroy(g_paletteOctreeRoot);
  }
  g_paletteOctreeRoot = OctreeNode_Create();
  fgets(s.local_120,0xff,s.local_18);
  while ((s.local_18->_flag & 0x10) == 0) {
    sscanf(s.local_120,s__d____d__d__d_1001e068,&s.local_20,&s.local_10,&s.local_1c,&s.local_c);
    s.local_14 = strchr(s.local_120,0x2d) + 1;
    s.local_14 = strchr(s.local_14,0x2d) + 1;

    Octree_InsertPathString(g_paletteOctreeRoot,s.local_14,s.local_20);
    s.rgbTemp = s.local_10 << 0x10 | s.local_1c << 8 | s.local_c;
    ((uint *)g_paletteRgbTable)[s.local_20] = s.rgbTemp;
    *(undefined1 *)(s.local_8 + s.local_20 * 2 + 2) = (undefined1)s.local_10;
    *(undefined1 *)((int)s.local_8 + s.local_20 * 4 + 5) = (undefined1)s.local_1c;
    *(undefined1 *)(s.local_8 + s.local_20 * 2 + 3) = (undefined1)s.local_c;
    if ((s.local_20 == 0) || (s.local_20 == 0xff)) {
      *(undefined1 *)((int)s.local_8 + s.local_20 * 4 + 7) = 0;
    }
    else {
      *(undefined1 *)((int)s.local_8 + s.local_20 * 4 + 7) = 1;
    }
    fgets(s.local_120,0xff,s.local_18);
  }
  DAT_100322d4 = 0;
  DAT_1001d240 = DAT_100322d4;
  _DAT_10031eb8 = OctreeNode_FinalizeSubtree(g_paletteOctreeRoot);
  DAT_1001d240 = DAT_1001d240 + -1;
  s.local_8[1] = 0x100;
  fclose(s.local_18);
  s.local_18 = (FILE *)0x0;
  if (param_2 != (char *)0x0) {
    s.local_18 = fopen(param_2,s__rb_1001e078);
  }
  if (s.local_18 != (FILE *)0x0) {
    fread(&DAT_10116cf0,0x404,1,s.local_18);
    fclose(s.local_18);
  }
  *(undefined1 *)((int)s.local_8 + 0x403) = 0;
  *(undefined1 *)((int)s.local_8 + 7) = *(undefined1 *)((int)s.local_8 + 0x403);
  InitOctreeBitTables();
  InitDiffSquaredLookupTable();

  return s.local_8;
}

// FUNCTION: CARDARTLIB 0x10004cd7
// FUNCTION: DRAWCARDLIB 0x10001287
// FUNCTION: DECKDLL 0x10009c42
bool InitDiffSquaredLookupTable(void)
{
  int local_c;
  int i;
  
  if (global_diff_squared_initialized != 0) 
    return 0;

  for (i = 0, local_c = -0xff; i < 0x200; i++, local_c++) {
    DAT_100ed310[i] = local_c * local_c;
  }
  global_diff_squared_initialized = 1;

  return 1;
}

// FUNCTION: CARDARTLIB 0x10004d49
// FUNCTION: DRAWCARDLIB 0x100012f9
// FUNCTION: DECKDLL 0x10009cb4
void OctreeNode_CollectLeafIndices(OctNode *param_1,int param_2,int *param_3)
{
  int i;
  
  if (param_1->flags != 0) {
    *(char *)(*param_3 + param_2) = (char)param_1->palette_idx;
    *param_3 = *param_3 + 1;   
  }
  else {
     for (i = 0; i < 8; i = i + 1) {
      if (param_1->children[i] != 0) {
        OctreeNode_CollectLeafIndices(param_1->children[i],param_2,param_3);
      }
    }
  }
}

// FUNCTION: CARDARTLIB 0x10004dc8
// FUNCTION: DRAWCARDLIB 0x10001378
// FUNCTION: DECKDLL 0x10009d33
int OctreeNode_FinalizeSubtree(OctNode *param_1)
{
  struct {
    size_t local_410;
    int i;
    int local_408;
    undefined1 local_404 [1024];
  } s;
  
  s.local_408 = 0;
  s.local_410 = 0;
  DAT_100322d4 = DAT_100322d4 + 1;
  if (DAT_1001d240 < DAT_100322d4) {
    DAT_1001d240 = DAT_100322d4;
  }
  if (param_1->flags != 0) {
    DAT_100322d4--;
    return 1;
  }

  for (s.i = 0; s.i < 8; s.i = s.i + 1) {
    if (param_1->children[s.i] != 0) {
      s.local_408 = s.local_408 + OctreeNode_FinalizeSubtree(param_1->children[s.i]);
      s.local_410 = s.local_410 + 1;
    }
  }
  if (s.local_410 != 0) {
    s.local_410 = 0;
    OctreeNode_CollectLeafIndices(param_1,(int)s.local_404,(int *)&s.local_410);
    param_1->list = malloc(s.local_410);
    param_1->list_count = s.local_410;
    memcpy(param_1->list,s.local_404,s.local_410);
  }
  DAT_100322d4 = DAT_100322d4 + -1;

  return s.local_408;
}

// FUNCTION: CARDARTLIB 0x10004f07
// FUNCTION: DRAWCARDLIB 0x100014b7
// FUNCTION: DECKDLL 0x10009e73
undefined4 Octree_InsertPathString(OctNode *param_1,char *param_2,unsigned int param_3)
{
  int iVar2;

  param_2 += strspn(param_2,s__sp_lf_1001e07c);
  while (*param_2 != '\0') {
    iVar2 = atoi(param_2);
    if (param_1->children[iVar2] == 0) {
      param_1->children[iVar2] = OctreeNode_Create();
    }
    param_1 = param_1->children[iVar2];

    param_2 += strspn(param_2 +=  
        strcspn(param_2 + strspn(param_2,s__sp_lf__1001e088),s__sp_lf__1001e084) + 
        strspn(param_2,s__sp_lf__1001e080)
      ,s__sp_lf__1001e08c);
  }
  param_1->flags = 1;
  param_1->palette_idx = param_3;
  return 0;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10004fe1
// FUNCTION: DRAWCARDLIB 0x10001591
// FUNCTION: DECKDLL 0x10009f53
int Octree_Destroy(OctNode *rootPtr)
{
  int i;
  int result;
  
  result = 0;
  if (rootPtr->flags != 0) {   
      free(rootPtr);
      return 1;
  }

  for (i = 0; i < 8; i = i + 1) {
    if (rootPtr->children[i] != 0) {
      result = result + Octree_Destroy(rootPtr->children[i]);
    }
  }
  if (rootPtr->list != 0) {
    free(rootPtr->list);
  }
  free(rootPtr);

  return result;
}

// FUNCTION: CARDARTLIB 0x1000508d
// FUNCTION: DRAWCARDLIB 0x1000163d
// FUNCTION: DECKDLL 0x1000a002
void Octree_BuildPathBytesFromRgb(uint rgb,undefined8 *out_path_words)
{
  struct {
    int part_g;
    int part_b;
    int part_r;
  } s;
  
  s.part_b = (rgb & 0xff);
  s.part_g = ((RGBQUAD *) &rgb)->rgbGreen;
  s.part_r = ((rgb & 0xff0000) >> 0x10);

  *out_path_words = ((undefined8 *)global_RedPathBitsTable)[s.part_r] | 
    ((undefined8 *)global_GreenPathBitsTable)[s.part_g] | 
    ((undefined8 *)global_BluePathBitsTable)[s.part_b];
}

// FUNCTION: CARDARTLIB 0x100050f1
// FUNCTION: DRAWCARDLIB 0x100016a1
// FUNCTION: DECKDLL 0x1000a066
undefined4 InitOctreeBitTables(void)
{
  struct {
    int local_10;
    int j;
    int i;
  } s;
  
  for (s.i = 0; s.i < 0x100; s.i++) {
    for (s.j = 0, s.local_10 = 0x80; s.j < 8; s.j++, s.local_10 = s.local_10 >> 1) {
      global_PTR_BluePathBitsTable[s.i * 8 + s.j] = (s.i & s.local_10) != 0 ? 4 : 0;

      global_PTR_GreenPathBitsTable[s.i * 8 + s.j] = (s.i & s.local_10) != 0 ? 2 : 0;

      global_PTR_RedPathBitsTable[s.i * 8 + s.j] = (s.i & s.local_10) != 0 ? 1 : 0;
    }
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x100051f5
// FUNCTION: DRAWCARDLIB 0x100017a5
// FUNCTION: DECKDLL 0x1000a169
undefined4 Octree_FindNearestColor(uint param_1)
{
  struct {
    int pal_r;        /* ebp - 0x34 */
    int best_dist;    /* ebp - 0x30 */
    byte* idx_list_base;/* ebp - 0x2c */
    uint best_idx;    /* ebp - 0x28 */
    int i;            /* ebp - 0x24 */
    int green;        /* ebp - 0x20 */
    int dist;         /* ebp - 0x1c */
    int pal_g;        /* ebp - 0x18 */
    int blue;         /* ebp - 0x14 */
    int red;          /* ebp - 0x10 */
    int pal_b;        /* ebp - 0xc */
    OctNode *node;        /* ebp - 8 */
    byte *path;       /* ebp - 4 */
  } s;

  s.path = (byte *)&g_octPathTmp;
  s.node = g_paletteOctreeRoot;

  Octree_BuildPathBytesFromRgb(param_1,(undefined8 *)&g_octPathTmp);

  //TODO: how on earth can you mimic this with /Od?
  __asm {
    mov eax, g_paletteOctreeRoot
    mov ebx, offset g_octPathTmp
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
  
  if (s.node->flags == 0) {
    s.idx_list_base = s.node->list;
    s.best_dist = 0x7fffffff;
    s.blue = (int)(param_1 & 0xff);
    s.green = (int)((byte *)&param_1)[1];
    s.red = (int)((param_1 & 0xff0000) >> 0x10);

    for (s.i = 0; s.i < s.node->list_count; s.i = s.i + 1) {
      s.pal_b = g_paletteRgbTable[s.idx_list_base[s.i]] & 0xff;

      s.pal_g = ((RGBQUAD *)&g_paletteRgbTable[s.idx_list_base[s.i]])->rgbGreen;

      s.pal_r = (g_paletteRgbTable[s.idx_list_base[s.i]] & 0xff0000) >> 0x10;

      s.dist = PTR_DAT_1001d244[s.green - s.pal_g] +
                PTR_DAT_1001d244[s.blue - s.pal_b] +
                PTR_DAT_1001d244[s.red - s.pal_r];

      if (s.dist < s.best_dist) {
        s.best_idx = s.idx_list_base[s.i];
        s.best_dist = s.dist;
      }
    }
    return g_paletteRgbTable[s.best_idx];
  }
  else {
    return g_paletteRgbTable[s.node->palette_idx];
  }

end:  ;
}

// FUNCTION: CARDARTLIB 0x10005383
// FUNCTION: DRAWCARDLIB 0x10001933
// FUNCTION: DECKDLL 0x1000a2f7
uint Octree_FindNearestPaletteIndex(uint param_1)
{
  struct {
    int idk; // edp - 0x34
    int local_34; // edp - 0x30
    byte *iVar2; // edp - 0x2c
    uint local_2c; // edp - 0x28
    int i; // ebp - 0x24
    int pal_g; // ebp - 0x20
    int pad4; // ebp - 0x1c
    int piVar1; // ebp - 0x18
    int pal_b; // ebp - 0x14
    int pal_r; // ebp - 0x10
    int pad1; // edp - 0xc
    OctNode *node; // edp - 8
    byte *pbVar5; // edp - 4
  } s;

  s.pbVar5 = &DAT_10031eb0;
  s.node = g_paletteOctreeRoot;
  
  Octree_BuildPathBytesFromRgb(param_1,(undefined8 *)&DAT_10031eb0);

  //TODO: how on earth can you mimic this with /Od?
  __asm {
    mov eax, g_paletteOctreeRoot
    mov ebx, offset DAT_10031eb0
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
  
  if (s.node->flags == 0) {
    s.iVar2 = s.node->list;
    s.local_34 = 0x7fffffff;
    s.pal_b = param_1 & 0xff;
    s.pal_g = ((RGBQUAD *)&param_1)->rgbGreen;
    s.pal_r = (param_1 & 0xff0000) >> 0x10;
    for (s.i = 0; s.i < s.node->list_count; s.i = s.i + 1) {
      s.idk = g_paletteRgbTable[s.iVar2[s.i]] & 0xff;
      s.piVar1 = ((RGBQUAD *)&g_paletteRgbTable[s.iVar2[s.i]])->rgbGreen;
      s.pad1 = (g_paletteRgbTable[s.iVar2[s.i]] & 0xff0000) >> 0x10;

      s.pad4 = PTR_DAT_1001d244[s.pal_b - s.pad1] +
        PTR_DAT_1001d244[s.pal_r - s.idk] +
        PTR_DAT_1001d244[s.pal_g - s.piVar1];

      if (s.pad4 < s.local_34) {
        s.local_2c = (uint)*(byte *)(s.i + s.iVar2);
        s.local_34 = s.pad4;
      }
    }
    return s.local_2c;
  }
  else
    return s.node->palette_idx;

  end: ;
}

// FUNCTION: CARDARTLIB 0x100054fb
// FUNCTION: DRAWCARDLIB 0x10001aab
// FUNCTION: DECKDLL 0x1000a46f
int Octree_FlattenLeafValues(int *param_1,int *param_2)
{
  int i;
  int result = 0;
  if (*param_1 != 0) {
    *param_2 = param_1[1];
    return 1;
  }
  
  for (i = 0; i < 8; i++) {
    if (param_1[i + 2] != 0) {
      int iVar1 = Octree_FlattenLeafValues((int *)param_1[i + 2],param_2);
      result = result + iVar1;
      param_2 = param_2 + iVar1;
    }
  }
  return result;
}

// This maybe shouldn't be in palette, it's not in DECKDLL
// FUNCTION: CARDARTLIB 0x10005591
// FUNCTION: DRAWCARDLIB 0x10001b41
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
void DestroyPaletteOctree(void)
{
  Octree_Destroy(g_paletteOctreeRoot);
  g_paletteOctreeRoot = (OctNode *)0x0;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000682c
// FUNCTION: DRAWCARDLIB 0x10002ddc
// FUNCTION: DECKDLL 0x1000b79f
int Palette_FindNearestEntryIndex(int param_1,int param_2,int param_3,byte *param_4)
{
  struct {
    int local_14; // edp - 0x10
    int local_10; // ebp - 0xc
    int local_c; // ebp - 8
    int iVar1; // edp - 4
  } s;
  
  s.local_14 = 0x7fffffff;
  for (s.local_c = 0; s.local_c < 0x100; s.local_c++, param_4 += 3) {
    s.iVar1 = *(int *)(PTR_DAT_1001d244 + (param_3 - (uint)param_4[2]) ) +
            *(int *)(PTR_DAT_1001d244 + (param_2 - (uint)param_4[1]) ) +
            *(int *)(PTR_DAT_1001d244 + (param_1 - (uint)*param_4) );


    if (s.iVar1 < s.local_14) {
      s.local_14 = s.iVar1;
      s.local_10 = s.local_c;
    }
  }
  return s.local_10;
}
