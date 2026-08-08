#define GLOBAL_STATE_IMPL
#include "global_state.h"

#include <errno.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "magic/src/game_support.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_other.h"
#ifdef SHANDALAR
#include "shandalar/src/shandalar_global_strings.h"
#endif

extern card_data_t global_cards_data[];

int CardTypeFromID(int csvid);
int is_card_available_in_installed_expansion(unsigned int csvid);
int save_or_load_data(void *buf, unsigned int count);
#ifdef SHANDALAR
void HideMouseCursorNested(void);
void ShowMouseCursorNested(void);
int GetSaveDriveIndex(void);
int RunTextMenuAt(char *menu_text, int left_x, int top_y);
int LoadGameFromPath(char *save_file_path);
#endif

// GLOBAL: MAGIC 0x005710f8
// GLOBAL: SHANDALAR 0x00588114
static char *s_sealed_deck_registry_path_005710e0 = "Software\\MicroProse\\Magic: The Gathering\\SealedDeckOptions";
// GLOBAL: MAGIC 0x005710ec
// GLOBAL: SHANDALAR 0x00588108
static char *s_solo_options_registry_path_00571284 = "Software\\MicroProse\\Magic: The Gathering\\SoloOptions";
// GLOBAL: MAGIC 0x005710f4
// GLOBAL: SHANDALAR 0x00588110
static char *s_gauntlet_options_registry_path_00571338 = "Software\\MicroProse\\Magic: The Gathering\\GauntletEx1Options";
// GLOBAL: MAGIC 0x00588100
// GLOBAL: SHANDALAR 0x00588100
static char *s_duel_options_registry_path_00588100 = "Software\\MicroProse\\Magic: The Gathering\\DuelOptions";

// FUNCTION: MAGIC 0x004a5a1b
// FUNCTION: SHANDALAR 0x00557b9b
void SetCardInDeck(int deck_index, int enabled)
{
  if (enabled == 1)
  {
    deck[deck_index] = deck[deck_index] | 0x4000;
  }
  else
  {
    deck[deck_index] = deck[deck_index] & 0x8fff;
  }
}

// FUNCTION: MAGIC 0x004ed13a
// FUNCTION: SHANDALAR 0x00501b31
int write_save_data(int file_descriptor, void *buffer, unsigned int count)
{
  if (write(file_descriptor, buffer, count) == -1)
  {
    g_save_errno = errno;
    return 0;
  }
  else
  {
    return 1;
  }
}

// FUNCTION: MAGIC 0x004ef073
// FUNCTION: SHANDALAR 0x00504063
unsigned int save_or_load_duel_mode_state(void)
{
  unsigned int result;

  result = 1;
  result &= save_or_load_data(&DAT_008cee6c, 4);
  result &= save_or_load_data(&DAT_008cec7c, 4);
  result &= save_or_load_data(&DAT_008b42e4, 4);
  result &= save_or_load_data(&DAT_008b323c, 4);
  result &= save_or_load_data(&DAT_00789104, 4);

  return result;
}

// FUNCTION: MAGIC 0x0048b144
// FUNCTION: SHANDALAR 0x004a6dd4
void __stdcall LoadSoloDuelRegistryOptions(void)
{
  struct
  {
    HKEY options_key;
    BYTE value_buffer[0x34];
    DWORD value_size;
  } s;

  if (RegOpenKeyExA(HKEY_CURRENT_USER, s_solo_options_registry_path_00571284, 0, KEY_QUERY_VALUE,
                    &s.options_key) == ERROR_SUCCESS)
  {
    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "Difficulty", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf((char *)s.value_buffer, "%d", &g_solo_duel_options.difficulty);
    else
      g_solo_duel_options.difficulty = 1;

    s.value_size = 0x32;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "PlayerDeck", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sprintf(g_solo_duel_options.player_deck, "%s", s.value_buffer);
    else
      g_solo_duel_options.player_deck[0] = '\0';

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "PlayerRandom", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf((char *)s.value_buffer, "%d", &g_solo_duel_options.player_random);
    else
      g_solo_duel_options.player_random = 1;

    s.value_size = 0x32;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "OpponentDeck", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sprintf(g_solo_duel_options.opponent_deck, "%s", s.value_buffer);
    else
      g_solo_duel_options.opponent_deck[0] = '\0';

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "OpponentRandom", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf((char *)s.value_buffer, "%d", &g_solo_duel_options.opponent_random);
    else
      g_solo_duel_options.opponent_random = 1;

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "BestOf", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf((char *)s.value_buffer, "%d", &g_solo_duel_options.best_of);
    else
      g_solo_duel_options.best_of = 1;

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "Ante", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf((char *)s.value_buffer, "%d", &g_solo_duel_options.ante);
    else
      g_solo_duel_options.ante = 1;

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "AllowSideboarding", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf((char *)s.value_buffer, "%d", &g_solo_duel_options.allow_sideboarding);
    else
      g_solo_duel_options.allow_sideboarding = 1;

    RegCloseKey(s.options_key);
  }
  else
  {
    g_solo_duel_options.difficulty = 1;
    g_solo_duel_options.player_deck[0] = '\0';
    g_solo_duel_options.player_random = 1;
    g_solo_duel_options.opponent_deck[0] = '\0';
    g_solo_duel_options.opponent_random = 1;
    g_solo_duel_options.best_of = 1;
    g_solo_duel_options.ante = 1;
    g_solo_duel_options.allow_sideboarding = 1;
  }
}

// FUNCTION: MAGIC 0x0048b69e
// FUNCTION: SHANDALAR 0x004a732e
void __stdcall LoadGauntletRegistryOptions(void)
{
  struct
  {
    HKEY options_key;
    BYTE value_buffer[0x34];
    DWORD value_size;
  } s;

  if (RegOpenKeyExA(HKEY_CURRENT_USER, s_gauntlet_options_registry_path_00571338, 0, KEY_QUERY_VALUE,
                    &s.options_key) == ERROR_SUCCESS)
  {
    s.value_size = 0x32;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "PlayerDeck", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sprintf(g_gauntlet_options.player_deck, "%s", s.value_buffer);
    else
      g_gauntlet_options.player_deck[0] = '\0';

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "PlayerRandom", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf((char *)s.value_buffer, "%d", &g_gauntlet_options.player_random);
    else
      g_gauntlet_options.player_random = 1;

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "BestOf", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf((char *)s.value_buffer, "%d", &g_gauntlet_options.best_of);
    else
      g_gauntlet_options.best_of = 1;

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "Ante", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf((char *)s.value_buffer, "%d", &g_gauntlet_options.ante);
    else
      g_gauntlet_options.ante = 1;

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "AllowSideboarding", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf((char *)s.value_buffer, "%d", &g_gauntlet_options.allow_sideboarding);
    else
      g_gauntlet_options.allow_sideboarding = 1;

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "GauntletLength", NULL, NULL, s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf((char *)s.value_buffer, "%d", &g_gauntlet_options.gauntlet_length);
    else
      g_gauntlet_options.gauntlet_length = 0x14;

    RegCloseKey(s.options_key);
  }
  else
  {
    g_gauntlet_options.player_deck[0] = '\0';
    g_gauntlet_options.player_random = 1;
    g_gauntlet_options.best_of = 1;
    g_gauntlet_options.ante = 1;
    g_gauntlet_options.allow_sideboarding = 1;
    g_gauntlet_options.gauntlet_length = 0x14;
  }
}

// FUNCTION: MAGIC 0x0048bace
// FUNCTION: SHANDALAR 0x004a775e
void __stdcall LoadSealedDeckRegistryOptions(void)
{
  struct
  {
    int booster_pack_index;
    char *booster_value_scan;
    int starter_pack_index;
    char *starter_value_scan;
    HKEY options_key;
    char value_buffer[0x34];
    DWORD value_size;
  } s;

  if (RegOpenKeyExA(HKEY_CURRENT_USER, s_sealed_deck_registry_path_005710e0, 0, KEY_QUERY_VALUE,
                    &s.options_key) == ERROR_SUCCESS)
  {
    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "Rounds", NULL, NULL, (BYTE *)s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf(s.value_buffer, "%d", &g_sealed_deck_options.rounds);
    else
      g_sealed_deck_options.rounds = 5;

    s.value_size = 0x32;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "Starters", NULL, NULL, (BYTE *)s.value_buffer, &s.value_size) == ERROR_SUCCESS)
    {
      sscanf(s.value_buffer, "%d", &g_sealed_deck_options.starter_count);
      s.starter_value_scan = s.value_buffer;
      s.starter_pack_index = 0;
      while (s.starter_value_scan != NULL && *s.starter_value_scan != '\0' &&
             !(g_sealed_deck_options.starter_count <= s.starter_pack_index))
      {
        while (*s.starter_value_scan >= '0' && *s.starter_value_scan <= '9')
          s.starter_value_scan++;
        while (*s.starter_value_scan == ' ' || *s.starter_value_scan == ',')
          s.starter_value_scan++;
        if (*s.starter_value_scan != '\0')
        {
          sscanf(s.starter_value_scan, "%d", &g_sealed_deck_options.starter_packs[s.starter_pack_index++]);
        }
      }
      for (; !(g_sealed_deck_options.starter_count <= s.starter_pack_index); s.starter_pack_index++)
        g_sealed_deck_options.starter_packs[s.starter_pack_index] = 2;
    }
    else
    {
      g_sealed_deck_options.starter_count = 2;
      g_sealed_deck_options.starter_packs[0] = 2;
      g_sealed_deck_options.starter_packs[1] = 2;
    }

    s.value_size = 0x32;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "Boosters", NULL, NULL, (BYTE *)s.value_buffer, &s.value_size) == ERROR_SUCCESS)
    {
      sscanf(s.value_buffer, "%d", &g_sealed_deck_options.booster_count);
      s.booster_value_scan = s.value_buffer;
      s.booster_pack_index = 0;
      while (s.booster_value_scan != NULL && *s.booster_value_scan != '\0' &&
             !(g_sealed_deck_options.booster_count <= s.booster_pack_index))
      {
        while (*s.booster_value_scan >= '0' && *s.booster_value_scan <= '9')
          s.booster_value_scan++;
        while (*s.booster_value_scan == ' ' || *s.booster_value_scan == ',')
          s.booster_value_scan++;
        if (*s.booster_value_scan != '\0')
        {
          sscanf(s.booster_value_scan, "%d", &g_sealed_deck_options.booster_packs[s.booster_pack_index++]);
        }
      }
      for (; !(g_sealed_deck_options.booster_count <= s.booster_pack_index); s.booster_pack_index++)
        g_sealed_deck_options.booster_packs[s.booster_pack_index] = 2;
    }
    else
    {
      g_sealed_deck_options.booster_count = 2;
      g_sealed_deck_options.booster_packs[0] = 2;
      g_sealed_deck_options.booster_packs[1] = 2;
    }

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "FreeLands", NULL, NULL, (BYTE *)s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf(s.value_buffer, "%d", &g_sealed_deck_options.free_lands);
    else
      g_sealed_deck_options.free_lands = 5;

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "BestOf", NULL, NULL, (BYTE *)s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf(s.value_buffer, "%d", &g_sealed_deck_options.best_of);
    else
      g_sealed_deck_options.best_of = 3;

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "Ante", NULL, NULL, (BYTE *)s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf(s.value_buffer, "%d", &g_sealed_deck_options.ante);
    else
      g_sealed_deck_options.ante = 0;

    s.value_size = 10;
    s.value_buffer[0] = '\0';
    if (RegQueryValueExA(s.options_key, "MinDeckSize", NULL, NULL, (BYTE *)s.value_buffer, &s.value_size) == ERROR_SUCCESS)
      sscanf(s.value_buffer, "%d", &g_sealed_deck_options.minimum_deck_size);
    else
      g_sealed_deck_options.minimum_deck_size = 0x28;

    RegCloseKey(s.options_key);
  }
  else
  {
    g_sealed_deck_options.rounds = 5;
    g_sealed_deck_options.starter_count = 1;
    g_sealed_deck_options.starter_packs[0] = 2;
    g_sealed_deck_options.booster_count = 2;
    g_sealed_deck_options.booster_packs[0] = 2;
    g_sealed_deck_options.booster_packs[1] = 2;
    g_sealed_deck_options.free_lands = 5;
    g_sealed_deck_options.best_of = 3;
    g_sealed_deck_options.ante = 0;
    g_sealed_deck_options.minimum_deck_size = 0x28;
  }
}

// FUNCTION: MAGIC 0x0048a8ee
// FUNCTION: SHANDALAR 0x004a657e
void save_duel_interface_options_to_registry(void)
{
  struct
  {
    BYTE *phase_stop_text;
    BYTE phase_stoppers[100];
    int step;
    int player;
    HKEY options_key;
    BYTE value_buffer[12];
    DWORD disposition;
  } s;

  if (RegCreateKeyExA(HKEY_CURRENT_USER, s_duel_options_registry_path_00588100, 0, NULL, 0, 0xf003f,
                      NULL, &s.options_key, &s.disposition) == ERROR_SUCCESS)
  {
    wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.layout);
    RegSetValueExA(s.options_key, "Layout", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.directive_tracks_mouse);
    RegSetValueExA(s.options_key, "DirectiveTracksMouse", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.show_coin_flips);
    RegSetValueExA(s.options_key, "ShowCoinFlips", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.show_cue_cards);
    RegSetValueExA(s.options_key, "ShowCueCards", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.show_power_toughness_on_cards);
    RegSetValueExA(s.options_key, "ShowPowerToughnessOnCards", 0, REG_SZ, s.value_buffer,
                   strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.show_abilities_on_cards);
    RegSetValueExA(s.options_key, "ShowAbilitiesOnCards", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.show_id_tags_on_cards);
    RegSetValueExA(s.options_key, "ShowIDTagsOnCards", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.show_invisible_effect_cards);
    RegSetValueExA(s.options_key, "ShowInvisibleEffectCards", 0, REG_SZ, s.value_buffer,
                   strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.show_all_cards_summon_sickness);
    RegSetValueExA(s.options_key, "ShowAllCardsSummonSickness", 0, REG_SZ, s.value_buffer,
                   strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.expand_text_box_on_big_card);
    RegSetValueExA(s.options_key, "ExpandTextBoxOnBigCard", 0, REG_SZ, s.value_buffer,
                   strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.see_next_draws_at_end_of_duel);
    RegSetValueExA(s.options_key, "SeeNextDrawsAtEndOfDuel", 0, REG_SZ, s.value_buffer,
                   strlen((char *)s.value_buffer) + 1);

    s.phase_stop_text = s.phase_stoppers;
    for (s.player = 0; s.player < 2; s.player++)
    {
      for (s.step = 0; s.step < 0x25; s.step++)
      {
        if (((int)(char)g_duel_phase_stop_settings[s.player].phase_flags[s.step] & PHASE_STOP_ENABLED) != 0)
        {
          *s.phase_stop_text = 'S';
          s.phase_stop_text++;
        }
        else
        {
          *s.phase_stop_text = '-';
          s.phase_stop_text++;
        }
      }
    }
    *s.phase_stop_text = '\0';
    RegSetValueExA(s.options_key, "PhaseStoppers", 0, REG_SZ, s.phase_stoppers,
                   strlen((char *)s.phase_stoppers) + 1);

    if ((g_duel_mode_flags & 1) == 0)
    {
      wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.player_territory_color);
      RegSetValueExA(s.options_key, "PlayerTerritoryColor", 0, REG_SZ, s.value_buffer,
                     strlen((char *)s.value_buffer) + 1);
    }

    if ((g_duel_mode_flags & 1) == 0)
    {
      wsprintfA((char *)s.value_buffer, "%d", g_duel_interface_options.player_territory_type);
      RegSetValueExA(s.options_key, "PlayerTerritoryType", 0, REG_SZ, s.value_buffer,
                     strlen((char *)s.value_buffer) + 1);
    }

    RegFlushKey(s.options_key);
    RegCloseKey(s.options_key);
  }
}

// FUNCTION: MAGIC 0x004ee0dc
// FUNCTION: SHANDALAR 0x005030bd
int save_or_load_data(void *buf, unsigned int count)
{
  int ok;

  ok = 1;
  if (global_saveload_loading)
  {
    if ((unsigned int)_read(g_save_file_fd, buf, count) != count)
    {
      ok = 0;
    }
  }
  else
  {
    ok = write_save_data(g_save_file_fd, buf, count);
  }

  if (ok == 0)
  {
    OutputDebugStringA("SHIT\n");
  }

  return ok;
}

// FUNCTION: MAGIC 0x004ed32e
// FUNCTION: SHANDALAR 0x005022fd
unsigned int save_or_load_ver1(void)
{
  unsigned int result;

  result = 1;
  result &= save_or_load_data((char *)global_cards_data + g_card_count * 0x48, 0x480);
  result &= save_or_load_data(initial_library, 0xc80);
  result &= save_or_load_data(&unk_00742fcc, 4);
  result &= save_or_load_data(&DAT_00715fa4, 4);
  result &= save_or_load_data(&DAT_0072c8e0, 4);
  result &= save_or_load_data(&stop_phase, 4);
  result &= save_or_load_data(&stop_phase_player, 4);
  result &= save_or_load_data(&unk_00715fb0, 4);
  result &= save_or_load_data(&DAT_007161cc, 4);
  result &= save_or_load_data(&phase_stop_suppressed, 4);
  result &= save_or_load_data(&ai_action_replay_available, 4);
  result &= save_or_load_data(&DAT_0093f4b0, 4);
  result &= save_or_load_data(&DAT_00742f68, 4);
  result &= save_or_load_data(&_DAT_00743020, 4);
  result &= save_or_load_data(&_DAT_00742fbc, 4);
  result &= save_or_load_data(&_DAT_00743024, 4);
  result &= save_or_load_data(&DAT_007161c8, 4);
  result &= save_or_load_data(&_DAT_00743088, 4);
  result &= save_or_load_data(&DAT_00742f64, 4);
  result &= save_or_load_data(&unk_00742f60, 4);
  result &= save_or_load_data(&DAT_007161d0, 4);
  result &= save_or_load_data(special_mana_pool, 0x40);
  result &= save_or_load_data(raw_mana_available_hex, 0x198);
  result &= save_or_load_data(mana_color_conversions, 0x58);
  result &= save_or_load_data(unk_00743080, 8);
  result &= save_or_load_data(raw_mana_available, 0x40);
  result &= save_or_load_data(basiclandtypes_controlled, 0x40);
  result &= save_or_load_data(&g_next_duel_life_delta, 4);
  result &= save_or_load_data(&player_starting_life, 4);
  result &= save_or_load_data(&one_deck_mode, 4);
  result &= save_or_load_data(&duel_active, 4);
  result &= save_or_load_data(&unk_007161d8, 4);
  result &= save_or_load_data(&encounter_opening_hand_size_modifier, 4);
  result &= save_or_load_data(&DAT_00742fc0, 4);
  result &= save_or_load_data(&deck, 2000);
  result &= save_or_load_data(global_card_instances, 0x161e8);
  result &= save_or_load_data(unk_0093b280, 0x25c0);
  result &= save_or_load_data(global_graveyard_slots, 4000);
  result &= save_or_load_data(global_exile, 4000);
  result &= save_or_load_data(global_library, 4000);
  result &= save_or_load_data(global_ante_cards, 0x80);
  result &= save_or_load_data(&DAT_0093a848, 4);
  result &= save_or_load_data(&g_duel_extra_turn_player, 4);
  result &= save_or_load_data(&ai_recorded_choice, 4);
  result &= save_or_load_data(&ai_recorded_action, 4);
  result &= save_or_load_data(DAT_008cfd70, 0x60);
  result &= save_or_load_data(&DAT_00789498, 8);
  result &= save_or_load_data(mana_charge, 0x20);
  result &= save_or_load_data(&DAT_0091b2c4, 4);
  result &= save_or_load_data(&ai_combat_value_weights, 0x10);
  result &= save_or_load_data(&spell_fizzled, 4);
  result &= save_or_load_data(active_cards_count, 8);
  result &= save_or_load_data(&ai_search_flags, 4);
  result &= save_or_load_data(&DAT_007a7c60, 100);
  result &= save_or_load_data(life, 8);
  result &= save_or_load_data(&DAT_007abce0, 8);
  result &= save_or_load_data(&DAT_00938e38, 8);
  result &= save_or_load_data(unk_007a7d80, 0x10);
  result &= save_or_load_data(&current_player, 4);
  result &= save_or_load_data(&current_encounter_color, 4);
  result &= save_or_load_data(&ai_modifier, 4);
  result &= save_or_load_data(&current_phase, 4);
  result &= save_or_load_data(&land_can_be_played, 4);
  result &= save_or_load_data(&attacking_creature_count, 4);
  result &= save_or_load_data(&DAT_00789714, 4);
  result &= save_or_load_data(&DAT_008951c8, 4);
  result &= save_or_load_data(&current_encounter_strength, 4);
  result &= save_or_load_data(&x_value, 4);
  result &= save_or_load_data(&max_x_value, 4);
  result &= save_or_load_data(&active_player, 4);
  result &= save_or_load_data(&other_player, 4);
  result &= save_or_load_data(&affected_card_controller, 4);
  result &= save_or_load_data(&affected_card, 4);
  result &= save_or_load_data(&affected_internal_card_id, 4);
  result &= save_or_load_data(&affected_card_color, 4);
  result &= save_or_load_data(&attacking_card_controller, 4);
  result &= save_or_load_data(&attacking_card, 4);
  result &= save_or_load_data(&event_result, 4);
  result &= save_or_load_data(&ai_search_force_pass, 4);
  result &= save_or_load_data(&ai_search_target_depth, 4);
  result &= save_or_load_data(&current_casting_player, 4);
  result &= save_or_load_data(&current_casting_internal_card_id, 4);
  result &= save_or_load_data(&DAT_00925d30, 4);
  result &= save_or_load_data(&DAT_008a8d6c, 4);
  result &= save_or_load_data(&DAT_007aadf0, 4);
  result &= save_or_load_data(&produced_mana_color, 4);
  result &= save_or_load_data(&current_spell_player, 4);
  result &= save_or_load_data(&current_spell_card, 4);
  result &= save_or_load_data(&unk_008cfd20, 4);
  result &= save_or_load_data(&trigger_condition, 4);
  result &= save_or_load_data(&trigger_cause_controller, 4);
  result &= save_or_load_data(&trigger_cause, 4);
  result &= save_or_load_data(&card_on_stack_controller, 4);
  result &= save_or_load_data(&card_on_stack, 4);
  result &= save_or_load_data(&current_turn, 4);
  result &= save_or_load_data(&current_action_event_code, 4);
  result &= save_or_load_data(&opponent_starting_card_id_2, 4);
  result &= save_or_load_data(&opponent_starting_card_id_1, 4);
  result &= save_or_load_data(&DAT_00925ac4, 4);
  result &= save_or_load_data(&DAT_007abc00, 0x40);
  result &= save_or_load_data(&DAT_008a8de4, 4);
  result &= save_or_load_data(&DAT_008cdab0, 8);
  result &= save_or_load_data(&DAT_0091bf70, 0x40);
  result &= save_or_load_data(&ai_search_best_score, 4);
  result &= save_or_load_data(&DAT_008ce4f0, 4);
  result &= save_or_load_data(&DAT_0077784c, 4);
  result &= save_or_load_data(&DAT_0093d850, 4);
  result &= save_or_load_data(&DAT_007ab2bc, 4);
  result &= save_or_load_data(&DAT_00791410, 4);
  result &= save_or_load_data(&unk_00777850, 4);
  result &= save_or_load_data(&unk_0091bfb4, 4);
  result &= save_or_load_data(unk_008cf1c0, 0x40);
  result &= save_or_load_data(&DAT_008cf690, 0x40);
  result &= save_or_load_data(unk_00939520, 0x40);
  result &= save_or_load_data(&card_types_in_play, 8);
  result &= save_or_load_data(&DAT_008b42e8, 8);
  result &= save_or_load_data(&unk_008b2880, 4);
  result &= save_or_load_data(&ai_search_time_limit, 4);
  result &= save_or_load_data(&ai_decision_code, 4);
  result &= save_or_load_data(unk_007a7c58, 8);
  result &= save_or_load_data(&opponent_deck_color_filter_by_color, 0x30);
  result &= save_or_load_data(&DAT_008b33fc, 4);
  result &= save_or_load_data(&unk_00938e2c, 4);
  result &= save_or_load_data(&first_turn_draw_pending, 4);
  result &= save_or_load_data(&starting_player_was_random, 4);
  result &= save_or_load_data(unk_007a79b0, 8);
  result &= save_or_load_data(stack_data, 0x80);
  result &= save_or_load_data(global_stack_cards, 0x100);
  result &= save_or_load_data(global_stack_damage_targets, 0x100);
  result &= save_or_load_data(stack_trigger_condition, 0x80);
  result &= save_or_load_data(stack_prompt_flags, 0x80);
  result &= save_or_load_data(&stack_size, 4);
  result &= save_or_load_data(&unk_008b3270, 4);
  result &= save_or_load_data(&unk_008cc844, 4);
  result &= save_or_load_data(&unk_00789a48, 4);
  result &= save_or_load_data(&last_single_card_event_result, 4);
  result &= save_or_load_data(&DAT_00777aa8, 4);
  result &= save_or_load_data(&unk_007a7c1c, 4);
  result &= save_or_load_data(&ai_search_try_count, 4);
  result &= save_or_load_data(TENTATIVE_timestamp_player, 2000);
  result &= save_or_load_data(TENTATIVE_timestamp_card, 2000);
  result &= save_or_load_data(&unk_00939330, 4);
  result &= save_or_load_data(&unk_0091c4fc, 4);
  result &= save_or_load_data(&combat_assignment_cancelled, 4);
  result &= save_or_load_data(&trigger_dispatch_depth, 4);
  result &= save_or_load_data(&max_trigger_dispatch_depth, 4);
  result &= save_or_load_data(&nested_trigger_depth, 4);
  result &= save_or_load_data(&response_action_taken, 4);
  result &= save_or_load_data(&opponent_initial_library_index, 4);
  result &= save_or_load_data(&g_selected_wizard_color, 4);
  result &= save_or_load_data(&g_deck_color_bitmap, 4);
  result &= save_or_load_data(&g_shandalar_difficulty, 4);
  result &= save_or_load_data(&skip_new_game_state_initialization, 4);
  result &= save_or_load_data(&g_lair_or_monster_slots, 0xc0);
  result &= save_or_load_data(&g_world_player_x, 4);
  result &= save_or_load_data(&g_world_player_y, 4);
  result &= save_or_load_data(&g_monster_timer, 4);
  result &= save_or_load_data(&g_town_slots, 0x3200);
  result &= save_or_load_data(&g_current_quest_type, 4);
  result &= save_or_load_data(&g_current_quest_destination, 4);
  result &= save_or_load_data(&g_current_quest_color, 4);
  result &= save_or_load_data(&g_current_quest_data, 4);
  result &= save_or_load_data(&g_current_quest_deadline, 4);
  result &= save_or_load_data(&g_amulet_inventory, 0x14);
  result &= save_or_load_data(&Gold, 4);
  result &= save_or_load_data(&g_food, 4);
  result &= save_or_load_data(&g_duel_victory_log, 1000);
  result &= save_or_load_data(&g_world_magic_bitmap, 4);
  result &= save_or_load_data(&Scards, 0xc0);
  result &= save_or_load_data(&g_defeated_wizards_bitmap, 4);
  result &= save_or_load_data(&g_castle_dungeon_slots, 0x2d0);
  result &= save_or_load_data(&g_siege_timer, 4);
  result &= save_or_load_data(&g_lairs_explored, 4);
  result &= save_or_load_data(&g_quest_restock_timer, 4);
  result &= save_or_load_data(&g_player_is_male, 4);
  result &= save_or_load_data(&g_starting_color, 4);
  result &= save_or_load_data(&g_next_duel_life_delta, 4);
  result &= save_or_load_data(&g_next_duel_card_id, 4);
  result &= save_or_load_data(&_currentDeck, 4);
  result &= save_or_load_data(&g_journal_entries, 32000);
  result &= save_or_load_data(&g_journal_entry_count, 4);
  result &= save_or_load_data(&g_duel_interface_options.player_territory_color, 4);
  result &= save_or_load_data(&g_duel_interface_options.player_territory_type, 4);
  result &= save_or_load_data(&g_last_duel_player_primary_color, 4);
  result &= save_or_load_data(&g_last_duel_enemy_primary_color, 4);
  result &= save_or_load_data(&g_last_duel_result_state, 4);
  result &= save_or_load_data(&g_name_entry_buffer, 0x40);
#ifdef SHANDALAR
  strcpy(g_player_name, g_name_entry_buffer);
#endif
  result &= save_or_load_data(&g_siege_indicator, 4);

  return result;
}

// FUNCTION: MAGIC 0x004ee3cb
// FUNCTION: SHANDALAR 0x005033bb
unsigned int save_or_load_ver2(void)
{
  unsigned int result;
  struct
  {
    int card_index;
    int card_type;
  } s;

  result = 1;
  result &= save_or_load_data((char *)global_cards_data + g_card_count * 0x48, 0x480);
  result &= save_or_load_data(initial_library, 0xc80);
  result &= save_or_load_data(&unk_00742fcc, 4);
  result &= save_or_load_data(&DAT_00715fa4, 4);
  result &= save_or_load_data(&DAT_0072c8e0, 4);
  result &= save_or_load_data(&stop_phase, 4);
  result &= save_or_load_data(&stop_phase_player, 4);
  result &= save_or_load_data(&unk_00715fb0, 4);
  result &= save_or_load_data(&DAT_007161cc, 4);
  result &= save_or_load_data(&phase_stop_suppressed, 4);
  result &= save_or_load_data(&ai_action_replay_available, 4);
  result &= save_or_load_data(&DAT_0093f4b0, 4);
  result &= save_or_load_data(&DAT_00742f68, 4);
  result &= save_or_load_data(&_DAT_00743020, 4);
  result &= save_or_load_data(&_DAT_00742fbc, 4);
  result &= save_or_load_data(&_DAT_00743024, 4);
  result &= save_or_load_data(&DAT_007161c8, 4);
  result &= save_or_load_data(&_DAT_00743088, 4);
  result &= save_or_load_data(&DAT_00742f64, 4);
  result &= save_or_load_data(&unk_00742f60, 4);
  result &= save_or_load_data(&DAT_007161d0, 4);
  result &= save_or_load_data(special_mana_pool, 0x40);
  result &= save_or_load_data(raw_mana_available_hex, 0x198);
  result &= save_or_load_data(mana_color_conversions, 0x58);
  result &= save_or_load_data(unk_00743080, 8);
  result &= save_or_load_data(raw_mana_available, 0x40);
  result &= save_or_load_data(basiclandtypes_controlled, 0x40);
  result &= save_or_load_data(&g_next_duel_life_delta, 4);
  result &= save_or_load_data(&player_starting_life, 4);
  result &= save_or_load_data(&one_deck_mode, 4);
  result &= save_or_load_data(&duel_active, 4);
  result &= save_or_load_data(&unk_007161d8, 4);
  result &= save_or_load_data(&encounter_opening_hand_size_modifier, 4);
  result &= save_or_load_data(&DAT_00742fc0, 4);
  result &= save_or_load_data(&deck, 2000);
  result &= save_or_load_data(global_card_instances, 0x161e8);
  result &= save_or_load_data(unk_0093b280, 0x25c0);
  result &= save_or_load_data(global_graveyard_slots, 4000);
  result &= save_or_load_data(global_exile, 4000);
  result &= save_or_load_data(global_library, 4000);
  result &= save_or_load_data(global_ante_cards, 0x80);
  result &= save_or_load_data(&DAT_0093a848, 4);
  result &= save_or_load_data(&g_duel_extra_turn_player, 4);
  result &= save_or_load_data(&ai_recorded_choice, 4);
  result &= save_or_load_data(&ai_recorded_action, 4);
  result &= save_or_load_data(DAT_008cfd70, 0x60);
  result &= save_or_load_data(&DAT_00789498, 8);
  result &= save_or_load_data(mana_charge, 0x20);
  result &= save_or_load_data(&DAT_0091b2c4, 4);
  result &= save_or_load_data(&ai_combat_value_weights, 0x10);
  result &= save_or_load_data(&spell_fizzled, 4);
  result &= save_or_load_data(active_cards_count, 8);
  result &= save_or_load_data(&ai_search_flags, 4);
  result &= save_or_load_data(&DAT_007a7c60, 100);
  result &= save_or_load_data(life, 8);
  result &= save_or_load_data(&DAT_007abce0, 8);
  result &= save_or_load_data(unk_008b44d0, 8);
  result &= save_or_load_data(&DAT_00938e38, 8);
  result &= save_or_load_data(unk_007a7d80, 0x10);
  result &= save_or_load_data(&current_player, 4);
  result &= save_or_load_data(&current_encounter_color, 4);
  result &= save_or_load_data(&ai_modifier, 4);
  result &= save_or_load_data(&current_phase, 4);
  result &= save_or_load_data(&land_can_be_played, 4);
  result &= save_or_load_data(&attacking_creature_count, 4);
  result &= save_or_load_data(&DAT_00789714, 4);
  result &= save_or_load_data(&DAT_008951c8, 4);
  result &= save_or_load_data(&current_encounter_strength, 4);
  result &= save_or_load_data(&x_value, 4);
  result &= save_or_load_data(&max_x_value, 4);
  result &= save_or_load_data(&active_player, 4);
  result &= save_or_load_data(&other_player, 4);
  result &= save_or_load_data(&affected_card_controller, 4);
  result &= save_or_load_data(&affected_card, 4);
  result &= save_or_load_data(&affected_internal_card_id, 4);
  result &= save_or_load_data(&affected_card_color, 4);
  result &= save_or_load_data(&attacking_card_controller, 4);
  result &= save_or_load_data(&attacking_card, 4);
  result &= save_or_load_data(&event_result, 4);
  result &= save_or_load_data(&ai_search_force_pass, 4);
  result &= save_or_load_data(&ai_search_target_depth, 4);
  result &= save_or_load_data(&current_casting_player, 4);
  result &= save_or_load_data(&current_casting_internal_card_id, 4);
  result &= save_or_load_data(&DAT_00925d30, 4);
  result &= save_or_load_data(&DAT_008a8d6c, 4);
  result &= save_or_load_data(&DAT_007aadf0, 4);
  result &= save_or_load_data(&produced_mana_color, 4);
  result &= save_or_load_data(&current_spell_player, 4);
  result &= save_or_load_data(&current_spell_card, 4);
  result &= save_or_load_data(&unk_008cfd20, 4);
  result &= save_or_load_data(&trigger_condition, 4);
  result &= save_or_load_data(&trigger_cause_controller, 4);
  result &= save_or_load_data(&trigger_cause, 4);
  result &= save_or_load_data(&card_on_stack_controller, 4);
  result &= save_or_load_data(&card_on_stack, 4);
  result &= save_or_load_data(&current_turn, 4);
  result &= save_or_load_data(&current_action_event_code, 4);
  result &= save_or_load_data(&opponent_starting_card_id_2, 4);
  result &= save_or_load_data(&opponent_starting_card_id_1, 4);
  result &= save_or_load_data(&DAT_00925ac4, 4);
  result &= save_or_load_data(&DAT_007abc00, 0x40);
  result &= save_or_load_data(&DAT_008a8de4, 4);
  result &= save_or_load_data(&DAT_008cdab0, 8);
  result &= save_or_load_data(&DAT_0091bf70, 0x40);
  result &= save_or_load_data(&ai_search_best_score, 4);
  result &= save_or_load_data(&DAT_008ce4f0, 4);
  result &= save_or_load_data(&DAT_0077784c, 4);
  result &= save_or_load_data(&DAT_0093d850, 4);
  result &= save_or_load_data(&DAT_007ab2bc, 4);
  result &= save_or_load_data(&DAT_00791410, 4);
  result &= save_or_load_data(&unk_00777850, 4);
  result &= save_or_load_data(&unk_0091bfb4, 4);
  result &= save_or_load_data(unk_008cf1c0, 0x40);
  result &= save_or_load_data(&DAT_008cf690, 0x40);
  result &= save_or_load_data(unk_00939520, 0x40);
  result &= save_or_load_data(&card_types_in_play, 8);
  result &= save_or_load_data(&DAT_008b42e8, 8);
  result &= save_or_load_data(&unk_008b2880, 4);
  result &= save_or_load_data(&ai_search_time_limit, 4);
  result &= save_or_load_data(&ai_decision_code, 4);
  result &= save_or_load_data(unk_007a7c58, 8);
  result &= save_or_load_data(&opponent_deck_color_filter_by_color, 0x30);
  result &= save_or_load_data(&DAT_008b33fc, 4);
  result &= save_or_load_data(&unk_00938e2c, 4);
  result &= save_or_load_data(&first_turn_draw_pending, 4);
  result &= save_or_load_data(&starting_player_was_random, 4);
  result &= save_or_load_data(unk_007a79b0, 8);
  result &= save_or_load_data(stack_data, 0x80);
  result &= save_or_load_data(global_stack_cards, 0x100);
  result &= save_or_load_data(global_stack_damage_targets, 0x100);
  result &= save_or_load_data(stack_trigger_condition, 0x80);
  result &= save_or_load_data(stack_prompt_flags, 0x80);
  result &= save_or_load_data(&stack_size, 4);
  result &= save_or_load_data(&unk_008b3270, 4);
  result &= save_or_load_data(&unk_008cc844, 4);
  result &= save_or_load_data(&unk_00789a48, 4);
  result &= save_or_load_data(&last_single_card_event_result, 4);
  result &= save_or_load_data(&DAT_00777aa8, 4);
  result &= save_or_load_data(&unk_007a7c1c, 4);
  result &= save_or_load_data(&ai_search_try_count, 4);
  result &= save_or_load_data(TENTATIVE_timestamp_player, 2000);
  result &= save_or_load_data(TENTATIVE_timestamp_card, 2000);
  result &= save_or_load_data(&unk_00939330, 4);
  result &= save_or_load_data(&unk_0091c4fc, 4);
  result &= save_or_load_data(&combat_assignment_cancelled, 4);
  result &= save_or_load_data(&trigger_dispatch_depth, 4);
  result &= save_or_load_data(&max_trigger_dispatch_depth, 4);
  result &= save_or_load_data(&nested_trigger_depth, 4);
  result &= save_or_load_data(&response_action_taken, 4);
  result &= save_or_load_data(&opponent_initial_library_index, 4);
  result &= save_or_load_data(&g_selected_wizard_color, 4);
  result &= save_or_load_data(&g_deck_color_bitmap, 4);
  result &= save_or_load_data(&g_shandalar_difficulty, 4);
  result &= save_or_load_data(&skip_new_game_state_initialization, 4);
  result &= save_or_load_data(&g_duel_interface_options.player_territory_color, 4);
  result &= save_or_load_data(&g_duel_interface_options.player_territory_type, 4);
  result &= save_or_load_data(&g_last_duel_player_primary_color, 4);
  result &= save_or_load_data(&g_last_duel_enemy_primary_color, 4);
  result &= save_or_load_data(&g_last_duel_result_state, 4);
  result &= save_or_load_data(&_PlayerFace, 4);
  result &= save_or_load_data(&_OpponFace, 4);
  result &= save_or_load_data(&DAT_008ce570, 0x104);
  result &= save_or_load_data(&DAT_008b3fc0, 0x104);
  result &= save_or_load_data(unk_009266d0, 0x32);
  result &= save_or_load_data(&g_player_name, 0x32);
  result &= save_or_load_data(&DAT_008cf6e0, 0x640);
  result &= save_or_load_data(&DAT_00925450, 0x640);

  if (global_saveload_loading == 1)
  {
    for (s.card_index = g_card_count; g_card_count + 0x10 > s.card_index; ++s.card_index)
    {
      if (*(int *)((char *)global_cards_data + s.card_index * 0x48 + 0x24) != -1)
      {
        s.card_type = CardTypeFromID(*(int *)((char *)global_cards_data + s.card_index * 0x48 + 0x24));
        *(int *)((char *)global_cards_data + s.card_index * 0x48 + 0x34) =
            *(int *)((char *)global_cards_data + s.card_type * 0x48 + 0x34);
      }
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x004ee1d1
// FUNCTION: SHANDALAR 0x005031c2
void save_gametype0(char *path)
{
  g_save_file_fd = _open(path, 0x8301, 0x80);
  if (g_save_file_fd != -1)
  {
    global_saveload_loading = 0;
    save_or_load_data(&DAT_007a7d68, 4);
    save_or_load_ver1();
    save_or_load_data(&_PlayerFace, 4);
    save_or_load_data(&_OpponFace, 4);
    save_or_load_data(unk_009266d0, 0x32);
    save_or_load_data(&g_player_name, 0x32);
    _close(g_save_file_fd);
  }
}

// FUNCTION: MAGIC 0x004c09b0
// FUNCTION: SHANDALAR 0x004ece40
int int_to_hex_digit(int value)
{
  if (value >= 0 && value <= 9)
    return value + 0x30;
  else if (value >= 10 && value <= 0xf)
    return value + 0x57;
  else
    return 0;
}

// FUNCTION: MAGIC 0x004ed186
int get_duel_save_slot_source(void)
{
  return g_save_path_needs_init;
}

// FUNCTION: MAGIC 0x004ed1ad
unsigned int load_duel_save_file_ver1(char *path)
{
  int player;
  int card;

  strcpy(path + 9, "SVE");
  g_save_file_fd = _open(path, 0x8000);
  if (g_save_file_fd == -1)
  {
    return 0;
  }

  global_saveload_loading = 1;
  save_or_load_ver1();
  _close(g_save_file_fd);
  for (player = 0; player < 2; player++)
  {
    for (card = 0; card < 0x96; card++)
    {
      if (global_card_instances[player][card].internal_card_id != -1)
        active_cards_count[player] = card;
    }
  }
  return 1;
}

// FUNCTION: MAGIC 0x004ecf12
// FUNCTION: SHANDALAR 0x005018e8
unsigned int load_or_probe_duel_save_slot(char *path, int probe_only)
{
  strcpy(path + 9, "SVE");
  if (probe_only != 0)
  {
    g_save_file_fd = _open(path, 0x8000);
    if (g_save_file_fd != -1)
    {
      strcat(g_ui_message_buffer, "OK\n");
    }
    else
    {
#ifdef SHANDALAR
      sprintf(g_ui_message_buffer, "%s\n", gs_loadsave_0077d1b0[2]);
#else
      sprintf(g_ui_message_buffer, "%s\n", _DAT_00742fb8);
#endif
    }
    _close(g_save_file_fd);
    return (g_save_file_fd != -1);
  }

#ifdef SHANDALAR
  return (unsigned int)LoadGameFromPath(path);
#else
  return load_duel_save_file_ver1(path);
#endif

  return 1;
}

// FUNCTION: MAGIC 0x004ecdc0
// FUNCTION: SHANDALAR 0x00501760
int load_selected_duel_save_slot(int player)
{
#ifdef SHANDALAR
  struct
  {
    int save_drive_index;
    unsigned int slot_index;
  } s;

  global_saveload_loading = 1;
  HideMouseCursorNested();
  s.save_drive_index = GetSaveDriveIndex();
  if (s.save_drive_index != -1)
  {
    if (player == -1)
    {
      strcpy(g_ui_message_buffer, "\x8cSelect Load File...\n");
      g_load_menu_valid_slot_mask = 0;

      for (s.slot_index = 0; s.slot_index < 10; s.slot_index = s.slot_index + 1)
      {
        g_save_file_path[7] = (char)int_to_hex_digit(s.slot_index);
        if (load_or_probe_duel_save_slot(g_save_file_path, 1) != 0)
        {
          g_load_menu_valid_slot_mask |= (1 << (unsigned char)s.slot_index);
        }
      }

      ShowMouseCursorNested();
      g_selected_save_slot_index = RunTextMenuAt(g_ui_message_buffer, 0x30, 0x40);
      HideMouseCursorNested();
      if ((g_load_menu_valid_slot_mask & (1 << (unsigned char)g_selected_save_slot_index)) == 0)
      {
        g_selected_save_slot_index = -1;
      }
    }
    else
    {
      g_selected_save_slot_index = player;
    }

    if (g_selected_save_slot_index != -1)
    {
      g_save_file_path[7] = (char)int_to_hex_digit(g_selected_save_slot_index);
      if (load_or_probe_duel_save_slot(g_save_file_path, 0) == 0)
      {
        g_selected_save_slot_index = -1;
      }
    }

    if (g_selected_save_slot_index == -1)
    {
      RunTextMenuAt("Error Loading Save Game File:EXITING\n ", 100, 0x50);
      exit(1);
    }

    ShowMouseCursorNested();
    return g_selected_save_slot_index;
  }
  else
  {
    ShowMouseCursorNested();
    return -1;
  }
#else
  struct
  {
    int save_slot_source;
    unsigned int slot;
  } s;

  global_saveload_loading = 1;
  s.save_slot_source = get_duel_save_slot_source();
  if (s.save_slot_source != -1)
  {
    if (player == -1)
    {
      strcpy(g_ui_message_buffer, "\x8cSelect Load File...\n");
      g_load_menu_valid_slot_mask = 0;
      for (s.slot = 0; s.slot < 10; s.slot++)
      {
        g_save_file_path[7] = (char)int_to_hex_digit(s.slot);
        if (load_or_probe_duel_save_slot(g_save_file_path, 1) != 0)
          g_load_menu_valid_slot_mask |= 1 << (char)s.slot;
      }
      g_selected_save_slot_index = 0;
      if ((g_load_menu_valid_slot_mask & (1 << (char)g_selected_save_slot_index)) == 0)
        g_selected_save_slot_index = -1;
    }
    else
    {
      g_selected_save_slot_index = player;
    }

    if (g_selected_save_slot_index != -1)
    {
      g_save_file_path[7] = (char)int_to_hex_digit(g_selected_save_slot_index);
      if (load_or_probe_duel_save_slot(g_save_file_path, 0) == 0)
        g_selected_save_slot_index = -1;
    }

    if (g_selected_save_slot_index == -1)
      exit(1);
    return g_selected_save_slot_index;
  }
  else
  {
    return -1;
  }
#endif
}

// FUNCTION: MAGIC 0x004ee26e
// FUNCTION: SHANDALAR 0x0050325f
unsigned int load_gametype0(char *path)
{
  struct
  {
    int card_index;
    unsigned int result;
    int save_version;
    int card_type;
  } s;

  s.result = 1;
  g_save_file_fd = _open(path, 0x8000);
  if (g_save_file_fd != -1)
  {
    global_saveload_loading = 1;
    s.result &= save_or_load_data(&s.save_version, 4);
    if (DAT_007a7d68 == s.save_version)
    {
      s.result &= save_or_load_ver1();
      s.result &= save_or_load_data(&_PlayerFace, 4);
      s.result &= save_or_load_data(&_OpponFace, 4);
      s.result &= save_or_load_data(unk_009266d0, 0x32);
      DAT_00926702 = 0;
      s.result &= save_or_load_data(g_player_name, 0x32);
      DAT_00789a12 = 0;
      for (s.card_index = g_card_count; s.card_index < g_card_count + 0x10; s.card_index++)
      {
        if (global_cards_data[s.card_index].id != -1)
        {
          s.card_type = CardTypeFromID(global_cards_data[s.card_index].id);
          global_cards_data[s.card_index].code_pointer = global_cards_data[s.card_type].code_pointer;
        }
      }
    }
    else
    {
      s.result = 0;
    }
    _close(g_save_file_fd);
  }
  else
  {
    s.result = 0;
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004ef0ea
// FUNCTION: SHANDALAR 0x005040da
void save_soloduel(char *path)
{
  g_save_file_fd = _open(path, 0x8301, 0x80);
  if (g_save_file_fd != -1)
  {
    global_saveload_loading = 0;
    save_or_load_data(&DAT_0057b178, 4);
    save_or_load_ver2();
    save_or_load_duel_mode_state();
    LoadSoloDuelRegistryOptions();
    save_or_load_data(&g_solo_duel_options.best_of, 4);
    save_or_load_data(&g_solo_duel_options.allow_sideboarding, 4);
    _close(g_save_file_fd);
  }
}

// FUNCTION: MAGIC 0x004ef26b
// FUNCTION: SHANDALAR 0x0050425c
void save_gauntlet(char *path)
{
  g_save_file_fd = _open(path, 0x8301, 0x80);
  if (g_save_file_fd != -1)
  {
    global_saveload_loading = 0;
    save_or_load_data(&DAT_0057b178, 4);
    save_or_load_ver2();
    save_or_load_duel_mode_state();
    LoadGauntletRegistryOptions();
    save_or_load_data(&g_gauntlet_options.best_of, 4);
    save_or_load_data(&g_gauntlet_options.allow_sideboarding, 4);
    save_or_load_data(&g_gauntlet_options.gauntlet_length, 4);
    save_or_load_data(&DAT_0091c9a8, 4);
    save_or_load_data(&DAT_008ce4fc, 4);
    save_or_load_data(&DAT_00925bf8, 4);
    save_or_load_data(&DAT_00777c10, 4);
    save_or_load_data(&DAT_00777c08, 4);
    _close(g_save_file_fd);
  }
}

// FUNCTION: MAGIC 0x004ef4ba
// FUNCTION: SHANDALAR 0x005044ac
void save_sealeddeck(char *path)
{
  g_save_file_fd = _open(path, 0x8301, 0x80);
  if (g_save_file_fd != -1)
  {
    global_saveload_loading = 0;
    save_or_load_data(&DAT_0057b178, 4);
    save_or_load_ver2();
    save_or_load_duel_mode_state();
    LoadSealedDeckRegistryOptions();
    save_or_load_data(&g_sealed_deck_options.best_of, 4);
    save_or_load_data((void *)DAT_0093d844, 0x4ae64);
    _close(g_save_file_fd);
  }
}

// FUNCTION: MAGIC 0x0048b482
// FUNCTION: SHANDALAR 0x004a7112
void save_solo_duel_options_to_registry(void)
{
  struct
  {
    HKEY options_key;
    BYTE value_buffer[12];
    DWORD disposition;
  } s;

  if (RegCreateKeyExA(HKEY_CURRENT_USER, s_solo_options_registry_path_00571284, 0, NULL, 0, 0xf003f,
                      NULL, &s.options_key, &s.disposition) == ERROR_SUCCESS)
  {
    wsprintfA((char *)s.value_buffer, "%d", g_solo_duel_options.difficulty);
    RegSetValueExA(s.options_key, "Difficulty", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    RegSetValueExA(s.options_key, "PlayerDeck", 0, REG_SZ, (BYTE *)g_solo_duel_options.player_deck,
                   strlen(g_solo_duel_options.player_deck) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_solo_duel_options.player_random);
    RegSetValueExA(s.options_key, "PlayerRandom", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    RegSetValueExA(s.options_key, "OpponentDeck", 0, REG_SZ, (BYTE *)g_solo_duel_options.opponent_deck,
                   strlen(g_solo_duel_options.opponent_deck) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_solo_duel_options.opponent_random);
    RegSetValueExA(s.options_key, "OpponentRandom", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_solo_duel_options.best_of);
    RegSetValueExA(s.options_key, "BestOf", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_solo_duel_options.ante);
    RegSetValueExA(s.options_key, "Ante", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_solo_duel_options.allow_sideboarding);
    RegSetValueExA(s.options_key, "AllowSideboarding", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    RegFlushKey(s.options_key);
    RegCloseKey(s.options_key);
  }
}

// FUNCTION: MAGIC 0x0048b91e
// FUNCTION: SHANDALAR 0x004a75af
void save_gauntlet_options_to_registry(void)
{
  struct
  {
    HKEY options_key;
    BYTE value_buffer[12];
    DWORD disposition;
  } s;

  if (RegCreateKeyExA(HKEY_CURRENT_USER, s_gauntlet_options_registry_path_00571338, 0, NULL, 0,
                      0xf003f, NULL, &s.options_key, &s.disposition) == ERROR_SUCCESS)
  {
    RegSetValueExA(s.options_key, "PlayerDeck", 0, REG_SZ, (BYTE *)g_gauntlet_options.player_deck,
                   strlen(g_gauntlet_options.player_deck) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_gauntlet_options.player_random);
    RegSetValueExA(s.options_key, "PlayerRandom", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_gauntlet_options.best_of);
    RegSetValueExA(s.options_key, "BestOf", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_gauntlet_options.ante);
    RegSetValueExA(s.options_key, "Ante", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_gauntlet_options.allow_sideboarding);
    RegSetValueExA(s.options_key, "AllowSideboarding", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    wsprintfA((char *)s.value_buffer, "%d", g_gauntlet_options.gauntlet_length);
    RegSetValueExA(s.options_key, "GauntletLength", 0, REG_SZ, s.value_buffer, strlen((char *)s.value_buffer) + 1);

    RegFlushKey(s.options_key);
    RegCloseKey(s.options_key);
  }
}

// FUNCTION: MAGIC 0x004ef622
// FUNCTION: SHANDALAR 0x00504614
int validate_loaded_duel_card_availability(void)
{
  struct
  {
    unsigned int card_id;
    int result;
    int card;
    int player;
    int internal_card_id;
  } s;

  s.result = 1;
  for (s.player = 0; s.player <= 1; s.player++)
  {
    for (s.card = 0; s.card < 0x97 && s.card <= active_cards_count[s.player]; s.card++)
    {
      s.internal_card_id = global_card_instances[s.player][s.card].original_internal_card_id;
      if (s.internal_card_id < damage_card_internal_card_id || damage_card_internal_card_id + 0x2d <= s.internal_card_id)
      {
        if (s.internal_card_id > -1)
          s.card_id = global_cards_data[s.internal_card_id].id;
        else
          s.card_id = 0xffffffff;
        if (s.card_id != 0xffffffff && s.card_id != 0x38d && is_card_available_in_installed_expansion(s.card_id) == 0)
          s.result = 0;
      }
    }

    for (s.card = 0; s.card < 500 && global_graveyard_slots[s.player][s.card] != -1; s.card++)
    {
      s.card_id = CardIDFromType(global_graveyard_slots[s.player][s.card]);
      if (s.card_id != 0xffffffff && is_card_available_in_installed_expansion(s.card_id) == 0)
        s.result = 0;
    }

    for (s.card = 0; s.card < 500 && global_library[s.player][s.card] != -1; s.card++)
    {
      s.card_id = CardIDFromType(global_library[s.player][s.card]);
      if (s.card_id != 0xffffffff && is_card_available_in_installed_expansion(s.card_id) == 0)
        s.result = 0;
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x004ef17b
// FUNCTION: SHANDALAR 0x0050416b
unsigned int load_duel_run_mode_1_save(char *path)
{
  struct
  {
    unsigned int result;
    int save_version;
  } s;

  s.result = 1;
  g_save_file_fd = _open(path, 0x8000);
  if (g_save_file_fd != -1)
  {
    global_saveload_loading = 1;
    s.result &= save_or_load_data(&s.save_version, 4);
    if (DAT_0057b178 == s.save_version)
    {
      s.result &= save_or_load_ver2();
      s.result &= save_or_load_duel_mode_state();
      s.result &= save_or_load_data(&g_solo_duel_options.best_of, 4);
      s.result &= save_or_load_data(&g_solo_duel_options.allow_sideboarding, 4);
      save_solo_duel_options_to_registry();
    }
    else
    {
      s.result = 0;
    }
    _close(g_save_file_fd);
  }
  else
  {
    s.result = 0;
  }

  if (s.result != 0 && validate_loaded_duel_card_availability() == 0)
    s.result = 0;

  return s.result;
}

// FUNCTION: MAGIC 0x004ef35a
// FUNCTION: SHANDALAR 0x0050434b
unsigned int load_duel_run_mode_2_save(char *path)
{
  struct
  {
    unsigned int result;
    int save_version;
  } s;

  s.result = 1;
  g_save_file_fd = _open(path, 0x8000);
  if (g_save_file_fd != -1)
  {
    global_saveload_loading = 1;
    s.result &= save_or_load_data(&s.save_version, 4);
    if (s.save_version == DAT_0057b178)
    {
      s.result &= save_or_load_ver2();
      s.result &= save_or_load_duel_mode_state();
      s.result &= save_or_load_data(&g_gauntlet_options.best_of, 4);
      s.result &= save_or_load_data(&g_gauntlet_options.allow_sideboarding, 4);
      s.result &= save_or_load_data(&g_gauntlet_options.gauntlet_length, 4);
      save_gauntlet_options_to_registry();
      s.result &= save_or_load_data(&DAT_0091c9a8, 4);
      s.result &= save_or_load_data(&DAT_008ce4fc, 4);
      s.result &= save_or_load_data(&DAT_00925bf8, 4);
      s.result &= save_or_load_data(&DAT_00777c10, 4);
      s.result &= save_or_load_data(&DAT_00777c08, 4);
    }
    else
    {
      s.result = 0;
    }
    _close(g_save_file_fd);
  }
  else
  {
    s.result = 0;
  }

  if (s.result != 0 && validate_loaded_duel_card_availability() == 0)
    s.result = 0;

  return s.result;
}

// FUNCTION: MAGIC 0x004ef54b
// FUNCTION: SHANDALAR 0x0050453d
unsigned int load_duel_run_mode_3_save(char *path)
{
  struct
  {
    unsigned int result;
    int save_version;
  } s;

  s.result = 1;
  g_save_file_fd = _open(path, 0x8000);
  if (g_save_file_fd != -1)
  {
    global_saveload_loading = 1;
    s.result &= save_or_load_data(&s.save_version, 4);
    if (s.save_version == DAT_0057b178)
    {
      s.result &= save_or_load_ver2();
      s.result &= save_or_load_duel_mode_state();
      s.result &= save_or_load_data(&g_sealed_deck_options.best_of, 4);
      s.result &= save_or_load_data(DAT_006abe40, 0x4ae64);
      DAT_0093d844 = DAT_006abe40;
    }
    else
    {
      s.result = 0;
    }
    _close(g_save_file_fd);
  }
  else
  {
    s.result = 0;
  }

  return s.result;
}
