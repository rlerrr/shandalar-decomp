#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "magic_sealed_build.h"
#include "deckdll/src/card_db.h"

/* These two routines use optimized code and omit the frame pointer in
 * the original executable, unlike the surrounding shell code. */
#pragma optimize("gty", on)
#pragma intrinsic(memcpy, memset)

extern int g_rarity_csv_sealed_template_counts[3][3];
extern int g_rarity_csv_sealed_card_weights[3][20];
extern int g_rarity_csv_sealed_land_weights[3][6];
extern int g_rarity_csv_sealed_starter_counts[3];
extern int g_rarity_csv_sealed_booster_counts[3];
extern int g_rarity_csv_sealed_color_weights[3][3];

typedef struct sealed_deck_evaluation_weights_t
{
  int reserved[52];
  int card_weights[20];
  int reserved_0120[6];
  int land_weights[6];
} sealed_deck_evaluation_weights_t;

typedef union sealed_deck_statistics_t
{
  int values[32];
  struct
  {
    int mana_producers[8];
    int color_cards[6];
    int color_mana_counts[6];
    int color_land_counts[6];
    int reserved_0068;
    int color_values[5];
  } fields;
} sealed_deck_statistics_t;

STATIC_ASSERT(sizeof(sealed_deck_evaluation_weights_t) == 0x150,
              sealed_deck_evaluation_weights_wrong_size);
STATIC_ASSERT(sizeof(sealed_deck_statistics_t) == 0x80,
              sealed_deck_statistics_wrong_size);

static __inline unsigned int shell_sealed_card_class_flags(
    card_ptr_t *card, int class_index)
{
  unsigned char shift;
  int modifiers;

  if (class_index <= 9)
  {
    shift = (unsigned char)(class_index * 3);
    modifiers = card->ai_modifiers1;
  }
  else
  {
    shift = (unsigned char)((class_index - 10) * 3);
    modifiers = card->ai_modifiers2;
  }
  return (modifiers & (7 << shift)) >> shift;
}

// FUNCTION: MAGIC 0x00454180
int shell_evaluate_sealed_deck(int *deck, int count, int *weights, int *card_values)
{
  struct
  {
    char mana_costs[8];
    int combat_base_value;
    int ability_weight4;
    int class_weight4;
    int ability_weight2;
    int class_weight2;
    int wall_dependency;
    int creature_dependency;
    int artifact_dependency;
    int total_value;
    int class_weight1;
    int land_bonus;
    int color_bonus;
    int card_base_value;
    int casting_factor;
    int *card_value_cursor;
    int *deck_cursor;
    int remaining_cards;
    int mana_penalties[6];
    sealed_deck_statistics_t stats;
    int class_counts[20];
    int class_percentages[20];
    int ability_counts[6];
    int ability_percentages[6];
    sealed_deck_evaluation_weights_t weights;
  } s;
  int *deck_cursor;
  int field_index;
  int color;
  int value;
  int mana_count;
  int cast_value;
  int ability_count;
  int ability_value;
  int combat_value;
  int class_weight1;
  int power;
  int toughness;
  int inflatable_multiplier;
  unsigned int flags;
  unsigned char mana_source_colors;
  card_ptr_t *card;

  if (deck == NULL || count == 0)
    return 0;
  if (g_sealed_deck_build_log != NULL)
    fprintf(g_sealed_deck_build_log, "\n**********COMPUTING DECK VALUE***************\n");
  if (weights != NULL)
    memcpy(&s.weights, weights, sizeof(s.weights));
  else
  {
    memcpy(s.weights.card_weights, g_rarity_csv_sealed_card_weights[1],
            sizeof(s.weights.card_weights));
    memcpy(s.weights.land_weights, g_rarity_csv_sealed_land_weights[1],
            sizeof(s.weights.land_weights));
  }
  for (field_index = 0; field_index <= 7; ++field_index)
  {
    s.stats.values[field_index] = 1;
    s.stats.values[field_index + 8] = 0;
    s.stats.values[field_index + 14] = 0;
    s.stats.values[field_index + 20] = 0;
  }
  s.stats.fields.mana_producers[7] = 0;
  memset(s.class_counts, 0, sizeof(s.class_counts));
  memset(s.ability_counts, 0, sizeof(s.ability_counts));
  if (count > 0)
  {
    deck_cursor = deck;
    s.mana_penalties[0] = count;
    do
    {
      card = &global_raw_cards_storage[*deck_cursor];
      if (*deck_cursor < 0)
        mana_source_colors = (unsigned char)(1 << (unsigned char)-*deck_cursor);
      else
        mana_source_colors = card->mana_source_colors;
      if (mana_source_colors != 0)
      {
        ++s.stats.fields.mana_producers[7];
        if ((mana_source_colors & 0x20) != 0)
          ++s.stats.fields.mana_producers[5];
        if ((mana_source_colors & 0x10) != 0)
          ++s.stats.fields.mana_producers[4];
        if ((mana_source_colors & 8) != 0)
          ++s.stats.fields.mana_producers[3];
        if ((mana_source_colors & 2) != 0)
          ++s.stats.fields.mana_producers[1];
        if ((mana_source_colors & 4) != 0)
          ++s.stats.fields.mana_producers[2];
      }
      if (card->req.req_white != 0)
        ++s.stats.fields.color_cards[5];
      if (card->req.req_red != 0)
        ++s.stats.fields.color_cards[4];
      if (card->req.req_green != 0)
        ++s.stats.fields.color_cards[3];
      if (card->req.req_black != 0)
        ++s.stats.fields.color_cards[1];
      if (card->req.req_blue != 0)
        ++s.stats.fields.color_cards[2];
      for (color = 1; color <= 5; ++color)
      {
        if ((((unsigned short)card->ai_counts_as_color >> (unsigned char)(color * 2)) & 3) != 0)
          ++s.stats.fields.color_mana_counts[color];
      }
      for (color = 1; color <= 5; ++color)
      {
        if ((((unsigned short)card->ai_counts_as_land >> (unsigned char)(color * 2)) & 3) != 0)
          ++s.stats.fields.color_land_counts[color];
      }
      for (field_index = 0; field_index < 20; ++field_index)
        if ((shell_sealed_card_class_flags(card, field_index) & 1) != 0)
          ++s.class_counts[field_index];
      for (field_index = 0; field_index < 6; ++field_index)
        if ((((int)(card->ai_abilities & (7 << (unsigned char)(field_index * 3))) >> (unsigned char)(field_index * 3)) & 1) != 0)
          ++s.ability_counts[field_index];
      ++deck_cursor;
    } while (--s.mana_penalties[0] != 0);
  }
  for (color = 1; color <= 5; ++color)
  {
    value = s.stats.fields.color_mana_counts[color] +
              (s.stats.fields.color_cards[color] * 100 / count + 5) / 10;
    if (value >= 9)
      value = 9;
    s.stats.fields.color_values[color - 1] = value;
  }
  for (field_index = 0; field_index < 20; ++field_index)
    s.class_percentages[field_index] = s.class_counts[field_index] * 100 / count;
  for (field_index = 0; field_index < 6; ++field_index)
  {
    if (s.class_counts[12] + s.class_counts[13] == 0)
      s.ability_percentages[field_index] = 0;
    else
      s.ability_percentages[field_index] = s.ability_counts[field_index] * 100 /
                                            (s.class_counts[12] + s.class_counts[13]);
  }
  if (g_sealed_deck_build_log != NULL)
    fprintf(g_sealed_deck_build_log, "Data pass (deck data):\n");
  if (g_sealed_deck_build_log != NULL)
    fprintf(g_sealed_deck_build_log, "  numManaProducers (xBUGRW total): %d %d %d %d %d %d  %d\n",
            s.stats.fields.mana_producers[0], s.stats.fields.mana_producers[1],
            s.stats.fields.mana_producers[2], s.stats.fields.mana_producers[3],
            s.stats.fields.mana_producers[4], s.stats.fields.mana_producers[5],
            s.stats.fields.mana_producers[7]);
  if (g_sealed_deck_build_log != NULL)
    fprintf(g_sealed_deck_build_log, "  numColor (BUGRW): %d %d %d %d %d\n",
            s.stats.fields.color_cards[1], s.stats.fields.color_cards[2],
            s.stats.fields.color_cards[3], s.stats.fields.color_cards[4],
            s.stats.fields.color_cards[5]);
  if (g_sealed_deck_build_log != NULL)
    fprintf(g_sealed_deck_build_log, "  CCsums (BUGRW): %d %d %d %d %d\n",
            s.stats.fields.color_mana_counts[1], s.stats.fields.color_mana_counts[2],
            s.stats.fields.color_mana_counts[3], s.stats.fields.color_mana_counts[4],
            s.stats.fields.color_mana_counts[5]);
  if (g_sealed_deck_build_log != NULL)
    fprintf(g_sealed_deck_build_log, "  ColorValue (BUGRW): %d %d %d %d %d\n",
            s.stats.fields.color_values[0], s.stats.fields.color_values[1],
            s.stats.fields.color_values[2], s.stats.fields.color_values[3],
            s.stats.fields.color_values[4]);
  if (g_sealed_deck_build_log != NULL)
    fprintf(g_sealed_deck_build_log, "  LandTotal (BUGRW): %d %d %d %d %d\n",
            s.stats.fields.color_land_counts[1], s.stats.fields.color_land_counts[2],
            s.stats.fields.color_land_counts[3], s.stats.fields.color_land_counts[4],
            s.stats.fields.color_land_counts[5]);
  if (g_sealed_deck_build_log != NULL)
  {
    fprintf(g_sealed_deck_build_log, "  numIsClass num(percent): ");
    for (field_index = 0; field_index < 20; ++field_index)
      fprintf(g_sealed_deck_build_log, "%d (%d) ",
              s.class_counts[field_index], s.class_percentages[field_index]);
    fprintf(g_sealed_deck_build_log, "\n");
  }
  if (g_sealed_deck_build_log != NULL)
  {
    fprintf(g_sealed_deck_build_log, "  numHasAbility num(percent): ");
    for (field_index = 0; field_index < 6; ++field_index)
      fprintf(g_sealed_deck_build_log, "%d (%d) ",
              s.ability_counts[field_index], s.ability_percentages[field_index]);
    fprintf(g_sealed_deck_build_log, "\n");
  }
  if (g_sealed_deck_build_log != NULL)
    fprintf(g_sealed_deck_build_log, "\nCardValue Name\t(LEFT:Cast Artifact Creature Wall)\t(RIGHT:Base Color Land Class-2 Ability-2 Class-4 Ability-4 Class-1 Combat [PTval Abilval])\n");
  s.total_value = 0;
  if (count > 0)
  {
    s.card_value_cursor = card_values;
    s.deck_cursor = deck;
    s.remaining_cards = count;
    do
    {
      card = &global_raw_cards_storage[*s.deck_cursor];
      if (*s.deck_cursor < 0)
      {
        value = 0;
        if (g_sealed_deck_build_log != NULL)
        {
          if (*s.deck_cursor == -1)
            fprintf(g_sealed_deck_build_log, "%4d %s\n", 0, "Free Land (swamp)");
          else if (*s.deck_cursor == -2)
            fprintf(g_sealed_deck_build_log, "%4d %s\n", 0, "Free Land (island)");
          else if (*s.deck_cursor == -3)
            fprintf(g_sealed_deck_build_log, "%4d %s\n", 0, "Free Land (forest)");
          else if (*s.deck_cursor == -4)
            fprintf(g_sealed_deck_build_log, "%4d %s\n", 0, "Free Land (mountain)");
          else if (*s.deck_cursor == -5)
            fprintf(g_sealed_deck_build_log, "%4d %s\n", 0, "Free Land (plains)");
          else
            fprintf(g_sealed_deck_build_log, "%4d %s %d\n", 0, "Free Land",
                    *s.deck_cursor);
        }
      }
      else if (card->card_type == 5)
      {
        value = card->ai_base_value;
        if (card->id == 0xef)
          color = 1;
        else if (card->id == 0x7e)
          color = 2;
        else if (card->id == 0x5b)
          color = 3;
        else if (card->id == 0xa4)
          color = 4;
        else if (card->id == 0xbc)
          color = 5;
        else
          color = -1;
        if (color != -1 && s.stats.fields.color_cards[color] <= 3)
          value += (s.stats.fields.color_cards[color] * 5 - 20) * 5;
        if (g_sealed_deck_build_log != NULL)
          fprintf(g_sealed_deck_build_log, "%4d %s\n", value, card->full_name);
      }
      else
      {
        s.mana_costs[0] = card->req.req_colorless;
        s.mana_costs[1] = card->req.req_black;
        s.mana_costs[2] = card->req.req_blue;
        s.mana_costs[3] = card->req.req_green;
        s.mana_costs[4] = card->req.req_red;
        s.mana_costs[5] = card->req.req_white;
        for (color = 0; color <= 5; ++color)
          if (s.mana_costs[color] == '(')
            s.mana_costs[color] = 0;
        for (color = 0; color <= 5; ++color)
        {
          if (color == 0)
            mana_count = s.stats.fields.mana_producers[7];
          else
            mana_count = s.stats.fields.mana_producers[color];
          if (s.mana_costs[color] == 0)
            s.mana_penalties[color] = 0;
          else if (mana_count < 1)
            s.mana_penalties[color] = 10000;
          else
            s.mana_penalties[color] =
                ((((s.mana_costs[color] * 5 + 5) * 100) / 10) / mana_count) *
                ((mana_count * -100) / count + 200);
        }
        cast_value = (10000 - s.mana_penalties[3] - s.mana_penalties[5] -
                        s.mana_penalties[1] - s.mana_penalties[2] -
                        s.mana_penalties[4] - s.mana_penalties[0]) / 100;
        if (cast_value <= 0)
          cast_value = 0;
        if ((card->ai_dependencies & 7) == 0)
          s.artifact_dependency = 100;
        else
          s.artifact_dependency = (s.class_percentages[11] * 100 / count) *
                                  g_rarity_csv_sealed_template_counts[0][0];
        if ((card->ai_dependencies & 0x38) == 0)
          s.creature_dependency = 100;
        else
          s.creature_dependency =
              ((s.class_percentages[13] + s.class_percentages[12]) * 100 / count) *
              g_rarity_csv_sealed_template_counts[0][1];
        if ((card->ai_dependencies & 0x1c0) == 0)
          s.wall_dependency = 100;
        else
          s.wall_dependency = (s.ability_percentages[4] * 100 / count) *
                              g_rarity_csv_sealed_template_counts[0][2];
        s.casting_factor = (s.artifact_dependency * cast_value *
                            s.wall_dependency * s.creature_dependency) / 1000000;
        s.color_bonus = 0;
        for (color = 1; color <= 5; ++color)
        {
          s.color_bonus +=
              ((unsigned short)card->ai_for_color >> (unsigned char)(color * 2) & 3) *
              s.stats.fields.color_cards[color];
        }
        s.color_bonus *= 10;
        s.land_bonus = 0;
        for (color = 1; color <= 5; ++color)
        {
          s.land_bonus +=
              ((unsigned short)card->ai_for_land >> (unsigned char)(color * 2) & 3) *
              s.stats.fields.color_land_counts[color];
        }
        s.land_bonus *= 10;
        s.class_weight2 = 0;
        for (field_index = 0; field_index < 20; ++field_index)
        {
          flags = shell_sealed_card_class_flags(card, field_index);
          if ((flags & 2) != 0)
            s.class_weight2 += g_rarity_csv_sealed_card_weights[0][field_index] *
                                s.class_percentages[field_index];
        }
        s.ability_weight2 = 0;
        for (field_index = 0; field_index < 6; ++field_index)
        {
          flags = (int)(card->ai_abilities & (7 << (unsigned char)(field_index * 3))) >> (unsigned char)(field_index * 3);
          if ((flags & 2) != 0)
          {
            value = 100 - g_rarity_csv_sealed_land_weights[0][field_index] *
                            s.ability_percentages[field_index];
            if (value <= 0)
              value = 0;
            s.ability_weight2 += value;
          }
        }
        s.class_weight4 = 0;
        for (field_index = 0; field_index < 20; ++field_index)
        {
          flags = shell_sealed_card_class_flags(card, field_index);
          if ((flags & 4) != 0)
            s.class_weight4 += s.weights.card_weights[field_index] *
                                g_rarity_csv_sealed_card_weights[0][field_index];
        }
        s.ability_weight4 = 0;
        for (field_index = 0; field_index < 6; ++field_index)
        {
          flags = (int)(card->ai_abilities & (7 << (unsigned char)(field_index * 3))) >> (unsigned char)(field_index * 3);
          if ((flags & 4) != 0)
            s.ability_weight4 += s.weights.land_weights[field_index] *
                                  g_rarity_csv_sealed_land_weights[0][field_index];
        }
        class_weight1 = 0;
        for (field_index = 0; field_index < 20; ++field_index)
        {
          flags = shell_sealed_card_class_flags(card, field_index);
          if ((flags & 1) != 0)
            class_weight1 += g_rarity_csv_sealed_card_weights[2][field_index] *
                                s.class_counts[field_index];
        }
        s.class_weight1 = class_weight1 / 10;
        power = (unsigned char)card->ai_power_toughness;
        toughness = (unsigned char)(card->ai_power_toughness >> 8);
        if (power < 0 || power > 12)
          power = 1;
        if (toughness < 0 || toughness > 12)
          toughness = 1;
        s.combat_base_value = power * g_rarity_csv_sealed_starter_counts[0] +
                            toughness * g_rarity_csv_sealed_booster_counts[0];
      inflatable_multiplier = 1;
        if (card->inflatable != 0)
          inflatable_multiplier = g_rarity_csv_sealed_color_weights[0][2];
        s.combat_base_value =
            power * g_rarity_csv_sealed_starter_counts[0] +
            toughness * g_rarity_csv_sealed_booster_counts[0] +
            ((unsigned char)(card->ai_inc_power_toughness >> 8) *
                 g_rarity_csv_sealed_color_weights[0][1] +
             (unsigned char)card->ai_inc_power_toughness *
                 g_rarity_csv_sealed_color_weights[0][0]) * inflatable_multiplier;
        ability_count = 0;
        ability_value = 1;
        for (field_index = 0; field_index < 6; ++field_index)
        {
          if (((int)(card->ai_abilities & (7 << (unsigned char)(field_index * 3))) >> (unsigned char)(field_index * 3) & 1) != 0)
          {
            ++ability_count;
            ability_value *= g_rarity_csv_sealed_land_weights[2][field_index];
          }
        }
        while (ability_count > 1)
        {
          ability_value /= 10;
          --ability_count;
        }
        combat_value = ability_value * s.combat_base_value;
        if (ability_count != 0)
          combat_value /= 10;
        s.card_base_value = s.class_weight1 + combat_value + s.ability_weight4 +
                            s.class_weight4 + s.ability_weight2 + card->ai_base_value +
                            s.class_weight2 + s.color_bonus + s.land_bonus;
        if (cast_value < 5)
          value = -500;
        else if (cast_value < 10)
          value = -100;
        else
          value = (s.card_base_value * s.casting_factor) / count;
        if (g_sealed_deck_build_log != NULL)
          fprintf(g_sealed_deck_build_log, "%4d %s \t(%3d: %3d %3d %3d %3d)\t(%4d: %4d %4d %4d %4d %4d %4d %4d %4d %4d [%4d %4d])\n", value, card->full_name,
                  s.casting_factor, cast_value, s.artifact_dependency,
                  s.creature_dependency, s.wall_dependency, s.card_base_value,
                  card->ai_base_value, s.color_bonus, s.land_bonus, s.class_weight2,
                  s.ability_weight2, s.class_weight4, s.ability_weight4,
                  s.class_weight1, combat_value, s.combat_base_value, ability_value);
      }
      if (card_values != NULL)
        *s.card_value_cursor = value;
      s.total_value += value;
      ++s.card_value_cursor;
      ++s.deck_cursor;
    } while (--s.remaining_cards != 0);
  }
  if (g_sealed_deck_build_log != NULL)
    fprintf(g_sealed_deck_build_log, "*********DECK VALUE: %d************\n\n", s.total_value);
  return s.total_value;
}

static __inline int shell_sealed_card_primary_color(int card_id)
{
  card_ptr_t *card;
  int color;

  card = &global_raw_cards_storage[card_id];
  color = -1;
  if (card_id == 0xbc)
    color = 5;
  else if (card_id == 0xa4)
    color = 4;
  else if (card_id == 0x5b)
    color = 3;
  else if (card_id == 0xef)
    color = 1;
  else if (card_id == 0x7e)
    color = 2;
  else if (card->req.req_white != 0)
    color = 5;
  else if (card->req.req_red != 0)
    color = 4;
  else if (card->req.req_green != 0)
    color = 3;
  else if (card->req.req_black != 0)
    color = 1;
  else if (card->req.req_blue != 0)
    color = 2;
  return color;
}

// FUNCTION: MAGIC 0x00454eb0
unsigned int shell_reduce_sealed_deck_colors(int *deck, int *count,
                                            int minimum_size)
{
  struct
  {
    int done;
    int color_counts[7];
    /* After ranking, the averages become the selection cursor, then
     * the remaining-card count for compaction and free-land changes. */
    union
    {
      int average_values[5];
      int *selected_color_cursor;
      int remaining_cards;
    } selection;
    int land_ids[6];
    union
    {
      struct
      {
        int ranked_colors[5];
        int selected_colors[300];
      } cards;
      int color_totals[305];
    } colors;
    int card_values[300];
    /* The totals occupy slots 1 through 5 of selected_colors until
     * ranking finishes; selection then overwrites those same slots. */
  } s;
  int card_count;
  int *deck_cursor;
  int *value_cursor;
  int remaining_cards;
  int color;
  int value;
  int rank_index;
  int best_value;
  int best_color;
  unsigned int color_mask;
  int selected_card_count;
  int output_count;
  int card_id;
  int next_color;
  const char *color_name;

  s.land_ids[0] = -1;
  s.land_ids[1] = 0xef;
  s.land_ids[2] = 0x7e;
  s.land_ids[3] = 0x5b;
  s.land_ids[4] = 0xa4;
  s.land_ids[5] = 0xbc;
  card_count = *count;
  value_cursor = &s.colors.cards.selected_colors[1];
  value_cursor[0] = 0;
  value_cursor[1] = 0;
  value_cursor[2] = 0;
  value_cursor[3] = 0;
  value_cursor[4] = 0;
  value_cursor = &s.color_counts[1];
  value_cursor[0] = 0;
  value_cursor[1] = 0;
  value_cursor[2] = 0;
  value_cursor[3] = 0;
  value_cursor[4] = 0;
  if (deck == NULL || count == NULL || card_count == 0 || minimum_size == 0)
    return (unsigned int)-1;
  if (g_sealed_deck_report != NULL)
    fprintf(g_sealed_deck_report, "\n))))) REDUCE DECK COLORS ))))))\n");
  if (g_sealed_deck_report != NULL)
  {
    g_sealed_deck_build_report_enabled = 1;
    g_sealed_deck_build_log = g_sealed_deck_report;
  }
  shell_evaluate_sealed_deck(deck, card_count, NULL, s.card_values);
  if (g_sealed_deck_report != NULL)
  {
    g_sealed_deck_build_report_enabled = 0;
    g_sealed_deck_build_log = NULL;
  }
  if (card_count > 0)
  {
    value_cursor = s.card_values;
    deck_cursor = deck;
    remaining_cards = card_count;
    best_color = s.card_values[0];
    do
    {
      if (*deck_cursor > 0)
      {
        color = shell_sealed_card_primary_color(*deck_cursor);
        best_color = color;
        if (color != -1)
          ++s.color_counts[color];
        value = *value_cursor;
        if (value <= 0)
          value = 0;
        /* A colorless card uses the preceding ranked-color slot in the
         * original layout, rather than one of the five color totals. */
        s.colors.color_totals[color + 5] += value;
      }
      ++value_cursor;
      ++deck_cursor;
    } while (--remaining_cards != 0);
  }
  else
    best_color = s.card_values[0];
  s.color_counts[0] = card_count - s.color_counts[3] - s.color_counts[4] -
                      s.color_counts[5] - s.color_counts[1] - s.color_counts[2];
  for (color = 1; color <= 5; ++color)
    s.selection.average_values[color - 1] =
        s.colors.cards.selected_colors[color] / s.color_counts[color];
  color_mask = 0;
  for (rank_index = 0; rank_index < 5; ++rank_index)
  {
    best_value = 0;
    for (color = 1; color <= 5; ++color)
    {
      if (s.selection.average_values[color - 1] > best_value &&
          (color_mask & (1 << (unsigned char)color)) == 0)
      {
        best_value = s.selection.average_values[color - 1];
        best_color = color;
      }
    }
    s.colors.cards.ranked_colors[rank_index] = best_color;
    color_mask |= 1 << (unsigned char)best_color;
  }
  if (g_sealed_deck_report != NULL)
  {
    fprintf(g_sealed_deck_report,
            "numColor B:%d U:%d G:%d R:%d W:%d other:%d\n",
            s.color_counts[1], s.color_counts[2], s.color_counts[3],
            s.color_counts[4], s.color_counts[5], s.color_counts[0]);
    fprintf(g_sealed_deck_report,
            "average card values B:%d U:%d G:%d R:%d W:%d\n\n",
            s.selection.average_values[0], s.selection.average_values[1],
            s.selection.average_values[2],
            s.selection.average_values[3], s.selection.average_values[4]);
  }
  color_mask = 0;
  s.color_counts[6] = 0;
  s.selection.selected_color_cursor = s.colors.cards.selected_colors;
  selected_card_count = s.color_counts[0];
  s.done = 0;
  do
  {
    color = -1;
    for (rank_index = 0; rank_index < 5 && color == -1; ++rank_index)
    {
      next_color = s.colors.cards.ranked_colors[rank_index];
      if ((color_mask & (1 << (unsigned char)next_color)) == 0 &&
          s.color_counts[next_color] + selected_card_count >= minimum_size)
        color = next_color;
    }
    if (color == -1)
    {
      for (rank_index = 0; rank_index < 5 && color == -1; ++rank_index)
        if ((color_mask &
             (1 << (unsigned char)s.colors.cards.ranked_colors[rank_index])) == 0)
          color = s.colors.cards.ranked_colors[rank_index];
    }
    if (color == -1)
      s.done = 1;
    else
    {
      selected_card_count += s.color_counts[color];
      ++s.color_counts[6];
      color_mask |= 1 << (unsigned char)color;
      *s.selection.selected_color_cursor++ = color;
      if (g_sealed_deck_report != NULL)
      {
        if (color == 1)
          color_name = "BLACK";
        else if (color == 2)
          color_name = "BLUE";
        else if (color == 3)
          color_name = "GREEN";
        else if (color == 4)
          color_name = "RED";
        else
          color_name = "WHITE";
        fprintf(g_sealed_deck_report, "Choose Color:%s\n", color_name);
      }
    }
    if (selected_card_count >= minimum_size)
      s.done = 1;
    if (s.color_counts[6] > 4)
      s.done = 1;
  } while (s.done == 0);
  output_count = 0;
  if (card_count > 0)
  {
    deck_cursor = deck;
    s.selection.remaining_cards = card_count;
    do
    {
      card_id = *deck_cursor;
      if (card_id <= 0)
        deck[output_count++] = card_id;
      else
      {
        color = shell_sealed_card_primary_color(card_id);
        if (color == -1 || (color_mask & (1 << (unsigned char)color)) != 0)
          deck[output_count++] = card_id;
      }
      ++deck_cursor;
    } while (--s.selection.remaining_cards != 0);
  }
  card_count = output_count;
  next_color = 0;
  if (card_count > 0)
  {
    s.selection.remaining_cards = card_count;
    deck_cursor = deck;
    do
    {
      if (*deck_cursor < 0)
      {
        for (color = 1; color <= 5; ++color)
        {
          if (*deck_cursor + color == 0 &&
              (color_mask & (1 << (unsigned char)color)) == 0)
          {
            if (g_sealed_deck_report != NULL)
              fprintf(g_sealed_deck_report, "Change free land %s to",
                      global_raw_cards_storage[s.land_ids[-*deck_cursor]].full_name);
            best_color = s.colors.cards.selected_colors[next_color];
            next_color = (next_color + 1) % s.color_counts[6];
            *deck_cursor = -best_color;
            if (g_sealed_deck_report != NULL)
              fprintf(g_sealed_deck_report, " %s \n",
                      global_raw_cards_storage[s.land_ids[best_color]].full_name);
          }
        }
      }
      ++deck_cursor;
    } while (--s.selection.remaining_cards != 0);
  }
  if (g_sealed_deck_report != NULL)
    fprintf(g_sealed_deck_report, "Deck size is now %d\n", card_count);
  if (g_sealed_deck_report != NULL)
    fprintf(g_sealed_deck_report, "))))))))))))))))))))))))))))))))\n\n");
  *count = card_count;
  return color_mask;
}

#pragma optimize("", off)
