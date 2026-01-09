#include "CardArtLib.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "inttypes.h"
#include "mystdbool.h"

typedef void * pointer;

typedef struct CatalogEntry {
  int key;
  uint offset;
  uint size;
} CatalogEntry;

// SIZE 0x114
typedef struct Catalog {
  FILE *file;
  int entry_count;
  CatalogEntry *entries;
  CatalogEntry *cached_entry;
  char path[0x104];
} Catalog;

typedef BOOL (WINAPI code)(HINSTANCE, DWORD, LPVOID);
void MSVC_StackProbe(void);

void __cdecl FreeIfNotNull(void *ptr)
{
  if (ptr != (void *)0x0) {
    free(ptr);
  }
  return;
}

/* Forward declarations for globals referenced before their definitions. */
extern undefined4 DAT_100ed70c;
extern undefined1 DAT_10031ca0[0x400];
extern unsigned char DAT_100326d8[0x1000];
extern undefined1 DAT_100eaf00[0x2000];
extern undefined1 DAT_100ecb10[0x800];
extern undefined1 DAT_10115cf0[0x800];
extern undefined1 DAT_101164f0[0x800];

void __cdecl RotateDwordsLeft1(undefined4 *param_1,int param_2);
char * __cdecl StrCopy(char *dst,const char *src);
char * __cdecl StrCat(char *dst,const char *src);
BITMAPINFO * __cdecl AllocBitmapInfoTopDown(int width,int height,int bits_per_pixel);
bool __cdecl FreeBitmapInfo(void *ptr);
uint * __cdecl Wvl_DecodeToBgr24(uint *param_1,int *param_2,int param_3,int param_4);
int * __cdecl Catalog_LoadWvlEntry(int param_1,uint *param_2,int param_3);
undefined8 * __cdecl Wvl_DecodeHaar(int *param_1,undefined8 *param_2);
void DestroyPaletteOctree(void);
int __cdecl Huffman8_DecodeBytes(undefined1 *out_bytes,undefined4 bitstream_start,undefined4 bitstream_end);
undefined4 __cdecl Huffman8_BuildDecodeTable(int node_count);
int __cdecl Huffman13_DecodeDwords(undefined4 *out_dwords,undefined4 bitstream_start,undefined4 bitstream_end);
int __cdecl Huffman13_Init(undefined4 bitstream_start,undefined4 symbol_table,undefined4 node_index_base);
undefined4 __cdecl Huffman13_BuildDecodeTable(int node_count);
int __cdecl Huffman13_DecodeDwordsWithZeroRuns(undefined8 *out_dwords,uint *bitstream,undefined4 bitstream_end);
undefined4 __cdecl Wvl_UnpackPieces(int param_1,int *param_2);
void ShutdownCardArtGdiResources(void);
void __cdecl checked_DeleteDC_DeleteObject(HDC param_1,HGDIOBJ param_2);
void DestroyCardArtPalette(void);
void CardArtLib_Shutdown(void);
void __cdecl InitBitmapInfo24bppTopDown(BITMAPINFO *bmi,int width,int height);
undefined * __cdecl FindVersionedSmallArtCacheEntry(int id,int version);
void __cdecl DestroyVersionedSmallArt(int id,int version);
void DestroyAllVersionedSmallArts(void);
undefined2 * __cdecl TENTATIVE_read_palette(char *param_1,char *param_2);
void __cdecl CopyBgr24RectIntoStridedBuffer(undefined8 *dst_bgr24,undefined8 *src_bgr24,int dst_x,int dst_y,
                                           int rect_width,int rect_height,int dst_stride_pixels);
void __cdecl CopyBytes(void *dst,const void *src,size_t size);
void __cdecl SetBytes(void *dst,uint value,size_t size);
void __cdecl Haar2D_ReconstructInPlace(int *coeffs,int full_size,int base_size);
void __cdecl Haar_CombineSumDiff(int *param_1,int *param_2,int *param_3,int param_4,int param_5,undefined4 param_6,
                          int param_7);
void __cdecl Haar_CombineSumDiffHalf(int *param_1,int *param_2,int *param_3,int param_4,int param_5,
                                    undefined4 param_6,int param_7);
undefined1 * __cdecl
YuvPlanesToBgr24(undefined1 *out_bgr24,int *luma,int width,int height,int chroma_u,int chroma_v,
                 int chroma_stride,undefined4 unused_chroma_height,int chroma_is_420);


// GLOBAL: CARDARTLIB 0x1001D05C
char s__assertFile_txt_1001d05c[] = "\\assertFile.txt";

// GLOBAL: CARDARTLIB 0x1001D06C
char DAT_1001d06c[] = "at";

// GLOBAL: CARDARTLIB 0x1001D070
char s_File__s__Line__d_1001d070[] = "File-> %s, Line-> %d\n";

// GLOBAL: CARDARTLIB 0x1001D088
char s__s_s_1001d088[] = "%s%s\n";

// GLOBAL: CARDARTLIB 0x1001D090
char s_Assertion_Error_1001d090[] = "Assertion Error";

// GLOBAL: CARDARTLIB 0x1001D0A0
char s__assertFile_txt_1001d0a0[] = "\\assertFile.txt";

// GLOBAL: CARDARTLIB 0x1001D0B0
char DAT_1001d0b0[] = "at";

// GLOBAL: CARDARTLIB 0x1001D0B4
char s_File__s__Line__d_1001d0b4[] = "File-> %s, Line-> %d\n";

// GLOBAL: CARDARTLIB 0x1001D0CC
char s__s_s_1001d0cc[] = "%s%s\n";

// GLOBAL: CARDARTLIB 0x1001D0D4
char s_Assertion_Error_1001d0d4[] = "Assertion Error";

// GLOBAL: CARDARTLIB 0x1001D0E4
undefined4 DAT_1001d0e4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001D0E8
char s__DUELPALall_TR_1001d0e8[] = "\\DUELPALall.TR";

// GLOBAL: CARDARTLIB 0x1001D0F8
char s__DUEL_plogpal_1001d0f8[] = "\\DUEL.plogpal";

// GLOBAL: CARDARTLIB 0x1001D108
undefined4 DAT_1001d108 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001D10C
char s_Too_many_open_Catalogs__Max__d_1001d10c[] = "Too many open Catalogs: Max %d\n";

// GLOBAL: CARDARTLIB 0x1001D12C
char s_D__Newmagic_sources_NedCard_Cata_1001d12c[] = "D:\\Newmagic\\sources\\NedCard\\Catalog.c";

// GLOBAL: CARDARTLIB 0x1001D154
char DAT_1001d154[] = "rb";

// GLOBAL: CARDARTLIB 0x1001D158
char s_Duplicate_short_name_found_in_ca_1001d158[] = "Duplicate short name found in catalogs\n%s entry %d and\n%s entry %d\nShortName value 0x%08lx";

// GLOBAL: CARDARTLIB 0x1001D1B4
char s_D__Newmagic_sources_NedCard_Cata_1001d1b4[] = "D:\\Newmagic\\sources\\NedCard\\Catalog.c";

// GLOBAL: CARDARTLIB 0x1001D1DC
undefined4 DAT_1001d1dc = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001D1E0
char s__CARDART_1001d1e0[] = "\\CARDART";

// GLOBAL: CARDARTLIB 0x1001D1EC
char s__CARDS_DAT_1001d1ec[] = "\\CARDS.DAT";

// GLOBAL: CARDARTLIB 0x1001D1F8
char DAT_1001d1f8[] = "rb";

// GLOBAL: CARDARTLIB 0x1001D1FC
char s__s__04d_c_WVL_1001d1fc[] = "%s\\%04d%c.WVL";

// GLOBAL: CARDARTLIB 0x1001D20C
char s__s__04d_WVL_1001d20c[] = "%s\\%04d.WVL";

// GLOBAL: CARDARTLIB 0x1001D218
char s__s__04d_WVL_1001d218[] = "%s\\%04d.WVL";

// GLOBAL: CARDARTLIB 0x1001D224
char s__s__04d_c_WVL_1001d224[] = "%s\\%04d%c.WVL";

// GLOBAL: CARDARTLIB 0x1001D234
char s__s__04d_WVL_1001d234[] = "%s\\%04d.WVL";

// GLOBAL: CARDARTLIB 0x1001D240
undefined4 DAT_1001d240 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001D244
undefined4 * PTR_DAT_1001d244 = &DAT_100ed70c;

// GLOBAL: CARDARTLIB 0x1001D248
undefined4 DAT_1001d248 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001D24C
undefined1 * PTR_DAT_1001d24c = DAT_101164f0;

// GLOBAL: CARDARTLIB 0x1001D250
undefined1 * PTR_DAT_1001d250 = DAT_10115cf0;

// GLOBAL: CARDARTLIB 0x1001D254
undefined1 * PTR_DAT_1001d254 = DAT_100ecb10;

// GLOBAL: CARDARTLIB 0x1001D258
undefined4 DAT_1001d258 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001D25C
undefined4 DAT_1001d25c = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001D260
int DAT_1001d260[16] = {0,0,2,4,12,7,10,12,12,0,0,0,2,2,3,2};

// GLOBAL: CARDARTLIB 0x1001D288
int DAT_1001d288[8] = {0,0,2,2,3,2,3,3};

// GLOBAL: CARDARTLIB 0x1001D2B0
int DAT_1001d2b0[8] = {0,0,16,16,42,32,32,48};

// GLOBAL: CARDARTLIB 0x1001D2D8
undefined1 DAT_1001d2d8[0x900];

// GLOBAL: CARDARTLIB 0x1001D2E4
int DAT_1001d2e4[0x900 / 4];

// GLOBAL: CARDARTLIB 0x1001D998
int DAT_1001d998[0x900 / 4];

// GLOBAL: CARDARTLIB 0x1001D9A4
int DAT_1001d9a4[0x900 / 4];

// GLOBAL: CARDARTLIB 0x1001E058
undefined1 * PTR_DAT_1001e058 = DAT_10031ca0 + 0x200;

// GLOBAL: CARDARTLIB 0x1001E05C
undefined4 DAT_1001e05c = 0xFFFFFFFF;

// GLOBAL: CARDARTLIB 0x1001E060
undefined4 DAT_1001e060 = 0xFFFFFFFF;

// GLOBAL: CARDARTLIB 0x1001E064
char DAT_1001e064[] = "rt";

// GLOBAL: CARDARTLIB 0x1001E068
char s__d____d__d__d_1001e068[] = "%d - %d %d %d";

// GLOBAL: CARDARTLIB 0x1001E078
char DAT_1001e078[] = "rb";

// GLOBAL: CARDARTLIB 0x1001E07C
char DAT_1001e07c[] = " \n";

// GLOBAL: CARDARTLIB 0x1001E080
char DAT_1001e080[] = " \n";

// GLOBAL: CARDARTLIB 0x1001E084
char DAT_1001e084[] = " \n";

// GLOBAL: CARDARTLIB 0x1001E088
char DAT_1001e088[] = " \n";

// GLOBAL: CARDARTLIB 0x1001E08C
char DAT_1001e08c[] = " \n";

// GLOBAL: CARDARTLIB 0x1001E090
char s_Not_enough_memory_for_delta_arra_1001e090[] = "Not enough memory for delta array\r\n";

// GLOBAL: CARDARTLIB 0x1001E0B4
char s_D__Newmagic_sources_NedCard_Pale_1001e0b4[] = "D:\\Newmagic\\sources\\NedCard\\Palette.c";

// GLOBAL: CARDARTLIB 0x1001E0E0
undefined4 DAT_1001e0e0 = 0x00000004;

// GLOBAL: CARDARTLIB 0x1001E118
unsigned char * g_waveletScaleToByteTable = DAT_100326d8 + 0x400;

// GLOBAL: CARDARTLIB 0x1001E11C
undefined4 DAT_1001e11c = 0x00000001;

// GLOBAL: CARDARTLIB 0x1001E120
undefined4 DAT_1001e120 = 0x00000002;

// GLOBAL: CARDARTLIB 0x1001E124
undefined4 g_haarScratchInit = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001E128
undefined1 * g_yuvClampTable = DAT_100eaf00 + 0x400;

// GLOBAL: CARDARTLIB 0x1001E12C
char s_SmallArt_cat_1001e12c[] = "SmallArt.cat";

// GLOBAL: CARDARTLIB 0x1001E13C
char s_MedArt_cat_1001e13c[] = "MedArt.cat";

// GLOBAL: CARDARTLIB 0x1001E148
char DAT_1001e148[] = "\n";

// GLOBAL: CARDARTLIB 0x1001E14C
char s_wavelet_pieces_has_illegal_value_1001e14c[] = "wavelet pieces has illegal value: %d";

// GLOBAL: CARDARTLIB 0x1001E174
char s_D__Newmagic_sources_NedCard_haar_1001e174[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: CARDARTLIB 0x1001E198
char s_Only_Works_on_24_bit_images_1001e198[] = "Only Works on 24 bit images\n";

// GLOBAL: CARDARTLIB 0x1001E1B8
char s_D__Newmagic_sources_NedCard_haar_1001e1b8[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: CARDARTLIB 0x1001E1DC
char s_Only_Works_on_24_bit_images_1001e1dc[] = "Only Works on 24 bit images\n";

// GLOBAL: CARDARTLIB 0x1001E1FC
char s_D__Newmagic_sources_NedCard_haar_1001e1fc[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: CARDARTLIB 0x1001E2A0
undefined4 DAT_1001e2a0 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10020210
char DAT_10020210[2000];

// GLOBAL: CARDARTLIB 0x100209E0
undefined4 DAT_100209e0 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100209E4
undefined4 DAT_100209e4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100209E8
unsigned char DAT_100209e8[0x400];
#define DAT_100209e9 DAT_100209e8[1]
#define DAT_100209ea DAT_100209e8[2]
#define DAT_100209eb DAT_100209e8[3]

// GLOBAL: CARDARTLIB 0x10020DE8
CRITICAL_SECTION DAT_10020de8;

// GLOBAL: CARDARTLIB 0x10020E84
undefined4 DAT_10020e84 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10020E00
uint DAT_10020e00[0x21];

// GLOBAL: CARDARTLIB 0x10020E88
unsigned char DAT_10020e88[0x100 * 0xc];
#define DAT_10020e8c DAT_10020e88[4]
#define DAT_10020e90 DAT_10020e88[8]

// GLOBAL: CARDARTLIB 0x10021A88
uint *DAT_10021a88 = (uint *)0x0;

// GLOBAL: CARDARTLIB 0x10021A8C
int _DAT_10021a8c = 0;

// GLOBAL: CARDARTLIB 0x10021A90
int DAT_10021a90 = 0;

// GLOBAL: CARDARTLIB 0x10021A94
undefined4 DAT_10021a94 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10021A98
int _DAT_10021a98 = 0;

// GLOBAL: CARDARTLIB 0x10021A9C
undefined4 DAT_10021a9c = 0x00000000;

// GLOBAL: CARDARTLIB 0x10021AA0
unsigned char DAT_10021aa0[0x100 * 8];
#define DAT_10021aa4 DAT_10021aa0[4]

// GLOBAL: CARDARTLIB 0x10031AA0
undefined1 DAT_10031aa0 = 0x00;

// GLOBAL: CARDARTLIB 0x10031AA1
undefined1 DAT_10031aa1 = 0x00;

// GLOBAL: CARDARTLIB 0x10031CA0
undefined1 DAT_10031ca0[0x400];

// GLOBAL: CARDARTLIB 0x10031EA4
undefined4 g_paletteOctreeRoot = 0x00000000;

// GLOBAL: CARDARTLIB 0x10031EA8
undefined4 DAT_10031ea8 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10031EB0
undefined1 DAT_10031eb0 = 0x00;

// GLOBAL: CARDARTLIB 0x10031EB1
undefined1 DAT_10031eb1 = 0x00;

// GLOBAL: CARDARTLIB 0x10031EB8
undefined4 _DAT_10031eb8 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10031EC0
undefined1 g_paletteRgbTable[0x400];

// GLOBAL: CARDARTLIB 0x100322C0
undefined1 DAT_100322c0 = 0x00;

// GLOBAL: CARDARTLIB 0x100322C1
undefined1 DAT_100322c1 = 0x00;

// GLOBAL: CARDARTLIB 0x100322CC
undefined4 DAT_100322cc = 0x00000000;

// GLOBAL: CARDARTLIB 0x100322D0
undefined4 DAT_100322d0 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100322D4
undefined4 DAT_100322d4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100322D8
undefined1 DAT_100322d8 = 0x00;

// GLOBAL: CARDARTLIB 0x100322D9
undefined1 DAT_100322d9 = 0x00;

// GLOBAL: CARDARTLIB 0x10032ADC
undefined4 DAT_10032adc = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032AE0
undefined4 g_waveletScaleToByteTableInit = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032AE8
int DAT_10032ae8[0x80];

// GLOBAL: CARDARTLIB 0x10032B04
undefined4 DAT_10032b04 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032B08
undefined4 DAT_10032b08 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032B10
undefined4 DAT_10032b10 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032C88
undefined4 DAT_10032c88 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032C8C
undefined4 DAT_10032c8c = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032C90
undefined4 DAT_10032c90 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032C94
undefined4 DAT_10032c94 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10032C98
undefined1 DAT_10032c98[0x2000];

// GLOBAL: CARDARTLIB 0x100326D8
unsigned char DAT_100326d8[0x1000];

// GLOBAL: CARDARTLIB 0x100AD498
unsigned char DAT_100ad498[0x200];

// GLOBAL: CARDARTLIB 0x100EA098
undefined4 DAT_100ea098 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100EA09C
undefined4 g_yuvClampTableInit = 0x00000000;

// GLOBAL: CARDARTLIB 0x100EA31C
int *g_haarScratchB = (int *)0x0;

// GLOBAL: CARDARTLIB 0x100EA320
int *g_haarScratchA = (int *)0x0;

// GLOBAL: CARDARTLIB 0x100EAB00
undefined1 DAT_100eab00 = 0x00;

// GLOBAL: CARDARTLIB 0x100EAB01
undefined1 DAT_100eab01 = 0x00;

// GLOBAL: CARDARTLIB 0x100EAF00
undefined1 DAT_100eaf00[0x2000];

// GLOBAL: CARDARTLIB 0x100ECB04
undefined4 _DAT_100ecb04 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100ECB10
undefined1 DAT_100ecb10[0x800];

// GLOBAL: CARDARTLIB 0x100ED310
int DAT_100ed310[0x200];

// GLOBAL: CARDARTLIB 0x100ED70C
undefined4 DAT_100ed70c = 0x00000000;

// GLOBAL: CARDARTLIB 0x100EDB10
undefined1 DAT_100edb10[5 * 0x8060];

// GLOBAL: CARDARTLIB 0x10115CF0
undefined1 DAT_10115cf0[0x800];

// GLOBAL: CARDARTLIB 0x101164F0
undefined1 DAT_101164f0[0x800];

// GLOBAL: CARDARTLIB 0x10116CF0
undefined2 DAT_10116cf0 = 0x0000;

// GLOBAL: CARDARTLIB 0x10116CF2
undefined2 DAT_10116cf2 = 0x0000;

// GLOBAL: CARDARTLIB 0x10116CF7
undefined1 DAT_10116cf7 = 0x00;

// GLOBAL: CARDARTLIB 0x101170F3
undefined1 DAT_101170f3 = 0x00;

// GLOBAL: CARDARTLIB 0x10117210
unsigned char DAT_10117210[0x80];

// GLOBAL: CARDARTLIB 0x10117290
Catalog DAT_10117290[5];

// GLOBAL: CARDARTLIB 0x10117100
void *DAT_10117100[0x41];

// GLOBAL: CARDARTLIB 0x101177F4
undefined4 DAT_101177f4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10117800
char DAT_10117800[0x105];

// GLOBAL: CARDARTLIB 0x10117910
char DAT_10117910[0x105];

// GLOBAL: CARDARTLIB 0x10121FE0
undefined4 g_versionedSmallArtCount = 0x00000000;

// GLOBAL: CARDARTLIB 0x10121FE4
undefined4 _DAT_10121fe4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10122000
undefined4 DAT_10122000 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10122004
undefined4 DAT_10122004 = 0x00000000;

// GLOBAL: CARDARTLIB 0x101221E8
undefined4 DAT_101221e8 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10117A20
CRITICAL_SECTION DAT_10117a20;

// GLOBAL: CARDARTLIB 0x101221F0
CRITICAL_SECTION DAT_101221f0;

// GLOBAL: CARDARTLIB 0x101221D0
CRITICAL_SECTION DAT_101221d0;

// GLOBAL: CARDARTLIB 0x101200A0
int DAT_101200a0[2000];

// GLOBAL: CARDARTLIB 0x10117A40
unsigned char DAT_10117a40[2000 * 0x10];
#define DAT_10117a44 DAT_10117a40[4]
#define DAT_10117a48 DAT_10117a40[8]
#define DAT_10117a4c DAT_10117a40[0xc]

// GLOBAL: CARDARTLIB 0x1011F740
unsigned char g_versionedSmallArtCache[100 * 0x18];
#define DAT_1011f744 g_versionedSmallArtCache[4]
#define DAT_1011f748 g_versionedSmallArtCache[8]
#define DAT_1011f74c g_versionedSmallArtCache[0xc]
#define DAT_1011f750 g_versionedSmallArtCache[0x10]
#define DAT_1011f754 g_versionedSmallArtCache[0x14]

// GLOBAL: CARDARTLIB 0x10121FF0
unsigned char DAT_10121ff0[0x14 * 0x18];
#define DAT_10121ff4 DAT_10121ff0[4]
#define DAT_10121ff8 DAT_10121ff0[8]
#define DAT_10121ffc DAT_10121ff0[0xc]

// GLOBAL: CARDARTLIB 0x10123340
code *DAT_10123340 = (code *)0x0;

// MATCHING
// FUNCTION: CARDARTLIB 0x10001000
void __cdecl assert(int condition,char *file,int line,char *fmt,...)

{
  char local_114 [260];
  time_t now;
  FILE *logFile;
  va_list args;

  if (condition == 0) {
    StrCopy(local_114,DAT_10117800);
    StrCat(local_114,s__assertFile_txt_1001d05c);
    logFile = fopen(local_114,DAT_1001d06c);

    va_start(args,fmt);
    if ((file != (char *)0x0) && (line != 0)) {
      sprintf(DAT_10020210,s_File__s__Line__d_1001d070,file,line);
    }

    _vsnprintf(DAT_10020210 + strlen(DAT_10020210),2000,fmt,args);
    
    time(&now);

    fprintf(logFile,s__s_s_1001d088,ctime(&now),DAT_10020210);
    fclose(logFile);

    MessageBoxA((HWND)0x0,DAT_10020210,s_Assertion_Error_1001d090,0x1000);
                    /* WARNING: Subroutine does not return */
    exit(0xff);  
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001116
void __cdecl assert_noabort(int condition,char *file,int line,char *fmt,...)
{
  char local_114 [260];
  time_t now;
  FILE *logFile;
  va_list args;

  if (condition == 0) {
    StrCopy(local_114,DAT_10117800);
    StrCat(local_114,s__assertFile_txt_1001d0a0);
    logFile = fopen(local_114,DAT_1001d0b0);

    va_start(args,fmt);
    if ((file != (char *)0x0) && (line != 0)) {
      sprintf(DAT_10020210,s_File__s__Line__d_1001d0b4,file,line);
    }

    _vsnprintf(DAT_10020210 + strlen(DAT_10020210),2000,fmt,args);
    
    time(&now);

    fprintf(logFile,s__s_s_1001d0cc,ctime(&now),DAT_10020210);
    fclose(logFile);

    MessageBoxA((HWND)0x0,DAT_10020210,s_Assertion_Error_1001d0d4,0x1000);
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
    InitializeCriticalSection((LPCRITICAL_SECTION)&DAT_10020de8);
  }

  if (DAT_1001d0e4 == 0)
    iVar1=0;
  
  if (iVar1 == 0)
    ShutdownCardArtGdiResources();

  return iVar1;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100012A8
void ShutdownCardArtGdiResources(void)

{
  if (DAT_1001d0e4 != (HDC)0x0) {
    checked_DeleteDC_DeleteObject(DAT_1001d0e4,DAT_100209e4);
    DAT_1001d0e4 = (HDC)0x0;
    DeleteCriticalSection((LPCRITICAL_SECTION)&DAT_10020de8);
  }
  if (DAT_100209e0 != 0) {
    DestroyCardArtPalette();
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100012FB
static void ApplyCardArtPaletteToDc(HDC hdc)

{
  SelectPalette(hdc,DAT_100209e0,0);
  RealizePalette(hdc);
  GdiFlush();
  SetDIBColorTable(hdc,0,0x100,(RGBQUAD *)&DAT_100209e8);
  SetStretchBltMode(hdc,3);
}

// FUNCTION: CARDARTLIB 0x1000134A
static BOOL CreateOffscreen32bppDibSection(int width,int height,HDC *out_dc,BITMAPINFO *bmi_optional,
                                          HBITMAP *out_bitmap,HGDIOBJ *out_prev_object,void **out_bits)

{
  HBITMAP local_44;
  HDC hdc;
  HDC local_3c;
  BITMAPINFO local_38;
  HGDIOBJ local_c;
  void *bits;
  
  local_3c = (HDC)0x0;
  local_44 = (HBITMAP)0x0;
  bits = (void *)0x0;
  if ((out_dc == (HDC *)0x0) || (out_bitmap == (HBITMAP *)0x0)) {
    return 0;
  }
 
  if (bmi_optional == (BITMAPINFO *)0x0) {
    bmi_optional = &local_38;
  }
  hdc = GetDC((HWND)0x0);
  if (hdc != (HDC)0x0) {
    ApplyCardArtPaletteToDc(hdc);
    local_3c = CreateCompatibleDC(hdc);
    if (local_3c != (HDC)0x0) {
      InitBitmapInfo24bppTopDown(bmi_optional,width,height);
      local_38.bmiHeader.biBitCount = 0x20;
      local_44 = CreateDIBSection(hdc,bmi_optional,0,&bits,(HANDLE)0x0,0);
      local_c = SelectObject(local_3c,local_44);
      ApplyCardArtPaletteToDc(local_3c);
    }
    ReleaseDC((HWND)0x0,hdc);
  }
  if (((local_3c == (HDC)0x0) || (local_44 == (HBITMAP)0x0)) || (bits == (void *)0x0)) {
    if (local_3c != (HDC)0x0) {
      DeleteDC(local_3c);
    }
    if (local_44 != (HBITMAP)0x0) {
      DeleteObject(local_44);
    }
    return 0;
  }
  else {
    if (out_dc != 0)
      *out_dc = local_3c;

    if (out_bitmap != 0)
      *out_bitmap = local_44;
    if (out_prev_object != (HGDIOBJ *)0x0) {
      *out_prev_object = local_c;
    }
    if (out_bits != (void **)0x0) {
      *out_bits = bits;
    }
    return 1;
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100014D2
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

// FUNCTION: CARDARTLIB 0x1000156D
static BOOL DrawBitmapSubrectToRect(HDC dst_dc,const RECT *dst_rect,HBITMAP bitmap,int src_x,int src_y,
                                   int src_width,int src_height)

{
  int dst_height;
  int dst_width;
  int dst_left;
  int dst_top;
  BITMAP bm;
  HGDIOBJ h;
  
  if (((dst_dc == (HDC)0x0) || (dst_rect == (RECT *)0x0)) || (bitmap == (HBITMAP)0x0)) {
    return 0;
  }

  EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10020de8);
  h = SelectObject(DAT_1001d0e4,bitmap);
  GetObjectA(bitmap,sizeof(bm),&bm);

  dst_left = dst_rect->left;
  dst_top = dst_rect->top;
  dst_width =  (dst_rect->right < dst_rect->left) ? bm.bmWidth : dst_rect->right - dst_rect->left;

  dst_height = (dst_rect->bottom < dst_rect->top) ? bm.bmHeight : dst_rect->bottom - dst_rect->top;

  ApplyCardArtPaletteToDc(DAT_1001d0e4);

  StretchBlt(dst_dc,dst_left,dst_top,dst_width,dst_height,DAT_1001d0e4,src_x,src_y,
    src_width <= bm.bmWidth ? src_width : bm.bmWidth,
    src_height <= bm.bmHeight ? src_height : bm.bmHeight, 0xcc0020);
  SelectObject(DAT_1001d0e4,h);
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10020de8);
  return 1;
}

// FUNCTION: CARDARTLIB 0x100016B7
static BOOL SetupDuelPalette(void)

{
  UINT UVar1;
  PALETTEENTRY local_628;
  uint local_624 [66];
  undefined4 local_51c;
  UINT local_514;
  uint local_510 [66];
  LOGPALETTE *local_408;
  PALETTEENTRY local_404 [256];
  
  local_51c = 1;
  StrCopy((char *)local_624,DAT_10117800);
  StrCat((char *)local_624,s__DUELPALall_TR_1001d0e8);
  StrCopy((char *)local_510,DAT_10117800);
  StrCat((char *)local_510,s__DUEL_plogpal_1001d0f8);
  local_408 = (LOGPALETTE *)TENTATIVE_read_palette((char *)local_624,(char *)local_510);
  if (local_408 != (LOGPALETTE *)0x0) {
    for (local_514 = 1; (int)local_514 < 0xff; local_514 = local_514 + 1) {
      local_408->palPalEntry[local_514].peFlags = '\x04';
    }
    DAT_100209e0 = CreatePalette(local_408);
    if (DAT_100209e0 != (HPALETTE)0x0) {
      local_628.peRed = 0xff;
      local_628.peGreen = 0xff;
      local_628.peBlue = 0xff;
      local_628.peFlags = '\0';
      SetPaletteEntries(DAT_100209e0,0xff,1,&local_628);
      local_628.peRed = 0xfe;
      local_628.peGreen = 0xfe;
      local_628.peBlue = 0xfe;
      local_628.peFlags = '\x04';
      SetPaletteEntries(DAT_100209e0,0xbf,1,&local_628);
      for (local_514 = 0xec; (int)local_514 < 0xff; local_514 = local_514 + 1) {
        local_628.peRed = '\x01';
        local_628.peGreen = '\x01';
        local_628.peBlue = '\x01';
        local_628.peFlags = '\x04';
        SetPaletteEntries(DAT_100209e0,local_514,1,&local_628);
      }
      UVar1 = GetPaletteEntries(DAT_100209e0,0,0x100,local_404);
      for (local_514 = 0; (int)local_514 < (int)UVar1; local_514 = local_514 + 1) {
        DAT_100209e8[local_514 * 4] = local_404[local_514].peBlue;
        DAT_100209e8[local_514 * 4 + 1] = local_404[local_514].peGreen;
        DAT_100209e8[local_514 * 4 + 2] = local_404[local_514].peRed;
        DAT_100209e8[local_514 * 4 + 3] = 0;
      }
      while (local_514 = UVar1, (int)local_514 < 0x100) {
        DAT_100209e8[local_514 * 4] = 0;
        DAT_100209e8[local_514 * 4 + 1] = 0;
        DAT_100209e8[local_514 * 4 + 2] = 0;
        DAT_100209e8[local_514 * 4 + 3] = 0;
        UVar1 = local_514 + 1;
      }
    }
    else
      local_51c=0;
  }
  else
    local_51c=0;

  return (BOOL)local_51c;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100019A8
void DestroyCardArtPalette(void)

{
  DeleteObject(DAT_100209e0);
  DAT_100209e0 = (HGDIOBJ)0x0;
  DestroyPaletteOctree();
}

// FUNCTION: CARDARTLIB 0x100019D0
static int Catalog_Open(const char *catalog_path)

{
  int iVar1;
  Catalog *catalog;
  FILE *catalog_file;
  int other_slot;
  int slot_index;
  int entry_index;
  
  slot_index = -1;
  other_slot = 0;
  do {
    if (4 < other_slot) {
LAB_10001a24:
      assert((uint)(slot_index != -1),s_D__Newmagic_sources_NedCard_Cata_1001d12c,0x43,
             s_Too_many_open_Catalogs__Max__d_1001d10c,5);
      catalog = &DAT_10117290[slot_index];
      catalog->cached_entry = (CatalogEntry *)0x0;
      StrCopy(catalog->path,catalog_path);
      catalog_file = fopen(catalog_path,&DAT_1001d154);
      catalog->file = catalog_file;
      if (catalog_file == (FILE *)0x0) {
        slot_index = 0;
      }
      else {
        fread(&catalog->entry_count,4,1,catalog_file);
        catalog->entries = (CatalogEntry *)malloc(catalog->entry_count * sizeof(CatalogEntry));
        fread(catalog->entries,sizeof(CatalogEntry),(size_t)catalog->entry_count,catalog_file);
        if (DAT_1001d108 != 0) {
          for (other_slot = 0; other_slot < 5; other_slot = other_slot + 1) {
            Catalog *other_catalog;
            if ((slot_index != other_slot) &&
                (*(int *)(&DAT_10117290 + other_slot * 0x114) != 0)) {
              other_catalog = (Catalog *)((char *)&DAT_10117290 + other_slot * sizeof(Catalog));
              entry_index = 0;
              while (entry_index < catalog->entry_count) {
                for (iVar1 = 0; iVar1 < other_catalog->entry_count; iVar1 = iVar1 + 1) {
                  assert((uint)(catalog->entries[entry_index].key != other_catalog->entries[iVar1].key),
                         s_D__Newmagic_sources_NedCard_Cata_1001d1b4,0x69,
                         s_Duplicate_short_name_found_in_ca_1001d158,catalog->path,entry_index,
                         other_catalog->path,iVar1,catalog->entries[entry_index].key);
                }
                entry_index = entry_index + 1;
              }
            }
          }
        }
        slot_index = slot_index + 1;
      }
      return slot_index;
    }
    if (*(int *)(&DAT_10117290 + other_slot * 0x114) == 0) {
      slot_index = other_slot;
      goto LAB_10001a24;
    }
    other_slot = other_slot + 1;
  } while( true );
}

// FUNCTION: CARDARTLIB 0x10001C1A
bool Catalog_Close(int handle)

{
  int slot_index;
  Catalog *catalog;
  FILE *catalog_file;
  
  slot_index = handle + -1;
  catalog = &DAT_10117290[slot_index];
  catalog_file = catalog->file;
  if (catalog_file != 0) {
    FreeIfNotNull(catalog->entries);
    fclose(catalog_file);
    catalog->entries = (CatalogEntry *)0x0;
    catalog->file = (FILE *)0x0;
    catalog->entry_count = 0;
    catalog->cached_entry = (CatalogEntry *)0x0;
  }
  return catalog_file != 0;
}

// FUNCTION: CARDARTLIB 0x10001CD6
static int CatalogEntry_CompareKey(const int *key, const CatalogEntry *entry)
{
  if (entry->key < *key) {
    return 1;
  }
  if (*key < entry->key) {
    return -1;
  }
}

// FUNCTION: CARDARTLIB 0x10001D26
CatalogEntry *Catalog_FindEntryCached(Catalog *catalog,const char *name)

{
  CatalogEntry *entry;
  uint key;
  
  key = Catalog_MakeKeyFromPath(name);
  if ((catalog->cached_entry == (CatalogEntry *)0x0) || (catalog->cached_entry->key != key)) {
    entry = (CatalogEntry *)bsearch(&key,catalog->entries,(size_t)catalog->entry_count,sizeof(CatalogEntry),
                                    CatalogEntry_CompareKey);
    catalog->cached_entry = entry;
  }
  else {
    entry = catalog->cached_entry;
  }
  return entry;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001DA1
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

// FUNCTION: CARDARTLIB 0x10001E4A
uint Catalog_MakeKeyFromPath(const char *path)

{
  uint local_134 [4];
  char local_124 [260];
  undefined4 local_1c;
  undefined4 local_20;
  int local_138;
  int iVar1;
  uint local_10;
  int local_8;
  local_10 = 3;
  
  local_20 = 0;
  local_138 = 0;
  local_8 = 0;
  _splitpath(path,(char *)local_134,local_124,(char *)&local_1c,(char *)local_134);
  path = (char *)&local_1c;
  StrCat((char *)&local_1c,(char *)local_134);
  while( true ) {
    iVar1 = (int)*path;
    path = path + 1;
    if (iVar1 == 0) break;
    if ((local_10 & 1) == 0) {
      local_8 = local_10 * iVar1 + local_8;
    }
    else {
      local_138 = local_10 * iVar1 + local_138;
    }
    local_10 = local_10 + 1;
  }
  return (int)(char)(((byte *)&local_1c)[1] ^ ((byte *)&local_1c)[0]) << 0x18 |
         (local_8 * local_138 & 0xffffffU);
}

// FUNCTION: CARDARTLIB 0x10001F40
void MemZeroDwords(undefined8 *param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = param_2;
  if (((uint)param_1 & 4) != 0) {
    *(undefined1 *)param_1 = 0;
    param_1 = (undefined8 *)((int)param_1 + 4);
    uVar1 = param_2 - 1;
    if (uVar1 == 0 || (int)param_2 < 1) {
      return;
    }
  }
  uVar2 = uVar1 >> 1;
  if (uVar2 != 0) {
    while (uVar2 = uVar2 - 1, uVar2 != 0) {
      *param_1 = 0;
      param_1 = param_1 + 1;
    }
    *param_1 = 0;
  }
  if ((uVar1 & 1) != 0) {
    *(undefined1 *)param_1 = 0;
  }
  return;
}

// FUNCTION: CARDARTLIB 0x10001F98
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl Huffman8_DecodeBytes(undefined1 *out_bytes,undefined4 bitstream_start,undefined4 bitstream_end)

{
  undefined1 *puVar1;
  uint uVar2;
  uint uVar3;
  byte local_28;
  int local_1c;
  int local_18;
  uint local_c;
  int local_8;
  
  puVar1 = out_bytes;
  DAT_10021a88 = (uint *)bitstream_start;
  _DAT_10021a8c = (int)bitstream_start;
  DAT_10021a90 = (int)bitstream_end;
  DAT_1001d1dc = 0;
  uVar2 = BitStream_ReadBits(8);
  for (local_18 = 0; local_18 < (int)uVar2; local_18 = local_18 + 1) {
    uVar3 = BitStream_ReadBits(9);
    *(uint *)(&DAT_10021aa0 + local_18 * 8) = uVar3;
    uVar3 = BitStream_ReadBits(9);
    *(uint *)(&DAT_10021aa4 + local_18 * 8) = uVar3;
  }
  Huffman8_BuildDecodeTable(uVar2);
  local_c = BitStream_ReadBits(8);
  do {
    while( true ) {
      if (local_c == 0xffffffff) {
        return (int)out_bytes - (int)puVar1;
      }
      if (*(int *)(&DAT_10020e88 + local_c * 0xc) < 0) break;
      uVar2 = *(uint *)(&DAT_10020e8c + local_c * 0xc);
      *out_bytes = (&DAT_10020e88)[local_c * 0xc];
      out_bytes = out_bytes + 1;
      local_28 = (byte)uVar2;
      uVar2 = BitStream_ReadBits(uVar2);
      if (uVar2 == 0xffffffff) {
        local_c = 0xffffffff;
      }
      else {
        local_c = (int)local_c >> (local_28 & 0x1f) | uVar2 << (8 - local_28 & 0x1f);
      }
    }
    local_8 = *(int *)(&DAT_10020e90 + local_c * 0xc);
    do {
      uVar2 = BitStream_ReadBit();
      if (uVar2 == 0xffffffff) goto LAB_10002132;
      if (uVar2 == 0) {
        local_1c = *(int *)(&DAT_10021aa4 + local_8 * 8);
      }
      else {
        local_1c = *(int *)(&DAT_10021aa0 + local_8 * 8);
      }
      local_8 = local_1c + -0x100;
    } while (-1 < local_8);
    *out_bytes = (undefined1)local_1c;
    out_bytes = out_bytes + 1;
LAB_10002132:
    local_c = BitStream_ReadBits(8);
  } while( true );
}

// FUNCTION: CARDARTLIB 0x10002154
undefined4 __cdecl Huffman8_BuildDecodeTable(int node_count)

{
  int iVar1;
  uint uVar2;
  int local_ac;
  int local_a4;
  uint local_a0;
  int local_9c;
  int local_98 [25];
  int local_34;
  int local_30 [7];
  int iStack_14;
  int local_c;
  int local_8;
  
  local_34 = 0;
  local_98[0] = 0x100;
  local_98[1] = 0x80;
  local_98[2] = 0x40;
  local_98[3] = 0x20;
  local_98[4] = 0x10;
  local_98[5] = 8;
  local_98[6] = 4;
  local_98[7] = 2;
  local_98[8] = 1;
  for (local_9c = 0; local_9c < 0x10; local_9c = local_9c + 1) {
    local_98[local_9c + 9] = 1;
  }
  local_8 = node_count + -1;
  local_30[0] = node_count + -1;
  do {
    iVar1 = local_34;
    local_34 = local_34 + 1;
    if (local_98[iVar1 + 9] == 0) {
      local_c = *(int *)(&DAT_10021aa4 + local_8 * 8);
    }
    else {
      local_c = *(int *)(&DAT_10021aa0 + local_8 * 8);
    }
    local_8 = local_c + -0x100;
    local_30[local_34] = local_c + -0x100;
    if (local_8 < 0) {
      local_a0 = 0;
      for (local_a4 = 0; local_a4 < local_34; local_a4 = local_a4 + 1) {
        local_a0 = local_a0 | local_98[local_a4 + 9] << ((byte)local_a4 & 0x1f);
      }
      for (local_9c = 0; local_9c < local_98[local_34]; local_9c = local_9c + 1) {
        uVar2 = local_9c << ((byte)local_34 & 0x1f);
        *(int *)(&DAT_10020e8c + (uVar2 | local_a0) * 0xc) = local_34;
        *(undefined4 *)(&DAT_10020e88 + (uVar2 | local_a0) * 0xc) =
             *(undefined4 *)(DAT_10021a9c + local_c * 4);
        *(undefined4 *)(&DAT_10020e90 + (uVar2 | local_a0) * 0xc) = 0xffffffff;
      }
      local_98[local_34 + 9] = 1;
      iVar1 = local_34;
      local_34 = local_34 + -1;
      local_98[iVar1 + 8] = local_98[iVar1 + 8] + -1;
      local_8 = local_30[local_34];
LAB_10002405:
      while ((iVar1 = local_34, -1 < local_98[9] && (local_98[local_34 + 9] < 0))) {
        local_98[local_34 + 9] = 1;
        local_34 = local_34 + -1;
        local_98[iVar1 + 8] = local_98[iVar1 + 8] + -1;
        local_8 = local_30[local_34];
      }
    }
    else if (local_34 == 8) {
      local_a0 = 0;
      for (local_ac = 0; local_ac < 8; local_ac = local_ac + 1) {
        local_a0 = local_a0 | local_98[local_ac + 9] << ((byte)local_ac & 0x1f);
      }
      *(undefined4 *)(&DAT_10020e88 + local_a0 * 0xc) = 0xffffffff;
      *(undefined4 *)(&DAT_10020e8c + local_a0 * 0xc) = 8;
      *(int *)(&DAT_10020e90 + local_a0 * 0xc) = local_8;
      local_98[0x11] = 1;
      local_34 = 7;
      local_98[0x10] = local_98[0x10] + -1;
      local_8 = iStack_14;
      goto LAB_10002405;
    }
    if (local_98[9] < 0) {
      return 0;
    }
  } while( true );
}

// FUNCTION: CARDARTLIB 0x10002457
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl Huffman13_DecodeDwords(undefined4 *out_dwords,undefined4 bitstream_start,undefined4 bitstream_end)

{
  undefined4 *puVar1;
  uint uVar2;
  byte local_24;
  int local_14;
  uint local_c;
  int local_8;
  
  puVar1 = out_dwords;
  DAT_10021a88 = bitstream_start;
  _DAT_10021a8c = bitstream_start;
  DAT_10021a90 = bitstream_end;
  DAT_1001d1dc = 0;
  local_c = BitStream_ReadBits(8);
  do {
    while( true ) {
      if (local_c == 0xffffffff) {
        return (int)out_dwords - (int)puVar1 >> 2;
      }
      if (0x7ffffffe < *(int *)(&DAT_10020e88 + local_c * 0xc)) break;
      uVar2 = *(uint *)(&DAT_10020e8c + local_c * 0xc);
      *out_dwords = *(undefined4 *)(&DAT_10020e88 + local_c * 0xc);
      out_dwords = out_dwords + 1;
      local_24 = (byte)uVar2;
      uVar2 = BitStream_ReadBits(uVar2);
      if (uVar2 == 0xffffffff) {
        local_c = 0xffffffff;
      }
      else {
        local_c = (int)local_c >> (local_24 & 0x1f) | uVar2 << (8 - local_24 & 0x1f);
      }
    }
    local_8 = *(int *)(&DAT_10020e90 + local_c * 0xc);
    do {
      uVar2 = BitStream_ReadBit();
      if (uVar2 == 0xffffffff) goto LAB_100025a6;
      if (uVar2 == 0) {
        local_14 = *(int *)(&DAT_10021aa4 + local_8 * 8);
      }
      else {
        local_14 = *(int *)(&DAT_10021aa0 + local_8 * 8);
      }
      local_8 = local_14 - _DAT_10021a98;
    } while (-1 < local_8);
    *out_dwords = *(undefined4 *)(DAT_10021a9c + local_14 * 4);
    out_dwords = out_dwords + 1;
LAB_100025a6:
    local_c = BitStream_ReadBits(8);
  } while( true );
}

// FUNCTION: CARDARTLIB 0x100025CB
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl Huffman13_Init(undefined4 bitstream_start,undefined4 symbol_table,undefined4 node_index_base)

{
  uint uVar1;
  uint local_c;
  int local_8;
  
  local_c = 0xd;
  for (local_8 = 0; local_8 < 0x20; local_8 = local_8 + 1) {
    *(uint *)(&DAT_10020e00 + local_8 * 4) = 0xffffffff >> ((byte)local_8 & 0x1f);
  }
  DAT_10021a88 = bitstream_start;
  _DAT_10021a8c = bitstream_start;
  DAT_10021a90 = 100000;
  DAT_1001d1dc = 0;
  DAT_10020e84 = 0;
  DAT_10021a94 = BitStream_ReadBits(0xd);
  for (local_8 = 0; local_8 < (int)DAT_10021a94; local_8 = local_8 + 1) {
    uVar1 = BitStream_ReadBits(0xd);
    *(uint *)(&DAT_10021aa0 + local_8 * 8) = uVar1;
    uVar1 = BitStream_ReadBits(0xd);
    *(uint *)(&DAT_10021aa4 + local_8 * 8) = uVar1;
    local_c = local_c + 0x1a;
  }
  _DAT_10021a98 = node_index_base;
  DAT_10021a9c = symbol_table;
  Huffman13_BuildDecodeTable(DAT_10021a94);
  return (uint)((local_c & 7) != 0) + ((int)(local_c + ((int)local_c >> 0x1f & 7U)) >> 3);
}

// FUNCTION: CARDARTLIB 0x100026E3
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl Huffman13_BuildDecodeTable(int node_count)

{
  int iVar1;
  uint uVar2;
  int local_fc;
  int local_f4;
  uint local_f0;
  int local_ec;
  int local_e8 [5];
  undefined4 local_d4;
  undefined4 local_d0;
  undefined4 local_cc;
  int local_c8 [35];
  int local_3c;
  int local_38 [7];
  int iStack_1c;
  int local_c;
  int local_8;
  
  local_3c = 0;
  local_d4 = 0x20;
  local_d0 = 0x40;
  local_cc = 0x80;
  local_c8[0] = 0x100;
  local_c8[1] = 0x200;
  local_c8[2] = 0x400;
  for (local_ec = 0; local_ec < 0x20; local_ec = local_ec + 1) {
    local_c8[local_ec + 3] = 1;
  }
  local_8 = node_count + -1;
  local_38[0] = node_count + -1;
  do {
    iVar1 = local_3c;
    local_3c = local_3c + 1;
    if (local_c8[iVar1 + 3] == 0) {
      local_c = *(int *)(&DAT_10021aa4 + local_8 * 8);
    }
    else {
      local_c = *(int *)(&DAT_10021aa0 + local_8 * 8);
    }
    local_8 = local_c - _DAT_10021a98;
    local_38[local_3c] = local_c - _DAT_10021a98;
    if (local_8 < 0) {
      local_f0 = 0;
      for (local_f4 = 0; local_f4 < local_3c; local_f4 = local_f4 + 1) {
        local_f0 = local_f0 | local_c8[local_f4 + 3] << ((byte)local_f4 & 0x1f);
      }
      for (local_ec = 0; local_ec < local_e8[8 - local_3c]; local_ec = local_ec + 1) {
        uVar2 = local_ec << ((byte)local_3c & 0x1f);
        *(int *)(&DAT_10020e8c + (uVar2 | local_f0) * 0xc) = local_3c;
        *(undefined4 *)(&DAT_10020e88 + (uVar2 | local_f0) * 0xc) =
             *(undefined4 *)(DAT_10021a9c + local_c * 4);
        *(undefined4 *)(&DAT_10020e90 + (uVar2 | local_f0) * 0xc) = 0xffffffff;
      }
      local_c8[local_3c + 3] = 1;
      iVar1 = local_3c;
      local_3c = local_3c + -1;
      local_c8[iVar1 + 2] = local_c8[iVar1 + 2] + -1;
      local_8 = local_38[local_3c];
LAB_100029d2:
      while ((iVar1 = local_3c, -1 < local_c8[3] && (local_c8[local_3c + 3] < 0))) {
        local_c8[local_3c + 3] = 1;
        local_3c = local_3c + -1;
        local_c8[iVar1 + 2] = local_c8[iVar1 + 2] + -1;
        local_8 = local_38[local_3c];
      }
    }
    else if (local_3c == 8) {
      local_f0 = 0;
      for (local_fc = 0; local_fc < 8; local_fc = local_fc + 1) {
        local_f0 = local_f0 | local_c8[local_fc + 3] << ((byte)local_fc & 0x1f);
      }
      *(undefined4 *)(&DAT_10020e88 + local_f0 * 0xc) = 0x7fffffff;
      *(undefined4 *)(&DAT_10020e8c + local_f0 * 0xc) = 8;
      *(int *)(&DAT_10020e90 + local_f0 * 0xc) = local_8;
      local_c8[0xb] = 1;
      local_3c = 7;
      local_c8[10] = local_c8[10] + -1;
      local_8 = iStack_1c;
      goto LAB_100029d2;
    }
    if (local_c8[3] < 0) {
      return 0;
    }
  } while( true );
}

// FUNCTION: CARDARTLIB 0x10002A33
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl FUN_10002a33(int param_1)

{
  int iVar1;
  int local_114;
  int local_110 [32];
  int local_90;
  int local_8c [32];
  int local_c;
  int local_8;
  
  local_90 = 0;
  for (local_114 = 0; local_114 < 0x20; local_114 = local_114 + 1) {
    local_110[local_114] = 1;
  }
  local_8c[0] = param_1 + -1;
  local_8 = local_8c[0];
  memset(&DAT_10117210,0,0x80);
  do {
    iVar1 = local_90;
    local_90 = local_90 + 1;
    if (local_110[iVar1] == 0) {
      local_c = *(int *)(&DAT_10021aa4 + local_8 * 8);
    }
    else {
      local_c = *(int *)(&DAT_10021aa0 + local_8 * 8);
    }
    local_8 = local_c - _DAT_10021a98;
    local_8c[local_90] = local_c - _DAT_10021a98;
    if (local_8 < 0) {
      *(int *)(&DAT_10117210 + local_90 * 4) = *(int *)(&DAT_10117210 + local_90 * 4) + 1;
      local_110[local_90] = 1;
      local_90 = local_90 + -1;
      local_110[local_90] = local_110[local_90] + -1;
      local_8 = local_8c[local_90];
      while ((-1 < local_110[0] && (local_110[local_90] < 0))) {
        local_110[local_90] = 1;
        local_90 = local_90 + -1;
        local_110[local_90] = local_110[local_90] + -1;
        local_8 = local_8c[local_90];
      }
    }
  } while (-1 < local_110[0]);
  return 0;
}

// FUNCTION: CARDARTLIB 0x10002BCE
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl FUN_10002bce(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  uint uVar1;
  int local_10;
  int local_c;
  int local_8;
  
  local_10 = 0;
  DAT_10021a88 = param_2;
  _DAT_10021a8c = param_2;
  DAT_10021a90 = param_3;
  DAT_1001d1dc = 0;
  local_8 = DAT_10021a94 + -1;
  do {
    do {
      uVar1 = BitStream_ReadBit();
      if (uVar1 == 0xffffffff) {
        return local_10;
      }
      if (uVar1 == 0) {
        local_c = *(int *)(&DAT_10021aa4 + local_8 * 8);
      }
      else {
        local_c = *(int *)(&DAT_10021aa0 + local_8 * 8);
      }
      local_8 = local_c - _DAT_10021a98;
    } while (-1 < local_8);
    if (local_c == 0) {
      uVar1 = BitStream_ReadBits(10);
      if ((int)uVar1 < 0) {
        return local_10;
      }
      memset(param_1,0,uVar1 << 2);
      param_1 = param_1 + uVar1;
      local_10 = local_10 + uVar1;
    }
    else {
      *param_1 = *(undefined4 *)(DAT_10021a9c + local_c * 4);
      param_1 = param_1 + 1;
      local_10 = local_10 + 1;
    }
    local_8 = DAT_10021a94 + -1;
  } while( true );
}

// FUNCTION: CARDARTLIB 0x10002CDE
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl Huffman13_DecodeDwordsWithZeroRuns(undefined8 *out_dwords,uint *bitstream,undefined4 bitstream_end)

{
  undefined8 *puVar1;
  int iVar2;
  uint uVar3;
  byte local_28;
  int local_14;
  uint local_c;
  int local_8;
  
  puVar1 = out_dwords;
  DAT_10021a88 = bitstream;
  _DAT_10021a8c = bitstream;
  DAT_10021a90 = bitstream_end;
  if (((uint)bitstream & 3) == 0) {
    DAT_1001d1dc = 0;
    DAT_10020e84 = 0;
  }
  else {
    iVar2 = 4 - ((uint)bitstream & 3);
    DAT_1001d1dc = iVar2 * 8;
    DAT_10020e84 = 0xffffffffU >> (0x20U - (char)DAT_1001d1dc & 0x1f) & *bitstream;
    DAT_10021a88 = (uint *)((int)bitstream + iVar2);
  }
  local_c = BitStream_ReadBits(8);
  while (local_c != 0xffffffff) {
    if (*(int *)(&DAT_10020e88 + local_c * 0xc) < 0x7fffffff) {
      uVar3 = *(uint *)(&DAT_10020e8c + local_c * 0xc);
      local_28 = (byte)uVar3;
      if (*(int *)(&DAT_10020e88 + local_c * 0xc) == -0x80000000) {
        uVar3 = BitStream_ReadBits(uVar3 + 2);
        if ((int)uVar3 < 0) break;
        uVar3 = uVar3 << (8 - local_28 & 0x1f) | (int)local_c >> (local_28 & 0x1f);
        MemZeroDwords(out_dwords,uVar3);
        out_dwords = (undefined8 *)((int)out_dwords + uVar3 * 4);
        local_c = BitStream_ReadBits(8);
      }
      else {
        *(undefined4 *)out_dwords = *(undefined4 *)(&DAT_10020e88 + local_c * 0xc);
        out_dwords = (undefined8 *)((int)out_dwords + 4);
        uVar3 = BitStream_ReadBits(uVar3);
        if (uVar3 == 0xffffffff) break;
        local_c = (int)local_c >> (local_28 & 0x1f) | uVar3 << (8 - local_28 & 0x1f);
      }
    }
    else {
      local_8 = *(int *)(&DAT_10020e90 + local_c * 0xc);
      do {
        uVar3 = BitStream_ReadBit();
        if (uVar3 == 0xffffffff) goto LAB_10002f46;
        if (uVar3 == 0) {
          local_14 = *(int *)(&DAT_10021aa4 + local_8 * 8);
        }
        else {
          local_14 = *(int *)(&DAT_10021aa0 + local_8 * 8);
        }
        local_8 = local_14 - _DAT_10021a98;
      } while (-1 < local_8);
      if (local_14 == 0) {
        uVar3 = BitStream_ReadBits(10);
        if (-1 < (int)uVar3) {
          MemZeroDwords(out_dwords,uVar3);
          out_dwords = (undefined8 *)((int)out_dwords + uVar3 * 4);
        }
      }
      else {
        *(undefined4 *)out_dwords = *(undefined4 *)(DAT_10021a9c + local_14 * 4);
        out_dwords = (undefined8 *)((int)out_dwords + 4);
      }
LAB_10002f46:
      local_c = BitStream_ReadBits(8);
    }
  }
  return (int)out_dwords - (int)puVar1 >> 2;
}

// FUNCTION: CARDARTLIB 0x10002F70
uint BitStream_ReadBits(uint bit_count)
{
  int iVar1;
  uint uVar2;
  uint uVar3;
  byte local_c;
  
  if (DAT_1001d1dc > bit_count) {
    uVar3 = *(uint *)(&DAT_10020e00 + (0x20 - bit_count) * 4) & DAT_10020e84;
    DAT_10020e84 = DAT_10020e84 >> ((byte)bit_count & 0x1f);
    DAT_1001d1dc = DAT_1001d1dc - bit_count;
  }
  else {
    iVar1 = bit_count - DAT_1001d1dc;
    if ((int)DAT_10021a88 + (4 - _DAT_10021a8c) < DAT_10021a90) {
      uVar2 = *DAT_10021a88;
      DAT_10021a88 = DAT_10021a88 + 1;
      local_c = (byte)DAT_1001d1dc;
      uVar3 = DAT_10020e84 |
              (*(uint *)(&DAT_10020e00 + (0x20 - iVar1) * 4) & uVar2) << (local_c & 0x1f);
      bit_count = (bit_count & 0xffffff00U) | (uint)(iVar1 & 0xff);
      DAT_10020e84 = uVar2 >> ((byte)bit_count & 0x1f);
      DAT_1001d1dc = 0x20 - iVar1;
    }
    else {
      uVar3 = 0xffffffff;
    }
  }
  return uVar3;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003060

static uint BitStream_ReadBit(void)
{
  uint result = 0;
  
  if (DAT_1001d1dc == 0) {
    if (DAT_10021a90 <= (int)DAT_10021a88 - _DAT_10021a8c) {
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

// FUNCTION: CARDARTLIB 0x100030F0
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID reserved)

{
  uint uVar1;
  
  (void)reserved;
  switch(reason) {
  case 0:
    CardArtLib_Shutdown();
    reason = 1;
    break;
  case 1:
    uVar1 = (uint)CardArtLib_Initialize(instance);
    reason = uVar1 & 1;
    break;
  case 2:
    reason = 1;
    break;
  case 3:
    reason = 1;
  }
  return (BOOL)reason;
}

// FUNCTION: CARDARTLIB 0x10003175
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

static BOOL CardArtLib_Initialize(HINSTANCE instance)

{
  char *pcVar1;
  uint auStackY_4a4a4 [66];
  size_t sStackY_4a39c;
  undefined1 auStackY_4a398 [4];
  int iStackY_4a394;
  FILE *pFStackY_4a390;
  undefined1 auStackY_4a38c [68];
  int aiStackY_4a348 [75978];
  int iVar2;
  
  MSVC_StackProbe();
  _DAT_10121fe4 = (undefined4)instance;
  InitializeCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  InitializeCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  InitializeCriticalSection((LPCRITICAL_SECTION)&DAT_101221d0);
  GetModuleFileNameA((HMODULE)0x0,&DAT_10117800,0x105);
  pcVar1 = strrchr(&DAT_10117800,0x5c);
  *pcVar1 = '\0';
  StrCopy(DAT_10117910,DAT_10117800);
  StrCat(DAT_10117910,s__CARDART_1001d1e0);
  InitCardArtGdiResources();
  StrCopy((char *)auStackY_4a4a4,DAT_10117800);
  StrCat((char *)auStackY_4a4a4,s__CARDS_DAT_1001d1ec);
  pFStackY_4a390 = fopen((char *)auStackY_4a4a4,&DAT_1001d1f8);
  if (pFStackY_4a390 == (FILE *)0x0) {
    for (iStackY_4a394 = 0; iStackY_4a394 < (int)sStackY_4a39c; iStackY_4a394 = iStackY_4a394 + 1) {
      DAT_101200a0[iStackY_4a394] = 1;
    }
  }
  else {
    fread(&sStackY_4a39c,4,1,pFStackY_4a390);
    fread(auStackY_4a398,4,1,pFStackY_4a390);
    fread(auStackY_4a38c,0x98,sStackY_4a39c,pFStackY_4a390);
    for (iStackY_4a394 = 0; iStackY_4a394 < (int)sStackY_4a39c; iStackY_4a394 = iStackY_4a394 + 1) {
      if (aiStackY_4a348[iStackY_4a394 * 0x26] < 1) {
        DAT_101200a0[iStackY_4a394] = 1;
      }
      else {
        DAT_101200a0[iStackY_4a394] = aiStackY_4a348[iStackY_4a394 * 0x26];
      }
    }
    fclose(pFStackY_4a390);
  }
  for (iVar2 = 0; iVar2 < 2000; iVar2 = iVar2 + 1) {
    *(undefined4 *)(DAT_10117a40 + iVar2 * 0x10) = 0;
  }
  for (iVar2 = 0; iVar2 < 100; iVar2 = iVar2 + 1) {
    *(undefined4 *)(g_versionedSmallArtCache + iVar2 * 0x18) = 0;
  }
  g_versionedSmallArtCount = 0;
  for (iVar2 = 0; iVar2 < 0x14; iVar2 = iVar2 + 1) {
    *(undefined4 *)(DAT_10121ff0 + iVar2 * 0x18) = 0;
  }
  DAT_101177f4 = 0;
  DAT_1001d258 = 3;
  return 1;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000343E
void CardArtLib_Shutdown(void)

{
  DestroyAllSmallArts();
  DestroyAllBigArts();
  ShutdownCardArtGdiResources();
  DeleteCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  DeleteCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  DeleteCriticalSection((LPCRITICAL_SECTION)&DAT_101221d0);
}

// FUNCTION: CARDARTLIB 0x10003480
int __cdecl LoadBigArt(int id,int version,int width,int height)

{
  HWND pHVar1;
  HDC hdc;
  int iVar2;
  int iVar3;
  int local_15c;
  uint local_154 [66];
  int local_4c;
  HBITMAP local_48;
  HDC local_44;
  int local_40;
  int *local_3c;
  void *local_38;
  uint *local_34;
  BITMAPINFO local_30;
  
                    /* 0x3480  10  LoadBigArt */
  local_40 = 1;
  if (id == -1) {
    local_40 = 0;
  }
  else {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
    local_4c = IsBigArtIn(id,version);
    if (local_4c != 0) {
      if ((*(int *)(local_4c + 8) == width) && (*(int *)(local_4c + 0xc) == height)) {
        LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
        return 1;
      }
      DestroyBigArt(id,version);
    }
    if ((*(int *)(&DAT_101200a0 + id * 4) < 2) || (version == 0)) {
      sprintf((char *)local_154,s__s__04d_WVL_1001d20c,&DAT_10117910,id);
    }
    else {
      sprintf((char *)local_154,s__s__04d_c_WVL_1001d1fc,&DAT_10117910,id,
               (int)(char)((char)version + '`'));
    }
    pHVar1 = GetDesktopWindow();
    hdc = GetDC(pHVar1);
    iVar2 = GetDeviceCaps(hdc,0xc);
    iVar3 = GetDeviceCaps(hdc,0xe);
    DAT_101221e8 = iVar2 * iVar3;
    pHVar1 = GetDesktopWindow();
    ReleaseDC(pHVar1,hdc);
    local_3c = Catalog_LoadWvlEntry(1,local_154,0);
    if (local_3c == (int *)0x0) {
      local_40 = 0;
    }
    else {
      local_44 = GetDC((HWND)0x0);
      ApplyCardArtPaletteToDc(local_44);
      InitBitmapInfo24bppTopDown(&local_30,width,height);
      local_48 = CreateDIBSection(local_44,&local_30,0,&local_38,(HANDLE)0x0,0);
      if (local_48 == (HBITMAP)0x0) {
        local_40 = 0;
      }
      else {
        local_34 = Wvl_DecodeToBgr24((uint *)0x0,local_3c,width,height);
        if (local_34 == (uint *)0x0) {
          local_40 = 0;
          DeleteObject(local_48);
        }
        else {
          if ((-width & 3U) == 0) {
            local_15c = 0;
          }
          else {
            local_15c = 4 - (-width & 3U);
          }
          memcpy(local_38,local_34,(width * 3 + local_15c) * height);
        }
      }
      ReleaseDC((HWND)0x0,local_44);
      Catalog_Unlock();
    }
    if (local_40 != 0) {
      if (0x13 < DAT_101177f4) {
        DestroyBigArt(DAT_10122000,DAT_10122004);
      }
      *(HBITMAP *)(&DAT_10121ff0 + DAT_101177f4 * 0x18) = local_48;
      *(void **)(&DAT_10121ff4 + DAT_101177f4 * 0x18) = local_38;
      *(int *)(&DAT_10121ff8 + DAT_101177f4 * 0x18) = width;
      *(int *)(&DAT_10121ffc + DAT_101177f4 * 0x18) = height;
      (&DAT_10122000)[DAT_101177f4 * 6] = id;
      (&DAT_10122004)[DAT_101177f4 * 6] = version;
      DAT_101177f4 = DAT_101177f4 + 1;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  }
  return local_40;
}

// FUNCTION: CARDARTLIB 0x100037BA
int __cdecl IsBigArtIn(int id,int version)

{
  undefined *local_c;
  int local_8;
  
                    /* 0x37ba  7  IsBigArtIn */
  local_c = (undefined *)0x0;
  if (id == -1) {
    local_c = (undefined *)0x0;
  }
  else {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
    local_8 = 0;
    while ((local_8 < DAT_101177f4 && (local_c == (undefined *)0x0))) {
      if (((&DAT_10122000)[local_8 * 6] == id) && ((&DAT_10122004)[local_8 * 6] == version)) {
        local_c = &DAT_10121ff0 + local_8 * 0x18;
      }
      local_8 = local_8 + 1;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  }
  return (int)local_c;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003867
int IsBigArtRightSize(int id,int version,int width,int height)

{
  int local_8 = 0;
  if (id == -1) {
    return 0;
  }

  EnterCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  local_8 = IsBigArtIn(id,version);
  if ((local_8 != 0) && ((*(int *)(local_8 + 8) != width || (*(int *)(local_8 + 0xc) != height))))
  {
    local_8 = 0;
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);

  return local_8;
}

// FUNCTION: CARDARTLIB 0x100038ED
int DrawBigArt(HDC hdc,RECT *rect,int id,int version)

{
  bool bVar1;
  HBRUSH hbr;
  int local_14;
  int local_10;
  int local_c;
  
                    /* 0x38ed  5  DrawBigArt */
  if (id == -1) {
    return 0;
  }

  EnterCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  local_c = 0;
  bVar1 = false;
  
  while ((local_c < DAT_101177f4 && (!bVar1))) {
    if (((&DAT_10122000)[local_c * 6] == id) && ((&DAT_10122004)[local_c * 6] == version)) {
      bVar1 = true;
      local_10 = local_c;
    }
    local_c = local_c + 1;
  }
  if (bVar1) {
    local_14 = DrawBitmapToRect(hdc,rect,*(HBITMAP *)(&DAT_10121ff0 + local_10 * 0x18));
  }
  else {
    local_14 = 0;
  }
  if (local_14 == 0) {
    hbr = GetStockObject(2);
    FillRect(hdc,rect,hbr);
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);

  return local_14;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100039EF
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

// FUNCTION: CARDARTLIB 0x10003A80
void __cdecl DestroyBigArt(int id,int version)

{
  bool bVar1;
  int local_10;
  int local_c;
  
  if (id == -1) {
    return;
  }

  EnterCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  local_c = 0;
  bVar1 = false;
  while ((local_c < DAT_101177f4 && (!bVar1))) {
    if (((&DAT_10122000)[local_c * 6] == id) && ((&DAT_10122004)[local_c * 6] == version)) {
      bVar1 = true;
      if (*(int *)(&DAT_10121ff0 + local_c * 0x18) != 0) {
        DeleteObject(*(HGDIOBJ *)(&DAT_10121ff0 + local_c * 0x18));
      }
      DAT_101177f4 = DAT_101177f4 + -1;
      for (local_10 = local_c; local_10 < DAT_101177f4; local_10 = local_10 + 1) {
        *(undefined4 *)(&DAT_10121ff0 + local_10 * 0x18) =
              *(undefined4 *)(&DAT_10121ff0 + (local_10 * 3 + 3) * 8);
        *(undefined4 *)(&DAT_10121ff4 + local_10 * 0x18) =
              *(undefined4 *)(&DAT_10121ff4 + (local_10 * 3 + 3) * 8);
        *(undefined4 *)(&DAT_10121ff8 + local_10 * 0x18) =
              *(undefined4 *)(&DAT_10121ff8 + (local_10 * 3 + 3) * 8);
        *(undefined4 *)(&DAT_10121ffc + local_10 * 0x18) =
              *(undefined4 *)(&DAT_10121ffc + (local_10 * 3 + 3) * 8);
        (&DAT_10122000)[local_10 * 6] = (&DAT_10122000)[(local_10 * 3 + 3) * 2];
        (&DAT_10122004)[local_10 * 6] = (&DAT_10122004)[(local_10 * 3 + 3) * 2];
      }
    }
    local_c = local_c + 1;
  }
  
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
}

// FUNCTION: CARDARTLIB 0x10003C0A
void DestroyAllBigArts(void)

{
  int local_8;
  
                    /* 0x3c0a  1  DestroyAllBigArts */
  EnterCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  for (local_8 = 0; local_8 < DAT_101177f4; local_8 = local_8 + 1) {
    DeleteObject(*(HGDIOBJ *)(&DAT_10121ff0 + local_8 * 0x18));
  }
  DAT_101177f4 = 0;
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  return;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003C70
void __cdecl InitBitmapInfo24bppTopDown(BITMAPINFO *bmi,int width,int height)

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

// FUNCTION: CARDARTLIB 0x10003CF0
int __cdecl LoadSmallArt(int id,int version,int width,int height)

{
  HWND pHVar1;
  HDC hdc;
  int iVar2;
  int iVar3;
  int local_158;
  uint local_150 [66];
  HBITMAP local_48;
  HDC local_44;
  int local_40;
  int *local_3c;
  void *local_38;
  uint *local_34;
  BITMAPINFO local_30;
  
                    /* 0x3cf0  11  LoadSmallArt */
  local_40 = 1;
  if (id == -1) {
    local_40 = 0;
  }
  else if (*(int *)(&DAT_101200a0 + id * 4) < 2) {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    if (*(int *)(&DAT_10117a40 + id * 0x10) != 0) {
      if ((*(int *)(&DAT_10117a48 + id * 0x10) == width) &&
         (*(int *)(&DAT_10117a4c + id * 0x10) == height)) {
        LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
        return 1;
      }
      DestroySmallArt(id,0);
    }
    pHVar1 = GetDesktopWindow();
    hdc = GetDC(pHVar1);
    iVar2 = GetDeviceCaps(hdc,0xc);
    iVar3 = GetDeviceCaps(hdc,0xe);
    DAT_101221e8 = iVar2 * iVar3;
    pHVar1 = GetDesktopWindow();
    ReleaseDC(pHVar1,hdc);
    sprintf((char *)local_150,s__s__04d_WVL_1001d218,&DAT_10117910,id);
    local_3c = Catalog_LoadWvlEntry(0,local_150,0);
    if (local_3c == (int *)0x0) {
      local_40 = 0;
    }
    else {
      local_44 = GetDC((HWND)0x0);
      ApplyCardArtPaletteToDc(local_44);
      InitBitmapInfo24bppTopDown(&local_30,width,height);
      local_48 = CreateDIBSection(local_44,&local_30,0,&local_38,(HANDLE)0x0,0);
      if (local_48 == (HBITMAP)0x0) {
        local_40 = 0;
      }
      else {
        local_34 = Wvl_DecodeToBgr24((uint *)0x0,local_3c,width,height);
        if (local_34 == (uint *)0x0) {
          local_40 = 0;
          DeleteObject(local_48);
        }
        else {
          if ((-width & 3U) == 0) {
            local_158 = 0;
          }
          else {
            local_158 = 4 - (-width & 3U);
          }
          memcpy(local_38,local_34,(width * 3 + local_158) * height);
        }
      }
      ReleaseDC((HWND)0x0,local_44);
      Catalog_Unlock();
    }
    if (local_40 != 0) {
      *(HBITMAP *)(&DAT_10117a40 + id * 0x10) = local_48;
      *(void **)(&DAT_10117a44 + id * 0x10) = local_38;
      *(int *)(&DAT_10117a48 + id * 0x10) = width;
      *(int *)(&DAT_10117a4c + id * 0x10) = height;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  }
  else {
    local_40 = LoadVersionedSmallArt(id,version,width,height);
  }
  return local_40;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003FB4
int IsSmallArtIn(int id,int version)
{
  if (id == -1) {
    return 0;
  }

  if (DAT_101200a0[id] > 1) {
    if (FindVersionedSmallArtCacheEntry(id,version) != (undefined *)0x0) {
      return 1;
    } else {
      return 0;
    }
  } else {  
    if (((HBITMAP *)DAT_10117a40)[id<<2] != 0) {
      return 1;
    } else {
      return 0;
    }
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000403D
int DrawSmallArt(HDC hdc,RECT *rect,int id,int version)
{
  int result;
  if ((hdc == (HDC)0x0) || (rect == (RECT *)0x0)) {
    return 0;
  }
  
  if (id == -1) {
    return 0;
  }
  
  if (DAT_101200a0[id] > 1) {
    DrawVersionedSmallArt(hdc,rect,id,version);
  }
  else {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    if (IsSmallArtIn(id,version) != 0) {
      result = DrawBitmapToRect(hdc,rect,((HBITMAP *)DAT_10117a40)[id<<2]);
    } else {
      result = 0;
    }
    if (result == 0) {
      FillRect(hdc,rect,GetStockObject(2));
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    
    return result;
  }
}

// FUNCTION: CARDARTLIB 0x10004128
undefined4 __cdecl ReloadSmallArtIfWrongSize(int id,int version,int width,int height)

{
  HGDIOBJ ho;
  int iVar1;
  undefined4 local_c;
  
                    /* 0x4128  13  ReloadSmallArtIfWrongSize */
  if (id == -1) {
    local_c = 0;
  }
  else if (*(int *)(&DAT_101200a0 + id * 4) < 2) {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    if (((*(int *)(&DAT_10117a40 + id * 0x10) == 0) ||
        (*(int *)(&DAT_10117a48 + id * 0x10) != width)) ||
       (*(int *)(&DAT_10117a4c + id * 0x10) != height)) {
      ho = *(HGDIOBJ *)(&DAT_10117a40 + id * 0x10);
      *(undefined4 *)(&DAT_10117a40 + id * 0x10) = 0;
      iVar1 = LoadSmallArt(id,version,width,height);
      if (iVar1 == 0) {
        *(HGDIOBJ *)(&DAT_10117a40 + id * 0x10) = ho;
        local_c = 0;
      }
      else {
        if (ho != (HGDIOBJ)0x0) {
          DeleteObject(ho);
        }
        local_c = 1;
      }
    }
    else {
      local_c = 1;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  }
  else {
    local_c = ReloadVersionedSmallArtIfWrongSize(id,version,width,height);
  }
  return local_c;
}

// FUNCTION: CARDARTLIB 0x10004251
void __cdecl DestroySmallArt(int id,int version)

{
                    /* 0x4251  4  DestroySmallArt */
  if (id != -1) {
    if (*(int *)(&DAT_101200a0 + id * 4) < 2) {
      EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
      if (*(int *)(&DAT_10117a40 + id * 0x10) != 0) {
        DeleteObject(*(HGDIOBJ *)(&DAT_10117a40 + id * 0x10));
        *(undefined4 *)(&DAT_10117a40 + id * 0x10) = 0;
      }
      LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    }
    else {
      DestroyVersionedSmallArt(id,version);
    }
  }
  return;
}

// FUNCTION: CARDARTLIB 0x100042DD
void DestroyAllSmallArts(void)

{
  int local_8;
  
                    /* 0x42dd  2  DestroyAllSmallArts */
  EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  DestroyAllVersionedSmallArts();
  for (local_8 = 0; local_8 < 2000; local_8 = local_8 + 1) {
    if (*(int *)(&DAT_101200a0 + local_8 * 4) < 2) {
      DestroySmallArt(local_8,0);
    }
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  return;
}

// FUNCTION: CARDARTLIB 0x10004346
int __cdecl LoadVersionedSmallArt(int id,int version,int width,int height)

{
  int local_158;
  uint local_154 [66];
  HBITMAP local_4c;
  HDC local_48;
  int local_44;
  int *local_40;
  void *local_3c;
  uint *local_38;
  BITMAPINFO local_34;
  undefined *local_8;
  
  local_44 = 1;
  if (id == -1) {
    local_44 = 0;
  }
  else {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    local_8 = FindVersionedSmallArtCacheEntry(id,version);
    if (local_8 != (undefined *)0x0) {
      if ((*(int *)(local_8 + 8) == width) && (*(int *)(local_8 + 0xc) == height)) {
        LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
        return 1;
      }
      DestroyVersionedSmallArt(id,version);
    }
    if (version == 0) {
      sprintf((char *)local_154,s__s__04d_WVL_1001d234,&DAT_10117910,id);
    }
    else {
      sprintf((char *)local_154,s__s__04d_c_WVL_1001d224,&DAT_10117910,id,
               (int)(char)((char)version + '`'));
    }
    local_40 = Catalog_LoadWvlEntry(0,local_154,0);
    if (local_40 == (int *)0x0) {
      local_44 = 0;
    }
    else {
      local_48 = GetDC((HWND)0x0);
      ApplyCardArtPaletteToDc(local_48);
      InitBitmapInfo24bppTopDown(&local_34,width,height);
      local_4c = CreateDIBSection(local_48,&local_34,0,&local_3c,(HANDLE)0x0,0);
      if (local_4c == (HBITMAP)0x0) {
        local_44 = 0;
      }
      else {
        local_38 = Wvl_DecodeToBgr24((uint *)0x0,local_40,width,height);
        if (local_38 == (uint *)0x0) {
          local_44 = 0;
          DeleteObject(local_4c);
        }
        else {
          if ((-width & 3U) == 0) {
            local_158 = 0;
          }
          else {
            local_158 = 4 - (-width & 3U);
          }
          memcpy(local_3c,local_38,(width * 3 + local_158) * height);
        }
      }
      ReleaseDC((HWND)0x0,local_48);
      Catalog_Unlock();
    }
    if (local_44 != 0) {
      *(HBITMAP *)(&g_versionedSmallArtCache + g_versionedSmallArtCount * 0x18) = local_4c;
      *(void **)(&DAT_1011f744 + g_versionedSmallArtCount * 0x18) = local_3c;
      *(int *)(&DAT_1011f748 + g_versionedSmallArtCount * 0x18) = width;
      *(int *)(&DAT_1011f74c + g_versionedSmallArtCount * 0x18) = height;
      *(int *)(&DAT_1011f750 + g_versionedSmallArtCount * 0x18) = id;
      *(int *)(&DAT_1011f754 + g_versionedSmallArtCount * 0x18) = version;
      g_versionedSmallArtCount = g_versionedSmallArtCount + 1;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  }
  return local_44;
}

// FUNCTION: CARDARTLIB 0x100045FE
undefined * __cdecl FindVersionedSmallArtCacheEntry(int id,int version)

{
  int local_c;
  undefined *local_8;
  
  local_8 = (undefined *)0x0;
  if (id == -1) {
    local_8 = (undefined *)0x0;
  }
  else {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    local_c = 0;
    while ((local_c < g_versionedSmallArtCount && (local_8 == (undefined *)0x0))) {
      if ((*(int *)(&DAT_1011f750 + local_c * 0x18) == id) &&
         (*(int *)(&DAT_1011f754 + local_c * 0x18) == version)) {
        local_8 = &g_versionedSmallArtCache + local_c * 0x18;
      }
      local_c = local_c + 1;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  }
  return local_8;
}

// FUNCTION: CARDARTLIB 0x100046AA
int __cdecl DrawVersionedSmallArt(HDC hdc,RECT *rect,int id,int version)

{
  bool bVar1;
  HBRUSH hbr;
  int local_14;
  int local_10;
  int local_c;
  
  if (id == -1) {
    local_14 = 0;
  }
  else {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    local_c = 0;
    bVar1 = false;
    while ((local_c < g_versionedSmallArtCount && (!bVar1))) {
      if ((*(int *)(&DAT_1011f750 + local_c * 0x18) == id) &&
         (*(int *)(&DAT_1011f754 + local_c * 0x18) == version)) {
        bVar1 = true;
        local_10 = local_c;
      }
      local_c = local_c + 1;
    }
    if (bVar1) {
      local_14 = DrawBitmapToRect(hdc,rect,*(HBITMAP *)(&g_versionedSmallArtCache + local_10 * 0x18));
    }
    else {
      local_14 = 0;
    }
    if (local_14 == 0) {
      hbr = GetStockObject(2);
      FillRect(hdc,rect,hbr);
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  }
  return local_14;
}

// FUNCTION: CARDARTLIB 0x100047AB
undefined4 __cdecl ReloadVersionedSmallArtIfWrongSize(int id,int version,int width,int height)

{
  undefined4 uVar1;
  undefined *puVar2;
  int iVar3;
  
  if (id == -1) {
    uVar1 = 0;
  }
  else {
    puVar2 = FindVersionedSmallArtCacheEntry(id,version);
    if (puVar2 != (undefined *)0x0) {
      if ((*(int *)(puVar2 + 8) == width) && (*(int *)(puVar2 + 0xc) == height)) {
        return 1;
      }
      DestroyVersionedSmallArt(id,version);
    }
    iVar3 = LoadVersionedSmallArt(id,version,width,height);
    if (iVar3 == 0) {
      uVar1 = 0;
    }
    else {
      uVar1 = 1;
    }
  }
  return uVar1;
}

// FUNCTION: CARDARTLIB 0x1000485A
void __cdecl DestroyVersionedSmallArt(int id,int version)

{
  bool bVar1;
  int local_10;
  int local_c;
  
  if (id != -1) {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    local_c = 0;
    bVar1 = false;
    while ((local_c < g_versionedSmallArtCount && (!bVar1))) {
      if ((*(int *)(&DAT_1011f750 + local_c * 0x18) == id) &&
         (*(int *)(&DAT_1011f754 + local_c * 0x18) == version)) {
        bVar1 = true;
        if (*(int *)(&g_versionedSmallArtCache + local_c * 0x18) != 0) {
          DeleteObject(*(HGDIOBJ *)(&g_versionedSmallArtCache + local_c * 0x18));
        }
        g_versionedSmallArtCount = g_versionedSmallArtCount + -1;
        for (local_10 = local_c; local_10 < g_versionedSmallArtCount; local_10 = local_10 + 1) {
          *(undefined4 *)(&g_versionedSmallArtCache + local_10 * 0x18) =
               *(undefined4 *)(&g_versionedSmallArtCache + (local_10 * 3 + 3) * 8);
          *(undefined4 *)(&DAT_1011f744 + local_10 * 0x18) =
               *(undefined4 *)(&DAT_1011f744 + (local_10 * 3 + 3) * 8);
          *(undefined4 *)(&DAT_1011f748 + local_10 * 0x18) =
               *(undefined4 *)(&DAT_1011f748 + (local_10 * 3 + 3) * 8);
          *(undefined4 *)(&DAT_1011f74c + local_10 * 0x18) =
               *(undefined4 *)(&DAT_1011f74c + (local_10 * 3 + 3) * 8);
          *(undefined4 *)(&DAT_1011f750 + local_10 * 0x18) =
               *(undefined4 *)(&DAT_1011f750 + (local_10 * 3 + 3) * 8);
          *(undefined4 *)(&DAT_1011f754 + local_10 * 0x18) =
               *(undefined4 *)(&DAT_1011f754 + (local_10 * 3 + 3) * 8);
        }
      }
      local_c = local_c + 1;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  }
  return;
}

// FUNCTION: CARDARTLIB 0x100049E4
void DestroyAllVersionedSmallArts(void)

{
  int local_8;
  
  EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  for (local_8 = 0; local_8 < g_versionedSmallArtCount; local_8 = local_8 + 1) {
    DeleteObject(*(HGDIOBJ *)(&g_versionedSmallArtCache + local_8 * 0x18));
  }
  g_versionedSmallArtCount = 0;
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  return;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10004A50
void * OctreeNode_Create(void)

{
  void *_Dst;
  
  _Dst = malloc(0x30);
  memset(_Dst,0,0x30);
  return _Dst;
}

// FUNCTION: CARDARTLIB 0x10004A83
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 * __cdecl TENTATIVE_read_palette(char *param_1,char *param_2)

{
  char local_120 [256];
  int local_20;
  int local_1c;
  FILE *local_18;
  char *local_14;
  int local_10;
  uint local_c;
  undefined2 *local_8;
  
  local_20 = 0;
  local_8 = &DAT_10116cf0;
  DAT_10116cf0 = 0x300;
  local_18 = fopen(param_1,DAT_1001e064);
  if (local_18 == (FILE *)0x0) {
    local_8 = (undefined2 *)0x0;
  }
  else {
    if (g_paletteOctreeRoot != (int *)0x0) {
      Octree_Destroy(g_paletteOctreeRoot);
    }
    g_paletteOctreeRoot = OctreeNode_Create();
    fgets(local_120,0xff,local_18);
    while ((local_18->_flag & 0x10) == 0) {
      sscanf(local_120,s__d____d__d__d_1001e068,&local_20,&local_10,&local_1c,&local_c);
      local_14 = strchr(local_120,0x2d);
      local_14 = local_14 + 1;
      local_14 = strchr(local_14,0x2d);
      local_14 = local_14 + 1;
      Octree_InsertPathString(g_paletteOctreeRoot,local_14,local_20);
      *(uint *)(g_paletteRgbTable + local_20 * 4) = local_10 << 0x10 | local_1c << 8 | local_c;
      *(undefined1 *)(local_8 + local_20 * 2 + 2) = (undefined1)local_10;
      *(undefined1 *)((int)local_8 + local_20 * 4 + 5) = (undefined1)local_1c;
      *(undefined1 *)(local_8 + local_20 * 2 + 3) = (undefined1)local_c;
      if ((local_20 == 0) || (local_20 == 0xff)) {
        *(undefined1 *)((int)local_8 + local_20 * 4 + 7) = 0;
      }
      else {
        *(undefined1 *)((int)local_8 + local_20 * 4 + 7) = 1;
      }
      fgets(local_120,0xff,local_18);
    }
    DAT_100322d4 = 0;
    DAT_1001d240 = 0;
    _DAT_10031eb8 = OctreeNode_FinalizeSubtree(g_paletteOctreeRoot);
    DAT_1001d240 = DAT_1001d240 + -1;
    local_8[1] = 0x100;
    fclose(local_18);
    local_18 = (FILE *)0x0;
    if (param_2 != (char *)0x0) {
      local_18 = fopen(param_2,DAT_1001e078);
    }
    if (local_18 != (FILE *)0x0) {
      fread(&DAT_10116cf0,0x404,1,local_18);
      fclose(local_18);
    }
    *(undefined1 *)((int)local_8 + 0x403) = 0;
    *(undefined1 *)((int)local_8 + 7) = *(undefined1 *)((int)local_8 + 0x403);
    InitOctreeBitTables();
    InitDiffSquaredLookupTable();
  }
  return local_8;
}

// FUNCTION: CARDARTLIB 0x10004CD7
undefined4 InitDiffSquaredLookupTable(void)

{
  undefined4 uVar1;
  int local_c;
  int local_8;
  
  if (DAT_1001d248 == 0) {
    local_c = -0xff;
    for (local_8 = 0; local_8 < 0x200; local_8 = local_8 + 1) {
      DAT_100ed310[local_8] = local_c * local_c;
      local_c = local_c + 1;
    }
    DAT_1001d248 = 1;
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

// FUNCTION: CARDARTLIB 0x10004D49
void __cdecl OctreeNode_CollectLeafIndices(int *param_1,int param_2,int *param_3)

{
  int local_8;
  
  if (*param_1 == 0) {
    for (local_8 = 0; local_8 < 8; local_8 = local_8 + 1) {
      if (param_1[local_8 + 2] != 0) {
        OctreeNode_CollectLeafIndices((int *)param_1[local_8 + 2],param_2,param_3);
      }
    }
  }
  else {
    *(char *)(*param_3 + param_2) = (char)param_1[1];
    *param_3 = *param_3 + 1;
  }
  return;
}

// FUNCTION: CARDARTLIB 0x10004DC8
int __cdecl OctreeNode_FinalizeSubtree(int *param_1)

{
  int iVar1;
  void *pvVar2;
  size_t local_410;
  int local_40c;
  int local_408;
  undefined1 local_404 [1024];
  
  iVar1 = DAT_100322d4;
  local_408 = 0;
  local_410 = 0;
  DAT_100322d4 = DAT_100322d4 + 1;
  if (DAT_1001d240 < DAT_100322d4) {
    DAT_1001d240 = DAT_100322d4;
  }
  if (*param_1 == 0) {
    for (local_40c = 0; local_40c < 8; local_40c = local_40c + 1) {
      if (param_1[local_40c + 2] != 0) {
        iVar1 = OctreeNode_FinalizeSubtree((int *)param_1[local_40c + 2]);
        local_408 = local_408 + iVar1;
        local_410 = local_410 + 1;
      }
    }
    if (local_410 != 0) {
      local_410 = 0;
      OctreeNode_CollectLeafIndices(param_1,(int)local_404,(int *)&local_410);
      pvVar2 = malloc(local_410);
      param_1[10] = (int)pvVar2;
      param_1[0xb] = local_410;
      memcpy((void *)param_1[10],local_404,local_410);
    }
    DAT_100322d4 = DAT_100322d4 + -1;
  }
  else {
    local_408 = 1;
    DAT_100322d4 = iVar1;
  }
  return local_408;
}

// FUNCTION: CARDARTLIB 0x10004F07
undefined4 __cdecl Octree_InsertPathString(undefined4 *param_1,char *param_2,undefined4 param_3)

{
  size_t sVar1;
  int iVar2;
  void *pvVar3;
  size_t sVar4;
  size_t sVar5;
  char *_Control;
  
  sVar1 = strspn(param_2,DAT_1001e07c);
  for (param_2 = param_2 + sVar1; *param_2 != '\0'; param_2 = param_2 + sVar1 + sVar4 + sVar5) {
    iVar2 = atoi(param_2);
    if (param_1[iVar2 + 2] == 0) {
      pvVar3 = OctreeNode_Create();
      param_1[iVar2 + 2] = pvVar3;
    }
    param_1 = (undefined4 *)param_1[iVar2 + 2];
    _Control = DAT_1001e084;
    sVar1 = strspn(param_2,DAT_1001e088);
    sVar1 = strcspn(param_2 + sVar1,_Control);
    sVar4 = strspn(param_2,DAT_1001e080);
    sVar5 = strspn(param_2 + sVar1 + sVar4,DAT_1001e08c);
  }
  *param_1 = 1;
  param_1[1] = param_3;
  return 0;
}

// FUNCTION: CARDARTLIB 0x10004FE1
int __cdecl Octree_Destroy(int *param_1)

{
  int iVar1;
  int local_c;
  int local_8;
  
  local_8 = 0;
  if (*param_1 == 0) {
    for (local_c = 0; local_c < 8; local_c = local_c + 1) {
      if (param_1[local_c + 2] != 0) {
        iVar1 = Octree_Destroy((int *)param_1[local_c + 2]);
        local_8 = local_8 + iVar1;
      }
    }
    if (param_1[10] != 0) {
      FreeIfNotNull((void *)param_1[10]);
    }
    FreeIfNotNull(param_1);
  }
  else {
    FreeIfNotNull(param_1);
    local_8 = 1;
  }
  return local_8;
}

// FUNCTION: CARDARTLIB 0x1000508D
void __cdecl Octree_BuildPathBytesFromRgb(uint rgb,uint *out_path_words)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  iVar1 = ((rgb & 0xff0000) >> 0x10) * 8;
  uVar4 = *(uint *)(PTR_DAT_1001d254 + iVar1 + 4);
  iVar2 = (rgb >> 8 & 0xff) * 8;
  uVar5 = *(uint *)(PTR_DAT_1001d250 + iVar2 + 4);
  iVar3 = (rgb & 0xff) * 8;
  uVar6 = *(uint *)(PTR_DAT_1001d24c + iVar3 + 4);
  *out_path_words = *(uint *)(PTR_DAT_1001d254 + iVar1) | *(uint *)(PTR_DAT_1001d250 + iVar2) |
             *(uint *)(PTR_DAT_1001d24c + iVar3);
  out_path_words[1] = uVar4 | uVar5 | uVar6;
  return;
}

// FUNCTION: CARDARTLIB 0x100050F1
undefined4 InitOctreeBitTables(void)

{
  uint local_10;
  int local_c;
  uint local_8;
  
  for (local_8 = 0; (int)local_8 < 0x100; local_8 = local_8 + 1) {
    local_10 = 0x80;
    for (local_c = 0; local_c < 8; local_c = local_c + 1) {
      if ((local_8 & local_10) == 0) {
        PTR_DAT_1001d24c[local_8 * 8 + local_c] = 0;
      }
      else {
        PTR_DAT_1001d24c[local_8 * 8 + local_c] = 4;
      }
      if ((local_8 & local_10) == 0) {
        PTR_DAT_1001d250[local_8 * 8 + local_c] = 0;
      }
      else {
        PTR_DAT_1001d250[local_8 * 8 + local_c] = 2;
      }
      if ((local_8 & local_10) == 0) {
        PTR_DAT_1001d254[local_8 * 8 + local_c] = 0;
      }
      else {
        PTR_DAT_1001d254[local_8 * 8 + local_c] = 1;
      }
      local_10 = (int)local_10 >> 1;
    }
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x100051F5
undefined4 __cdecl Octree_FindNearestColor(uint param_1)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  byte *pbVar5;
  int local_34;
  uint local_2c;
  int local_28;
  
  Octree_BuildPathBytesFromRgb(param_1,(uint *)&DAT_100322c0);
  pbVar5 = &DAT_100322c0;
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
                          ((param_1 >> 8 & 0xff) -
                          (uint)g_paletteRgbTable[(uint)*(byte *)(local_28 + iVar2) * 4 + 1]) * 4) +
                  *(int *)(PTR_DAT_1001d244 +
                          ((param_1 & 0xff) -
                          (*(uint *)(g_paletteRgbTable + (uint)*(byte *)(local_28 + iVar2) * 4) & 0xff)) *
                          4) +
                  *(int *)(PTR_DAT_1001d244 +
                          (((param_1 & 0xff0000) >> 0x10) -
                          ((*(uint *)(g_paletteRgbTable + (uint)*(byte *)(local_28 + iVar2) * 4) & 0xff0000)
                           >> 0x10)) * 4);
          if (iVar4 < local_34) {
            local_2c = (uint)*(byte *)(local_28 + iVar2);
            local_34 = iVar4;
          }
        }
        return *(undefined4 *)(g_paletteRgbTable + local_2c * 4);
      }
      return *(undefined4 *)(g_paletteRgbTable + piVar3[1] * 4);
    }
    piVar3 = piVar1;
  } while ((char)*piVar1 != '\x01');
  return *(undefined4 *)(g_paletteRgbTable + piVar1[1] * 4);
}

// FUNCTION: CARDARTLIB 0x10005383
uint __cdecl Octree_FindNearestPaletteIndex(uint param_1)

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

// FUNCTION: CARDARTLIB 0x100054FB
int __cdecl Octree_FlattenLeafValues(int *param_1,int *param_2)

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
undefined4 __cdecl QuantizeBgr24ToNearestPaletteColorInPlace(uint *bgr24,int height,int width,int row_padding)

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
undefined4 __cdecl QuantizeBgr24ToPaletteIndicesInPlace(uint *bgr24,int height,int width,int row_padding)

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

// FUNCTION: CARDARTLIB 0x1000570F
/* WARNING: Type propagation algorithm not settling */

int __cdecl DitherBgr24ToPaletteColors(int dither_kernel_id,int serpentine,uint *bgr24,int height,int width,int row_padding)

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

// FUNCTION: CARDARTLIB 0x10005CB4
void __cdecl RotateDwordsLeft1(undefined4 *param_1,int param_2)

{
  undefined4 uVar1;
  
  uVar1 = *param_1;
  memcpy(param_1,param_1 + 1,param_2 * 4 - 4);
  param_1[param_2 + -1] = uVar1;
  return;
}

// FUNCTION: CARDARTLIB 0x10005CF5
undefined4 __cdecl InitErrorDiffusionDeltaTables(int dither_kernel_id,int delta_table_ptrs_base)

{
  int iVar1;
  int iVar2;
  void *pvVar3;
  int local_14;
  int local_10;
  
  iVar1 = DAT_1001d2b0[dither_kernel_id];
  for (local_10 = 0; local_10 < DAT_1001d260[dither_kernel_id]; local_10 = local_10 + 1) {
    iVar2 = *(int *)(&DAT_1001d2d8 + local_10 * 0x10 + dither_kernel_id * 0xc0);
    if (*(int *)(delta_table_ptrs_base + iVar2 * 4) == 0) {
      pvVar3 = malloc(0x800);
      *(void **)(delta_table_ptrs_base + iVar2 * 4) = pvVar3;
      assert((uint)(*(int *)(delta_table_ptrs_base + iVar2 * 4) != 0),s_D__Newmagic_sources_NedCard_Pale_1001e0b4,
             0x4fd,s_Not_enough_memory_for_delta_arra_1001e090);
      for (local_14 = -0x100; local_14 < 0x100; local_14 = local_14 + 1) {
        *(int *)(*(int *)(delta_table_ptrs_base + iVar2 * 4) + 0x400 + local_14 * 4) =
             ((iVar2 * local_14 + (iVar1 >> 1)) * 0x100) / DAT_1001d2b0[dither_kernel_id];
      }
      *(int *)(&DAT_1001d2e4 + local_10 * 0x10 + dither_kernel_id * 0xc0) =
           *(int *)(delta_table_ptrs_base + iVar2 * 4) + 0x3fc;
    }
    else {
      *(int *)(&DAT_1001d2e4 + local_10 * 0x10 + dither_kernel_id * 0xc0) =
           *(int *)(delta_table_ptrs_base + iVar2 * 4) + 0x400;
    }
  }
  for (local_10 = 0; local_10 < DAT_1001d260[dither_kernel_id]; local_10 = local_10 + 1) {
    *(int *)(&DAT_1001d9a4 + local_10 * 0x10 + dither_kernel_id * 0xc0) =
         *(int *)(delta_table_ptrs_base +
                 *(int *)(&DAT_1001d998 + local_10 * 0x10 + dither_kernel_id * 0xc0) * 4) + 0x3fc;
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x10005EA2
undefined4 __cdecl FUN_10005ea2(int param_1,byte *param_2,int param_3,int param_4,int param_5)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  byte bVar5;
  byte bVar6;
  byte bVar7;
  byte bVar8;
  byte bVar9;
  byte bVar10;
  byte bVar11;
  byte bVar12;
  byte bVar13;
  byte bVar14;
  byte bVar15;
  byte bVar16;
  byte bVar17;
  byte bVar18;
  size_t _Size;
  byte *pbVar19;
  byte *pbVar20;
  byte *_Dst;
  void *_Dst_00;
  byte *local_1c;
  int local_18;
  int local_10;
  
  _Dst = param_2;
  _Size = param_3 * 3;
  _Dst_00 = malloc((_Size + param_5) * param_4 + 0x10);
  if (DAT_10031ea8 == 0) {
    for (local_10 = -0x200; local_10 < 0x200; local_10 = local_10 + 1) {
      if ((local_10 < 0) || (0xff < local_10)) {
        if (local_10 < 0) {
          PTR_DAT_1001e058[local_10] = 0;
        }
        else {
          PTR_DAT_1001e058[local_10] = 0xff;
        }
      }
      else {
        PTR_DAT_1001e058[local_10] = (undefined1)local_10;
      }
    }
    DAT_10031ea8 = 1;
  }
  memcpy(_Dst_00,param_2,_Size);
  param_2 = param_2 + _Size + param_5;
  local_1c = (byte *)((int)_Dst_00 + _Size + param_5);
  for (local_18 = 1; local_18 < param_4 + -1; local_18 = local_18 + 1) {
    *local_1c = *param_2;
    local_1c[1] = param_2[1];
    local_1c[2] = param_2[2];
    pbVar19 = param_2;
    pbVar20 = local_1c;
    for (local_10 = 1; local_1c = pbVar20 + 3, param_2 = pbVar19 + 3, local_10 < param_3 + -1;
        local_10 = local_10 + 1) {
      bVar1 = param_2[(param_3 * -3 - param_5) + -2];
      bVar2 = param_2[(param_3 * -3 - param_5) + 1];
      bVar3 = param_2[(param_3 * -3 - param_5) + 4];
      bVar4 = pbVar19[1];
      bVar5 = pbVar19[4];
      bVar6 = pbVar19[7];
      bVar7 = param_2[_Size + param_5 + -2];
      bVar8 = param_2[_Size + param_5 + 1];
      bVar9 = param_2[_Size + param_5 + 4];
      bVar10 = param_2[(param_3 * -3 - param_5) + -1];
      bVar11 = param_2[(param_3 * -3 - param_5) + 2];
      bVar12 = param_2[(param_3 * -3 - param_5) + 5];
      bVar13 = pbVar19[2];
      bVar14 = pbVar19[5];
      bVar15 = pbVar19[8];
      bVar16 = param_2[_Size + param_5 + -1];
      bVar17 = param_2[_Size + param_5 + 2];
      bVar18 = param_2[_Size + param_5 + 5];
      *local_1c = PTR_DAT_1001e058
                  [(int)(((((((uint)param_2[param_3 * -3 - param_5] * -2 -
                             (uint)param_2[(param_3 * -3 - param_5) + -3]) -
                            (uint)param_2[(param_3 * -3 - param_5) + 3]) + (uint)*pbVar19 * -2 +
                            (uint)*param_2 * param_1 + (uint)pbVar19[6] * -2) -
                          (uint)param_2[_Size + param_5 + -3]) + (uint)param_2[_Size + param_5] * -2
                         ) - (uint)param_2[_Size + param_5 + 3]) / (param_1 + -0xc)];
      pbVar20[4] = PTR_DAT_1001e058
                   [(int)(((((((uint)bVar2 * -2 - (uint)bVar1) - (uint)bVar3) + (uint)bVar4 * -2 +
                             (uint)bVar5 * param_1 + (uint)bVar6 * -2) - (uint)bVar7) +
                          (uint)bVar8 * -2) - (uint)bVar9) / (param_1 + -0xc)];
      pbVar20[5] = PTR_DAT_1001e058
                   [(int)(((((((uint)bVar11 * -2 - (uint)bVar10) - (uint)bVar12) + (uint)bVar13 * -2
                             + (uint)bVar14 * param_1 + (uint)bVar15 * -2) - (uint)bVar16) +
                          (uint)bVar17 * -2) - (uint)bVar18) / (param_1 + -0xc)];
      pbVar19 = param_2;
      pbVar20 = local_1c;
    }
    *local_1c = *param_2;
    pbVar20[4] = pbVar19[4];
    pbVar20[5] = pbVar19[5];
    param_2 = pbVar19 + param_5 + 6;
    local_1c = pbVar20 + param_5 + 6;
  }
  memcpy(local_1c,param_2,_Size);
  memcpy(_Dst,_Dst_00,(_Size + param_5) * param_4);
  FreeIfNotNull(_Dst_00);
  return 0;
}

// FUNCTION: CARDARTLIB 0x100062C7
undefined4 __cdecl
DitherBgr24ToRgbQuantizedF8(int dither_kernel_id,int serpentine,uint *bgr24,int height,int width,int row_padding)

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
    InitErrorDiffusionDeltaTables(dither_kernel_id,0x10117100);
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

// FUNCTION: CARDARTLIB 0x10006809
void DestroyPaletteOctree(void)

{
  Octree_Destroy(g_paletteOctreeRoot);
  g_paletteOctreeRoot = (int *)0x0;
  return;
}

// FUNCTION: CARDARTLIB 0x1000682C
int __cdecl Palette_FindNearestEntryIndex(int param_1,int param_2,int param_3,byte *param_4)

{
  int iVar1;
  int local_14;
  int local_10;
  int local_c;
  
  local_14 = 0x7fffffff;
  for (local_c = 0; local_c < 0x100; local_c = local_c + 1) {
    iVar1 = *(int *)(PTR_DAT_1001d244 + (param_3 - (uint)param_4[2]) * 4) +
            *(int *)(PTR_DAT_1001d244 + (param_2 - (uint)param_4[1]) * 4) +
            *(int *)(PTR_DAT_1001d244 + (param_1 - (uint)*param_4) * 4);
    if (iVar1 < local_14) {
      local_10 = local_c;
      local_14 = iVar1;
    }
    param_4 = param_4 + 3;
  }
  return local_10;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100068D0
uint __cdecl Rgb888_QuantizeToF8(uint param_1)

{
  return param_1 & 0xf8f8f8;
}

// FUNCTION: CARDARTLIB 0x100068F0
int * __cdecl Catalog_LoadWvlEntry(int param_1,uint *param_2,int param_3)

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
  EnterCriticalSection((LPCRITICAL_SECTION)&DAT_101221d0);
  _splitpath((char *)param_2,(char *)0x0,(char *)local_314,(char *)local_210,(char *)local_110);
  if (DAT_10032adc == 0) {
    StrCopy((char *)local_41c,(char *)local_314);
    StrCat((char *)local_41c,s_SmallArt_cat_1001e12c);
    DAT_1001e11c = Catalog_Open((char *)local_41c);
    StrCopy((char *)local_41c,(char *)local_314);
    StrCat((char *)local_41c,s_MedArt_cat_1001e13c);
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
  StrCopy((char *)local_314,(char *)local_210);
  StrCat((char *)local_314,(char *)local_110);
  _strlwr((char *)local_314);
  if (local_c != (int *)0x0) {
    memset(local_c,0,0x1b0);
    StrCopy((char *)(local_c + 0x27),(const char *)param_2);
    local_c[0x68] = (int)&DAT_100ad498;
    local_10 = Catalog_ReadEntry(DAT_100ea098,(char *)local_314,(void **)(local_c + 0x68));
    if (local_10 == 0xffffffff) {
      StrCat((char *)param_2,DAT_1001e148);
      OutputDebugStringA((LPCSTR)param_2);
      LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221d0);
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
          Catalog_Unlock();
          LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221d0);
          local_c = (int *)0x0;
        }
        else {
          local_c[0x6a] = 1;
          LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221d0);
        }
      }
    }
  }
  return local_c;
}

// FUNCTION: CARDARTLIB 0x10006BB5
undefined4 Catalog_Unlock(void)

{
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221d0);
  return 0;
}

// FUNCTION: CARDARTLIB 0x10006BE3
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 * __cdecl Wvl_DecodeHaar(int *param_1,undefined8 *param_2)

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
void __cdecl
CopyBgr24RectIntoStridedBuffer(undefined8 *dst_bgr24,undefined8 *src_bgr24,int dst_x,int dst_y,int rect_width,
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

// FUNCTION: CARDARTLIB 0x1000709C
void __cdecl
FUN_1000709c(int *param_1,int *param_2,int *param_3,int param_4,int param_5,undefined4 param_6,
            int param_7)

{
  int *piVar1;
  int *piVar2;
  int local_20;
  int local_1c;
  int *local_c;
  int local_8;
  
  for (local_8 = 0; piVar1 = param_1, local_8 < param_5; local_8 = local_8 + 1) {
    piVar2 = param_1 + param_4 + -1;
    local_c = param_3 + param_7 * 2;
    for (; param_1 < piVar2; param_1 = param_1 + 1) {
      local_1c = param_1[1] * 0xb504;
      local_20 = param_1[1] * 0xb504;
      if (*param_2 != 0) {
        local_1c = local_1c + *param_2 * 0xb504;
        local_20 = local_20 + *param_2 * -0xb504;
      }
      *local_c = local_1c >> 0x10;
      local_c[param_7] = local_20 >> 0x10;
      param_2 = param_2 + 1;
      local_c = local_c + param_7 * 2;
    }
    *param_3 = *param_2 * 0xb504 + *piVar1 * 0xb504 >> 0x10;
    param_3[param_7] = *param_2 * -0xb504 + *piVar1 * 0xb504 >> 0x10;
    param_3 = param_3 + 1;
    param_1 = param_1 + 1;
    param_2 = param_2 + 1;
  }
  return;
}

// FUNCTION: CARDARTLIB 0x10007238
void __cdecl CopyBytes(void *dst,const void *src,size_t size)

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
void __cdecl SetBytes(void *dst,uint value,size_t size)

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

// FUNCTION: CARDARTLIB 0x100072DC
void __cdecl Haar2D_ReconstructInPlace(int *coeffs,int full_size,int base_size)

{
  int *piVar1;
  int *local_18;
  int local_10;
  
  if (g_haarScratchInit == 0) {
    local_18 = malloc(0x32000);
    g_haarScratchA = local_18;
    g_haarScratchB = malloc(0x32000);
    g_haarScratchInit = 1;
  }
  else {
    local_18 = g_haarScratchA;
  }
  piVar1 = g_haarScratchB;
  for (local_10 = base_size; local_10 < full_size; local_10 = local_10 << 1) {
    Haar_CombineSumDiff(coeffs,coeffs + local_10 * local_10,local_18,local_10,local_10,local_10 * 2,local_10);
    Haar_CombineSumDiff(coeffs + local_10 * local_10 * 2,coeffs + local_10 * local_10 * 3,piVar1,local_10,
                        local_10,local_10 * 2,local_10);
    Haar_CombineSumDiffHalf(local_18,piVar1,coeffs,local_10,local_10 * 2,local_10 * 2,local_10 * 2);
  }
  return;
}

// FUNCTION: CARDARTLIB 0x10007419
void __cdecl
Haar_CombineSumDiff(int *param_1,int *param_2,int *param_3,int param_4,int param_5,undefined4 param_6,int param_7)

{
  int *piVar1;
  int *piVar2;
  int *local_c;
  int local_8;
  
  for (local_8 = 0; piVar1 = param_1, local_8 < param_5; local_8 = local_8 + 1) {
    piVar2 = param_1 + param_4;
    local_c = param_3 + param_7 * 2;
    while (param_1 = param_1 + 1, param_1 < piVar2) {
      *local_c = *param_2 + *param_1;
      local_c[param_7] = *param_1 - *param_2;
      param_2 = param_2 + 1;
      local_c = local_c + param_7 * 2;
    }
    *param_3 = *param_2 + *piVar1;
    param_3[param_7] = *piVar1 - *param_2;
    param_3 = param_3 + 1;
    param_2 = param_2 + 1;
  }
  return;
}

// FUNCTION: CARDARTLIB 0x100074EE
void __cdecl
Haar_CombineSumDiffHalf(int *param_1,int *param_2,int *param_3,int param_4,int param_5,undefined4 param_6,int param_7)

{
  int *piVar1;
  int *piVar2;
  int *local_c;
  int local_8;
  
  for (local_8 = 0; piVar1 = param_1, local_8 < param_5; local_8 = local_8 + 1) {
    piVar2 = param_1 + param_4;
    local_c = param_3 + param_7 * 2;
    while (param_1 = param_1 + 1, param_1 < piVar2) {
      *local_c = *param_1 + *param_2 >> 1;
      local_c[param_7] = *param_1 - *param_2 >> 1;
      param_2 = param_2 + 1;
      local_c = local_c + param_7 * 2;
    }
    *param_3 = *param_2 + *piVar1 >> 1;
    param_3[param_7] = *piVar1 - *param_2 >> 1;
    param_3 = param_3 + 1;
    param_2 = param_2 + 1;
  }
  return;
}

// FUNCTION: CARDARTLIB 0x100075C9
undefined2 __cdecl FUN_100075c9(uint param_1,uint param_2)

{
  undefined1 *puVar1;
  undefined1 uVar2;
  uint uVar3;
  undefined1 *puVar4;
  
  puVar4 = (undefined1 *)(param_1 & 0xffffffe0);
  uVar3 = param_2 >> 6;
  do {
    uVar2 = *puVar4;
    puVar1 = puVar4 + 0x20;
    puVar4 = puVar4 + 0x40;
    uVar3 = uVar3 - 1;
  } while (uVar3 != 0);
  return CONCAT11(*puVar1,uVar2);
}

// FUNCTION: CARDARTLIB 0x100075EF
undefined1 * __cdecl
YuvPlanesToBgr24(undefined1 *out_bgr24,int *luma,int width,int height,int chroma_u,int chroma_v,
                 int chroma_stride,undefined4 unused_chroma_height,int chroma_is_420)

{
  undefined1 *puVar1;
  int iVar2;
  int local_2c;
  int local_28;
  int local_20;
  uint local_1c;
  int *local_14;
  int *local_10;
  int local_c;
  int local_8;
  
  if (g_yuvClampTableInit == 0) {
    for (local_1c = -0x400; (int)local_1c < 0x1c00; local_1c = local_1c + 1) {
      if ((int)local_1c < 1) {
        g_yuvClampTable[local_1c] = 0;
      }
      else {
        iVar2 = (int)local_1c >> 2;
        if (0xfe < iVar2) {
          iVar2 = 0xff;
        }
        g_yuvClampTable[local_1c] = (char)iVar2;
      }
    }
    g_yuvClampTableInit = 1;
  }
  if (out_bgr24 == (undefined1 *)0x0) {
    out_bgr24 = malloc(width * width * 3 + 0x10);
  }
  puVar1 = out_bgr24;
  for (local_20 = 0; local_20 < height; local_20 = local_20 + 1) {
    iVar2 = local_20;
    if (chroma_is_420 != 0) {
      iVar2 = local_20 / 2;
    }
    local_10 = (int *)(iVar2 * chroma_stride * 4 + chroma_v);
    local_14 = (int *)(iVar2 * chroma_stride * 4 + chroma_u);
    for (local_1c = 0; (int)local_1c < width; local_1c = local_1c + 1) {
      iVar2 = *luma;
      if (chroma_is_420 == 0) {
        local_2c = *local_14;
        local_28 = *local_10;
        local_28 = (local_28 >> 3) + (local_28 >> 1) + local_28;
      }
      else {
        if ((local_1c & 1) == 0) {
          local_2c = *local_14;
          local_28 = *local_10;
        }
        else {
          local_2c = (local_14[width - 1U != local_1c] + *local_14) / 2;
          local_28 = (local_10[width - 1U != local_1c] + *local_10) / 2;
        }
        local_28 = (local_28 >> 3) + (local_28 >> 1) + local_28;
      }
      local_8 = local_2c * 2 + -0x400 + iVar2;
      local_c = local_28 + -0x333 + iVar2;
      *out_bgr24 = g_yuvClampTable[local_8];
      out_bgr24[1] = g_yuvClampTable
                   [((iVar2 * 2 - (iVar2 >> 2)) - (local_c >> 1)) -
                     ((local_8 >> 2) - (local_8 >> 4))];
      out_bgr24[2] = g_yuvClampTable[local_c];
      if (chroma_is_420 == 0) {
        local_14 = local_14 + 1;
        local_10 = local_10 + 1;
      }
      else if ((local_1c & 1) != 0) {
        local_14 = local_14 + 1;
        local_10 = local_10 + 1;
      }
      luma = luma + 1;
      out_bgr24 = out_bgr24 + 3;
    }
  }
  return puVar1;
}

// FUNCTION: CARDARTLIB 0x100078B8
undefined4 __cdecl Wvl_UnpackPieces(int param_1,int *param_2)

{
  int *piVar1;
  uint *puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  void *pvVar8;
  void *_Dst;
  void *_Dst_00;
  int local_28;
  void *local_18;
  
  iVar5 = param_2[7] / (int)(2 - (uint)(param_2[10] == 1));
  iVar6 = iVar5 / (int)(2 - (uint)(*param_2 == 0));
  iVar3 = param_2[9];
  iVar4 = *(int *)param_2[0x68];
  piVar1 = (int *)param_2[0x68] + 1;
  *piVar1 = -0x80000000;
  iVar7 = Huffman13_Init(piVar1 + iVar4,piVar1,iVar4);
  local_18 = (void *)((int)(piVar1 + iVar4) + iVar7);
  for (local_28 = 0; local_28 < param_2[10]; local_28 = local_28 + 1) {
    pvVar8 = (void *)((iVar5 * iVar5 + iVar6 * iVar6 * 2 + 0x40) * local_28 * 4 + param_1);
    _Dst = (void *)((int)pvVar8 + iVar5 * iVar5 * 4 + 0x80);
    _Dst_00 = (void *)((int)_Dst + iVar6 * iVar6 * 4 + 0x80);
    memcpy(pvVar8,local_18,iVar3 * iVar3 * 4);
    puVar2 = (uint *)((int)local_18 + iVar3 * iVar3 * 4);
    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)((int)pvVar8 + iVar3 * iVar3 * 4),puVar2,param_2[local_28 + 0x17]);
    pvVar8 = (void *)((int)puVar2 + param_2[local_28 + 0x17]);
    memcpy(_Dst,pvVar8,iVar3 * iVar3 * 4);
    puVar2 = (uint *)((int)pvVar8 + iVar3 * iVar3 * 4);
    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)((int)_Dst + iVar3 * iVar3 * 4),puVar2,param_2[local_28 + 0x1b]);
    pvVar8 = (void *)((int)puVar2 + param_2[local_28 + 0x1b]);
    memcpy(_Dst_00,pvVar8,iVar3 * iVar3 * 4);
    puVar2 = (uint *)((int)pvVar8 + iVar3 * iVar3 * 4);
    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)((int)_Dst_00 + iVar3 * iVar3 * 4),puVar2,param_2[local_28 + 0x1f]);
    local_18 = (void *)((int)puVar2 + param_2[local_28 + 0x1f]);
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x10007AEF
bool __cdecl ClearWindowClientWhite(HWND hwnd)

{
  HDC hDC;
  HBRUSH hbr;
  RECT local_14;
  
  if (hwnd != (HWND)0x0) {
    hDC = GetDC(hwnd);
    GetClientRect(hwnd,&local_14);
    hbr = GetStockObject(0);
    FillRect(hDC,&local_14,hbr);
    ReleaseDC(hwnd,hDC);
  }
  return hwnd != (HWND)0x0;
}

// FUNCTION: CARDARTLIB 0x10007B58
int __cdecl
BlitBgr24ToHwnd(HWND hwnd,const void *bgr24,int x,int y,DWORD width,DWORD height)

{
  BITMAPINFO *lpbmi;
  HDC hdc;
  int iVar1;
  
  lpbmi = AllocBitmapInfoTopDown((int)width,(int)height,0x18);
  hdc = GetDC(hwnd);
  iVar1 = SetDIBitsToDevice(hdc,x,y,width,height,0,0,0,height,(void *)bgr24,lpbmi,0);
  ReleaseDC(hwnd,hdc);
  FreeBitmapInfo(lpbmi);
  return iVar1;
}

// FUNCTION: CARDARTLIB 0x10007BDB
BITMAPINFO * __cdecl AllocBitmapInfoTopDown(int width,int height,int bits_per_pixel)

{
  int local_10;
  undefined4 *local_c;
  BITMAPINFO *local_8;
  
  if (bits_per_pixel == 8) {
    local_8 = malloc(0x42c);
  }
  else if (bits_per_pixel == 0x18) {
    local_8 = malloc(0x2c);
  }
  else {
    local_8 = malloc(0x2c);
  }
  local_8->bmiHeader.biSize = 0x28;
  local_8->bmiHeader.biWidth = width;
  local_8->bmiHeader.biHeight = -height;
  local_8->bmiHeader.biPlanes = 1;
  local_8->bmiHeader.biBitCount = (short)bits_per_pixel;
  local_8->bmiHeader.biCompression = 0;
  local_8->bmiHeader.biSizeImage = 0;
  local_8->bmiHeader.biXPelsPerMeter = 0;
  local_8->bmiHeader.biYPelsPerMeter = 0;
  if (bits_per_pixel == 8) {
    local_8->bmiHeader.biClrUsed = 0x100;
    local_8->bmiHeader.biClrImportant = 0x100;
    local_c = (undefined4 *)&local_8->bmiColors[0];
    for (local_10 = 0; local_10 < 0x100; local_10 = local_10 + 1) {
      *(short *)local_c = (short)local_10;
      local_c = (undefined4 *)((int)local_c + 2);
    }
  }
  else {
    local_8->bmiHeader.biClrUsed = 0;
    local_8->bmiHeader.biClrImportant = 0;
  }
  return local_8;
}

// FUNCTION: CARDARTLIB 0x10007CFF
bool __cdecl FreeBitmapInfo(void *ptr)

{
  FreeIfNotNull(ptr);
  return true;
}

// FUNCTION: CARDARTLIB 0x10007D20
/* WARNING: Removing unreachable block (ram,0x10007f81) */
/* WARNING: Removing unreachable block (ram,0x10007fbb) */
/* WARNING: Removing unreachable block (ram,0x10007f8b) */
/* WARNING: Removing unreachable block (ram,0x10007e82) */
/* WARNING: Removing unreachable block (ram,0x10007e9f) */
/* WARNING: Removing unreachable block (ram,0x10007e8c) */

unsigned char * __cdecl Wvl_DecodeToBgr24BufferScaled(unsigned char *param_1,int *param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  int *piVar5;
  undefined8 *puVar6;
  int iVar7;
  undefined8 *local_40;
  int local_3c;
  int local_38;
  int local_34;
  int local_2c;
  int *local_28;
  undefined8 *local_24;
  
  local_38 = 0;
  local_24 = (undefined8 *)0x0;
  if (param_2 == (int *)0x0) {
    puVar4 = (undefined4 *)0x0;
  }
  else {
    iVar7 = (DAT_1001e0e0 - (param_3 * 3) % DAT_1001e0e0) % DAT_1001e0e0;
    if (param_1 == (undefined4 *)0x0) {
      param_1 = malloc((param_3 * 3 + iVar7) * param_4 + 4);
    }
    puVar4 = param_1;
    iVar1 = param_2[7];
    iVar2 = param_2[8];
    if (param_2[0x6a] == 0) {
      local_40 = Wvl_DecodeHaar(param_2,(undefined8 *)0x0);
    }
    else {
      local_40 = (undefined8 *)param_2[0x6b];
    }
    iVar3 = param_2[7];
    piVar5 = malloc(param_3 << 2);
    local_2c = 0;
    local_28 = piVar5;
    for (local_34 = 0; local_34 < param_3; local_34 = local_34 + 1) {
      *local_28 = (local_2c >> 0xf & 0xfffffffeU) + (local_2c >> 0x10);
      local_2c = local_2c + (iVar1 << 0x10) / param_3;
      local_28 = local_28 + 1;
    }
    for (local_3c = 0; local_3c < param_4; local_3c = local_3c + 1) {
      puVar6 = (undefined8 *)(iVar3 * 3 * (local_38 >> 0x10) + (int)local_40);
      if (local_24 == puVar6) {
        memcpy(param_1,(void *)((int)param_1 + (param_3 * -3 - iVar7)),param_3 * 3);
        param_1 = (undefined4 *)((int)param_1 + param_3 * 3);
      }
      else {
        local_28 = piVar5;
        for (local_34 = 0; local_24 = puVar6, local_34 < param_3; local_34 = local_34 + 1) {
          *param_1 = *(undefined4 *)(*local_28 + (int)puVar6);
          param_1 = (undefined4 *)((int)param_1 + 3);
          local_28 = local_28 + 1;
        }
      }
      local_38 = local_38 + (iVar2 << 0x10) / param_4;
      param_1 = (undefined4 *)((int)param_1 + iVar7);
    }
    FreeIfNotNull(piVar5);
    if (param_2[0x6a] == 0) {
      FreeIfNotNull(local_40);
    }
  }
  return puVar4;
}

// FUNCTION: CARDARTLIB 0x10008015
bool __cdecl DrawWvlToHwnd(HWND hwnd,int *wvl,DWORD width,DWORD height)

{
  undefined4 *puVar1;
  
  if (wvl != (int *)0x0) {
    puVar1 = (undefined4 *)Wvl_DecodeToBgr24BufferScaled((unsigned char *)0x0,wvl,width,height);
    BlitBgr24ToHwnd(hwnd,puVar1,0,0,width,height);
    FreeIfNotNull(puVar1);
  }
  return wvl != (int *)0x0;
}

// FUNCTION: CARDARTLIB 0x1000807F
/* WARNING: Removing unreachable block (ram,0x10008470) */
/* WARNING: Removing unreachable block (ram,0x1000824d) */

uint * __cdecl Wvl_DecodeToBgr24(uint *param_1,int *param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  uint *local_5054;
  uint local_5050 [4096];
  undefined8 *local_1050;
  int local_104c;
  int local_1048;
  int local_1044;
  uint local_1040;
  int local_103c;
  uint *local_1038;
  int local_1034;
  uint *local_1030;
  undefined4 local_102c;
  uint *local_1028;
  int local_1024;
  int local_1020;
  size_t local_101c;
  int local_1018;
  undefined4 local_1014;
  byte *local_1010;
  uint local_100c [1016];
  undefined4 uStackY_2c;
  
  MSVC_StackProbe();
  local_1034 = 0;
  local_1048 = 0;
  local_102c = 0;
  local_1030 = local_5050;
  local_1038 = local_100c;
  local_1040 = (uint)(param_1 != (uint *)0x0);
  if (param_2 == (int *)0x0) {
    local_5054 = (uint *)0x0;
  }
  else {
    local_1018 = param_2[7] << 0x10;
    local_1024 = param_2[8] << 0x10;
    iVar1 = local_1018 / param_3;
    iVar2 = local_1024 / param_4;
    if (param_2[0x6a] == 0) {
      local_1050 = Wvl_DecodeHaar(param_2,(undefined8 *)&DAT_10032c98);
    }
    else {
      local_1050 = (undefined8 *)param_2[0x6b];
    }
    local_103c = param_2[7];
    local_1014 = 0;
    local_1020 = (DAT_1001e0e0 - (param_3 * 3) % DAT_1001e0e0) % DAT_1001e0e0;
    if (param_1 == (uint *)0x0) {
      local_5054 = (uint *)&DAT_100ad498;
    }
    else {
      local_5054 = param_1;
    }
    local_1034 = 0;
    for (local_1044 = 0; local_1044 < param_3; local_1044 = local_1044 + 1) {
      *local_1030 = local_1034 >> 8;
      local_1034 = local_1034 + iVar1;
      local_1030 = local_1030 + 1;
    }
    param_1 = local_5054;
    if (param_2[8] < param_4) {
      param_1 = (uint *)((int)local_5054 + (param_4 - param_2[8]) * (param_3 * 3 + local_1020));
    }
    local_1028 = param_1;
    for (local_104c = 0; local_104c < param_2[8]; local_104c = local_104c + 1) {
      local_1030 = local_5050;
      for (local_1044 = 0; local_1044 < param_3; local_1044 = local_1044 + 1) {
        local_1010 = (byte *)((int)local_1050 +
                             ((int)*local_1030 >> 8) * 3 + local_103c * 3 * local_104c);
        *(byte *)param_1 =
             (char)(((uint)local_1010[3] - (uint)*local_1010) * (*local_1030 & 0xff) >> 8) +
             *local_1010;
        *(byte *)((int)param_1 + 1) =
             (char)(((uint)local_1010[4] - (uint)local_1010[1]) * (*local_1030 & 0xff) >> 8) +
             local_1010[1];
        *(byte *)((int)param_1 + 2) =
             (char)(((uint)local_1010[5] - (uint)local_1010[2]) * (*local_1030 & 0xff) >> 8) +
             local_1010[2];
        param_1 = (uint *)((int)param_1 + 3);
        local_1030 = local_1030 + 1;
      }
      param_1 = (uint *)((int)param_1 + local_1020);
    }
    local_1048 = 0;
    for (local_1044 = 0; local_1044 < param_4; local_1044 = local_1044 + 1) {
      *local_1038 = local_1048 >> 8;
      local_1048 = local_1048 + iVar2;
      local_1038 = local_1038 + 1;
    }
    local_101c = param_3 * 3 + local_1020;
    if (param_4 < param_2[8]) {
      memcpy
                (local_5050,(uint *)((param_2[8] + -1) * local_101c + (int)local_5054),local_101c);
    }
    for (local_1044 = 0; local_1044 < param_3; local_1044 = local_1044 + 1) {
      param_1 = (uint *)(local_1044 * 3 + (int)local_5054);
      local_1038 = local_100c;
      for (local_104c = 0; local_104c < param_4 + -1; local_104c = local_104c + 1) {
        iVar1 = param_2[8] + -2;
        if ((int)*local_1038 >> 8 <= param_2[8] + -2) {
          iVar1 = (int)*local_1038 >> 8;
        }
        local_1010 = (byte *)((int)local_1028 + iVar1 * local_101c + local_1044 * 3);
        *(byte *)param_1 =
             (char)(((uint)local_1010[local_101c] - (uint)*local_1010) * (*local_1038 & 0xff) >> 8)
             + *local_1010;
        *(byte *)((int)param_1 + 1) =
             (char)(((uint)local_1010[local_101c + 1] - (uint)local_1010[1]) * (*local_1038 & 0xff)
                   >> 8) + local_1010[1];
        *(byte *)((int)param_1 + 2) =
             (char)(((uint)local_1010[local_101c + 2] - (uint)local_1010[2]) * (*local_1038 & 0xff)
                   >> 8) + local_1010[2];
        local_1038 = local_1038 + 1;
        param_1 = (uint *)((int)param_1 + local_101c);
      }
    }
    if (param_4 < param_2[8]) {
      memcpy
                ((uint *)((param_4 + -1) * local_101c + (int)local_5054),local_5050,local_101c);
    }
    else {
      memset((uint *)((param_4 + -1) * local_101c + (int)local_5054),0,local_101c);
    }
    iVar1 = (4 - (param_3 * 3) % 4) % 4;
    if (DAT_1001d258 == 0) {
      QuantizeBgr24ToNearestPaletteColorInPlace(local_5054,param_4,param_3,iVar1);
    }
    else if (DAT_101221e8 == 0x10) {
      uStackY_2c = 0x10008757;
      DitherBgr24ToRgbQuantizedF8(DAT_1001d258,DAT_1001d25c,local_5054,param_4,param_3,iVar1);
    }
    else if (DAT_101221e8 == 8) {
      uStackY_2c = 0x10008793;
      DitherBgr24ToPaletteColors(DAT_1001d258,DAT_1001d25c,local_5054,param_4,param_3,iVar1);
    }
  }
  return local_5054;
}

// FUNCTION: CARDARTLIB 0x100087A6
undefined4 __cdecl DrawWvlToHwndDithered(HWND hwnd,int *wvl,DWORD width,DWORD height)

{
  undefined4 uVar1;
  uint *puVar2;
  
  if (wvl == (int *)0x0) {
    uVar1 = 0;
  }
  else {
    puVar2 = Wvl_DecodeToBgr24((uint *)0x0,wvl,width,height);
    DitherBgr24ToPaletteColors(DAT_1001d258,DAT_1001d25c,puVar2,height,width,
                 (DAT_1001e0e0 - (int)(width * 3) % DAT_1001e0e0) % DAT_1001e0e0);
    BlitBgr24ToHwnd(hwnd,puVar2,0,0,width,height);
    if ((uint *)wvl[0x6b] != puVar2) {
      FreeIfNotNull(puVar2);
    }
    uVar1 = 1;
  }
  return uVar1;
}

// FUNCTION: CARDARTLIB 0x10008867
void * __cdecl AlignedAlloc8(int size)

{
  undefined4 *ptr;
  
  ptr = malloc(size + 8);
  if (((uint)ptr & 7) == 0) {
    ptr[1] = 0;
    ptr = ptr + 2;
  }
  else {
    *ptr = 0xffffffff;
    ptr = ptr + 1;
  }
  return ptr;
}

// FUNCTION: CARDARTLIB 0x100088B9
void __cdecl AlignedFree8(void *ptr)

{
  undefined4 local_8;
  
  if (*(int *)((int)ptr + -4) == 0) {
    local_8 = (void *)((int)ptr + -8);
  }
  else {
    local_8 = (void *)((int)ptr + -4);
  }
  FreeIfNotNull(local_8);
  return;
}

// FUNCTION: CARDARTLIB 0x100088F3
int __cdecl
BlitInt32ToHwndGrayscale(HWND hwnd,int pixels_int32,int dst_x,int dst_y,DWORD width,DWORD height)

{
  undefined1 *_Dst;
  int iVar1;
  BITMAPINFO *lpbmi;
  HDC hdc;
  int iVar2;
  int local_2c;
  int local_28;
  int local_1c;
  undefined1 *local_14;
  undefined1 local_10;
  int local_8;
  
  local_1c = 0;
  local_8 = 0;
  _Dst = malloc(width * height * 3 + 8);
  memset(_Dst,0,width * height * 3);
  local_14 = _Dst;
  for (local_2c = 0; local_2c < (int)height; local_2c = local_2c + 1) {
    for (local_28 = 0; local_28 < (int)width; local_28 = local_28 + 1) {
      iVar1 = *(int *)(local_28 * 4 + width * local_2c * 4 + pixels_int32) >> 2;
      iVar2 = iVar1;
      if ((local_8 <= iVar1) && (iVar2 = local_8, local_1c < iVar1)) {
        local_1c = iVar1;
      }
      local_8 = iVar2;
      if (iVar1 < 1) {
        iVar1 = 0;
      }
      if (0xfe < iVar1) {
        iVar1 = 0xff;
      }
      local_10 = (undefined1)iVar1;
      local_14[2] = local_10;
      local_14[1] = local_14[2];
      *local_14 = local_14[1];
      local_14 = local_14 + 3;
    }
  }
  lpbmi = AllocBitmapInfoTopDown((int)width,(int)height,0x18);
  hdc = GetDC(hwnd);
  iVar2 = SetDIBitsToDevice(hdc,dst_x,dst_y,width,height,0,0,0,height,_Dst,lpbmi,0);
  ReleaseDC(hwnd,hdc);
  FreeBitmapInfo(lpbmi);
  FreeIfNotNull(_Dst);
  return iVar2;
}

// FUNCTION: CARDARTLIB 0x10009490
char * __cdecl StrCopy(char *dst,const char *src)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  
  puVar4 = (uint *)dst;
  while (((uint)src & 3) != 0) {
    bVar1 = *(byte *)src;
    uVar3 = (uint)bVar1;
    src = (const char *)((int)src + 1);
    if (bVar1 == 0) goto LAB_10009578;
    *(byte *)puVar4 = bVar1;
    puVar4 = (uint *)((int)puVar4 + 1);
  }
  do {
    uVar2 = *(uint *)src;
    uVar3 = *(uint *)src;
    src = (const char *)((int)src + 4);
    if (((uVar2 ^ 0xffffffff ^ uVar2 + 0x7efefeff) & 0x81010100) != 0) {
      if ((char)uVar3 == '\0') {
LAB_10009578:
        *(byte *)puVar4 = (byte)uVar3;
        return dst;
      }
      if ((char)(uVar3 >> 8) == '\0') {
        *(short *)puVar4 = (short)uVar3;
        return dst;
      }
      if ((uVar3 & 0xff0000) == 0) {
        *(short *)puVar4 = (short)uVar3;
        *(byte *)((int)puVar4 + 2) = 0;
        return dst;
      }
      if ((uVar3 & 0xff000000) == 0) {
        *puVar4 = uVar3;
        return dst;
      }
    }
    *puVar4 = uVar3;
    puVar4 = puVar4 + 1;
  } while( true );
}

// FUNCTION: CARDARTLIB 0x100094A0
char * __cdecl StrCat(char *dst,const char *src)

{
  byte bVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  uint *puVar5;
  
  puVar3 = (uint *)dst;
  do {
    if (((uint)puVar3 & 3) == 0) goto LAB_100094bc;
    uVar4 = *puVar3;
    puVar3 = (uint *)((int)puVar3 + 1);
  } while ((byte)uVar4 != 0);
  goto LAB_100094ef;
  while( true ) {
    if ((uVar4 & 0xff0000) == 0) {
      puVar5 = (uint *)((int)puVar5 + 2);
      goto joined_r0x1000950b;
    }
    if ((uVar4 & 0xff000000) == 0) break;
LAB_100094bc:
    do {
      puVar5 = puVar3;
      puVar3 = puVar5 + 1;
    } while (((*puVar5 ^ 0xffffffff ^ *puVar5 + 0x7efefeff) & 0x81010100) == 0);
    uVar4 = *puVar5;
    if ((char)uVar4 == '\0') goto joined_r0x1000950b;
    if ((char)(uVar4 >> 8) == '\0') {
      puVar5 = (uint *)((int)puVar5 + 1);
      goto joined_r0x1000950b;
    }
  }
LAB_100094ef:
  puVar5 = (uint *)((int)puVar3 + -1);
joined_r0x1000950b:
  do {
    if (((uint)src & 3) == 0) {
      do {
        uVar2 = *(uint *)src;
        uVar4 = *(uint *)src;
        src = (const char *)((int)src + 4);
        if (((uVar2 ^ 0xffffffff ^ uVar2 + 0x7efefeff) & 0x81010100) != 0) {
          if ((char)uVar4 == '\0') {
LAB_10009578:
            *(byte *)puVar5 = (byte)uVar4;
            return dst;
          }
          if ((char)(uVar4 >> 8) == '\0') {
            *(short *)puVar5 = (short)uVar4;
            return dst;
          }
          if ((uVar4 & 0xff0000) == 0) {
            *(short *)puVar5 = (short)uVar4;
            *(byte *)((int)puVar5 + 2) = 0;
            return dst;
          }
          if ((uVar4 & 0xff000000) == 0) {
            *puVar5 = uVar4;
            return dst;
          }
        }
        *puVar5 = uVar4;
        puVar5 = puVar5 + 1;
      } while( true );
    }
    bVar1 = *(byte *)src;
    uVar4 = (uint)bVar1;
    src = (const char *)((int)src + 1);
    if (bVar1 == 0) goto LAB_10009578;
    *(byte *)puVar5 = bVar1;
    puVar5 = (uint *)((int)puVar5 + 1);
  } while( true );
}

// FUNCTION: CARDARTLIB 0x1000B7A0
/* WARNING: Unable to track spacebase fully for stack */

void MSVC_StackProbe(void)

{  uint in_EAX;
  undefined1 *puVar1;
  undefined4 unaff_retaddr;
  
  //puVar1 = &stack0x00000004;
  for (; 0xfff < in_EAX; in_EAX = in_EAX - 0x1000) {
    puVar1 = puVar1 + -0x1000;
  }
  *(undefined4 *)(puVar1 + (-4 - in_EAX)) = unaff_retaddr;
  return;
}

// FUNCTION: CARDARTLIB 0x1000BFE0
uint entry(undefined4 param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint local_8;
  
  local_8 = 1;
  if ((param_2 == 0) && (DAT_1001e2a0 == 0)) {
    local_8 = 0;
  }
  else {
    if ((param_2 == 1) || (param_2 == 2)) {
      if (DAT_10123340 != (code *)0x0) {
        local_8 = (*DAT_10123340)((HINSTANCE)param_1,param_2,(LPVOID)param_3);
      }
      if (local_8 != 0) {
        //local_8 = __CRT_INIT((HINSTANCE)param_1,param_2,(LPVOID)param_3);
      }
      if (local_8 == 0) {
        return 0;
      }
    }
    local_8 = (uint)DllMain((HINSTANCE)param_1,param_2,(LPVOID)param_3);
    if ((param_2 == 1) && (local_8 == 0)) {
      //__CRT_INIT((HINSTANCE)param_1,0,(LPVOID)0);
    }
    if ((param_2 == 0) || (param_2 == 3)) {
      //iVar1 = __CRT_INIT((HINSTANCE)param_1,param_2,(LPVOID)param_3);
      if (iVar1 == 0) {
        local_8 = 0;
      }
      if ((local_8 != 0) && (DAT_10123340 != (code *)0x0)) {
        local_8 = (*DAT_10123340)((HINSTANCE)param_1,param_2,(LPVOID)param_3);
      }
    }
  }
  return local_8;
}
