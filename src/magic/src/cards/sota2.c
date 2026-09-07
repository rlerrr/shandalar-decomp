#include <stdio.h>
#include <string.h>
#include "../game_support.h"
#include "../global_duel_ui_ids.h"
#include "../global_strings.h"

int gain_control(int player, int card);
int coin_flip(int player, char *dialog_title, int show_dialog_if_animation_is_off);
int card_argothian_treefolk(int player, int card, event_t event);
int card_sea_serpent(int player, int card, event_t event);

static int veteran_bodyguard_prevent_damage_callback(int source_player,
                                                     int source_card,
                                                     int player,
                                                     int card,
                                                     int internal_card_id);
static int veteran_bodyguard_mark_other_protectors(int unused,
                                                   int source_card,
                                                   int player,
                                                   int card,
                                                   int internal_card_id);
static int veteran_bodyguard_or_martyrs_present(int unused1,
                                                int unused2,
                                                int unused3,
                                                int unused4,
                                                int internal_card_id);
static __inline int select_own_artifact_for_cost(int player, int card, target_t *target, char *prompt)
{
  return C_real_select_target(player,
                              player,
                              player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_ARTIFACT,
                              TYPE_NONE,
                              0,
                              0,
                              COLOR_TEST_0,
                              COLOR_TEST_0,
                              -1,
                              -1,
                              -1,
                              -1,
                              0,
                              0,
                              0,
                              prompt,
                              1,
                              target);
}

static __inline int own_artifact_available(int player)
{
  return real_target_available((int *)0,
                               TARGET_SCAN_DIRECT,
                               player,
                               player,
                               player,
                               TARGET_ZONE_IN_PLAY,
                               TYPE_ARTIFACT,
                               TYPE_NONE,
                               0,
                               0,
                               COLOR_TEST_0,
                               COLOR_TEST_0,
                               -1,
                               -1,
                               -1,
                               -1,
                               0,
                               0,
                               0);
}

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

static __inline int create_activated_control_effect(int source_player, int source_card, target_t target)
{
  int legacy_card;
  int new_card;

  legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                     g_card_on_stack,
                                     g_control_aura_special_internal_card_id,
                                     target.player,
                                     target.card);
  if (legacy_card != -1)
  {
    PLAYER_CARD_INSTANCE(g_card_on_stack_controller, legacy_card).damage_source_player = (char)-1;
    PLAYER_CARD_INSTANCE(g_card_on_stack_controller, legacy_card).damage_source_card = -1;
    PLAYER_CARD_INSTANCE(g_card_on_stack_controller, legacy_card).token_status |= STATUS_PERMANENT;
    PLAYER_CARD_INSTANCE(g_card_on_stack_controller, legacy_card).targets[0].player = g_card_on_stack_controller;
    PLAYER_CARD_INSTANCE(g_card_on_stack_controller, legacy_card).targets[0].card = g_card_on_stack;
    PLAYER_CARD_INSTANCE(g_card_on_stack_controller, legacy_card).number_of_targets = 1;
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(source_player, source_card).parent_controller,
                             PLAYER_CARD_INSTANCE(source_player, source_card).parent_card)
            .info_slot > 1)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(source_player, source_card).parent_controller,
                           PLAYER_CARD_INSTANCE(source_player, source_card).parent_card)
          .targets[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(source_player, source_card).parent_controller,
                                        PLAYER_CARD_INSTANCE(source_player, source_card).parent_card)
                       .info_slot]
          .player = source_player;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(source_player, source_card).parent_controller,
                           PLAYER_CARD_INSTANCE(source_player, source_card).parent_card)
          .targets[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(source_player, source_card).parent_controller,
                                        PLAYER_CARD_INSTANCE(source_player, source_card).parent_card)
                       .info_slot]
          .card = legacy_card;
      --PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(source_player, source_card).parent_controller,
                             PLAYER_CARD_INSTANCE(source_player, source_card).parent_card)
            .info_slot;
    }
    if (resolve_control_aura_conflict(source_player, legacy_card))
    {
      PLAYER_CARD_INSTANCE(g_card_on_stack_controller, legacy_card).token_status |= STATUS_CONTROLLED;
      if (target.player != source_player)
      {
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(WAV_CONTROL);
        }
        new_card = gain_control(
            (int)PLAYER_CARD_INSTANCE(g_card_on_stack_controller, legacy_card).damage_target_player,
            PLAYER_CARD_INSTANCE(g_card_on_stack_controller, legacy_card).damage_target_card);
        PLAYER_CARD_INSTANCE(g_card_on_stack_controller, legacy_card).damage_target_player = (char)source_player;
        PLAYER_CARD_INSTANCE(g_card_on_stack_controller, legacy_card).damage_target_card = new_card;
      }
    }
  }

  return legacy_card;
}

// FUNCTION: SHANDALAR 0x004873a6
static int remove_control_effect_from_source(int source_player, int source_card, int effect_player, int effect_card)
{
  int found;
  int current_target;

  found = 0;
  current_target = PLAYER_CARD_INSTANCE(source_player, source_card).info_slot;
  while (++current_target < 19 && found == 0)
  {
    if (PLAYER_CARD_INSTANCE(source_player, source_card).targets[current_target].player == effect_player &&
        PLAYER_CARD_INSTANCE(source_player, source_card).targets[current_target].card == effect_card)
    {
      for (;
           PLAYER_CARD_INSTANCE(source_player, source_card).info_slot + 1 < current_target;
           --current_target)
      {
        SET_TARGET(PLAYER_CARD_INSTANCE(source_player, source_card).targets[current_target], PLAYER_CARD_INSTANCE(source_player, source_card).targets[current_target - 1]);
      }
      ++PLAYER_CARD_INSTANCE(source_player, source_card).info_slot;
      found = 1;
    }
  }
  return found;
}

static __inline int is_card_id(int player, int card, int csvid)
{
  return PLAYER_CARD_INSTANCE(player, card).internal_card_id != -1 &&
         global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id == csvid;
}

static __inline int controls_card_id(int player, int csvid)
{
  int current_card;

  for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
  {
    if (is_in_play(player, current_card) && is_card_id(player, current_card, csvid))
    {
      return 1;
    }
  }
  return 0;
}

static __inline int damage_card_source_is_artifact(card_instance_t *damage)
{
  return damage->internal_card_id == g_damage_card_internal_card_id &&
         (damage->eot_toughness & TYPE_ARTIFACT) != 0;
}

// FUNCTION: SHANDALAR 0x0048686a
static int artifact_creature_damage_available(void)
{
  struct
  {
    card_instance_t *instance;
    int current_card;
    int available;
    int current_player;
  } s;

  s.current_player = 0;
  s.available = 0;
  for (; s.current_player < 2 && s.available == 0; ++s.current_player)
  {
    for (s.current_card = 0;
         s.current_card < g_active_cards_count[s.current_player] && s.available == 0;
         ++s.current_card)
    {
      s.instance = &PLAYER_CARD_INSTANCE(s.current_player, s.current_card);
      if (s.instance->internal_card_id == g_damage_card_internal_card_id &&
          is_in_play(s.current_player, s.current_card) != 0 &&
          s.instance->damage_target_card != -1 &&
          s.instance->info_slot != 0 &&
          (global_cards_data[PLAYER_CARD_INSTANCE(
                                 (int)s.instance->damage_target_player,
                                 s.instance->damage_target_card)
                                 .internal_card_id]
               .type &
           (TYPE_ARTIFACT | TYPE_CREATURE)) == (TYPE_ARTIFACT | TYPE_CREATURE))
      {
        s.available = 1;
      }
    }
  }
  return s.available;
}

static __inline int is_noncreature_artifact(int player, int card)
{
  int type;

  if (PLAYER_CARD_INSTANCE(player, card).internal_card_id == -1)
  {
    return 0;
  }

  type = global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type;
  return (type & TYPE_ARTIFACT) != 0 && (type & TYPE_CREATURE) == 0;
}

// FUNCTION: MAGIC 0x0046d000
// FUNCTION: SHANDALAR 0x00470b00
int card_abu_ja_far(int player, int card, event_t event)
{
  int effect_card;

  if (event == EVENT_CHANGE_TYPE)
  {
    PLAYER_CARD_INSTANCE(player, card).destroys_if_blocked |= 0x3f;
    return 0;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0 &&
      PLAYER_CARD_INSTANCE(player, card).kill_code != KILL_REMOVE &&
      ((g_current_player == player && (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0) ||
       (g_current_player != player && (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking != -1)))
  {
    effect_card = add_card_to_hand(player, g_duel_generated_internal_card_id_12);
    if (effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, effect_card).original_internal_card_id =
          PLAYER_CARD_INSTANCE(player, card).internal_card_id;
      PLAYER_CARD_INSTANCE(player, effect_card).state |= STATE_IN_PLAY;
      PLAYER_CARD_INSTANCE(player, effect_card).display_pic_info = CARD_ID_ABU_JAFAR;
      PLAYER_CARD_INSTANCE(player, effect_card).unknown0x14 = TRIGGER_GRAVEYARD_FROM_PLAY;
      PLAYER_CARD_INSTANCE(player, effect_card).kill_code = KILL_DESTROY;
      PLAYER_CARD_INSTANCE(player, effect_card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, effect_card).damage_source_card = card;
      PLAYER_CARD_INSTANCE(player, effect_card).damage_target_card =
          (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking;
      if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0)
      {
        PLAYER_CARD_INSTANCE(player, effect_card).info_slot = -2;
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, effect_card).info_slot =
            (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking;
      }
      TENTATIVE_set_timestamps(player, effect_card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046d478
// FUNCTION: SHANDALAR 0x00470f7a
int card_aladdin(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_RESOLVE_SPELL)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0x12;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           has_mana(player, COLOR_RED, 2) &&
           has_mana(player, COLOR_ANY, 3) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 1 - player,
                                 TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_ACTIVATE)
  {
    g_mana_charge[COLOR_COLORLESS] = 1;
    charge_mana(player, COLOR_RED, 2);
    if (g_spell_fizzled != 1)
    {
      load_text("promptsX1.txt", "ALADDIN");
      if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0,
                               g_text_lines[0], 1, &target) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0].card = target.card;
        PLAYER_CARD_INSTANCE(player, card).targets[0].player = target.player;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
        if (player == g_other_player &&
            ((global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].type & TYPE_CREATURE) != 0 ||
             (global_cards_data[PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id].extra_ability & 0x1001) != 0))
        {
          g_ai_modifier += 0x30;
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION &&
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
              .internal_card_id != -1)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      create_activated_control_effect(player, card, target);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  if (g_trigger_condition == TRIGGER_LEAVE_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_turn &&
      g_trigger_cause_controller == player &&
      g_trigger_cause == card &&
      PLAYER_CARD_INSTANCE(player, card).info_slot < 18)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      target.card = PLAYER_CARD_INSTANCE(player, card).info_slot;
      while (++target.card < 19)
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[target.card].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[target.card].card,
                  KILL_DESTROY);
        PLAYER_CARD_INSTANCE(player, card).targets[target.card].card = -1;
        PLAYER_CARD_INSTANCE(player, card).targets[target.card].player =
            PLAYER_CARD_INSTANCE(player, card).targets[target.card].card;
      }
      PLAYER_CARD_INSTANCE(player, card).info_slot = 18;
    }
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    if (g_affected_card == card && g_affected_card_controller == player)
    {
      target.card = PLAYER_CARD_INSTANCE(player, card).info_slot;
      while (++target.card < 19)
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[target.card].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[target.card].card,
                  KILL_DESTROY);
        PLAYER_CARD_INSTANCE(player, card).targets[target.card].card = -1;
        PLAYER_CARD_INSTANCE(player, card).targets[target.card].player =
            PLAYER_CARD_INSTANCE(player, card).targets[target.card].card;
      }
      PLAYER_CARD_INSTANCE(player, card).info_slot = 18;
    }
    else if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).targets[0].player == player &&
             PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).targets[0].card == card &&
             PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id ==
                 g_control_aura_special_internal_card_id)
    {
      remove_control_effect_from_source(player, card, g_affected_card_controller, g_affected_card);
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046e478
// FUNCTION: SHANDALAR 0x00471f68
int card_argivian_archaeologist(int player, int card, event_t event)
{
  struct
  {
    int hand_card;
    int graveyard_index;
  } s;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
           has_mana(player, COLOR_WHITE, 2) &&
           ((g_graveyard_card_types)[player] & TYPE_ARTIFACT) != 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_WHITE, 2);
    if (g_spell_fizzled != 1)
    {
      if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) || (g_duel_ai_mode_state == 1))
      {
        s.graveyard_index = find_highest_value_graveyard_card_by_type(player, TYPE_ARTIFACT);
      }
      else
      {
        do
        {
          s.graveyard_index = show_deck(player, global_graveyard_slots[player], 500,
                                        "Pick an artifact", 0, "Cancel");
        } while (s.graveyard_index != -1 &&
                 (global_cards_data[global_graveyard_slots[player][s.graveyard_index]].type & TYPE_ARTIFACT) == 0);
      }

      if (s.graveyard_index != -1)
      {
        PLAYER_CARD_INSTANCE(player, card).info_slot = s.graveyard_index;
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = global_graveyard_slots[player][s.graveyard_index];
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(player, card).info_slot != -1 &&
        global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot] != -1 &&
        global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot] == (int)PLAYER_CARD_INSTANCE(player, card).eot_toughness &&
        (global_cards_data[global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot]].type & TYPE_ARTIFACT) != 0)
    {
      s.hand_card = add_card_to_hand(player, global_graveyard_slots[player][PLAYER_CARD_INSTANCE(player, card).info_slot]);
      if (s.hand_card != -1)
      {
        remove_card_from_graveyard(player, PLAYER_CARD_INSTANCE(player, card).info_slot);
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046e8d0
// FUNCTION: SHANDALAR 0x004723c1
int card_argivian_blacksmith(int player, int card, event_t event)
{
  struct
  {
    int target_internal_card_id;
    char prompt[300];
    target_t target;
    int selected;
    int valid_selection;
    int done;
  } s;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((g_land_can_be_played & 4) != 0 &&
        (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
        artifact_creature_damage_available())
    {
      return 99;
    }
    return 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    s.done = 0;
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    do
    {
      do
      {
        s.valid_selection = 0;
        load_text("promptsX1.txt", "ARGIVIAN_BLACKSMITH");
        sprintf(s.prompt, g_text_lines[0], (int)PLAYER_CARD_INSTANCE(player, card).number_of_targets + 1, 2);
        s.selected = C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0,
                                          0, COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id,
                                          -1, -1, -1, 0, 0, 0, s.prompt, 3, &s.target);
        if (s.selected == 0)
        {
          s.valid_selection = 1;
        }
        else if (PLAYER_CARD_INSTANCE(player, card).number_of_targets == 0 ||
                 (PLAYER_CARD_INSTANCE(player, card).targets[0].player == s.target.player &&
                  PLAYER_CARD_INSTANCE(player, card).targets[0].card == s.target.card))
        {
          if (PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_card != -1)
          {
            if (PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_player,
                                     PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_card)
                    .internal_card_id == -1)
            {
              s.target_internal_card_id =
                  PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_player,
                                       PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_card)
                      .original_internal_card_id;
            }
            else
            {
              s.target_internal_card_id =
                  PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_player,
                                       PLAYER_CARD_INSTANCE(s.target.player, s.target.card).damage_target_card)
                      .internal_card_id;
            }
            if ((global_cards_data[s.target_internal_card_id].type & (TYPE_ARTIFACT | TYPE_CREATURE)) ==
                (TYPE_ARTIFACT | TYPE_CREATURE))
            {
              s.valid_selection = 1;
            }
          }
          if (s.valid_selection == 0 && g_duel_ai_mode_state == 1)
          {
            rewind_recorded_action();
          }
          else if (s.valid_selection == 0)
          {
            set_duel_prompt_text(g_text_lines[1]);
            Sleep(0x9c4);
            set_duel_prompt_text("");
          }
        }
        else if (g_duel_ai_mode_state == 1)
        {
          rewind_recorded_action();
        }
        else
        {
          set_duel_prompt_text(g_text_lines[2]);
          Sleep(0x9c4);
          set_duel_prompt_text("");
        }
      } while (s.valid_selection == 0);

      if (s.selected == 0)
      {
        if (s.target.card == -1)
        {
          g_spell_fizzled = 1;
        }
        else
        {
          s.done = 1;
        }
      }
      else
      {
        do
        {
          PLAYER_CARD_INSTANCE(player, card).targets[(int)PLAYER_CARD_INSTANCE(player, card).number_of_targets] = s.target;
          ++PLAYER_CARD_INSTANCE(player, card).number_of_targets;
          if (g_recorded_action_player == 0)
          {
            break;
          }
        } while (PLAYER_CARD_INSTANCE(player, card).number_of_targets < 2);
      }
    } while (PLAYER_CARD_INSTANCE(player, card).number_of_targets < 2 && g_spell_fizzled != 1 && s.done != 1);

    if (g_spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    while (PLAYER_CARD_INSTANCE(player, card).number_of_targets > 0)
    {
      --PLAYER_CARD_INSTANCE(player, card).number_of_targets;
      SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[(int)PLAYER_CARD_INSTANCE(player, card).number_of_targets]);
      if (C_real_validate_target(s.target.player, s.target.card, (char *)0, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_NONE, TYPE_NONE, 0,
                                 0, COLOR_TEST_0, COLOR_TEST_0, g_damage_card_internal_card_id,
                                 -1, -1, -1, 0, 0, 0))
      {
        if (PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot != 0)
        {
          --PLAYER_CARD_INSTANCE(s.target.player, s.target.card).info_slot;
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  if (event == EVENT_CHECK_PUMP &&
      (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0)
  {
    ++g_global_toughness_bonus[player];
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046f265
// FUNCTION: SHANDALAR 0x00472d56
int card_argothian_pixies(int player, int card, event_t event)
{
  card_argothian_treefolk(player, card, event);
  if (event == EVENT_BLOCK_LEGALITY &&
      card == g_attacking_card &&
      player == g_attacking_card_controller &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller,
                                              g_affected_card)
                             .internal_card_id]
           .type &
       TYPE_ARTIFACT) != 0)
  {
    g_event_result = 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046f305
// FUNCTION: SHANDALAR 0x00472df4
int card_argothian_treefolk(int player, int card, event_t event)
{
  if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0 &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == card &&
      (int)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player &&
      (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color & COLOR_TEST_ARTIFACT) != 0)
  {
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046f47c
// FUNCTION: SHANDALAR 0x00472f6b
int card_atog(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;

  if (event == EVENT_CAST_SPELL &&
      g_affected_card_controller == player &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller,
                                              g_affected_card)
                             .internal_card_id]
           .type &
       TYPE_ARTIFACT) != 0)
  {
    g_ai_modifier += 0x30;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 player,
                                 player,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT,
                                 TYPE_NONE,
                                 0,
                                 0,
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 -1,
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

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "ATOG");
    if (C_real_select_target(player,
                             player,
                             player,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT,
                             TYPE_NONE,
                             0,
                             0,
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
                             &target) != 0)
    {
      // PLAYER_CARD_INSTANCE(player, card).targets[0] = target;
      // PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(target.player, target.card, KILL_SACRIFICE);
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
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .internal_card_id != -1)
    {
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += 2;
      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .info_slot += 0x200;
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
  }

  if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY &&
      g_current_phase == PHASE_DISCARD &&
      player == g_other_player)
  {
    g_ai_modifier += g_duel_summary.artifact_counts[1] * 0x18;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046fb1e
// FUNCTION: SHANDALAR 0x0047360e
int card_camel(int player, int card, event_t event)
{
  int source_internal_card_id;
  int current_card;

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    if (PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player,
                             PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card)
            .internal_card_id != -1)
    {
      source_internal_card_id = PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player,
                                                     PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card)
                                    .internal_card_id;
    }
    else
    {
      source_internal_card_id = PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player,
                                                     PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card)
                                    .original_internal_card_id;
    }

    if (global_cards_data[source_internal_card_id].id == CARD_ID_DESERT)
    {
      if ((int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player &&
          PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == card)
      {
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot = 0;
      }

      if (g_current_player == player && (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking != -1)
      {
        for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
        {
          if (is_in_play(player, current_card) &&
              PLAYER_CARD_INSTANCE(player, card).blocking == PLAYER_CARD_INSTANCE(player, current_card).blocking &&
              (int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player &&
              PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == current_card)
          {
            PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot = 0;
          }
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0046ffa0
// FUNCTION: SHANDALAR 0x00473a91
int card_citanul_druid(int player, int card, event_t event)
{
  if (g_trigger_condition == TRIGGER_SPELL_CAST &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      player == g_current_turn &&
      (PLAYER_CARD_INSTANCE(player, card).state & 0x20) == 0 &&
      player != g_trigger_cause_controller &&
      PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id != -1 &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_trigger_cause_controller, g_trigger_cause).internal_card_id].type &
       TYPE_ARTIFACT) != 0)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      ++PLAYER_CARD_INSTANCE(player, card).counter_power;
      ++PLAYER_CARD_INSTANCE(player, card).counter_toughness;
      add_special_counters(player, card, 1);
    }
  }

  return 0;
}

static int clone_find_data_card_callback(int source_player,
                                         int source_card,
                                         int player,
                                         int card,
                                         int internal_card_id);

// FUNCTION: MAGIC 0x00470140
// FUNCTION: SHANDALAR 0x00473c2e
int card_clone(int player, int card, event_t event)
{
  target_t selected_target;
  int data_card_slot;
  int source_internal_card_id;
  int cloned_internal_card_id;
  int data_card_controller;
  unsigned int data_card_location;

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
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
                                 0x200,
                                 0,
                                 0);
  }

  if (((event == EVENT_CAST_SPELL) && (card == g_affected_card)) && (player == g_affected_card_controller))
  {
    load_text("promptsX1.txt", "CLONE");
    if (C_real_select_target(player,
                             2,
                             2,
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
                             TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
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
                               TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                               0,
                               0))
    {
      if (global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).original_internal_card_id]
              .id == CARD_ID_VESUVAN_DOPPELGANGER)
      {
        data_card_location = dispatch_function_to_all_cards_in_play(selected_target.player,
                                                                    selected_target.card,
                                                                    clone_find_data_card_callback,
                                                                    -1);
        if (data_card_location == -1)
        {
          source_internal_card_id = 0;
        }
        else
        {
          source_internal_card_id = PLAYER_CARD_INSTANCE((data_card_location >> 8) & 0xff,
                                                         data_card_location & 0xff)
                                        .info_slot;
        }
        data_card_controller = player;
        data_card_slot = add_card_to_hand(player, find_internal_card_id_by_csv_id(CARD_ID_DATA_CARD));
        if (data_card_slot != -1)
        {
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).state =
              ((((unsigned int)(((player == 0) - 1) >> 8)) & 0x1000) | 2);
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).token_status |= 0x10008;
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).damage_target_player =
              (char)player;
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).damage_target_card = card;
          cloned_internal_card_id = create_a_card_type(source_internal_card_id);
          if (cloned_internal_card_id != -1)
          {
            PLAYER_CARD_INSTANCE(player, card).internal_card_id = cloned_internal_card_id;
            PLAYER_CARD_INSTANCE(player, card).dummy3 = cloned_internal_card_id;
            PLAYER_CARD_INSTANCE(player, card).regen_status |= 0x1000000;
            PLAYER_CARD_INSTANCE(player, card).mana_color = global_cards_data[cloned_internal_card_id].color;
            if ((global_cards_data[cloned_internal_card_id].type & TYPE_ARTIFACT) != 0)
            {
              ++g_duel_summary.artifact_counts[player];
            }
            if ((global_cards_data[cloned_internal_card_id].type & TYPE_ENCHANTMENT) != 0)
            {
              ++g_duel_summary.enchantment_counts[player];
            }
            g_card_types_in_play[player] |= global_cards_data[cloned_internal_card_id].type;
            PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).info_slot =
                source_internal_card_id;
            PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness |=
                global_cards_data[source_internal_card_id].extra_ability & 1;
            dispatch_event_to_single_card(player, card, EVENT_CAST_SPELL, 1 - player, -1);
            dispatch_event_to_single_card(player, card, EVENT_RESOLVE_SPELL, 1 - player, -1);
            global_cards_data[cloned_internal_card_id].code_pointer =
                global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card)
                                      .original_internal_card_id]
                    .code_pointer;
            global_cards_data[cloned_internal_card_id].extra_ability |= 1;
          }
        }
      }
      else
      {
        PLAYER_CARD_INSTANCE(player, card).dummy3 =
            PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).internal_card_id;
        PLAYER_CARD_INSTANCE(player, card).internal_card_id = PLAYER_CARD_INSTANCE(player, card).dummy3;
        PLAYER_CARD_INSTANCE(player, card).color =
            PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).color;
        process_card_enters_play(player, card);
      }
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      kill_card(player, card, KILL_DESTROY);
    }
  }

  if (((event == EVENT_CHANGE_TYPE) && ((g_land_can_be_played & 0x20000) == 0)) && ((card == g_affected_card) && (player == g_affected_card_controller)))
  {
    g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00474721
int choose_cuombajj_witches_auto_target(int source_player, int source_card, int chooser, target_t *target)
{
  int current_card;
  int internal_card_id;
  int score;
  int best_score;
  unsigned int abilities;

  best_score = -1;
  target->player = chooser;
  target->card = -1;

  current_card = 0;
  while (current_card < g_active_cards_count[chooser])
  {
    if (is_in_play(chooser, current_card))
    {
      internal_card_id = PLAYER_CARD_INSTANCE(chooser, current_card).internal_card_id;
      if ((global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0 &&
          PLAYER_CARD_INSTANCE(chooser, current_card).toughness -
                  PLAYER_CARD_INSTANCE(chooser, current_card).damage_on_card ==
              1)
      {
        score = 0;
        abilities = PLAYER_CARD_INSTANCE(chooser, current_card).regen_status;
        if ((abilities & get_protections_from(source_player, source_card)) == 0)
        {
          if ((abilities & KEYWORD_REGENERATION) != 0 &&
              has_mana(chooser, single_color_test_bit_to_color_t((int)global_cards_data[internal_card_id].color), 1) != 0)
          {
            ++current_card;
            continue;
          }
          if ((PLAYER_CARD_INSTANCE(chooser, current_card).state & STATE_TAPPED) != 0)
          {
            score = 3;
          }
          if ((global_cards_data[internal_card_id].extra_ability & 1) != 0)
          {
            score += 2;
          }
          if ((global_cards_data[internal_card_id].extra_ability & 0x1000) != 0)
          {
            ++score;
          }
          while (abilities != 0)
          {
            if ((abilities & 1) != 0)
            {
              ++score;
            }
            abilities = (unsigned int)((int)abilities >> 1);
          }
          if (best_score < score)
          {
            best_score = score;
            target->card = current_card;
          }
        }
      }
    }
    ++current_card;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047095d
// FUNCTION: SHANDALAR 0x0047444f
int card_cuombajj_witches(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "CUOMBAJJ_WITCHES");
    select_damage_target(player, card, 1);
    if (g_spell_fizzled != 1)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    deal_damage_to_selected_target(player, card, event, 1);
    if ((g_active_player == player && (g_duel_network_flags & 2) == 0) ||
        g_duel_ai_mode_state == 1)
    {
      choose_cuombajj_witches_auto_target(g_card_on_stack_controller, g_card_on_stack, player, &target);
      load_text("promptsX1.txt", "CUOMBAJJ_WITCHES");
      do_dialog(1 - player, player, card, target.player, target.card, g_text_lines[1], 0);
    }
    else
    {
      load_text("promptsX1.txt", "CUOMBAJJ_WITCHES");
      C_real_select_target(1 - player, 2, player, TARGET_ZONE_PLAYERS | TARGET_ZONE_IN_PLAY,
                           TYPE_CREATURE, TYPE_NONE, 0,
                           get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                           -1, -1, -1, -1, 0, 0, 0, g_text_lines[0], 1, &target);
    }
    if (target.player != -1)
    {
      if (target.card == -1)
      {
        damage_player(target.player, 1, g_card_on_stack_controller, g_card_on_stack);
      }
      else
      {
        damage_creature(target.player, target.card, 1, g_card_on_stack_controller, g_card_on_stack);
      }
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00470e7e
// FUNCTION: SHANDALAR 0x00474970
int card_demonic_hordes(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 && real_target_available((int *)0,
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
                                                                                               0) != 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "DEMONIC_HORDES");
    if (select_target_land_and_store(player, 2, card))
    {
      PLAYER_CARD_INSTANCE(player, card).state |= 0x10;
      g_ai_modifier += (PLAYER_CARD_INSTANCE(player, card).targets[0].player == player ? -2 : 3) * 12;
    }
    else
    {
      g_spell_fizzled = 1;
    }

    if (g_spell_fizzled == 1)
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
      kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card, KILL_DESTROY);
    }
    else
    {
      g_spell_fizzled = 1;
    }

    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
  }

  if (event == EVENT_SETUP_UPKEEP_COSTS && card == g_affected_card && player == g_affected_card_controller && player == g_current_player && g_event_player == player)
  {
    PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 1;
    PLAYER_CARD_INSTANCE(player, card).upkeep_black += 3;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    if (real_target_available((int *)0,
                              TARGET_SCAN_DIRECT,
                              player,
                              player,
                              player,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_LAND,
                              TYPE_NONE,
                              0,
                              0,
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
      if (g_other_player == player || (g_duel_network_flags & 2) != 0)
      {
        load_text("promptsX1.txt", "DEMONIC_HORDES");
        C_real_select_target(1 - player,
                             2,
                             2,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_LAND,
                             TYPE_NONE,
                             0,
                             0,
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
                             0,
                             &selected_target);
      }
      else
      {
        select_best_land_target_by_score(1 - player, player, (int *)&selected_target);
        do_dialog(player,
                  player,
                  card,
                  selected_target.player,
                  selected_target.card,
                  "selects for player to sacrifice.",
                  0);
      }

      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(selected_target.player, selected_target.card, KILL_SACRIFICE);
    }

    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).state |= STATE_TAPPED;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047146f
// FUNCTION: SHANDALAR 0x00474f5d
int card_desert_nomads(int player, int card, event_t event)
{
  int source_internal_card_id;

  if (event == EVENT_BLOCK_LEGALITY &&
      g_attacking_card == card &&
      g_attacking_card_controller == player &&
      controls_card_id(1 - player, CARD_ID_DESERT))
  {
    g_event_result = 1;
  }

  if (event == EVENT_DEAL_DAMAGE &&
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)
  {
    if (PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player,
                             PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card)
            .internal_card_id == -1)
    {
      source_internal_card_id =
          PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player,
                               PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card)
              .original_internal_card_id;
    }
    else
    {
      source_internal_card_id =
          PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player,
                               PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card)
              .internal_card_id;
    }
    if (global_cards_data[source_internal_card_id].id == CARD_ID_DESERT &&
        (int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_player == player &&
        PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_target_card == card)
    {
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot = 0;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00471838
// FUNCTION: SHANDALAR 0x00475328
int card_dwarven_demolition_team(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return ((PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 && real_target_available((int *)0,
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
                                                                                               0xffffffff,
                                                                                               0xffffffff,
                                                                                               1,
                                                                                               0,
                                                                                               0));
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "DWARVEN_DTEAM");
    if (C_real_select_target(player,
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
                             TARGET_SPECIAL_WALL,
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
                               TARGET_SPECIAL_WALL,
                               0,
                               0))
    {
      kill_card(target.player, target.card, KILL_DESTROY);
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00471ba6
// FUNCTION: SHANDALAR 0x00475696
int card_dwarven_weaponsmith(int player, int card, event_t event)
{
  target_t target;
  target_t artifact;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return g_current_phase == PHASE_UPKEEP &&
           g_current_player == player &&
           CAN_TAP(player, card) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player,
                                 TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "DWARVEN_WEAPONSMITH");
    if (select_own_artifact_for_cost(player, card, &artifact, g_text_lines[0]) == 0)
    {
      g_spell_fizzled = 1;
    }
    else
    {
      load_text("promptsX1.txt", "DWARVEN_WEAPONSMITH");
      if (C_real_select_target(player, 2, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0,
                               g_text_lines[1], 1, &target))
      {
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(WAV_SACRFICE);
        }
        kill_card(artifact.player, artifact.card, KILL_SACRIFICE);
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

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      ++PLAYER_CARD_INSTANCE(target.player, target.card).counter_power;
      ++PLAYER_CARD_INSTANCE(target.player, target.card).counter_toughness;
      PLAYER_CARD_INSTANCE(target.player, target.card).special_counters = ((PLAYER_CARD_INSTANCE(target.player, target.card).special_counters + 1) & 0xff) |
                                                                          (PLAYER_CARD_INSTANCE(target.player, target.card).special_counters & 0xffffff00);
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_COUNTER);
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00476297
int select_erhnam_djinn_target(int player, int card, target_t *target)
{
  int current_card;
  int internal_card_id;
  int score;
  int best_score;
  unsigned int abilities;

  best_score = 999;
  target->player = g_active_player;
  target->card = -1;
  for (current_card = 0; current_card < g_active_cards_count[g_active_player]; ++current_card)
  {
    internal_card_id = PLAYER_CARD_INSTANCE(g_active_player, current_card).internal_card_id;
    if (is_in_play(g_active_player, current_card) &&
        (global_cards_data[internal_card_id].type & TYPE_CREATURE) != 0 &&
        global_cards_data[internal_card_id].subtype != SUB_WALL)
    {
      score = 0;
      abilities = PLAYER_CARD_INSTANCE(g_active_player, current_card).regen_status;
      if ((abilities & get_protections_from(player, card)) == 0)
      {
        if ((abilities & (1 << ((unsigned char)get_hacked_color(player, card, COLOR_GREEN) - 1))) != 0)
        {
          score = 99;
        }
        if ((PLAYER_CARD_INSTANCE(g_active_player, current_card).state & STATE_TAPPED) != 0)
        {
          score += 3;
        }
        if ((global_cards_data[internal_card_id].extra_ability & 1) != 0)
        {
          score += 2;
        }
        if ((global_cards_data[internal_card_id].extra_ability & 0x1000) != 0)
        {
          ++score;
        }
        while (abilities != 0)
        {
          if ((abilities & 1) != 0)
          {
            ++score;
          }
          abilities >>= 1;
        }
        if (score < best_score)
        {
          best_score = score;
          target->card = current_card;
        }
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004722ff
// FUNCTION: SHANDALAR 0x00475df0
int card_erhnam_djinn(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player &&
      player == g_other_player && (g_duel_network_flags & 2) == 0)
  {
    select_erhnam_djinn_target(player, card, &target);
    if (PLAYER_CARD_INSTANCE(target.player, target.card).internal_card_id != -1 &&
        g_life[player] <= PLAYER_CARD_INSTANCE(target.player, target.card).power * 2)
    {
      g_ai_modifier -= 0x30;
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == PHASE_UPKEEP && g_current_player == g_event_player &&
        player == g_current_player && (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0 &&
        real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 1 - player, 1 - player,
                              TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                              get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                              -1, -1, -1, -1, TARGET_SPECIAL_NON_WALL, 0, 0) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
      g_activation_event_flags |= 3;
      return 1;
    }
  }
  else
  {
    if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
      g_upkeep_payment_completed = 1;
      g_event_result |= 1;
    }

    if (event == EVENT_UPKEEP_COSTS_UNPAID &&
        real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 1 - player, 1 - player,
                              TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                              get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                              -1, -1, -1, -1, TARGET_SPECIAL_NON_WALL, 0, 0) != 0)
    {
      if (player == g_active_player || (g_duel_network_flags & 2) != 0)
      {
        load_text("promptsX1.txt", "ERHNAM_DJINN");
        C_real_select_target(player, 1 - player, 1 - player, TARGET_ZONE_IN_PLAY,
                             TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card),
                             COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                             TARGET_SPECIAL_NON_WALL, 0, 0, g_text_lines[0], 0, &target);
      }
      else
      {
        select_erhnam_djinn_target(g_card_on_stack_controller, g_card_on_stack, &target);
      }
      if (target.card != -1)
      {
        legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack,
                                           g_duel_generated_internal_card_id_1f, target.player, target.card);
        if (legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, legacy_card).info_slot =
              1 << (unsigned char)(get_hacked_color(player, card, COLOR_GREEN) - 1);
          PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 1;
        }
      }
    }

    if (event == EVENT_CLEANUP)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
    }

    if (event == 199 && g_current_phase == 0x1f &&
        g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_GREEN)] == 0)
    {
      g_ai_modifier += 0x60;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004729c8
// FUNCTION: SHANDALAR 0x004764bc
int card_gaea_s_avenger(int player, int card, event_t event)
{
  int legacy_card;

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_1a, player, card);
    if (legacy_card != -1)
    {
      PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0x10020e;
      PLAYER_CARD_INSTANCE(player, legacy_card).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = COLOR_TEST_ARTIFACT;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_card;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00472b0a
// FUNCTION: SHANDALAR 0x004765fe
int card_ghazban_ogre(int player, int card, event_t event)
{
  int new_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == PHASE_UPKEEP &&
        g_life[player] < g_life[1 - player] &&
        g_current_player == g_event_player &&
        g_current_player == player &&
        (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= UPKEEP_UPKEEP_TRIGGER | UPKEEP_UPKEEP_NODIALOG;
      g_activation_event_flags |= 3;
      return 1;
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID && g_life[player] < g_life[1 - player])
  {
    new_card = gain_control(g_card_on_stack_controller, g_card_on_stack);
    if (new_card != -1)
    {
      PLAYER_CARD_INSTANCE(1 - player, new_card).info_slot |= 1;
    }
  }

  if (event == EVENT_SHOULD_AI_PLAY &&
      g_current_phase == PHASE_DISCARD &&
      1 - player == g_current_player &&
      g_life[player] < g_life[1 - player])
  {
    new_card = gain_control(player, card);
  }

  if (event == EVENT_CAST_SPELL &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_duel_ai_mode_state == 1 &&
      g_other_player == player &&
      g_life[player] < g_life[1 - player])
  {
    new_card = gain_control(player, card);
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00472df0
// FUNCTION: SHANDALAR 0x004768e5
int card_giant_badger(int player, int card, event_t event)
{
  if (event == EVENT_DECLARE_BLOCKERS &&
      g_current_phase == PHASE_DECLARE_BLOCKERS &&
      (int)(char)PLAYER_CARD_INSTANCE(player, card).blocking != -1 &&
      player != g_current_player &&
      global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].code_pointer !=
          card_two_headed_giant_of_foriys_legacy)
  {
    give_pump_until_eot(player, card, player, card, 2, 2);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00472f31
// FUNCTION: SHANDALAR 0x00476a27
int card_goblin_artisans(int player, int card, event_t event)
{
  char dialog_text[900];

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_spell_player != -1)
    {
      return (g_land_can_be_played & 0x20) != 0 &&
                     (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
                     C_real_validate_target(g_current_spell_player, g_current_spell_card, (char *)0, player,
                                            player, player, 0, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                                            COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                                            TARGET_SPECIAL_SPELL_ON_STACK, 0, 0) != 0
                 ? 99
                 : 0;
    }
    else
    {
      return 0;
    }
  }

  if (event == EVENT_ACTIVATE && g_current_spell_player != -1)
  {
    PLAYER_CARD_INSTANCE(player, card).targets[0].player = g_current_spell_player;
    PLAYER_CARD_INSTANCE(player, card).targets[0].card = g_current_spell_card;
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    if (g_other_player == player)
    {
      g_ai_modifier += 0x30 - g_duel_summary.hand_counts[player] * 0x18;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0, player, player, player, 0,
                               TYPE_ARTIFACT, TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, TARGET_SPECIAL_SPELL_ON_STACK, 0, 0) != 0)
    {
      int choice;

      load_text("promptsX1.txt", "GOBLIN_ARTISANS");
      if (g_other_player == player)
      {
        sprintf(dialog_text, " %s\n %s", g_text_lines[1], g_text_lines[2]);
      }
      else
      {
        sprintf(dialog_text, "%s\n %s\n %s", g_text_lines[0], g_text_lines[1], g_text_lines[2]);
      }
      choice = do_dialog(player, player, card, -1, -1, dialog_text, internal_rand(2));
      if (coin_flip(player, get_displayed_card_name(player, card), 1) == choice)
      {
        draw_card_for_player(player);
      }
      else
      {
        kill_card(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                  PLAYER_CARD_INSTANCE(player, card).targets[0].card, KILL_BURY);
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004732e4
// FUNCTION: SHANDALAR 0x00476ddc
int card_granite_gargoyle(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_UNTAP_PHASE)
  {
    ++g_ai_mana_demand_by_color[player][COLOR_RED];
  }

  if (event == EVENT_CAST_SPELL)
  {
    if (g_affected_card == card)
    {
      if (g_affected_card_controller == player)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      }
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = has_mana(player, COLOR_RED, 1);
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_code(0);
    result = 0;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      result = has_mana(player, COLOR_RED, 1);
      if (result != 0)
      {
        if (g_current_player == player)
        {
          charge_mana(player, 4, -1);
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
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                               PLAYER_CARD_INSTANCE(player, card).parent_card)
              .internal_card_id != -1)
      {
        *(int *)&PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
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
          result = create_legacy_effect(g_card_on_stack_controller,
                                        g_card_on_stack,
                                        LEGACY_EFFECT_PUMP,
                                        g_card_on_stack_controller,
                                        g_card_on_stack);
          if (result != -1)
          {
            *(unsigned int *)&PLAYER_CARD_INSTANCE(player, result).info_slot |= 0x80000;
          }
        }
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }

    if (event == EVENT_TOU_BOOST)
    {
      result = has_mana(player, COLOR_RED, 1);
    }
    else
    {
      if (event == EVENT_CAN_WASTE_MANA && g_raw_mana_available[player][COLOR_RED] != 0)
      {
        g_event_result |= 1;
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

      if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY)
      {
        PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
        PLAYER_CARD_INSTANCE(player, card).info_slot = PLAYER_CARD_INSTANCE(player, card).eot_toughness;
      }

      result = 0;
    }
  }

  return result;
}

// FUNCTION: MAGIC 0x00473ab4
// FUNCTION: SHANDALAR 0x004775a8
static int guardian_beast_destroy_attached_aura(int source_player,
                                                int source_card,
                                                int target_player,
                                                int target_card,
                                                int target_internal_card_id);

int card_guardian_beast(int player, int card, event_t event)
{
  struct
  {
    int legacy_card;
    int current_card;
  } s;

  if (event == EVENT_RESOLVE_SPELL ||
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) != 0)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
  }

  if (event == EVENT_CHANGE_TYPE &&
      is_in_play(player, card) &&
      g_affected_card_controller == player &&
      (global_cards_data[g_event_result].type & (TYPE_CREATURE | TYPE_ARTIFACT)) == TYPE_ARTIFACT)
  {
    if ((g_land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS) != 0)
    {
      if ((PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).token_status &
           STATUS_CANNOT_BE_DESTROYED) == 0)
      {
        s.legacy_card = create_legacy_effect(player,
                                             card,
                                             g_control_aura_legacy_internal_card_id,
                                             g_affected_card_controller,
                                             g_affected_card);
        if (s.legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, s.legacy_card).targets[0].player = -1;
          PLAYER_CARD_INSTANCE(player, s.legacy_card).targets[0].card = -1;
          PLAYER_CARD_INSTANCE(player, s.legacy_card).token_status |=
              STATUS_INVISIBLE_FX | STATUS_PERMANENT;
          PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).token_status |=
              STATUS_CANNOT_BE_DESTROYED;
        }
      }
    }
    else
    {
      g_land_can_be_played |= LCBP_NEED_EVENT_CHANGE_TYPE_SECOND_PASS;
    }
  }

  if (event == EVENT_ABILITIES &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    if (g_affected_card_controller == player &&
        (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller,
                                                g_affected_card)
                               .internal_card_id]
             .type &
         (TYPE_CREATURE | TYPE_ARTIFACT)) == TYPE_ARTIFACT)
    {
      g_event_result |= KEYWORD_PROT_ENCHANTMENTS;
    }

    if (PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
      dispatch_function_to_all_cards_in_play(player,
                                             card,
                                             guardian_beast_destroy_attached_aura,
                                             -1);
    }
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
      (int)(char)PLAYER_CARD_INSTANCE(g_affected_card_controller,
                                      g_affected_card)
              .kill_code == KILL_DESTROY &&
      is_in_play(g_affected_card_controller, g_affected_card) &&
      g_affected_card_controller == player &&
      (global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller,
                                              g_affected_card)
                             .internal_card_id]
           .type &
       (TYPE_CREATURE | TYPE_ARTIFACT)) == TYPE_ARTIFACT)
  {
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).kill_code = 0;
    PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).unknown0x14 = 0;
    ++g_event_result;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    for (s.current_card = 0;
         s.current_card < g_active_cards_count[player];
         ++s.current_card)
    {
      if (PLAYER_CARD_INSTANCE(player, s.current_card).internal_card_id ==
              g_control_aura_legacy_internal_card_id &&
          (int)(char)PLAYER_CARD_INSTANCE(player, s.current_card).damage_source_player == player &&
          PLAYER_CARD_INSTANCE(player, s.current_card).damage_source_card == card)
      {
        kill_card(player, s.current_card, KILL_DESTROY);
      }
    }
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00477b57
static int guardian_beast_destroy_attached_aura(int source_player,
                                                int source_card,
                                                int target_player,
                                                int target_card,
                                                int target_internal_card_id)
{
  if ((global_cards_data[target_internal_card_id].type & TYPE_ENCHANTMENT) != 0 &&
      (int)(char)PLAYER_CARD_INSTANCE(target_player, target_card).damage_target_player == source_player &&
      (global_cards_data[PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(target_player,
                                                                              target_card)
                                                  .damage_target_player,
                                              PLAYER_CARD_INSTANCE(target_player,
                                                                   target_card)
                                                  .damage_target_card)
                             .internal_card_id]
           .type &
       (TYPE_CREATURE | TYPE_ARTIFACT)) == TYPE_ARTIFACT)
  {
    kill_card(target_player, target_card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00474179
// FUNCTION: SHANDALAR 0x00477c68
int card_hasran_ogress(int player, int card, event_t event)
{
  char dialog_text[900];
  int paid;

  if (event == EVENT_DECLARE_ATTACKERS &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0 &&
      PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    paid = 0;
    if (has_mana(player, COLOR_ANY, 2) != 0)
    {
      load_text("promptsX1.txt", "HASRAN_OGRESS");
      sprintf(dialog_text, " %s\n %s", g_text_lines[0], g_text_lines[1]);
      if (do_dialog(player, player, card, -1, -1, dialog_text, 0) == 0)
      {
        push_card_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0);
        charge_mana(player, COLOR_COLORLESS, 2);
        obliterate_top_card_of_stack();
        if (g_spell_fizzled != 1)
        {
          paid = 1;
        }
        else
        {
          g_spell_fizzled = -1;
        }
      }
    }
    if (paid == 0)
    {
      damage_player(player, 3, player, card);
    }
  }

  if (event == EVENT_ATTACK_LEGALITY &&
      player == g_other_player &&
      (g_duel_network_flags & 2) == 0)
  {
    if (has_mana(player, COLOR_ANY, 1) < 4)
    {
      g_event_result = 1;
    }
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004789d3
int ifh_biff_efreet_damage_flyer(int source_player, int source_card, int target_player, int target_card, int target_internal_card_id)
{
  if ((global_cards_data[target_internal_card_id].type & TYPE_CREATURE) != 0 &&
      (C_get_abilities(target_player, target_card, EVENT_ABILITIES, -1) & KEYWORD_FLYING) != 0)
  {
    damage_creature(target_player, target_card, 1, source_player, source_card);
  }

  return 0;
}

static __inline void create_ifh_biff_efreet_effect(int source_player, int source_card, int parent_player, int parent_card)
{
  int effect_player;
  int effect_card;

  effect_player = 1 - source_player;
  effect_card = add_card_to_hand(effect_player, g_duel_generated_internal_card_id_25);
  if (effect_card != -1)
  {
    PLAYER_CARD_INSTANCE(effect_player, effect_card).state |= STATE_IN_PLAY | (source_player != 0 ? STATE_OWNED_BY_OPPONENT : 0);
    PLAYER_CARD_INSTANCE(effect_player, effect_card).display_pic_info =
        global_cards_data[PLAYER_CARD_INSTANCE(parent_player, parent_card).internal_card_id].id;
    PLAYER_CARD_INSTANCE(effect_player, effect_card).damage_source_player = (char)source_player;
    PLAYER_CARD_INSTANCE(effect_player, effect_card).damage_source_card = source_card;
    PLAYER_CARD_INSTANCE(effect_player, effect_card).token_status |= STATUS_PERMANENT;
    PLAYER_CARD_INSTANCE(parent_player, parent_card).damage_source_player = (char)effect_player;
    PLAYER_CARD_INSTANCE(parent_player, parent_card).damage_source_card = effect_card;
    PLAYER_CARD_INSTANCE(effect_player, effect_card).color = PLAYER_CARD_INSTANCE(parent_player, parent_card).color;
  }
}

// FUNCTION: MAGIC 0x004743a9
// FUNCTION: SHANDALAR 0x00477e97
int card_ifh_biff_efreet(int player, int card, event_t event)
{
  int source_player;
  int source_card;
  int effect_player;
  int effect_card;

  if (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id == g_legacy_display_internal_card_id &&
      PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1 &&
      PLAYER_CARD_INSTANCE(player, card).damage_source_card != -1)
  {
    PLAYER_CARD_INSTANCE(player, card).color =
        PLAYER_CARD_INSTANCE((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_source_card)
            .color;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_ifh_biff_efreet_effect(player, card, player, card);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return has_mana(player, COLOR_GREEN, 1);
  }

  if (event == EVENT_ACTIVATE)
  {
    charge_mana(player, COLOR_GREEN, 1);
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                               PLAYER_CARD_INSTANCE(player, card).parent_card)
                              .internal_card_id]
            .id == g_legacy_display_internal_card_id)
    {
      source_player = (int)(char)PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                                      PLAYER_CARD_INSTANCE(player, card).parent_card)
                          .damage_source_player;
      source_card = PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                         PLAYER_CARD_INSTANCE(player, card).parent_card)
                        .damage_source_card;
    }
    else
    {
      source_player = g_card_on_stack_controller;
      source_card = g_card_on_stack;
    }
    damage_player(1 - player, 1, source_player, source_card);
    damage_player(player, 1, source_player, source_card);
    dispatch_function_to_all_cards_in_play(source_player, source_card, ifh_biff_efreet_damage_flyer, -1);
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].id == CARD_ID_IFH_BIFF_EFREET)
  {
    kill_card((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
              PLAYER_CARD_INSTANCE(player, card).damage_source_card,
              KILL_REMOVE);
  }

  if (g_trigger_condition == TRIGGER_LEAVE_PLAY &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].id == CARD_ID_IFH_BIFF_EFREET &&
      player == g_trigger_cause_controller &&
      card == g_trigger_cause &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= RESOLVE_TRIGGER_MANDATORY;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      kill_card((int)(char)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                PLAYER_CARD_INSTANCE(player, card).damage_source_card,
                KILL_REMOVE);
    }
  }

  if (event == EVENT_CARDCONTROLLED &&
      global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id == g_legacy_display_internal_card_id &&
      PLAYER_CARD_INSTANCE(player, card).damage_source_player == g_affected_card_controller &&
      PLAYER_CARD_INSTANCE(player, card).damage_source_card == g_affected_card &&
      player == g_affected_card_controller)
  {
    effect_player = 1 - g_affected_card_controller;
    effect_card = add_card_to_hand(effect_player, g_duel_generated_internal_card_id_25);
    if (effect_card != -1)
    {
      PLAYER_CARD_INSTANCE(effect_player, effect_card).state |= STATE_IN_PLAY | (g_affected_card_controller != 0 ? STATE_OWNED_BY_OPPONENT : 0);
      PLAYER_CARD_INSTANCE(effect_player, effect_card).display_pic_info =
          global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].id;
      PLAYER_CARD_INSTANCE(effect_player, effect_card).damage_source_player = (char)g_affected_card_controller;
      PLAYER_CARD_INSTANCE(effect_player, effect_card).damage_source_card = g_affected_card;
      PLAYER_CARD_INSTANCE(effect_player, effect_card).token_status |= STATUS_PERMANENT;
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player = (char)effect_player;
      PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card = effect_card;
      PLAYER_CARD_INSTANCE(effect_player, effect_card).color =
          PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).color;
    }
    PLAYER_CARD_INSTANCE(player, card).internal_card_id = -1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00474f4d
// FUNCTION: SHANDALAR 0x00478a33
int card_juggernaut(int player, int card, event_t event)
{
  if ((event == EVENT_BLOCK_LEGALITY) && g_attacking_card == card && player == g_attacking_card_controller && global_cards_data[PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id].subtype == 0)
  {
    g_event_result = 1;
  }
  if ((event == EVENT_MUST_ATTACK) && ((PLAYER_CARD_INSTANCE(player, card).state & STATE_UNKNOWN8000) == 0))
  {
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_UNKNOWN8000;
    if (can_attack(player, card))
    {
      g_attacking_creature_count = 1;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047506c
// FUNCTION: SHANDALAR 0x00478b53
int card_khabal_ghoul(int player, int card, event_t event)
{
  if ((g_trigger_condition == TRIGGER_EOT || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_duel_summary.creatures_died != 0 &&
      player == g_current_turn)
  {
    if (event == EVENT_TRIGGER)
    {
      g_event_result |= 2;
    }
    if (event == EVENT_RESOLVE_TRIGGER || event == EVENT_SHOULD_AI_PLAY)
    {
      add_special_counters(player, card, g_duel_summary.creatures_died);
      PLAYER_CARD_INSTANCE(player, card).counter_power += g_duel_summary.creatures_died;
      PLAYER_CARD_INSTANCE(player, card).counter_toughness += g_duel_summary.creatures_died;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004751d9
// FUNCTION: SHANDALAR 0x00478cc1
int card_king_suleiman(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 1 - player,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, TARGET_SPECIAL_DJINN_OR_EFREET, 0, 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "KING_SULEIMAN");
    if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, TARGET_SPECIAL_DJINN_OR_EFREET, 0, 0,
                             g_text_lines[0], 1, &target) != 0)
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

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, TARGET_SPECIAL_DJINN_OR_EFREET, 0, 0) != 0)
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
  }

  return 0;
}

// FUNCTION: MAGIC 0x00475555
// FUNCTION: SHANDALAR 0x0047903d
int card_kird_ape(int player, int card, event_t event)
{
  if (g_affected_card == card &&
      g_affected_card_controller == player &&
      g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_GREEN)] > 0)
  {
    if (event == EVENT_POWER)
    {
      ++g_event_result;
    }
    if (event == EVENT_TOUGHNESS)
    {
      g_event_result += 2;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x004755ca
// FUNCTION: SHANDALAR 0x004790b4
int card_martyrs_of_korlis(int player, int card, event_t event)
{
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_or_martyrs_present, player) != -1)
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
    }
  }

  if (g_trigger_condition == TRIGGER_END_DAMAGE_PREV &&
      PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0 &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      g_current_turn == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = event;
    if (event == EVENT_TRIGGER)
    {
      if (dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_prevent_damage_callback, -1) != -1)
      {
        g_event_result |= 2;
      }
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_prevent_damage_callback, -1);
      dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_mark_other_protectors, player);
    }
  }

  if (event == EVENT_CLEANUP && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004757d7
// FUNCTION: SHANDALAR 0x004792c1
int card_merchant_ship(int player, int card, event_t event)
{
  struct
  {
    int unblocked;
    int opponent;
    int current_card;
  } s;

  if (event == EVENT_DECLARE_BLOCKERS &&
      g_current_player == player &&
      ((global_card_instances[player] + card)->state & STATE_ATTACKING) != 0)
  {
    s.unblocked = 1;
    s.opponent = 1 - player;
    for (s.current_card = 0; s.current_card < g_active_cards_count[s.opponent]; ++s.current_card)
    {
      if (is_in_play(s.opponent, s.current_card) &&
          PLAYER_CARD_INSTANCE(s.opponent, s.current_card).blocking == card)
      {
        s.unblocked = 0;
        break;
      }
    }

    if (s.unblocked != 0)
    {
      gain_life(player, 2, player, card);
    }
  }

  card_sea_serpent(player, card, event);

  if (event == EVENT_ATTACK_RATING &&
      card == g_affected_card &&
      g_affected_card_controller == player)
  {
    g_ai_score -= 12;
    return 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY)
  {
    // TODO: The original has an unreachable AI-modifier instruction for some reason?
    if (0)
    {
      g_ai_modifier += 48;
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00475953
// FUNCTION: SHANDALAR 0x0047943c
int card_mijae_djinn(int player, int card, event_t event)
{
  char dialog_text[900];
  int choice;

  if (event == EVENT_DECLARE_ATTACKERS &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_ATTACKING) != 0 &&
      PLAYER_CARD_INSTANCE(player, card).info_slot == 0)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    load_text("promptsX1.txt", "MIJAE_DJINN");
    sprintf(dialog_text, " %s\n %s", g_text_lines[0], g_text_lines[1]);
    choice = do_dialog(player, player, card, -1, -1, dialog_text, internal_rand(2));
    if (coin_flip(player, get_displayed_card_name(player, card), 1) != choice)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_ATTACKING;
      PLAYER_CARD_INSTANCE(player, card).blocking = -1;
    }
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00475b66
// FUNCTION: SHANDALAR 0x0047964f
int card_nettling_imp(int player, int card, event_t event)
{
  target_t target;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 && player != g_current_player && g_current_phase < 0x1a && real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 1 - player, 1 - player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0, get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0, -1, -1, 0xffffffff, 0xffffffff, 0x80, 0, 0x1000);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "NETTLING_IMP");
    if (C_real_select_target(player,
                             1 - player,
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
                             TARGET_SPECIAL_NON_WALL,
                             0,
                             TARGET_STATE_SUMMONING_SICK,
                             g_text_lines[0],
                             1,
                             &target) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = target.card;
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = target.player;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      if (player == g_other_player)
      {
        g_ai_modifier += 0x30;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                               (char *)0,
                               player,
                               1 - player,
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
                               TARGET_SPECIAL_NON_WALL,
                               0,
                               TARGET_STATE_SUMMONING_SICK) != 0)
    {
      create_legacy_effect(g_card_on_stack_controller, g_card_on_stack, g_duel_generated_internal_card_id_1e,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                           PLAYER_CARD_INSTANCE(player, card).targets[0].card);
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

// FUNCTION: MAGIC 0x00475f9a
// FUNCTION: SHANDALAR 0x00479a85
int card_old_man_of_the_sea(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int legacy_card;
    int total_power;
    int opponent;
    int controlled_power;
    int current_card;
    int new_card;
  } s;

  if (event == EVENT_UNTAP && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~UNTAP_STATUS_WILL_UNTAP;
  }

  if (g_current_phase == PHASE_UNTAP &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (event == EVENT_TRIGGER &&
        (PLAYER_CARD_INSTANCE(player, card).untap_status & UNTAP_STATUS_COULD_UNTAP) != 0 &&
        (PLAYER_CARD_INSTANCE(player, card).untap_status & UNTAP_STATUS_WILL_UNTAP) == 0 &&
        (g_ai_score_baseline & global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type) == 0)
    {
      if ((g_other_player == player && (g_duel_network_flags & 2) == 0) ||
          g_duel_ai_mode_state == 1 ||
          g_duel_network_state != 0)
      {
        if (PLAYER_CARD_INSTANCE(player, card).damage_source_player == -1 ||
            PLAYER_CARD_INSTANCE(player, card).damage_source_card == -1 ||
            PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                    .internal_card_id != g_control_aura_special_internal_card_id ||
            PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                 PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                    .damage_target_player != player)
        {
          g_event_result |= RESOLVE_TRIGGER_MANDATORY;
        }
        else
        {
          s.total_power = 0;
          s.controlled_power =
              PLAYER_CARD_INSTANCE(
                  (int)PLAYER_CARD_INSTANCE(
                      (int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                      PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                      .damage_target_player,
                  PLAYER_CARD_INSTANCE(
                      (int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                      PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                      .damage_target_card)
                  .power;
          s.opponent = 1 - player;
          s.current_card = 0;
          while (s.current_card < g_active_cards_count[s.opponent] && g_event_result == 0)
          {
            if (is_in_play(s.opponent, s.current_card) != 0 &&
                (global_cards_data[PLAYER_CARD_INSTANCE(s.opponent, s.current_card).internal_card_id].type &
                 TYPE_CREATURE) != 0)
            {
              s.total_power += PLAYER_CARD_INSTANCE(s.opponent, s.current_card).power;
              if (PLAYER_CARD_INSTANCE(s.opponent, s.current_card).power <
                      PLAYER_CARD_INSTANCE(player, card).power &&
                  s.controlled_power < PLAYER_CARD_INSTANCE(s.opponent, s.current_card).power)
              {
                g_event_result |= RESOLVE_TRIGGER_MANDATORY;
              }
            }
            ++s.current_card;
          }
        }
      }
      else
      {
        g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      PLAYER_CARD_INSTANCE(player, card).untap_status |= UNTAP_STATUS_WILL_UNTAP;
    }
  }

  if (event == EVENT_CAST_SPELL && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = -1;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 1 - player,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1,
                                 TARGET_PT_LESSER_OR_EQUAL | (PLAYER_CARD_INSTANCE(player, card).power - 1),
                                 -1, 0, 0, 0);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      load_text("promptsX1.txt", "OLD_MAN_OF_THE_SEA");
      if (C_real_select_target(
              player, 2, 1 - player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
              get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
              -1, -1,
              TARGET_PT_LESSER_OR_EQUAL | (PLAYER_CARD_INSTANCE(player, card).power - 1),
              -1, 0, 0, 0, g_text_lines[0], 1, &s.target) != 0)
      {
        PLAYER_CARD_INSTANCE(player, card).targets[0] = s.target;
        PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
      else
      {
        g_spell_fizzled = 1;
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION &&
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
                .internal_card_id != -1)
    {
      SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (C_real_validate_target(
              s.target.player, s.target.card, (char *)0, player, 2, 2,
              TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
              get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
              -1, -1,
              TARGET_PT_LESSER_OR_EQUAL |
                  (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                        PLAYER_CARD_INSTANCE(player, card).parent_card)
                       .power -
                   1),
              -1, 0, 0, 0))
      {
        s.legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                             g_card_on_stack,
                                             g_control_aura_special_internal_card_id,
                                             s.target.player,
                                             s.target.card);
        if (s.legacy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_source_player = (char)-1;
          PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_source_card = -1;
          PLAYER_CARD_INSTANCE(player, s.legacy_card).targets[0].player = player;
          PLAYER_CARD_INSTANCE(player, s.legacy_card).targets[0].card = card;
          PLAYER_CARD_INSTANCE(player, s.legacy_card).number_of_targets = 1;
          PLAYER_CARD_INSTANCE(player, s.legacy_card).token_status |= STATUS_PERMANENT;
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                               PLAYER_CARD_INSTANCE(player, card).parent_card)
              .damage_source_player = (char)player;
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                               PLAYER_CARD_INSTANCE(player, card).parent_card)
              .damage_source_card = s.legacy_card;
          PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                               PLAYER_CARD_INSTANCE(player, card).parent_card)
              .info_slot = s.legacy_card;
          if (resolve_control_aura_conflict(player, s.legacy_card) != 0)
          {
            PLAYER_CARD_INSTANCE(player, s.legacy_card).token_status |= STATUS_CONTROLLED;
            if (s.target.player != player)
            {
              if (g_duel_ai_mode_state != 1)
              {
                play_sound_effect(WAV_CONTROL);
              }
              s.new_card = gain_control(
                  (int)PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_player,
                  PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_card);
              PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_player = (char)player;
              PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_card = s.new_card;
            }
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
    }

    if (g_trigger_condition == TRIGGER_LEAVE_PLAY &&
        g_affected_card == card &&
        g_affected_card_controller == player &&
        player == g_current_turn &&
        PLAYER_CARD_INSTANCE(player, card).info_slot != 0 &&
        PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1 &&
        PLAYER_CARD_INSTANCE(player, card).damage_source_card != -1 &&
        PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .internal_card_id == g_control_aura_special_internal_card_id &&
        g_trigger_cause_controller == player &&
        g_trigger_cause == card)
    {
      if (event == EVENT_TRIGGER)
      {
        g_event_result |= RESOLVE_TRIGGER_MANDATORY;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        kill_card((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                  PLAYER_CARD_INSTANCE(player, card).damage_source_card,
                  KILL_DESTROY);
      }
    }

    if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {
      if (g_affected_card == card &&
          g_affected_card_controller == player &&
          PLAYER_CARD_INSTANCE(player, card).info_slot != -1)
      {
        kill_card((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                  PLAYER_CARD_INSTANCE(player, card).damage_source_card,
                  KILL_DESTROY);
      }

      if (PLAYER_CARD_INSTANCE(player, card).damage_source_player == g_affected_card_controller &&
          PLAYER_CARD_INSTANCE(player, card).damage_source_card == g_affected_card)
      {
        PLAYER_CARD_INSTANCE(player, card).damage_source_card = -1;
        PLAYER_CARD_INSTANCE(player, card).damage_source_player =
            (char)PLAYER_CARD_INSTANCE(player, card).damage_source_card;
      }
    }

    if (PLAYER_CARD_INSTANCE(player, card).info_slot != -1 &&
        (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
    {
      kill_card((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                PLAYER_CARD_INSTANCE(player, card).damage_source_card,
                KILL_DESTROY);
      PLAYER_CARD_INSTANCE(player, card).info_slot = -1;
    }

    if (PLAYER_CARD_INSTANCE(player, card).info_slot != -1 &&
        event != EVENT_CHANGE_TYPE &&
        event != EVENT_TOUGHNESS &&
        PLAYER_CARD_INSTANCE(player, card).damage_source_player != -1 &&
        PLAYER_CARD_INSTANCE(player, card).damage_source_card != -1 &&
        PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .damage_target_player != -1 &&
        PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                             PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                .damage_target_card != -1)
    {
      s.opponent =
          (int)PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                    PLAYER_CARD_INSTANCE(player, card).damage_source_card)
              .damage_target_player;
      s.current_card =
          PLAYER_CARD_INSTANCE((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                               PLAYER_CARD_INSTANCE(player, card).damage_source_card)
              .damage_target_card;
      if (PLAYER_CARD_INSTANCE(player, card).power <=
              PLAYER_CARD_INSTANCE(s.opponent, s.current_card).power ||
          (global_cards_data[PLAYER_CARD_INSTANCE(s.opponent, s.current_card).internal_card_id].type &
           TYPE_CREATURE) == 0)
      {
        kill_card((int)PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                  PLAYER_CARD_INSTANCE(player, card).damage_source_card,
                  KILL_DESTROY);
        PLAYER_CARD_INSTANCE(player, card).info_slot = -1;
      }
    }
  }

  return 0;
}

// FUNCTION: MAGIC 0x00477791
// FUNCTION: SHANDALAR 0x0047b286
int card_orcish_mechanics(int player, int card, event_t event)
{
  target_t artifact;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) && own_artifact_available(player);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(1);
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "ORCISH_MECHANICS");
    if (C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE,
                             0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0,
                             g_text_lines[0], 1, &artifact) != 0)
    {
      strcpy(g_text_lines[0], g_text_lines[1]);
      if (select_damage_target(player, card, 2))
      {
        if (g_duel_ai_mode_state != 1)
        {
          play_sound_effect(WAV_SACRFICE);
        }
        kill_card(artifact.player, artifact.card, KILL_SACRIFICE);
        PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    deal_damage_to_selected_target(player, card, event, 2);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004779f6
// FUNCTION: SHANDALAR 0x0047b4eb
int card_phyrexian_gremlins(int player, int card, event_t event)
{
  struct
  {
    target_t target;
    int legacy_card;
    int opponent;
    int target_card;
    unsigned int target_is_creature;
    int target_power;
    int target_internal_card_id;
    int current_card;
    int target_mana_value;
    int target_player;
    int candidate_internal_card_id;
  } s;

  if (event == EVENT_UNTAP &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).untap_status &= ~UNTAP_STATUS_WILL_UNTAP;
  }

  if (g_current_phase == PHASE_UNTAP &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    if (event == EVENT_TRIGGER &&
        (PLAYER_CARD_INSTANCE(player, card).untap_status & UNTAP_STATUS_COULD_UNTAP) != 0 &&
        (PLAYER_CARD_INSTANCE(player, card).untap_status & UNTAP_STATUS_WILL_UNTAP) == 0 &&
        (g_ai_score_baseline & global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type) == 0)
    {
      if ((g_other_player == player && (g_duel_network_flags & 2) == 0) ||
          g_duel_ai_mode_state == 1 ||
          g_duel_network_state != 0)
      {
        s.legacy_card = PLAYER_CARD_INSTANCE(player, card).info_slot;
        if (s.legacy_card == -1 ||
            (PLAYER_CARD_INSTANCE(
                 (int)PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_player,
                 PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_card)
                 .untap_status &
             UNTAP_STATUS_COULD_UNTAP) != 0 ||
            (PLAYER_CARD_INSTANCE(
                 (int)PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_player,
                 PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_card)
                 .state &
             STATE_TAPPED) == 0)
        {
          g_event_result |= RESOLVE_TRIGGER_MANDATORY;
        }
        else
        {
          s.target_player =
              (int)PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_player;
          s.target_card = PLAYER_CARD_INSTANCE(player, s.legacy_card).damage_target_card;
          s.target_internal_card_id =
              PLAYER_CARD_INSTANCE(s.target_player, s.target_card).internal_card_id;
          s.target_is_creature =
              global_cards_data[s.target_internal_card_id].type & TYPE_CREATURE;
          if (s.target_is_creature == 0)
          {
            s.target_power = 0;
          }
          else
          {
            s.target_power = PLAYER_CARD_INSTANCE(s.target_player, s.target_card).power;
          }
          s.target_mana_value = global_cards_data[s.target_internal_card_id].cc[1];
          s.opponent = 1 - player;
          s.current_card = 0;
          while (s.current_card < g_active_cards_count[s.opponent] && g_event_result == 0)
          {
            if (is_in_play(s.opponent, s.current_card) &&
                (global_cards_data[PLAYER_CARD_INSTANCE(s.opponent, s.current_card).internal_card_id].type &
                 (TYPE_ARTIFACT | TYPE_CREATURE)) == (TYPE_ARTIFACT | TYPE_CREATURE) &&
                s.target_power < PLAYER_CARD_INSTANCE(s.opponent, s.current_card).power)
            {
              g_event_result |= RESOLVE_TRIGGER_MANDATORY;
            }
            ++s.current_card;
          }
          s.current_card = 0;
          while (s.current_card < g_active_cards_count[s.opponent] && g_event_result == 0)
          {
            s.candidate_internal_card_id =
                PLAYER_CARD_INSTANCE(s.opponent, s.current_card).internal_card_id;
            if (is_in_play(s.opponent, s.current_card) &&
                (global_cards_data[s.candidate_internal_card_id].type & TYPE_ARTIFACT) != 0 &&
                s.target_mana_value < global_cards_data[s.candidate_internal_card_id].cc[1])
            {
              g_event_result |= RESOLVE_TRIGGER_MANDATORY;
            }
            ++s.current_card;
          }
        }
      }
      else
      {
        g_event_result |= RESOLVE_TRIGGER_OPTIONAL;
      }
    }

    if (event == EVENT_RESOLVE_TRIGGER)
    {
      PLAYER_CARD_INSTANCE(player, card).untap_status |= UNTAP_STATUS_WILL_UNTAP;
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    return CAN_TAP(player, card) &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "PHYREXIAN_GREMLINS");
    if (C_real_select_target(player, 2, 2, TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, 0, 0, 0,
                             g_text_lines[0], 1, &s.target) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0] = s.target;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      if (player == g_other_player &&
          (g_duel_network_flags & 2) == 0 &&
          s.target.player == g_other_player)
      {
        g_ai_modifier -= 0x30;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_CAST_SPELL &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = -1;
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(s.target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(s.target.player, s.target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      s.legacy_card = create_legacy_effect(g_card_on_stack_controller,
                                           g_card_on_stack,
                                           g_duel_generated_iid_21,
                                           s.target.player,
                                           s.target.card);
      if (s.legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(g_card_on_stack_controller, s.legacy_card).token_status |=
            STATUS_PERMANENT;
        PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                             PLAYER_CARD_INSTANCE(player, card).parent_card)
            .info_slot = s.legacy_card;
        PLAYER_CARD_INSTANCE(s.target.player, s.target.card).state |= STATE_TAPPED;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot != -1)
  {
    kill_card(player, PLAYER_CARD_INSTANCE(player, card).info_slot, KILL_BURY);
  }

  if (PLAYER_CARD_INSTANCE(player, card).info_slot != -1 &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    s.legacy_card = PLAYER_CARD_INSTANCE(player, card).info_slot;
    kill_card(player, s.legacy_card, KILL_BURY);
    PLAYER_CARD_INSTANCE(player, s.legacy_card).info_slot = -1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00478606
// FUNCTION: SHANDALAR 0x0047c106
int card_priest_of_yawgmoth(int player, int card, event_t event)
{
  target_t artifact;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 player | 4,
                                 player | 4,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_ARTIFACT,
                                 TYPE_NONE,
                                 0,
                                 0,
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 -1,
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

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "PRIEST_OF_YAWGMOTH");
    if (C_real_select_target(player,
                             player | 4,
                             player | 4,
                             TARGET_ZONE_IN_PLAY,
                             TYPE_ARTIFACT,
                             TYPE_NONE,
                             0,
                             0,
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
                             0,
                             &artifact))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], artifact);
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      kill_card(artifact.player, artifact.card, KILL_SACRIFICE);
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    produce_mana(
        player,
        get_sleighted_color(player, card, COLOR_BLACK),
        (int)global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                                    PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                                   .original_internal_card_id]
                .cc[0] +
            (int)global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                                        PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                                       .original_internal_card_id]
                .cc[1]);
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047894c
// FUNCTION: SHANDALAR 0x0047c44c
int card_rock_hydra(int player, int card, event_t event)
{
  int *piVar1;
  unsigned int uVar3;
  int iVar4;
  unsigned int damage_prevented;
  target_t selected_damage;
  int legacy_card;
  int valid_target_selected;

  if (((event == EVENT_CAST_SPELL) && (g_affected_card == card)) && (g_affected_card_controller == player))
  {
    set_special_counters(player, card, g_x_value);
    if (g_other_player == player)
    {
      g_ai_modifier += (g_x_value + g_x_value * 2 - 9) << 3;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    legacy_card = create_legacy_effect(player, card, g_duel_generated_internal_card_id_1a, player, card);
    if (legacy_card != -1)
    {
      uVar3 = C_get_special_counters(player, card);
      PLAYER_CARD_INSTANCE(player, legacy_card).info_slot = (uVar3 << 8) | uVar3;
      PLAYER_CARD_INSTANCE(player, legacy_card).eot_toughness = 0x40c;
      PLAYER_CARD_INSTANCE(player, legacy_card).token_status = 0x10020;
      PLAYER_CARD_INSTANCE(player, card).damage_source_player = (char)player;
      PLAYER_CARD_INSTANCE(player, card).damage_source_card = legacy_card;
    }
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((g_land_can_be_played & 4) == 0) || ((iVar4 = has_mana(player, 4, 1)) == 0)) || (((uVar3 = C_get_special_counters(player, card)) == 0) || ((iVar4 = has_effect_source_type(player, card, 0xffffffff)) == 0)))
    {
      if (((g_current_phase == 4) && (player == g_current_player) && (player == g_event_player)) && ((iVar4 = has_mana(player, 4, 3)) != 0))
      {
        if (((player == g_other_player) && ((g_duel_network_flags & 2) == 0)) && (g_duel_summary.hand_counts[player] - *(int *)((char *)g_basiclandtypes_controlled + player * 0x20 + 0x10) == 3))
        {
          g_activation_event_flags |= 3;
        }
        iVar4 = 1;
      }
      else
      {
        iVar4 = 0;
      }
    }
    else
    {
      iVar4 = 99;
    }
  }
  else if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    iVar4 = 0;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      if ((g_land_can_be_played & 4) == 0)
      {
        charge_mana(player, 4, 3);
        if (g_spell_fizzled != 1)
        {
          PLAYER_CARD_INSTANCE(player, card).targets[0].player = player;
          PLAYER_CARD_INSTANCE(player, card).targets[0].card = card;
          PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
        }
      }
      else
      {
        charge_mana(player, 4, 1);
        if (g_spell_fizzled != 1)
        {
          valid_target_selected = 0;
          do
          {
            load_text("promptsX1.txt", "ROCK_HYDRA");
            iVar4 = C_real_select_target(player,
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
                                         0,
                                         g_text_lines[0],
                                         1,
                                         &selected_damage);
            if (iVar4 == 0)
            {
              g_spell_fizzled = 1;
            }
            else if (PLAYER_CARD_INSTANCE(selected_damage.player, selected_damage.card).damage_source_player == player && PLAYER_CARD_INSTANCE(selected_damage.player, selected_damage.card).damage_source_card == card)
            {
              SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_damage);
              PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
              valid_target_selected = 1;
            }
            else if (g_duel_ai_mode_state == 1)
            {
              rewind_recorded_action();
            }
            else
            {
              set_duel_prompt_text(g_text_lines[1]);
              Sleep(0x9c4);
              set_duel_prompt_text("");
            }
          } while ((g_spell_fizzled != 1) && (valid_target_selected == 0));
        }
      }
    }

    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                               PLAYER_CARD_INSTANCE(player, card).parent_card)
              .internal_card_id != -1)
      {
        if ((g_land_can_be_played & 4) == 0)
        {
          add_special_counter(g_card_on_stack_controller, g_card_on_stack);
          piVar1 = &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                         PLAYER_CARD_INSTANCE(player, card).parent_card)
                        .info_slot;
          *piVar1 += 0x101;
        }
        else
        {
          uVar3 = C_real_validate_target(
              PLAYER_CARD_INSTANCE(player, card).targets[0].player,
              PLAYER_CARD_INSTANCE(player, card).targets[0].card,
              (char *)0,
              player,
              ANYBODY,
              ANYBODY,
              TARGET_ZONE_IN_PLAY,
              TYPE_NONE,
              TYPE_NONE,
              0,
              0,
              COLOR_TEST_0,
              COLOR_TEST_0,
              g_damage_card_internal_card_id,
              ~SUB_WALL,
              -1,
              -1,
              0,
              0,
              0);
          if (uVar3 == 0)
          {
            g_spell_fizzled = 1;
          }
          else if (0 < PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                            PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                           .info_slot)
          {
            piVar1 = &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                           PLAYER_CARD_INSTANCE(player, card).targets[0].card)
                          .info_slot;
            *piVar1 += -1;
          }
        }
      }

      PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                           PLAYER_CARD_INSTANCE(player, card).parent_card)
          .number_of_targets = 0;
    }

    if ((((event == EVENT_DEAL_DAMAGE) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).internal_card_id == g_damage_card_internal_card_id)) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_card == card)) && ((PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).damage_source_player == player) && (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot != 0)))
    {
      uVar3 = C_get_special_counters(player, card);
      if (PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot < (int)uVar3)
      {
        damage_prevented = PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot;
      }
      else
      {
        damage_prevented = C_get_special_counters(player, card);
      }
      piVar1 = &PLAYER_CARD_INSTANCE(g_affected_card_controller, g_affected_card).info_slot;
      *piVar1 -= damage_prevented;
      remove_special_counters(player, card, damage_prevented);
      piVar1 = &PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).damage_source_player,
                                     PLAYER_CARD_INSTANCE(player, card).damage_source_card)
                    .info_slot;
      *piVar1 -= (damage_prevented << 8) | damage_prevented;
    }
    iVar4 = 0;
  }
  return iVar4;
}

// FUNCTION: MAGIC 0x004794d8
// FUNCTION: SHANDALAR 0x0047cfdc
int card_rukh_egg(int player, int card, event_t event)
{
  if (event == EVENT_GRAVEYARD_FROM_PLAY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0 &&
      PLAYER_CARD_INSTANCE(player, card).kill_code != KILL_REMOVE)
  {
    create_legacy_effect(player, card, g_duel_generated_internal_card_id_1d, -1, -1);
    g_ai_modifier += 0x60;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004a0403
int select_and_sacrifice_artifact(int player)
{
  target_t target;

  if (C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY,
                           TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                           COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                           0, 0, 0, g_text_lines[0], 0, &target))
  {
    if (g_duel_ai_mode_state != 1)
    {
      play_sound_effect(WAV_SACRFICE);
    }
    kill_card(target.player, target.card, KILL_SACRIFICE);
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x004795a4
// FUNCTION: SHANDALAR 0x0047d0aa
int card_sage_of_lat_nam(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player,
                                 TARGET_ZONE_IN_PLAY, TYPE_ARTIFACT, TYPE_NONE, 0, 0,
                                 COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1, 0, 0, 0);
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "SAGE_OF_LAT_NAM");
    if (select_and_sacrifice_artifact(player) != 0)
    {
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_card_for_player(player);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004796cc
// FUNCTION: SHANDALAR 0x0047d1d2
int card_sedge_troll(int player, int card, event_t event)
{
  int color;

  if (card == g_affected_card && player == g_affected_card_controller)
  {
    if (g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLACK)] > 0)
    {
      if (event == EVENT_POWER)
      {
        ++g_event_result;
      }
      if (event == EVENT_TOUGHNESS)
      {
        ++g_event_result;
      }
    }
  }

  if (event == EVENT_UNTAP_PHASE)
  {
    color = get_hacked_color(player, card, COLOR_BLACK);
    ((int *)((char *)g_ai_mana_demand_by_color + player * 0x20))[color] += 2;
  }

  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
  {
    return generic_regeneration_ability(player, card, event, 1, 1);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x00487ce0
int select_ai_land_to_sacrifice(int player, target_t *target, unsigned int avoid_color_mask,
                                unsigned int prefer_color_mask)
{
  int current_card;
  int current_attachment;
  int internal_card_id;
  int max_active_cards;
  int opponent;
  int score;
  int best_score;
  int least_color;
  int least_color_count;
  int color;
  int found;

  best_score = 999;
  found = 0;
  opponent = 1 - player;

  for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
  {
    internal_card_id = PLAYER_CARD_INSTANCE(player, current_card).internal_card_id;
    if (is_in_play(player, current_card) &&
        (global_cards_data[internal_card_id].type & TYPE_LAND) != 0)
    {
      score = global_cards_data[internal_card_id].subtype == SUB_DUAL_LAND;
      if (internal_card_id > 4 && internal_card_id < g_card_count)
      {
        ++score;
      }

      max_active_cards = g_active_cards_count[player];
      if (max_active_cards <= g_active_cards_count[opponent])
      {
        max_active_cards = g_active_cards_count[opponent];
      }
      for (current_attachment = 0; current_attachment < max_active_cards; ++current_attachment)
      {
        if (is_in_play(opponent, current_attachment) &&
            PLAYER_CARD_INSTANCE(opponent, current_attachment).damage_target_player == player &&
            PLAYER_CARD_INSTANCE(opponent, current_attachment).damage_target_card == current_card)
        {
          --score;
        }
        if (is_in_play(player, current_attachment) &&
            PLAYER_CARD_INSTANCE(player, current_attachment).damage_target_player == player &&
            PLAYER_CARD_INSTANCE(player, current_attachment).damage_target_card == current_card)
        {
          ++score;
        }
      }

      least_color = -1;
      least_color_count = 999;
      for (color = COLOR_BLACK; color < COLOR_BLUE; ++color)
      {
        if (((1 << (unsigned char)color) & PLAYER_CARD_INSTANCE(player, current_card).mana_color) != 0 &&
            g_basiclandtypes_controlled[player][color] < least_color_count)
        {
          least_color_count = g_basiclandtypes_controlled[player][color];
          least_color = color;
        }
      }
      if (least_color != -1)
      {
        score += 2 - g_basiclandtypes_controlled[player][least_color];
      }
      if ((avoid_color_mask & PLAYER_CARD_INSTANCE(player, current_card).mana_color) != 0)
      {
        score -= 10;
      }
      if ((prefer_color_mask & PLAYER_CARD_INSTANCE(player, current_card).mana_color) != 0)
      {
        score += 10;
      }
      if (score < best_score)
      {
        best_score = score;
        target->player = player;
        target->card = current_card;
        found = 1;
      }
    }
  }

  return found;
}

// FUNCTION: MAGIC 0x004797ae
// FUNCTION: SHANDALAR 0x0047d2b6
int card_serendib_djinn(int player, int card, event_t event)
{
  target_t land;
  int current_card;

  if (card == g_affected_card &&
      player == g_affected_card_controller &&
      g_basiclandtypes_controlled[player][COLOR_ANY] == 0)
  {
    kill_card(player, card, KILL_DESTROY);
  }

  if (event == EVENT_CAST_SPELL &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      g_basiclandtypes_controlled[player][COLOR_ANY] < 2)
  {
    g_ai_modifier -= 0x60;
  }

  if (event == EVENT_CHECK_UPK_PAYMENT && g_basiclandtypes_controlled[player][COLOR_ANY] == 0)
  {
    g_event_result |= 1;
  }

  if (event == EVENT_SETUP_UPKEEP_COSTS &&
      card == g_affected_card &&
      player == g_affected_card_controller &&
      PLAYER_CARD_INSTANCE(player, card).info_slot == 0 &&
      player == g_current_player &&
      player == g_event_player)
  {
    PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= UPKEEP_UPKEEP_TRIGGER | UPKEEP_UPKEEP_NODIALOG;
  }

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    if (g_basiclandtypes_controlled[player][COLOR_ANY] != 0)
    {
      if (player == g_other_player && (g_duel_network_flags & 2) == 0)
      {
        select_ai_land_to_sacrifice(player, &land, 0, 1 << (unsigned char)get_hacked_color(player, card, COLOR_BLUE));
      }
      else
      {
        load_text("promptsX1.txt", "SERENDIB_DJINN");
        C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY,
                             TYPE_LAND, TYPE_NONE, 0, 0,
                             COLOR_TEST_0, COLOR_TEST_0, -1, -1, -1, -1,
                             0, 0, 0, g_text_lines[0], 0, &land);
      }
      if (g_duel_ai_mode_state != 1)
      {
        play_sound_effect(WAV_SACRFICE);
      }
      if (card_has_basic_land_type(land.player, land.card, get_hacked_color(player, card, COLOR_BLUE)) != 0)
      {
        damage_player(player, 3, player, card);
      }
      kill_card(land.player, land.card, KILL_SACRIFICE);
    }
    else
    {
      g_event_result |= 1;
    }
  }

  if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) &&
      card == g_affected_card &&
      player == g_affected_card_controller)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  if (event == EVENT_SHOULD_AI_PLAY &&
      g_current_phase == 0x1f &&
      player != g_current_player &&
      player == g_other_player)
  {
    for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) &&
          (global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].type & TYPE_LAND) != 0)
      {
        kill_card(player, current_card, KILL_DESTROY);
      }
    }

    if (g_basiclandtypes_controlled[player][get_hacked_color(player, card, COLOR_BLUE)] == g_basiclandtypes_controlled[player][COLOR_ANY])
    {
      damage_player(player, 3, player, card);
    }
  }

  if (event == EVENT_ATTACK_RATING && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_score -= 0x24;
  }

  if (event == EVENT_BLOCK_RATING && card == g_affected_card && player == g_affected_card_controller)
  {
    g_ai_score += 0x24;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00479c7b
// FUNCTION: SHANDALAR 0x0047d770
int card_serendib_efreet(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == PHASE_UPKEEP &&
        g_current_player == g_event_player &&
        player == g_current_player &&
        (PLAYER_CARD_INSTANCE(player, card).info_slot & 1) == 0)
    {
      PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= UPKEEP_UPKEEP_TRIGGER | UPKEEP_UPKEEP_NODIALOG;
      g_activation_event_flags |= 3;
      return 1;
    }
    return 0;
  }

  if (event == EVENT_UPKEEP_PHASE && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot |= 1;
    g_upkeep_payment_completed = 1;
    g_event_result |= 1;
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    damage_player(player, 1, g_card_on_stack_controller, g_card_on_stack);
  }

  if (event == EVENT_SHOULD_AI_PLAY &&
      g_current_phase == 0x1f &&
      player != g_current_player)
  {
    damage_player(player, 1, player, card);
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot &= ~1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x00479e4c
// FUNCTION: SHANDALAR 0x0047d940
int card_singing_tree(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & (STATE_TAPPED | STATE_SUMMONSICK_NOTAP)) == 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 1 - player,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, TARGET_STATE_ATTACKING, 0);
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "SINGING_TREES");
    if (C_real_select_target(player, 2, 1 - player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, 0, TARGET_STATE_ATTACKING, 0,
                             g_text_lines[0], 1, &target) != 0)
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

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, TARGET_STATE_ATTACKING, 0) != 0)
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack,
                                         LEGACY_EFFECT_PUMP, target.player, target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_power =
            -C_get_abilities(target.player, target.card, EVENT_POWER, -1);
      }
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

// FUNCTION: MAGIC 0x0047a22a
// FUNCTION: SHANDALAR 0x0047dd1e
int card_two_headed_giant_of_foriys(int player, int card, event_t event)
{
  int giant_copy_internal_id;
  int giant_copy_card;
  int current_card;

  if (g_trigger_condition == 0xdf && g_affected_card == card && g_affected_card_controller == player && g_current_turn == player && player != g_current_player && player == g_trigger_cause_controller && card == g_trigger_cause)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e)
    {
      giant_copy_internal_id = create_a_card_type(PLAYER_CARD_INSTANCE(player, card).internal_card_id);
      if (giant_copy_internal_id != -1)
      {
        global_cards_data[giant_copy_internal_id].code_pointer = card_two_headed_giant_of_foriys_legacy;
        global_cards_data[giant_copy_internal_id].extra_ability = 0;
        global_cards_data[giant_copy_internal_id].id = g_multiblock_display_internal_card_id;

        giant_copy_card = add_card_to_hand(player, giant_copy_internal_id);
        if (giant_copy_card != -1)
        {
          PLAYER_CARD_INSTANCE(player, giant_copy_card).state = PLAYER_CARD_INSTANCE(player, card).state & 0xfffffff7;
          PLAYER_CARD_INSTANCE(player, giant_copy_card).regen_status = PLAYER_CARD_INSTANCE(player, card).regen_status;
          PLAYER_CARD_INSTANCE(player, giant_copy_card).token_status = 0x8000008;
          PLAYER_CARD_INSTANCE(player, giant_copy_card).info_slot = 2;
          PLAYER_CARD_INSTANCE(player, giant_copy_card).damage_source_player = (char)player;
          PLAYER_CARD_INSTANCE(player, giant_copy_card).damage_source_card = card;
          PLAYER_CARD_INSTANCE(player, giant_copy_card).display_pic_info =
              global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].id;
        }
      }
    }
  }

  if (g_trigger_condition == 0xd4 && g_affected_card == card && g_affected_card_controller == player && g_current_turn == player && player == g_trigger_cause_controller && card == g_trigger_cause)
  {
    if (event == 0x7d)
    {
      g_event_result |= 2;
    }
    if (event == 0x7e && PLAYER_CARD_INSTANCE(player, card).blocking != -1)
    {
      for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
      {
        if (is_in_play(player, current_card) && global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].code_pointer == card_two_headed_giant_of_foriys_legacy && PLAYER_CARD_INSTANCE(player, current_card).damage_source_player == player && PLAYER_CARD_INSTANCE(player, current_card).damage_source_card == card)
        {
          kill_card(player, current_card, KILL_DESTROY);
        }
      }
    }
  }

  if (event == 0x77 && g_affected_card == card && g_affected_card_controller == player && PLAYER_CARD_INSTANCE(player, card).blocking != -1)
  {
    for (current_card = 0; current_card < g_active_cards_count[player]; ++current_card)
    {
      if (is_in_play(player, current_card) && global_cards_data[PLAYER_CARD_INSTANCE(player, current_card).internal_card_id].code_pointer == card_two_headed_giant_of_foriys_legacy && PLAYER_CARD_INSTANCE(player, current_card).damage_source_player == player && PLAYER_CARD_INSTANCE(player, current_card).damage_source_card == card)
      {
        kill_card(player, current_card, KILL_DESTROY);
      }
    }
  }

  return 0;
}

static int C_vesuvan_doppelganger_helper(int source_player,
                                         int source_card,
                                         int player,
                                         int card,
                                         int internal_card_id);

// FUNCTION: MAGIC 0x0047a847
// FUNCTION: SHANDALAR 0x0047e344
int card_vesuvan_doppelganger(int player, int card, event_t event)
{
  target_t selected_target;
  int data_card_location;
  int data_card_controller;
  int data_card_slot;
  int target_data_card_location;
  int source_internal_card_id;
  int copied_internal_id;
  int lookup_player;
  int lookup_card;
  int change_form;
  int delegate_to_copy;
  short copied_power_modifier;
  short copied_toughness_modifier;
  unsigned int counter_count;
  char dialog[900];
  unsigned int result;

  lookup_player = player;
  lookup_card = card;
  data_card_location = -1;
  data_card_controller = -1;
  data_card_slot = -1;
  delegate_to_copy = 0;
  result = 0;

  if (event == EVENT_RESOLVE_ACTIVATION && PLAYER_CARD_INSTANCE(player, card).parent_controller != -1 && PLAYER_CARD_INSTANCE(player, card).parent_card != -1)
  {
    lookup_player = PLAYER_CARD_INSTANCE(player, card).parent_controller;
    lookup_card = PLAYER_CARD_INSTANCE(player, card).parent_card;
  }

  if (is_in_play(lookup_player, lookup_card))
  {
    if (PLAYER_CARD_INSTANCE(lookup_player, lookup_card).internal_card_id == g_stack_proxy_internal_card_id)
    {
      data_card_location = dispatch_function_to_all_cards_in_play(g_card_on_stack_controller,
                                                                  g_card_on_stack,
                                                                  clone_find_data_card_callback,
                                                                  -1);
    }
    else
    {
      data_card_location = dispatch_function_to_all_cards_in_play(lookup_player,
                                                                  lookup_card,
                                                                  clone_find_data_card_callback,
                                                                  -1);
    }
    if (data_card_location != -1)
    {
      data_card_controller = (data_card_location >> 8) & 0xff;
      data_card_slot = data_card_location & 0xff;
      delegate_to_copy = 1;
    }
  }

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0,
                                 TARGET_SCAN_DIRECT,
                                 player,
                                 2,
                                 2,
                                 0x200,
                                 TYPE_CREATURE,
                                 0,
                                 0,
                                 get_protections_from(player, card),
                                 0,
                                 0,
                                 -1,
                                 -1,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                                 0,
                                 0);
  }

  if (event == EVENT_CAST_SPELL && card == g_affected_card && player == g_affected_card_controller)
  {
    load_text("promptsX1.txt", "VESUVAN_DOPPELGANGER");
    if (C_real_select_target(player,
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
                             TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                             0,
                             0,
                             g_text_lines[0],
                             1,
                             &selected_target))
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      g_spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                               PLAYER_CARD_INSTANCE(player, card).targets[0].card,
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
                               TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                               0,
                               0))
    {
      SET_TARGET(selected_target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
      if (global_cards_data[PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).internal_card_id].id == CARD_ID_VESUVAN_DOPPELGANGER)
      {
        target_data_card_location =
            dispatch_function_to_all_cards_in_play(selected_target.player, selected_target.card, clone_find_data_card_callback, -1);
        if (target_data_card_location == -1)
        {
          source_internal_card_id = 0;
        }
        else
        {
          source_internal_card_id =
              PLAYER_CARD_INSTANCE((target_data_card_location >> 8) & 0xff, target_data_card_location & 0xff)
                  .info_slot;
        }
      }
      else
      {
        source_internal_card_id = PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card).internal_card_id;
      }

      data_card_slot = add_card_to_hand(player, find_internal_card_id_by_csv_id(CARD_ID_DATA_CARD));
      if (data_card_slot != -1)
      {
        data_card_controller = player;
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).state = ((((unsigned int)(((player == 0) - 1) >> 8)) & 0x1000) | STATE_IN_PLAY);
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).token_status |= 0x10008;
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).damage_target_player = (char)player;
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).damage_target_card = card;
        copied_internal_id = create_a_card_type(source_internal_card_id);
        if (copied_internal_id != -1)
        {
          PLAYER_CARD_INSTANCE(player, card).internal_card_id = copied_internal_id;
          PLAYER_CARD_INSTANCE(player, card).dummy3 = copied_internal_id;
          PLAYER_CARD_INSTANCE(player, card).regen_status |= 0x1000000;
          PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_VIGILANCE;
          PLAYER_CARD_INSTANCE(player, card).color = global_cards_data[copied_internal_id].color;
          if ((global_cards_data[copied_internal_id].type & TYPE_ARTIFACT) != 0)
          {
            ++g_duel_summary.artifact_counts[player];
          }
          if ((global_cards_data[copied_internal_id].type & TYPE_ENCHANTMENT) != 0)
          {
            ++g_duel_summary.enchantment_counts[player];
          }
          g_card_types_in_play[player] |= global_cards_data[copied_internal_id].type;
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).info_slot = source_internal_card_id;
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness |= global_cards_data[source_internal_card_id].extra_ability & 1;
          dispatch_event_to_single_card(player, card, EVENT_CAST_SPELL, 1 - player, -1);
          dispatch_event_to_single_card(player, card, EVENT_RESOLVE_SPELL, 1 - player, -1);
          global_cards_data[copied_internal_id].code_pointer =
              global_cards_data[PLAYER_CARD_INSTANCE(player, card).original_internal_card_id].code_pointer;
          global_cards_data[copied_internal_id].extra_ability |= 1;
        }
      }
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    }
    else
    {
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
      kill_card(player, card, KILL_DESTROY);
    }
  }

  if (event == EVENT_CHANGE_TYPE && (g_land_can_be_played & 0x20000) == 0 && card == g_affected_card && player == g_affected_card_controller && is_in_play(player, card))
  {
    g_event_result = PLAYER_CARD_INSTANCE(player, card).dummy3;
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (g_current_phase == EVENT_UPKEEP_PHASE && player == g_current_player && player == g_event_player && data_card_controller != -1 && data_card_slot != -1 && (PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness & 2) == 0 && real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2, 0x200, TYPE_CREATURE, 0, 0, get_protections_from(player, card), 0, 0, -1, -1, -1, -1, TARGET_SPECIAL_USE_ORIGINAL_TYPE, 0, 0))
    {
      result = 1;
    }
  }

  if (event == EVENT_ACTIVATE && card == g_affected_card && player == g_affected_card_controller && g_current_phase == EVENT_UPKEEP_PHASE && player == g_current_player && player == g_event_player && data_card_controller != -1 && data_card_slot != -1 && (PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness & 2) == 0)
  {
    change_form = 1;
    load_text("promptsX1.txt", "VESUVAN_DOPPELGANGER");
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_OUBLIETTED) == 0 &&
        (PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness & 1) != 0 &&
        PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).info_slot != 0 &&
        global_cards_data[PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).info_slot].code_pointer(player, card, EVENT_CAN_ACTIVATE) != 0)
    {
      sprintf(dialog, " %s\n %s", g_text_lines[1], g_text_lines[2]);
      if (do_dialog(player, player, card, -1, -1, dialog, 0) != 0)
      {
        change_form = 0;
      }
    }
    if (change_form &&
        !C_real_select_target(player,
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
                              TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                              0,
                              0,
                              g_text_lines[0],
                              1,
                              &selected_target))
    {
      g_spell_fizzled = 1;
    }
    else if (change_form)
    {
      SET_TARGET(PLAYER_CARD_INSTANCE(player, card).targets[0], selected_target);
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
      PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness |= 6;
      delegate_to_copy = 0;
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION && (lookup_player != player || lookup_card != card))
  {
    if (data_card_controller != -1 && data_card_slot != -1 && (PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness & 4) != 0)
    {
      if (C_real_validate_target(PLAYER_CARD_INSTANCE(player, card).targets[0].player,
                                 PLAYER_CARD_INSTANCE(player, card).targets[0].card,
                                 (char *)0,
                                 player,
                                 2,
                                 2,
                                 TARGET_ZONE_IN_PLAY,
                                 TYPE_CREATURE,
                                 TYPE_NONE,
                                 0,
                                 get_protections_from(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card),
                                 COLOR_TEST_0,
                                 COLOR_TEST_0,
                                 -1,
                                 ~SUB_WALL,
                                 -1,
                                 -1,
                                 TARGET_SPECIAL_USE_ORIGINAL_TYPE,
                                 0,
                                 0))
      {
        if (global_cards_data[PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card).internal_card_id].id == CARD_ID_VESUVAN_DOPPELGANGER)
        {
          target_data_card_location =
              dispatch_function_to_all_cards_in_play(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card, clone_find_data_card_callback, -1);
          if (target_data_card_location == -1)
          {
            source_internal_card_id = 0;
          }
          else
          {
            source_internal_card_id =
                PLAYER_CARD_INSTANCE((target_data_card_location >> 8) & 0xff, target_data_card_location & 0xff)
                    .info_slot;
          }
        }
        else
        {
          source_internal_card_id = PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).targets[0].player, PLAYER_CARD_INSTANCE(player, card).targets[0].card).internal_card_id;
        }

        copied_internal_id = create_a_card_type(source_internal_card_id);
        if (copied_internal_id != -1)
        {
          PLAYER_CARD_INSTANCE(lookup_player, lookup_card).internal_card_id = copied_internal_id;
          PLAYER_CARD_INSTANCE(lookup_player, lookup_card).dummy3 = copied_internal_id;
          PLAYER_CARD_INSTANCE(lookup_player, lookup_card).regen_status |= 0x1000000;
          PLAYER_CARD_INSTANCE(lookup_player, lookup_card).state &= ~STATE_VIGILANCE;
          PLAYER_CARD_INSTANCE(lookup_player, lookup_card).color = global_cards_data[copied_internal_id].color;
          set_special_counters(g_card_on_stack_controller, g_card_on_stack, 0);
          copied_toughness_modifier = 0;
          copied_power_modifier = 0;
          counter_count = (PLAYER_CARD_INSTANCE(lookup_player, lookup_card).special_counters >> 8) & 0xff;
          if (counter_count != 0)
          {
            copied_toughness_modifier = (short)(counter_count * -2);
          }
          counter_count = (PLAYER_CARD_INSTANCE(lookup_player, lookup_card).special_counters >> 16) & 0xff;
          if (counter_count != 0)
          {
            copied_power_modifier = (short)-(short)counter_count;
            copied_toughness_modifier -= (short)counter_count;
          }
          counter_count = PLAYER_CARD_INSTANCE(lookup_player, lookup_card).special_counters >> 24;
          if (counter_count != 0)
          {
            copied_toughness_modifier -= (short)(unsigned char)counter_count;
          }
          counter_count = PLAYER_CARD_INSTANCE(lookup_player, lookup_card).counters & 0xff;
          if (counter_count != 0)
          {
            copied_power_modifier += (short)counter_count;
            copied_toughness_modifier += (short)counter_count;
          }
          counter_count = (PLAYER_CARD_INSTANCE(lookup_player, lookup_card).counters >> 8) & 0xff;
          if (counter_count != 0)
          {
            copied_power_modifier += (short)counter_count;
            copied_toughness_modifier += (short)counter_count;
          }
          PLAYER_CARD_INSTANCE(lookup_player, lookup_card).counter_power = copied_power_modifier;
          PLAYER_CARD_INSTANCE(lookup_player, lookup_card).counter_toughness = copied_toughness_modifier;
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).info_slot = source_internal_card_id;
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness &= ~1;
          PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness |= global_cards_data[source_internal_card_id].extra_ability & 1;
          dispatch_function_to_all_cards_in_play(g_card_on_stack_controller,
                                                 g_card_on_stack,
                                                 C_vesuvan_doppelganger_helper,
                                                 -1);
          dispatch_event_to_single_card(g_card_on_stack_controller,
                                        g_card_on_stack,
                                        EVENT_CAST_SPELL,
                                        1 - PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                        -1);
          dispatch_event_to_single_card(g_card_on_stack_controller,
                                        g_card_on_stack,
                                        EVENT_RESOLVE_SPELL,
                                        1 - PLAYER_CARD_INSTANCE(player, card).parent_controller,
                                        -1);
          global_cards_data[copied_internal_id].code_pointer =
              global_cards_data[PLAYER_CARD_INSTANCE(lookup_player, lookup_card).original_internal_card_id].code_pointer;
          global_cards_data[copied_internal_id].extra_ability |= 1;
        }
      }

      PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness &= ~4;
      delegate_to_copy = 0;
    }

    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller, PLAYER_CARD_INSTANCE(player, card).parent_card).number_of_targets = 0;
  }

  if (event == EVENT_CLEANUP && data_card_controller != -1 && data_card_slot != -1)
  {
    PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).eot_toughness &= ~2;
  }

  if (lookup_player == player && lookup_card == card &&
      (PLAYER_CARD_INSTANCE(player, card).state & STATE_INVISIBLE) == 0 &&
      delegate_to_copy &&
      data_card_controller != -1 &&
      data_card_slot != -1 &&
      PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).info_slot != 0)
  {
    result |= global_cards_data[PLAYER_CARD_INSTANCE(data_card_controller, data_card_slot).info_slot].code_pointer(player, card, event);
  }

  return result;
}

// FUNCTION: MAGIC 0x0047c351
static int clone_find_data_card_callback(int source_player,
                                         int source_card,
                                         int player,
                                         int card,
                                         int internal_card_id)
{
  if (global_cards_data[internal_card_id].id == CARD_ID_DATA_CARD && PLAYER_CARD_INSTANCE(player, card).damage_target_player == source_player && PLAYER_CARD_INSTANCE(player, card).damage_target_card == source_card)
  {
    g_event_result = player << 8 | card;
    return 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047c409
static int C_vesuvan_doppelganger_helper(int source_player,
                                         int source_card,
                                         int player,
                                         int card,
                                         int internal_card_id)
{
  if ((global_cards_data[internal_card_id].type & TYPE_EFFECT) && (PLAYER_CARD_INSTANCE(player, card).token_status & STATUS_PERMANENT) && PLAYER_CARD_INSTANCE(player, card).damage_source_player == source_player && PLAYER_CARD_INSTANCE(player, card).damage_source_card == source_card)
  {
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

static int veteran_bodyguard_prevent_damage_callback(int source_player,
                                                     int source_card,
                                                     int player,
                                                     int card,
                                                     int internal_card_id);
static int veteran_bodyguard_mark_other_protectors(int unused,
                                                   int source_card,
                                                   int player,
                                                   int card,
                                                   int internal_card_id);
static int veteran_bodyguard_or_martyrs_present(int unused1,
                                                int unused2,
                                                int unused3,
                                                int unused4,
                                                int internal_card_id);
// FUNCTION: MAGIC 0x0047c4e9
// FUNCTION: SHANDALAR 0x0047ffee
int card_veteran_bodyguard(int player, int card, event_t event)
{
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_or_martyrs_present, player) != -1)
    {
      PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
    }
  }

  if (g_trigger_condition == TRIGGER_END_DAMAGE_PREV && PLAYER_CARD_INSTANCE(player, card).eot_toughness == 0 && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && g_affected_card == card && g_affected_card_controller == player && g_current_turn == player && player != g_current_player && is_in_play(player, card) && (g_current_phase == PHASE_NORMAL_COMBAT_DAMAGE || g_current_phase == PHASE_FIRST_STRIKE_DAMAGE))
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = event;
    if (event == EVENT_TRIGGER)
    {
      if (dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_prevent_damage_callback, -1) != -1)
      {
        g_event_result |= 2;
      }
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_prevent_damage_callback, -1);
      dispatch_function_to_all_cards_in_play(player, card, veteran_bodyguard_mark_other_protectors, player);
    }
  }

  if (event == EVENT_CLEANUP && g_affected_card == card && g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047c736
// FUNCTION: SHANDALAR 0x00480238
static int veteran_bodyguard_prevent_damage_callback(int source_player,
                                                     int source_card,
                                                     int player,
                                                     int card,
                                                     int internal_card_id)
{
  struct
  {
    // int pending_event;
    card_instance_t *preventer;
    int result;
    int source_card_id;
    int protects_damage;
  } s;

  s.protects_damage = 0;
  s.result = 0;
  s.source_card_id = global_cards_data[PLAYER_CARD_INSTANCE(source_player, source_card).internal_card_id].id;
  s.preventer = &PLAYER_CARD_INSTANCE(player, card);

  if (g_damage_card_internal_card_id == internal_card_id && s.preventer->info_slot != 0 && s.preventer->damage_target_player == source_player && s.preventer->damage_target_card == -1 && ((s.source_card_id == CARD_ID_VETERAN_BODYGUARD && (s.preventer->token_status & 0x140000) != 0) || (s.source_card_id == CARD_ID_MARTYRS_OF_KORLIS && (s.preventer->eot_toughness & 0x40) != 0)))
  {
    s.protects_damage = 1;
  }

  if (s.protects_damage)
  {
    switch (PLAYER_CARD_INSTANCE(source_player, source_card).info_slot)
    {
    case EVENT_TRIGGER:
      s.result = 1;
      g_event_result = 2;
      break;
    case EVENT_RESOLVE_TRIGGER:
      s.preventer->damage_target_player = (char)source_player;
      s.preventer->damage_target_card = source_card;
      break;
    default:
      break;
    }
  }

  return s.result;
}

// FUNCTION: MAGIC 0x0047c8ca
// FUNCTION: SHANDALAR 0x004803cc
static int veteran_bodyguard_mark_other_protectors(int unused,
                                                   int source_card,
                                                   int player,
                                                   int card,
                                                   int internal_card_id)
{
  (void)unused;

  if ((global_cards_data[internal_card_id].id == CARD_ID_VETERAN_BODYGUARD || global_cards_data[internal_card_id].id == CARD_ID_MARTYRS_OF_KORLIS) && card != source_card)
  {
    PLAYER_CARD_INSTANCE(player, card).eot_toughness = 1;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047c948
// FUNCTION: SHANDALAR 0x0048044a
static int veteran_bodyguard_or_martyrs_present(int unused1,
                                                int unused2,
                                                int unused3,
                                                int unused4,
                                                int internal_card_id)
{
  int result;

  (void)unused1;
  (void)unused2;
  (void)unused3;
  (void)unused4;

  result = 0;
  if (global_cards_data[internal_card_id].id == CARD_ID_VETERAN_BODYGUARD || global_cards_data[internal_card_id].id == CARD_ID_MARTYRS_OF_KORLIS)
  {
    result = 1;
    g_event_result = result;
  }

  return result;
}

// FUNCTION: MAGIC 0x0047c9a2
// FUNCTION: SHANDALAR 0x004804a4
int card_wyluli_wolf(int player, int card, event_t event)
{
  target_t target;
  int legacy_card;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return (PLAYER_CARD_INSTANCE(player, card).state & 0x20010) == 0 &&
           real_target_available((int *)0, TARGET_SCAN_DIRECT, player, 2, 2,
                                 TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                                 get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                                 -1, -1, -1, -1, 0, 0, 0) != 0;
  }

  if (event == EVENT_GET_SELECTED_CARD)
  {
    load_recorded_action_target(0);
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "WYLULI_WOLF");
    if (C_real_select_target(player, 2, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                             get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                             -1, -1, -1, -1, 0, 0, 0,
                             g_text_lines[0], 1, &target) != 0)
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

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    SET_TARGET(target, PLAYER_CARD_INSTANCE(player, card).targets[0]);
    if (C_real_validate_target(target.player, target.card, (char *)0, player, 2, 2,
                               TARGET_ZONE_IN_PLAY, TYPE_CREATURE, TYPE_NONE, 0,
                               get_protections_from(player, card), COLOR_TEST_0, COLOR_TEST_0,
                               -1, -1, -1, -1, 0, 0, 0) != 0)
    {
      legacy_card = create_legacy_effect(g_card_on_stack_controller, g_card_on_stack,
                                         LEGACY_EFFECT_PUMP, target.player, target.card);
      if (legacy_card != -1)
      {
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_power = 1;
        PLAYER_CARD_INSTANCE(player, legacy_card).counter_toughness = 1;
      }
    }
    else
    {
      g_spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .number_of_targets = 0;
  }

  if (event == EVENT_CHECK_PUMP &&
      (PLAYER_CARD_INSTANCE(player, card).state & 0x20014) == 0)
  {
    ++g_global_power_bonus[player];
    ++g_global_toughness_bonus[player];
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047cde7
// FUNCTION: SHANDALAR 0x004808e9
int card_yawgmoth_demon(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CHECK_UPK_PAYMENT)
  {
    if (own_artifact_available(player) != 0)
    {
      g_event_result |= 1;
    }
  }

  if (event == EVENT_SETUP_UPKEEP_COSTS &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot == 0 &&
      g_current_player == player &&
      g_event_player == player)
  {
    PLAYER_CARD_INSTANCE(player, card).upkeep_flags |= 0x101;
  }

  if (event == EVENT_UPKEEP_PHASE &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    ++PLAYER_CARD_INSTANCE(player, card).info_slot;
    if (own_artifact_available(player) == 0)
    {
      g_event_result |= 1;
    }
    else
    {
      load_text("promptsX1.txt", "YAWGMOTH_DEMON");
      result = select_and_sacrifice_artifact(player);
    }
  }

  if (event == EVENT_UPKEEP_COSTS_UNPAID)
  {
    load_text("promptsX1.txt", "YAWGMOTH_DEMON");
    do_dialog(player, player, card, -1, -1, g_text_lines[1], 0);
    damage_player(player, 2, g_card_on_stack_controller, g_card_on_stack);
    PLAYER_CARD_INSTANCE(PLAYER_CARD_INSTANCE(player, card).parent_controller,
                         PLAYER_CARD_INSTANCE(player, card).parent_card)
        .state |= STATE_TAPPED;
  }

  if (event == EVENT_SHOULD_AI_PLAY && own_artifact_available(player) != 0)
  {
    damage_player(player, 2, player, card);
    PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
  }

  if ((event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY) &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}

// FUNCTION: MAGIC 0x0047d194
// FUNCTION: SHANDALAR 0x00480c9e
int card_ydwen_efreet(int player, int card, event_t event)
{
  char dialog_text[900];
  int choice;

  if (event == EVENT_BLOCK_LEGALITY &&
      g_affected_card == card &&
      g_affected_card_controller == player &&
      PLAYER_CARD_INSTANCE(player, card).info_slot != 0)
  {
    g_event_result = 1;
  }

  if (event == EVENT_DECLARE_BLOCKERS &&
      player != g_current_player &&
      PLAYER_CARD_INSTANCE(player, card).blocking != -1 &&
      global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].code_pointer !=
          card_two_headed_giant_of_foriys_legacy)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 1;
    load_text("promptsX1.txt", "YDWEN_EFREET");
    sprintf(dialog_text, " %s\n %s", g_text_lines[0], g_text_lines[1]);
    choice = do_dialog(player, player, card, -1, -1, dialog_text, internal_rand(2));
    if (coin_flip(player, get_displayed_card_name(player, card), 1) != choice)
    {
      PLAYER_CARD_INSTANCE(player, card).blocking = -1;
      PLAYER_CARD_INSTANCE(player, card).state &= ~STATE_BLOCKING;
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
    }
    TENTATIVE_reassess_all_cards(0, 0xff);
  }

  if (event == EVENT_CLEANUP)
  {
    PLAYER_CARD_INSTANCE(player, card).info_slot = 0;
  }

  return 0;
}
