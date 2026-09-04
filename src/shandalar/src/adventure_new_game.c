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

// FUNCTION: SHANDALAR 0x004184d2
void LoadTownHintMetadata(void)
{
  struct
  {
    int first_csvid;
    char difficulty_codes[8];
    int hint_index;
    int second_csvid;
    int card_index;
    char line[256];
    FILE *hints_file;
    int scan_result;
  } s;

  s.hints_file = fopen("hints.txt", "rt");
  s.hint_index = 0;
  do
  {
    s.scan_result = fscanf(s.hints_file, "%[^\n]", s.line);
    if (s.line[0] == '.')
    {
      sscanf(s.line + 1, "%d %d %s", &s.first_csvid, &s.second_csvid, s.difficulty_codes);
      g_hint_card_pairs[s.hint_index].first = s.first_csvid;
      g_hint_card_pairs[s.hint_index].second = s.second_csvid;
      g_hint_difficulty_masks[s.hint_index] = 0;

      if (strchr(s.difficulty_codes, 'A') != (char *)0)
      {
        g_hint_difficulty_masks[s.hint_index] = g_hint_difficulty_masks[s.hint_index] | 1;
      }

      if (strchr(s.difficulty_codes, 'B') != (char *)0)
      {
        g_hint_difficulty_masks[s.hint_index] = g_hint_difficulty_masks[s.hint_index] | 2;
      }

      if (strchr(s.difficulty_codes, 'C') != (char *)0)
      {
        g_hint_difficulty_masks[s.hint_index] = g_hint_difficulty_masks[s.hint_index] | 4;
      }

      if (strchr(s.difficulty_codes, 'D') != (char *)0)
      {
        g_hint_difficulty_masks[s.hint_index] = g_hint_difficulty_masks[s.hint_index] | 8;
      }

      s.card_index = find_internal_card_id_by_csv_id(s.first_csvid);
      s.card_index = find_internal_card_id_by_csv_id(s.second_csvid);
      s.scan_result = fscanf(s.hints_file, "%[\n]", s.line);
      g_hint_text_offsets[s.hint_index] = ftell(s.hints_file);
      s.hint_index = s.hint_index + 1;
    }
    else
    {
      s.scan_result = fscanf(s.hints_file, "%[\n]", s.line);
    }
  } while ((s.hint_index < 0x100) && (s.scan_result != -1));

  do
  {
    g_hint_card_pairs[s.hint_index].second = -1;
    g_hint_card_pairs[s.hint_index].first = g_hint_card_pairs[s.hint_index].second;
    s.hint_index = s.hint_index + 1;
  } while (s.hint_index < 0x100);

  fclose(s.hints_file);
}
// FUNCTION: SHANDALAR 0x0041786e
void LoadCardRaritiesAndCsvOffsets(void)
{
  struct
  {
    int icon_height_scaled;
    int icon_width_scaled;
    long selected_state_sprite;
    int location_block_start_index;
    int inner_index;
    FILE *entry_index;
    int scan_result;
  } s;

  for (s.location_block_start_index = 0; s.location_block_start_index < 0x4e2; s.location_block_start_index = s.location_block_start_index + 1)
  {
    g_master_csv_offsets[s.location_block_start_index] = -1;
  }

  s.entry_index = fopen("concise.csv", "rt");
  s.inner_index = 0;

  for (s.location_block_start_index = 0; s.location_block_start_index < g_card_count; s.location_block_start_index = s.location_block_start_index + 1)
  {
    s.inner_index = global_cards_data[s.location_block_start_index].id;
    s.scan_result = fscanf(s.entry_index, "%d %d %ld\n", &s.icon_width_scaled, &s.icon_height_scaled, &s.selected_state_sprite);
    global_cards_data[s.location_block_start_index].rarity = (unsigned char)s.icon_height_scaled;
    g_master_csv_offsets[s.inner_index] = s.selected_state_sprite;
  }

  fclose(s.entry_index);
}
// FUNCTION: SHANDALAR 0x004bdd0a
void InitializeNewGameState(void)
{
  struct
  {
    int icon_width_scaled;
    unsigned int uVar3;
    unsigned int uVar2;
    int location_block_start_index;
    int entry_index;
    int difficulty_copy;
  } s;

  if (g_duel_ai_mode_state != 0)
  {
    return;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 500; s.location_block_start_index = s.location_block_start_index + 1)
  {
    deck[s.location_block_start_index] = -1;
  }

  if (g_skip_new_game_state_initialization)
  {
    return;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 7; s.location_block_start_index = s.location_block_start_index + 1)
  {
    g_opponent_deck_color_filter_by_color[s.location_block_start_index] = 0;
    g_duel_special_rules_by_color[s.location_block_start_index] = -1;
  }

  g_world_magic_bitmap = g_world_magic_bitmap | (1 << (g_selected_wizard_color * 2));
  Scards[g_selected_wizard_color * 2].worldmagic_city = 0;

  s.uVar2 = 1 << (BYTE)g_selected_wizard_color;
  s.entry_index = g_shandalar_difficulty + 1;
  s.difficulty_copy = s.entry_index;
  if (g_shandalar_difficulty == 3)
  {
    s.entry_index = 1;
    s.entry_index = 3;
  }

  switch (g_shandalar_difficulty)
  {
  case 0:
    if ((s.uVar2 < 1) & 0x20)
    {
      AddRandomStarterDeckCards(s.uVar2, 0xd, 7, 0xf, 1, 1);
    }
    else
    {
      AddRandomStarterDeckCards(s.uVar2, 0xd, 0xc, 10, 1, 1);
    }
    break;
  case 1:
    AddRandomStarterDeckCards(s.uVar2, 0xb, 4, 0xc, 1, 1);
    AddRandomStarterDeckCards(PickRandomColorBitExcludingMask(s.uVar2), 4, 3, 4, 0, 1);
    break;
  case 2:
    AddRandomStarterDeckCards(s.uVar2, 9, 3, 9, 1, 1);
    s.uVar3 = PickRandomColorBitExcludingMask(s.uVar2);
    AddRandomStarterDeckCards(s.uVar3, 5, 3, 4, 0, 1);
    AddRandomStarterDeckCards(PickRandomColorBitExcludingMask(s.uVar2 | s.uVar3), 4, 3, 3, 0, 1);
    break;
  case 3:
    AddRandomStarterDeckCards(s.uVar2, 6, 3, 5, 1, 1);
    AddRandomStarterDeckCards(1, 0xb, 5, 0xe, 0, 1);
    break;
  }

  g_journal_entry_count = 0;
  for (s.location_block_start_index = 0; s.location_block_start_index < 5; s.location_block_start_index = s.location_block_start_index + 1)
  {
    g_amulet_inventory[s.location_block_start_index] = 0;
  }

  g_amulet_inventory[g_selected_wizard_color - 1]++;
  for (s.location_block_start_index = 0; s.location_block_start_index < 3 - g_shandalar_difficulty; s.location_block_start_index = s.location_block_start_index + 1)
  {
    s.icon_width_scaled = internal_rand(5);
    g_amulet_inventory[s.icon_width_scaled]++;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 0x80; s.location_block_start_index = s.location_block_start_index + 1)
  {
    g_town_slots[s.location_block_start_index].location_type = -1;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 8; s.location_block_start_index = s.location_block_start_index + 1)
  {
    g_lair_or_monster_slots[s.location_block_start_index].entry_type = SHANDALAR_ENTRY_NONE;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 1000; s.location_block_start_index = s.location_block_start_index + 1)
  {
    g_duel_victory_log[0] = 0;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 4; s.location_block_start_index = s.location_block_start_index + 1)
  {
    g_ai_combat_value_weights[s.location_block_start_index] = 8;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 0x96; s.location_block_start_index = s.location_block_start_index + 1)
  {
    deck[s.location_block_start_index] = deck[s.location_block_start_index] | 0x10000;
  }
  g_selected_wizard_color = -1;
}

// FUNCTION: SHANDALAR 0x004bdccc
unsigned int PickRandomColorBitExcludingMask(unsigned int excluded_mask)
{
  int entry_index;

  do
  {
    entry_index = internal_rand(5) + 1;
  } while ((excluded_mask & (1U << (entry_index & 0xff))) != 0);

  return 1U << (entry_index & 0xff);
}

// FUNCTION: SHANDALAR 0x004be0bf
int AddRandomStarterDeckCards(unsigned int color_mask, int land_count, int spell_count, int creature_count, int add_rare, int allow_artifact_spells)
{
  struct
  {
    unsigned int required_color_mask;
    unsigned int candidate_color_filter;
    unsigned int use_artifact_pool;
    unsigned int selected_card_id;
    unsigned int added_count;
    int duplicate_attempt_count;
    int card_ok;
  } s;

  for (s.added_count = 0; land_count > (int)s.added_count; s.added_count = s.added_count + 1)
  {
    s.selected_card_id = PickRandomCardMatchingTypeAndColor(1, color_mask);
    if ((is_card_color_compatible_with_mask((int)(char)global_cards_data[s.selected_card_id].color, color_mask, 0) != 0) &&
        ((int)s.selected_card_id <= 4) &&
        ((global_cards_data[s.selected_card_id].expansion & 0xc1) != 0))
    {
      AddCardToDeckSorted(s.selected_card_id);
    }
    else
    {
      s.added_count = s.added_count - 1;
    }
  }

  for (s.added_count = 0; spell_count > (int)s.added_count; s.added_count = s.added_count + 1)
  {
    if (allow_artifact_spells != 0)
    {
      s.use_artifact_pool = rand() & 1;
    }
    else
    {
      s.use_artifact_pool = 0;
    }

    if (s.use_artifact_pool != 0)
    {
      s.candidate_color_filter = 1;
    }
    else
    {
      s.candidate_color_filter = color_mask;
    }

    s.selected_card_id = PickRandomCardMatchingTypeAndColor((-(unsigned int)(s.use_artifact_pool == 0) & 0xffffffc4) + 0x40, s.candidate_color_filter);
    if (((g_shandalar_difficulty == 0) && ((global_cards_data[s.selected_card_id].static_ability & 3) != 0)) ||
        ((global_cards_data[s.selected_card_id].extra_ability & 0x900) != 0))
    {
      s.added_count = s.added_count - 1;
    }
    else
    {
      if ((global_cards_data[s.selected_card_id].type & 4) != 0)
      {
        s.use_artifact_pool = 0;
      }

      if (s.use_artifact_pool != 0)
      {
        s.required_color_mask = 1;
      }
      else
      {
        s.required_color_mask = color_mask;
      }

      if ((is_card_color_compatible_with_mask((int)(char)global_cards_data[s.selected_card_id].color, s.required_color_mask, 0) != 0) &&
          (GetCardRarity(s.selected_card_id) <= ((s.added_count & 1) == 0 ? 2 : 1)) &&
          ((global_cards_data[s.selected_card_id].expansion & 0xc1) != 0))
      {
        AddCardToDeckSorted(s.selected_card_id);
      }
      else
      {
        s.added_count = s.added_count - 1;
      }
    }
  }

  s.duplicate_attempt_count = 0;
  for (s.added_count = 0; creature_count > (int)s.added_count; s.added_count = s.added_count + 1, s.duplicate_attempt_count = s.duplicate_attempt_count + 1)
  {
    s.selected_card_id = PickRandomCardMatchingTypeAndColor(2, color_mask);
    if (((g_shandalar_difficulty <= 3) && ((global_cards_data[s.selected_card_id].static_ability & 3) != 0)) ||
        ((global_cards_data[s.selected_card_id].extra_ability & 0x900) != 0))
    {
      s.added_count = s.added_count - 1;
      s.duplicate_attempt_count = s.duplicate_attempt_count - 1;
    }
    else
    {
      if (s.duplicate_attempt_count < 1000)
      {
        s.card_ok = 1 <= GetRemainingAllowedCardCopies(s.selected_card_id);
      }
      else
      {
        s.card_ok = 1;
      }

      if ((is_card_color_compatible_with_mask((int)(char)global_cards_data[s.selected_card_id].color, color_mask, 0) != 0) &&
          (GetCardRarity(s.selected_card_id) <= ((s.added_count & 1) == 0 ? 2 : 1)) &&
          (s.card_ok && ((global_cards_data[s.selected_card_id].expansion & 0xc1) != 0)))
      {
        AddCardToDeckSorted(s.selected_card_id);
      }
      else
      {
        s.added_count = s.added_count - 1;
      }
    }
  }

  if (add_rare != 0)
  {
    do
    {
      do
      {
        s.selected_card_id = PickRandomCardMatchingTypeAndColor(0xe, 1);
      } while (is_card_color_compatible_with_mask((int)(char)global_cards_data[s.selected_card_id].color, color_mask, 1) == 0);
    } while ((GetCardRarity(s.selected_card_id) < 3) ||
             (GetRemainingAllowedCardCopies(s.selected_card_id) <= 0) ||
             ((g_shandalar_difficulty == 0 && ((global_cards_data[s.selected_card_id].static_ability & 3) != 0))) ||
             ((global_cards_data[s.selected_card_id].extra_ability & 0x900) != 0) ||
             ((global_cards_data[s.selected_card_id].expansion & 0xc1) == 0));
  }

  AddCardToDeckSorted(s.selected_card_id);
  return 0;
}

// FUNCTION: SHANDALAR 0x0056bcf7
int PickRandomCardMatchingTypeAndColor(unsigned int type_mask, unsigned int color_mask)
{
  struct
  {
    int attempt_count;
    int card_index;
    int found;
  } s;

  s.attempt_count = 0;
  do
  {
    s.found = 0;
    s.card_index = internal_rand(g_card_count - 0x39);
    if ((type_mask != 0) && ((type_mask & global_cards_data[s.card_index].type) == 0))
    {
    }
    else if ((color_mask != 1) && ((color_mask & (int)(char)global_cards_data[s.card_index].color) == 0))
    {
    }
    else
    {
      s.found = 1;
    }
  } while (s.found == 0 && ++s.attempt_count < 999);

  return s.card_index;
}

// FUNCTION: SHANDALAR 0x004bb1cf
int GetRemainingAllowedCardCopies(unsigned int card_id)
{
  struct
  {
    int color_index;
    int copies_in_deck;
    int total_non_market_cards;
    int i;
    int max_color_count;
    int color_counts[7];
  } s;

  if ((int)card_id <= 4)
  {
    return 99;
  }

  for (s.i = 0; s.i < 7; s.i = s.i + 1)
  {
    s.color_counts[s.i] = 0;
  }

  s.total_non_market_cards = 0;
  s.copies_in_deck = s.total_non_market_cards;

  for (s.i = 0; s.i < 500; s.i = s.i + 1)
  {
    if ((deck[s.i] != -1) && ((deck[s.i] & 0x4000) == 0))
    {
      s.total_non_market_cards = s.total_non_market_cards + 1;
      s.color_index = single_color_test_bit_to_color_t(global_cards_data[deck[s.i] & 0xfff].color);
      s.color_counts[s.color_index] = s.color_counts[s.color_index] + 1;
    }

    if (((unsigned int)deck[s.i] & 0xffff7fffU) == card_id)
    {
      s.copies_in_deck = s.copies_in_deck + 1;
      g_last_matching_deck_card_index = s.i;
    }
  }

  s.max_color_count = -1;
  for (s.i = 1; s.i < 7; s.i = s.i + 1)
  {
    if (s.max_color_count < s.color_counts[s.i])
    {
      s.max_color_count = s.color_counts[s.i];
    }
  }

  g_deck_color_bitmap = 0;
  for (s.i = 1; s.i < 7; s.i = s.i + 1)
  {
    if (((s.max_color_count * 2) / 3) <= s.color_counts[s.i])
    {
      g_deck_color_bitmap = g_deck_color_bitmap | (1 << (unsigned char)s.i);
    }
  }

  s.max_color_count = 1;
  if (s.total_non_market_cards >= 0x14)
  {
    s.max_color_count = 1;
  }
  if (s.total_non_market_cards >= 0x28)
  {
    s.max_color_count = 2;
  }
  if (s.total_non_market_cards >= 0x3c)
  {
    s.max_color_count = 3;
  }
  if ((g_world_magic_bitmap & 0x20) != 0)
  {
    s.max_color_count = s.max_color_count + 1;
  }

  if ((global_cards_data[card_id].extra_ability & 0x100) != 0)
  {
    s.max_color_count = (g_shandalar_difficulty <= s.max_color_count);
  }
  else if ((global_cards_data[card_id].expansion & 0xc1) == 0)
  {
    s.max_color_count = s.max_color_count / 2;
  }

  return s.max_color_count - s.copies_in_deck;
}

// FUNCTION: SHANDALAR 0x004bb458
int FindWorldMagicCardIndex(shandalar_worldmagic_index_t inx)
{
  int entry_index;

  for (entry_index = 0; entry_index < g_card_count + 0x10; entry_index = entry_index + 1)
  {
    if (global_cards_data[entry_index].id == Scards[inx].worldmagic_csvid)
    {
      return entry_index;
    }
  }
}

// FUNCTION: SHANDALAR 0x004290e2
void AddJournalEntry(int entry_type, int entry_arg)
{
  if (g_journal_entry_count < 2000)
  {
    g_journal_entries[g_journal_entry_count].type = entry_type;
    g_journal_entries[g_journal_entry_count].arg = entry_arg;
    g_journal_entries[g_journal_entry_count].tile_x = g_world_player_x / 32;
    g_journal_entries[g_journal_entry_count].tile_y = g_world_player_y / 32;
    g_journal_entry_count = g_journal_entry_count + 1;
  }
}

// FUNCTION: SHANDALAR 0x0052280c
int UpdateStatWindowProgress(void *progress_values)
{
  if (g_statwin_exports_by_ordinal[1] != 0)
  {
    return ((int(__cdecl *)(void *))g_statwin_exports_by_ordinal[1])(progress_values);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x0056302b
int MapWizardColorToDisplayIndex(int wizard_color)
{
  switch (wizard_color)
  {
  case 1:
    return 4;
  case 2:
    return 2;
  case 3:
    return 3;
  case 4:
    return 1;
  case 5:
    return 0;
  case 6:
    return 5;
  default:
    return -1;
  }
}

// FUNCTION: SHANDALAR 0x0056bff1
void RebuildDeckEntriesByCardGroup(void)
{
  struct
  {
    int deck_slot_index;
    int card_index;
    unsigned int saved_deck_entries[500];
    int saved_journal_entry_count;
  } s;

  for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
  {
    s.saved_deck_entries[s.card_index] = deck[s.card_index];
    deck[s.card_index] = -1;
  }
  s.saved_journal_entry_count = g_journal_entry_count;
  for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
  {
    if (s.saved_deck_entries[s.card_index] != 0xffffffff)
    {
      s.deck_slot_index = AddCardToDeckSorted(s.saved_deck_entries[s.card_index] & 0xfff);
      deck[s.deck_slot_index] |= ((int)s.saved_deck_entries[s.card_index] & -4096);
    }
  }
  g_journal_entry_count = s.saved_journal_entry_count;
}

// FUNCTION: SHANDALAR 0x004ed0bd
int RecountDeckCardTotals(void)
{
  int deck_index;

  g_deck_active_card_count = g_deck_total_card_count = 0;
  for (deck_index = 0; deck_index < 500; deck_index = deck_index + 1)
  {
    if (deck[deck_index] != -1)
    {
      g_deck_total_card_count = g_deck_total_card_count + 1;
      if ((deck[deck_index] & 0x4000) == 0)
      {
        g_deck_active_card_count++;
      }
    }
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x00559807
int RunRandomAiDuelDemo(void)
{
  int creature_type;
  int ante_index;
  int player;

  for (player = 0; player < 2; player++)
  {
    switch (internal_rand(5))
    {
    case 0:
      ClearAndLoadInitialLibraryFromDeckFile("decks\\0016.dck", player, 1, -1);
      creature_type = 2;
      break;
    case 1:
      ClearAndLoadInitialLibraryFromDeckFile("decks\\0283.dck", player, 1, -1);
      creature_type = 10;
      break;
    case 2:
      ClearAndLoadInitialLibraryFromDeckFile("decks\\0150.dck", player, 1, -1);
      creature_type = 0x10;
      break;
    case 3:
      ClearAndLoadInitialLibraryFromDeckFile("decks\\0076.dck", player, 1, -1);
      creature_type = 0x17;
      break;
    case 4:
      ClearAndLoadInitialLibraryFromDeckFile("decks\\0102.dck", player, 1, -1);
      creature_type = 0x20;
      break;
    }
  }

  g_selected_wizard_color = 0;
  g_opponent_initial_library_index = 1;
  for (ante_index = 0; ante_index < 0x10; ante_index++)
  {
    global_ante_cards[1][ante_index] = -1;
    global_ante_cards[0][ante_index] = global_ante_cards[1][ante_index];
  }

  g_duel_network_state = 1;
  RunDuelEngine(0, creature_type);
  g_duel_network_state = 0;
  RefreshAdventureInterfaceLayout();
  return 0;
}

