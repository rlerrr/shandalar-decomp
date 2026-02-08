#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include "inttypes.h"
#include "mystdbool.h"
#include "sidlib/pcxw.h"
#include "sidlib/pic.h"
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "cardartlib/src/catalog.h"

/* Function prototypes (auto-generated). */
#include "autoproto.h"


typedef void * pointer;
typedef char byte;
typedef unsigned short ushort;
typedef BOOL (WINAPI code)(HINSTANCE,DWORD,LPVOID);

#define CONCAT12(x,y) ((undefined4)(((undefined1)(x)) | ((undefined4)(undefined2)(y) << 8)))


undefined1 DAT_10021fe0[0x3c];
unsigned char DAT_1003a048[0x3c];
undefined4 _DAT_1003a048 = 0;
undefined4 _DAT_1003a058 = 0;
undefined4 DAT_1003a05c = 0;
char DAT_1003a064[0x20];
char DAT_1003a090[260];

char DAT_100f36c0[0x105];
undefined4 DAT_100f5b5c = 0;
undefined4 DAT_100f6908 = 0;
undefined4 DAT_100f77e0 = 0;
undefined4 DAT_100f8140 = 0;
undefined4 DAT_100f952c = 0;
undefined4 DAT_10126554 = 0;
undefined4 DAT_10128640 = 0;
undefined4 DAT_100223ac = 0;
undefined4 DAT_100223bc = 0;

// GLOBAL: DRAWCARDLIB 0x10021048
undefined4 DAT_10021048 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1002201c
char s__DuelArt_1002201c[] = "\\DuelArt";

// GLOBAL: DRAWCARDLIB 0x10022028
char s__Duel_dat_10022028[] = "\\Duel.dat";

// GLOBAL: DRAWCARDLIB 0x10022034
char s__Tt0530m__TTF_10022034[] = "\\Tt0530m_.TTF";

// GLOBAL: DRAWCARDLIB 0x10022044
char s__Magis____TTF_10022044[] = "\\Magis___.TTF";

// GLOBAL: DRAWCARDLIB 0x10022054
char s__Tt0127m__TTF_10022054[] = "\\Tt0127m_.TTF";

// GLOBAL: DRAWCARDLIB 0x10022064
char s__Tt0085m__TTF_10022064[] = "\\Tt0085m_.TTF";

// GLOBAL: DRAWCARDLIB 0x10022074
char s__Tt0298m__TTF_10022074[] = "\\Tt0298m_.TTF";

// GLOBAL: DRAWCARDLIB 0x10022084
char s__Tt0299m__TTF_10022084[] = "\\Tt0299m_.TTF";

// GLOBAL: DRAWCARDLIB 0x10022094
char s__Tt0300m__TTF_10022094[] = "\\Tt0300m_.TTF";

// GLOBAL: DRAWCARDLIB 0x100220a4
char s__s_ManaSymbols_pic_100220a4[] = "%s\\ManaSymbols.pic";

// GLOBAL: DRAWCARDLIB 0x100220b8
char s__s_CardSets_pic_100220b8[] = "%s\\CardSets.pic";

// GLOBAL: DRAWCARDLIB 0x100220c8
char s__s_CardBack_pic_100220c8[] = "%s\\CardBack.pic";

// GLOBAL: DRAWCARDLIB 0x100220d8
char s_BigCardTitle_100220d8[] = "BigCardTitle";

// GLOBAL: DRAWCARDLIB 0x100220e8
char s_BigCardSubtitle_100220e8[] = "BigCardSubtitle";

// GLOBAL: DRAWCARDLIB 0x100220f8
char s_BigCardPT_100220f8[] = "BigCardPT";

// GLOBAL: DRAWCARDLIB 0x10022104
char s_BigCardText_10022104[] = "BigCardText";

// GLOBAL: DRAWCARDLIB 0x10022110
char s_BigCardText_10022110[] = "BigCardText";

// GLOBAL: DRAWCARDLIB 0x1002211c
char s_SmallCardTitle_1002211c[] = "SmallCardTitle";

// GLOBAL: DRAWCARDLIB 0x1002212c
char DAT_1002212c[] = "size";

// GLOBAL: DRAWCARDLIB 0x1002213c
char DAT_1002213c[] = "bold";

// GLOBAL: DRAWCARDLIB 0x1002214c
char DAT_1002214c[] = "font";

// GLOBAL: DRAWCARDLIB 0x10022134
char s_Fonts_10022134[] = "Fonts";

// GLOBAL: DRAWCARDLIB 0x10022144
char s_Fonts_10022144[] = "Fonts";

// GLOBAL: DRAWCARDLIB 0x10022154
char s_MS_Sans_Serif_10022154[] = "MS Sans Serif";

// GLOBAL: DRAWCARDLIB 0x10022164
char s_Fonts_10022164[] = "Fonts";

// GLOBAL: DRAWCARDLIB 0x1002216c
char s__Magim____TTF_1002216c[] = "\\Magim___.TTF";

// GLOBAL: DRAWCARDLIB 0x1002217c
char s__Tt0530m__TTF_1002217c[] = "\\Tt0530m_.TTF";

// GLOBAL: DRAWCARDLIB 0x1002218c
char s__Magis____TTF_1002218c[] = "\\Magis___.TTF";

// GLOBAL: DRAWCARDLIB 0x1002219c
char s__Tt0127m__TTF_1002219c[] = "\\Tt0127m_.TTF";

// GLOBAL: DRAWCARDLIB 0x100221ac
char s__Tt0085m__TTF_100221ac[] = "\\Tt0085m_.TTF";

// GLOBAL: DRAWCARDLIB 0x100221bc
char s__Tt0298m__TTF_100221bc[] = "\\Tt0298m_.TTF";

// GLOBAL: DRAWCARDLIB 0x100221cc
char s__Tt0299m__TTF_100221cc[] = "\\Tt0299m_.TTF";

// GLOBAL: DRAWCARDLIB 0x100221dc
char s__Tt0300m__TTF_100221dc[] = "\\Tt0300m_.TTF";

// GLOBAL: DRAWCARDLIB 0x100221ec
char DAT_100221ec[] = "*";

// GLOBAL: DRAWCARDLIB 0x100221f0
char DAT_100221f0[] = "%d+*";

// GLOBAL: DRAWCARDLIB 0x100221f8
char DAT_100221f8[] = "%d";

// GLOBAL: DRAWCARDLIB 0x100221fc
char DAT_100221fc[] = "/";

// GLOBAL: DRAWCARDLIB 0x10022200
char DAT_10022200[] = "*";

// GLOBAL: DRAWCARDLIB 0x10022204
char DAT_10022204[] = "%d+*";

// GLOBAL: DRAWCARDLIB 0x1002220c
char DAT_1002220c[] = "%d";

// GLOBAL: DRAWCARDLIB 0x10022210
char DAT_10022210[] = "";

// GLOBAL: DRAWCARDLIB 0x10022214
char DAT_10022214[] = "|H1";

// GLOBAL: DRAWCARDLIB 0x10022218
char DAT_10022218[] = "";

// GLOBAL: DRAWCARDLIB 0x1002221c
char DAT_1002221c[] = "|H2";

// GLOBAL: DRAWCARDLIB 0x10022220
char DAT_10022220[] = "";

// GLOBAL: DRAWCARDLIB 0x10022224
char DAT_10022224[] = "|H3";

// GLOBAL: DRAWCARDLIB 0x10022228
char DAT_10022228[] = "";

// GLOBAL: DRAWCARDLIB 0x1002222c
char DAT_1002222c[] = "|H4";

// GLOBAL: DRAWCARDLIB 0x10022230
char DAT_10022230[] = "";

// GLOBAL: DRAWCARDLIB 0x10022234
char DAT_10022234[] = "|H";

// GLOBAL: DRAWCARDLIB 0x10022238
char DAT_10022238[] = "";

// GLOBAL: DRAWCARDLIB 0x1002223c
char DAT_1002223c[] = "|S1";

// GLOBAL: DRAWCARDLIB 0x10022240
char DAT_10022240[] = "";

// GLOBAL: DRAWCARDLIB 0x10022244
char DAT_10022244[] = "|S2";

// GLOBAL: DRAWCARDLIB 0x10022248
char DAT_10022248[] = "";

// GLOBAL: DRAWCARDLIB 0x1002224c
char DAT_1002224c[] = "|S3";

// GLOBAL: DRAWCARDLIB 0x10022250
char DAT_10022250[] = "";

// GLOBAL: DRAWCARDLIB 0x10022254
char DAT_10022254[] = "|S4";

// GLOBAL: DRAWCARDLIB 0x10022258
char DAT_10022258[] = "";

// GLOBAL: DRAWCARDLIB 0x1002225c
char DAT_1002225c[] = "|S";

// GLOBAL: DRAWCARDLIB 0x10022260
char s_CARDBK_Green_10022260[] = "CARDBK_Green";

// GLOBAL: DRAWCARDLIB 0x10022270
char s_CARDBK_White_10022270[] = "CARDBK_White";

// GLOBAL: DRAWCARDLIB 0x10022280
char s_CARDBK_Blue_10022280[] = "CARDBK_Blue";

// GLOBAL: DRAWCARDLIB 0x1002228c
char s_CARDBK_Black_1002228c[] = "CARDBK_Black";

// GLOBAL: DRAWCARDLIB 0x1002229c
char s_CARDBK_Red_1002229c[] = "CARDBK_Red";

// GLOBAL: DRAWCARDLIB 0x100222a8
char s_CARDBK_Gold_100222a8[] = "CARDBK_Gold";

// GLOBAL: DRAWCARDLIB 0x100222b4
char s_CARDBK_Artifact_100222b4[] = "CARDBK_Artifact";

// GLOBAL: DRAWCARDLIB 0x100222c4
char s_CARDBK_GreenLand_100222c4[] = "CARDBK_GreenLand";

// GLOBAL: DRAWCARDLIB 0x100222d8
char s_CARDBK_WhiteLand_100222d8[] = "CARDBK_WhiteLand";

// GLOBAL: DRAWCARDLIB 0x100222ec
char s_CARDBK_BlueLand_100222ec[] = "CARDBK_BlueLand";

// GLOBAL: DRAWCARDLIB 0x100222fc
char s_CARDBK_BlackLand_100222fc[] = "CARDBK_BlackLand";

// GLOBAL: DRAWCARDLIB 0x10022310
char s_CARDBK_RedLand_10022310[] = "CARDBK_RedLand";

// GLOBAL: DRAWCARDLIB 0x10022320
char s_CARDBK_DarklandsLand_10022320[] = "CARDBK_DarklandsLand";

// GLOBAL: DRAWCARDLIB 0x10022338
char s_CARDBK_FallenEmpiresLand_10022338[] = "CARDBK_FallenEmpiresLand";

// GLOBAL: DRAWCARDLIB 0x10022354
char s_CARDBK_AntiquitiesLand_10022354[] = "CARDBK_AntiquitiesLand";

// GLOBAL: DRAWCARDLIB 0x1002236c
char s_CARDBK_LegendsLand_1002236c[] = "CARDBK_LegendsLand";

// GLOBAL: DRAWCARDLIB 0x10022380
char s_CARDBK_ArabianNightsLand_10022380[] = "CARDBK_ArabianNightsLand";

// GLOBAL: DRAWCARDLIB 0x1002239c
char s_CARDBK_Special_1002239c[] = "CARDBK_Special";

// GLOBAL: DRAWCARDLIB 0x100223b0
char s__s__s_pic_100223b0[] = "%s\\%s.pic";

// GLOBAL: DRAWCARDLIB 0x10022500
undefined4 DAT_10022500 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10022504
undefined4 DAT_10022504 = 0xFFFFFFFF;

// GLOBAL: DRAWCARDLIB 0x10026a48
char s__CARDART_10026a48[] = "\\CARDART";

// GLOBAL: DRAWCARDLIB 0x10026a54
char s__CARDS_DAT_10026a54[] = "\\CARDS.DAT";

// GLOBAL: DRAWCARDLIB 0x10026a60
char DAT_10026a60[] = "rb";

// GLOBAL: DRAWCARDLIB 0x10026a64
char DAT_10026a64[] = "File %s could not be opened: EXITING\r\n";

// GLOBAL: DRAWCARDLIB 0x10026b98
undefined4 DAT_10026b98 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10028af0
undefined1 DAT_10028af0 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10028af1
undefined1 DAT_10028af1 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10028ef8
undefined4 DAT_10028ef8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10028f00
undefined1 DAT_10028f00 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10028f01
undefined1 DAT_10028f01 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10029311
undefined1 DAT_10029311 = 0x00;

// GLOBAL: DRAWCARDLIB 0x1002931c
undefined4 DAT_1002931c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10029320
undefined4 DAT_10029320 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fc8
undefined4 DAT_10039fc8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fcc
undefined4 DAT_10039fcc = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fd0
undefined4 DAT_10039fd0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fd4
undefined4 DAT_10039fd4 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fd8
undefined4 DAT_10039fd8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fdc
undefined4 DAT_10039fdc = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fe0
undefined4 DAT_10039fe0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fe4
undefined4 DAT_10039fe4 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fe8
undefined4 DAT_10039fe8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fec
undefined4 DAT_10039fec = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039ff0
undefined4 DAT_10039ff0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039ff4
undefined4 DAT_10039ff4 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039ff8
undefined4 DAT_10039ff8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039ffc
undefined4 DAT_10039ffc = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a000
undefined4 DAT_1003a000 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a004
undefined4 DAT_1003a004 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a008
undefined4 DAT_1003a008 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a00c
undefined4 DAT_1003a00c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a010
undefined4 DAT_1003a010 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a014
undefined4 DAT_1003a014 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a018
undefined4 DAT_1003a018 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a01c
undefined4 DAT_1003a01c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a020
undefined4 DAT_1003a020 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a024
undefined4 DAT_1003a024 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a028
undefined4 DAT_1003a028 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a02c
undefined4 DAT_1003a02c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a030
undefined4 DAT_1003a030 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a034
undefined4 DAT_1003a034 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a038
undefined4 DAT_1003a038 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a03c
undefined4 DAT_1003a03c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a040
undefined4 DAT_1003a040 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a084
undefined4 DAT_1003a084 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a088
undefined4 DAT_1003a088 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a08c
undefined4 DAT_1003a08c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a198
undefined4 DAT_1003a198 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a19c
undefined4 DAT_1003a19c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a1a0
undefined4 DAT_1003a1a0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a1a4
undefined4 DAT_1003a1a4 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a1a8
undefined1 DAT_1003a1a8 = 0x00;

// GLOBAL: DRAWCARDLIB 0x1003a1a9
undefined1 DAT_1003a1a9 = 0x00;

// GLOBAL: DRAWCARDLIB 0x1003a9ac
undefined4 DAT_1003a9ac = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a9b0
undefined4 DAT_1003a9b0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a9d4
undefined4 DAT_1003a9d4 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a9d8
undefined4 DAT_1003a9d8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a9e0
undefined4 DAT_1003a9e0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003ab58
undefined4 DAT_1003ab58 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003ab5c
undefined4 DAT_1003ab5c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003ab60
undefined4 DAT_1003ab60 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003ab64
undefined4 DAT_1003ab64 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f1f68
undefined4 DAT_100f1f68 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f1f6c
undefined4 DAT_100f1f6c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f2e30
undefined1 DAT_100f2e30 = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f2e34
undefined1 DAT_100f2e34 = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f2e38
undefined1 DAT_100f2e38 = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f2e3c
undefined1 DAT_100f2e3c = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f23b4
undefined4 DAT_100f23b4 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f23b8
undefined4 DAT_100f23b8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f23c0
undefined4 DAT_100f23c0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f23c8
undefined4 DAT_100f23c8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f23cc
undefined4 DAT_100f23cc = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f23d0
undefined4 DAT_100f23d0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f23d4
undefined4 DAT_100f23d4 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f23d8
undefined4 DAT_100f23d8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f23dc
undefined4 DAT_100f23dc = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f37d0
char DAT_100f37d0[100 * 1024];

// GLOBAL: DRAWCARDLIB 0x100f3833
char DAT_100f3833[100 * 1024];

// GLOBAL: DRAWCARDLIB 0x10124510
undefined4 _DAT_10124510 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10124538
undefined4 DAT_10124538 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10124540
undefined4 DAT_10124540 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10124544
undefined4 DAT_10124544 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10124550
undefined1 DAT_10124550 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10124551
undefined1 DAT_10124551 = 0x00;

// GLOBAL: DRAWCARDLIB 0x1012655c
undefined4 DAT_1012655c = 0x00000000;

// FUNCTION: DRAWCARDLIB 0x10004970
undefined4 prepare_fonts_and_imgs(void)
{
  LOGFONTA *pLVar1;
  undefined4 uVar2;
  CHAR local_10c [264];
  
  strcpy(&DAT_1003a090,&global_base_directory);
  strcat(&DAT_1003a090,s__DuelArt_1002201c);
  strcat(&DAT_1003a090,s__Duel_dat_10022028);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0530m__TTF_10022034);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Magis____TTF_10022044);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0127m__TTF_10022054);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0085m__TTF_10022064);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0298m__TTF_10022074);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0299m__TTF_10022084);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0300m__TTF_10022094);
  AddFontResourceA(local_10c);
  sprintf(local_10c,s__s_ManaSymbols_pic_100220a4,&DAT_100f36c0);
  DAT_1003a198 = load_pic(local_10c);
  sprintf(local_10c,s__s_CardSets_pic_100220b8,&DAT_100f36c0);
  DAT_10039fe4 = load_pic(local_10c);
  sprintf(local_10c,s__s_CardBack_pic_100220c8,&DAT_100f36c0);
  DAT_1003a010 = load_pic(local_10c);
  pLVar1 = (LOGFONTA *)FUN_10004e82(s_BigCardTitle_100220d8,0);
  DAT_1003a008 = CreateFontIndirectA(pLVar1);
  pLVar1 = (LOGFONTA *)FUN_10004e82(s_BigCardSubtitle_100220e8,0);
  DAT_10039fdc = CreateFontIndirectA(pLVar1);
  pLVar1 = (LOGFONTA *)FUN_10004e82(s_BigCardPT_100220f8,0);
  DAT_10039ff8 = CreateFontIndirectA(pLVar1);
  pLVar1 = (LOGFONTA *)FUN_10004e82(s_BigCardText_10022104,0);
  DAT_1003a000 = CreateFontIndirectA(pLVar1);
  pLVar1 = (LOGFONTA *)FUN_10004e82(s_BigCardText_10022110,1);
  DAT_1003a040 = CreateFontIndirectA(pLVar1);
  pLVar1 = (LOGFONTA *)FUN_10004e82(s_SmallCardTitle_1002211c,0);
  DAT_1003a018 = CreateFontIndirectA(pLVar1);
  DAT_10039ffc = FUN_1000b00c(0xbe);
  DAT_1003a02c = FUN_1000b00c(0xca);
  DAT_10039fec = FUN_1000b00c(0xc9);
  DAT_10039fe0 = FUN_1000b00c(0xbf);
  DAT_1003a08c = FUN_1000b00c(0xd8);
  DAT_1003a19c = FUN_1000b00c(0x31);
  DAT_1003a1a0 = FUN_1000b00c(0xc9);
  DAT_1012655c = FUN_1000b00c(0xc4);
  DAT_1003a1a4 = FUN_1000b00c(0xc4);
  DAT_1003a01c = CreatePen(0,0,DAT_1003a1a4);
  DAT_1003a030 = CreateSolidBrush(DAT_10039ffc);
  DAT_1003a038 = CreateSolidBrush(DAT_1003a02c);
  DAT_1003a024 = 0;
  DAT_10039ff4 = 0;
  DAT_1003a03c = 0;
  DAT_1003a088 = 0;
  DAT_1003a034 = 0;
  DAT_10039fd4 = 0;
  DAT_10039fcc = 0;
  DAT_1003a028 = 0;
  DAT_10039fc8 = 0;
  DAT_1003a084 = 0;
  DAT_10039fd0 = 0;
  DAT_1003a014 = 0;
  DAT_1003a00c = 0;
  DAT_10039fe8 = 0;
  DAT_1003a004 = 0;
  DAT_10039ff0 = 0;
  DAT_10039fd8 = 0;
  DAT_1003a020 = 0;
  if ((((((DAT_1003a198 == 0) || (DAT_10039fe4 == 0)) || (DAT_1003a010 == 0)) ||
       ((DAT_1003a008 == (HFONT)0x0 || (DAT_10039fdc == (HFONT)0x0)))) ||
      ((DAT_1003a000 == (HFONT)0x0 || ((DAT_1003a040 == (HFONT)0x0 || (DAT_1003a018 == (HFONT)0x0)))
       ))) || ((DAT_1003a01c == (HPEN)0x0 ||
               ((DAT_1003a030 == (HBRUSH)0x0 || (DAT_1003a038 == (HBRUSH)0x0)))))) {
    FUN_10004f8e();
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

// FUNCTION: DRAWCARDLIB 0x10004e82
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined * FUN_10004e82(undefined4 param_1,int param_2)

{
  CHAR local_6c [100];
  UINT local_8;
  
  memcpy(&DAT_1003a048,&DAT_10021fe0,0x3c);
  strcpy(local_6c,DAT_1002212c);
  strcat(local_6c,param_1);
  _DAT_1003a048 = GetPrivateProfileIntA(s_Fonts_10022134,local_6c,0x14,&DAT_1003a090);
  strcpy(local_6c,DAT_1002213c);
  strcat(local_6c,param_1);
  local_8 = GetPrivateProfileIntA(s_Fonts_10022144,local_6c,0,&DAT_1003a090);
  if (local_8 != 0) {
    _DAT_1003a058 = 700;
  }
  if (param_2 != 0) {
    DAT_1003a05c = 1;
  }
  strcpy(local_6c,DAT_1002214c);
  strcat(local_6c,param_1);
  GetPrivateProfileStringA
            (s_Fonts_10022164,local_6c,s_MS_Sans_Serif_10022154,&DAT_1003a064,0x20,&DAT_1003a090);
  return &DAT_1003a048;
}

// FUNCTION: DRAWCARDLIB 0x10004f8e
void FUN_10004f8e(void)

{
  CHAR local_10c [264];
  
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Magim____TTF_1002216c);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0530m__TTF_1002217c);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Magis____TTF_1002218c);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0127m__TTF_1002219c);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0085m__TTF_100221ac);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0298m__TTF_100221bc);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0299m__TTF_100221cc);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&global_base_directory);
  strcat(local_10c,s__Tt0300m__TTF_100221dc);
  RemoveFontResourceA(local_10c);
  if (DAT_1003a198 != 0) {
    FUN_1000ac34(DAT_1003a198);
    DAT_1003a198 = 0;
  }
  if (DAT_10039fe4 != 0) {
    FUN_1000ac34(DAT_10039fe4);
    DAT_10039fe4 = 0;
  }
  if (DAT_1003a010 != 0) {
    FUN_1000ac34(DAT_1003a010);
    DAT_1003a010 = 0;
  }
  if (DAT_10039fcc != 0) {
    FUN_1000ac34(DAT_10039fcc);
    DAT_10039fcc = 0;
  }
  if (DAT_10039fd4 != 0) {
    FUN_1000ac34(DAT_10039fd4);
    DAT_10039fd4 = 0;
  }
  if (DAT_1003a034 != 0) {
    FUN_1000ac34(DAT_1003a034);
    DAT_1003a034 = 0;
  }
  if (DAT_1003a088 != 0) {
    FUN_1000ac34(DAT_1003a088);
    DAT_1003a088 = 0;
  }
  if (DAT_1003a03c != 0) {
    FUN_1000ac34(DAT_1003a03c);
    DAT_1003a03c = 0;
  }
  if (DAT_10039ff4 != 0) {
    FUN_1000ac34(DAT_10039ff4);
    DAT_10039ff4 = 0;
  }
  if (DAT_1003a024 != 0) {
    FUN_1000ac34(DAT_1003a024);
    DAT_1003a024 = 0;
  }
  if (DAT_10039fd8 != 0) {
    FUN_1000ac34(DAT_10039fd8);
    DAT_10039fd8 = 0;
  }
  if (DAT_10039ff0 != 0) {
    FUN_1000ac34(DAT_10039ff0);
    DAT_10039ff0 = 0;
  }
  if (DAT_1003a004 != 0) {
    FUN_1000ac34(DAT_1003a004);
    DAT_1003a004 = 0;
  }
  if (DAT_10039fe8 != 0) {
    FUN_1000ac34(DAT_10039fe8);
    DAT_10039fe8 = 0;
  }
  if (DAT_1003a00c != 0) {
    FUN_1000ac34(DAT_1003a00c);
    DAT_1003a00c = 0;
  }
  if (DAT_1003a014 != 0) {
    FUN_1000ac34(DAT_1003a014);
    DAT_1003a014 = 0;
  }
  if (DAT_10039fd0 != 0) {
    FUN_1000ac34(DAT_10039fd0);
    DAT_10039fd0 = 0;
  }
  if (DAT_1003a084 != 0) {
    FUN_1000ac34(DAT_1003a084);
    DAT_1003a084 = 0;
  }
  if (DAT_10039fc8 != 0) {
    FUN_1000ac34(DAT_10039fc8);
    DAT_10039fc8 = 0;
  }
  if (DAT_1003a028 != 0) {
    FUN_1000ac34(DAT_1003a028);
    DAT_1003a028 = 0;
  }
  if (DAT_1003a020 != 0) {
    FUN_1000ac34(DAT_1003a020);
    DAT_1003a020 = 0;
  }
  if (DAT_1003a008 != (HGDIOBJ)0x0) {
    DeleteObject(DAT_1003a008);
  }
  DAT_1003a008 = (HGDIOBJ)0x0;
  if (DAT_10039fdc != (HGDIOBJ)0x0) {
    DeleteObject(DAT_10039fdc);
  }
  DAT_10039fdc = (HGDIOBJ)0x0;
  if (DAT_10039ff8 != (HGDIOBJ)0x0) {
    DeleteObject(DAT_10039ff8);
  }
  DAT_10039ff8 = (HGDIOBJ)0x0;
  if (DAT_1003a000 != (HGDIOBJ)0x0) {
    DeleteObject(DAT_1003a000);
  }
  DAT_1003a000 = (HGDIOBJ)0x0;
  if (DAT_1003a040 != (HGDIOBJ)0x0) {
    DeleteObject(DAT_1003a040);
  }
  DAT_1003a040 = (HGDIOBJ)0x0;
  if (DAT_1003a018 != (HGDIOBJ)0x0) {
    DeleteObject(DAT_1003a018);
  }
  DAT_1003a018 = (HGDIOBJ)0x0;
  if (DAT_1003a01c != (HGDIOBJ)0x0) {
    DeleteObject(DAT_1003a01c);
  }
  DAT_1003a01c = (HGDIOBJ)0x0;
  if (DAT_1003a030 != (HGDIOBJ)0x0) {
    DeleteObject(DAT_1003a030);
  }
  DAT_1003a030 = (HGDIOBJ)0x0;
  if (DAT_1003a038 != (HGDIOBJ)0x0) {
    DeleteObject(DAT_1003a038);
  }
  DAT_1003a038 = (HGDIOBJ)0x0;
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000558b
void DestroyAllCardBackgrounds(void)

{
                    /* 0x558b  2  DestroyAllCardBackgrounds */
  if (DAT_10039fcc != 0) {
    FUN_1000ac34(DAT_10039fcc);
    DAT_10039fcc = 0;
  }
  if (DAT_10039fd4 != 0) {
    FUN_1000ac34(DAT_10039fd4);
    DAT_10039fd4 = 0;
  }
  if (DAT_1003a034 != 0) {
    FUN_1000ac34(DAT_1003a034);
    DAT_1003a034 = 0;
  }
  if (DAT_1003a088 != 0) {
    FUN_1000ac34(DAT_1003a088);
    DAT_1003a088 = 0;
  }
  if (DAT_1003a03c != 0) {
    FUN_1000ac34(DAT_1003a03c);
    DAT_1003a03c = 0;
  }
  if (DAT_10039ff4 != 0) {
    FUN_1000ac34(DAT_10039ff4);
    DAT_10039ff4 = 0;
  }
  if (DAT_1003a024 != 0) {
    FUN_1000ac34(DAT_1003a024);
    DAT_1003a024 = 0;
  }
  if (DAT_10039fd8 != 0) {
    FUN_1000ac34(DAT_10039fd8);
    DAT_10039fd8 = 0;
  }
  if (DAT_10039ff0 != 0) {
    FUN_1000ac34(DAT_10039ff0);
    DAT_10039ff0 = 0;
  }
  if (DAT_1003a004 != 0) {
    FUN_1000ac34(DAT_1003a004);
    DAT_1003a004 = 0;
  }
  if (DAT_10039fe8 != 0) {
    FUN_1000ac34(DAT_10039fe8);
    DAT_10039fe8 = 0;
  }
  if (DAT_1003a00c != 0) {
    FUN_1000ac34(DAT_1003a00c);
    DAT_1003a00c = 0;
  }
  if (DAT_1003a014 != 0) {
    FUN_1000ac34(DAT_1003a014);
    DAT_1003a014 = 0;
  }
  if (DAT_10039fd0 != 0) {
    FUN_1000ac34(DAT_10039fd0);
    DAT_10039fd0 = 0;
  }
  if (DAT_1003a084 != 0) {
    FUN_1000ac34(DAT_1003a084);
    DAT_1003a084 = 0;
  }
  if (DAT_10039fc8 != 0) {
    FUN_1000ac34(DAT_10039fc8);
    DAT_10039fc8 = 0;
  }
  if (DAT_1003a028 != 0) {
    FUN_1000ac34(DAT_1003a028);
    DAT_1003a028 = 0;
  }
  if (DAT_1003a020 != 0) {
    FUN_1000ac34(DAT_1003a020);
    DAT_1003a020 = 0;
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x10005830
void DrawCardBack(HDC param_1,RECT *param_2)

{
  int iVar1;
  int iVar2;
  HBRUSH pHVar3;
  RECT local_14;
  
                    /* 0x5830  3  DrawCardBack */
  if ((param_1 != (HDC)0x0) && (param_2 != (RECT *)0x0)) {
    if (DAT_1003a010 == 0) {
      pHVar3 = GetStockObject(4);
      FillRect(param_1,param_2,pHVar3);
    }
    else {
      iVar1 = ((param_2->right - param_2->left) * 3) / 100;
      if (iVar1 < 2) {
        iVar1 = 1;
      }
      iVar2 = ((param_2->bottom - param_2->top) * 2) / 100;
      if (iVar2 < 2) {
        iVar2 = 1;
      }
      pHVar3 = GetStockObject(4);
      FillRect(param_1,param_2,pHVar3);
      SetRect(&local_14,param_2->left + iVar1,param_2->top + iVar2,param_2->right - iVar1,
              param_2->bottom - iVar2);
      DrawBitmapToRect(param_1,&local_14,DAT_1003a010);
    }
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000592c
undefined4
DrawFullCard(HDC param_1,int *param_2,undefined4 *param_3,undefined4 param_4,uint param_5,
            int param_6,LPCSTR param_7)

{
  HGDIOBJ pvVar1;
  size_t sVar2;
  int iVar3;
  HBRUSH pHVar4;
  LPCSTR pCVar5;
  undefined4 uVar6;
  undefined1 local_578 [4];
  int local_574;
  int local_570;
  int local_560;
  undefined4 local_55c;
  undefined4 local_558;
  TEXTMETRICA local_554;
  RECT local_51c;
  int local_50c;
  uint local_508;
  RECT local_504;
  LPCSTR local_4f4;
  int local_4f0;
  int *local_4ec;
  RECT local_4e8;
  HBRUSH local_4d8;
  RECT local_4d4;
  RECT local_4c4;
  int local_4b4;
  COLORREF local_4b0;
  undefined1 local_4ac [1000];
  int local_c4;
  RECT local_c0;
  char local_b0 [52];
  RECT local_7c;
  undefined4 local_6c;
  HANDLE local_68;
  RECT local_64;
  RECT local_54;
  int local_44;
  RECT local_40;
  int local_30;
  int local_2c;
  int local_28;
  RECT local_24;
  RECT local_14;
  
                    /* 0x592c  4  DrawFullCard */
  if (((param_1 == (HDC)0x0) || (param_2 == (int *)0x0)) || (param_3 == (undefined4 *)0x0)) {
    local_6c = 0;
  }
  else {
    local_30 = SaveDC(param_1);
    local_c4 = 200;
    local_2c = 300;
    SetMapMode(param_1,7);
    SetWindowExtEx(param_1,local_c4,local_2c,(LPSIZE)0x0);
    SetViewportExtEx(param_1,param_2[2] - *param_2,param_2[3] - param_2[1],(LPSIZE)0x0);
    SetWindowOrgEx(param_1,local_c4 / 2,local_2c / 2,(LPPOINT)0x0);
    SetViewportOrgEx(param_1,*param_2 + (param_2[2] - *param_2) / 2,
                     param_2[1] + (param_2[3] - param_2[1]) / 2,(LPPOINT)0x0);
    local_44 = 6;
    local_28 = 6;
    SetRect(&local_40,10,5,0xbc,0x15);
    SetRect(&local_24,0xc,8,0xbe,0x14);
    SetRect(&local_4d4,0x15,0x19,0xb5,0xa4);
    SetRect(&local_64,0xc,0xa8,0xba,0xb3);
    SetRect(&local_14,0xc,0xa9,0xba,0xb4);
    SetRect(&local_54,0x1c,0xb9,0xae,0x10c);
    SetRect(&local_7c,0x14,0xb4,0xb6,0x10f);
    SetRect(&local_c0,0xc,0x114,0xbc,0x124);
    SetRect(&local_4c4,0xc,0x114,0xbc,0x124);
    if (((param_3[3] == -1) || ((*(byte *)(param_3 + 3) & 0x10) != 0)) ||
       ((*(byte *)(param_3 + 3) & 0x80) != 0)) {
      local_4d8 = CreateSolidBrush(DAT_10039ffc);
      SelectObject(param_1,local_4d8);
    }
    else {
      local_4d8 = CreateSolidBrush(DAT_1003a02c);
      SelectObject(param_1,local_4d8);
    }
    pvVar1 = GetStockObject(8);
    SelectObject(param_1,pvVar1);
    RoundRect(param_1,0,0,local_c4,local_2c,local_44 / 2,local_28 / 2);
    pvVar1 = GetStockObject(4);
    SelectObject(param_1,pvVar1);
    if (local_4d8 != (HBRUSH)0x0) {
      DeleteObject(local_4d8);
    }
    local_4ec = (int *)FUN_100067c7(param_3);
    FUN_10006a41(local_4ec);
    SetRect(&local_4e8,local_44,local_28,local_c4 - local_44,local_2c - local_28);
    if (*local_4ec == 0) {
      pHVar4 = GetStockObject(0);
      FillRect(param_1,&local_4e8,pHVar4);
    }
    else {
      DrawBitmapToRect(param_1,&local_4e8,*local_4ec);
    }
    local_68 = (HANDLE)*local_4ec;
    local_4b4 = 1;
    local_4b0 = FUN_1000b00c(0xbf);
    SelectObject(param_1,DAT_1003a008);
    SetBkMode(param_1,1);
    SetTextColor(param_1,DAT_10039fec);
    OffsetRect(&local_40,local_4b4,local_4b4);
    DrawTextA(param_1,(LPCSTR)param_3[1],-1,&local_40,0x824);
    OffsetRect(&local_40,-local_4b4,-local_4b4);
    SetTextColor(param_1,local_4b0);
    DrawTextA(param_1,(LPCSTR)param_3[1],-1,&local_40,0x824);
    local_4f0 = 100;
    SelectObject(param_1,DAT_10039ff8);
    if ((param_3[0x1f] != 0) || (param_3[0x20] != 0)) {
      local_b0[0] = '\0';
      if (param_3[0x1f] == local_4f0) {
        strcat(local_b0,DAT_100221ec);
      }
      else if (local_4f0 < (int)param_3[0x1f]) {
        iVar3 = param_3[0x1f] - local_4f0;
        pCVar5 = DAT_100221f0;
        sVar2 = strlen(local_b0);
        wsprintfA(local_b0 + sVar2,pCVar5,iVar3);
      }
      else {
        uVar6 = param_3[0x1f];
        pCVar5 = DAT_100221f8;
        sVar2 = strlen(local_b0);
        wsprintfA(local_b0 + sVar2,pCVar5,uVar6);
      }
      strcat(local_b0,DAT_100221fc);
      if (param_3[0x20] == local_4f0) {
        strcat(local_b0,DAT_10022200);
      }
      else if (local_4f0 < (int)param_3[0x20]) {
        iVar3 = param_3[0x20] - local_4f0;
        pCVar5 = DAT_10022204;
        sVar2 = strlen(local_b0);
        wsprintfA(local_b0 + sVar2,pCVar5,iVar3);
      }
      else {
        uVar6 = param_3[0x20];
        pCVar5 = DAT_1002220c;
        sVar2 = strlen(local_b0);
        wsprintfA(local_b0 + sVar2,pCVar5,uVar6);
      }
      SetTextColor(param_1,DAT_10039fec);
      OffsetRect(&local_4c4,local_4b4,local_4b4);
      DrawTextA(param_1,local_b0,-1,&local_4c4,0x26);
      OffsetRect(&local_4c4,-local_4b4,-local_4b4);
      SetTextColor(param_1,local_4b0);
      DrawTextA(param_1,local_b0,-1,&local_4c4,0x26);
    }
    SelectObject(param_1,DAT_10039fdc);
    SetBkMode(param_1,1);
    if ((param_3[5] != -1) && (param_3[5] != 0)) {
      local_4f4 = (LPCSTR)param_3[7];
      SetTextColor(param_1,DAT_10039fec);
      OffsetRect(&local_64,local_4b4,local_4b4);
      DrawTextA(param_1,local_4f4,-1,&local_64,0x24);
      OffsetRect(&local_64,-local_4b4,-local_4b4);
      SetTextColor(param_1,local_4b0);
      DrawTextA(param_1,local_4f4,-1,&local_64,0x24);
    }
    if (param_3[0x10] != 0) {
      wsprintfA(local_b0,param_7,param_3[0x10]);
      SetTextColor(param_1,DAT_10039fec);
      OffsetRect(&local_c0,local_4b4,local_4b4);
      DrawTextA(param_1,local_b0,-1,&local_c0,0x824);
      OffsetRect(&local_c0,-local_4b4,-local_4b4);
      SetTextColor(param_1,local_4b0);
      DrawTextA(param_1,local_b0,-1,&local_c0,0x824);
    }
    if (((param_3[5] != 5) && (param_3[5] != 8)) && (param_3[5] != 0)) {
      FUN_10006eed(param_1,&local_24,param_3 + 10);
    }
    if (param_3[3] != -1) {
      FUN_10006d5f(param_1,&local_14,param_3[3]);
    }
    CopyRect(&local_504,&local_4d4);
    LPtoDP(param_1,(LPPOINT)&local_504,2);
    if ((param_5 & 0xf) == 0) {
      iVar3 = IsBigArtIn(*param_3,param_4);
      if (iVar3 == 0) {
        DrawSmallArt(param_1,&local_4d4,*param_3,param_4);
      }
      else {
        DrawBigArt(param_1,&local_4d4,*param_3,param_4);
      }
    }
    else if ((param_5 & 0xf) == 1) {
      iVar3 = IsBigArtIn(*param_3,param_4);
      if (iVar3 == 0) {
        if (((param_5 & 0x10) != 0) && (iVar3 = IsSmallArtIn(*param_3,param_4), iVar3 != 0)) {
          DrawSmallArt(param_1,&local_4d4,*param_3,param_4);
        }
        LoadBigArt(*param_3,param_4,local_504.right - local_504.left,
                   local_504.bottom - local_504.top);
      }
      DrawBigArt(param_1,&local_4d4,*param_3,param_4);
    }
    else {
      iVar3 = IsBigArtIn(*param_3,param_4);
      if (((iVar3 == 0) && ((param_5 & 0x10) != 0)) &&
         (iVar3 = IsSmallArtIn(*param_3,param_4), iVar3 != 0)) {
        DrawSmallArt(param_1,&local_4d4,*param_3,param_4);
      }
      iVar3 = LoadBigArt(*param_3,param_4,local_504.right - local_504.left,
                         local_504.bottom - local_504.top);
      if (iVar3 == 0) {
        DrawSmallArt(param_1,&local_4d4,*param_3,param_4);
      }
      else {
        DrawBigArt(param_1,&local_4d4,*param_3,param_4);
      }
    }
    local_6c = IsBigArtRightSize(*param_3,param_4,local_504.right - local_504.left,
                                 local_504.bottom - local_504.top);
    strcpy(local_4ac,param_3[0x1d]);
    FUN_1000b06a(local_4ac,DAT_10022214,1,DAT_10022210);
    FUN_1000b06a(local_4ac,DAT_1002221c,1,DAT_10022218);
    FUN_1000b06a(local_4ac,DAT_10022224,1,DAT_10022220);
    FUN_1000b06a(local_4ac,DAT_1002222c,1,DAT_10022228);
    FUN_1000b06a(local_4ac,DAT_10022234,1,DAT_10022230);
    FUN_1000b06a(local_4ac,DAT_1002223c,1,DAT_10022238);
    FUN_1000b06a(local_4ac,DAT_10022244,1,DAT_10022240);
    FUN_1000b06a(local_4ac,DAT_1002224c,1,DAT_10022248);
    FUN_1000b06a(local_4ac,DAT_10022254,1,DAT_10022250);
    FUN_1000b06a(local_4ac,DAT_1002225c,1,DAT_10022258);
    SetTextColor(param_1,DAT_1003a1a0);
    SetBkMode(param_1,1);
    if (param_6 != 0) {
      SelectObject(param_1,DAT_1003a000);
      local_508 = CalcDrawManaText(param_1,&local_54,local_4ac);
      local_508 = local_508 >> 0x10;
      GetTextMetricsA(param_1,&local_554);
      CopyRect(&local_51c,&local_54);
      local_51c.top = local_51c.top + local_508 + local_554.tmHeight / 2;
      SelectObject(param_1,DAT_1003a040);
      DrawTextA(param_1,(LPCSTR)param_3[0x1e],-1,&local_51c,0x410);
      local_50c = local_51c.bottom - local_54.top;
      if (local_54.bottom - local_54.top < local_50c) {
        local_560 = local_54.top - local_7c.top;
        local_54.top = local_54.bottom - local_50c;
        local_7c.top = local_54.top - local_560;
        if (local_68 == (HANDLE)0x0) {
          pHVar4 = GetStockObject(0);
          FillRect(param_1,&local_7c,pHVar4);
        }
        else {
          GetObjectA(local_68,0x18,local_578);
          local_558 = 0x359;
          local_55c = 0x140;
          DrawBitmapSubrectToRect(param_1,&local_7c,local_68,(local_574 * 0x49) / 1000,
                       (local_570 * 0x25d) / 1000,(local_574 * 0x359) / 1000,
                       (local_570 * 0x140) / 1000);
        }
      }
    }
    SelectObject(param_1,DAT_1003a000);
    local_508 = DrawManaText(param_1,&local_54,local_4ac,1);
    local_508 = local_508 >> 0x10;
    GetTextMetricsA(param_1,&local_554);
    CopyRect(&local_51c,&local_54);
    local_51c.top = local_51c.top + local_508 + local_554.tmHeight / 3;
    SelectObject(param_1,DAT_1003a040);
    DrawTextA(param_1,(LPCSTR)param_3[0x1e],-1,&local_51c,0x10);
    RestoreDC(param_1,local_30);
  }
  return local_6c;
}

// FUNCTION: DRAWCARDLIB 0x100067c7
undefined4 * FUN_100067c7(int param_1)

{
  int iVar1;
  undefined4 *local_8;
  
  if (*(int *)(param_1 + 0x10) == 1) {
    local_8 = &DAT_1003a088;
  }
  else if (*(int *)(param_1 + 0x10) == 8) {
    local_8 = &DAT_10039fd4;
  }
  else if (*(int *)(param_1 + 0x10) == 7) {
    local_8 = &DAT_1003a03c;
  }
  else if (*(int *)(param_1 + 0x10) == 5) {
    local_8 = &DAT_10039fcc;
  }
  else if (*(int *)(param_1 + 0x10) == 2) {
    local_8 = &DAT_1003a034;
  }
  else if (*(int *)(param_1 + 0x10) == 4) {
    local_8 = &DAT_10039ff4;
  }
  else if (*(int *)(param_1 + 0x10) == 0) {
    local_8 = &DAT_1003a024;
  }
  else if (*(int *)(param_1 + 0x10) == 3) {
    local_8 = &DAT_1003a024;
  }
  else if (*(int *)(param_1 + 0x10) == 6) {
    if ((*(byte *)(param_1 + 0xc) & 2) == 0) {
      if ((*(byte *)(param_1 + 0xc) & 4) == 0) {
        if ((*(byte *)(param_1 + 0xc) & 0x20) == 0) {
          if ((*(byte *)(param_1 + 0xd) & 1) == 0) {
            if ((*(byte *)(param_1 + 0xc) & 8) == 0) {
              iVar1 = strcmp(*(char **)(param_1 + 4),&DAT_100f952c);
              if (iVar1 == 0) {
                local_8 = &DAT_10039fe8;
              }
              else {
                iVar1 = strcmp(*(char **)(param_1 + 4),&DAT_100f8140);
                if (iVar1 == 0) {
                  local_8 = &DAT_10039ff0;
                }
                else {
                  iVar1 = strcmp(*(char **)(param_1 + 4),&DAT_100f77e0);
                  if (iVar1 == 0) {
                    local_8 = &DAT_1003a00c;
                  }
                  else {
                    iVar1 = strcmp(*(char **)(param_1 + 4),&DAT_100f5b5c);
                    if (iVar1 == 0) {
                      local_8 = &DAT_10039fd8;
                    }
                    else {
                      iVar1 = strcmp(*(char **)(param_1 + 4),&DAT_100f6908);
                      if (iVar1 == 0) {
                        local_8 = &DAT_1003a004;
                      }
                      else {
                        local_8 = &DAT_1003a084;
                      }
                    }
                  }
                }
              }
            }
            else {
              local_8 = &DAT_1003a084;
            }
          }
          else {
            local_8 = &DAT_10039fc8;
          }
        }
        else {
          local_8 = &DAT_1003a014;
        }
      }
      else {
        local_8 = &DAT_1003a028;
      }
    }
    else {
      local_8 = &DAT_1003a084;
    }
  }
  else if (*(int *)(param_1 + 0x10) == -1) {
    local_8 = &DAT_1003a020;
  }
  else {
    local_8 = &DAT_1003a024;
  }
  return local_8;
}

// FUNCTION: DRAWCARDLIB 0x10006a41
undefined4 FUN_10006a41(int *param_1)

{
  undefined4 uVar1;
  int iVar2;
  char local_170 [264];
  char local_68 [100];
  
  if (param_1 == (int *)0x0) {
    uVar1 = 0;
  }
  else if (*param_1 == 0) {
    if (param_1 == &DAT_10039fcc) {
      strcpy(local_68,s_CARDBK_Green_10022260);
    }
    else if (param_1 == &DAT_10039fd4) {
      strcpy(local_68,s_CARDBK_White_10022270);
    }
    else if (param_1 == &DAT_1003a034) {
      strcpy(local_68,s_CARDBK_Blue_10022280);
    }
    else if (param_1 == &DAT_1003a088) {
      strcpy(local_68,s_CARDBK_Black_1002228c);
    }
    else if (param_1 == &DAT_1003a03c) {
      strcpy(local_68,s_CARDBK_Red_1002229c);
    }
    else if (param_1 == &DAT_10039ff4) {
      strcpy(local_68,s_CARDBK_Gold_100222a8);
    }
    else if (param_1 == &DAT_1003a024) {
      strcpy(local_68,s_CARDBK_Artifact_100222b4);
    }
    else if (param_1 == &DAT_10039fd8) {
      strcpy(local_68,s_CARDBK_GreenLand_100222c4);
    }
    else if (param_1 == &DAT_10039ff0) {
      strcpy(local_68,s_CARDBK_WhiteLand_100222d8);
    }
    else if (param_1 == &DAT_1003a004) {
      strcpy(local_68,s_CARDBK_BlueLand_100222ec);
    }
    else if (param_1 == &DAT_10039fe8) {
      strcpy(local_68,s_CARDBK_BlackLand_100222fc);
    }
    else if (param_1 == &DAT_1003a00c) {
      strcpy(local_68,s_CARDBK_RedLand_10022310);
    }
    else if (param_1 == &DAT_1003a014) {
      strcpy(local_68,s_CARDBK_DarklandsLand_10022320);
    }
    else if (param_1 == &DAT_10039fd0) {
      strcpy(local_68,s_CARDBK_FallenEmpiresLand_10022338);
    }
    else if (param_1 == &DAT_1003a084) {
      strcpy(local_68,s_CARDBK_AntiquitiesLand_10022354);
    }
    else if (param_1 == &DAT_10039fc8) {
      strcpy(local_68,s_CARDBK_LegendsLand_1002236c);
    }
    else if (param_1 == &DAT_1003a028) {
      strcpy(local_68,s_CARDBK_ArabianNightsLand_10022380);
    }
    else if (param_1 == &DAT_1003a020) {
      strcpy(local_68,s_CARDBK_Special_1002239c);
    }
    else {
      strcpy(local_68,&DAT_100223ac);
    }
    if (local_68[0] != '\0') {
      sprintf(local_170,s__s__s_pic_100223b0,&DAT_100f36c0,local_68);
      iVar2 = load_pic(local_170);
      *param_1 = iVar2;
    }
    if (*param_1 == 0) {
      uVar1 = 0;
    }
    else {
      uVar1 = 1;
    }
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

// FUNCTION: DRAWCARDLIB 0x10006d5f
void FUN_10006d5f(int param_1,int param_2,uint param_3)

{
  undefined1 local_44 [4];
  int local_40;
  int local_3c;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  RECT local_1c;
  int local_c;
  int local_8;
  
  if ((((((param_1 != 0) && (param_2 != 0)) && ((param_3 & 0x800) == 0)) &&
       ((param_3 != 0xffffffff && ((param_3 & 0x10) == 0)))) &&
      (((param_3 & 0x80) == 0 && (DAT_10039fe4 != (HANDLE)0x0)))) &&
     (((param_3 & 0x2e) != 0 || ((param_3 & 0x100) != 0)))) {
    GetObjectA(DAT_10039fe4,0x18,local_44);
    local_24 = local_40 / 10;
    local_2c = local_3c;
    if ((param_3 & 0x20) == 0) {
      if ((param_3 & 0x100) == 0) {
        if ((param_3 & 4) == 0) {
          if ((param_3 & 2) == 0) {
            if ((param_3 & 8) != 0) {
              local_20 = local_24 << 3;
            }
          }
          else {
            local_20 = local_24 * 6;
          }
        }
        else {
          local_20 = local_24 << 2;
        }
      }
      else {
        local_20 = local_24 * 2;
      }
    }
    else {
      local_20 = 0;
    }
    local_c = local_24 + local_20;
    local_8 = *(int *)(param_2 + 0xc) - *(int *)(param_2 + 4);
    local_28 = (local_8 * local_24) / local_3c;
    SetRect(&local_1c,*(int *)(param_2 + 8) - local_28,*(int *)(param_2 + 4),*(int *)(param_2 + 8),
            *(int *)(param_2 + 4) + local_8);
    FUN_1000aab9(param_1,&local_1c,DAT_10039fe4,local_24,local_2c,local_20,0,local_c,0);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x10006eed
void FUN_10006eed(int param_1,int param_2,char *param_3)

{
  int iVar1;
  undefined1 uVar2;
  int iVar3;
  size_t sVar4;
  char local_48 [20];
  undefined4 local_34;
  undefined4 local_30;
  int local_2c;
  char *local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  
  if (((param_1 != 0) && (param_2 != 0)) && (param_3 != (char *)0x0)) {
    local_20 = (int)*param_3;
    local_1c = (int)param_3[1];
    local_c = (int)param_3[8];
    local_14 = (int)param_3[5];
    local_10 = (int)param_3[7];
    local_18 = (int)param_3[2];
    FUN_10006ff3(&local_20,local_48);
    iVar3 = *(int *)(param_2 + 0xc) - *(int *)(param_2 + 4);
    iVar1 = *(int *)(param_2 + 8);
    local_24 = iVar3;
    sVar4 = strlen(local_48);
    local_2c = iVar1 - (sVar4 >> 1) * iVar3;
    local_34 = *(undefined4 *)(param_2 + 4);
    local_28 = local_48;
    while (*local_28 != '\0') {
      uVar2 = FUN_10007296(&local_28);
      local_30 = (uint)uVar2;
      FUN_100074b2(param_1,local_30,local_2c,local_34,iVar3,local_24);
      local_2c = local_2c + iVar3;
    }
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x10006ff3
int FUN_10006ff3(int *param_1,int param_2)

{
  int local_58;
  int local_54;
  int local_50;
  char local_4c [52];
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
  if ((param_1 == (int *)0x0) || (param_2 == 0)) {
    local_50 = 0;
  }
  else {
    local_10 = param_1[5];
    local_8 = param_1[3];
    local_14 = param_1[4];
    local_58 = param_1[1];
    local_54 = param_1[2];
    local_18 = *param_1;
    local_50 = 0;
    if (local_18 != 0) {
      if (local_18 == -1) {
        local_4c[0] = '|';
        local_4c[1] = 'X';
        local_50 = 2;
      }
      else if (local_18 == 0x48) {
        local_4c[0] = '|';
        local_4c[1] = 'X';
        local_50 = 2;
      }
      else if (local_18 == 0x28) {
        local_4c[0] = '|';
        local_4c[1] = 'X';
        local_50 = 2;
      }
      else if ((local_18 < 1) || (9 < local_18)) {
        if (local_18 == 10) {
          local_4c[0] = '|';
          local_4c[1] = '1';
          local_4c[2] = 0x30;
          local_50 = 3;
        }
      }
      else {
        local_4c[0] = '|';
        local_4c[1] = (char)local_18 + '0';
        local_50 = 2;
      }
    }
    local_c = 0;
    while (local_c == 0) {
      if (local_10 == 0) {
        if (local_8 == 0) {
          if (local_14 == 0) {
            if (local_58 == 0) {
              if (local_54 == 0) {
                local_4c[local_50] = '\0';
                local_c = 1;
              }
              else {
                local_4c[local_50] = '|';
                local_4c[local_50 + 1] = 'U';
                local_50 = local_50 + 2;
                local_54 = local_54 + -1;
              }
            }
            else {
              local_4c[local_50] = '|';
              local_4c[local_50 + 1] = 'B';
              local_50 = local_50 + 2;
              local_58 = local_58 + -1;
            }
          }
          else {
            local_4c[local_50] = '|';
            local_4c[local_50 + 1] = 'R';
            local_50 = local_50 + 2;
            local_14 = local_14 + -1;
          }
        }
        else {
          local_4c[local_50] = '|';
          local_4c[local_50 + 1] = 'G';
          local_50 = local_50 + 2;
          local_8 = local_8 + -1;
        }
      }
      else {
        local_4c[local_50] = '|';
        local_4c[local_50 + 1] = 'W';
        local_50 = local_50 + 2;
        local_10 = local_10 + -1;
      }
    }
    if ((((local_18 == 0) && (local_50 == 0)) && (local_10 == 0)) &&
       (((local_8 == 0 && (local_14 == 0)) && ((local_58 == 0 && (local_54 == 0)))))) {
      strcpy(local_4c,&DAT_100223bc);
    }
    if (param_2 != 0) {
      strcpy(param_2,local_4c);
    }
  }
  return local_50;
}

// FUNCTION: DRAWCARDLIB 0x10007296
int FUN_10007296(int *param_1)

{
  char local_c;
  char *local_8;
  
  local_8 = (char *)*param_1;
  local_c = '\0';
  if (*local_8 == '|') {
    if (local_8[1] == 'X') {
      local_c = -0x10;
    }
    else if ((local_8[1] == '1') && (local_8[2] == '0')) {
      local_c = -0x11;
      local_8 = local_8 + 1;
    }
    else if (local_8[1] == '0') {
      local_c = -0xf;
    }
    else if (local_8[1] == '1') {
      local_c = -0xe;
    }
    else if (local_8[1] == '2') {
      local_c = -0xd;
    }
    else if (local_8[1] == '3') {
      local_c = -0xc;
    }
    else if (local_8[1] == '4') {
      local_c = -0xb;
    }
    else if (local_8[1] == '5') {
      local_c = -10;
    }
    else if (local_8[1] == '6') {
      local_c = -9;
    }
    else if (local_8[1] == '7') {
      local_c = -8;
    }
    else if (local_8[1] == '8') {
      local_c = -7;
    }
    else if (local_8[1] == '9') {
      local_c = -6;
    }
    else if (local_8[1] == 'T') {
      local_c = -0x12;
    }
    else if (local_8[1] == 'B') {
      local_c = -2;
    }
    else if (local_8[1] == 'U') {
      local_c = -3;
    }
    else if (local_8[1] == 'W') {
      local_c = -5;
    }
    else if (local_8[1] == 'G') {
      local_c = -1;
    }
    else if (local_8[1] == 'R') {
      local_c = -4;
    }
  }
  if (local_c != '\0') {
    local_8 = local_8 + 2;
  }
  *param_1 = (int)local_8;
  return (int)local_c;
}

// FUNCTION: DRAWCARDLIB 0x100074b2
void FUN_100074b2(int param_1,char param_2,int param_3,int param_4,int param_5,int param_6)

{
  undefined1 local_3c [4];
  int local_38;
  int local_34;
  int local_24;
  int local_20;
  int local_1c;
  RECT local_18;
  int local_8;
  
  if (((param_1 != 0) && (-0x13 < param_2)) && (param_2 < '\0')) {
    GetObjectA(DAT_1003a198,0x18,local_3c);
    local_20 = local_34;
    local_24 = local_34;
    local_8 = local_38 - local_34;
    if (param_2 == -0x10) {
      local_1c = 0;
    }
    else if (param_2 == -0xf) {
      local_1c = local_34;
    }
    else if (param_2 == -0xe) {
      local_1c = local_34 * 2;
    }
    else if (param_2 == -0xd) {
      local_1c = local_34 * 3;
    }
    else if (param_2 == -0xc) {
      local_1c = local_34 << 2;
    }
    else if (param_2 == -0xb) {
      local_1c = local_34 * 5;
    }
    else if (param_2 == -10) {
      local_1c = local_34 * 6;
    }
    else if (param_2 == -9) {
      local_1c = local_34 * 7;
    }
    else if (param_2 == -8) {
      local_1c = local_34 << 3;
    }
    else if (param_2 == -7) {
      local_1c = local_34 * 9;
    }
    else if (param_2 == -6) {
      local_1c = local_34 * 10;
    }
    else if (param_2 == -0x11) {
      local_1c = local_34 * 0xb;
    }
    else if (param_2 == -5) {
      local_1c = local_34 * 0xc;
    }
    else if (param_2 == -4) {
      local_1c = local_34 * 0xd;
    }
    else if (param_2 == -3) {
      local_1c = local_34 * 0xe;
    }
    else if (param_2 == -2) {
      local_1c = local_34 * 0xf;
    }
    else if (param_2 == -1) {
      local_1c = local_34 << 4;
    }
    else if (param_2 == -0x12) {
      local_1c = local_34 * 0x11;
    }
    SetRect(&local_18,param_3,param_4,param_5 + param_3,param_6 + param_4);
    FUN_1000aab9(param_1,&local_18,DAT_1003a198,local_20,local_24,local_1c,0,local_8,0);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000775c
undefined4 CalcDrawManaText(HDC param_1,int param_2,int param_3)

{
  undefined4 uVar1;
  int nSavedDC;
  
                    /* 0x775c  1  CalcDrawManaText */
  if (((param_1 == (HDC)0x0) || (param_2 == 0)) || (param_3 == 0)) {
    uVar1 = 0;
  }
  else {
    nSavedDC = SaveDC(param_1);
    IntersectClipRect(param_1,0,0,1,1);
    uVar1 = DrawManaText(param_1,param_2,param_3,0);
    RestoreDC(param_1,nSavedDC);
  }
  return uVar1;
}

// FUNCTION: DRAWCARDLIB 0x100077dd
uint DrawManaText(HDC param_1,int *param_2,char *param_3,int param_4)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  HGDIOBJ pvVar5;
  size_t sVar6;
  char local_b4 [52];
  int local_80;
  int local_7c;
  size_t local_78;
  int local_74;
  int local_70;
  int local_6c;
  char local_68;
  uint local_64;
  int local_60;
  TEXTMETRICA local_5c;
  int local_24;
  RECT local_20;
  SIZE local_10;
  int local_8;
  
                    /* 0x77dd  5  DrawManaText */
  if (((param_1 == (HDC)0x0) || (param_2 == (int *)0x0)) || (param_3 == (char *)0x0)) {
    uVar2 = 0;
  }
  else if (*param_3 == '\0') {
    uVar2 = 0;
  }
  else {
    local_74 = SaveDC(param_1);
    local_64 = 0;
    local_8 = 0;
    GetTextMetricsA(param_1,&local_5c);
    iVar3 = local_5c.tmExternalLeading + local_5c.tmHeight;
    SetRect(&local_20,0,0,0,local_5c.tmHeight);
    LPtoDP(param_1,(LPPOINT)&local_20,2);
    SetRect(&local_20,0,0,local_20.bottom - local_20.top,0);
    DPtoLP(param_1,(LPPOINT)&local_20,2);
    local_80 = ((local_20.right - local_20.left) * 0x4b) / 100;
    iVar4 = ((local_20.right - local_20.left) * 0x55) / 100;
    local_24 = (local_5c.tmHeight * 0x4b) / 100;
    local_7c = local_5c.tmHeight;
    IntersectClipRect(param_1,*param_2,param_2[1],param_2[2] + 1,param_2[3]);
    pvVar5 = GetStockObject(4);
    SelectObject(param_1,pvVar5);
    pvVar5 = GetStockObject(8);
    SelectObject(param_1,pvVar5);
    local_60 = *param_2;
    local_70 = param_2[1];
    while (*param_3 != '\0') {
      if (*param_3 == ' ') {
        local_6c = 0;
        local_b4[0] = *param_3;
        iVar1 = local_6c;
        while( true ) {
          local_6c = iVar1 + 1;
          param_3 = param_3 + 1;
          if ((*param_3 == '\0') || (*param_3 != ' ')) break;
          local_b4[iVar1 + 1] = *param_3;
          iVar1 = local_6c;
        }
        local_b4[iVar1 + 1] = '\0';
        GetTextExtentPoint32A(param_1,local_b4,local_6c,&local_10);
        local_60 = local_60 + local_10.cx;
      }
      else if ((*param_3 == '\0') || (*param_3 != '\n')) {
        local_68 = FUN_10007296(&param_3);
        if (local_68 == '\0') {
          local_6c = 0;
          local_b4[0] = *param_3;
          iVar1 = local_6c;
          while( true ) {
            local_6c = iVar1 + 1;
            param_3 = param_3 + 1;
            if (((*param_3 == '\0') || (*param_3 == ' ')) ||
               ((*param_3 == '\n' || (*param_3 == '|')))) break;
            local_b4[iVar1 + 1] = *param_3;
            iVar1 = local_6c;
          }
          local_b4[iVar1 + 1] = '\0';
          GetTextExtentPoint32A(param_1,local_b4,local_6c,&local_10);
          if (param_2[2] < local_10.cx + local_60) {
            uVar2 = local_60 - *param_2;
            if (local_60 - *param_2 <= (int)local_64) {
              uVar2 = local_64;
            }
            local_70 = local_70 + iVar3;
            local_60 = *param_2;
            local_64 = uVar2;
          }
          sVar6 = strlen(local_b4);
          TextOutA(param_1,local_60,local_70,local_b4,sVar6);
          local_60 = local_60 + local_10.cx;
        }
        else {
          local_6c = 0;
          local_b4[0] = local_68;
          while ((local_6c = local_6c + 1, *param_3 != '\0' &&
                 (local_68 = FUN_10007296(&param_3), local_68 != '\0'))) {
            local_b4[local_6c] = local_68;
          }
          local_b4[local_6c] = '\0';
          local_10.cx = local_6c * local_80;
          if (param_2[2] < local_10.cx + local_60) {
            uVar2 = local_60 - *param_2;
            if (local_60 - *param_2 <= (int)local_64) {
              uVar2 = local_64;
            }
            local_70 = local_70 + iVar3;
            local_60 = *param_2;
            local_64 = uVar2;
          }
          local_78 = strlen(local_b4);
          for (local_6c = 0; local_6c < (int)local_78; local_6c = local_6c + 1) {
            if (param_4 == 0) {
              Ellipse(param_1,local_60 + (iVar4 - local_80) / 2,local_70 + (local_7c - local_24) / 2
                      ,local_80 + (iVar4 - local_80) / 2 + local_60,
                      local_24 + (local_7c - local_24) / 2 + local_70);
            }
            else {
              FUN_100074b2(param_1,*(undefined4 *)(local_b4 + local_6c),
                           local_60 + (iVar4 - local_80) / 2,local_70 + (local_7c - local_24) / 2,
                           local_80,local_24);
            }
            local_60 = local_60 + iVar4;
          }
          if ((*param_3 == ':') || (*param_3 == ',')) {
            local_b4[0] = *param_3;
            param_3 = param_3 + 1;
            local_6c = 1;
            local_b4[1] = 0;
            GetTextExtentPoint32A(param_1,local_b4,1,&local_10);
            sVar6 = strlen(local_b4);
            TextOutA(param_1,local_60,local_70,local_b4,sVar6);
            local_60 = local_60 + local_10.cx;
          }
        }
      }
      else {
        param_3 = param_3 + 1;
        local_70 = local_70 + iVar3 + iVar3 / 3;
        local_60 = *param_2;
      }
    }
    uVar2 = local_60 - *param_2;
    if (local_60 - *param_2 <= (int)local_64) {
      uVar2 = local_64;
    }
    local_8 = (iVar3 + local_70) - param_2[1];
    local_64 = uVar2;
    RestoreDC(param_1,local_74);
    uVar2 = local_8 << 0x10 | local_64 & 0xffff;
  }
  return uVar2;
}

// FUNCTION: DRAWCARDLIB 0x10007dd7
void DrawSmallCard(HDC param_1,int *param_2,undefined4 *param_3,undefined4 param_4,int param_5)

{
  HGDIOBJ h;
  int iVar1;
  RECT local_34;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
                    /* 0x7dd7  6  DrawSmallCard */
  if (((param_1 != (HDC)0x0) && (param_2 != (int *)0x0)) && (param_3 != (undefined4 *)0x0)) {
    local_8 = SaveDC(param_1);
    SetMapMode(param_1,8);
    SetWindowExtEx(param_1,200,0x118,(LPSIZE)0x0);
    SetViewportExtEx(param_1,param_2[2] - *param_2,param_2[3] - param_2[1],(LPSIZE)0x0);
    SetWindowOrgEx(param_1,0,0,(LPPOINT)0x0);
    SetViewportOrgEx(param_1,*param_2,param_2[1],(LPPOINT)0x0);
    FUN_10008065(param_1,param_2,param_3);
    DrawSmallCardTitle(param_1,param_2,param_3[2],0,1);
    h = GetStockObject(5);
    SelectObject(param_1,h);
    SelectObject(param_1,DAT_1003a01c);
    Rectangle(param_1,0,0,200,0x118);
    SetMapMode(param_1,1);
    local_18 = param_2[2] - *param_2;
    local_1c = param_2[3] - param_2[1];
    local_20 = (local_18 * 0x12) / 0xe4;
    local_24 = (local_1c * 8) / 100 + (local_1c * 0xb) / 100 + -2;
    local_c = ((local_18 * 0xd3) / 0xe4 - local_20) + 1;
    local_14 = (local_1c * 0xb2) / 0xbf - local_24;
    SetRect(&local_34,local_20,local_24,local_c + local_20,local_14 + local_24);
    iVar1 = IsSmallArtIn(*param_3,param_4);
    if (iVar1 == 0) {
      LoadSmallArt(*param_3,param_4,local_c,local_14);
    }
    else if (param_5 != 0) {
      ReloadSmallArtIfWrongSize(*param_3,param_4,local_c,local_14);
    }
    local_10 = DrawSmallArt(param_1,&local_34,*param_3,param_4);
    if (local_10 == 0) {
      DrawBigArt(param_1,&local_34,*param_3,param_4);
    }
    RestoreDC(param_1,local_8);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x10008065
void FUN_10008065(HDC param_1,RECT *param_2,int param_3)

{
  HBRUSH hbr;
  undefined1 local_38 [4];
  undefined4 local_34;
  int local_30;
  int *local_20;
  RECT local_1c;
  undefined4 local_c;
  undefined4 local_8;
  
  if (((param_1 != (HDC)0x0) && (param_2 != (RECT *)0x0)) && (param_3 != 0)) {
    local_20 = (int *)FUN_100067c7(param_3);
    FUN_10006a41(local_20);
    if (*local_20 == 0) {
      hbr = GetStockObject(0);
      FillRect(param_1,param_2,hbr);
    }
    else {
      GetObjectA((HANDLE)*local_20,0x18,local_38);
      SetRect(&local_1c,0,0,200,0x118);
      DrawBitmapSubrectToRect(param_1,&local_1c,*local_20,0,0,local_34,(local_30 * 0x3b) / 100);
      local_8 = 0x1e;
      local_c = 0x16;
      SetRect(&local_1c,0,0x16,200,0x34);
      DrawBitmapSubrectToRect(param_1,&local_1c,*local_20,0,2,local_34,
                   (((local_30 * 0x3b) / 100) * 0xb) / 100 + 2);
    }
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x100081b2
void DrawSmallCardTitle(HDC param_1,int *param_2,int param_3,int param_4,int param_5)

{
  size_t sVar1;
  COLORREF local_70;
  char local_6c [100];
  int local_8;
  
                    /* 0x81b2  7  DrawSmallCardTitle */
  if (((param_1 != (HDC)0x0) && (param_2 != (int *)0x0)) && (param_3 != 0)) {
    strcpy(local_6c,param_3);
    local_8 = SaveDC(param_1);
    SetMapMode(param_1,8);
    SetWindowExtEx(param_1,200,0x118,(LPSIZE)0x0);
    SetViewportExtEx(param_1,param_2[2] - *param_2,param_2[3] - param_2[1],(LPSIZE)0x0);
    SetWindowOrgEx(param_1,0,0,(LPPOINT)0x0);
    SetViewportOrgEx(param_1,*param_2,param_2[1],(LPPOINT)0x0);
    if (param_4 == 0) {
      local_70 = DAT_10039fe0;
    }
    else if (param_4 == 2) {
      local_70 = DAT_1003a08c;
    }
    else {
      local_70 = DAT_1003a19c;
    }
    SelectObject(param_1,DAT_1003a018);
    SetTextAlign(param_1,0);
    if (param_5 == 0) {
      SetBkMode(param_1,2);
      SetBkColor(param_1,DAT_1012655c);
    }
    else {
      SetBkMode(param_1,1);
    }
    SetTextColor(param_1,DAT_10039fec);
    sVar1 = strlen(local_6c);
    TextOutA(param_1,5,1,local_6c,sVar1);
    SetTextColor(param_1,local_70);
    SetBkMode(param_1,1);
    sVar1 = strlen(local_6c);
    TextOutA(param_1,2,-2,local_6c,sVar1);
    RestoreDC(param_1,local_8);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000aab9
undefined4
FUN_1000aab9(HDC param_1,int *param_2,HANDLE param_3,int param_4,int param_5,int param_6,int param_7
            ,int param_8,int param_9)

{
  undefined4 uVar1;
  int local_30;
  undefined1 local_2c [24];
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
  if (((param_1 == (HDC)0x0) || (param_2 == (int *)0x0)) || (param_3 == (HANDLE)0x0)) {
    uVar1 = 0;
  }
  else {
    EnterCriticalSection((LPCRITICAL_SECTION)&global_critical_section_for_drawing);
    local_10 = SaveDC(param_1);
    SelectObject(DAT_10022500,param_3);
    GetObjectA(param_3,0x18,local_2c);
    local_8 = *param_2;
    local_c = param_2[1];
    if (param_2[2] < *param_2) {
      local_14 = param_4;
    }
    else {
      local_14 = param_2[2] - *param_2;
    }
    if (param_2[3] < param_2[1]) {
      local_30 = param_5;
    }
    else {
      local_30 = param_2[3] - param_2[1];
    }
    ApplyCardArtPaletteToDc(DAT_10022500);
    StretchBlt(param_1,local_8,local_c,local_14,local_30,DAT_10022500,param_8,param_9,param_4,
               param_5,0x8800c6);
    ApplyCardArtPaletteToDc(DAT_10022500);
    StretchBlt(param_1,local_8,local_c,local_14,local_30,DAT_10022500,param_6,param_7,param_4,
               param_5,0xee0086);
    RestoreDC(param_1,local_10);
    LeaveCriticalSection((LPCRITICAL_SECTION)&global_critical_section_for_drawing);
    uVar1 = 1;
  }
  return uVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000ac34
void FUN_1000ac34(HANDLE param_1)

{
  int iVar1;
  DWORD *pDVar2;
  OSVERSIONINFOA local_f4;
  HANDLE local_60;
  undefined1 local_5c [20];
  int local_48;
  HANDLE local_10;
  int local_c;
  int local_8;
  
  if (DAT_10022504 == -1) {
    local_f4.dwOSVersionInfoSize = 0x94;
    pDVar2 = &local_f4.dwMajorVersion;
    for (iVar1 = 0x24; iVar1 != 0; iVar1 = iVar1 + -1) {
      *pDVar2 = 0;
      pDVar2 = pDVar2 + 1;
    }
    GetVersionExA(&local_f4);
    if (local_f4.dwPlatformId == 1) {
      DAT_10022504 = 1;
    }
    else {
      DAT_10022504 = 0;
    }
  }
  if (param_1 != (HANDLE)0x0) {
    GetObjectA(param_1,0x54,local_5c);
    local_60 = local_10;
    local_8 = local_48 + local_c;
    DeleteObject(param_1);
    if ((local_60 != (HANDLE)0x0) && (DAT_10022504 != 0)) {
      CloseHandle(local_60);
    }
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000b00c
uint FUN_1000b00c(int param_1)

{
  return CONCAT12((&g_cardArtPalette)[param_1 * 4],
                  CONCAT11((&g_cardArtPalette)[param_1 * 4],(&g_cardArtPalette)[param_1 * 4])) | 0x2000000;
}

// FUNCTION: DRAWCARDLIB 0x1000b06a
void FUN_1000b06a(char *param_1,char *param_2,int param_3,char *param_4)

{
  size_t sVar1;
  int iVar2;
  int local_3fc;
  char local_3f8 [1000];
  size_t local_10;
  int local_c;
  char *local_8;
  
  if ((((param_1 != (char *)0x0) && (param_2 != (char *)0x0)) && (param_4 != (char *)0x0)) &&
     ((sVar1 = strlen(param_1), sVar1 != 0 && (sVar1 = strlen(param_2), sVar1 != 0)))) {
    local_10 = strlen(param_2);
    local_8 = param_1;
    local_3f8[0] = '\0';
    local_3fc = 0;
    while (*local_8 != '\0') {
      local_c = 0;
      if (((param_3 != 0) && (iVar2 = strncmp(local_8,param_2,local_10), iVar2 == 0)) ||
         ((param_3 == 0 && (iVar2 = _strnicmp(local_8,param_2,local_10), iVar2 == 0)))) {
        local_c = 1;
      }
      if (local_c == 0) {
        local_3f8[local_3fc] = *local_8;
        local_8 = local_8 + 1;
        local_3f8[local_3fc + 1] = '\0';
        local_3fc = local_3fc + 1;
      }
      else {
        strcat(local_3f8,param_4);
        sVar1 = strlen(param_4);
        local_8 = local_8 + local_10;
        local_3fc = local_3fc + sVar1;
      }
    }
    strcpy(param_1,local_3f8);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000bd90
uint DLLMain(undefined4 param_1,uint param_2,undefined4 param_3)
{
  uint uVar1;
  
  (void)param_3;
  switch(param_2) {
  case 0:
    FUN_10004f8e();
    FUN_1000c0ca();
    param_2 = 1;
    break;
  case 1:
    uVar1 = read_cfg(param_1);
    param_2 = prepare_fonts_and_imgs();
    param_2 = uVar1 & 1 & param_2;
    break;
  case 2:
    param_2 = 1;
    break;
  case 3:
    param_2 = 1;
  }
  return param_2;
}

// FUNCTION: DRAWCARDLIB 0x1000be22
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int read_cfg(undefined4 param_1)

{
  char *pcVar1;
  void *_DstBuf;
  char acStackY_4a4a0 [264];
  size_t sStackY_4a398;
  size_t sStackY_4a394;
  int iStackY_4a390;
  FILE *pFStackY_4a38c;
  undefined1 auStackY_4a388 [4];
  int aiStackY_4a384 [75993];
  int iVar2;
  
  iVar2 = 1;
  _DAT_10124510 = param_1;
  InitializeCriticalSection(&global_critical_section_for_catalog);
  GetModuleFileNameA((HMODULE)0x0,&global_base_directory,0x105);
  pcVar1 = strrchr(&global_base_directory,0x5c);
  *pcVar1 = '\0';
  strcpy((uint *)&DAT_100f36c0,(uint *)&global_base_directory);
  strcat((uint *)&DAT_100f36c0,(uint *)s__CARDART_10026a48);
  DAT_10021048 = 3;
  DAT_10124538 = 0x10;
  InitCardArtGdiResources();
  strcpy(acStackY_4a4a0,(uint *)&global_base_directory);
  strcat(acStackY_4a4a0,(uint *)s__CARDS_DAT_10026a54);
  pFStackY_4a38c = fopen(acStackY_4a4a0,DAT_10026a60);
  if (pFStackY_4a38c == (FILE *)0x0) {
    iVar2 = 0;
  }
  else {
    fread(&sStackY_4a398,4,1,pFStackY_4a38c);
    fread(&sStackY_4a394,4,1,pFStackY_4a38c);
    fread(auStackY_4a388,0x98,sStackY_4a398,pFStackY_4a38c);
    _DstBuf = malloc(sStackY_4a394);
    if (_DstBuf == (void *)0x0) {
      iVar2 = 0;
    }
    else {
      fread(_DstBuf,1,sStackY_4a394,pFStackY_4a38c);
      for (iStackY_4a390 = 0; iStackY_4a390 < (int)sStackY_4a398; iStackY_4a390 = iStackY_4a390 + 1)
      {
        aiStackY_4a384[iStackY_4a390 * 0x26] = aiStackY_4a384[iStackY_4a390 * 0x26] + (int)_DstBuf;
        strncpy(DAT_100f37d0 + iStackY_4a390 * 100,(char *)aiStackY_4a384[iStackY_4a390 * 0x26],
                 100);
        DAT_100f3833[iStackY_4a390 * 100] = 0;
      }
      free(_DstBuf);
    }
    fclose(pFStackY_4a38c);
  }
  if (iVar2 == 0) {
    for (iStackY_4a390 = 0; iStackY_4a390 < (int)sStackY_4a398; iStackY_4a390 = iStackY_4a390 + 1) {
      strcpy((uint *)(&DAT_100f37d0 + iStackY_4a390 * 100),(uint *)&DAT_10026a64);
    }
  }
  return iVar2;
}

// FUNCTION: DRAWCARDLIB 0x1000c0ca
void FUN_1000c0ca(void)
{
  ShutdownCardArtGdiResources();
  DeleteCriticalSection(&global_critical_section_for_catalog);
}