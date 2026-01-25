#include "cdtools.h"
#include <stdio.h>
#include <winnt.h>
#include <winreg.h>
#include "mystdbool.h"

//TODO: wtf is a 3 byte??
#define undefined3 undefined4

//TODO: this from newer MSVC?
#define LSTATUS LONG

// GLOBAL: CDTOOLS 0x10010030
char s_pctS_10010030[] = "%s\\";

// GLOBAL: CDTOOLS 0x10010034
char s_MTG_10010034[] = "MTG";

// GLOBAL: CDTOOLS 0x10010038
char s_DuelSounds_Manaball_wav_10010038[] = "DuelSounds\\Manaball.wav";

// GLOBAL: CDTOOLS 0x10010050
char s__s__s_10010050[] = "%s\\%s";

// GLOBAL: CDTOOLS 0x10010058
char s_Sound_Locmus15_wav_10010058[] = "Sound\\Locmus15.wav";

// GLOBAL: CDTOOLS 0x1001006C
char s__s__s_1001006c[] = "%s\\%s";

// GLOBAL: CDTOOLS 0x10010074
char s_pctS_10010074[] = "%s\\";

// GLOBAL: CDTOOLS 0x10010078
char s_MTG_10010078[] = "MTG";

// GLOBAL: CDTOOLS 0x1001007C
char s_DuelSounds_Deep_wav_1001007c[] = "DuelSounds\\Deep.wav";

// GLOBAL: CDTOOLS 0x10010090
char s__s__s_10010090[] = "%s\\%s";

// GLOBAL: CDTOOLS 0x10010098
char s_Sound_Locmus15_wav_10010098[] = "Sound\\Locmus15.wav";

// GLOBAL: CDTOOLS 0x100100AC
char s__s__s_100100ac[] = "%s\\%s";

// GLOBAL: CDTOOLS 0x100100b4
char s_pctS_100100b4[] = "%s\\";

// GLOBAL: CDTOOLS 0x100100b8
char s_MTG_100100b8[] = "MTG";

// GLOBAL: CDTOOLS 0x100100BC
char s_DuelSounds_Manaball_wav_100100bc[] = "DuelSounds\\Manaball.wav";

// GLOBAL: CDTOOLS 0x100100D4
char s__s__s_100100d4[] = "%s\\%s";

// GLOBAL: CDTOOLS 0x100100DC
char s_Sound_Locmus15_wav_100100dc[] = "Sound\\Locmus15.wav";

// GLOBAL: CDTOOLS 0x100100F0
char s__s__s_100100f0[] = "%s\\%s";

// GLOBAL: CDTOOLS 0x100100f8
char S_pctS_100100f8[] = "%s\\";

// GLOBAL: CDTOOLS 0x10010110
char s_Software_Microsoft_Windows_Curre_10010110[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer";

// GLOBAL: CDTOOLS 0x1001014C
char s_NoDriveTypeAutoRun_1001014c[] = "NoDriveTypeAutoRun";

// GLOBAL: CDTOOLS 0x10010160
char s_NoDriveTypeAutoRun_10010160[] = "NoDriveTypeAutoRun";

// GLOBAL: CDTOOLS 0x10010174
char s_QueryCancelAutoPlay_10010174[] = "QueryCancelAutoPlay";

// GLOBAL: CDTOOLS 0x10010188
char s_Software_Microsoft_Windows_Curre_10010188[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer";

// GLOBAL: CDTOOLS 0x100101C4
char s_NoDriveTypeAutoRun_100101c4[] = "NoDriveTypeAutoRun";

// GLOBAL: CDTOOLS 0x100123cc
HINSTANCE DAT_100123cc = 0x0;

// GLOBAL: CDTOOLS 0x100123d0
undefined4 DAT_100123d0 = 0x00000007;

// GLOBAL: CDTOOLS 0x100123d4
undefined4 DAT_100123d4 = 0x0;

// GLOBAL: CDTOOLS 0x10013520
undefined4 DAT_10013520 = 0x00000000;

// FUNCTION: CDTOOLS 0x10001000
undefined4 __cdecl CheckOriginalCD(undefined4 param_1,int *param_2,undefined4 param_3)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined4 uVar3;
  HANDLE pvVar4;
  CHAR local_330 [3];
  undefined1 local_32d;
  CHAR local_22c [260];
  int local_128;
  int local_124;
  int local_120;
  DWORD local_11c;
  DWORD local_118;
  DWORD local_114;
  DWORD local_110;
  char local_10c [264];
  
                    /* 0x1000  4  CheckOriginalCD */
  local_120 = 0;
  local_124 = 0;
  while ((local_124 < *param_2 && (local_120 == 0))) {
    bVar1 = IsCDDrive(param_3);
    local_128 = CONCAT31(extraout_var,bVar1);
    if (local_128 == 0) {
      Sleep(1000);
    }
    else {
      sprintf(local_10c,&s_pctS_10010030,param_3);
      local_128 = GetVolumeInformationA
                            (local_10c,local_330,0x104,&local_11c,&local_118,&local_114,local_22c,
                             0x104);
      local_32d = 0;
      iVar2 = strcmp(local_330,&s_MTG_10010034);
      if (iVar2 == 0) {
        local_120 = 1;
      }
      else {
        Sleep(1000);
      }
    }
    local_124 = local_124 + 1;
  }
  if (local_120 == 0) {
    uVar3 = 0;
  }
  else {
    sprintf(local_10c,s__s__s_10010050,param_3,s_DuelSounds_Manaball_wav_10010038);
    pvVar4 = CreateFileA(local_10c,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x80,(HANDLE)0x0);
    if (pvVar4 == (HANDLE)0xffffffff) {
      sprintf(local_10c,s__s__s_1001006c,param_3,s_Sound_Locmus15_wav_10010058);
      pvVar4 = CreateFileA(local_10c,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x80,(HANDLE)0x0);
      if (pvVar4 == (HANDLE)0xffffffff) {
        uVar3 = 0;
      }
      else {
        local_110 = GetFileSize(pvVar4,(LPDWORD)0x0);
        if (local_110 == 0x307864) {
          CloseHandle(pvVar4);
          uVar3 = 1;
        }
        else {
          CloseHandle(pvVar4);
          uVar3 = 0;
        }
      }
    }
    else {
      CloseHandle(pvVar4);
      uVar3 = 0;
    }
  }
  return uVar3;
}

// FUNCTION: CDTOOLS 0x10001234
undefined4 __cdecl CheckDoPCD(undefined4 param_1,int *param_2,undefined4 param_3)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined4 uVar3;
  HANDLE pvVar4;
  CHAR local_330 [3];
  undefined1 local_32d;
  CHAR local_22c [260];
  int local_128;
  int local_124;
  int local_120;
  DWORD local_11c;
  DWORD local_118;
  DWORD local_114;
  DWORD local_110;
  char local_10c [264];
  
                    /* 0x1234  3  CheckDoPCD */
  local_120 = 0;
  local_124 = 0;
  while ((local_124 < *param_2 && (local_120 == 0))) {
    bVar1 = IsCDDrive(param_3);
    local_128 = CONCAT31(extraout_var,bVar1);
    if (local_128 == 0) {
      Sleep(1000);
    }
    else {
      sprintf(local_10c,&s_pctS_10010074,param_3);
      local_128 = GetVolumeInformationA
                            (local_10c,local_330,0x104,&local_11c,&local_118,&local_114,local_22c,
                             0x104);
      local_32d = 0;
      iVar2 = strcmp(local_330,&s_MTG_10010078);
      if (iVar2 == 0) {
        local_120 = 1;
      }
      else {
        Sleep(1000);
      }
    }
    local_124 = local_124 + 1;
  }
  if (local_120 == 0) {
    uVar3 = 0;
  }
  else {
    sprintf(local_10c,s__s__s_10010090,param_3,s_DuelSounds_Deep_wav_1001007c);
    pvVar4 = CreateFileA(local_10c,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x80,(HANDLE)0x0);
    if (pvVar4 == (HANDLE)0xffffffff) {
      uVar3 = 0;
    }
    else {
      CloseHandle(pvVar4);
      sprintf(local_10c,s__s__s_100100ac,param_3,s_Sound_Locmus15_wav_10010098);
      pvVar4 = CreateFileA(local_10c,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x80,(HANDLE)0x0);
      if (pvVar4 == (HANDLE)0xffffffff) {
        uVar3 = 0;
      }
      else {
        local_110 = GetFileSize(pvVar4,(LPDWORD)0x0);
        if (local_110 == 0x307864) {
          CloseHandle(pvVar4);
          uVar3 = 1;
        }
        else {
          CloseHandle(pvVar4);
          uVar3 = 0;
        }
      }
    }
  }
  return uVar3;
}

// FUNCTION: CDTOOLS 0x10001468
undefined4 __cdecl CheckSotaCD(undefined4 param_1,int *param_2,undefined4 param_3)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined4 uVar3;
  HANDLE pvVar4;
  CHAR local_330 [3];
  undefined1 local_32d;
  CHAR local_22c [260];
  int local_128;
  int local_124;
  int local_120;
  DWORD local_11c;
  DWORD local_118;
  DWORD local_114;
  DWORD local_110;
  char local_10c [264];
  
                    /* 0x1468  5  CheckSotaCD */
  local_120 = 0;
  local_124 = 0;
  while ((local_124 < *param_2 && (local_120 == 0))) {
    bVar1 = IsCDDrive(param_3);
    local_128 = CONCAT31(extraout_var,bVar1);
    if (local_128 == 0) {
      Sleep(1000);
    }
    else {
      sprintf(local_10c,&s_pctS_100100b4,param_3);
      local_128 = GetVolumeInformationA
                            (local_10c,local_330,0x104,&local_11c,&local_118,&local_114,local_22c,
                             0x104);
      local_32d = 0;
      iVar2 = strcmp(local_330,&s_MTG_100100b8);
      if (iVar2 == 0) {
        local_120 = 1;
      }
      else {
        Sleep(1000);
      }
    }
    local_124 = local_124 + 1;
  }
  if (local_120 == 0) {
    uVar3 = 0;
  }
  else {
    sprintf(local_10c,s__s__s_100100d4,param_3,s_DuelSounds_Manaball_wav_100100bc);
    pvVar4 = CreateFileA(local_10c,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x80,(HANDLE)0x0);
    if (pvVar4 == (HANDLE)0xffffffff) {
      uVar3 = 0;
    }
    else {
      CloseHandle(pvVar4);
      sprintf(local_10c,s__s__s_100100f0,param_3,s_Sound_Locmus15_wav_100100dc);
      pvVar4 = CreateFileA(local_10c,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x80,(HANDLE)0x0);
      if (pvVar4 == (HANDLE)0xffffffff) {
        uVar3 = 0;
      }
      else {
        local_110 = GetFileSize(pvVar4,(LPDWORD)0x0);
        if (local_110 == 0x307864) {
          CloseHandle(pvVar4);
          uVar3 = 1;
        }
        else {
          CloseHandle(pvVar4);
          uVar3 = 0;
        }
      }
    }
  }
  return uVar3;
}

// FUNCTION: CDTOOLS 0x1000169C
bool __cdecl IsCDDrive(undefined4 param_1)
{
  UINT UVar1;
  char local_10c [264];
  
                    /* 0x169c  6  IsCDDrive */
  sprintf(local_10c,&S_pctS_100100f8,param_1);
  UVar1 = GetDriveTypeA(local_10c);
  return UVar1 == 5;
}

// FUNCTION: CDTOOLS 0x100017C0
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 Autoplay_ShutDown(void)

{
  LSTATUS LVar1;
  HKEY local_18;
  BYTE local_14 [4];
  DWORD local_10 [3];
  
                    /* 0x17c0  2  Autoplay_ShutDown */
  local_10[2] = 0x95;
  local_10[1] = 0xff;
  local_14[0] = 0xff;
  local_14[1] = '\0';
  local_14[2] = '\0';
  local_14[3] = '\0';
  local_10[0] = 4;
  local_18 = (HKEY)0x0;
  DAT_100123d0 = 0x95;
  LVar1 = RegOpenKeyExA((HKEY)0x80000001,s_Software_Microsoft_Windows_Curre_10010110,0,0xf003f,
                        &local_18);
  if (LVar1 == 0) {
    LVar1 = RegQueryValueExA(local_18,s_NoDriveTypeAutoRun_1001014c,(LPDWORD)0x0,(LPDWORD)0x0,
                             &DAT_100123d0,local_10);
    if (LVar1 == 0) {
      RegSetValueExA(local_18,s_NoDriveTypeAutoRun_10010160,0,3,local_14,4);
    }
    else {
      DAT_100123d0 = local_10[2];
    }
    RegFlushKey(local_18);
    RegCloseKey(local_18);
  }
  DAT_100123d4 = RegisterWindowMessageA(s_QueryCancelAutoPlay_10010174);
  return 1;
}

// FUNCTION: CDTOOLS 0x10001893
undefined4 Autoplay_Restore(void)

{
  LSTATUS LVar1;
  HKEY local_c;
  undefined4 local_8;
  
                    /* 0x1893  1  Autoplay_Restore */
  local_8 = 0;
  LVar1 = RegOpenKeyExA((HKEY)0x80000001,s_Software_Microsoft_Windows_Curre_10010188,0,0xf003f,
                        &local_c);
  if (LVar1 == 0) {
    local_8 = 1;
    RegSetValueExA(local_c,s_NoDriveTypeAutoRun_100101c4,0,3,&DAT_100123d0,4);
    RegFlushKey(local_c);
    RegCloseKey(local_c);
  }
  return local_8;
}

// FUNCTION: CDTOOLS 0x10001910
int WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID reserved)
{
  DAT_100123cc = hDllHandle;
  switch(nReason) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
    break;
  }
  return 1;
}

// LIBRARY: CDTOOLS 0x10002010 SYMBOL
// _DllMainCRTStartup