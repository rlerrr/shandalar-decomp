#include "manalinkinterface.h"
#include <stdio.h>
#include <winnt.h>
#include <winreg.h>
#include "mystdbool.h"

void DebugLog(char* str, int idk);

// GLOBAL: MANALINKINTERFACE 0x10008000
undefined4 DAT_10008000 = 0x00006938;

// GLOBAL: MANALINKINTERFACE 0x10009030
HANDLE g_FamInterfaceGpd_handle = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x10009034
undefined4 g_fam_InterfaceMutex = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x10009038
HANDLE  g_famI_SendEvent = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x1000903C
char s_DLL_PROCESS_ATTACH_1000903c[] = "DLL_PROCESS_ATTACH";

// GLOBAL: MANALINKINTERFACE 0x10009050
char s_fam_InterfaceMutex_10009050[] = "fam_InterfaceMutex";

// GLOBAL: MANALINKINTERFACE 0x10009064
char s_ManalinkInterface_10009064[] = "ManalinkInterface";

// GLOBAL: MANALINKINTERFACE 0x10009078
char s_An_error_has_occurred_while_crea_10009078[] = "An error has occurred while creating the interface mutex.";

// GLOBAL: MANALINKINTERFACE 0x100090B4
char s_famI_SendEvent_100090b4[] = "famI_SendEvent";

// GLOBAL: MANALINKINTERFACE 0x100090C4
char s_ManalinkInterface_dll_100090c4[] = "ManalinkInterface.dll";

// GLOBAL: MANALINKINTERFACE 0x100090DC
char s_An_error_has_occurred_while_crea_100090dc[] = "An error has occurred while creating the send event.";

// GLOBAL: MANALINKINTERFACE 0x10009114
char s_DLL_PROCESS_DETACH_10009114[] = "DLL_PROCESS_DETACH";

// GLOBAL: MANALINKINTERFACE 0x10009128
char s_FamInterfaceGpd_10009128[] = "FamInterfaceGpd";

// GLOBAL: MANALINKINTERFACE 0x10009138
char s_gpd_is_located_at_0x_x_10009138[] = "gpd is located at 0x%x";

// GLOBAL: MANALINKINTERFACE 0x10009150
char s_The_size_of_the_data_in_this_pac_10009150[] = "The size of the data in this packet is larger than %lu bytes";

// GLOBAL: MANALINKINTERFACE 0x10009190
char s_FamInterface_Error_10009190[] = "FamInterface Error";

// GLOBAL: MANALINKINTERFACE 0x100091A4
char s_Could_not_lock_down_the_packet_s_100091a4[] = "Could not lock down the packet's global data";

// GLOBAL: MANALINKINTERFACE 0x100091D4
char s_Wait_is_busy_100091d4[] = "Wait is busy";

// GLOBAL: MANALINKINTERFACE 0x100091F0
undefined4 DAT_100091f0 = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x10009200
undefined4 DAT_10009200 = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x1000924C
undefined4 DAT_1000924c = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x10009250
undefined4 DAT_10009250 = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x1000b31c
HINSTANCE DAT_1000b31c = 0x0;

// GLOBAL: MANALINKINTERFACE 0x1000B320
undefined4* g_FamInterfaceGpd_addr = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x1000C448
undefined4 DAT_1000c448 = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x1000C44C
undefined4 DAT_1000c44c = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x1000C458
undefined4 DAT_1000c458 = 0x00000000;

enum GpdFlags {
  GPD_FLAGS_ALIVE = 1,
  GPD_FLAGS_TEN = 2,
  GPD_FLAGS_HOST = 4,
  GPD_FLAGS_HAS_OPPONENT = 8,
};

// FUNCTION: MANALINKINTERFACE 0x10001000
int WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID reserved)
{
  //int BVar1 = nReason;
  
  switch (nReason) {
    case 1:
      DebugLog(s_DLL_PROCESS_ATTACH_1000903c, 0);
      DAT_1000b31c = hDllHandle;
      DisableThreadLibraryCalls(hDllHandle);
      if (g_fam_InterfaceMutex == (HANDLE)0x0) {
        g_fam_InterfaceMutex = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,s_fam_InterfaceMutex_10009050);
      }
      if (g_fam_InterfaceMutex == (HANDLE)0x0) {
        MessageBoxA((HWND)0x0,s_An_error_has_occurred_while_crea_10009078,s_ManalinkInterface_10009064
                    ,0x10);
        return 0;
      }
      if (g_famI_SendEvent == (HANDLE)0x0) {
        if ((g_famI_SendEvent = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,1,0,s_famI_SendEvent_100090b4)) == (HANDLE)0x0) {
          MessageBoxA((HWND)0x0,s_An_error_has_occurred_while_crea_100090dc,
                      s_ManalinkInterface_dll_100090c4,0x10);
          return 0;
        }
      }
      return InitFamInterfaceGpd();
    case 0:
      DebugLog(s_DLL_PROCESS_DETACH_10009114, 0);
      return CloseFamInterfaceGpd();
  }
  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x1000111B
BOOL InitFamInterfaceGpd(void)
{
  struct {
    char debugMsg [1024];
    BOOL success;
  } s;
  
  g_FamInterfaceGpd_handle = CreateFileMappingA((HANDLE)0xffffffff,(LPSECURITY_ATTRIBUTES)0x0,4,0,DAT_10008000,
                                    s_FamInterfaceGpd_10009128);
  if (g_FamInterfaceGpd_handle == (HANDLE)0x0) {
    return 0;
  }

  s.success = GetLastError() != ERROR_ALREADY_EXISTS;
  g_FamInterfaceGpd_addr = MapViewOfFile(g_FamInterfaceGpd_handle,2,0,0,0);
  sprintf(s.debugMsg, s_gpd_is_located_at_0x_x_10009138,g_FamInterfaceGpd_addr);
  DebugLog(s.debugMsg, 0);
  if (g_FamInterfaceGpd_addr == (LPVOID)0x0) {
    return 0;
  }

  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x100011E4
BOOL CloseFamInterfaceGpd(void)
{
  if (UnmapViewOfFile(g_FamInterfaceGpd_addr) == 0) {
    return 0;
  }

  return CloseHandle(g_FamInterfaceGpd_handle);
}

// FUNCTION: MANALINKINTERFACE 0x1000121B
void DebugLog(char* str, int logLevel)
{
  //Prolly has some debug-mode stuff in here
}

// FUNCTION: MANALINKINTERFACE 0x10001226
undefined4 FamInterface_Flush(void)
{
  if ((*g_FamInterfaceGpd_addr & 0x4000) != 0) {
    if (WaitForSingleObject(g_fam_InterfaceMutex,0xffffffff) == 0) {
      *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x400;
      ReleaseMutex(g_fam_InterfaceMutex);
      while ((*g_FamInterfaceGpd_addr & 0x400) != 0) {
        Sleep(100);
      }
    }

    return 0;
  }
  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x1000129A
undefined4 __cdecl FamInterface_SendPacket(int param_1,uint param_2)
{
  struct {
    char debugMsg [1024];
    LPVOID local_8;
  }s;
  
  while ((*g_FamInterfaceGpd_addr & 0x100) != 0) {
    Sleep(0xfa);
  }
  
  if (WaitForSingleObject(g_fam_InterfaceMutex,0xffffffff) == 0) {
    if ((*g_FamInterfaceGpd_addr & 0x4000) == 0) {
      ReleaseMutex(g_fam_InterfaceMutex);
      return 10;
    }
    if (*(uint *)(param_1 + 0x28) < 0x3400) {
      *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x100;
      g_FamInterfaceGpd_addr[4] = param_2;
      g_FamInterfaceGpd_addr[5] = *(uint *)(param_1 + 0x20);
      g_FamInterfaceGpd_addr[6] = *(uint *)(param_1 + 0x24);
      g_FamInterfaceGpd_addr[7] = *(uint *)(param_1 + 0x28);
      if (*(uint *)(param_1 + 0x28) > 0) {
        s.local_8 = GlobalLock(*(HGLOBAL *)(param_1 + 0x2c));
        memcpy(g_FamInterfaceGpd_addr + 8,s.local_8,*(size_t *)(param_1 + 0x28));
        GlobalUnlock(*(HGLOBAL *)(param_1 + 0x2c));
      }
      *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x40;
      ReleaseMutex(g_fam_InterfaceMutex);
      SetEvent(g_famI_SendEvent);
      return 0;
    } else {
      sprintf(s.debugMsg,s_The_size_of_the_data_in_this_pac_10009150,0x3400);
      MessageBoxA((HWND)0x0,s.debugMsg,s_FamInterface_Error_10009190,0);
    }
    ReleaseMutex(g_fam_InterfaceMutex);
  }
  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x10001410
undefined4 __cdecl FamInterface_ReceivePacket(uint param_1,int param_2)
{
  if (WaitForSingleObject(g_fam_InterfaceMutex,0xffffffff) == 0) {
    if ((*g_FamInterfaceGpd_addr & 0x80) == 0) {
      ReleaseMutex(g_fam_InterfaceMutex);
      return 4;
    }

    if ((*g_FamInterfaceGpd_addr & 0x4000) == 0) {
      ReleaseMutex(g_fam_InterfaceMutex);
      return 10;
    }

    if (g_FamInterfaceGpd_addr[0xd09] != param_1) {
      ReleaseMutex(g_fam_InterfaceMutex);
      return 4;
    }

    *(uint *)(param_2 + 0x20) = g_FamInterfaceGpd_addr[0xd08];
    *(uint *)(param_2 + 0x28) = g_FamInterfaceGpd_addr[0xd0a];
    if (*(uint *)(param_2 + 0x28) > 0) {
      *(HGLOBAL *)(param_2 + 0x2c) = GlobalAlloc(0x42,*(size_t *)(param_2 + 0x28));
      if (*(int *)(param_2 + 0x2c) == 0) {
        DebugLog(s_Could_not_lock_down_the_packet_s_100091a4, 1);
        *(undefined4 *)(param_2 + 0x20) = 0;
        *(undefined4 *)(param_2 + 0x28) = 0;
        *(undefined4 *)(param_2 + 0x2c) = 0;
        ReleaseMutex(g_fam_InterfaceMutex);
        return 1;
      }

      memcpy(GlobalLock(*(HGLOBAL *)(param_2 + 0x2c)),g_FamInterfaceGpd_addr + 0xd0b,*(size_t *)(param_2 + 0x28));
      GlobalUnlock(*(HGLOBAL *)(param_2 + 0x2c));
    }
    else {
      *(undefined4 *)(param_2 + 0x2c) = 0;
    }
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr & 0xffffff7f;
    ReleaseMutex(g_fam_InterfaceMutex);
    return 0;
  }

  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x100015A4
int __cdecl FamInterface_WaitForOpponent(uint param_1)
{
  int result = 1;
  
  if (FamInterface_HasOpponent() == 0) {
    return result;
  }

  if ((*g_FamInterfaceGpd_addr & 0x4000) == 0) {
    return 10;
  }

  while ((*g_FamInterfaceGpd_addr & 0x200) != 0) {
    DebugLog(s_Wait_is_busy_100091d4, 0);
    Sleep(500);
  }
  *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x200;
  g_FamInterfaceGpd_addr[1] = param_1;
  *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x20;
  while ((*g_FamInterfaceGpd_addr & 0x10) == 0) {
    Sleep(0);
  }

  if (g_FamInterfaceGpd_addr[2] == param_1) {
    result = 0;
  }

  *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr & 0xffffffef;
  *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr & 0xfffffdff;

  return result;
}

// FUNCTION: MANALINKINTERFACE 0x10001682
undefined4 FamInterface_Taunt(void)
{
  if ((*g_FamInterfaceGpd_addr & 0x4000) != 0) {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x800;
    return 0;
  }
  return 10;
}

// FUNCTION: MANALINKINTERFACE 0x100016B8
undefined4 __cdecl FamInterface_SetDuelVersion(undefined1 param_1,undefined1 param_2)
{
  ((undefined1 *)(g_FamInterfaceGpd_addr))[0x6935] = param_1;
  ((undefined1 *)(g_FamInterfaceGpd_addr))[0x6936] = param_2;
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x100016E8
undefined4 __cdecl FamInterface_SetDuelState(int param_1)
{
  if (param_1 != 0) {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x8000;    
  }
  else {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr & 0xffff7fff;
  }
  *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x20000;
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x1000172A
undefined4 __cdecl FamInterface_GetDuelState(uint *param_1)
{
  *param_1 = *g_FamInterfaceGpd_addr & 0x8000;
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x1000174D
undefined4 __cdecl FamInterface_PostDuelResult(uint param_1)

{
                    /* 0x174d  10  FamInterface_PostDuelResult */
  if (((*g_FamInterfaceGpd_addr & 2) != 0) && ((*g_FamInterfaceGpd_addr & 0x8000) != 0)) {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x40000;
    g_FamInterfaceGpd_addr[3] = param_1;
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
  if (FamInterface_HasOpponent() != 0) {
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x10000;
    return 0;
  }
  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x100017E4
bool FamInterface_GoneToMovies(void)
{
  if (FamInterface_HasOpponent() != 0) {
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
  return ((*g_FamInterfaceGpd_addr & GPD_FLAGS_HAS_OPPONENT) != 0) && ((*g_FamInterfaceGpd_addr & 0x80000) != 0)? 1 : 0;
}

// FUNCTION: MANALINKINTERFACE 0x100018BB
undefined4 __cdecl FamInterface_SendFile(uint param_1,char *param_2)
{
  if ((*g_FamInterfaceGpd_addr & 0x4000) == 0) {
    return 10;
  }

  while ((*g_FamInterfaceGpd_addr & 0x2000) != 0) {
    Sleep(0xfa);
  }

  if ((*g_FamInterfaceGpd_addr & GPD_FLAGS_HAS_OPPONENT) != 0) {  
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x2000;
    g_FamInterfaceGpd_addr[0x1a0b] = param_1;
    _mbscpy((char *)(g_FamInterfaceGpd_addr + 0x1a0c),param_2);
    *g_FamInterfaceGpd_addr = *g_FamInterfaceGpd_addr | 0x1000;
    return 0;
  }
  return 1;
}
