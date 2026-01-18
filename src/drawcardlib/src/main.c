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

typedef void * pointer;
typedef char byte;
typedef unsigned short ushort;
typedef BOOL (WINAPI code)(HINSTANCE,DWORD,LPVOID);

#define CONCAT12(x,y) ((undefined4)(((undefined1)(x)) | ((undefined4)(undefined2)(y) << 8)))

// FUNCTION: DRAWCARDLIB 0x10155000
void __fastcall FUN_10155000(undefined4 param_1,undefined4 param_2,ushort *param_3) {
  //TODO: this needs to be decompiled into a separate static library
}

// FUNCTION: DRAWCARDLIB 0x10155484
void FUN_10155484() {
  //TODO: this needs to be decompiled into a separate static library
}

/* Globals referenced before their definitions (provide placeholders for the build). */
undefined4 DAT_10021050[4];
undefined4 DAT_10021078[4];
undefined4 DAT_100210a0[4];
undefined1 DAT_100210c8[0x8000];
undefined1 DAT_100210d4[0x8000];
undefined1 DAT_10021788[0x1000];
undefined1 DAT_10021794[0x1000];
undefined1 DAT_10128e40[5 * 0x8060];
undefined4 DAT_10152430[0x41];

/* Placeholder globals for missing decompiler labels. */
char DAT_10021e54[] = "rt";
char DAT_10021e68[] = "rb";
char DAT_10021e6c[] = " \n";
char DAT_10021e70[] = " \n";
char DAT_10021e74[] = " \n";
char DAT_10021e78[] = " \n";
char DAT_10021e7c[] = " \n";
char DAT_10021f18[] = "rb";
char DAT_10021f60[] = "rb";
char DAT_10021fd8[] = "w+b";
undefined1 DAT_10028f10 = 0;
undefined1 DAT_10028f11 = 0;
undefined4 _DAT_10028f08 = 0;
undefined4 _DAT_10029fb4 = 0;
undefined4 _DAT_10029fc0 = 0;
undefined4 DAT_10029fc8 = 0;
undefined4 DAT_10029fcc = 0;
undefined4 DAT_10029328 = 0;
undefined4 DAT_100293b0 = 0;
undefined4 DAT_100293b4 = 0;
undefined4 DAT_100293b8 = 0;
undefined1 DAT_10021fe0[0x3c];
unsigned char DAT_1003a048[0x3c];
undefined4 _DAT_1003a048 = 0;
undefined4 _DAT_1003a058 = 0;
undefined4 DAT_1003a05c = 0;
char DAT_1003a064[0x20];
char DAT_1003a090[260];
unsigned char DAT_1003a9b8[4];
undefined4 DAT_1003ab68 = 0;
char DAT_100b5368[4];
char DAT_100f35b0[0x105];
char DAT_100f36c0[0x105];
undefined4 DAT_100f5b5c = 0;
undefined4 DAT_100f6908 = 0;
undefined4 DAT_100f77e0 = 0;
undefined4 DAT_100f8140 = 0;
undefined4 DAT_100f952c = 0;
undefined4 DAT_10126554 = 0;
undefined4 _DAT_10126554 = 0;
undefined4 DAT_10126d30 = 0;
undefined4 DAT_10127d42 = 0;
undefined4 DAT_10127d80 = 0;
undefined4 DAT_10127dc0 = 0;
undefined4 _DAT_10127d4c = 0;
undefined4 _DAT_10127d4e = 0;
undefined4 _DAT_10127d84 = 0;
undefined4 _DAT_10127d86 = 0;
undefined4 _DAT_10127d88 = 0;
undefined4 DAT_10127e40 = 0;
undefined4 DAT_10127e44 = 0;
undefined4 DAT_10128640 = 0;
undefined4 DAT_10151020 = 0;
undefined4 DAT_10151024 = 0;
undefined4 DAT_10151820 = 0;
undefined4 DAT_10151824 = 0;
undefined4 DAT_100223ac = 0;
undefined4 DAT_100223bc = 0;
undefined4 DAT_10022428 = 0;

/* Function prototypes (auto-generated). */
#include "autoproto.h"

// GLOBAL: DRAWCARDLIB 0x1000be0b
pointer switchdataD_1000be0b = 0x1000BDDE;

// GLOBAL: DRAWCARDLIB 0x10021030
undefined4 DAT_10021030 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10021034
undefined1 *PTR_DAT_10021034 = (undefined1 *)0x10128A3C;

// GLOBAL: DRAWCARDLIB 0x10021038
undefined4 DAT_10021038 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1002103c
undefined1 *PTR_DAT_1002103c = (undefined1 *)0x10151820;

// GLOBAL: DRAWCARDLIB 0x10021040
undefined1 *PTR_DAT_10021040 = (undefined1 *)0x10151020;

// GLOBAL: DRAWCARDLIB 0x10021044
undefined1 *PTR_DAT_10021044 = (undefined1 *)0x10127E40;

// GLOBAL: DRAWCARDLIB 0x10021048
undefined4 DAT_10021048 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1002104c
undefined4 DAT_1002104c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10021e48
undefined1 *PTR_DAT_10021e48 = (undefined1 *)0x10028CF0;

// GLOBAL: DRAWCARDLIB 0x10021e4c
undefined4 DAT_10021e4c = 0xFFFFFFFF;

// GLOBAL: DRAWCARDLIB 0x10021e50
undefined4 DAT_10021e50 = 0xFFFFFFFF;

// GLOBAL: DRAWCARDLIB 0x10021e58
char s__d____d__d__d_10021e58[] = "%d - %d %d %d";

// GLOBAL: DRAWCARDLIB 0x10021e80
char s_Not_enough_memory_for_delta_arra_10021e80[] = "Not enough memory for delta array\r\n";

// GLOBAL: DRAWCARDLIB 0x10021ea4
char s_D__Newmagic_sources_NedCard_Pale_10021ea4[] = "D:\\Newmagic\\sources\\NedCard\\Palette.c";

// GLOBAL: DRAWCARDLIB 0x10021ecc
undefined4 DAT_10021ecc = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10021ed0
pointer PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0 = (pointer)0x10021EDC;

// GLOBAL: DRAWCARDLIB 0x10021ed4
undefined4 gPcxBufferWidth = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10021ed8
undefined4 gPcxBufferHeight = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10021edc
char s_D__Newmagic_sources_sidlib_Pcxw__10021edc[] = "D:\\Newmagic\\sources\\sidlib\\Pcxw.c";

// GLOBAL: DRAWCARDLIB 0x10021f00
char s_Error_Opening_File__s_10021f00[] = "Error Opening File %s\r\n";

// GLOBAL: DRAWCARDLIB 0x10021f1c
char s__s_Not_a_256_color_palettized_pc_10021f1c[] = "%s Not a 256 color palettized pcx file\r\n";

// GLOBAL: DRAWCARDLIB 0x10021f48
char s_Error_Opening_File__s_10021f48[] = "Error Opening File %s\r\n";

// GLOBAL: DRAWCARDLIB 0x10021f64
char s__s_Not_a_pcx_file_10021f64[] = "%s Not a pcx file\r\n";

// GLOBAL: DRAWCARDLIB 0x10021f78
char s__s_Not_a_version_5_pcx_file_10021f78[] = "%s Not a version 5 pcx file\r\n";

// GLOBAL: DRAWCARDLIB 0x10021f98
char s__s_is_not_in_a_recognizable_form_10021f98[] = "%s is not in a recognizable format\r\n";

// GLOBAL: DRAWCARDLIB 0x10021fc0
char s_Error_Opening_File__s_10021fc0[] = "Error Opening File %s\r\n";

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

// GLOBAL: DRAWCARDLIB 0x100223c0
undefined4 DAT_100223c0 = 0x00000004;

// GLOBAL: DRAWCARDLIB 0x100223f8
undefined1 *PTR_DAT_100223f8 = (undefined1 *)0x1003A5A8;

// GLOBAL: DRAWCARDLIB 0x100223fc
undefined4 DAT_100223fc = 0x00000001;

// GLOBAL: DRAWCARDLIB 0x10022400
undefined4 DAT_10022400 = 0x00000002;

// GLOBAL: DRAWCARDLIB 0x10022404
undefined4 DAT_10022404 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10022408
undefined1 *PTR_DAT_10022408 = (undefined1 *)0x10124950;

// GLOBAL: DRAWCARDLIB 0x1002240c
char s_SmallArt_cat_1002240c[] = "SmallArt.cat";

// GLOBAL: DRAWCARDLIB 0x1002241c
char s_MedArt_cat_1002241c[] = "MedArt.cat";

// GLOBAL: DRAWCARDLIB 0x1002242c
char s_wavelet_pieces_has_illegal_value_1002242c[] = "wavelet pieces has illegal value: %d";

// GLOBAL: DRAWCARDLIB 0x10022454
char s_D__Newmagic_sources_NedCard_haar_10022454[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: DRAWCARDLIB 0x10022478
char s_Only_Works_on_24_bit_images_10022478[] = "Only Works on 24 bit images\n";

// GLOBAL: DRAWCARDLIB 0x10022498
char s_D__Newmagic_sources_NedCard_haar_10022498[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: DRAWCARDLIB 0x100224bc
char s_Only_Works_on_24_bit_images_100224bc[] = "Only Works on 24 bit images\n";

// GLOBAL: DRAWCARDLIB 0x100224dc
char s_D__Newmagic_sources_NedCard_haar_100224dc[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: DRAWCARDLIB 0x10022500
undefined4 DAT_10022500 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10022504
undefined4 DAT_10022504 = 0xFFFFFFFF;

// GLOBAL: DRAWCARDLIB 0x10022508
char s__DUELPALall_TR_10022508[] = "\\DUELPALall.TR";

// GLOBAL: DRAWCARDLIB 0x10022518
char s__DUEL_plogpal_10022518[] = "\\DUEL.plogpal";

// GLOBAL: DRAWCARDLIB 0x10022528
undefined1 *PTR_DAT_10022528 = (undefined1 *)0x100F23B0;

// GLOBAL: DRAWCARDLIB 0x1002252c
ushort *PTR_DAT_1002252c = (ushort *)0x100F35A0;

// GLOBAL: DRAWCARDLIB 0x10022530
undefined4 DAT_10022530 = 0xFFFFFFFF;

// GLOBAL: DRAWCARDLIB 0x10022534
undefined4 DAT_10022534 = 0x69627072;

// GLOBAL: DRAWCARDLIB 0x10022538
undefined2 DAT_10022538 = 0x7374;

// GLOBAL: DRAWCARDLIB 0x1002253a
undefined1 DAT_1002253a = 0x00;

// GLOBAL: DRAWCARDLIB 0x1002253c
char DAT_1002253c[] = ".pcx";

// GLOBAL: DRAWCARDLIB 0x10022544
char DAT_10022544[] = "rb";

// GLOBAL: DRAWCARDLIB 0x10022548
undefined2 DAT_10022548 = 0x0000;

// GLOBAL: DRAWCARDLIB 0x1002254a
undefined1 DAT_1002254a = 0x00;

// GLOBAL: DRAWCARDLIB 0x1002254c
undefined2 DAT_1002254c = 0x0000;

// GLOBAL: DRAWCARDLIB 0x10026548
undefined4 gImageWidth = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1002654c
undefined4 gImageHeight = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10026554
undefined4 DAT_10026554 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1002655c
undefined1 DAT_1002655c = 0x00;

// GLOBAL: DRAWCARDLIB 0x1002655d
undefined1 DAT_1002655d = 0x00;

// GLOBAL: DRAWCARDLIB 0x1002655e
undefined1 DAT_1002655e = 0x00;

// GLOBAL: DRAWCARDLIB 0x1002655f
undefined1 DAT_1002655f = 0x00;

// GLOBAL: DRAWCARDLIB 0x10026560
undefined4 DAT_10026560 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10026564
undefined4 DAT_10026564 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10026568
undefined4 DAT_10026568 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x1002656c
undefined1 DAT_1002656c = 0x00;

// GLOBAL: DRAWCARDLIB 0x1002656d
undefined1 DAT_1002656d = 0x00;

// GLOBAL: DRAWCARDLIB 0x1002656e
undefined4 DAT_1002656e = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10026572
undefined1 DAT_10026572 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10026973
undefined1 DAT_10026973 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10026974
undefined4 DAT_10026974 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10026978
char s_Too_many_open_Catalogs__Max__d_10026978[] = "Too many open Catalogs: Max %d\n";

// GLOBAL: DRAWCARDLIB 0x10026998
char s_D__Newmagic_sources_NedCard_Cata_10026998[] = "D:\\Newmagic\\sources\\NedCard\\Catalog.c";

// GLOBAL: DRAWCARDLIB 0x100269c0
char DAT_100269c0[] = "rb";

// GLOBAL: DRAWCARDLIB 0x100269c4
char s_Duplicate_short_name_found_in_ca_100269c4[] = "Duplicate short name found in catalogs\n%s entry %d and\n%s entry %d\nShortName value 0x%08lx";

// GLOBAL: DRAWCARDLIB 0x10026a20
char s_D__Newmagic_sources_NedCard_Cata_10026a20[] = "D:\\Newmagic\\sources\\NedCard\\Catalog.c";

// GLOBAL: DRAWCARDLIB 0x10026a48
char s__CARDART_10026a48[] = "\\CARDART";

// GLOBAL: DRAWCARDLIB 0x10026a54
char s__CARDS_DAT_10026a54[] = "\\CARDS.DAT";

// GLOBAL: DRAWCARDLIB 0x10026a60
char DAT_10026a60[] = "rb";

// GLOBAL: DRAWCARDLIB 0x10026a64
char DAT_10026a64[] = "File %s could not be opened: EXITING\r\n";

// GLOBAL: DRAWCARDLIB 0x10026a94
char s__assertFile_txt_10026a94[] = "\\assertFile.txt";

// GLOBAL: DRAWCARDLIB 0x10026aa4
char DAT_10026aa4[] = "at";

// GLOBAL: DRAWCARDLIB 0x10026aa8
char s_File____s__Line____d_10026aa8[] = "File-> %s, Line-> %d\n";

// GLOBAL: DRAWCARDLIB 0x10026ac0
char s__s_s_10026ac0[] = "%s%s\n";

// GLOBAL: DRAWCARDLIB 0x10026ac8
char s_Assertion_Error_10026ac8[] = "Assertion Error";

// GLOBAL: DRAWCARDLIB 0x10026ad8
char s__assertFile_txt_10026ad8[] = "\\assertFile.txt";

// GLOBAL: DRAWCARDLIB 0x10026aec
char s_File____s__Line____d_10026aec[] = "File-> %s, Line-> %d\n";

// GLOBAL: DRAWCARDLIB 0x10026b04
char s__s_s_10026b04[] = "%s%s\n";

// GLOBAL: DRAWCARDLIB 0x10026b0c
char s_Assertion_Error_10026b0c[] = "Assertion Error";

// GLOBAL: DRAWCARDLIB 0x10026b98
undefined4 DAT_10026b98 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10028af0
undefined1 DAT_10028af0 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10028af1
undefined1 DAT_10028af1 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10028ef4
undefined4 DAT_10028ef4 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10028ef8
undefined4 DAT_10028ef8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10028f00
undefined1 DAT_10028f00 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10028f01
undefined1 DAT_10028f01 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10029310
undefined1 DAT_10029310 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10029311
undefined1 DAT_10029311 = 0x00;

// GLOBAL: DRAWCARDLIB 0x1002931c
undefined4 DAT_1002931c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10029320
undefined4 DAT_10029320 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10029324
undefined4 DAT_10029324 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100293ac
undefined4 DAT_100293ac = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10029fb0
uint *DAT_10029fb0 = (uint *)0x0;

// GLOBAL: DRAWCARDLIB 0x10029fb8
uint *DAT_10029fb8 = (uint *)0x0;

// GLOBAL: DRAWCARDLIB 0x10029fbc
undefined4 DAT_10029fbc = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10029fc4
undefined4 DAT_10029fc4 = 0x00000000;

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

// GLOBAL: DRAWCARDLIB 0x100f1f70
undefined4 DAT_100f1f70 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f1f74
undefined4 DAT_100f1f74 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f1f78
undefined1 DAT_100f1f78 = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f1f79
undefined1 DAT_100f1f79 = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f1f7a
undefined1 DAT_100f1f7a = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f1f7b
undefined1 DAT_100f1f7b = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f2378
CRITICAL_SECTION DAT_100f2378;

// GLOBAL: DRAWCARDLIB 0x100f2394
undefined4 DAT_100f2394 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f239c
undefined4 DAT_100f239c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f23a0
undefined4 DAT_100f23a0 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f23a4
undefined4 DAT_100f23a4 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f23a8
undefined4 _DAT_100f23a8 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x100f2e30
undefined1 DAT_100f2e30 = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f2e34
undefined1 DAT_100f2e34 = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f2e38
undefined1 DAT_100f2e38 = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f2e3c
undefined1 DAT_100f2e3c = 0x00;

// GLOBAL: DRAWCARDLIB 0x100f33a0
undefined1 DAT_100f33a0[0x200];

// GLOBAL: DRAWCARDLIB 0x100f23b0
undefined4 DAT_100f23b0 = 0x00000000;

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

// GLOBAL: DRAWCARDLIB 0x100f23e0
char DAT_100f23e0[2048];

// GLOBAL: DRAWCARDLIB 0x100f3394
ushort *DAT_100f3394 = (ushort *)0x0;

// GLOBAL: DRAWCARDLIB 0x100f35a0
void (__cdecl *DAT_100f35a0)() = (void (__cdecl *)())0x0;

// GLOBAL: DRAWCARDLIB 0x100f37d0
char DAT_100f37d0[100 * 1024];

// GLOBAL: DRAWCARDLIB 0x100f3833
char DAT_100f3833[100 * 1024];

// GLOBAL: DRAWCARDLIB 0x10124510
undefined4 _DAT_10124510 = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10124520
CRITICAL_SECTION DAT_10124520;

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

// GLOBAL: DRAWCARDLIB 0x10127d30
FILE *gPcxInFile = (FILE *)0x0;

// GLOBAL: DRAWCARDLIB 0x10127d34
FILE *gPcxOutFile = (FILE *)0x0;

// GLOBAL: DRAWCARDLIB 0x10127d38
char *gPcxPath = (char *)0x0;

// GLOBAL: DRAWCARDLIB 0x10127d40
undefined1 DAT_10127d40 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10127d41
undefined1 DAT_10127d41 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10127d43
undefined1 DAT_10127d43 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10127d44
undefined2 DAT_10127d44 = 0x0000;

// GLOBAL: DRAWCARDLIB 0x10127d46
undefined2 DAT_10127d46 = 0x0000;

// GLOBAL: DRAWCARDLIB 0x10127d48
undefined2 DAT_10127d48 = 0x0000;

// GLOBAL: DRAWCARDLIB 0x10127d4a
undefined2 DAT_10127d4a = 0x0000;

// GLOBAL: DRAWCARDLIB 0x10127d81
undefined1 DAT_10127d81 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10127d82
undefined2 gPcxBytesPerLine = 0x0000;

// GLOBAL: DRAWCARDLIB 0x10128a3c
undefined4 DAT_10128a3c = 0x00000000;

// GLOBAL: DRAWCARDLIB 0x10152020
undefined2 DAT_10152020 = 0x0000;

// GLOBAL: DRAWCARDLIB 0x10152022
undefined2 DAT_10152022 = 0x0000;

// GLOBAL: DRAWCARDLIB 0x10152027
undefined1 DAT_10152027 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10152423
undefined1 DAT_10152423 = 0x00;

// GLOBAL: DRAWCARDLIB 0x10153660
code *DAT_10153660 = (code *)0x0;

// MATCHING
// FUNCTION: DRAWCARDLIB 0x10001000
void * FUN_10001000(void)
{
  void *_Dst;
  
  _Dst = malloc(0x30);
  memset(_Dst,0,0x30);
  return _Dst;
}

// FUNCTION: DRAWCARDLIB 0x10001033
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 * FUN_10001033(char *param_1,char *param_2)

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
  local_8 = &DAT_10152020;
  DAT_10152020 = 0x300;
  local_18 = fopen(param_1,&DAT_10021e54);
  if (local_18 == (FILE *)0x0) {
    local_8 = (undefined2 *)0x0;
  }
  else {
    if (DAT_10028ef4 != 0) {
      FUN_10001591(DAT_10028ef4);
    }
    DAT_10028ef4 = FUN_10001000();
    fgets(local_120,0xff,local_18);
    while ((local_18->_flag & 0x10) == 0) {
      sscanf(local_120,s__d____d__d__d_10021e58,&local_20,&local_10,&local_1c,&local_c);
      local_14 = strchr(local_120,0x2d);
      local_14 = local_14 + 1;
      local_14 = strchr(local_14,0x2d);
      local_14 = local_14 + 1;
      FUN_100014b7(DAT_10028ef4,local_14,local_20);
      *(uint *)(&DAT_10028f10 + local_20 * 4) = local_10 << 0x10 | local_1c << 8 | local_c;
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
    DAT_10029324 = 0;
    DAT_10021030 = 0;
    _DAT_10028f08 = FUN_10001378(DAT_10028ef4);
    DAT_10021030 = DAT_10021030 + -1;
    local_8[1] = 0x100;
    fclose(local_18);
    local_18 = (FILE *)0x0;
    if (param_2 != (char *)0x0) {
      local_18 = fopen(param_2,&DAT_10021e68);
    }
    if (local_18 != (FILE *)0x0) {
      fread(&DAT_10152020,0x404,1,local_18);
      fclose(local_18);
    }
    *(undefined1 *)((int)local_8 + 0x403) = 0;
    *(undefined1 *)((int)local_8 + 7) = *(undefined1 *)((int)local_8 + 0x403);
    FUN_100016a1();
    FUN_10001287();
  }
  return local_8;
}

// FUNCTION: DRAWCARDLIB 0x10001287
undefined4 FUN_10001287(void)

{
  undefined4 uVar1;
  int local_c;
  int local_8;
  
  if (DAT_10021038 == 0) {
    local_c = -0xff;
    for (local_8 = 0; local_8 < 0x200; local_8 = local_8 + 1) {
      *(int *)(&DAT_10128640 + local_8 * 4) = local_c * local_c;
      local_c = local_c + 1;
    }
    DAT_10021038 = 1;
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

// FUNCTION: DRAWCARDLIB 0x100012f9
void FUN_100012f9(int *param_1,int param_2,int *param_3)

{
  int local_8;
  
  if (*param_1 == 0) {
    for (local_8 = 0; local_8 < 8; local_8 = local_8 + 1) {
      if (param_1[local_8 + 2] != 0) {
        FUN_100012f9(param_1[local_8 + 2],param_2,param_3);
      }
    }
  }
  else {
    *(char *)(*param_3 + param_2) = (char)param_1[1];
    *param_3 = *param_3 + 1;
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x10001378
int FUN_10001378(int *param_1)

{
  int iVar1;
  void *pvVar2;
  size_t local_410;
  int local_40c;
  int local_408;
  undefined1 local_404 [1024];
  
  iVar1 = DAT_10029324;
  local_408 = 0;
  local_410 = 0;
  DAT_10029324 = DAT_10029324 + 1;
  if (DAT_10021030 < DAT_10029324) {
    DAT_10021030 = DAT_10029324;
  }
  if (*param_1 == 0) {
    for (local_40c = 0; local_40c < 8; local_40c = local_40c + 1) {
      if (param_1[local_40c + 2] != 0) {
        iVar1 = FUN_10001378(param_1[local_40c + 2]);
        local_408 = local_408 + iVar1;
        local_410 = local_410 + 1;
      }
    }
    if (local_410 != 0) {
      local_410 = 0;
      FUN_100012f9(param_1,local_404,&local_410);
      pvVar2 = malloc(local_410);
      param_1[10] = (int)pvVar2;
      param_1[0xb] = local_410;
      memcpy((void *)param_1[10],local_404,local_410);
    }
    DAT_10029324 = DAT_10029324 + -1;
  }
  else {
    local_408 = 1;
    DAT_10029324 = iVar1;
  }
  return local_408;
}

// FUNCTION: DRAWCARDLIB 0x100014b7
undefined4 FUN_100014b7(undefined4 *param_1,char *param_2,undefined4 param_3)

{
  size_t sVar1;
  int iVar2;
  undefined4 uVar3;
  size_t sVar4;
  size_t sVar5;
  char *_Control;
  
  sVar1 = strspn(param_2,&DAT_10021e6c);
  for (param_2 = param_2 + sVar1; *param_2 != '\0'; param_2 = param_2 + sVar1 + sVar4 + sVar5) {
    iVar2 = atoi(param_2);
    if (param_1[iVar2 + 2] == 0) {
      uVar3 = FUN_10001000();
      param_1[iVar2 + 2] = uVar3;
    }
    param_1 = (undefined4 *)param_1[iVar2 + 2];
    _Control = &DAT_10021e74;
    sVar1 = strspn(param_2,&DAT_10021e78);
    sVar1 = strcspn(param_2 + sVar1,_Control);
    sVar4 = strspn(param_2,&DAT_10021e70);
    sVar5 = strspn(param_2 + sVar1 + sVar4,&DAT_10021e7c);
  }
  *param_1 = 1;
  param_1[1] = param_3;
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x10001591
int FUN_10001591(int *param_1)

{
  int iVar1;
  int local_c;
  int local_8;
  
  local_8 = 0;
  if (*param_1 == 0) {
    for (local_c = 0; local_c < 8; local_c = local_c + 1) {
      if (param_1[local_c + 2] != 0) {
        iVar1 = FUN_10001591(param_1[local_c + 2]);
        local_8 = local_8 + iVar1;
      }
    }
    if (param_1[10] != 0) {
      FUN_1000cde0(param_1[10]);
    }
    FUN_1000cde0(param_1);
  }
  else {
    FUN_1000cde0(param_1);
    local_8 = 1;
  }
  return local_8;
}

// FUNCTION: DRAWCARDLIB 0x1000163d
void FUN_1000163d(uint param_1,uint *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  iVar1 = ((param_1 & 0xff0000) >> 0x10) * 8;
  uVar4 = *(uint *)(&DAT_10127e44 + iVar1);
  iVar2 = (param_1 >> 8 & 0xff) * 8;
  uVar5 = *(uint *)(&DAT_10151024 + iVar2);
  iVar3 = (param_1 & 0xff) * 8;
  uVar6 = *(uint *)(&DAT_10151824 + iVar3);
  *param_2 = *(uint *)(&DAT_10127e40 + iVar1) | *(uint *)(&DAT_10151020 + iVar2) |
             *(uint *)(&DAT_10151820 + iVar3);
  param_2[1] = uVar4 | uVar5 | uVar6;
  return;
}

// FUNCTION: DRAWCARDLIB 0x100016a1
undefined4 FUN_100016a1(void)

{
  uint local_10;
  int local_c;
  uint local_8;
  
  for (local_8 = 0; (int)local_8 < 0x100; local_8 = local_8 + 1) {
    local_10 = 0x80;
    for (local_c = 0; local_c < 8; local_c = local_c + 1) {
      if ((local_8 & local_10) == 0) {
        PTR_DAT_1002103c[local_8 * 8 + local_c] = 0;
      }
      else {
        PTR_DAT_1002103c[local_8 * 8 + local_c] = 4;
      }
      if ((local_8 & local_10) == 0) {
        PTR_DAT_10021040[local_8 * 8 + local_c] = 0;
      }
      else {
        PTR_DAT_10021040[local_8 * 8 + local_c] = 2;
      }
      if ((local_8 & local_10) == 0) {
        PTR_DAT_10021044[local_8 * 8 + local_c] = 0;
      }
      else {
        PTR_DAT_10021044[local_8 * 8 + local_c] = 1;
      }
      local_10 = (int)local_10 >> 1;
    }
  }
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x100017a5
undefined4 FUN_100017a5(uint param_1)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  byte *pbVar5;
  int local_34;
  uint local_2c;
  int local_28;
  
  FUN_1000163d(param_1,&DAT_10029310);
  pbVar5 = &DAT_10029310;
  piVar3 = DAT_10028ef4;
  do {
    piVar1 = (int *)piVar3[*pbVar5 + 2];
    pbVar5 = pbVar5 + 1;
    if (piVar1 == (int *)0x0) {
      if (*piVar3 == 0) {
        iVar2 = piVar3[10];
        local_34 = 0x7fffffff;
        for (local_28 = 0; local_28 < piVar3[0xb]; local_28 = local_28 + 1) {
          iVar4 = *(int *)(PTR_DAT_10021034 +
                          ((param_1 >> 8 & 0xff) -
                          (uint)(byte)(&DAT_10028f11)[(uint)*(byte *)(local_28 + iVar2) * 4]) * 4) +
                  *(int *)(PTR_DAT_10021034 +
                          ((param_1 & 0xff) -
                          (*(uint *)(&DAT_10028f10 + (uint)*(byte *)(local_28 + iVar2) * 4) & 0xff))
                          * 4) +
                  *(int *)(PTR_DAT_10021034 +
                          (((param_1 & 0xff0000) >> 0x10) -
                          ((*(uint *)(&DAT_10028f10 + (uint)*(byte *)(local_28 + iVar2) * 4) &
                           0xff0000) >> 0x10)) * 4);
          if (iVar4 < local_34) {
            local_2c = (uint)*(byte *)(local_28 + iVar2);
            local_34 = iVar4;
          }
        }
        return *(undefined4 *)(&DAT_10028f10 + local_2c * 4);
      }
      return *(undefined4 *)(&DAT_10028f10 + piVar3[1] * 4);
    }
    piVar3 = piVar1;
  } while ((char)*piVar1 != '\x01');
  return *(undefined4 *)(&DAT_10028f10 + piVar1[1] * 4);
}

// FUNCTION: DRAWCARDLIB 0x10001933
uint FUN_10001933(uint param_1)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  byte *pbVar5;
  int local_34;
  uint local_2c;
  int local_28;
  
  FUN_1000163d(param_1,&DAT_10028f00);
  pbVar5 = &DAT_10028f00;
  piVar3 = DAT_10028ef4;
  do {
    piVar1 = (int *)piVar3[*pbVar5 + 2];
    pbVar5 = pbVar5 + 1;
    if (piVar1 == (int *)0x0) {
      if (*piVar3 == 0) {
        iVar2 = piVar3[10];
        local_34 = 0x7fffffff;
        for (local_28 = 0; local_28 < piVar3[0xb]; local_28 = local_28 + 1) {
          iVar4 = *(int *)(PTR_DAT_10021034 +
                          ((param_1 & 0xff) -
                          ((*(uint *)(&DAT_10028f10 + (uint)*(byte *)(local_28 + iVar2) * 4) &
                           0xff0000) >> 0x10)) * 4) +
                  *(int *)(PTR_DAT_10021034 +
                          (((param_1 & 0xff0000) >> 0x10) -
                          (*(uint *)(&DAT_10028f10 + (uint)*(byte *)(local_28 + iVar2) * 4) & 0xff))
                          * 4) +
                  *(int *)(PTR_DAT_10021034 +
                          ((param_1 >> 8 & 0xff) -
                          (uint)(byte)(&DAT_10028f11)[(uint)*(byte *)(local_28 + iVar2) * 4]) * 4);
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

// FUNCTION: DRAWCARDLIB 0x10001aab
int FUN_10001aab(int *param_1,int *param_2)

{
  int iVar1;
  int local_c;
  int local_8;
  
  local_8 = 0;
  if (*param_1 == 0) {
    for (local_c = 0; local_c < 8; local_c = local_c + 1) {
      if (param_1[local_c + 2] != 0) {
        iVar1 = FUN_10001aab(param_1[local_c + 2],param_2);
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

// FUNCTION: DRAWCARDLIB 0x10001b41
undefined4 FUN_10001b41(undefined4 *param_1,int param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 local_18;
  undefined4 local_10;
  undefined4 local_c;
  
  for (local_10 = 0; local_10 < param_2; local_10 = local_10 + 1) {
    local_c = 0;
    local_18 = *param_1;
    for (; local_c < param_3 * 3; local_c = local_c + 3) {
      uVar1 = *(undefined4 *)(local_c + 3 + (int)param_1);
      uVar2 = FUN_100017a5(local_18);
      *(undefined4 *)(local_c + (int)param_1) = uVar2;
      local_18 = uVar1;
    }
    param_1 = (undefined4 *)((int)param_1 + param_4 + param_3 * 3);
  }
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x10001bd9
undefined4 FUN_10001bd9(uint *param_1,int param_2,int param_3,int param_4)

{
  undefined1 uVar1;
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
      uVar2 = local_20 & 0xffffff;
      local_20 = *(uint *)(local_14 + 3 + (int)param_1);
      uVar1 = FUN_10001933(uVar2);
      *(undefined1 *)(local_8 + (int)local_10) = uVar1;
      if (*(uint *)(&DAT_10028f10 + (uint)*(byte *)(local_8 + (int)local_10) * 4) != uVar2) {
        *(undefined4 *)(&DAT_10028f10 + (uint)*(byte *)(local_8 + (int)local_10) * 4) = 0;
      }
      local_8 = local_8 + 1;
    }
    param_1 = (uint *)((int)param_1 + param_4 + param_3 * 3);
    local_10 = (uint *)((int)local_10 + param_4 + param_3);
  }
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x10001cbf
/* WARNING: Type propagation algorithm not settling */

int FUN_10001cbf(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6)

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
  local_7c = &DAT_100210c8 + param_1 * 0xc0;
  local_18 = param_5 * 8 + 0x50U >> 2;
  if (param_1 == 0) {
    DAT_10021e4c = param_1;
    param_4 = 0;
  }
  else if (param_1 == 1) {
    DAT_10021e4c = param_1;
    param_4 = FUN_10001b41(param_3,param_4,param_5,param_6);
  }
  else {
    if (DAT_10029320 == 0) {
      for (local_4c = -0x200; (int)local_4c < 0x200; local_4c = local_4c + 1) {
        if (((int)local_4c < 0) || (0xff < (int)local_4c)) {
          if ((int)local_4c < 0) {
            PTR_DAT_10021e48[local_4c] = 0;
          }
          else {
            PTR_DAT_10021e48[local_4c] = 0xff;
          }
        }
        else {
          PTR_DAT_10021e48[local_4c] = (undefined1)local_4c;
        }
      }
      DAT_10029320 = 1;
    }
    if (param_1 != DAT_10021e4c) {
      for (local_4c = 0; local_4c < 0x41; local_4c = local_4c + 1) {
        if (*(int *)(&DAT_10152430 + local_4c * 4) != 0) {
          FUN_1000cde0(*(undefined4 *)(&DAT_10152430 + local_4c * 4));
          *(undefined4 *)(&DAT_10152430 + local_4c * 4) = 0;
        }
      }
      FUN_100022a5(param_1,&DAT_10152430);
      DAT_10021e4c = param_1;
    }
    for (local_4c = 0; local_4c < 5; local_4c = local_4c + 1) {
      FUN_10008cf3(DAT_10128e40 + local_4c * 0x8060,0,0x8060);
      local_70[local_4c + 1] = (undefined *)(DAT_10128e40 + local_4c * 0x8060 + 0x28);
    }
    iVar1 = *(int *)(&DAT_10021050 + param_1 * 4);
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
        uVar2 = *(uint *)(local_c + param_3);
        local_28 = uVar2 & 0xffffff;
        *(uint *)(local_c + param_3) = *(uint *)(local_c + param_3) & 0xff000000;
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
          local_8 = (uint)(byte)PTR_DAT_10021e48[(uVar2 & 0xff) + ((int)*psVar4 >> 8)];
          local_38 = (uint)(byte)PTR_DAT_10021e48[(local_28 >> 8 & 0xff) + ((int)psVar4[1] >> 8)];
          local_74 = (uint)(byte)PTR_DAT_10021e48[(local_28 >> 0x10) + ((int)psVar4[2] >> 8)];
          local_28 = local_38 << 8 | local_74 << 0x10 | local_8;
          local_84 = FUN_100017a5(local_28);
        }
        *(uint *)(local_c + param_3) = *(uint *)(local_c + param_3) | local_84;
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
      FUN_10002264(local_70 + 1,*(undefined4 *)(&DAT_10021078 + param_1 * 4));
      memset(local_70[*(int *)(&DAT_10021078 + param_1 * 4)] + -0x28,0,local_18 << 2);
      if (param_2 != 0) {
        local_3c = -local_3c;
        local_7c = &DAT_100210c8 + (uint)(local_3c == -1) * 0x6c0 + param_1 * 0xc0;
      }
      param_3 = param_3 + param_5 * 3 + param_6;
    }
  }
  return param_4;
}

// FUNCTION: DRAWCARDLIB 0x10002264
void FUN_10002264(undefined4 *param_1,int param_2)

{
  undefined4 uVar1;
  
  uVar1 = *param_1;
  memcpy(param_1,param_1 + 1,param_2 * 4 - 4);
  param_1[param_2 + -1] = uVar1;
  return;
}

// FUNCTION: DRAWCARDLIB 0x100022a5
undefined4 FUN_100022a5(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  void *pvVar3;
  int local_14;
  int local_10;
  
  iVar1 = *(int *)(&DAT_100210a0 + param_1 * 4);
  for (local_10 = 0; local_10 < *(int *)(&DAT_10021050 + param_1 * 4); local_10 = local_10 + 1) {
    iVar2 = *(int *)(&DAT_100210c8 + local_10 * 0x10 + param_1 * 0xc0);
    if (*(int *)(param_2 + iVar2 * 4) == 0) {
      pvVar3 = malloc(0x800);
      *(void **)(param_2 + iVar2 * 4) = pvVar3;
      FUN_1000c0f0(*(int *)(param_2 + iVar2 * 4) != 0,s_D__Newmagic_sources_NedCard_Pale_10021ea4,
                   0x4fd,s_Not_enough_memory_for_delta_arra_10021e80);
      for (local_14 = -0x100; local_14 < 0x100; local_14 = local_14 + 1) {
        *(int *)(*(int *)(param_2 + iVar2 * 4) + 0x400 + local_14 * 4) =
             ((iVar2 * local_14 + (iVar1 >> 1)) * 0x100) / *(int *)(&DAT_100210a0 + param_1 * 4);
      }
      *(int *)(&DAT_100210d4 + local_10 * 0x10 + param_1 * 0xc0) =
           *(int *)(param_2 + iVar2 * 4) + 0x3fc;
    }
    else {
      *(int *)(&DAT_100210d4 + local_10 * 0x10 + param_1 * 0xc0) =
           *(int *)(param_2 + iVar2 * 4) + 0x400;
    }
  }
  for (local_10 = 0; local_10 < *(int *)(&DAT_10021050 + param_1 * 4); local_10 = local_10 + 1) {
    *(int *)(&DAT_10021794 + local_10 * 0x10 + param_1 * 0xc0) =
         *(int *)(param_2 + *(int *)(&DAT_10021788 + local_10 * 0x10 + param_1 * 0xc0) * 4) + 0x3fc;
  }
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x10002452
undefined4 FUN_10002452(int param_1,byte *param_2,int param_3,int param_4,int param_5)

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
  void *_Dst;
  void *_Dst_00;
  byte *local_1c;
  int local_18;
  int local_10;
  
  _Dst = param_2;
  _Size = param_3 * 3;
  _Dst_00 = malloc((_Size + param_5) * param_4 + 0x10);
  if (DAT_10028ef8 == 0) {
    for (local_10 = -0x200; local_10 < 0x200; local_10 = local_10 + 1) {
      if ((local_10 < 0) || (0xff < local_10)) {
        if (local_10 < 0) {
          PTR_DAT_10021e48[local_10] = 0;
        }
        else {
          PTR_DAT_10021e48[local_10] = 0xff;
        }
      }
      else {
        PTR_DAT_10021e48[local_10] = (undefined1)local_10;
      }
    }
    DAT_10028ef8 = 1;
  }
  memcpy(_Dst_00,param_2,_Size);
  param_2 = (byte *)((int)param_2 + _Size + param_5);
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
      *local_1c = PTR_DAT_10021e48
                  [(int)(((((((uint)param_2[param_3 * -3 - param_5] * -2 -
                             (uint)param_2[(param_3 * -3 - param_5) + -3]) -
                            (uint)param_2[(param_3 * -3 - param_5) + 3]) + (uint)*pbVar19 * -2 +
                            (uint)*param_2 * param_1 + (uint)pbVar19[6] * -2) -
                          (uint)param_2[_Size + param_5 + -3]) + (uint)param_2[_Size + param_5] * -2
                         ) - (uint)param_2[_Size + param_5 + 3]) / (param_1 + -0xc)];
      pbVar20[4] = PTR_DAT_10021e48
                   [(int)(((((((uint)bVar2 * -2 - (uint)bVar1) - (uint)bVar3) + (uint)bVar4 * -2 +
                             (uint)bVar5 * param_1 + (uint)bVar6 * -2) - (uint)bVar7) +
                          (uint)bVar8 * -2) - (uint)bVar9) / (param_1 + -0xc)];
      pbVar20[5] = PTR_DAT_10021e48
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
  FUN_1000cde0(_Dst_00);
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x10002877
/* WARNING: Type propagation algorithm not settling */

undefined4 FUN_10002877(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6)

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
  local_7c = &DAT_100210c8 + param_1 * 0xc0;
  local_18 = param_5 * 8 + 0x50U >> 2;
  if (DAT_1002931c == 0) {
    for (local_4c = -0x200; (int)local_4c < 0x200; local_4c = local_4c + 1) {
      if (((int)local_4c < 0) || (0xff < (int)local_4c)) {
        if ((int)local_4c < 0) {
          PTR_DAT_10021e48[local_4c] = 0;
        }
        else {
          PTR_DAT_10021e48[local_4c] = 0xff;
        }
      }
      else {
        PTR_DAT_10021e48[local_4c] = (undefined1)local_4c;
      }
    }
    DAT_1002931c = 1;
  }
  if (DAT_10021e50 != param_1) {
    for (local_4c = 0; local_4c < 0x41; local_4c = local_4c + 1) {
      if (*(int *)(&DAT_10152430 + local_4c * 4) != 0) {
        FUN_1000cde0(*(undefined4 *)(&DAT_10152430 + local_4c * 4));
        *(undefined4 *)(&DAT_10152430 + local_4c * 4) = 0;
      }
    }
    FUN_100022a5(param_1,&DAT_10152430);
    DAT_10021e50 = param_1;
  }
  for (local_4c = 0; local_4c < 5; local_4c = local_4c + 1) {
    memset(DAT_10128e40 + local_4c * 0x8060,0,0x8060);
    local_70[local_4c + 1] = (undefined *)(DAT_10128e40 + local_4c * 0x8060 + 0x28);
  }
  iVar2 = *(int *)(&DAT_10021050 + param_1 * 4);
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
      local_8 = (uint)(byte)PTR_DAT_10021e48[(uVar3 & 0xff) + ((int)*psVar6 >> 8)];
      local_38 = (uint)(byte)PTR_DAT_10021e48[(uVar5 >> 8 & 0xff) + ((int)psVar6[1] >> 8)];
      bVar1 = PTR_DAT_10021e48[(uVar5 >> 0x10) + ((int)psVar6[2] >> 8)];
      local_28 = (uint)(byte)PTR_DAT_10021e48[(uVar5 >> 8 & 0xff) + ((int)psVar6[1] >> 8)] << 8 |
                 (uint)bVar1 << 0x10 |
                 (uint)(byte)PTR_DAT_10021e48[(uVar3 & 0xff) + ((int)*psVar6 >> 8)];
      if (local_28 == 0) {
        local_84 = 0;
      }
      else if (local_28 == 0xffffff) {
        local_84 = 0xffffff;
      }
      else {
        local_84 = FUN_10002e80(local_28);
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
    FUN_10002264(local_70 + 1,*(undefined4 *)(&DAT_10021078 + param_1 * 4));
    memset((void *)(local_70[*(int *)(&DAT_10021078 + param_1 * 4)] + -0x28),0,local_18 << 2);
    if (param_2 != 0) {
      local_3c = -local_3c;
      local_7c = &DAT_100210c8 + (uint)(local_3c == -1) * 0x6c0 + param_1 * 0xc0;
    }
    param_3 = param_3 + param_5 * 3 + param_6;
  }
  return 1;
}

// FUNCTION: DRAWCARDLIB 0x10002db9
void FUN_10002db9(void)

{
  FUN_10001591(DAT_10028ef4);
  DAT_10028ef4 = 0;
  return;
}

// FUNCTION: DRAWCARDLIB 0x10002ddc
int FUN_10002ddc(int param_1,int param_2,int param_3,byte *param_4)

{
  int iVar1;
  int local_14;
  int local_10;
  int local_c;
  
  local_14 = 0x7fffffff;
  for (local_c = 0; local_c < 0x100; local_c = local_c + 1) {
    iVar1 = *(int *)(PTR_DAT_10021034 + (param_3 - (uint)param_4[2]) * 4) +
            *(int *)(PTR_DAT_10021034 + (param_2 - (uint)param_4[1]) * 4) +
            *(int *)(PTR_DAT_10021034 + (param_1 - (uint)*param_4) * 4);
    if (iVar1 < local_14) {
      local_10 = local_c;
      local_14 = iVar1;
    }
    param_4 = param_4 + 3;
  }
  return local_10;
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x10002e80
uint FUN_10002e80(uint param_1)
{
  return param_1 & 0xf8f8f8;
}

// FUNCTION: DRAWCARDLIB 0x10002ea0
void FUN_10002ea0(undefined8 *param_1,uint param_2)

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

// FUNCTION: DRAWCARDLIB 0x10002ef8
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_10002ef8(undefined1 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 uVar3;
  byte local_28;
  int local_1c;
  int local_18;
  uint local_c;
  int local_8;
  
  puVar1 = param_1;
  DAT_10029fb0 = param_2;
  _DAT_10029fb4 = param_2;
  DAT_10029fb8 = param_3;
  DAT_10021ecc = 0;
  iVar2 = BitStream_ReadBits(8);
  for (local_18 = 0; local_18 < iVar2; local_18 = local_18 + 1) {
    uVar3 = BitStream_ReadBits(9);
    *(undefined4 *)(&DAT_10029fc8 + local_18 * 8) = uVar3;
    uVar3 = BitStream_ReadBits(9);
    *(undefined4 *)(&DAT_10029fcc + local_18 * 8) = uVar3;
  }
  FUN_100030b4(iVar2);
  local_c = BitStream_ReadBits(8);
  do {
    while( true ) {
      if (local_c == 0xffffffff) {
        return (int)param_1 - (int)puVar1;
      }
      if (*(int *)(&DAT_100293b0 + local_c * 0xc) < 0) break;
      uVar3 = *(undefined4 *)(&DAT_100293b4 + local_c * 0xc);
      *param_1 = (&DAT_100293b0)[local_c * 0xc];
      param_1 = param_1 + 1;
      local_28 = (byte)uVar3;
      iVar2 = BitStream_ReadBits(uVar3);
      if (iVar2 == -1) {
        local_c = 0xffffffff;
      }
      else {
        local_c = (int)local_c >> (local_28 & 0x1f) | iVar2 << (8 - local_28 & 0x1f);
      }
    }
    local_8 = *(int *)(&DAT_100293b8 + local_c * 0xc);
    do {
      iVar2 = BitStream_ReadBit();
      if (iVar2 == -1) goto LAB_10003092;
      if (iVar2 == 0) {
        local_1c = *(int *)(&DAT_10029fcc + local_8 * 8);
      }
      else {
        local_1c = *(int *)(&DAT_10029fc8 + local_8 * 8);
      }
      local_8 = local_1c + -0x100;
    } while (-1 < local_8);
    *param_1 = (undefined1)local_1c;
    param_1 = param_1 + 1;
LAB_10003092:
    local_c = BitStream_ReadBits(8);
  } while( true );
}

// FUNCTION: DRAWCARDLIB 0x100030b4
undefined4 FUN_100030b4(int param_1)

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
      local_c = *(int *)(&DAT_10029fcc + local_8 * 8);
    }
    else {
      local_c = *(int *)(&DAT_10029fc8 + local_8 * 8);
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
        *(int *)(&DAT_100293b4 + (uVar2 | local_a0) * 0xc) = local_34;
        *(undefined4 *)(&DAT_100293b0 + (uVar2 | local_a0) * 0xc) =
             *(undefined4 *)(DAT_10029fc4 + local_c * 4);
        *(undefined4 *)(&DAT_100293b8 + (uVar2 | local_a0) * 0xc) = 0xffffffff;
      }
      local_98[local_34 + 9] = 1;
      iVar1 = local_34;
      local_34 = local_34 + -1;
      local_98[iVar1 + 8] = local_98[iVar1 + 8] + -1;
      local_8 = local_30[local_34];
LAB_10003365:
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
      *(undefined4 *)(&DAT_100293b0 + local_a0 * 0xc) = 0xffffffff;
      *(undefined4 *)(&DAT_100293b4 + local_a0 * 0xc) = 8;
      *(int *)(&DAT_100293b8 + local_a0 * 0xc) = local_8;
      local_98[0x11] = 1;
      local_34 = 7;
      local_98[0x10] = local_98[0x10] + -1;
      local_8 = iStack_14;
      goto LAB_10003365;
    }
    if (local_98[9] < 0) {
      return 0;
    }
  } while( true );
}

// FUNCTION: DRAWCARDLIB 0x100033b7
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_100033b7(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int iVar3;
  byte local_24;
  int local_14;
  uint local_c;
  int local_8;
  
  puVar2 = param_1;
  DAT_10029fb0 = param_2;
  _DAT_10029fb4 = param_2;
  DAT_10029fb8 = param_3;
  DAT_10021ecc = 0;
  local_c = BitStream_ReadBits(8);
  do {
    while( true ) {
      if (local_c == 0xffffffff) {
        return (int)param_1 - (int)puVar2 >> 2;
      }
      if (0x7ffffffe < *(int *)(&DAT_100293b0 + local_c * 0xc)) break;
      uVar1 = *(undefined4 *)(&DAT_100293b4 + local_c * 0xc);
      *param_1 = *(undefined4 *)(&DAT_100293b0 + local_c * 0xc);
      param_1 = param_1 + 1;
      local_24 = (byte)uVar1;
      iVar3 = BitStream_ReadBits(uVar1);
      if (iVar3 == -1) {
        local_c = 0xffffffff;
      }
      else {
        local_c = (int)local_c >> (local_24 & 0x1f) | iVar3 << (8 - local_24 & 0x1f);
      }
    }
    local_8 = *(int *)(&DAT_100293b8 + local_c * 0xc);
    do {
      iVar3 = BitStream_ReadBit();
      if (iVar3 == -1) goto LAB_10003506;
      if (iVar3 == 0) {
        local_14 = *(int *)(&DAT_10029fcc + local_8 * 8);
      }
      else {
        local_14 = *(int *)(&DAT_10029fc8 + local_8 * 8);
      }
      local_8 = local_14 - _DAT_10029fc0;
    } while (-1 < local_8);
    *param_1 = *(undefined4 *)(DAT_10029fc4 + local_14 * 4);
    param_1 = param_1 + 1;
LAB_10003506:
    local_c = BitStream_ReadBits(8);
  } while( true );
}

// FUNCTION: DRAWCARDLIB 0x1000352b
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_1000352b(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  uint local_c;
  int local_8;
  
  local_c = 0xd;
  for (local_8 = 0; local_8 < 0x20; local_8 = local_8 + 1) {
    *(uint *)(&DAT_10029328 + local_8 * 4) = 0xffffffff >> ((byte)local_8 & 0x1f);
  }
  DAT_10029fb0 = param_1;
  _DAT_10029fb4 = param_1;
  DAT_10029fb8 = 100000;
  DAT_10021ecc = 0;
  DAT_100293ac = 0;
  DAT_10029fbc = BitStream_ReadBits(0xd);
  for (local_8 = 0; local_8 < DAT_10029fbc; local_8 = local_8 + 1) {
    uVar1 = BitStream_ReadBits(0xd);
    *(undefined4 *)(&DAT_10029fc8 + local_8 * 8) = uVar1;
    uVar1 = BitStream_ReadBits(0xd);
    *(undefined4 *)(&DAT_10029fcc + local_8 * 8) = uVar1;
    local_c = local_c + 0x1a;
  }
  _DAT_10029fc0 = param_3;
  DAT_10029fc4 = param_2;
  FUN_10003643(DAT_10029fbc);
  return (uint)((local_c & 7) != 0) + ((int)(local_c + ((int)local_c >> 0x1f & 7U)) >> 3);
}

// FUNCTION: DRAWCARDLIB 0x10003643
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_10003643(int param_1)

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
      local_c = *(int *)(&DAT_10029fcc + local_8 * 8);
    }
    else {
      local_c = *(int *)(&DAT_10029fc8 + local_8 * 8);
    }
    local_8 = local_c - _DAT_10029fc0;
    local_38[local_3c] = local_c - _DAT_10029fc0;
    if (local_8 < 0) {
      local_f0 = 0;
      for (local_f4 = 0; local_f4 < local_3c; local_f4 = local_f4 + 1) {
        local_f0 = local_f0 | local_c8[local_f4 + 3] << ((byte)local_f4 & 0x1f);
      }
      for (local_ec = 0; local_ec < local_e8[8 - local_3c]; local_ec = local_ec + 1) {
        uVar2 = local_ec << ((byte)local_3c & 0x1f);
        *(int *)(&DAT_100293b4 + (uVar2 | local_f0) * 0xc) = local_3c;
        *(undefined4 *)(&DAT_100293b0 + (uVar2 | local_f0) * 0xc) =
             *(undefined4 *)(DAT_10029fc4 + local_c * 4);
        *(undefined4 *)(&DAT_100293b8 + (uVar2 | local_f0) * 0xc) = 0xffffffff;
      }
      local_c8[local_3c + 3] = 1;
      iVar1 = local_3c;
      local_3c = local_3c + -1;
      local_c8[iVar1 + 2] = local_c8[iVar1 + 2] + -1;
      local_8 = local_38[local_3c];
LAB_10003932:
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
      *(undefined4 *)(&DAT_100293b0 + local_f0 * 0xc) = 0x7fffffff;
      *(undefined4 *)(&DAT_100293b4 + local_f0 * 0xc) = 8;
      *(int *)(&DAT_100293b8 + local_f0 * 0xc) = local_8;
      local_c8[0xb] = 1;
      local_3c = 7;
      local_c8[10] = local_c8[10] + -1;
      local_8 = iStack_1c;
      goto LAB_10003932;
    }
    if (local_c8[3] < 0) {
      return 0;
    }
  } while( true );
}

// FUNCTION: DRAWCARDLIB 0x10003993
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_10003993(int param_1)

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
  memset(&DAT_10127dc0,0,0x80);
  do {
    iVar1 = local_90;
    local_90 = local_90 + 1;
    if (local_110[iVar1] == 0) {
      local_c = *(int *)(&DAT_10029fcc + local_8 * 8);
    }
    else {
      local_c = *(int *)(&DAT_10029fc8 + local_8 * 8);
    }
    local_8 = local_c - _DAT_10029fc0;
    local_8c[local_90] = local_c - _DAT_10029fc0;
    if (local_8 < 0) {
      *(int *)(&DAT_10127dc0 + local_90 * 4) = *(int *)(&DAT_10127dc0 + local_90 * 4) + 1;
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

// FUNCTION: DRAWCARDLIB 0x10003b2e
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_10003b2e(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int local_10;
  int local_c;
  int local_8;
  
  local_10 = 0;
  DAT_10029fb0 = param_2;
  _DAT_10029fb4 = param_2;
  DAT_10029fb8 = param_3;
  DAT_10021ecc = 0;
  local_8 = DAT_10029fbc + -1;
  do {
    do {
      iVar1 = BitStream_ReadBit();
      if (iVar1 == -1) {
        return local_10;
      }
      if (iVar1 == 0) {
        local_c = *(int *)(&DAT_10029fcc + local_8 * 8);
      }
      else {
        local_c = *(int *)(&DAT_10029fc8 + local_8 * 8);
      }
    local_8 = local_c - _DAT_10029fc0;
    } while (-1 < local_8);
    if (local_c == 0) {
      iVar1 = BitStream_ReadBits(10);
      if (iVar1 < 0) {
        return local_10;
      }
      memset(param_1,0,iVar1 << 2);
      param_1 = param_1 + iVar1;
      local_10 = local_10 + iVar1;
    }
    else {
      *param_1 = *(undefined4 *)(DAT_10029fc4 + local_c * 4);
      param_1 = param_1 + 1;
      local_10 = local_10 + 1;
    }
    local_8 = DAT_10029fbc + -1;
  } while( true );
}

// FUNCTION: DRAWCARDLIB 0x10003c3e
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_10003c3e(undefined4 *param_1,uint *param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  byte local_28;
  int local_14;
  uint local_c;
  int local_8;
  
  puVar1 = param_1;
  DAT_10029fb0 = param_2;
  _DAT_10029fb4 = param_2;
  DAT_10029fb8 = param_3;
  if (((uint)param_2 & 3) == 0) {
    DAT_10021ecc = 0;
    DAT_100293ac = 0;
  }
  else {
    iVar2 = 4 - ((uint)param_2 & 3);
    DAT_10021ecc = iVar2 * 8;
    DAT_100293ac = 0xffffffffU >> (0x20U - (char)DAT_10021ecc & 0x1f) & *param_2;
    DAT_10029fb0 = (uint *)((int)param_2 + iVar2);
  }
  local_c = BitStream_ReadBits(8);
  while (local_c != 0xffffffff) {
    if (*(int *)(&DAT_100293b0 + local_c * 0xc) < 0x7fffffff) {
      iVar2 = *(int *)(&DAT_100293b4 + local_c * 0xc);
      local_28 = (byte)iVar2;
      if (*(int *)(&DAT_100293b0 + local_c * 0xc) == -0x80000000) {
        iVar2 = BitStream_ReadBits(iVar2 + 2);
        if (iVar2 < 0) break;
        uVar3 = iVar2 << (8 - local_28 & 0x1f) | (int)local_c >> (local_28 & 0x1f);
        FUN_10002ea0(param_1,uVar3);
        param_1 = param_1 + uVar3;
        local_c = BitStream_ReadBits(8);
      }
      else {
        *param_1 = *(undefined4 *)(&DAT_100293b0 + local_c * 0xc);
        param_1 = param_1 + 1;
        iVar2 = BitStream_ReadBits(iVar2);
        if (iVar2 == -1) break;
        local_c = (int)local_c >> (local_28 & 0x1f) | iVar2 << (8 - local_28 & 0x1f);
      }
    }
    else {
      local_8 = *(int *)(&DAT_100293b8 + local_c * 0xc);
      do {
        iVar2 = BitStream_ReadBit();
        if (iVar2 == -1) goto LAB_10003ea6;
        if (iVar2 == 0) {
          local_14 = *(int *)(&DAT_10029fcc + local_8 * 8);
        }
        else {
          local_14 = *(int *)(&DAT_10029fc8 + local_8 * 8);
        }
        local_8 = local_14 - _DAT_10029fc0;
      } while (-1 < local_8);
      if (local_14 == 0) {
        iVar2 = BitStream_ReadBits(10);
        if (-1 < iVar2) {
          FUN_10002ea0(param_1,iVar2);
          param_1 = param_1 + iVar2;
        }
      }
      else {
        *param_1 = *(undefined4 *)(DAT_10029fc4 + local_14 * 4);
        param_1 = param_1 + 1;
      }
LAB_10003ea6:
      local_c = BitStream_ReadBits(8);
    }
  }
  return (int)param_1 - (int)puVar1 >> 2;
}

// FUNCTION: DRAWCARDLIB 0x10003ed0
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint BitStream_ReadBits(uint bitCount)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  byte local_c;
  byte bVar4;
  
  if (DAT_10021ecc < bitCount) {
    iVar1 = bitCount - DAT_10021ecc;
    if ((int)DAT_10029fb0 + (4 - _DAT_10029fb4) < DAT_10029fb8) {
      uVar2 = *DAT_10029fb0;
      DAT_10029fb0 = DAT_10029fb0 + 1;
      local_c = (byte)DAT_10021ecc;
      uVar3 = DAT_100293ac |
              (*(uint *)(&DAT_10029328 + (0x20 - iVar1) * 4) & uVar2) << (local_c & 0x1f);
      bVar4 = (byte)iVar1;
      DAT_100293ac = uVar2 >> (bVar4 & 0x1f);
      DAT_10021ecc = 0x20 - iVar1;
    }
    else {
      uVar3 = 0xffffffff;
    }
  }
  else {
    uVar3 = *(uint *)(&DAT_10029328 + (0x20 - bitCount) * 4) & DAT_100293ac;
    DAT_100293ac = DAT_100293ac >> ((byte)bitCount & 0x1f);
    DAT_10021ecc = DAT_10021ecc - bitCount;
  }
  return uVar3;
}

// FUNCTION: DRAWCARDLIB 0x10003fc0
uint BitStream_ReadBit(void)
{
  uint result = 0;
  
  if (DAT_10021ecc == 0) {
    if (DAT_10029fb8 <= (int)DAT_10029fb0 - _DAT_10029fb4) {
      return 0xffffffff;
    }
    DAT_100293ac = *DAT_10029fb0;
    DAT_10029fb0 = DAT_10029fb0 + 1;
    DAT_10021ecc = 0x20;
  }
  result = (uint)((DAT_100293ac & 1) != 0);
  DAT_100293ac = DAT_100293ac >> 1;
  DAT_10021ecc = DAT_10021ecc + -1;
  return result;
}

// FUNCTION: DRAWCARDLIB 0x10004050
undefined1 * PcxLoad8bppImage(char *path,undefined1 *imagePixels,void *paletteOut)

{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  HGLOBAL pvVar4;
  uint uVar5;
  undefined4 local_18;
  int local_14;
  int local_8;
  
  gPcxInFile = fopen(path,&DAT_10021f18);
  FUN_1000c0f0(gPcxInFile != (FILE *)0x0,PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0x69,
               s_Error_Opening_File__s_10021f00,path);
  gPcxPath = path;
  PcxReadHeaderAndPalette(paletteOut);
  if ((DAT_10127d43 == '\b') && (DAT_10127d81 == '\x01')) {
    local_18 = 1;
  }
  else {
    local_18 = 0;
  }
  FUN_1000c0f0(local_18,PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0x6f,
               s__s_Not_a_256_color_palettized_pc_10021f1c,gPcxPath);
  uVar2 = 4 - (gImageWidth & 3);
  uVar5 = (int)uVar2 >> 0x1f;
  iVar3 = ((uVar2 ^ uVar5) - uVar5 & 3 ^ uVar5) - uVar5;
  if ((gImageWidth == gPcxBufferWidth) && (gImageHeight == gPcxBufferHeight)) {
    memset(imagePixels,0,(gPcxBufferWidth + iVar3) * gPcxBufferHeight);
  }
  else {
    pvVar4 = GlobalHandle(imagePixels);
    GlobalUnlock(pvVar4);
    pvVar4 = GlobalHandle(imagePixels);
    GlobalUnlock(pvVar4);
    pvVar4 = GlobalHandle(imagePixels);
    GlobalFree(pvVar4);
    pvVar4 = GlobalAlloc(0x40,(gImageWidth + iVar3) * gImageHeight);
    imagePixels = GlobalLock(pvVar4);
    pvVar4 = GlobalHandle(imagePixels);
    GlobalLock(pvVar4);
    gPcxBufferWidth = gImageWidth;
    gPcxBufferHeight = gImageHeight;
  }
  puVar1 = imagePixels;
  for (local_8 = 0; local_8 < gImageHeight; local_8 = local_8 + 1) {
    PcxReadScanlineRle(&DAT_10126d30);
    for (local_14 = 0; local_14 < (int)gImageWidth; local_14 = local_14 + 1) {
      *imagePixels = (&DAT_10126d30)[local_14];
      imagePixels = imagePixels + 1;
    }
    imagePixels = imagePixels + iVar3;
  }
  fclose(gPcxInFile);
  return puVar1;
}

// FUNCTION: DRAWCARDLIB 0x10004267
bool PcxReadHeaderAndPaletteFromPath(char *path,void *paletteOut)

{
  int iVar1;
  
  gPcxInFile = fopen(path,&DAT_10021f60);
  FUN_1000c0f0(gPcxInFile != (FILE *)0x0,PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0x9c,
               s_Error_Opening_File__s_10021f48,path);
  gPcxPath = path;
  iVar1 = PcxReadHeaderAndPalette(paletteOut);
  if (iVar1 != 0) {
    fclose(gPcxInFile);
  }
  return iVar1 != 0;
}

// FUNCTION: DRAWCARDLIB 0x100042ea
undefined4 PcxReadHeaderAndPalette(void *paletteOut)

{
  int local_8;
  
  fread(&DAT_10127d40,0x80,1,gPcxInFile);
  FUN_1000c0f0(DAT_10127d40 == '\n',PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0xad,
               s__s_Not_a_pcx_file_10021f64,gPcxPath);
  FUN_1000c0f0(DAT_10127d41 == '\x05',PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0xae,
               s__s_Not_a_version_5_pcx_file_10021f78,gPcxPath);
  gImageWidth = ((uint)DAT_10127d48 - (uint)DAT_10127d44) + 1;
  gImageHeight = ((uint)DAT_10127d4a - (uint)DAT_10127d46) + 1;
  if (paletteOut != (void *)0x0) {
    if ((DAT_10127d81 == '\x01') && (DAT_10127d43 == '\b')) {
      fseek(gPcxInFile,-0x300,2);
      fread(paletteOut,1,0x300,gPcxInFile);
      fseek(gPcxInFile,0x80,0);
    }
    else if ((DAT_10127d81 == '\x04') && (DAT_10127d43 == '\x01')) {
      fseek(gPcxInFile,0x10,2);
      for (local_8 = 0; local_8 < 0x10; local_8 = local_8 + 1) {
        fread((void *)(local_8 * 4 + (int)paletteOut),1,3,gPcxInFile);
      }
      fseek(gPcxInFile,0x80,0);
    }
    else {
      FUN_1000c0f0(0,PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0xd4,
                   s__s_is_not_in_a_recognizable_form_10021f98,gPcxPath);
    }
  }
  return 1;
}

// FUNCTION: DRAWCARDLIB 0x100044d0
undefined4 PcxReadScanlineRle(byte *dstScanline)

{
  uint uVar1;
  int iVar2;
  int local_18;
  uint local_10;
  
  local_18 = (int)gPcxBytesPerLine;
  while (0 < local_18) {
    uVar1 = fgetc(gPcxInFile);
    if (((byte)uVar1 & 0xc0) == 0xc0) {
      uVar1 = uVar1 & 0x3f;
      iVar2 = fgetc(gPcxInFile);
      if (uVar1 < 2) {
        *dstScanline = (byte)iVar2;
        dstScanline = dstScanline + 1;
        local_18 = local_18 + -1;
      }
      else {
        for (local_10 = 0; local_10 < uVar1; local_10 = local_10 + 1) {
          *dstScanline = (byte)iVar2;
          dstScanline = dstScanline + 1;
        }
        local_18 = local_18 - uVar1;
      }
    }
    else {
      *dstScanline = (byte)uVar1;
      dstScanline = dstScanline + 1;
      local_18 = local_18 + -1;
    }
  }
  return 1;
}

// FUNCTION: DRAWCARDLIB 0x100045ac
undefined4
PcxSave8bppImage(undefined *srcPixels,char *path,void* palette,undefined4 unused_param_4,
            undefined4 unused_param_5,int width,int height)

{
  int local_100c;
  undefined1 local_1008 [4064];
  undefined4 uStackY_28;
  undefined1 *puStackY_24;
  undefined *puStackY_20;
  undefined4 uStackY_1c;
  
  MSVC_StackProbe();
  uStackY_1c = 0x100045ce;
  gPcxOutFile = fopen(path,&DAT_10021fd8);
  uStackY_1c = 0x146;
  puStackY_20 = PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0;
  puStackY_24 = (undefined1 *)(uint)(gPcxOutFile != (FILE *)0x0);
  uStackY_28 = 0x100045fa;
  FUN_1000c0f0(               (uint)(gPcxOutFile != (FILE *)0x0),
               PTR_s_D__Newmagic_sources_sidlib_Pcxw__10021ed0,0x146,
               s_Error_Opening_File__s_10021fc0);
  gPcxPath = path;
  uStackY_1c = 0x10004612;
  PcxWriteHeader(width,(short)height);
  for (local_100c = 0; local_100c < height; local_100c = local_100c + 1) {
    uStackY_1c = unused_param_4;
    puStackY_20 = srcPixels;
    puStackY_24 = local_1008;
    uStackY_28 = 0x1000465b;
    FUN_1000b80f();
    uStackY_1c = 0x1000466e;
    PcxWriteScanlineRle(local_1008,width);
  }
  PcxWritePalette256(palette);
  fclose(gPcxOutFile);
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x1000469c
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 PcxWriteHeader(undefined4 width,short height)

{
  ushort uVar1;
  ushort uVar2;
  
  DAT_10127d40 = 10;
  DAT_10127d41 = 5;
  DAT_10127d42 = 1;
  DAT_10127d43 = 8;
  DAT_10127d44 = 0;
  uVar1 = (ushort)width;
  DAT_10127d48 = uVar1 - 1;
  DAT_10127d46 = 0;
  DAT_10127d4a = height + -1;
  _DAT_10127d4c = 0;
  _DAT_10127d4e = 0;
  DAT_10127d80 = 0;
  DAT_10127d81 = 1;
  uVar2 = (ushort)((int)width >> 0x1f);
  gPcxBytesPerLine = uVar1 + (((uVar1 ^ uVar2) - uVar2 & 1 ^ uVar2) - uVar2);
  _DAT_10127d84 = 1;
  _DAT_10127d86 = 0;
  _DAT_10127d88 = 0;
  fwrite(&DAT_10127d40,0x80,1,gPcxOutFile);
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x10004760
undefined4 PcxWriteScanlineRle(char *srcScanline,int widthBytes)

{
  int iVar1;
  int local_14;
  uint local_c;
  uint local_8;
  
  local_8 = 0;
  local_14 = 0;
  while (local_14 < widthBytes) {
    local_8 = (local_8 & 0xffffff00U) | (uint)(byte)*srcScanline;
    if ((widthBytes - local_14 == 1) || (srcScanline[1] != *srcScanline)) {
      PcxWriteRleByte(local_8);
      local_14 = local_14 + 1;
      srcScanline = srcScanline + 1;
    }
    else {
      iVar1 = widthBytes - local_14;
      if (0x3e < iVar1) {
        iVar1 = 0x3f;
      }
      local_c = CountRepeats(local_8,srcScanline,iVar1);
      local_14 = local_14 + local_c;
      srcScanline = srcScanline + local_c;
      local_c = local_c | 0xc0;
      fwrite(&local_c,1,1,gPcxOutFile);
      fwrite(&local_8,1,1,gPcxOutFile);
    }
  }
  if (local_14 < gPcxBytesPerLine) {
    local_8 = local_8 & 0xffffff00U;
    fwrite(&local_8,1,1,gPcxOutFile);
  }
  return 1;
}

// FUNCTION: DRAWCARDLIB 0x10004873
void PcxWriteRleByte(byte value)

{
  undefined1 local_8 [4];
  
  local_8[0] = 0xc1;
  if ((value & 0xc0) == 0xc0) {
    fwrite(local_8,1,1,gPcxOutFile);
  }
  fwrite(&value,1,1,gPcxOutFile);
  return;
}

// FUNCTION: DRAWCARDLIB 0x100048c0
int CountRepeats(char value,char *buffer,int maxCount)

{
  int local_8;
  
  local_8 = 0;
  while ((maxCount != 0 && (*buffer == value))) {
    local_8 = local_8 + 1;
    buffer = buffer + 1;
    maxCount = maxCount + -1;
  }
  return local_8;
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x1000491f
undefined4 PcxWritePalette256(void *palette)
{
  undefined1 local_8 [4];
  
  local_8[0] = 0xc;
  fwrite(local_8,1,1,gPcxOutFile);
  fwrite(palette,3,0x100,gPcxOutFile);
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x10004970
undefined4 prepare_fonts_and_imgs(void)

{
  LOGFONTA *pLVar1;
  undefined4 uVar2;
  CHAR local_10c [264];
  
  strcpy(&DAT_1003a090,&DAT_100f35b0);
  strcat(&DAT_1003a090,s__DuelArt_1002201c);
  strcat(&DAT_1003a090,s__Duel_dat_10022028);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Tt0530m__TTF_10022034);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Magis____TTF_10022044);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Tt0127m__TTF_10022054);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Tt0085m__TTF_10022064);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Tt0298m__TTF_10022074);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Tt0299m__TTF_10022084);
  AddFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Tt0300m__TTF_10022094);
  AddFontResourceA(local_10c);
  sprintf(local_10c,s__s_ManaSymbols_pic_100220a4,&DAT_100f36c0);
  DAT_1003a198 = FUN_1000b700(local_10c);
  sprintf(local_10c,s__s_CardSets_pic_100220b8,&DAT_100f36c0);
  DAT_10039fe4 = FUN_1000b700(local_10c);
  sprintf(local_10c,s__s_CardBack_pic_100220c8,&DAT_100f36c0);
  DAT_1003a010 = FUN_1000b700(local_10c);
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
  
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Magim____TTF_1002216c);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Tt0530m__TTF_1002217c);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Magis____TTF_1002218c);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Tt0127m__TTF_1002219c);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Tt0085m__TTF_100221ac);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Tt0298m__TTF_100221bc);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
  strcat(local_10c,s__Tt0299m__TTF_100221cc);
  RemoveFontResourceA(local_10c);
  strcpy(local_10c,&DAT_100f35b0);
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
      FUN_1000a885(param_1,&local_14,DAT_1003a010);
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
      FUN_1000a885(param_1,&local_4e8,*local_4ec);
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
          FUN_1000a8ed(param_1,&local_7c,local_68,(local_574 * 0x49) / 1000,
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
      iVar2 = FUN_1000b700(local_170);
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
      FUN_1000a8ed(param_1,&local_1c,*local_20,0,0,local_34,(local_30 * 0x3b) / 100);
      local_8 = 0x1e;
      local_c = 0x16;
      SetRect(&local_1c,0,0x16,200,0x34);
      FUN_1000a8ed(param_1,&local_1c,*local_20,0,2,local_34,
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

// FUNCTION: DRAWCARDLIB 0x10008370
undefined * FUN_10008370(int param_1,char *param_2,int param_3)

{
  size_t sVar1;
  undefined4 uVar2;
  undefined1 local_41c [264];
  char local_314 [260];
  char local_210 [256];
  char local_110 [256];
  int local_10;
  undefined *local_c;
  char *local_8;
  
  local_c = &DAT_1003a9b8;
  EnterCriticalSection((LPCRITICAL_SECTION)&DAT_10124520);
  _splitpath(param_2,(char *)0x0,local_314,local_210,local_110);
  if (DAT_1003a9ac == 0) {
    strcpy(local_41c,local_314);
    strcat(local_41c,s_SmallArt_cat_1002240c);
    DAT_100223fc = FUN_1000b820(local_41c);
    strcpy(local_41c,local_314);
    strcat(local_41c,s_MedArt_cat_1002241c);
    DAT_10022400 = FUN_1000b820(local_41c);
    DAT_1003a9ac = 1;
  }
  sVar1 = strlen(local_314);
  local_8 = local_314 + sVar1;
  if (param_1 == 0) {
    DAT_100f1f68 = DAT_100223fc;
  }
  else {
    if (param_1 != 1) {
      return (undefined *)0x0;
    }
    DAT_100f1f68 = DAT_10022400;
  }
  strcpy(local_314,local_210);
  strcat(local_314,local_110);
  _strlwr(local_314);
  if (local_c != (undefined *)0x0) {
    memset(local_c,0,0x1b0);
    strcpy(local_c + 0x9c,param_2);
    *(undefined **)(local_c + 0x1a0) = &DAT_100b5368;
    local_10 = FUN_1000bbf1(DAT_100f1f68,local_314,local_c + 0x1a0);
    if (local_10 == -1) {
      strcat(param_2,&DAT_10022428);
      OutputDebugStringA(param_2);
      LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10124520);
      local_c = (undefined *)0x0;
    }
    else {
      *(int *)(local_c + 0x1a4) = local_10 + -0x9c;
      memcpy(local_c,*(void **)(local_c + 0x1a0),0x9c);
      *(int *)(local_c + 0x1a0) = *(int *)(local_c + 0x1a0) + 0x9c;
      if (*(int *)(local_c + 0x28) == 4) {
        *(int *)(local_c + 0x1c) = *(int *)(local_c + 0x1c) << 1;
        *(int *)(local_c + 0x20) = *(int *)(local_c + 0x20) << 1;
      }
      if (param_3 != 0) {
        uVar2 = FUN_10008663(local_c,0);
        *(undefined4 *)(local_c + 0x1ac) = uVar2;
        if (*(int *)(local_c + 0x1ac) == 0) {
          FUN_10008635(local_c);
          LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10124520);
          local_c = (undefined *)0x0;
        }
        else {
          *(undefined4 *)(local_c + 0x1a8) = 1;
          LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10124520);
        }
      }
    }
  }
  return local_c;
}

// FUNCTION: DRAWCARDLIB 0x10008635
undefined4 FUN_10008635(void)

{
  LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_10124520);
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x10008663
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void * FUN_10008663(int *param_1,void *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  void *pvVar5;
  int iVar6;
  int iVar7;
  bool bVar8;
  void *local_50;
  int local_4c;
  int local_48;
  int local_34;
  int local_2c;
  int local_8;
  
  if (DAT_1003a9b0 == 0) {
    for (local_4c = -0x400; local_4c < 0x401; local_4c = local_4c + 1) {
      if ((local_4c < 0) || (0xf8 < local_4c)) {
        if (local_4c < 10) {
          PTR_DAT_100223f8[local_4c] = 0;
        }
        else {
          PTR_DAT_100223f8[local_4c] = 0xff;
        }
      }
      else {
        PTR_DAT_100223f8[local_4c] = (char)((local_4c * 0xff) / 0xf8);
      }
    }
    DAT_1003a9b0 = 1;
  }
  bVar8 = param_2 != (void *)0x0;
  if (bVar8) {
    FUN_10002ea0(param_2,(int)(param_1[0x24] + 2000 + (param_1[0x24] + 2000 >> 0x1f & 3U)) >> 2);
  }
  else {
    param_2 = malloc(param_1[0x24] + 2000);
    FUN_10002ea0(param_2,(int)(param_1[0x24] + 2000 + (param_1[0x24] + 2000 >> 0x1f & 3U)) >> 2);
  }
  _DAT_10126554 = FUN_10009338(param_2,param_1);
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
    FUN_1000c0f0(0,s_D__Newmagic_sources_NedCard_haar_10022454,0x15e,
                 s_wavelet_pieces_has_illegal_value_1002242c,param_1[10]);
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
    pvVar5 = (void *)((iVar3 * iVar3 + local_2c * local_2c * 2 + 0x40) * local_48 * 4 + (int)param_2
                     );
    iVar6 = (int)pvVar5 + iVar3 * iVar3 * 4 + 0x80;
    iVar7 = local_2c * local_2c * 4 + iVar6 + 0x80;
    FUN_10008d5c(pvVar5,iVar3,iVar2);
    FUN_10008d5c(iVar6,local_2c,iVar2);
    FUN_10008d5c(iVar7,local_2c,iVar2);
    if (local_48 < param_1[10] / 2) {
      local_50 = (void *)FUN_1000906f(&DAT_1003ab68,pvVar5,iVar3,iVar3,iVar6,iVar7,local_2c,local_2c
                                      ,*param_1);
    }
    else if (param_1[10] < 2) {
      local_50 = (void *)FUN_1000906f(&DAT_1003ab68,pvVar5,iVar3,iVar4,iVar6,iVar7,local_2c,local_34
                                      ,*param_1);
    }
    else {
      local_50 = (void *)FUN_1000906f(&DAT_1003ab68,pvVar5,iVar3,param_1[8] - iVar3,iVar6,iVar7,
                                      local_2c,local_34,*param_1);
    }
    if (param_1[10] < 2) {
      if (!bVar8) {
        FUN_1000cde0(param_2);
      }
      param_2 = local_50;
    }
    else {
      FUN_10008ab5(param_2,local_50,(local_48 % local_8) * (iVar1 / local_8),
                   (local_48 / local_8) * (iVar1 / local_8),iVar3,iVar3,iVar1,iVar1);
      FUN_1000cde0(local_50);
    }
  }
  return param_2;
}

// FUNCTION: DRAWCARDLIB 0x10008ab5
void FUN_10008ab5(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6,
                 int param_7)

{
  undefined4 local_8;
  
  param_1 = param_1 + (param_7 * param_4 + param_3) * 3;
  for (local_8 = 0; local_8 < param_6; local_8 = local_8 + 1) {
    FUN_10008cb8(param_1,param_2,param_5 * 3);
    param_1 = param_1 + param_7 * 3;
    param_2 = param_2 + param_5 * 3;
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x10008b1c
void FUN_10008b1c(int *param_1,int *param_2,int *param_3,int param_4,int param_5,undefined4 param_6,
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

// FUNCTION: DRAWCARDLIB 0x10008cb8
void FUN_10008cb8(undefined8 *param_1,undefined8 *param_2,uint param_3)

{
  uint uVar1;
  
  for (uVar1 = param_3 >> 3; uVar1 != 0; uVar1 = uVar1 - 1) {
    *param_1 = *param_2;
    param_2 = param_2 + 1;
    param_1 = param_1 + 1;
  }
  param_3 = param_3 & 7;
  if (param_3 != 0) {
    for (; param_3 != 0; param_3 = param_3 - 1) {
      *(undefined1 *)param_1 = *(undefined1 *)param_2;
      param_2 = (undefined8 *)((int)param_2 + 1);
      param_1 = (undefined8 *)((int)param_1 + 1);
    }
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x10008cf3
void FUN_10008cf3(undefined8 *param_1,uint param_2,uint param_3)

{
  memset(param_1,(int)(byte)param_2,param_3);
  return;
}

// FUNCTION: DRAWCARDLIB 0x10008d5c
void FUN_10008d5c(int param_1,int param_2,int param_3)

{
  void *pvVar1;
  undefined4 local_18;
  undefined4 local_10;
  
  if (DAT_10022404 == 0) {
    local_18 = malloc(0x32000);
    DAT_10124544 = local_18;
    DAT_10124540 = malloc(0x32000);
    DAT_10022404 = 1;
  }
  else {
    local_18 = DAT_10124544;
  }
  pvVar1 = DAT_10124540;
  for (local_10 = param_3; local_10 < param_2; local_10 = local_10 << 1) {
    FUN_10008e99(param_1,local_10 * local_10 * 4 + param_1,local_18,local_10,local_10,local_10 * 2,
                 local_10);
    FUN_10008e99(local_10 * local_10 * 8 + param_1,local_10 * local_10 * 0xc + param_1,pvVar1,
                 local_10,local_10,local_10 * 2,local_10);
    FUN_10008f6e(local_18,pvVar1,param_1,local_10,local_10 * 2,local_10 * 2,local_10 * 2);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x10008e99
void FUN_10008e99(int *param_1,int *param_2,int *param_3,int param_4,int param_5,undefined4 param_6,
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

// FUNCTION: DRAWCARDLIB 0x10008f6e
void FUN_10008f6e(int *param_1,int *param_2,int *param_3,int param_4,int param_5,undefined4 param_6,
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

// FUNCTION: DRAWCARDLIB 0x10009049
undefined2 FUN_10009049(uint param_1,uint param_2)

{
  undefined1 *puVar1;
  undefined1 uVar2;
  undefined1 *puVar3;
  
  puVar3 = (undefined1 *)(param_1 & 0xffffffe0);
  param_2 = param_2 >> 6;
  do {
    uVar2 = *puVar3;
    puVar1 = puVar3 + 0x20;
    puVar3 = puVar3 + 0x40;
    param_2 = param_2 - 1;
  } while (param_2 != 0);
  return CONCAT11(*puVar1,uVar2);
}

// FUNCTION: DRAWCARDLIB 0x1000906f
undefined1 *
FUN_1000906f(undefined1 *param_1,int *param_2,int param_3,int param_4,int param_5,int param_6,
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
  
  if (DAT_100f1f6c == 0) {
    for (local_1c = -0x400; (int)local_1c < 0x1c00; local_1c = local_1c + 1) {
      if ((int)local_1c < 1) {
        PTR_DAT_10022408[local_1c] = 0;
      }
      else {
        iVar2 = (int)local_1c >> 2;
        if (0xfe < iVar2) {
          iVar2 = 0xff;
        }
        PTR_DAT_10022408[local_1c] = (char)iVar2;
      }
    }
    DAT_100f1f6c = 1;
  }
  if (param_1 == (undefined1 *)0x0) {
    param_1 = malloc(param_3 * param_3 * 3 + 0x10);
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
      *param_1 = PTR_DAT_10022408[local_8];
      param_1[1] = PTR_DAT_10022408
                   [((iVar2 * 2 - (iVar2 >> 2)) - (local_c >> 1)) -
                    ((local_8 >> 2) - (local_8 >> 4))];
      param_1[2] = PTR_DAT_10022408[local_c];
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

// FUNCTION: DRAWCARDLIB 0x10009338
undefined4 FUN_10009338(int param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  void *_Dst;
  void *_Dst_00;
  void *_Dst_01;
  int local_28;
  void *local_18;
  
  iVar4 = param_2[7] / (int)(2 - (uint)(param_2[10] == 1));
  iVar5 = iVar4 / (int)(2 - (uint)(*param_2 == 0));
  iVar2 = param_2[9];
  iVar3 = *(int *)param_2[0x68];
  piVar1 = (int *)param_2[0x68] + 1;
  *piVar1 = -0x80000000;
  iVar6 = FUN_1000352b(piVar1 + iVar3,piVar1,iVar3);
  local_18 = (void *)((int)(piVar1 + iVar3) + iVar6);
  for (local_28 = 0; local_28 < param_2[10]; local_28 = local_28 + 1) {
    _Dst = (void *)((iVar4 * iVar4 + iVar5 * iVar5 * 2 + 0x40) * local_28 * 4 + param_1);
    _Dst_00 = (void *)((int)_Dst + iVar4 * iVar4 * 4 + 0x80);
    _Dst_01 = (void *)((int)_Dst_00 + iVar5 * iVar5 * 4 + 0x80);
    memcpy(_Dst,local_18,iVar2 * iVar2 * 4);
    local_18 = (void *)((int)local_18 + iVar2 * iVar2 * 4);
    FUN_10003c3e((void *)((int)_Dst + iVar2 * iVar2 * 4),local_18,param_2[local_28 + 0x17]);
    local_18 = (void *)((int)local_18 + param_2[local_28 + 0x17]);
    memcpy(_Dst_00,local_18,iVar2 * iVar2 * 4);
    local_18 = (void *)((int)local_18 + iVar2 * iVar2 * 4);
    FUN_10003c3e((void *)((int)_Dst_00 + iVar2 * iVar2 * 4),local_18,param_2[local_28 + 0x1b]);
    local_18 = (void *)((int)local_18 + param_2[local_28 + 0x1b]);
    memcpy(_Dst_01,local_18,iVar2 * iVar2 * 4);
    local_18 = (void *)((int)local_18 + iVar2 * iVar2 * 4);
    FUN_10003c3e((void *)((int)_Dst_01 + iVar2 * iVar2 * 4),local_18,param_2[local_28 + 0x1f]);
    local_18 = (void *)((int)local_18 + param_2[local_28 + 0x1f]);
  }
  return 0;
}

// FUNCTION: DRAWCARDLIB 0x1000956f
bool FUN_1000956f(HWND param_1)

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

// FUNCTION: DRAWCARDLIB 0x100095d8
int FUN_100095d8(HWND param_1,void *param_2,int param_3,int param_4,DWORD param_5,DWORD param_6)

{
  BITMAPINFO *lpbmi;
  HDC hdc;
  int iVar1;
  
  lpbmi = (BITMAPINFO *)FUN_1000965b(param_5,param_6,0x18);
  hdc = GetDC(param_1);
  iVar1 = SetDIBitsToDevice(hdc,param_3,param_4,param_5,param_6,0,0,0,param_6,param_2,lpbmi,0);
  ReleaseDC(param_1,hdc);
  FUN_1000977f(lpbmi);
  return iVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000965b
undefined4 * FUN_1000965b(undefined4 param_1,int param_2,int param_3)

{
  int local_10;
  undefined4 *local_c;
  undefined4 *local_8;
  
  if (param_3 == 8) {
    local_8 = malloc(0x42c);
  }
  else if (param_3 == 0x18) {
    local_8 = malloc(0x2c);
  }
  else {
    local_8 = malloc(0x2c);
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

// FUNCTION: DRAWCARDLIB 0x1000977f
undefined4 FUN_1000977f(undefined4 param_1)

{
  FUN_1000cde0(param_1);
  return 1;
}

// FUNCTION: DRAWCARDLIB 0x100097a0
/* WARNING: Removing unreachable block (ram,0x10009a01) */
/* WARNING: Removing unreachable block (ram,0x10009a3b) */
/* WARNING: Removing unreachable block (ram,0x10009a0b) */
/* WARNING: Removing unreachable block (ram,0x10009902) */
/* WARNING: Removing unreachable block (ram,0x1000991f) */
/* WARNING: Removing unreachable block (ram,0x1000990c) */

undefined4 * FUN_100097a0(undefined4 *param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  int local_40;
  int local_3c;
  int local_38;
  int local_34;
  int local_2c;
  int *local_28;
  int local_24;
  
  local_38 = 0;
  local_24 = 0;
  if (param_2 == 0) {
    puVar3 = (undefined4 *)0x0;
  }
  else {
    iVar6 = (DAT_100223c0 - (param_3 * 3) % DAT_100223c0) % DAT_100223c0;
    if (param_1 == (undefined4 *)0x0) {
      param_1 = malloc((param_3 * 3 + iVar6) * param_4 + 4);
    }
    puVar3 = param_1;
    iVar5 = *(int *)(param_2 + 0x1c);
    iVar1 = *(int *)(param_2 + 0x20);
    if (*(int *)(param_2 + 0x1a8) == 0) {
      local_40 = FUN_10008663(param_2,0);
    }
    else {
      local_40 = *(int *)(param_2 + 0x1ac);
    }
    iVar2 = *(int *)(param_2 + 0x1c);
    piVar4 = malloc(param_3 << 2);
    local_2c = 0;
    local_28 = piVar4;
    for (local_34 = 0; local_34 < param_3; local_34 = local_34 + 1) {
      *local_28 = (local_2c >> 0xf & 0xfffffffeU) + (local_2c >> 0x10);
      local_2c = local_2c + (iVar5 << 0x10) / param_3;
      local_28 = local_28 + 1;
    }
    for (local_3c = 0; local_3c < param_4; local_3c = local_3c + 1) {
      iVar5 = iVar2 * 3 * (local_38 >> 0x10) + local_40;
      if (local_24 == iVar5) {
        memcpy(param_1,(void *)((int)param_1 + (param_3 * -3 - iVar6)),param_3 * 3);
        param_1 = (undefined4 *)((int)param_1 + param_3 * 3);
      }
      else {
        local_28 = piVar4;
        for (local_34 = 0; local_24 = iVar5, local_34 < param_3; local_34 = local_34 + 1) {
          *param_1 = *(undefined4 *)(*local_28 + iVar5);
          param_1 = (undefined4 *)((int)param_1 + 3);
          local_28 = local_28 + 1;
        }
      }
      local_38 = local_38 + (iVar1 << 0x10) / param_4;
      param_1 = (undefined4 *)((int)param_1 + iVar6);
    }
    FUN_1000cde0(piVar4);
    if (*(int *)(param_2 + 0x1a8) == 0) {
      FUN_1000cde0(local_40);
    }
  }
  return puVar3;
}

// FUNCTION: DRAWCARDLIB 0x10009a95
bool FUN_10009a95(undefined4 param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  
  if (param_2 != 0) {
    uVar1 = FUN_100097a0(0,param_2,param_3,param_4);
    FUN_100095d8(param_1,uVar1,0,0,param_3,param_4);
    FUN_1000cde0(uVar1);
  }
  return param_2 != 0;
}

// FUNCTION: DRAWCARDLIB 0x10009aff
/* WARNING: Removing unreachable block (ram,0x10009ef0) */
/* WARNING: Removing unreachable block (ram,0x10009ccd) */

char * FUN_10009aff(char *param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  char *local_5054;
  uint local_5050 [4096];
  int local_1050;
  int local_104c;
  int local_1048;
  int local_1044;
  uint local_1040;
  int local_103c;
  uint *local_1038;
  int local_1034;
  uint *local_1030;
  undefined4 local_102c;
  char *local_1028;
  int local_1024;
  int local_1020;
  size_t local_101c;
  int local_1018;
  undefined4 local_1014;
  byte *local_1010;
  uint local_100c [1016];
  undefined4 uStackY_2c;
  int iStackY_28;
  undefined4 uStackY_24;
  char *pcStackY_20;
  
  MSVC_StackProbe();
  local_1034 = 0;
  local_1048 = 0;
  local_102c = 0;
  local_1030 = local_5050;
  local_1038 = local_100c;
  local_1040 = (uint)(param_1 != (char *)0x0);
  if (param_2 == 0) {
    local_5054 = (char *)0x0;
  }
  else {
    local_1018 = *(int *)(param_2 + 0x1c) << 0x10;
    local_1024 = *(int *)(param_2 + 0x20) << 0x10;
    iVar1 = local_1018 / param_3;
    iVar2 = local_1024 / param_4;
    if (*(int *)(param_2 + 0x1a8) == 0) {
      local_1050 = (int)FUN_10008663((int *)param_2,(void *)0x0);
    }
    else {
      local_1050 = *(int *)(param_2 + 0x1ac);
    }
    local_103c = *(int *)(param_2 + 0x1c);
    local_1014 = 0;
    local_1020 = (DAT_100223c0 - (param_3 * 3) % DAT_100223c0) % DAT_100223c0;
    if (param_1 == (char *)0x0) {
      local_5054 = &DAT_100b5368;
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
    if (*(int *)(param_2 + 0x20) < param_4) {
      param_1 = local_5054 + (param_4 - *(int *)(param_2 + 0x20)) * (param_3 * 3 + local_1020);
    }
    local_1028 = param_1;
    for (local_104c = 0; local_104c < *(int *)(param_2 + 0x20); local_104c = local_104c + 1) {
      local_1030 = local_5050;
      for (local_1044 = 0; local_1044 < param_3; local_1044 = local_1044 + 1) {
        local_1010 = (byte *)(((int)*local_1030 >> 8) * 3 + local_103c * 3 * local_104c + local_1050
                             );
        *param_1 = (char)(((uint)local_1010[3] - (uint)*local_1010) * (*local_1030 & 0xff) >> 8) +
                   *local_1010;
        param_1[1] = (char)(((uint)local_1010[4] - (uint)local_1010[1]) * (*local_1030 & 0xff) >> 8)
                     + local_1010[1];
        param_1[2] = (char)(((uint)local_1010[5] - (uint)local_1010[2]) * (*local_1030 & 0xff) >> 8)
                     + local_1010[2];
        param_1 = param_1 + 3;
        local_1030 = local_1030 + 1;
      }
      param_1 = param_1 + local_1020;
    }
    local_1048 = 0;
    for (local_1044 = 0; local_1044 < param_4; local_1044 = local_1044 + 1) {
      *local_1038 = local_1048 >> 8;
      local_1048 = local_1048 + iVar2;
      local_1038 = local_1038 + 1;
    }
    local_101c = param_3 * 3 + local_1020;
    if (param_4 < *(int *)(param_2 + 0x20)) {
      pcStackY_20 = (char *)0x10009f57;
      memcpy
                (local_5050,local_5054 + (*(int *)(param_2 + 0x20) + -1) * local_101c,local_101c);
    }
    for (local_1044 = 0; local_1044 < param_3; local_1044 = local_1044 + 1) {
      param_1 = local_5054 + local_1044 * 3;
      local_1038 = local_100c;
      for (local_104c = 0; local_104c < param_4 + -1; local_104c = local_104c + 1) {
        iVar1 = *(int *)(param_2 + 0x20) + -2;
        if ((int)*local_1038 >> 8 <= iVar1) {
          iVar1 = (int)*local_1038 >> 8;
        }
        local_1010 = (byte *)(local_1028 + iVar1 * local_101c + local_1044 * 3);
        *param_1 = (char)(((uint)local_1010[local_101c] - (uint)*local_1010) * (*local_1038 & 0xff)
                         >> 8) + *local_1010;
        param_1[1] = (char)(((uint)local_1010[local_101c + 1] - (uint)local_1010[1]) *
                            (*local_1038 & 0xff) >> 8) + local_1010[1];
        param_1[2] = (char)(((uint)local_1010[local_101c + 2] - (uint)local_1010[2]) *
                            (*local_1038 & 0xff) >> 8) + local_1010[2];
        local_1038 = local_1038 + 1;
        param_1 = param_1 + local_101c;
      }
    }
    if (param_4 < *(int *)(param_2 + 0x20)) {
      pcStackY_20 = (char *)0x1000a11a;
      memcpy(local_5054 + (param_4 + -1) * local_101c,local_5050,local_101c);
    }
    else {
      pcStackY_20 = (char *)0x1000a142;
      memset(local_5054 + (param_4 + -1) * local_101c,0,local_101c);
    }
    if (DAT_10021048 == 0) {
      pcStackY_20 = local_5054;
      uStackY_24 = 0x1000a19b;
      FUN_10001b41((undefined4 *)local_5054,param_4,param_3,local_1020);
    }
    else if (DAT_10124538 == 0x10) {
      pcStackY_20 = local_5054;
      uStackY_24 = DAT_1002104c;
      iStackY_28 = DAT_10021048;
      uStackY_2c = 0x1000a1d7;
      FUN_10002877(DAT_10021048,DAT_1002104c,(int)local_5054,param_4,param_3,local_1020);
    }
    else if (DAT_10124538 == 8) {
      pcStackY_20 = local_5054;
      uStackY_24 = DAT_1002104c;
      iStackY_28 = DAT_10021048;
      uStackY_2c = 0x1000a213;
      FUN_10001cbf(DAT_10021048,DAT_1002104c,(int)local_5054,param_4,param_3,local_1020);
    }
  }
  return local_5054;
}

// FUNCTION: DRAWCARDLIB 0x1000a226
undefined4 FUN_1000a226(undefined4 param_1,int param_2,int param_3,undefined4 param_4)

{
  undefined4 uVar1;
  int iVar2;
  
  if (param_2 == 0) {
    uVar1 = 0;
  }
  else {
    iVar2 = FUN_10009aff(0,param_2,param_3,param_4);
    FUN_10001cbf(DAT_10021048,DAT_1002104c,iVar2,param_4,param_3,
                 (DAT_100223c0 - (param_3 * 3) % DAT_100223c0) % DAT_100223c0);
    FUN_100095d8(param_1,iVar2,0,0,param_3,param_4);
    if (*(int *)(param_2 + 0x1ac) != iVar2) {
      FUN_1000cde0(iVar2);
    }
    uVar1 = 1;
  }
  return uVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000a2e7
undefined4 * FUN_1000a2e7(int param_1)

{
  undefined4 *local_8;
  
  local_8 = malloc(param_1 + 8);
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

// FUNCTION: DRAWCARDLIB 0x1000a339
void FUN_1000a339(int param_1)

{
  undefined4 local_8;
  
  if (*(int *)(param_1 + -4) == 0) {
    local_8 = param_1 + -8;
  }
  else {
    local_8 = param_1 + -4;
  }
  FUN_1000cde0(local_8);
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000a373
int FUN_1000a373(HWND param_1,int param_2,int param_3,int param_4,DWORD param_5,DWORD param_6)

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
  _Dst = malloc(param_5 * param_6 * 3 + 8);
  memset(_Dst,0,param_5 * param_6 * 3);
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
  lpbmi = (BITMAPINFO *)FUN_1000965b(param_5,param_6,0x18);
  hdc = GetDC(param_1);
  iVar2 = SetDIBitsToDevice(hdc,param_3,param_4,param_5,param_6,0,0,0,param_6,_Dst,lpbmi,0);
  ReleaseDC(param_1,hdc);
  FUN_1000977f(lpbmi);
  FUN_1000cde0(_Dst);
  return iVar2;
}

// FUNCTION: DRAWCARDLIB 0x1000a520
void FUN_1000a520(undefined4 *param_1,undefined4 param_2,int param_3)

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

// FUNCTION: DRAWCARDLIB 0x1000a5a0
bool FUN_1000a5a0(void)

{
  int iVar1;
  bool bVar2;
  
  iVar1 = FUN_1000acf5();
  if (DAT_10022500 == 0) {
    FUN_1000a6ca(10,10,&DAT_10022500,0,&DAT_100f1f74,0,0);
    InitializeCriticalSection((LPCRITICAL_SECTION)&DAT_100f2378);
  }
  bVar2 = DAT_10022500 != 0;
  if (!bVar2 || iVar1 == 0) {
    FUN_1000a628();
  }
  return bVar2 && iVar1 != 0;
}

// FUNCTION: DRAWCARDLIB 0x1000a628
void FUN_1000a628(void)

{
  if (DAT_10022500 != 0) {
    FUN_1000a852(DAT_10022500,DAT_100f1f74);
    DAT_10022500 = 0;
    DeleteCriticalSection((LPCRITICAL_SECTION)&DAT_100f2378);
  }
  if (DAT_100f1f70 != 0) {
    FUN_1000afe6();
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000a67b
void FUN_1000a67b(HDC param_1)

{
  SelectPalette(param_1,DAT_100f1f70,0);
  RealizePalette(param_1);
  GdiFlush();
  SetDIBColorTable(param_1,0,0x100,(RGBQUAD *)&DAT_100f1f78);
  SetStretchBltMode(param_1,3);
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000a6ca
undefined4
FUN_1000a6ca(undefined4 param_1,undefined4 param_2,undefined4 *param_3,BITMAPINFO *param_4,
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
      FUN_1000a67b(hdc);
      local_3c = CreateCompatibleDC(hdc);
      if (local_3c != (HDC)0x0) {
        FUN_1000a520(param_4,param_1,param_2);
        local_38.bmiHeader.biBitCount = 0x20;
        local_44 = CreateDIBSection(hdc,param_4,0,&local_8,(HANDLE)0x0,0);
        local_c = SelectObject(local_3c,local_44);
        FUN_1000a67b(local_3c);
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

// FUNCTION: DRAWCARDLIB 0x1000a852
void FUN_1000a852(HDC param_1,HGDIOBJ param_2)

{
  if (param_1 != (HDC)0x0) {
    DeleteDC(param_1);
  }
  if (param_2 != (HGDIOBJ)0x0) {
    DeleteObject(param_2);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000a885
undefined4 FUN_1000a885(int param_1,int param_2,HANDLE param_3)

{
  undefined4 uVar1;
  undefined1 local_1c [4];
  undefined4 local_18;
  undefined4 local_14;
  
  if (((param_1 == 0) || (param_2 == 0)) || (param_3 == (HANDLE)0x0)) {
    uVar1 = 0;
  }
  else {
    GetObjectA(param_3,0x18,local_1c);
    uVar1 = FUN_1000a8ed(param_1,param_2,param_3,0,0,local_18,local_14);
  }
  return uVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000a8ed
undefined4
FUN_1000a8ed(HDC param_1,int *param_2,HANDLE param_3,int param_4,int param_5,int param_6,int param_7
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
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_100f2378);
    h = SelectObject(DAT_10022500,param_3);
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
    FUN_1000a67b(DAT_10022500);
    if (param_7 <= local_20) {
      local_20 = param_7;
    }
    if (param_6 <= local_24) {
      local_24 = param_6;
    }
    StretchBlt(param_1,local_8,local_c,local_10,local_2c,DAT_10022500,param_4,param_5,local_24,
               local_20,0xcc0020);
    SelectObject(DAT_10022500,h);
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_100f2378);
    uVar1 = 1;
  }
  return uVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000aa37
undefined4 FUN_1000aa37(undefined4 param_1,undefined4 param_2,HANDLE param_3)

{
  undefined4 uVar1;
  undefined1 local_34 [4];
  int local_30;
  undefined4 local_2c;
  undefined4 local_1c;
  int local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  
  GetObjectA(param_3,0x18,local_34);
  local_18 = local_30 / 2;
  local_1c = local_2c;
  local_10 = 0;
  local_c = 0;
  local_14 = 0;
  local_8 = local_18;
  uVar1 = FUN_1000aab9(param_1,param_2,param_3,local_18,local_2c,0,0,local_18,0);
  return uVar1;
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
    EnterCriticalSection((LPCRITICAL_SECTION)&DAT_100f2378);
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
    FUN_1000a67b(DAT_10022500);
    StretchBlt(param_1,local_8,local_c,local_14,local_30,DAT_10022500,param_8,param_9,param_4,
               param_5,0x8800c6);
    FUN_1000a67b(DAT_10022500);
    StretchBlt(param_1,local_8,local_c,local_14,local_30,DAT_10022500,param_6,param_7,param_4,
               param_5,0xee0086);
    RestoreDC(param_1,local_10);
    LeaveCriticalSection((LPCRITICAL_SECTION)&DAT_100f2378);
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

// FUNCTION: DRAWCARDLIB 0x1000acf5
undefined4 FUN_1000acf5(void)

{
  UINT UVar1;
  PALETTEENTRY local_628;
  undefined1 local_624 [264];
  undefined4 local_51c;
  UINT local_514;
  undefined1 local_510 [264];
  LOGPALETTE *local_408;
  PALETTEENTRY local_404 [256];
  
  local_51c = 1;
  strcpy(local_624,&DAT_100f35b0);
  strcat(local_624,s__DUELPALall_TR_10022508);
  strcpy(local_510,&DAT_100f35b0);
  strcat(local_510,s__DUEL_plogpal_10022518);
  local_408 = (LOGPALETTE *)FUN_10001033(local_624,local_510);
  if (local_408 == (LOGPALETTE *)0x0) {
    local_51c = 0;
  }
  else {
    for (local_514 = 1; (int)local_514 < 0xff; local_514 = local_514 + 1) {
      local_408->palPalEntry[local_514].peFlags = '\x04';
    }
    DAT_100f1f70 = CreatePalette(local_408);
    if (DAT_100f1f70 == (HPALETTE)0x0) {
      local_51c = 0;
    }
    else {
      local_628.peRed = 0xff;
      local_628.peGreen = 0xff;
      local_628.peBlue = 0xff;
      local_628.peFlags = '\0';
      SetPaletteEntries(DAT_100f1f70,0xff,1,&local_628);
      local_628.peRed = 0xfe;
      local_628.peGreen = 0xfe;
      local_628.peBlue = 0xfe;
      local_628.peFlags = '\x04';
      SetPaletteEntries(DAT_100f1f70,0xbf,1,&local_628);
      for (local_514 = 0xec; (int)local_514 < 0xff; local_514 = local_514 + 1) {
        local_628.peRed = '\x01';
        local_628.peGreen = '\x01';
        local_628.peBlue = '\x01';
        local_628.peFlags = '\x04';
        SetPaletteEntries(DAT_100f1f70,local_514,1,&local_628);
      }
      UVar1 = GetPaletteEntries(DAT_100f1f70,0,0x100,local_404);
      for (local_514 = 0; (int)local_514 < (int)UVar1; local_514 = local_514 + 1) {
        (&DAT_100f1f78)[local_514 * 4] = local_404[local_514].peBlue;
        (&DAT_100f1f79)[local_514 * 4] = local_404[local_514].peGreen;
        (&DAT_100f1f7a)[local_514 * 4] = local_404[local_514].peRed;
        (&DAT_100f1f7b)[local_514 * 4] = 0;
      }
      while (local_514 = UVar1, (int)local_514 < 0x100) {
        (&DAT_100f1f78)[local_514 * 4] = 0;
        (&DAT_100f1f79)[local_514 * 4] = 0;
        (&DAT_100f1f7a)[local_514 * 4] = 0;
        (&DAT_100f1f7b)[local_514 * 4] = 0;
        UVar1 = local_514 + 1;
      }
    }
  }
  return local_51c;
}

// FUNCTION: DRAWCARDLIB 0x1000afe6
void FUN_1000afe6(void)

{
  DeleteObject(DAT_100f1f70);
  DAT_100f1f70 = (HGDIOBJ)0x0;
  FUN_10002db9();
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000b00c
uint FUN_1000b00c(int param_1)

{
  return CONCAT12((&DAT_100f1f78)[param_1 * 4],
                  CONCAT11((&DAT_100f1f79)[param_1 * 4],(&DAT_100f1f7a)[param_1 * 4])) | 0x2000000;
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

// FUNCTION: DRAWCARDLIB 0x1000b1e0
undefined * FUN_1000b1e0(int param_1,int param_2,int param_3)

{
  int iVar1;
  DWORD dwMaximumSizeLow;
  undefined4 uVar2;
  HANDLE pvVar3;
  HDC pHVar4;
  HBITMAP pHVar5;
  undefined *puVar6;
  uint uVar7;
  
  *(int *)(PTR_DAT_10022528 + 0x20) = param_1;
  *(int *)(PTR_DAT_10022528 + 0x24) = param_2;
  *(int *)(PTR_DAT_10022528 + 0x28) = param_3;
  iVar1 = param_3 * param_1 + (param_3 * param_1 >> 0x1f & 7U);
  uVar7 = iVar1 >> 0x1f;
  if (((iVar1 >> 3 ^ uVar7) - uVar7 & 3 ^ uVar7) == uVar7) {
    *(undefined4 *)(PTR_DAT_10022528 + 0x2c) = 0;
  }
  else {
    iVar1 = param_3 * param_1 + (param_3 * param_1 >> 0x1f & 7U);
    uVar7 = iVar1 >> 0x1f;
    *(uint *)(PTR_DAT_10022528 + 0x2c) = 4 - (((iVar1 >> 3 ^ uVar7) - uVar7 & 3 ^ uVar7) - uVar7);
  }
  iVar1 = (param_1 + *(int *)(PTR_DAT_10022528 + 0x2c)) * param_3 * param_2;
  dwMaximumSizeLow = ((int)(iVar1 + (iVar1 >> 0x1f & 7U)) >> 3) + 0x10;
  *(DWORD *)(PTR_DAT_10022528 + 0x1c) = dwMaximumSizeLow;
  uVar2 = FUN_1000965b(param_1,param_2,param_3);
  *(undefined4 *)(PTR_DAT_10022528 + 0x10) = uVar2;
  if (*(int *)(PTR_DAT_10022528 + 0x10) == 0) {
    puVar6 = (undefined *)0x0;
  }
  else {
    pvVar3 = CreateFileMappingA((HANDLE)0xffffffff,(LPSECURITY_ATTRIBUTES)0x0,0x8000004,0,
                                dwMaximumSizeLow,(LPCSTR)0x0);
    *(HANDLE *)PTR_DAT_10022528 = pvVar3;
    if (*(int *)PTR_DAT_10022528 == 0) {
      FUN_1000977f(*(undefined4 *)(PTR_DAT_10022528 + 0x10));
      puVar6 = (undefined *)0x0;
    }
    else {
      pHVar4 = GetDC((HWND)0x0);
      *(HDC *)(PTR_DAT_10022528 + 4) = pHVar4;
      FUN_1000a67b(*(undefined4 *)(PTR_DAT_10022528 + 4));
      pHVar5 = CreateDIBSection(*(HDC *)(PTR_DAT_10022528 + 4),
                                *(BITMAPINFO **)(PTR_DAT_10022528 + 0x10),(uint)(param_3 == 8),
                                (void **)(PTR_DAT_10022528 + 0x18),*(HANDLE *)PTR_DAT_10022528,0);
      *(HBITMAP *)(PTR_DAT_10022528 + 8) = pHVar5;
      ReleaseDC((HWND)0x0,*(HDC *)(PTR_DAT_10022528 + 4));
      if (*(int *)(PTR_DAT_10022528 + 8) == 0) {
        FUN_1000977f(*(undefined4 *)(PTR_DAT_10022528 + 0x10));
        CloseHandle(*(HANDLE *)PTR_DAT_10022528);
        puVar6 = (undefined *)0x0;
      }
      else {
        FUN_1000977f(*(undefined4 *)(PTR_DAT_10022528 + 0x10));
        puVar6 = PTR_DAT_10022528;
      }
    }
  }
  return puVar6;
}

// FUNCTION: DRAWCARDLIB 0x1000b40f
undefined4
FUN_1000b40f(int param_1,undefined4 param_2,undefined4 param_3,char *param_4,undefined1 *param_5)

{
  char *_Str2;
  int iVar1;
  int local_414;
  int local_410;
  undefined1 local_40c [1024];
  int local_c;
  int local_8;
  
  local_8 = 8;
  _Str2 = strchr(param_4,0x2e);
  iVar1 = strcmpi(DAT_1002253c,_Str2);
  if (iVar1 == 0) {
    gPcxInFile = fopen(param_4,DAT_10022544);
    if (gPcxInFile == (FILE *)0x0) {
      return 0;
    }
    gPcxPath = param_4;
    if (param_5 == (undefined1 *)0x1) {
      param_5 = local_40c;
    }
    if (param_5 == (undefined1 *)0x0) {
      PcxReadHeaderAndPalette(0);
      if (param_1 < 0) {
        gImageHeight = 0;
      }
      if ((int)gImageWidth % 3 == 0) {
        local_410 = 0;
      }
      else {
        local_410 = 4 - (int)gImageWidth % 3;
      }
      DAT_100f2394 = gImageWidth + local_410;
      FUN_1000b1e0(DAT_100f2394,gImageHeight,local_8);
      local_c = *(int *)(PTR_DAT_10022528 + 0x18);
      for (DAT_100f239c = 0; DAT_100f239c < gImageHeight; DAT_100f239c = DAT_100f239c + 1) {
        PcxReadScanlineRle(local_c);
        local_c = local_c + ((int)(local_8 + (local_8 >> 0x1f & 7U)) >> 3) * DAT_100f2394;
      }
      fclose(gPcxInFile);
    }
    else {
      PcxReadHeaderAndPalette(param_5 + 6);
      *param_5 = 0x4d;
      param_5[1] = 0x31;
      *(undefined2 *)(param_5 + 2) = 0x300;
      param_5[4] = 0;
      param_5[5] = 0xff;
    }
  }
  else {
    DAT_100f23a0 = FUN_1000b745(param_4,0x8000);
    if (DAT_100f23a0 == -1) {
      return 0;
    }
    FUN_1000b7a2(DAT_100f23a0);
    FUN_10155000(0,0,(ushort *)param_5);
    if ((gImageWidth & 3) == 0) {
      local_414 = 0;
    }
    else {
      local_414 = 4 - (gImageWidth & 3);
    }
    DAT_100f2394 = gImageWidth + local_414;
    iVar1 = FUN_1000b1e0(gImageWidth,gImageHeight,local_8);
    if (iVar1 == 0) {
      *(undefined4 *)(PTR_DAT_10022528 + 8) = 0;
    }
    else {
      local_c = *(int *)(PTR_DAT_10022528 + 0x18);
      DAT_100f239c = 0;
      while (DAT_100f239c < gImageHeight) {
        FUN_10155484(local_c,gImageWidth);
        DAT_100f239c = DAT_100f239c + 1;
        local_c = local_c + *(int *)(PTR_DAT_10022528 + 0x2c) +
                            ((int)(gImageWidth * local_8 +
                                  ((int)(gImageWidth * local_8) >> 0x1f & 7U)) >> 3);
      }
    }
    FUN_1000b778(DAT_100f23a0);
  }
  return *(undefined4 *)(PTR_DAT_10022528 + 8);
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x1000b700
int FUN_1000b700(undefined4 param_1)
{
  int iVar1;
  
  iVar1 = FUN_1000b40f(0,0,0,param_1,0);
  if (iVar1 != 0) {
    CloseHandle(*(HANDLE *)PTR_DAT_10022528);
  }
  return iVar1;
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x1000b745
int FUN_1000b745(char *param_1,int param_2)
{
  int iVar1;
  
  iVar1 = _open(param_1,param_2);
  _DAT_100f23a8 = 0xffffffff;
  return iVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000b778
void FUN_1000b778(int param_1)

{
  if (param_1 != DAT_10022530) {
    _close(param_1);
  }
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000b7a2
void FUN_1000b7a2(undefined4 param_1)

{
  DAT_100f23a4 = param_1;
  DAT_100f3394 = PTR_DAT_1002252c;
  DAT_100f35a0 = (void (__cdecl *)())FUN_1000b7c9;
  return;
}

// MATCHING
// FUNCTION: DRAWCARDLIB 0x1000b7c9
int FUN_1000b7c9(void)
{
  int iVar1;
  
  iVar1 = _read(DAT_100f23a4,DAT_100f33a0,0x200);
  DAT_100f3394 = (ushort *)DAT_100f33a0;
  return iVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000b804
void FUN_1000b804(void)
{
  return;
}


// FUNCTION: DRAWCARDLIB 0x1000b80f
void FUN_1000b80f(void) 
{ 
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000b820
int FUN_1000b820(char *param_1)

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
LAB_1000b874:
      FUN_1000c0f0(local_c != -1,s_D__Newmagic_sources_NedCard_Cata_10026998,0x43,
                   s_Too_many_open_Catalogs__Max__d_10026978,5);
      iVar1 = local_c * 0x114;
      *(undefined4 *)(&DAT_100f2e3c + iVar1) = 0;
      strcpy(iVar1 + 0x100f2e40,param_1);
      pFVar3 = fopen(param_1,DAT_100269c0);
      *(FILE **)(&DAT_100f2e30 + iVar1) = pFVar3;
      pFVar3 = *(FILE **)(&DAT_100f2e30 + iVar1);
      if (pFVar3 == (FILE *)0x0) {
        local_c = 0;
      }
      else {
        fread(&DAT_100f2e34 + iVar1,4,1,pFVar3);
        pvVar4 = malloc(*(int *)(&DAT_100f2e34 + iVar1) * 0xc);
        *(void **)(&DAT_100f2e38 + iVar1) = pvVar4;
        fread(*(void **)(&DAT_100f2e38 + iVar1),0xc,*(size_t *)(&DAT_100f2e34 + iVar1),pFVar3);
        if (DAT_10026974 != 0) {
          for (local_10 = 0; local_10 < 5; local_10 = local_10 + 1) {
            if ((local_c != local_10) && (*(int *)(&DAT_100f2e30 + local_10 * 0x114) != 0)) {
              iVar2 = local_10 * 0x114;
              local_1c = 0;
              while (*(int *)(&DAT_100f2e34 + iVar1) != 0) {
                for (local_20 = 0; local_20 < *(int *)(&DAT_100f2e34 + iVar2);
                    local_20 = local_20 + 1) {
                  FUN_1000c0f0(*(int *)(*(int *)(&DAT_100f2e38 + iVar1) + local_1c * 0xc) !=
                               *(int *)(*(int *)(&DAT_100f2e38 + iVar2) + local_1c * 0xc),
                               s_D__Newmagic_sources_NedCard_Cata_10026a20,0x69,
                               s_Duplicate_short_name_found_in_ca_100269c4,iVar1 + 0x100f2e40,
                               local_1c,iVar2 + 0x100f2e40,local_20,
                               *(undefined4 *)(*(int *)(&DAT_100f2e38 + iVar1) + local_1c * 0xc));
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
    if (*(int *)(&DAT_100f2e30 + local_10 * 0x114) == 0) {
      local_c = local_10;
      goto LAB_1000b874;
    }
    local_10 = local_10 + 1;
  } while( true );
}

// FUNCTION: DRAWCARDLIB 0x1000ba6a
bool FUN_1000ba6a(int param_1)

{
  int iVar1;
  
  param_1 = param_1 + -1;
  iVar1 = *(int *)(&DAT_100f2e30 + param_1 * 0x114);
  if (iVar1 != 0) {
    FUN_1000cde0(*(undefined4 *)(&DAT_100f2e38 + param_1 * 0x114));
    fclose(*(FILE **)(&DAT_100f2e30 + param_1 * 0x114));
    *(undefined4 *)(&DAT_100f2e38 + param_1 * 0x114) = 0;
    *(undefined4 *)(&DAT_100f2e30 + param_1 * 0x114) = 0;
    *(undefined4 *)(&DAT_100f2e34 + param_1 * 0x114) = 0;
  }
  return iVar1 != 0;
}

// FUNCTION: DRAWCARDLIB 0x1000bb26
undefined4 FUN_1000bb26(int *param_1,int *param_2)

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

// FUNCTION: DRAWCARDLIB 0x1000bb76
void * FUN_1000bb76(int param_1,undefined4 param_2)

{
  void *pvVar1;
  int local_8;
  
  local_8 = FUN_1000bc9a(param_2);
  if ((*(int *)(param_1 + 0xc) == 0) || (**(int **)(param_1 + 0xc) != local_8)) {
    pvVar1 = bsearch(&local_8,*(void **)(param_1 + 8),*(size_t *)(param_1 + 4),0xc,FUN_1000bb26);
    *(void **)(param_1 + 0xc) = pvVar1;
  }
  else {
    pvVar1 = *(void **)(param_1 + 0xc);
  }
  return pvVar1;
}

// FUNCTION: DRAWCARDLIB 0x1000bbf1
size_t FUN_1000bbf1(int param_1,undefined4 param_2,int *param_3)

{
  undefined4 *puVar1;
  int iVar2;
  size_t sVar3;
  void *pvVar4;
  
  puVar1 = (undefined4 *)(&DAT_100f2e30 + (param_1 + -1) * 0x114);
  iVar2 = FUN_1000bb76(puVar1,param_2);
  if (iVar2 == 0) {
    sVar3 = 0xffffffff;
  }
  else {
    if (*param_3 == 0) {
      pvVar4 = malloc(*(int *)(iVar2 + 8) + 0x10);
      *param_3 = (int)pvVar4;
    }
    fseek((FILE *)*puVar1,*(long *)(iVar2 + 4),0);
    sVar3 = fread((void *)*param_3,1,*(size_t *)(iVar2 + 8),(FILE *)*puVar1);
  }
  return sVar3;
}

// FUNCTION: DRAWCARDLIB 0x1000bc9a
uint FUN_1000bc9a(byte *param_1)

{
  int iVar1;
  int local_138;
  char local_134 [16];
  char local_124 [260];
  undefined4 local_20;
  byte local_1c;
  byte local_1b;
  uint local_10;
  int local_8;
  
  local_10 = 3;
  local_20 = 0;
  local_138 = 0;
  local_8 = 0;
  _splitpath((char *)param_1,local_134,local_124,(char *)&local_1c,local_134);
  param_1 = &local_1c;
  strcat(&local_1c,local_134);
  while( true ) {
    iVar1 = (int)(char)*param_1;
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
  return (int)(char)(local_1b ^ local_1c) << 0x18 | local_8 * local_138 & 0xffffffU;
}

// FUNCTION: DRAWCARDLIB 0x1000bd90
uint FUN_1000bd90(undefined4 param_1,uint param_2,undefined4 param_3)

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
  
  MSVC_StackProbe();
  iVar2 = 1;
  _DAT_10124510 = param_1;
  InitializeCriticalSection((LPCRITICAL_SECTION)&DAT_10124520);
  GetModuleFileNameA((HMODULE)0x0,&DAT_100f35b0,0x105);
  pcVar1 = strrchr(&DAT_100f35b0,0x5c);
  *pcVar1 = '\0';
  strcpy((uint *)&DAT_100f36c0,(uint *)&DAT_100f35b0);
  strcat((uint *)&DAT_100f36c0,(uint *)s__CARDART_10026a48);
  DAT_10021048 = 3;
  DAT_10124538 = 0x10;
  FUN_1000a5a0();
  strcpy(acStackY_4a4a0,(uint *)&DAT_100f35b0);
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
      FUN_1000cde0(_DstBuf);
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
  FUN_1000a628();
  DeleteCriticalSection((LPCRITICAL_SECTION)&DAT_10124520);
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000c0f0
void FUN_1000c0f0(int ok,const char *file,int line,const char *fmt,...)
{
  FILE *log;
  time_t now;
  size_t used;
  va_list args;
  
  if (ok != 0) {
    return;
  }
  
  DAT_100f23e0[0] = '\0';
  if ((file != (const char *)0x0) && (line != 0)) {
    sprintf(DAT_100f23e0,s_File____s__Line____d_10026aa8,file,line);
  }
  used = strlen(DAT_100f23e0);
  va_start(args,fmt);
  _vsnprintf(DAT_100f23e0 + used,2000 - used,fmt,args);
  va_end(args);
  
  now = time((time_t *)0x0);
  log = fopen(s__assertFile_txt_10026a94,DAT_10026aa4);
  if (log != (FILE *)0x0) {
    fprintf(log,s__s_s_10026ac0,ctime(&now),DAT_100f23e0);
    fclose(log);
  }
  
  MessageBoxA((HWND)0x0,DAT_100f23e0,s_Assertion_Error_10026ac8,0x1000);
  exit(0xff);
}

// FUNCTION: DRAWCARDLIB 0x1000c206
void FUN_1000c206(int ok,const char *file,int line,const char *fmt,...)
{
  FILE *log;
  time_t now;
  size_t used;
  va_list args;
  
  if (ok != 0) {
    return;
  }
  
  DAT_100f23e0[0] = '\0';
  if ((file != (const char *)0x0) && (line != 0)) {
    sprintf(DAT_100f23e0,s_File____s__Line____d_10026aec,file,line);
  }
  used = strlen(DAT_100f23e0);
  va_start(args,fmt);
  _vsnprintf(DAT_100f23e0 + used,2000 - used,fmt,args);
  va_end(args);
  
  now = time((time_t *)0x0);
  log = fopen(s__assertFile_txt_10026ad8,DAT_10026aa4);
  if (log != (FILE *)0x0) {
    fprintf(log,s__s_s_10026b04,ctime(&now),DAT_100f23e0);
    fclose(log);
  }
  
  MessageBoxA((HWND)0x0,DAT_100f23e0,s_Assertion_Error_10026b0c,0x1000);
  exit(0xff);
}

// FUNCTION: DRAWCARDLIB 0x1000cde0
void FUN_1000cde0(undefined4 param_1)
{
  _free_dbg(param_1,1);
  return;
}

// FUNCTION: DRAWCARDLIB 0x1000ead0
/* WARNING: Unable to track spacebase fully for stack */

void MSVC_StackProbe(void)
{ 
  /*
  uint in_EAX;
  undefined1 *puVar1;
  undefined4 unaff_retaddr;
  
  puVar1 = &stack0x00000004;
  for (; 0xfff < in_EAX; in_EAX = in_EAX - 0x1000) {
    puVar1 = puVar1 + -0x1000;
  }
  *(undefined4 *)(puVar1 + (-4 - in_EAX)) = unaff_retaddr;
  */
  return;
}
