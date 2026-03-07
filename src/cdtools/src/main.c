#include "cdtools.h"
#include <stdio.h>
#include <winnt.h>
#include <winreg.h>
#include "mystdbool.h"

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

// GLOBAL: CDTOOLS 0x100100fc
char s_Testdir_tmp_100100fc[] = "%s\\Testdir.tmp";

// GLOBAL: CDTOOLS 0x1001010c
char DAT_1001010c[] = "wb";

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
HINSTANCE DAT_100123cc;

// GLOBAL: CDTOOLS 0x100123d0
undefined4 g_originalNoDriveTypeAutoRun;

// GLOBAL: CDTOOLS 0x100123d4
undefined4 DAT_100123d4;

// FUNCTION: CDTOOLS 0x10001000
undefined4 __cdecl CheckOriginalCD(undefined4 param_1,int *param_2,undefined4 param_3)
{
  struct {
    HANDLE hFile;                /* EBP - 0x330 */
    CHAR volumeName[0x104];      /* EBP - 0x32c */
    CHAR fileSystemName[0x104];  /* EBP - 0x228 */
    int result;                  /* EBP - 0x124 */
    int tries;                   /* EBP - 0x120 */
    int found;                   /* EBP - 0x11c */
    DWORD volumeSerial;          /* EBP - 0x118 */
    DWORD maximumComponentLength;/* EBP - 0x114 */
    DWORD fileSystemFlags;       /* EBP - 0x110 */
    DWORD fileSize;              /* EBP - 0x10c */
    CHAR rootPath[0x108];        /* EBP - 0x108 */
  } s;
  
  for (s.found = 0, s.tries = 0; *param_2 > s.tries && s.found == 0; s.tries++) {
    s.result = IsCDDrive(param_3);
    if (s.result != 0) {
      sprintf(s.rootPath, s_pctS_10010030, param_3);
      s.result = GetVolumeInformationA(s.rootPath, s.volumeName, 0x104, &s.volumeSerial,
                                      &s.maximumComponentLength, &s.fileSystemFlags,
                                      s.fileSystemName, 0x104);
      s.volumeName[3] = 0;
      if (strcmp(s.volumeName, s_MTG_10010034) == 0) {
        s.found = 1;
      }
      else {
        Sleep(1000);
      }
    }
    else {
      Sleep(1000);
    }
  }

  if (s.found == 0) {
    return 0;
  }

  sprintf(s.rootPath, s__s__s_10010050, param_3, s_DuelSounds_Manaball_wav_10010038);
  s.hFile = CreateFileA(s.rootPath, 0x80000000, 1, (LPSECURITY_ATTRIBUTES)0x0, 3, 0x80, (HANDLE)0x0);
  if (s.hFile != (HANDLE)-1) {
    CloseHandle(s.hFile);
    return 0;
  }

  sprintf(s.rootPath, s__s__s_1001006c, param_3, s_Sound_Locmus15_wav_10010058);
  s.hFile = CreateFileA(s.rootPath, 0x80000000, 1, (LPSECURITY_ATTRIBUTES)0x0, 3, 0x80, (HANDLE)0x0);
  if (s.hFile == (HANDLE)-1) {
    return 0;
  }

  s.fileSize = GetFileSize(s.hFile, (LPDWORD)0x0);
  if (s.fileSize != 0x307864) {
    CloseHandle(s.hFile);
    return 0;
  }

  CloseHandle(s.hFile);
  return 1;
}

// FUNCTION: CDTOOLS 0x10001234
undefined4 __cdecl CheckDoPCD(undefined4 param_1,int *param_2,undefined4 param_3)
{
  struct {
    HANDLE hFile;                /* EBP - 0x330 */
    CHAR volumeName[0x104];      /* EBP - 0x32c */
    CHAR fileSystemName[0x104];  /* EBP - 0x228 */
    int result;                  /* EBP - 0x124 */
    int tries;                   /* EBP - 0x120 */
    int found;                   /* EBP - 0x11c */
    DWORD volumeSerial;          /* EBP - 0x118 */
    DWORD maximumComponentLength;/* EBP - 0x114 */
    DWORD fileSystemFlags;       /* EBP - 0x110 */
    DWORD fileSize;              /* EBP - 0x10c */
    CHAR rootPath[0x108];        /* EBP - 0x108 */
  } s;
  
  for (s.found = 0, s.tries = 0; *param_2 > s.tries && s.found == 0; s.tries++) {
    s.result = IsCDDrive(param_3);
    if (s.result != 0) {
      sprintf(s.rootPath, s_pctS_10010074, param_3);
      s.result = GetVolumeInformationA(s.rootPath, s.volumeName, 0x104, &s.volumeSerial,
                                      &s.maximumComponentLength, &s.fileSystemFlags,
                                      s.fileSystemName, 0x104);
      s.volumeName[3] = 0;
      if (strcmp(s.volumeName, s_MTG_10010078) == 0) {
        s.found = 1;
      }
      else {
        Sleep(1000);
      }
    }
    else {
      Sleep(1000);
    }
  }

  if (s.found == 0) {
    return 0;
  }
  
  sprintf(s.rootPath,s__s__s_10010090,param_3,s_DuelSounds_Deep_wav_1001007c);
  s.hFile = CreateFileA(s.rootPath,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x80,(HANDLE)0x0);
  if (s.hFile == (HANDLE)-1) {
    return 0;
  }

  CloseHandle(s.hFile);
  
  sprintf(s.rootPath,s__s__s_100100ac,param_3,s_Sound_Locmus15_wav_10010098);
  s.hFile = CreateFileA(s.rootPath,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x80,(HANDLE)0x0);
  if (s.hFile == (HANDLE)-1) {      
    return 0;
  } 

  s.fileSize = GetFileSize(s.hFile,(LPDWORD)0x0);
  if (s.fileSize != 0x307864) {
    CloseHandle(s.hFile);
    return 0;
  }

  CloseHandle(s.hFile);
  return 1;
}

// FUNCTION: CDTOOLS 0x10001468
undefined4 __cdecl CheckSotaCD(undefined4 param_1,int *param_2,undefined4 param_3)

{
  struct {
    HANDLE hFile;                /* EBP - 0x330 */
    CHAR volumeName[0x104];      /* EBP - 0x32c */
    CHAR fileSystemName[0x104];  /* EBP - 0x228 */
    int result;                  /* EBP - 0x124 */
    int tries;                   /* EBP - 0x120 */
    int found;                   /* EBP - 0x11c */
    DWORD volumeSerial;          /* EBP - 0x118 */
    DWORD maximumComponentLength;/* EBP - 0x114 */
    DWORD fileSystemFlags;       /* EBP - 0x110 */
    DWORD fileSize;              /* EBP - 0x10c */
    CHAR rootPath[0x108];        /* EBP - 0x108 */
  } s;
  
  for (s.found = 0, s.tries = 0; *param_2 > s.tries && s.found == 0; s.tries++) {
    s.result = IsCDDrive(param_3);
    if (s.result != 0) {
      sprintf(s.rootPath, s_pctS_100100b4, param_3);
      s.result = GetVolumeInformationA(s.rootPath, s.volumeName, 0x104, &s.volumeSerial,
                                      &s.maximumComponentLength, &s.fileSystemFlags,
                                      s.fileSystemName, 0x104);
      s.volumeName[3] = 0;
      if (strcmp(s.volumeName, s_MTG_100100b8) == 0) {
        s.found = 1;
      }
      else {
        Sleep(1000);
      }
    }
    else {
      Sleep(1000);
    }
  }

  if (s.found == 0) {
    return 0;
  }

  sprintf(s.rootPath,s__s__s_100100d4,param_3,s_DuelSounds_Manaball_wav_100100bc);
  s.hFile = CreateFileA(s.rootPath,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x80,(HANDLE)0x0);
  if (s.hFile == (HANDLE)-1) {
    return 0;
  }

  CloseHandle(s.hFile);
  sprintf(s.rootPath,s__s__s_100100f0,param_3,s_Sound_Locmus15_wav_100100dc);
  s.hFile = CreateFileA(s.rootPath,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x80,(HANDLE)0x0);
  if (s.hFile == (HANDLE)-1) {
    return 0;
  }

  s.fileSize = GetFileSize(s.hFile,(LPDWORD)0x0);
  if (s.fileSize != 0x307864) {
    CloseHandle(s.hFile);
    return 0;
  }

  CloseHandle(s.hFile);
  return 1;
}

// FUNCTION: CDTOOLS 0x1000169C
bool __cdecl IsCDDrive(undefined4 param_1)
{
  struct {
    DWORD sectorsPerCluster; /* EBP - 0x224 */
    FILE * testDirTmp; /* EBP - 0x220*/
    DWORD numberOfFreeClusters; /* EBP - 0x21c */
    DWORD bytesPerSector; /* EBP - 0x218 */
    DWORD totalNumberOfClusters; /* EBP - 0x214 */
    char idk [0x108]; /* EBP - 0x210*/
    char rootPathName [0x106]; /* EBP - 0x108*/
  } s;
  
  sprintf(s.rootPathName,&S_pctS_100100f8,param_1);

  //Rest of the code is unreachable?
  if (GetDriveTypeA(s.rootPathName) == 5)
    return 1;
  else
    return 0;

  if (GetDiskFreeSpaceA(s.rootPathName,&s.sectorsPerCluster,
                            &s.bytesPerSector,&s.numberOfFreeClusters,
                            &s.totalNumberOfClusters) == 0) {
    return 1;
  }

  if (s.sectorsPerCluster * s.bytesPerSector * s.numberOfFreeClusters != 0) 
    return 0;

  sprintf(s.idk,s_Testdir_tmp_100100fc, param_1);
  s.testDirTmp = fopen(s.idk,&DAT_1001010c);
  if (s.testDirTmp != 0) {
    fclose(s.testDirTmp);
    unlink(s.idk);
    return 0;
  }
  
  return 1;
}

// FUNCTION: CDTOOLS 0x100017C0
undefined4 Autoplay_ShutDown(void)
{
  struct {
    HKEY hKey;
    DWORD autoRunToSet;
    DWORD lpcbData;
    DWORD disableAutoRun;
    DWORD noDriveTypeAutoRun;
  } s;
  
  s.noDriveTypeAutoRun = 0x95;
  s.disableAutoRun = 0xff;
  s.autoRunToSet = s.disableAutoRun;

  s.lpcbData = 4;
  s.hKey = (HKEY)0x0;
  g_originalNoDriveTypeAutoRun = s.noDriveTypeAutoRun;

  if (RegOpenKeyExA((HKEY)0x80000001,s_Software_Microsoft_Windows_Curre_10010110,0,0xf003f,
                        &s.hKey) == 0) {

    if (RegQueryValueExA(s.hKey,s_NoDriveTypeAutoRun_1001014c,(LPDWORD)0x0,(LPDWORD)0x0,
                             &g_originalNoDriveTypeAutoRun,&s.lpcbData) == 0) {
      RegSetValueExA(s.hKey,s_NoDriveTypeAutoRun_10010160,0,3,&s.autoRunToSet,4);
    }
    else {
      g_originalNoDriveTypeAutoRun = s.noDriveTypeAutoRun;
    }
    RegFlushKey(s.hKey);
    RegCloseKey(s.hKey);
  }
  DAT_100123d4 = RegisterWindowMessageA(s_QueryCancelAutoPlay_10010174);
  return 1;
}

// FUNCTION: CDTOOLS 0x10001893
BOOL Autoplay_Restore(void)
{
  HKEY hKey;
  BOOL result;
  
  result = 0;
  if (RegOpenKeyExA((HKEY)0x80000001,s_Software_Microsoft_Windows_Curre_10010188,0,0xf003f,
                        &hKey) == 0) {
    result = 1;
    RegSetValueExA(hKey,s_NoDriveTypeAutoRun_100101c4,0,3,&g_originalNoDriveTypeAutoRun,4);
    RegFlushKey(hKey);
    RegCloseKey(hKey);
  }
  return result;
}

// FUNCTION: CDTOOLS 0x10001910
int WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID reserved)
{
  DAT_100123cc = hDllHandle;
  switch(nReason) {
    case 1:
      break;
    case 0:
      break;
    case 2:
      break;
    case 3:
    break;
  }
  return 1;
}
