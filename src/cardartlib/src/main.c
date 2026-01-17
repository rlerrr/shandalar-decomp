#include "CardArtLib.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "inttypes.h"
#include "mystdbool.h"
#include "defs.h"

// SIZE 0xc
typedef struct CatalogEntry {
  int key;
  uint offset;
  uint size;
} CatalogEntry;
STATIC_ASSERT(sizeof(CatalogEntry) == 0xc, CatalogEntry_wrong_size);

// SIZE 0x114
typedef struct Catalog {
  FILE *file;
  int entry_count;
  CatalogEntry *entries;
  CatalogEntry *cached_entry;
  char path[0x104];
} Catalog;
STATIC_ASSERT(sizeof(Catalog) == 0x114, Catalog_wrong_size);

typedef BOOL (WINAPI code)(HINSTANCE, DWORD, LPVOID);

// FUNCTION: CARDARTLIB 0x1000a210
void FreeIfNotNull(void *ptr)
{
    _free_dbg(ptr, 1);
}

/* Forward declarations for globals referenced before their definitions. */
extern undefined4 DAT_100ed70c;
extern undefined1 DAT_10031ca0[0x400];
extern unsigned char DAT_100326d8[0x1000];
extern undefined1 DAT_100eaf00[0x2000];
extern undefined1 DAT_100ecb10[0x800];
extern undefined1 DAT_10115cf0[0x800];
extern undefined1 DAT_101164f0[0x800];

// GLOBAL: CARDARTLIB 0x1001d05c
char s__assertFile_txt_1001d05c[] = "\\assertFile.txt";

// GLOBAL: CARDARTLIB 0x1001d06c
char DAT_1001d06c[] = "at";

// GLOBAL: CARDARTLIB 0x1001d070
char s_File__s__Line__d_1001d070[] = "File-> %s, Line-> %d\n";

// GLOBAL: CARDARTLIB 0x1001d088
char s__s_s_1001d088[] = "%s%s\n";

// GLOBAL: CARDARTLIB 0x1001d090
char s_Assertion_Error_1001d090[] = "Assertion Error";

// GLOBAL: CARDARTLIB 0x1001d0a0
char s__assertFile_txt_1001d0a0[] = "\\assertFile.txt";

// GLOBAL: CARDARTLIB 0x1001d0b0
char DAT_1001d0b0[] = "at";

// GLOBAL: CARDARTLIB 0x1001d0b4
char s_File__s__Line__d_1001d0b4[] = "File-> %s, Line-> %d\n";

// GLOBAL: CARDARTLIB 0x1001d0cc
char s__s_s_1001d0cc[] = "%s%s\n";

// GLOBAL: CARDARTLIB 0x1001d0d4
char s_Assertion_Error_1001d0d4[] = "Assertion Error";

// GLOBAL: CARDARTLIB 0x1001d0e4
undefined4 DAT_1001d0e4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d0e8
char s__DUELPALall_TR_1001d0e8[] = "\\DUELPALall.TR";

// GLOBAL: CARDARTLIB 0x1001d0f8
char s__DUEL_plogpal_1001d0f8[] = "\\DUEL.plogpal";

// GLOBAL: CARDARTLIB 0x1001d108
undefined4 DAT_1001d108 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d10c
char s_Too_many_open_Catalogs__Max__d_1001d10c[] = "Too many open Catalogs: Max %d\n";

// GLOBAL: CARDARTLIB 0x1001d12c
char s_D__Newmagic_sources_NedCard_Cata_1001d12c[] = "D:\\Newmagic\\sources\\NedCard\\Catalog.c";

// GLOBAL: CARDARTLIB 0x1001d154
char DAT_1001d154[] = "rb";

// GLOBAL: CARDARTLIB 0x1001d158
char s_Duplicate_short_name_found_in_ca_1001d158[] = "Duplicate short name found in catalogs\n%s entry %d and\n%s entry %d\nShortName value 0x%08lx";

// GLOBAL: CARDARTLIB 0x1001d1b4
char s_D__Newmagic_sources_NedCard_Cata_1001d1b4[] = "D:\\Newmagic\\sources\\NedCard\\Catalog.c";

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

// GLOBAL: CARDARTLIB 0x1001d258
undefined4 DAT_1001d258 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d25c
undefined4 DAT_1001d25c = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d260
int DAT_1001d260[0xa] = {0,0,2,4,12,7,10,12,12,0};

// GLOBAL: CARDARTLIB 0x1001d288
int DAT_1001d288[8] = {0,0,2,2,3,2,3,3};

// GLOBAL: CARDARTLIB 0x1001d2b0
int DAT_1001d2b0[0xa] = {0,0,16,16,42,32,32,48};

// GLOBAL: CARDARTLIB 0x1001d2d8
undefined1 DAT_1001d2d8[0xc];

// GLOBAL: CARDARTLIB 0x1001d2e4
undefined1 DAT_1001d2e4[0x6b4];

// GLOBAL: CARDARTLIB 0x1001d998
undefined1 DAT_1001d998[0xc];

// GLOBAL: CARDARTLIB 0x1001d9a4
undefined1 DAT_1001d9a4[0x6b4];

// GLOBAL: CARDARTLIB 0x1001e058
undefined1 * PTR_DAT_1001e058 = DAT_10031ca0 + 0x200;

// GLOBAL: CARDARTLIB 0x1001e05c
undefined4 DAT_1001e05c = 0xFFFFFFFF;

// GLOBAL: CARDARTLIB 0x1001e060
undefined4 DAT_1001e060 = 0xFFFFFFFF;

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

// GLOBAL: CARDARTLIB 0x1001e090
char s_Not_enough_memory_for_delta_arra_1001e090[] = "Not enough memory for delta array\r\n";

// GLOBAL: CARDARTLIB 0x1001e0b4
char s_D__Newmagic_sources_NedCard_Pale_1001e0b4[] = "D:\\Newmagic\\sources\\NedCard\\Palette.c";

// GLOBAL: CARDARTLIB 0x1001e0e0
int DAT_1001e0e0 = 0x00000004;

// GLOBAL: CARDARTLIB 0x1001e118
unsigned char * g_waveletScaleToByteTable = DAT_100326d8 + 0x400;

// GLOBAL: CARDARTLIB 0x1001e11c
undefined4 DAT_1001e11c = 0x00000001;

// GLOBAL: CARDARTLIB 0x1001e120
undefined4 DAT_1001e120 = 0x00000002;

// GLOBAL: CARDARTLIB 0x1001e124
undefined4 g_haarScratchInit = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001e128
undefined1 * g_yuvClampTable = DAT_100eaf00 + 0x400;

// GLOBAL: CARDARTLIB 0x1001e12c
char s_SmallArt_cat_1001e12c[] = "SmallArt.cat";

// GLOBAL: CARDARTLIB 0x1001e13c
char s_MedArt_cat_1001e13c[] = "MedArt.cat";

// GLOBAL: CARDARTLIB 0x1001e148
char s__lf_1001e148[] = "\n";

// GLOBAL: CARDARTLIB 0x1001e14c
char s_wavelet_pieces_has_illegal_value_1001e14c[] = "wavelet pieces has illegal value: %d";

// GLOBAL: CARDARTLIB 0x1001e174
char s_D__Newmagic_sources_NedCard_haar_1001e174[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: CARDARTLIB 0x1001e198
char s_Only_Works_on_24_bit_images_1001e198[] = "Only Works on 24 bit images\n";

// GLOBAL: CARDARTLIB 0x1001e1b8
char s_D__Newmagic_sources_NedCard_haar_1001e1b8[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: CARDARTLIB 0x1001e1dc
char s_Only_Works_on_24_bit_images_1001e1dc[] = "Only Works on 24 bit images\n";

// GLOBAL: CARDARTLIB 0x1001e1fc
char s_D__Newmagic_sources_NedCard_haar_1001e1fc[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: CARDARTLIB 0x1001e2a0
int __proc_attached = 0;

// GLOBAL: CARDARTLIB 0x10020210
char g_assertMessageBuffer[2000];

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

// GLOBAL: CARDARTLIB 0x10031aa0
undefined1 DAT_10031aa0 = 0x00;

// GLOBAL: CARDARTLIB 0x10031aa1
undefined1 DAT_10031aa1 = 0x00;

// GLOBAL: CARDARTLIB 0x10031ca0
undefined1 DAT_10031ca0[0x400];

// GLOBAL: CARDARTLIB 0x10031ea4
undefined4 g_paletteOctreeRoot = 0x00000000;

// GLOBAL: CARDARTLIB 0x10031ea8
undefined4 DAT_10031ea8 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10031eb0
undefined1 DAT_10031eb0 = 0x00;

// GLOBAL: CARDARTLIB 0x10031eb1
undefined1 DAT_10031eb1 = 0x00;

// GLOBAL: CARDARTLIB 0x10031eb8
undefined4 _DAT_10031eb8 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10031ec0
undefined1 g_paletteRgbTable[0x400];

// GLOBAL: CARDARTLIB 0x100322c0
undefined1 DAT_100322c0 = 0x00;

// GLOBAL: CARDARTLIB 0x100322c1
undefined1 DAT_100322c1 = 0x00;

// GLOBAL: CARDARTLIB 0x100322cc
undefined4 DAT_100322cc = 0x00000000;

// GLOBAL: CARDARTLIB 0x100322d0
undefined4 DAT_100322d0 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100322d4
int DAT_100322d4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100322d8
undefined1 DAT_100322d8 = 0x00;

// GLOBAL: CARDARTLIB 0x100322d9
undefined1 DAT_100322d9 = 0x00;

// GLOBAL: CARDARTLIB 0x10032adc
undefined4 DAT_10032adc = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032ae0
undefined4 g_waveletScaleToByteTableInit = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032ae8
int DAT_10032ae8[0x80];

// GLOBAL: CARDARTLIB 0x10032b04
undefined4 DAT_10032b04 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032b08
undefined4 DAT_10032b08 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032b10
undefined4 DAT_10032b10 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032c88
undefined4 DAT_10032c88 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032c8c
undefined4 DAT_10032c8c = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032c90
undefined4 DAT_10032c90 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032c94
undefined4 DAT_10032c94 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032c98
undefined1 DAT_10032c98[0x2000];

// GLOBAL: CARDARTLIB 0x100326d8
unsigned char DAT_100326d8[0x1000];

// GLOBAL: CARDARTLIB 0x100ad498
unsigned char DAT_100ad498[0x200];

// GLOBAL: CARDARTLIB 0x100ea098
undefined4 DAT_100ea098 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100ea09c
undefined4 g_yuvClampTableInit = 0x00000000;

// GLOBAL: CARDARTLIB 0x100ea31c
int *g_haarScratchB = (int *)0x0;

// GLOBAL: CARDARTLIB 0x100ea320
int *g_haarScratchA = (int *)0x0;

// GLOBAL: CARDARTLIB 0x100eab00
undefined1 DAT_100eab00 = 0x00;

// GLOBAL: CARDARTLIB 0x100eab01
undefined1 DAT_100eab01 = 0x00;

// GLOBAL: CARDARTLIB 0x100eaf00
undefined1 DAT_100eaf00[0x2000];

// GLOBAL: CARDARTLIB 0x100ecb04
undefined4 _DAT_100ecb04 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100ecb10
undefined1 DAT_100ecb10[0x800];

// GLOBAL: CARDARTLIB 0x100ed310
int DAT_100ed310[0x200 / 4];

// GLOBAL: CARDARTLIB 0x100ed70c
undefined4 DAT_100ed70c = 0x00000000;

// GLOBAL: CARDARTLIB 0x100edb10
undefined1 DAT_100edb10[5 * 0x8060];

// GLOBAL: CARDARTLIB 0x10115cf0
undefined1 DAT_10115cf0[0x800];

// GLOBAL: CARDARTLIB 0x101164f0
undefined1 DAT_101164f0[0x800];

// GLOBAL: CARDARTLIB 0x10116cf0
undefined2 DAT_10116cf0 = 0x0000;

// GLOBAL: CARDARTLIB 0x10116cf2
undefined2 DAT_10116cf2 = 0x0000;

// GLOBAL: CARDARTLIB 0x10116cf7
undefined1 DAT_10116cf7 = 0x00;

// GLOBAL: CARDARTLIB 0x101170f3
undefined1 DAT_101170f3 = 0x00;

// GLOBAL: CARDARTLIB 0x10117210
unsigned char DAT_10117210[0x80];

// GLOBAL: CARDARTLIB 0x10117290
Catalog DAT_10117290[5];

// GLOBAL: CARDARTLIB 0x10117100
void *DAT_10117100[0x41];

// GLOBAL: CARDARTLIB 0x101177f4
int g_versionedBigArtCount = 0x00000000;

// GLOBAL: CARDARTLIB 0x10117800
char DAT_10117800[0x105];

// GLOBAL: CARDARTLIB 0x10117910
char DAT_10117910[0x105];

// GLOBAL: CARDARTLIB 0x10121fe0
int g_versionedSmallArtCount = 0x00000000;

// GLOBAL: CARDARTLIB 0x10121fe4
undefined4 _DAT_10121fe4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x101221e8
undefined4 DAT_101221e8 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10117a20
CRITICAL_SECTION DAT_10117a20;

// GLOBAL: CARDARTLIB 0x101221f0
CRITICAL_SECTION DAT_101221f0;

// GLOBAL: CARDARTLIB 0x101221d0
CRITICAL_SECTION DAT_101221d0;

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
uint * Wvl_DecodeToBgr24(uint *param_1,int *param_2,int param_3,int param_4);
int * Catalog_LoadWvlEntry(int param_1,uint *param_2,int param_3);
undefined8 * Wvl_DecodeHaar(int *param_1,undefined8 *param_2);
void DestroyPaletteOctree(void);
int Huffman13_DecodeDwords(undefined4 *out_dwords,undefined4 bitstream_start,undefined4 bitstream_end);
int Huffman13_Init(undefined4 bitstream_start,undefined4 symbol_table,undefined4 node_index_base);
int Huffman13_BuildDecodeTable(int node_count);
int Huffman13_DecodeDwordsWithZeroRuns(undefined8 *out_dwords,uint *bitstream,undefined4 bitstream_end);
undefined4 Wvl_UnpackPieces(int param_1,int *param_2);
void ShutdownCardArtGdiResources(void);
void checked_DeleteDC_DeleteObject(HDC param_1,HGDIOBJ param_2);
void DestroyCardArtPalette(void);
void CardArtLib_Shutdown(void);
void InitBitmapInfo24bppTopDown(BITMAPINFO *bmi,int width,int height);
VersionedArtCacheEntry * FindVersionedSmallArtCacheEntry(int id,int version);
void DestroyVersionedSmallArt(int id,int version);
void DestroyAllVersionedSmallArts(void);
undefined2 * ReadPalette(char *param_1,char *param_2);
void CopyBgr24RectIntoStridedBuffer(undefined8 *dst_bgr24,undefined8 *src_bgr24,int dst_x,int dst_y,
                                           int rect_width,int rect_height,int dst_stride_pixels);
void CopyBytes(void *dst,const void *src,size_t size);
void SetBytes(void *dst,uint value,size_t size);
void Haar2D_ReconstructInPlace(int *coeffs,int full_size,int base_size);
void Haar_CombineSumDiff(int *src_a,int *src_b,int *dst,int width,int rows,undefined4 src_stride_unused,
                         int dst_stride);
void Haar_CombineSumDiffHalf(int *src_a,int *src_b,int *dst,int width,int rows,
                             undefined4 src_stride_unused,int dst_stride);
undefined1 * YuvPlanesToBgr24(undefined1 *out_bgr24,int *luma,int width,int height,int chroma_u,int chroma_v,
                 int chroma_stride,undefined4 unused_chroma_height,int chroma_is_420);


// MATCHING
// FUNCTION: CARDARTLIB 0x10001000
void assert(int condition,char *file,int line,char *fmt,...)
{
  char local_114 [260];
  time_t now;
  FILE *logFile;
  va_list args;

  if (condition == 0) {
    strcpy(local_114,DAT_10117800);
    strcat(local_114,s__assertFile_txt_1001d05c);
    logFile = fopen(local_114,DAT_1001d06c);

    va_start(args,fmt);
    if ((file != (char *)0x0) && (line != 0)) {
      sprintf(g_assertMessageBuffer,s_File__s__Line__d_1001d070,file,line);
    }

    _vsnprintf(g_assertMessageBuffer + strlen(g_assertMessageBuffer),2000,fmt,args);
    
    time(&now);

    fprintf(logFile,s__s_s_1001d088,ctime(&now),g_assertMessageBuffer);
    fclose(logFile);

    MessageBoxA((HWND)0x0,g_assertMessageBuffer,s_Assertion_Error_1001d090,0x1000);
    exit(0xff);  
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001116
void assert_noabort(int condition,char *file,int line,char *fmt,...)
{
  char local_114 [260];
  time_t now;
  FILE *logFile;
  va_list args;

  if (condition == 0) {
    strcpy(local_114,DAT_10117800);
    strcat(local_114,s__assertFile_txt_1001d0a0);
    logFile = fopen(local_114,DAT_1001d0b0);

    va_start(args,fmt);
    if ((file != (char *)0x0) && (line != 0)) {
      sprintf(g_assertMessageBuffer,s_File__s__Line__d_1001d0b4,file,line);
    }

    _vsnprintf(g_assertMessageBuffer + strlen(g_assertMessageBuffer),2000,fmt,args);
    
    time(&now);

    fprintf(logFile,s__s_s_1001d0cc,ctime(&now),g_assertMessageBuffer);
    fclose(logFile);

    MessageBoxA((HWND)0x0,g_assertMessageBuffer,s_Assertion_Error_1001d0d4,0x1000);
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001220
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
  strcpy((char *)s.local_624,DAT_10117800);
  strcat((char *)s.local_624,s__DUELPALall_TR_1001d0e8);
  strcpy((char *)s.local_510,DAT_10117800);
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
void DestroyCardArtPalette(void)
{
  DeleteObject(DAT_100209e0);
  DAT_100209e0 = (HGDIOBJ)0x0;
  DestroyPaletteOctree();
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100019d0
static int Catalog_Open(const char *catalog_path)
{
  //Stack layout won't behave
  struct {
    int iVar1; //ebp - 0x1c
    int entry_index; //ebp - 0x18
    Catalog *other_catalog; //ebp - 0x14
    Catalog *catalog; //ebp - 0x10
    
    int other_slot; //ebp - 0xc
    int slot_index; //ebp - 8
    FILE *catalog_file; //ebp - 4
  } s;

  s.slot_index = -1;

  for (s.other_slot = 0; s.other_slot < 5; s.other_slot++) {
    if (DAT_10117290[s.other_slot].file == 0) {
      s.slot_index = s.other_slot;
      break;
    }    
  }
  
  assert((uint)(s.slot_index != -1),s_D__Newmagic_sources_NedCard_Cata_1001d12c,0x43,
            s_Too_many_open_Catalogs__Max__d_1001d10c,5);
            
  s.catalog = &DAT_10117290[s.slot_index];
  s.catalog->cached_entry = (CatalogEntry *)0x0;
  strcpy(s.catalog->path,catalog_path);
  s.catalog->file = fopen(catalog_path,&DAT_1001d154);
  s.catalog_file = s.catalog->file;
  if (s.catalog_file == (FILE *)0x0)
    return 0;
  
  fread(&s.catalog->entry_count,4,1,s.catalog_file);
  s.catalog->entries = (CatalogEntry *)malloc(s.catalog->entry_count * sizeof(CatalogEntry));
  fread(s.catalog->entries,sizeof(CatalogEntry),(size_t)s.catalog->entry_count,s.catalog_file);
  if (DAT_1001d108 != 0) {
    for (s.other_slot = 0; s.other_slot < 5; s.other_slot = s.other_slot + 1) {
      if (s.slot_index == s.other_slot) 
        continue;
        
      if (DAT_10117290[s.other_slot].file == 0)
        continue;

      s.other_catalog = (Catalog *)((char *)&DAT_10117290 + s.other_slot * sizeof(Catalog));

      for (s.entry_index = 0; 0 != s.catalog->entry_count; s.entry_index++) {
        for (s.iVar1 = 0; s.iVar1 < s.other_catalog->entry_count; s.iVar1 = s.iVar1 + 1) {
          assert(s.catalog->entries[s.entry_index].key - s.other_catalog->entries[s.entry_index].key != 0,
                  s_D__Newmagic_sources_NedCard_Cata_1001d1b4,0x69,
                  s_Duplicate_short_name_found_in_ca_1001d158,s.catalog->path,s.entry_index,
                  s.other_catalog->path,s.iVar1,s.catalog->entries[s.entry_index].key);
        }
      }
    }
  }
    
  return s.slot_index + 1;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001c1a
bool Catalog_Close(int handle)
{
  if (DAT_10117290[--handle].file == 0) 
    return 0;
  FreeIfNotNull(DAT_10117290[handle].entries);
  fclose(DAT_10117290[handle].file);
  DAT_10117290[handle].entries = (CatalogEntry *)0x0;
  DAT_10117290[handle].file = (FILE *)0x0;
  DAT_10117290[handle].entry_count = 0;
  return 1;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001cd6
static int CatalogEntry_CompareKey(const int *key, const CatalogEntry *entry)
{
  if (entry->key < *key) {
    return 1;
  }
  else if (*key < entry->key) {
    return -1;
  } else {
    return 0;
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001d26
CatalogEntry *Catalog_FindEntryCached(Catalog *catalog,const char *name)
{
  CatalogEntry *entry;
  uint key;
  
  key = Catalog_MakeKeyFromPath(name);
  if ((catalog->cached_entry != (CatalogEntry *)0x0) && (catalog->cached_entry->key == key)) {
    return catalog->cached_entry;
  }

  entry = (CatalogEntry *)bsearch(&key,catalog->entries,(size_t)catalog->entry_count,sizeof(CatalogEntry),
                                  CatalogEntry_CompareKey);
  catalog->cached_entry = entry;    
  return entry;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001da1
size_t Catalog_ReadEntry(int catalog_handle,const char *name,void **buffer)

{
  Catalog *catalog;
  CatalogEntry *entry;

  catalog = (Catalog *)((char *)&DAT_10117290 + (catalog_handle + -1) * sizeof(Catalog));
  entry = Catalog_FindEntryCached(catalog,name);
  if (entry == (CatalogEntry *)0x0) {
    return 0xffffffff;
  }

  if (*buffer == (void *)0x0) {
    *buffer = malloc(entry->size + 0x10);
  }
  fseek(catalog->file,(long)entry->offset,0);
  return fread(*buffer,1,entry->size,catalog->file);  
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001e4a
uint Catalog_MakeKeyFromPath(const char *path)
{
  //Stack layout is a disaster with weird padding
  struct {
    int acc_odd;
    uint ext_buf[4];
    char dir_buf[256];
    
    undefined4 key;
    int pad0;
    undefined4 filename_buf;
    int pad1;
    int pad2;
    uint multiplier;
    int ch;
    int acc_even;
  } s;

  s.multiplier = 3;
  s.pad0 = 0;
  s.acc_odd = 0;
  s.acc_even = 0;
  _splitpath(path,(char *)s.ext_buf,s.dir_buf,(char *)&s.filename_buf,(char *)s.ext_buf);
  path = (char *)&s.filename_buf;
  strcat((char *)&s.filename_buf,(char *)s.ext_buf);
  s.key = (undefined4)(((int)(signed char)path[1] ^ (int)(signed char)path[0]) << 0x18);

  while ((s.ch = (int)(signed char)*(path++)) != 0) {
    if ((s.multiplier & 1) != 0) {
      s.acc_odd = s.multiplier * s.ch + s.acc_odd;
    }
    else {
      s.acc_even = s.multiplier * s.ch + s.acc_even;
    }
    s.multiplier = s.multiplier + 1;
  }

  s.key = (undefined4)((uint)s.key | (s.acc_even * s.acc_odd & 0xffffffU));
  return (uint)s.key;
}

// FUNCTION: CARDARTLIB 0x10001f40
void MemZeroDwords(undefined8 *param_1,uint param_2)
{
  //TODO: I don't think MSVC can generate this??  This gotta be a library
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
  InitializeCriticalSection(&DAT_101221d0);
  GetModuleFileNameA((HMODULE)0x0,&DAT_10117800,0x105);
  s.last_slash = strrchr(&DAT_10117800,0x5c);
  *s.last_slash = '\0';
  strcpy(DAT_10117910,DAT_10117800);
  strcat(DAT_10117910,s__CARDART_1001d1e0);
  InitCardArtGdiResources();
  strcpy(s.cards_dat_path,DAT_10117800);
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
  DAT_1001d258 = 3;
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
  DeleteCriticalSection(&DAT_101221d0);
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
  DAT_101221e8 = GetDeviceCaps(s.desktop_hdc,0xc) * GetDeviceCaps(s.desktop_hdc,0xe);
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

  for (s.i = 0; s.i < g_versionedBigArtCount && (s.result == (undefined *)0x0); s.i++) {
    if (g_versionedBigArtCache[s.i].id == id && g_versionedBigArtCache[s.i].version == version) {
      s.result = &g_versionedBigArtCache[s.i];
    }
  }

  LeaveCriticalSection(&DAT_101221f0);

  return (int)s.result;
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
  DAT_101221e8 = GetDeviceCaps(s.desktop_hdc,0xc) * GetDeviceCaps(s.desktop_hdc,0xe);
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
void * OctreeNode_Create(void)
{
  void *_Dst;
  
  _Dst = malloc(0x30);
  memset(_Dst,0,0x30);
  return _Dst;
}

// FUNCTION: CARDARTLIB 0x10004a83
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
void OctreeNode_CollectLeafIndices(int *param_1,int param_2,int *param_3)
{
  int i;
  
  if (*param_1 != 0) {
    *(char *)(*param_3 + param_2) = (char)param_1[1];
    *param_3 = *param_3 + 1;   
  }
  else {
     for (i = 0; i < 8; i = i + 1) {
      if (param_1[i + 2] != 0) {
        OctreeNode_CollectLeafIndices((int *)param_1[i + 2],param_2,param_3);
      }
    }
  }
}

// FUNCTION: CARDARTLIB 0x10004dc8
int OctreeNode_FinalizeSubtree(int *param_1)
{
  struct {
    size_t local_410;
    int i;
    int local_408;
    undefined1 local_404 [1024];
  } s;
  
  //iVar1 = DAT_100322d4;
  s.local_408 = 0;
  s.local_410 = 0;
  DAT_100322d4 = DAT_100322d4 + 1;
  if (DAT_1001d240 < DAT_100322d4) {
    DAT_1001d240 = DAT_100322d4;
  }
  if (*param_1 != 0) {
    DAT_100322d4--;
    return 1;
  }

  for (s.i = 0; s.i < 8; s.i = s.i + 1) {
    if (param_1[s.i + 2] != 0) {
      s.local_408 = s.local_408 + OctreeNode_FinalizeSubtree((int *)param_1[s.i + 2]);
      s.local_410 = s.local_410 + 1;
    }
  }
  if (s.local_410 != 0) {
    s.local_410 = 0;
    OctreeNode_CollectLeafIndices(param_1,(int)s.local_404,(int *)&s.local_410);
    param_1[10] = (int)malloc(s.local_410);
    param_1[0xb] = s.local_410;
    memcpy((void *)param_1[10],s.local_404,s.local_410);
  }
  DAT_100322d4 = DAT_100322d4 + -1;

  return s.local_408;
}

// FUNCTION: CARDARTLIB 0x10004f07
undefined4 Octree_InsertPathString(undefined4 *param_1,char *param_2,undefined4 param_3)
{
  int iVar2;

  param_2 += strspn(param_2,s__sp_lf_1001e07c);
  while (*param_2 != '\0') {
    iVar2 = atoi(param_2);
    if (param_1[iVar2 + 2] == 0) {
      param_1[iVar2 + 2] = OctreeNode_Create();
    }
    param_1 = (undefined4 *)param_1[iVar2 + 2];

    param_2 += strspn(param_2 +=  
        strcspn(param_2 + strspn(param_2,s__sp_lf__1001e088),s__sp_lf__1001e084) + 
        strspn(param_2,s__sp_lf__1001e080)
      ,s__sp_lf__1001e08c);
  }
  *param_1 = 1;
  param_1[1] = param_3;
  return 0;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10004fe1
int Octree_Destroy(int *rootPtr)
{
  int i;
  int result;
  
  result = 0;
  if (*rootPtr != 0) {   
      FreeIfNotNull(rootPtr);
      return 1;
  }

  for (i = 0; i < 8; i = i + 1) {
    if (rootPtr[i + 2] != 0) {
      result = result + Octree_Destroy((int *)rootPtr[i + 2]);
    }
  }
  if (rootPtr[10] != 0) {
    FreeIfNotNull((void *)rootPtr[10]);
  }
  FreeIfNotNull(rootPtr);

  return result;
}

// FUNCTION: CARDARTLIB 0x1000508d
void Octree_BuildPathBytesFromRgb(uint rgb,char *out_path_words)
{
  struct {
    int iVar2;
    int iVar3;
    int iVar1;
  } s;
  
  s.iVar3 = (rgb & 0xff);
  s.iVar2 = ((RGBQUAD *) &rgb)->rgbGreen;
  s.iVar1 = ((rgb & 0xff0000) >> 0x10);

  *(uint *)out_path_words = *(uint *)(DAT_100ecb10 + s.iVar1 * 8) | *(uint *)(DAT_10115cf0 + s.iVar2 * 8) | *(uint *)(DAT_101164f0 + s.iVar3 * 8);
  *(uint *)(out_path_words + 4) = *(uint *)(DAT_100ecb10 + s.iVar1 * 8 + 4) | *(uint *)(DAT_10115cf0 + s.iVar2 * 8 + 4) | *(uint *)(DAT_101164f0 + s.iVar3 * 8 + 4);
}

// FUNCTION: CARDARTLIB 0x100050f1
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
undefined4 Octree_FindNearestColor(uint param_1)
{
  struct {
    int pal_r;        /* ebp - 0x34 */
    int best_dist;    /* ebp - 0x30 */
    int idx_list_base;/* ebp - 0x2c */
    uint best_idx;    /* ebp - 0x28 */
    int i;            /* ebp - 0x24 */
    int green;        /* ebp - 0x20 */
    int dist;         /* ebp - 0x1c */
    int pal_g;        /* ebp - 0x18 */
    int blue;         /* ebp - 0x14 */
    int red;          /* ebp - 0x10 */
    int pal_b;        /* ebp - 0xc */
    int *node;        /* ebp - 8 */
    byte *path;       /* ebp - 4 */
  } s;

  s.path = &DAT_100322c0;
  s.node = (int *)g_paletteOctreeRoot;

  Octree_BuildPathBytesFromRgb(param_1,(char *)&DAT_100322c0);

  for (;;) {
    s.idx_list_base = s.node[(uint)*s.path + 2];
    s.path = s.path + 1;
    if (s.idx_list_base == 0) {
      if (*s.node == 0) {
        s.idx_list_base = s.node[10];
        s.best_dist = 0x7fffffff;
        s.blue = (int)(param_1 & 0xff);
        s.green = (int)((byte *)&param_1)[1];
        s.red = (int)((param_1 & 0xff0000) >> 0x10);

        for (s.i = 0; s.i < s.node[0xb]; s.i = s.i + 1) {
          s.dist = (uint)*(byte *)(s.idx_list_base + s.i);
          s.pal_g = (int)(uint)g_paletteRgbTable[(uint)s.dist * 4 + 1];

          s.pal_r = *(uint *)(g_paletteRgbTable + (uint)s.dist * 4);
          s.pal_b = s.pal_r & 0xff;
          s.pal_r = (s.pal_r & 0xff0000) >> 0x10;

          s.dist = ((int *)PTR_DAT_1001d244)[s.green - s.pal_g] +
                   ((int *)PTR_DAT_1001d244)[s.blue - s.pal_b] +
                   ((int *)PTR_DAT_1001d244)[s.red - s.pal_r];

          if (s.dist < s.best_dist) {
            s.best_dist = s.dist;
            s.best_idx = (uint)*(byte *)(s.idx_list_base + s.i);
          }
        }
        return *(undefined4 *)(g_paletteRgbTable + s.best_idx * 4);
      }
      return *(undefined4 *)(g_paletteRgbTable + s.node[1] * 4);
    }
    s.node = (int *)s.idx_list_base;
    if ((char)*s.node == '\x01') break;
  }
  return *(undefined4 *)(g_paletteRgbTable + s.node[1] * 4);
}

// FUNCTION: CARDARTLIB 0x10005383
uint Octree_FindNearestPaletteIndex(uint param_1)
{
  int *piVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  byte *pbVar5;
  int local_34;
  uint local_2c;
  int local_28;
  
  Octree_BuildPathBytesFromRgb(param_1,(uint *)&DAT_10031eb0);
  pbVar5 = &DAT_10031eb0;
  piVar3 = g_paletteOctreeRoot;
  do {
    piVar1 = (int *)piVar3[*pbVar5 + 2];
    pbVar5 = pbVar5 + 1;
    if (piVar1 == (int *)0x0) {
      if (*piVar3 == 0) {
        iVar2 = piVar3[10];
        local_34 = 0x7fffffff;
        for (local_28 = 0; local_28 < piVar3[0xb]; local_28 = local_28 + 1) {
          iVar4 = *(int *)(PTR_DAT_1001d244 +
                          ((param_1 & 0xff) -
                          ((*(uint *)(g_paletteRgbTable + (uint)*(byte *)(local_28 + iVar2) * 4) & 0xff0000)
                           >> 0x10)) * 4) +
                  *(int *)(PTR_DAT_1001d244 +
                          (((param_1 & 0xff0000) >> 0x10) -
                          (*(uint *)(g_paletteRgbTable + (uint)*(byte *)(local_28 + iVar2) * 4) & 0xff)) *
                          4) +
                  *(int *)(PTR_DAT_1001d244 +
                          ((param_1 >> 8 & 0xff) -
                          (uint)g_paletteRgbTable[(uint)*(byte *)(local_28 + iVar2) * 4 + 1]) * 4);
          if (iVar4 < local_34) {
            local_2c = (uint)*(byte *)(local_28 + iVar2);
            local_34 = iVar4;
          }
        }
        return local_2c;
      }
      return piVar3[1];
    }
    piVar3 = piVar1;
  } while ((char)*piVar1 != '\x01');
  return piVar1[1];
}

// FUNCTION: CARDARTLIB 0x100054fb
int Octree_FlattenLeafValues(int *param_1,int *param_2)

{
  int iVar1;
  int local_c;
  int local_8;
  
  local_8 = 0;
  if (*param_1 == 0) {
    for (local_c = 0; local_c < 8; local_c = local_c + 1) {
      if (param_1[local_c + 2] != 0) {
        iVar1 = Octree_FlattenLeafValues((int *)param_1[local_c + 2],param_2);
        local_8 = local_8 + iVar1;
        param_2 = param_2 + iVar1;
      }
    }
  }
  else {
    *param_2 = param_1[1];
    local_8 = 1;
  }
  return local_8;
}

// FUNCTION: CARDARTLIB 0x10005591
undefined4 QuantizeBgr24ToNearestPaletteColorInPlace(uint *bgr24,int height,int width,int row_padding)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 local_18;
  undefined4 local_10;
  undefined4 local_c;
  
  for (local_10 = 0; local_10 < height; local_10 = local_10 + 1) {
    local_c = 0;
    local_18 = *bgr24;
    for (; local_c < width * 3; local_c = local_c + 3) {
      uVar1 = *(uint *)(local_c + 3 + (int)bgr24);
      uVar2 = Octree_FindNearestColor(local_18);
      *(undefined4 *)(local_c + (int)bgr24) = uVar2;
      local_18 = uVar1;
    }
    bgr24 = (uint *)((int)bgr24 + row_padding + width * 3);
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x10005629
undefined4 QuantizeBgr24ToPaletteIndicesInPlace(uint *bgr24,int height,int width,int row_padding)

{
  uint uVar1;
  uint uVar2;
  uint local_20;
  int local_18;
  int local_14;
  uint *local_10;
  int local_8;
  
  local_10 = bgr24;
  for (local_18 = 0; local_18 < height; local_18 = local_18 + 1) {
    local_8 = 0;
    local_14 = 0;
    local_20 = *bgr24;
    for (; local_14 < width * 3; local_14 = local_14 + 3) {
      uVar1 = local_20 & 0xffffff;
      local_20 = *(uint *)(local_14 + 3 + (int)bgr24);
      uVar2 = Octree_FindNearestPaletteIndex(uVar1);
      *(char *)(local_8 + (int)local_10) = (char)uVar2;
      if (*(uint *)(&g_paletteRgbTable + (uint)*(byte *)(local_8 + (int)local_10) * 4) != uVar1) {
        *(undefined4 *)(&g_paletteRgbTable + (uint)*(byte *)(local_8 + (int)local_10) * 4) = 0;
      }
      local_8 = local_8 + 1;
    }
    bgr24 = (uint *)((int)bgr24 + row_padding + width * 3);
    local_10 = (uint *)((int)local_10 + row_padding + width);
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x1000570f
/* WARNING: Type propagation algorithm not settling */

int DitherBgr24ToPaletteColors(int dither_kernel_id,int serpentine,uint *bgr24,int height,int width,int row_padding)
{
  int iVar1;
  uint uVar2;
  int iVar3;
  short *psVar4;
  int local_88;
  uint local_84;
  int local_80;
  undefined *local_7c;
  uint local_74;
  undefined *local_70 [6];
  int local_58;
  undefined2 local_54;
  undefined2 local_52;
  undefined2 local_50;
  undefined2 local_4e;
  uint local_4c;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  uint local_38;
  int local_34;
  undefined *local_30;
  undefined4 local_2c;
  uint local_28;
  undefined2 local_24;
  undefined2 local_22;
  undefined2 local_20;
  undefined2 local_1e;
  int local_1c;
  uint local_18;
  undefined2 local_14;
  undefined2 local_12;
  undefined2 local_10;
  undefined2 local_e;
  int local_c;
  uint local_8;
  
  local_14 = 0;
  local_12 = 0;
  local_10 = 0;
  local_e = 0;
  local_54 = 0;
  local_52 = 0;
  local_50 = 0;
  local_4e = 0;
  local_24 = 0xffff;
  local_22 = 0xffff;
  local_20 = 0xffff;
  local_1e = 0;
  local_3c = 1;
  local_2c = 0;
  local_7c = &DAT_1001d2d8 + dither_kernel_id * 0xc0;
  local_18 = width * 8 + 0x50U >> 2;
  if (dither_kernel_id == 0) {
    DAT_1001e05c = dither_kernel_id;
    height = 0;
  }
  else if (dither_kernel_id == 1) {
    DAT_1001e05c = dither_kernel_id;
    QuantizeBgr24ToNearestPaletteColorInPlace(bgr24,height,width,row_padding);
    return 0;
  }
  else {
    if (DAT_100322d0 == 0) {
      for (local_4c = -0x200; (int)local_4c < 0x200; local_4c = local_4c + 1) {
        if (((int)local_4c < 0) || (0xff < (int)local_4c)) {
          if ((int)local_4c < 0) {
            PTR_DAT_1001e058[local_4c] = 0;
          }
          else {
            PTR_DAT_1001e058[local_4c] = 0xff;
          }
        }
        else {
          PTR_DAT_1001e058[local_4c] = (undefined1)local_4c;
        }
      }
      DAT_100322d0 = 1;
    }
    if (dither_kernel_id != DAT_1001e05c) {
      for (local_4c = 0; local_4c < 0x41; local_4c = local_4c + 1) {
        if (DAT_10117100[local_4c] != (void *)0x0) {
          FreeIfNotNull(DAT_10117100[local_4c]);
          DAT_10117100[local_4c] = (void *)0x0;
        }
      }
      InitErrorDiffusionDeltaTables(dither_kernel_id,(int)DAT_10117100);
      DAT_1001e05c = dither_kernel_id;
    }
    for (local_4c = 0; local_4c < 5; local_4c = local_4c + 1) {
      SetBytes((void *)(DAT_100edb10 + local_4c * 0x8060),0,0x8060);
      local_70[local_4c + 1] = (undefined *)(DAT_100edb10 + local_4c * 0x8060 + 0x28);
    }
    iVar1 = DAT_1001d260[dither_kernel_id];
    for (local_58 = 0; local_58 < height; local_58 = local_58 + 1) {
      if (local_3c < 1) {
        local_80 = width + -1;
        local_1c = -1;
        local_88 = -3;
      }
      else {
        local_80 = 0;
        local_1c = width;
        local_88 = 3;
      }
      local_c = local_80 * 3;
      for (local_4c = local_80; local_4c != local_1c; local_4c = local_4c + local_3c) {
        uVar2 = *(uint *)(local_c + (int)bgr24);
        local_28 = uVar2 & 0xffffff;
        *(uint *)(local_c + (int)bgr24) = *(uint *)(local_c + (int)bgr24) & 0xff000000;
        psVar4 = (short *)(local_4c * 8 + (int)local_70[1]);
        if (local_28 == 0) {
          local_84 = 0;
          local_74 = 0;
          local_38 = 0;
          local_8 = 0;
        }
        else if (local_28 == 0xffffff) {
          local_74 = 0xff;
          local_38 = 0xff;
          local_8 = 0xff;
          local_84 = 0xffffff;
        }
        else {
          local_8 = (uint)(byte)PTR_DAT_1001e058[(uVar2 & 0xff) + ((int)*psVar4 >> 8)];
          local_38 = (uint)(byte)PTR_DAT_1001e058[(local_28 >> 8 & 0xff) + ((int)psVar4[1] >> 8)];
          local_74 = (uint)(byte)PTR_DAT_1001e058[(local_28 >> 0x10) + ((int)psVar4[2] >> 8)];
          local_28 = local_38 << 8 | local_74 << 0x10 | local_8;
          local_84 = Octree_FindNearestColor(local_28);
        }
        *(uint *)(local_c + (int)bgr24) = *(uint *)(local_c + (int)bgr24) | local_84;
        local_40 = local_8 - (local_84 & 0xff);
        local_34 = local_38 - (local_84 >> 8 & 0xff);
        local_70[0] = local_7c + iVar1 * 0x10;
        for (local_30 = local_7c; local_30 < local_7c + iVar1 * 0x10; local_30 = local_30 + 0x10) {
          local_44 = *(int *)(local_30 + 4);
          local_48 = *(int *)(local_30 + 8);
          iVar3 = *(int *)(local_30 + 0xc);
          psVar4 = (short *)((*(int *)(local_30 + 4) + local_4c) * 8 +
                            (int)local_70[*(int *)(local_30 + 8) + 1]);
          *psVar4 = (short)*(undefined4 *)(iVar3 + local_40 * 4) + *psVar4;
          psVar4[1] = (short)*(undefined4 *)(iVar3 + local_34 * 4) + psVar4[1];
          psVar4[2] = (short)*(undefined4 *)(iVar3 + (local_74 - (local_84 >> 0x10)) * 4) +
                      psVar4[2];
        }
        local_c = local_c + local_88;
      }
      RotateDwordsLeft1(local_70 + 1,DAT_1001d288[dither_kernel_id]);
      memset(local_70[DAT_1001d288[dither_kernel_id]] + -0x28,0,local_18 << 2);
      if (serpentine != 0) {
        local_3c = -local_3c;
        local_7c = &DAT_1001d2d8 + (uint)(local_3c == -1) * 0x6c0 + dither_kernel_id * 0xc0;
      }
      bgr24 = (uint *)((int)bgr24 + width * 3 + row_padding);
    }
  }
  return height;
}

// FUNCTION: CARDARTLIB 0x10005cb4
void RotateDwordsLeft1(undefined4 *param_1,uint param_2)
{
  undefined4 uVar1;  
  uVar1 = *param_1;

  //TODO: how to get this to compile to "lea eax, [eax*4 - 4]"" ??
  memcpy(param_1,param_1 + 1,param_2 * 4 - 4);
  param_1[param_2 -1] = uVar1;
}

// FUNCTION: CARDARTLIB 0x10005cf5
undefined4 InitErrorDiffusionDeltaTables(int dither_kernel_id,int* delta_table_ptrs_base)
{
  struct {
    int local_14;
    int local_10;
    int iVar2;
    int iVar1;
  } s;
  
  s.iVar1 = DAT_1001d2b0[dither_kernel_id] >> 1;
  for (s.local_10 = 0; s.local_10 < DAT_1001d260[dither_kernel_id]; s.local_10 = s.local_10 + 1) {
    s.iVar2 = ((int *)((char *)DAT_1001d2d8 + dither_kernel_id * 0xc0))[s.local_10 * 4];
    if (delta_table_ptrs_base[s.iVar2] != 0) {
      ((int *)(DAT_1001d2e4 + dither_kernel_id * 0xc0))[s.local_10 * 4] =
            delta_table_ptrs_base[s.iVar2] + 0x400;
      continue;
    }
    
    delta_table_ptrs_base[s.iVar2] = malloc(0x800);
    assert((uint)(delta_table_ptrs_base[s.iVar2] != 0),s_D__Newmagic_sources_NedCard_Pale_1001e0b4,
            0x4fd,s_Not_enough_memory_for_delta_arra_1001e090);
    for (s.local_14 = -0x100; s.local_14 < 0x100; s.local_14 = s.local_14 + 1) {
      *(int *)( delta_table_ptrs_base[s.iVar2] + 0x400 + s.local_14 * 4) =
            ((s.iVar2 * s.local_14 + s.iVar1) * 0x100) / DAT_1001d2b0[dither_kernel_id];
    }
    ((int *)(DAT_1001d2e4 + dither_kernel_id * 0xc0))[s.local_10 * 4] = delta_table_ptrs_base[s.iVar2] + 0x3fc;
  }
  
  for (s.local_10 = 0; s.local_10 < DAT_1001d260[dither_kernel_id]; s.local_10 = s.local_10 + 1) {
    s.iVar2 = ((int *)((char *)DAT_1001d998 + dither_kernel_id * 0xc0))[s.local_10 * 4];
    
    ((int *)(DAT_1001d9a4 + dither_kernel_id * 0xc0))[s.local_10 * 4] = delta_table_ptrs_base[s.iVar2] + 0x3fc;
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x100062c7
undefined4 DitherBgr24ToRgbQuantizedF8(int dither_kernel_id,int serpentine,uint *bgr24,int height,int width,int row_padding)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  short *psVar6;
  int local_88;
  uint local_84;
  int local_80;
  undefined *local_7c;
  int local_70 [6];
  int local_58;
  undefined2 local_54;
  undefined2 local_52;
  undefined2 local_50;
  undefined2 local_4e;
  uint local_4c;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  uint local_38;
  int local_34;
  undefined *local_30;
  undefined4 local_2c;
  uint local_28;
  undefined2 local_24;
  undefined2 local_22;
  undefined2 local_20;
  undefined2 local_1e;
  int local_1c;
  uint local_18;
  undefined2 local_14;
  undefined2 local_12;
  undefined2 local_10;
  undefined2 local_e;
  int local_c;
  uint local_8;
  
  local_14 = 0;
  local_12 = 0;
  local_10 = 0;
  local_e = 0;
  local_54 = 0;
  local_52 = 0;
  local_50 = 0;
  local_4e = 0;
  local_24 = 0xffff;
  local_22 = 0xffff;
  local_20 = 0xffff;
  local_1e = 0;
  local_3c = 1;
  local_2c = 0;
  local_7c = &DAT_1001d2d8 + dither_kernel_id * 0xc0;
  local_18 = width * 8 + 0x50U >> 2;
  if (DAT_100322cc == 0) {
    for (local_4c = -0x200; (int)local_4c < 0x200; local_4c = local_4c + 1) {
      if (((int)local_4c < 0) || (0xff < (int)local_4c)) {
        if ((int)local_4c < 0) {
          PTR_DAT_1001e058[local_4c] = 0;
        }
        else {
          PTR_DAT_1001e058[local_4c] = 0xff;
        }
      }
      else {
        PTR_DAT_1001e058[local_4c] = (undefined1)local_4c;
      }
    }
    DAT_100322cc = 1;
  }
  if (DAT_1001e060 != dither_kernel_id) {
    for (local_4c = 0; local_4c < 0x41; local_4c = local_4c + 1) {
      if (*(int *)(&DAT_10117100 + local_4c * 4) != 0) {
        FreeIfNotNull(*(void **)(&DAT_10117100 + local_4c * 4));
        *(undefined4 *)(&DAT_10117100 + local_4c * 4) = 0;
      }
    }
    InitErrorDiffusionDeltaTables(dither_kernel_id,DAT_10117100);
    DAT_1001e060 = dither_kernel_id;
  }
  for (local_4c = 0; local_4c < 5; local_4c = local_4c + 1) {
    SetBytes((void *)(DAT_100edb10 + local_4c * 0x8060),0,0x8060);
    local_70[local_4c + 1] = local_4c * 0x8060 + 0x100edb38;
  }
  iVar2 = *(int *)(&DAT_1001d260 + dither_kernel_id * 4);
  for (local_58 = 0; local_58 < height; local_58 = local_58 + 1) {
    if (local_3c < 1) {
      local_80 = width + -1;
      local_1c = -1;
      local_88 = -3;
    }
    else {
      local_80 = 0;
      local_1c = width;
      local_88 = 3;
    }
    local_c = local_80 * 3;
    for (local_4c = local_80; local_4c != local_1c; local_4c = local_4c + local_3c) {
      uVar3 = *(uint *)(local_c + (int)bgr24);
      uVar5 = uVar3 & 0xffffff;
      *(uint *)(local_c + (int)bgr24) = *(uint *)(local_c + (int)bgr24) & 0xff000000;
      psVar6 = (short *)(local_4c * 8 + local_70[1]);
      local_8 = (uint)(byte)PTR_DAT_1001e058[(uVar3 & 0xff) + ((int)*psVar6 >> 8)];
      local_38 = (uint)(byte)PTR_DAT_1001e058[(uVar5 >> 8 & 0xff) + ((int)psVar6[1] >> 8)];
      bVar1 = PTR_DAT_1001e058[(uVar5 >> 0x10) + ((int)psVar6[2] >> 8)];
      local_28 = (uint)(byte)PTR_DAT_1001e058[(uVar5 >> 8 & 0xff) + ((int)psVar6[1] >> 8)] << 8 |
                 (uint)bVar1 << 0x10 |
                 (uint)(byte)PTR_DAT_1001e058[(uVar3 & 0xff) + ((int)*psVar6 >> 8)];
      if (local_28 == 0) {
        local_84 = 0;
      }
      else if (local_28 == 0xffffff) {
        local_84 = 0xffffff;
      }
      else {
        local_84 = Rgb888_QuantizeToF8(local_28);
      }
      *(uint *)(local_c + (int)bgr24) = *(uint *)(local_c + (int)bgr24) | local_84;
      local_40 = local_8 - (local_84 & 0xff);
      local_34 = local_38 - (local_84 >> 8 & 0xff);
      local_30 = local_7c;
      for (local_70[0] = 0; local_70[0] < iVar2; local_70[0] = local_70[0] + 1) {
        local_44 = *(int *)(local_30 + 4);
        local_48 = *(int *)(local_30 + 8);
        iVar4 = *(int *)(local_30 + 0xc);
        psVar6 = (short *)(local_70[*(int *)(local_30 + 8) + 1] +
                          (*(int *)(local_30 + 4) + local_4c) * 8);
        *psVar6 = (short)*(undefined4 *)(iVar4 + local_40 * 4) + *psVar6;
        psVar6[1] = (short)*(undefined4 *)(iVar4 + local_34 * 4) + psVar6[1];
        psVar6[2] = (short)*(undefined4 *)(iVar4 + ((uint)bVar1 - (local_84 >> 0x10 & 0xff)) * 4) +
                    psVar6[2];
        local_30 = local_30 + 0x10;
      }
      local_c = local_c + local_88;
    }
    RotateDwordsLeft1(local_70 + 1,*(int *)(&DAT_1001d288 + dither_kernel_id * 4));
    memset((void *)(local_70[*(int *)(&DAT_1001d288 + dither_kernel_id * 4)] + -0x28),0,local_18 << 2);
    if (serpentine != 0) {
      local_3c = -local_3c;
      local_7c = &DAT_1001d2d8 + (uint)(local_3c == -1) * 0x6c0 + dither_kernel_id * 0xc0;
    }
    bgr24 = (uint *)((int)bgr24 + width * 3 + row_padding);
  }
  return 1;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10006809
void DestroyPaletteOctree(void)
{
  Octree_Destroy(g_paletteOctreeRoot);
  g_paletteOctreeRoot = (int *)0x0;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000682c
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

// MATCHING
// FUNCTION: CARDARTLIB 0x100068d0
uint Rgb888_QuantizeToF8(uint param_1)
{
  return param_1 & 0xf8f8f8;
}

// FUNCTION: CARDARTLIB 0x100068f0
int * Catalog_LoadWvlEntry(int param_1,uint *param_2,int param_3)
{
  size_t sVar1;
  undefined8 *puVar2;
  uint local_41c [66];
  uint local_314 [65];
  uint local_210 [64];
  uint local_110 [64];
  size_t local_10;
  int *local_c;
  int local_8;
  
  local_c = (int *)&DAT_10032ae8;
  EnterCriticalSection(&DAT_101221d0);
  _splitpath((char *)param_2,(char *)0x0,(char *)local_314,(char *)local_210,(char *)local_110);
  if (DAT_10032adc == 0) {
    strcpy((char *)local_41c,(char *)local_314);
    strcat((char *)local_41c,s_SmallArt_cat_1001e12c);
    DAT_1001e11c = Catalog_Open((char *)local_41c);
    strcpy((char *)local_41c,(char *)local_314);
    strcat((char *)local_41c,s_MedArt_cat_1001e13c);
    DAT_1001e120 = Catalog_Open((char *)local_41c);
    DAT_10032adc = 1;
  }
  sVar1 = strlen((char *)local_314);
  local_8 = (int)local_314 + sVar1;
  if (param_1 == 0) {
    DAT_100ea098 = DAT_1001e11c;
  }
  else {
    if (param_1 != 1) {
      return (int *)0x0;
    }
    DAT_100ea098 = DAT_1001e120;
  }
  strcpy((char *)local_314,(char *)local_210);
  strcat((char *)local_314,(char *)local_110);
  _strlwr((char *)local_314);
  if (local_c != (int *)0x0) {
    memset(local_c,0,0x1b0);
    strcpy((char *)(local_c + 0x27),(const char *)param_2);
    local_c[0x68] = (int)&DAT_100ad498;
    local_10 = Catalog_ReadEntry(DAT_100ea098,(char *)local_314,(void **)(local_c + 0x68));
    if (local_10 == 0xffffffff) {
      strcat((char *)param_2,s__lf_1001e148);
      OutputDebugStringA((LPCSTR)param_2);
      LeaveCriticalSection(&DAT_101221d0);
      local_c = (int *)0x0;
    }
    else {
      local_c[0x69] = local_10 - 0x9c;
      memcpy(local_c,(void *)local_c[0x68],0x9c);
      local_c[0x68] = local_c[0x68] + 0x9c;
      if (local_c[10] == 4) {
        local_c[7] = local_c[7] << 1;
        local_c[8] = local_c[8] << 1;
      }
      if (param_3 != 0) {
        puVar2 = Wvl_DecodeHaar(local_c,(undefined8 *)0x0);
        local_c[0x6b] = (int)puVar2;
        if (local_c[0x6b] == 0) {
          Catalog_Unlock(local_c);
          LeaveCriticalSection(&DAT_101221d0);
          local_c = (int *)0x0;
        }
        else {
          local_c[0x6a] = 1;
          LeaveCriticalSection(&DAT_101221d0);
        }
      }
    }
  }
  return local_c;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10006bb5
BOOL Catalog_Unlock(int unused)
{
  LeaveCriticalSection(&DAT_101221d0);

  //Wonder what this did originally
  if (unused == 0)
    return 0;  
  return 0;
}

// FUNCTION: CARDARTLIB 0x10006be3
undefined8 * Wvl_DecodeHaar(int *param_1,undefined8 *param_2)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int *piVar6;
  int *piVar7;
  bool bVar8;
  undefined8 *local_50;
  int local_4c;
  int local_48;
  int local_34;
  int local_2c;
  int local_8;
  
  if (g_waveletScaleToByteTableInit == 0) {
    for (local_4c = -0x400; local_4c < 0x401; local_4c = local_4c + 1) {
      if ((local_4c < 0) || (0xf8 < local_4c)) {
        if (local_4c < 10) {
          g_waveletScaleToByteTable[local_4c] = 0;
        }
        else {
          g_waveletScaleToByteTable[local_4c] = 0xff;
        }
      }
      else {
        g_waveletScaleToByteTable[local_4c] = (char)((local_4c * 0xff) / 0xf8);
      }
    }
    g_waveletScaleToByteTableInit = 1;
  }
  bVar8 = param_2 != (undefined8 *)0x0;
  if (bVar8) {
    MemZeroDwords(param_2,(int)(param_1[0x24] + 2000 + (param_1[0x24] + 2000 >> 0x1f & 3U)) >> 2);
  }
  else {
    param_2 = malloc(param_1[0x24] + 2000);
    MemZeroDwords(param_2,(int)(param_1[0x24] + 2000 + (param_1[0x24] + 2000 >> 0x1f & 3U)) >> 2);
  }
  _DAT_100ecb04 = Wvl_UnpackPieces((int)param_2,param_1);
  if (param_1[10] == 1) {
    local_8 = 1;
  }
  else if (param_1[10] == 4) {
    local_8 = 2;
  }
  else if (param_1[10] == 0x10) {
    local_8 = 4;
  }
  else {
    assert(0,s_D__Newmagic_sources_NedCard_haar_1001e174,0x15e,
           s_wavelet_pieces_has_illegal_value_1001e14c,param_1[10]);
  }
  iVar1 = param_1[7];
  iVar3 = param_1[7] / local_8;
  iVar2 = param_1[9];
  iVar4 = param_1[8] / local_8;
  for (local_48 = 0; local_48 < param_1[10]; local_48 = local_48 + 1) {
    local_34 = iVar4;
    local_2c = iVar3;
    if (*param_1 != 0) {
      local_34 = (iVar4 / local_8) / (int)((param_1[10] == 1) + 1);
      local_2c = (iVar3 / local_8) / (int)((param_1[10] == 1) + 1);
    }
    piVar5 = (int *)((iVar3 * iVar3 + local_2c * local_2c * 2 + 0x40) * local_48 * 4 + (int)param_2)
    ;
    piVar6 = piVar5 + iVar3 * iVar3 + 0x20;
    piVar7 = piVar6 + local_2c * local_2c + 0x20;
    Haar2D_ReconstructInPlace(piVar5,iVar3,iVar2);
    Haar2D_ReconstructInPlace(piVar6,local_2c,iVar2);
    Haar2D_ReconstructInPlace(piVar7,local_2c,iVar2);
    if (local_48 < param_1[10] / 2) {
      local_50 = (undefined8 *)
    YuvPlanesToBgr24(DAT_10032c98,piVar5,iVar3,iVar3,(int)piVar6,(int)piVar7,local_2c,local_2c,*param_1);
    }
    else if (param_1[10] < 2) {
      local_50 = (undefined8 *)
    YuvPlanesToBgr24(DAT_10032c98,piVar5,iVar3,iVar4,(int)piVar6,(int)piVar7,local_2c,local_34,*param_1);
    }
    else {
      local_50 = (undefined8 *)
    YuvPlanesToBgr24(DAT_10032c98,piVar5,iVar3,param_1[8] - iVar3,(int)piVar6,(int)piVar7,local_2c,local_34,
                     *param_1);
    }
    if (param_1[10] < 2) {
      if (!bVar8) {
        FreeIfNotNull(param_2);
      }
      param_2 = local_50;
    }
    else {
      CopyBgr24RectIntoStridedBuffer(param_2,local_50,(local_48 % local_8) * (iVar1 / local_8),
                                     (local_48 / local_8) * (iVar1 / local_8),iVar3,iVar3,iVar1);
      FreeIfNotNull(local_50);
    }
  }
  return param_2;
}

// FUNCTION: CARDARTLIB 0x10007035
void CopyBgr24RectIntoStridedBuffer(undefined8 *dst_bgr24,undefined8 *src_bgr24,int dst_x,int dst_y,int rect_width,
                               int rect_height,int dst_stride_pixels)

{
  undefined4 local_8;
  
  dst_bgr24 = (undefined8 *)((int)dst_bgr24 + (dst_stride_pixels * dst_y + dst_x) * 3);
  for (local_8 = 0; local_8 < rect_height; local_8 = local_8 + 1) {
    CopyBytes(dst_bgr24,src_bgr24,rect_width * 3);
    dst_bgr24 = (undefined8 *)((int)dst_bgr24 + dst_stride_pixels * 3);
    src_bgr24 = (undefined8 *)((int)src_bgr24 + rect_width * 3);
  }
  return;
}

// FUNCTION: CARDARTLIB 0x10007238
void CopyBytes(void *dst,const void *src,size_t size)
{
  uint uVar1;
  undefined8 *dst_qword;
  const undefined8 *src_qword;
  unsigned char *dst_byte;
  const unsigned char *src_byte;
  
  dst_qword = (undefined8 *)dst;
  src_qword = (const undefined8 *)src;
  for (uVar1 = (uint)size >> 3; uVar1 != 0; uVar1 = uVar1 - 1) {
    *dst_qword = *src_qword;
    src_qword = src_qword + 1;
    dst_qword = dst_qword + 1;
  }
  dst_byte = (unsigned char *)dst_qword;
  src_byte = (const unsigned char *)src_qword;
  uVar1 = (uint)size & 7;
  if (uVar1 != 0) {
    for (; uVar1 != 0; uVar1 = uVar1 - 1) {
      *dst_byte = *src_byte;
      src_byte = src_byte + 1;
      dst_byte = dst_byte + 1;
    }
  }
  return;
}

// FUNCTION: CARDARTLIB 0x10007273
void SetBytes(void *dst,uint value,size_t size)

{
  //TODO: memset((void *)param_1,(int)(param_2 & 0xff),param_3);
  undefined8 uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  longlong lVar5;
  undefined8 *dst_qword;
  unsigned char *dst_byte;
  
  dst_qword = (undefined8 *)dst;
  uVar2 = value << 8 | value;
  uVar4 = (int)uVar2 >> 0x1f | ((int)uVar2 >> 0x1f) << 0x10 | uVar2 >> 0x10;
  lVar5 = _allshl(0x20,uVar4);
  uVar1 = CONCAT44(uVar4 | (uint)((ulonglong)lVar5 >> 0x20),uVar2 | uVar2 << 0x10 | (uint)lVar5);
  iVar3 = ((uint)size >> 3) - 1;
  do {
    *dst_qword = uVar1;
    dst_qword = dst_qword + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  *dst_qword = uVar1;
  dst_byte = (unsigned char *)dst_qword;
  for (uVar4 = (uint)size & 7; uVar4 != 0; uVar4 = uVar4 - 1) {
    *dst_byte = (unsigned char)value;
    dst_byte = dst_byte + 1;
  }
  return;
}

// FUNCTION: CARDARTLIB 0x100072dc
void Haar2D_ReconstructInPlace(int *coeffs,int full_size,int base_size)
{
  struct {
    int *idk;
    int *piVar1; // ebp - 0x18
    int *haarScratchA; // ebp - 0x14
    int *haarScratchB; // ebp - 0x10
    int i; // ebp - 0xc
    int pad2; // ebp - 8
    int *pad3; // ebp - 4
  } s;
  
  if (g_haarScratchInit == 0) {
    g_haarScratchA = malloc(0x32000);
    s.haarScratchA = g_haarScratchA;
    g_haarScratchB = malloc(0x32000);

    s.haarScratchB = g_haarScratchB;
    g_haarScratchInit = 1;
  }
  else {
    s.haarScratchA = g_haarScratchA;
    s.haarScratchB = g_haarScratchB;
  }


  for (s.i = base_size; s.i < full_size; s.i = s.i << 1) {
    s.pad2 = coeffs;
    s.idk = &coeffs[s.i*s.i];

    Haar_CombineSumDiff(s.pad2,s.idk,s.haarScratchA,s.i,s.i,s.i * 2,s.i);

    s.pad3 = &coeffs[s.i*s.i*2];
    s.piVar1 = &coeffs[s.i*s.i*3];

    Haar_CombineSumDiff(s.pad3,s.piVar1,s.haarScratchB, s.i,
                        s.i,s.i * 2,s.i);
                        
    Haar_CombineSumDiffHalf(s.haarScratchA,s.haarScratchB,coeffs,s.i,s.i * 2,s.i * 2,s.i * 2);
  }
}

// FUNCTION: CARDARTLIB 0x10007419
void Haar_CombineSumDiff(int *src_a,int *src_b,int *dst,int width,int rows,undefined4 src_stride_unused,int dst_stride)
{
  struct {
    int *src_a_end; // ebp - 0x10
    int *src_a_row0; // ebp - 0xc
    int *dst_row; // ebp - 8
    int row; // ebp - 4
  } s;
  
  s.row = dst_stride;

  for (s.row = 0; s.row < rows; s.row++, dst++, src_b++) {
    s.dst_row = dst;
    s.src_a_row0 = src_a;
    s.src_a_end = (width - 1) + src_a + 1;
    s.dst_row = s.dst_row + dst_stride * 2;
    src_a++;

    for (; s.src_a_end > src_a; src_a++, src_b++, s.dst_row += dst_stride * 2) {
      *s.dst_row = *src_b + *src_a;

      //s.dst_row[dst_stride] = *src_a - *src_b;
      *(int *)((char *)s.dst_row + dst_stride * 4) = *src_a - *src_b;
    }
    
    *dst = *src_b + *s.src_a_row0;
    dst[dst_stride] = *s.src_a_row0 - *src_b;
  }
}

// FUNCTION: CARDARTLIB 0x100074ee
void Haar_CombineSumDiffHalf(int *src_a,int *src_b,int *dst,int width,int rows,undefined4 src_stride_unused,int dst_stride)
{
  struct {
    int *src_a_end; // ebp - 0x10
    int *src_a_row0; // ebp - 0xc
    int *dst_row; // ebp - 8
    int row; // ebp - 4
  } s;
  
  for (s.row = 0; s.row < rows; s.row++, dst++, src_b++) {
    s.dst_row = dst;
    s.src_a_row0 = src_a;
    s.src_a_end = (width - 1) + src_a + 1;
    s.dst_row = s.dst_row + dst_stride * 2;
    src_a++;

    for (; s.src_a_end > src_a; src_a++, src_b++, s.dst_row += dst_stride * 2) {
      *s.dst_row = (*src_b + *src_a) >> 1;
      *(int *)((char *)s.dst_row + dst_stride * 4) = (*src_a - *src_b) >> 1;
      //s.dst_row[dst_stride] = (*src_a - *src_b) >> 1;
    }
    
    *dst = (*src_b + *s.src_a_row0) >> 1;
    //*(int *)((char *)dst + dst_stride * 4) = ((0 - *src_b) + *s.src_a_row0) >> 1;
    dst[dst_stride] = (*s.src_a_row0 - *src_b) >> 1;
  }
}

// FUNCTION: CARDARTLIB 0x100075ef
undefined1 * YuvPlanesToBgr24(undefined1 *out_bgr24,int *luma,int width,int height,int chroma_u,int chroma_v,
                 int chroma_stride,undefined4 unused_chroma_height,int chroma_is_420)

{
  struct {
    int y; /* ebp - 0x2c */
    int u; /* ebp - 0x28 */
    int v; /* ebp - 0x24 */
    undefined1 *out_base; /* ebp - 0x20 */
    int row; /* ebp - 0x1c */
    int col; /* ebp - 0x18 */
    int green; /* ebp - 0x14 */
    int *chroma_u_ptr; /* ebp - 0x10 */
    int *chroma_v_ptr; /* ebp - 0xc */
    int red; /* ebp - 8 */
    int blue; /* ebp - 4 */
  } s;
  
  if (g_yuvClampTableInit == 0) {
    for (s.col = -0x400; s.col < 0x1c00; s.col = s.col + 1) {
      if (0 < s.col) {
        g_yuvClampTable[s.col] = (char)(((s.col >> 2) <= 0xff) ? (s.col >> 2) : 0xff);
      } else {
        g_yuvClampTable[s.col] = 0;
      } 
    }
    g_yuvClampTableInit = 1;
  }
  if (out_bgr24 == (undefined1 *)0x0) {
    out_bgr24 = malloc(width * width * 3 + 0x10);
  }
  s.out_base = out_bgr24;
  for (s.row = 0; s.row < height; s.row = s.row + 1) {
    s.y = s.row;
    if (chroma_is_420 != 0) {
      s.y = s.y / 2;
    }

    s.y = (s.y * chroma_stride) << 2;
    s.chroma_u_ptr = (int *)(chroma_u + s.y);
    s.chroma_v_ptr = (int *)(chroma_v + s.y);

    for (s.col = 0; s.col < width; s.col = s.col + 1, luma = luma + 1, out_bgr24 = out_bgr24 + 3) {
      s.y = *luma;

      if (chroma_is_420 != 0) {
        if ((s.col & 1) == 0) {
          s.u = *s.chroma_u_ptr;
          s.v = *s.chroma_v_ptr;
        } else {
          s.u = (s.chroma_u_ptr[((uint)(width - 1) - (uint)s.col) != 0] + *s.chroma_u_ptr) / 2;
          s.v = (s.chroma_v_ptr[((uint)(width - 1) - (uint)s.col) != 0] + *s.chroma_v_ptr) / 2;
        }
        s.red = ((s.v >> 3) + (s.v >> 1) + s.v) - 0x333 + s.y;
        s.blue = (s.u * 2) - 0x400 + s.y;
        s.green = (((s.y * 2) - (s.y >> 2)) - (s.red >> 1)) - ((s.blue >> 2) - (s.blue >> 4));

        out_bgr24[0] = g_yuvClampTable[s.blue];
        out_bgr24[1] = g_yuvClampTable[s.green];
        out_bgr24[2] = g_yuvClampTable[s.red];

        if ((s.col & 1) != 0) {
          s.chroma_u_ptr = s.chroma_u_ptr + 1;
          s.chroma_v_ptr = s.chroma_v_ptr + 1;
        }
      } else {
        s.u = *s.chroma_u_ptr;
        s.v = *s.chroma_v_ptr;

        s.red = ((s.v >> 3) + (s.v >> 1) + s.v) - 0x333 + s.y;
        s.blue = (s.u * 2) - 0x400 + s.y;
        s.green = (((s.y * 2) - (s.y >> 2)) - (s.red >> 1)) - ((s.blue >> 2) - (s.blue >> 4));

        out_bgr24[0] = g_yuvClampTable[s.blue];
        out_bgr24[1] = g_yuvClampTable[s.green];
        out_bgr24[2] = g_yuvClampTable[s.red];

        s.chroma_u_ptr = s.chroma_u_ptr + 1;
        s.chroma_v_ptr = s.chroma_v_ptr + 1;
      }
    }
  }
  return s.out_base;
}

// FUNCTION: CARDARTLIB 0x100078b8
undefined4 Wvl_UnpackPieces(int param_1,int *param_2)
{
  struct {
    byte *dst_y;        /* ebp - 0x3c */
    int *wvl;           /* ebp - 0x38 */
    int node_count;     /* ebp - 0x34 */
    int chroma_w;       /* ebp - 0x30 */
    int base_size;      /* ebp - 0x2c */
    int chroma_h;       /* ebp - 0x28 */
    int layer;          /* ebp - 0x24 */
    uint *symbol_table; /* ebp - 0x20 */
    int full_w;         /* ebp - 0x1c */
    byte *dst_v;        /* ebp - 0x18 */
    byte *bitstream;    /* ebp - 0x14 */
    byte *huff_data;    /* ebp - 0x10 */
    int full_h;         /* ebp - 0x0c */
    int tmp;            /* ebp - 0x08 */
    byte *dst_u;        /* ebp - 0x04 */
  } s;

  s.wvl = param_2;

  s.full_w = s.wvl[7] / (2 - (s.wvl[10] < 2));
  s.full_h = s.full_w;
  s.chroma_w = s.full_w / (2 - (s.wvl[0] < 1));
  s.chroma_h = s.chroma_w;
  s.base_size = s.wvl[9];

  s.huff_data = (byte *)s.wvl[0x68];
  s.bitstream = s.huff_data;
  s.node_count = *(int *)s.bitstream;
  s.bitstream = s.bitstream + 4;
  s.symbol_table = (uint *)s.bitstream;
  *s.symbol_table = 0x80000000;
  s.bitstream = s.bitstream + (s.node_count << 2);
  s.bitstream =
      s.bitstream +
      Huffman13_Init((undefined4)s.bitstream,(undefined4)s.symbol_table,(undefined4)s.node_count);

  for (s.layer = 0; s.layer < s.wvl[10]; s.layer = s.layer + 1) {
    s.tmp = s.chroma_w * s.chroma_h;
    s.tmp = (s.full_w * s.full_h) + s.tmp * 2;
    s.tmp = s.tmp + 0x40;
    s.dst_y = (byte *)(param_1 + ((s.tmp * s.layer) << 2));

    s.dst_u += (s.full_w * s.full_h) << 2 + 0x80;

    s.dst_v += (s.chroma_w * s.chroma_h) << 2 + 0x80;

    memcpy(s.dst_y,s.bitstream,(s.base_size * s.base_size) << 2);

    s.dst_y += (s.base_size * s.base_size) << 2;

    s.bitstream += (s.base_size * s.base_size) << 2;

    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)s.dst_y,(uint *)s.bitstream,s.wvl[s.layer + 0x17]);
    s.bitstream = s.bitstream + s.wvl[s.layer + 0x17];

    memcpy(s.dst_u,s.bitstream,(s.base_size * s.base_size) << 2);

    s.dst_u += (s.base_size * s.base_size) << 2;

    s.bitstream += (s.base_size * s.base_size) << 2;

    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)s.dst_u,(uint *)s.bitstream,s.wvl[s.layer + 0x1b]);
    s.bitstream = s.bitstream + s.wvl[s.layer + 0x1b];

    memcpy(s.dst_v,s.bitstream,(s.base_size * s.base_size) << 2);

    s.dst_v += (s.base_size * s.base_size) << 2;

    s.bitstream += (s.base_size * s.base_size) << 2;

    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)s.dst_v,(uint *)s.bitstream,s.wvl[s.layer + 0x1f]);
    s.bitstream = s.bitstream + s.wvl[s.layer + 0x1f];
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x1000807f
uint * Wvl_DecodeToBgr24(byte *param_1,int *wvl_entry,int width,int height)
{
  struct WvlDecodeToBgr24Stack {
    int unk_5060;
    int unk_505c;
    int y_step;
    int x_step;
    uint *out_base;
    uint x_map[4096];
    byte *decoded; // ebp - 0x104c
    int y; // ebp - 0x1048
    int y_acc; // ebp - 0x1044
    int x; // ebp - 0x1040
    int out_nonnull; // ebp - 0x103c
    int src_width; // ebp - 0x1038
    uint *y_map_ptr; // ebp - 0x1034
    int x_acc; // ebp - 0x1030
    uint *x_map_ptr; // ebp - 0x102c
    byte *out_start; // ebp - 0x1028
    byte *out_ptr; // ebp - 0x1024
    int y_scale; // ebp - 0x1020
    int row_pad; // ebp - 0x101c
    int row_bytes; // ebp - 0x1018
    int x_scale; // ebp - 0x1014
    int zero_1010; // ebp - 0x1010
    byte *src_ptr; // ebp - 0x100c
    uint y_map[1016]; // ebp - 0x1008
    int tmp_28; // ebp - 0x28
    int tmp_24; // ebp - 0x24
    int tmp_20; // ebp - 0x20
    int tmp_1c; // ebp - 0x1c
    int tmp_18; // ebp - 0x18
    int tmp_14; // ebp - 0x14
    int tmp_10; // ebp - 0x10
    int tmp_c; // ebp - 0xc
    int tmp_8; // ebp - 0x8
    int tmp_4; // ebp - 0x4
  } s;

  s.x_acc = 0;
  s.y_acc = 0;
  s.out_base = param_1;
  s.out_start = (byte *)0;
  s.x_map_ptr = s.x_map;
  s.y_map_ptr = s.y_map;
  if (param_1 != (uint *)0) {
    s.out_nonnull = 1;
  } else {
    s.out_nonnull = 0;
  }

  if (wvl_entry == (int *)0) {
    return (uint *)0;
  }

  s.x_scale = wvl_entry[7];
  s.y_scale = wvl_entry[8];
  s.x_scale <<= 0x10;
  s.y_scale <<= 0x10;
  s.x_step = s.x_scale / width;
  s.y_step = s.y_scale / height;

  if (wvl_entry[0x6a] != 0) {
    s.decoded = (byte *)wvl_entry[0x6b];
  } else {
    s.decoded = (byte *)Wvl_DecodeHaar(wvl_entry,(undefined8 *)&DAT_10032c98);
  }

  s.src_width = wvl_entry[7];
  s.tmp_8 = 3;
  s.zero_1010 = 0;
  s.row_pad = (DAT_1001e0e0 - (width * 3) % DAT_1001e0e0) % DAT_1001e0e0;

  if (param_1 == (uint *)0) {
    param_1 = (uint *)&DAT_100ad498;
    s.out_base = param_1;
  } else {
    s.out_base = param_1;
  }
  
  for (s.x = 0, s.x_acc = 0; s.x < width; s.x += 1, s.x_acc += s.x_step, s.x_map_ptr += 1) {
    if (s.tmp_8 == 3) {
      *s.x_map_ptr = (uint)(s.x_acc >> 8);
    } else {
      assert(0,s_D__Newmagic_sources_NedCard_haar_1001e1b8,0x6f7,
             s_Only_Works_on_24_bit_images_1001e198);
    }    
  }

  //param_1 = s.out_base;
  if (wvl_entry[8] < height) {
    param_1 = (uint *)((int)s.out_base + (height - wvl_entry[8]) * (width * 3 + s.row_pad));
  }
  else
    s.out_ptr = (byte *)param_1;
  
  for (s.y = 0; s.y < wvl_entry[8]; s.y += 1, param_1 += s.row_pad) {
    s.tmp_4 = (int)s.decoded + (s.src_width * s.tmp_8 + s.zero_1010) * s.y;
    s.x_map_ptr = s.x_map;

    for (s.x = 0; s.x < width; s.x += 1, s.x_map_ptr += 1) {
      s.src_ptr = (byte *)(s.tmp_4 + (((int)*s.x_map_ptr >> 8) * 3));

      s.tmp_10 = (int)((uint)s.src_ptr[3] - (uint)s.src_ptr[0]);
      s.tmp_10 *= ((int)*s.x_map_ptr & 0xff);
      s.tmp_10 >>= 8;
      s.tmp_10 += (int)(uint)s.src_ptr[0];
      *(byte *)param_1 = (byte)s.tmp_10;

      s.tmp_10 = (int)((uint)s.src_ptr[4] - (uint)s.src_ptr[1]);
      s.tmp_10 *= ((int)*s.x_map_ptr & 0xff);
      s.tmp_10 >>= 8;
      s.tmp_10 += (int)(uint)s.src_ptr[1];
      *(byte *)((int)param_1 + 1) = (byte)s.tmp_10;

      s.tmp_10 = (int)((uint)s.src_ptr[5] - (uint)s.src_ptr[2]);
      s.tmp_10 *= ((int)*s.x_map_ptr & 0xff);
      s.tmp_10 >>= 8;
      s.tmp_10 += (int)(uint)s.src_ptr[2];
      *(byte *)((int)param_1 + 2) = (byte)s.tmp_10;

      param_1 = (uint *)((int)param_1 + 3);
    }
  }

  s.y_acc = 0;
  s.y_map_ptr = s.y_map;
  s.y = 0;
  while (s.y < height) {
    if (s.tmp_8 != 3) {
      assert(0,s_D__Newmagic_sources_NedCard_haar_1001e1fc,0x730,
             s_Only_Works_on_24_bit_images_1001e1dc);
      continue;
    }
    *s.y_map_ptr = (uint)(s.y_acc >> 8);
    s.y_acc += s.y_step;
    s.y_map_ptr += 1;
    s.y += 1;
  }

  s.row_bytes = width * 3 + s.row_pad;
  if (height < wvl_entry[8]) {
    memcpy(s.x_map,(void *)((wvl_entry[8] - 1) * s.row_bytes + (int)s.out_base),s.row_bytes);
  }

  s.x = 0;
  while (s.x < width) {
    param_1 = (uint *)((int)s.out_base + s.x * 3);
    s.y_map_ptr = s.y_map;
    s.y = 0;
    while (s.y < height - 1) {
      s.tmp_10 = wvl_entry[8] - 2;
      if ((int)*s.y_map_ptr >> 8 <= wvl_entry[8] - 2) {
        s.tmp_10 = (int)*s.y_map_ptr >> 8;
      }

      s.src_ptr = s.out_ptr + s.tmp_10 * s.row_bytes + s.x * 3;

      s.tmp_10 = (int)((uint)s.src_ptr[s.row_bytes] - (uint)s.src_ptr[0]);
      s.tmp_10 *= ((int)*s.y_map_ptr & 0xff);
      s.tmp_10 >>= 8;
      s.tmp_10 += (int)(uint)s.src_ptr[0];
      *(byte *)param_1 = (byte)s.tmp_10;

      s.tmp_10 = (int)((uint)s.src_ptr[s.row_bytes + 1] - (uint)s.src_ptr[1]);
      s.tmp_10 *= ((int)*s.y_map_ptr & 0xff);
      s.tmp_10 >>= 8;
      s.tmp_10 += (int)(uint)s.src_ptr[1];
      *(byte *)((int)param_1 + 1) = (byte)s.tmp_10;

      s.tmp_10 = (int)((uint)s.src_ptr[s.row_bytes + 2] - (uint)s.src_ptr[2]);
      s.tmp_10 *= ((int)*s.y_map_ptr & 0xff);
      s.tmp_10 >>= 8;
      s.tmp_10 += (int)(uint)s.src_ptr[2];
      *(byte *)((int)param_1 + 2) = (byte)s.tmp_10;

      s.y_map_ptr += 1;
      param_1 = (uint *)((int)param_1 + s.row_bytes);
      s.y += 1;
    }
    s.x += 1;
  }

  if (height < wvl_entry[8]) {
    memcpy((void *)((height - 1) * s.row_bytes + (int)s.out_base),s.x_map,s.row_bytes);
  } else {
    memset((void *)((height - 1) * s.row_bytes + (int)s.out_base),0,s.row_bytes);
  }

  s.unk_5060 = 4;
  s.tmp_10 = (width * 3) % s.unk_5060;
  s.tmp_10 = s.unk_5060 - s.tmp_10;
  s.unk_505c = s.tmp_10 % s.unk_5060;
  if (DAT_1001d258 == 0) {
    QuantizeBgr24ToNearestPaletteColorInPlace(s.out_base,height,width,s.unk_505c);
  } else if (DAT_101221e8 == 0x10) {
    DitherBgr24ToRgbQuantizedF8(DAT_1001d258,DAT_1001d25c,s.out_base,height,width,s.unk_505c);
  } else if (DAT_101221e8 == 8) {
    DitherBgr24ToPaletteColors(DAT_1001d258,DAT_1001d25c,s.out_base,height,width,s.unk_505c);
  }

  return s.out_base;
}
