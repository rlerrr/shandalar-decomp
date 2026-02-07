#include "CardArtLib.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "inttypes.h"
#include "mystdbool.h"
#include "defs.h"
#include "catalog.h"
#include "haar.h"
#include "assert.h"

typedef BOOL (WINAPI code)(HINSTANCE, DWORD, LPVOID);

/* Forward declarations for globals referenced before their definitions. */
void FreeIfNotNull(void *ptr);
extern undefined4 DAT_100ed70c;
extern undefined1 DAT_100ecb10[0x800];
extern undefined1 DAT_10115cf0[0x800];
extern undefined1 DAT_101164f0[0x800];

// GLOBAL: CARDARTLIB 0x1001d0e4
undefined4 DAT_1001d0e4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d0e8
char s__DUELPALall_TR_1001d0e8[] = "\\DUELPALall.TR";

// GLOBAL: CARDARTLIB 0x1001d0f8
char s__DUEL_plogpal_1001d0f8[] = "\\DUEL.plogpal";

// GLOBAL: CARDARTLIB 0x1001d1dc
undefined4 DAT_1001d1dc = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d1e0
char s__CARDART_1001d1e0[] = "\\CARDART";

// GLOBAL: CARDARTLIB 0x1001d1ec
char s__CARDS_DAT_1001d1ec[] = "\\CARDS.DAT";

// GLOBAL: CARDARTLIB 0x1001d1f8
char s__rb_1001d1f8[] = "rb";

// GLOBAL: CARDARTLIB 0x1001d1fc
char s__s__04d_c_WVL_1001d1fc[] = "%s\\%04d%c.WVL";

// GLOBAL: CARDARTLIB 0x1001d20c
char s__s__04d_WVL_1001d20c[] = "%s\\%04d.WVL";

// GLOBAL: CARDARTLIB 0x1001d218
char s__s__04d_WVL_1001d218[] = "%s\\%04d.WVL";

// GLOBAL: CARDARTLIB 0x1001d224
char s__s__04d_c_WVL_1001d224[] = "%s\\%04d%c.WVL";

// GLOBAL: CARDARTLIB 0x1001d234
char s__s__04d_WVL_1001d234[] = "%s\\%04d.WVL";

// GLOBAL: CARDARTLIB 0x1001d240
int DAT_1001d240 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d244
undefined4 * PTR_DAT_1001d244 = &DAT_100ed70c;

// GLOBAL: CARDARTLIB 0x1001d248
undefined4 DAT_1001d248 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d24c
undefined1 * PTR_DAT_1001d24c = DAT_101164f0;

// GLOBAL: CARDARTLIB 0x1001d250
undefined1 * PTR_DAT_1001d250 = DAT_10115cf0;

// GLOBAL: CARDARTLIB 0x1001d254
undefined1 * PTR_DAT_1001d254 = DAT_100ecb10;

// GLOBAL: CARDARTLIB 0x1001e064
char s__rt_1001e064[] = "rt";

// GLOBAL: CARDARTLIB 0x1001e068
char s__d____d__d__d_1001e068[] = "%d - %d %d %d";

// GLOBAL: CARDARTLIB 0x1001e078
char s__rb_1001e078[] = "rb";

// GLOBAL: CARDARTLIB 0x1001e07c
char s__sp_lf_1001e07c[] = " \n";

// GLOBAL: CARDARTLIB 0x1001e080
char s__sp_lf__1001e080[] = " \n";

// GLOBAL: CARDARTLIB 0x1001e084
char s__sp_lf__1001e084[] = " \n";

// GLOBAL: CARDARTLIB 0x1001e088
char s__sp_lf__1001e088[] = " \n";

// GLOBAL: CARDARTLIB 0x1001e08c
char s__sp_lf__1001e08c[] = " \n";

// GLOBAL: CARDARTLIB 0x100209e0
undefined4 DAT_100209e0 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100209e4
undefined4 DAT_100209e4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100209e8
RGBQUAD g_cardArtPalette[0x100];

// GLOBAL: CARDARTLIB 0x10020de8
CRITICAL_SECTION DAT_10020de8;

// GLOBAL: CARDARTLIB 0x10020e84
undefined4 DAT_10020e84 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10020e00
uint DAT_10020e00[0x21];

// GLOBAL: CARDARTLIB 0x10020e88
unsigned char DAT_10020e88[0x100 * 0xc];
#define DAT_10020e8c DAT_10020e88[4]
#define DAT_10020e90 DAT_10020e88[8]

// GLOBAL: CARDARTLIB 0x10021a88
uint *DAT_10021a88 = (uint *)0x0;

// GLOBAL: CARDARTLIB 0x10021a8c
int DAT_10021a8c = 0;

// GLOBAL: CARDARTLIB 0x10021a90
int DAT_10021a90 = 0;

// GLOBAL: CARDARTLIB 0x10021a94
undefined4 DAT_10021a94 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10021a98
int DAT_10021a98 = 0;

// GLOBAL: CARDARTLIB 0x10021a9c
undefined4 DAT_10021a9c = 0x00000000;

// GLOBAL: CARDARTLIB 0x10021aa0
unsigned char DAT_10021aa0[0x100 * 8];
#define DAT_10021aa4 DAT_10021aa0[4]

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

// GLOBAL: CARDARTLIB 0x10031ea4
OctNode* g_paletteOctreeRoot = 0x00000000;

// GLOBAL: CARDARTLIB 0x10031eb0
undefined1 DAT_10031eb0 = 0x00;

// GLOBAL: CARDARTLIB 0x10031eb8
undefined4 _DAT_10031eb8 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10031ec0
uint g_paletteRgbTable[0x100];

// GLOBAL: CARDARTLIB 0x100322c0
undefined1 *g_octPathTmp = 0x00;

// GLOBAL: CARDARTLIB 0x100322d4
int DAT_100322d4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100ecb10
undefined1 DAT_100ecb10[0x800];

// GLOBAL: CARDARTLIB 0x100ed310
int DAT_100ed310[0x200 / 4];

// GLOBAL: CARDARTLIB 0x100ed70c
undefined4 DAT_100ed70c = 0x00000000;

// GLOBAL: CARDARTLIB 0x10115cf0
undefined1 DAT_10115cf0[0x800];

// GLOBAL: CARDARTLIB 0x101164f0
undefined1 DAT_101164f0[0x800];

// GLOBAL: CARDARTLIB 0x10116cf0
undefined2 DAT_10116cf0 = 0x0000;

// GLOBAL: CARDARTLIB 0x101177f4
int g_versionedBigArtCount = 0x00000000;

// GLOBAL: CARDARTLIB 0x10117910
char DAT_10117910[0x105];

// GLOBAL: CARDARTLIB 0x10121fe0
int g_versionedSmallArtCount = 0x00000000;

// GLOBAL: CARDARTLIB 0x10121fe4
undefined4 _DAT_10121fe4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10117a20
CRITICAL_SECTION DAT_10117a20;

// GLOBAL: CARDARTLIB 0x101221f0
CRITICAL_SECTION DAT_101221f0;

// GLOBAL: CARDARTLIB 0x101200a0
int g_cardPicCounts[2000];

// SIZE 0x10
typedef struct ArtCacheEntry {
  HBITMAP hbm;
  void *bits;
  int width;
  int height;
} ArtCacheEntry;
STATIC_ASSERT(sizeof(ArtCacheEntry) == 0x10, ArtCacheEntry_wrong_size);

// GLOBAL: CARDARTLIB 0x10117a40
ArtCacheEntry g_SmallArtCache[2000];

// SIZE 0x18
typedef struct VersionedArtCacheEntry {
  HBITMAP hbm;
  void *bits;
  int width;
  int height;
  int id;
  int version;
} VersionedArtCacheEntry;
STATIC_ASSERT(sizeof(VersionedArtCacheEntry) == 0x18, VersionedArtCacheEntry_wrong_size);

// GLOBAL: CARDARTLIB 0x1011f740
VersionedArtCacheEntry g_versionedSmallArtCache[100];

// GLOBAL: CARDARTLIB 0x10121ff0
VersionedArtCacheEntry g_versionedBigArtCache[0x14];

void RotateDwordsLeft1(undefined4 *param_1,int param_2);
void DestroyPaletteOctree(void);
int Huffman13_DecodeDwords(undefined4 *out_dwords,undefined4 bitstream_start,undefined4 bitstream_end);
int Huffman13_Init(undefined4 bitstream_start,undefined4 symbol_table,undefined4 node_index_base);
int Huffman13_BuildDecodeTable(int node_count);
int Huffman13_DecodeDwordsWithZeroRuns(undefined8 *out_dwords,uint *bitstream,undefined4 bitstream_end);
void ShutdownCardArtGdiResources(void);
void checked_DeleteDC_DeleteObject(HDC param_1,HGDIOBJ param_2);
void DestroyCardArtPalette(void);
void CardArtLib_Shutdown(void);
void InitBitmapInfo24bppTopDown(BITMAPINFO *bmi,int width,int height);
VersionedArtCacheEntry * FindVersionedSmallArtCacheEntry(int id,int version);
void DestroyVersionedSmallArt(int id,int version);
void DestroyAllVersionedSmallArts(void);
undefined2 * ReadPalette(char *param_1,char *param_2);
void SetBytes(void *dst,uint value,size_t size);

// MATCHING
// FUNCTION: CARDARTLIB 0x10001220
// FUNCTION: DRAWCARDLIB 0x1000a5a0
static BOOL InitCardArtGdiResources(void)
{
  BOOL iVar1 = 1;
  
  if (!SetupDuelPalette())
    iVar1 = 0;

  if (DAT_1001d0e4 == 0) {
    CreateOffscreen32bppDibSection(10,10,(HDC *)&DAT_1001d0e4,(BITMAPINFO *)0x0,
                                  (HBITMAP *)&DAT_100209e4,(HGDIOBJ *)0x0,(void **)0x0);
    InitializeCriticalSection(&DAT_10020de8);
  }

  if (DAT_1001d0e4 == 0)
    iVar1=0;
  
  if (iVar1 == 0)
    ShutdownCardArtGdiResources();

  return iVar1;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100012a8
// FUNCTION: DRAWCARDLIB 0x1000a628
void ShutdownCardArtGdiResources(void)
{
  if (DAT_1001d0e4 != (HDC)0x0) {
    checked_DeleteDC_DeleteObject(DAT_1001d0e4,DAT_100209e4);
    DAT_1001d0e4 = (HDC)0x0;
    DeleteCriticalSection(&DAT_10020de8);
  }
  if (DAT_100209e0 != 0) {
    DestroyCardArtPalette();
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100012fb
// FUNCTION: DRAWCARDLIB 0x1000a67b
static void ApplyCardArtPaletteToDc(HDC hdc)
{
  SelectPalette(hdc,DAT_100209e0,0);
  RealizePalette(hdc);
  GdiFlush();
  SetDIBColorTable(hdc,0,0x100,g_cardArtPalette);
  SetStretchBltMode(hdc,3);
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000134a
// FUNCTION: DRAWCARDLIB 0x1000a6ca
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
static BOOL DrawBitmapToRect(HDC dst_dc,const RECT *dst_rect,HBITMAP bitmap)
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
static BOOL DrawBitmapSubrectToRect(HDC dst_dc,const RECT *dst_rect,HBITMAP bitmap,int src_x,int src_y,
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

  EnterCriticalSection(&DAT_10020de8);
  v.h = SelectObject(DAT_1001d0e4,bitmap);
  GetObjectA(bitmap,sizeof(v.bm),&v.bm);

  v.dst_left = dst_rect->left;
  v.dst_top = dst_rect->top;
  v.dst_width =  (dst_rect->right < dst_rect->left) ? v.bm.bmWidth : dst_rect->right - dst_rect->left;

  v.dst_height = (dst_rect->bottom < dst_rect->top) ? v.bm.bmHeight : dst_rect->bottom - dst_rect->top;

  ApplyCardArtPaletteToDc(DAT_1001d0e4);

  StretchBlt(dst_dc,v.dst_left,v.dst_top,v.dst_width,v.dst_height,DAT_1001d0e4,src_x,src_y,
    src_width <= v.bm.bmWidth ? src_width : v.bm.bmWidth,
    src_height <= v.bm.bmHeight ? src_height : v.bm.bmHeight, 0xcc0020);
  SelectObject(DAT_1001d0e4,v.h);
  LeaveCriticalSection(&DAT_10020de8);
  return 1;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100016b7
// FUNCTION: DRAWCARDLIB 0x1000acf5
static BOOL SetupDuelPalette(void)
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
    DAT_100209e0 = CreatePalette(s.local_408);
    if (DAT_100209e0 != (HPALETTE)0x0) {
      s.local_628.peRed = 0xff;
      s.local_628.peGreen = 0xff;
      s.local_628.peBlue = 0xff;
      s.local_628.peFlags = '\0';
      SetPaletteEntries(DAT_100209e0,0xff,1,&s.local_628);
      s.local_628.peRed = 0xfe;
      s.local_628.peGreen = 0xfe;
      s.local_628.peBlue = 0xfe;
      s.local_628.peFlags = '\x04';
      SetPaletteEntries(DAT_100209e0,0xbf,1,&s.local_628);
      for (s.local_514 = 0xec; (int)s.local_514 < 0xff; s.local_514 = s.local_514 + 1) {
        s.local_628.peRed = '\x01';
        s.local_628.peGreen = '\x01';
        s.local_628.peBlue = '\x01';
        s.local_628.peFlags = '\x04';
        SetPaletteEntries(DAT_100209e0,s.local_514,1,&s.local_628);
      }
      s.UVar1 = GetPaletteEntries(DAT_100209e0,0,0x100,s.local_404);
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
void DestroyCardArtPalette(void)
{
  DeleteObject(DAT_100209e0);
  DAT_100209e0 = (HGDIOBJ)0x0;
  DestroyPaletteOctree();
}

// FUNCTION: CARDARTLIB 0x10001f40
// FUNCTION: DRAWCARDLIB 0x10002ea0
void MemZeroDwords(undefined8 *param_1,uint param_2)
{
  //TODO: I don't think MSVC can generate this??  This gotta be inline asm
  __asm {
    mov edi, dword ptr param_1
    mov ecx, dword ptr param_2
    test edi, 4
    je 0xd
    mov byte ptr [edi], 0
    add edi, 4
    dec ecx
    jle 0x2e
    push ecx
    shr ecx, 1
    dec ecx
    jl 0x16
    fldz
    je 0xc
    fst qword ptr [edi]
    add edi, 8
    dec ecx
    jne -0xc
    fstp qword ptr [edi]
    pop ecx
    and ecx, 1
    je 0x3
    mov byte ptr [edi], 0
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10002457
// FUNCTION: DRAWCARDLIB 0x100033b7
int Huffman13_DecodeDwords(undefined4 *out_dwords,undefined4 bitstream_start,undefined4 bitstream_end)
{
  struct {
    uint bit_count;
    uint new_bits;
    uint bit;
    int local_14;
    int symbol;
    undefined4 *out_base;
    int lookahead;
    int node;
  } s;

  s.local_14 = 0;
  s.out_base = out_dwords;
  DAT_10021a88 = bitstream_start;
  DAT_10021a8c = DAT_10021a88;
  DAT_10021a90 = bitstream_end;
  DAT_1001d1dc = 0;
  s.lookahead = (int)BitStream_ReadBits(8);
  while (s.lookahead != -1) {
    if (*(int *)(DAT_10020e88 + s.lookahead * 0xc) < 0x7fffffff) {
      s.bit_count = *(uint *)(DAT_10020e88 + s.lookahead * 0xc + 4);
      *(undefined4 *)out_dwords = *(undefined4 *)(DAT_10020e88 + s.lookahead * 0xc);
      out_dwords = out_dwords + 1;
      s.lookahead = s.lookahead >> (byte)s.bit_count;
      s.new_bits = BitStream_ReadBits(s.bit_count);
      if (s.new_bits != 0xffffffff) {
        s.lookahead = s.lookahead | (int)(s.new_bits << (8 - s.bit_count));
      }
      else {
        s.lookahead = (int)s.new_bits;
      }
    }
    else {
      s.node = *(int *)(DAT_10020e88 + s.lookahead * 0xc + 8);
      while ((s.bit = BitStream_ReadBit()) != 0xffffffff) {
        if (s.bit != 0) {
          s.symbol = *(int *)(DAT_10021aa0 + s.node * 8);
        }
        else {
          s.symbol = *(int *)(DAT_10021aa0 + s.node * 8 + 4);
        }
        s.node = s.symbol - DAT_10021a98;
        if (s.node < 0) {
          *(undefined4 *)out_dwords = *(undefined4 *)(DAT_10021a9c + s.symbol * 4);
          out_dwords = out_dwords + 1;
          break;
        }
      }
      s.lookahead = (int)BitStream_ReadBits(8);
    }
  }
  return (int)out_dwords - (int)s.out_base >> 2;
}

// FUNCTION: CARDARTLIB 0x100025cb
// FUNCTION: DRAWCARDLIB 0x1000352b
int Huffman13_Init(undefined4 bitstream_start,undefined4 symbol_table,undefined4 node_index_base)
{
  struct {
    int result;
    int i;
  } s;

  s.result = 0xd;
  s.i = 0;
  
  for (; s.i < 0x20; s.i++) {
    DAT_10020e00[s.i] = 0xffffffff >> (byte)s.i;
  }

  DAT_10021a88 = bitstream_start;
  DAT_10021a8c = DAT_10021a88;
  DAT_10021a90 = 100000;
  DAT_1001d1dc = 0;
  DAT_10020e84 = DAT_1001d1dc;
  DAT_10021a94 = BitStream_ReadBits(0xd);
  for (s.i = 0; (int)DAT_10021a94 > s.i; s.i++) {
    ((uint *)&DAT_10021aa0)[s.i*2] = BitStream_ReadBits(0xd);
    ((uint *)&DAT_10021aa4)[s.i*2] = BitStream_ReadBits(0xd);
    s.result += 0x1a;
  }
  DAT_10021a98 = node_index_base;
  DAT_10021a9c = symbol_table;
  Huffman13_BuildDecodeTable(DAT_10021a94);

  s.result = (s.result / 8) + ((s.result & 7) != 0);
  return s.result;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100026e3
// FUNCTION: DRAWCARDLIB 0x10003643
int Huffman13_BuildDecodeTable(int node_count)
{
  struct {
    int prev_depth;   /* -0xfc */
    int bit_i_8;      /* -0xf8 */
    int suffix;       /* -0xf4 */
    int bit_i;        /* -0xf0 */
    int prefix;       /* -0xec */
    int i;            /* -0xe8 */
    int pow2[11];     /* -0xe4..-0xbc */
    int bits[0x20];   /* -0xb8..-0x3c */
    int depth;        /* -0x38 */
    int node_stack[9];/* -0x34..-0x14 */
    int pad_10;       /* -0x10 */
    int pad_0c;       /* -0x0c */
    int sym;          /* -0x08 */
    int node;         /* -0x04 */
  } s;

  s.depth = 0;
  s.pow2[0] = 1;
  s.pow2[1] = 2;
  s.pow2[2] = 4;
  s.pow2[3] = 8;
  s.pow2[4] = 0x10;
  s.pow2[5] = 0x20;
  s.pow2[6] = 0x40;
  s.pow2[7] = 0x80;
  s.pow2[8] = 0x100;
  s.pow2[9] = 0x200;
  s.pow2[10] = 0x400;

  for (s.i = 0; s.i < 0x20; s.i++) {
    s.bits[s.i] = 1;
  }

  s.node = node_count - 1;
  s.node_stack[0] = s.node;

  do {
    s.prev_depth = s.depth;
    s.depth++;

    if (s.bits[s.prev_depth] != 0) {
      s.sym = ((int *)DAT_10021aa0)[s.node * 2];
    } else {
      s.sym = ((int *)DAT_10021aa0)[s.node * 2 + 1];
    }

    s.node = s.sym - DAT_10021a98;
    s.node_stack[s.depth] = s.node;

    if (s.node < 0) {
      s.bit_i = 0;
      s.prefix = 0;
      while (s.depth > s.bit_i) {
        s.prefix |= s.bits[s.bit_i] << (byte)s.bit_i;
        s.bit_i++;
      }

      for (s.i = 0; s.i < s.pow2[8 - s.depth]; s.i++) {
        s.suffix = s.i << (byte)s.depth;
        ((int *)(DAT_10020e88 + 4))[((s.suffix | s.prefix) * 3)] = s.depth;
        ((int *)(DAT_10020e88 + 0))[((s.suffix | s.prefix) * 3)] = ((int *)DAT_10021a9c)[s.sym];
        ((int *)(DAT_10020e88 + 8))[((s.suffix | s.prefix) * 3)] = -1;
      }

      s.bits[s.depth] = 1;
      s.depth--;
      s.bits[s.depth]--;
      s.node = s.node_stack[s.depth];
    } else if (s.depth == 8) {
      s.bit_i_8 = 0;
      s.prefix = 0;
      while (s.depth > s.bit_i_8) {
        s.prefix |= s.bits[s.bit_i_8] << (byte)s.bit_i_8;
        s.bit_i_8++;
      }

      ((int *)(DAT_10020e88 + 0))[(s.prefix * 3)] = 0x7fffffff;
      ((int *)(DAT_10020e88 + 4))[(s.prefix * 3)] = s.depth;
      ((int *)(DAT_10020e88 + 8))[(s.prefix * 3)] = s.node;

      s.bits[s.depth] = 1;
      s.depth--;
      s.bits[s.depth]--;
      s.node = s.node_stack[s.depth];
    } else
      continue;

    for (;s.bits[0] >= 0 && s.bits[s.depth] < 0;) {
      s.bits[s.depth] = 1;
      s.depth--;
      s.bits[s.depth]--;
      s.node = s.node_stack[s.depth];
    }
  } while (s.bits[0] >= 0);

  return 0;
}

// FUNCTION: CARDARTLIB 0x10002cde
// FUNCTION: DRAWCARDLIB 0x10003c3e
int Huffman13_DecodeDwordsWithZeroRuns(undefined8 *out_dwords,uint *bitstream,undefined4 bitstream_end)
{
  struct {
    int run_len10;        /* -0x2c */
    int run_bits;         /* -0x28 */
    int bit_count;        /* -0x24 */
    int bits8;            /* -0x20 */
    int align_bytes;      /* -0x1c */
    int bit;              /* -0x18 */
    int zero;             /* -0x14 */
    int leaf_sym;         /* -0x10 */
    undefined8 *out_base; /* -0x0c */
    int lookahead;        /* -0x08 */
    int tree_node;        /* -0x04 */
  } s;

  s.zero = 0;
  s.out_base = out_dwords;

  DAT_10021a88 = bitstream;
  DAT_10021a8c = DAT_10021a88;
  DAT_10021a90 = bitstream_end;

  if (((uint)bitstream & 3) == 0) {
    DAT_1001d1dc = 0;
    DAT_10020e84 = DAT_1001d1dc;
  } else {
    s.align_bytes = 4 - ((uint)bitstream & 3);
    DAT_1001d1dc = s.align_bytes * 8;
    DAT_10020e84 = 0xffffffffU >> (0x20U - DAT_1001d1dc) & *bitstream;
    *(int *)&DAT_10021a88 += s.align_bytes;
  }

  s.lookahead = (int)BitStream_ReadBits(8);

  while (s.lookahead != -1) {
    if (((int *)DAT_10020e88)[s.lookahead * 3] < 0x7fffffff) {
      s.bit_count = ((int *)(DAT_10020e88 + 4))[s.lookahead * 3];

      if (((int *)DAT_10020e88)[s.lookahead * 3] == (int)0x80000000) {
        s.run_bits = (int)BitStream_ReadBits((uint)(s.bit_count + 2));
        if (s.run_bits < 0) {
          break;
        }

        s.run_bits = (s.lookahead >> (byte)s.bit_count) | (s.run_bits << (8 - s.bit_count));

        MemZeroDwords(out_dwords, (uint)s.run_bits);
        *(int *)&out_dwords += (s.run_bits << 2);
        s.lookahead = (int)BitStream_ReadBits(8);
      }
      else {
        *(int *)out_dwords = ((int *)DAT_10020e88)[s.lookahead * 3];
        out_dwords = (undefined8 *)((int)out_dwords + 4);

        s.lookahead = s.lookahead >> (byte)s.bit_count;
        s.bits8 = (int)BitStream_ReadBits((uint)s.bit_count);
        if (s.bits8 != -1) {        
          s.lookahead |= (s.bits8 << (8 - s.bit_count));
        }
        else
          break;
      }
    }
    else {
      s.tree_node = ((int *)(DAT_10020e88 + 8))[s.lookahead * 3];

      while ((s.bit = (int)BitStream_ReadBit()) != -1) {
        if (s.bit != 0) {
          s.leaf_sym = ((int *)DAT_10021aa0)[s.tree_node * 2];
        } else {
          s.leaf_sym = ((int *)DAT_10021aa0)[s.tree_node * 2 + 1];
        }

        s.tree_node = s.leaf_sym - DAT_10021a98;

        if (s.tree_node < 0) {
          if (s.leaf_sym == 0) {
            s.run_len10 = (int)BitStream_ReadBits(10);
            if (s.run_len10 < 0) {
              break;
            }

            MemZeroDwords(out_dwords, (uint)s.run_len10);
            *(int *)&out_dwords += (s.run_len10 << 2);
          }
          else {
            *(int *)out_dwords = *(int *)(DAT_10021a9c + s.leaf_sym * 4);
            out_dwords = (undefined8 *)((int)out_dwords + 4);
          }
          break;
        }
      }
    
      s.lookahead = (int)BitStream_ReadBits(8);
    }
  }

  (void)s.zero;
  return ((int)out_dwords - (int)s.out_base) >> 2;
}

// FUNCTION: CARDARTLIB 0x10002f70
// FUNCTION: DRAWCARDLIB 0x10003ed0
uint BitStream_ReadBits(uint bit_count)
{
  uint bits_in_buf;
  uint result;

  if (DAT_1001d1dc > bit_count) {
    result = DAT_10020e00[0x20 - bit_count] & DAT_10020e84;
    DAT_10020e84 = DAT_10020e84 >> (byte)bit_count;
    DAT_1001d1dc = DAT_1001d1dc - bit_count;
    return result;
  } else {
    bits_in_buf = DAT_1001d1dc;
    result = DAT_10020e84;
    bit_count = (int) bit_count - (int)DAT_1001d1dc;

    if ((int)DAT_10021a88 - DAT_10021a8c + 4 >= DAT_10021a90) {
      return 0xffffffff;
    }

    DAT_10020e84 = *DAT_10021a88;
    DAT_10021a88 = DAT_10021a88 + 1;
    result = result | (DAT_10020e00[0x20 - bit_count] & DAT_10020e84) << (byte)bits_in_buf;
    DAT_10020e84 = DAT_10020e84 >> (byte)bit_count;
    DAT_1001d1dc = 0x20 - bit_count;
  }
  return result;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003060
// FUNCTION: DRAWCARDLIB 0x10003fc0
static uint BitStream_ReadBit(void)
{
  uint result = 0;
  
  if (DAT_1001d1dc == 0) {
    if (DAT_10021a90 <= (int)DAT_10021a88 - DAT_10021a8c) {
      return 0xffffffff;
    }
    DAT_10020e84 = *DAT_10021a88;
    DAT_10021a88 = DAT_10021a88 + 1;
    DAT_1001d1dc = 0x20;
  }
  if ((DAT_10020e84 & 1) != 0)
    result = 1;

  DAT_10020e84 = DAT_10020e84 >> 1;
  DAT_1001d1dc = DAT_1001d1dc + -1;
  return result;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100030f0
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID reserved)
{
  uint uVar1;
  
  (void)reserved;
  switch(reason) {
  case 1:
    uVar1 = 1;
    uVar1 = (uint)CardArtLib_Initialize(instance) & uVar1;
    return uVar1;
  case 2:
    return 1;
  case 3:
    return 1;
  case 0:
    CardArtLib_Shutdown();
    return 1;
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003175
static BOOL CardArtLib_Initialize(HINSTANCE instance)
{
  struct {
    char *last_slash;              /* -0x4a4a4 */
    char cards_dat_path[0x108];    /* -0x4a4a0 */
    int card_count;                /* -0x4a398 */
    int unk_4a394;                 /* -0x4a394 */
    int idx;                       /* -0x4a390 */
    FILE *cards_file;              /* -0x4a38c */
    card_ptr_t records[2000];      /* -0x4a388 */
    int result;                    /* -0x08 */
    int i;                         /* -0x04 */
  } s;

  s.result = 1;
  _DAT_10121fe4 = (undefined4)instance;
  InitializeCriticalSection(&DAT_10117a20);
  InitializeCriticalSection(&DAT_101221f0);
  InitializeCriticalSection(&global_critical_section_for_catalog);
  GetModuleFileNameA((HMODULE)0x0,&global_base_directory,0x105);
  s.last_slash = strrchr(&global_base_directory,0x5c);
  *s.last_slash = '\0';
  strcpy(DAT_10117910,global_base_directory);
  strcat(DAT_10117910,s__CARDART_1001d1e0);
  InitCardArtGdiResources();
  strcpy(s.cards_dat_path,global_base_directory);
  strcat(s.cards_dat_path,s__CARDS_DAT_1001d1ec);
  s.cards_file = fopen(s.cards_dat_path,&s__rb_1001d1f8);

  if (s.cards_file != (FILE *)0x0) {
    fread(&s.card_count,4,1,s.cards_file);
    fread(&s.unk_4a394,4,1,s.cards_file);
    fread(s.records,0x98,(size_t)s.card_count,s.cards_file);

    for (s.idx = 0; s.idx < s.card_count; s.idx = s.idx + 1) {
      if (s.records[s.idx].num_pics > 0) {
        g_cardPicCounts[s.idx] = s.records[s.idx].num_pics;
      } else {
        g_cardPicCounts[s.idx] = 1;
      }
    }

    fclose(s.cards_file);
  } else {
    for (s.idx = 0; s.idx < s.card_count; s.idx = s.idx + 1) {
      g_cardPicCounts[s.idx] = 1;
    }
  }

  for (s.i = 0; s.i < 2000; s.i = s.i + 1) {
    g_SmallArtCache[s.i].hbm = 0;
  }

  for (s.i = 0; s.i < 100; s.i = s.i + 1) {
    g_versionedSmallArtCache[s.i].hbm = 0;
  }
  g_versionedSmallArtCount = 0;

  for (s.i = 0; s.i < 0x14; s.i = s.i + 1) {
    g_versionedBigArtCache[s.i].hbm = 0;
  }

  g_versionedBigArtCount = 0;
  global_dither_kernel_id = 3;
  return s.result;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000343e
void CardArtLib_Shutdown(void)
{
  DestroyAllSmallArts();
  DestroyAllBigArts();
  ShutdownCardArtGdiResources();
  DeleteCriticalSection(&DAT_10117a20);
  DeleteCriticalSection(&DAT_101221f0);
  DeleteCriticalSection(&global_critical_section_for_catalog);
}

// FUNCTION: CARDARTLIB 0x10003480
int LoadBigArt(int id,int version,int width,int height)
{
  struct {
    int align_bytes;          /* -0x158 */
    HDC desktop_hdc;          /* -0x154 */
    char wvl_path[0x108];     /* -0x150 */
    int existing;             /* -0x48 */
    HBITMAP bitmap;           /* -0x44 */
    HDC mem_dc;               /* -0x40 */
    int ok;                   /* -0x3c */
    int *wvl_entry;           /* -0x38 */
    void *dib_bits;           /* -0x34 */
    uint *decoded;            /* -0x30 */
    BITMAPINFO bmi;           /* -0x2c */
  } s;

  s.ok = 1;
  if (id == -1) {
    return 0;
  }

  EnterCriticalSection(&DAT_101221f0);

  s.existing = IsBigArtIn(id,version);
  if (s.existing != 0) {
    if ((*(int *)(s.existing + 8) == width) && (*(int *)(s.existing + 0xc) == height)) {
      LeaveCriticalSection(&DAT_101221f0);
      return 1;
    }
    else
      DestroyBigArt(id,version);
  }

  if ((g_cardPicCounts[id] > 1) && (version != 0)) {
    sprintf(s.wvl_path,s__s__04d_c_WVL_1001d1fc,&DAT_10117910,id,(int)(char)(version + '`'));
  } else {
    sprintf(s.wvl_path,s__s__04d_WVL_1001d20c,&DAT_10117910,id);
  }

  s.desktop_hdc = GetDC(GetDesktopWindow());
  global_color_depth = GetDeviceCaps(s.desktop_hdc,BITSPIXEL) * GetDeviceCaps(s.desktop_hdc,PLANES);
  ReleaseDC(GetDesktopWindow(),s.desktop_hdc);

  s.wvl_entry = Catalog_LoadWvlEntry(1,s.wvl_path,0);
  if (s.wvl_entry != (int *)0x0) {
    s.mem_dc = GetDC((HWND)0x0);
    ApplyCardArtPaletteToDc(s.mem_dc);
    InitBitmapInfo24bppTopDown(&s.bmi,width,height);
    s.bitmap = CreateDIBSection(s.mem_dc,&s.bmi,0,&s.dib_bits,(HANDLE)0x0,0);
    if (s.bitmap != (HBITMAP)0x0) {
      s.decoded = Wvl_DecodeToBgr24((uint *)0x0,s.wvl_entry,width,height);
      if (s.decoded != (uint *)0x0) {
        if ((-(width + width - width) & 3U) != 0) {
          s.align_bytes = 4 - (-(width + width - width) & 3U);
        }
        else
          s.align_bytes = 0;

        memcpy(s.dib_bits,s.decoded,(width * 3 + s.align_bytes) * height);
      } else {
        s.ok = 0;
        DeleteObject(s.bitmap);
      }
    } else {
      s.ok = 0;
    }
    ReleaseDC((HWND)0x0,s.mem_dc);
    Catalog_Unlock(s.wvl_entry);
  } else {
    s.ok = 0;
  }

  if (s.ok != 0) {
    if ((int)g_versionedBigArtCount >= 0x14) {
      DestroyBigArt(g_versionedBigArtCache[0].id, g_versionedBigArtCache[0].version);
    }

    g_versionedBigArtCache[g_versionedBigArtCount].hbm = s.bitmap;
    g_versionedBigArtCache[g_versionedBigArtCount].bits = s.dib_bits;
    g_versionedBigArtCache[g_versionedBigArtCount].width = width;
    g_versionedBigArtCache[g_versionedBigArtCount].height = height;
    g_versionedBigArtCache[g_versionedBigArtCount].id = id;
    g_versionedBigArtCache[g_versionedBigArtCount].version = version;
    g_versionedBigArtCount = g_versionedBigArtCount + 1;
  }

  LeaveCriticalSection(&DAT_101221f0);
  return s.ok;
}

// FUNCTION: CARDARTLIB 0x100037ba
int IsBigArtIn(int id,int version)
{
  struct {
    int result;
    int i;
  } s;

  s.result = 0;
  if (id == -1)
    return 0;

  EnterCriticalSection(&DAT_101221f0);

  for (s.i = 0; s.i < g_versionedBigArtCount && s.result == 0; s.i++) {
    if (g_versionedBigArtCache[s.i].id == id && g_versionedBigArtCache[s.i].version == version) {
      s.result = &g_versionedBigArtCache[s.i];
    }
  }

  LeaveCriticalSection(&DAT_101221f0);

  return s.result;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003867
int IsBigArtRightSize(int id,int version,int width,int height)
{
  int local_8 = 0;
  if (id == -1) {
    return 0;
  }

  EnterCriticalSection(&DAT_101221f0);
  local_8 = IsBigArtIn(id,version);
  if ((local_8 != 0) && ((*(int *)(local_8 + 8) != width || (*(int *)(local_8 + 0xc) != height))))
  {
    local_8 = 0;
  }
  LeaveCriticalSection(&DAT_101221f0);

  return local_8;
}

// FUNCTION: CARDARTLIB 0x100038ed
int DrawBigArt(HDC hdc,RECT *rect,int id,int version)
{
  struct {
    int result;
    int found_index;
    int i;
    int found;
  } s;

  if (id == -1) {
    return 0;
  }

  EnterCriticalSection(&DAT_101221f0);
  s.i = 0;
  s.found = 0;
  for (; s.i < (int)g_versionedBigArtCount && (s.found == 0); s.i++) {
    if (g_versionedBigArtCache[s.i].id == id && g_versionedBigArtCache[s.i].version == version) {
      s.found = 1;
      s.found_index = s.i;
    }
  }

  if (s.found != 0) {
    s.result = DrawBitmapToRect(hdc,rect,g_versionedBigArtCache[s.found_index].hbm);
  }
  else {
    s.result = 0;
  }

  if (s.result == 0) {
    FillRect(hdc,rect,GetStockObject(2));
  }

  LeaveCriticalSection(&DAT_101221f0);
  return s.result;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100039ef
int ReloadBigArtIfWrongSize(int id,int version,int width,int height)
{
  if (id == -1) {
    return 0;
  }

  if (IsBigArtRightSize(id,version,width,height) != 0) {
    return 1;
  } else {
    DestroyBigArt(id,version);
  }

  if (LoadBigArt(id,version,width,height) != 0) {
    return 1;
  } else {
    return 0;
  }
}

// FUNCTION: CARDARTLIB 0x10003a80
void DestroyBigArt(int id,int version)
{
  struct {
    int j;
    int i;
    int local_4;
  } s;

  if (id == -1) {
    return;
  }

  EnterCriticalSection(&DAT_101221f0);
  s.i = 0;
  s.local_4 = 0;

  for (; s.i < g_versionedBigArtCount && (s.local_4 == 0); s.i++) {
    if (g_versionedBigArtCache[s.i].id == id && g_versionedBigArtCache[s.i].version == version) {
      s.local_4 = 1;

      if (g_versionedBigArtCache[s.i].hbm != 0) {
        DeleteObject(g_versionedBigArtCache[s.i].hbm);
      }

      g_versionedBigArtCount--;

      s.j = s.i;
      for (; s.j < g_versionedBigArtCount; s.j++) {
        g_versionedBigArtCache[s.j].hbm = g_versionedBigArtCache[s.j+1].hbm;
        g_versionedBigArtCache[s.j].bits = g_versionedBigArtCache[s.j+1].bits;
        g_versionedBigArtCache[s.j].width = g_versionedBigArtCache[s.j+1].width;
        g_versionedBigArtCache[s.j].height = g_versionedBigArtCache[s.j+1].height;
        g_versionedBigArtCache[s.j].id = g_versionedBigArtCache[s.j+1].id;
        g_versionedBigArtCache[s.j].version = g_versionedBigArtCache[s.j+1].version;
      }
    }
  }

  LeaveCriticalSection(&DAT_101221f0);
}

// FUNCTION: CARDARTLIB 0x10003c0a
void DestroyAllBigArts(void)
{
  int i;
  EnterCriticalSection(&DAT_101221f0);
  for (i = 0; i < g_versionedBigArtCount; i = i + 1) {
    DeleteObject(g_versionedBigArtCache[i].hbm);
  }
  g_versionedBigArtCount = 0;
  LeaveCriticalSection(&DAT_101221f0);
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003c70
// FUNCTION: DRAWCARDLIB 0x1000a520
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

// FUNCTION: CARDARTLIB 0x10003cf0
int LoadSmallArt(int id,int version,int width,int height)
{
  struct {
    int row_pad;
    HDC desktop_hdc;
    uint wvl_path[66];
    HBITMAP hbm;
    HDC hdc;
    int ok;
    int *wvl_entry;
    void *dib_bits;
    uint *decoded_bgr;
    BITMAPINFO bmi;
  } s;

  s.ok = 1;
  if (id == -1) {
    return 0;
  }

  if (g_cardPicCounts[id] > 1) {
    return LoadVersionedSmallArt(id,version,width,height);
  }

  EnterCriticalSection(&DAT_10117a20);

  if (g_SmallArtCache[id].hbm != (HBITMAP)0) {
    if (g_SmallArtCache[id].width == width && g_SmallArtCache[id].height == height) {
      LeaveCriticalSection(&DAT_10117a20);
      return 1;
    }
    else
      DestroySmallArt(id,0);
  }

  s.desktop_hdc = GetDC(GetDesktopWindow());
  global_color_depth = GetDeviceCaps(s.desktop_hdc,BITSPIXEL) * GetDeviceCaps(s.desktop_hdc,PLANES);
  ReleaseDC(GetDesktopWindow(),s.desktop_hdc);

  sprintf((char *)s.wvl_path,s__s__04d_WVL_1001d218,&DAT_10117910,id);
  s.wvl_entry = Catalog_LoadWvlEntry(0,s.wvl_path,0);
  if (s.wvl_entry != (int *)0x0) {
    s.hdc = GetDC((HWND)0x0);
    ApplyCardArtPaletteToDc(s.hdc);
    InitBitmapInfo24bppTopDown(&s.bmi,width,height);
    s.hbm = CreateDIBSection(s.hdc,&s.bmi,0,&s.dib_bits,(HANDLE)0x0,0);
    if (s.hbm != (HBITMAP)0x0) {
      s.decoded_bgr = Wvl_DecodeToBgr24((uint *)0x0,s.wvl_entry,width,height);
      if (s.decoded_bgr != (uint *)0x0) {
        do {
          if ((-(width + width - width) & 3) != 0) {
            s.row_pad = 4 - (-(width + width - width) & 3);
            break;
          }
          s.row_pad = 0;
        } while (0);
        memcpy(s.dib_bits,s.decoded_bgr,(width * 3 + s.row_pad) * height);
      }
      else {
        s.ok = 0;
        DeleteObject(s.hbm);
      }
    }
    else {
      s.ok = 0;
    }
    ReleaseDC((HWND)0x0,s.hdc);
    Catalog_Unlock(s.wvl_entry);
  }
  else {
    s.ok = 0;
  }

  if (s.ok != 0) {
    g_SmallArtCache[id].hbm = s.hbm;
    g_SmallArtCache[id].bits = s.dib_bits;
    g_SmallArtCache[id].width = width;
    g_SmallArtCache[id].height = height;
  }

  LeaveCriticalSection(&DAT_10117a20);
  return s.ok;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003fb4
int IsSmallArtIn(int id,int version)
{
  if (id == -1) {
    return 0;
  }

  if (g_cardPicCounts[id] > 1) {
    if (FindVersionedSmallArtCacheEntry(id,version) != (undefined *)0x0) {
      return 1;
    } else {
      return 0;
    }
  } else {  
    if (g_SmallArtCache[id].hbm != 0) {
      return 1;
    } else {
      return 0;
    }
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000403d
int DrawSmallArt(HDC hdc,RECT *rect,int id,int version)
{
  int result;
  if ((hdc == (HDC)0x0) || (rect == (RECT *)0x0)) {
    return 0;
  }
  
  if (id == -1) {
    return 0;
  }
  
  if (g_cardPicCounts[id] > 1) {
    DrawVersionedSmallArt(hdc,rect,id,version);
  }
  else {
    EnterCriticalSection(&DAT_10117a20);
    if (IsSmallArtIn(id,version) != 0) {
      result = DrawBitmapToRect(hdc,rect,g_SmallArtCache[id].hbm);
    } else {
      result = 0;
    }
    if (result == 0) {
      FillRect(hdc,rect,GetStockObject(2));
    }
    LeaveCriticalSection(&DAT_10117a20);
    
    return result;
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10004128
bool ReloadSmallArtIfWrongSize(int id,int version,int width,int height)
{
  struct {
    int result;
    HGDIOBJ old;
  } s;

  if (id == -1) {
    return 0;
  }

  if (g_cardPicCounts[id] > 1) {
    return ReloadVersionedSmallArtIfWrongSize(id,version,width,height);
  }

  EnterCriticalSection(&DAT_10117a20);

  if (g_SmallArtCache[id].hbm != (HGDIOBJ)0x0 && g_SmallArtCache[id].width == width && g_SmallArtCache[id].height == height) {    s.result = 1;
  }
  else {
    s.old = g_SmallArtCache[id].hbm;
    g_SmallArtCache[id].hbm = (HGDIOBJ)0x0;
    if (LoadSmallArt(id,version,width,height) != 0) {
      if (s.old != (HGDIOBJ)0x0) {
        DeleteObject(s.old);
      }
      s.result = 1;
    }
    else {
      g_SmallArtCache[id].hbm = s.old;
      s.result = 0;
    }
  }

  LeaveCriticalSection(&DAT_10117a20);
  return s.result;
}

// FUNCTION: CARDARTLIB 0x10004251
void DestroySmallArt(int id,int version)
{
  if (id == -1) 
    return;

  if ((g_cardPicCounts)[id] > 1) {
    DestroyVersionedSmallArt(id,version);
  } else {
    EnterCriticalSection(&DAT_10117a20);
    if (g_SmallArtCache[id].hbm != 0) {
      DeleteObject(g_SmallArtCache[id].hbm);
      g_SmallArtCache[id].hbm = 0;
    }
    LeaveCriticalSection(&DAT_10117a20);
  }
}

// FUNCTION: CARDARTLIB 0x100042dd
void DestroyAllSmallArts(void)
{
  int i;
  
  EnterCriticalSection(&DAT_10117a20);
  DestroyAllVersionedSmallArts();
  for (i = 0; i < 2000; i = i + 1) {
    if (g_cardPicCounts[i] <= 1) {
      DestroySmallArt(i, 0);
    }
  }
  LeaveCriticalSection(&DAT_10117a20);
}

// FUNCTION: CARDARTLIB 0x10004346
int LoadVersionedSmallArt(int id,int version,int width,int height)
{
  struct {
    int row_pad;
    uint wvl_path[66];
    HBITMAP hbm;
    HDC hdc;
    int ok;
    int *wvl_entry;
    void *dib_bits;
    uint *decoded_bgr;
    BITMAPINFO bmi;
    VersionedArtCacheEntry *cache_entry;
  } s;

  s.ok = 1;
  if (id == -1) {
    return 0;
  }

  EnterCriticalSection(&DAT_10117a20);

  s.cache_entry = (VersionedArtCacheEntry *)FindVersionedSmallArtCacheEntry(id,version);
  if (s.cache_entry != (VersionedArtCacheEntry *)0x0) {
    if ((s.cache_entry->width == width) && (s.cache_entry->height == height)) {
      LeaveCriticalSection(&DAT_10117a20);
      return 1;
    }
    else
      DestroyVersionedSmallArt(id,version);
  }

  if (version != 0) {
    sprintf((char *)s.wvl_path,s__s__04d_c_WVL_1001d224,&DAT_10117910,id,(int)(char)(version + '`'));
  }
  else {
    sprintf((char *)s.wvl_path,s__s__04d_WVL_1001d234,&DAT_10117910,id);
  }

  s.wvl_entry = Catalog_LoadWvlEntry(0,s.wvl_path,0);
  if (s.wvl_entry != (int *)0x0) {
    s.hdc = GetDC((HWND)0x0);
    ApplyCardArtPaletteToDc(s.hdc);
    InitBitmapInfo24bppTopDown(&s.bmi,width,height);
    s.hbm = CreateDIBSection(s.hdc,&s.bmi,0,&s.dib_bits,(HANDLE)0x0,0);
    if (s.hbm != (HBITMAP)0x0) {
      s.decoded_bgr = Wvl_DecodeToBgr24((uint *)0x0,s.wvl_entry,width,height);
      if (s.decoded_bgr != (uint *)0x0) {
        if ((-(width + width - width) & 3) != 0) {
          s.row_pad = 4 - (-(width + width - width) & 3);
        }
        else {
          s.row_pad = 0;
        }
        memcpy(s.dib_bits,s.decoded_bgr,(width * 3 + s.row_pad) * height);
      }
      else {
        s.ok = 0;
        DeleteObject(s.hbm);
      }
    }
    else {
      s.ok = 0;
    }
    ReleaseDC((HWND)0x0,s.hdc);
    Catalog_Unlock(s.wvl_entry);
  }
  else {
    s.ok = 0;
  }

  if (s.ok != 0) {
    g_versionedSmallArtCache[g_versionedSmallArtCount].hbm = s.hbm;
    g_versionedSmallArtCache[g_versionedSmallArtCount].bits = s.dib_bits;
    g_versionedSmallArtCache[g_versionedSmallArtCount].width = width;
    g_versionedSmallArtCache[g_versionedSmallArtCount].height = height;
    g_versionedSmallArtCache[g_versionedSmallArtCount].id = id;
    g_versionedSmallArtCache[g_versionedSmallArtCount].version = version;
    g_versionedSmallArtCount++;
  }

  LeaveCriticalSection(&DAT_10117a20);
  return s.ok;
}

// FUNCTION: CARDARTLIB 0x100045fe
VersionedArtCacheEntry *FindVersionedSmallArtCacheEntry(int id,int version)
{
  int i;
  VersionedArtCacheEntry *ptr;
  
  ptr = 0;
  if (id == -1)
    return 0;

  EnterCriticalSection(&DAT_10117a20);

  for (i = 0 ; i < (int)g_versionedSmallArtCount && ptr == 0; i++) {
    if (g_versionedSmallArtCache[i].id == id && g_versionedSmallArtCache[i].version == version) {
      ptr = &g_versionedSmallArtCache[i];
    }
  }

  LeaveCriticalSection(&DAT_10117a20);

  return ptr;
}

// FUNCTION: CARDARTLIB 0x100046aa
int DrawVersionedSmallArt(HDC hdc,RECT *rect,int id,int version)
{
  struct {
    int local_14;
    int local_10;
    int i;
    bool bVar1;
  } s;
  
  if (id == -1) 
    return 0;
   
  EnterCriticalSection(&DAT_10117a20);

  for (s.i = 0, s.bVar1 = false; s.i < g_versionedSmallArtCount && (!s.bVar1); s.i++) {
    if (g_versionedSmallArtCache[s.i].id == id && g_versionedSmallArtCache[s.i].version == version) {
      s.bVar1 = true;
      s.local_10 = s.i;
    }
  }
  
  if (s.bVar1) {
    s.local_14 = DrawBitmapToRect(hdc, rect, g_versionedSmallArtCache[s.local_10].hbm);
  }
  else {
    s.local_14 = 0;
  }
  if (s.local_14 == 0) {
    FillRect(hdc,rect,GetStockObject(2));
  }
  LeaveCriticalSection(&DAT_10117a20);

  return s.local_14;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100047ab
bool ReloadVersionedSmallArtIfWrongSize(int id,int version,int width,int height)
{
  VersionedArtCacheEntry *cache_entry;

  if (id == -1) {
    return 0;
  }

  cache_entry = FindVersionedSmallArtCacheEntry(id,version);
  if (cache_entry != (undefined *)0x0) {
    if ((*(int *)(cache_entry + 8) == width) && (*(int *)(cache_entry + 0xc) == height)) {
      return 1;
    }
    else {
      DestroyVersionedSmallArt(id,version);
    }
  }

  if (LoadVersionedSmallArt(id,version,width,height) != 0) {
    return 1;
  }
  else {
    return 0;
  }
}

// FUNCTION: CARDARTLIB 0x1000485a
void DestroyVersionedSmallArt(int id,int version)
{
  struct {
    int j;    
    int i;
    bool found;
  } s;

  if (id == -1) {
    return;
  }

  EnterCriticalSection(&DAT_10117a20);

  for (s.i = 0, s.found = 0;s.i < g_versionedSmallArtCount && s.found == 0; s.i++) {
    if (g_versionedSmallArtCache[s.i].id == id && g_versionedSmallArtCache[s.i].version == version) {
      s.found = 1;

      if (g_versionedSmallArtCache[s.i].hbm != 0) {
        DeleteObject(g_versionedSmallArtCache[s.i].hbm);
      }

      g_versionedSmallArtCount = g_versionedSmallArtCount + -1;

      for (s.j = s.i; s.j < g_versionedSmallArtCount; s.j++) {
        g_versionedSmallArtCache[s.j].hbm = g_versionedSmallArtCache[s.j + 1].hbm;
        g_versionedSmallArtCache[s.j].bits = g_versionedSmallArtCache[s.j + 1].bits;
        g_versionedSmallArtCache[s.j].width = g_versionedSmallArtCache[s.j + 1].width;
        g_versionedSmallArtCache[s.j].height = g_versionedSmallArtCache[s.j + 1].height;
        g_versionedSmallArtCache[s.j].id = g_versionedSmallArtCache[s.j + 1].id;
        g_versionedSmallArtCache[s.j].version = g_versionedSmallArtCache[s.j + 1].version;
      }
    }
  }

  LeaveCriticalSection(&DAT_10117a20);
}

// FUNCTION: CARDARTLIB 0x100049e4
void DestroyAllVersionedSmallArts(void)
{
  int i;
  
  EnterCriticalSection(&DAT_10117a20);
  for (i = 0; i < g_versionedSmallArtCount; i = i + 1) {
    DeleteObject(g_versionedSmallArtCache[i].hbm);
  }
  g_versionedSmallArtCount = 0;
  LeaveCriticalSection(&DAT_10117a20);
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10004a50
// FUNCTION: DRAWCARDLIB 0x1000a520
void * OctreeNode_Create(void)
{
  void *_Dst;
  
  _Dst = malloc(0x30);
  memset(_Dst,0,0x30);
  return _Dst;
}

// FUNCTION: CARDARTLIB 0x10004a83
// FUNCTION: DRAWCARDLIB 0x10001033
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

  if (g_paletteOctreeRoot != (int *)0x0) {
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
bool InitDiffSquaredLookupTable(void)
{
  int local_c;
  int i;
  
  if (DAT_1001d248 != 0) 
    return 0;

  for (i = 0, local_c = -0xff; i < 0x200; i++, local_c++) {
    DAT_100ed310[i] = local_c * local_c;
  }
  DAT_1001d248 = 1;

  return 1;
}

// FUNCTION: CARDARTLIB 0x10004d49
// FUNCTION: DRAWCARDLIB 0x100012f9
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
int Octree_Destroy(OctNode *rootPtr)
{
  int i;
  int result;
  
  result = 0;
  if (rootPtr->flags != 0) {   
      FreeIfNotNull(rootPtr);
      return 1;
  }

  for (i = 0; i < 8; i = i + 1) {
    if (rootPtr->children[i] != 0) {
      result = result + Octree_Destroy(rootPtr->children[i]);
    }
  }
  if (rootPtr->list != 0) {
    FreeIfNotNull(rootPtr->list);
  }
  FreeIfNotNull(rootPtr);

  return result;
}

// FUNCTION: CARDARTLIB 0x1000508d
// FUNCTION: DRAWCARDLIB 0x1000163d
void Octree_BuildPathBytesFromRgb(uint rgb,undefined8 *out_path_words)
{
  struct {
    int iVar2;
    int iVar3;
    int iVar1;
  } s;
  
  s.iVar3 = (rgb & 0xff);
  s.iVar2 = ((RGBQUAD *) &rgb)->rgbGreen;
  s.iVar1 = ((rgb & 0xff0000) >> 0x10);

  *out_path_words = ((undefined8 *)DAT_100ecb10)[s.iVar1] | 
    ((undefined8 *)DAT_10115cf0)[s.iVar2] | 
    ((undefined8 *)DAT_101164f0)[s.iVar3];
}

// FUNCTION: CARDARTLIB 0x100050f1
// FUNCTION: DRAWCARDLIB 0x100016a1
undefined4 InitOctreeBitTables(void)
{
  struct {
    int local_10;
    int j;
    int i;
  } s;
  
  for (s.i = 0; s.i < 0x100; s.i++) {
    for (s.j = 0, s.local_10 = 0x80; s.j < 8; s.j++, s.local_10 = s.local_10 >> 1) {
      PTR_DAT_1001d24c[s.i * 8 + s.j] = (s.i & s.local_10) != 0 ? 4 : 0;

      PTR_DAT_1001d250[s.i * 8 + s.j] = (s.i & s.local_10) != 0 ? 2 : 0;

      PTR_DAT_1001d254[s.i * 8 + s.j] = (s.i & s.local_10) != 0 ? 1 : 0;
    }
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x100051f5
// FUNCTION: DRAWCARDLIB 0x100017a5
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

  s.path = &g_octPathTmp;
  s.node = g_paletteOctreeRoot;

  Octree_BuildPathBytesFromRgb(param_1,&g_octPathTmp);

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
  
  Octree_BuildPathBytesFromRgb(param_1,(uint *)&DAT_10031eb0);

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
void DestroyPaletteOctree(void)
{
  Octree_Destroy(g_paletteOctreeRoot);
  g_paletteOctreeRoot = (int *)0x0;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000682c
// FUNCTION: DRAWCARDLIB 0x10002ddc
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

// FUNCTION: CARDARTLIB 0x10007273
// FUNCTION: DRAWCARDLIB 0x10008cf3
void SetBytes(void *dst, int value, size_t num)
{
  //This is weird because it doesn't do anything?
  __int64 uVar1;
  
  uVar1 = (__int64)(value << 8 | value);
  uVar1 |= uVar1 << 16;
  uVar1 |= uVar1 << 32;

  //TODO: this looks like real inline asm but who knows
  __asm {
    fld qword ptr [ebp - 8]
    mov edi, dst
    mov ecx, num
    push ecx
    shr ecx, 3
    dec ecx
    fst qword ptr [edi]
    add edi, 8
    dec ecx
    jne -0xc
    fstp qword ptr [edi]
    pop ecx
    and ecx, 7
    mov eax, value
    rep stosb
  }
}

// FUNCTION: CARDARTLIB 0x1000a210
void FreeIfNotNull(void *ptr)
{
    _free_dbg(ptr, 1);
}
