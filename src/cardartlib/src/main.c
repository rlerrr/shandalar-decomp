#include "CardArtLib.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "inttypes.h"
#include "mystdbool.h"

typedef void * pointer;

/* Ghidra often labels CRT imports with a leading underscore (e.g. `_fopen`).
 * Map them to the standard names so MSVC headers provide prototypes. */
#define _fopen fopen
#define _fclose fclose
#define _fread fread
#define _fseek fseek
#define _fgets fgets
#define _fprintf fprintf
#define _sprintf sprintf
#define _strlen strlen
#define _time time
#define _ctime ctime
#define _malloc malloc
#define _memset memset
#define _strchr strchr
#define _strrchr strrchr
#define _sscanf sscanf
#define _strspn strspn
#define _strcspn strcspn
#define _atoi atoi
#define _atol atol
#define _bsearch bsearch
#define __splitpath _splitpath
#define __strlwr _strlwr
#define __vsnprintf _vsnprintf
#define FID_conflict__memcpy memcpy

typedef BOOL (WINAPI code)(HINSTANCE, DWORD, LPVOID);
BOOL WINAPI _CRT_INIT(HINSTANCE hinst, DWORD reason, LPVOID reserved);
void FUN_1000b7a0(void);

void __cdecl FUN_1000a210(void *ptr)
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

void __cdecl FUN_10005cb4(undefined4 *param_1,int param_2);
uint * __cdecl FUN_10009490(uint *param_1,uint *param_2);
uint * __cdecl FUN_100094a0(uint *param_1,uint *param_2);
undefined4 * __cdecl FUN_10007bdb(undefined4 param_1,int param_2,int param_3);
undefined4 __cdecl FUN_10007cff(void *param_1);
uint * __cdecl FUN_1000807f(uint *param_1,int *param_2,int param_3,int param_4);
int * __cdecl FUN_100068f0(int param_1,uint *param_2,int param_3);
undefined8 * __cdecl FUN_10006be3(int *param_1,undefined8 *param_2);
void FUN_10006809(void);
void FUN_100012a8(void);
void __cdecl checked_DeleteDC_DeleteObject(HDC param_1,HGDIOBJ param_2);
void FUN_100019a8(void);
void FUN_1000343e(void);
void __cdecl FUN_10003c70(undefined4 *param_1,undefined4 param_2,int param_3);
undefined * __cdecl FUN_100045fe(int param_1,int param_2);
void __cdecl FUN_1000485a(int id,int version);
void FUN_100049e4(void);
undefined2 * __cdecl TENTATIVE_read_palette(char *param_1,char *param_2);
void __cdecl FUN_10007035(undefined8 *param_1,undefined8 *param_2,int param_3,int param_4,int param_5,int param_6,
                          int param_7);
void __cdecl FUN_10007238(undefined8 *param_1,undefined8 *param_2,uint param_3);
void __cdecl FUN_10007273(undefined8 *param_1,uint param_2,uint param_3);
void __cdecl FUN_100072dc(int *param_1,int param_2,int param_3);
void __cdecl FUN_10007419(int *param_1,int *param_2,int *param_3,int param_4,int param_5,undefined4 param_6,
                          int param_7);
void __cdecl FUN_100074ee(int *param_1,int *param_2,int *param_3,int param_4,int param_5,undefined4 param_6,
                          int param_7);
undefined1 * __cdecl
FUN_100075ef(undefined1 *param_1,int *param_2,int param_3,int param_4,int param_5,int param_6,int param_7,
             undefined4 param_8,int param_9);

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
unsigned char * PTR_DAT_1001e118 = DAT_100326d8 + 0x400;

// GLOBAL: CARDARTLIB 0x1001E11C
undefined4 DAT_1001e11c = 0x00000001;

// GLOBAL: CARDARTLIB 0x1001E120
undefined4 DAT_1001e120 = 0x00000002;

// GLOBAL: CARDARTLIB 0x1001E124
undefined4 DAT_1001e124 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001E128
undefined1 * PTR_DAT_1001e128 = DAT_100eaf00 + 0x400;

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
undefined4 DAT_10031ea4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10031EA8
undefined4 DAT_10031ea8 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10031EB0
undefined1 DAT_10031eb0 = 0x00;

// GLOBAL: CARDARTLIB 0x10031EB1
undefined1 DAT_10031eb1 = 0x00;

// GLOBAL: CARDARTLIB 0x10031EB8
undefined4 _DAT_10031eb8 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10031EC0
undefined1 DAT_10031ec0[0x400];

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
undefined4 DAT_10032ae0 = 0x00000000;

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
undefined4 DAT_100ea09c = 0x00000000;

// GLOBAL: CARDARTLIB 0x100EA31C
int *DAT_100ea31c = (int *)0x0;

// GLOBAL: CARDARTLIB 0x100EA320
int *DAT_100ea320 = (int *)0x0;

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
unsigned char DAT_10117290[5 * 0x114];
#define DAT_10117294 DAT_10117290[4]
#define DAT_10117298 DAT_10117290[8]
#define DAT_1011729c DAT_10117290[0xc]
#define DAT_101172a0 DAT_10117290[0x10]

// GLOBAL: CARDARTLIB 0x10117100
void *DAT_10117100[0x41];

// GLOBAL: CARDARTLIB 0x101177F4
undefined4 DAT_101177f4 = 0x00000000;

// GLOBAL: CARDARTLIB 0x10117800
char DAT_10117800[0x105];

// GLOBAL: CARDARTLIB 0x10117910
char DAT_10117910[0x105];

// GLOBAL: CARDARTLIB 0x10121FE0
undefined4 DAT_10121fe0 = 0x00000000;

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
unsigned char DAT_1011f740[100 * 0x18];
#define DAT_1011f744 DAT_1011f740[4]
#define DAT_1011f748 DAT_1011f740[8]
#define DAT_1011f74c DAT_1011f740[0xc]
#define DAT_1011f750 DAT_1011f740[0x10]
#define DAT_1011f754 DAT_1011f740[0x14]

// GLOBAL: CARDARTLIB 0x10121FF0
unsigned char DAT_10121ff0[0x14 * 0x18];
#define DAT_10121ff4 DAT_10121ff0[4]
#define DAT_10121ff8 DAT_10121ff0[8]
#define DAT_10121ffc DAT_10121ff0[0xc]

// GLOBAL: CARDARTLIB 0x10123340
code *DAT_10123340 = (code *)0x0;

// GLOBAL: CARDARTLIB 0x101241C8
pointer PTR_SetPaletteEntries_101241c8 = 0x00124518;

// GLOBAL: CARDARTLIB 0x101241CC
pointer PTR_GetPaletteEntries_101241cc = 0x00124504;

// GLOBAL: CARDARTLIB 0x101241D0
pointer PTR_StretchBlt_101241d0 = 0x001244F6;

// GLOBAL: CARDARTLIB 0x101241D4
pointer PTR_GetObjectA_101241d4 = 0x001244E8;

// GLOBAL: CARDARTLIB 0x101241D8
pointer PTR_CreateCompatibleDC_101241d8 = 0x001244D2;

// GLOBAL: CARDARTLIB 0x101241DC
pointer PTR_CreateDIBSection_101241dc = 0x001244BE;

// GLOBAL: CARDARTLIB 0x101241E0
pointer PTR_SelectObject_101241e0 = 0x001244AE;

// GLOBAL: CARDARTLIB 0x101241E4
pointer PTR_SetDIBitsToDevice_101241e4 = 0x0012455E;

// GLOBAL: CARDARTLIB 0x101241E8
pointer PTR_GetStockObject_101241e8 = 0x0012454C;

// GLOBAL: CARDARTLIB 0x101241EC
pointer PTR_GetDeviceCaps_101241ec = 0x0012453C;

// GLOBAL: CARDARTLIB 0x101241F0
pointer PTR_CreatePalette_101241f0 = 0x0012452C;

// GLOBAL: CARDARTLIB 0x101241F4
pointer PTR_DeleteObject_101241f4 = 0x00124492;

// GLOBAL: CARDARTLIB 0x101241F8
pointer PTR_DeleteDC_101241f8 = 0x001244A2;

// GLOBAL: CARDARTLIB 0x101241FC
pointer PTR_SetStretchBltMode_101241fc = 0x0012443C;

// GLOBAL: CARDARTLIB 0x10124200
pointer PTR_SetDIBColorTable_10124200 = 0x00124450;

// GLOBAL: CARDARTLIB 0x10124204
pointer PTR_GdiFlush_10124204 = 0x00124464;

// GLOBAL: CARDARTLIB 0x10124208
pointer PTR_RealizePalette_10124208 = 0x00124470;

// GLOBAL: CARDARTLIB 0x1012420C
pointer PTR_SelectPalette_1012420c = 0x00124482;

// GLOBAL: CARDARTLIB 0x10124214
pointer PTR_OutputDebugStringA_10124214 = 0x001243BA;

// GLOBAL: CARDARTLIB 0x10124218
pointer PTR_GetModuleFileNameA_10124218 = 0x001243A4;

// GLOBAL: CARDARTLIB 0x1012421C
pointer PTR_EnterCriticalSection_1012421c = 0x0012438C;

// GLOBAL: CARDARTLIB 0x10124220
pointer PTR_LeaveCriticalSection_10124220 = 0x00124374;

// GLOBAL: CARDARTLIB 0x10124224
pointer PTR_DeleteCriticalSection_10124224 = 0x0012435C;

// GLOBAL: CARDARTLIB 0x1012428C
pointer PTR_InitializeCriticalSection_1012428c = 0x00124340;

// GLOBAL: CARDARTLIB 0x10124324
pointer PTR_MessageBoxA_10124324 = 0x001243DE;

// GLOBAL: CARDARTLIB 0x10124328
pointer PTR_ReleaseDC_10124328 = 0x001243EC;

// GLOBAL: CARDARTLIB 0x1012432C
pointer PTR_GetDC_1012432c = 0x001243F8;

// GLOBAL: CARDARTLIB 0x10124330
pointer PTR_GetDesktopWindow_10124330 = 0x00124400;

// GLOBAL: CARDARTLIB 0x10124334
pointer PTR_FillRect_10124334 = 0x00124414;

// GLOBAL: CARDARTLIB 0x10124338
pointer PTR_GetClientRect_10124338 = 0x00124420;

// FUNCTION: CARDARTLIB 0x10001000
void __cdecl assert(int condition,char *file,int line,char *fmt,...)

{
  size_t messageLen;
  FILE *logFile;
  time_t now;
  va_list args;
  uint local_114 [65];

  if (condition != 0) {
    return;
  }

  FUN_10009490(local_114,(uint *)&DAT_10117800);
  FUN_100094a0(local_114,(uint *)s__assertFile_txt_1001d05c);
  logFile = _fopen((char *)local_114,DAT_1001d06c);

  DAT_10020210[0] = '\0';
  if ((file != (char *)0x0) && (line != 0)) {
    sprintf(DAT_10020210,s_File__s__Line__d_1001d070,file,line);
  }

  messageLen = strlen(DAT_10020210);
  va_start(args,fmt);
  __vsnprintf(DAT_10020210 + messageLen,2000 - messageLen,fmt,args);
  va_end(args);

  _time(&now);
  if (logFile != (FILE *)0x0) {
    fprintf(logFile,s__s_s_1001d088,_ctime(&now),DAT_10020210);
    fclose(logFile);
  }

  MessageBoxA((HWND)0x0,DAT_10020210,s_Assertion_Error_1001d090,0x1000);
                  /* WARNING: Subroutine does not return */
  exit(0xff);
}

// FUNCTION: CARDARTLIB 0x10001116
void __cdecl FUN_10001116(int param_1,int param_2,int param_3,char *param_4,...)

{
  size_t messageLen;
  FILE *logFile;
  time_t now;
  va_list args;
  uint local_114 [65];

  if (param_1 != 0) {
    return;
  }

  FUN_10009490(local_114,(uint *)&DAT_10117800);
  FUN_100094a0(local_114,(uint *)s__assertFile_txt_1001d0a0);
  logFile = _fopen((char *)local_114,DAT_1001d0b0);

  DAT_10020210[0] = '\0';
  if ((param_2 != 0) && (param_3 != 0)) {
    sprintf(DAT_10020210,s_File__s__Line__d_1001d0b4,(char *)param_2,param_3);
  }

  messageLen = _strlen(DAT_10020210);
  va_start(args,param_4);
  __vsnprintf(DAT_10020210 + messageLen,2000 - messageLen,param_4,args);
  va_end(args);

  _time(&now);
  if (logFile != (FILE *)0x0) {
    _fprintf(logFile,s__s_s_1001d0cc,_ctime(&now),DAT_10020210);
    _fclose(logFile);
  }

  MessageBoxA((HWND)0x0,DAT_10020210,s_Assertion_Error_1001d0d4,0x1000);
                  /* WARNING: Subroutine does not return */
  _exit(0xff);
}

// FUNCTION: CARDARTLIB 0x10001220
bool FUN_10001220(void)

{
  int iVar1;
  bool bVar2;
  
  iVar1 = TENTATIVE_setup_palette();
  if (DAT_1001d0e4 == 0) {
    FUN_1000134a(10,10,&DAT_1001d0e4,(BITMAPINFO *)0x0,&DAT_100209e4,(undefined4 *)0x0,(int *)0x0);
    InitializeCriticalSection((LPCRITICAL_SECTION)&DAT_10020de8);
  }
  bVar2 = DAT_1001d0e4 != 0;
  if (!bVar2 || iVar1 == 0) {
    FUN_100012a8();
  }
  return bVar2 && iVar1 != 0;
}

// FUNCTION: CARDARTLIB 0x100012A8
void FUN_100012a8(void)

{
  if (DAT_1001d0e4 != (HDC)0x0) {
    checked_DeleteDC_DeleteObject(DAT_1001d0e4,DAT_100209e4);
    DAT_1001d0e4 = (HDC)0x0;
    DeleteCriticalSection((LPCRITICAL_SECTION)&DAT_10020de8);
  }
  if (DAT_100209e0 != 0) {
    FUN_100019a8();
  }
  return;
}

// FUNCTION: CARDARTLIB 0x100012FB
void __cdecl FUN_100012fb(HDC hdc)

{
  SelectPalette(hdc,DAT_100209e0,0);
  RealizePalette(hdc);
  GdiFlush();
  SetDIBColorTable(hdc,0,0x100,(RGBQUAD *)&DAT_100209e8);
  SetStretchBltMode(hdc,3);
  return;
}

// FUNCTION: CARDARTLIB 0x1000134A
undefined4 __cdecl
FUN_1000134a(undefined4 param_1,int param_2,undefined4 *param_3,BITMAPINFO *param_4,
            undefined4 *param_5,undefined4 *param_6,int *param_7)

{
  undefined4 uVar1;
  HDC hdc;
  HBITMAP local_44;
  HDC local_3c;
  BITMAPINFO local_38;
  HGDIOBJ local_c;
  void *local_8;
  
  local_3c = (HDC)0x0;
  local_44 = (HBITMAP)0x0;
  local_8 = (void *)0x0;
  if ((param_3 == (undefined4 *)0x0) || (param_5 == (undefined4 *)0x0)) {
    uVar1 = 0;
  }
  else {
    if (param_4 == (BITMAPINFO *)0x0) {
      param_4 = &local_38;
    }
    hdc = GetDC((HWND)0x0);
    if (hdc != (HDC)0x0) {
      FUN_100012fb(hdc);
      local_3c = CreateCompatibleDC(hdc);
      if (local_3c != (HDC)0x0) {
        FUN_10003c70((undefined4 *)param_4,param_1,param_2);
        local_38.bmiHeader.biBitCount = 0x20;
        local_44 = CreateDIBSection(hdc,param_4,0,&local_8,(HANDLE)0x0,0);
        local_c = SelectObject(local_3c,local_44);
        FUN_100012fb(local_3c);
      }
      ReleaseDC((HWND)0x0,hdc);
    }
    if (((local_3c == (HDC)0x0) || (local_44 == (HBITMAP)0x0)) || (local_8 == (void *)0x0)) {
      if (local_3c != (HDC)0x0) {
        DeleteDC(local_3c);
      }
      if (local_44 != (HBITMAP)0x0) {
        DeleteObject(local_44);
      }
      uVar1 = 0;
    }
    else {
      if (param_3 != (undefined4 *)0x0) {
        *param_3 = local_3c;
      }
      if (param_5 != (undefined4 *)0x0) {
        *param_5 = local_44;
      }
      if (param_6 != (undefined4 *)0x0) {
        *param_6 = local_c;
      }
      if (param_7 != (int *)0x0) {
        *param_7 = (int)local_8;
      }
      uVar1 = 1;
    }
  }
  return uVar1;
}

// FUNCTION: CARDARTLIB 0x100014D2
void __cdecl checked_DeleteDC_DeleteObject(HDC param_1,HGDIOBJ param_2)

{
  if (param_1 != (HDC)0x0) {
    DeleteDC(param_1);
  }
  if (param_2 != (HGDIOBJ)0x0) {
    DeleteObject(param_2);
  }
  return;
}

// FUNCTION: CARDARTLIB 0x10001505
undefined4 __cdecl FUN_10001505(HDC param_1,int *param_2,HANDLE param_3)

{
  undefined4 uVar1;
  undefined1 local_1c [4];
  int local_18;
  int local_14;
  
  if (((param_1 == (HDC)0x0) || (param_2 == (int *)0x0)) || (param_3 == (HANDLE)0x0)) {
    uVar1 = 0;
  }
  else {
    GetObjectA(param_3,0x18,local_1c);
    uVar1 = FUN_1000156d(param_1,param_2,param_3,0,0,local_18,local_14);
  }
  return uVar1;
}

// FUNCTION: CARDARTLIB 0x1000156D
undefined4 __cdecl
FUN_1000156d(HDC param_1,int *param_2,HANDLE param_3,int param_4,int param_5,int param_6,int param_7
            )

{
  undefined4 uVar1;
  HGDIOBJ h;
  int local_2c;
  undefined1 local_28 [4];
  int local_24;
  int local_20;
  int local_10;
  int local_c;
  int local_8;
  
  if (((param_1 == (HDC)0x0) || (param_2 == (int *)0x0)) || (param_3 == (HANDLE)0x0)) {
    uVar1 = 0;
  }
  else {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10020de8);
    h = SelectObject(DAT_1001d0e4,param_3);
    GetObjectA(param_3,0x18,local_28);
    local_8 = *param_2;
    local_c = param_2[1];
    if (param_2[2] < *param_2) {
      local_10 = local_24;
    }
    else {
      local_10 = param_2[2] - *param_2;
    }
    if (param_2[3] < param_2[1]) {
      local_2c = local_20;
    }
    else {
      local_2c = param_2[3] - param_2[1];
    }
    FUN_100012fb(DAT_1001d0e4);
    if (param_7 <= local_20) {
      local_20 = param_7;
    }
    if (param_6 <= local_24) {
      local_24 = param_6;
    }
    StretchBlt(param_1,local_8,local_c,local_10,local_2c,DAT_1001d0e4,param_4,param_5,local_24,
               local_20,0xcc0020);
    SelectObject(DAT_1001d0e4,h);
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10020de8);
    uVar1 = 1;
  }
  return uVar1;
}

// FUNCTION: CARDARTLIB 0x100016B7
undefined4 TENTATIVE_setup_palette(void)

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
  FUN_10009490(local_624,(uint *)&DAT_10117800);
  FUN_100094a0(local_624,(uint *)s__DUELPALall_TR_1001d0e8);
  FUN_10009490(local_510,(uint *)&DAT_10117800);
  FUN_100094a0(local_510,(uint *)s__DUEL_plogpal_1001d0f8);
  local_408 = (LOGPALETTE *)TENTATIVE_read_palette((char *)local_624,(char *)local_510);
  if (local_408 == (LOGPALETTE *)0x0) {
    local_51c = 0;
  }
  else {
    for (local_514 = 1; (int)local_514 < 0xff; local_514 = local_514 + 1) {
      local_408->palPalEntry[local_514].peFlags = '\x04';
    }
    DAT_100209e0 = CreatePalette(local_408);
    if (DAT_100209e0 == (HPALETTE)0x0) {
      local_51c = 0;
    }
    else {
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
  }
  return local_51c;
}

// FUNCTION: CARDARTLIB 0x100019A8
void FUN_100019a8(void)

{
  DeleteObject(DAT_100209e0);
  DAT_100209e0 = (HGDIOBJ)0x0;
  FUN_10006809();
  return;
}

// FUNCTION: CARDARTLIB 0x100019D0
int __cdecl FUN_100019d0(uint *param_1)

{
  int iVar1;
  int iVar2;
  FILE *pFVar3;
  void *pvVar4;
  int local_20;
  int local_1c;
  int local_10;
  int local_c;
  
  local_c = -1;
  local_10 = 0;
  do {
    if (4 < local_10) {
LAB_10001a24:
      assert((uint)(local_c != -1),s_D__Newmagic_sources_NedCard_Cata_1001d12c,0x43,
             s_Too_many_open_Catalogs__Max__d_1001d10c,5);
      iVar1 = local_c * 0x114;
      *(undefined4 *)(&DAT_1011729c + iVar1) = 0;
      FUN_10009490((uint *)(iVar1 + 0x101172a0),param_1);
      pFVar3 = _fopen((char *)param_1,&DAT_1001d154);
      *(FILE **)(&DAT_10117290 + iVar1) = pFVar3;
      pFVar3 = *(FILE **)(&DAT_10117290 + iVar1);
      if (pFVar3 == (FILE *)0x0) {
        local_c = 0;
      }
      else {
        _fread(&DAT_10117294 + iVar1,4,1,pFVar3);
        pvVar4 = _malloc(*(int *)(&DAT_10117294 + iVar1) * 0xc);
        *(void **)(&DAT_10117298 + iVar1) = pvVar4;
        _fread(*(void **)(&DAT_10117298 + iVar1),0xc,*(size_t *)(&DAT_10117294 + iVar1),pFVar3);
        if (DAT_1001d108 != 0) {
          for (local_10 = 0; local_10 < 5; local_10 = local_10 + 1) {
            if ((local_c != local_10) && (*(int *)(&DAT_10117290 + local_10 * 0x114) != 0)) {
              iVar2 = local_10 * 0x114;
              local_1c = 0;
              while (*(int *)(&DAT_10117294 + iVar1) != 0) {
                for (local_20 = 0; local_20 < *(int *)(&DAT_10117294 + iVar2);
                    local_20 = local_20 + 1) {
                  assert((uint)(*(int *)(*(int *)(&DAT_10117298 + iVar1) + local_1c * 0xc) !=
                               *(int *)(*(int *)(&DAT_10117298 + iVar2) + local_1c * 0xc)),
                         s_D__Newmagic_sources_NedCard_Cata_1001d1b4,0x69,
                         s_Duplicate_short_name_found_in_ca_1001d158,iVar1 + 0x101172a0,local_1c,
                         iVar2 + 0x101172a0,local_20,
                         *(undefined4 *)(*(int *)(&DAT_10117298 + iVar1) + local_1c * 0xc));
                }
                local_1c = local_1c + 1;
              }
            }
          }
        }
        local_c = local_c + 1;
      }
      return local_c;
    }
    if (*(int *)(&DAT_10117290 + local_10 * 0x114) == 0) {
      local_c = local_10;
      goto LAB_10001a24;
    }
    local_10 = local_10 + 1;
  } while( true );
}

// FUNCTION: CARDARTLIB 0x10001C1A
bool __cdecl FUN_10001c1a(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = param_1 + -1;
  iVar2 = *(int *)(&DAT_10117290 + iVar1 * 0x114);
  if (iVar2 != 0) {
    FUN_1000a210(*(void **)(&DAT_10117298 + iVar1 * 0x114));
    _fclose(*(FILE **)(&DAT_10117290 + iVar1 * 0x114));
    *(undefined4 *)(&DAT_10117298 + iVar1 * 0x114) = 0;
    *(undefined4 *)(&DAT_10117290 + iVar1 * 0x114) = 0;
    *(undefined4 *)(&DAT_10117294 + iVar1 * 0x114) = 0;
  }
  return iVar2 != 0;
}

// FUNCTION: CARDARTLIB 0x10001CD6
undefined4 __cdecl FUN_10001cd6(int *param_1,int *param_2)

{
  undefined4 uVar1;
  
  if (*param_2 < *param_1) {
    uVar1 = 1;
  }
  else if (*param_1 < *param_2) {
    uVar1 = 0xffffffff;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

// FUNCTION: CARDARTLIB 0x10001D26
void * __cdecl FUN_10001d26(int param_1,char *param_2)

{
  void *pvVar1;
  uint local_8;
  
  local_8 = FUN_10001e4a(param_2);
  if ((*(int *)(param_1 + 0xc) == 0) || (**(uint **)(param_1 + 0xc) != local_8)) {
    pvVar1 = _bsearch(&local_8,*(void **)(param_1 + 8),*(size_t *)(param_1 + 4),0xc,FUN_10001cd6);
    *(void **)(param_1 + 0xc) = pvVar1;
  }
  else {
    pvVar1 = *(void **)(param_1 + 0xc);
  }
  return pvVar1;
}

// FUNCTION: CARDARTLIB 0x10001DA1
size_t __cdecl FUN_10001da1(int param_1,char *param_2,int *param_3)

{
  undefined4 *puVar1;
  void *pvVar2;
  size_t sVar3;
  void *pvVar4;
  
  puVar1 = (undefined4 *)(&DAT_10117290 + (param_1 + -1) * 0x114);
  pvVar2 = FUN_10001d26((int)puVar1,param_2);
  if (pvVar2 == (void *)0x0) {
    sVar3 = 0xffffffff;
  }
  else {
    if (*param_3 == 0) {
      pvVar4 = _malloc(*(int *)((int)pvVar2 + 8) + 0x10);
      *param_3 = (int)pvVar4;
    }
    _fseek((FILE *)*puVar1,*(long *)((int)pvVar2 + 4),0);
    sVar3 = _fread((void *)*param_3,1,*(size_t *)((int)pvVar2 + 8),(FILE *)*puVar1);
  }
  return sVar3;
}

// FUNCTION: CARDARTLIB 0x10001E4A
uint __cdecl FUN_10001e4a(char *param_1)

{
  int iVar1;
  int local_138;
  uint local_134 [4];
  char local_124 [260];
  undefined4 local_20;
  undefined4 local_1c;
  uint local_10;
  int local_8;
  
  local_10 = 3;
  local_20 = 0;
  local_138 = 0;
  local_8 = 0;
  __splitpath(param_1,(char *)local_134,local_124,(char *)&local_1c,(char *)local_134);
  param_1 = (char *)&local_1c;
  FUN_100094a0(&local_1c,local_134);
  while( true ) {
    iVar1 = (int)*param_1;
    param_1 = param_1 + 1;
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
void __cdecl FUN_10001f40(undefined8 *param_1,uint param_2)

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

int __cdecl FUN_10001f98(undefined1 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  uint uVar2;
  uint uVar3;
  byte local_28;
  int local_1c;
  int local_18;
  uint local_c;
  int local_8;
  
  puVar1 = param_1;
  DAT_10021a88 = (uint *)param_2;
  _DAT_10021a8c = (int)param_2;
  DAT_10021a90 = (int)param_3;
  DAT_1001d1dc = 0;
  uVar2 = FUN_10002f70(8);
  for (local_18 = 0; local_18 < (int)uVar2; local_18 = local_18 + 1) {
    uVar3 = FUN_10002f70(9);
    *(uint *)(&DAT_10021aa0 + local_18 * 8) = uVar3;
    uVar3 = FUN_10002f70(9);
    *(uint *)(&DAT_10021aa4 + local_18 * 8) = uVar3;
  }
  FUN_10002154(uVar2);
  local_c = FUN_10002f70(8);
  do {
    while( true ) {
      if (local_c == 0xffffffff) {
        return (int)param_1 - (int)puVar1;
      }
      if (*(int *)(&DAT_10020e88 + local_c * 0xc) < 0) break;
      uVar2 = *(uint *)(&DAT_10020e8c + local_c * 0xc);
      *param_1 = (&DAT_10020e88)[local_c * 0xc];
      param_1 = param_1 + 1;
      local_28 = (byte)uVar2;
      uVar2 = FUN_10002f70(uVar2);
      if (uVar2 == 0xffffffff) {
        local_c = 0xffffffff;
      }
      else {
        local_c = (int)local_c >> (local_28 & 0x1f) | uVar2 << (8 - local_28 & 0x1f);
      }
    }
    local_8 = *(int *)(&DAT_10020e90 + local_c * 0xc);
    do {
      uVar2 = FUN_10003060();
      if (uVar2 == 0xffffffff) goto LAB_10002132;
      if (uVar2 == 0) {
        local_1c = *(int *)(&DAT_10021aa4 + local_8 * 8);
      }
      else {
        local_1c = *(int *)(&DAT_10021aa0 + local_8 * 8);
      }
      local_8 = local_1c + -0x100;
    } while (-1 < local_8);
    *param_1 = (undefined1)local_1c;
    param_1 = param_1 + 1;
LAB_10002132:
    local_c = FUN_10002f70(8);
  } while( true );
}

// FUNCTION: CARDARTLIB 0x10002154
undefined4 __cdecl FUN_10002154(int param_1)

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
  local_8 = param_1 + -1;
  local_30[0] = param_1 + -1;
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

int __cdecl FUN_10002457(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  uint uVar2;
  byte local_24;
  int local_14;
  uint local_c;
  int local_8;
  
  puVar1 = param_1;
  DAT_10021a88 = param_2;
  _DAT_10021a8c = param_2;
  DAT_10021a90 = param_3;
  DAT_1001d1dc = 0;
  local_c = FUN_10002f70(8);
  do {
    while( true ) {
      if (local_c == 0xffffffff) {
        return (int)param_1 - (int)puVar1 >> 2;
      }
      if (0x7ffffffe < *(int *)(&DAT_10020e88 + local_c * 0xc)) break;
      uVar2 = *(uint *)(&DAT_10020e8c + local_c * 0xc);
      *param_1 = *(undefined4 *)(&DAT_10020e88 + local_c * 0xc);
      param_1 = param_1 + 1;
      local_24 = (byte)uVar2;
      uVar2 = FUN_10002f70(uVar2);
      if (uVar2 == 0xffffffff) {
        local_c = 0xffffffff;
      }
      else {
        local_c = (int)local_c >> (local_24 & 0x1f) | uVar2 << (8 - local_24 & 0x1f);
      }
    }
    local_8 = *(int *)(&DAT_10020e90 + local_c * 0xc);
    do {
      uVar2 = FUN_10003060();
      if (uVar2 == 0xffffffff) goto LAB_100025a6;
      if (uVar2 == 0) {
        local_14 = *(int *)(&DAT_10021aa4 + local_8 * 8);
      }
      else {
        local_14 = *(int *)(&DAT_10021aa0 + local_8 * 8);
      }
      local_8 = local_14 - _DAT_10021a98;
    } while (-1 < local_8);
    *param_1 = *(undefined4 *)(DAT_10021a9c + local_14 * 4);
    param_1 = param_1 + 1;
LAB_100025a6:
    local_c = FUN_10002f70(8);
  } while( true );
}

// FUNCTION: CARDARTLIB 0x100025CB
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl FUN_100025cb(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  uint uVar1;
  uint local_c;
  int local_8;
  
  local_c = 0xd;
  for (local_8 = 0; local_8 < 0x20; local_8 = local_8 + 1) {
    *(uint *)(&DAT_10020e00 + local_8 * 4) = 0xffffffff >> ((byte)local_8 & 0x1f);
  }
  DAT_10021a88 = param_1;
  _DAT_10021a8c = param_1;
  DAT_10021a90 = 100000;
  DAT_1001d1dc = 0;
  DAT_10020e84 = 0;
  DAT_10021a94 = FUN_10002f70(0xd);
  for (local_8 = 0; local_8 < (int)DAT_10021a94; local_8 = local_8 + 1) {
    uVar1 = FUN_10002f70(0xd);
    *(uint *)(&DAT_10021aa0 + local_8 * 8) = uVar1;
    uVar1 = FUN_10002f70(0xd);
    *(uint *)(&DAT_10021aa4 + local_8 * 8) = uVar1;
    local_c = local_c + 0x1a;
  }
  _DAT_10021a98 = param_3;
  DAT_10021a9c = param_2;
  FUN_100026e3(DAT_10021a94);
  return (uint)((local_c & 7) != 0) + ((int)(local_c + ((int)local_c >> 0x1f & 7U)) >> 3);
}

// FUNCTION: CARDARTLIB 0x100026E3
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl FUN_100026e3(int param_1)

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
  local_8 = param_1 + -1;
  local_38[0] = param_1 + -1;
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
  _memset(&DAT_10117210,0,0x80);
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
      uVar1 = FUN_10003060();
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
      uVar1 = FUN_10002f70(10);
      if ((int)uVar1 < 0) {
        return local_10;
      }
      _memset(param_1,0,uVar1 << 2);
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

int __cdecl FUN_10002cde(undefined8 *param_1,uint *param_2,undefined4 param_3)

{
  undefined8 *puVar1;
  int iVar2;
  uint uVar3;
  byte local_28;
  int local_14;
  uint local_c;
  int local_8;
  
  puVar1 = param_1;
  DAT_10021a88 = param_2;
  _DAT_10021a8c = param_2;
  DAT_10021a90 = param_3;
  if (((uint)param_2 & 3) == 0) {
    DAT_1001d1dc = 0;
    DAT_10020e84 = 0;
  }
  else {
    iVar2 = 4 - ((uint)param_2 & 3);
    DAT_1001d1dc = iVar2 * 8;
    DAT_10020e84 = 0xffffffffU >> (0x20U - (char)DAT_1001d1dc & 0x1f) & *param_2;
    DAT_10021a88 = (uint *)((int)param_2 + iVar2);
  }
  local_c = FUN_10002f70(8);
  while (local_c != 0xffffffff) {
    if (*(int *)(&DAT_10020e88 + local_c * 0xc) < 0x7fffffff) {
      uVar3 = *(uint *)(&DAT_10020e8c + local_c * 0xc);
      local_28 = (byte)uVar3;
      if (*(int *)(&DAT_10020e88 + local_c * 0xc) == -0x80000000) {
        uVar3 = FUN_10002f70(uVar3 + 2);
        if ((int)uVar3 < 0) break;
        uVar3 = uVar3 << (8 - local_28 & 0x1f) | (int)local_c >> (local_28 & 0x1f);
        FUN_10001f40(param_1,uVar3);
        param_1 = (undefined8 *)((int)param_1 + uVar3 * 4);
        local_c = FUN_10002f70(8);
      }
      else {
        *(undefined4 *)param_1 = *(undefined4 *)(&DAT_10020e88 + local_c * 0xc);
        param_1 = (undefined8 *)((int)param_1 + 4);
        uVar3 = FUN_10002f70(uVar3);
        if (uVar3 == 0xffffffff) break;
        local_c = (int)local_c >> (local_28 & 0x1f) | uVar3 << (8 - local_28 & 0x1f);
      }
    }
    else {
      local_8 = *(int *)(&DAT_10020e90 + local_c * 0xc);
      do {
        uVar3 = FUN_10003060();
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
        uVar3 = FUN_10002f70(10);
        if (-1 < (int)uVar3) {
          FUN_10001f40(param_1,uVar3);
          param_1 = (undefined8 *)((int)param_1 + uVar3 * 4);
        }
      }
      else {
        *(undefined4 *)param_1 = *(undefined4 *)(DAT_10021a9c + local_14 * 4);
        param_1 = (undefined8 *)((int)param_1 + 4);
      }
LAB_10002f46:
      local_c = FUN_10002f70(8);
    }
  }
  return (int)param_1 - (int)puVar1 >> 2;
}

// FUNCTION: CARDARTLIB 0x10002F70
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint __cdecl FUN_10002f70(uint param_1)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  byte local_c;
  
  if (DAT_1001d1dc < param_1) {
    iVar1 = param_1 - DAT_1001d1dc;
    if ((int)DAT_10021a88 + (4 - _DAT_10021a8c) < DAT_10021a90) {
      uVar2 = *DAT_10021a88;
      DAT_10021a88 = DAT_10021a88 + 1;
      local_c = (byte)DAT_1001d1dc;
      uVar3 = DAT_10020e84 |
              (*(uint *)(&DAT_10020e00 + (0x20 - iVar1) * 4) & uVar2) << (local_c & 0x1f);
      param_1 = (param_1 & 0xffffff00U) | (uint)(iVar1 & 0xff);
      DAT_10020e84 = uVar2 >> ((byte)param_1 & 0x1f);
      DAT_1001d1dc = 0x20 - iVar1;
    }
    else {
      uVar3 = 0xffffffff;
    }
  }
  else {
    uVar3 = *(uint *)(&DAT_10020e00 + (0x20 - param_1) * 4) & DAT_10020e84;
    DAT_10020e84 = DAT_10020e84 >> ((byte)param_1 & 0x1f);
    DAT_1001d1dc = DAT_1001d1dc - param_1;
  }
  return uVar3;
}

// FUNCTION: CARDARTLIB 0x10003060
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_10003060(void)

{
  uint local_8;
  
  if (DAT_1001d1dc == 0) {
    if (DAT_10021a90 <= (int)DAT_10021a88 - _DAT_10021a8c) {
      return 0xffffffff;
    }
    DAT_10020e84 = *DAT_10021a88;
    DAT_10021a88 = DAT_10021a88 + 1;
    DAT_1001d1dc = 0x20;
  }
  local_8 = (uint)((DAT_10020e84 & 1) != 0);
  DAT_10020e84 = DAT_10020e84 >> 1;
  DAT_1001d1dc = DAT_1001d1dc + -1;
  return local_8;
}

// FUNCTION: CARDARTLIB 0x100030F0
uint FUN_100030f0(undefined4 param_1,uint param_2)

{
  uint uVar1;
  
  switch(param_2) {
  case 0:
    FUN_1000343e();
    param_2 = 1;
    break;
  case 1:
    uVar1 = FUN_10003175(param_1);
    param_2 = uVar1 & 1;
    break;
  case 2:
    param_2 = 1;
    break;
  case 3:
    param_2 = 1;
  }
  return param_2;
}

// FUNCTION: CARDARTLIB 0x10003175
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl FUN_10003175(undefined4 param_1)

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
  
  FUN_1000b7a0();
  _DAT_10121fe4 = param_1;
  InitializeCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  InitializeCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  InitializeCriticalSection((LPCRITICAL_SECTION)&DAT_101221d0);
  GetModuleFileNameA((HMODULE)0x0,&DAT_10117800,0x105);
  pcVar1 = _strrchr(&DAT_10117800,0x5c);
  *pcVar1 = '\0';
  FUN_10009490((uint *)&DAT_10117910,(uint *)&DAT_10117800);
  FUN_100094a0((uint *)&DAT_10117910,(uint *)s__CARDART_1001d1e0);
  FUN_10001220();
  FUN_10009490(auStackY_4a4a4,(uint *)&DAT_10117800);
  FUN_100094a0(auStackY_4a4a4,(uint *)s__CARDS_DAT_1001d1ec);
  pFStackY_4a390 = _fopen((char *)auStackY_4a4a4,&DAT_1001d1f8);
  if (pFStackY_4a390 == (FILE *)0x0) {
    for (iStackY_4a394 = 0; iStackY_4a394 < (int)sStackY_4a39c; iStackY_4a394 = iStackY_4a394 + 1) {
      DAT_101200a0[iStackY_4a394] = 1;
    }
  }
  else {
    _fread(&sStackY_4a39c,4,1,pFStackY_4a390);
    _fread(auStackY_4a398,4,1,pFStackY_4a390);
    _fread(auStackY_4a38c,0x98,sStackY_4a39c,pFStackY_4a390);
    for (iStackY_4a394 = 0; iStackY_4a394 < (int)sStackY_4a39c; iStackY_4a394 = iStackY_4a394 + 1) {
      if (aiStackY_4a348[iStackY_4a394 * 0x26] < 1) {
        DAT_101200a0[iStackY_4a394] = 1;
      }
      else {
        DAT_101200a0[iStackY_4a394] = aiStackY_4a348[iStackY_4a394 * 0x26];
      }
    }
    _fclose(pFStackY_4a390);
  }
  for (iVar2 = 0; iVar2 < 2000; iVar2 = iVar2 + 1) {
    *(undefined4 *)(DAT_10117a40 + iVar2 * 0x10) = 0;
  }
  for (iVar2 = 0; iVar2 < 100; iVar2 = iVar2 + 1) {
    *(undefined4 *)(DAT_1011f740 + iVar2 * 0x18) = 0;
  }
  DAT_10121fe0 = 0;
  for (iVar2 = 0; iVar2 < 0x14; iVar2 = iVar2 + 1) {
    *(undefined4 *)(DAT_10121ff0 + iVar2 * 0x18) = 0;
  }
  DAT_101177f4 = 0;
  DAT_1001d258 = 3;
  return 1;
}

// FUNCTION: CARDARTLIB 0x1000343E
void FUN_1000343e(void)

{
  DestroyAllSmallArts();
  DestroyAllBigArts();
  FUN_100012a8();
  DeleteCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  DeleteCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  DeleteCriticalSection((LPCRITICAL_SECTION)&DAT_101221d0);
  return;
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
      _sprintf((char *)local_154,s__s__04d_WVL_1001d20c,&DAT_10117910,id);
    }
    else {
      _sprintf((char *)local_154,s__s__04d_c_WVL_1001d1fc,&DAT_10117910,id,
               (int)(char)((char)version + '`'));
    }
    pHVar1 = GetDesktopWindow();
    hdc = GetDC(pHVar1);
    iVar2 = GetDeviceCaps(hdc,0xc);
    iVar3 = GetDeviceCaps(hdc,0xe);
    DAT_101221e8 = iVar2 * iVar3;
    pHVar1 = GetDesktopWindow();
    ReleaseDC(pHVar1,hdc);
    local_3c = FUN_100068f0(1,local_154,0);
    if (local_3c == (int *)0x0) {
      local_40 = 0;
    }
    else {
      local_44 = GetDC((HWND)0x0);
      FUN_100012fb(local_44);
      FUN_10003c70((undefined4 *)&local_30,width,height);
      local_48 = CreateDIBSection(local_44,&local_30,0,&local_38,(HANDLE)0x0,0);
      if (local_48 == (HBITMAP)0x0) {
        local_40 = 0;
      }
      else {
        local_34 = FUN_1000807f((uint *)0x0,local_3c,width,height);
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
          FID_conflict__memcpy(local_38,local_34,(width * 3 + local_15c) * height);
        }
      }
      ReleaseDC((HWND)0x0,local_44);
      FUN_10006bb5();
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

// FUNCTION: CARDARTLIB 0x10003867
int __cdecl IsBigArtRightSize(int id,int version,int width,int height)

{
  int local_8;
  
                    /* 0x3867  8  IsBigArtRightSize */
  if (id == -1) {
    local_8 = 0;
  }
  else {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
    local_8 = IsBigArtIn(id,version);
    if ((local_8 != 0) && ((*(int *)(local_8 + 8) != width || (*(int *)(local_8 + 0xc) != height))))
    {
      local_8 = 0;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  }
  return local_8;
}

// FUNCTION: CARDARTLIB 0x100038ED
int __cdecl DrawBigArt(HDC hdc,RECT *rect,int id,int version)

{
  bool bVar1;
  HBRUSH hbr;
  int local_14;
  int local_10;
  int local_c;
  
                    /* 0x38ed  5  DrawBigArt */
  if (id == -1) {
    local_14 = 0;
  }
  else {
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
      local_14 = FUN_10001505(hdc,&rect->left,*(HANDLE *)(&DAT_10121ff0 + local_10 * 0x18));
    }
    else {
      local_14 = 0;
    }
    if (local_14 == 0) {
      hbr = GetStockObject(2);
      FillRect(hdc,rect,hbr);
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221f0);
  }
  return local_14;
}

// FUNCTION: CARDARTLIB 0x100039EF
int __cdecl ReloadBigArtIfWrongSize(int id,int version,int width,int height)

{
  int iVar1;
  
                    /* 0x39ef  12  ReloadBigArtIfWrongSize */
  if (id == -1) {
    iVar1 = 0;
  }
  else {
    iVar1 = IsBigArtRightSize(id,version,width,height);
    if (iVar1 == 0) {
      DestroyBigArt(id,version);
      iVar1 = LoadBigArt(id,version,width,height);
      if (iVar1 == 0) {
        iVar1 = 0;
      }
      else {
        iVar1 = 1;
      }
    }
    else {
      iVar1 = 1;
    }
  }
  return iVar1;
}

// FUNCTION: CARDARTLIB 0x10003A80
void __cdecl DestroyBigArt(int id,int version)

{
  bool bVar1;
  int local_10;
  int local_c;
  
                    /* 0x3a80  3  DestroyBigArt */
  if (id != -1) {
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
  return;
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

// FUNCTION: CARDARTLIB 0x10003C70
void __cdecl FUN_10003c70(undefined4 *param_1,undefined4 param_2,int param_3)

{
  *param_1 = 0x28;
  param_1[1] = param_2;
  param_1[2] = -param_3;
  *(undefined2 *)(param_1 + 3) = 1;
  *(undefined2 *)((int)param_1 + 0xe) = 0x18;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[8] = 0x100;
  param_1[9] = 0x100;
  return;
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
    _sprintf((char *)local_150,s__s__04d_WVL_1001d218,&DAT_10117910,id);
    local_3c = FUN_100068f0(0,local_150,0);
    if (local_3c == (int *)0x0) {
      local_40 = 0;
    }
    else {
      local_44 = GetDC((HWND)0x0);
      FUN_100012fb(local_44);
      FUN_10003c70((undefined4 *)&local_30,width,height);
      local_48 = CreateDIBSection(local_44,&local_30,0,&local_38,(HANDLE)0x0,0);
      if (local_48 == (HBITMAP)0x0) {
        local_40 = 0;
      }
      else {
        local_34 = FUN_1000807f((uint *)0x0,local_3c,width,height);
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
          FID_conflict__memcpy(local_38,local_34,(width * 3 + local_158) * height);
        }
      }
      ReleaseDC((HWND)0x0,local_44);
      FUN_10006bb5();
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
    local_40 = FUN_10004346(id,version,width,height);
  }
  return local_40;
}

// FUNCTION: CARDARTLIB 0x10003FB4
int __cdecl IsSmallArtIn(int id,int version)

{
  int iVar1;
  undefined *puVar2;
  
                    /* 0x3fb4  9  IsSmallArtIn */
  if (id == -1) {
    iVar1 = 0;
  }
  else if (*(int *)(&DAT_101200a0 + id * 4) < 2) {
    if (*(int *)(&DAT_10117a40 + id * 0x10) == 0) {
      iVar1 = 0;
    }
    else {
      iVar1 = 1;
    }
  }
  else {
    puVar2 = FUN_100045fe(id,version);
    if (puVar2 == (undefined *)0x0) {
      iVar1 = 0;
    }
    else {
      iVar1 = 1;
    }
  }
  return iVar1;
}

// FUNCTION: CARDARTLIB 0x1000403D
int __cdecl DrawSmallArt(HDC hdc,RECT *rect,int id,int version)

{
  int iVar1;
  HBRUSH hbr;
  int local_8;
  
                    /* 0x403d  6  DrawSmallArt */
  if ((hdc == (HDC)0x0) || (rect == (RECT *)0x0)) {
    local_8 = 0;
  }
  else if (id == -1) {
    local_8 = 0;
  }
  else if (*(int *)(&DAT_101200a0 + id * 4) < 2) {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    iVar1 = IsSmallArtIn(id,version);
    if (iVar1 == 0) {
      local_8 = 0;
    }
    else {
      local_8 = FUN_10001505(hdc,&rect->left,*(HANDLE *)(&DAT_10117a40 + id * 0x10));
    }
    if (local_8 == 0) {
      hbr = GetStockObject(2);
      FillRect(hdc,rect,hbr);
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  }
  else {
    local_8 = FUN_100046aa(hdc,rect,id,version);
  }
  return local_8;
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
    local_c = FUN_100047ab(id,version,width,height);
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
      FUN_1000485a(id,version);
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
  FUN_100049e4();
  for (local_8 = 0; local_8 < 2000; local_8 = local_8 + 1) {
    if (*(int *)(&DAT_101200a0 + local_8 * 4) < 2) {
      DestroySmallArt(local_8,0);
    }
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  return;
}

// FUNCTION: CARDARTLIB 0x10004346
int __cdecl FUN_10004346(int id,int version,int width,int height)

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
    local_8 = FUN_100045fe(id,version);
    if (local_8 != (undefined *)0x0) {
      if ((*(int *)(local_8 + 8) == width) && (*(int *)(local_8 + 0xc) == height)) {
        LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
        return 1;
      }
      FUN_1000485a(id,version);
    }
    if (version == 0) {
      _sprintf((char *)local_154,s__s__04d_WVL_1001d234,&DAT_10117910,id);
    }
    else {
      _sprintf((char *)local_154,s__s__04d_c_WVL_1001d224,&DAT_10117910,id,
               (int)(char)((char)version + '`'));
    }
    local_40 = FUN_100068f0(0,local_154,0);
    if (local_40 == (int *)0x0) {
      local_44 = 0;
    }
    else {
      local_48 = GetDC((HWND)0x0);
      FUN_100012fb(local_48);
      FUN_10003c70((undefined4 *)&local_34,width,height);
      local_4c = CreateDIBSection(local_48,&local_34,0,&local_3c,(HANDLE)0x0,0);
      if (local_4c == (HBITMAP)0x0) {
        local_44 = 0;
      }
      else {
        local_38 = FUN_1000807f((uint *)0x0,local_40,width,height);
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
          FID_conflict__memcpy(local_3c,local_38,(width * 3 + local_158) * height);
        }
      }
      ReleaseDC((HWND)0x0,local_48);
      FUN_10006bb5();
    }
    if (local_44 != 0) {
      *(HBITMAP *)(&DAT_1011f740 + DAT_10121fe0 * 0x18) = local_4c;
      *(void **)(&DAT_1011f744 + DAT_10121fe0 * 0x18) = local_3c;
      *(int *)(&DAT_1011f748 + DAT_10121fe0 * 0x18) = width;
      *(int *)(&DAT_1011f74c + DAT_10121fe0 * 0x18) = height;
      *(int *)(&DAT_1011f750 + DAT_10121fe0 * 0x18) = id;
      *(int *)(&DAT_1011f754 + DAT_10121fe0 * 0x18) = version;
      DAT_10121fe0 = DAT_10121fe0 + 1;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  }
  return local_44;
}

// FUNCTION: CARDARTLIB 0x100045FE
undefined * __cdecl FUN_100045fe(int param_1,int param_2)

{
  int local_c;
  undefined *local_8;
  
  local_8 = (undefined *)0x0;
  if (param_1 == -1) {
    local_8 = (undefined *)0x0;
  }
  else {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    local_c = 0;
    while ((local_c < DAT_10121fe0 && (local_8 == (undefined *)0x0))) {
      if ((*(int *)(&DAT_1011f750 + local_c * 0x18) == param_1) &&
         (*(int *)(&DAT_1011f754 + local_c * 0x18) == param_2)) {
        local_8 = &DAT_1011f740 + local_c * 0x18;
      }
      local_c = local_c + 1;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  }
  return local_8;
}

// FUNCTION: CARDARTLIB 0x100046AA
int __cdecl FUN_100046aa(HDC param_1,RECT *param_2,int param_3,int param_4)

{
  bool bVar1;
  HBRUSH hbr;
  int local_14;
  int local_10;
  int local_c;
  
  if (param_3 == -1) {
    local_14 = 0;
  }
  else {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    local_c = 0;
    bVar1 = false;
    while ((local_c < DAT_10121fe0 && (!bVar1))) {
      if ((*(int *)(&DAT_1011f750 + local_c * 0x18) == param_3) &&
         (*(int *)(&DAT_1011f754 + local_c * 0x18) == param_4)) {
        bVar1 = true;
        local_10 = local_c;
      }
      local_c = local_c + 1;
    }
    if (bVar1) {
      local_14 = FUN_10001505(param_1,&param_2->left,*(HANDLE *)(&DAT_1011f740 + local_10 * 0x18));
    }
    else {
      local_14 = 0;
    }
    if (local_14 == 0) {
      hbr = GetStockObject(2);
      FillRect(param_1,param_2,hbr);
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  }
  return local_14;
}

// FUNCTION: CARDARTLIB 0x100047AB
undefined4 __cdecl FUN_100047ab(int id,int version,int width,int height)

{
  undefined4 uVar1;
  undefined *puVar2;
  int iVar3;
  
  if (id == -1) {
    uVar1 = 0;
  }
  else {
    puVar2 = FUN_100045fe(id,version);
    if (puVar2 != (undefined *)0x0) {
      if ((*(int *)(puVar2 + 8) == width) && (*(int *)(puVar2 + 0xc) == height)) {
        return 1;
      }
      FUN_1000485a(id,version);
    }
    iVar3 = FUN_10004346(id,version,width,height);
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
void __cdecl FUN_1000485a(int id,int version)

{
  bool bVar1;
  int local_10;
  int local_c;
  
  if (id != -1) {
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
    local_c = 0;
    bVar1 = false;
    while ((local_c < DAT_10121fe0 && (!bVar1))) {
      if ((*(int *)(&DAT_1011f750 + local_c * 0x18) == id) &&
         (*(int *)(&DAT_1011f754 + local_c * 0x18) == version)) {
        bVar1 = true;
        if (*(int *)(&DAT_1011f740 + local_c * 0x18) != 0) {
          DeleteObject(*(HGDIOBJ *)(&DAT_1011f740 + local_c * 0x18));
        }
        DAT_10121fe0 = DAT_10121fe0 + -1;
        for (local_10 = local_c; local_10 < DAT_10121fe0; local_10 = local_10 + 1) {
          *(undefined4 *)(&DAT_1011f740 + local_10 * 0x18) =
               *(undefined4 *)(&DAT_1011f740 + (local_10 * 3 + 3) * 8);
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
void FUN_100049e4(void)

{
  int local_8;
  
  EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  for (local_8 = 0; local_8 < DAT_10121fe0; local_8 = local_8 + 1) {
    DeleteObject(*(HGDIOBJ *)(&DAT_1011f740 + local_8 * 0x18));
  }
  DAT_10121fe0 = 0;
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10117a20);
  return;
}

// FUNCTION: CARDARTLIB 0x10004A50
void * FUN_10004a50(void)

{
  void *_Dst;
  
  _Dst = _malloc(0x30);
  _memset(_Dst,0,0x30);
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
  local_18 = _fopen(param_1,DAT_1001e064);
  if (local_18 == (FILE *)0x0) {
    local_8 = (undefined2 *)0x0;
  }
  else {
    if (DAT_10031ea4 != (int *)0x0) {
      FUN_10004fe1(DAT_10031ea4);
    }
    DAT_10031ea4 = FUN_10004a50();
    _fgets(local_120,0xff,local_18);
    while ((local_18->_flag & 0x10) == 0) {
      _sscanf(local_120,s__d____d__d__d_1001e068,&local_20,&local_10,&local_1c,&local_c);
      local_14 = _strchr(local_120,0x2d);
      local_14 = local_14 + 1;
      local_14 = _strchr(local_14,0x2d);
      local_14 = local_14 + 1;
      FUN_10004f07(DAT_10031ea4,local_14,local_20);
      *(uint *)(DAT_10031ec0 + local_20 * 4) = local_10 << 0x10 | local_1c << 8 | local_c;
      *(undefined1 *)(local_8 + local_20 * 2 + 2) = (undefined1)local_10;
      *(undefined1 *)((int)local_8 + local_20 * 4 + 5) = (undefined1)local_1c;
      *(undefined1 *)(local_8 + local_20 * 2 + 3) = (undefined1)local_c;
      if ((local_20 == 0) || (local_20 == 0xff)) {
        *(undefined1 *)((int)local_8 + local_20 * 4 + 7) = 0;
      }
      else {
        *(undefined1 *)((int)local_8 + local_20 * 4 + 7) = 1;
      }
      _fgets(local_120,0xff,local_18);
    }
    DAT_100322d4 = 0;
    DAT_1001d240 = 0;
    _DAT_10031eb8 = FUN_10004dc8(DAT_10031ea4);
    DAT_1001d240 = DAT_1001d240 + -1;
    local_8[1] = 0x100;
    _fclose(local_18);
    local_18 = (FILE *)0x0;
    if (param_2 != (char *)0x0) {
      local_18 = _fopen(param_2,DAT_1001e078);
    }
    if (local_18 != (FILE *)0x0) {
      _fread(&DAT_10116cf0,0x404,1,local_18);
      _fclose(local_18);
    }
    *(undefined1 *)((int)local_8 + 0x403) = 0;
    *(undefined1 *)((int)local_8 + 7) = *(undefined1 *)((int)local_8 + 0x403);
    FUN_100050f1();
    FUN_10004cd7();
  }
  return local_8;
}

// FUNCTION: CARDARTLIB 0x10004CD7
undefined4 FUN_10004cd7(void)

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
void __cdecl FUN_10004d49(int *param_1,int param_2,int *param_3)

{
  int local_8;
  
  if (*param_1 == 0) {
    for (local_8 = 0; local_8 < 8; local_8 = local_8 + 1) {
      if (param_1[local_8 + 2] != 0) {
        FUN_10004d49((int *)param_1[local_8 + 2],param_2,param_3);
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
int __cdecl FUN_10004dc8(int *param_1)

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
        iVar1 = FUN_10004dc8((int *)param_1[local_40c + 2]);
        local_408 = local_408 + iVar1;
        local_410 = local_410 + 1;
      }
    }
    if (local_410 != 0) {
      local_410 = 0;
      FUN_10004d49(param_1,(int)local_404,(int *)&local_410);
      pvVar2 = _malloc(local_410);
      param_1[10] = (int)pvVar2;
      param_1[0xb] = local_410;
      FID_conflict__memcpy((void *)param_1[10],local_404,local_410);
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
undefined4 __cdecl FUN_10004f07(undefined4 *param_1,char *param_2,undefined4 param_3)

{
  size_t sVar1;
  int iVar2;
  void *pvVar3;
  size_t sVar4;
  size_t sVar5;
  char *_Control;
  
  sVar1 = _strspn(param_2,DAT_1001e07c);
  for (param_2 = param_2 + sVar1; *param_2 != '\0'; param_2 = param_2 + sVar1 + sVar4 + sVar5) {
    iVar2 = _atoi(param_2);
    if (param_1[iVar2 + 2] == 0) {
      pvVar3 = FUN_10004a50();
      param_1[iVar2 + 2] = pvVar3;
    }
    param_1 = (undefined4 *)param_1[iVar2 + 2];
    _Control = DAT_1001e084;
    sVar1 = _strspn(param_2,DAT_1001e088);
    sVar1 = _strcspn(param_2 + sVar1,_Control);
    sVar4 = _strspn(param_2,DAT_1001e080);
    sVar5 = _strspn(param_2 + sVar1 + sVar4,DAT_1001e08c);
  }
  *param_1 = 1;
  param_1[1] = param_3;
  return 0;
}

// FUNCTION: CARDARTLIB 0x10004FE1
int __cdecl FUN_10004fe1(int *param_1)

{
  int iVar1;
  int local_c;
  int local_8;
  
  local_8 = 0;
  if (*param_1 == 0) {
    for (local_c = 0; local_c < 8; local_c = local_c + 1) {
      if (param_1[local_c + 2] != 0) {
        iVar1 = FUN_10004fe1((int *)param_1[local_c + 2]);
        local_8 = local_8 + iVar1;
      }
    }
    if (param_1[10] != 0) {
      FUN_1000a210((void *)param_1[10]);
    }
    FUN_1000a210(param_1);
  }
  else {
    FUN_1000a210(param_1);
    local_8 = 1;
  }
  return local_8;
}

// FUNCTION: CARDARTLIB 0x1000508D
void __cdecl FUN_1000508d(uint param_1,uint *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  iVar1 = ((param_1 & 0xff0000) >> 0x10) * 8;
  uVar4 = *(uint *)(PTR_DAT_1001d254 + iVar1 + 4);
  iVar2 = (param_1 >> 8 & 0xff) * 8;
  uVar5 = *(uint *)(PTR_DAT_1001d250 + iVar2 + 4);
  iVar3 = (param_1 & 0xff) * 8;
  uVar6 = *(uint *)(PTR_DAT_1001d24c + iVar3 + 4);
  *param_2 = *(uint *)(PTR_DAT_1001d254 + iVar1) | *(uint *)(PTR_DAT_1001d250 + iVar2) |
             *(uint *)(PTR_DAT_1001d24c + iVar3);
  param_2[1] = uVar4 | uVar5 | uVar6;
  return;
}

// FUNCTION: CARDARTLIB 0x100050F1
undefined4 FUN_100050f1(void)

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
undefined4 __cdecl FUN_100051f5(uint param_1)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  byte *pbVar5;
  int local_34;
  uint local_2c;
  int local_28;
  
  FUN_1000508d(param_1,(uint *)&DAT_100322c0);
  pbVar5 = &DAT_100322c0;
  piVar3 = DAT_10031ea4;
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
                          (uint)DAT_10031ec0[(uint)*(byte *)(local_28 + iVar2) * 4 + 1]) * 4) +
                  *(int *)(PTR_DAT_1001d244 +
                          ((param_1 & 0xff) -
                          (*(uint *)(DAT_10031ec0 + (uint)*(byte *)(local_28 + iVar2) * 4) & 0xff)) *
                          4) +
                  *(int *)(PTR_DAT_1001d244 +
                          (((param_1 & 0xff0000) >> 0x10) -
                          ((*(uint *)(DAT_10031ec0 + (uint)*(byte *)(local_28 + iVar2) * 4) & 0xff0000)
                           >> 0x10)) * 4);
          if (iVar4 < local_34) {
            local_2c = (uint)*(byte *)(local_28 + iVar2);
            local_34 = iVar4;
          }
        }
        return *(undefined4 *)(DAT_10031ec0 + local_2c * 4);
      }
      return *(undefined4 *)(DAT_10031ec0 + piVar3[1] * 4);
    }
    piVar3 = piVar1;
  } while ((char)*piVar1 != '\x01');
  return *(undefined4 *)(DAT_10031ec0 + piVar1[1] * 4);
}

// FUNCTION: CARDARTLIB 0x10005383
uint __cdecl FUN_10005383(uint param_1)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  byte *pbVar5;
  int local_34;
  uint local_2c;
  int local_28;
  
  FUN_1000508d(param_1,(uint *)&DAT_10031eb0);
  pbVar5 = &DAT_10031eb0;
  piVar3 = DAT_10031ea4;
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
                          ((*(uint *)(DAT_10031ec0 + (uint)*(byte *)(local_28 + iVar2) * 4) & 0xff0000)
                           >> 0x10)) * 4) +
                  *(int *)(PTR_DAT_1001d244 +
                          (((param_1 & 0xff0000) >> 0x10) -
                          (*(uint *)(DAT_10031ec0 + (uint)*(byte *)(local_28 + iVar2) * 4) & 0xff)) *
                          4) +
                  *(int *)(PTR_DAT_1001d244 +
                          ((param_1 >> 8 & 0xff) -
                          (uint)DAT_10031ec0[(uint)*(byte *)(local_28 + iVar2) * 4 + 1]) * 4);
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
int __cdecl FUN_100054fb(int *param_1,int *param_2)

{
  int iVar1;
  int local_c;
  int local_8;
  
  local_8 = 0;
  if (*param_1 == 0) {
    for (local_c = 0; local_c < 8; local_c = local_c + 1) {
      if (param_1[local_c + 2] != 0) {
        iVar1 = FUN_100054fb((int *)param_1[local_c + 2],param_2);
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
undefined4 __cdecl FUN_10005591(uint *param_1,int param_2,int param_3,int param_4)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 local_18;
  undefined4 local_10;
  undefined4 local_c;
  
  for (local_10 = 0; local_10 < param_2; local_10 = local_10 + 1) {
    local_c = 0;
    local_18 = *param_1;
    for (; local_c < param_3 * 3; local_c = local_c + 3) {
      uVar1 = *(uint *)(local_c + 3 + (int)param_1);
      uVar2 = FUN_100051f5(local_18);
      *(undefined4 *)(local_c + (int)param_1) = uVar2;
      local_18 = uVar1;
    }
    param_1 = (uint *)((int)param_1 + param_4 + param_3 * 3);
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x10005629
undefined4 __cdecl FUN_10005629(uint *param_1,int param_2,int param_3,int param_4)

{
  uint uVar1;
  uint uVar2;
  uint local_20;
  int local_18;
  int local_14;
  uint *local_10;
  int local_8;
  
  local_10 = param_1;
  for (local_18 = 0; local_18 < param_2; local_18 = local_18 + 1) {
    local_8 = 0;
    local_14 = 0;
    local_20 = *param_1;
    for (; local_14 < param_3 * 3; local_14 = local_14 + 3) {
      uVar1 = local_20 & 0xffffff;
      local_20 = *(uint *)(local_14 + 3 + (int)param_1);
      uVar2 = FUN_10005383(uVar1);
      *(char *)(local_8 + (int)local_10) = (char)uVar2;
      if (*(uint *)(&DAT_10031ec0 + (uint)*(byte *)(local_8 + (int)local_10) * 4) != uVar1) {
        *(undefined4 *)(&DAT_10031ec0 + (uint)*(byte *)(local_8 + (int)local_10) * 4) = 0;
      }
      local_8 = local_8 + 1;
    }
    param_1 = (uint *)((int)param_1 + param_4 + param_3 * 3);
    local_10 = (uint *)((int)local_10 + param_4 + param_3);
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x1000570F
/* WARNING: Type propagation algorithm not settling */

int __cdecl FUN_1000570f(int param_1,int param_2,uint *param_3,int param_4,int param_5,int param_6)

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
  local_7c = &DAT_1001d2d8 + param_1 * 0xc0;
  local_18 = param_5 * 8 + 0x50U >> 2;
  if (param_1 == 0) {
    DAT_1001e05c = param_1;
    param_4 = 0;
  }
  else if (param_1 == 1) {
    DAT_1001e05c = param_1;
    param_4 = FUN_10005591(param_3,param_4,param_5,param_6);
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
    if (param_1 != DAT_1001e05c) {
      for (local_4c = 0; local_4c < 0x41; local_4c = local_4c + 1) {
        if (DAT_10117100[local_4c] != (void *)0x0) {
          FUN_1000a210(DAT_10117100[local_4c]);
          DAT_10117100[local_4c] = (void *)0x0;
        }
      }
      FUN_10005cf5(param_1,(int)DAT_10117100);
      DAT_1001e05c = param_1;
    }
    for (local_4c = 0; local_4c < 5; local_4c = local_4c + 1) {
      FUN_10007273((undefined8 *)(DAT_100edb10 + local_4c * 0x8060),0,0x8060);
      local_70[local_4c + 1] = (undefined *)(DAT_100edb10 + local_4c * 0x8060 + 0x28);
    }
    iVar1 = DAT_1001d260[param_1];
    for (local_58 = 0; local_58 < param_4; local_58 = local_58 + 1) {
      if (local_3c < 1) {
        local_80 = param_5 + -1;
        local_1c = -1;
        local_88 = -3;
      }
      else {
        local_80 = 0;
        local_1c = param_5;
        local_88 = 3;
      }
      local_c = local_80 * 3;
      for (local_4c = local_80; local_4c != local_1c; local_4c = local_4c + local_3c) {
        uVar2 = *(uint *)(local_c + (int)param_3);
        local_28 = uVar2 & 0xffffff;
        *(uint *)(local_c + (int)param_3) = *(uint *)(local_c + (int)param_3) & 0xff000000;
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
          local_84 = FUN_100051f5(local_28);
        }
        *(uint *)(local_c + (int)param_3) = *(uint *)(local_c + (int)param_3) | local_84;
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
      FUN_10005cb4(local_70 + 1,DAT_1001d288[param_1]);
      _memset(local_70[DAT_1001d288[param_1]] + -0x28,0,local_18 << 2);
      if (param_2 != 0) {
        local_3c = -local_3c;
        local_7c = &DAT_1001d2d8 + (uint)(local_3c == -1) * 0x6c0 + param_1 * 0xc0;
      }
      param_3 = (uint *)((int)param_3 + param_5 * 3 + param_6);
    }
  }
  return param_4;
}

// FUNCTION: CARDARTLIB 0x10005CB4
void __cdecl FUN_10005cb4(undefined4 *param_1,int param_2)

{
  undefined4 uVar1;
  
  uVar1 = *param_1;
  FID_conflict__memcpy(param_1,param_1 + 1,param_2 * 4 - 4);
  param_1[param_2 + -1] = uVar1;
  return;
}

// FUNCTION: CARDARTLIB 0x10005CF5
undefined4 __cdecl FUN_10005cf5(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  void *pvVar3;
  int local_14;
  int local_10;
  
  iVar1 = DAT_1001d2b0[param_1];
  for (local_10 = 0; local_10 < DAT_1001d260[param_1]; local_10 = local_10 + 1) {
    iVar2 = *(int *)(&DAT_1001d2d8 + local_10 * 0x10 + param_1 * 0xc0);
    if (*(int *)(param_2 + iVar2 * 4) == 0) {
      pvVar3 = _malloc(0x800);
      *(void **)(param_2 + iVar2 * 4) = pvVar3;
      assert((uint)(*(int *)(param_2 + iVar2 * 4) != 0),s_D__Newmagic_sources_NedCard_Pale_1001e0b4,
             0x4fd,s_Not_enough_memory_for_delta_arra_1001e090);
      for (local_14 = -0x100; local_14 < 0x100; local_14 = local_14 + 1) {
        *(int *)(*(int *)(param_2 + iVar2 * 4) + 0x400 + local_14 * 4) =
             ((iVar2 * local_14 + (iVar1 >> 1)) * 0x100) / DAT_1001d2b0[param_1];
      }
      *(int *)(&DAT_1001d2e4 + local_10 * 0x10 + param_1 * 0xc0) =
           *(int *)(param_2 + iVar2 * 4) + 0x3fc;
    }
    else {
      *(int *)(&DAT_1001d2e4 + local_10 * 0x10 + param_1 * 0xc0) =
           *(int *)(param_2 + iVar2 * 4) + 0x400;
    }
  }
  for (local_10 = 0; local_10 < DAT_1001d260[param_1]; local_10 = local_10 + 1) {
    *(int *)(&DAT_1001d9a4 + local_10 * 0x10 + param_1 * 0xc0) =
         *(int *)(param_2 + *(int *)(&DAT_1001d998 + local_10 * 0x10 + param_1 * 0xc0) * 4) + 0x3fc;
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
  _Dst_00 = _malloc((_Size + param_5) * param_4 + 0x10);
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
  FID_conflict__memcpy(_Dst_00,param_2,_Size);
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
  FID_conflict__memcpy(local_1c,param_2,_Size);
  FID_conflict__memcpy(_Dst,_Dst_00,(_Size + param_5) * param_4);
  FUN_1000a210(_Dst_00);
  return 0;
}

// FUNCTION: CARDARTLIB 0x100062C7
undefined4 __cdecl
FUN_100062c7(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6)

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
  local_7c = &DAT_1001d2d8 + param_1 * 0xc0;
  local_18 = param_5 * 8 + 0x50U >> 2;
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
  if (DAT_1001e060 != param_1) {
    for (local_4c = 0; local_4c < 0x41; local_4c = local_4c + 1) {
      if (*(int *)(&DAT_10117100 + local_4c * 4) != 0) {
        FUN_1000a210(*(void **)(&DAT_10117100 + local_4c * 4));
        *(undefined4 *)(&DAT_10117100 + local_4c * 4) = 0;
      }
    }
    FUN_10005cf5(param_1,0x10117100);
    DAT_1001e060 = param_1;
  }
  for (local_4c = 0; local_4c < 5; local_4c = local_4c + 1) {
    _memset(&DAT_100edb10 + local_4c * 0x8060,0,0x8060);
    local_70[local_4c + 1] = local_4c * 0x8060 + 0x100edb38;
  }
  iVar2 = *(int *)(&DAT_1001d260 + param_1 * 4);
  for (local_58 = 0; local_58 < param_4; local_58 = local_58 + 1) {
    if (local_3c < 1) {
      local_80 = param_5 + -1;
      local_1c = -1;
      local_88 = -3;
    }
    else {
      local_80 = 0;
      local_1c = param_5;
      local_88 = 3;
    }
    local_c = local_80 * 3;
    for (local_4c = local_80; local_4c != local_1c; local_4c = local_4c + local_3c) {
      uVar3 = *(uint *)(local_c + param_3);
      uVar5 = uVar3 & 0xffffff;
      *(uint *)(local_c + param_3) = *(uint *)(local_c + param_3) & 0xff000000;
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
        local_84 = FUN_100068d0(local_28);
      }
      *(uint *)(local_c + param_3) = *(uint *)(local_c + param_3) | local_84;
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
    FUN_10005cb4(local_70 + 1,*(int *)(&DAT_1001d288 + param_1 * 4));
    _memset((void *)(local_70[*(int *)(&DAT_1001d288 + param_1 * 4)] + -0x28),0,local_18 << 2);
    if (param_2 != 0) {
      local_3c = -local_3c;
      local_7c = &DAT_1001d2d8 + (uint)(local_3c == -1) * 0x6c0 + param_1 * 0xc0;
    }
    param_3 = param_3 + param_5 * 3 + param_6;
  }
  return 1;
}

// FUNCTION: CARDARTLIB 0x10006809
void FUN_10006809(void)

{
  FUN_10004fe1(DAT_10031ea4);
  DAT_10031ea4 = (int *)0x0;
  return;
}

// FUNCTION: CARDARTLIB 0x1000682C
int __cdecl FUN_1000682c(int param_1,int param_2,int param_3,byte *param_4)

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

// FUNCTION: CARDARTLIB 0x100068D0
uint __cdecl FUN_100068d0(uint param_1)

{
  return param_1 & 0xf8f8f8;
}

// FUNCTION: CARDARTLIB 0x100068F0
int * __cdecl FUN_100068f0(int param_1,uint *param_2,int param_3)

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
  __splitpath((char *)param_2,(char *)0x0,(char *)local_314,(char *)local_210,(char *)local_110);
  if (DAT_10032adc == 0) {
    FUN_10009490(local_41c,local_314);
    FUN_100094a0(local_41c,(uint *)s_SmallArt_cat_1001e12c);
    DAT_1001e11c = FUN_100019d0(local_41c);
    FUN_10009490(local_41c,local_314);
    FUN_100094a0(local_41c,(uint *)s_MedArt_cat_1001e13c);
    DAT_1001e120 = FUN_100019d0(local_41c);
    DAT_10032adc = 1;
  }
  sVar1 = _strlen((char *)local_314);
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
  FUN_10009490(local_314,local_210);
  FUN_100094a0(local_314,local_110);
  __strlwr((char *)local_314);
  if (local_c != (int *)0x0) {
    _memset(local_c,0,0x1b0);
    FUN_10009490((uint *)(local_c + 0x27),param_2);
    local_c[0x68] = (int)&DAT_100ad498;
    local_10 = FUN_10001da1(DAT_100ea098,(char *)local_314,local_c + 0x68);
    if (local_10 == 0xffffffff) {
      FUN_100094a0(param_2,(uint *)&DAT_1001e148);
      OutputDebugStringA((LPCSTR)param_2);
      LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221d0);
      local_c = (int *)0x0;
    }
    else {
      local_c[0x69] = local_10 - 0x9c;
      FID_conflict__memcpy(local_c,(void *)local_c[0x68],0x9c);
      local_c[0x68] = local_c[0x68] + 0x9c;
      if (local_c[10] == 4) {
        local_c[7] = local_c[7] << 1;
        local_c[8] = local_c[8] << 1;
      }
      if (param_3 != 0) {
        puVar2 = FUN_10006be3(local_c,(undefined8 *)0x0);
        local_c[0x6b] = (int)puVar2;
        if (local_c[0x6b] == 0) {
          FUN_10006bb5();
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
undefined4 FUN_10006bb5(void)

{
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_101221d0);
  return 0;
}

// FUNCTION: CARDARTLIB 0x10006BE3
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 * __cdecl FUN_10006be3(int *param_1,undefined8 *param_2)

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
  
  if (DAT_10032ae0 == 0) {
    for (local_4c = -0x400; local_4c < 0x401; local_4c = local_4c + 1) {
      if ((local_4c < 0) || (0xf8 < local_4c)) {
        if (local_4c < 10) {
          PTR_DAT_1001e118[local_4c] = 0;
        }
        else {
          PTR_DAT_1001e118[local_4c] = 0xff;
        }
      }
      else {
        PTR_DAT_1001e118[local_4c] = (char)((local_4c * 0xff) / 0xf8);
      }
    }
    DAT_10032ae0 = 1;
  }
  bVar8 = param_2 != (undefined8 *)0x0;
  if (bVar8) {
    FUN_10001f40(param_2,(int)(param_1[0x24] + 2000 + (param_1[0x24] + 2000 >> 0x1f & 3U)) >> 2);
  }
  else {
    param_2 = _malloc(param_1[0x24] + 2000);
    FUN_10001f40(param_2,(int)(param_1[0x24] + 2000 + (param_1[0x24] + 2000 >> 0x1f & 3U)) >> 2);
  }
  _DAT_100ecb04 = FUN_100078b8((int)param_2,param_1);
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
    FUN_100072dc(piVar5,iVar3,iVar2);
    FUN_100072dc(piVar6,local_2c,iVar2);
    FUN_100072dc(piVar7,local_2c,iVar2);
    if (local_48 < param_1[10] / 2) {
      local_50 = (undefined8 *)
    FUN_100075ef(DAT_10032c98,piVar5,iVar3,iVar3,(int)piVar6,(int)piVar7,local_2c,
                              local_2c,*param_1);
    }
    else if (param_1[10] < 2) {
      local_50 = (undefined8 *)
    FUN_100075ef(DAT_10032c98,piVar5,iVar3,iVar4,(int)piVar6,(int)piVar7,local_2c,
                              local_34,*param_1);
    }
    else {
      local_50 = (undefined8 *)
    FUN_100075ef(DAT_10032c98,piVar5,iVar3,param_1[8] - iVar3,(int)piVar6,(int)piVar7,
                              local_2c,local_34,*param_1);
    }
    if (param_1[10] < 2) {
      if (!bVar8) {
        FUN_1000a210(param_2);
      }
      param_2 = local_50;
    }
    else {
      FUN_10007035(param_2,local_50,(local_48 % local_8) * (iVar1 / local_8),
                   (local_48 / local_8) * (iVar1 / local_8),iVar3,iVar3,iVar1);
      FUN_1000a210(local_50);
    }
  }
  return param_2;
}

// FUNCTION: CARDARTLIB 0x10007035
void __cdecl
FUN_10007035(undefined8 *param_1,undefined8 *param_2,int param_3,int param_4,int param_5,int param_6
            ,int param_7)

{
  undefined4 local_8;
  
  param_1 = (undefined8 *)((int)param_1 + (param_7 * param_4 + param_3) * 3);
  for (local_8 = 0; local_8 < param_6; local_8 = local_8 + 1) {
    FUN_10007238(param_1,param_2,param_5 * 3);
    param_1 = (undefined8 *)((int)param_1 + param_7 * 3);
    param_2 = (undefined8 *)((int)param_2 + param_5 * 3);
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
void __cdecl FUN_10007238(undefined8 *param_1,undefined8 *param_2,uint param_3)

{
  uint uVar1;
  
  for (uVar1 = param_3 >> 3; uVar1 != 0; uVar1 = uVar1 - 1) {
    *param_1 = *param_2;
    param_2 = param_2 + 1;
    param_1 = param_1 + 1;
  }
  uVar1 = param_3 & 7;
  if (uVar1 != 0) {
    for (; uVar1 != 0; uVar1 = uVar1 - 1) {
      *(undefined1 *)param_1 = *(undefined1 *)param_2;
      param_2 = (undefined8 *)((int)param_2 + 1);
      param_1 = (undefined8 *)((int)param_1 + 1);
    }
  }
  return;
}

// FUNCTION: CARDARTLIB 0x10007273
void __cdecl FUN_10007273(undefined8 *param_1,uint param_2,uint param_3)

{
  //TODO: _memset((void *)param_1,(int)(param_2 & 0xff),param_3);
  undefined8 uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  longlong lVar5;
  
  uVar2 = param_2 << 8 | param_2;
  uVar4 = (int)uVar2 >> 0x1f | ((int)uVar2 >> 0x1f) << 0x10 | uVar2 >> 0x10;
  lVar5 = _allshl(0x20,uVar4);
  uVar1 = CONCAT44(uVar4 | (uint)((ulonglong)lVar5 >> 0x20),uVar2 | uVar2 << 0x10 | (uint)lVar5);
  iVar3 = (param_3 >> 3) - 1;
  do {
    *param_1 = uVar1;
    param_1 = param_1 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  *param_1 = uVar1;
  for (uVar4 = param_3 & 7; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(char *)param_1 = (char)param_2;
    param_1 = (undefined8 *)((int)param_1 + 1);
  }
  return;
}

// FUNCTION: CARDARTLIB 0x100072DC
void __cdecl FUN_100072dc(int *param_1,int param_2,int param_3)

{
  int *piVar1;
  int *local_18;
  int local_10;
  
  if (DAT_1001e124 == 0) {
    local_18 = _malloc(0x32000);
    DAT_100ea320 = local_18;
    DAT_100ea31c = _malloc(0x32000);
    DAT_1001e124 = 1;
  }
  else {
    local_18 = DAT_100ea320;
  }
  piVar1 = DAT_100ea31c;
  for (local_10 = param_3; local_10 < param_2; local_10 = local_10 << 1) {
    FUN_10007419(param_1,param_1 + local_10 * local_10,local_18,local_10,local_10,local_10 * 2,
                 local_10);
    FUN_10007419(param_1 + local_10 * local_10 * 2,param_1 + local_10 * local_10 * 3,piVar1,local_10
                 ,local_10,local_10 * 2,local_10);
    FUN_100074ee(local_18,piVar1,param_1,local_10,local_10 * 2,local_10 * 2,local_10 * 2);
  }
  return;
}

// FUNCTION: CARDARTLIB 0x10007419
void __cdecl
FUN_10007419(int *param_1,int *param_2,int *param_3,int param_4,int param_5,undefined4 param_6,
            int param_7)

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
FUN_100074ee(int *param_1,int *param_2,int *param_3,int param_4,int param_5,undefined4 param_6,
            int param_7)

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
FUN_100075ef(undefined1 *param_1,int *param_2,int param_3,int param_4,int param_5,int param_6,
            int param_7,undefined4 param_8,int param_9)

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
  
  if (DAT_100ea09c == 0) {
    for (local_1c = -0x400; (int)local_1c < 0x1c00; local_1c = local_1c + 1) {
      if ((int)local_1c < 1) {
        PTR_DAT_1001e128[local_1c] = 0;
      }
      else {
        iVar2 = (int)local_1c >> 2;
        if (0xfe < iVar2) {
          iVar2 = 0xff;
        }
        PTR_DAT_1001e128[local_1c] = (char)iVar2;
      }
    }
    DAT_100ea09c = 1;
  }
  if (param_1 == (undefined1 *)0x0) {
    param_1 = _malloc(param_3 * param_3 * 3 + 0x10);
  }
  puVar1 = param_1;
  for (local_20 = 0; local_20 < param_4; local_20 = local_20 + 1) {
    iVar2 = local_20;
    if (param_9 != 0) {
      iVar2 = local_20 / 2;
    }
    local_10 = (int *)(iVar2 * param_7 * 4 + param_6);
    local_14 = (int *)(iVar2 * param_7 * 4 + param_5);
    for (local_1c = 0; (int)local_1c < param_3; local_1c = local_1c + 1) {
      iVar2 = *param_2;
      if (param_9 == 0) {
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
          local_2c = (local_14[param_3 - 1U != local_1c] + *local_14) / 2;
          local_28 = (local_10[param_3 - 1U != local_1c] + *local_10) / 2;
        }
        local_28 = (local_28 >> 3) + (local_28 >> 1) + local_28;
      }
      local_8 = local_2c * 2 + -0x400 + iVar2;
      local_c = local_28 + -0x333 + iVar2;
      *param_1 = PTR_DAT_1001e128[local_8];
      param_1[1] = PTR_DAT_1001e128
                   [((iVar2 * 2 - (iVar2 >> 2)) - (local_c >> 1)) -
                    ((local_8 >> 2) - (local_8 >> 4))];
      param_1[2] = PTR_DAT_1001e128[local_c];
      if (param_9 == 0) {
        local_14 = local_14 + 1;
        local_10 = local_10 + 1;
      }
      else if ((local_1c & 1) != 0) {
        local_14 = local_14 + 1;
        local_10 = local_10 + 1;
      }
      param_2 = param_2 + 1;
      param_1 = param_1 + 3;
    }
  }
  return puVar1;
}

// FUNCTION: CARDARTLIB 0x100078B8
undefined4 __cdecl FUN_100078b8(int param_1,int *param_2)

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
  iVar7 = FUN_100025cb(piVar1 + iVar4,piVar1,iVar4);
  local_18 = (void *)((int)(piVar1 + iVar4) + iVar7);
  for (local_28 = 0; local_28 < param_2[10]; local_28 = local_28 + 1) {
    pvVar8 = (void *)((iVar5 * iVar5 + iVar6 * iVar6 * 2 + 0x40) * local_28 * 4 + param_1);
    _Dst = (void *)((int)pvVar8 + iVar5 * iVar5 * 4 + 0x80);
    _Dst_00 = (void *)((int)_Dst + iVar6 * iVar6 * 4 + 0x80);
    FID_conflict__memcpy(pvVar8,local_18,iVar3 * iVar3 * 4);
    puVar2 = (uint *)((int)local_18 + iVar3 * iVar3 * 4);
    FUN_10002cde((undefined8 *)((int)pvVar8 + iVar3 * iVar3 * 4),puVar2,param_2[local_28 + 0x17]);
    pvVar8 = (void *)((int)puVar2 + param_2[local_28 + 0x17]);
    FID_conflict__memcpy(_Dst,pvVar8,iVar3 * iVar3 * 4);
    puVar2 = (uint *)((int)pvVar8 + iVar3 * iVar3 * 4);
    FUN_10002cde((undefined8 *)((int)_Dst + iVar3 * iVar3 * 4),puVar2,param_2[local_28 + 0x1b]);
    pvVar8 = (void *)((int)puVar2 + param_2[local_28 + 0x1b]);
    FID_conflict__memcpy(_Dst_00,pvVar8,iVar3 * iVar3 * 4);
    puVar2 = (uint *)((int)pvVar8 + iVar3 * iVar3 * 4);
    FUN_10002cde((undefined8 *)((int)_Dst_00 + iVar3 * iVar3 * 4),puVar2,param_2[local_28 + 0x1f]);
    local_18 = (void *)((int)puVar2 + param_2[local_28 + 0x1f]);
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x10007AEF
bool __cdecl FUN_10007aef(HWND param_1)

{
  HDC hDC;
  HBRUSH hbr;
  RECT local_14;
  
  if (param_1 != (HWND)0x0) {
    hDC = GetDC(param_1);
    GetClientRect(param_1,&local_14);
    hbr = GetStockObject(0);
    FillRect(hDC,&local_14,hbr);
    ReleaseDC(param_1,hDC);
  }
  return param_1 != (HWND)0x0;
}

// FUNCTION: CARDARTLIB 0x10007B58
int __cdecl
FUN_10007b58(HWND param_1,void *param_2,int param_3,int param_4,DWORD param_5,DWORD param_6)

{
  BITMAPINFO *lpbmi;
  HDC hdc;
  int iVar1;
  
  lpbmi = (BITMAPINFO *)FUN_10007bdb(param_5,param_6,0x18);
  hdc = GetDC(param_1);
  iVar1 = SetDIBitsToDevice(hdc,param_3,param_4,param_5,param_6,0,0,0,param_6,param_2,lpbmi,0);
  ReleaseDC(param_1,hdc);
  FUN_10007cff(lpbmi);
  return iVar1;
}

// FUNCTION: CARDARTLIB 0x10007BDB
undefined4 * __cdecl FUN_10007bdb(undefined4 param_1,int param_2,int param_3)

{
  int local_10;
  undefined4 *local_c;
  undefined4 *local_8;
  
  if (param_3 == 8) {
    local_8 = _malloc(0x42c);
  }
  else if (param_3 == 0x18) {
    local_8 = _malloc(0x2c);
  }
  else {
    local_8 = _malloc(0x2c);
  }
  *local_8 = 0x28;
  local_8[1] = param_1;
  local_8[2] = -param_2;
  *(undefined2 *)(local_8 + 3) = 1;
  *(short *)((int)local_8 + 0xe) = (short)param_3;
  local_8[4] = 0;
  local_8[5] = 0;
  local_8[6] = 0;
  local_8[7] = 0;
  if (param_3 == 8) {
    local_8[8] = 0x100;
    local_8[9] = 0x100;
    local_c = local_8 + 10;
    for (local_10 = 0; local_10 < 0x100; local_10 = local_10 + 1) {
      *(short *)local_c = (short)local_10;
      local_c = (undefined4 *)((int)local_c + 2);
    }
  }
  else {
    local_8[8] = 0;
    local_8[9] = 0;
  }
  return local_8;
}

// FUNCTION: CARDARTLIB 0x10007CFF
undefined4 __cdecl FUN_10007cff(void *param_1)

{
  FUN_1000a210(param_1);
  return 1;
}

// FUNCTION: CARDARTLIB 0x10007D20
/* WARNING: Removing unreachable block (ram,0x10007f81) */
/* WARNING: Removing unreachable block (ram,0x10007fbb) */
/* WARNING: Removing unreachable block (ram,0x10007f8b) */
/* WARNING: Removing unreachable block (ram,0x10007e82) */
/* WARNING: Removing unreachable block (ram,0x10007e9f) */
/* WARNING: Removing unreachable block (ram,0x10007e8c) */

undefined4 * __cdecl FUN_10007d20(undefined4 *param_1,int *param_2,int param_3,int param_4)

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
      param_1 = _malloc((param_3 * 3 + iVar7) * param_4 + 4);
    }
    puVar4 = param_1;
    iVar1 = param_2[7];
    iVar2 = param_2[8];
    if (param_2[0x6a] == 0) {
      local_40 = FUN_10006be3(param_2,(undefined8 *)0x0);
    }
    else {
      local_40 = (undefined8 *)param_2[0x6b];
    }
    iVar3 = param_2[7];
    piVar5 = _malloc(param_3 << 2);
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
        FID_conflict__memcpy(param_1,(void *)((int)param_1 + (param_3 * -3 - iVar7)),param_3 * 3);
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
    FUN_1000a210(piVar5);
    if (param_2[0x6a] == 0) {
      FUN_1000a210(local_40);
    }
  }
  return puVar4;
}

// FUNCTION: CARDARTLIB 0x10008015
bool __cdecl FUN_10008015(HWND param_1,int *param_2,DWORD param_3,DWORD param_4)

{
  undefined4 *puVar1;
  
  if (param_2 != (int *)0x0) {
    puVar1 = FUN_10007d20((undefined4 *)0x0,param_2,param_3,param_4);
    FUN_10007b58(param_1,puVar1,0,0,param_3,param_4);
    FUN_1000a210(puVar1);
  }
  return param_2 != (int *)0x0;
}

// FUNCTION: CARDARTLIB 0x1000807F
/* WARNING: Removing unreachable block (ram,0x10008470) */
/* WARNING: Removing unreachable block (ram,0x1000824d) */

uint * __cdecl FUN_1000807f(uint *param_1,int *param_2,int param_3,int param_4)

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
  
  FUN_1000b7a0();
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
      local_1050 = FUN_10006be3(param_2,(undefined8 *)&DAT_10032c98);
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
      FID_conflict__memcpy
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
      FID_conflict__memcpy
                ((uint *)((param_4 + -1) * local_101c + (int)local_5054),local_5050,local_101c);
    }
    else {
      _memset((uint *)((param_4 + -1) * local_101c + (int)local_5054),0,local_101c);
    }
    iVar1 = (4 - (param_3 * 3) % 4) % 4;
    if (DAT_1001d258 == 0) {
      FUN_10005591(local_5054,param_4,param_3,iVar1);
    }
    else if (DAT_101221e8 == 0x10) {
      uStackY_2c = 0x10008757;
      FUN_100062c7(DAT_1001d258,DAT_1001d25c,(int)local_5054,param_4,param_3,iVar1);
    }
    else if (DAT_101221e8 == 8) {
      uStackY_2c = 0x10008793;
      FUN_1000570f(DAT_1001d258,DAT_1001d25c,local_5054,param_4,param_3,iVar1);
    }
  }
  return local_5054;
}

// FUNCTION: CARDARTLIB 0x100087A6
undefined4 __cdecl FUN_100087a6(HWND param_1,int *param_2,DWORD param_3,DWORD param_4)

{
  undefined4 uVar1;
  uint *puVar2;
  
  if (param_2 == (int *)0x0) {
    uVar1 = 0;
  }
  else {
    puVar2 = FUN_1000807f((uint *)0x0,param_2,param_3,param_4);
    FUN_1000570f(DAT_1001d258,DAT_1001d25c,puVar2,param_4,param_3,
                 (DAT_1001e0e0 - (int)(param_3 * 3) % DAT_1001e0e0) % DAT_1001e0e0);
    FUN_10007b58(param_1,puVar2,0,0,param_3,param_4);
    if ((uint *)param_2[0x6b] != puVar2) {
      FUN_1000a210(puVar2);
    }
    uVar1 = 1;
  }
  return uVar1;
}

// FUNCTION: CARDARTLIB 0x10008867
undefined4 * __cdecl FUN_10008867(int param_1)

{
  undefined4 *local_8;
  
  local_8 = _malloc(param_1 + 8);
  if (((uint)local_8 & 7) == 0) {
    local_8[1] = 0;
    local_8 = local_8 + 2;
  }
  else {
    *local_8 = 0xffffffff;
    local_8 = local_8 + 1;
  }
  return local_8;
}

// FUNCTION: CARDARTLIB 0x100088B9
void __cdecl FUN_100088b9(int param_1)

{
  undefined4 local_8;
  
  if (*(int *)(param_1 + -4) == 0) {
    local_8 = (void *)(param_1 + -8);
  }
  else {
    local_8 = (void *)(param_1 + -4);
  }
  FUN_1000a210(local_8);
  return;
}

// FUNCTION: CARDARTLIB 0x100088F3
int __cdecl
FUN_100088f3(HWND param_1,int param_2,int param_3,int param_4,DWORD param_5,DWORD param_6)

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
  _Dst = _malloc(param_5 * param_6 * 3 + 8);
  _memset(_Dst,0,param_5 * param_6 * 3);
  local_14 = _Dst;
  for (local_2c = 0; local_2c < (int)param_6; local_2c = local_2c + 1) {
    for (local_28 = 0; local_28 < (int)param_5; local_28 = local_28 + 1) {
      iVar1 = *(int *)(local_28 * 4 + param_5 * local_2c * 4 + param_2) >> 2;
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
  lpbmi = (BITMAPINFO *)FUN_10007bdb(param_5,param_6,0x18);
  hdc = GetDC(param_1);
  iVar2 = SetDIBitsToDevice(hdc,param_3,param_4,param_5,param_6,0,0,0,param_6,_Dst,lpbmi,0);
  ReleaseDC(param_1,hdc);
  FUN_10007cff(lpbmi);
  FUN_1000a210(_Dst);
  return iVar2;
}

// LIBRARY: CARDARTLIB 0x10008AA0 SYMBOL
// __cinit

// LIBRARY: CARDARTLIB 0x10008AF0 SYMBOL
// _exit

// LIBRARY: CARDARTLIB 0x10008B10 SYMBOL
// __exit

// LIBRARY: CARDARTLIB 0x10008B30 SYMBOL
// __cexit

// LIBRARY: CARDARTLIB 0x10008B50 SYMBOL
// doexit

// LIBRARY: CARDARTLIB 0x10008C60 SYMBOL
// FID_conflict:__lockexit

// LIBRARY: CARDARTLIB 0x10008C80 SYMBOL
// FID_conflict:__lockexit

// LIBRARY: CARDARTLIB 0x10008CA0 SYMBOL
// __initterm

// LIBRARY: CARDARTLIB 0x10008CE0 SYMBOL
// _fclose

// LIBRARY: CARDARTLIB 0x10008D70 SYMBOL
// __fclose_lk

// LIBRARY: CARDARTLIB 0x10008E40 SYMBOL
// _fprintf

// LIBRARY: CARDARTLIB 0x10008F10 SYMBOL
// _ctime

// LIBRARY: CARDARTLIB 0x10008F60 SYMBOL
// _time

// LIBRARY: CARDARTLIB 0x100090F0 SYMBOL
// __vsnprintf

// LIBRARY: CARDARTLIB 0x100091E0 SYMBOL
// _strlen

// LIBRARY: CARDARTLIB 0x10009260 SYMBOL
// _sprintf

// LIBRARY: CARDARTLIB 0x10009350 SYMBOL
// __fsopen

// LIBRARY: CARDARTLIB 0x10009460 SYMBOL
// _fopen

// FUNCTION: CARDARTLIB 0x10009490
uint * __cdecl FUN_10009490(uint *param_1,uint *param_2)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  
  puVar4 = param_1;
  while (((uint)param_2 & 3) != 0) {
    bVar1 = (byte)*param_2;
    uVar3 = (uint)bVar1;
    param_2 = (uint *)((int)param_2 + 1);
    if (bVar1 == 0) goto LAB_10009578;
    *(byte *)puVar4 = bVar1;
    puVar4 = (uint *)((int)puVar4 + 1);
  }
  do {
    uVar2 = *param_2;
    uVar3 = *param_2;
    param_2 = param_2 + 1;
    if (((uVar2 ^ 0xffffffff ^ uVar2 + 0x7efefeff) & 0x81010100) != 0) {
      if ((char)uVar3 == '\0') {
LAB_10009578:
        *(byte *)puVar4 = (byte)uVar3;
        return param_1;
      }
      if ((char)(uVar3 >> 8) == '\0') {
        *(short *)puVar4 = (short)uVar3;
        return param_1;
      }
      if ((uVar3 & 0xff0000) == 0) {
        *(short *)puVar4 = (short)uVar3;
        *(byte *)((int)puVar4 + 2) = 0;
        return param_1;
      }
      if ((uVar3 & 0xff000000) == 0) {
        *puVar4 = uVar3;
        return param_1;
      }
    }
    *puVar4 = uVar3;
    puVar4 = puVar4 + 1;
  } while( true );
}

// FUNCTION: CARDARTLIB 0x100094A0
uint * __cdecl FUN_100094a0(uint *param_1,uint *param_2)

{
  byte bVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  uint *puVar5;
  
  puVar3 = param_1;
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
    if (((uint)param_2 & 3) == 0) {
      do {
        uVar2 = *param_2;
        uVar4 = *param_2;
        param_2 = param_2 + 1;
        if (((uVar2 ^ 0xffffffff ^ uVar2 + 0x7efefeff) & 0x81010100) != 0) {
          if ((char)uVar4 == '\0') {
LAB_10009578:
            *(byte *)puVar5 = (byte)uVar4;
            return param_1;
          }
          if ((char)(uVar4 >> 8) == '\0') {
            *(short *)puVar5 = (short)uVar4;
            return param_1;
          }
          if ((uVar4 & 0xff0000) == 0) {
            *(short *)puVar5 = (short)uVar4;
            *(byte *)((int)puVar5 + 2) = 0;
            return param_1;
          }
          if ((uVar4 & 0xff000000) == 0) {
            *puVar5 = uVar4;
            return param_1;
          }
        }
        *puVar5 = uVar4;
        puVar5 = puVar5 + 1;
      } while( true );
    }
    bVar1 = (byte)*param_2;
    uVar4 = (uint)bVar1;
    param_2 = (uint *)((int)param_2 + 1);
    if (bVar1 == 0) goto LAB_10009578;
    *(byte *)puVar5 = bVar1;
    puVar5 = (uint *)((int)puVar5 + 1);
  } while( true );
}

// LIBRARY: CARDARTLIB 0x10009580 SYMBOL
// _fread

// LIBRARY: CARDARTLIB 0x100095D0 SYMBOL
// __fread_lk

// LIBRARY: CARDARTLIB 0x100097A0 SYMBOL
// _malloc

// LIBRARY: CARDARTLIB 0x100097D0 SYMBOL
// __malloc_dbg

// LIBRARY: CARDARTLIB 0x10009800 SYMBOL
// __nh_malloc_dbg

// LIBRARY: CARDARTLIB 0x10009880 SYMBOL
// __heap_alloc_dbg

// LIBRARY: CARDARTLIB 0x10009BC0 SYMBOL
// __calloc_dbg

// LIBRARY: CARDARTLIB 0x10009C30 SYMBOL
// __realloc_dbg

// LIBRARY: CARDARTLIB 0x10009C80 SYMBOL
// realloc_help

// LIBRARY: CARDARTLIB 0x1000A210 SYMBOL
// FUN_1000a210

// LIBRARY: CARDARTLIB 0x1000A230 SYMBOL
// __free_dbg

// LIBRARY: CARDARTLIB 0x1000A260 SYMBOL
// __free_dbg_lk

// LIBRARY: CARDARTLIB 0x1000A690 SYMBOL
// _CheckBytes

// LIBRARY: CARDARTLIB 0x1000A720 SYMBOL
// __CrtCheckMemory

// LIBRARY: CARDARTLIB 0x1000AAC0 SYMBOL
// __CrtSetDbgFlag

// LIBRARY: CARDARTLIB 0x1000AAF0 SYMBOL
// __CrtIsValidPointer

// LIBRARY: CARDARTLIB 0x1000AB50 SYMBOL
// __CrtIsValidHeapPointer

// LIBRARY: CARDARTLIB 0x1000AC20 SYMBOL
// __CrtMemCheckpoint

// LIBRARY: CARDARTLIB 0x1000AD70 SYMBOL
// __CrtMemDumpAllObjectsSince

// LIBRARY: CARDARTLIB 0x1000B050 SYMBOL
// __printMemBlockData

// LIBRARY: CARDARTLIB 0x1000B150 SYMBOL
// __CrtDumpMemoryLeaks

// LIBRARY: CARDARTLIB 0x1000B1E0 SYMBOL
// _bsearch

// LIBRARY: CARDARTLIB 0x1000B300 SYMBOL
// _fseek

// LIBRARY: CARDARTLIB 0x1000B380 SYMBOL
// __fseek_lk

// LIBRARY: CARDARTLIB 0x1000B4C0 SYMBOL
// __splitpath

// LIBRARY: CARDARTLIB 0x1000B710 SYMBOL
// _memset

// LIBRARY: CARDARTLIB 0x1000B770 SYMBOL
// _strrchr

// FUNCTION: CARDARTLIB 0x1000B7A0
/* WARNING: Unable to track spacebase fully for stack */

void FUN_1000b7a0(void)

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

// LIBRARY: CARDARTLIB 0x1000B7D0 SYMBOL
// FID_conflict:_memcpy

// LIBRARY: CARDARTLIB 0x1000B920 SYMBOL
// _fgets

// LIBRARY: CARDARTLIB 0x1000BA80 SYMBOL
// _strchr

// LIBRARY: CARDARTLIB 0x1000BB40 SYMBOL
// _sscanf

// LIBRARY: CARDARTLIB 0x1000BC10 SYMBOL
// _strspn

// LIBRARY: CARDARTLIB 0x1000BC50 SYMBOL
// _strcspn

// LIBRARY: CARDARTLIB 0x1000BC90 SYMBOL
// _atol

// LIBRARY: CARDARTLIB 0x1000BDB0 SYMBOL
// _atoi

// LIBRARY: CARDARTLIB 0x1000BDD0 SYMBOL
// __allshl

// LIBRARY: CARDARTLIB 0x1000BDF0 SYMBOL
// __CRT_INIT@12

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
    local_8 = FUN_100030f0(param_1,param_2);
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

// LIBRARY: CARDARTLIB 0x1000C120 SYMBOL
// __amsg_exit

// LIBRARY: CARDARTLIB 0x1000C180 SYMBOL
// __mtinitlocks

// LIBRARY: CARDARTLIB 0x1000C1C0 SYMBOL
// __mtdeletelocks

// LIBRARY: CARDARTLIB 0x1000C280 SYMBOL
// __lock

// LIBRARY: CARDARTLIB 0x1000C330 SYMBOL
// FUN_1000c330

// LIBRARY: CARDARTLIB 0x1000C350 SYMBOL
// __lock_file

// LIBRARY: CARDARTLIB 0x1000C3B0 SYMBOL
// FID_conflict:__lock_file2

// LIBRARY: CARDARTLIB 0x1000C3F0 SYMBOL
// __unlock_file

// LIBRARY: CARDARTLIB 0x1000C450 SYMBOL
// FID_conflict:__lock_file2

// LIBRARY: CARDARTLIB 0x1000C490 SYMBOL
// __CrtDbgBreak

// LIBRARY: CARDARTLIB 0x1000C4B0 SYMBOL
// __CrtDbgReport

// LIBRARY: CARDARTLIB 0x1000C8A0 SYMBOL
// _CrtMessageWindow

// LIBRARY: CARDARTLIB 0x1000CBD0 SYMBOL
// __close

// LIBRARY: CARDARTLIB 0x1000CC70 SYMBOL
// __close_lk

// LIBRARY: CARDARTLIB 0x1000CD40 SYMBOL
// __freebuf

// LIBRARY: CARDARTLIB 0x1000CDD0 SYMBOL
// __fflush_lk

// LIBRARY: CARDARTLIB 0x1000CE40 SYMBOL
// __flush

// LIBRARY: CARDARTLIB 0x1000CF00 SYMBOL
// __flushall

// LIBRARY: CARDARTLIB 0x1000CF20 SYMBOL
// flsall

// LIBRARY: CARDARTLIB 0x1000D080 SYMBOL
// __stbuf

// LIBRARY: CARDARTLIB 0x1000D1D0 SYMBOL
// __ftbuf

// LIBRARY: CARDARTLIB 0x1000D270 SYMBOL
// __output

// LIBRARY: CARDARTLIB 0x1000E000 SYMBOL
// write_char

// LIBRARY: CARDARTLIB 0x1000E080 SYMBOL
// write_multi_char

// LIBRARY: CARDARTLIB 0x1000E0D0 SYMBOL
// write_string

// LIBRARY: CARDARTLIB 0x1000E130 SYMBOL
// get_int_arg

// LIBRARY: CARDARTLIB 0x1000E150 SYMBOL
// get_int64_arg

// LIBRARY: CARDARTLIB 0x1000E180 SYMBOL
// get_short_arg

// LIBRARY: CARDARTLIB 0x1000E1A0 SYMBOL
// _asctime

// LIBRARY: CARDARTLIB 0x1000E350 SYMBOL
// store_dt

// LIBRARY: CARDARTLIB 0x1000E390 SYMBOL
// _localtime

// LIBRARY: CARDARTLIB 0x1000E5F0 SYMBOL
// ___loctotime_t

// LIBRARY: CARDARTLIB 0x1000E710 SYMBOL
// __flsbuf

// LIBRARY: CARDARTLIB 0x1000E9B0 SYMBOL
// __openfile

// LIBRARY: CARDARTLIB 0x1000ED70 SYMBOL
// __getstream

// LIBRARY: CARDARTLIB 0x1000EF10 SYMBOL
// __filbuf

// LIBRARY: CARDARTLIB 0x1000F100 SYMBOL
// __read

// LIBRARY: CARDARTLIB 0x1000F1B0 SYMBOL
// __read_lk

// LIBRARY: CARDARTLIB 0x1000F670 SYMBOL
// __callnewh

// LIBRARY: CARDARTLIB 0x1000F6D0 SYMBOL
// __malloc_base

// LIBRARY: CARDARTLIB 0x1000F700 SYMBOL
// __nh_malloc_base

// LIBRARY: CARDARTLIB 0x1000F7A0 SYMBOL
// __heap_alloc_base

// LIBRARY: CARDARTLIB 0x1000F820 SYMBOL
// FUN_1000f820

// LIBRARY: CARDARTLIB 0x1000F840 SYMBOL
// __expand_base

// LIBRARY: CARDARTLIB 0x1000F930 SYMBOL
// __realloc_base

// LIBRARY: CARDARTLIB 0x1000FB60 SYMBOL
// __free_base

// LIBRARY: CARDARTLIB 0x1000FBF0 SYMBOL
// __heapchk

// LIBRARY: CARDARTLIB 0x1000FC90 SYMBOL
// __heap_init

// LIBRARY: CARDARTLIB 0x1000FCF0 SYMBOL
// __heap_term

// LIBRARY: CARDARTLIB 0x1000FD50 SYMBOL
// ___sbh_new_region

// LIBRARY: CARDARTLIB 0x1000FF60 SYMBOL
// ___sbh_release_region

// LIBRARY: CARDARTLIB 0x1000FFF0 SYMBOL
// ___sbh_decommit_pages

// LIBRARY: CARDARTLIB 0x10010170 SYMBOL
// ___sbh_find_block

// LIBRARY: CARDARTLIB 0x10010220 SYMBOL
// ___sbh_free_block

// LIBRARY: CARDARTLIB 0x100102B0 SYMBOL
// ___sbh_alloc_block

// LIBRARY: CARDARTLIB 0x10010780 SYMBOL
// ___sbh_alloc_block_from_page

// LIBRARY: CARDARTLIB 0x10010A80 SYMBOL
// ___sbh_resize_block

// LIBRARY: CARDARTLIB 0x10010C40 SYMBOL
// ___sbh_heap_check

// LIBRARY: CARDARTLIB 0x10010EB0 SYMBOL
// __isctype

// LIBRARY: CARDARTLIB 0x10010F70 SYMBOL
// __lseek

// LIBRARY: CARDARTLIB 0x10011020 SYMBOL
// __lseek_lk

// LIBRARY: CARDARTLIB 0x10011110 SYMBOL
// __ftell_lk

// LIBRARY: CARDARTLIB 0x100113D0 SYMBOL
// __dosmaperr

// LIBRARY: CARDARTLIB 0x100114B0 SYMBOL
// FUN_100114b0

// LIBRARY: CARDARTLIB 0x100114D0 SYMBOL
// FUN_100114d0

// LIBRARY: CARDARTLIB 0x100114F0 SYMBOL
// __mbsnbcpy

// LIBRARY: CARDARTLIB 0x10011620 SYMBOL
// __setmbcp

// LIBRARY: CARDARTLIB 0x100119A0 SYMBOL
// getSystemCP

// LIBRARY: CARDARTLIB 0x10011A30 SYMBOL
// _CPtoLCID

// LIBRARY: CARDARTLIB 0x10011AD0 SYMBOL
// setSBCS

// LIBRARY: CARDARTLIB 0x10011B50 SYMBOL
// ___initmbctable

// LIBRARY: CARDARTLIB 0x10011B70 SYMBOL
// __input

// LIBRARY: CARDARTLIB 0x10012F20 SYMBOL
// __hextodec

// LIBRARY: CARDARTLIB 0x10012F90 SYMBOL
// __inc

// LIBRARY: CARDARTLIB 0x10012FE0 SYMBOL
// __un_inc

// LIBRARY: CARDARTLIB 0x10013010 SYMBOL
// __whiteout

// LIBRARY: CARDARTLIB 0x100130A0 SYMBOL
// __mtinit

// LIBRARY: CARDARTLIB 0x10013140 SYMBOL
// __mtterm

// LIBRARY: CARDARTLIB 0x10013180 SYMBOL
// __initptd

// LIBRARY: CARDARTLIB 0x100131A0 SYMBOL
// __getptd

// LIBRARY: CARDARTLIB 0x10013250 SYMBOL
// __freeptd

// LIBRARY: CARDARTLIB 0x10013360 SYMBOL
// __ioinit

// LIBRARY: CARDARTLIB 0x100136B0 SYMBOL
// __ioterm

// LIBRARY: CARDARTLIB 0x10013770 SYMBOL
// __setenvp

// LIBRARY: CARDARTLIB 0x100138B0 SYMBOL
// __setargv

// LIBRARY: CARDARTLIB 0x10013980 SYMBOL
// parse_cmdline

// LIBRARY: CARDARTLIB 0x10013D40 SYMBOL
// ___crtGetEnvironmentStringsA

// LIBRARY: CARDARTLIB 0x10013FA0 SYMBOL
// ___set_app_type

// LIBRARY: CARDARTLIB 0x10013FC0 SYMBOL
// __FF_MSGBANNER

// LIBRARY: CARDARTLIB 0x10014020 SYMBOL
// __NMSG_WRITE

// LIBRARY: CARDARTLIB 0x10014240 SYMBOL
// ___initstdio

// LIBRARY: CARDARTLIB 0x100143A0 SYMBOL
// ___endstdio

// LIBRARY: CARDARTLIB 0x100143D0 SYMBOL
// __itoa

// LIBRARY: CARDARTLIB 0x10014430 SYMBOL
// xtoa

// LIBRARY: CARDARTLIB 0x100144F0 SYMBOL
// __snprintf

// LIBRARY: CARDARTLIB 0x100145E0 SYMBOL
// _raise

// LIBRARY: CARDARTLIB 0x10014870 SYMBOL
// siglookup

// LIBRARY: CARDARTLIB 0x100148E0 SYMBOL
// ___crtMessageBoxA

// LIBRARY: CARDARTLIB 0x100149C0 SYMBOL
// _strncpy

// LIBRARY: CARDARTLIB 0x10014AC0 SYMBOL
// __alloc_osfhnd

// LIBRARY: CARDARTLIB 0x10014CB0 SYMBOL
// __set_osfhnd

// LIBRARY: CARDARTLIB 0x10014DC0 SYMBOL
// __free_osfhnd

// LIBRARY: CARDARTLIB 0x10014EF0 SYMBOL
// __get_osfhandle

// LIBRARY: CARDARTLIB 0x10014F90 SYMBOL
// __lock_fhandle

// LIBRARY: CARDARTLIB 0x10015030 SYMBOL
// __unlock_fhandle

// LIBRARY: CARDARTLIB 0x10015070 SYMBOL
// __commit

// LIBRARY: CARDARTLIB 0x10015190 SYMBOL
// __write

// LIBRARY: CARDARTLIB 0x10015240 SYMBOL
// __write_lk

// LIBRARY: CARDARTLIB 0x10015540 SYMBOL
// __isatty

// LIBRARY: CARDARTLIB 0x10015590 SYMBOL
// _wctomb

// LIBRARY: CARDARTLIB 0x10015610 SYMBOL
// __wctomb_lk

// LIBRARY: CARDARTLIB 0x100156F0 SYMBOL
// __aulldiv

// LIBRARY: CARDARTLIB 0x10015760 SYMBOL
// __aullrem

// LIBRARY: CARDARTLIB 0x100157E0 SYMBOL
// ___tzset

// LIBRARY: CARDARTLIB 0x10015830 SYMBOL
// __tzset_lk

// LIBRARY: CARDARTLIB 0x10015BC0 SYMBOL
// __isindst

// LIBRARY: CARDARTLIB 0x10015C00 SYMBOL
// __isindst_lk

// LIBRARY: CARDARTLIB 0x10015F70 SYMBOL
// cvtdate

// LIBRARY: CARDARTLIB 0x10016180 SYMBOL
// _gmtime

// LIBRARY: CARDARTLIB 0x100163B0 SYMBOL
// __getbuf

// LIBRARY: CARDARTLIB 0x10016470 SYMBOL
// __sopen

// LIBRARY: CARDARTLIB 0x10016AB0 SYMBOL
// ___crtGetStringTypeW

// LIBRARY: CARDARTLIB 0x10016D10 SYMBOL
// ___crtGetStringTypeA

// LIBRARY: CARDARTLIB 0x10016EB0 SYMBOL
// _mbtowc

// LIBRARY: CARDARTLIB 0x10016F30 SYMBOL
// __mbtowc_lk

// LIBRARY: CARDARTLIB 0x100170E0 SYMBOL
// _isspace

// LIBRARY: CARDARTLIB 0x10017130 SYMBOL
// __ungetc_lk

// LIBRARY: CARDARTLIB 0x10017260 SYMBOL
// _wcslen

// LIBRARY: CARDARTLIB 0x100172B0 SYMBOL
// __fcloseall

// LIBRARY: CARDARTLIB 0x100173A0 SYMBOL
// FUN_100173a0

// LIBRARY: CARDARTLIB 0x100173C0 SYMBOL
// __fptrap

// LIBRARY: CARDARTLIB 0x100173E0 SYMBOL
// _strcmp

// LIBRARY: CARDARTLIB 0x10017470 SYMBOL
// _wcstombs

// LIBRARY: CARDARTLIB 0x100174F0 SYMBOL
// __wcstombs_lk

// LIBRARY: CARDARTLIB 0x10017870 SYMBOL
// wcsncnt

// LIBRARY: CARDARTLIB 0x100178E0 SYMBOL
// __getenv_lk

// LIBRARY: CARDARTLIB 0x100179D0 SYMBOL
// __chsize_lk

// LIBRARY: CARDARTLIB 0x10017C60 SYMBOL
// FID_conflict:_memcpy

// LIBRARY: CARDARTLIB 0x10017DB0 SYMBOL
// ___init_time

// LIBRARY: CARDARTLIB 0x10017EB0 SYMBOL
// __get_lc_time

// LIBRARY: CARDARTLIB 0x10018360 SYMBOL
// __free_lc_time

// LIBRARY: CARDARTLIB 0x10018680 SYMBOL
// storeTimeFmt

// LIBRARY: CARDARTLIB 0x10018810 SYMBOL
// ___init_numeric

// LIBRARY: CARDARTLIB 0x10018AD0 SYMBOL
// fix_grouping

// LIBRARY: CARDARTLIB 0x10018B60 SYMBOL
// ___init_monetary

// LIBRARY: CARDARTLIB 0x10018CA0 SYMBOL
// __get_lc_lconv

// LIBRARY: CARDARTLIB 0x10018E80 SYMBOL
// fix_grouping

// LIBRARY: CARDARTLIB 0x10018F10 SYMBOL
// __free_lc_lconv

// LIBRARY: CARDARTLIB 0x10018FC0 SYMBOL
// ___init_ctype

// LIBRARY: CARDARTLIB 0x100193C0 SYMBOL
// FUN_100193c0

// LIBRARY: CARDARTLIB 0x100193E0 SYMBOL
// _strncmp

// LIBRARY: CARDARTLIB 0x10019420 SYMBOL
// _strpbrk

// LIBRARY: CARDARTLIB 0x10019460 SYMBOL
// __mbsnbicoll

// LIBRARY: CARDARTLIB 0x100194D0 SYMBOL
// ___wtomb_environ

// LIBRARY: CARDARTLIB 0x100195A0 SYMBOL
// __setmode_lk

// LIBRARY: CARDARTLIB 0x100196C0 SYMBOL
// ___getlocaleinfo

// LIBRARY: CARDARTLIB 0x10019990 SYMBOL
// ___crtGetLocaleInfoW

// LIBRARY: CARDARTLIB 0x10019B50 SYMBOL
// ___crtGetLocaleInfoA

// LIBRARY: CARDARTLIB 0x10019D20 SYMBOL
// __strcmpi

// LIBRARY: CARDARTLIB 0x10019DF0 SYMBOL
// ___crtCompareStringA

// LIBRARY: CARDARTLIB 0x1001A250 SYMBOL
// _strncnt

// LIBRARY: CARDARTLIB 0x1001A2C0 SYMBOL
// ___crtsetenv

// LIBRARY: CARDARTLIB 0x1001A630 SYMBOL
// findenv

// LIBRARY: CARDARTLIB 0x1001A6D0 SYMBOL
// copy_environ

// LIBRARY: CARDARTLIB 0x1001A7D0 SYMBOL
// __tolower_lk

// LIBRARY: CARDARTLIB 0x1001A910 SYMBOL
// __mbschr

// LIBRARY: CARDARTLIB 0x1001AA20 SYMBOL
// ___crtLCMapStringA

// LIBRARY: CARDARTLIB 0x1001AD40 SYMBOL
// _strncnt

// LIBRARY: CARDARTLIB 0x1001ADB0 SYMBOL
// __strlwr

