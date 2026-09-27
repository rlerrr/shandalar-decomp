#include "magic_shell_network_match.h"

// GLOBAL: MAGIC 0x0093f4a8
int g_multiplayer_no_valid_decks;

// FUNCTION: MAGIC 0x004560d4
void shell_apply_multiplayer_options(HWND hwnd)
{
  LRESULT deck_count;

  ShowWindow(GetDlgItem(hwnd, 0x728), SW_HIDE);
  SendDlgItemMessageA(hwnd, 0x72f, 0x465, 0, 0x10005);
  if (g_shell_multiplayer_options.best_of == 0)
  {
    g_shell_multiplayer_options.minimum_deck_size = 40;
    g_shell_multiplayer_options.free_play = 0;
    g_shell_multiplayer_options.best_of = 1;
    g_shell_multiplayer_options.deck_type = 0;
    g_shell_multiplayer_options.player_random = 1;
    g_shell_multiplayer_options.ante = 1;
    g_shell_multiplayer_options.allow_sideboarding = 0;
    shell_save_multiplayer_options();
  }
  if (g_shell_multiplayer_options.best_of > 5)
  {
    g_shell_multiplayer_options.best_of = 5;
    shell_save_multiplayer_options();
  }
  else
  {
    if (g_shell_multiplayer_options.best_of % 2 == 0)
    {
      g_shell_multiplayer_options.best_of |= 1;
      shell_save_multiplayer_options();
    }
  }
  SendDlgItemMessageA(hwnd, 0x72f, 0x467, 0, (WORD)g_shell_multiplayer_options.best_of);
  if (g_shell_multiplayer_options.ante != 0)
  {
    CheckDlgButton(hwnd, 0x725, 1);
  }
  else
  {
    CheckDlgButton(hwnd, 0x725, 0);
  }
  if (g_shell_multiplayer_options.free_play != 0)
  {
    CheckDlgButton(hwnd, 0x732, 1);
    CheckDlgButton(hwnd, 0x731, 0);
  }
  else
  {
    CheckDlgButton(hwnd, 0x731, 1);
    CheckDlgButton(hwnd, 0x732, 0);
  }
  CheckDlgButton(hwnd, 0x729, 0);
  CheckDlgButton(hwnd, 0x72b, 0);
  CheckDlgButton(hwnd, 0x72c, 0);
  CheckDlgButton(hwnd, 0x730, 0);
  CheckDlgButton(hwnd, 0x72a, 0);
  if (g_shell_multiplayer_options.deck_type == 0)
  {
    CheckDlgButton(hwnd, 0x729, 1);
  }
  else if (g_shell_multiplayer_options.deck_type == 1)
  {
    CheckDlgButton(hwnd, 0x72b, 1);
  }
  else if (g_shell_multiplayer_options.deck_type == 2)
  {
    CheckDlgButton(hwnd, 0x72c, 1);
  }
  else if (g_shell_multiplayer_options.deck_type == 3)
  {
    CheckDlgButton(hwnd, 0x730, 1);
  }
  else if (g_shell_multiplayer_options.deck_type == 4)
  {
    CheckDlgButton(hwnd, 0x72a, 1);
  }
  else
  {
    g_shell_multiplayer_options.deck_type = 4;
    CheckDlgButton(hwnd, 0x72a, 1);
    shell_save_multiplayer_options();
  }
  if (g_shell_multiplayer_options.minimum_deck_size == 0x28)
  {
    CheckDlgButton(hwnd, 0x734, 1);
    CheckDlgButton(hwnd, 0x735, 0);
  }
  else if (g_shell_multiplayer_options.minimum_deck_size == 0x3c)
  {
    CheckDlgButton(hwnd, 0x735, 1);
    CheckDlgButton(hwnd, 0x734, 0);
  }
  else
  {
    CheckDlgButton(hwnd, 0x735, 1);
    CheckDlgButton(hwnd, 0x734, 0);
    g_shell_multiplayer_options.minimum_deck_size = 60;
    shell_save_multiplayer_options();
  }
  SendDlgItemMessageA(hwnd, 0x723, 0x14b, 0, 0);
  deck_count = shell_populate_multiplayer_decks(hwnd, 0x723,
      g_shell_multiplayer_options.deck_type, g_shell_multiplayer_options.ante,
      g_shell_multiplayer_options.minimum_deck_size);
  if (deck_count < 1)
    g_multiplayer_no_valid_decks = 1;
  else
    g_multiplayer_no_valid_decks = 0;
  SendDlgItemMessageA(hwnd, 0x723, 0x14a, 0, (LPARAM)g_multiplayer_button_text[0]);
  SendDlgItemMessageA(hwnd, 0x723, 0x14e, 0, 0);
  if (g_shell_multiplayer_options.player_random == 0)
  {
    SendDlgItemMessageA(hwnd, 0x723, 0x14d, 0, (LPARAM)g_shell_multiplayer_options.player_deck);
  }
}
