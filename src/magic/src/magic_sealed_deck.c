#include <stdlib.h>
#include "defs.h"
#include "deckdll/src/card_db.h"
#include "magic_sealed_deck.h"

typedef struct sealed_deck_starter_template_t
{
  int color_mask;
  int land_count;
  int spell_count;
  int allow_artifact_spells;
} sealed_deck_starter_template_t;

extern sealed_deck_starter_template_t g_sealed_starter_templates[3];
extern int global_available_slots;
int is_card_available_in_expansion(unsigned int csvid, int expansion);

// FUNCTION: MAGIC 0x004c3677
int shell_choose_sealed_pack_variation(int pack_type, int is_starter)
{
  int variation;

  if (pack_type < 0 || pack_type > 8)
    return 0;
  if (is_starter != 0)
    variation = rand() % g_sealed_pack_definitions[pack_type].rounds;
  else
    variation = rand() % g_sealed_pack_definitions[pack_type].color_count;
  return variation;
}

// FUNCTION: MAGIC 0x004c3b99
int shell_pick_sealed_cards(int pack_type, unsigned int rarity,
                           unsigned int card_type, int requested_count,
                           int allow_duplicates, int *cards)
{
  struct
  {
    int expansion_bit;
    int maximum_rarity;
    int card_rarity;
    int minimum_rarity;
    int rarity_shift;
    int selected_count;
    int initial_index;
    int card_index;
    int candidates[2000];
    int candidates_exhausted;
    int candidate_count;
    card_ptr_t *card;
  } s;

  if (cards == NULL)
    return 0;
  if (requested_count == 0)
    return 0;
  s.candidate_count = 0;
  if (pack_type == 1 || pack_type == 2 || pack_type == 8 || pack_type == 7)
  {
    if (pack_type == 1)
      s.rarity_shift = 0;
    else if (pack_type == 2)
      s.rarity_shift = 4;
    else if (pack_type == 8)
      s.rarity_shift = 8;
    else
      s.rarity_shift = 12;
    if (rarity == 4)
    {
      s.minimum_rarity = 1;
      s.maximum_rarity = 3;
    }
    else if (rarity == 1)
    {
      s.minimum_rarity = 5;
      s.maximum_rarity = 8;
    }
    else if (rarity == 2)
    {
      s.minimum_rarity = 9;
      s.maximum_rarity = 9;
    }
    else
      rarity = (unsigned int)-1;
    for (s.card_index = 0, s.card = global_raw_cards_storage;
         s.card_index < global_available_slots; ++s.card_index, ++s.card)
    {
      s.card_rarity =
          (s.card->expansion_rarity & (15 << (unsigned char)s.rarity_shift)) >>
          (unsigned char)s.rarity_shift;
      if ((is_card_available_in_expansion(s.card->id, 0) ||
           is_card_available_in_expansion(s.card->id, 1) ||
           is_card_available_in_expansion(s.card->id, 2)) &&
          (pack_type == -1 || s.card_rarity != 0) &&
          (card_type == (unsigned int)-1 || s.card->card_type == card_type) &&
          (rarity == (unsigned int)-1 ||
           (s.minimum_rarity <= s.card_rarity &&
            s.card_rarity <= s.maximum_rarity)))
        s.candidates[s.candidate_count++] = s.card->id;
    }
  }
  else
  {
    if (pack_type == 0)
      s.expansion_bit = 11;
    else if (pack_type == 3)
      s.expansion_bit = 2;
    else if (pack_type == 4)
      s.expansion_bit = 1;
    else if (pack_type == 5)
      s.expansion_bit = 8;
    else if (pack_type == 6)
      s.expansion_bit = 5;
    else
      pack_type = -1;
    for (s.card_index = 0, s.card = global_raw_cards_storage;
         s.card_index < global_available_slots; ++s.card_index, ++s.card)
    {
      if ((is_card_available_in_expansion(s.card->id, 0) ||
           is_card_available_in_expansion(s.card->id, 1) ||
           is_card_available_in_expansion(s.card->id, 2)) &&
          (pack_type == -1 ||
           (s.card->expansion & (1 << s.expansion_bit)) != 0) &&
          (card_type == (unsigned int)-1 || s.card->card_type == card_type) &&
          (rarity == (unsigned int)-1 || s.card->rarity == rarity))
        s.candidates[s.candidate_count++] = s.card->id;
    }
  }
  if (s.candidate_count == 0)
    return 0;
  s.candidates_exhausted = 0;
  s.selected_count = 0;
  while (!s.candidates_exhausted && s.selected_count < requested_count)
  {
    s.card_index = rand() % s.candidate_count;
    if (s.candidates[s.card_index] == -1)
    {
      s.initial_index = s.card_index;
      do
      {
        ++s.card_index;
        if (s.card_index >= s.candidate_count)
          s.card_index = 0;
      } while (s.candidates[s.card_index] == -1 &&
               s.initial_index != s.card_index);
    }
    if (s.candidates[s.card_index] != -1)
    {
      cards[s.selected_count++] = s.candidates[s.card_index];
      if (allow_duplicates == 0)
        s.candidates[s.card_index] = -1;
    }
    else
      s.candidates_exhausted = 1;
  }
  return s.selected_count;
}

// FUNCTION: MAGIC 0x004c4065
int shell_generate_sealed_free_lands(int previous_color, int count,
                                     int *cards, int *first_color)
{
  struct
  {
    int card_index;
    int starting_color;
    int color;
    int land_ids[2000];
    int color_count;
  } s;

  if (cards == NULL)
    return 0;
  if (count == 0)
    return 0;
  s.color_count = 0;
  s.land_ids[s.color_count++] = 0xef;
  s.land_ids[s.color_count++] = 0xbc;
  s.land_ids[s.color_count++] = 0x5b;
  s.land_ids[s.color_count++] = 0x7e;
  s.land_ids[s.color_count++] = 0xa4;
  if (previous_color == -1)
    s.starting_color = rand() % s.color_count;
  else
    s.starting_color =
        (previous_color + 1 + rand() % (s.color_count - 1)) % s.color_count;
  s.color = s.starting_color;
  s.card_index = 0;
  while (s.card_index < count)
  {
    cards[s.card_index++] = s.land_ids[s.color];
    s.color = (s.color + 1) % s.color_count;
  }
  if (first_color != NULL)
    *first_color = s.starting_color;
  return s.card_index;
}

// FUNCTION: MAGIC 0x004c37ab
int shell_generate_sealed_starter(int pack_type, int *cards)
{
  struct
  {
    int result;
    int card_count;
    int picked_count;
    int requested_count;
    int rarity_count;
    int first_land_color;
  } s;

  if (cards == NULL)
    return 0;
  if (pack_type < 0 || pack_type >= 9)
    return 0;
  if (g_sealed_pack_definitions[pack_type].booster_count == 0)
    return 0;
  s.result = 1;
  s.card_count = 0;
  s.requested_count = 3;
  s.rarity_count = 0;
  while (s.rarity_count < s.requested_count && s.result != 0)
  {
    s.picked_count = shell_pick_sealed_cards(
        pack_type, 2, (unsigned int)-1, s.requested_count - s.rarity_count,
        0, cards + s.card_count);
    if (s.picked_count <= 0)
      s.result = 0;
    s.card_count += s.picked_count;
    s.rarity_count += s.picked_count;
  }
  s.requested_count = 11;
  s.card_count += shell_generate_sealed_free_lands(
      -1, s.requested_count, cards + s.card_count, &s.first_land_color);
  s.requested_count = 9;
  s.rarity_count = 0;
  while (s.rarity_count < s.requested_count && s.result != 0)
  {
    s.picked_count = shell_pick_sealed_cards(
        pack_type, 4, (unsigned int)-1, s.requested_count - s.rarity_count,
        0, cards + s.card_count);
    if (s.picked_count <= 0)
      s.result = 0;
    s.card_count += s.picked_count;
    s.rarity_count += s.picked_count;
  }
  s.requested_count = 11;
  s.card_count += shell_generate_sealed_free_lands(
      s.first_land_color, s.requested_count, cards + s.card_count, NULL);
  s.requested_count = 26;
  s.rarity_count = 0;
  while (s.rarity_count < s.requested_count && s.result != 0)
  {
    s.picked_count = shell_pick_sealed_cards(
        pack_type, 1, (unsigned int)-1, s.requested_count - s.rarity_count,
        0, cards + s.card_count);
    if (s.picked_count <= 0)
      s.result = 0;
    s.card_count += s.picked_count;
    s.rarity_count += s.picked_count;
  }
  if (s.card_count != 60)
    s.result = 0;
  return s.result;
}

// FUNCTION: MAGIC 0x004c39b7
int shell_generate_sealed_booster(int pack_type, int *cards, int *card_count)
{
  struct
  {
    int generated_count;
    int result;
    int picked_count;
    sealed_deck_starter_template_t *pack_template;
    int requested_count;
    int rarity_count;
  } s;

  if (cards == NULL)
    return 0;
  if (pack_type < 0 || pack_type >= 9)
    return 0;
  s.result = 1;
  s.generated_count = 0;
  s.pack_template = &g_sealed_starter_templates[
      g_sealed_pack_definitions[pack_type].free_lands];
  s.requested_count = s.pack_template->land_count;
  s.rarity_count = 0;
  if (s.requested_count != 0)
  {
    while (s.rarity_count < s.requested_count && s.result != 0)
    {
      s.picked_count = shell_pick_sealed_cards(
          pack_type, 1, (unsigned int)-1, s.requested_count - s.rarity_count,
          0, cards + s.generated_count);
      if (s.picked_count <= 0)
        s.result = 0;
      s.generated_count += s.picked_count;
      s.rarity_count += s.picked_count;
    }
  }
  s.requested_count = s.pack_template->spell_count;
  s.rarity_count = 0;
  if (s.requested_count != 0)
  {
    while (s.rarity_count < s.requested_count && s.result != 0)
    {
      s.picked_count = shell_pick_sealed_cards(
          pack_type, 4, (unsigned int)-1, s.requested_count - s.rarity_count,
          0, cards + s.generated_count);
      if (s.picked_count <= 0)
        s.result = 0;
      s.generated_count += s.picked_count;
      s.rarity_count += s.picked_count;
    }
  }
  s.requested_count = s.pack_template->allow_artifact_spells;
  s.rarity_count = 0;
  if (s.requested_count != 0)
  {
    while (s.rarity_count < s.requested_count &&
           s.result != 0)
    {
      s.picked_count = shell_pick_sealed_cards(
          pack_type, 2, (unsigned int)-1,
          s.requested_count - s.rarity_count,
          0, cards + s.generated_count);
      if (s.picked_count <= 0)
        s.result = 0;
      s.generated_count += s.picked_count;
      s.rarity_count += s.picked_count;
    }
  }
  if (card_count != NULL)
    *card_count = s.generated_count;
  return s.result;
}

// FUNCTION: MAGIC 0x004c5482
void shell_generate_sealed_packs(sealed_deck_pack_t *packs, int *cards,
                                int *card_count, int starter_count,
                                int *starter_types, int booster_count,
                                int *booster_types)
{
  struct
  {
    int generated_count;
    int card_index;
    int pack_index;
    int pack_count;
    int source_index;
    sealed_deck_pack_t *pack;
  } s;

  s.pack_count = 0;
  for (s.source_index = 0; s.source_index < starter_count; ++s.source_index)
  {
    s.pack = &packs[s.pack_count++];
    s.pack->is_starter = 1;
    s.pack->pack_type = starter_types[s.source_index];
    s.pack->variation = shell_choose_sealed_pack_variation(
        s.pack->pack_type, s.pack->is_starter);
    s.pack->opened = 0;
    s.pack->card_count = 60;
    shell_generate_sealed_starter(s.pack->pack_type, s.pack->cards);
  }
  for (s.source_index = 0; s.source_index < booster_count; ++s.source_index)
  {
    s.pack = &packs[s.pack_count++];
    s.pack->is_starter = 0;
    s.pack->pack_type = booster_types[s.source_index];
    s.pack->variation = shell_choose_sealed_pack_variation(
        s.pack->pack_type, s.pack->is_starter);
    s.pack->opened = 0;
    shell_generate_sealed_booster(s.pack->pack_type, s.pack->cards,
                                  &s.pack->card_count);
  }
  if (cards != NULL)
  {
    s.generated_count = 0;
    for (s.pack_index = 0; s.pack_index < s.pack_count; ++s.pack_index)
    {
      for (s.card_index = 0;
           s.card_index < packs[s.pack_index].card_count; ++s.card_index)
        cards[s.generated_count++] = *(packs[s.pack_index].cards + s.card_index);
    }
    if (card_count != NULL)
      *card_count = s.generated_count;
  }
}
