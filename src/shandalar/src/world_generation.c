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

// FUNCTION: SHANDALAR 0x004f6d90
void GenerateAdventureWorldMap(void)
{
  struct
  {
    int icon_y_scaled;
    int neighbor_is_open;           // ebp - 0x20
    int uVar1;                      // ebp - 0x1c
    int icon_width_scaled;          // ebp - 0x18
    int i;                          // ebp - 0x14
    int location_block_start_index; // ebp - 0x10
    unsigned int entry_index;       // ebp - 0xc
    int inner_index;                // ebp - 0x8
    int selected_state_sprite;      // ebp - 0x4
  } s;

start:

  InitializeAnimatedNoiseGrid();
  FillGraphicsRect(g_page2_window_bounds, 0, 0, 0x140, 200, 0);
  s.icon_y_scaled = 0;
  for (s.location_block_start_index = 0; s.location_block_start_index < 0x40; s.location_block_start_index++)
  {
    for (s.icon_width_scaled = 0; s.icon_width_scaled < 0x40; s.icon_width_scaled++)
    {
      if (((s.location_block_start_index < 2) || (s.icon_width_scaled < 2)) || ((0x3e <= s.location_block_start_index) || (0x3e <= s.icon_width_scaled)))
      {
        PutGraphicsPixel(g_page2_window_bounds, s.location_block_start_index, s.icon_width_scaled, 0);
        continue;
      }

      s.selected_state_sprite = (s.location_block_start_index + s.icon_width_scaled) * 3 - 0x20;
      s.inner_index = (s.icon_width_scaled - s.location_block_start_index) * 3 + 100;
      if ((s.selected_state_sprite < 4) || (s.inner_index < 4) || ((0x13c <= s.selected_state_sprite) || (0xc4 <= s.inner_index)))
      {
        PutGraphicsPixel(g_page2_window_bounds, s.location_block_start_index, s.icon_width_scaled, 0);
        continue;
      }

      s.uVar1 = CalculateWorldTerrainValue(s.location_block_start_index, s.icon_width_scaled);
      switch (s.uVar1 / 8)
      {
      case 0:
      case 1:
        s.entry_index = 0;
        break;
      case 2:
        s.entry_index = 1;
        if (0x16 <= s.uVar1)
        {
          s.entry_index = 8;
        }
        break;
      case 3:
        s.entry_index = 3;
        break;
      case 4:
        s.entry_index = 6;
        if (s.uVar1 < 0x22)
        {
          s.entry_index = 0xd;
        }
        break;
      case 5:
        if (0x2a >= s.uVar1)
        {
          s.entry_index = 10;
          break;
        }
      case 6:
        s.entry_index = 2;
        break;
      case 7:
        if (s.uVar1 >= 0x3c)
        {
          s.entry_index = 4;
        }
        else
        {
          s.entry_index = 0xc;
        }
        if (s.icon_width_scaled < 0x10 || s.icon_width_scaled > 0x30)
        {
          s.entry_index = 0xf;
        }

        if (s.uVar1 >= 0x3c)
        {
          s.entry_index = 5;
        }
        else
        {
          s.entry_index = 0xf;
        }

        break;

      case 8:
      case 9:
      case 10:
      case 11:
        s.entry_index = 5;
        break;
      }

      PutGraphicsPixel(g_page2_window_bounds, s.location_block_start_index, s.icon_width_scaled, (int)s.entry_index);
      if (s.entry_index != 0)
      {
        s.icon_y_scaled++;
      }
    }
  }

  if (s.icon_y_scaled < 0x6d6)
  {
    // TODO : Why didn't they just write this as do/while?
    goto start;
  }

  for (s.location_block_start_index = 1; s.location_block_start_index < 0x3f; s.location_block_start_index++)
  {
    for (s.icon_width_scaled = 1; s.icon_width_scaled < 0x3f; s.icon_width_scaled++)
    {
      s.entry_index = GetWorldTileType(s.location_block_start_index, s.icon_width_scaled);
      if (s.entry_index == 0)
      {
        s.neighbor_is_open = 0;
        for (s.i = 1; s.i <= 8; s.i += 2)
        {
          if (GetWorldTileType(s.location_block_start_index + g_neighbor_dx[s.i], s.icon_width_scaled + g_neighbor_dy[s.i]) == 0)
          {
            s.neighbor_is_open = 1;
            break;
          }
        }
        if (s.neighbor_is_open == 0)
        {
          PutGraphicsPixel(g_page2_window_bounds, s.location_block_start_index, s.icon_width_scaled, 6);
        }
      }
    }
  }

  PropagatePathConnectivity();
  if (GenerateWorldTownSlots() == 0)
  {
    goto start;
  }

  GenerateTownConnections();
  for (s.i = 0; s.i < 7; s.i++)
  {
    g_lair_or_monster_slots[s.i].respawn_timestamp = (4 - g_shandalar_difficulty) * s.i * -100;
  }
}

// FUNCTION: SHANDALAR 0x004f717a
int GenerateWorldTownSlots(void)
{
  struct
  {
    int random_timer;                       // ebp - 0x4c
    int world_magic_town_count;             // ebp - 0x48
    int generation_pass;                    // ebp - 0x44
    int placement_attempt_count;            // ebp - 0x40
    int nearest_town_distance;              // ebp - 0x3c
    unsigned int world_magic_mask;          // ebp - 0x38
    int valid_world;                        // ebp - 0x34
    int scan_index;                         // ebp - 0x30
    int magic_slot_or_trade_color_index;    // ebp - 0x2c
    int town_index;                         // ebp - 0x28
    int candidate_y;                        // ebp - 0x24
    int candidate_x;                        // ebp - 0x20
    int special_location_world_magic_index; // ebp - 0x1c
    unsigned int tile_type;                 // ebp - 0x18
    int nearest_city_distance;              // ebp - 0x14
    int distance;                           // ebp - 0x10
    int special_location_mask;              // ebp - 0xc
    int placement_slot_index;               // ebp - 0x8
    int major_town_count;                   // ebp - 0x4
  } s;

  s.generation_pass = 0;
generate_pass:
  if (4 < ++s.generation_pass)
  {
    return 0;
  }

  s.world_magic_town_count = 0;
  s.major_town_count = s.world_magic_town_count;
  s.special_location_mask = s.major_town_count;

  for (s.town_index = 0; s.town_index < 0xc; s.town_index = s.town_index + 1)
  {
    Scards[s.town_index].worldmagic_city = 0;
  }

  s.placement_slot_index = (int)clock() % 0x80;

  memset(g_town_slots, 0xff, 0x3200);

  for (s.town_index = 0; s.town_index < 0x80; s.town_index = s.town_index + 1)
  {
    s.placement_attempt_count = 0;
    do
    {
      s.valid_world = 0;
      s.candidate_x = internal_rand(0x40);
      s.candidate_y = internal_rand(0x40);
      s.tile_type = GetWorldTileType(s.candidate_x, s.candidate_y);
      if (s.tile_type == 0)
      {
      }
      else
      {
        s.nearest_town_distance = 0x7fff;
        s.nearest_city_distance = 0x7fff;
        for (s.scan_index = 0; s.scan_index < 0x80; s.scan_index = s.scan_index + 1)
        {
          if (g_town_slots[s.scan_index].world_x == -1)
          {
            continue;
          }

          s.distance = ApproximateDistance(s.candidate_x - g_town_slots[s.scan_index].world_x, s.candidate_y - g_town_slots[s.scan_index].world_y);
          if (s.distance < s.nearest_town_distance)
          {
            s.nearest_town_distance = s.distance;
          }
          if ((s.distance < s.nearest_city_distance) && (g_town_slots[s.scan_index].location_type == 3))
          {
            s.nearest_city_distance = s.distance;
          }
        }

        s.placement_attempt_count = s.placement_attempt_count + 1;
        if (7 - s.placement_attempt_count / 100 > s.nearest_town_distance)
        {
        }
        else
        {
          s.valid_world = 1;

          g_town_slots[s.placement_slot_index].world_x = s.candidate_x;
          g_town_slots[s.placement_slot_index].world_y = s.candidate_y;

          if (0x20 < s.nearest_city_distance)
          {
            g_town_slots[s.placement_slot_index].location_type = 3;
          }
          else if (0xa < s.nearest_town_distance)
          {
            g_town_slots[s.placement_slot_index].location_type = 2;
          }
          else
          {
            g_town_slots[s.placement_slot_index].location_type = 1;
          }

          g_town_slots[s.placement_slot_index].trade_color_and_type = 0;
          g_town_slots[s.placement_slot_index].status_and_ruling_wizard = g_town_slots[s.placement_slot_index].trade_color_and_type;

          for (s.scan_index = 0; s.scan_index < 8; s.scan_index = s.scan_index + 1)
          {
            g_town_slots[s.placement_slot_index].card_restock_timers[s.scan_index] = 0xfffffc18;
          }
          g_town_slots[s.placement_slot_index].quest_restock_timer = 0xfffffc18;
          g_town_slots[s.placement_slot_index].special_card_restock_timer = 0xfffffc18;

          s.special_location_world_magic_index = 0;
          if (s.tile_type == 3)
          {
            s.special_location_world_magic_index = 1;
          }
          if (s.tile_type == 1)
          {
            s.special_location_world_magic_index = 2;
          }
          if (s.tile_type == 2)
          {
            s.special_location_world_magic_index = 3;
          }
          if (s.tile_type == 5)
          {
            s.special_location_world_magic_index = 4;
          }
          if (s.tile_type == 6)
          {
            s.special_location_world_magic_index = 5;
          }

          if ((0x10 < s.nearest_city_distance) && (s.special_location_world_magic_index != 0) && ((s.special_location_mask & 1 << s.special_location_world_magic_index) == 0))
          {
            g_town_slots[s.placement_slot_index].location_type = 4;
            s.special_location_mask = s.special_location_mask | 1 << s.special_location_world_magic_index;
          }

          s.world_magic_mask = GetWorldTileMagicMask(s.tile_type);
          if ((s.placement_slot_index != 0) && ((g_town_slots[s.placement_slot_index].location_type == 3) || (g_town_slots[s.placement_slot_index].location_type == 2)))
          {
            for (s.scan_index = 0; s.scan_index < 99; s.scan_index = s.scan_index + 1)
            {
              s.magic_slot_or_trade_color_index = internal_rand(10) + 2;
              if (Scards[s.magic_slot_or_trade_color_index].worldmagic_city != 0)
              {
                continue;
              }

              if ((s.world_magic_mask & 1 << (char)(s.magic_slot_or_trade_color_index / 2)) == 0)
              {
                continue;
              }

              Scards[s.magic_slot_or_trade_color_index].worldmagic_city = s.placement_slot_index;
              break;
            }

            if (0x63 <= s.scan_index)
            {
              s.random_timer = internal_rand(2);
              Scards[s.random_timer].worldmagic_city = s.placement_slot_index;
            }

            if (s.world_magic_town_count < 10)
            {
              g_town_slots[s.placement_slot_index].status_and_ruling_wizard = g_town_slots[s.placement_slot_index].status_and_ruling_wizard | 1;

              s.world_magic_town_count++;
            }
          }

          if (s.valid_world != 0)
          {
            SetWorldMapPixelFlags(0x10, s.candidate_x, s.candidate_y);
          }
          s.placement_slot_index = (s.placement_slot_index + s.placement_slot_index * 4 + 1) % 0x80;
        }
      }
    } while (s.valid_world == 0);

    if (1 < g_town_slots[s.town_index].location_type)
    {
      s.major_town_count = s.major_town_count + 1;
    }
  }

  s.valid_world = 1;
  if ((s.special_location_mask != 0x3e) || (s.major_town_count < 0x1e))
  {
    s.valid_world = 0;
  }

  for (s.magic_slot_or_trade_color_index = 0; s.magic_slot_or_trade_color_index < 6; s.magic_slot_or_trade_color_index = s.magic_slot_or_trade_color_index + 1)
  {
    do
    {
      do
      {
        s.town_index = internal_rand(0x80);
      } while (g_town_slots[s.town_index].location_type <= 1);
    } while ((g_town_slots[s.town_index].location_type == 4) || (g_town_slots[s.town_index].trade_color_and_type != 0));
    g_town_slots[s.town_index].trade_color_and_type = 1 << (char)s.magic_slot_or_trade_color_index;
  }

  s.scan_index = internal_rand(0xc);
  for (s.town_index = 0; s.town_index < 0x80; s.town_index = s.town_index + 1)
  {
    if ((1 < g_town_slots[s.town_index].location_type) && (g_town_slots[s.town_index].location_type < 4))
    {
      if ((s.scan_index & 1) != 0)
      {
        g_town_slots[s.town_index].trade_color_and_type = 1 << (char)((s.scan_index % 0xc) / 2);
      }
      else
      {
        g_town_slots[s.town_index].trade_color_and_type = (((s.scan_index % 10) / 2) + 1) * 0x100;
      }
      s.scan_index = s.scan_index + 1;
    }
  }

  for (s.town_index = 0; s.town_index < 0xc; s.town_index = s.town_index + 1)
  {
    if (Scards[s.town_index].worldmagic_city == 0)
    {
      s.valid_world = 0;
    }
    if ((g_world_magic_bitmap & 1 << (char)s.town_index) != 0)
    {
      Scards[s.town_index].worldmagic_city = 0;
    }
  }

  if (s.valid_world == 0)
  {
    for (s.town_index = 0; s.town_index < 0x80; s.town_index = s.town_index + 1)
    {
      ClearWorldMapPixelFlags(0x10, g_town_slots[s.town_index].world_x, g_town_slots[s.town_index].world_y);
    }

    for (s.town_index = 0; s.town_index < 0xc; s.town_index = s.town_index + 1)
    {
      Scards[s.town_index].worldmagic_city = 0;
    }
    goto generate_pass;
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x004f78d3
void GenerateTownConnections(void)
{
  struct
  {
    int connection_index; /* -0x24 */
    int best_dist;        /* -0x20 */
    int candidate_index;  /* -0x1c */
    int town_index;       /* -0x18 */
    int sample_index;     /* -0x14 */
    int distance;         /* -0x10 */
    int chosen_index;     /* -0x0c */
    int best_index;       /* -0x08 */
    int attempts;         /* -0x04 */
  } s;

  for (s.town_index = 0; s.town_index < 0x80; s.town_index = s.town_index + 1)
  {
    for (s.connection_index = 0; s.connection_index < g_town_slots[s.town_index].location_type; s.connection_index = s.connection_index + 1)
    {
      s.attempts = 0;
    retry:
      s.best_dist = 0x7fff;
      s.sample_index = 0;
      for (; s.sample_index < 0x2a; s.sample_index = s.sample_index + 1)
      {
        s.candidate_index = internal_rand(0x80);
        s.distance =
            ApproximateDistance(g_town_slots[s.town_index].world_x - g_town_slots[s.candidate_index].world_x,
                                g_town_slots[s.town_index].world_y - g_town_slots[s.candidate_index].world_y);
        if (s.distance < s.best_dist)
        {
          s.best_dist = s.distance;
          s.chosen_index = s.best_index;
          s.best_index = s.candidate_index;
        }
      }

      if (CreateTownConnectionPath(g_town_slots[s.town_index].world_x, g_town_slots[s.town_index].world_y,
                                   g_town_slots[s.chosen_index].world_x, g_town_slots[s.chosen_index].world_y) == 0)
      {
        if (++s.attempts < 3)
        {
          goto retry;
        }
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x004f7a3c
int CreateTownConnectionPath(int start_x, int start_y, int target_x, int target_y)
{
  struct
  {
    int step_count;      /* -0x30 */
    int best_direction;  /* -0x2c */
    int best_score;      /* -0x28 */
    int flags_masked;    /* -0x24 */
    int direction_index; /* -0x20 */
    int cur_y;           /* -0x1c */
    int cur_x;           /* -0x18 */
    int target_dist;     /* -0x14 */
    int next_y;          /* -0x10 */
    int next_x;          /* -0x0c */
    int tile_type;       /* -0x08 */
    int score;           /* -0x04 */
  } s;

  s.cur_x = start_x;
  s.cur_y = start_y;
  s.step_count = 0;

  BlitGraphicsRect(g_page2_window_bounds, 0, 0, 0x40, 0x80, g_page2_window_bounds, 0x40, 0);
  SetWorldMapPixelFlags(0x20, s.cur_x, s.cur_y);

  do
  {
    s.target_dist = ApproximateDistance(target_x - s.cur_x, target_y - s.cur_y);
    s.best_direction = -1;
    s.best_score = 0x7fff;

    for (s.direction_index = 1; s.direction_index <= 8; s.direction_index = s.direction_index + 1)
    {
      s.next_x = g_neighbor_dx[s.direction_index] + s.cur_x;
      s.next_y = g_neighbor_dy[s.direction_index] + s.cur_y;

      s.tile_type = GetWorldTileType(s.next_x, s.next_y);
      if (s.tile_type == 0)
      {
        continue;
      }

      s.score = ApproximateDistance(target_x - s.next_x, target_y - s.next_y);
      if (s.target_dist <= s.score)
      {
        continue;
      }

      if ((s.tile_type == 2) || (s.tile_type == 0xb))
      {
        s.score = s.score + 1;
      }
      if ((s.tile_type == 4) || (s.tile_type == 5))
      {
        s.score = s.score + 4;
      }

      if ((GetWorldMapPixelFlags(s.next_x, s.next_y) & 0x20) != 0)
      {
        if (s.step_count >= 1)
        {
          s.score = s.score - 4;
        }
      }

      if (s.best_score > s.score)
      {
        s.best_score = s.score;
        s.best_direction = s.direction_index;
      }
    }

    if (s.best_direction == -1)
    {
      BlitGraphicsRect(g_page2_window_bounds, 0x40, 0, 0x40, 0x80, g_page2_window_bounds, 0, 0);
      return 0;
    }

    s.next_x = g_neighbor_dx[s.best_direction] + s.cur_x;
    s.next_y = g_neighbor_dy[s.best_direction] + s.cur_y;
    s.flags_masked = GetWorldMapPixelFlags(s.next_x, s.next_y) & 0x20;

    MarkPathConnection(s.cur_x, s.cur_y, s.best_direction);

    if ((s.flags_masked != 0) && (s.step_count >= 1))
    {
      return 1;
    }

    s.cur_x = s.next_x;
    s.cur_y = s.next_y;
    s.step_count = s.step_count + 1;
  } while ((s.cur_x != target_x) || (s.cur_y != target_y));

  return 1;
}

// FUNCTION: SHANDALAR 0x004f7c7d
void PropagatePathConnectivity(void)
{
  struct
  {
    int scan_x;       /* -0x14 */
    int y;            /* -0x10 */
    int x;            /* -0x0c */
    int found_change; /* -0x08 */
    int scan_y;       /* -0x04 */
  } s;

  PutGraphicsPixel(g_page2_window_bounds, 0xa8, 0x58, 0xff);
  do
  {
    s.found_change = 0;
    for (s.x = 4; s.x < 0x40; s.x = s.x + 4)
    {
      for (s.y = 4; s.y < 0x40; s.y = s.y + 4)
      {
        if (ReadGraphicsPixel(g_page2_window_bounds->page_number, s.x + 0x80, s.y + 0x40) != 0)
        {
          FillGraphicsRect(g_page2_window_bounds, 0x40, 0, 0x40, 0x40, 0);
          FloodFillPathConnectivity(s.x, s.y, 8);
          PutGraphicsPixel(g_page2_window_bounds, s.x + 0x80, s.y + 0x40, 0);

          for (s.scan_x = 0; s.scan_x < 0x40; s.scan_x = s.scan_x + 1)
          {
            for (s.scan_y = 0; s.scan_y < 0x40; s.scan_y = s.scan_y + 1)
            {
              if (ReadGraphicsPixel(g_page2_window_bounds->page_number, s.scan_x + 0x40, s.scan_y) != 0)
              {
                if (ReadGraphicsPixel(g_page2_window_bounds->page_number, s.scan_x + 0x80, s.scan_y) == 0)
                {
                  s.found_change = 1;
                  PutGraphicsPixel(g_page2_window_bounds, s.scan_x + 0x80, s.scan_y, 0xff);
                  PutGraphicsPixel(g_page2_window_bounds, s.scan_x + 0x80, s.scan_y + 0x40, 0xff);
                }
              }
            }
          }
        }
      }
    }
  } while (s.found_change != 0);

  for (s.x = 0; s.x < 0x40; s.x = s.x + 1)
  {
    for (s.y = 0; s.y < 0x40; s.y = s.y + 1)
    {
      if (ReadGraphicsPixel(g_page2_window_bounds->page_number, s.x + 0x80, s.y) == 0)
      {
        PutGraphicsPixel(g_page2_window_bounds, s.x, s.y, 0);
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x004f7eb2
void FloodFillPathConnectivity(int x, int y, unsigned int depth)
{
  struct
  {
    char tile_value; /* -0x10 */
    char pad_0d[3];
    char direction_index; /* -0x0c */
    char pad_09[3];
    char next_y; /* -0x08 */
    char pad_05[3];
    char next_x; /* -0x04 */
    char pad_01[3];
  } s;

  PutGraphicsPixel(g_page2_window_bounds, x + 0x40, y, depth);
  if ((int)depth <= 1)
  {
  }
  else
  {
    for (s.direction_index = 1; (int)s.direction_index <= 8; s.direction_index = s.direction_index + 2)
    {
      s.next_x = (char)(g_neighbor_dx[(int)s.direction_index] + x);
      s.next_y = (char)(g_neighbor_dy[(int)s.direction_index] + y);
      s.tile_value = (char)ReadGraphicsPixel(g_page2_window_bounds->page_number, s.next_x + 0x40, s.next_y);
      if ((int)s.tile_value < (int)depth)
      {
        if (ReadGraphicsPixel(g_page2_window_bounds->page_number, s.next_x, s.next_y) != 0)
        {
          FloodFillPathConnectivity(s.next_x, s.next_y, depth - 1);
        }
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x004f7fb9
int CalculateWorldTerrainValue(int x, int y)
{
  int edge_penalty;
  int terrain_value;

  edge_penalty = 0x10 / (x + 2);
  edge_penalty = edge_penalty + 0x10 / (y + 2);
  edge_penalty = edge_penalty + 0x10 / (0x41 - x);
  edge_penalty = edge_penalty + 0x10 / (0x41 - y);

  edge_penalty = abs(x - 0x20) + abs(y - 0x20);
  edge_penalty = (edge_penalty * edge_penalty) / 512;
  edge_penalty += abs(x - y) / 16;

  x = x << 5;
  y = y << 5;
  terrain_value = SampleAnimatedNoiseGridBilinear(x, y) << 2;

  terrain_value = terrain_value + SampleAnimatedNoiseGridBilinear(x << 3, y << 3) * 2;
  terrain_value = terrain_value + SampleAnimatedNoiseGridBilinear(x << 4, y << 4);
  terrain_value = terrain_value - ClampIntToRange(edge_penalty, 0, 0xc) * 0x200;

  return ClampIntToRange(((7 * terrain_value) / 64) / 4, 0, 100);
}

// FUNCTION: SHANDALAR 0x004f8101
void InitializeAnimatedNoiseGrid(void)
{
  struct
  {
    int weighted_neighbor_sum;
    int x;
    int direction_index;
    int y;
  } s;

  for (s.y = 0; s.y < 0x12; s.y = s.y + 1)
  {
    for (s.x = 0; s.x < 0x12; s.x = s.x + 1)
    {
      g_animated_noise_grid[0].samples[s.y][s.x] = (char)internal_rand(0x10);
    }
    g_animated_noise_grid[0].samples[s.y][0x12] = g_animated_noise_grid[0].samples[s.y][0];
  }
  for (s.x = 0; s.x < 0x12; s.x = s.x + 1)
  {
    g_animated_noise_grid[0].samples[0x12][s.x] = g_animated_noise_grid[0].samples[0][s.x];
  }
  for (s.y = 0; s.y <= 0x10; s.y = s.y + 1)
  {
    for (s.x = 0; s.x <= 0x10; s.x = s.x + 1)
    {
      s.weighted_neighbor_sum = (int)g_animated_noise_grid[0].samples[s.y][s.x] << 2;
      for (s.direction_index = 1; s.direction_index <= 8; s.direction_index = s.direction_index + 1)
      {
        s.weighted_neighbor_sum +=
            (int)g_animated_noise_grid[0].samples[s.y + g_neighbor_dx[s.direction_index] + 1][s.x + g_neighbor_dy[s.direction_index] + 1];
      }
      g_animated_noise_grid[1].samples[s.y][s.x] = g_animated_noise_grid[0].samples[s.y][s.x];
    }
  }
  for (s.y = 0; s.y <= 0x10; s.y = s.y + 1)
  {
    g_animated_noise_grid[1].samples[s.y][0x10] = g_animated_noise_grid[1].samples[s.y][0];
  }
  for (s.x = 0; s.x <= 0x10; s.x = s.x + 1)
  {
    g_animated_noise_grid[1].samples[0x10][s.x] = g_animated_noise_grid[1].samples[0][s.x];
  }
}

// FUNCTION: SHANDALAR 0x004f82f2
int SampleAnimatedNoiseGridBilinear(int x_fixed, int y_fixed)
{
  int weighted_sample;
  unsigned int entry_index;
  unsigned int inner_index;
  unsigned int location_block_start_index;
  unsigned int selected_state_sprite;

  x_fixed = x_fixed - 0x80;
  y_fixed = y_fixed - 0x80;
  entry_index = (x_fixed >> 8) & 0xf;
  location_block_start_index = (x_fixed & 0xff) / 8;
  inner_index = (y_fixed >> 8) & 0xf;
  selected_state_sprite = (y_fixed & 0xff) / 8;

  weighted_sample = (int)g_animated_noise_grid[1].samples[entry_index][inner_index] * (0x20 - selected_state_sprite) * (0x20 - location_block_start_index);
  weighted_sample = weighted_sample + (int)g_animated_noise_grid[1].samples[entry_index + 1][inner_index] * (0x20 - selected_state_sprite) * location_block_start_index;
  weighted_sample = weighted_sample + (int)g_animated_noise_grid[1].samples[entry_index][inner_index + 1] * (0x20 - location_block_start_index) * selected_state_sprite;
  weighted_sample = weighted_sample + (int)g_animated_noise_grid[1].samples[entry_index + 1][inner_index + 1] * selected_state_sprite * location_block_start_index;
  return weighted_sample / 32;
}

// FUNCTION: SHANDALAR 0x005081fa
void InitializeCastleDungeonSlots(void)
{
  struct
  {
    int card_index;
    int nearest_distance;
    int nearest_town_index;
    int scan_index;
    int candidate_y;
    int candidate_x;
    int distance;
    int mana_castle_town_index;
    int inner_index;
    int entry_index;
  } s;

  for (s.entry_index = 0; s.entry_index < 0xf; s.entry_index = s.entry_index + 1)
  {
    g_castle_dungeon_slots[s.entry_index].card_slot_3 = -1;
    g_castle_dungeon_slots[s.entry_index].card_slot_2 = g_castle_dungeon_slots[s.entry_index].card_slot_3;
    g_castle_dungeon_slots[s.entry_index].card_slot_1 = g_castle_dungeon_slots[s.entry_index].card_slot_2;
    g_castle_dungeon_slots[s.entry_index].times_entered = -1;
    g_castle_dungeon_slots[s.entry_index].reserved_2c = g_castle_dungeon_slots[s.entry_index].times_entered;
  }

  s.entry_index = 0;
  for (s.card_index = 0; s.card_index < g_card_count - 0x39; s.card_index = s.card_index + 1)
  {
    if ((global_cards_data[s.card_index].extra_ability & 0x100) == 0)
    {
      continue;
    }

    do
    {
      s.entry_index = internal_rand(10) + 5;
    } while (g_castle_dungeon_slots[s.entry_index].card_slot_3 != -1);

    if (g_castle_dungeon_slots[s.entry_index].card_slot_1 == -1)
    {
      g_castle_dungeon_slots[s.entry_index].card_slot_1 = s.card_index;
    }
    else if (g_castle_dungeon_slots[s.entry_index].card_slot_2 == -1)
    {
      g_castle_dungeon_slots[s.entry_index].card_slot_2 = s.card_index;
    }
    else
    {
      g_castle_dungeon_slots[s.entry_index].card_slot_3 = s.card_index;
    }
  }

  for (s.entry_index = 0; s.entry_index < 0xf; s.entry_index = s.entry_index + 1)
  {
    do
    {
      do
      {
        do
        {
          s.candidate_x = internal_rand(0x40);
          s.candidate_y = internal_rand(0x40);
        } while (GetWorldTileType(s.candidate_x, s.candidate_y) == 0);
      } while ((GetWorldMapPixelFlags(s.candidate_x, s.candidate_y) & 0x30) != 0);

      s.nearest_distance = 0xff;
      for (s.scan_index = 0; s.scan_index < 0x80; s.scan_index = s.scan_index + 1)
      {
        if ((s.entry_index < 5) && (g_town_slots[s.scan_index].location_type == 4))
        {
          if (GetWorldTileMagicMask(GetWorldTileType(g_town_slots[s.scan_index].world_x, g_town_slots[s.scan_index].world_y)) == 1 << (s.entry_index + 1))
          {
            s.mana_castle_town_index = s.scan_index;
          }
        }

        if ((g_town_slots[s.scan_index].location_type <= 1) || (g_town_slots[s.scan_index].location_type == 4))
        {
          continue;
        }

        s.distance = ApproximateDistance(g_town_slots[s.scan_index].world_x - s.candidate_x,
                                         g_town_slots[s.scan_index].world_y - s.candidate_y);
        if (s.nearest_distance > s.distance)
        {
          s.nearest_distance = s.distance;
          s.nearest_town_index = s.scan_index;
        }
      }

      for (s.scan_index = 0; s.scan_index < s.entry_index; s.scan_index = s.scan_index + 1)
      {
        s.distance = ApproximateDistance(g_castle_dungeon_slots[s.scan_index].world_x - s.candidate_x, g_castle_dungeon_slots[s.scan_index].world_y - s.candidate_y);
        if (s.nearest_distance > s.distance)
        {
          s.nearest_distance = s.distance;
        }
      }
    } while (s.nearest_distance < 4);

    SetWorldMapPixelFlags(0x40, s.candidate_x, s.candidate_y);
    g_castle_dungeon_slots[s.entry_index].world_x = s.candidate_x;
    g_castle_dungeon_slots[s.entry_index].world_y = s.candidate_y;
    g_castle_dungeon_slots[s.entry_index].north_of_town_index = s.nearest_town_index;
    g_castle_dungeon_slots[s.entry_index].color = (unsigned char)(internal_rand(5) + 1);
    g_castle_dungeon_slots[s.entry_index].monster_flags = 2;

    if (s.entry_index < 5)
    {
      g_castle_dungeon_slots[s.entry_index].color = s.entry_index + 1;
      g_castle_dungeon_slots[s.entry_index].monster_flags = 0x81;
      g_castle_dungeon_slots[s.entry_index].world_x = g_town_slots[s.mana_castle_town_index].world_x;
      g_castle_dungeon_slots[s.entry_index].world_y = g_town_slots[s.mana_castle_town_index].world_y;
    }

    s.inner_index = 0;
    if ((int)(unsigned int)(unsigned char)g_castle_dungeon_slots[s.entry_index].monster_flags <= internal_rand(2) + 1)
    {
      if (1 < (int)(unsigned int)(unsigned char)g_castle_dungeon_slots[s.entry_index].monster_flags)
      {
        s.inner_index = 0x1c;
      }
      else
      {
        s.inner_index = 0x10;
      }
      g_castle_dungeon_slots[s.entry_index].monster_flags = g_castle_dungeon_slots[s.entry_index].monster_flags | 0x80;
    }
    else
    {
      s.inner_index = 0;
      for (s.scan_index = 0; s.scan_index < (int)(unsigned int)(unsigned char)g_castle_dungeon_slots[s.entry_index].monster_flags; s.scan_index = s.scan_index + 1)
      {
        s.inner_index = s.inner_index + s.scan_index * 2 + 4;
      }
    }

    if (g_castle_dungeon_slots[s.entry_index].card_slot_2 == -1)
    {
      s.inner_index = (s.inner_index * 3) / 2;
    }
    if (g_castle_dungeon_slots[s.entry_index].card_slot_3 != -1)
    {
      s.inner_index = (s.inner_index * 2) / 3;
    }

    g_castle_dungeon_slots[s.entry_index].card_in_effect =
        g_dungeon_card_effect_by_monster_flags[((g_castle_dungeon_slots[s.entry_index].monster_flags & 0xc0) != 0 ? 4 : 0) +
                                               (g_castle_dungeon_slots[s.entry_index].monster_flags & 0x7f) + internal_rand(2)];
    g_castle_dungeon_slots[s.entry_index].rules_bitmap = 1;

    switch (s.inner_index / 4)
    {
    case 0:
    case 1:
    case 2:
      g_castle_dungeon_slots[s.entry_index].monster_flags++;
    case 3:
      g_castle_dungeon_slots[s.entry_index].card_in_effect = g_dungeon_card_effect_by_color[(char)g_castle_dungeon_slots[s.entry_index].color];
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap | 1 << (char)(internal_rand(5) + 4);
      break;
    case 4:
      g_castle_dungeon_slots[s.entry_index].card_in_effect = g_dungeon_card_effect_by_color[(char)g_castle_dungeon_slots[s.entry_index].color];
      break;
    case 5:
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap | 1 << (char)(internal_rand(5) + 4);
      g_castle_dungeon_slots[s.entry_index].card_in_effect = -1;
      break;
    case 6:
      g_castle_dungeon_slots[s.entry_index].card_in_effect = -1;
      break;
    }

    if (((g_castle_dungeon_slots[s.entry_index].rules_bitmap & 0x100) != 0) ||
        (((g_castle_dungeon_slots[s.entry_index].monster_flags & 0x3f) < 2) && ((g_castle_dungeon_slots[s.entry_index].rules_bitmap & 0x20) != 0)))
    {
      g_castle_dungeon_slots[s.entry_index].card_in_effect = g_dungeon_card_effect_by_color[(char)g_castle_dungeon_slots[s.entry_index].color];
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap & 0xfffffedf;
    }

    if ((g_castle_dungeon_slots[s.entry_index].monster_flags & 0x7f) == 1)
    {
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap & 0xfffffffe;
    }

    if (s.entry_index < 5)
    {
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap | 1;
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap | 2;
      g_castle_dungeon_slots[s.entry_index].card_in_effect = g_mana_castle_card_effect_by_difficulty[s.entry_index][g_shandalar_difficulty];
    }
  }
}

// FUNCTION: SHANDALAR 0x0043146b
unsigned int GetWorldTileType(int x, int y)
{
  if ((0x40 <= x) || (x < 0))
  {
    x = 0;
  }
  if ((0x40 <= y) || (y < 0))
  {
    y = 0;
  }
  return GetGraphicsPixelColorRef(g_page2_window_bounds, x, y) & 0xf;
}

// FUNCTION: SHANDALAR 0x005611c8
unsigned int GetWorldTileMagicMask(unsigned int tile_type)
{
  unsigned int entry_index;

  switch (tile_type)
  {
  case 3:
    entry_index = 2;
    break;
  case 0xb:
    entry_index = 10;
    break;
  case 1:
    entry_index = 4;
    break;
  case 2:
    entry_index = 8;
    break;
  case 4:
    entry_index = 0x30;
    break;
  case 5:
    entry_index = 0x10;
    break;
  case 6:
    entry_index = 0x20;
    break;
  case 10:
    entry_index = 0x28;
    break;
  case 0xf:
    entry_index = 0x18;
    break;
  case 7:
    entry_index = 0x24;
    break;
  case 0xc:
    entry_index = 0x12;
    break;
  case 0xd:
    entry_index = 0x22;
    break;
  case 0xe:
    entry_index = 0xc;
    break;
  case 9:
    entry_index = 0x14;
    break;
  case 8:
    entry_index = 6;
    break;
  default:
    entry_index = 0;
    break;
  }

  return entry_index;
}

// FUNCTION: SHANDALAR 0x004314ca
unsigned int GetWorldMapPixelFlags(int x, int y)
{
  if (x >= 0x40 || x < 0)
    return 0;

  if (y >= 0x40 || y < 0)
    return 0;

  return GetGraphicsPixelColorRef(g_page2_window_bounds, x, y);
}

// FUNCTION: SHANDALAR 0x00431526
void SetWorldMapPixelFlags(unsigned int mask, int x, int y)
{
  if (x >= 0x40 || x < 0)
    return;

  if (y >= 0x40 || y < 0)
    return;

  PutGraphicsPixel(g_page2_window_bounds, x, y, GetGraphicsPixelColorRef(g_page2_window_bounds, x, y) | mask);
}

// FUNCTION: SHANDALAR 0x00431593
void ClearWorldMapPixelFlags(unsigned int mask, int x, int y)
{
  if (x >= 0x40 || x < 0)
    return;

  if (y >= 0x40 || y < 0)
    return;

  PutGraphicsPixel(g_page2_window_bounds, x, y, GetGraphicsPixelColorRef(g_page2_window_bounds, x, y) & ~mask);
}

// FUNCTION: SHANDALAR 0x0043174d
void MarkPathConnection(int x, int y, int direction_index)
{
  if (x >= 0x40 || x < 0)
    return;

  if (y >= 0x40 || y < 0)
    return;

  PutGraphicsPixel(g_page2_window_bounds, x, y + 0x40,
                   GetGraphicsPixelColorRef(g_page2_window_bounds, x, y + 0x40) | (1 << (direction_index - 1)));
  SetWorldMapPixelFlags(0x20, x, y);

  x += g_neighbor_dx[direction_index];
  y += g_neighbor_dy[direction_index];
  direction_index = ((direction_index + 3) & 7) + 1;

  PutGraphicsPixel(g_page2_window_bounds, x, y + 0x40,
                   GetGraphicsPixelColorRef(g_page2_window_bounds, x, y + 0x40) | (1 << (direction_index - 1)));
  SetWorldMapPixelFlags(0x20, x, y);
}

// FUNCTION: SHANDALAR 0x00431859
unsigned int WorldRoadTileHasDirection(int tile_x, int tile_y, int direction_index)
{
  if ((tile_x >= 0x40) || (tile_x < 0))
  {
    return 0;
  }

  if ((tile_y >= 0x40) || (tile_y < 0))
  {
    return 0;
  }

  return GetGraphicsPixelColorRef(g_page2_window_bounds, tile_x, tile_y + 0x40) & (1U << ((unsigned char)(direction_index - 1U)));
}

