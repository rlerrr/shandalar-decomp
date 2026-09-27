#include "magic_shell_network_match.h"
#include "global_state.h"
#include <stdio.h>

extern int g_manalink_current_opponent_present;
extern int g_manalink_is_host;
extern HANDLE global_mutex_UpdateLowerDialog;

// GLOBAL: MAGIC 0x0093dac0
char g_multiplayer_status_text[6][300];
// GLOBAL: MAGIC 0x0093ed88
int g_multiplayer_resume_requested;
// GLOBAL: MAGIC 0x0093ed8c
int g_multiplayer_dialog_state;
// GLOBAL: MAGIC 0x0093ed90
int g_multiplayer_duel_ready;

// FUNCTION: MAGIC 0x004564d3
void shell_update_multiplayer_status(HWND hwnd, int state)
{
  char status_text[100];
  if (g_manalink_current_opponent_present == 0)
  {
    ShowWindow(GetDlgItem(hwnd, 1), 0);
    ShowWindow(GetDlgItem(hwnd, 0x72e), 0);
  }
  else if (g_manalink_is_host != 0)
  {
    if (state == 0)
    {
      ShowWindow(GetDlgItem(hwnd, 0x72e), 0);
      EnableWindow(GetDlgItem(hwnd, 1), 1);
      SetDlgItemTextA(hwnd, 1, g_multiplayer_button_text[3]);
      ShowWindow(GetDlgItem(hwnd, 1), 5);
      EnableWindow(GetDlgItem(hwnd, 0x72e), 0);
      SetDlgItemTextA(hwnd, 0x72e, g_multiplayer_button_text[2]);
      sprintf(status_text, g_multiplayer_status_text[1], g_saved_player_name);
      SetDlgItemTextA(hwnd, 0x721, status_text);
    }
    else if (state == 1)
    {
      ShowWindow(GetDlgItem(hwnd, 1), 0);
      ShowWindow(GetDlgItem(hwnd, 0x72e), 0);
      sprintf(status_text, g_multiplayer_status_text[2], g_saved_player_name);
      SetDlgItemTextA(hwnd, 0x721, status_text);
    }
    else if (state == 2)
    {
      ShowWindow(GetDlgItem(hwnd, 1), 0);
      ShowWindow(GetDlgItem(hwnd, 0x72e), 0);
    }
  }
  else
  {
    if (state == 0)
    {
      ShowWindow(GetDlgItem(hwnd, 1), 0);
      ShowWindow(GetDlgItem(hwnd, 0x72e), 0);
      sprintf(status_text, g_multiplayer_status_text[3], g_saved_player_name);
      SetDlgItemTextA(hwnd, 0x721, status_text);
    }
    else if (state == 1)
    {
      EnableWindow(GetDlgItem(hwnd, 1), 1);
      SetDlgItemTextA(hwnd, 1, g_multiplayer_button_text[4]);
      ShowWindow(GetDlgItem(hwnd, 1), 5);
      EnableWindow(GetDlgItem(hwnd, 0x72e), 1);
      SetDlgItemTextA(hwnd, 0x72e, g_multiplayer_button_text[5]);
      ShowWindow(GetDlgItem(hwnd, 0x72e), 5);
      if (g_multiplayer_resume_requested != 0)
      {
        sprintf(status_text, g_multiplayer_status_text[5], g_saved_player_name);
      }
      else
      {
        sprintf(status_text, g_multiplayer_status_text[4], g_saved_player_name);
      }
      SetDlgItemTextA(hwnd, 0x721, status_text);
    }
    else if (state == 2)
    {
      ShowWindow(GetDlgItem(hwnd, 1), 0);
      ShowWindow(GetDlgItem(hwnd, 0x72e), 0);
    }
  }
}

// FUNCTION: MAGIC 0x0045683b
void shell_update_multiplayer_dialog_state(HWND hwnd, int state)
{
  DWORD style;

  while (WaitForSingleObject(global_mutex_UpdateLowerDialog, INFINITE) != WAIT_OBJECT_0)
  {
  }
  shell_apply_multiplayer_options(hwnd);
  shell_update_multiplayer_status(hwnd, state);
  if (g_manalink_current_opponent_present == 0)
  {
    SetDlgItemTextA(hwnd, 0x721, g_multiplayer_status_text[0]);
    EnableWindow(GetDlgItem(hwnd, 0x722), 0);
    EnableWindow(GetDlgItem(hwnd, 0x723), 0);
    EnableWindow(GetDlgItem(hwnd, 0x725), 0);
    EnableWindow(GetDlgItem(hwnd, 0x727), 0);
    EnableWindow(GetDlgItem(hwnd, 0x72f), 0);
    EnableWindow(GetDlgItem(hwnd, 0x731), 0);
    EnableWindow(GetDlgItem(hwnd, 0x732), 0);
    EnableWindow(GetDlgItem(hwnd, 0x729), 0);
    EnableWindow(GetDlgItem(hwnd, 0x72a), 0);
    EnableWindow(GetDlgItem(hwnd, 0x72b), 0);
    EnableWindow(GetDlgItem(hwnd, 0x72c), 0);
    EnableWindow(GetDlgItem(hwnd, 0x730), 0);
    EnableWindow(GetDlgItem(hwnd, 0x733), 0);
    EnableWindow(GetDlgItem(hwnd, 0x734), 0);
    EnableWindow(GetDlgItem(hwnd, 0x735), 0);
  }
  else
  {
    EnableWindow(GetDlgItem(hwnd, 0x722), 1);
    EnableWindow(GetDlgItem(hwnd, 0x723), 1);
    if (g_manalink_is_host != 0 && state == 0)
    {
      EnableWindow(GetDlgItem(hwnd, 0x725), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x725), GWL_STYLE);
      style &= 0xfffffffd;
      style |= 3;
      SetWindowLongA(GetDlgItem(hwnd, 0x725), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x727), 1);
      EnableWindow(GetDlgItem(hwnd, 0x72f), 1);
      SendDlgItemMessageA(hwnd, 0x72f, 0x465, 0, 0x10005);
      SendDlgItemMessageA(hwnd, 0x72f, 0x467, 0, (WORD)g_shell_multiplayer_options.best_of);
      SendDlgItemMessageA(hwnd, 0x727, 0xc5, 1, 0);
      EnableWindow(GetDlgItem(hwnd, 0x731), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x731), GWL_STYLE);
      style &= 0xfffffffb;
      style |= 9;
      SetWindowLongA(GetDlgItem(hwnd, 0x731), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x732), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x732), GWL_STYLE);
      style &= 0xfffffffb;
      style |= 9;
      SetWindowLongA(GetDlgItem(hwnd, 0x732), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x729), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x729), GWL_STYLE);
      style &= 0xfffffffb;
      style |= 9;
      SetWindowLongA(GetDlgItem(hwnd, 0x729), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x72a), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x72a), GWL_STYLE);
      style &= 0xfffffffb;
      style |= 9;
      SetWindowLongA(GetDlgItem(hwnd, 0x72a), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x72b), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x72b), GWL_STYLE);
      style &= 0xfffffffb;
      style |= 9;
      SetWindowLongA(GetDlgItem(hwnd, 0x72b), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x72c), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x72c), GWL_STYLE);
      style &= 0xfffffffb;
      style |= 9;
      SetWindowLongA(GetDlgItem(hwnd, 0x72c), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x730), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x730), GWL_STYLE);
      style &= 0xfffffffb;
      style |= 9;
      SetWindowLongA(GetDlgItem(hwnd, 0x730), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x72e), 1);
      EnableWindow(GetDlgItem(hwnd, 0x733), 1);
      EnableWindow(GetDlgItem(hwnd, 0x734), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x734), GWL_STYLE);
      style &= 0xfffffffb;
      style |= 9;
      SetWindowLongA(GetDlgItem(hwnd, 0x734), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x735), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x735), GWL_STYLE);
      style &= 0xfffffffb;
      style |= 9;
      SetWindowLongA(GetDlgItem(hwnd, 0x735), GWL_STYLE, style);
        }
    else
    {
      EnableWindow(GetDlgItem(hwnd, 0x725), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x725), GWL_STYLE);
      style &= 0xfffffffc;
      style |= 2;
      SetWindowLongA(GetDlgItem(hwnd, 0x725), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x727), 0);
      EnableWindow(GetDlgItem(hwnd, 0x72f), 0);
      SendDlgItemMessageA(hwnd, 0x72f, 0x465, 0, 0x10005);
      SendDlgItemMessageA(hwnd, 0x72f, 0x467, 0, (WORD)g_shell_multiplayer_options.best_of);
      SendDlgItemMessageA(hwnd, 0x727, 0xc5, 1, 0);
      EnableWindow(GetDlgItem(hwnd, 0x731), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x731), GWL_STYLE);
      style &= 0xfffffff6;
      style |= 4;
      SetWindowLongA(GetDlgItem(hwnd, 0x731), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x732), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x732), GWL_STYLE);
      style &= 0xfffffff6;
      style |= 4;
      SetWindowLongA(GetDlgItem(hwnd, 0x732), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x729), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x729), GWL_STYLE);
      style &= 0xfffffff6;
      style |= 4;
      SetWindowLongA(GetDlgItem(hwnd, 0x729), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x72a), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x72a), GWL_STYLE);
      style &= 0xfffffff6;
      style |= 4;
      SetWindowLongA(GetDlgItem(hwnd, 0x72a), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x72b), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x72b), GWL_STYLE);
      style &= 0xfffffff6;
      style |= 4;
      SetWindowLongA(GetDlgItem(hwnd, 0x72b), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x72c), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x72c), GWL_STYLE);
      style &= 0xfffffff6;
      style |= 4;
      SetWindowLongA(GetDlgItem(hwnd, 0x72c), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x730), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x730), GWL_STYLE);
      style &= 0xfffffff6;
      style |= 4;
      SetWindowLongA(GetDlgItem(hwnd, 0x730), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x72e), 1);
      EnableWindow(GetDlgItem(hwnd, 0x733), 1);
      EnableWindow(GetDlgItem(hwnd, 0x734), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x734), GWL_STYLE);
      style &= 0xfffffff6;
      style |= 4;
      SetWindowLongA(GetDlgItem(hwnd, 0x734), GWL_STYLE, style);
      EnableWindow(GetDlgItem(hwnd, 0x735), 1);
      style = GetWindowLongA(GetDlgItem(hwnd, 0x735), GWL_STYLE);
      style &= 0xfffffff6;
      style |= 4;
      SetWindowLongA(GetDlgItem(hwnd, 0x735), GWL_STYLE, style);
        }
  }
  ReleaseMutex(global_mutex_UpdateLowerDialog);
}
