#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

// FUNCTION: MAGIC 0x004ab920
// FUNCTION: SHANDALAR 0x004a3190
int card_army_of_allah(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004ab9c2
// FUNCTION: SHANDALAR 0x004a3232
int card_blaze_of_glory(int player, int card, event_t event)
{
  target_t selected_target;

  if (event == EVENT_CAN_CAST)
  {
    return (current_phase == PHASE_BEFORE_BLOCKING && real_target_available((int *)0,
                                                                            TARGET_SCAN_DIRECT,
                                                                            player,
                                                                            1 - human_player,
                                                                            1 - human_player,
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
                                                                            0,
                                                                            0,
                                                                            0))
               ? 1
               : 0;
  }

  if (((event == EVENT_CAST_SPELL) && (affected_card == card)) && (affected_card_controller == player))
  {
    load_text("promptsX1.txt", "BLAZE_OF_GLORY");
    if (C_real_select_target(player,
                             1 - human_player,
                             1 - human_player,
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
                             text_lines[0],
                             1,
                             &selected_target))
    {
      PLAYER_CARD_INSTANCE(player, card).targets[0].player = selected_target.player;
      PLAYER_CARD_INSTANCE(player, card).targets[0].card = selected_target.card;
      PLAYER_CARD_INSTANCE(player, card).number_of_targets = 1;
    }
    else
    {
      spell_fizzled = 1;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    selected_target.player = PLAYER_CARD_INSTANCE(player, card).targets[0].player;
    selected_target.card = PLAYER_CARD_INSTANCE(player, card).targets[0].card;
    if (C_real_validate_target(selected_target.player,
                               selected_target.card,
                               (char *)0,
                               player,
                               1 - human_player,
                               1 - human_player,
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
      create_legacy_effect(player, card, unk_008a8df0, selected_target.player, selected_target.card);
    }
    else
    {
      spell_fizzled = 1;
    }
    PLAYER_CARD_INSTANCE(player, card).number_of_targets = 0;
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004abcd8
// FUNCTION: SHANDALAR 0x004a354a
int card_guardian_angel(int player, int card, event_t event)
{
  card_instance_t *instance;
  card_instance_t *damage;
  target_t selected_target;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    if ((unk_008b4278 & 4) == 0 || !real_target_available((int *)0,
                                                          TARGET_SCAN_DIRECT,
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
                                                          unk_009266a4,
                                                          ~SUB_WALL,
                                                          -1,
                                                          -1,
                                                          0,
                                                          0,
                                                          0))
    {
      return 0;
    }
    return 99;
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"promptsX1.txt", "GUARDIAN_ANGLE");
    if (!C_real_select_target(player,
                              2,
                              2,
                              TARGET_ZONE_IN_PLAY,
                              TYPE_NONE,
                              TYPE_NONE,
                              0,
                              0,
                              COLOR_TEST_0,
                              COLOR_TEST_0,
                              unk_009266a4,
                              ~SUB_WALL,
                              -1,
                              -1,
                              0,
                              0,
                              0,
                              text_lines[0],
                              1,
                              &selected_target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0] = selected_target;
      instance->number_of_targets = 1;
      instance->info_slot = x_value;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    selected_target = instance->targets[0];
    if (!C_real_validate_target(selected_target.player,
                                selected_target.card,
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
                                unk_009266a4,
                                ~SUB_WALL,
                                -1,
                                -1,
                                0,
                                0,
                                0))
    {
      spell_fizzled = 1;
    }
    else
    {
      damage = &PLAYER_CARD_INSTANCE(selected_target.player, selected_target.card);
      if (instance->info_slot < damage->info_slot)
      {
        damage->info_slot -= instance->info_slot;
      }
      else
      {
        damage->info_slot = 0;
      }
      create_legacy_effect(player,
                           card,
                           unk_008cf1bc,
                           (int)damage->damage_source_player,
                           damage->damage_source_card);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_DESTROY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ac114
// FUNCTION: SHANDALAR 0x004a3988
int card_natural_selection(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int available[10];
  int selected[3];
  int top_three[3];
  int count;
  int choice_count;
  int index;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    load_text("promptsX1.txt", "NATURAL_SELECTION");
    if (!C_real_select_target(player,
                              2,
                              1 - player,
                              TARGET_ZONE_PLAYERS,
                              TYPE_NONE,
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
                              text_lines[0],
                              1,
                              &target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = target.player;
      instance->targets[0].card = target.card;
      instance->number_of_targets = 1;
      if (player == active_player)
      {
        ai_modifier += 0x18;
      }
    }
    return 0;
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    load_text("promptsX1.txt", "NATURAL_SELECTION_2");
    count = 0;
    for (index = 0; index < 3; ++index)
    {
      top_three[index] = -1;
      available[index] = 0;
      if (global_library[instance->targets[0].player][index] != -1)
      {
        top_three[index] = global_library[instance->targets[0].player][index];
        available[index] = 1;
        ++count;
      }
    }

    if ((player == human_player || (unk_00926804 & 2) != 0) && unk_008a9000 != 1)
    {
      if (count > 0)
      {
        choice_count =
            FUN_004a62d7(player, top_three, available, count, 0x89684c, 1, selected, 0, count);
      }
      else
      {
        choice_count = 0;
      }
    }
    else if (internal_rand(2) == 0)
    {
      choice_count = 0;
    }
    else
    {
      choice_count = 0;
      for (index = 0; index < count; ++index)
      {
        selected[index] = index;
        ++choice_count;
      }
    }

    if (choice_count < count)
    {
      FUN_004b59b2(player, instance->targets[0].player);
    }
    else
    {
      for (index = 0; index < choice_count; ++index)
      {
        global_library[instance->targets[0].player][index] = top_three[selected[index]];
      }
    }

    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ac52b
// FUNCTION: SHANDALAR 0x004a3da3
int card_psionic_blast(int player, int card, event_t event)
{
  card_instance_t *instance;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) && (player == affected_card_controller))
  {
    load_text((int)"promptsX1.txt", "PSIONIC_BLAST");
    FUN_0054ac4d(player, card, 4);
    if (player == active_player)
    {
      ai_modifier += (3 - PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).toughness) * 0xc;
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (FUN_0054af10(player, card, EVENT_RESOLVE_SPELL, 4) != 0)
    {
      damage_player(player, 2, player, card);
    }
    instance->number_of_targets = 0;
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

// FUNCTION: MAGIC 0x004ac6c0
// FUNCTION: SHANDALAR 0x004a3f3b
int card_reverse_polarity(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004acf05
// FUNCTION: SHANDALAR 0x004a4784
int card_sewers_of_estark(int player, int card, event_t event)
{

}
// FUNCTION: MAGIC 0x004ad263
// FUNCTION: SHANDALAR 0x004a4ae4
int card_artifact_blast(int player, int card, event_t event)
{

}

// FUNCTION: MAGIC 0x004ad5b4
// FUNCTION: SHANDALAR 0x004a4e37
int card_sacrifice(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_t target;
  int iid;
  int amount;

  instance = &PLAYER_CARD_INSTANCE(player, card);

  if (event == EVENT_CAN_CAST)
  {
    return real_target_available((int *)0, TARGET_SCAN_DIRECT, player, player, player, 0x200, 2, 0, 0,
                                 0, 0, 0, -1, -1, 0xffffffff, 0xffffffff, 0, 0, 0);
  }

  if ((event == EVENT_CAST_SPELL) && (card == card_on_stack) && (player == card_on_stack_controller))
  {
    load_text((int)"promptsX1.txt", "SACRIFICE");
    if (!C_real_select_target(player, player, player, TARGET_ZONE_IN_PLAY, TYPE_CREATURE,
                              TYPE_NONE, 0, 0, COLOR_TEST_0, COLOR_TEST_0, -1, ~SUB_WALL,
                              -1, -1, 0, 0, 0, text_lines[0], 0, &target))
    {
      spell_fizzled = 1;
    }
    else
    {
      instance->targets[0].player = player;
      instance->targets[0].card = target.card;
      instance->number_of_targets = 1;
      kill_card(player, target.card, KILL_SACRIFICE);
    }
  }

  if (event == EVENT_RESOLVE_SPELL)
  {
    iid = PLAYER_CARD_INSTANCE(instance->targets[0].player, instance->targets[0].card).internal_card_id;
    amount = (int)(char)global_cards_data[iid].cc[0] + (int)(char)global_cards_data[iid].cc[1];
    if (global_cards_data[iid].cc[1] == 0xff)
    {
      ++amount;
    }
    produce_mana(player, COLOR_BLACK, amount);
    kill_card(player, card, KILL_BURY);
  }

  return 0;
}

