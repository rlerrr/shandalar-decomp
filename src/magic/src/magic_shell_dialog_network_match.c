#include "magic_shell_network_match.h"
#include <string.h>
#include <stdio.h>

// GLOBAL: MAGIC 0x008a8e80
shell_multiplayer_options_t g_shell_multiplayer_options;

// GLOBAL: MAGIC 0x005710f0
static char *s_multiplayer_registry_path =
    "Software\\MicroProse\\Magic: The Gathering\\MultiOptions";

// FUNCTION: MAGIC 0x0048c25d
void shell_load_multiplayer_options(void)
{
  struct
  {
    HKEY key;
    char value[52];
    DWORD value_size;
  } s;

  if (RegOpenKeyExA(HKEY_CURRENT_USER, s_multiplayer_registry_path, 0,
                    KEY_QUERY_VALUE, &s.key) == ERROR_SUCCESS)
  {
    s.value_size = 10;
    s.value[0] = '\0';
    if (RegQueryValueExA(s.key, "DeckMinimum", NULL, NULL, (BYTE *)s.value,
                         &s.value_size) == ERROR_SUCCESS)
    {
      sscanf(s.value, "%d", &g_shell_multiplayer_options.minimum_deck_size);
    }
    else
    {
      g_shell_multiplayer_options.minimum_deck_size = 40;
    }
    s.value_size = 10;
    s.value[0] = '\0';
    if (RegQueryValueExA(s.key, "FreePlay", NULL, NULL, (BYTE *)s.value,
                         &s.value_size) == ERROR_SUCCESS)
    {
      sscanf(s.value, "%d", &g_shell_multiplayer_options.free_play);
    }
    else
    {
      g_shell_multiplayer_options.free_play = 1;
    }
    s.value_size = 10;
    s.value[0] = '\0';
    if (RegQueryValueExA(s.key, "DeckType", NULL, NULL, (BYTE *)s.value,
                         &s.value_size) == ERROR_SUCCESS)
    {
      sscanf(s.value, "%d", &g_shell_multiplayer_options.deck_type);
    }
    else
    {
      g_shell_multiplayer_options.deck_type = 0;
    }
    s.value_size = 50;
    s.value[0] = '\0';
    if (RegQueryValueExA(s.key, "PlayerDeck", NULL, NULL, (BYTE *)s.value,
                         &s.value_size) == ERROR_SUCCESS)
    {
      sprintf(g_shell_multiplayer_options.player_deck, "%s", s.value);
    }
    else
    {
      g_shell_multiplayer_options.player_deck[0] = '\0';
    }
    s.value_size = 10;
    s.value[0] = '\0';
    if (RegQueryValueExA(s.key, "PlayerRandom", NULL, NULL, (BYTE *)s.value,
                         &s.value_size) == ERROR_SUCCESS)
    {
      sscanf(s.value, "%d", &g_shell_multiplayer_options.player_random);
    }
    else
    {
      g_shell_multiplayer_options.player_random = 1;
    }
    s.value_size = 10;
    s.value[0] = '\0';
    if (RegQueryValueExA(s.key, "BestOf", NULL, NULL, (BYTE *)s.value,
                         &s.value_size) == ERROR_SUCCESS)
    {
      sscanf(s.value, "%d", &g_shell_multiplayer_options.best_of);
    }
    else
    {
      g_shell_multiplayer_options.best_of = 99;
    }
    s.value_size = 10;
    s.value[0] = '\0';
    if (RegQueryValueExA(s.key, "Ante", NULL, NULL, (BYTE *)s.value,
                         &s.value_size) == ERROR_SUCCESS)
    {
      sscanf(s.value, "%d", &g_shell_multiplayer_options.ante);
    }
    else
    {
      g_shell_multiplayer_options.ante = 1;
    }
    s.value_size = 10;
    s.value[0] = '\0';
    if (RegQueryValueExA(s.key, "AllowSideboarding", NULL, NULL, (BYTE *)s.value,
                         &s.value_size) == ERROR_SUCCESS)
    {
      sscanf(s.value, "%d", &g_shell_multiplayer_options.allow_sideboarding);
    }
    else
    {
      g_shell_multiplayer_options.allow_sideboarding = 0;
    }
    RegCloseKey(s.key);
  }
  else
  {
    g_shell_multiplayer_options.minimum_deck_size = 40;
    g_shell_multiplayer_options.deck_type = 1;
    g_shell_multiplayer_options.player_deck[0] = '\0';
    g_shell_multiplayer_options.player_random = 1;
    g_shell_multiplayer_options.free_play = 1;
    g_shell_multiplayer_options.best_of = 99;
    g_shell_multiplayer_options.ante = 1;
    g_shell_multiplayer_options.allow_sideboarding = 0;
  }
}

// FUNCTION: MAGIC 0x0048c5a1
void shell_save_multiplayer_options(void)
{
  struct
  {
    HKEY key;
    char value[12];
    DWORD disposition;
  } s;

  if (RegCreateKeyExA(HKEY_CURRENT_USER, s_multiplayer_registry_path, 0,
                      NULL, 0, KEY_ALL_ACCESS, NULL, &s.key,
                      &s.disposition) == ERROR_SUCCESS)
  {
    wsprintfA(s.value, "%d", g_shell_multiplayer_options.minimum_deck_size);
    RegSetValueExA(s.key, "DeckMinimum", 0, REG_SZ,
                   (BYTE *)s.value, strlen(s.value) + 1);
    wsprintfA(s.value, "%d", g_shell_multiplayer_options.deck_type);
    RegSetValueExA(s.key, "DeckType", 0, REG_SZ,
                   (BYTE *)s.value, strlen(s.value) + 1);
    RegSetValueExA(s.key, "PlayerDeck", 0, REG_SZ,
                   (BYTE *)g_shell_multiplayer_options.player_deck,
                   strlen(g_shell_multiplayer_options.player_deck) + 1);
    wsprintfA(s.value, "%d", g_shell_multiplayer_options.player_random);
    RegSetValueExA(s.key, "PlayerRandom", 0, REG_SZ,
                   (BYTE *)s.value, strlen(s.value) + 1);
    wsprintfA(s.value, "%d", g_shell_multiplayer_options.best_of);
    RegSetValueExA(s.key, "BestOf", 0, REG_SZ,
                   (BYTE *)s.value, strlen(s.value) + 1);
    if (g_shell_multiplayer_options.free_play == 0)
      wsprintfA(s.value, "%d", g_shell_multiplayer_options.free_play);
    RegSetValueExA(s.key, "FreePlay", 0, REG_SZ,
                   (BYTE *)s.value, strlen(s.value) + 1);
    wsprintfA(s.value, "%d", g_shell_multiplayer_options.ante);
    RegSetValueExA(s.key, "Ante", 0, REG_SZ,
                   (BYTE *)s.value, strlen(s.value) + 1);
    wsprintfA(s.value, "%d", g_shell_multiplayer_options.allow_sideboarding);
    RegSetValueExA(s.key, "AllowSideboarding", 0, REG_SZ,
                   (BYTE *)s.value, strlen(s.value) + 1);
    RegFlushKey(s.key);
    RegCloseKey(s.key);
  }
}

// FUNCTION: MAGIC 0x00497b9a
int shell_update_dci_rank(int rank, int opponent_rank, int duel_result)
{
  struct
  {
    /* Preserve the original store/reload before conversion to int. */
    volatile double updated_rank;
    double adjustment;
    double result;
    double expected;
  } s;

  if (rank >= 2400)
    s.adjustment = 16.0;
  else if (rank >= 2100)
    s.adjustment = 24.0;
  else
    s.adjustment = 32.0;
  if (duel_result == 1)
    s.result = 1.0;
  else if (duel_result == 0)
    s.result = 0.0;
  else
    s.result = 0.5;
  s.expected = 1.0 / (1.0 + (opponent_rank - (double)rank) / 400.0 * 10.0);
  s.updated_rank = rank + (s.adjustment * s.result - s.expected);
  return (int)s.updated_rank;
}
