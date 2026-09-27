#include "magic_shell_network_match.h"
#include "network.h"
#include <process.h>

extern HANDLE global_mutex_LowerDialog;
extern HWND g_manalink_lower_dialog_hwnd;
void shell_begin_duel_trace(char *message);

// FUNCTION: MAGIC 0x00459c79
void __cdecl shell_wait_for_multiplayer_parameters(void *context)
{
  HWND hwnd;

  hwnd = (HWND)context;
  OutputDebugStringA("Entering Get Duel Parameters.\n");
  shell_begin_duel_trace("Start of Magic: The Gathering\n");
  TENTATIVE_wait_for_network_result(1, 7);
  WaitForSingleObject(global_mutex_LowerDialog, INFINITE);
  ReleaseMutex(global_mutex_LowerDialog);
  OutputDebugStringA("Getting Duel Parameters.\n");
  if (g_manalink_lower_dialog_hwnd != NULL)
    SendMessageA(g_manalink_lower_dialog_hwnd, WM_COMMAND, 0x401, 0);
  _endthread();
}

// FUNCTION: MAGIC 0x00459d01
void __cdecl shell_wait_for_multiplayer_response(void *context)
{
  HWND hwnd;

  hwnd = (HWND)context;
  OutputDebugStringA("Entering Guest Response.\n");
  TENTATIVE_wait_for_network_result(1, 8);
  WaitForSingleObject(global_mutex_LowerDialog, INFINITE);
  ReleaseMutex(global_mutex_LowerDialog);
  OutputDebugStringA("Getting Guest Response.\n");
  if (g_manalink_lower_dialog_hwnd != NULL)
    SendMessageA(g_manalink_lower_dialog_hwnd, WM_COMMAND, 0x402,
                 g_network_result_packet.result);
  _endthread();
}

// FUNCTION: MAGIC 0x00459d80
void __cdecl shell_wait_for_multiplayer_start(void *context)
{
  HWND hwnd;

  hwnd = (HWND)context;
  OutputDebugStringA("Entering Start a Duel.\n");
  TENTATIVE_wait_for_network_result(1, 9);
  OutputDebugStringA("Getting Start a Duel.\n");
  WaitForSingleObject(global_mutex_LowerDialog, INFINITE);
  ReleaseMutex(global_mutex_LowerDialog);
  g_multiplayer_duel_ready = 1;
  if (g_manalink_lower_dialog_hwnd != NULL)
    PostMessageA(g_manalink_lower_dialog_hwnd, WM_COMMAND, IDOK, 0);
  _endthread();
}
