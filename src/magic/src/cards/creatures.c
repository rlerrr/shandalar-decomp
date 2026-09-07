#include <stdio.h>
#include <string.h>
#include "../game_support.h"
#include "../global_duel_ui_ids.h"
#include "../global_strings.h"
#include "../network.h"

int get_card_display_pic_num(card_id_t card_id, int player, int card);
int check_duel_finished(void);
int card_sea_serpent(int player, int card, event_t event);
void replace_text_token(char *text, char *search, int case_sensitive, char *replace);
int AddCardToDeckSorted(int card_id);
void remove_card_from_shandalar_deck_by_id(unsigned int card_id);
int create_damage_effect_copy_for_target_player(int player, int card);
extern int combat_eval_abilities;

static __inline int give_pump_until_eot(int player, int card, int target_player, int target_card, int power, int toughness)
{
  int legacy_card;

  legacy_card = create_legacy_effect(player, card, LEGACY_EFFECT_PUMP, target_player, target_card);
  if (legacy_card != -1)
  {
    PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = power;
    PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = toughness;
  }

  return legacy_card;
}

static __inline int random_magic_color(void)
{
  return internal_rand(5) + 1;
}

static __inline void set_card_to_random_color(int player, int card)
{
  int color;

  color = random_magic_color();
  PLAYER_CARD_INSTANCE(player, card).color = (char)(1 << (unsigned char)color);
}

static __inline int is_blue_land(int player, int card)
{
  int internal_card_id;

  internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
  return internal_card_id != -1 &&
         (global_cards_data[internal_card_id].type & TYPE_LAND) != 0 &&
         ((unsigned char)global_cards_data[internal_card_id].color & (1 << COLOR_BLUE)) != 0;
}

static __inline int sacrifice_blue_land(int player)
{
  int current_card;

  for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
  {
    if (is_in_play(player, current_card) != 0 && is_blue_land(player, current_card) != 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(player, current_card, KILL_SACRIFICE);
      return 1;
    }
  }

  return 0;
}

static __inline int return_permanent_to_hand(int target_player, int target_card)
{
  int internal_card_id;

  internal_card_id = PLAYER_CARD_INSTANCE(target_player, target_card).internal_card_id;
  if ((PLAYER_CARD_INSTANCE(target_player, target_card).token_status & STATUS_TOKEN) == 0)
  {
    add_card_to_hand(target_player, internal_card_id);
  }
  kill_card(target_player, target_card, KILL_REMOVE);

  return 0;
}

static __inline int permanent_is_enchanted(int target_player, int target_card)
{
  int current_player;
  int current_card;

  for (current_player = 0; current_player < 2; ++current_player)
  {
    for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
    {
      if (is_in_play(current_player, current_card) != 0 &&
          (global_cards_data[PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id].type & TYPE_ENCHANTMENT) != 0 &&
          (int)PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player == target_player &&
          PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card == target_card)
      {
        return 1;
      }
    }
  }

  return 0;
}

static __inline int hand_has_creature_card(int target_player)
{
  int current_card;

  for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
  {
    if (PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id != -1 &&
        (PLAYER_CARD_INSTANCE(target_player, current_card).state & STATE_IN_PLAY) == 0 &&
        (global_cards_data[PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      return 1;
    }
  }

  return 0;
}

static __inline void discard_random_creature_card_from_hand(int target_player)
{
  int candidates[500];
  int candidate_count;
  int current_card;

  candidate_count = 0;
  for (current_card = 0; current_card < g_active_cards_count[target_player]; ++current_card)
  {
    if (PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id != -1 &&
        (PLAYER_CARD_INSTANCE(target_player, current_card).state & STATE_IN_PLAY) == 0 &&
        (global_cards_data[PLAYER_CARD_INSTANCE(target_player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      candidates[candidate_count] = current_card;
      ++candidate_count;
    }
  }

  if (candidate_count != 0)
  {
    discard_card_from_hand(target_player, candidates[internal_rand(candidate_count)]);
  }
}

static __inline void transfer_random_card_from_hand(int from_player, int to_player)
{
  int candidates[500];
  int candidate_count;
  int current_card;
  int selected_card;
  int internal_card_id;

  candidate_count = 0;
  for (current_card = 0; current_card < g_active_cards_count[from_player]; ++current_card)
  {
    if (PLAYER_CARD_INSTANCE(from_player, current_card).internal_card_id != -1 &&
        (PLAYER_CARD_INSTANCE(from_player, current_card).state & STATE_IN_PLAY) == 0)
    {
      candidates[candidate_count] = current_card;
      ++candidate_count;
    }
  }

  if (candidate_count != 0)
  {
    selected_card = candidates[internal_rand(candidate_count)];
    internal_card_id = PLAYER_CARD_INSTANCE(from_player, selected_card).internal_card_id;
    add_card_to_hand(to_player, internal_card_id);
    discard_card_from_hand(from_player, selected_card);
  }
}

// FUNCTION: MAGIC 0x0053aa50
// FUNCTION: SHANDALAR 0x00488ca0
int card_dummy(int player, int card, event_t event)
{
  return 0;
}

// FUNCTION: MAGIC 0x0053aa62
// FUNCTION: SHANDALAR 0x00488cb2
int card_data_card(int player, int card, event_t event)
{
  return 0;
}

// FUNCTION: MAGIC 0x0053aa74
// FUNCTION: SHANDALAR 0x00488cc4
unsigned int get_protections_from(int player, int card)
{
  struct
  {
    unsigned int type;
    unsigned int illegal_abilities;
  } s;

  s.illegal_abilities = 0;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1 ||
      PLAYER_CARD_INSTANCE(player, card).internal_card_id == g_stack_proxy_internal_card_id)
  {
    s.type = global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].type;
  }
  else
  {
    s.type = global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type;
  }

  if ((s.type & TYPE_SORCERY) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_SORCERIES;
  }
  if ((s.type & TYPE_INSTANT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_INSTANTS;
  }
  if ((s.type & TYPE_INTERRUPT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_INTERRUPTS;
  }
  if ((s.type & TYPE_ENCHANTMENT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_ENCHANTMENTS;
  }
  if ((s.type & TYPE_ARTIFACT) != 0)
  {
    s.illegal_abilities |= KEYWORD_PROT_ARTIFACTS;
  }

  return (0x800 << (unsigned char)(single_color_test_bit_to_color_t(
                                       PLAYER_CARD_INSTANCE(player, card).color) -
                                   1)) |
         s.illegal_abilities;
}

// FUNCTION: MAGIC 0x0053ac18
// FUNCTION: SHANDALAR 0x00488e68
int card_prismatic_dragon(int player, int card, event_t event)
{
  struct
  {
    int activation_random;
    int upkeep_random;
    char prompt[300];
    int color;
  } s;

  if ((g_trigger_condition == TRIGGER_UPKEEP || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_player &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x35);
      }
      if ((g_duel_network_flags & 2) == 0)
      {
        s.upkeep_random = internal_rand(5);
      }
      else
      {
        s.upkeep_random = network_random(player, 5);
      }
      s.color = s.upkeep_random + 1;
      PLAYER_CARD_INSTANCE(player, card).color =
          (char)(1 << (unsigned char)s.color);
      load_text("prompts.txt", "PRISMATIC_DRAGON");
      strcpy(s.prompt, "\n");
      strcat(s.prompt, g_text_lines[s.color]);
      do_dialog(player, player, card, -1, -1, s.prompt, 0);
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return PLAYER_CARD_INSTANCE(player, card).info_slot == 0 &&
           has_mana(player, COLOR_ANY, 2);
  }
  else if (event == EVENT_ACTIVATE &&
           has_mana(player, COLOR_ANY, 2))
  {
    charge_mana(player, COLOR_COLORLESS, 2);
    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x35);
      }
      if ((g_duel_network_flags & 2) == 0)
      {
        s.activation_random = internal_rand(5);
      }
      else
      {
        s.activation_random = network_random(g_card_on_stack_controller, 5);
      }
      s.color = s.activation_random + 1;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .color = (char)(1 << (unsigned char)s.color);
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "PRISMATIC_DRAGON");
        strcpy(s.prompt, "\n");
        strcat(s.prompt, g_text_lines[s.color]);
        do_dialog(player, player, card, -1, -1, s.prompt, 0);
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot |= 1 << (unsigned char)g_current_player;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }
  return 0;
}

// FUNCTION: MAGIC 0x0053b155
// FUNCTION: SHANDALAR 0x004893a9
int card_rainbow_knights(int player, int card, event_t event)
{
  struct
  {
    unsigned char random_color;
    char padding[7];
    unsigned int random_amount;
    char prompt[900];
    int can_pay_white;
    int default_choice;
    int legacy_card;
    int saved_event_result;
  } s;
  int can_pay_colorless;
  int choice;

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    if ((g_duel_network_flags & 2) == 0)
    {
      s.random_color = (unsigned char)internal_rand(5);
    }
    else
    {
      s.random_color = (unsigned char)network_random(player, 5);
    }
    PLAYER_CARD_INSTANCE(player, card).eot_toughness =
        KEYWORD_PROT_BLACK << (s.random_color);
    return 0;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).regen_status |=
        PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana(player, COLOR_WHITE, 2) != 0)
    {
      return 1;
    }
    return has_mana(player, COLOR_ANY, 1) != 0 &&
           (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0x100) == 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    can_pay_colorless = has_mana(player, COLOR_ANY, 1);
    s.can_pay_white = has_mana(player, COLOR_WHITE, 2);
    PLAYER_CARD_INSTANCE(player, card).eot_toughness &= ~1;
    if (s.can_pay_white != 0 ||
        (can_pay_colorless != 0 &&
         (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0x100) == 0))
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "RAINBOW_KNIGHTS");
        if (s.can_pay_white == 0)
        {
          sprintf(s.prompt, " _%s", g_text_lines[0]);
        }
        else
        {
          sprintf(s.prompt, " %s", g_text_lines[0]);
        }
        if (can_pay_colorless == 0 ||
            (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0x100) != 0)
        {
          sprintf(s.prompt + strlen(s.prompt), "\n _%s", g_text_lines[0]);
        }
        else
        {
          sprintf(s.prompt + strlen(s.prompt), "\n %s", g_text_lines[0]);
        }
        sprintf(s.prompt + strlen(s.prompt), "\n %s", g_text_lines[1]);
      }

      if (g_attacking_creature_count == 0 ||
          ((PLAYER_CARD_INSTANCE(player, card).state & 8) == 0 &&
           ((PLAYER_CARD_INSTANCE(player, card).state & 4) == 0 ||
            (PLAYER_CARD_INSTANCE(player, card).state & 0x200) == 0)))
      {
        if (s.can_pay_white == 0)
        {
          if (can_pay_colorless == 0 ||
              (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0x100) != 0)
          {
            s.default_choice = 2;
          }
          else
          {
            s.default_choice = 1;
          }
        }
        else
        {
          s.default_choice = 0;
        }
      }
      else if (can_pay_colorless == 0 ||
               (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0x100) != 0)
      {
        s.default_choice = s.can_pay_white == 0 ? 2 : 0;
      }
      else
      {
        s.default_choice = 1;
      }

      choice = do_dialog(player, player, card, -1, -1, s.prompt, s.default_choice);
      if (choice == 0)
      {
        if (s.can_pay_white != 0)
        {
          charge_mana(player, COLOR_WHITE, 2);
          if (g_spell_fizzled != 1)
          {
            PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
            PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
            PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
            if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
            {
              PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
            }
          }
        }
      }
      else if (choice == 1)
      {
        charge_mana(player, COLOR_COLORLESS, 1);
        if (g_spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
          PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, card).eot_toughness |= 1;
        }
      }
      else if (choice == 2)
      {
        g_spell_fizzled = 1;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }
    else if ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 1) == 0)
    {
      if ((g_duel_network_flags & 2) == 0)
      {
        s.random_amount = internal_rand(3);
      }
      else
      {
        s.random_amount = network_random(g_card_on_stack_controller, 3);
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += s.random_amount & 0xff;
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "RAINBOW_KNIGHTS");
        sprintf(s.prompt, g_text_lines[3], s.random_amount);
        do_dialog(player, player, card, -1, -1, s.prompt, 0);
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x3a);
      }
      if (s.random_amount != 0 &&
          (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= ~0x80000;
        s.legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                             g_card_on_stack,
                                             LEGACY_EFFECT_PUMP,
                                             g_card_on_stack_controller,
                                             g_card_on_stack);
        if (s.legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, s.legacy_card).counter_power = (short)s.random_amount;
          PLAYER_CARD_INSTANCE(player, s.legacy_card).info_slot |= 0x80000;
          PLAYER_CARD_INSTANCE(player, s.legacy_card).eot_toughness = 1;
        }
      }
    }
    else if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                   PLAYER_CARD_INSTANCE(player, card).parent_card)
                  .eot_toughness &
              0x100) == 0)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .eot_toughness &= 0x1ff800;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .eot_toughness |= 0x100;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      s.legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           g_duel_generated_internal_card_id_03,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
      if (s.legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, s.legacy_card).info_slot = 0x100;
        PLAYER_CARD_INSTANCE(player, s.legacy_card).regen_status = 0;
        PLAYER_CARD_INSTANCE(player, s.legacy_card).eot_toughness = 2;
      }
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x3a);
      }
    }
    return 0;
  }
  else if (event == EVENT_ABILITIES &&
           card == g_affected_card &&
           player == g_affected_card_controller &&
           (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0)
  {
    g_event_result |= PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0x1ff800;
    s.saved_event_result = g_event_result;
    destroy_attached_auras_of_color(
        player,
        card,
        single_color_test_bit_to_color_t(
            (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0x1ff800) >> 10));
    g_event_result = s.saved_event_result;
    return 0;
  }
  else if (event == EVENT_CHECK_ABILITIES &&
           card == g_affected_card &&
           player == g_affected_card_controller &&
           has_mana(player, COLOR_ANY, 1) != 0)
  {
    combat_eval_abilities |= KEYWORD_FIRST_STRIKE;
    return 0;
  }
  else if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    PLAYER_CARD_INSTANCE(player, card).eot_toughness &= ~0x100;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053c256
// FUNCTION: SHANDALAR 0x0048a4aa
int card_sindbad(int player, int card, event_t event)
{
  int drawn_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    drawn_card = draw_card_for_player(player);
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "SINDBAD");
      do_dialog(player, player, card, player, drawn_card, g_text_lines[0], 0);
    }
    if ((global_cards_data[PLAYER_CARD_INSTANCE(player, drawn_card).internal_card_id].type &
         TYPE_LAND) == 0)
    {
      move_card_to_graveyard(player, drawn_card);
      PLAYER_CARD_INSTANCE(player, drawn_card).internal_card_id = -1;
      --g_duel_summary.hand_counts[player];
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x18);
      }
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053c406
// FUNCTION: SHANDALAR 0x0048a65a
int card_tempest_efreet(int player, int card, event_t event)
{
  int choice;
  int attempts;
  int opponent;
  int random_card_index;
  char prompt[900];

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "TEMPEST_EFREET");
      sprintf(prompt, " %s\n %s\n %s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
    }

    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1 - player;
    PLAYER_CARD_INSTANCE(player, card).info_slot =
        do_dialog(PLAYER_CARD_INSTANCE(player, card).eot_toughness,
                  player,
                  card,
                  -1,
                  -1,
                  prompt,
                  0);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    opponent = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    choice = PLAYER_CARD_INSTANCE(player, card).info_slot;

    if (choice == 0)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .state ^= STATE_OWNED_BY_OPPONENT;
      if (g_duel_ai_mode_state == 1)
      {
        ;
      }
      else
      {
        play_sound_effect(0xf);
      }

      kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_REMOVE);

      if (g_active_player == player)
      {
        remove_card_from_shandalar_deck_by_id(
            (unsigned int)PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
                .internal_card_id);
      }
      else
      {
        AddCardToDeckSorted(
            PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                 PLAYER_CARD_INSTANCE(player, card).parent_card)
                .internal_card_id);
      }
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .internal_card_id = -1;

      attempts = 0;
      if ((g_duel_network_flags & 2) == 0 || g_other_player != player)
      {
        do
        {
          do
          {
            random_card_index = internal_rand(g_active_cards_count[opponent]);
          } while (PLAYER_CARD_INSTANCE(opponent, random_card_index).internal_card_id == -1);
          if ((PLAYER_CARD_INSTANCE(opponent, random_card_index).state & STATE_IN_PLAY) == 0)
          {
            break;
          }
          ++attempts;
        } while (attempts < 999);

        if ((g_duel_network_flags & 2) != 0 && g_active_player == player)
        {
          if (attempts > 998)
          {
            random_card_index = -1;
          }
          g_network_result_packet.result = random_card_index;
          g_network_result_packet.packet_type = 0x14;
          TENTATIVE_send_network_result(player, 0x14);
        }
      }
      else
      {
        TENTATIVE_wait_for_network_result(player, 0x14);
        random_card_index = g_network_result_packet.result;
        if (g_network_result_packet.result == -1)
        {
          attempts = 999;
        }
      }

      if (attempts < 999)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "TEMPEST_EFREET");
          sprintf(prompt, " %s", g_text_lines[3]);
          do_dialog(player, player, card, opponent, random_card_index, prompt, 0);
        }

        PLAYER_CARD_INSTANCE(opponent, random_card_index).state ^= STATE_OWNED_BY_OPPONENT;
        move_card_to_graveyard(opponent, random_card_index);
        if (g_active_player == player)
        {
          AddCardToDeckSorted(PLAYER_CARD_INSTANCE(opponent, random_card_index).internal_card_id);
        }
        else
        {
          remove_card_from_shandalar_deck_by_id(
              (unsigned int)PLAYER_CARD_INSTANCE(opponent, random_card_index).internal_card_id);
        }
        PLAYER_CARD_INSTANCE(opponent, random_card_index).internal_card_id = -1;
      }
    }
    else if (choice == 1)
    {
      g_life[opponent] -= 10;
      kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_DESTROY);
    }
    else if (choice == 2)
    {
      if (g_duel_ai_mode_state == 1)
      {
        g_life[opponent] = -99;
      }
      else
      {
        exit_duel_thread(opponent);
      }
      kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_DESTROY);
    }

    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053cbde
// FUNCTION: SHANDALAR 0x0048ae32
int card_xenic_poltergeist(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int legacy_card;
    int animated_internal_card_id;
  } s;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT,
                                 TYPE_CREATURE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0) != 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "XENIC_POLTERGEIST");
    }
    if (C_real_select_target(player,
                             2,
                             player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT,
                             TYPE_CREATURE,
                             0,
                             get_protections_from(player, card),
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             -1,
                             -1,
                             -1,
                             -1,
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &s.target) != 0)
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], s.target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(s.target.player,
                               s.target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT,
                               TYPE_CREATURE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               -1,
                               -1,
                               -1,
                               0,
                               0,
                               0) != 0)
    {
      s.legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           g_duel_generated_internal_card_id_14,
                                           s.target.player,
                                           s.target.card);
      if (s.legacy_card != -1)
      {
        s.animated_internal_card_id =
            create_a_card_type(PLAYER_CARD_INSTANCE(s.target.player, s.target.card).internal_card_id);
        if (s.animated_internal_card_id != -1)
        {
          PLAYER_CARD_INSTANCE(player, s.legacy_card).dummy3 =
              s.animated_internal_card_id;
          global_cards_data[s.animated_internal_card_id].type = TYPE_ARTIFACT | TYPE_CREATURE;
          global_cards_data[s.animated_internal_card_id].toughness =
              global_cards_data[PLAYER_CARD_INSTANCE(s.target.player, s.target.card).internal_card_id].cc[1];
          global_cards_data[s.animated_internal_card_id].power =
              global_cards_data[s.animated_internal_card_id].toughness;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d06d
// FUNCTION: SHANDALAR 0x0048b2c1
int card_uncle_istvan(int player, int card, event_t event)
{
  int source_internal_card_id;

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == card &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player)
  {
    if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player,
                             PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card)
            .internal_card_id != -1)
    {
      source_internal_card_id =
          PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player,
                               PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card)
              .internal_card_id;
    }
    else
    {
      source_internal_card_id =
          PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player,
                               PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card)
              .original_internal_card_id;
    }

    if ((global_cards_data[source_internal_card_id].type & TYPE_CREATURE) != 0)
    {
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d355
// FUNCTION: SHANDALAR 0x0048b5a9
int card_ironclaw_orcs(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY && g_affected_card == card && g_affected_card_controller == player)
  {
    if (C_get_abilities(g_attacking_card_controller, g_attacking_card, EVENT_POWER, card) > 1)
    {
      g_event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d3bc
// FUNCTION: SHANDALAR 0x0048b610
int card_amrou_kithkin(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY && card == g_attacking_card && player == g_attacking_card_controller)
  {
    if (C_get_abilities(g_affected_card_controller, g_affected_card, EVENT_POWER, -1) > 2)
    {
      g_event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d41f
// FUNCTION: SHANDALAR 0x0048b673
int card_elven_riders(int player, int card, event_t event)
{
  if (event == EVENT_BLOCK_LEGALITY && card == g_attacking_card && player == g_attacking_card_controller)
  {
    if (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype != 0 &&
        (C_get_abilities(g_affected_card_controller, g_affected_card, EVENT_ABILITIES, -1) & KEYWORD_FLYING) == 0)
    {
      g_event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d4c9
// FUNCTION: SHANDALAR 0x0048b71c
int card_hypnotic_specter(int player, int card, event_t event)
{
  if ((event == EVENT_DEAL_DAMAGE) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id))
  {
    if ((PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0))
    {
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)g_affected_card_controller;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = g_affected_card;
    }

    return 0;
  }

  if ((((g_trigger_condition == TRIGGER_DEAL_DAMAGE) && (g_affected_card == card)) && (g_affected_card_controller == player) && (PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1)) && (player == g_current_turn))
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      discard((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player, 1, 0);
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = -1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053d778
// FUNCTION: SHANDALAR 0x0048b9cb
int card_marsh_viper(int player, int card, event_t event)
{
  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id ==
          g_damage_card_internal_card_id)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1 &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)g_affected_card_controller;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = g_affected_card;
    }
    return 0;
  }

  if (g_trigger_condition == TRIGGER_DEAL_DAMAGE &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1 &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      g_poison_counters[(int)PLAYER_CARD_INSTANCE(player, card).damage_source_player] += 2;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = -1;
      check_duel_finished();
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053da26
// FUNCTION: SHANDALAR 0x0048bc78
int card_pit_scorpion(int player, int card, event_t event)
{
  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id ==
          g_damage_card_internal_card_id)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1 &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)g_affected_card_controller;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = g_affected_card;
    }
    return 0;
  }

  if (g_trigger_condition == TRIGGER_DEAL_DAMAGE &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1 &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      ++g_poison_counters[(int)PLAYER_CARD_INSTANCE(player, card).damage_source_player];
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = -1;
      check_duel_finished();
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053dcd4
// FUNCTION: SHANDALAR 0x0048bf26
int card_nafs_asp(int player, int card, event_t event)
{
  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1 &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)g_affected_card_controller;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = g_affected_card;
    }

    return 0;
  }

  if (g_trigger_condition == TRIGGER_DEAL_DAMAGE &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1 &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness =
          (int)PLAYER_CARD_INSTANCE(player, card).damage_source_player;
      create_damage_effect_copy_for_target_player(player, card);
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = -1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053dfb5
// FUNCTION: SHANDALAR 0x0048c204
int create_damage_effect_copy_for_target_player(int player, int card)
{
  struct
  {
    int target_player;
    int effect_card;
  } s;

  s.target_player = (int)PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  s.effect_card = add_card_to_hand(s.target_player, g_damage_effect_internal_card_id);
  if (s.effect_card != -1)
  {
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).state |= STATE_IN_PLAY;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).token_status |= STATUS_OBLITERATED;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).info_slot = 1;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).color = PLAYER_CARD_INSTANCE(player, card).color;
    *(unsigned int *)&PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).display_pic_csv_id =
        (get_card_display_pic_num(0x1bc, player, card) << 16) | 0x1bc;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).damage_source_player = (char)player;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).damage_source_card = card;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).damage_target_player = (char)s.target_player;
    PLAYER_CARD_INSTANCE(s.target_player, s.effect_card).damage_target_card = -1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053e216
// FUNCTION: SHANDALAR 0x0048c465
int card_giant_tortoise(int player, int card, event_t event)
{
  if (event == EVENT_TOUGHNESS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    g_event_result += 3;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053e28a
// FUNCTION: SHANDALAR 0x0048c4db
int card_personal_incarnation(int player, int card, event_t event)
{
  target_t target;
  int damage_source_player;
  int max_damage;
  int damage_to_deal;
  int damage_card;
  int replacement_card;

  if (event == 0x21)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == card)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    }
    return 0;
  }

  if (event == 0x73)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).info_slot & 1) != 0 && (g_land_can_be_played & 4) != 0)
    {
      return 99;
    }
    return 0;
  }

  if (event == 0x90)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) != 0 && (g_land_can_be_played & 4) != 0)
  {
    damage_source_player = (PLAYER_CARD_INSTANCE(player, card).state & 0x1000) != 0;
    do
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "PERSONAL_INCARNATION");
      }

      if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0, 0,
                               COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id, ~SUB_WALL,
                               -1, -1, 0, 0, 0, g_text_lines[0], 1, &target))
      {
        if (PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_player == player && PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_card == card)
        {
          if (g_duel_ai_mode_state != 1)
          {
            load_text("prompts.txt", "PERSONAL_INCARNATION");
          }
          if (PLAYER_CARD_INSTANCE(target.player, target.card).info_slot + PLAYER_CARD_INSTANCE(player, card).damage_on_card < 6)
          {
            max_damage = 0;
          }
          else
          {
            max_damage = PLAYER_CARD_INSTANCE(target.player, target.card).info_slot - (5 - PLAYER_CARD_INSTANCE(player, card).damage_on_card);
          }
          damage_to_deal = choose_a_number(player, g_text_lines[1], max_damage);
          if (damage_to_deal > PLAYER_CARD_INSTANCE(target.player, target.card).info_slot)
          {
            damage_to_deal = PLAYER_CARD_INSTANCE(target.player, target.card).info_slot;
          }
          damage_card = damage_creature(damage_source_player, -1, damage_to_deal,
                                        PLAYER_CARD_INSTANCE(target.player, target.card).damage_source_player, PLAYER_CARD_INSTANCE(target.player, target.card).damage_source_card);
          if (damage_card != -1)
          {
            PLAYER_CARD_INSTANCE(player, damage_card).display_pic_info = PLAYER_CARD_INSTANCE(target.player, target.card).display_pic_info;
            PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
            PLAYER_CARD_INSTANCE(target.player, target.card).info_slot -= damage_to_deal;
          }
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
    } while (!g_spell_fizzled && (PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_player != player || PLAYER_CARD_INSTANCE(target.player, target.card).damage_target_card != card));
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
    return 0;
  }

  if (event == 0x77 && g_affected_card == card && g_affected_card_controller == player && PLAYER_CARD_INSTANCE(player, card).kill_code != 4 && is_in_play(player, card))
  {
    replacement_card = add_card_to_hand(player, g_duel_generated_internal_card_id_12);
    if (replacement_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, replacement_card).original_internal_card_id = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, replacement_card).state |= (PLAYER_CARD_INSTANCE(player, card).state & 0x1000) | STATE_IN_PLAY;
      PLAYER_CARD_INSTANCE(player, replacement_card).display_pic_info = 0xb5;
      TENTATIVE_set_timestamps(player, replacement_card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053ea3b
// FUNCTION: SHANDALAR 0x0048cc8c
int card_ali_from_cairo(int player, int card, event_t event)
{
  struct
  {
    int damage_to_prevent;
    target_t target;
    char prompt[300];
    int current_card;
  } s;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_modifier += 0x30;
  }
  else if (event == EVENT_DAMAGE_PREVENTION)
  {
    if (g_life[player] > 0)
    {
      s.current_card = 0;
      s.damage_to_prevent = 0;
      while (1)
      {
        if ((g_active_cards_count[g_other_player] > g_active_cards_count[g_active_player]
                 ? g_active_cards_count[g_other_player]
                 : g_active_cards_count[g_active_player]) <= s.current_card)
        {
          break;
        }
        if (PLAYER_CARD_INSTANCE(g_active_player, s.current_card).internal_card_id == g_damage_card_internal_card_id &&
            (PLAYER_CARD_INSTANCE(g_active_player, s.current_card).state & 0x800002) == STATE_IN_PLAY &&
            PLAYER_CARD_INSTANCE(g_active_player, s.current_card).damage_target_player == player &&
            PLAYER_CARD_INSTANCE(g_active_player, s.current_card).damage_target_card == -1)
        {
          s.damage_to_prevent += PLAYER_CARD_INSTANCE(g_active_player, s.current_card).info_slot;
        }
        if (PLAYER_CARD_INSTANCE(g_other_player, s.current_card).internal_card_id == g_damage_card_internal_card_id &&
            (PLAYER_CARD_INSTANCE(g_other_player, s.current_card).state & 0x800002) == STATE_IN_PLAY &&
            PLAYER_CARD_INSTANCE(g_other_player, s.current_card).damage_target_player == player &&
            PLAYER_CARD_INSTANCE(g_other_player, s.current_card).damage_target_card == -1)
        {
          s.damage_to_prevent += PLAYER_CARD_INSTANCE(g_other_player, s.current_card).info_slot;
        }
        s.current_card++;
      }

      if (s.damage_to_prevent > 0 && g_life[player] <= s.damage_to_prevent)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "ALI_FROM_CAIRO");
          sprintf(s.prompt, "\n%s", g_text_lines[0]);
          do_dialog(player, player, card, -1, -1, s.prompt, 0);
        }

        s.damage_to_prevent += 1 - g_life[player];
        PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
        while (PLAYER_CARD_INSTANCE(player, card).info_slot < s.damage_to_prevent)
        {
          if (g_duel_ai_mode_state != 1)
          {
            load_text("prompts.txt", "ALI_FROM_CAIRO_TARGET");
            sprintf(s.prompt, g_text_lines[0], PLAYER_CARD_INSTANCE(player, card).info_slot + 1, s.damage_to_prevent);
          }

          C_real_select_target(player, 2, player, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE,
                               0, 0, COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id,
                               -1, -1, -1, 0, 0, 0, s.prompt, 0, &s.target);
          if (PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_player == player &&
              PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_card == -1)
          {
            PLAYER_CARD_INSTANCE(player, card).info_slot++;
            if (PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot != 0)
            {
              PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot--;
            }
            if (g_recorded_action_player == 1)
            {
              while (PLAYER_CARD_INSTANCE(player, card).info_slot < s.damage_to_prevent &&
                     PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot != 0)
              {
                PLAYER_CARD_INSTANCE(player, card).info_slot++;
                PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot--;
              }
            }
            TENTATIVE_reassess_all_cards(0, 0x20);
          }
          else if (g_duel_ai_mode_state != 1)
          {
            load_text("prompts.txt", "ALI_FROM_CAIRO_INVALID");
            set_duel_prompt_text(g_text_lines[2]);
            Sleep(2000);
            set_duel_prompt_text(g_empty_duel_prompt_text);
          }
        }
      }
    }
  }
  else if (event == EVENT_ATTACK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_score += 0x18;
  }
  else if (event == EVENT_BLOCK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_score -= 0x18;
  }
  else if (event == EVENT_SHOULD_AI_PLAY && (PLAYER_CARD_INSTANCE(player, card).state & STATE_IN_PLAY) != 0)
  {
    if (player == g_other_player)
    {
      g_ai_modifier += 0x1e0;
    }
    else
    {
      g_ai_modifier -= 0x1e0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053f22d
// FUNCTION: SHANDALAR 0x0048d477
int card_fungusaur(int player, int card, event_t event)
{
  if (event == EVENT_UNKNOWN80)
  {
    if (((((g_land_can_be_played & 0x200) != 0) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player)) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == card)) && (0 < PLAYER_CARD_INSTANCE(player, card).damage_on_card))
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }

    return 0;
  }

  if ((((g_trigger_condition == TRIGGER_DEAL_DAMAGE) && (card == g_affected_card)) && (player == g_affected_card_controller)) && (0 < PLAYER_CARD_INSTANCE(player, card).damage_on_card))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
  }
  if ((((g_trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) && ((card == g_affected_card) && ((player == g_affected_card_controller) && (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)))) && (player == g_current_turn))
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if ((event == EVENT_RESOLVE_TRIGGER) || (event == EVENT_SHOULD_AI_PLAY))
    {
      PLAYER_CARD_INSTANCE(player, card).counter_power++;

      PLAYER_CARD_INSTANCE(player, card).counter_toughness++;

      add_special_counters(player, card, 1);
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053f521
// FUNCTION: SHANDALAR 0x0048d76b
int card_carrion_ants(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_ANY, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_ANY, 1) != 0)
    {
      if (player == g_current_player)
      {
        charge_mana(player, COLOR_COLORLESS, -1);
        if (g_x_value < 1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
      }
      else
      {
        charge_mana(player, COLOR_COLORLESS, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= ~STATE_ETB_THIS_TURN;
        legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_POW_BOOST || event == EVENT_TOU_BOOST)
  {
    return has_mana(player, COLOR_ANY, 1);
  }

  if ((event == EVENT_CAN_WASTE_MANA) && g_raw_mana_available[player][COLOR_COLORLESS] != 0)
  {
    g_event_result |= 1;
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (player == g_other_player)
    {
      g_ai_modifier += (g_basiclandtypes_controlled[player][COLOR_ANY] * 3 + 6) * 4;
    }
    else
    {
      g_ai_modifier -= (g_basiclandtypes_controlled[player][COLOR_ANY] * 3 + 6) * 4;
    }
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0053fe1c
// FUNCTION: SHANDALAR 0x0048e066
int card_shivan_dragon(int player, int card, event_t event)
{
  int legacy;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_RED];
    return 0;
  }
  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_RED, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_RED, 1) != 0)
    {
      if (g_current_player == player)
      {
        charge_mana(player, 4, -1);
        if (g_x_value > 0)
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
      else
      {
        charge_mana(player, 4, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
    }

    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      *(int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                    PLAYER_CARD_INSTANCE(player, card).parent_card)
           .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &= ~STATE_ETB_THIS_TURN;
        legacy = create_legacy_effect(g_card_on_stack_controller,
                                      g_card_on_stack,
                                      LEGACY_EFFECT_PUMP,
                                      g_card_on_stack_controller,
                                      g_card_on_stack);
        if (legacy != -1)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, legacy).info_slot |= 0x80000;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
    }

    return 0;
  }
  if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_RED, 1);
  }
  if ((event == EVENT_CAN_WASTE_MANA) && g_raw_mana_available[player][COLOR_RED] != 0)
  {
    g_event_result |= 1;
    return 0;
  }
  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (g_other_player == player)
    {
      g_ai_modifier += (g_basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * 4;
    }
    else
    {
      g_ai_modifier -= (g_basiclandtypes_controlled[player][COLOR_RED] * 3 + 3) * 4;
    }
  }

  if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054060c
// FUNCTION: SHANDALAR 0x0048e853
int card_dragon_whelp(int player, int card, event_t event)
{
  int saved_max_x_value;
  int legacy_card;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_RED];
    return 0;
  }

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (player == g_affected_card_controller))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_RED, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_RED, 1) != 0)
    {
      g_x_value = 0;
      if (player == g_current_player)
      {
        saved_max_x_value = g_max_x_value;
        if ((((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff0000) == (0x30000 & 0xffff0000))) || g_recorded_action_player != 1)
        {
          g_max_x_value = -1;
        }
        else
        {
          g_max_x_value = 3 - ((PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff0000) >> 16);
        }
        charge_mana(player, COLOR_RED, -1);
        if (g_x_value <= 0)
        {
          g_spell_fizzled = 1;
        }
        g_max_x_value = saved_max_x_value;
      }
      else
      {
        charge_mana(player, COLOR_RED, 1);
        g_x_value = 1;
      }

      *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).eot_toughness &= 0xff0000;
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).eot_toughness += (g_x_value << 16) + g_x_value;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &= ~STATE_ETB_THIS_TURN;
        legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
        if (legacy_card != -1)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_POW_BOOST)
  {
    return ClampIntToRange(has_mana(player, COLOR_RED, 1), 0, 3) -
           PLAYER_CARD_INSTANCE(player, card).info_slot;
  }

  if ((((g_trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) && (g_affected_card == card) && (g_affected_card_controller == player) && ((PLAYER_CARD_INSTANCE(player, card).info_slot & 0xff) != 0)) && g_current_turn == player)
  {
    if (PLAYER_CARD_INSTANCE(player, card).info_slot > 3)
    {
      if (event == EVENT_TRIGGER)
      {
        g_event_result |= 2;
      }
      if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
      {
        kill_card(player, card, KILL_DESTROY);
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    }
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (player == g_other_player)
    {
      g_ai_modifier += g_basiclandtypes_controlled[player][COLOR_RED] * 0xc;
    }
    else
    {
      g_ai_modifier -= g_basiclandtypes_controlled[player][COLOR_RED] * 0xc;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00540fc8
// FUNCTION: SHANDALAR 0x0048f20f
int card_goblin_balloon_brigade(int player, int card, event_t event)
{
  int legacy_card;

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0x20;
    return 0;
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_RED];
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((PLAYER_CARD_INSTANCE(player, card).info_slot != 0) && (has_mana(player, COLOR_RED, 1) != 0));
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_RED, 1) != 0)
    {
      charge_mana(player, COLOR_RED, 1);
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if ((g_other_player == player) && ((g_duel_network_flags & 2) == 0))
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot = 0x20;
      legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         g_duel_generated_internal_card_id_03,
                                         g_card_on_stack_controller,
                                         g_card_on_stack);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = 0x20;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }

    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00541405
// FUNCTION: SHANDALAR 0x0048f64a
int card_whirling_dervish(int player, int card, event_t event)
{
  int saved_event_result;

  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_PROT_BLACK << (get_sleighted_color(player, card, COLOR_BLACK) - 1);
    saved_event_result = g_event_result;
    destroy_attached_auras_of_color(player, card, COLOR_BLACK);
    g_event_result = saved_event_result;
  }
  else if (event == EVENT_DEAL_DAMAGE &&
           PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == -1 &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    }
  }
  else if ((((g_trigger_condition == TRIGGER_EOT) || (event == EVENT_SHOULD_AI_PLAY)) &&
            g_affected_card == card &&
            g_affected_card_controller == player &&
            PLAYER_CARD_INSTANCE(player, card).info_slot != 0) &&
           player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      add_special_counter(player, card);
      ++PLAYER_CARD_INSTANCE(player, card).counter_power;
      ++PLAYER_CARD_INSTANCE(player, card).counter_toughness;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00541740
// FUNCTION: SHANDALAR 0x0048f983
int card_nightmare(int player, int card, event_t event)
{
  int legacy_effect_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_effect_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_1a, player, card);
    if (legacy_effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).info_slot = 1;
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).eot_toughness = 0x10d;
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).token_status |= 0x2000000;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_effect_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005418b4
// FUNCTION: SHANDALAR 0x0048faf7
int card_angry_mob(int player, int card, event_t event)
{
  int legacy_effect_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_effect_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_1a, player, card);
    if (legacy_effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).info_slot = 1;
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).eot_toughness = 0x10e;
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, legacy_effect_card).token_status |= 0x2000000;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_effect_card;
    }
  }
  else if ((event == EVENT_POWER || event == EVENT_TOUGHNESS) &&
           g_affected_card == card &&
           g_affected_card_controller == player)
  {
    if (g_current_player == player)
    {
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_source_card)
          .eot_toughness |= 2;
    }
    else
    {
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                           PLAYER_CARD_INSTANCE(player, card).damage_source_card)
          .eot_toughness &= ~STATE_IN_PLAY;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00541b88
// FUNCTION: SHANDALAR 0x0048fdcb
int card_gaea_s_liege(int player, int card, event_t event)
{
  int effect_card;
  target_t selected_target;

  if (event == EVENT_RESOLVE_SPELL)
  {
    effect_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_1a, player, card);
    if (effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, effect_card).info_slot = 3;
      PLAYER_CARD_INSTANCE(player, effect_card).eot_toughness = 0x10d;
      PLAYER_CARD_INSTANCE(player, effect_card).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, effect_card).token_status |= 0x2000000;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = effect_card;
    }
  }
  else if (((event == EVENT_POWER) || (event == EVENT_TOUGHNESS)) && card == g_affected_card && player == g_affected_card_controller)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) == 0)
    {
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player, PLAYER_CARD_INSTANCE(player, card).damage_source_card)
          .eot_toughness |= 1;
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player, PLAYER_CARD_INSTANCE(player, card).damage_source_card)
          .eot_toughness &= ~STATE_IN_PLAY;
    }
    else
    {
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player, PLAYER_CARD_INSTANCE(player, card).damage_source_card)
          .eot_toughness |= 2;
      PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player, PLAYER_CARD_INSTANCE(player, card).damage_source_card)
          .eot_toughness &= ~STATE_JUST_DRAWED;
    }
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 && real_target_available((int *)0,
                                                                                           TARGET_SCAN_DIRECT,
                                                                                           player,
                                                                                           2,
                                                                                           2,
                                                                                           TARGET_ZONE_IN_PLAY,
                                                                                           TYPE_LAND,
                                                                                           TYPE_NONE,
                                                                                           0,
                                                                                           get_protections_from(player, card),
                                                                                           0,
                                                                                           0,
                                                                                           -1,
                                                                                           -1,
                                                                                           -1,
                                                                                           -1,
                                                                                           0,
                                                                                           0,
                                                                                           0) != 0)
    {
      return 1;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "GAEAS_LIEGE");
    }

    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_LAND,
                             TYPE_NONE,
                             0,
                             get_protections_from(player, card),
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             -1,
                             ~SUB_WALL,
                             -1,
                             -1,
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(selected_target, PLAYER_CARD_INSTANCE(player, card).targets[0]);

    if (C_real_validate_target(selected_target.player,
                               selected_target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_LAND,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).info_slot = 3;
      effect_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         g_duel_generated_internal_card_id_0f,
                                         selected_target.player,
                                         selected_target.card);
      if (effect_card != -1)
      {
        PLAYER_CARD_INSTANCE(g_card_on_stack_controller, effect_card).state |= 0x10;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }

    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
  }
  else if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) && card == g_affected_card && player == g_affected_card_controller && (PLAYER_CARD_INSTANCE(player, card).info_slot & 0x4000) != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~STATE_BLOCKED;
    C_get_abilities(player, card, EVENT_POWER, -1);
    C_get_abilities(player, card, EVENT_TOUGHNESS, -1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x005424a4
// FUNCTION: SHANDALAR 0x004906e7
int card_plague_rats(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_1a, player, card);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0x20f;
      PLAYER_CARD_INSTANCE(player, legacy_card).token_status = STATUS_INVISIBLE_FX | STATUS_PERMANENT;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (unsigned char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542610
// FUNCTION: SHANDALAR 0x00490853
int card_keldon_warlord(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_1a, player, card);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0x80d;
      PLAYER_CARD_INSTANCE(player, legacy_card).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542720
// FUNCTION: SHANDALAR 0x00490963
int card_ghost_ship(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return generic_regeneration_ability(player, card, event, 2, 3);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054276d
// FUNCTION: SHANDALAR 0x004909b0
int generic_regeneration_ability(int player, int card, event_t event, unsigned int color, int amount)
{
  int can_activate;

  if (event == EVENT_CAN_ACTIVATE && (g_land_can_be_played & 0x200) != 0 && PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    can_activate = 1;
    if (PLAYER_CARD_INSTANCE(player, card).kill_code != 2)
    {
      can_activate = 0;
    }
    if (can_activate != 0 && (PLAYER_CARD_INSTANCE(player, card).state & 0x800002) != 2)
    {
      can_activate = 0;
    }
    if (can_activate != 0 && (PLAYER_CARD_INSTANCE(player, card).regen_status & 0x200) == 0)
    {
      can_activate = 0;
    }
    if (can_activate != 0 && has_mana(player, color, amount) == 0)
    {
      can_activate = 0;
    }
    if (can_activate != 0)
    {
      return 99;
    }
    return 0;
  }
  else if (event == 0x90)
  {
    load_recorded_action_target(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE && (g_land_can_be_played & 0x200) != 0)
  {
    charge_mana(player, color, amount);
    if (g_spell_fizzled != 1)
    {
      g_upkeep_payment_completed = 1;
      PLAYER_CARD_INSTANCE(player, card).info_slot += 1;
    }

    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION && (g_land_can_be_played & 0x200) != 0)
  {
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).info_slot = 0;
    regenerate_card(g_card_on_stack_controller, g_card_on_stack);
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542a2a
// FUNCTION: SHANDALAR 0x00490c6d
void regenerate_card(int player, int card)
{
  struct
  {
    int found_linked_card; /* [ebp-0xc] */
    int test_player;       /* [ebp-0x8] */
    int test_card;         /* [ebp-0x4] */
  } s;

  s.test_player = 0;
  s.found_linked_card = 0;

  while (s.test_player < 2 && s.found_linked_card == 0)
  {
    s.test_card = 0;
    while (s.test_card < g_active_cards_count[s.test_player] && s.found_linked_card == 0)
    {
      if (is_in_play(s.test_player, s.test_card) != 0 &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_player == player &&
          PLAYER_CARD_INSTANCE(s.test_player, s.test_card).damage_target_card == card &&
          ((PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == g_duel_generated_internal_card_id_0f &&
            (PLAYER_CARD_INSTANCE(s.test_player, s.test_card).token_status & 0x00800000) != 0) ||
           PLAYER_CARD_INSTANCE(s.test_player, s.test_card).internal_card_id == g_duel_generated_internal_card_id_17))
      {
        s.found_linked_card = 1;
      }

      ++s.test_card;
    }

    ++s.test_player;
  }

  if (s.found_linked_card == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).kill_code = 0;
    PLAYER_CARD_INSTANCE(player, card).unknown0x14 = 0;
    PLAYER_CARD_INSTANCE(player, card).damage_on_card = 0;
    tap_card_and_dispatch_event(player, card);
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_REGEN);
    }
    PLAYER_CARD_INSTANCE(player, card).token_status &= ~0x80u;
    PLAYER_CARD_INSTANCE(player, card).blocking = 0xff;
    PLAYER_CARD_INSTANCE(player, card).state &= ~0x0c;
  }
}

// FUNCTION: MAGIC 0x00542d29
// FUNCTION: SHANDALAR 0x00490f6c
int card_uthden_troll(int player, int card, event_t event)
{
  unsigned int color;

  color = single_color_test_bit_to_color_t(
      *(char *)&global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].color);
  if (event == EVENT_UNTAP_PHASE)
  {
    g_ai_mana_demand_by_color[player][color] += 2;
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return generic_regeneration_ability(player, card, event, color, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00542de2
// FUNCTION: SHANDALAR 0x00491025
int card_vampire_bats(int player, int card, event_t event)
{
  int legacy_card;
  int saved_max_x_value;
  int mana_available;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_BLACK];
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (player == g_other_player &&
        (g_duel_network_flags & 2) == 0 &&
        player == g_current_player &&
        PLAYER_CARD_INSTANCE(player, card).info_slot != 0 &&
        g_current_phase < 0x1a)
    {
      return 0;
    }
    return has_mana(player, COLOR_BLACK, 1) &&
           (int)(PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xffff0000) < 0x20000;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    mana_available = has_mana(player, COLOR_BLACK, 1);
    saved_max_x_value = g_max_x_value;
    if (mana_available != 0 && (int)(PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xffff0000) < 0x20000)
    {
      if (((PLAYER_CARD_INSTANCE(player, card).eot_toughness >> 16) & 0xf) == 0)
      {
        g_max_x_value = 2;
      }
      else
      {
        g_max_x_value = 1;
      }
      if (player == g_current_player)
      {
        charge_mana(player, COLOR_BLACK, -1);
        if (g_x_value < 1)
        {
          g_spell_fizzled = 1;
        }
      }
      else
      {
        charge_mana(player, COLOR_BLACK, 1);
        g_x_value = 1;
      }

      g_max_x_value = saved_max_x_value;
      PLAYER_CARD_INSTANCE(player, card).eot_toughness &= 0xf0000;
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).eot_toughness += g_x_value * 0x10001;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id == -1)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= ~STATE_ETB_THIS_TURN;
        legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_POW_BOOST)
  {
    return ClampIntToRange(g_special_mana_pool[player][COLOR_BLACK],
                           0,
                           2 - PLAYER_CARD_INSTANCE(player, card).info_slot);
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00543640
// FUNCTION: SHANDALAR 0x00491883
int card_frozen_shade(int player, int card, event_t event)
{
  int legacy_effect_card;

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    return 0;
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_BLACK];
    return 0;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_BLACK, 1);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_BLACK, 1) != 0)
    {
      if (player == g_current_player)
      {
        charge_mana(player, COLOR_BLACK, -1);
        if (g_x_value > 0)
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
      else
      {
        charge_mana(player, COLOR_BLACK, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &= ~STATE_ETB_THIS_TURN;
        legacy_effect_card = create_legacy_effect(g_card_on_stack_controller,
                                                  g_card_on_stack,
                                                  LEGACY_EFFECT_PUMP,
                                                  g_card_on_stack_controller,
                                                  g_card_on_stack);
        if (legacy_effect_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_effect_card).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, legacy_effect_card).counter_toughness = 1;
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, legacy_effect_card).info_slot |= 0x80000;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_BLACK, 1);
  }

  if (event == EVENT_TOU_BOOST)
  {
    return has_mana(player, COLOR_BLACK, 1);
  }

  if ((event == EVENT_CAN_WASTE_MANA) && (g_raw_mana_available[player][COLOR_BLACK] != 0))
  {
    g_event_result |= 1;
    return 0;
  }

  if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00543f11
// FUNCTION: SHANDALAR 0x00492155
int card_killer_bees(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_GREEN];
    return 0;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if (g_affected_card == card)
    {
      if (g_affected_card_controller == player)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
        return 0;
      }
    }
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_GREEN, 1);
  }
  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }
  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_GREEN, 1) != 0)
    {
      if (g_current_player == player)
      {
        charge_mana(player, COLOR_GREEN, -1);
        if (g_x_value > 0)
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
      else
      {
        charge_mana(player, COLOR_GREEN, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
    }
    return 0;
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;

      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += (PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff) * 0x100;

      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;

      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= ~0x80000;
        legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }

    return 0;
  }
  if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_GREEN, 1);
  }
  if (event == EVENT_TOU_BOOST)
  {
    return has_mana(player, COLOR_GREEN, 1);
  }
  if ((event == EVENT_CAN_WASTE_MANA) && g_raw_mana_available[player][COLOR_GREEN] != 0)
  {
    g_event_result |= 1;
    return 0;
  }
  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (player == g_other_player)
    {
      g_ai_modifier += (g_basiclandtypes_controlled[player][COLOR_GREEN] + 2) * 3 * 4;
    }
    else
    {
      g_ai_modifier -= (g_basiclandtypes_controlled[player][COLOR_GREEN] + 2) * 3 * 4;
    }
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005447da
// FUNCTION: SHANDALAR 0x00492a1e
int card_wall_of_water(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_BLUE];
    return 0;
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if (g_affected_card == card)
    {
      if (g_affected_card_controller == player)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
        return 0;
      }
    }
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_BLUE, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    return 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_BLUE, 1) != 0)
    {
      if (player == g_current_player)
      {
        charge_mana(player, COLOR_BLUE, -1);
        if (g_x_value < 1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = g_x_value;
        }
      }
      else
      {
        charge_mana(player, COLOR_BLUE, 1);
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
      }

      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
        {
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      }
    }
    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      *(int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                    PLAYER_CARD_INSTANCE(player, card).parent_card)
           .info_slot += PLAYER_CARD_INSTANCE(player, card).eot_toughness & 0xff;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        *(unsigned int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
             .info_slot &= ~STATE_ETB_THIS_TURN;
        legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
          *(unsigned int *)&PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }
  else if (event == EVENT_POW_BOOST)
  {
    return has_mana(player, COLOR_BLUE, 1);
  }
  else if ((event == EVENT_CAN_WASTE_MANA) && g_raw_mana_available[player][COLOR_BLUE] != 0)
  {
    g_event_result |= 1;
    return 0;
  }
  else
  {
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      if (player == g_other_player)
      {
        g_ai_modifier += g_basiclandtypes_controlled[player][COLOR_BLUE] * 0xc;
      }
      else
      {
        g_ai_modifier -= g_basiclandtypes_controlled[player][COLOR_BLUE] * 0xc;
      }
    }

    if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
      PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    }
    return 0;
  }
}

// FUNCTION: MAGIC 0x00544ff5
// FUNCTION: SHANDALAR 0x00493239
int card_rag_man(int player, int card, event_t event)
{
  struct
  {
    int selected_card;
    int attempts;
    int target_player;
    int current_card;
    int internal_card_id;
    int hand_internal_ids[30];
    int found;
  } s;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_BLACK];
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_mana(player, COLOR_BLACK, 3) == 0 ||
        (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) != 0)
    {
      return 0;
    }
    return player == g_current_player;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      charge_mana(player, COLOR_BLACK, 3);
      if (g_spell_fizzled != 1)
      {
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    s.target_player = 1 - player;

    if ((g_active_player == player || (g_duel_network_flags & 2) != 0) &&
        g_duel_ai_mode_state != 1 &&
        g_duel_network_state == 0)
    {
      if (g_active_player == player)
      {
        s.attempts = 0;
        for (s.current_card = 0; s.current_card < g_active_cards_count[s.target_player]; ++s.current_card)
        {
          s.internal_card_id = PLAYER_CARD_INSTANCE(s.target_player, s.current_card).internal_card_id;
          if (s.internal_card_id != -1 &&
              (PLAYER_CARD_INSTANCE(s.target_player, s.current_card).state & STATE_IN_PLAY) == 0)
          {
            s.hand_internal_ids[s.attempts] = s.internal_card_id;
            ++s.attempts;
          }
        }
        load_text("prompts.txt", "RAGMAN");
      }
      show_deck(player, s.hand_internal_ids, s.attempts,
                g_text_lines[0], 0, gs_done_008b40e0);
    }

    if ((g_duel_network_flags & 2) != 0 && g_other_player == player)
    {
      TENTATIVE_wait_for_network_result(player, 0x14);
      s.selected_card = g_network_result_packet.result;
      if (s.selected_card == -1)
      {
        s.attempts = 999;
      }
    }
    else
    {
      s.attempts = 0;
      do
      {
        s.selected_card = internal_rand(g_active_cards_count[s.target_player]);
        s.internal_card_id = PLAYER_CARD_INSTANCE(s.target_player, s.selected_card).internal_card_id;
        ++s.attempts;
        if (s.attempts >= 999)
        {
          break;
        }
      } while (s.internal_card_id == -1 ||
               (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) == 0 ||
               (PLAYER_CARD_INSTANCE(s.target_player, s.selected_card).state & STATE_IN_PLAY) != 0);

      if ((g_duel_network_flags & 2) != 0 && g_active_player == player)
      {
        if (s.attempts >= 999)
        {
          s.selected_card = -1;
        }
        g_network_result_packet.result = s.selected_card;
        g_network_result_packet.packet_type = 0x14;
        TENTATIVE_send_network_result(player, 0x14);
      }
    }

    if (s.attempts < 999)
    {
      s.found = 1;
    }
    else
    {
      s.found = 0;
      for (s.current_card = 0;
           s.current_card < g_active_cards_count[s.target_player] && s.found == 0;
           ++s.current_card)
      {
        s.internal_card_id = PLAYER_CARD_INSTANCE(s.target_player, s.current_card).internal_card_id;
        if (s.internal_card_id != -1 &&
            (global_cards_data[s.internal_card_id].type & TYPE_CREATURE) != 0 &&
            (PLAYER_CARD_INSTANCE(s.target_player, s.current_card).state & STATE_IN_PLAY) == 0)
        {
          s.found = 1;
          s.selected_card = s.current_card;
        }
      }
    }

    if (s.found != 0)
    {
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(0x19);
      }
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "RAGMAN");
        do_dialog(player, player, card, s.target_player, s.selected_card, g_text_lines[1], 0);
      }
      move_card_to_graveyard(s.target_player, s.selected_card);
      PLAYER_CARD_INSTANCE(s.target_player, s.selected_card).internal_card_id = -1;
      --g_duel_summary.hand_counts[s.target_player];
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005455e2
// FUNCTION: SHANDALAR 0x00493829
int card_time_elemental(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_BLUE];
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           has_mana(player, COLOR_BLUE, 2) != 0 &&
           has_mana(player, COLOR_ANY, 4) != 0 &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 -1,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0x40) != 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }
  else if (event == EVENT_ACTIVATE &&
           (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
  {
    g_mana_charge[COLOR_COLORLESS] = 2;
    charge_mana(player, COLOR_BLUE, 2);
    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "TIME_ELEMENTAL");
      }
      if (C_real_select_target(player,
                               2,
                               1 - player,
                               TARGET_ZONE_IN_PLAY,
                               TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               -1,
                               -1,
                               -1,
                               0,
                               0,
                               0x40,
                               g_text_lines[0],
                               1,
                               &target) != 0)
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               -1,
                               -1,
                               -1,
                               0,
                               0,
                               0x40))
    {
      hurkyls_recall_bounce_artifact(target.player, target.card);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }
  else
  {
    if ((event == EVENT_DECLARE_ATTACKERS || event == EVENT_SHOULD_AI_PLAY) &&
        (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0)
    {
      create_legacy_effect(player, card, g_duel_generated_internal_card_id_1c, player, -1);
    }
    if ((event == EVENT_DECLARE_BLOCKERS || event == EVENT_SHOULD_AI_PLAY) &&
        g_current_phase == PHASE_DECLARE_BLOCKERS &&
        PLAYER_CARD_INSTANCE(player, card).blocking != -1 &&
        global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].code_pointer !=
            card_two_headed_giant_of_foriys_legacy)
    {
      create_legacy_effect(player, card, g_duel_generated_internal_card_id_1c, player, -1);
    }
    if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
    if (event == EVENT_ATTACK_RATING &&
        g_affected_card == card &&
        g_affected_card_controller == player)
    {
      g_ai_score += 0x78;
    }
    else if (event == EVENT_BLOCK_RATING &&
             g_affected_card == card &&
             g_affected_card_controller == player)
    {
      g_ai_score -= 0x78;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00545c2e
// FUNCTION: SHANDALAR 0x00493e75
int card_northern_paladin(int player, int card, event_t event)
{
  target_t target;
  int result;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_WHITE];
    result = 0;
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    result = 1;
    if ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) != 0)
    {
      result = 0;
    }
    if (result != 0 && has_mana(player, COLOR_WHITE, 2) == 0)
    {
      result = 0;
    }
    if (result != 0)
    {
      result = real_target_available((int *)0,
                                     TARGET_SCAN_DIRECT,
                                     player,
                                     2,
                                     2,
                                     TARGET_ZONE_IN_PLAY,
                                     TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                     TYPE_NONE,
                                     0,
                                     get_protections_from(player, card),
                                     1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
                                     0,
                                     -1,
                                     -1,
                                     0xffffffff,
                                     0xffffffff,
                                     0,
                                     0,
                                     0);
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = has_mana(player, COLOR_WHITE, 2);
    if (result != 0 && (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
    {
      charge_mana(player, COLOR_WHITE, 2);
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "NORTHERN_PALADIN");
        }

        if (C_real_select_target(player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0,
                                 g_text_lines[0],
                                 1,
                                 &target) != 0)
        {
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
    }
    result = 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TARGET_TYPE_TOKEN | TYPE_ARTIFACT | TYPE_ENCHANTMENT | TYPE_CREATURE | TYPE_LAND,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               1 << (unsigned char)get_sleighted_color(player, card, COLOR_BLACK),
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0) != 0)
    {
      kill_card(target.player, target.card, KILL_DESTROY);
    }
    else
    {
      g_spell_fizzled = 1;
    }

    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    result = 0;
  }
  else
  {
    result = 0;
  }

  return result;
}

// FUNCTION: MAGIC 0x005460cb
// FUNCTION: SHANDALAR 0x00494312
int card_royal_assassin(int player, int card, event_t event)
{
  int result;
  target_t selected_target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = 0;
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      result = real_target_available((int *)0,
                                     TARGET_SCAN_DIRECT,
                                     player,
                                     2,
                                     2,
                                     0x200,
                                     2,
                                     0,
                                     0,
                                     get_protections_from(player, card),
                                     0,
                                     0,
                                     -1,
                                     -1,
                                     -1,
                                     -1,
                                     0,
                                     1,
                                     0);
    }
    return result;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ROYAL_ASSASSIN");
    }

    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE,
                             TYPE_NONE,
                             0,
                             get_protections_from(player, card),
                             0,
                             0,
                             -1,
                             ~SUB_WALL,
                             -1,
                             -1,
                             0,
                             TARGET_STATE_TAPPED,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(selected_target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(selected_target.player,
                               selected_target.card,
                               (char *)0,
                               player,
                               ANYBODY,
                               ANYBODY,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               TARGET_STATE_TAPPED,
                               0))
    {
      kill_card(selected_target.player, selected_target.card, KILL_DESTROY);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }
  else if (((event == EVENT_ATTACK_RATING) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_score += 0x30;
    return 0;
  }
  else if (((event == EVENT_BLOCK_RATING) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    g_ai_score += -0x30;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005466d0
// FUNCTION: SHANDALAR 0x00494917
int card_osai_vultures(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (int)C_get_special_counters(player, card) > 1;
  }

  if (event == EVENT_ACTIVATE && (int)C_get_special_counters(player, card) > 1)
  {
    PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
    PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    if (PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot |= 0x80000;
    }
    remove_special_counters(player, card, 2);
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += 1;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += 0x100;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
      if ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                PLAYER_CARD_INSTANCE(player, card).parent_card)
               .info_slot &
           0x80000) != 0)
      {
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot &= ~0x80000;
        legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           LEGACY_EFFECT_PUMP,
                                           g_card_on_stack_controller,
                                           g_card_on_stack);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = 1;
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot |= 0x80000;
        }
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if ((g_trigger_condition == TRIGGER_EOT || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_duel_summary.creatures_died != 0 &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      add_special_counter(player, card);
    }
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00546d3b
// FUNCTION: SHANDALAR 0x00494f82
int card_murk_dwellers(int player, int card, event_t event)
{
  int blocker;
  int defending_player;
  int unblocked;

  if (event == EVENT_DECLARE_BLOCKERS &&
      player == g_current_player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0)
  {
    defending_player = 1 - player;
    unblocked = 1;
    for (blocker = 0; blocker < g_active_cards_count[defending_player]; ++blocker)
    {
      if (is_in_play(defending_player, blocker) &&
          PLAYER_CARD_INSTANCE(defending_player, blocker).blocking == card)
      {
        unblocked = 0;
        break;
      }
    }
    if (unblocked)
    {
      give_pump_until_eot(player, card, player, card, 2, 0);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00546eca
// FUNCTION: SHANDALAR 0x00495111
int card_sorceress_queen(int player, int card, event_t event)
{
  target_t target;
  int current_card;
  int current_player;
  int legacy_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE &&
      (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "SORCERESS_QUEEN");
    }
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_CANNOT_TARGET;
    TENTATIVE_reassess_all_cards(0, 0x20);
    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE,
                             TYPE_NONE,
                             0,
                             get_protections_from(player, card),
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             -1,
                             ~SUB_WALL,
                             -1,
                             -1,
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_CANNOT_TARGET;
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      for (current_player = 0; current_player < 2; ++current_player)
      {
        for (current_card = 0; current_card < g_active_cards_count[current_player]; ++current_card)
        {
          if (PLAYER_CARD_INSTANCE(current_player, current_card).internal_card_id == g_duel_generated_iid_1b &&
              (PLAYER_CARD_INSTANCE(current_player, current_card).state &
               (STATE_OUBLIETTED | STATE_IN_PLAY)) == STATE_IN_PLAY &&
              PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_player == target.player &&
              PLAYER_CARD_INSTANCE(current_player, current_card).damage_target_card == target.card)
          {
            PLAYER_CARD_INSTANCE(current_player, current_card).token_status &= ~0x1000000;
          }
        }
      }

      legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         g_duel_generated_iid_1b,
                                         target.player,
                                         target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).token_status |= 0x1000000;
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_power =
            -(global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].power &
              0xbfff);
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness =
            2 - (global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].toughness &
                 0xbfff);
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_ATTACK_RATING && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_score += 12;
    return 0;
  }

  if (event == EVENT_BLOCK_RATING && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_score -= 12;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054760a
// FUNCTION: SHANDALAR 0x00495851
int card_stone_giant(int player, int card, event_t event)
{
  struct
  {
    int result;
    target_t selected_target;
    int legacy_card;
  } s;

  if (event == EVENT_CAN_ACTIVATE)
  {
    s.result = 1;
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) != 0)
    {
      s.result = 0;
    }
    if (s.result)
    {
      s.result = real_target_available((int *)0,
                                       TARGET_SCAN_DIRECT,
                                       player,
                                       player,
                                       player,
                                       TARGET_ZONE_IN_PLAY,
                                       TYPE_CREATURE,
                                       0,
                                       0,
                                       get_protections_from(player, card),
                                       0,
                                       0,
                                       -1,
                                       -1,
                                       (int)PLAYER_CARD_INSTANCE(player, card).power - 1U | 0x2000,
                                       0xffffffff,
                                       0,
                                       0,
                                       0);
    }
    return s.result;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "STONE_GIANT");
    }
    if (C_real_select_target(player,
                             player,
                             player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE,
                             TYPE_NONE,
                             0,
                             get_protections_from(player, card),
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             -1,
                             ~SUB_WALL,
                             -1,
                             C_get_abilities(player, card, EVENT_POWER, -1) - 1U | 0x2000,
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &s.selected_target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], s.selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if ((g_other_player == player) &&
          ((PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                .regen_status &
            0x20) != 0))
      {
        g_ai_modifier += -0x60;
      }
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(s.selected_target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(s.selected_target.player,
                               s.selected_target.card,
                               (char *)0,
                               player,
                               player,
                               player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               (int)PLAYER_CARD_INSTANCE(player, card).power - 1U | TARGET_PT_LESSER_OR_EQUAL,
                               0,
                               0,
                               0))
    {
      s.legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           g_duel_generated_internal_card_id_03,
                                           s.selected_target.player,
                                           s.selected_target.card);
      if (s.legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, s.legacy_card).kill_code = 5;
        PLAYER_CARD_INSTANCE(player, s.legacy_card).info_slot = 0x20;
        PLAYER_CARD_INSTANCE(s.selected_target.player, s.selected_target.card).regen_status = 0x8000000;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00547bb3
// FUNCTION: SHANDALAR 0x00495df9
int card_dwarven_warriors(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 && real_target_available((int *)0,
                                                                                           TARGET_SCAN_DIRECT,
                                                                                           player,
                                                                                           2,
                                                                                           2,
                                                                                           TARGET_ZONE_IN_PLAY,
                                                                                           TYPE_CREATURE,
                                                                                           TYPE_NONE,
                                                                                           0,
                                                                                           get_protections_from(player, card),
                                                                                           COLOR_TEST_0,
                                                                                           COLOR_TEST_0,
                                                                                           -1,
                                                                                           -1,
                                                                                           0x2002,
                                                                                           0xffffffff,
                                                                                           0,
                                                                                           0,
                                                                                           0))
    {
      return 1;
    }
  }
  else if (event == 0x90)
  {
    load_recorded_action_target(0);
  }
  else if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0))
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "DWARVEN_WARRIORS");
    }
    if (C_real_select_target(player,
                             2,
                             player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE,
                             TYPE_NONE,
                             0,
                             get_protections_from(player, card),
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             -1,
                             ~SUB_WALL,
                             0x2002,
                             -1,
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               0x2002,
                               -1,
                               0,
                               0,
                               0))
    {
      create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_02, target.player, target.card);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00547f99
// FUNCTION: SHANDALAR 0x004961df
int card_cave_people(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
           has_mana(player, COLOR_RED, 2) &&
           has_mana(player, COLOR_ANY, 3) &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & 0x44) != 0)
  {
    if (event == EVENT_POWER)
    {
      ++g_event_result;
    }
    if (event == EVENT_TOUGHNESS)
    {
      g_event_result -= 2;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      g_mana_charge[COLOR_COLORLESS] = 1;
      charge_mana(player, COLOR_RED, 2);
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "CAVE_PEOPLE");
        }
        if (C_real_select_target(player,
                                 2,
                                 player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0,
                                 g_text_lines[0],
                                 1,
                                 &target))
        {
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          if (player == g_other_player &&
              ((PLAYER_CARD_INSTANCE(target.player, target.card).regen_status &
                (1 << (unsigned char)(get_hacked_color(player, card, COLOR_RED) - 1))) != 0 ||
               target.player == g_active_player))
          {
            g_ai_modifier -= 0x60;
          }
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         g_duel_generated_internal_card_id_03,
                                         target.player,
                                         target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot =
            1 << (unsigned char)(get_hacked_color(player, card, COLOR_RED) - 1);
      }
      PLAYER_CARD_INSTANCE(target.player, target.card).regen_status = KEYWORD_RECALC_ABILITIES;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x005485e9
// FUNCTION: SHANDALAR 0x0049682f
int card_pradesh_gypsies(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;
  int result;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = 1;
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) != 0)
    {
      result = 0;
    }
    if (result != 0 && has_mana(player, COLOR_GREEN, 1) == 0)
    {
      result = 0;
    }
    if (result != 0 && has_mana(player, COLOR_ANY, 2) == 0)
    {
      result = 0;
    }
    if (result != 0)
    {
      result = real_target_available((int *)0,
                                     TARGET_SCAN_DIRECT,
                                     player,
                                     2,
                                     2,
                                     TARGET_ZONE_IN_PLAY,
                                     TYPE_CREATURE,
                                     TYPE_NONE,
                                     0,
                                     get_protections_from(player, card),
                                     COLOR_TEST_0,
                                     COLOR_TEST_0,
                                     -1,
                                     ~SUB_WALL,
                                     -1,
                                     -1,
                                     0,
                                     0,
                                     0);
    }
    return result;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      g_mana_charge[COLOR_COLORLESS] = 1;
      charge_mana(player, COLOR_GREEN, 1);
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "PRADESH_GYPSIES");
        }

        if (C_real_select_target(player,
                                 2,
                                 player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0,
                                 g_text_lines[0],
                                 1,
                                 &target))
        {
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         LEGACY_EFFECT_PUMP,
                                         target.player,
                                         target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = -2;
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = 0;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  if (event == EVENT_CHECK_PUMP && (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
  {
    if (has_mana(player, COLOR_GREEN, 1) != 0 && has_mana(player, COLOR_ANY, 2) != 0)
    {
      g_global_power_bonus[1 - player] -= 2;
    }
    return 0;
  }

  if (event == EVENT_ATTACK_RATING &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      has_mana(player, COLOR_GREEN, 1) != 0)
  {
    g_ai_score += 0xc;
    return 0;
  }

  if (event == EVENT_BLOCK_RATING &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      has_mana(player, COLOR_GREEN, 1) != 0)
  {
    g_ai_score -= 0xc;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00548bd9
// FUNCTION: SHANDALAR 0x00496e1d
int card_samite_healer(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    int result = 1;
    if ((g_land_can_be_played & 4) == 0)
    {
      result = 0;
    }
    if (PLAYER_CARD_INSTANCE(player, card).state & 0x20010)
    {
      result = 0;
    }
    if (result != 0)
    {
      if (real_target_available((int *)0,
                                TARGET_SCAN_DIRECT,
                                player,
                                2,
                                2,
                                0x200,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                g_damage_card_internal_card_id,
                                -1,
                                -1,
                                -1,
                                0,
                                0,
                                0))
      {
        result = 99;
      }
      else
      {
        result = 0;
      }
    }
    return result;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "SAMITE_HEALER");
    }
    if (C_real_select_target(player,
                             2,
                             2,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_NONE,
                             TYPE_NONE,
                             0,
                             0,
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             g_damage_card_internal_card_id,
                             -1,
                             -1,
                             -1,
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_NONE,
                               TYPE_NONE,
                               0,
                               0,
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               g_damage_card_internal_card_id,
                               -1,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      if (PLAYER_CARD_INSTANCE(target.player, target.card).info_slot != 0)
      {
        --PLAYER_CARD_INSTANCE(target.player, target.card).info_slot;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  if (event == 0x3b && (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
  {
    ++g_global_toughness_bonus[player];
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549014
// FUNCTION: SHANDALAR 0x00497258
int card_verduran_enchantress(int player, int card, event_t event)
{
  int library_position;

  if ((((((g_trigger_condition == TRIGGER_SPELL_CAST) && (g_affected_card == card)) && (g_affected_card_controller == player)) && ((g_current_turn == player) && (g_current_player == player))) && (((PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0) && ((g_affected_card_controller == player) && (PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id != -1)))) && ((global_cards_data[PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id]
                                                                                                                                                                                                                                                                                                                                                                                                                  .type &
                                                                                                                                                                                                                                                                                                                                                                                                              TYPE_ENCHANTMENT) != 0))
  {
    if (event == EVENT_TRIGGER)
    {
      if ((g_active_player == player) || ((g_duel_network_flags & 2) != 0))
      {
        g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
      }
      else
      {
        library_position = 0;
        while ((library_position < 500) && (global_library[player][library_position] != -1))
        {
          ++library_position;
        }

        if ((g_duel_summary.hand_counts[player] < 8) && (5 < library_position))
        {
          g_event_result |= RESOLVE_TRIGGER_MANDATORY;
        }
        else
        {
          g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
        }
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      dispatch_event(player, card, EVENT_PLAY_ABILITY);
      draw_card_for_player(player);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054920c
// FUNCTION: SHANDALAR 0x0049744a
int card_serra_angel(int player, int card, event_t event)
{
  if (g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_VIGILANCE;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054926e
// FUNCTION: SHANDALAR 0x004974ab
int card_giant_spider(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES)
  {
    if (g_affected_card == card)
    {
      if (g_affected_card_controller == player)
      {
        if (g_current_player == player)
        {
          g_event_result &= ~KEYWORD_FLYING;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x005492be
// FUNCTION: SHANDALAR 0x004974fb
int card_zombie_master(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES)
  {
    if (is_in_play(player, card))
    {
      if (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card)
                                .internal_card_id]
              .subtype == SUB_ZOMBIE)
      {
        g_event_result |= (1 << (get_hacked_color(player, card, COLOR_BLACK) - 1U)) + KEYWORD_REGENERATION;
      }
    }
  }

  if (((event == EVENT_GRAVEYARD_FROM_PLAY) && (g_affected_card == card)) && (player == g_affected_card_controller))
  {
    dispatch_three_arg_callback_to_cards_in_play(mark_zombie_as_nonregenerable, -1);
    TENTATIVE_reassess_all_cards(0, 0xff);
  }

  return 0;
}

// FUNCTION: MAGIC 0x005493a6
// FUNCTION: SHANDALAR 0x004975e2
int mark_zombie_as_nonregenerable(int player, int card, int internal_card_id)
{
  if (global_cards_data[internal_card_id].subtype == SUB_ZOMBIE)
  {
    PLAYER_CARD_INSTANCE(player, card).regen_status |= 0x8000000;
  }

  return 1;
}

// FUNCTION: MAGIC 0x00549404
// FUNCTION: SHANDALAR 0x00497640
int card_scathe_zombies(int player, int card, event_t event)
{
  if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) || (event == EVENT_RESOLVE_ACTIVATION))
  {
    return generic_regeneration_ability(player, card, event, 1, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549451
// FUNCTION: SHANDALAR 0x0049768d
int card_goblin_king(int player, int card, event_t event)
{
  if (is_in_play(player, card) && is_in_play(g_affected_card_controller, g_affected_card) && (char)global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype == 3)
  {
    if (event == EVENT_ABILITIES)
    {
      g_event_result |= 1 << (get_hacked_color(player, card, COLOR_RED) - 1);
    }
    if (event == EVENT_POWER || event == EVENT_TOUGHNESS)
    {
      ++g_event_result;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549524
// FUNCTION: SHANDALAR 0x00497760
int card_erg_raiders(int player, int card, event_t event)
{
  if (event == EVENT_SHOULD_AI_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_current_player == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & 0x30044) == 0)
  {
    if ((g_active_player == player || (g_duel_network_flags & 2) != 0) &&
        g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ERGRAIDERS");
      do_dialog(player, player, card, -1, -1, g_text_lines[0], 0);
    }
    damage_player(player, 2, player, card);
  }

  if (g_trigger_condition == TRIGGER_EOT &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_current_player == player &&
      player == g_current_turn &&
      (PLAYER_CARD_INSTANCE(player, card).state & 0x30044) == 0)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      if ((g_active_player == player || (g_duel_network_flags & 2) != 0) &&
          g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "ERGRAIDERS");
        do_dialog(player, player, card, -1, -1, g_text_lines[0], 0);
      }
      damage_player(player, 2, player, card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00549726
// FUNCTION: SHANDALAR 0x00497962
int card_el_hajjaj(int player, int card, event_t event)
{
  int damage_player;
  int damage_card;
  int target_toughness;
  int life_gain;

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id ==
          g_damage_card_internal_card_id &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player &&
        PLAYER_CARD_INSTANCE(player, card).eot_toughness < 19)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).eot_toughness].card =
          g_affected_card;
      PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).eot_toughness].player =
          g_affected_card_controller;
      ++PLAYER_CARD_INSTANCE(player, card).eot_toughness;
    }
  }
  else if (g_trigger_condition == TRIGGER_DEAL_DAMAGE &&
           card == g_affected_card &&
           player == g_affected_card_controller &&
           PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0 &&
           player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      do
      {
        --PLAYER_CARD_INSTANCE(player, card).eot_toughness;
        damage_player =
            PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).eot_toughness].player;
        damage_card =
            PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).eot_toughness].card;
        if (PLAYER_CARD_INSTANCE(damage_player, damage_card).damage_target_card == -1)
        {
          life_gain = ClampIntToRange(
              PLAYER_CARD_INSTANCE(damage_player, damage_card).info_slot,
              0,
              g_life[(int)(char)PLAYER_CARD_INSTANCE(damage_player, damage_card).damage_target_player]);
        }
        else
        {
          target_toughness = C_get_abilities(
              (int)(char)PLAYER_CARD_INSTANCE(damage_player, damage_card).damage_target_player,
              PLAYER_CARD_INSTANCE(damage_player, damage_card).damage_target_card,
              EVENT_TOUGHNESS,
              -1);
          life_gain = ClampIntToRange(PLAYER_CARD_INSTANCE(damage_player, damage_card).info_slot,
                                      0,
                                      target_toughness);
        }
        gain_life(player, life_gain);
      } while (PLAYER_CARD_INSTANCE(player, card).eot_toughness != 0);
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00498365
int TENTATIVE_sacrifice_leviathan_lands(int player, int card, int allow_cancel)
{
  struct
  {
    target_t selected_targets[2];
    char prompt[300];
    int cleanup_count;
    int selected_count;
    int land_type;
  } s;

  s.land_type = get_hacked_color(player, card, COLOR_BLUE) - 1;

  if (g_duel_ai_mode_state != 1)
  {
    load_text("prompts.txt", "LEVIATHAN");
    strcpy(s.prompt, g_text_lines[0]);
  }

  if (s.land_type != 1 && g_duel_ai_mode_state != 1)
  {
    replace_text_token(s.prompt, gs_land_word_lower_00709310[COLOR_BLUE], 0,
                       gs_land_word_upper_00709410[get_hacked_color(player, card, COLOR_BLUE)]);
  }

  s.selected_count = 0;
  while (s.selected_count < 2 && g_spell_fizzled != 1)
  {
    if (C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY,
                             TYPE_NONE, TYPE_NONE, 0, 0,
                             COLOR_TEST_0, COLOR_TEST_0, s.land_type, -1, -1, -1,
                             0, 0, 0, s.prompt, allow_cancel != 0,
                             &s.selected_targets[s.selected_count]))
    {
      PLAYER_CARD_INSTANCE(s.selected_targets[s.selected_count].player,
                           s.selected_targets[s.selected_count].card)
          .state |=
          STATE_CANNOT_TARGET | STATE_TARGETTED;
      TENTATIVE_reassess_all_cards(0, 0x20);
    }
    else
    {
      for (s.cleanup_count = 0; s.cleanup_count < s.selected_count; ++s.cleanup_count)
      {
        PLAYER_CARD_INSTANCE(s.selected_targets[s.cleanup_count].player,
                             s.selected_targets[s.cleanup_count].card)
            .state &=
            ~(STATE_CANNOT_TARGET | STATE_TARGETTED);
      }
      TENTATIVE_reassess_all_cards(0, 0x20);
      g_spell_fizzled = 1;
    }
    ++s.selected_count;
  }

  if (g_spell_fizzled == 1)
  {
    g_spell_fizzled = -1;
    return 0;
  }

  for (s.selected_count = 0; s.selected_count < 2; ++s.selected_count)
  {
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_SACRFICE);
    }
    kill_card(s.selected_targets[s.selected_count].player,
              s.selected_targets[s.selected_count].card,
              KILL_SACRIFICE);
  }

  return 1;
}

// FUNCTION: MAGIC 0x00549c89
// FUNCTION: SHANDALAR 0x00497ec5
int card_leviathan(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    return 0;
  }

  if (event == EVENT_UNTAP &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~3;
    return 0;
  }

  if (event == EVENT_SET_UNTAP_COST &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 &&
        g_current_player == player &&
        player == g_event_player)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x10;
    }
    return 0;
  }

  if (event == EVENT_CHECK_UNTAP_PAYMENT)
  {
    if (g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLUE)] < 2)
    {
      g_event_result |= 1;
    }
    return 0;
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    if (TENTATIVE_sacrifice_leviathan_lands(player, card, 1))
    {
      PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_TAPPED;
    }
    else
    {
      g_event_result |= 1;
    }
    return 0;
  }

  if (event == EVENT_ATTACK_LEGALITY)
  {
    if (PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0 &&
        g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLUE)] < 2)
    {
      g_event_result = 1;
    }
    return 0;
  }

  if (g_trigger_condition == TRIGGER_PAY_TO_ATTACK &&
      g_current_phase == PHASE_DECLARE_ATTACKERS &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      g_current_player == g_current_turn &&
      PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0 &&
      g_trigger_cause_controller == player &&
      g_trigger_cause == card)
  {
    if (g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLUE)] >= 2)
    {
      if (event == EVENT_TRIGGER)
      {
        g_event_result |= 2;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        if (!TENTATIVE_sacrifice_leviathan_lands(player, card, 0))
        {
          g_combat_assignment_cancelled = 1;
          g_spell_fizzled = 0;
        }
        else
        {
          PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
        }
      }
    }
    else
    {
      g_combat_assignment_cancelled = 1;
    }
    return 0;
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
    PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054a397
// FUNCTION: SHANDALAR 0x004985d4
int card_brothers_of_fire(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_RED, 2) &&
           has_mana(player, COLOR_ANY, 3);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    g_mana_charge[COLOR_COLORLESS] = 1;
    charge_mana(player, COLOR_RED, 2);
    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "BROTHERS_OF_FIRE");
      }
      select_damage_target(player, card, 1);
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (deal_damage_to_selected_target(player, card, event, 1) != 0)
    {
      damage_player(player, 1, g_card_on_stack_controller, g_card_on_stack);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054a549
// FUNCTION: SHANDALAR 0x00498786
int card_crimson_manticore(int player, int card, event_t event)
{
  int can_activate;
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    can_activate = 1;
    if ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) != 0)
    {
      can_activate = 0;
    }
    if (can_activate != 0 && has_mana(player, COLOR_RED, 1) == 0)
    {
      can_activate = 0;
    }
    if (can_activate != 0)
    {
      can_activate = real_target_available((int *)0,
                                           TARGET_SCAN_DIRECT,
                                           player,
                                           2,
                                           2,
                                           TARGET_ZONE_IN_PLAY,
                                           TYPE_CREATURE,
                                           TYPE_NONE,
                                           0,
                                           get_protections_from(player, card),
                                           COLOR_TEST_0,
                                           COLOR_TEST_0,
                                           -1,
                                           ~SUB_WALL,
                                           -1,
                                           -1,
                                           0,
                                           TARGET_STATE_IN_COMBAT,
                                           0);
    }
    return can_activate;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE &&
      (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
  {
    charge_mana(player, COLOR_RED, 1);
    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "CRIMSON_MANTICORE");
      }
      if (C_real_select_target(player,
                               2,
                               1 - player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               TARGET_STATE_IN_COMBAT,
                               0,
                               g_text_lines[0],
                               1,
                               &target) != 0)
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               TARGET_STATE_IN_COMBAT,
                               0) != 0)
    {
      damage_creature(target.player, target.card, 1, g_card_on_stack_controller, g_card_on_stack);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054a96d
// FUNCTION: SHANDALAR 0x00498baa
int card_prodigal_sorcerer(int player, int card, event_t event)
{
  unsigned int *state_ptr;
  unsigned int result;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = (unsigned int)((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    result = 0;
  }
  else if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PRODIGAL_SORCERER");
    }
    select_damage_target(player, card, 1);
    if (g_spell_fizzled != 1)
    {
      state_ptr = (unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state;
      *state_ptr |= 0x10;
    }
    result = 0;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    deal_damage_to_selected_target(player, card, event, 1);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    result = 0;
  }
  else if ((event == EVENT_CHECK_PUMP) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20014) == 0))
  {
    --g_global_toughness_bonus[1 - player];
    result = 0;
  }
  else
  {
    if ((event == EVENT_SHOULD_AI_PLAY) && (player == g_current_player) && (g_other_player == player) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0))
    {
      g_ai_modifier += 0x18;
    }

    if (((event == EVENT_ATTACK_RATING) && (g_affected_card == card)) && (g_affected_card_controller == player))
    {
      g_ai_score += 0x30;
      result = 0;
    }
    else if (((event == EVENT_BLOCK_RATING) && (g_affected_card == card)) && (g_affected_card_controller == player))
    {
      g_ai_score += -0x30;
      result = 0;
    }
    else
    {
      result = 0;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0054ac4d
// FUNCTION: SHANDALAR 0x00498e8a
int select_damage_target(int player, int card, int damage_unused)
{
  struct
  {
    char *prompt;
    int target_card;
    target_t target;
    int unused;
  } s;

  s.unused = 0;
  if (g_active_player == player || (g_duel_network_flags & 2) != 0)
  {
    if (g_duel_ai_mode_state != 1)
    {
      if (C_real_select_target(player,
                               2,
                               1 - player,
                               TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0,
                               g_text_lines[0],
                               1,
                               &s.target))
      {
        s.target_card = s.target.card;
        g_target_player_choice = s.target.player;
      }
      else
      {
        g_spell_fizzled = 1;
        s.target_card = -1;
        g_target_player_choice = -1;
      }
    }
    else
    {
      s.target_card = -1;
      g_target_player_choice = 1 - player;
    }
  }
  else
  {
    if (g_duel_ai_mode_state == 1)
    {
      if (internal_rand(3) != 0)
        g_ai_recorded_choice = 0;
      else
        g_ai_recorded_choice = 1;

      record_ai_action_selection();
    }
    else
    {
      replay_ai_action_selection();
    }

    if (g_ai_recorded_choice == 0)
    {
      if (g_duel_ai_mode_state == 1)
        s.prompt = "";
      else
        s.prompt = g_text_lines[0];

      C_real_select_target(player,
                           2,
                           1 - player,
                           TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
                           TYPE_CREATURE,
                           TYPE_NONE,
                           0,
                           get_protections_from(player, card),
                           COLOR_TEST_0,
                           COLOR_TEST_0,
                           -1,
                           ~SUB_WALL,
                           -1,
                           -1,
                           0,
                           0,
                           0,
                           s.prompt,
                           1,
                           &s.target);
      s.target_card = s.target.card;
      g_target_player_choice = s.target.player;
    }
    else
    {
      s.target_card = -1;
      g_target_player_choice = 1 - player;
      if (g_duel_ai_mode_state == 1)
      {
        g_ai_recorded_choice = 0;
        g_ai_recorded_action = ((g_target_player_choice == 0) ? 0 : 0x100) | 0xff;
        record_ai_action_selection();
      }
      else
      {
        replay_ai_action_selection();
      }
    }
  }

  if (g_spell_fizzled != 1)
  {
    PLAYER_CARD_INSTANCE(player, card).targets[0].card = s.target_card;
    PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_target_player_choice;
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    return 1;
  }
  else
    return 0;
}

// FUNCTION: MAGIC 0x0054af10
// FUNCTION: SHANDALAR 0x0049914d
int deal_damage_to_selected_target(int player, int card, event_t event, int amount)
{
  int source_player;
  int source_card;

  if (PLAYER_CARD_INSTANCE(player, card).number_of_targets == 0)
    return 0;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    source_player = g_card_on_stack_controller;
    source_card = g_card_on_stack;
  }
  else
  {
    source_player = player;
    source_card = card;
  }

  if (PLAYER_CARD_INSTANCE(player, card).targets[0].player == -1 && PLAYER_CARD_INSTANCE(player, card).targets[0].card == -1)
    return 0;

  if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                             PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                             NULL,
                             player,
                             2,
                             2,
                             TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE,
                             TYPE_NONE,
                             0,
                             get_protections_from(player, card),
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             -1,
                             ~SUB_WALL,
                             -1,
                             -1,
                             0,
                             0,
                             0))
  {
    if (PLAYER_CARD_INSTANCE(player, card).targets[0].card == -1)
    {
      damage_player(PLAYER_CARD_INSTANCE(player, card).targets[0].player, amount, source_player, source_card);
    }
    else
    {
      damage_creature(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                      PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                      amount,
                      source_player,
                      source_card);
    }

    return 1;
  }

  g_spell_fizzled = 1;
  return 0;
}

// FUNCTION: MAGIC 0x0054b1ad
// FUNCTION: SHANDALAR 0x004993ea
int card_pirate_ship(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOTAP | STATE_TAPPED)) == 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PIRATE_SHIP");
    }
    select_damage_target(player, card, 1);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    deal_damage_to_selected_target(player, card, event, 1);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  card_sea_serpent(player, card, event);

  return 0;
}

// FUNCTION: MAGIC 0x0054b360
// FUNCTION: SHANDALAR 0x0049959d
int card_sea_serpent(int player, int card, event_t event)
{
  if (is_in_play(player, card))
  {
    if (g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLUE)] == 0)
    {
      kill_card(player, card, KILL_DESTROY);
    }
  }

  if (event == EVENT_ATTACK_LEGALITY)
  {
    if (g_basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_BLUE)] == 0)
    {
      g_event_result = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b401
// FUNCTION: SHANDALAR 0x0049963e
int card_elder_land_wurm(int player, int card, event_t event)
{
  if (event == EVENT_DECLARE_BLOCKERS &&
      player != g_current_player &&
      (global_card_instances[player] + card)->blocking != -1)
  {
    (global_card_instances[player] + card)->info_slot = 1;
    return 0;
  }

  if (event == EVENT_ATTACK_LEGALITY)
  {
    if ((global_card_instances[player] + card)->info_slot == 0)
    {
      g_event_result = 1;
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b4ed
// FUNCTION: SHANDALAR 0x00499729
int card_island_fish_jasconius(int player, int card, event_t event)
{
  char prompt[600];

  if (event == EVENT_SET_UNTAP_COST &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 &&
      g_current_player == player &&
      player == g_event_player)
  {
    PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x10;
    PLAYER_CARD_INSTANCE(player, card).mana_to_untap[COLOR_BLUE] += 3;
  }

  card_sea_serpent(player, card, event);

  if (event == EVENT_UNTAP &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~3;
  }
  else if (event == EVENT_CAST_SPELL &&
           g_affected_card == card &&
           g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).mana_to_untap[COLOR_BLUE] += 3;
  }
  else if (g_trigger_condition == TRIGGER_DURING_UPKEEP &&
           g_current_player == player &&
           g_affected_card == card &&
           g_affected_card_controller == player &&
           player == g_current_turn &&
           (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0 &&
           has_mana(player, COLOR_BLUE, 3) != 0)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 1;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "ISLAND_FISH_JASCONIUS");
        sprintf(prompt, " %s\n %s", g_text_lines[0], g_text_lines[1]);
      }
      if (do_dialog(player, player, card, -1, -1, prompt, 0) == 0)
      {
        charge_mana(player, COLOR_BLUE, 3);
        if (g_spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_TAPPED;
        }
        else
        {
          g_spell_fizzled = -1;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054b8ad
// FUNCTION: SHANDALAR 0x00499ae9
int card_lord_of_atlantis(int player, int card, event_t event)
{
  if (is_in_play(player, card) && is_in_play(g_affected_card_controller, g_affected_card) && (char)global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype == 1)
  {
    if (event == EVENT_ABILITIES)
    {
      g_event_result |= 1 << (get_hacked_color(player, card, COLOR_BLUE) - 1);
    }
    else if (event == EVENT_POWER || event == EVENT_TOUGHNESS)
    {
      ++g_event_result;
    }
    else if (event == EVENT_GRAVEYARD_FROM_PLAY && (PLAYER_CARD_INSTANCE(player, card).token_status & 0x80) != 0)
    {
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).regen_status |= 0xe000000;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054ba0b
// FUNCTION: SHANDALAR 0x00499c47
int card_goblin_rock_sled(int player, int card, event_t event)
{
  if (event == EVENT_ATTACK_LEGALITY &&
      g_basiclandtypes_controlled[1 - player][get_hacked_color(player, card, COLOR_RED)] == 0)
  {
    g_event_result = 1;
  }

  if (event == EVENT_DECLARE_BLOCKERS &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0)
  {
    create_legacy_effect(player, card, g_duel_generated_internal_card_id_0b, player, card);
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054baf9
// FUNCTION: SHANDALAR 0x00499d35
int card_orcish_artillery(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOTAP | STATE_TAPPED)) == 0 ? 1 : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "ORCISH_ARTILLERY");
    }
    select_damage_target(player, card, 2);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (deal_damage_to_selected_target(player, card, event, 2) != 0)
    {
      damage_player(player, 3, g_card_on_stack_controller, g_card_on_stack);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;

    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054bcc8
// FUNCTION: SHANDALAR 0x00499f04
int card_psionic_entity(int player, int card, event_t event)
{
  if (event == EVENT_SHOULD_AI_PLAY &&
      is_in_play(player, card) &&
      PLAYER_CARD_INSTANCE(player, card).toughness > 3)
  {
    if (player == g_other_player)
    {
      g_ai_modifier += 200;
    }
    else
    {
      g_ai_modifier -= 200;
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOTAP | STATE_TAPPED)) == 0 ? 1 : 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "PSIONIC_ENTITY");
    }
    select_damage_target(player, card, 2);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (deal_damage_to_selected_target(player, card, event, 2) != 0 &&
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
                .internal_card_id != -1)
    {
      damage_creature(g_card_on_stack_controller, g_card_on_stack, 3,
                      g_card_on_stack_controller, g_card_on_stack);
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054bfaf
// FUNCTION: SHANDALAR 0x0049a1eb
int card_scavenging_ghoul(int player, int card, event_t event)
{
  struct
  {
    int activate_result;
    int can_activate_result;
  } s;

  if (event == EVENT_CAN_ACTIVATE && (g_land_can_be_played & 0x200) != 0)
  {
    s.can_activate_result = generic_regeneration_ability(player, card, event, 0, 0);
    if (C_get_special_counters(player, card) == 0)
    {
      s.can_activate_result = 0;
    }
    return s.can_activate_result;
  }

  if (event == EVENT_ACTIVATE && (g_land_can_be_played & 0x200) != 0)
  {
    s.activate_result = generic_regeneration_ability(player, card, event, 0, 0);
    remove_special_counters(player, card, 1);
    return s.activate_result;
  }

  if (event == EVENT_RESOLVE_ACTIVATION && (g_land_can_be_played & 0x200) != 0)
  {
    return generic_regeneration_ability(player, card, event, 0, 0);
  }

  if ((g_trigger_condition == 0xcd || event == 199) && g_affected_card == card && g_affected_card_controller == player && g_duel_summary.creatures_died != 0 && player == g_current_turn)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e || event == 199)
    {
      add_special_counters(player, card, g_duel_summary.creatures_died);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054c131
// FUNCTION: SHANDALAR 0x0049a36e
int card_sengir_vampire(int player, int card, event_t event)
{
  struct
  {
    int dead_internal_id; /* [ebp-0x10] */
    int count_triggered;  /* [ebp-0x0c] */
    int shift_index;      /* [ebp-0x08] */
    int tracked;          /* [ebp-0x04] */
  } s;

  if (event == 0x6e && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)
  {
    if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card != -1)
    {
      s.shift_index = PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player;
      if (PLAYER_CARD_INSTANCE(
              s.shift_index, PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card)
              .internal_card_id == -1)
      {
        s.dead_internal_id =
            PLAYER_CARD_INSTANCE(
                s.shift_index, PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card)
                .original_internal_card_id;
      }
      else
      {
        s.dead_internal_id =
            PLAYER_CARD_INSTANCE(
                s.shift_index, PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card)
                .internal_card_id;
      }

      if ((global_cards_data[s.dead_internal_id].type & TYPE_CREATURE) != 0 && PLAYER_CARD_INSTANCE(player, card).eot_toughness < 0x13)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).eot_toughness].card =
            PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card;
        PLAYER_CARD_INSTANCE(player, card).targets[PLAYER_CARD_INSTANCE(player, card).eot_toughness].player =
            s.shift_index;
        ++PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      }
    }
  }
  else if (event == 0x77)
  {
    s.count_triggered = 0;
    for (s.tracked = 0; s.tracked < PLAYER_CARD_INSTANCE(player, card).eot_toughness; ++s.tracked)
    {
      if (PLAYER_CARD_INSTANCE(player, card).targets[s.tracked].card == g_affected_card && PLAYER_CARD_INSTANCE(player, card).targets[s.tracked].player == g_affected_card_controller && PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code != 4 && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).token_status & 0x4000000) == 0)
      {
        s.shift_index = s.tracked;
        if (!s.count_triggered)
        {
          ++PLAYER_CARD_INSTANCE(player, card).info_slot;
          s.count_triggered = 1;
        }

        while (++s.shift_index < PLAYER_CARD_INSTANCE(player, card).eot_toughness)
        {
          PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index - 1].player =
              PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index].player;
          PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index - 1].card =
              PLAYER_CARD_INSTANCE(player, card).targets[s.shift_index].card;
        }
        --PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      }
    }
  }
  else if (g_trigger_condition == 0xd5 && PLAYER_CARD_INSTANCE(player, card).info_slot != 0 && g_current_turn == player && g_affected_card == card && g_affected_card_controller == player)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e)
    {
      add_special_counters(player, card, PLAYER_CARD_INSTANCE(player, card).info_slot);
      PLAYER_CARD_INSTANCE(player, card).counter_power += (short)PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, card).counter_toughness +=
          (short)PLAYER_CARD_INSTANCE(player, card).info_slot;
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
    }
  }
  else if (event == 0x22 || event == 199)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cb2b
// FUNCTION: SHANDALAR 0x0049ad68
int card_junun_efreet(int player, int card, event_t event)
{
  if (event == EVENT_SETUP_UPKEEP_COSTS &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (g_current_player == player && player == g_event_player)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
      PLAYER_CARD_INSTANCE(player, card).upkeep_black += 2;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY && g_basiclandtypes_controlled[player][COLOR_BLACK] < 2)
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cc76
// FUNCTION: SHANDALAR 0x0049aeb3
int card_phantasmal_forces(int player, int card, event_t event)
{
  if (((event == EVENT_SETUP_UPKEEP_COSTS) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    if ((g_current_player == player) && (player == g_event_player))
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
      PLAYER_CARD_INSTANCE(player, card).upkeep_blue += 1;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    return 0;
  }

  if ((event == EVENT_SHOULD_AI_PLAY) && (g_basiclandtypes_controlled[player][COLOR_BLUE] < 1))
  {
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cdc0
// FUNCTION: SHANDALAR 0x0049affe
int card_force_of_nature(int player, int card, event_t event)
{
  if (event == EVENT_SETUP_UPKEEP_COSTS && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_current_player == player)
    {
      if (g_event_player == player)
      {
        PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
        PLAYER_CARD_INSTANCE(player, card).upkeep_green =
            PLAYER_CARD_INSTANCE(player, card).upkeep_green + '\x04';
      }
    }

    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    load_text("prompts.txt", "FORCE_OF_NATURE");
    do_dialog(player, player, card, -1, -1, g_text_lines[0], 0);
    damage_player(player, 8, g_card_on_stack_controller, g_card_on_stack);
    return 0;
  }

  if ((event == EVENT_SHOULD_AI_PLAY) && (g_basiclandtypes_controlled[player][COLOR_GREEN] < 4))
  {
    damage_player(player, 8, player, card);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054cf44
// FUNCTION: SHANDALAR 0x0049b183
int card_llanowar_elves(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (unsigned int)((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0);
  }

  if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0))
  {
    undeclare_mana_available_and_produce_it(player, COLOR_GREEN, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
    g_produced_mana_color = COLOR_GREEN;
    return 0;
  }

  if ((event == EVENT_COUNT_MANA) && (g_affected_card == card) && (g_affected_card_controller == player))
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      declare_mana_available(player, COLOR_GREEN, 1);
    }
    return 0;
  }

  if ((event == EVENT_ATTACK_RATING) && (g_affected_card == card) && (g_affected_card_controller == player))
  {
    g_ai_score += 0x18 / (g_basiclandtypes_controlled[player][COLOR_GREEN] + 2);
    return 0;
  }

  if ((event == EVENT_BLOCK_RATING) && (g_affected_card == card) && (g_affected_card_controller == player))
  {
    g_ai_score -= 0x60 / (g_basiclandtypes_controlled[player][COLOR_GREEN] + 2);
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054d153
// FUNCTION: SHANDALAR 0x0049b392
int card_birds_of_paradise(int player, int card, event_t event)
{
  struct
  {
    char message[300];
    color_t choice_hint;
    int current_color;
    unsigned int available_colors;
  } s;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0;
  }

  if ((event == EVENT_ACTIVATE) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0))
  {
    s.available_colors = PLAYER_CARD_INSTANCE(player, card).mana_color;
    if ((((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || (g_duel_ai_mode_state == 1)) || (g_duel_network_state != 0))
    {
      s.choice_hint = -1;
      for (s.current_color = 1; (s.current_color <= 5) && (s.choice_hint == -1); ++s.current_color)
      {
        if (((g_required_mana_color_mask & (1 << (((unsigned char)s.current_color)))) != 0) && ((s.available_colors & (1 << (((unsigned char)s.current_color)))) != 0))
        {
          s.choice_hint = s.current_color;
        }
      }
      if ((s.choice_hint == -1) && ((g_required_mana_color_mask & 1) != 0))
      {
        s.choice_hint = COLOR_BLACK;
      }
      if ((s.choice_hint == -1) && ((g_required_mana_color_mask & 0x40) != 0))
      {
        s.choice_hint = COLOR_BLACK;
      }
      if (s.choice_hint == -1)
      {
        g_spell_fizzled = 1;
      }
    }
    else
    {
      s.choice_hint = -1;
    }
    if (g_spell_fizzled != 1)
    {
      if (g_duel_ai_mode_state != 1)
      {
        load_text("prompts.txt", "BIRDS_OF_PARADISE");
      }
      s.choice_hint = choose_a_color(player, g_text_lines[0], 1, s.choice_hint, PLAYER_CARD_INSTANCE(player, card).mana_color);
      if (s.choice_hint == ~COLOR_COLORLESS)
      {
        g_spell_fizzled = 1;
      }
      else
      {
        produce_mana(player, s.choice_hint, 1);
        undeclare_mana_available_hex(player, PLAYER_CARD_INSTANCE(player, card).mana_color, 1);
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        g_produced_mana_color = s.choice_hint;
        if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_ai_mode_state != 1))
        {
          load_text("prompts.txt", "BIRDS_OF_PARADISE");
          if (s.choice_hint == COLOR_BLACK)
          {
            strcpy(s.message, g_text_lines[1]);
          }
          else if (s.choice_hint == COLOR_BLUE)
          {
            strcpy(s.message, g_text_lines[2]);
          }
          else if (s.choice_hint == COLOR_GREEN)
          {
            strcpy(s.message, g_text_lines[3]);
          }
          else if (s.choice_hint == COLOR_RED)
          {
            strcpy(s.message, g_text_lines[4]);
          }
          else
          {
            strcpy(s.message, g_text_lines[5]);
          }
          do_dialog(player, player, card, -1, -1, s.message, 0);
        }
      }
    }
    return 0;
  }

  if ((event == EVENT_COUNT_MANA) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      declare_mana_available_hex(player, PLAYER_CARD_INSTANCE(player, card).mana_color, 1);
    }
    return 0;
  }

  if ((event == 0x8a) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    g_ai_score += 0x60 / (g_basiclandtypes_controlled[player][7] + 2);
    return 0;
  }

  if ((event == 0x8b) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    g_ai_score -= 0x60 / (g_basiclandtypes_controlled[player][7] + 2);
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054d69d
// FUNCTION: SHANDALAR 0x0049b8dc
int card_cosmic_horror(int player, int card, event_t event)
{
  if (event == EVENT_SETUP_UPKEEP_COSTS &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (g_current_player == player && player == g_event_player)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
      PLAYER_CARD_INSTANCE(player, card).upkeep_black += 3;
      PLAYER_CARD_INSTANCE(player, card).upkeep_colorless += 3;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "COSMIC_HORROR");
    }
    do_dialog(player, player, card, -1, -1, g_text_lines[0], 0);
    damage_player(player, 7, g_card_on_stack_controller, g_card_on_stack);
    kill_card(g_card_on_stack_controller, g_card_on_stack, KILL_BURY);
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY &&
      (g_basiclandtypes_controlled[player][COLOR_BLACK] < 3 ||
       g_basiclandtypes_controlled[player][COLOR_ANY] < 6))
  {
    damage_player(player, 7, player, card);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054d8ca
// FUNCTION: SHANDALAR 0x0049bb0a
int card_lord_of_the_pit(int player, int card, event_t event)
{
  int creature_to_sacrifice;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    if (g_duel_summary.creature_counts[player] < 2)
    {
      g_ai_modifier -= 0xa8;
    }
    return 0;
  }

  if (event == EVENT_CHECK_UPK_PAYMENT)
  {
    if (has_other_creature_in_play(player, card) == 0)
    {
      g_event_result |= 1;
    }
    return 0;
  }

  if (event == EVENT_SETUP_UPKEEP_COSTS && g_affected_card == card && g_affected_card_controller == player)
  {
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    if (has_other_creature_in_play(player, card) == 0)
    {
      ++g_ai_speculation_depth;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    if (has_other_creature_in_play(player, card) == 0)
    {
      g_event_result |= 1;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).state |= 0x100000;
      TENTATIVE_reassess_all_cards(0, 0x20);
      load_text("prompts.txt", "LORD_OF_THE_PIT");

      creature_to_sacrifice = choose_creature_to_sacrifice(player);

      PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_CANNOT_TARGET;
      if (creature_to_sacrifice != -1)
      {
        kill_card(player, creature_to_sacrifice, KILL_SACRIFICE);
      }
      else
      {
        g_event_result |= 1;
      }
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player(player, 7, g_card_on_stack_controller, g_card_on_stack);
    return 0;
  }

  if (event == EVENT_CLEANUP && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  if (event == EVENT_ATTACK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_score -= 0x30;
  }
  else if (event == EVENT_BLOCK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_score += 0x30;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054dccd
// FUNCTION: SHANDALAR 0x0049bf0e
int has_other_creature_in_play(int player, int card)
{
  int current_card;

  for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
  {
    if (current_card != card && is_in_play(player, current_card) && (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_CREATURE) != 0)
    {
      return 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054dd89
// FUNCTION: SHANDALAR 0x0049bfca
int card_ball_lightning(int player, int card, event_t event)
{
  if (g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).state &= ~(STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP);
  }

  if (event == 199)
  {
    kill_card(player, card, KILL_BURY);
  }

  if (g_trigger_condition == TRIGGER_EOT &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      kill_card(player, card, KILL_BURY);
    }
  }

  if (event == EVENT_ATTACK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_score -= 0x3c;
  }
  else if (event == EVENT_BLOCK_RATING && g_affected_card == card && g_affected_card_controller == player)
  {
    g_ai_score += 0x3c;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054dee6
// FUNCTION: SHANDALAR 0x0049c127
int card_cyclopean_mummy(int player, int card, event_t event)
{
  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).kill_code != KILL_REMOVE)
  {
    PLAYER_CARD_INSTANCE(player, card).kill_code = KILL_REMOVE;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054df54
// FUNCTION: SHANDALAR 0x0049c195
int card_nether_shadow(int player, int card, event_t event)
{
  int hand_card;
  int hand_player;

  if (card == g_affected_card)
  {
    if (player == g_affected_card_controller)
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_SUMMONSICK_BOTH;
    }
  }

  if (event == 0x8d ||
      (event == 0x77 &&
       card == g_affected_card &&
       player == g_affected_card_controller &&
       (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0 &&
       PLAYER_CARD_INSTANCE(player, card).kill_code != 4))
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x1000) != 0)
    {
      hand_player = g_other_player;
    }
    else
    {
      hand_player = g_active_player;
    }

    hand_card = add_card_to_hand(hand_player, g_duel_generated_internal_card_id_11);
    if (hand_card != -1)
    {
      *(unsigned int *)&PLAYER_CARD_INSTANCE(hand_player, hand_card).state |= 2;
      PLAYER_CARD_INSTANCE(hand_player, hand_card).display_pic_info =
          global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id;
    }

    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e160
// FUNCTION: SHANDALAR 0x0049c3a1
int card_nether_link(int player, int card, event_t event)
{
  int nether_shadow_iid;
  int creature_count;
  int graveyard_index;
  int hand_card;

  if ((g_trigger_condition == TRIGGER_END_UPKEEP || event == EVENT_SHOULD_AI_PLAY) &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      player == g_current_player &&
      player == g_current_turn)
  {
    nether_shadow_iid = find_internal_card_id_by_csv_id(CARD_ID_NETHER_SHADOW);
    creature_count = 0;
    for (graveyard_index = 499; graveyard_index >= 0; --graveyard_index)
    {
      if (global_graveyard_slots[player][graveyard_index] != -1 &&
          (global_cards_data[global_graveyard_slots[player][graveyard_index]].type & TYPE_CREATURE) != 0)
      {
        ++creature_count;
        if (global_graveyard_slots[player][graveyard_index] == nether_shadow_iid &&
            creature_count > 3)
        {
          if (event == EVENT_TRIGGER)
          {
            g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
          }
          if (event != EVENT_RESOLVE_TRIGGER && event != EVENT_SHOULD_AI_PLAY)
          {
            return 0;
          }

          hand_card = add_card_to_hand(player, nether_shadow_iid);
          if (hand_card == -1)
          {
            return 0;
          }
          process_card_enters_play(player, hand_card);
          PLAYER_CARD_INSTANCE(player, hand_card).state &= ~0x30000;
          remove_card_from_graveyard(player, graveyard_index);
          kill_card(player, card, KILL_REMOVE);
          TENTATIVE_reassess_all_cards(0, 0x30);
          load_text("prompts.txt", "NETHER_SHADOW");
          do_dialog(player, player, hand_card, -1, -1, g_text_lines[0], 0);
          return 0;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e36e
// FUNCTION: SHANDALAR 0x0049c5b0
int card_white_knight(int player, int card, event_t event)
{
  int saved_event_result;

  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_PROT_BLACK << ((get_sleighted_color(player, card, COLOR_BLACK) - 1U));
    saved_event_result = g_event_result;
    destroy_attached_auras_of_color(player, card, COLOR_BLACK);
    g_event_result = saved_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e3ef
// FUNCTION: SHANDALAR 0x0049c630
int card_black_knight(int player, int card, event_t event)
{
  int saved_event_result;

  if ((event == 0x34) && (card == g_affected_card) && (player == g_affected_card_controller))
  {
    g_event_result |= 0x800 << (get_sleighted_color(player, card, 5) - 1U);
    saved_event_result = g_event_result;
    destroy_attached_auras_of_color(player, card, 5);
    g_event_result = saved_event_result;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e470
// FUNCTION: SHANDALAR 0x0049c6b1
void destroy_attached_auras_of_color(int player, int card, int color)
{
  int test_player;
  int test_card;

  for (test_player = 0; test_player < 2; ++test_player)
  {
    for (test_card = 0; test_card < g_active_cards_count[test_player]; ++test_card)
    {
      if (is_in_play(test_player, test_card))
      {
        if ((int)(char)PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_player == player &&
            PLAYER_CARD_INSTANCE(test_player, test_card).damage_target_card == card)
        {
          if (((1 << (unsigned char)get_sleighted_color(player, card, color)) &
               (char)PLAYER_CARD_INSTANCE(test_player, test_card).color) != 0 &&
              (((unsigned char)global_cards_data[PLAYER_CARD_INSTANCE(test_player, test_card).internal_card_id].type & 4) != 0))
          {
            kill_card(test_player, test_card, 1);
          }
        }
      }
    }
  }
}

// FUNCTION: MAGIC 0x0054e5f3
// FUNCTION: SHANDALAR 0x0049c834
int card_ali_baba(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_RED, 1) &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 -1,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_WALL,
                                 0,
                                 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (has_mana(player, COLOR_RED, 1))
    {
      charge_mana(player, COLOR_RED, 1);
      if (g_spell_fizzled != 1)
      {
        if (g_duel_ai_mode_state != 1)
        {
          load_text("prompts.txt", "ALI_BABA");
        }
        if (C_real_select_target(player,
                                 2,
                                 1 - player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 -1,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_WALL,
                                 0,
                                 0,
                                 g_text_lines[0],
                                 1,
                                 &target))
        {
          SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        }
        else
        {
          g_spell_fizzled = 1;
        }
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               -1,
                               -1,
                               -1,
                               TARGET_SPECIAL_WALL,
                               0,
                               0))
    {
      PLAYER_CARD_INSTANCE(target.player, target.card).state |= STATE_TAPPED;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054e986
// FUNCTION: SHANDALAR 0x0049cbc7
int card_ley_druid(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 && real_target_available((int *)0,
                                                                                              TARGET_SCAN_DIRECT,
                                                                                              player,
                                                                                              2,
                                                                                              2,
                                                                                              0x200,
                                                                                              TYPE_LAND,
                                                                                              0,
                                                                                              0,
                                                                                              get_protections_from(player, card),
                                                                                              0,
                                                                                              0,
                                                                                              -1,
                                                                                              -1,
                                                                                              -1,
                                                                                              -1,
                                                                                              0,
                                                                                              0,
                                                                                              0) != 0;
  }

  if (event == 0x90)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "LEY_DRUID");
    }

    if (C_real_select_target(player,
                             2,
                             player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_LAND,
                             TYPE_NONE,
                             0,
                             get_protections_from(player, card),
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             -1,
                             -1,
                             -1,
                             -1,
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &target))
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_LAND,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               -1,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      PLAYER_CARD_INSTANCE(target.player, target.card).state &= ~STATE_TAPPED;
      dispatch_event_to_single_card(target.player, target.card, EVENT_UNTAP_PHASE, -1, -1);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054ed44
// FUNCTION: SHANDALAR 0x0049cf85
int card_venom(int player, int card, event_t event)
{
  int result;
  int enchanted_player;
  int enchanted_card;
  int opponent;
  int current_card;
  int blocking_card;
  int blocking_iid;
  char grouped_blocker;

  if (event == EVENT_CHANGE_TYPE &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_card != -1 &&
      PLAYER_CARD_INSTANCE(player, card).damage_target_player != -1)
  {
    PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_target_player,
                         PLAYER_CARD_INSTANCE(player, card).damage_target_card)
        .destroys_if_blocked |= 0x3f;
  }

  result = generic_creature_ability_aura(player, card, event, 0);

  if (event == EVENT_DECLARE_BLOCKERS)
  {
    enchanted_player = (int)PLAYER_CARD_INSTANCE(player, card).damage_target_player;
    enchanted_card = PLAYER_CARD_INSTANCE(player, card).damage_target_card;

    if (enchanted_player != -1 &&
        enchanted_card != -1 &&
        is_in_play(enchanted_player, enchanted_card) != 0)
    {
      opponent = 1 - enchanted_player;
      if (enchanted_player == g_current_player &&
          (PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).state & (STATE_ATTACKING | STATE_ATTACKED)) != 0)
      {
        if (PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking == -1)
        {
          blocking_card = enchanted_card;
        }
        else
        {
          blocking_card = (int)(char)PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking;
        }

        for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
        {
          blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
          if (blocking_iid != -1 &&
              PLAYER_CARD_INSTANCE(opponent, current_card).blocking == blocking_card &&
              (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0 &&
              global_cards_data[blocking_iid].subtype != SUB_WALL)
          {
            create_legacy_effect(player, card, g_duel_generated_internal_card_id_06, opponent, current_card);
          }
        }
      }

      if (enchanted_player != g_current_player &&
          PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking != -1)
      {
        grouped_blocker = (char)PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking).blocking;
        if (grouped_blocker == -1)
        {
          blocking_iid = PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking).internal_card_id;
          if (blocking_iid != -1 &&
              (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0 &&
              global_cards_data[blocking_iid].subtype != SUB_WALL)
          {
            create_legacy_effect(player, card, g_duel_generated_internal_card_id_06, opponent, (int)(char)PLAYER_CARD_INSTANCE(enchanted_player, enchanted_card).blocking);
          }
        }
        else
        {
          for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
          {
            blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
            if (is_in_play(opponent, current_card) != 0 &&
                blocking_iid != -1 &&
                PLAYER_CARD_INSTANCE(opponent, current_card).blocking == grouped_blocker &&
                (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0 &&
                global_cards_data[blocking_iid].subtype != SUB_WALL)
            {
              create_legacy_effect(player, card, g_duel_generated_internal_card_id_06, opponent, current_card);
            }
          }
        }
      }
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x0054f7fe
// FUNCTION: SHANDALAR 0x0049da3f
int card_thicket_basilisk(int player, int card, event_t event)
{
  int opponent;
  int current_card;
  int blocking_card;
  int blocking_iid;
  char grouped_blocker;

  if (event == 0x3c)
  {
    PLAYER_CARD_INSTANCE(player, card).destroys_if_blocked = PLAYER_CARD_INSTANCE(player, card).destroys_if_blocked | 0x3f;
  }
  else if (event == 0x1a)
  {
    opponent = 1 - player;
    if ((player == g_current_player) && ((PLAYER_CARD_INSTANCE(player, card).state & 0x44) != 0))
    {
      if (PLAYER_CARD_INSTANCE(player, card).blocking == -1)
      {
        blocking_card = card;
      }
      else
      {
        blocking_card = (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking;
      }

      for (current_card = 0; current_card < g_active_cards_count[opponent]; current_card = current_card + 1)
      {
        blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
        if (((PLAYER_CARD_INSTANCE(opponent, current_card).blocking == blocking_card) && (blocking_iid != -1)) && ((global_cards_data[blocking_iid].subtype != 0) && ((global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)))
        {
          create_legacy_effect(player, card, g_duel_generated_internal_card_id_06, opponent, current_card);
        }
      }
    }

    if ((player != g_current_player) && (PLAYER_CARD_INSTANCE(player, card).blocking != -1))
    {
      grouped_blocker = (char)PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).blocking;
      if (grouped_blocker == -1)
      {
        blocking_iid = PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).internal_card_id;
        if ((blocking_iid != -1) && ((global_cards_data[blocking_iid].subtype != 0) && ((global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)))
        {
          create_legacy_effect(player, card, g_duel_generated_internal_card_id_06, opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking);
        }
      }
      else
      {
        for (current_card = 0; current_card < g_active_cards_count[opponent]; current_card = current_card + 1)
        {
          blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
          if ((is_in_play(opponent, current_card) != 0) && (PLAYER_CARD_INSTANCE(opponent, current_card).blocking == grouped_blocker) && ((global_cards_data[blocking_iid].subtype != 0) && ((global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0)))
          {
            create_legacy_effect(player, card, g_duel_generated_internal_card_id_06, opponent, current_card);
          }
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054fcbc
// FUNCTION: SHANDALAR 0x0049deff
int card_wall_of_dust(int player, int card, event_t event)
{
  int opponent;
  int current_card;
  char grouped_blocker;

  if (event == EVENT_DECLARE_BLOCKERS)
  {
    opponent = 1 - player;
    if (g_current_player != player &&
        PLAYER_CARD_INSTANCE(player, card).blocking != -1 &&
        PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
      grouped_blocker =
          (char)PLAYER_CARD_INSTANCE(opponent,
                                     (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking)
              .blocking;
      if (grouped_blocker == -1)
      {
        create_legacy_effect(player,
                             card,
                             g_duel_generated_internal_card_id_08,
                             opponent,
                             (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking);
        PLAYER_CARD_INSTANCE(opponent,
                             (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking)
            .token_status |= 0x8000;
      }
      else
      {
        for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
        {
          if (is_in_play(opponent, current_card) != 0 &&
              PLAYER_CARD_INSTANCE(opponent, current_card).blocking == grouped_blocker)
          {
            create_legacy_effect(player, card, g_duel_generated_internal_card_id_08, opponent, current_card);
            PLAYER_CARD_INSTANCE(opponent, current_card).token_status |= 0x8000;
          }
        }
      }
    }
  }
  else if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0054ffb0
// FUNCTION: SHANDALAR 0x0049e1f3
int card_sisters_of_the_flame(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0;
  }

  if (event == EVENT_ACTIVATE && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    undeclare_mana_available_and_produce_it(player, COLOR_RED, 1);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    g_produced_mana_color = COLOR_RED;
    return 0;
  }

  if (event == EVENT_COUNT_MANA && g_affected_card == card && g_affected_card_controller == player)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
    {
      declare_mana_available(player, COLOR_RED, 1);
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0055011b
// FUNCTION: SHANDALAR 0x0049e35e
int card_apprentice_wizard(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
           has_mana(player, COLOR_BLUE, 1);
  }

  if (event == EVENT_ACTIVATE && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && has_mana(player, COLOR_BLUE, 1))
  {
    charge_mana(player, COLOR_BLUE, 1);
    if (g_spell_fizzled != 1)
    {
      undeclare_mana_available_and_produce_it(player, COLOR_COLORLESS, 3);
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      g_produced_mana_color = COLOR_COLORLESS;
    }
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0055025c
// FUNCTION: SHANDALAR 0x0049e49f
int card_abomination(int player, int card, event_t event)
{
  int opponent;
  int current_card;
  int blocking_card;
  int blocking_iid;
  char grouped_blocker;
  color_test_t affected_colors;

  opponent = 1 - player;

  if (event == EVENT_CHANGE_TYPE)
  {
    PLAYER_CARD_INSTANCE(player, card).destroys_if_blocked |=
        (char)((1 << (unsigned char)get_sleighted_color(player, card, COLOR_GREEN)) |
               (1 << (unsigned char)get_sleighted_color(player, card, COLOR_WHITE)) |
               0x80);
  }
  else if (event == EVENT_DECLARE_BLOCKERS)
  {
    affected_colors = (1 << (unsigned char)get_sleighted_color(player, card, COLOR_GREEN)) |
                      (1 << (unsigned char)get_sleighted_color(player, card, COLOR_WHITE));

    if (player == g_current_player &&
        (PLAYER_CARD_INSTANCE(player, card).state & (STATE_ATTACKING | STATE_ATTACKED)) != 0)
    {
      if (PLAYER_CARD_INSTANCE(player, card).blocking == -1)
      {
        blocking_card = card;
      }
      else
      {
        blocking_card = (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking;
      }

      for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
      {
        blocking_iid = PLAYER_CARD_INSTANCE(opponent, current_card).internal_card_id;
        if (blocking_iid != -1 &&
            PLAYER_CARD_INSTANCE(opponent, current_card).blocking == blocking_card &&
            (global_cards_data[blocking_iid].type & TYPE_CREATURE) != 0 &&
            (PLAYER_CARD_INSTANCE(opponent, current_card).color & affected_colors) != 0)
        {
          create_legacy_effect(player, card, g_duel_generated_internal_card_id_06, opponent, current_card);
        }
      }
    }

    if ((player != g_current_player) && (PLAYER_CARD_INSTANCE(player, card).blocking != -1))
    {
      grouped_blocker = (char)PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).blocking;
      if (grouped_blocker == -1)
      {
        if ((PLAYER_CARD_INSTANCE(opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking).color &
             affected_colors) != 0)
        {
          create_legacy_effect(player, card, g_duel_generated_internal_card_id_06, opponent, (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking);
        }
      }
      else
      {
        for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
        {
          if (is_in_play(opponent, current_card) != 0 &&
              PLAYER_CARD_INSTANCE(opponent, current_card).blocking == grouped_blocker &&
              (PLAYER_CARD_INSTANCE(opponent, current_card).color & affected_colors) != 0)
          {
            create_legacy_effect(player, card, g_duel_generated_internal_card_id_06, opponent, current_card);
          }
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00550761
// FUNCTION: SHANDALAR 0x0049e9a4
int card_radjan_spirit(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "RADJAN_SPIRIT");
    }

    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE,
                             TYPE_NONE,
                             0,
                             get_protections_from(player, card),
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             -1,
                             ~SUB_WALL,
                             -1,
                             -1,
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      if (g_other_player == player &&
          (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card).regen_status &
           KEYWORD_FLYING) != 0)
      {
        g_ai_modifier -= 0x48;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         g_duel_generated_internal_card_id_07,
                                         target.player,
                                         target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = KEYWORD_FLYING;
      }
      PLAYER_CARD_INSTANCE(target.player, target.card).regen_status = KEYWORD_RECALC_ABILITIES;
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
    return 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00550c4b
// FUNCTION: SHANDALAR 0x0049ee8e
int card_hurr_jackal(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOTAP | STATE_TAPPED)) == 0 &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(player, card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 0,
                                 0,
                                 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE && (PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOTAP | STATE_TAPPED)) == 0)
  {
    if (g_duel_ai_mode_state != 1)
    {
      load_text("prompts.txt", "HURR_JACKAL");
    }

    if (C_real_select_target(player,
                             2,
                             1 - player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE,
                             TYPE_NONE,
                             0,
                             get_protections_from(player, card),
                             COLOR_TEST_0,
                             COLOR_TEST_0,
                             -1,
                             ~SUB_WALL,
                             -1,
                             -1,
                             0,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &target))
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if ((PLAYER_CARD_INSTANCE(target.player, target.card).regen_status & 0x200) == 0)
      {
        g_ai_modifier -= 0x30;
      }
      else
      {
        g_ai_modifier += 0x18;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player,
                               target.card,
                               (char *)0,
                               player,
                               2,
                               2,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_CREATURE,
                               TYPE_NONE,
                               0,
                               get_protections_from(player, card),
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               ~SUB_WALL,
                               -1,
                               -1,
                               0,
                               0,
                               0))
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                         g_card_on_stack,
                                         g_duel_generated_internal_card_id_0f,
                                         target.player,
                                         target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).token_status |= 0x800000;
        PLAYER_CARD_INSTANCE(target.player, target.card).regen_status = 0x8000000;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
    return 0;
  }

  return 0;
}
