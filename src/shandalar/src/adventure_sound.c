#include <windows.h>
#include <mmsystem.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>
#include <errno.h>

#include "defs.h"
#include "shandalar.h"
#include "shandalar_internal.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/global_other.h"
#include "magic/src/game_support.h"
#include "magic/src/duel_engine.h"
#include "magic/src/shared_startup.h"
#include "shandalar_global_strings.h"
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/card_db.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

// FUNCTION: SHANDALAR 0x005616cb
void EnsureAdvfac64Loaded(int state)
{
  if (state != g_advfac64_load_state)
  {
    LoadPcxIntoPageNoPalette("advfac64.pic");
    g_advfac64_load_state = state;
  }
}

// FUNCTION: SHANDALAR 0x00562736
void PlaySoundWithPitchAndPan(int sound_id, int volume, int pitch_percent, int pan_percent)
{
  Sound snd;

  memset(&snd, 0, 0x20);
  snd.volume = volume << 2;
  snd.sampleRate = (pitch_percent * 0x5622) / 100;
  snd.pan = pan_percent << 2;
  snd.flags &= 0xfffffffe;
  snd.flags &= 0xffffffef;
  sound_play(sound_id, &snd);
}

// FUNCTION: SHANDALAR 0x005627ee
void PlaySoundWithPan(int sound_id, int volume, int pan)
{
  Sound snd;

  memset(&snd, 0, 0x20);
  snd.volume = volume << 2;
  snd.sampleRate = 0x5622;
  snd.pan = pan << 2;
  sound_play(sound_id, &snd);
}

// FUNCTION: SHANDALAR 0x0056279e
void PlayLoopingSoundWithPan(int sound_id, int volume, int pan_percent)
{
  Sound snd;

  memset(&snd, 0, 0x20);
  snd.volume = volume << 2;
  snd.sampleRate = 0x5622;
  snd.pan = pan_percent << 2;
  snd.flags = (int)snd.flags | 1;
  sound_play(sound_id, &snd);
}

// FUNCTION: SHANDALAR 0x00562835
void LoadLoopingSound(char *sound_path, int channel)
{
  Sound snd;

  memset(&snd, 0, 0x20);
  snd.flags = (int)snd.flags | 4;
  snd.volume = 400;
  snd.sampleRate = 0;
  snd.pan = 0;
  LoadSoundWithDriveFallback(sound_path, channel, &snd);
  set_sound_loop(channel, 1);
}

// FUNCTION: SHANDALAR 0x00562a29
void UpdateAmbientWizardColorSound(int wizard_color)
{
  int channel = wizard_color + 9;

  PlaySoundWithPitchAndPan(channel, 100, internal_rand(0x32) + 0x46, internal_rand(200) - 100);

  if (internal_rand(3) == 0)
  {
    sound_unload(channel);
    switch (wizard_color)
    {
    case 1:
      switch (internal_rand(3))
      {
      case 0:
        LoadSoundWithDriveFallback("x:sound\\kbird1.wav", channel, 0);
        break;
      case 1:
        LoadSoundWithDriveFallback("x:sound\\kland1.wav", channel, 0);
        break;
      case 2:
        LoadSoundWithDriveFallback("x:sound\\kland2.wav", channel, 0);
        break;
      }
      break;
    case 2:
      switch (internal_rand(3))
      {
      case 0:
        LoadSoundWithDriveFallback("x:sound\\bbird1.wav", channel, 0);
        break;
      case 1:
        LoadSoundWithDriveFallback("x:sound\\bland1.wav", channel, 0);
        break;
      case 2:
        LoadSoundWithDriveFallback("x:sound\\bland2.wav", channel, 0);
        break;
      }
      break;
    case 3:
      switch (internal_rand(2))
      {
      case 0:
        LoadSoundWithDriveFallback("x:sound\\gbird1.wav", channel, 0);
        break;
      case 1:
        LoadSoundWithDriveFallback("x:sound\\gland1.wav", channel, 0);
        break;
      }
      break;
    case 4:
      switch (internal_rand(2))
      {
      case 0:
        LoadSoundWithDriveFallback("x:sound\\rbird1.wav", channel, 0);
        break;
      case 1:
        LoadSoundWithDriveFallback("x:sound\\rland1.wav", channel, 0);
        break;
      }
      break;
    case 5:
      switch (internal_rand(2))
      {
      case 0:
        LoadSoundWithDriveFallback("x:sound\\wbird1.wav", channel, 0);
        break;
      case 1:
        LoadSoundWithDriveFallback("x:sound\\wland1.wav", channel, 0);
        break;
      }
      break;
    }
  }
}

// FUNCTION: SHANDALAR 0x00564ee7
int LoadAdvStringsFile(const char *filename)
{
  struct
  {
    int ok;
    int line_count;
    int i;
    char *next_buf;
  } s;

  s.ok = 1;
  s.ok &= LoadTextSectionStringTable(filename, "PLAYERNAMES", gs_playernames_0077c5a0, 0xe, gs_playernames_buf_0077e2c0, gs_playernames_buf_0077e2c0 + 0x2bc, (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "DIFFICULTYLEVELS", gs_difficultylevels_0077d130, 4, gs_difficultylevels_buf_0074d790, gs_difficultylevels_buf_0074d790 + 0x64, (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "DUNGEON_NAMES", gs_dungeon_names_00780820, 0x11, gs_dungeon_names_buf_0074c970, gs_dungeon_names_buf_0074c970 + 0x352, (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "LAIR_NAMES", gs_lair_names_0077c020, 0x13, gs_lair_names_buf_0074bd30, gs_lair_names_buf_0074bd30 + 0x3b6, (char **)0);

  LoadTextSectionLines(filename, "CITYNAMES_FORMAT");
  strcpy(gs_cityname_format_left_0074c950, g_text_lines[0]);
  strcpy(gs_cityname_format_right_0077f190, g_text_lines[1]);

  LoadTextSectionLines(filename, "CITYNAME_VILLAGE");
  strcpy(gs_cityname_village_0077cfd0, g_text_lines[0]);

  LoadTextSectionLines(filename, "CITYNAME_CASTLE");
  strcpy(gs_cityname_castle_00765dc0, g_text_lines[0]);

  LoadTextSectionLines(filename, "CITYNAME_MANACASTLE");

  for (s.i = 0; s.i < 5; s.i = s.i + 1)
  {
    strcpy(gs_cityname_manacastle_0077de00[s.i + 1], g_text_lines[s.i]);
  }
  strcpy(gs_cityname_manacastle_0077de00[0], gs_cityname_castle_00765dc0);

  s.next_buf = gs_citynames_buf_0077d610;
  s.ok &= LoadTextSectionStringTable(filename, "CITYNAMES_FIRSTHALF", gs_citynames_firsthalf_0077e060, 0x10, gs_citynames_buf_0077d610,
                                     gs_citynames_buf_0077d610 + sizeof(gs_citynames_buf_0077d610), &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "CITYNAMES_SECONDHALF", gs_citynames_secondhalf_007653e0, 0x10, s.next_buf,
                                     gs_citynames_buf_0077d610 + sizeof(gs_citynames_buf_0077d610), (char **)0);

  s.next_buf = gs_worldmagic_buf_0074b160;
  LoadTextSectionLines(filename, "WORLDMAGIC");
  strcpy(gs_worldmagic_title_0077e1d0, g_text_lines[0]);
  s.ok &= LoadTextSectionStringTable(filename, "WORLDMAGIC_NAMES", gs_worldmagic_names_00780660, 0xc, gs_worldmagic_buf_0074b160,
                                     gs_worldmagic_buf_0074b160 + sizeof(gs_worldmagic_buf_0074b160), &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "WORLDMAGIC_EXPLAINS", gs_worldmagic_explains_0074b8f0, 0xc, s.next_buf,
                                     gs_worldmagic_buf_0074b160 + sizeof(gs_worldmagic_buf_0074b160), (char **)0);

  s.ok &= LoadTextSectionStringTable(filename, "LOGSTRINGS", gs_logstrings_0077c9a0, 0x10, gs_logstrings_buf_0077c680,
                                     gs_logstrings_buf_0077c680 + sizeof(gs_logstrings_buf_0077c680), (char **)0);

  LoadTextSectionLines(filename, "WIZARDNAMES");
  for (s.i = 0; s.i < 5; s.i = s.i + 1)
  {
    strcpy(gs_wizardnames_0077ee70[s.i + 1], g_text_lines[s.i]);
  }
  strcpy(gs_wizardnames_0077ee70[0], "");

  s.line_count = LoadTextSectionLines(filename, "CREATURENAMES");
  s.line_count = MIN(s.line_count, (int)gs_creature_name_count_00593934);
  for (s.i = 0; s.i < s.line_count; s.i = s.i + 1)
  {
    strcpy(g_shandalar_monster_definitions[s.i].name, g_text_lines[s.i]);
  }
  for (s.i = s.line_count; s.i < (int)gs_creature_name_count_00593934; s.i = s.i + 1)
  {
    strcpy(g_shandalar_monster_definitions[s.i].name, "");
  }

  s.line_count = LoadTextSectionLines(filename, "CREATURENAME_ARTICLES");
  s.line_count = MIN(s.line_count, (int)gs_creature_name_count_00593934);
  for (s.i = 0; s.i < s.line_count; s.i = s.i + 1)
  {
    strcpy(g_shandalar_monster_definitions[s.i].article, g_text_lines[s.i]);
  }
  for (s.i = s.line_count; s.i < (int)gs_creature_name_count_00593934; s.i = s.i + 1)
  {
    strcpy(g_shandalar_monster_definitions[s.i].name, "");
  }

  s.line_count = LoadTextSectionLines(filename, "CREATURENAMES_PLURAL");
  s.line_count = MIN(s.line_count, (int)gs_creature_name_count_00593934);
  for (s.i = 0; s.i < s.line_count; s.i = s.i + 1)
  {
    strcpy(g_shandalar_monster_definitions[s.i].plural, g_text_lines[s.i]);
  }
  for (s.i = s.line_count; s.i < (int)gs_creature_name_count_00593934; s.i = s.i + 1)
  {
    strcpy(g_shandalar_monster_definitions[s.i].plural, "");
  }

  LoadTextSectionLines(filename, "DIRECTIONS");
  for (s.i = 0; s.i < 4; s.i = s.i + 1)
  {
    strcpy(gs_directions_00765d50[s.i], g_text_lines[s.i]);
  }

  s.next_buf = gs_cardclassnames_buf_0074c5c0;
  s.ok &= LoadTextSectionStringTable(filename, "CARDCLASSNAMES", gs_cardclassnames_0077cf70, 9, gs_cardclassnames_buf_0074c5c0,
                                     gs_cardclassnames_buf_0074c5c0 + sizeof(gs_cardclassnames_buf_0074c5c0), &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "CARDCLASSNAMES_PLURAL", gs_cardclassnames_plural_0077e1f0, 9, s.next_buf,
                                     gs_cardclassnames_buf_0074c5c0 + sizeof(gs_cardclassnames_buf_0074c5c0), (char **)0);

  LoadTextSectionLines(filename, "SPELLNAMES");
  strcpy(gs_spellname_primary_0077e6e0, g_text_lines[0]);
  for (s.i = 1; s.i < 5; s.i = s.i + 1)
  {
    strcpy(gs_spellnames_0077e220[s.i], g_text_lines[s.i]);
  }
  strcpy(gs_spellnames_0077e220[0], g_text_lines[6]);

  LoadTextSectionLines(filename, "MANANAMES");
  for (s.i = 0; s.i < 5; s.i = s.i + 1)
  {
    strcpy(gs_mananames_0074d980[s.i + 1], g_text_lines[s.i]);
  }
  strcpy(gs_mananames_0074d980[0], "");

  LoadTextSectionLines(filename, "LANDWALKS");
  for (s.i = 0; s.i < 5; s.i = s.i + 1)
  {
    strcpy(gs_landwalks_007806f0[s.i + 1], g_text_lines[s.i]);
  }
  strcpy(gs_landwalks_007806f0[0], "");

  LoadTextSectionLines(filename, "AMULETNAMES");
  for (s.i = 0; s.i < 6; s.i = s.i + 1)
  {
    strcpy(gs_amuletnames_0077d090[s.i], g_text_lines[s.i]);
  }

  LoadTextSectionLines(filename, "AMULETNAMES_PLURAL");
  for (s.i = 0; s.i < 6; s.i = s.i + 1)
  {
    strcpy(gs_amuletnames_plural_0077edd0[s.i], g_text_lines[s.i]);
  }

  LoadTextSectionLines(filename, "COLORCARDS");
  for (s.i = 0; s.i < 6; s.i = s.i + 1)
  {
    strcpy(gs_colorcards_0077c5e0[s.i], g_text_lines[s.i]);
  }

  s.ok &= LoadTextSectionStringTable(filename, "CAVE_SHOWCLUES", gs_cave_showclues_0077efa0, 0x17, gs_cave_showclues_buf_007658d0,
                                     gs_cave_showclues_buf_007658d0 + sizeof(gs_cave_showclues_buf_007658d0), (char **)0);

  s.next_buf = gs_encounter_buf_0074da70;
  s.ok &= LoadTextSectionStringTable(filename, "ENCOUNTER_PREDUEL", gs_encounter_preduel_0077f0d0, 0x30, gs_encounter_buf_0074da70,
                                     gs_encounter_buf_0074da70 + sizeof(gs_encounter_buf_0074da70), &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "ENCOUNTER_POSTDUEL", gs_encounter_postduel_0077f050, 0x20, s.next_buf,
                                     gs_encounter_buf_0074da70 + sizeof(gs_encounter_buf_0074da70), (char **)0);

  s.next_buf = gs_visit_buf_0077f610;
  s.ok &= LoadTextSectionStringTable(filename, "VISIT", gs_visit_0077c4f0, 0x23, gs_visit_buf_0077f610, gs_visit_buf_0077f610 + sizeof(gs_visit_buf_0077f610),
                                     &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "VISIT_CITYBUY", gs_visit_citybuy_0077f1d0, 4, s.next_buf,
                                     gs_visit_buf_0077f610 + sizeof(gs_visit_buf_0077f610), (char **)0);

  s.ok &= LoadTextSectionStringTable(filename, "CASTLEWIN", gs_castlewin_0074b8c0, 0xb, gs_castlewin_buf_0077e700,
                                     gs_castlewin_buf_0077e700 + sizeof(gs_castlewin_buf_0077e700), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "DUNGEON", gs_dungeon_0077f000, 0x14, gs_dungeon_buf_00780870, gs_dungeon_buf_00780870 + sizeof(gs_dungeon_buf_00780870),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "HINTTEXT", gs_hinttext_0077e580, 2, gs_hinttext_buf_0077e5a0, gs_hinttext_buf_0077e5a0 + sizeof(gs_hinttext_buf_0077e5a0),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "QUESTFAILED", gs_questfailed_0077c580, 6, gs_questfailed_buf_0074b930,
                                     gs_questfailed_buf_0074b930 + sizeof(gs_questfailed_buf_0074b930), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "MONSTERLAIR", gs_monsterlair_0074cff0, 6, gs_monsterlair_buf_0074d010,
                                     gs_monsterlair_buf_0074d010 + sizeof(gs_monsterlair_buf_0074d010), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "BUYANYCARD", gs_buyanycard_0074ccd0, 6, gs_buyanycard_buf_0077f1e0,
                                     gs_buyanycard_buf_0077f1e0 + sizeof(gs_buyanycard_buf_0077f1e0), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "QUESTSTATUS", gs_queststatus_0077e0a0, 0x1a, gs_queststatus_buf_0074d270,
                                     gs_queststatus_buf_0074d270 + sizeof(gs_queststatus_buf_0074d270), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "NEWSFLASH", gs_newsflash_0077d140, 0xb, gs_newsflash_buf_0077d1c0,
                                     gs_newsflash_buf_0077d1c0 + sizeof(gs_newsflash_buf_0077d1c0), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "CITYCARDTEXT", gs_citycardtext_0074ccf0, 0x31, gs_citycardtext_buf_0074c0f0,
                                     gs_citycardtext_buf_0074c0f0 + sizeof(gs_citycardtext_buf_0074c0f0), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "BROWSE", gs_browse_0074da20, 0x11,
                                     gs_city_browse_buf_0077dc50,
                                     gs_city_browse_buf_0077dc50 + sizeof(gs_city_browse_buf_0077dc50),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "SHOWDECK", gs_showdeck_0074b920, 3, gs_showdeck_buf_0074d890,
                                     gs_showdeck_buf_0074d890 + sizeof(gs_showdeck_buf_0074d890), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "LOADSAVE", gs_loadsave_0077d1b0, 3, gs_loadsave_buf_0077d040, gs_loadsave_buf_0077d040 + sizeof(gs_loadsave_buf_0077d040),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "STATS", gs_stats_0077cfa0, 9, gs_stats_buf_0074ce20, gs_stats_buf_0074ce20 + sizeof(gs_stats_buf_0074ce20),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "ANALYZE", gs_analyze_0074b870, 0x11, gs_analyze_buf_0074afb0, gs_analyze_buf_0074afb0 + sizeof(gs_analyze_buf_0074afb0),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "RIDDLE", gs_riddle_0077cf20, 0x11, gs_riddle_buf_0077ca20, gs_riddle_buf_0077ca20 + sizeof(gs_riddle_buf_0077ca20),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "LAIR", gs_lair_0077e180, 0x13, gs_lair_buf_007800c0, gs_lair_buf_007800c0 + sizeof(gs_lair_buf_007800c0),
                                     (char **)0);

  s.next_buf = gs_wiseman_buf_00780c60;
  s.ok &= LoadTextSectionStringTable(filename, "WISEMAN", gs_wiseman_0074d840, 0x13, gs_wiseman_buf_00780c60, gs_wiseman_buf_00780c60 + sizeof(gs_wiseman_buf_00780c60),
                                     &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "CITYWISEMAN", gs_citywiseman_0074d800, 0xd, s.next_buf, gs_wiseman_buf_00780c60 + sizeof(gs_wiseman_buf_00780c60),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "CITYSCREEN_BUTTONS", gs_cityscreen_buttons_0077f5e0, 8, gs_cityscreen_buttons_buf_0077f450,
                                     gs_cityscreen_buttons_buf_0077f450 + sizeof(gs_cityscreen_buttons_buf_0077f450), (char **)0);

  LoadTextSectionLines(filename, "SHOWLIST");
  strcpy(gs_showcard_text_0077e110.title, g_text_lines[0]);
  strcpy(gs_showcard_text_0077e110.accept_keys, g_text_lines[1]);

  LoadTextSectionLines(filename, "SHOWLIBRARY");
  strcpy(gs_showlibrary_text_0074bcc0.title, g_text_lines[0]);
  strcpy(gs_showlibrary_text_0074bcc0.accept_keys, g_text_lines[1]);

  return s.ok;
}

// FUNCTION: SHANDALAR 0x00565c7e
int LoadTextSectionStringTable(const char *filename, const char *section, char **out_table, int max_entries, char *string_buf,
                               char *string_buf_end, char **out_next_buf)
{
  struct
  {
    int count;
    int overflow;
    size_t line_len;
    int i;
    char *end;
    char *cursor;
  } s;

  s.overflow = 0;
  s.count = LoadTextSectionLines(filename, section);
  s.count = MIN(s.count, max_entries);

  s.cursor = string_buf;
  s.end = string_buf_end;

  for (s.i = 0; s.i < s.count && s.overflow == 0; s.i++)
  {
    s.line_len = strlen(g_text_lines[s.i]);
    if (s.cursor + s.line_len < s.end)
    {
      strcpy(s.cursor, g_text_lines[s.i]);
      out_table[s.i] = s.cursor;
      s.cursor = s.cursor + s.line_len + 1;
    }
    else
    {
      s.overflow = 1;
    }
  }

  for (s.i = s.count; s.i < max_entries; s.i++)
  {
    out_table[s.i] = (char *)"";
  }

  if (out_next_buf != (char **)0)
  {
    *out_next_buf = s.cursor;
  }

  if (s.overflow != 0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

// FUNCTION: SHANDALAR 0x00565fdb
int FindNextTextBlock(char *scan_start, char *scan_end, int *out_block_start, int *out_next_scan)
{
  if (scan_start == (char *)0 || scan_end == (char *)0 || scan_end <= scan_start || out_block_start == (int *)0 ||
      out_next_scan == (int *)0)
  {
    return 0;
  }

  while (*scan_start != '\0' && scan_start < scan_end && strncmp(scan_start, "STARTBLOCK", 10))
  {
    ++scan_start;
  }

  if (strncmp(scan_start, "STARTBLOCK", 10) == 0)
  {
    scan_start += 0xc;
    if (out_block_start != (int *)0)
    {
      *out_block_start = (int)scan_start;
    }

    while (*scan_start != '\0' && scan_start < scan_end && strncmp(scan_start, "ENDBLOCK", 8))
    {
      ++scan_start;
    }

    if (strncmp(scan_start, "ENDBLOCK", 8) == 0)
    {
      *scan_start = '\0';
      if (out_next_scan != (int *)0)
      {
        *out_next_scan = (int)(scan_start + 8);
      }
    }
    else if (out_next_scan != (int *)0)
    {
      *out_next_scan = 0;
    }
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: SHANDALAR 0x00565dbc
int LoadAdvBlocksFile(const char *filename)
{
  struct
  {
    HANDLE file_handle;
    int ok;
    DWORD bytes_read;
    DWORD file_size;
    int block_text_start;
    int i;
    char *buffer_end;
    char *carriage_return;
    char *cursor;
    char *next_block_scan;
  } s;

  s.ok = 1;
  s.file_handle = CreateFileA(filename, 0x80000000, 1, (LPSECURITY_ATTRIBUTES)0, 3, 0x8000080, (HANDLE)0);
  if (s.file_handle != (HANDLE)-1)
  {
    s.file_size = GetFileSize(s.file_handle, (LPDWORD)0);
    g_advblocks_file_buffer = (char *)malloc(s.file_size + 1);
    if (g_advblocks_file_buffer != (char *)0)
    {
      ReadFile(s.file_handle, (void *)g_advblocks_file_buffer, s.file_size, &s.bytes_read, (LPOVERLAPPED)0);
      s.cursor = g_advblocks_file_buffer;
      s.buffer_end = s.cursor + s.bytes_read;

      for (s.i = 0; s.i < 4; ++s.i)
      {
        if (FindNextTextBlock(s.cursor, s.buffer_end, &s.block_text_start, (int *)&s.next_block_scan) != 0)
        {
          gs_advblock_city_0074c930[s.i] = (char *)s.block_text_start;
          s.cursor = s.next_block_scan;
          while (s.carriage_return = strchr(gs_advblock_city_0074c930[s.i], 0xd))
          {
            strcpy(s.carriage_return, s.carriage_return + 1);
          }
        }
        else
        {
          s.ok = 0;
        }
      }

      for (s.i = 0; s.i < 0xc; ++s.i)
      {
        if (FindNextTextBlock(s.cursor, s.buffer_end, &s.block_text_start, (int *)&s.next_block_scan) != 0)
        {
          gs_advblock_general_0077c9e0[s.i] = (char *)s.block_text_start;
          s.cursor = s.next_block_scan;
          while (s.carriage_return = strchr(gs_advblock_general_0077c9e0[s.i], 0xd))
          {
            strcpy(s.carriage_return, s.carriage_return + 1);
          }
        }
        else
        {
          s.ok = 0;
        }
      }
    }
    else
    {
      s.ok = 0;
    }

    CloseHandle(s.file_handle);
  }
  else
  {
    s.ok = 0;
  }

  return s.ok;
}

// FUNCTION: SHANDALAR 0x00562d03
void PreloadWorldAmbientSounds(void)
{
  LoadSoundWithDriveFallback("x:sound\\kwalkl.wav", 0, 0);
  LoadSoundWithDriveFallback("x:sound\\kwalkr.wav", 1, 0);
  LoadSoundWithDriveFallback("x:sound\\bwalkl.wav", 2, 0);
  LoadSoundWithDriveFallback("x:sound\\bwalkr.wav", 3, 0);
  LoadSoundWithDriveFallback("x:sound\\gwalkl.wav", 4, 0);
  LoadSoundWithDriveFallback("x:sound\\gwalkr.wav", 5, 0);
  LoadSoundWithDriveFallback("x:sound\\rwalkl.wav", 6, 0);
  LoadSoundWithDriveFallback("x:sound\\rwalkr.wav", 7, 0);
  LoadSoundWithDriveFallback("x:sound\\wwalkl.wav", 8, 0);
  LoadSoundWithDriveFallback("x:sound\\wwalkr.wav", 9, 0);
  LoadSoundWithDriveFallback("x:sound\\kbird1.wav", 10, 0);
  LoadSoundWithDriveFallback("x:sound\\bbird1.wav", 0xb, 0);
  LoadSoundWithDriveFallback("x:sound\\gbird1.wav", 0xc, 0);
  LoadSoundWithDriveFallback("x:sound\\rbird1.wav", 0xd, 0);
  LoadSoundWithDriveFallback("x:sound\\wbird1.wav", 0xe, 0);
}

// FUNCTION: SHANDALAR 0x00565faa
void FreeAdvblocksFileBuffer(void)
{
  if (g_advblocks_file_buffer != 0)
  {
    free(g_advblocks_file_buffer);
    g_advblocks_file_buffer = 0;
  }
}

// FUNCTION: SHANDALAR 0x00562e0d
char FindDriveWithAsset(char *filename)
{
  FILE *asset_file;
  char drive_string[256] = "a:\\";

  for (; drive_string[0] <= (int)'z'; ++drive_string[0])
  {
    if (GetDriveTypeA(drive_string) == 5)
    {
      strcat(drive_string, filename);
      asset_file = fopen(drive_string, "rb");
      if (asset_file != (FILE *)0)
      {
        fclose(asset_file);
        return drive_string[0];
      }
      else
      {
        drive_string[2] = 0;
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x005626b0
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent)
{
  Sound sound_playback;

  sound_unload(channel);
  LoadSoundWithDriveFallback(sound_path, channel, 0);
  memset(&sound_playback, 0, sizeof(sound_playback));
  sound_playback.volume = volume << 2;
  sound_playback.sampleRate = (pitch_percent * 0x5622) / 100;
  sound_playback.pan = pan_percent << 2;
  sound_playback.flags &= 0xfffffffe;
  sound_playback.flags &= 0xffffffef;
  sound_play(channel, &sound_playback);
}

// FUNCTION: SHANDALAR 0x00562ed0
char GetSoundAssetDriveLetter(void)
{
  char cwd[256];

  if (g_sound_drive_initialized == 0)
  {
    g_sound_drive_letter = FindDriveWithAsset("sound\\locmus1.wav");
    g_sound_drive_initialized = 1;
  }

  if (g_local_sound_missing == 0)
  {
    getcwd(cwd, 0x100);
    return cwd[0];
  }
  else
  {
    return g_sound_drive_letter;
  }
}

// FUNCTION: SHANDALAR 0x00562f92
unsigned int LoadSoundWithDriveFallback(char *filename, int channel, Sound *sound)
{
#ifdef MODERN_FIXES
  char filename_copy[260];
  strcpy(filename_copy, filename);
  filename = filename_copy;
#endif

  if (g_cached_cwd_initialized == 0)
  {
    _getcwd(g_cached_cwd, 0x100);
    g_cached_cwd_initialized = 1;
  }

  if (filename[0] == 'x')
  {
    filename[0] = g_cached_cwd[0];
    if (FileExists(filename) == 0)
    {
      filename[0] = GetSoundAssetDriveLetter();
    }
  }

  while (g_sound_loader_busy != 0)
  {
  }

  sound_load(filename, channel, sound);
  return 0;
}

// FUNCTION: SHANDALAR 0x00417dc6
int FileExists(const char *filename)
{
  FILE *file;

  file = fopen(filename, "rt");
  if (file == (FILE *)0)
  {
    return 0;
  }

  fclose(file);
  return 1;
}

