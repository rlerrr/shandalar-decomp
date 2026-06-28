#define GLOBAL_STATE_IMPL
#include "global_state.h"

#include <errno.h>
#include <io.h>
#include <string.h>

extern card_data_t global_cards_data[];
#ifdef SHANDALAR
extern int g_card_count;
#endif

int CardTypeFromID(int csvid);
int save_or_load_data(void *buf, unsigned int count);

// FUNCTION: MAGIC 0x004a5a1b
// FUNCTION: SHANDALAR 0x00557b9b
void SetCardInDeck(int param_1,int param_2)
{
  if (param_2 == 1) {
    deck[param_1] = deck[param_1] | 0x4000;
  }
  else {
    deck[param_1] = deck[param_1] & 0x8fff;
  }
  return;
}

// FUNCTION: MAGIC 0x004ed13a
// FUNCTION: SHANDALAR 0x00501b31
int FUN_004ed13a(int file_descriptor, void *buffer, unsigned int count)
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
unsigned int FUN_004ef073(void)
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
void __stdcall FUN_0048b144(void)
{
}

// FUNCTION: MAGIC 0x0048b69e
// FUNCTION: SHANDALAR 0x004a732e
void __stdcall FUN_0048b69e(void)
{
}

// FUNCTION: MAGIC 0x0048bace
// FUNCTION: SHANDALAR 0x004a775e
void __stdcall FUN_0048bace(void)
{
}

// FUNCTION: MAGIC 0x004ee0dc
// FUNCTION: SHANDALAR 0x005030bd
int __cdecl save_or_load_data(void *buf, unsigned int count)
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
    ok = FUN_004ed13a(g_save_file_fd, buf, count);
  }

  if (ok == 0)
  {
    OutputDebugStringA("SHIT\n");
  }

  return ok;
}

// FUNCTION: MAGIC 0x004ed32e
// FUNCTION: SHANDALAR 0x005022fd
unsigned int __cdecl save_or_load_ver1(void)
{
  unsigned int result;

  result = 1;
#ifdef SHANDALAR
  result &= save_or_load_data((char *)global_cards_data + g_card_count * 0x48, 0x480);
#else
  result &= save_or_load_data((char *)global_cards_data + unk_0093f4b8 * 0x48, 0x480);
#endif
  result &= save_or_load_data(initial_library, 0xc80);
  result &= save_or_load_data(&unk_00742fcc, 4);
  result &= save_or_load_data(&DAT_00715fa4, 4);
  result &= save_or_load_data(&DAT_0072c8e0, 4);
  result &= save_or_load_data(&unk_00716248, 4);
  result &= save_or_load_data(&unk_00716244, 4);
  result &= save_or_load_data(&unk_00715fb0, 4);
  result &= save_or_load_data(&DAT_007161cc, 4);
  result &= save_or_load_data(&DAT_00742fc8, 4);
  result &= save_or_load_data(&DAT_0072c8e4, 4);
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
  result &= save_or_load_data(unk_00742f70, 0x40);
  result &= save_or_load_data(raw_mana_available_hex, 0x198);
  result &= save_or_load_data(unk_007161e0, 0x58);
  result &= save_or_load_data(unk_00743080, 8);
  result &= save_or_load_data(raw_mana_available, 0x40);
  result &= save_or_load_data(basiclandtypes_controlled, 0x40);
  result &= save_or_load_data(&g_next_duel_life_delta, 4);
  result &= save_or_load_data(&DAT_00716024, 4);
  result &= save_or_load_data(&DAT_0074302c, 4);
  result &= save_or_load_data(&unk_00742fc4, 4);
  result &= save_or_load_data(&unk_007161d8, 4);
  result &= save_or_load_data(&DAT_00742fd0, 4);
  result &= save_or_load_data(&DAT_00742fc0, 4);
  result &= save_or_load_data(&deck, 2000);
  result &= save_or_load_data(global_card_instances, 0x161e8);
  result &= save_or_load_data(unk_0093b280, 0x25c0);
  result &= save_or_load_data(global_graveyard_slots, 4000);
  result &= save_or_load_data(global_exile, 4000);
  result &= save_or_load_data(global_library, 4000);
  result &= save_or_load_data(global_ante_cards, 0x80);
  result &= save_or_load_data(&DAT_0093a848, 4);
  result &= save_or_load_data(&DAT_00926668, 4);
  result &= save_or_load_data(&unk_00939340, 4);
  result &= save_or_load_data(&unk_00925bb8, 4);
  result &= save_or_load_data(DAT_008cfd70, 0x60);
  result &= save_or_load_data(&DAT_00789498, 8);
  result &= save_or_load_data(unk_008ce510, 0x20);
  result &= save_or_load_data(&DAT_0091b2c4, 4);
  result &= save_or_load_data(&DAT_007a7d10, 0x10);
  result &= save_or_load_data(&spell_fizzled, 4);
  result &= save_or_load_data(active_cards_count, 8);
  result &= save_or_load_data(&DAT_00777854, 4);
  result &= save_or_load_data(&DAT_007a7c60, 100);
  result &= save_or_load_data(life, 8);
  result &= save_or_load_data(&DAT_007abce0, 8);
  result &= save_or_load_data(&DAT_00938e38, 8);
  result &= save_or_load_data(unk_007a7d80, 0x10);
  result &= save_or_load_data(&human_player, 4);
  result &= save_or_load_data(&DAT_008ce538, 4);
  result &= save_or_load_data(&ai_modifier, 4);
  result &= save_or_load_data(&current_phase, 4);
  result &= save_or_load_data(&unk_008b4278, 4);
  result &= save_or_load_data(&unk_008b60e0, 4);
  result &= save_or_load_data(&DAT_00789714, 4);
  result &= save_or_load_data(&DAT_008951c8, 4);
  result &= save_or_load_data(&DAT_007a7874, 4);
  result &= save_or_load_data(&x_value, 4);
  result &= save_or_load_data(&max_x_value, 4);
  result &= save_or_load_data(&unk_008b35ec, 4);
  result &= save_or_load_data(&active_player, 4);
  result &= save_or_load_data(&affected_card_controller, 4);
  result &= save_or_load_data(&affected_card, 4);
  result &= save_or_load_data(&unk_008b4dd0, 4);
  result &= save_or_load_data(&unk_008cf6d4, 4);
  result &= save_or_load_data(&attacking_card_controller, 4);
  result &= save_or_load_data(&attacking_card, 4);
  result &= save_or_load_data(&event_result, 4);
  result &= save_or_load_data(&DAT_008a8d74, 4);
  result &= save_or_load_data(&DAT_008a8d7c, 4);
  result &= save_or_load_data(&DAT_0078930c, 4);
  result &= save_or_load_data(&DAT_00789b7c, 4);
  result &= save_or_load_data(&DAT_00925d30, 4);
  result &= save_or_load_data(&DAT_008a8d6c, 4);
  result &= save_or_load_data(&DAT_007aadf0, 4);
  result &= save_or_load_data(&produced_mana_color, 4);
  result &= save_or_load_data(&unk_008ce508, 4);
  result &= save_or_load_data(&unk_008ce4f4, 4);
  result &= save_or_load_data(&unk_008cfd20, 4);
  result &= save_or_load_data(&trigger_condition, 4);
  result &= save_or_load_data(&trigger_cause_controller, 4);
  result &= save_or_load_data(&trigger_cause, 4);
  result &= save_or_load_data(&card_on_stack_controller, 4);
  result &= save_or_load_data(&card_on_stack, 4);
  result &= save_or_load_data(&current_turn, 4);
  result &= save_or_load_data(&DAT_007aadec, 4);
  result &= save_or_load_data(&unk_00789308, 4);
  result &= save_or_load_data(&DAT_008cf6d0, 4);
  result &= save_or_load_data(&DAT_00925ac4, 4);
  result &= save_or_load_data(&DAT_007abc00, 0x40);
  result &= save_or_load_data(&DAT_008a8de4, 4);
  result &= save_or_load_data(&DAT_008cdab0, 8);
  result &= save_or_load_data(&DAT_0091bf70, 0x40);
  result &= save_or_load_data(&DAT_0089652c, 4);
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
  result &= save_or_load_data(&DAT_0091c500, 4);
  result &= save_or_load_data(&DAT_00777a9c, 4);
  result &= save_or_load_data(unk_007a7c58, 8);
  result &= save_or_load_data(&DAT_008b3240, 0x30);
  result &= save_or_load_data(&DAT_008b33fc, 4);
  result &= save_or_load_data(&unk_00938e2c, 4);
  result &= save_or_load_data(&DAT_008b484c, 4);
  result &= save_or_load_data(&DAT_008a8ffc, 4);
  result &= save_or_load_data(unk_007a79b0, 8);
  result &= save_or_load_data(unk_00939180, 0x80);
  result &= save_or_load_data(global_stack_cards, 0x100);
  result &= save_or_load_data(global_stack_damage_targets, 0x100);
  result &= save_or_load_data(unk_00895030, 0x80);
  result &= save_or_load_data(unk_007a7750, 0x80);
  result &= save_or_load_data(&unk_008b2934, 4);
  result &= save_or_load_data(&unk_008b3270, 4);
  result &= save_or_load_data(&unk_008cc844, 4);
  result &= save_or_load_data(&unk_00789a48, 4);
  result &= save_or_load_data(&DAT_008cf1b8, 4);
  result &= save_or_load_data(&DAT_00777aa8, 4);
  result &= save_or_load_data(&unk_007a7c1c, 4);
  result &= save_or_load_data(&unk_008cc840, 4);
  result &= save_or_load_data(TENTATIVE_timestamp_player, 2000);
  result &= save_or_load_data(TENTATIVE_timestamp_card, 2000);
  result &= save_or_load_data(&unk_00939330, 4);
  result &= save_or_load_data(&unk_0091c4fc, 4);
  result &= save_or_load_data(&unk_0078944c, 4);
  result &= save_or_load_data(&DAT_00939510, 4);
  result &= save_or_load_data(&DAT_008ce4ec, 4);
  result &= save_or_load_data(&unk_0091a6d0, 4);
  result &= save_or_load_data(&unk_00926790, 4);
  result &= save_or_load_data(&DAT_0057a750, 4);
  result &= save_or_load_data(&g_selected_wizard_color, 4);
  result &= save_or_load_data(&g_deck_color_bitmap, 4);
  result &= save_or_load_data(&g_shandalar_difficulty, 4);
  result &= save_or_load_data(&DAT_00715f10, 4);
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
  result &= save_or_load_data(&DAT_0091c970.value_002c, 4);
  result &= save_or_load_data(&DAT_0091c970.value_0030, 4);
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
unsigned int __cdecl save_or_load_ver2(void)
{
  unsigned int result;
  struct
  {
    int card_index;
    int card_type;
  } locals;

  result = 1;
#ifdef SHANDALAR
  result &= save_or_load_data((char *)global_cards_data + g_card_count * 0x48, 0x480);
#else
  result &= save_or_load_data((char *)global_cards_data + unk_0093f4b8 * 0x48, 0x480);
#endif
  result &= save_or_load_data(initial_library, 0xc80);
  result &= save_or_load_data(&unk_00742fcc, 4);
  result &= save_or_load_data(&DAT_00715fa4, 4);
  result &= save_or_load_data(&DAT_0072c8e0, 4);
  result &= save_or_load_data(&unk_00716248, 4);
  result &= save_or_load_data(&unk_00716244, 4);
  result &= save_or_load_data(&unk_00715fb0, 4);
  result &= save_or_load_data(&DAT_007161cc, 4);
  result &= save_or_load_data(&DAT_00742fc8, 4);
  result &= save_or_load_data(&DAT_0072c8e4, 4);
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
  result &= save_or_load_data(unk_00742f70, 0x40);
  result &= save_or_load_data(raw_mana_available_hex, 0x198);
  result &= save_or_load_data(unk_007161e0, 0x58);
  result &= save_or_load_data(unk_00743080, 8);
  result &= save_or_load_data(raw_mana_available, 0x40);
  result &= save_or_load_data(basiclandtypes_controlled, 0x40);
  result &= save_or_load_data(&g_next_duel_life_delta, 4);
  result &= save_or_load_data(&DAT_00716024, 4);
  result &= save_or_load_data(&DAT_0074302c, 4);
  result &= save_or_load_data(&unk_00742fc4, 4);
  result &= save_or_load_data(&unk_007161d8, 4);
  result &= save_or_load_data(&DAT_00742fd0, 4);
  result &= save_or_load_data(&DAT_00742fc0, 4);
  result &= save_or_load_data(&deck, 2000);
  result &= save_or_load_data(global_card_instances, 0x161e8);
  result &= save_or_load_data(unk_0093b280, 0x25c0);
  result &= save_or_load_data(global_graveyard_slots, 4000);
  result &= save_or_load_data(global_exile, 4000);
  result &= save_or_load_data(global_library, 4000);
  result &= save_or_load_data(global_ante_cards, 0x80);
  result &= save_or_load_data(&DAT_0093a848, 4);
  result &= save_or_load_data(&DAT_00926668, 4);
  result &= save_or_load_data(&unk_00939340, 4);
  result &= save_or_load_data(&unk_00925bb8, 4);
  result &= save_or_load_data(DAT_008cfd70, 0x60);
  result &= save_or_load_data(&DAT_00789498, 8);
  result &= save_or_load_data(unk_008ce510, 0x20);
  result &= save_or_load_data(&DAT_0091b2c4, 4);
  result &= save_or_load_data(&DAT_007a7d10, 0x10);
  result &= save_or_load_data(&spell_fizzled, 4);
  result &= save_or_load_data(active_cards_count, 8);
  result &= save_or_load_data(&DAT_00777854, 4);
  result &= save_or_load_data(&DAT_007a7c60, 100);
  result &= save_or_load_data(life, 8);
  result &= save_or_load_data(&DAT_007abce0, 8);
  result &= save_or_load_data(unk_008b44d0, 8);
  result &= save_or_load_data(&DAT_00938e38, 8);
  result &= save_or_load_data(unk_007a7d80, 0x10);
  result &= save_or_load_data(&human_player, 4);
  result &= save_or_load_data(&DAT_008ce538, 4);
  result &= save_or_load_data(&ai_modifier, 4);
  result &= save_or_load_data(&current_phase, 4);
  result &= save_or_load_data(&unk_008b4278, 4);
  result &= save_or_load_data(&unk_008b60e0, 4);
  result &= save_or_load_data(&DAT_00789714, 4);
  result &= save_or_load_data(&DAT_008951c8, 4);
  result &= save_or_load_data(&DAT_007a7874, 4);
  result &= save_or_load_data(&x_value, 4);
  result &= save_or_load_data(&max_x_value, 4);
  result &= save_or_load_data(&unk_008b35ec, 4);
  result &= save_or_load_data(&active_player, 4);
  result &= save_or_load_data(&affected_card_controller, 4);
  result &= save_or_load_data(&affected_card, 4);
  result &= save_or_load_data(&unk_008b4dd0, 4);
  result &= save_or_load_data(&unk_008cf6d4, 4);
  result &= save_or_load_data(&attacking_card_controller, 4);
  result &= save_or_load_data(&attacking_card, 4);
  result &= save_or_load_data(&event_result, 4);
  result &= save_or_load_data(&DAT_008a8d74, 4);
  result &= save_or_load_data(&DAT_008a8d7c, 4);
  result &= save_or_load_data(&DAT_0078930c, 4);
  result &= save_or_load_data(&DAT_00789b7c, 4);
  result &= save_or_load_data(&DAT_00925d30, 4);
  result &= save_or_load_data(&DAT_008a8d6c, 4);
  result &= save_or_load_data(&DAT_007aadf0, 4);
  result &= save_or_load_data(&produced_mana_color, 4);
  result &= save_or_load_data(&unk_008ce508, 4);
  result &= save_or_load_data(&unk_008ce4f4, 4);
  result &= save_or_load_data(&unk_008cfd20, 4);
  result &= save_or_load_data(&trigger_condition, 4);
  result &= save_or_load_data(&trigger_cause_controller, 4);
  result &= save_or_load_data(&trigger_cause, 4);
  result &= save_or_load_data(&card_on_stack_controller, 4);
  result &= save_or_load_data(&card_on_stack, 4);
  result &= save_or_load_data(&current_turn, 4);
  result &= save_or_load_data(&DAT_007aadec, 4);
  result &= save_or_load_data(&unk_00789308, 4);
  result &= save_or_load_data(&DAT_008cf6d0, 4);
  result &= save_or_load_data(&DAT_00925ac4, 4);
  result &= save_or_load_data(&DAT_007abc00, 0x40);
  result &= save_or_load_data(&DAT_008a8de4, 4);
  result &= save_or_load_data(&DAT_008cdab0, 8);
  result &= save_or_load_data(&DAT_0091bf70, 0x40);
  result &= save_or_load_data(&DAT_0089652c, 4);
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
  result &= save_or_load_data(&DAT_0091c500, 4);
  result &= save_or_load_data(&DAT_00777a9c, 4);
  result &= save_or_load_data(unk_007a7c58, 8);
  result &= save_or_load_data(&DAT_008b3240, 0x30);
  result &= save_or_load_data(&DAT_008b33fc, 4);
  result &= save_or_load_data(&unk_00938e2c, 4);
  result &= save_or_load_data(&DAT_008b484c, 4);
  result &= save_or_load_data(&DAT_008a8ffc, 4);
  result &= save_or_load_data(unk_007a79b0, 8);
  result &= save_or_load_data(unk_00939180, 0x80);
  result &= save_or_load_data(global_stack_cards, 0x100);
  result &= save_or_load_data(global_stack_damage_targets, 0x100);
  result &= save_or_load_data(unk_00895030, 0x80);
  result &= save_or_load_data(unk_007a7750, 0x80);
  result &= save_or_load_data(&unk_008b2934, 4);
  result &= save_or_load_data(&unk_008b3270, 4);
  result &= save_or_load_data(&unk_008cc844, 4);
  result &= save_or_load_data(&unk_00789a48, 4);
  result &= save_or_load_data(&DAT_008cf1b8, 4);
  result &= save_or_load_data(&DAT_00777aa8, 4);
  result &= save_or_load_data(&unk_007a7c1c, 4);
  result &= save_or_load_data(&unk_008cc840, 4);
  result &= save_or_load_data(TENTATIVE_timestamp_player, 2000);
  result &= save_or_load_data(TENTATIVE_timestamp_card, 2000);
  result &= save_or_load_data(&unk_00939330, 4);
  result &= save_or_load_data(&unk_0091c4fc, 4);
  result &= save_or_load_data(&unk_0078944c, 4);
  result &= save_or_load_data(&DAT_00939510, 4);
  result &= save_or_load_data(&DAT_008ce4ec, 4);
  result &= save_or_load_data(&unk_0091a6d0, 4);
  result &= save_or_load_data(&unk_00926790, 4);
  result &= save_or_load_data(&DAT_0057a750, 4);
  result &= save_or_load_data(&g_selected_wizard_color, 4);
  result &= save_or_load_data(&g_deck_color_bitmap, 4);
  result &= save_or_load_data(&g_shandalar_difficulty, 4);
  result &= save_or_load_data(&DAT_00715f10, 4);
  result &= save_or_load_data(&DAT_0091c970.value_002c, 4);
  result &= save_or_load_data(&DAT_0091c970.value_0030, 4);
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
#ifdef SHANDALAR
    for (locals.card_index = g_card_count; g_card_count + 0x10 > locals.card_index; ++locals.card_index)
#else
    for (locals.card_index = unk_0093f4b8; locals.card_index < unk_0093f4b8 + 0x10; ++locals.card_index)
#endif
    {
      if (*(int *)((char *)global_cards_data + locals.card_index * 0x48 + 0x24) != -1)
      {
        locals.card_type = CardTypeFromID(*(int *)((char *)global_cards_data + locals.card_index * 0x48 + 0x24));
        *(int *)((char *)global_cards_data + locals.card_index * 0x48 + 0x34) =
            *(int *)((char *)global_cards_data + locals.card_type * 0x48 + 0x34);
      }
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x004ee1d1
// FUNCTION: SHANDALAR 0x005031c2
void __cdecl save_gametype0(char *path)
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

// FUNCTION: MAGIC 0x004ef0ea
// FUNCTION: SHANDALAR 0x005040da
void __cdecl save_soloduel(char *path)
{
  g_save_file_fd = _open(path, 0x8301, 0x80);
  if (g_save_file_fd != -1)
  {
    global_saveload_loading = 0;
    save_or_load_data(&DAT_0057b178, 4);
    save_or_load_ver2();
    FUN_004ef073();
    FUN_0048b144();
    save_or_load_data(&DAT_008cefc0.value_0008, 4);
    save_or_load_data(&DAT_008cefc0.value_000c, 4);
    _close(g_save_file_fd);
  }
}

// FUNCTION: MAGIC 0x004ef26b
// FUNCTION: SHANDALAR 0x0050425c
void __cdecl save_gauntlet(char *path)
{
  g_save_file_fd = _open(path, 0x8301, 0x80);
  if (g_save_file_fd != -1)
  {
    global_saveload_loading = 0;
    save_or_load_data(&DAT_0057b178, 4);
    save_or_load_ver2();
    FUN_004ef073();
    FUN_0048b69e();
    save_or_load_data(&DAT_008cd930.value_0004, 4);
    save_or_load_data(&DAT_008cd930.value_0008, 4);
    save_or_load_data(&DAT_008cd930.value_0044, 4);
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
void __cdecl save_sealeddeck(char *path)
{
  g_save_file_fd = _open(path, 0x8301, 0x80);
  if (g_save_file_fd != -1)
  {
    global_saveload_loading = 0;
    save_or_load_data(&DAT_0057b178, 4);
    save_or_load_ver2();
    FUN_004ef073();
    FUN_0048bace();
    save_or_load_data(&DAT_00926340.value_002c, 4);
    save_or_load_data((void *)DAT_0093d844, 0x4ae64);
    _close(g_save_file_fd);
  }
}
