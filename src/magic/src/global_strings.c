#define GLOBAL_STRINGS_IMPL
#include "global_strings.h"
#include "global_state.h"
#include <stdio.h>
#include "cardartlib/src/assert.h"

// FUNCTION: SHANDALAR 0x0056ca10
// FUNCTION: MAGIC 0x004ec830
int load_text(const char *file_name, const char *section_name)
{
  struct
  {
    size_t line_length;
    FILE *f;
    int i;
    char *line_ptr;
    char line[300];
    char path[264];
    char section_line[300];
    int num_read;
    int num_text;
  } s;

  strcpy(s.section_line, "@");
  strcat(s.section_line, section_name);
  strcat(s.section_line, "\n");

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\");
  strcpy(s.path, file_name);
  // if (!strchr(file_name, '.'))
  //   strcat(s.path, ".txt");

  s.f = fopen(s.path, "rt");
  if (s.f == NULL)
    return 0;

  while (strcmp(s.section_line, s.line))
  {
    if (!fgets(s.line, 300, s.f))
    {
      fclose(s.f);
      return 0;
    }
  }

  fscanf(s.f, "%d", &s.num_text);
  fgets(s.line, 80, s.f);

  s.num_read = 0;
  for (s.i = 0; s.i < s.num_text && s.i < 250; s.i++)
  {
    s.line_ptr = g_text_lines[s.i];
    if (!fgets(s.line_ptr, 300, s.f))
    {
      fclose(s.f);
      return -s.num_read;
    }
    s.line_length = strlen(s.line_ptr);
    s.line_ptr[s.line_length - 1] = 0;
    s.num_read++;
  }

  fclose(s.f);
  if (s.num_read < s.num_text)
  {
    return -s.num_read;
  }
  else
  {
    return s.num_read;
  }
}

// FUNCTION: MAGIC 0x004eca6d
// FUNCTION: SHANDALAR 0x0056cc4d
int LoadTextSectionLines(const char *filename, const char *section_name)
{
  int loaded_count;
  int line_index;
  size_t line_length;
  int out_index;
  int char_index;

  if (g_duel_ai_mode_state == 1)
  {
    loaded_count = 0;
  }
  else
  {
    loaded_count = load_text(filename, section_name);
    for (line_index = 0; line_index < abs(loaded_count); ++line_index)
    {
      line_length = strlen(g_text_lines[line_index]);
      out_index = 0;
      for (char_index = 0; char_index < (int)line_length; ++char_index)
      {
        if (g_text_lines[line_index][char_index] == '\\' && g_text_lines[line_index][char_index + 1] == 'n')
        {
          g_text_lines[line_index][out_index] = '\n';
          ++char_index;
        }
        else
        {
          g_text_lines[line_index][out_index] = g_text_lines[line_index][char_index];
        }

        ++out_index;
      }

      g_text_lines[line_index][out_index] = '\0';
    }
  }

  return loaded_count;
}

// FUNCTION: SHANDALAR 0x004629be
// FUNCTION: MAGIC 0x00491f1e
void load_global_ui_strings(char *filename)
{
  int loaded_count;
  int color_index;

  load_text("MP_UISTRINGS.TXT", "MULTIPLAYERSTRINGS");
  strcpy(gs_waiting_for_opponent_007a7d30, g_text_lines[0]);

  load_text(filename, "GAMETITLE");
  strcpy(gs_magic_the_gathering_title_00789460, g_text_lines[0]);

  load_text(filename, "COLORWORDS");
  loaded_count = 0;
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(gs_color_word_lower_00709210[color_index], g_text_lines[loaded_count++]);
  }
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(gs_color_word_capitalized_00709390[color_index], g_text_lines[loaded_count++]);
  }
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(gs_color_word_upper_00709190[color_index], g_text_lines[loaded_count++]);
  }

  load_text(filename, "COLORLESSMANA");
  strcpy(gs_color_word_lower_00709210[0], g_text_lines[0]);
  strcpy(gs_color_word_capitalized_00709390[0], g_text_lines[0]);

  load_text(filename, "LANDWORDS");
  loaded_count = 0;
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(gs_land_word_lower_00709310[color_index], g_text_lines[loaded_count++]);
  }
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(gs_land_word_capitalized_00709290[color_index], g_text_lines[loaded_count++]);
  }
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(gs_land_word_upper_00709410[color_index], g_text_lines[loaded_count++]);
  }

  load_text(filename, "ABILITYWORDS");
  loaded_count = 0;
  for (color_index = 0; color_index < 0x11; ++color_index)
  {
    strcpy(gs_ability_word_008b4de0[color_index], g_text_lines[loaded_count++]);
  }

  loaded_count = load_text(filename, "HUNTING_SUBTYPENAMES");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_hunting_subtype_name_00926930[color_index], g_text_lines[color_index]);
  }

  loaded_count = gs_words_to_hack_count_008cd924 = load_text(filename, "WORDSTOHACK_BLACK");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_to_hack_00789c80[COLOR_BLACK][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSTOHACK_BLUE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_to_hack_00789c80[COLOR_BLUE][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSTOHACK_GREEN");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_to_hack_00789c80[COLOR_GREEN][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSTOHACK_RED");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_to_hack_00789c80[COLOR_RED][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSTOHACK_WHITE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_to_hack_00789c80[COLOR_WHITE][color_index], g_text_lines[color_index]);
  }

  load_text(filename, "WORDSHACKED_BLACK");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_hacked_0091d260[COLOR_BLACK][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSHACKED_BLUE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_hacked_0091d260[COLOR_BLUE][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSHACKED_GREEN");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_hacked_0091d260[COLOR_GREEN][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSHACKED_RED");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_hacked_0091d260[COLOR_RED][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSHACKED_WHITE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_hacked_0091d260[COLOR_WHITE][color_index], g_text_lines[color_index]);
  }

  loaded_count = gs_words_to_sleight_count_00789c34 =load_text(filename, "WORDSTOSLEIGHT_BLACK");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_to_sleight_00781bd0[COLOR_BLACK][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSTOSLEIGHT_BLUE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_to_sleight_00781bd0[COLOR_BLUE][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSTOSLEIGHT_GREEN");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_to_sleight_00781bd0[COLOR_GREEN][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSTOSLEIGHT_RED");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_to_sleight_00781bd0[COLOR_RED][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSTOSLEIGHT_WHITE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_to_sleight_00781bd0[COLOR_WHITE][color_index], g_text_lines[color_index]);
  }

  load_text(filename, "WORDSLEIGHTED_BLACK");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_sleighted_008a9c10[COLOR_BLACK][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSLEIGHTED_BLUE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_sleighted_008a9c10[COLOR_BLUE][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSLEIGHTED_GREEN");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_sleighted_008a9c10[COLOR_GREEN][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSLEIGHTED_RED");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_sleighted_008a9c10[COLOR_RED][color_index], g_text_lines[color_index]);
  }
  load_text(filename, "WORDSLEIGHTED_WHITE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(gs_words_sleighted_008a9c10[COLOR_WHITE][color_index], g_text_lines[color_index]);
  }

  load_text(filename, "PROMPT_STILLTHINKING");
  strcpy(gs_still_thinking_0091bd10, g_text_lines[0]);

  load_text(filename, "ABOUTBOX");
  strcpy(gs_aboutbox_text_007aa930, g_text_lines[0]);
  strcat(gs_aboutbox_text_007aa930, "\n\n");
  strcat(gs_aboutbox_text_007aa930, g_text_lines[1]);
  strcat(gs_aboutbox_text_007aa930, "\n\n");
  strcat(gs_aboutbox_text_007aa930, g_text_lines[2]);

  load_text(filename, "PROMPT_MAIN");
  strcpy(gs_prompt_main_phase_precombat_cast_spells_008cc710, g_text_lines[0]);
  strcpy(gs_prompt_main_phase_precombat_cast_spells_play_land_00777970, g_text_lines[1]);
  strcpy(gs_prompt_main_phase_postcombat_cast_spells_007ab2d0, g_text_lines[2]);
  strcpy(gs_prompt_main_phase_postcombat_cast_spells_play_land_0091cf50, g_text_lines[3]);
  strcpy(gs_prompt_combat_choose_attackers_0093d990, g_text_lines[4]);
  strcpy(gs_prompt_illegal_attacker_008b3d20, g_text_lines[5]);
  strcpy(gs_prompt_band_with_other_attacker_007a7af0, g_text_lines[6]);
  strcpy(gs_prompt_combat_choose_blockers_008b4140, g_text_lines[7]);

  load_text(filename, "PROMPT_NEWFULLCARD");
  strcpy(gs_prompt_new_full_card_0091b150, g_text_lines[0]);

  load_text(filename, "PROMPT_ACTION");
  strcpy(gs_casting_008b4850, g_text_lines[0]);
  strcpy(gs_activating_007912e0, g_text_lines[1]);
  strcpy(gs_processing_008ce290, g_text_lines[2]);

  load_text(filename, "PROMPT_FASTEFFECTS");
  strcpy(gs_triggered_effects_009396a0, g_text_lines[0]);
  strcpy(gs_interrupts_0093a850, g_text_lines[1]);
  strcpy(gs_fast_effects_00926520, g_text_lines[2]);

  load_text(filename, "PROMPT_CHECKFEPHASE");
  strcpy(gs_damage_prevention_00939200, g_text_lines[0]);
  strcpy(gs_trying_to_cast_007a79c0, g_text_lines[1]);
  strcpy(gs_cast_008b4720, g_text_lines[2]);
  strcpy(gs_activate_0091b2d0, g_text_lines[3]);
  strcpy(gs_process_00789320, g_text_lines[4]);
  strcpy(gs_upkeep_phase_00777d30, g_text_lines[5]);
  strcpy(gs_draw_phase_0091c100, g_text_lines[6]);
  strcpy(gs_main_phase_008cee80, g_text_lines[7]);
  strcpy(gs_assign_attackers_008b45f0, g_text_lines[8]);
  strcpy(gs_assign_blockers_008a8c40, g_text_lines[9]);
  strcpy(gs_discard_phase_0091d080, g_text_lines[10]);
  strcpy(gs_use_regeneration_effects_0091c680, g_text_lines[0xb]);

  load_text("promptsX1.txt", "PROMPT_ENDHEALING");
  strcpy(gs_end_damage_prevention_00789740, g_text_lines[0]);

  load_text("promptsX1.txt", "PROMPT_ATTACKERSELECTION");
  strcpy(gs_attacker_selected_008b32d0, g_text_lines[0]);

  load_text("promptsX1.txt", "PROMPT_BLOCKERSELECTION");
  strcpy(gs_blocker_selected_00926210, g_text_lines[0]);

  load_text("promptsX1.txt", "PROMPT_GAINLIFE");
  strcpy(gs_gain_life_007895e0, g_text_lines[0]);

  load_text("promptsX2.txt", "PROMPT_TURNSEQUENCE");
  strcpy(gs_pay_for_blocker_009263f0, g_text_lines[0]);
  strcpy(gs_end_draw_0091c510, g_text_lines[1]);
  strcpy(gs_end_main_008b26c0, g_text_lines[2]);
  strcpy(gs_end_discard_007ab020, g_text_lines[3]);

  load_text(filename, "PROMPT_SPECIALFEPHASE");
  strcpy(gs_card_into_play_0091c840, g_text_lines[0]);
  strcpy(gs_card_leaving_play_007aaef0, g_text_lines[1]);
  strcpy(gs_damage_dealing_00777ab0, g_text_lines[2]);
  strcpy(gs_graveyard_order_0091cbd0, g_text_lines[3]);
  strcpy(gs_cards_to_graveyard_008a8ed0, g_text_lines[4]);
  strcpy(gs_draw_a_card_008961c0, g_text_lines[5]);
  strcpy(gs_casting_00896400, g_text_lines[6]);
  strcpy(gs_tapping_00925d40, g_text_lines[7]);
  strcpy(gs_begin_upkeep_008cf080, g_text_lines[8]);
  strcpy(gs_end_upkeep_00925c00, g_text_lines[9]);
  strcpy(gs_draw_phase_008cf560, g_text_lines[10]);
  strcpy(gs_choose_attackers_00939570, g_text_lines[0xb]);
  strcpy(gs_pay_for_attacker_007a7880, g_text_lines[0xc]);
  strcpy(gs_choose_defenders_00789a50, g_text_lines[0xd]);
  strcpy(gs_end_of_combat_008b43a0, g_text_lines[0xe]);
  strcpy(gs_end_of_turn_008cd980, g_text_lines[0xf]);

  load_text(filename, "PROMPT_GRABMANA");
  strcpy(gs_tap_for_mana_0091c230, g_text_lines[0]);
  strcpy(gs_mana_so_far_008a9010, g_text_lines[1]);
  strcpy(gs_mana_so_far_max_00791420, g_text_lines[2]);

  load_text(filename, "PROMPT_ILLEGALTARGET");
  strcpy(gs_illegal_target_008b3e50, g_text_lines[0]);
  strcpy(gs_illegal_target_with_reason_008ce7e0, g_text_lines[1]);

  load_text(filename, "PROMPT_ILLEGALTARGETWHY");
  strcpy(gs_illegal_target_why_player_008b3110, g_text_lines[0]);
  strcpy(gs_illegal_target_why_cant_target_this_0091a6e0, g_text_lines[1]);
  strcpy(gs_illegal_target_why_where_0091cd00, g_text_lines[2]);
  strcpy(gs_illegal_target_why_controller_0091bbe0, g_text_lines[3]);
  strcpy(gs_illegal_target_why_owner_007aacc0, g_text_lines[4]);
  strcpy(gs_illegal_target_why_type_008a9a00, g_text_lines[5]);
  strcpy(gs_illegal_target_why_abilities_0091a810, g_text_lines[6]);
  strcpy(gs_illegal_target_why_color_008ce3c0, g_text_lines[7]);
  strcpy(gs_illegal_target_why_card_type_008cfde0, g_text_lines[8]);
  strcpy(gs_illegal_target_why_subtype_008cea20, g_text_lines[9]);
  strcpy(gs_illegal_target_why_power_00781aa0, g_text_lines[10]);
  strcpy(gs_illegal_target_why_toughness_007894b0, g_text_lines[0xb]);
  strcpy(gs_illegal_target_why_walls_008b34c0, g_text_lines[0xc]);
  strcpy(gs_illegal_target_why_spell_0091be40, g_text_lines[0xd]);
  strcpy(gs_illegal_target_why_basic_land_0093d860, g_text_lines[0xe]);
  strcpy(gs_illegal_target_why_artifact_creature_008ced40, g_text_lines[0xf]);
  strcpy(gs_illegal_target_why_target_player_0091c360, g_text_lines[0x10]);
  strcpy(gs_illegal_target_why_tapped_00925080, g_text_lines[0x11]);
  strcpy(gs_illegal_target_why_attacking_00896540, g_text_lines[0x12]);
  strcpy(gs_illegal_target_why_attacked_0091bfc0, g_text_lines[0x13]);
  strcpy(gs_illegal_target_why_blocked_0091caa0, g_text_lines[0x14]);
  strcpy(gs_illegal_target_why_blocking_007ab160, g_text_lines[0x15]);
  strcpy(gs_illegal_target_why_attacking_blocking_008cc850, g_text_lines[0x16]);
  strcpy(gs_illegal_target_why_enchanted_008cf3c0, g_text_lines[0x17]);
  strcpy(gs_illegal_target_why_casted_00939050, g_text_lines[0x18]);
  strcpy(gs_illegal_target_why_cast_resolved_008ce6a0, g_text_lines[0x19]);
  strcpy(gs_illegal_target_why_damaged_007911b0, g_text_lines[0x1a]);
  strcpy(gs_illegal_target_why_can_untap_008b3be0, g_text_lines[0x1b]);
  strcpy(gs_illegal_target_why_will_untap_008ceb50, g_text_lines[0x1c]);

  load_text(global_ui_strings_filename, "DIALOGBUTTONS");
  strcpy(gs_ok_00924800, g_text_lines[0]);
  strcpy(gs_cancel_008a8c20, g_text_lines[1]);
  strcpy(gs_done_008b40e0, g_text_lines[2]);

  load_text(global_ui_strings_filename, "DIALOG_DUELOPTIONS");
  for (color_index = 0; color_index < 0x13; ++color_index)
  {
    strcpy(gs_duel_option_008b35f0[color_index], g_text_lines[color_index]);
  }

  load_text(global_ui_strings_filename, "CARDTITLES");
  strcpy(gs_cardtitle_damage_008cfd30, g_text_lines[0]);
  strcpy(gs_cardtitle_hunting_00926750, g_text_lines[1]);
  strcpy(gs_cardtitle_activation_007aaeb0, g_text_lines[2]);
  strcpy(gs_cardtitle_upkeep_008b4100, g_text_lines[3]);
  strcpy(gs_cardtitle_draw_a_card_008b4330, g_text_lines[4]);

  load_text(global_ui_strings_filename, "MULTIBLOCKCARD");
  strcpy(gs_multiblock_creature_008cf040, g_text_lines[0]);
  strcpy(gs_multiblock_shadow_blocker_00925e70, g_text_lines[1]);

  load_text(global_ui_strings_filename, "ENCHANTMENTTYPENAME");
  strcpy(gs_enchantment_008b3f80, g_text_lines[0]);

  load_text(global_ui_strings_filename, "ARTISTLINE");
  strcpy(gs_illus_00789130, g_text_lines[0]);

  load_text(global_ui_strings_filename, "WINDOWTITLES");
  strcpy(gs_window_title_opponent_attack_009267a0, g_text_lines[0]);
  strcpy(gs_window_title_your_attack_00926010, g_text_lines[1]);
  strcpy(gs_window_title_spell_chain_008b42f0, g_text_lines[2]);
  strcpy(gs_window_title_opponent_0091c820, g_text_lines[3]);
  strcpy(gs_window_title_your_hand_00777bf0, g_text_lines[4]);
  strcpy(gs_window_title_save_game_008951d0, g_text_lines[5]);

  load_text(global_ui_strings_filename, "CUECARD_OTHER");
  strcpy(gs_cuecard_minimized_attack_window_008b4280, g_text_lines[0]);
  strcpy(gs_cuecard_minimized_spell_chain_00926090, g_text_lines[1]);
  strcpy(gs_cuecard_opponent_library_00789bd0, g_text_lines[2]);
  strcpy(gs_cuecard_your_library_0091ca20, g_text_lines[3]);
  strcpy(gs_cuecard_opponent_graveyard_0091c7b0, g_text_lines[4]);
  strcpy(gs_cuecard_your_graveyard_0091c9b0, g_text_lines[5]);
  strcpy(gs_cuecard_scrollbar_008b2900, g_text_lines[6]);
  strcpy(gs_cuecard_scroll_thumb_0093a810, g_text_lines[7]);

  load_text(global_ui_strings_filename, "CUECARD_LIFE");
  strcpy(gs_cuecard_opponent_life_points_008b30a0, g_text_lines[0]);
  strcpy(gs_cuecard_opponent_life_points_and_poison_009252f0, g_text_lines[1]);
  strcpy(gs_cuecard_your_life_points_009394a0, g_text_lines[2]);
  strcpy(gs_cuecard_your_life_points_and_poison_008a9ba0, g_text_lines[3]);
  strcpy(gs_cuecard_opponent_life_points_lich_0091d1b0, g_text_lines[4]);
  strcpy(gs_cuecard_opponent_life_points_and_poison_lich_008b2890, g_text_lines[5]);
  strcpy(gs_cuecard_your_life_points_lich_007a7810, g_text_lines[6]);
  strcpy(gs_cuecard_your_life_points_and_poison_lich_008cf4f0, g_text_lines[7]);

  load_text(global_ui_strings_filename, "CUECARD_MANAPOOL");
  for (color_index = 0; color_index < 7; ++color_index)
  {
    strcpy(gs_cuecard_opponent_mana_pool_008cff20[color_index], g_text_lines[color_index]);
  }
  for (color_index = 0; color_index < 7; ++color_index)
  {
    strcpy(gs_cuecard_your_mana_pool_008cc2e0[color_index], g_text_lines[color_index + 7]);
  }

  load_text(global_ui_strings_filename, "CUECARD_PHASEBAR");
  strcpy(gs_phasebar_opponent_untap_008a8d80, g_text_lines[0]);
  strcpy(gs_phasebar_opponent_upkeep_008a9b30, g_text_lines[1]);
  strcpy(gs_phasebar_opponent_draw_0091c490, g_text_lines[2]);
  strcpy(gs_phasebar_opponent_main_precombat_00924790, g_text_lines[3]);
  strcpy(gs_phasebar_opponent_main_combat_00926380, g_text_lines[4]);
  strcpy(gs_phasebar_opponent_main_postcombat_00925fa0, g_text_lines[5]);
  strcpy(gs_phasebar_opponent_discard_00938e40, g_text_lines[6]);
  strcpy(gs_phasebar_opponent_cleanup_007aae40, g_text_lines[7]);
  strcpy(gs_phasebar_your_untap_008b3280, g_text_lines[8]);
  strcpy(gs_phasebar_your_upkeep_007a7c20, g_text_lines[9]);
  strcpy(gs_phasebar_your_draw_0091b110, g_text_lines[10]);
  strcpy(gs_phasebar_your_main_precombat_008b2840, g_text_lines[0xb]);
  strcpy(gs_phasebar_your_main_declare_combat_007a7cd0, g_text_lines[0xc]);
  strcpy(gs_phasebar_your_main_postcombat_00926670, g_text_lines[0xd]);
  strcpy(gs_phasebar_your_discard_00939350, g_text_lines[0xe]);
  strcpy(gs_phasebar_your_cleanup_00789b90, g_text_lines[0xf]);
  strcpy(gs_phasebar_choose_attackers_008966e0, g_text_lines[0x10]);
  strcpy(gs_phasebar_attacker_fast_effects_0093a7c0, g_text_lines[0x11]);
  strcpy(gs_phasebar_assign_defenders_00925a90, g_text_lines[0x12]);
  strcpy(gs_phasebar_blocker_fast_effects_00925bc0, g_text_lines[0x13]);
  strcpy(gs_phasebar_resolve_first_strike_damage_0091d220, g_text_lines[0x14]);
  strcpy(gs_phasebar_resolve_normal_damage_008cec80, g_text_lines[0x15]);
  strcpy(gs_phasebar_main_phase_postcombat_00789870, g_text_lines[0x16]);

  load_text(global_ui_strings_filename, "CUECARD_SMALLCARD");
  strcpy(gs_cuecard_damage_to_player_007a77d0, g_text_lines[0]);
  strcpy(gs_cuecard_this_card_will_untap_00925040, g_text_lines[1]);
  strcpy(gs_cuecard_damage_n_0091b290, g_text_lines[2]);
  strcpy(gs_cuecard_not_controlled_by_owner_008a8e40, g_text_lines[3]);
  strcpy(gs_cuecard_is_a_target_00789c40, g_text_lines[4]);
  strcpy(gs_cuecard_cant_target_this_008b4990, g_text_lines[5]);
  strcpy(gs_cuecard_is_a_target_cant_target_again_008cf230, g_text_lines[6]);
  strcpy(gs_cuecard_dying_0091c640, g_text_lines[7]);
  strcpy(gs_cuecard_summoning_sickness_008a8e00, g_text_lines[8]);
  strcpy(gs_cuecard_phased_007aae00, g_text_lines[9]);

  LoadTextSectionLines(global_ui_strings_filename, "PROMPT_RESOLUTIONERROR");
  strcpy(g_default_duel_prompt_text, g_text_lines[0]);
}
