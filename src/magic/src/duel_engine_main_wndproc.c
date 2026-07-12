#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <string.h>

#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "defs.h"
#include "magic/src/duel_engine.h"
#include "magic/src/game_support.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"
#include "drawcardlib/src/pic.h"

#ifndef SHANDALAR
extern HWND global_main_hwnd;
#define DUEL_SHELL_WINDOW_HWND g_duel_parent_window_hwnd
#define DUEL_MAIN_WINDOW_HWND global_main_hwnd
#else
extern HWND g_main_window_hwnd;
#define DUEL_SHELL_WINDOW_HWND g_main_window_hwnd
#define DUEL_MAIN_WINDOW_HWND g_main_window_hwnd
#endif

extern int life[2];
extern HINSTANCE g_app_instance;
extern HWND DAT_008a8dec;
extern HWND DAT_008a8d78;
extern int DAT_0091c998;
extern CRITICAL_SECTION g_card_render_lock;
void checked_DeleteDC_DeleteObject(HDC dc, HGDIOBJ obj);
void set_duel_prompt_context(HWND hwnd, char *prompt, unsigned int mode);
int FUN_004a09c6(int player, char *prompt, int maxnum);
unsigned int load_gametype0(char *path);
int play_duel(int player, int creature_type);
void FUN_00464b6f(void);
void FUN_0043de00(void);
void FUN_004458d4(void);
void FUN_004ecdc0(int player);
void FUN_004ef17b(char *path);
void FUN_004ef35a(char *path);
void FUN_004ef54b(char *path);
int FUN_0044b56a(void);
void FUN_0044a7e0(unsigned int player);
int CountDuelPoolEligibleTowns(void);
char *FUN_004e1c4f(int creature_type);
void FUN_004c0a0a(void);
void FUN_004b559a(int color);
int FUN_004b5416(void);
void FUN_00464700(int wizard_color);
void FUN_0046476f(int player, int internal_card_id);
int DrawRandomCardFromInitialLibrary(int library_index);
void FUN_004b3dee(int player, int wizard_color);
void FUN_004b3cc2(int *redraw_result, int *ante_result, int *ante_info);
void FUN_004996d0(unsigned int *starting_player,
                  unsigned int *mulligan_accepted,
                  unsigned int starting_player_value,
                  int random_starting_player,
                  int opponent_ante_card,
                  int player_ante_card,
                  int redraw_result,
                  int ante_result,
                  int ante_info,
                  int *out_flag);
void TENTATIVE_reassess_all_cards();
void append_to_trace_txt(char *text);
void AddCardToCLPacket(int card_in_packet);
int GetCardFromCLPacket(int packet_index);
extern int DAT_008ced00[16];
#ifdef SHANDALAR
int DAT_007a79b8;
#else
extern int DAT_007a79b8;
#endif
#ifndef SHANDALAR
extern HANDLE global_mutex_GameInit;
#endif

// GLOBAL: MAGIC 0x008a9140
OPENFILENAMEA g_duel_save_game_openfilename;

// GLOBAL: MAGIC 0x00746368
void *g_duel_player_face_pic;

// GLOBAL: MAGIC 0x00579f28
// GLOBAL: SHANDALAR 0x0058f658
char s_Congratulations_00579f28[0x14] = "Congratulations!";

// GLOBAL: MAGIC 0x00579f3c
// GLOBAL: SHANDALAR 0x0058f66c
char s_Too_bad_00579f3c[8] = "Too bad";

// GLOBAL: MAGIC 0x00579f44
// GLOBAL: SHANDALAR 0x0058f674
char s_Oh_well_00579f44[0xc] = "Oh well...";

// GLOBAL: MAGIC 0x00579f50
// GLOBAL: SHANDALAR 0x0058f680
char s_Want_to_play_again_00579f50[0x18] = "\n\nWant to play again?";

// GLOBAL: MAGIC 0x00579f68
// GLOBAL: SHANDALAR 0x0058f698
char s_End_of_duel_00579f68[0xc] = "End of duel";

// GLOBAL: MAGIC 0x005732b4
char s_Starting_the_duel_005732b4[0x14] = "Starting the duel.\n";

// GLOBAL: MAGIC 0x0091c9a4
// GLOBAL: SHANDALAR 0x00930ae4
HANDLE g_duel_thread_handle;

// GLOBAL: MAGIC 0x00579f24
// GLOBAL: SHANDALAR 0x0058f654
int g_duel_main_window_minimized;

// GLOBAL: MAGIC 0x008b3bcc
// GLOBAL: SHANDALAR 0x008c7d4c
HWND g_duel_palette_window_hwnd;

// GLOBAL: MAGIC 0x008b3274
// GLOBAL: SHANDALAR 0x008c7424
HWND g_duel_player_battlefield_window_hwnd;

// GLOBAL: MAGIC 0x008cc700
// GLOBAL: SHANDALAR 0x008e0880
int g_duel_modal_action_active;

// GLOBAL: MAGIC 0x008ce504
int DAT_008ce504;

// GLOBAL: MAGIC 0x00777aa4
// GLOBAL: SHANDALAR 0x0078e834
int g_duel_selected_player_card;

// GLOBAL: MAGIC 0x008a8d70
// GLOBAL: SHANDALAR 0x008bcf70
int g_duel_selected_opponent_card;

// GLOBAL: MAGIC 0x008b294c
// GLOBAL: SHANDALAR 0x008c6afc
int g_duel_selection_pending;

extern int g_duel_cached_life_player_1;
extern int g_duel_cached_life_player_0;
extern int g_duel_cached_state_007abce4;
extern int g_duel_cached_state_007abce0;
extern int g_duel_cached_unk_008b44d0_player_1;
extern int g_duel_cached_unk_008b44d0_player_0;

// GLOBAL: MAGIC 0x008cd920
HWND g_duel_life_status_window_1_hwnd;

// GLOBAL: MAGIC 0x00938e28
HWND g_duel_life_status_window_2_hwnd;

// GLOBAL: MAGIC 0x0094ca2c
HWND g_duel_toggleable_status_window_hwnd;

extern int g_duel_cached_library_count_player_1;
extern int g_duel_cached_library_count_player_0;
extern int g_duel_cached_graveyard_count_player_1;
extern int g_duel_cached_graveyard_count_player_0;
extern int g_duel_cached_exile_count_player_1;
extern int g_duel_cached_exile_count_player_0;
extern HWND g_duel_prompt_context_hwnd;

// GLOBAL: MAGIC 0x008b4370
// GLOBAL: SHANDALAR 0x008c84f0
BITMAPINFO g_duel_backbuffer_bmi;

// GLOBAL: MAGIC 0x00925ad0
// GLOBAL: SHANDALAR 0x00939c00
char g_duel_action_request_copy[0xe8];

// GLOBAL: MAGIC 0x008961b0
// GLOBAL: SHANDALAR 0x008aa3b0
int g_duel_current_selection_forced;

// GLOBAL: MAGIC 0x0074a230
// GLOBAL: SHANDALAR 0x00746b08
int g_duel_main_window_closing;

// GLOBAL: MAGIC 0x00579f20
// GLOBAL: SHANDALAR 0x0058f650
int g_duel_main_window_deferred_resize;

typedef struct
{
  unsigned char pad_00[0x1f]; // +0x00
  char life;                  // +0x1f
  unsigned char pad_20;       // +0x20
  char kind;                  // +0x21
  unsigned char pad_22[0x0e]; // +0x22
  int flags;                  // +0x30
  unsigned char pad_31[0x58]; // +0x34
} ShandalarMonsterDefinition;
STATIC_ASSERT(sizeof(ShandalarMonsterDefinition) == 0x8c, shandalar_monster_definition_wrong_size);

// GLOBAL: MAGIC 0x00746880
ShandalarMonsterDefinition g_shandalar_monster_definitions[56];

// FUNCTION: MAGIC 0x00447627
// FUNCTION: SHANDALAR 0x004511a5
void notify_duel_action(int player, unsigned int value)
{
}

// FUNCTION: MAGIC 0x004dbea9
// FUNCTION: SHANDALAR 0x00542b64
int create_duel_child_windows(HWND parent_window)
{
  return 1;
}

// FUNCTION: MAGIC 0x004dc512
// FUNCTION: SHANDALAR 0x005431cd
void layout_duel_child_windows(HWND hwnd, int layout)
{
}

// FUNCTION: MAGIC 0x004e8f49
// FUNCTION: SHANDALAR 0x00506d61
void refresh_duel_window(HWND hwnd)
{
}

// FUNCTION: MAGIC 0x004bd131
void resize_duel_hand_window(HWND hwnd)
{
  (void)hwnd;
}

// FUNCTION: MAGIC 0x004ddbf4
// FUNCTION: SHANDALAR 0x005448a5
void run_duel_timer_tick(void)
{
}

// FUNCTION: MAGIC 0x0049d77b
// FUNCTION: SHANDALAR 0x00539128
void show_post_duel_draws(int duel_result)
{
}

// FUNCTION: MAGIC 0x0046452a
// FUNCTION: SHANDALAR 0x00426c59
void set_player_directive_value(int player, int value)
{
}

// FUNCTION: MAGIC 0x0044a1ca
// FUNCTION: SHANDALAR 0x00453d4d
void get_current_duel_selection(int *selected_player, int *selected_card)
{
}

// FUNCTION: MAGIC 0x0044a2bd
// FUNCTION: SHANDALAR 0x00453e40
int can_use_current_duel_selection(void)
{
  return 0;
}

// FUNCTION: MAGIC 0x00506240
int pick_internal_card_from_list_dialog(char *prompt, int initial_card_id, int filter)
{
  (void)prompt;
  (void)initial_card_id;
  (void)filter;
  return -1;
}

// FUNCTION: MAGIC 0x004649ef
void show_opponent_library_window(int unused_color)
{
  (void)unused_color;
}

// FUNCTION: MAGIC 0x00464a28
void show_player_library_window(int unused_color)
{
  (void)unused_color;
}

// FUNCTION: MAGIC 0x0048a8ee
void save_duel_interface_options_to_registry(void)
{
}

// FUNCTION: MAGIC 0x004d2a09
void copy_autosave_to_save_file(LPCSTR save_path)
{
  (void)save_path;
}

// FUNCTION: MAGIC 0x00464b6f
// FUNCTION: SHANDALAR 0x00562893
void FUN_00464b6f(void)
{
}

// FUNCTION: MAGIC 0x0043de00
void FUN_0043de00(void)
{
}

// FUNCTION: MAGIC 0x004458d4
void FUN_004458d4(void)
{
}

// FUNCTION: MAGIC 0x004ecdc0
void FUN_004ecdc0(int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x004ef17b
void FUN_004ef17b(char *path)
{
  (void)path;
}

// FUNCTION: MAGIC 0x004ef35a
void FUN_004ef35a(char *path)
{
  (void)path;
}

// FUNCTION: MAGIC 0x004ef54b
void FUN_004ef54b(char *path)
{
  (void)path;
}

// FUNCTION: MAGIC 0x0044b56a
int FUN_0044b56a(void)
{
  return 0;
}

// FUNCTION: MAGIC 0x0044a7e0
// FUNCTION: SHANDALAR 0x00403160
void FUN_0044a7e0(unsigned int player)
{
  (void)player;
}

// FUNCTION: MAGIC 0x004b6091
// FUNCTION: SHANDALAR 0x0056c892
int CountDuelPoolEligibleTowns(void)
{
  int duel_pool_count;
  int town_index;

  duel_pool_count = 0;
  for (town_index = 0; town_index < 0x80; town_index = town_index + 1)
  {
    if ((((g_town_slots[town_index].status_and_ruling_wizard & 0xff01U) == 1) &&
         (1 < g_town_slots[town_index].location_type)) &&
        (g_town_slots[town_index].location_type < 4))
    {
      duel_pool_count = duel_pool_count + 1;
    }
  }
  return duel_pool_count;
}

// FUNCTION: MAGIC 0x004e1c4f
// FUNCTION: SHANDALAR 0x00561441
char *FUN_004e1c4f(int creature_type)
{
  (void)creature_type;
  return DAT_007a7c60;
}

// FUNCTION: MAGIC 0x004c0a0a
// FUNCTION: SHANDALAR 0x004ece9a
void FUN_004c0a0a(void)
{
}

// FUNCTION: MAGIC 0x004b559a
// FUNCTION: SHANDALAR 0x0056bd9d
void FUN_004b559a(int color)
{
  (void)color;
}

// FUNCTION: MAGIC 0x004b5416
int FUN_004b5416(void)
{
  return 0;
}

// FUNCTION: MAGIC 0x00464700
void FUN_00464700(int wizard_color)
{
  (void)wizard_color;
}

// FUNCTION: MAGIC 0x0046476f
void FUN_0046476f(int player, int internal_card_id)
{
  (void)player;
  (void)internal_card_id;
}

// FUNCTION: MAGIC 0x004b3dee
void FUN_004b3dee(int player, int wizard_color)
{
  (void)player;
  (void)wizard_color;
}

// FUNCTION: MAGIC 0x004b3cc2
void FUN_004b3cc2(int *redraw_result, int *ante_result, int *ante_info)
{
  (void)redraw_result;
  (void)ante_result;
  (void)ante_info;
}

// FUNCTION: MAGIC 0x004996d0
void FUN_004996d0(unsigned int *starting_player,
                  unsigned int *mulligan_accepted,
                  unsigned int starting_player_value,
                  int random_starting_player,
                  int opponent_ante_card,
                  int player_ante_card,
                  int redraw_result,
                  int ante_result,
                  int ante_info,
                  int *out_flag)
{
  (void)starting_player;
  (void)mulligan_accepted;
  (void)starting_player_value;
  (void)random_starting_player;
  (void)opponent_ante_card;
  (void)player_ante_card;
  (void)redraw_result;
  (void)ante_result;
  (void)ante_info;
  (void)out_flag;
}

// FUNCTION: MAGIC 0x004b23a9
// FUNCTION: SHANDALAR 0x0056862e
int play_duel(int player, int creature_type)
{
  struct
  {
    int unused_68;                  // ebp - 0x68
    int wizard_color;               // ebp - 0x64
    int library_count;              // ebp - 0x60
    int quantity;                   // ebp - 0x5c
    int loop_5c;                    // ebp - 0x58
    int packet_card;                // ebp - 0x54
    int loop_player;                // ebp - 0x50
    int shandalar_deck_minimums[6]; // ebp - 0x4c
    unsigned int mulligan_accepted; // ebp - 0x34
    int ante_result;                // ebp - 0x30
    int victory_count;              // ebp - 0x2c
    int opening_hand_count;         // ebp - 0x28
    int random_starting_player;     // ebp - 0x24
    int town_index;                 // ebp - 0x20
    int redraw_result;              // ebp - 0x1c
    int card_index;                 // ebp - 0x18
    int ante_info;                  // ebp - 0x14
    unsigned int next_player;       // ebp - 0x10
    unsigned int starting_player;   // ebp - 0xc
    int town_count;                 // ebp - 0x8
    int player_index;               // ebp - 0x4
  } s;

  (void)player;
  if ((g_duel_mode_flags & 1) == 0)
  {
    creature_type = -1;
  }

  FUN_00464b6f();
  unk_00742fc4 = 1;
  for (s.player_index = 0; s.player_index < 2; s.player_index = s.player_index + 1)
  {
    for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
    {
      global_card_instances[s.player_index][s.card_index].original_internal_card_id = -1;
      global_card_instances[s.player_index][s.card_index].internal_card_id =
          global_card_instances[s.player_index][s.card_index].original_internal_card_id;
    }
    for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      global_exile[s.player_index][s.card_index] = -1;
      global_graveyard_slots[s.player_index][s.card_index] = global_exile[s.player_index][s.card_index];
    }
    DAT_00938e38[s.player_index] = 0;
    active_cards_count[s.player_index] = 0;
  }

  g_duel_special_land_card_ids[1] = 0xef;
  g_duel_special_land_card_ids[2] = 0x7e;
  g_duel_special_land_card_ids[3] = 0x5b;
  g_duel_special_land_card_ids[4] = 0xa4;
  g_duel_special_land_card_ids[5] = 0xbc;
  unk_007abc78 = 0;
  unk_008b44d0[0] = 0;
  unk_008b44d0[1] = 0;
  FUN_0043de00();
  FUN_004458d4();

  if (creature_type != -1)
  {
    s.shandalar_deck_minimums[4] = 0;
    s.shandalar_deck_minimums[0] = 0x1e;
    s.shandalar_deck_minimums[1] = 0x23;
    s.shandalar_deck_minimums[2] = 0x28;
    s.shandalar_deck_minimums[3] = 0x28;
    life[0] = 10;
    if ((g_world_magic_bitmap & 2) != 0)
    {
      life[0] += 2;
    }
    if ((g_world_magic_bitmap & 0x800) != 0)
    {
      life[0] += 3;
    }
    if ((g_world_magic_bitmap & 0x80) != 0)
    {
      life[0] += 5;
    }
    life[0] = CountDuelPoolEligibleTowns();
    life[0] += g_next_duel_life_delta;
    life[0] += DAT_0074636c;
    if (g_next_duel_card_id > 0 && g_next_duel_card_id <= 5)
    {
      life[0] += g_next_duel_card_id;
    }
    DAT_00716024 = life[0];
    g_next_duel_life_delta = 0;
    life[1] = g_shandalar_monster_definitions[creature_type].life;
    if (creature_type <= 0x24 && creature_type % 7 != 0)
    {
      life[1] += g_shandalar_difficulty * 2;
    }
    else if (creature_type <= 0x24 && creature_type % 7 == 0)
    {
      life[1] += g_shandalar_difficulty * 5;
    }
    else if (creature_type < 0x37)
    {
      life[1] += g_shandalar_difficulty * 2;
    }
    else if (creature_type >= 0x37)
    {
      life[1] += g_shandalar_difficulty * 50;
    }
    if (g_shandalar_monster_definitions[creature_type].kind == 0xb)
    {
      for (s.card_index = 0; s.card_index < 10; s.card_index = s.card_index + 1)
      {
        if ((g_world_magic_bitmap & (1 << (byte)s.card_index)) != 0)
        {
          life[1]++;
        }
      }
    }
    if (g_shandalar_monster_definitions[creature_type].kind == 0xc)
    {
      life[1] += 10;
      s.victory_count = 0;
      s.town_count = 0;
      for (s.card_index = 0; s.card_index < 1000; s.card_index = s.card_index + 1)
      {
        if (g_duel_victory_log[s.card_index] == 0)
        {
          break;
        }
        if ((g_duel_victory_log[s.card_index] >> 4) == DAT_008ce538)
        {
          s.victory_count++;
        }
      }
      life[1] -= s.victory_count;
      for (s.town_index = 0; s.town_index < 128; s.town_index = s.town_index + 1)
      {
        if ((g_town_slots[s.town_index].status_and_ruling_wizard & 0xff00) != 0)
        {
          if (((g_town_slots[s.town_index].status_and_ruling_wizard >> 8) - 1) == s.card_index)
          {
            s.town_count++;
          }
        }
      }
      life[1] += s.town_count * g_shandalar_difficulty;
      life[1] = MAX(life[1], g_shandalar_difficulty * 5 + 20);
    }
    if (g_shandalar_monster_definitions[creature_type].kind == 0xd)
    {
      life[1] = g_shandalar_difficulty * 100 + 100;
    }
    strcpy(DAT_007a7c60, FUN_004e1c4f(creature_type));
    s.opening_hand_count = ClampIntToRange(DAT_007a7874 + g_shandalar_difficulty + 4, 0, 99);
    if (g_shandalar_difficulty == 3)
    {
      s.opening_hand_count = 7;
    }
    s.opening_hand_count += DAT_00742fd0;
    s.opening_hand_count = 7;
    if (s.opening_hand_count < 3)
    {
      s.opening_hand_count = 3;
    }
    unk_007161d8 = 7 - s.opening_hand_count;
    FUN_004c0a0a();
    if (g_shandalar_difficulty != 0 && internal_rand(2) != 0 && g_duel_ai_mode_state == 0)
    {
      s.starting_player = 1;
    }
    else
    {
      s.starting_player = 0;
    }
    s.random_starting_player = 1;
    DAT_008a8ffc = 1;
    if (DAT_00776514 != 0 || g_next_duel_card_id == 0)
    {
      if (DAT_00776514 != 0)
      {
        s.starting_player = 1;
      }
      else
      {
        s.starting_player = 0;
      }
      s.random_starting_player = 0;
      DAT_008a8ffc = 0;
      DAT_00776514 = 0;
    }

    if (g_duel_ai_mode_state != 0)
    {
    }
    else
    {
      DAT_00776510 = 0;
      for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
      {
        if (deck[s.card_index] != -1 && (deck[s.card_index] & 0x4000) == 0)
        {
          s.shandalar_deck_minimums[4]++;
        }
      }
      if (s.shandalar_deck_minimums[4] < s.shandalar_deck_minimums[g_shandalar_difficulty])
      {
        DAT_00776510 = 1;
        memcpy(DAT_00775d40, deck, 2000);
        for (s.card_index = 0;
             s.card_index < s.shandalar_deck_minimums[g_shandalar_difficulty] - s.shandalar_deck_minimums[4];
             s.card_index = s.card_index + 1)
        {
          FUN_004b559a(internal_rand(5));
        }
      }
      for (s.card_index = 0; s.card_index < 0x3c; s.card_index = s.card_index + 1)
      {
        global_library[1][s.card_index] = 0;
        global_library[0][s.card_index] = global_library[1][s.card_index];
      }
      for (s.card_index = 0x3c; s.card_index < 500; s.card_index = s.card_index + 1)
      {
        global_library[1][s.card_index] = -1;
        global_library[0][s.card_index] = global_library[1][s.card_index];
      }
      TENTATIVE_reassess_all_cards();
      for (s.town_index = 0; s.town_index < 4; s.town_index = s.town_index + 1)
      {
        for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
        {
          if ((deck[s.card_index] & 0x4000) == 0 &&
              (deck[s.card_index] & 0xfff) == global_ante_cards[0][s.town_index])
          {
            deck[s.card_index] |= 0x8000;
            break;
          }
        }
      }
      for (s.card_index = 0; s.card_index < 7; s.card_index = s.card_index + 1)
      {
        if (g_selected_wizard_color == -1)
        {
          add_card_to_hand(0, FUN_004b5416());
        }
        else
        {
          add_card_to_hand(0, DrawRandomCardFromInitialLibrary(g_selected_wizard_color));
        }
      }
      if (DAT_0057a750 != -1)
      {
        if (g_selected_wizard_color == -1)
        {
          FUN_00464700(DAT_0057a750);
          if (DAT_008ced00[0] != -1)
          {
            FUN_0046476f(0, DAT_008ced00[0]);
          }
        }
        for (s.card_index = 0; s.opening_hand_count > s.card_index; s.card_index = s.card_index + 1)
        {
          add_card_to_hand(1, DrawRandomCardFromInitialLibrary(g_selected_wizard_color != -1));
        }
      }
      FUN_004b3cc2(&s.ante_info, &s.ante_result, &s.redraw_result);
      FUN_004996d0(&s.starting_player,
                   &s.mulligan_accepted,
                   s.starting_player,
                   s.random_starting_player,
                   DAT_008ced00[0],
                   global_ante_cards[0][0],
                   s.ante_info,
                   s.ante_result,
                   s.redraw_result,
                   &s.shandalar_deck_minimums[5]);
      if (s.mulligan_accepted != 0)
      {
        for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
        {
          global_card_instances[0][s.card_index].internal_card_id = -1;
          global_card_instances[0][s.card_index].original_internal_card_id = -1;
        }
        for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
        {
          if (deck[s.card_index] != -1)
          {
            deck[s.card_index] &= -32769;
          }
        }
        for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
        {
          if ((deck[s.card_index] & 0xfff) == global_ante_cards[0][0])
          {
            deck[s.card_index] |= 0x8000;
            break;
          }
        }
        for (s.card_index = 0; s.card_index < 7; s.card_index = s.card_index + 1)
        {
          if (g_selected_wizard_color == -1)
          {
            add_card_to_hand(0, FUN_004b5416());
          }
          else
          {
            add_card_to_hand(0, DrawRandomCardFromInitialLibrary(g_selected_wizard_color));
          }
        }
        TENTATIVE_reassess_all_cards(0, 0x30);
      }
      if (s.ante_result != 0 || (s.mulligan_accepted != 0 && s.redraw_result != 0))
      {
        for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
        {
          global_card_instances[1][s.card_index].internal_card_id = -1;
          global_card_instances[1][s.card_index].original_internal_card_id = -1;
        }
        if (g_selected_wizard_color == -1)
        {
          FUN_00464700(DAT_0057a750);
          if (DAT_008ced00[0] != -1)
          {
            FUN_0046476f(0, DAT_008ced00[0]);
          }
        }
        for (s.card_index = 0; s.opening_hand_count > s.card_index; s.card_index = s.card_index + 1)
        {
          add_card_to_hand(1, DrawRandomCardFromInitialLibrary(g_selected_wizard_color != -1));
        }
        TENTATIVE_reassess_all_cards(0, 0x30);
      }
      if (g_selected_wizard_color == -1)
      {
        DAT_0057a750 = 0;
      }
      for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
      {
        if (g_selected_wizard_color == -1)
        {
          global_library[0][s.card_index] = FUN_004b5416();
        }
        else
        {
          global_library[0][s.card_index] = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
        }
        global_library[1][s.card_index] = DrawRandomCardFromInitialLibrary(DAT_0057a750);
      }
      g_selected_wizard_color = -1;
      if ((g_shandalar_monster_definitions[creature_type].flags & 2) != 0 && g_monster_timer % 3 == 0)
      {
        memcpy(global_library[1], global_library[0], 1000);
        s.card_index = g_duel_ai_mode_state;
        g_duel_ai_mode_state = 1;
        FUN_004b59b2(0, 1);
        g_duel_ai_mode_state = s.card_index;
        memcpy(global_card_instances[1], global_card_instances[0], 0xb0f4);
        for (s.card_index = 0; s.card_index < 0x96; s.card_index = s.card_index + 1)
        {
          if (global_card_instances[1][s.card_index].internal_card_id != -1)
          {
            global_card_instances[1][s.card_index].state |= 0x1000;
          }
        }
        unk_007161d8 = 0;
      }
      g_duel_ai_mode_state = 0;
      FUN_0043de00();
      if (DAT_008cf6d0 != -1)
      {
        s.card_index = add_card_to_hand(1, DAT_008cf6d0);
        FUN_0051a41c(1, s.card_index);
        DAT_008cf6d0 = -1;
        if (g_next_duel_card_id == -1)
        {
          g_next_duel_card_id = 0;
        }
      }
      if (unk_00789308 != -1)
      {
        s.card_index = add_card_to_hand(1, unk_00789308);
        FUN_0051a41c(1, s.card_index);
        unk_00789308 = -1;
        if (g_next_duel_card_id == -1)
        {
          g_next_duel_card_id = 0;
        }
      }
      if (g_next_duel_card_id > 5)
      {
        s.card_index = add_card_to_hand(0, g_next_duel_card_id);
        FUN_0051a41c(0, s.card_index);
      }
    }
  }
  else
  {
    if ((g_duel_network_flags & 2) != 0)
    {
      append_to_trace_txt(s_Starting_the_duel_005732b4);
    }
    life[1] = 0x14;
    life[0] = life[1];
    FUN_00449bef(DAT_007a7c60);
    s.opening_hand_count = 7;
    unk_007161d8 = 0;
    DAT_008951c8 = 1;
    DAT_008ce538 = -1;
    FUN_004c0a0a();
    if ((g_duel_network_flags & 2) != 0)
    {
      FUN_00501d78(0);
      FUN_00501deb(1);
    }
    if (DAT_00896718 == -1)
    {
      if ((g_duel_network_flags & 2) != 0)
      {
        s.starting_player = internal_rand(2);
      }
      else
      {
        if (g_shandalar_difficulty != 0 && internal_rand(2) != 0 && g_duel_ai_mode_state == 0)
        {
          s.starting_player = 1;
        }
        else
        {
          s.starting_player = 0;
        }
      }
      s.random_starting_player = 1;
    }
    else
    {
      s.starting_player = DAT_00896718;
      s.random_starting_player = 0;
    }
    if ((g_duel_network_flags & 2) != 0)
    {
      if (DAT_007a79b8 != 0)
      {
        g_network_result_packet_type = 1;
        g_network_result_value = 1 - s.starting_player;
        TENTATIVE_send_network_result(0, 1);
      }
      else
      {
        TENTATIVE_wait_for_network_result(1, 1);
        s.starting_player = g_network_result_value;
      }
    }
    DAT_008a8ffc = 1;
    if (g_duel_use_previous_backdrop_colors == 0)
    {
      for (s.loop_5c = 0; s.loop_5c < 0x3c; s.loop_5c = s.loop_5c + 1)
      {
        global_library[1][s.loop_5c] = 0;
        global_library[0][s.loop_5c] = global_library[1][s.loop_5c];
      }
      for (s.loop_5c = 0x3c; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
      {
        global_library[1][s.loop_5c] = -1;
        global_library[0][s.loop_5c] = global_library[1][s.loop_5c];
      }
      TENTATIVE_reassess_all_cards(0, 0x30);
      for (s.loop_5c = 0; s.loop_5c < 0x10; s.loop_5c = s.loop_5c + 1)
      {
        DAT_008ced00[s.loop_5c] = -1;
        global_ante_cards[0][s.loop_5c] = DAT_008ced00[s.loop_5c];
      }
      if (DAT_00789104 != 0)
      {
        if ((g_duel_network_flags & 2) != 0)
        {
          global_ante_cards[0][0] = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
          g_network_result_packet_type = 2;
          g_network_result_value = global_ante_cards[0][0];
          TENTATIVE_send_network_result(0, 2);
          TENTATIVE_wait_for_network_result(1, 2);
          DAT_008ced00[0] = g_network_result_value;
        }
        else
        {
          global_ante_cards[0][0] = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
          DAT_008ced00[0] = DrawRandomCardFromInitialLibrary(DAT_0057a750);
        }
      }
      if ((g_duel_network_flags & 2) != 0)
      {
        for (s.loop_5c = 0; s.loop_5c < 7; s.loop_5c = s.loop_5c + 1)
        {
          s.packet_card = DrawRandomCardFromInitialLibrary(g_selected_wizard_color);
          add_card_to_hand(0, s.packet_card);
          AddCardToCLPacket(s.packet_card);
        }
        AddCardToCLPacket(-1);
        TENTATIVE_send_network_result(s.player_index, 3);
        TENTATIVE_wait_for_network_result(1, 3);
        s.packet_card = 0;
        s.loop_5c = s.packet_card;
        while (s.packet_card != -1)
        {
          s.unused_68 = s.loop_5c;
          s.loop_5c++;
          s.packet_card = GetCardFromCLPacket(s.unused_68);
          if (s.packet_card != -1)
          {
            add_card_to_hand(1, s.packet_card);
          }
        }
      }
      else
      {
        for (s.loop_5c = 0; s.loop_5c < 7; s.loop_5c = s.loop_5c + 1)
        {
          add_card_to_hand(0, DrawRandomCardFromInitialLibrary(g_selected_wizard_color));
          add_card_to_hand(1, DrawRandomCardFromInitialLibrary(DAT_0057a750));
        }
      }
      FUN_004b3cc2(&s.ante_info, &s.ante_result, &s.redraw_result);
      for (s.loop_player = 0; s.loop_player <= 1; s.loop_player++)
      {
        if (s.loop_player == 0)
        {
          s.wizard_color = g_selected_wizard_color;
        }
        else
        {
          s.wizard_color = DAT_0057a750;
        }
        s.library_count = 0;
        for (s.loop_5c = 0; s.loop_5c < 200; s.loop_5c = s.loop_5c + 1)
        {
          if (initial_library[s.wizard_color][s.loop_5c].csvid != -1)
          {
            for (s.quantity = 0; s.quantity < initial_library[s.wizard_color][s.loop_5c].numcards;
                 s.quantity = s.quantity + 1)
            {
              global_library[s.loop_player][s.library_count] = 0;
              s.library_count++;
            }
          }
        }
        for (s.loop_5c = s.library_count; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
        {
          global_library[s.loop_player][s.loop_5c] = -1;
        }
      }
      FUN_004996d0(&s.starting_player,
                   &s.mulligan_accepted,
                   s.starting_player,
                   s.random_starting_player,
                   DAT_008ced00[0],
                   global_ante_cards[0][0],
                   s.ante_info,
                   s.ante_result,
                   s.redraw_result,
                   &s.shandalar_deck_minimums[5]);
      if (s.mulligan_accepted != 0)
      {
        FUN_004b3dee(0, g_selected_wizard_color);
        TENTATIVE_reassess_all_cards(0, 0x30);
      }
      if (((g_duel_network_flags & 2) == 0 && (s.ante_result != 0 || (s.mulligan_accepted != 0 && s.redraw_result != 0))) ||
          ((g_duel_network_flags & 2) != 0 && s.shandalar_deck_minimums[5] != 0))
      {
        FUN_004b3dee(1, DAT_0057a750);
        TENTATIVE_reassess_all_cards(0, 0x30);
      }
      for (s.loop_player = 0; s.loop_player <= 1; s.loop_player++)
      {
        if ((g_duel_network_flags & 2) && s.loop_player == 1)
        {
          for (s.loop_5c = 0; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
          {
            global_library[s.loop_player][s.loop_5c] = -1;
          }

          continue;
        }

        if (s.loop_player == 0)
        {
          s.wizard_color = g_selected_wizard_color;
        }
        else
        {
          s.wizard_color = DAT_0057a750;
        }
        s.library_count = 0;
        for (s.loop_5c = 0; s.loop_5c < 200; s.loop_5c = s.loop_5c + 1)
        {
          if (initial_library[s.wizard_color][s.loop_5c].csvid != -1)
          {
            for (s.quantity = 0; s.quantity < initial_library[s.wizard_color][s.loop_5c].numcards;
                 s.quantity = s.quantity + 1)
            {
              global_library[s.loop_player][s.library_count] =
                  CardTypeFromID(initial_library[s.wizard_color][s.loop_5c].csvid);
              s.library_count++;
            }
            initial_library[s.wizard_color][s.loop_5c].numcards = 0;
          }
        }
        for (s.loop_5c = s.library_count; s.loop_5c < 500; s.loop_5c = s.loop_5c + 1)
        {
          global_library[s.loop_player][s.loop_5c] = -1;
        }
      }
      g_selected_wizard_color = -1;
      s.loop_5c = g_duel_ai_mode_state;
      g_duel_ai_mode_state = 1;
      FUN_004b59b2(0, 0);
      FUN_004b59b2(1, 1);
      g_duel_ai_mode_state = s.loop_5c;
    }
  }

  FUN_0043de00();

  if (g_duel_ai_mode_state == -1)
  {
    FUN_004ecdc0(0);
  }
  if (g_duel_ai_mode_state == -2)
  {
    FUN_004ecdc0(1);
  }
  if (g_duel_ai_mode_state == -10 && 0 < g_duel_run_mode)
  {
    if (g_duel_run_mode == 1)
    {
      FUN_004ef17b(DAT_008a915c);
    }
    else if (g_duel_run_mode == 2)
    {
      FUN_004ef35a(DAT_008a915c);
    }
    else if (g_duel_run_mode == 3)
    {
      FUN_004ef54b(DAT_008a915c);
    }
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = human_player;
  }
  else if (g_duel_ai_mode_state == -10)
  {
    load_gametype0(DAT_008a915c);
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = human_player;
  }
  else if (g_duel_ai_mode_state == -1)
  {
    DAT_008b484c = 0;
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = 1;
  }
  else if (g_duel_ai_mode_state == -2)
  {
    DAT_008b484c = 0;
    TENTATIVE_reassess_all_cards(0, 0xff);
    s.next_player = 0;
  }
  else
  {
    DAT_008b484c = 1;
    s.next_player = s.starting_player;
  }

#ifndef SHANDALAR
  do
  {
  } while (WaitForSingleObject(global_mutex_GameInit, 0xffffffff) != 0);
#endif
  while (DAT_0091bfb0 == 0 && FUN_0044b56a() == 0)
  {
    if (DAT_0093a848 != 0)
    {
      if ((DAT_0093a848 & 1U) != 0)
      {
        DAT_0093a848 = 0;
        if (s.next_player == 1)
        {
          g_duel_extra_turn_player = -1;
        }
        FUN_0044a7e0(0);
        g_duel_extra_turn_player = -1;
      }
      else
      {
        DAT_0093a848 = 0;
        if (s.next_player == 0)
        {
          g_duel_extra_turn_player = -1;
        }
        FUN_0044a7e0(1);
      }
      g_duel_extra_turn_player = -1;
    }
    else
    {
      FUN_0044a7e0(s.next_player);
      s.next_player = 1 - s.next_player;
    }
  }
#ifndef SHANDALAR
  ReleaseMutex(global_mutex_GameInit);
#endif

  if (DAT_0074302c == 0)
  {
    for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
    {
      if (deck[s.card_index] != -1)
      {
        deck[s.card_index] &= -32769;
      }
    }
  }
  else
  {
    OutputDebugStringA("OneDeck ONEDECK ONE DECK\n");
  }

  g_next_duel_card_id = -1;
  DAT_008cf6d0 = g_next_duel_card_id;
  unk_00789308 = DAT_008cf6d0;
  g_duel_ai_mode_state = 0;
  for (s.card_index = 0; s.card_index < 4; s.card_index = s.card_index + 1)
  {
    DAT_007a7d10[s.card_index] = 8;
  }
  unk_00742fc4 = 0;
  DAT_00742fc0 = 1;
  if (life[0] > 0 && DAT_007abce0 < 10 && (life[1] <= 0 || DAT_007abce4 >= 10))
  {
    return 1;
  }
  if (life[1] > 0 && DAT_007abce4 < 10 && (life[0] <= 0 || DAT_007abce0 >= 10))
  {
    return 0;
  }
  return -1;
}

// FUNCTION: MAGIC 0x004dd413
// FUNCTION: SHANDALAR 0x005440c4
int get_primary_color_from_duel_deck(int player)
{
  (void)player;
  return 0;
}

// FUNCTION: MAGIC 0x004dd719
void apply_duel_backdrop_art(int player, int color, int variant)
{
  (void)player;
  (void)color;
  (void)variant;
}

// FUNCTION: MAGIC 0x004ee26e
unsigned int load_gametype0(char *path)
{
  (void)path;
  return 0;
}

// FUNCTION: MAGIC 0x004dbe21
// FUNCTION: SHANDALAR 0x00542adc
DWORD WINAPI start_duel_thread(LPVOID unused)
{
  DWORD result;
  int counter;

  (void)unused;
  srand(GetTickCount());
  for (counter = 0; counter < rand() % 50000; counter = counter + 1)
  {
    rand();
  }

  InitializeDuelUiGlobalIds();
  result = play_duel(0, g_duel_creature_type);
  PostMessageA(g_duel_window_hwnd, 0x401, result, 0);
  return result;
}

void dispatch_duel_engine_message(MSG *message);

// FUNCTION: MAGIC 0x004da411
// FUNCTION: SHANDALAR 0x005410d1
LRESULT CALLBACK wndproc_MAGICGAME_MainClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int minmaxinfo;
    char pad_0004[0x108];
    int command_player;
    int command_selected_card;
    int command_saved_phase_flags;
    int enemy_backdrop_result;
    int player_backdrop_color;
    int enemy_backdrop_color;
    int player_backdrop_result;

    union
    {
      char help_path[264];
      struct
      {
        int player_color;
        int enemy_color;
        int player_variant;
        char path[252];
      } backdrop;
    } help_area;

    int slot_index;
    BOOL visible;
    int screen_height;
    int screen_width;
    int has_replay_prompt;
    char replay_prompt[100];
    int should_quit;
    HWND duel_result;
    int *posted_action_result;
    int selected_card;
    int selected_player;
    POINT cursor_pos;
    unsigned int hit_test;
    HWND cursor_window;
    int *action_request;
    MSG modal_msg;
    DWORD thread_exit_code;
    int *action_result;
    BOOL have_message;
    unsigned int action_result_ok;
    int done;
  } s;

  if (msg == 0x464)
  {
    notify_duel_action(0, (unsigned int)wparam);
    return 0;
  }

  switch (msg)
  {
  case 0x433:
  case 0x434:
    return 0;

  case 0x403:
    if (g_your_attack_window_hwnd != (HWND)0)
    {
      SendMessageA(g_your_attack_window_hwnd, WM_CLOSE, 0, 0);
    }
    KillTimer(hwnd, g_duel_timer_id);
    s.action_request = (int *)wparam;
    s.action_result = (int *)lparam;
    g_duel_modal_action_active = 1;
    memcpy(g_duel_action_request_copy, s.action_request, 0xe8);
    GetCursorPos(&s.cursor_pos);
    s.cursor_window = WindowFromPoint(s.cursor_pos);
    s.hit_test = SendMessageA(s.cursor_window, WM_NCHITTEST, 0,
                              ((s.cursor_pos.y << 0x10) & 0xffffU) | (s.cursor_pos.x & 0xffffU));
    SendMessageA(s.cursor_window, WM_SETCURSOR, (WPARAM)s.cursor_window, (s.hit_test & 0xffff) | 0x2000000);
    set_player_directive_value(1, s.action_request[0x38]);
    set_player_directive_value(0, s.action_request[0x39]);
    set_duel_prompt_context(g_duel_prompt_context_hwnd, (char *)(s.action_request + 6), s.action_request[5]);
    get_current_duel_selection(&s.selected_player, &s.selected_card);
    if (((s.selected_card == 0x15) && (s.selected_player == 1)) && (can_use_current_duel_selection() != 0))
    {
      g_duel_current_selection_forced = 1;
    }
    s.done = 0;
    while (s.done == 0)
    {
      GetExitCodeThread(g_duel_thread_handle, &s.thread_exit_code);
      if (s.thread_exit_code != STILL_ACTIVE)
      {
        s.done = 1;
      }
      s.have_message = PeekMessageA(&s.modal_msg, (HWND)0, 0, 0, PM_REMOVE);
      if (g_duel_main_window_closing != 0)
      {
        if ((s.have_message != 0) && (s.modal_msg.message == 0x464))
        {
          s.have_message = 0;
        }
        s.done = 1;
        s.action_result[0] = -5;
        s.action_result[1] = -1;
        s.action_result[2] = -1;
        if (s.action_result[0] == 0)
        {
          s.action_result_ok = 1;
        }
        else
        {
          s.action_result_ok = 0;
        }
      }
      if (s.have_message != 0)
      {
        if (s.modal_msg.message == 0x464)
        {
          s.posted_action_result = (int *)s.modal_msg.lParam;
          s.done = 1;
          s.action_result_ok = (unsigned int)(s.posted_action_result[0] == 0);
          memcpy(s.action_result, s.posted_action_result, 0x10);
        }
        else if (s.modal_msg.message == WM_QUIT)
        {
          PostQuitMessage(s.modal_msg.wParam);
          s.done = 1;
          s.action_result_ok = 0;
          s.action_result[0] = -2;
        }
        else
        {
          dispatch_duel_engine_message(&s.modal_msg);
        }
      }
    }
    set_duel_prompt_context(g_duel_prompt_context_hwnd, (char *)0, 0);
    set_player_directive_value(1, 0);
    set_player_directive_value(0, 0);
    g_duel_modal_action_active = 0;
    SetTimer(hwnd, g_duel_timer_id, 45000, (TIMERPROC)0);
    return s.action_result_ok;

  case 0x401:
    s.duel_result = (HWND)wparam;
    s.should_quit = 1;
    if (g_duel_thread_handle != (HANDLE)0)
    {
      WaitForSingleObject(g_duel_thread_handle, INFINITE);
      CloseHandle(g_duel_thread_handle);
      g_duel_thread_handle = (HANDLE)0;
    }
    if ((g_duel_mode_flags & 1) != 0)
    {
      if (g_duel_interface_options.see_next_draws_at_end_of_duel != 0)
      {
        show_post_duel_draws((int)s.duel_result);
      }
    }
    else if ((g_duel_mode_flags & 2) != 0)
    {
      s.has_replay_prompt = 1;
      if (s.duel_result == (HWND)1)
      {
        strcpy(s.replay_prompt, s_Congratulations_00579f28);
      }
      else if (s.duel_result == (HWND)0)
      {
        strcpy(s.replay_prompt, s_Too_bad_00579f3c);
      }
      else if (s.duel_result == (HWND)-1)
      {
        strcpy(s.replay_prompt, s_Oh_well_00579f44);
      }
      else
      {
        s.has_replay_prompt = 0;
      }
      if (s.has_replay_prompt != 0)
      {
        strcat(s.replay_prompt, s_Want_to_play_again_00579f50);
        if (MessageBoxA(hwnd, s.replay_prompt, s_End_of_duel_00579f68, MB_YESNO) == IDYES)
        {
          s.should_quit = 0;
          SendMessageA(hwnd, 0x400, 0, 0);
        }
      }
    }
    if (s.should_quit != 0)
    {
      g_duel_exit_code = (int)s.duel_result;
      PostQuitMessage((int)s.duel_result);
    }
    return 0;

  case 0x400:
    g_duel_main_window_closing = 0;
    if (DAT_0091c998 == 2)
    {
      ShowWindow(g_duel_card_preview_window_hwnd, SW_HIDE);
    }
    else
    {
      SendMessageA(g_duel_card_preview_window_hwnd, 0x401, (WPARAM)-1, 0);
    }
    SendMessageA(g_duel_full_card_window_hwnd, 0x40c, 0, 0);
    SendMessageA(g_duel_life_window_hwnd, 0x40c, 0, 0);
    SendMessageA(g_duel_player_battlefield_window_hwnd, 0x40c, 0, 0);
    SendMessageA(g_duel_help_owner_hwnd, 0x40c, 0, 0);
    g_duel_selected_player_card = -1;
    g_duel_selected_opponent_card = -1;
    g_duel_selection_pending = 0;
    g_duel_current_selection_forced = -1;
    SendMessageA(DAT_008a8dec, 0x432, 0, 0);
    ShowWindow(DAT_008a8dec, SW_SHOW);
    ShowWindow(DAT_008a8d78, SW_HIDE);
    g_duel_cached_life_player_1 = 0;
    g_duel_cached_life_player_0 = g_duel_cached_life_player_1;
    g_duel_cached_state_007abce4 = 0;
    g_duel_cached_state_007abce0 = g_duel_cached_state_007abce4;
    g_duel_cached_unk_008b44d0_player_1 = 0;
    g_duel_cached_unk_008b44d0_player_0 = g_duel_cached_unk_008b44d0_player_1;
    SendMessageA(g_duel_life_status_window_1_hwnd, 0x432, 0, 0);
    SendMessageA(g_duel_life_status_window_2_hwnd, 0x432, 0, 0);
    for (s.screen_width = 0; s.screen_width < 7; s.screen_width = s.screen_width + 1)
    {
      g_duel_cached_raw_mana_player_1[s.screen_width] = 0;
      g_duel_cached_raw_mana_player_0[s.screen_width] = g_duel_cached_raw_mana_player_1[s.screen_width];
    }
    SendMessageA(unk_008ce534, 0x432, 0, 0);
    SendMessageA(unk_00939344, 0x432, 0, 0);
    g_duel_cached_library_count_player_1 = 1;
    g_duel_cached_library_count_player_0 = g_duel_cached_library_count_player_1;
    SendMessageA(DAT_0091ce30, 0x432, 0, 0);
    SendMessageA(DAT_0092680c, 0x432, 0, 0);
    g_duel_cached_graveyard_count_player_1 = 0;
    g_duel_cached_graveyard_count_player_0 = g_duel_cached_graveyard_count_player_1;
    g_duel_cached_exile_count_player_1 = 0;
    g_duel_cached_exile_count_player_0 = g_duel_cached_exile_count_player_1;
    SendMessageA(g_duel_player_graveyard_window_hwnd, 0x432, 0, 0);
    SendMessageA(g_phasebar_your_untap_window_hwnd, 0x432, 0, 0);
    SendMessageA(g_duel_attack_phase_window_hwnd, 0x40c, 0, 0);
    g_multiblock_creature_count = 0;
    global_stack_cards[0].player = -1;
    SendMessageA(g_duel_phase_display_window_hwnd, 0x40c, 0, 0);
    g_duel_modal_action_active = 0;
    ShowWindow(g_duel_opponent_face_window_hwnd, SW_HIDE);
    ShowWindow(g_duel_player_face_window_hwnd, SW_HIDE);
    UpdateWindow(hwnd);
    SetFocus(hwnd);
    if (g_duel_ai_mode_state == -10 && g_duel_run_mode > 0)
    {
    }
    else
    {
      if (g_duel_ai_mode_state == -10)
      {
        load_gametype0(g_duel_save_game_openfilename.lpstrFile);
      }
      else if ((g_duel_mode_flags & 4) != 0)
      {
        g_last_duel_enemy_primary_color = get_primary_color_from_duel_deck(DAT_0057a750);
        g_last_duel_player_primary_color = get_primary_color_from_duel_deck(g_selected_wizard_color);
      }
      else if ((g_duel_mode_flags & 1) != 0)
      {
        g_last_duel_enemy_primary_color = get_primary_color_from_duel_deck(DAT_0057a750);
        g_last_duel_player_primary_color = get_primary_color_from_duel_deck(g_selected_wizard_color);
      }
    }
    if ((g_duel_mode_flags & 0x10) != 0)
    {
      SendMessageA(g_duel_opponent_face_window_hwnd, 0x439, 0, 0);
      SendMessageA(g_duel_player_face_window_hwnd, 0x439, 0, 0);
    }
    else if ((g_duel_mode_flags & 1) != 0)
    {
      if (g_duel_creature_type != -1)
      {
        sprintf(s.help_area.backdrop.path, "%s\\%03d.pic", global_faces_path, g_duel_creature_type);
        s.screen_height = (int)load_pic(s.help_area.backdrop.path);
      }
      else
      {
        s.screen_height = 0;
      }
      SendMessageA(g_duel_opponent_face_window_hwnd, 0x439, (WPARAM)s.screen_height, 0);
      s.screen_height = (int)g_duel_player_face_pic;
      SendMessageA(g_duel_player_face_window_hwnd, 0x439, (WPARAM)s.screen_height, 1);
    }
    else
    {
      s.screen_height = (int)load_pic((char *)DAT_008ce570);
      SendMessageA(g_duel_opponent_face_window_hwnd, 0x439, (WPARAM)s.screen_height, 0);
      s.screen_height = (int)load_pic((char *)DAT_008b3fc0);
      SendMessageA(g_duel_player_face_window_hwnd, 0x439, (WPARAM)s.screen_height, 0);
    }
    if ((g_duel_mode_flags & 0x10) != 0)
    {
      s.help_area.backdrop.player_color = 1;
      s.help_area.backdrop.enemy_color = s.help_area.backdrop.player_color;
      s.help_area.backdrop.player_variant = 2;
      s.player_backdrop_result = s.help_area.backdrop.player_variant;
    }
    else if (g_duel_use_previous_backdrop_colors != 0)
    {
      s.help_area.backdrop.enemy_color = g_last_duel_enemy_primary_color;
      s.player_backdrop_result = g_last_duel_result_state;
      if (DAT_0091c99c == -1)
      {
        s.help_area.backdrop.player_color = g_last_duel_player_primary_color;
      }
      else
      {
        s.help_area.backdrop.player_color = DAT_0091c99c;
      }
      s.help_area.backdrop.player_variant = DAT_0091c9a0;
    }
    else
    {
      s.help_area.backdrop.enemy_color = g_last_duel_enemy_primary_color;
      s.player_backdrop_result = rand() % 3;
      if (DAT_0091c99c == -1)
      {
        s.help_area.backdrop.player_color = g_last_duel_player_primary_color;
      }
      else
      {
        s.help_area.backdrop.player_color = DAT_0091c99c;
      }
      s.help_area.backdrop.player_variant = DAT_0091c9a0;
      if ((s.help_area.backdrop.player_color == s.help_area.backdrop.enemy_color) &&
          (s.help_area.backdrop.player_variant == s.player_backdrop_result))
      {
        s.player_backdrop_result = (s.player_backdrop_result + 1) % 3;
      }
      g_last_duel_result_state = s.player_backdrop_result;
    }
    apply_duel_backdrop_art(1, s.help_area.backdrop.enemy_color, s.player_backdrop_result);
    apply_duel_backdrop_art(0, s.help_area.backdrop.player_color, s.help_area.backdrop.player_variant);
    if (IsWindowVisible(hwnd) == 0)
    {
      ShowWindow(hwnd, SW_SHOW);
      SetForegroundWindow(hwnd);
      UpdateWindow(hwnd);
      ShowWindow(g_duel_full_card_window_hwnd, SW_SHOW);
      ShowWindow(g_duel_life_window_hwnd, SW_SHOW);
    }
    if ((g_duel_mode_flags & 0x10) != 0)
    {
      g_duel_thread_handle = (HANDLE)0;
      PostMessageA(hwnd, 0x401, 0, 0);
    }
    else
    {
      g_duel_thread_handle = CreateThread((LPSECURITY_ATTRIBUTES)0,
                                          0,
                                          start_duel_thread,
                                          (LPVOID)0,
                                          0,
                                          (LPDWORD)&s.enemy_backdrop_color);
      SetThreadPriority(g_duel_thread_handle, THREAD_PRIORITY_NORMAL);
    }
    return 0;

  case WM_CLOSE:
    life[0] = 0;
    g_duel_main_window_closing = 1;
    return 0;

  case WM_COMMAND:
    switch ((unsigned int)wparam & 0xffff)
    {
    case 0x279:
      g_duel_interface_options.show_id_tags_on_cards =
          (g_duel_interface_options.show_id_tags_on_cards == 0);
      save_duel_interface_options_to_registry();
      SendMessageA(g_duel_player_battlefield_window_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_help_owner_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_attack_phase_window_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_phase_display_window_hwnd, 0x435, 0, 0);
      break;

    case 0x27a:
      g_duel_interface_options.show_invisible_effect_cards =
          (g_duel_interface_options.show_invisible_effect_cards == 0);
      save_duel_interface_options_to_registry();
      notify_duel_action(0, 0xff);
      break;

    case 0x27c:
      g_duel_interface_options.show_all_cards_summon_sickness =
          (g_duel_interface_options.show_all_cards_summon_sickness == 0);
      save_duel_interface_options_to_registry();
      SendMessageA(g_duel_player_battlefield_window_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_help_owner_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_attack_phase_window_hwnd, 0x435, 0, 0);
      SendMessageA(g_duel_phase_display_window_hwnd, 0x435, 0, 0);
      break;

    case 0x27b:
      if ((g_duel_network_flags & 2) == 0)
      {
        g_duel_save_game_openfilename.hwndOwner = g_duel_window_hwnd;
        g_duel_save_game_openfilename.lpstrTitle = gs_window_title_save_game_008951d0;
        g_duel_save_game_openfilename.Flags = 0x2a000c;
        if (GetSaveFileNameA(&g_duel_save_game_openfilename) != 0)
        {
          copy_autosave_to_save_file(g_duel_save_game_openfilename.lpstrFile);
        }
      }
      break;

    case 599:
      if (g_duel_startup_state != 0)
      {
        g_duel_startup_state = 0;
      }
      else
      {
        g_duel_startup_state = 1;
      }
      if (DAT_008ce504 == 0)
      {
        g_duel_startup_state = 0;
      }
      break;

    case 0x25c:
      if (g_duel_startup_state != 0)
      {
        g_duel_surface_reset_state = (g_duel_surface_reset_state == 0);
        resize_duel_hand_window(g_duel_life_window_hwnd);
      }
      break;

    case 0x25d:
      if (g_duel_startup_state != 0)
      {
        show_opponent_library_window(DAT_0057a750);
      }
      break;

    case 0x25e:
      if (g_duel_startup_state != 0)
      {
        show_player_library_window(DAT_0057a750);
      }
      break;

    case 0x26b:
    case 0x26c:
      if (g_duel_startup_state != 0)
      {
        if (((unsigned int)wparam & 0xffff) == 0x26b)
        {
          s.player_backdrop_color = 0;
        }
        else
        {
          s.player_backdrop_color = 1;
        }
        s.enemy_backdrop_result = pick_internal_card_from_list_dialog("Pick a card to put into play", -1, -1);
        s.command_selected_card = (int)(char)DAT_007abc90[human_player * 0x26 + current_phase];
        DAT_007abc90[human_player * 0x26 + current_phase] =
            (unsigned char)((int)(char)DAT_007abc90[human_player * 0x26 + current_phase] & 0xfe);
        s.command_saved_phase_flags = add_card_to_hand(s.player_backdrop_color, s.enemy_backdrop_result);
        if (s.command_saved_phase_flags != -1)
        {
          FUN_0051a41c(s.player_backdrop_color, s.command_saved_phase_flags);
        }
        DAT_007abc90[human_player * 0x26 + current_phase] = (unsigned char)s.command_selected_card;
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x26d:
    case 0x26e:
      if (g_duel_startup_state != 0)
      {
        if (((unsigned int)wparam & 0xffff) == 0x26d)
        {
          s.player_backdrop_color = 0;
        }
        else
        {
          s.player_backdrop_color = 1;
        }
        s.enemy_backdrop_result = pick_internal_card_from_list_dialog("Pick a card to put into hand", -1, -1);
        s.command_saved_phase_flags = add_card_to_hand(s.player_backdrop_color, s.enemy_backdrop_result);
        hand_count[s.player_backdrop_color] += 1;
        if (((g_duel_network_flags & 2) != 0) && (unk_008b35ec == s.player_backdrop_color))
        {
          g_network_result_packet_type = 0x12;
          g_network_result_value = s.enemy_backdrop_result;
          TENTATIVE_send_network_result(unk_008b35ec, 0x12);
        }
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x269:
    case 0x26a:
      if (g_duel_startup_state != 0)
      {
        if (((unsigned int)wparam & 0xffff) == 0x269)
        {
          s.player_backdrop_color = 0;
        }
        else
        {
          s.player_backdrop_color = 1;
        }
        FUN_0043e18b(s.player_backdrop_color);
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x26f:
    case 0x270:
      if (g_duel_startup_state != 0)
      {
        if (((unsigned int)wparam & 0xffff) == 0x26f)
        {
          s.player_backdrop_color = 0;
        }
        else
        {
          s.player_backdrop_color = 1;
        }
        life[s.player_backdrop_color] = FUN_004a09c6(0,
                                                     s.player_backdrop_color == 0 ? "Set player lives to:" : "Set opponent lives tp:",
                                                     life[s.player_backdrop_color]);
        notify_duel_action(0, 0xff);
      }
      break;

    case 0x267:
    case 0x268:
      if (g_duel_startup_state != 0)
      {
        if (((unsigned int)wparam & 0xffff) == 0x267)
        {
          s.player_backdrop_color = 1;
        }
        else
        {
          s.player_backdrop_color = 0;
        }
        life[s.player_backdrop_color] = 0;
        SendMessageA(g_duel_life_status_window_1_hwnd, 0x432, 0, 0);
        SendMessageA(g_duel_life_status_window_2_hwnd, 0x432, 0, 0);
      }
      break;

    case 0x263:
      if (g_duel_startup_state != 0)
      {
        life[0] = 0;
        life[1] = 0;
        SendMessageA(g_duel_life_status_window_1_hwnd, 0x432, 0, 0);
        SendMessageA(g_duel_life_status_window_2_hwnd, 0x432, 0, 0);
      }
      break;

    case 0x271:
      if (g_duel_startup_state != 0)
      {
        ShowWindow(g_duel_palette_window_hwnd, SW_SHOW);
      }
      break;

    case 0x275:
      break;

    case 0x276:
      if (g_duel_startup_state != 0)
      {
        if (g_duel_palette_refresh_pending == 0)
        {
          g_duel_palette_refresh_pending = 1;
        }
        else
        {
          g_duel_palette_refresh_pending = 0;
        }
      }
      break;

    case 0x277:
      if (g_duel_startup_state != 0)
      {
        ShowWindow(g_duel_toggleable_status_window_hwnd,
                   -((IsWindowVisible(g_duel_toggleable_status_window_hwnd) == 0)) & SW_SHOW);
      }
      break;

    case 0x273:
      if (g_duel_startup_state != 0)
      {
        g_shared_startup_completed = (g_shared_startup_completed == 0);
        SendMessageA(g_duel_player_battlefield_window_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_help_owner_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_full_card_window_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_life_window_hwnd, 0x435, 0, 0);
        InvalidateRect(g_duel_card_preview_window_hwnd, (RECT *)0, 0);
      }
      break;

    case 0x274:
      if (g_duel_startup_state != 0)
      {
        g_duel_network_state = 0;
      }
      break;

    case 0x272:
      if (g_duel_startup_state != 0)
      {
        g_duel_dialog_refresh_state = (g_duel_dialog_refresh_state == 0);
        SendMessageA(g_duel_player_battlefield_window_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_help_owner_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_full_card_window_hwnd, 0x435, 0, 0);
        SendMessageA(g_duel_life_window_hwnd, 0x435, 0, 0);
        InvalidateRect(DAT_0091ce30, (RECT *)0, 1);
        InvalidateRect(DAT_0092680c, (RECT *)0, 1);
      }
      break;
    }
    return 0;

  case WM_CREATE:
    g_duel_thread_handle = (HANDLE)0;
    g_duel_selected_player_card = -1;
    g_duel_selected_opponent_card = -1;
    g_duel_selection_pending = 0;
    g_duel_cached_life_player_1 = 0;
    g_duel_cached_life_player_0 = g_duel_cached_life_player_1;
    g_duel_cached_state_007abce4 = 0;
    g_duel_cached_state_007abce0 = g_duel_cached_state_007abce4;
    g_duel_cached_unk_008b44d0_player_1 = 0;
    g_duel_cached_unk_008b44d0_player_0 = g_duel_cached_unk_008b44d0_player_1;
    for (s.command_player = 0; s.command_player < 7; s.command_player = s.command_player + 1)
    {
      g_duel_cached_raw_mana_player_1[s.command_player] = 0;
      g_duel_cached_raw_mana_player_0[s.command_player] = g_duel_cached_raw_mana_player_1[s.command_player];
    }
    g_duel_cached_library_count_player_1 = 1;
    g_duel_cached_library_count_player_0 = g_duel_cached_library_count_player_1;
    g_duel_cached_graveyard_count_player_1 = 0;
    g_duel_cached_graveyard_count_player_0 = g_duel_cached_graveyard_count_player_1;
    g_duel_cached_exile_count_player_1 = 0;
    g_duel_cached_exile_count_player_0 = g_duel_cached_exile_count_player_1;
    g_multiblock_creature_count = 0;
    g_duel_modal_action_active = 0;
    if (create_duel_child_windows(hwnd) == 0)
    {
      return -1;
    }
    g_duel_palette_window_hwnd = CreateWindowExA(0, "MAGIC_PaletteClass", "Palette", 0x80cc0000,
                                                 0x14, 0x14, 300, 0x15e, hwnd, (HMENU)0,
                                                 g_app_instance, (LPVOID)0);
    g_your_attack_window_hwnd = (HWND)0;
    g_duel_timer_id = 0x14;
    return 0;

  case WM_DESTROY:
    strcpy(s.pad_0004, global_base_directory);
    strcat(s.pad_0004, "\\duel.hlp");
    WinHelpA(g_duel_window_hwnd, s.pad_0004, HELP_QUIT, 0);
    KillTimer(hwnd, g_duel_timer_id);
    return 0;

  case WM_DISPLAYCHANGE:
    g_display_color_depth = (int)wparam;
    EnterCriticalSection(&g_card_render_lock);
    checked_DeleteDC_DeleteObject(g_shared_offscreen_dc, g_shared_offscreen_bitmap);
    if (CreateOffscreen32bppDibSection(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                                       &g_shared_offscreen_dc, &g_duel_backbuffer_bmi,
                                       &g_shared_offscreen_bitmap, &g_shared_offscreen_prev_object,
                                       &g_shared_offscreen_bits) == 0)
    {
      MessageBoxA(hwnd, DAT_007898b0, gs_magic_the_gathering_title_00789460, MB_ICONEXCLAMATION);
      ShowWindow(hwnd, SW_HIDE);
    }
    else
    {
      ShowWindow(hwnd, SW_SHOW);
    }
    LeaveCriticalSection(&g_card_render_lock);
    if (IsIconic(hwnd) == 0)
    {
      MoveWindow(hwnd, 1, 0, ((unsigned int)lparam & 0xffff) - 1, HIWORD(lparam), 1);
    }
    else
    {
      g_duel_main_window_deferred_resize = 1;
    }
    return 0;

  case WM_ERASEBKGND:
    return 1;

  case 0x501:
    g_duel_main_window_deferred_resize = 0;
    MoveWindow(hwnd, 1, 0, GetSystemMetrics(SM_CXSCREEN) - 1, GetSystemMetrics(SM_CYSCREEN), 1);
    return 0;

  case WM_QUERYOPEN:
    if (g_duel_main_window_deferred_resize != 0)
    {
      PostMessageA(hwnd, 0x501, 0, 0);
    }

    if (DUEL_SHELL_WINDOW_HWND != (HWND)0)
    {
      ShowWindow(DUEL_SHELL_WINDOW_HWND, SW_SHOW);
    }

    return 1;

  case WM_GETMINMAXINFO:
    s.minmaxinfo = (int)lparam;
    ((int *)s.minmaxinfo)[6] = 0x32;
    ((int *)s.minmaxinfo)[7] = 0x32;
    return 0;

  case WM_SIZE:
    if ((wparam == SIZE_RESTORED) && (g_duel_main_window_minimized == 0))
    {
      LockWindowUpdate(hwnd);
      layout_duel_child_windows(hwnd, DAT_0091c998);
      refresh_duel_window(g_duel_player_battlefield_window_hwnd);
      refresh_duel_window(g_duel_help_owner_hwnd);
      LockWindowUpdate((HWND)0);
    }

    if (wparam == SIZE_MINIMIZED)
    {
      g_duel_main_window_minimized = 1;
    }
    else if (wparam == SIZE_RESTORED)
    {
      g_duel_main_window_minimized = 0;
    }

    if ((wparam == SIZE_MINIMIZED) && (DUEL_SHELL_WINDOW_HWND != (HWND)0))
    {
      ShowWindow(DUEL_SHELL_WINDOW_HWND, SW_SHOW);
    }
    return 0;

  case 0x30f:
  case 0x310:
  case 0x311:
    return FUN_10025b5e(hwnd, msg, wparam, lparam);

  case WM_TIMER:
    if ((int)wparam == g_duel_timer_id)
    {
      if (g_duel_modal_action_active == 0)
      {
        if (g_your_attack_window_hwnd != (HWND)0)
        {
        }
        else
        {
          run_duel_timer_tick();
        }
      }
    }
    return 0;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return 0;
}
