#include "manalinkinterface.h"
#include <stdio.h>
#include <winnt.h>
#include <winreg.h>
#include "mystdbool.h"

void FUN_1000121b(void);

// GLOBAL: MANALINKINTERFACE 0x10008000
undefined4 DAT_10008000 = 0x00006938;

// GLOBAL: MANALINKINTERFACE 0x10009030
undefined4 DAT_10009030 = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x10009034
undefined4 DAT_10009034 = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x10009038
undefined4 DAT_10009038 = 0x00000000;

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
undefined4* DAT_1000b320 = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x1000C448
undefined4 DAT_1000c448 = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x1000C44C
undefined4 DAT_1000c44c = 0x00000000;

// GLOBAL: MANALINKINTERFACE 0x1000C458
undefined4 DAT_1000c458 = 0x00000000;

// FUNCTION: MANALINKINTERFACE 0x10001000
int WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID reserved)
{
  BOOL BVar1;
  
  if (nReason == 0) {
    FUN_1000121b();
    BVar1 = FUN_100011e4();
  }
  else if (nReason == 1) {
    FUN_1000121b();
    DAT_1000b31c = hDllHandle;
    DisableThreadLibraryCalls(hDllHandle);
    if (DAT_10009034 == (HANDLE)0x0) {
      DAT_10009034 = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,s_fam_InterfaceMutex_10009050);
    }
    if (DAT_10009034 == (HANDLE)0x0) {
      MessageBoxA((HWND)0x0,s_An_error_has_occurred_while_crea_10009078,s_ManalinkInterface_10009064
                  ,0x10);
      BVar1 = 0;
    }
    else {
      if ((DAT_10009038 == (HANDLE)0x0) &&
         (DAT_10009038 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,1,0,s_famI_SendEvent_100090b4),
         DAT_10009038 == (HANDLE)0x0)) {
        MessageBoxA((HWND)0x0,s_An_error_has_occurred_while_crea_100090dc,
                    s_ManalinkInterface_dll_100090c4,0x10);
        return 0;
      }
      BVar1 = FUN_1000111b();
    }
  }
  else {
    BVar1 = 1;
  }
  return BVar1;
}

// FUNCTION: MANALINKINTERFACE 0x1000111B
undefined4 FUN_1000111b(void)

{
  undefined4 uVar1;
  DWORD DVar2;
  char local_408 [1024];
  uint local_8;
  
  DAT_10009030 = CreateFileMappingA((HANDLE)0xffffffff,(LPSECURITY_ATTRIBUTES)0x0,4,0,DAT_10008000,
                                    s_FamInterfaceGpd_10009128);
  if (DAT_10009030 == (HANDLE)0x0) {
    uVar1 = 0;
  }
  else {
    DVar2 = GetLastError();
    local_8 = (uint)(DVar2 != 0xb7);
    DAT_1000b320 = MapViewOfFile(DAT_10009030,2,0,0,0);
    sprintf(local_408,s_gpd_is_located_at_0x_x_10009138,DAT_1000b320);
    FUN_1000121b();
    if (DAT_1000b320 == (LPVOID)0x0) {
      uVar1 = 0;
    }
    else {
      uVar1 = 1;
    }
  }
  return uVar1;
}

// FUNCTION: MANALINKINTERFACE 0x100011E4
BOOL FUN_100011e4(void)

{
  BOOL BVar1;
  
  BVar1 = UnmapViewOfFile(DAT_1000b320);
  if (BVar1 == 0) {
    BVar1 = 0;
  }
  else {
    BVar1 = CloseHandle(DAT_10009030);
  }
  return BVar1;
}

// FUNCTION: MANALINKINTERFACE 0x1000121B
void FUN_1000121b(void)
{
  return;
}


// FUNCTION: MANALINKINTERFACE 0x10001226
undefined4 FamInterface_Flush(void)

{
  DWORD DVar1;
  undefined4 uVar2;
  
                    /* 0x1226  2  FamInterface_Flush */
  if ((*DAT_1000b320 & 0x4000) == 0) {
    uVar2 = 1;
  }
  else {
    DVar1 = WaitForSingleObject(DAT_10009034,0xffffffff);
    if (DVar1 == 0) {
      *DAT_1000b320 = *DAT_1000b320 | 0x400;
      ReleaseMutex(DAT_10009034);
      while ((*DAT_1000b320 & 0x400) != 0) {
        Sleep(100);
      }
    }
    uVar2 = 0;
  }
  return uVar2;
}

// FUNCTION: MANALINKINTERFACE 0x1000129A
undefined4 __cdecl FamInterface_SendPacket(int param_1,uint param_2)

{
  DWORD DVar1;
  char local_408 [1024];
  LPVOID local_8;
  
  while ((*DAT_1000b320 & 0x100) != 0) {
    Sleep(0xfa);
  }
  DVar1 = WaitForSingleObject(DAT_10009034,0xffffffff);
  if (DVar1 == 0) {
    if ((*DAT_1000b320 & 0x4000) == 0) {
      ReleaseMutex(DAT_10009034);
      return 10;
    }
    if (*(uint *)(param_1 + 0x28) < 0x3400) {
      *DAT_1000b320 = *DAT_1000b320 | 0x100;
      DAT_1000b320[4] = param_2;
      DAT_1000b320[5] = *(uint *)(param_1 + 0x20);
      DAT_1000b320[6] = *(uint *)(param_1 + 0x24);
      DAT_1000b320[7] = *(uint *)(param_1 + 0x28);
      if (*(int *)(param_1 + 0x28) != 0) {
        local_8 = GlobalLock(*(HGLOBAL *)(param_1 + 0x2c));
        memcpy(DAT_1000b320 + 8,local_8,*(size_t *)(param_1 + 0x28));
        GlobalUnlock(*(HGLOBAL *)(param_1 + 0x2c));
      }
      *DAT_1000b320 = *DAT_1000b320 | 0x40;
      ReleaseMutex(DAT_10009034);
      SetEvent(DAT_10009038);
      return 0;
    }
    sprintf(local_408,s_The_size_of_the_data_in_this_pac_10009150,0x3400);
    MessageBoxA((HWND)0x0,local_408,s_FamInterface_Error_10009190,0);
    ReleaseMutex(DAT_10009034);
  }
  return 1;
}

// FUNCTION: MANALINKINTERFACE 0x10001410
undefined4 __cdecl FamInterface_ReceivePacket(uint param_1,int param_2)

{
  DWORD DVar1;
  undefined4 uVar2;
  HGLOBAL pvVar3;
  LPVOID _Dst;
  uint *_Src;
  size_t _Size;
  
                    /* 0x1410  11  FamInterface_ReceivePacket */
  DVar1 = WaitForSingleObject(DAT_10009034,0xffffffff);
  if (DVar1 == 0) {
    if ((*DAT_1000b320 & 0x80) == 0) {
      ReleaseMutex(DAT_10009034);
      uVar2 = 4;
    }
    else if ((*DAT_1000b320 & 0x4000) == 0) {
      ReleaseMutex(DAT_10009034);
      uVar2 = 10;
    }
    else if (DAT_1000b320[0xd09] == param_1) {
      *(uint *)(param_2 + 0x20) = DAT_1000b320[0xd08];
      *(uint *)(param_2 + 0x28) = DAT_1000b320[0xd0a];
      if (*(int *)(param_2 + 0x28) == 0) {
        *(undefined4 *)(param_2 + 0x2c) = 0;
      }
      else {
        pvVar3 = GlobalAlloc(0x42,*(size_t *)(param_2 + 0x28));
        *(HGLOBAL *)(param_2 + 0x2c) = pvVar3;
        if (*(int *)(param_2 + 0x2c) == 0) {
          FUN_1000121b();
          *(undefined4 *)(param_2 + 0x20) = 0;
          *(undefined4 *)(param_2 + 0x28) = 0;
          *(undefined4 *)(param_2 + 0x2c) = 0;
          ReleaseMutex(DAT_10009034);
          return 1;
        }
        _Size = *(size_t *)(param_2 + 0x28);
        _Src = DAT_1000b320 + 0xd0b;
        _Dst = GlobalLock(*(HGLOBAL *)(param_2 + 0x2c));
        memcpy(_Dst,_Src,_Size);
        GlobalUnlock(*(HGLOBAL *)(param_2 + 0x2c));
      }
      *DAT_1000b320 = *DAT_1000b320 & 0xffffff7f;
      ReleaseMutex(DAT_10009034);
      uVar2 = 0;
    }
    else {
      ReleaseMutex(DAT_10009034);
      uVar2 = 4;
    }
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

// FUNCTION: MANALINKINTERFACE 0x100015A4
undefined1 __cdecl FamInterface_WaitForOpponent(uint param_1)

{
  int iVar1;
  undefined1 uVar2;
  
                    /* 0x15a4  18  FamInterface_WaitForOpponent */
  iVar1 = FamInterface_HasOpponent();
  if (iVar1 == 0) {
    uVar2 = 1;
  }
  else if ((*DAT_1000b320 & 0x4000) == 0) {
    uVar2 = 10;
  }
  else {
    while ((*DAT_1000b320 & 0x200) != 0) {
      FUN_1000121b();
      Sleep(500);
    }
    *DAT_1000b320 = *DAT_1000b320 | 0x200;
    DAT_1000b320[1] = param_1;
    *DAT_1000b320 = *DAT_1000b320 | 0x20;
    while ((*DAT_1000b320 & 0x10) == 0) {
      Sleep(0);
    }
    uVar2 = DAT_1000b320[2] != param_1;
    *DAT_1000b320 = *DAT_1000b320 & 0xffffffef;
    *DAT_1000b320 = *DAT_1000b320 & 0xfffffdff;
  }
  return uVar2;
}

// FUNCTION: MANALINKINTERFACE 0x10001682
undefined4 FamInterface_Taunt(void)

{
  undefined4 uVar1;
  
                    /* 0x1682  16  FamInterface_Taunt */
  if ((*DAT_1000b320 & 0x4000) == 0) {
    uVar1 = 10;
  }
  else {
    *DAT_1000b320 = *DAT_1000b320 | 0x800;
    uVar1 = 0;
  }
  return uVar1;
}

// FUNCTION: MANALINKINTERFACE 0x100016B8
undefined4 __cdecl FamInterface_SetDuelVersion(undefined1 param_1,undefined1 param_2)

{
                    /* 0x16b8  15  FamInterface_SetDuelVersion */
  *(undefined1 *)(DAT_1000b320 + 0x6935) = param_1;
  *(undefined1 *)(DAT_1000b320 + 0x6936) = param_2;
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x100016E8
undefined4 __cdecl FamInterface_SetDuelState(int param_1)

{
                    /* 0x16e8  14  FamInterface_SetDuelState */
  if (param_1 == 0) {
    *DAT_1000b320 = *DAT_1000b320 & 0xffff7fff;
  }
  else {
    *DAT_1000b320 = *DAT_1000b320 | 0x8000;
  }
  *DAT_1000b320 = *DAT_1000b320 | 0x20000;
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x1000172A
undefined4 __cdecl FamInterface_GetDuelState(uint *param_1)

{
                    /* 0x172a  3  FamInterface_GetDuelState */
  *param_1 = *DAT_1000b320 & 0x8000;
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x1000174D
undefined4 __cdecl FamInterface_PostDuelResult(uint param_1)

{
                    /* 0x174d  10  FamInterface_PostDuelResult */
  if (((*DAT_1000b320 & 2) != 0) && ((*DAT_1000b320 & 0x8000) != 0)) {
    *DAT_1000b320 = *DAT_1000b320 | 0x40000;
    DAT_1000b320[3] = param_1;
  }
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x10001793
undefined4 FamInterface_UpdateScreenName(void)

{
                    /* 0x1793  17  FamInterface_UpdateScreenName */
  *DAT_1000b320 = *DAT_1000b320 | 0x20000;
  return 0;
}

// FUNCTION: MANALINKINTERFACE 0x100017B0
bool FamInterface_EndSession(void)

{
  int iVar1;
  
                    /* 0x17b0  1  FamInterface_EndSession */
  iVar1 = FamInterface_HasOpponent();
  if (iVar1 != 0) {
    *DAT_1000b320 = *DAT_1000b320 | 0x10000;
  }
  return iVar1 == 0;
}

// FUNCTION: MANALINKINTERFACE 0x100017E4
bool FamInterface_GoneToMovies(void)

{
  int iVar1;
  
                    /* 0x17e4  4  FamInterface_GoneToMovies */
  iVar1 = FamInterface_HasOpponent();
  if (iVar1 != 0) {
    *DAT_1000b320 = *DAT_1000b320 | 0x100000;
  }
  return iVar1 == 0;
}

// FUNCTION: MANALINKINTERFACE 0x10001818
uint FamInterface_IsManaLinkRunning(void)

{
                    /* 0x1818  7  FamInterface_IsManaLinkRunning */
  return *DAT_1000b320 & 0x4000;
}

// FUNCTION: MANALINKINTERFACE 0x10001834
uint FamInterface_IsNetworkAlive(void)

{
                    /* 0x1834  8  FamInterface_IsNetworkAlive */
  return *DAT_1000b320 & 1;
}

// FUNCTION: MANALINKINTERFACE 0x1000184E
uint FamInterface_IsNetworkTEN(void)

{
                    /* 0x184e  9  FamInterface_IsNetworkTEN */
  return *DAT_1000b320 & 2;
}

// FUNCTION: MANALINKINTERFACE 0x10001868
uint FamInterface_IsHost(void)

{
                    /* 0x1868  6  FamInterface_IsHost */
  return *DAT_1000b320 & 4;
}

// FUNCTION: MANALINKINTERFACE 0x10001882
undefined4 FamInterface_HasOpponent(void)

{
  undefined4 uVar1;
  
                    /* 0x1882  5  FamInterface_HasOpponent */
  if (((*DAT_1000b320 & 8) == 0) || ((DAT_1000b320[2] & 8) == 0)) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

// FUNCTION: MANALINKINTERFACE 0x100018BB
undefined4 __cdecl FamInterface_SendFile(uint param_1,char *param_2)

{
  undefined4 uVar1;
  
                    /* 0x18bb  12  FamInterface_SendFile */
  if ((*DAT_1000b320 & 0x4000) == 0) {
    uVar1 = 10;
  }
  else {
    while ((*DAT_1000b320 & 0x2000) != 0) {
      Sleep(0xfa);
    }
    if ((*DAT_1000b320 & 8) == 0) {
      uVar1 = 1;
    }
    else {
      *DAT_1000b320 = *DAT_1000b320 | 0x2000;
      DAT_1000b320[0x1a0b] = param_1;
      _mbscpy((char *)(DAT_1000b320 + 0x1a0c),param_2);
      *DAT_1000b320 = *DAT_1000b320 | 0x1000;
      uVar1 = 0;
    }
  }
  return uVar1;
}
