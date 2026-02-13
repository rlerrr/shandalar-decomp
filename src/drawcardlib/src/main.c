#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include "mystdbool.h"
#include "defs.h"
#include "sidlib/pcxw.h"
#include "sidlib/pic.h"
#include "cardartlib/CardArtLib.h"
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

// GLOBAL: DRAWCARDLIB 0x10021fe0
LOGFONTA global_template_font;

// GLOBAL: DRAWCARDLIB 0x1003a048
LOGFONTA global_temp_font;

// GLOBAL: DRAWCARDLIB 0x1003a090
char global_dual_dat_filename[260];

char global_cardart_directory[0x105];
undefined4 DAT_100f5b5c = 0;
undefined4 DAT_100f6908 = 0;
undefined4 DAT_100f77e0 = 0;
undefined4 DAT_100f8140 = 0;
undefined4 DAT_100f952c = 0;
undefined4 DAT_10126554 = 0;
undefined4 DAT_10128640 = 0;
undefined4 DAT_100223ac = 0;

// GLOBAL: DRAWCARDLIB 0x100223bc
char s__0_100223bc[] = "|0";

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
char s_size_1002212c[] = "size";

// GLOBAL: DRAWCARDLIB 0x1002213c
char s_bold_1002213c[] = "bold";

// GLOBAL: DRAWCARDLIB 0x1002214c
char s_font_1002214c[] = "font";

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
char s_empty_10022210[] = "";

// GLOBAL: DRAWCARDLIB 0x10022214
char s__H1_10022214[] = "|H1";

// GLOBAL: DRAWCARDLIB 0x10022218
char s_empty_10022218[] = "";

// GLOBAL: DRAWCARDLIB 0x1002221c
char s__H2_1002221c[] = "|H2";

// GLOBAL: DRAWCARDLIB 0x10022220
char s_empty_10022220[] = "";

// GLOBAL: DRAWCARDLIB 0x10022224
char s__H3_10022224[] = "|H3";

// GLOBAL: DRAWCARDLIB 0x10022228
char s_empty_10022228[] = "";

// GLOBAL: DRAWCARDLIB 0x1002222c
char S__H4_1002222c[] = "|H4";

// GLOBAL: DRAWCARDLIB 0x10022230
char s_empty_10022230[] = "";

// GLOBAL: DRAWCARDLIB 0x10022234
char s__H_10022234[] = "|H";

// GLOBAL: DRAWCARDLIB 0x10022238
char s_empty_10022238[] = "";

// GLOBAL: DRAWCARDLIB 0x1002223c
char s__S1_1002223c[] = "|S1";

// GLOBAL: DRAWCARDLIB 0x10022240
char s_empty_10022240[] = "";

// GLOBAL: DRAWCARDLIB 0x10022244
char s__S2_10022244[] = "|S2";

// GLOBAL: DRAWCARDLIB 0x10022248
char s_empty_10022248[] = "";

// GLOBAL: DRAWCARDLIB 0x1002224c
char S__S3_1002224c[] = "|S3";

// GLOBAL: DRAWCARDLIB 0x10022250
char s_empty_10022250[] = "";

// GLOBAL: DRAWCARDLIB 0x10022254
char s__S4_10022254[] = "|S4";

// GLOBAL: DRAWCARDLIB 0x10022258
char s_empty_10022258[] = "";

// GLOBAL: DRAWCARDLIB 0x1002225c
char s__S_1002225c[] = "|S";

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

// GLOBAL: DRAWCARDLIB 0x10022504
undefined4 global_dwPlatformId = 0xFFFFFFFF;

// GLOBAL: DRAWCARDLIB 0x10026a48
char s__CARDART_10026a48[] = "\\CARDART";

// GLOBAL: DRAWCARDLIB 0x10026a54
char s__CARDS_DAT_10026a54[] = "\\CARDS.DAT";

// GLOBAL: DRAWCARDLIB 0x10026a60
char s_rb_10026a60[] = "rb";

// GLOBAL: DRAWCARDLIB 0x10026a64
char s_File__s_could_not_be_opened__EXITING_10026a64[] = "File %s could not be opened: EXITING\r\n";

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
undefined4 global_CARDBK_LegendsLand = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fcc
undefined4 global_CARDBK_Green = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fd0
undefined4 global_CARDBK_FallenEmpiresLand = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fd4
undefined4 global_CARDBK_White = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fd8
undefined4 global_CARDBK_GreenLand = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fdc
undefined4 DAT_10039fdc = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fe0
COLORREF DAT_10039fe0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fe4
undefined4 global_CardSets = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fe8
undefined4 global_CARDBK_BlackLand = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039fec
COLORREF DAT_10039fec = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039ff0
undefined4 global_CARDBK_WhiteLand = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039ff4
undefined4 global_CARDBK_Gold = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039ff8
undefined4 DAT_10039ff8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10039ffc
COLORREF DAT_10039ffc = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a000
undefined4 DAT_1003a000 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a004
undefined4 global_CARDBK_BlueLand = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a008
undefined4 DAT_1003a008 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a00c
undefined4 global_CARDBK_RedLand = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a010
undefined4 global_CardBack = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a014
undefined4 global_CARDBK_DarklandsLand = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a018
undefined4 DAT_1003a018 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a01c
undefined4 DAT_1003a01c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a020
undefined4 global_CARDBK_Special = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a024
undefined4 global_CARDBK_Artifact = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a028
undefined4 global_CARDBK_ArabianNightsLand = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a02c
COLORREF DAT_1003a02c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a030
undefined4 DAT_1003a030 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a034
undefined4 global_CARDBK_Blue = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a038
undefined4 DAT_1003a038 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a03c
undefined4 global_CARDBK_Red = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a040
undefined4 DAT_1003a040 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a084
undefined4 global_CARDBK_AntiquitiesLand = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a088
undefined4 global_CARDBK_Black = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a08c
COLORREF DAT_1003a08c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a198
undefined4 global_ManaSymbols = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a19c
COLORREF DAT_1003a19c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a1a0
COLORREF DAT_1003a1a0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1003a1a4
COLORREF DAT_1003a1a4 = 0x00000000;

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
COLORREF DAT_1012655c = 0x00000000;

// FUNCTION: DRAWCARDLIB 0x10004970
undefined4 prepare_fonts_and_imgs(void)
{
  CHAR local_10c [264];
  
  strcpy(&global_dual_dat_filename,&global_base_directory);
  strcat(&global_dual_dat_filename,s__DuelArt_1002201c);
  strcat(&global_dual_dat_filename,s__Duel_dat_10022028);
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
  sprintf(local_10c,s__s_ManaSymbols_pic_100220a4,&global_cardart_directory);
  global_ManaSymbols = load_pic(local_10c);
  sprintf(local_10c,s__s_CardSets_pic_100220b8,&global_cardart_directory);
  global_CardSets = load_pic(local_10c);
  sprintf(local_10c,s__s_CardBack_pic_100220c8,&global_cardart_directory);
  global_CardBack = load_pic(local_10c);

  DAT_1003a008 = CreateFontIndirectA(LoadFontFromIni(s_BigCardTitle_100220d8,0));
  DAT_10039fdc = CreateFontIndirectA(LoadFontFromIni(s_BigCardSubtitle_100220e8,0));
  DAT_10039ff8 = CreateFontIndirectA(LoadFontFromIni(s_BigCardPT_100220f8,0));
  DAT_1003a000 = CreateFontIndirectA(LoadFontFromIni(s_BigCardText_10022104,0));
  DAT_1003a040 = CreateFontIndirectA(LoadFontFromIni(s_BigCardText_10022110,1));
  DAT_1003a018 = CreateFontIndirectA(LoadFontFromIni(s_SmallCardTitle_1002211c,0));
  DAT_10039ffc = GetPaletteColor(0xbe);
  DAT_1003a02c = GetPaletteColor(0xca);
  DAT_10039fec = GetPaletteColor(0xc9);
  DAT_10039fe0 = GetPaletteColor(0xbf);
  DAT_1003a08c = GetPaletteColor(0xd8);
  DAT_1003a19c = GetPaletteColor(0x31);
  DAT_1003a1a0 = GetPaletteColor(0xc9);
  DAT_1012655c = GetPaletteColor(0xc4);
  DAT_1003a1a4 = GetPaletteColor(0xc4);
  DAT_1003a01c = CreatePen(0,0,DAT_1003a1a4);
  DAT_1003a030 = CreateSolidBrush(DAT_10039ffc);
  DAT_1003a038 = CreateSolidBrush(DAT_1003a02c);
  global_CARDBK_Artifact = 0;
  global_CARDBK_Gold = global_CARDBK_Artifact;
  global_CARDBK_Red = global_CARDBK_Gold;
  global_CARDBK_Black = global_CARDBK_Red;
  global_CARDBK_Blue = global_CARDBK_Black;
  global_CARDBK_White = global_CARDBK_Blue;
  global_CARDBK_Green = global_CARDBK_White;
  global_CARDBK_ArabianNightsLand = 0;
  global_CARDBK_LegendsLand = global_CARDBK_ArabianNightsLand;
  global_CARDBK_AntiquitiesLand = global_CARDBK_LegendsLand;
  global_CARDBK_FallenEmpiresLand = global_CARDBK_AntiquitiesLand;
  global_CARDBK_DarklandsLand = global_CARDBK_FallenEmpiresLand;
  global_CARDBK_RedLand = global_CARDBK_DarklandsLand;
  global_CARDBK_BlackLand = global_CARDBK_RedLand;
  global_CARDBK_BlueLand = global_CARDBK_BlackLand;
  global_CARDBK_WhiteLand = global_CARDBK_BlueLand;
  global_CARDBK_GreenLand = global_CARDBK_WhiteLand;
  global_CARDBK_Special = 0;
  if ((((((global_ManaSymbols == 0) || (global_CardSets == 0)) || (global_CardBack == 0)) ||
       ((DAT_1003a008 == (HFONT)0x0 || (DAT_10039fdc == (HFONT)0x0)))) ||
      ((DAT_1003a000 == (HFONT)0x0 || ((DAT_1003a040 == (HFONT)0x0 || (DAT_1003a018 == (HFONT)0x0)))
       ))) || ((DAT_1003a01c == (HPEN)0x0 ||
               ((DAT_1003a030 == (HBRUSH)0x0 || (DAT_1003a038 == (HBRUSH)0x0)))))) {
    DestroyAllResources();
    return 0;
  }

  return 1;
}

// FUNCTION: DRAWCARDLIB 0x10004e82
LOGFONTA * LoadFontFromIni(char* param_1,int param_2)
{
  CHAR key [100];
  UINT local_8;
  
  memcpy(&global_temp_font,&global_template_font,sizeof(LOGFONTA));
  strcpy(key,s_size_1002212c);
  strcat(key,param_1);
  global_temp_font.lfHeight = GetPrivateProfileIntA(s_Fonts_10022134,key,0x14,&global_dual_dat_filename);

  strcpy(key,s_bold_1002213c);
  strcat(key,param_1);
  local_8 = GetPrivateProfileIntA(s_Fonts_10022144,key,0,&global_dual_dat_filename);
  if (local_8 != 0) {
    global_temp_font.lfWeight = FW_BOLD;
  }
  if (param_2 != 0) {
    global_temp_font.lfItalic = TRUE;
  }

  strcpy(key,s_font_1002214c);
  strcat(key,param_1);
  GetPrivateProfileStringA(s_Fonts_10022164,key,s_MS_Sans_Serif_10022154,&global_temp_font.lfFaceName,0x20,&global_dual_dat_filename);
  return &global_temp_font;
}

void __inline DestroyAllCardBackgrounds_impl(void)
{
  if (global_CARDBK_Green != 0) {
    DeleteAndCloseObject(global_CARDBK_Green);
    global_CARDBK_Green = 0;
  }
  if (global_CARDBK_White != 0) {
    DeleteAndCloseObject(global_CARDBK_White);
    global_CARDBK_White = 0;
  }
  if (global_CARDBK_Blue != 0) {
    DeleteAndCloseObject(global_CARDBK_Blue);
    global_CARDBK_Blue = 0;
  }
  if (global_CARDBK_Black != 0) {
    DeleteAndCloseObject(global_CARDBK_Black);
    global_CARDBK_Black = 0;
  }
  if (global_CARDBK_Red != 0) {
    DeleteAndCloseObject(global_CARDBK_Red);
    global_CARDBK_Red = 0;
  }
  if (global_CARDBK_Gold != 0) {
    DeleteAndCloseObject(global_CARDBK_Gold);
    global_CARDBK_Gold = 0;
  }
  if (global_CARDBK_Artifact != 0) {
    DeleteAndCloseObject(global_CARDBK_Artifact);
    global_CARDBK_Artifact = 0;
  }
  if (global_CARDBK_GreenLand != 0) {
    DeleteAndCloseObject(global_CARDBK_GreenLand);
    global_CARDBK_GreenLand = 0;
  }
  if (global_CARDBK_WhiteLand != 0) {
    DeleteAndCloseObject(global_CARDBK_WhiteLand);
    global_CARDBK_WhiteLand = 0;
  }
  if (global_CARDBK_BlueLand != 0) {
    DeleteAndCloseObject(global_CARDBK_BlueLand);
    global_CARDBK_BlueLand = 0;
  }
  if (global_CARDBK_BlackLand != 0) {
    DeleteAndCloseObject(global_CARDBK_BlackLand);
    global_CARDBK_BlackLand = 0;
  }
  if (global_CARDBK_RedLand != 0) {
    DeleteAndCloseObject(global_CARDBK_RedLand);
    global_CARDBK_RedLand = 0;
  }
  if (global_CARDBK_DarklandsLand != 0) {
    DeleteAndCloseObject(global_CARDBK_DarklandsLand);
    global_CARDBK_DarklandsLand = 0;
  }
  if (global_CARDBK_FallenEmpiresLand != 0) {
    DeleteAndCloseObject(global_CARDBK_FallenEmpiresLand);
    global_CARDBK_FallenEmpiresLand = 0;
  }
  if (global_CARDBK_AntiquitiesLand != 0) {
    DeleteAndCloseObject(global_CARDBK_AntiquitiesLand);
    global_CARDBK_AntiquitiesLand = 0;
  }
  if (global_CARDBK_LegendsLand != 0) {
    DeleteAndCloseObject(global_CARDBK_LegendsLand);
    global_CARDBK_LegendsLand = 0;
  }
  if (global_CARDBK_ArabianNightsLand != 0) {
    DeleteAndCloseObject(global_CARDBK_ArabianNightsLand);
    global_CARDBK_ArabianNightsLand = 0;
  }
  if (global_CARDBK_Special != 0) {
    DeleteAndCloseObject(global_CARDBK_Special);
    global_CARDBK_Special = 0;
  }
}

// FUNCTION: DRAWCARDLIB 0x10004f8e
void DestroyAllResources(void)
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
  if (global_ManaSymbols != 0) {
    DeleteAndCloseObject(global_ManaSymbols);
    global_ManaSymbols = 0;
  }
  if (global_CardSets != 0) {
    DeleteAndCloseObject(global_CardSets);
    global_CardSets = 0;
  }
  if (global_CardBack != 0) {
    DeleteAndCloseObject(global_CardBack);
    global_CardBack = 0;
  }

  DestroyAllCardBackgrounds_impl();
  
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
}

// FUNCTION: DRAWCARDLIB 0x1000558b
void DestroyAllCardBackgrounds(void) {
  DestroyAllCardBackgrounds_impl();
}

// FUNCTION: DRAWCARDLIB 0x10005830
void DrawCardBack(HDC dc,RECT *rect)
{
  int iVar1;
  int iVar2;
  HBRUSH pHVar3;
  RECT local_14;
  
                    /* 0x5830  3  DrawCardBack */
  if ((dc != (HDC)0x0) && (rect != (RECT *)0x0)) {
    if (global_CardBack == 0) {
      pHVar3 = GetStockObject(4);
      FillRect(dc,rect,pHVar3);
    }
    else {
      iVar1 = ((rect->right - rect->left) * 3) / 100;
      if (iVar1 < 2) {
        iVar1 = 1;
      }
      iVar2 = ((rect->bottom - rect->top) * 2) / 100;
      if (iVar2 < 2) {
        iVar2 = 1;
      }
      pHVar3 = GetStockObject(4);
      FillRect(dc,rect,pHVar3);
      SetRect(&local_14,rect->left + iVar1,rect->top + iVar2,rect->right - iVar1,
              rect->bottom - iVar2);
      DrawBitmapToRect(dc,&local_14,global_CardBack);
    }
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000592c
undefined4
DrawFullCard(HDC dc,RECT *rect,card_ptr_t *card,undefined4 param_4,uint param_5,
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
  char rules_text [1000];
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
  int nSavedDC;
  int local_2c;
  int local_28;
  RECT local_24;
  RECT local_14;
  
                    /* 0x592c  4  DrawFullCard */
  if (((dc == (HDC)0x0) || (rect == (int *)0x0)) || (card == (undefined4 *)0x0)) {
    local_6c = 0;
  }
  else {
    nSavedDC = SaveDC(dc);
    local_c4 = 200;
    local_2c = 300;
    SetMapMode(dc,7);
    SetWindowExtEx(dc,local_c4,local_2c,(LPSIZE)0x0);
    SetViewportExtEx(dc, rect->right - rect->left, rect->bottom - rect->top, (LPSIZE)0x0);
    SetWindowOrgEx(dc,local_c4 / 2,local_2c / 2,(LPPOINT)0x0);
    SetViewportOrgEx(dc,rect->left + (rect->right - rect->left) / 2,
                     rect->top + (rect->bottom - rect->top) / 2,(LPPOINT)0x0);
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
    if (((card->expansion == -1) || ((card->expansion & 0x10) != 0)) ||
       ((*(byte *)(card + 3) & 0x80) != 0)) {
      local_4d8 = CreateSolidBrush(DAT_10039ffc);
      SelectObject(dc,local_4d8);
    }
    else {
      local_4d8 = CreateSolidBrush(DAT_1003a02c);
      SelectObject(dc,local_4d8);
    }
    pvVar1 = GetStockObject(8);
    SelectObject(dc,pvVar1);
    RoundRect(dc,0,0,local_c4,local_2c,local_44 / 2,local_28 / 2);
    pvVar1 = GetStockObject(4);
    SelectObject(dc,pvVar1);
    if (local_4d8 != (HBRUSH)0x0) {
      DeleteObject(local_4d8);
    }
    local_4ec = (int *)GetCardBackground(card);
    LoadCardBackground(local_4ec);
    SetRect(&local_4e8,local_44,local_28,local_c4 - local_44,local_2c - local_28);
    if (*local_4ec == 0) {
      pHVar4 = GetStockObject(0);
      FillRect(dc,&local_4e8,pHVar4);
    }
    else {
      DrawBitmapToRect(dc,&local_4e8,*local_4ec);
    }
    local_68 = (HANDLE)*local_4ec;
    local_4b4 = 1;
    local_4b0 = GetPaletteColor(0xbf);
    SelectObject(dc,DAT_1003a008);
    SetBkMode(dc,1);
    SetTextColor(dc,DAT_10039fec);
    OffsetRect(&local_40,local_4b4,local_4b4);
    DrawTextA(dc,card->full_name,-1,&local_40,0x824);
    OffsetRect(&local_40,-local_4b4,-local_4b4);
    SetTextColor(dc,local_4b0);
    DrawTextA(dc,card->full_name,-1,&local_40,0x824);
    local_4f0 = 100;
    SelectObject(dc,DAT_10039ff8);
    if ((card->power != 0) || (card->toughness != 0)) {
      local_b0[0] = '\0';
      if (card->power == local_4f0) {
        strcat(local_b0,DAT_100221ec);
      }
      else if (local_4f0 < card->power) {
        iVar3 = card->power - local_4f0;
        pCVar5 = DAT_100221f0;
        sVar2 = strlen(local_b0);
        wsprintfA(local_b0 + sVar2,pCVar5,iVar3);
      }
      else {
        uVar6 = card->power;
        pCVar5 = DAT_100221f8;
        sVar2 = strlen(local_b0);
        wsprintfA(local_b0 + sVar2,pCVar5,uVar6);
      }
      strcat(local_b0,DAT_100221fc);
      if (card->toughness == local_4f0) {
        strcat(local_b0,DAT_10022200);
      }
      else if (local_4f0 < card->toughness) {
        iVar3 = card->toughness - local_4f0;
        pCVar5 = DAT_10022204;
        sVar2 = strlen(local_b0);
        wsprintfA(local_b0 + sVar2,pCVar5,iVar3);
      }
      else {
        uVar6 = card->toughness;
        pCVar5 = DAT_1002220c;
        sVar2 = strlen(local_b0);
        wsprintfA(local_b0 + sVar2,pCVar5,uVar6);
      }
      SetTextColor(dc,DAT_10039fec);
      OffsetRect(&local_4c4,local_4b4,local_4b4);
      DrawTextA(dc,local_b0,-1,&local_4c4,0x26);
      OffsetRect(&local_4c4,-local_4b4,-local_4b4);
      SetTextColor(dc,local_4b0);
      DrawTextA(dc,local_b0,-1,&local_4c4,0x26);
    }
    SelectObject(dc,DAT_10039fdc);
    SetBkMode(dc,1);
    if ((card->card_type != -1) && (card->card_type != 0)) {
      local_4f4 = card->type_text;
      SetTextColor(dc,DAT_10039fec);
      OffsetRect(&local_64,local_4b4,local_4b4);
      DrawTextA(dc,local_4f4,-1,&local_64,0x24);
      OffsetRect(&local_64,-local_4b4,-local_4b4);
      SetTextColor(dc,local_4b0);
      DrawTextA(dc,local_4f4,-1,&local_64,0x24);
    }
    if (card->artist != 0) {
      wsprintfA(local_b0,param_7,card->artist);
      SetTextColor(dc,DAT_10039fec);
      OffsetRect(&local_c0,local_4b4,local_4b4);
      DrawTextA(dc,local_b0,-1,&local_c0,0x824);
      OffsetRect(&local_c0,-local_4b4,-local_4b4);
      SetTextColor(dc,local_4b0);
      DrawTextA(dc,local_b0,-1,&local_c0,0x824);
    }
    if (((card->card_type != 5) && (card->card_type != 8)) && (card->card_type != 0)) {
      DrawCastingCost(dc,&local_24,&card->req);
    }
    if (card->expansion != -1) {
      DrawCardSet(dc,&local_14,card->expansion);
    }
    CopyRect(&local_504,&local_4d4);
    LPtoDP(dc,(LPPOINT)&local_504,2);
    if ((param_5 & 0xf) == 0) {
      iVar3 = IsBigArtIn(card->id,param_4);
      if (iVar3 == 0) {
        DrawSmallArt(dc,&local_4d4,card->id,param_4);
      }
      else {
        DrawBigArt(dc,&local_4d4,card->id,param_4);
      }
    }
    else if ((param_5 & 0xf) == 1) {
      iVar3 = IsBigArtIn(card->id,param_4);
      if (iVar3 == 0) {
        if (((param_5 & 0x10) != 0) && (iVar3 = IsSmallArtIn(card->id,param_4), iVar3 != 0)) {
          DrawSmallArt(dc,&local_4d4,card->id,param_4);
        }
        LoadBigArt(card->id,param_4,local_504.right - local_504.left,
                   local_504.bottom - local_504.top);
      }
      DrawBigArt(dc,&local_4d4,card->id,param_4);
    }
    else {
      iVar3 = IsBigArtIn(card->id,param_4);
      if (((iVar3 == 0) && ((param_5 & 0x10) != 0)) &&
         (iVar3 = IsSmallArtIn(card->id,param_4), iVar3 != 0)) {
        DrawSmallArt(dc,&local_4d4,card->id,param_4);
      }
      iVar3 = LoadBigArt(card->id,param_4,local_504.right - local_504.left,
                         local_504.bottom - local_504.top);
      if (iVar3 == 0) {
        DrawSmallArt(dc,&local_4d4,card->id,param_4);
      }
      else {
        DrawBigArt(dc,&local_4d4,card->id,param_4);
      }
    }
    local_6c = IsBigArtRightSize(card->id,param_4,local_504.right - local_504.left,
                                 local_504.bottom - local_504.top);
    strcpy(rules_text,card->rules_text);
    ReplaceSubstring(rules_text,s__H1_10022214,1,s_empty_10022210);
    ReplaceSubstring(rules_text,s__H2_1002221c,1,s_empty_10022218);
    ReplaceSubstring(rules_text,s__H3_10022224,1,s_empty_10022220);
    ReplaceSubstring(rules_text,S__H4_1002222c,1,s_empty_10022228);
    ReplaceSubstring(rules_text,s__H_10022234,1,s_empty_10022230);
    ReplaceSubstring(rules_text,s__S1_1002223c,1,s_empty_10022238);
    ReplaceSubstring(rules_text,s__S2_10022244,1,s_empty_10022240);
    ReplaceSubstring(rules_text,S__S3_1002224c,1,s_empty_10022248);
    ReplaceSubstring(rules_text,s__S4_10022254,1,s_empty_10022250);
    ReplaceSubstring(rules_text,s__S_1002225c,1,s_empty_10022258);
    SetTextColor(dc,DAT_1003a1a0);
    SetBkMode(dc,1);
    if (param_6 != 0) {
      SelectObject(dc,DAT_1003a000);
      local_508 = CalcDrawManaText(dc,&local_54,rules_text);
      local_508 = local_508 >> 0x10;
      GetTextMetricsA(dc,&local_554);
      CopyRect(&local_51c,&local_54);
      local_51c.top = local_51c.top + local_508 + local_554.tmHeight / 2;
      SelectObject(dc,DAT_1003a040);
      DrawTextA(dc,card->flavor_text,-1,&local_51c,0x410);
      local_50c = local_51c.bottom - local_54.top;
      if (local_54.bottom - local_54.top < local_50c) {
        local_560 = local_54.top - local_7c.top;
        local_54.top = local_54.bottom - local_50c;
        local_7c.top = local_54.top - local_560;
        if (local_68 == (HANDLE)0x0) {
          pHVar4 = GetStockObject(0);
          FillRect(dc,&local_7c,pHVar4);
        }
        else {
          GetObjectA(local_68,0x18,local_578);
          local_558 = 0x359;
          local_55c = 0x140;
          DrawBitmapSubrectToRect(dc,&local_7c,local_68,(local_574 * 0x49) / 1000,
                       (local_570 * 0x25d) / 1000,(local_574 * 0x359) / 1000,
                       (local_570 * 0x140) / 1000);
        }
      }
    }
    SelectObject(dc,DAT_1003a000);
    local_508 = DrawManaText(dc,&local_54,rules_text,1);
    local_508 = local_508 >> 0x10;
    GetTextMetricsA(dc,&local_554);
    CopyRect(&local_51c,&local_54);
    local_51c.top = local_51c.top + local_508 + local_554.tmHeight / 3;
    SelectObject(dc,DAT_1003a040);
    DrawTextA(dc,card->flavor_text,-1,&local_51c,0x10);
    RestoreDC(dc,nSavedDC);
  }
  return local_6c;
}

// FUNCTION: DRAWCARDLIB 0x100067c7
undefined4 * GetCardBackground(int param_1)

{
  int iVar1;
  undefined4 *local_8;
  
  if (*(int *)(param_1 + 0x10) == 1) {
    local_8 = &global_CARDBK_Black;
  }
  else if (*(int *)(param_1 + 0x10) == 8) {
    local_8 = &global_CARDBK_White;
  }
  else if (*(int *)(param_1 + 0x10) == 7) {
    local_8 = &global_CARDBK_Red;
  }
  else if (*(int *)(param_1 + 0x10) == 5) {
    local_8 = &global_CARDBK_Green;
  }
  else if (*(int *)(param_1 + 0x10) == 2) {
    local_8 = &global_CARDBK_Blue;
  }
  else if (*(int *)(param_1 + 0x10) == 4) {
    local_8 = &global_CARDBK_Gold;
  }
  else if (*(int *)(param_1 + 0x10) == 0) {
    local_8 = &global_CARDBK_Artifact;
  }
  else if (*(int *)(param_1 + 0x10) == 3) {
    local_8 = &global_CARDBK_Artifact;
  }
  else if (*(int *)(param_1 + 0x10) == 6) {
    if ((*(byte *)(param_1 + 0xc) & 2) == 0) {
      if ((*(byte *)(param_1 + 0xc) & 4) == 0) {
        if ((*(byte *)(param_1 + 0xc) & 0x20) == 0) {
          if ((*(byte *)(param_1 + 0xd) & 1) == 0) {
            if ((*(byte *)(param_1 + 0xc) & 8) == 0) {
              iVar1 = strcmp(*(char **)(param_1 + 4),&DAT_100f952c);
              if (iVar1 == 0) {
                local_8 = &global_CARDBK_BlackLand;
              }
              else {
                iVar1 = strcmp(*(char **)(param_1 + 4),&DAT_100f8140);
                if (iVar1 == 0) {
                  local_8 = &global_CARDBK_WhiteLand;
                }
                else {
                  iVar1 = strcmp(*(char **)(param_1 + 4),&DAT_100f77e0);
                  if (iVar1 == 0) {
                    local_8 = &global_CARDBK_RedLand;
                  }
                  else {
                    iVar1 = strcmp(*(char **)(param_1 + 4),&DAT_100f5b5c);
                    if (iVar1 == 0) {
                      local_8 = &global_CARDBK_GreenLand;
                    }
                    else {
                      iVar1 = strcmp(*(char **)(param_1 + 4),&DAT_100f6908);
                      if (iVar1 == 0) {
                        local_8 = &global_CARDBK_BlueLand;
                      }
                      else {
                        local_8 = &global_CARDBK_AntiquitiesLand;
                      }
                    }
                  }
                }
              }
            }
            else {
              local_8 = &global_CARDBK_AntiquitiesLand;
            }
          }
          else {
            local_8 = &global_CARDBK_LegendsLand;
          }
        }
        else {
          local_8 = &global_CARDBK_DarklandsLand;
        }
      }
      else {
        local_8 = &global_CARDBK_ArabianNightsLand;
      }
    }
    else {
      local_8 = &global_CARDBK_AntiquitiesLand;
    }
  }
  else if (*(int *)(param_1 + 0x10) == -1) {
    local_8 = &global_CARDBK_Special;
  }
  else {
    local_8 = &global_CARDBK_Artifact;
  }
  return local_8;
}

// FUNCTION: DRAWCARDLIB 0x10006a41
undefined4 LoadCardBackground(int *param_1)

{
  undefined4 uVar1;
  int iVar2;
  char local_170 [264];
  char local_68 [100];
  
  if (param_1 == (int *)0x0) {
    uVar1 = 0;
  }
  else if (*param_1 == 0) {
    if (param_1 == &global_CARDBK_Green) {
      strcpy(local_68,s_CARDBK_Green_10022260);
    }
    else if (param_1 == &global_CARDBK_White) {
      strcpy(local_68,s_CARDBK_White_10022270);
    }
    else if (param_1 == &global_CARDBK_Blue) {
      strcpy(local_68,s_CARDBK_Blue_10022280);
    }
    else if (param_1 == &global_CARDBK_Black) {
      strcpy(local_68,s_CARDBK_Black_1002228c);
    }
    else if (param_1 == &global_CARDBK_Red) {
      strcpy(local_68,s_CARDBK_Red_1002229c);
    }
    else if (param_1 == &global_CARDBK_Gold) {
      strcpy(local_68,s_CARDBK_Gold_100222a8);
    }
    else if (param_1 == &global_CARDBK_Artifact) {
      strcpy(local_68,s_CARDBK_Artifact_100222b4);
    }
    else if (param_1 == &global_CARDBK_GreenLand) {
      strcpy(local_68,s_CARDBK_GreenLand_100222c4);
    }
    else if (param_1 == &global_CARDBK_WhiteLand) {
      strcpy(local_68,s_CARDBK_WhiteLand_100222d8);
    }
    else if (param_1 == &global_CARDBK_BlueLand) {
      strcpy(local_68,s_CARDBK_BlueLand_100222ec);
    }
    else if (param_1 == &global_CARDBK_BlackLand) {
      strcpy(local_68,s_CARDBK_BlackLand_100222fc);
    }
    else if (param_1 == &global_CARDBK_RedLand) {
      strcpy(local_68,s_CARDBK_RedLand_10022310);
    }
    else if (param_1 == &global_CARDBK_DarklandsLand) {
      strcpy(local_68,s_CARDBK_DarklandsLand_10022320);
    }
    else if (param_1 == &global_CARDBK_FallenEmpiresLand) {
      strcpy(local_68,s_CARDBK_FallenEmpiresLand_10022338);
    }
    else if (param_1 == &global_CARDBK_AntiquitiesLand) {
      strcpy(local_68,s_CARDBK_AntiquitiesLand_10022354);
    }
    else if (param_1 == &global_CARDBK_LegendsLand) {
      strcpy(local_68,s_CARDBK_LegendsLand_1002236c);
    }
    else if (param_1 == &global_CARDBK_ArabianNightsLand) {
      strcpy(local_68,s_CARDBK_ArabianNightsLand_10022380);
    }
    else if (param_1 == &global_CARDBK_Special) {
      strcpy(local_68,s_CARDBK_Special_1002239c);
    }
    else {
      strcpy(local_68,&DAT_100223ac);
    }
    if (local_68[0] != '\0') {
      sprintf(local_170,s__s__s_pic_100223b0,&global_cardart_directory,local_68);
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
void DrawCardSet(int param_1,RECT* param_2,uint param_3)
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
      (((param_3 & 0x80) == 0 && (global_CardSets != (HANDLE)0x0)))) &&
     (((param_3 & 0x2e) != 0 || ((param_3 & 0x100) != 0)))) {
    GetObjectA(global_CardSets,0x18,local_44);
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
    DrawMaskedBitmapToRect(param_1,&local_1c,global_CardSets,local_24,local_2c,local_20,0,local_c,0);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x10006eed
void DrawCastingCost(HDC dc, RECT* param_2, casting_cost_t *castingCost)
{
  struct {
    int width;
    char castingCostString [20];
    int top;
    char symbol;
    int left;
    char *stringPtr;
    int height;
    int costReqs[6];
    int pad;
  }s;
  
  if (dc == 0 || param_2 == 0 || castingCost == NULL) {
  } else {
    s.costReqs[0] = (int)(char)castingCost->req_colorless;
    s.costReqs[1] = (int)(char)castingCost->req_black;
    s.costReqs[5] = (int)(char)castingCost->req_white;
    s.costReqs[3] = (int)(char)castingCost->req_green;
    s.costReqs[4] = (int)(char)castingCost->req_red;
    s.costReqs[2] = (int)(char)castingCost->req_blue;
    BuildCostString(s.costReqs, s.castingCostString);
    s.height = param_2->bottom - param_2->top;
    s.width = s.height;
    s.left = param_2->right - (strlen(s.castingCostString) >> 1) * s.width;
    s.top = param_2->top;
    s.stringPtr = s.castingCostString;
    while (*s.stringPtr != '\0') {
      s.symbol = GetNextManaSymbol(&s.stringPtr);
      DrawManaSymbol(dc,*(int*)&s.symbol,s.left,s.top,s.width,s.height);
      s.left += s.width;
    }
  }
}

// FUNCTION: DRAWCARDLIB 0x10006ff3
int BuildCostString(int *costReqs, char* costStringOut)
{
  int req_black;
  int req_blue;
  int length;
  char result [52];
  int req_colorless;
  int req_red;
  int req_white;
  int done;
  int req_green;
  
  if ((costReqs == (int *)0x0) || (costStringOut == 0)) {
    return 0;
  }

  req_white = costReqs[5];
  req_green = costReqs[3];
  req_red = costReqs[4];
  req_black = costReqs[1];
  req_blue = costReqs[2];
  req_colorless = costReqs[0];
  length = 0;
  if (req_colorless != 0) {
    if (req_colorless == -1) {
      result[0] = '|';
      result[1] = 'X';
      length = 2;
    }
    else if (req_colorless == 0x48) {
      result[0] = '|';
      result[1] = 'X';
      length = 2;
    }
    else if (req_colorless == 0x28) {
      result[0] = '|';
      result[1] = 'X';
      length = 2;
    }
    else if ((req_colorless < 1) || (9 < req_colorless)) {
      if (req_colorless == 10) {
        result[0] = '|';
        result[1] = '1';
        result[2] = 0x30;
        length = 3;
      }
    }
    else {
      result[0] = '|';
      result[1] = (char)req_colorless + '0';
      length = 2;
    }
  }
  done = 0;
  while (done == 0) {
    if (req_white == 0) {
      if (req_green == 0) {
        if (req_red == 0) {
          if (req_black == 0) {
            if (req_blue == 0) {
              result[length] = '\0';
              done = 1;
            }
            else {
              result[length] = '|';
              result[length + 1] = 'U';
              length = length + 2;
              req_blue = req_blue + -1;
            }
          }
          else {
            result[length] = '|';
            result[length + 1] = 'B';
            length = length + 2;
            req_black = req_black + -1;
          }
        }
        else {
          result[length] = '|';
          result[length + 1] = 'R';
          length = length + 2;
          req_red = req_red + -1;
        }
      }
      else {
        result[length] = '|';
        result[length + 1] = 'G';
        length = length + 2;
        req_green = req_green + -1;
      }
    }
    else {
      result[length] = '|';
      result[length + 1] = 'W';
      length = length + 2;
      req_white = req_white + -1;
    }
  }
  if ((((req_colorless == 0) && (length == 0)) && (req_white == 0)) && (((req_green == 0 && (req_red == 0)) && ((req_black == 0 && (req_blue == 0)))))) {
    // 0 cost
    strcpy(result,&s__0_100223bc);
  }
  if (costStringOut != 0) {
    strcpy(costStringOut,result);
  }
  return length;
}

// FUNCTION: DRAWCARDLIB 0x10007296
char GetNextManaSymbol(char *param_1)
{
  char *chr;
  char result;
  
  chr = (char *)*param_1;
  result = '\0';
  if (*chr == '|') {
    if (chr[1] == 'X') {
      result = -0x10;
    }
    else if ((chr[1] == '1') && (chr[2] == '0')) {
      result = -0x11;
      chr = chr + 1;
    }
    else if (chr[1] == '0') {
      result = -0xf;
    }
    else if (chr[1] == '1') {
      result = -0xe;
    }
    else if (chr[1] == '2') {
      result = -0xd;
    }
    else if (chr[1] == '3') {
      result = -0xc;
    }
    else if (chr[1] == '4') {
      result = -0xb;
    }
    else if (chr[1] == '5') {
      result = -10;
    }
    else if (chr[1] == '6') {
      result = -9;
    }
    else if (chr[1] == '7') {
      result = -8;
    }
    else if (chr[1] == '8') {
      result = -7;
    }
    else if (chr[1] == '9') {
      result = -6;
    }
    else if (chr[1] == 'T') {
      result = -0x12;
    }
    else if (chr[1] == 'B') {
      result = -2;
    }
    else if (chr[1] == 'U') {
      result = -3;
    }
    else if (chr[1] == 'W') {
      result = -5;
    }
    else if (chr[1] == 'G') {
      result = -1;
    }
    else if (chr[1] == 'R') {
      result = -4;
    }
  }
  if (result != '\0') {
    chr++;
    chr++;
  }
  *param_1 = (int)chr;
  return (int)result;
}

// FUNCTION: DRAWCARDLIB 0x100074b2
void DrawManaSymbol(HDC dc,char param_2,int left,int top,int width,int height)
{
  undefined1 local_3c [4];
  int local_38;
  int local_34;
  int local_24;
  int local_20;
  int local_1c;
  RECT local_18;
  int local_8;
  
  if (((dc != 0) && (-0x13 < param_2)) && (param_2 < '\0')) {
    GetObjectA(global_ManaSymbols,0x18,local_3c);
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
    SetRect(&local_18,left,top,width + left,height + top);
    DrawMaskedBitmapToRect(dc,&local_18,global_ManaSymbols,local_20,local_24,local_1c,0,local_8,0);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000775c
undefined4 CalcDrawManaText(HDC dc,RECT *rect,char* text)
{
  undefined4 uVar1;
  int nSavedDC;
  
                    /* 0x775c  1  CalcDrawManaText */
  if (((dc == (HDC)0x0) || (rect == 0)) || (text == 0)) {
    uVar1 = 0;
  }
  else {
    nSavedDC = SaveDC(dc);
    IntersectClipRect(dc,0,0,1,1);
    uVar1 = DrawManaText(dc,rect,text,0);
    RestoreDC(dc,nSavedDC);
  }
  return uVar1;
}

// FUNCTION: DRAWCARDLIB 0x100077dd
uint DrawManaText(HDC dc,RECT *param_2,char *text,int param_4)
{
  struct {
    int line_height;
    int symbol_step;
    char buf [52];
    int symbol_width;
    int font_height;
    int temp;
    int nSavedDC;
    int top;
    int idx;
    char symbol;
    char pad_55[3];
    uint max_width;
    int left;
    TEXTMETRICA text_metrics;
    int symbol_height;
    RECT rc;
    SIZE size;
    int height;
  } s;
  
                    /* 0x77dd  5  DrawManaText */
  if (((dc == (HDC)0x0) || (param_2 == (RECT *)0x0)) || (text == (char *)0x0)) {
    return 0;
  }
  if (*text == '\0') {
    return 0;
  }
  s.nSavedDC = SaveDC(dc);
  s.max_width = 0;
  s.height = 0;
  GetTextMetricsA(dc,&s.text_metrics);
  s.line_height = s.text_metrics.tmExternalLeading + s.text_metrics.tmHeight;
  SetRect(&s.rc,0,0,0,s.text_metrics.tmHeight);
  LPtoDP(dc,(LPPOINT)&s.rc,2);
  SetRect(&s.rc,0,0,s.rc.bottom - s.rc.top,0);
  DPtoLP(dc,(LPPOINT)&s.rc,2);
  s.symbol_width = ((s.rc.right - s.rc.left) * 0x4b) / 100;
  s.symbol_step = ((s.rc.right - s.rc.left) * 0x55) / 100;
  s.symbol_height = (s.text_metrics.tmHeight * 0x4b) / 100;
  s.font_height = s.text_metrics.tmHeight;
  IntersectClipRect(dc,param_2->left,param_2->top,param_2->right + 1,param_2->bottom);
  SelectObject(dc,GetStockObject(4));
  SelectObject(dc,GetStockObject(8));
  s.left = param_2->left;
  s.top = param_2->top;
  while (*text != '\0') {
    if (*text == ' ') {
      s.idx = 0;
      s.buf[s.idx] = *text;
      while (1) {
        text = text + 1;
        s.idx = s.idx + 1;
        if ((*text == '\0') || (*text != ' ')) break;
        s.buf[s.idx] = *text;
      }
      s.buf[s.idx] = '\0';
      GetTextExtentPoint32A(dc,s.buf,s.idx,&s.size);
      s.left = s.left + s.size.cx;
    }
    else if ((*text == '\0') || (*text != '\n')) {
      s.symbol = GetNextManaSymbol(&text);
      if (s.symbol == '\0') {
        s.idx = 0;
        s.buf[s.idx] = *text;
        while (1) {
          text = text + 1;
          s.idx = s.idx + 1;
          if (((*text == '\0') || (*text == ' ')) || ((*text == '\n' || (*text == '|')))) break;
          s.buf[s.idx] = *text;
        }
        s.buf[s.idx] = '\0';
        GetTextExtentPoint32A(dc,s.buf,s.idx,&s.size);
        if (param_2->right < s.size.cx + s.left) {
          s.temp = s.left - param_2->left;
          if (s.left - param_2->left <= (int)s.max_width) {
            s.temp = (int)s.max_width;
          }
          s.top = s.top + s.line_height;
          s.left = param_2->left;
          s.max_width = s.temp;
        }
        TextOutA(dc,s.left,s.top,s.buf,strlen(s.buf));
        s.left = s.left + s.size.cx;
      }
      else {
        s.idx = 0;
        s.buf[s.idx] = s.symbol;
        while (1) {
          s.idx = s.idx + 1;
          if (*text == '\0') break;
          s.symbol = GetNextManaSymbol(&text);
          if (s.symbol == '\0') break;
          s.buf[s.idx] = s.symbol;
        }
        s.buf[s.idx] = '\0';
        s.size.cx = s.idx * s.symbol_width;
        if (param_2->right < s.size.cx + s.left) {
          s.temp = s.left - param_2->left;
          if (s.left - param_2->left <= (int)s.max_width) {
            s.temp = (int)s.max_width;
          }
          s.top = s.top + s.line_height;
          s.left = param_2->left;
          s.max_width = s.temp;
        }
        s.temp = strlen(s.buf);
        for (s.idx = 0; s.idx < s.temp; s.idx = s.idx + 1) {
          if (param_4 == 0) {
            Ellipse(dc,s.left + (s.symbol_step - s.symbol_width) / 2,
                    s.top + (s.font_height - s.symbol_height) / 2,
                    s.symbol_width + (s.symbol_step - s.symbol_width) / 2 + s.left,
                    s.symbol_height + (s.font_height - s.symbol_height) / 2 + s.top);
          }
          else {
            DrawManaSymbol(dc,s.buf[s.idx],s.left + (s.symbol_step - s.symbol_width) / 2,
                         s.top + (s.font_height - s.symbol_height) / 2,s.symbol_width,s.symbol_height);
          }
          s.left = s.left + s.symbol_step;
        }
        if ((*text == ':') || (*text == ',')) {
          s.buf[0] = *text;
          text = text + 1;
          s.idx = 1;
          s.buf[1] = '\0';
          GetTextExtentPoint32A(dc,s.buf,1,&s.size);
          TextOutA(dc,s.left,s.top,s.buf,strlen(s.buf));
          s.left = s.left + s.size.cx;
        }
      }
    }
    else {
      text = text + 1;
      s.top = s.top + s.line_height + s.line_height / 3;
      s.left = param_2->left;
    }
  }
  s.temp = s.left - param_2->left;
  if (s.left - param_2->left <= (int)s.max_width) {
    s.temp = (int)s.max_width;
  }
  s.height = (s.line_height + s.top) - param_2->top;
  s.max_width = s.temp;
  RestoreDC(dc,s.nSavedDC);
  return (s.height << 0x10) | (s.max_width & 0xffff);
}

// FUNCTION: DRAWCARDLIB 0x10007dd7
void DrawSmallCard(HDC dc,RECT *rect,undefined4 *param_3,undefined4 param_4,int param_5)
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
  int nSavedDC;
  
                    /* 0x7dd7  6  DrawSmallCard */
  if (((dc != (HDC)0x0) && (rect != (int *)0x0)) && (param_3 != (undefined4 *)0x0)) {
    nSavedDC = SaveDC(dc);
    SetMapMode(dc,8);
    SetWindowExtEx(dc,200,0x118,(LPSIZE)0x0);
    SetViewportExtEx(dc,rect->right - rect->left,rect->bottom - rect->top,(LPSIZE)0x0);
    SetWindowOrgEx(dc,0,0,(LPPOINT)0x0);
    SetViewportOrgEx(dc,rect,rect->top,(LPPOINT)0x0);
    DrawCardBackground(dc,rect,param_3);
    DrawSmallCardTitle(dc,rect,param_3[2],0,1);
    h = GetStockObject(5);
    SelectObject(dc,h);
    SelectObject(dc,DAT_1003a01c);
    Rectangle(dc,0,0,200,0x118);
    SetMapMode(dc,1);
    local_18 = rect->right - rect->left;
    local_1c = rect->bottom - rect->top;
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
    local_10 = DrawSmallArt(dc,&local_34,*param_3,param_4);
    if (local_10 == 0) {
      DrawBigArt(dc,&local_34,*param_3,param_4);
    }
    RestoreDC(dc,nSavedDC);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x10008065
void DrawCardBackground(HDC param_1,RECT *param_2,int cardBack)
{
  HBRUSH hbr;
  undefined1 local_38 [4];
  undefined4 local_34;
  int local_30;
  int *local_20;
  RECT local_1c;
  undefined4 local_c;
  undefined4 local_8;
  
  if (((param_1 != (HDC)0x0) && (param_2 != (RECT *)0x0)) && (cardBack != 0)) {
    local_20 = (int *)GetCardBackground(cardBack);
    LoadCardBackground(local_20);
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
  int nSavedDC;
  
                    /* 0x81b2  7  DrawSmallCardTitle */
  if (((param_1 != (HDC)0x0) && (param_2 != (int *)0x0)) && (param_3 != 0)) {
    strcpy(local_6c,param_3);
    nSavedDC = SaveDC(param_1);
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
    RestoreDC(param_1,nSavedDC);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000aab9
undefined4
DrawMaskedBitmapToRect(HDC dc,RECT *rect,HANDLE param_3,int wSrc,int hSrc,int param_6,int param_7
            ,int param_8,int param_9)

{
  undefined4 uVar1;
  int hDest;
  undefined1 local_2c [24];
  int wDest;
  int nSavedDC;
  int local_c;
  int local_8;
  
  if (((dc == (HDC)0x0) || (rect == (int *)0x0)) || (param_3 == (HANDLE)0x0)) {
    uVar1 = 0;
  }
  else {
    EnterCriticalSection(&global_critical_section_for_drawing);
    nSavedDC = SaveDC(dc);
    SelectObject(global_screen_dc,param_3);
    GetObjectA(param_3,0x18,local_2c);
    local_8 = rect->left;
    local_c = rect->top;
    if (rect->right < rect->left) {
      wDest = wSrc;
    }
    else {
      wDest = rect->right - rect->left;
    }
    if (rect->bottom < rect->top) {
      hDest = hSrc;
    }
    else {
      hDest = rect->bottom - rect->top;
    }
    ApplyCardArtPaletteToDc(global_screen_dc);
    StretchBlt(dc,local_8,local_c,wDest,hDest,global_screen_dc,param_8,param_9,wSrc,
               hSrc,SRCAND);
    ApplyCardArtPaletteToDc(global_screen_dc);
    StretchBlt(dc,local_8,local_c,wDest,hDest,global_screen_dc,param_6,param_7,wSrc,
               hSrc,SRCPAINT);
    RestoreDC(dc,nSavedDC);
    LeaveCriticalSection(&global_critical_section_for_drawing);
    uVar1 = 1;
  }
  return uVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000ac34
void DeleteAndCloseObject(HANDLE param_1)
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
  
  if (global_dwPlatformId == -1) {
    local_f4.dwOSVersionInfoSize = 0x94;
    pDVar2 = &local_f4.dwMajorVersion;
    for (iVar1 = 0x24; iVar1 != 0; iVar1 = iVar1 + -1) {
      *pDVar2 = 0;
      pDVar2 = pDVar2 + 1;
    }
    GetVersionExA(&local_f4);
    if (local_f4.dwPlatformId == 1) {
      //Windows 95/98?
      global_dwPlatformId = 1;
    }
    else {
      global_dwPlatformId = 0;
    }
  }
  if (param_1 != (HANDLE)0x0) {
    GetObjectA(param_1,0x54,local_5c);
    local_60 = local_10;
    local_8 = local_48 + local_c;
    DeleteObject(param_1);
    if ((local_60 != (HANDLE)0x0) && (global_dwPlatformId != 0)) {
      CloseHandle(local_60);
    }
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000b00c
COLORREF GetPaletteColor(int index)
{
  struct {
    int g;
    int r;
    int b;
  } rgb;

  rgb.r = g_cardArtPalette[index].rgbRed;
  rgb.g = g_cardArtPalette[index].rgbGreen;
  rgb.b = g_cardArtPalette[index].rgbBlue;

  return ((byte)rgb.g << 8) | (byte)rgb.r | ((byte)rgb.b << 16) | 0x02000000;
}

// FUNCTION: DRAWCARDLIB 0x1000b06a
void ReplaceSubstring(char *inOutStr,char *needle,int caseSensitive,char *replacement)
{
  // TODO: original looks like __asm to me
  int outLen;
  char outStr [1000];
  size_t needleLen;
  int matched;
  char *cur;

  if (((inOutStr != (char *)0x0) && (needle != (char *)0x0)) && (replacement != (char *)0x0)) {
    if (strlen(inOutStr) != 0) {
      if (strlen(needle) != 0) {
        needleLen = strlen(needle);
        cur = inOutStr;
        outStr[0] = '\0';
        outLen = 0;
        while (*cur != '\0') {
          matched = 0;
          if (caseSensitive != 0) {
            if (strncmp(cur,needle,needleLen) == 0) {
              matched = 1;
            }
          }
          else {
            if (_strnicmp(cur,needle,needleLen) == 0) {
              matched = 1;
            }
          }
          if (matched == 0) {
            outStr[outLen] = *cur;
            cur++;
            outLen++;
            outStr[outLen] = '\0';
          }
          else {
            strcat(outStr,replacement);
            outLen += strlen(replacement);
            cur += needleLen;
          }
        }
        strcpy(inOutStr,outStr);
      }
    }
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
    DestroyAllResources();
    DrawCardLib_Shutdown();
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
  char cards_dat_filename [264];
  size_t global_available_slots;
  size_t record_size;
  int iStackY_4a390;
  FILE *cards_dat;
  undefined1 auStackY_4a388 [4];
  int aiStackY_4a384 [75993];
  int iVar2;
  
  iVar2 = 1;
  _DAT_10124510 = param_1;
  InitializeCriticalSection(&global_critical_section_for_catalog);
  GetModuleFileNameA((HMODULE)0x0,&global_base_directory,0x105);
  pcVar1 = strrchr(&global_base_directory,0x5c);
  *pcVar1 = '\0';
  strcpy((uint *)&global_cardart_directory,(uint *)&global_base_directory);
  strcat((uint *)&global_cardart_directory,(uint *)s__CARDART_10026a48);
  DAT_10021048 = 3;
  DAT_10124538 = 0x10;
  InitCardArtGdiResources();
  strcpy(cards_dat_filename,(uint *)&global_base_directory);
  strcat(cards_dat_filename,(uint *)s__CARDS_DAT_10026a54);
  cards_dat = fopen(cards_dat_filename,s_rb_10026a60);
  if (cards_dat == (FILE *)0x0) {
    iVar2 = 0;
  }
  else {
    fread(&global_available_slots,4,1,cards_dat);
    fread(&record_size,4,1,cards_dat);
    fread(auStackY_4a388,0x98,global_available_slots,cards_dat);
    _DstBuf = malloc(record_size);
    if (_DstBuf == (void *)0x0) {
      iVar2 = 0;
    }
    else {
      fread(_DstBuf,1,record_size,cards_dat);
      for (iStackY_4a390 = 0; iStackY_4a390 < (int)global_available_slots; iStackY_4a390 = iStackY_4a390 + 1)
      {
        aiStackY_4a384[iStackY_4a390 * 0x26] = aiStackY_4a384[iStackY_4a390 * 0x26] + (int)_DstBuf;
        strncpy(DAT_100f37d0 + iStackY_4a390 * 100,(char *)aiStackY_4a384[iStackY_4a390 * 0x26],
                 100);
        DAT_100f3833[iStackY_4a390 * 100] = 0;
      }
      free(_DstBuf);
    }
    fclose(cards_dat);
  }
  if (iVar2 == 0) {
    for (iStackY_4a390 = 0; iStackY_4a390 < (int)global_available_slots; iStackY_4a390 = iStackY_4a390 + 1) {
      strcpy((uint *)(&DAT_100f37d0 + iStackY_4a390 * 100),(uint *)&s_File__s_could_not_be_opened__EXITING_10026a64);
    }
  }
  return iVar2;
}

// FUNCTION: DRAWCARDLIB 0x1000c0ca
void DrawCardLib_Shutdown(void)
{
  ShutdownCardArtGdiResources();
  DeleteCriticalSection(&global_critical_section_for_catalog);
}
