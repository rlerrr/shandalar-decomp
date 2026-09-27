#include "magic_shell_screen_name.h"
#include "manalinkinterface/manalinkinterface.h"
#include <process.h>

// GLOBAL: MAGIC 0x007775b8
int g_screen_name_profile_changed;

// GLOBAL: MAGIC 0x007775bc
int g_screen_name_worker_running;

// FUNCTION: MAGIC 0x0048eb09
void __cdecl shell_screen_name_network_worker(void *context)
{
  HWND hwnd;

  hwnd = (HWND)context;
  while (g_screen_name_worker_running != 0)
  {
    Sleep(500);
    if (FamInterface_HasOpponent() != 0 || FamInterface_IsNetworkTEN() != 0)
    {
      EnableWindow(GetDlgItem(hwnd, 0x742), FALSE);
      EnableWindow(GetDlgItem(hwnd, 0x717), FALSE);
      EnableWindow(GetDlgItem(hwnd, 0x741), FALSE);
      ShowWindow(GetDlgItem(hwnd, 0x742), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, 0x717), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, 0x741), SW_HIDE);
    }
    else
    {
      EnableWindow(GetDlgItem(hwnd, 0x742), TRUE);
      EnableWindow(GetDlgItem(hwnd, 0x717), TRUE);
      EnableWindow(GetDlgItem(hwnd, 0x741), TRUE);
      ShowWindow(GetDlgItem(hwnd, 0x742), SW_SHOW);
      ShowWindow(GetDlgItem(hwnd, 0x717), SW_SHOW);
      ShowWindow(GetDlgItem(hwnd, 0x741), SW_SHOW);
    }
  }
  _endthread();
}
