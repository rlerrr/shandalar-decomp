#include <windows.h>
#include <stdio.h>
#include "mystdbool.h"
#include "cardartlib/src/assert.h"

#define SHARED_RESOURCES_IMPL
#include "shared_resources.h"

// GLOBAL: DECKDLL 0x10035b48
// GLOBAL: MAGIC 0x00571d28
// GLOBAL: SHANDALAR 0x00585930
static LOGFONT global_font_template_ini = {
    0,                           // lfHeight
    0,                           // lfWidth
    0,                           // lfEscapement
    0,                           // lfOrientation
    FW_NORMAL,                   // lfWeight
    FALSE,                       // lfItalic
    FALSE,                       // lfUnderline
    FALSE,                       // lfStrikeOut
    ANSI_CHARSET,                // lfCharSet
    OUT_DEFAULT_PRECIS,          // lfOutPrecision
    CLIP_DEFAULT_PRECIS,         // lfClipPrecision
    DEFAULT_QUALITY,             // lfQuality
    DEFAULT_PITCH | FF_DONTCARE, // lfPitchAndFamily
    {0}                          // lfFaceName[LF_FACESIZE]
};

// FUNCTION: DECKDLL 0x10025727
// FUNCTION: MAGIC 0x00495e95
// FUNCTION: SHANDALAR 0x00466928
LOGFONT *LoadFontFromIni(char *name, int italic)
{
  struct
  {
    char key[100]; /* ebp - 0x68 */
    UINT bold;     /* ebp - 0x4 */
  } s;

  memcpy(&global_font_from_ini, &global_font_template_ini, sizeof(LOGFONT));

  strcpy(s.key, "size");
  strcat(s.key, name);
  global_font_from_ini.lfHeight = GetPrivateProfileIntA("Fonts", s.key, 0x14, global_duel_dat_path);

  strcpy(s.key, "bold");
  strcat(s.key, name);
  s.bold = GetPrivateProfileIntA("Fonts", s.key, 0, global_duel_dat_path);
  if (s.bold != 0)
    global_font_from_ini.lfWeight = FW_BOLD;

  if (italic != 0)
    global_font_from_ini.lfItalic = TRUE;

  strcpy(s.key, "font");
  strcat(s.key, name);
  GetPrivateProfileStringA("Fonts", s.key, "MS Sans Serif",
                           global_font_from_ini.lfFaceName, 0x20, global_duel_dat_path);
  return &global_font_from_ini;
}

// FUNCTION: DECKDLL 0x10011680
// FUNCTION: MAGIC 0x00558fb0
// FUNCTION: SHANDALAR 0x0056f300
bool create_fonts(void)
{
  struct
  {
    char path[MAX_PATH + 4]; /* big buffer first */
    int local_c;             /* ebp - 0x8 */
    int local_8;             /* ebp - 0x4 */
  } s;

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0530m_.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Magis___.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0127m_.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0085m_.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0298m_.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0299m_.TTF");
  AddFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0300m_.TTF");
  AddFontResourceA(s.path);

  global_fonts_init_state = 3;

  sprintf(s.path, "%s\\Damage.pic", global_cardart_path);
  global_damage_pic = load_pic(s.path);
  sprintf(s.path, "%s\\CardCounters.pic", global_cardart_path);
  global_card_counters_pic = load_pic(s.path);
  sprintf(s.path, "%s\\ManaSymbols.pic", global_cardart_path);
  global_mana_symbols_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Abilities.pic", global_cardart_path);
  global_abilities_pic = load_pic(s.path);
  sprintf(s.path, "%s\\ManaStripes.pic", global_cardart_path);
  global_mana_stripes_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Summon.pic", global_cardart_path);
  global_summon_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Dying.pic", global_cardart_path);
  global_dying_pic = load_pic(s.path);
  sprintf(s.path, "%s\\Target.pic", global_cardart_path);
  global_target_pic = load_pic(s.path);
  sprintf(s.path, "%s\\CantTarget.pic", global_cardart_path);
  global_cant_target_pic = load_pic(s.path);
  sprintf(s.path, "%s\\WillUntap.pic", global_cardart_path);
  global_will_untap_pic = load_pic(s.path);

  global_smallcard_pt_font = CreateFontIndirectA(LoadFontFromIni("SmallCardPT", 0));
  global_damage_font = CreateFontIndirectA(LoadFontFromIni("Damage", 0));
  global_idtag_font = CreateFontIndirectA(LoadFontFromIni("IDTag", 0));

  global_palette_col_c9 = GetPaletteColor(0xc9);
  global_palette_col_9e_a = GetPaletteColor(0x9e);
  global_palette_col_9e_b = GetPaletteColor(0x9e);
  global_palette_col_7c = GetPaletteColor(0x7c);
  global_palette_col_2f = GetPaletteColor(0x2f);
  global_palette_col_bf = GetPaletteColor(0xbf);
  global_palette_col_5d = GetPaletteColor(0x5d);
  global_palette_col_1f = GetPaletteColor(0x1f);
  global_pen_palette_5d = CreatePen(6, 2, global_palette_col_5d);
  global_pen_palette_1f = CreatePen(6, 2, global_palette_col_1f);

  for (s.local_c = 0, s.local_8 = 1; s.local_c < 10; ++s.local_c)
  {
    global_debug_pens[s.local_c] =
        CreatePen(6, 3,
                  ((unsigned int)(BYTE)(s.local_c * 75) << 16) |
                      ((unsigned int)(BYTE)(s.local_c * 20) << 8) |
                      (unsigned int)(BYTE)(s.local_c * 10));
    if (global_debug_pens[s.local_c] == (HPEN)0)
      s.local_8 = 0;
  }

  if (!global_damage_pic || !global_card_counters_pic || !global_mana_symbols_pic ||
      !global_abilities_pic || !global_mana_stripes_pic || !global_summon_pic ||
      !global_dying_pic || !global_target_pic || !global_cant_target_pic ||
      !global_will_untap_pic || !global_smallcard_pt_font || !global_damage_font ||
      !global_idtag_font || !global_pen_palette_5d || !global_pen_palette_1f || !s.local_8)
  {
    destroy_create_fonts_resources();
    return false;
  }

  return true;
}

// FUNCTION: DECKDLL 0x10011c41
// FUNCTION: MAGIC 0x00559571
// FUNCTION: SHANDALAR 0x0056f8c1
void destroy_create_fonts_resources(void)
{
  struct
  {
    char path[MAX_PATH + 4];
    int i;
  } s;

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Magim___.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0530m_.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Magis___.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0127m_.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0085m_.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0298m_.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0299m_.TTF");
  RemoveFontResourceA(s.path);
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Tt0300m_.TTF");
  RemoveFontResourceA(s.path);

  if (global_damage_pic)
  {
    delete_and_close_object(global_damage_pic);
    global_damage_pic = NULL;
  }
  if (global_card_counters_pic)
  {
    delete_and_close_object(global_card_counters_pic);
    global_card_counters_pic = NULL;
  }
  if (global_mana_symbols_pic)
  {
    delete_and_close_object(global_mana_symbols_pic);
    global_mana_symbols_pic = NULL;
  }
  if (global_abilities_pic)
  {
    delete_and_close_object(global_abilities_pic);
    global_abilities_pic = NULL;
  }
  if (global_mana_stripes_pic)
  {
    delete_and_close_object(global_mana_stripes_pic);
    global_mana_stripes_pic = NULL;
  }
  if (global_summon_pic)
  {
    delete_and_close_object(global_summon_pic);
    global_summon_pic = NULL;
  }
  if (global_dying_pic)
  {
    delete_and_close_object(global_dying_pic);
    global_dying_pic = NULL;
  }
  if (global_target_pic)
  {
    delete_and_close_object(global_target_pic);
    global_target_pic = NULL;
  }
  if (global_cant_target_pic)
  {
    delete_and_close_object(global_cant_target_pic);
    global_cant_target_pic = NULL;
  }
  if (global_will_untap_pic)
  {
    delete_and_close_object(global_will_untap_pic);
    global_will_untap_pic = NULL;
  }

  if (global_smallcard_pt_font)
    DeleteObject(global_smallcard_pt_font);
  global_smallcard_pt_font = NULL;
  if (global_damage_font)
    DeleteObject(global_damage_font);
  global_damage_font = NULL;
  if (global_idtag_font)
    DeleteObject(global_idtag_font);
  global_idtag_font = NULL;
  if (global_pen_palette_5d)
    DeleteObject(global_pen_palette_5d);
  global_pen_palette_5d = NULL;
  if (global_pen_palette_1f)
    DeleteObject(global_pen_palette_1f);
  global_pen_palette_1f = NULL;

  for (s.i = 0; s.i < 10; s.i++)
    if (global_debug_pens[s.i])
    {
      DeleteObject(global_debug_pens[s.i]);
      global_debug_pens[s.i] = NULL;
    }
}

// GLOBAL: DECKDLL 0x10035b3c
// GLOBAL: MAGIC 0x00571d1c
// GLOBAL: SHANDALAR 0x00585924
static int global_deleteobj_platform_id = -1;

#pragma intrinsic(memset)

// FUNCTION: DECKDLL 0x1002417b
// FUNCTION: MAGIC 0x004948e1
// FUNCTION: SHANDALAR 0x0046537b
void delete_and_close_object(HANDLE obj)
{
  struct
  {
    char dbg_buf[500];
    OSVERSIONINFOA ver;
    HANDLE section_handle;
    DIBSECTION dib_section;
    char *bits_pointer;
  } s;

  if (global_deleteobj_platform_id == -1)
  {
    s.ver.dwOSVersionInfoSize = 0x94;
    memset(&s.ver.dwMajorVersion, 0, 0x90);
    GetVersionExA(&s.ver);
    if (s.ver.dwPlatformId == 1)
      global_deleteobj_platform_id = 1;
    else
      global_deleteobj_platform_id = 0;
  }

  if (obj != (HANDLE)0)
  {
    GetObjectA(obj, 0x54, &s.dib_section);
    s.section_handle = s.dib_section.dshSection;
    s.bits_pointer = (char *)s.dib_section.dsBm.bmBits;
    s.bits_pointer += s.dib_section.dsOffset;
    DeleteObject(obj);
    if (s.section_handle != (HANDLE)0 && global_deleteobj_platform_id != 0)
      CloseHandle(s.section_handle);
  }

  if (global_DIB_debug != 0)
  {
    sprintf(s.dbg_buf, "  %08x DestroyDIBSection (file mapping: %08x)\n", obj, s.section_handle);
    OutputDebugStringA(s.dbg_buf);
  }
}

#pragma function(memset)
