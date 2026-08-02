#include <stdio.h>
#include <windows.h>
#include <winnt.h>
#include <winreg.h>
#include "inttypes.h"
#include "mystdbool.h"

void DebugLog(char *str, int log_level);

// GLOBAL: MANALINKINTERFACE 0x10008000
undefined4 g_fam_interface_mapping_size = 0x00006938;

// GLOBAL: MANALINKINTERFACE 0x10009030
HANDLE g_FamInterfaceGpd_handle = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x10009034
undefined4 g_fam_InterfaceMutex = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x10009038
HANDLE g_famI_SendEvent = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x1000b31c
HINSTANCE global_hinstance;

// GLOBAL: MANALINKINTERFACE 0x1000B320
undefined4 *g_FamInterfaceGpd_addr;

enum GpdFlags
{
  GPD_FLAGS_ALIVE = 1,
  GPD_FLAGS_TEN = 2,
  GPD_FLAGS_HOST = 4,
  GPD_FLAGS_HAS_OPPONENT = 8,
};

// FUNCTION: MANALINKINTERFACE 0x10001000
int WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID reserved)
{
  // int BVar1 = nReason;

  switch (nReason)
  {
  case 1:
    DebugLog("DLL_PROCESS_ATTACH", 0);
    global_hinstance = hDllHandle;
    DisableThreadLibraryCalls(hDllHandle);
    if (g_fam_InterfaceMutex == (HANDLE)0x0)
    {
      g_fam_InterfaceMutex = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0, 0, "fam_InterfaceMutex");
    }
    if (g_fam_InterfaceMutex == (HANDLE)0x0)
    {
      MessageBoxA((HWND)0x0, "An error has occurred while creating the interface mutex.", "ManalinkInterface", 0x10);
      return 0;
    }
    if (g_famI_SendEvent == (HANDLE)0x0)
    {
      if ((g_famI_SendEvent = CreateEventA((LPSECURITY_ATTRIBUTES)0x0, 1, 0, "famI_SendEvent")) == (HANDLE)0x0)
      {
        MessageBoxA((HWND)0x0, "An error has occurred while creating the send event.",
                    "ManalinkInterface.dll", 0x10);
        return 0;
      }
    }
    return InitFamInterfaceGpd();
  case 0:
    DebugLog("DLL_PROCESS_DETACH", 0);
    return CloseFamInterfaceGpd();
  }
  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x1000111B
BOOL InitFamInterfaceGpd(void)
{
  struct
  {
    char debugMsg[1024];
    BOOL success;
  } s;

  g_FamInterfaceGpd_handle = CreateFileMappingA((HANDLE)0xffffffff, (LPSECURITY_ATTRIBUTES)0x0, 4, 0, g_fam_interface_mapping_size,
                                                "FamInterfaceGpd");
  if (g_FamInterfaceGpd_handle == (HANDLE)0x0)
  {
    return 0;
  }

  s.success = GetLastError() != ERROR_ALREADY_EXISTS;
  g_FamInterfaceGpd_addr = MapViewOfFile(g_FamInterfaceGpd_handle, 2, 0, 0, 0);
  sprintf(s.debugMsg, "gpd is located at 0x%x", g_FamInterfaceGpd_addr);
  DebugLog(s.debugMsg, 0);
  if (g_FamInterfaceGpd_addr == (LPVOID)0x0)
  {
    return 0;
  }

  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x100011E4
BOOL CloseFamInterfaceGpd(void)
{
  if (UnmapViewOfFile(g_FamInterfaceGpd_addr) == 0)
  {
    return 0;
  }

  return CloseHandle(g_FamInterfaceGpd_handle);
}

// FUNCTION: MANALINKINTERFACE 0x1000121B
void DebugLog(char *str, int logLevel)
{
  // Prolly has some debug-mode stuff in here
}

// FUNCTION: MANALINKINTERFACE 0x10001226
undefined4 FamInterface_Flush(void)
{
  if ((*g_FamInterfaceGpd_addr & 0x4000) != 0)
  {
    if (WaitForSingleObject(g_fam_InterfaceMutex, 0xffffffff) == 0)
    {
      *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x400;
      ReleaseMutex(g_fam_InterfaceMutex);
      while ((*g_FamInterfaceGpd_addr & 0x400) != 0)
      {
        Sleep(100);
      }
    }

    return 0;
  }
  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x1000129A
undefined4 __cdecl FamInterface_SendPacket(int packet, uint packet_type)
{
  struct
  {
    char debugMsg[1024];
    LPVOID packet_data;
  } s;

  while ((*g_FamInterfaceGpd_addr & 0x100) != 0)
  {
    Sleep(0xfa);
  }

  if (WaitForSingleObject(g_fam_InterfaceMutex, 0xffffffff) == 0)
  {
    if ((*g_FamInterfaceGpd_addr & 0x4000) == 0)
    {
      ReleaseMutex(g_fam_InterfaceMutex);
      return 10;
    }
    if (*(uint *)(packet + 0x28) < 0x3400)
    {
      *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x100;
      g_FamInterfaceGpd_addr[4] = packet_type;
      g_FamInterfaceGpd_addr[5] = *(uint *)(packet + 0x20);
      g_FamInterfaceGpd_addr[6] = *(uint *)(packet + 0x24);
      g_FamInterfaceGpd_addr[7] = *(uint *)(packet + 0x28);
      if (*(uint *)(packet + 0x28) > 0)
      {
        s.packet_data = GlobalLock(*(HGLOBAL *)(packet + 0x2c));
        memcpy(g_FamInterfaceGpd_addr + 8, s.packet_data, *(size_t *)(packet + 0x28));
        GlobalUnlock(*(HGLOBAL *)(packet + 0x2c));
      }
      *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x40;
      ReleaseMutex(g_fam_InterfaceMutex);
      SetEvent(g_famI_SendEvent);
      return 0;
    }
    else
    {
      sprintf(s.debugMsg, "The size of the data in this packet is larger than %lu bytes", 0x3400);
      MessageBoxA((HWND)0x0, s.debugMsg, "FamInterface Error", 0);
    }
    ReleaseMutex(g_fam_InterfaceMutex);
  }
  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x10001410
undefined4 __cdecl FamInterface_ReceivePacket(uint packet_type, int out_packet)
{
  if (WaitForSingleObject(g_fam_InterfaceMutex, 0xffffffff) == 0)
  {
    if ((*g_FamInterfaceGpd_addr & 0x80) == 0)
    {
      ReleaseMutex(g_fam_InterfaceMutex);
      return 4;
    }

    if ((*g_FamInterfaceGpd_addr & 0x4000) == 0)
    {
      ReleaseMutex(g_fam_InterfaceMutex);
      return 10;
    }

    if (g_FamInterfaceGpd_addr[0xd09] != packet_type)
    {
      ReleaseMutex(g_fam_InterfaceMutex);
      return 4;
    }

    *(uint *)(out_packet + 0x20) = g_FamInterfaceGpd_addr[0xd08];
    *(uint *)(out_packet + 0x28) = g_FamInterfaceGpd_addr[0xd0a];
    if (*(uint *)(out_packet + 0x28) > 0)
    {
      *(HGLOBAL *)(out_packet + 0x2c) = GlobalAlloc(0x42, *(size_t *)(out_packet + 0x28));
      if (*(int *)(out_packet + 0x2c) == 0)
      {
        DebugLog("Could not lock down the packet's global data", 1);
        *(undefined4 *)(out_packet + 0x20) = 0;
        *(undefined4 *)(out_packet + 0x28) = 0;
        *(undefined4 *)(out_packet + 0x2c) = 0;
        ReleaseMutex(g_fam_InterfaceMutex);
        return 1;
      }

      memcpy(GlobalLock(*(HGLOBAL *)(out_packet + 0x2c)), g_FamInterfaceGpd_addr + 0xd0b, *(size_t *)(out_packet + 0x28));
      GlobalUnlock(*(HGLOBAL *)(out_packet + 0x2c));
    }
    else
    {
      *(undefined4 *)(out_packet + 0x2c) = 0;
    }
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr & 0xffffff7f;
    ReleaseMutex(g_fam_InterfaceMutex);
    return 0;
  }

  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x100015A4
int __cdecl FamInterface_WaitForOpponent(uint session_id)
{
  int result = 1;

  if (FamInterface_HasOpponent() == 0)
  {
    return result;
  }

  if ((*g_FamInterfaceGpd_addr & 0x4000) == 0)
  {
    return 10;
  }

  while ((*g_FamInterfaceGpd_addr & 0x200) != 0)
  {
    DebugLog("Wait is busy", 0);
    Sleep(500);
  }
  *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x200;
  g_FamInterfaceGpd_addr[1] = session_id;
  *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x20;
  while ((*g_FamInterfaceGpd_addr & 0x10) == 0)
  {
    Sleep(0);
  }

  if (g_FamInterfaceGpd_addr[2] == session_id)
  {
    result = 0;
  }

  *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr & 0xffffffef;
  *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr & 0xfffffdff;

  return result;
}

// FUNCTION: MANALINKINTERFACE 0x10001682
undefined4 FamInterface_Taunt(void)
{
  if ((*g_FamInterfaceGpd_addr & 0x4000) != 0)
  {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x800;
    return 0;
  }
  return 10;
}

// FUNCTION: MANALINKINTERFACE 0x100016B8
undefined4 __cdecl FamInterface_SetDuelVersion(undefined1 major_version, undefined1 minor_version)
{
  ((undefined1 *)(g_FamInterfaceGpd_addr))[0x6935] = major_version;
  ((undefined1 *)(g_FamInterfaceGpd_addr))[0x6936] = minor_version;
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x100016E8
undefined4 __cdecl FamInterface_SetDuelState(int in_duel)
{
  if (in_duel != 0)
  {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x8000;
  }
  else
  {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr & 0xffff7fff;
  }
  *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x20000;
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x1000172A
undefined4 __cdecl FamInterface_GetDuelState(uint *out_duel_state)
{
  *out_duel_state = *g_FamInterfaceGpd_addr & 0x8000;
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x1000174D
undefined4 __cdecl FamInterface_PostDuelResult(uint duel_result)

{
  /* 0x174d  10  FamInterface_PostDuelResult */
  if (((*g_FamInterfaceGpd_addr & 2) != 0) && ((*g_FamInterfaceGpd_addr & 0x8000) != 0))
  {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x40000;
    g_FamInterfaceGpd_addr[3] = duel_result;
  }
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x10001793
bool FamInterface_UpdateScreenName(void)
{
  *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x20000;
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x100017B0
bool FamInterface_EndSession(void)
{
  if (FamInterface_HasOpponent() != 0)
  {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x10000;
    return 0;
  }
  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x100017E4
bool FamInterface_GoneToMovies(void)
{
  if (FamInterface_HasOpponent() != 0)
  {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x100000;
    return 0;
  }
  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x10001818
uint FamInterface_IsManaLinkRunning(void)
{
  return *g_FamInterfaceGpd_addr & 0x4000;
}

// FUNCTION: MANALINKINTERFACE 0x10001834
uint FamInterface_IsNetworkAlive(void)
{
  return *g_FamInterfaceGpd_addr & GPD_FLAGS_ALIVE;
}

// FUNCTION: MANALINKINTERFACE 0x1000184E
uint FamInterface_IsNetworkTEN(void)
{
  return *g_FamInterfaceGpd_addr & GPD_FLAGS_TEN;
}

// FUNCTION: MANALINKINTERFACE 0x10001868
uint FamInterface_IsHost(void)
{
  return *g_FamInterfaceGpd_addr & GPD_FLAGS_HOST;
}

// FUNCTION: MANALINKINTERFACE 0x10001882
undefined4 FamInterface_HasOpponent(void)
{
  return ((*g_FamInterfaceGpd_addr & GPD_FLAGS_HAS_OPPONENT) != 0) && ((*g_FamInterfaceGpd_addr & 0x80000) != 0) ? 1 : 0;
}

// FUNCTION: MANALINKINTERFACE 0x100018BB
undefined4 __cdecl FamInterface_SendFile(uint file_type, char *filename)
{
  if ((*g_FamInterfaceGpd_addr & 0x4000) == 0)
  {
    return 10;
  }

  while ((*g_FamInterfaceGpd_addr & 0x2000) != 0)
  {
    Sleep(0xfa);
  }

  if ((*g_FamInterfaceGpd_addr & GPD_FLAGS_HAS_OPPONENT) != 0)
  {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x2000;
    g_FamInterfaceGpd_addr[0x1a0b] = file_type;
    _mbscpy((char *)(g_FamInterfaceGpd_addr + 0x1a0c), filename);
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x1000;
    return 0;
  }
  return 1;
}
