#include "magsnd.h"
#include "defs.h"
#include "mystdbool.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <vfw.h>

typedef unsigned char byte;
typedef int (__stdcall code)();

HRESULT WINAPI DirectSoundCreate(const GUID *lpGuid, void **ppDS, void *pUnkOuter);

/*
 * Internal structures inferred from magsnd.dll usage.
 *
 * Notes:
 * - `SndInstance` objects are allocated as 0x204 bytes (see FUN_10005480).
 * - Many fields are still unknown; only offsets observed in code are named.
 * - Keep MSVC 4.20/C89 compatibility; avoid relying on `sizeof(SndInstance)` for layout.
 */

#pragma pack(push, 1)
typedef struct SndWaveFormatTagChannels {
  unsigned short wFormatTag;     /* +0x00 */
  unsigned short nChannels;      /* +0x02 */
  uint nSamplesPerSec;   /* +0x04 */
  uint nAvgBytesPerSec;  /* +0x08 */
  unsigned short nBlockAlign;    /* +0x0c */
  unsigned short wBitsPerSample; /* +0x0e */
  unsigned short cbSize;         /* +0x10 */
} SndWaveFormat;
#pragma pack(pop)

typedef struct SndDsBufferDesc {
  uint dwSize;        /* +0x00 */
  uint dwFlags;       /* +0x04 */
  uint dwBufferBytes; /* +0x08 */
  uint dwReserved;    /* +0x0c */
  void *lpwfxFormat;  /* +0x10 */
} SndDsBufferDesc;

typedef struct SndInstance SndInstance;
struct SndInstance {
  void *streamOrMmio;      /* 0x000: PAVISTREAM for AVI path, otherwise used as opaque */
  uint flags;              /* 0x004 */
  uint flags2;             /* 0x008 */
  uint lruCounter;         /* 0x00c */
  int slotIndex;           /* 0x010 */
  int loadId;              /* 0x014 */
  int activeMarkerIndex;   /* 0x018 */
  byte pad_01c[0x30 - 0x1c];
  uint markerCount;        /* 0x030 */
  void *markers[0x11];     /* 0x034 */
  SndWaveFormat waveFmt;   /* 0x078 (18 bytes) */
  unsigned short pad_08a;  /* 0x08a */
  int blockBytes;          /* 0x08c */
  int blockScale;          /* 0x090 */
  int blockCount;          /* 0x094 */
  byte pad_098[0xa4 - 0x98];
  void *dsLockedPtr;       /* 0x0a4: pointer from IDirectSoundBuffer::Lock */
  SndDsBufferDesc dsDesc;  /* 0x0a8 (DSBUFFERDESC, size 0x14) */
  void *dsBuffer;          /* 0x0bc: LPDIRECTSOUNDBUFFER */
  byte pad_0c0[0x180 - 0xc0];
  MMIOINFO mmioInfo;       /* 0x180 */
  HMMIO hMmio;             /* 0x1c8 */
  int dataBytes;           /* 0x1cc */
  uint writeCursorBytes;   /* 0x1d0 */
  uint readCursorBytes;    /* 0x1d4 */
  uint ringCursorBytes;    /* 0x1d8 */
  uint timeBytes;          /* 0x1dc */
  int mmioBaseOffset;      /* 0x1e0 */
  byte pad_1e4[0x1f8 - 0x1e4];
  SndInstance *prevActive; /* 0x1f8 */
  SndInstance *nextActive; /* 0x200 */
};

extern byte g_sndSlots;

static SndInstance **SndSlotPtr(int idx)
{
  return (SndInstance **)((byte *)&g_sndSlots + idx * 4);
}

undefined4 __cdecl InitSnd(int param_1, undefined4 param_2, byte param_3);
void __cdecl ReleaseSnd(void);
undefined4 __cdecl GetSndHWND(void);
int __cdecl LoadSnd(LPSTR param_1, int param_2, Sound *param_3);
undefined4 __cdecl UnloadSnd(int param_1);
undefined4 __cdecl UnloadAllSnds(void);
undefined4 __cdecl PlaySnd(int param_1, Sound *param_2);
int __cdecl PlaySndFile(LPSTR param_1, int param_2, Sound *param_3);
undefined4 __cdecl StopSnd(int param_1);
void __cdecl StopAllSnds(void);
undefined4 __cdecl PlayMidiFile(void);
undefined4 __cdecl SetPitch(int param_1, undefined4 param_2);
undefined4 __cdecl GetPitch(void);
undefined4 __cdecl SetVol(int param_1, uint param_2);
undefined4 __cdecl GetVol(void);
undefined4 __cdecl SetPan(int param_1, int param_2);
undefined4 __cdecl GetPan(void);
undefined4 __cdecl UpdateSnd(void);
int __cdecl SetSndMarker(int param_1, uint param_2);
int __cdecl PlaySndMarker(int param_1, uint param_2);
undefined4 __cdecl GetSndTime(int param_1, uint *param_2);
undefined4 __cdecl ResetSnd(void);
undefined4 __cdecl GetSndState(int param_1, undefined4 *param_2);
void * __cdecl GetAVISndBuff(int param_1, uint param_2);
undefined4 __cdecl ReleaseAVISndBuff(int param_1);
undefined4 __cdecl IsSndLoaded(int param_1, undefined4 *param_2);
undefined4 __cdecl GetLRUSnd(int *param_1, int param_2, int param_3);

void __cdecl FUN_10003748(int param_1);
void __cdecl FUN_10004239(int param_1);
void __cdecl FUN_10004292(int param_1);
void __cdecl FUN_10004311(int param_1);
void __cdecl FUN_1000436a(int param_1);
undefined4 __cdecl StartUpdateTimer(undefined4 resolutionMs);
void __cdecl FUN_1000448d(void);
void CALLBACK FUN_100043e9(UINT u1, UINT u2, DWORD dw1, DWORD dw2, DWORD dw3);
void __cdecl FUN_10004986(undefined4 *param_1);
undefined4 __cdecl LoadFromFile(char *filename, SndInstance **outSnd);
undefined4 __cdecl LoadWaveMmio(LPSTR filename, SndInstance **outSnd, undefined4 scratch);
undefined4 __cdecl FUN_10004e4c(undefined4 *param_1, int param_2);
undefined4 __cdecl PrimeAviAudio(SndInstance *sndObj, undefined4 unused);
undefined4 __cdecl LoadAviAudioStream(void *stream, SndInstance **outSnd, undefined4 scratch);
void __cdecl FUN_10005770(SndInstance *sndObj, undefined4 param_2);
SndInstance * __cdecl FUN_10005480(int *param_1, undefined4 param_2, undefined4 *param_3, undefined4 param_4);
undefined4 __cdecl FUN_10005562(SndInstance *param_1);
void * __cdecl operator_new(unsigned int size);
void __cdecl operator_delete(void *p);

typedef unsigned short ushort;

// GLOBAL: MAGSND 0x10007040
byte DAT_10007040[0x14];

// GLOBAL: MAGSND 0x10007058
byte DAT_10007058[0x12];

// GLOBAL: MAGSND 0x1000708C
char DAT_1000708c[] = "rb";

// GLOBAL: MAGSND 0x10007030
undefined4 _DAT_10007030 = 0x00000000;

// GLOBAL: MAGSND 0x100070A8
byte g_sndSlots;

// GLOBAL: MAGSND 0x1000707C
undefined4 _DAT_1000707c = 0x00000000;

// GLOBAL: MAGSND 0x10007088
undefined4 _DAT_10007088 = 0x00000000;

// GLOBAL: MAGSND 0x100074E8
byte DAT_100074e8;

// GLOBAL: MAGSND 0x100074EC
byte DAT_100074ec;

// GLOBAL: MAGSND 0x10008500
CRITICAL_SECTION g_sndCs;

// GLOBAL: MAGSND 0x10007010
undefined4 g_activeSndListHead = 0x00000000;

// GLOBAL: MAGSND 0x10007014
undefined4 DAT_10007014 = 0x00000000;

// GLOBAL: MAGSND 0x10007018
undefined4 DAT_10007018 = 0x00000000;

// GLOBAL: MAGSND 0x1000701C
undefined4 DAT_1000701c = 0x00000000;

// GLOBAL: MAGSND 0x10007020
undefined4 DAT_10007020 = 0x00000001;

// GLOBAL: MAGSND 0x10007024
undefined4 g_updateTimerActive = 0x00000000;

// GLOBAL: MAGSND 0x10007028
undefined4 DAT_10007028 = 0x00000000;

// GLOBAL: MAGSND 0x1000702C
undefined4 g_updateResolutionMs = 0x00000019;

// GLOBAL: MAGSND 0x10007034
undefined4 DAT_10007034 = 0x00000000;

// GLOBAL: MAGSND 0x10007038
undefined4 DAT_10007038 = 0x00000000;

// GLOBAL: MAGSND 0x1000706C
undefined4 DAT_1000706c = 0x00000000;

// GLOBAL: MAGSND 0x10007074
undefined4 DAT_10007074 = 0x00000000;

// GLOBAL: MAGSND 0x10007080
undefined4 g_updatePeriodMs = 0x00000021;

// GLOBAL: MAGSND 0x10007090
undefined4 DAT_10007090 = 0x00000000;

// GLOBAL: MAGSND 0x100070A0
int *DAT_100070a0 = (int *)0x0;

// GLOBAL: MAGSND 0x100084E8
undefined4 g_sndHwnd = 0x00000000;

// GLOBAL: MAGSND 0x100084F0
int *DAT_100084f0 = (int *)0x0;

// GLOBAL: MAGSND 0x100084F4
undefined4 g_updateTimerId = 0x00000000;

// GLOBAL: MAGSND 0x100084F8
undefined4 DAT_100084f8 = 0x00000000;

// GLOBAL: MAGSND 0x10008590
code *DAT_10008590 = (code *)0x0;

// FUNCTION: MAGSND 0x10001000
undefined4 __cdecl InitSnd(int param_1,undefined4 param_2,byte param_3)

{
  int local_8;
  undefined4 local_4;
  
                     /* 0x1000  1  InitSnd */
  if ((param_3 & 2) != 0) {
    if (DAT_10007034 == 0) {
      return 4;
    }
  }
  if ((param_3 & 2) != 0) {
    if (DAT_10007034 != 0) {
      return 0;
    }
  }
  if (DAT_10007034 == 0) {
    if (param_1 != 0) {
      local_8 = DirectSoundCreate(0,&DAT_100084f0,0);
      if (local_8 != 0) {
        return 4;
      }
      local_8 = (**(code **)(*DAT_100084f0 + 0x18))(DAT_100084f0,param_1,3);
      if (local_8 != 0) {
        ReleaseSnd();
        return 4;
      }
      local_8 = (**(code **)(*DAT_100084f0 + 0xc))(DAT_100084f0,&DAT_10007040,&DAT_100070a0,0);
      if (local_8 != 0) {
        ReleaseSnd();
        return 4;
      }
      local_8 = (**(code **)(*DAT_100070a0 + 0x38))(DAT_100070a0,&DAT_10007058);
      if (local_8 != 0) {
        (**(code **)(*DAT_100070a0 + 0x14))(DAT_100070a0,&DAT_10007058,0x12,&local_4);
      }
      g_sndHwnd = param_1;
      DAT_10007034 = DAT_10007034 + 1;
      InitializeCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    }
  }
  if ((param_3 & 1) != 0) {
    if (g_updateTimerActive != 0) {
      FUN_1000448d();
    }
    DAT_10007020 = 0;
  }
  return 0;
}

// FUNCTION: MAGSND 0x10001176
void ReleaseSnd(void)

{
                    /* 0x1176  2  ReleaseSnd */
  if (DAT_10007034 == 0)
    return;

  UnloadAllSnds();
  if (g_updateTimerActive != 0) {
    FUN_1000448d();
  }
  (**(code **)(*DAT_100084f0 + 8))(DAT_100084f0);
  DAT_100084f0 = (int *)0x0;
  g_sndHwnd = 0;
  DAT_10007034 = 0;
  DeleteCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  return;
}

// FUNCTION: MAGSND 0x100011E8
undefined4 GetSndHWND(void)

{
                    /* 0x11e8  25  GetSndHWND */
  return g_sndHwnd;
}

// FUNCTION: MAGSND 0x100011FD
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl LoadSnd(LPSTR param_1,int param_2,Sound *param_3)

{
  struct {
    int local_4;
  } s;

                    /* 0x11fd  3  LoadSnd */
  if ((param_3 != (Sound *)0) && ((*(uint *)((char *)param_3 + 0x1c) >> 4 & 1) != 0) &&
     (param_2 != 0)) {
    if (0x10f < param_2) {
      return 5;
    }
    if (param_2 < 0x100) {
      return 5;
    }
    if (*(int *)(&g_sndSlots + param_2 * 4) != 0) {
      return 0;
    }
  }
  else {
    if (0x100 < param_2) {
      return 5;
    }
    if (param_2 < 0) {
      return 5;
    }
    if (*(int *)(&g_sndSlots + param_2 * 4) != 0) {
      return 0;
    }
  }
  if ((param_3 != (Sound *)0) && ((*(uint *)((char *)param_3 + 0x1c) >> 2 & 1) != 0)) {
    if ((*(uint *)((char *)param_3 + 0x1c) >> 4 & 1) == 0) {
      s.local_4 = LoadWaveMmio(param_1,SndSlotPtr(param_2),0);
      if (s.local_4 != 0) {
        *(undefined4 *)(&g_sndSlots + param_2 * 4) = 0;
        return s.local_4;
      }
      FUN_10004e4c((undefined4 *)*SndSlotPtr(param_2),0);
    }
    else {
      s.local_4 = LoadAviAudioStream((void *)param_1,SndSlotPtr(param_2),0);
      if (s.local_4 != 0) {
        *(undefined4 *)(&g_sndSlots + param_2 * 4) = 0;
        return s.local_4;
      }
      if ((*(uint *)((char *)param_3 + 0x1c) >> 5 & 1) == 0) {
        PrimeAviAudio(*SndSlotPtr(param_2),0);
        *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) =
             *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) & 0xffffffbf;
      }
      else {
        *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) =
             *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) | 0x40;
      }
      _DAT_10007030 = _DAT_10007030 + 1;
    }
    *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 4) =
         *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 4) | 0x20;
    FUN_10004239(*(int *)(&g_sndSlots + param_2 * 4));
    *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) =
         *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) | 2;
  }
  else {
    s.local_4 = LoadFromFile(param_1,SndSlotPtr(param_2));
    if (s.local_4 != 0) {
      *(undefined4 *)(&g_sndSlots + param_2 * 4) = 0;
      return s.local_4;
    }
  }
  EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  FUN_10004311(*(int *)(&g_sndSlots + param_2 * 4));
  *(int *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x10) = param_2;
  if ((param_3 != (Sound *)0) && (*(int *)((char *)param_3 + 0x18) != 0)) {
    *(undefined4 *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x14) = *(undefined4 *)((char *)param_3 + 0x18);
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x100014BA
undefined4 __cdecl UnloadSnd(int param_1)

{
  undefined4 uVar1;
  uint local_8;
  
                    /* 0x14ba  4  UnloadSnd */
  if ((param_1 < 0x110) && (-1 < param_1)) {
    EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    if (*(int *)(&g_sndSlots + param_1 * 4) == 0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar1 = 1;
    }
    else {
      FUN_100026d1(param_1);
      FUN_1000436a(*(int *)(&g_sndSlots + param_1 * 4));
      if ((*(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 8) >> 1 & 1) != 0) {
        FUN_10004292(*(int *)(&g_sndSlots + param_1 * 4));
        if ((*(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 8) >> 5 & 1) == 0) {
          FUN_10004986(*(undefined4 **)(&g_sndSlots + param_1 * 4));
          for (local_8 = 0; local_8 < *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x30);
              local_8 = local_8 + 1) {
            if (*(int *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x38 + local_8 * 4) != 0) {
              FUN_10004986(*(undefined4 **)
                            (*(int *)(&g_sndSlots + param_1 * 4) + 0x38 + local_8 * 4));
            }
          }
        }
      }
      FUN_10005562(*SndSlotPtr(param_1));
      *(undefined4 *)(&g_sndSlots + param_1 * 4) = 0;
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar1 = 0;
    }
  }
  else {
    uVar1 = 5;
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x1000161D
undefined4 UnloadAllSnds(void)

{
                    /* 0x161d  5  UnloadAllSnds */
  EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  while (g_activeSndListHead != 0) {
    UnloadSnd(*(int *)(g_activeSndListHead + 0x10));
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10001668
undefined4 __cdecl PlaySnd(int param_1,Sound *param_2)

{
  undefined4 local_4;
  
                     /* 0x1668  6  PlaySnd */
  if (0x10f < param_1 || param_1 < 0) {
    return 5;
  }
  EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  if (*(int *)(&g_sndSlots + param_1 * 4) == 0) {
    LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    return 1;
  }
  local_4 = FUN_10001701(*(undefined4 **)(&g_sndSlots + param_1 * 4),(int *)param_2);
  *(int *)(*(int *)(&g_sndSlots + param_1 * 4) + 0xc) =
       *(int *)(*(int *)(&g_sndSlots + param_1 * 4) + 0xc) + 1;
  LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10001701
int __cdecl FUN_10001701(undefined4 *param_1,int *param_2)

{
  int iVar1;
  int local_1c;
  int *local_18;
  uint local_14;
  int local_10;
  int local_c;
  int local_8;
  
  local_14 = 0;
  local_c = 0;
  if (((uint)param_1[2] >> 1 & 1) == 0) {
    if ((param_2 == (int *)0x0) || (((uint)param_2[7] >> 1 & 1) == 0)) {
      iVar1 = FUN_100040c9((int)param_1,(int *)&local_18);
      if (iVar1 != 0) {
        return iVar1;
      }
      local_8 = 0;
    }
    else {
      local_18 = (int *)param_1[0x2f];
    }
  }
  else {
    if ((*(byte *)(param_1 + 1) & 1) != 0) {
      FUN_100026d1(param_1[4]);
    }
    if (((DAT_10007020 == 1) && (g_updateTimerActive == 0)) &&
       (local_8 = StartUpdateTimer(g_updateResolutionMs), local_8 != 0)) {
      UnloadSnd(param_1[4]);
      return local_8;
    }
    if (DAT_10007020 != 0) {
      DAT_10007028 = DAT_10007028 + 1;
    }
    local_14 = local_14 | 1;
    local_18 = (int *)param_1[0x2f];
    if ((((uint)param_1[1] >> 5 & 1) == 0) && (local_8 = FUN_10003656(param_1), local_8 != 0)) {
      UnloadSnd(param_1[4]);
      return local_8;
    }
  }
  if (param_2 == (int *)0x0) {
    local_1c = 0;
    param_1[0x7c] = 400;
    local_c = param_1[0x1f];
    param_1[0x7b] = local_c;
    local_10 = 0;
    param_1[0x7a] = 0;
  }
  else {
    local_1c = *param_2;
    if (400 < local_1c) {
      local_1c = 400;
    }
    param_1[0x7c] = local_1c;
    local_1c = (local_1c * 5 + -2000) * 2;
    if (param_2[1] == 0) {
      local_c = param_1[0x1f];
    }
    else {
      local_c = param_2[1];
    }
    param_1[0x7b] = local_c;
    if (param_2[2] == 0) {
      local_10 = 0;
    }
    else {
      local_10 = param_2[2];
    }
    param_1[0x7a] = local_10;
    local_10 = local_10 * 10;
    if ((*(byte *)(param_2 + 7) & 1) != 0) {
      local_14 = local_14 | 1;
      param_1[2] = param_1[2] | 1;
    }
    if (((uint)param_2[7] >> 3 & 1) != 0) {
      param_1[2] = param_1[2] | 4;
    }
  }
  (**(code **)(*local_18 + 0x3c))(local_18,local_1c);
  (**(code **)(*local_18 + 0x44))(local_18,local_c);
  (**(code **)(*local_18 + 0x40))(local_18,local_10);
  (**(code **)(*local_18 + 0x34))(local_18,0);
  iVar1 = (**(code **)(*local_18 + 0x30))(local_18,0,0,local_14);
  if (iVar1 == 0) {
    param_1[1] = param_1[1] | 1;
    param_1[1] = param_1[1] & 0xffffffdf;
    iVar1 = 0;
  }
  else {
    iVar1 = 9;
  }
  return iVar1;
}

// FUNCTION: MAGSND 0x10001A1F
int __cdecl PlaySndFile(LPSTR param_1,int param_2,Sound *param_3)

{
  struct {
    undefined1 local_1c[4];
    int local_18;
    int local_14;
    int local_10;
    int local_c;
    undefined4 local_8;
  } s;

                    /* 0x1a1f  7  PlaySndFile */
  s.local_8 = 0;
  s.local_18 = 0;
  s.local_10 = 0;
  if (0xff < param_2) {
    return 5;
  }
  if (param_2 < 0) {
    return 5;
  }
  EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  if (*(int *)(&g_sndSlots + param_2 * 4) != 0) {
    LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    return 2;
  }
  s.local_c = LoadWaveMmio(param_1,SndSlotPtr(param_2),s.local_8);
  if (s.local_c != 0) {
    LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    return s.local_c;
  }
  FUN_10004311(*(int *)(&g_sndSlots + param_2 * 4));
  FUN_10004239(*(int *)(&g_sndSlots + param_2 * 4));
  if ((DAT_10007020 == 1) && (g_updateTimerActive == 0)) {
    s.local_c = StartUpdateTimer(g_updateResolutionMs);
    if (s.local_c != 0) {
      UnloadSnd(param_2);
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      return s.local_c;
    }
  }
  *(int *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x10) = param_2;
  if (param_3 != (Sound *)0) {
    s.local_18 = *(int *)param_3;
    *(int *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x1f0) = s.local_18;
    if (400 < s.local_18) {
      s.local_18 = 400;
    }
    s.local_18 = (s.local_18 * 5 + -2000) * 2;
    if (((int *)param_3)[1] == 0) {
      s.local_10 = *(int *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x7c);
    }
    else {
      s.local_10 = ((int *)param_3)[1];
    }
    *(int *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x1ec) = s.local_10;
    if (((int *)param_3)[2] == 0) {
      s.local_14 = 0;
    }
    else {
      s.local_14 = ((int *)param_3)[2];
    }
    *(int *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x1e8) = s.local_14;
    s.local_14 = s.local_14 * 10;
    if ((*(byte *)((int *)param_3 + 7) & 1) != 0) {
      *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) =
           *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) | 1;
    }
    if (((uint)((int *)param_3)[7] >> 3 & 1) != 0) {
      *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) =
           *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) | 4;
    }
  }
  else {
    s.local_18 = 0;
    *(undefined4 *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x1f0) = 400;
    s.local_10 = *(int *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x7c);
    *(int *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x1ec) = s.local_10;
    s.local_14 = 0;
    *(undefined4 *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x1e8) = 0;
  }
  *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) =
       *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 8) | 2;
  (**(code **)(**(int **)(*(int *)(&g_sndSlots + param_2 * 4) + 0xbc) + 0x3c))
            (*(undefined4 *)(*(int *)(&g_sndSlots + param_2 * 4) + 0xbc),s.local_18);
  *(int *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x1f0) = s.local_18;
  (**(code **)(**(int **)(*(int *)(&g_sndSlots + param_2 * 4) + 0xbc) + 0x44))
            (*(undefined4 *)(*(int *)(&g_sndSlots + param_2 * 4) + 0xbc),s.local_10);
  *(int *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x1ec) = s.local_10;
  (**(code **)(**(int **)(*(int *)(&g_sndSlots + param_2 * 4) + 0xbc) + 0x40))
            (*(undefined4 *)(*(int *)(&g_sndSlots + param_2 * 4) + 0xbc),s.local_14);
  *(int *)(*(int *)(&g_sndSlots + param_2 * 4) + 0x1e8) = s.local_14;
  FUN_10004e4c((undefined4 *)*SndSlotPtr(param_2),0);
  (**(code **)(**(int **)(*(int *)(&g_sndSlots + param_2 * 4) + 0xbc) + 0x30))
            (*(undefined4 *)(*(int *)(&g_sndSlots + param_2 * 4) + 0xbc),0,0,1);
  (**(code **)(**(int **)(*(int *)(&g_sndSlots + param_2 * 4) + 0xbc) + 0x10))
            (*(undefined4 *)(*(int *)(&g_sndSlots + param_2 * 4) + 0xbc),
             *(int *)(&g_sndSlots + param_2 * 4) + 0x1d8,s.local_1c);
  *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 4) =
       *(uint *)(*(int *)(&g_sndSlots + param_2 * 4) + 4) | 1;
  LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10001E52
int __cdecl FUN_10001e52(undefined4 *param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  
  iVar1 = param_1[0xd];
  piVar2 = param_1 + param_2 + 0xd;
  iVar3 = LoadWaveMmio((LPSTR)*param_1,(SndInstance **)piVar2,0x10);
  if ((iVar3 == 0) &&
     (iVar3 = FUN_10004e4c((undefined4 *)*piVar2,*(int *)(iVar1 + (param_2 * 3 + -3) * 8 + 0x14)),
     iVar3 == 0)) {
    *(uint *)(*piVar2 + 4) = *(uint *)(*piVar2 + 4) | 0x20;
    FUN_10004239(*piVar2);
    *(uint *)(*piVar2 + 8) = *(uint *)(*piVar2 + 8) | 2;
    *(uint *)(*piVar2 + 8) = *(uint *)(*piVar2 + 8) | 0x10;
    *(uint *)(*piVar2 + 8) = param_1[2] & 1 | *(uint *)(*piVar2 + 8) & 0xfffffffe;
    *(undefined4 *)(*piVar2 + 0x10) = param_1[4];
  }
  return iVar3;
}

// FUNCTION: MAGSND 0x10001F6F
int __cdecl FUN_10001f6f(int param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  
  if (((*(uint *)(param_1 + 4) >> 6 & 1) == 0) || (*(int *)(param_1 + 0x18) != param_2 + -1)) {
    puVar1 = *(undefined4 **)(param_1 + 0x34 + param_2 * 4);
    iVar2 = FUN_10004e4c(puVar1,*(int *)(*(int *)(param_1 + 0x34) + (param_2 * 3 + -3) * 8 + 0x14));
    if (iVar2 == 0) {
      puVar1[1] = puVar1[1] | 0x20;
      puVar1[1] = puVar1[1] & 0xfffffffb;
      puVar1[1] = puVar1[1] & 0xffffffef;
      puVar1[0x76] = 0;
      puVar1[2] = *(uint *)(param_1 + 8) & 1 | puVar1[2] & 0xfffffffe;
      puVar1[0x7c] = *(undefined4 *)(param_1 + 0x1f0);
      puVar1[0x7b] = *(undefined4 *)(param_1 + 0x1ec);
      puVar1[0x7a] = *(undefined4 *)(param_1 + 0x1e8);
      iVar2 = 0;
    }
  }
  else {
    iVar2 = 0xd;
  }
  return iVar2;
}

// FUNCTION: MAGSND 0x1000208C
void __cdecl FUN_1000208c(int *param_1,int *param_2)

{
  int iVar1;
  undefined4 local_c;
  
  iVar1 = *param_1;
  *param_1 = *param_2;
  for (local_c = 0; local_c < 0x10; local_c = local_c + 1) {
    if (*(int *)(iVar1 + 0x38 + local_c * 4) == *param_1) {
      *(int *)(*param_1 + 0x38 + local_c * 4) = iVar1;
    }
    else {
      *(undefined4 *)(*param_1 + 0x38 + local_c * 4) = *(undefined4 *)(iVar1 + 0x38 + local_c * 4);
    }
    *(undefined4 *)(iVar1 + 0x38 + local_c * 4) = 0;
  }
  *(undefined4 *)(*param_1 + 0x10) = *(undefined4 *)(iVar1 + 0x10);
  *(uint *)(iVar1 + 8) = *(uint *)(iVar1 + 8) | 0x10;
  *(uint *)(*param_1 + 8) = *(uint *)(*param_1 + 8) & 0xffffffef;
  FUN_1000436a(iVar1);
  FUN_10004311(*param_1);
  return;
}

// FUNCTION: MAGSND 0x1000215C
int __cdecl SetSndMarker(int param_1,uint param_2)

{
  undefined4 *puVar1;
  int local_8;
  
                    /* 0x215c  18  SetSndMarker */
  if ((param_1 < 0x100) && (-1 < param_1)) {
    EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    puVar1 = *(undefined4 **)(&g_sndSlots + param_1 * 4);
    if (puVar1 == (undefined4 *)0x0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      local_8 = 1;
    }
    else if ((puVar1[0xc] == 0) || ((uint)puVar1[0xc] < param_2)) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      local_8 = 5;
    }
    else {
      if (puVar1[param_2 + 0xd] == 0) {
        local_8 = FUN_10001e52(puVar1,param_2);
      }
      else {
        local_8 = FUN_10001f6f((int)puVar1,param_2);
      }
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    }
  }
  else {
    local_8 = 5;
  }
  return local_8;
}

// FUNCTION: MAGSND 0x10002249
int __cdecl PlaySndMarker(int param_1,uint param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int local_24;
  undefined4 local_20;
  undefined4 local_1c;
  uint local_8;
  
                    /* 0x2249  19  PlaySndMarker */
  if ((param_1 < 0x100) && (-1 < param_1)) {
    if (((int)param_2 < 0x11) && (-1 < (int)param_2)) {
      EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      iVar2 = *(int *)(&g_sndSlots + param_1 * 4);
      if (iVar2 == 0) {
        LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
        iVar2 = 1;
      }
      else if ((*(int *)(iVar2 + 0x30) == 0) || (*(uint *)(iVar2 + 0x30) < param_2)) {
        LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
        iVar2 = 5;
      }
      else {
        puVar1 = *(undefined4 **)(iVar2 + 0x34 + param_2 * 4);
        if (puVar1 == (undefined4 *)0x0) {
          LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
          iVar2 = 1;
        }
        else if (((uint)puVar1[1] >> 5 & 1) == 0) {
          LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
          iVar2 = 1;
        }
        else {
          if ((*(byte *)(iVar2 + 4) & 1) != 0) {
            FUN_100026d1(param_1);
          }
          puVar1[0x7c] = *(undefined4 *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x1f0);
          local_24 = puVar1[0x7c];
          puVar1[0x7b] = *(undefined4 *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x1ec);
          local_20 = puVar1[0x7b];
          puVar1[0x7a] = *(undefined4 *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x1e8);
          local_1c = puVar1[0x7a];
          local_8 = puVar1[2] & 1 | local_8 & 0xfffffffe;
          iVar2 = FUN_10001701(puVar1,&local_24);
          if (iVar2 == 0) {
            *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) =
                 *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) | 0x40;
            *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x18) = param_2 - 1;
            *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) =
                 *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) | 1;
          }
          LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
        }
      }
    }
    else {
      iVar2 = 5;
    }
  }
  else {
    iVar2 = 5;
  }
  return iVar2;
}

// FUNCTION: MAGSND 0x1000246E
undefined4 __cdecl StopSnd(int param_1)

{
  struct {
    int local_14;
    int *local_10;
    int local_c;
    int local_8;
    int local_4;
  } s;

                     /* 0x246e  8  StopSnd */
  if (0x10f < param_1) {
    return 5;
  }
  if (param_1 < 0) {
    return 5;
  }
  EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  if (*(int *)(&g_sndSlots + param_1 * 4) == 0) {
    LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    return 1;
  }
  if ((*(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) >> 6 & 1) != 0) {
    s.local_4 = *(int *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x18);
    s.local_8 = *(int *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x38 + s.local_4 * 4);
    if (s.local_8 == 0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      return 1;
    }
    *(uint *)(s.local_8 + 4) = *(uint *)(s.local_8 + 4) | 2;
  }
  else {
    if ((*(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 8) >> 1 & 1) != 0) {
      *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) =
           *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) | 2;
    }
    else {
      s.local_14 = (**(code **)(**(int **)(*(int *)(&g_sndSlots + param_1 * 4) + 0xbc) + 0x48))
                        (*(undefined4 *)(*(int *)(&g_sndSlots + param_1 * 4) + 0xbc));
      if (s.local_14 != 0) {
        LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
        return 9;
      }
    }
  }
  if ((*(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 8) >> 1 & 1) == 0) {
    for (s.local_c = 0; s.local_c < 0x10; s.local_c = s.local_c + 1) {
      s.local_10 = (int *)(*(int *)(&g_sndSlots + param_1 * 4) + s.local_c * 0xc + 0xc0);
      if (*s.local_10 == 0) {
        break;
      }
      s.local_14 = (**(code **)(*(int *)*s.local_10 + 0x48))(*s.local_10);
      if (s.local_14 != 0) {
        LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
        return 9;
      }
    }
    *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) =
         *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) & 0xfffffffe;
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10002676
void StopAllSnds(void)
{
  int local_8;
  
                     /* 0x2676  9  StopAllSnds */
  EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  local_8 = g_activeSndListHead;
  while (local_8 != 0) {
    StopSnd(*(int *)(local_8 + 0x10));
    local_8 = *(int *)(local_8 + 0x200);
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  return;
}

// FUNCTION: MAGSND 0x100026D1
undefined4 __cdecl FUN_100026d1(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int local_10;
  
  if ((param_1 < 0x110) && (-1 < param_1)) {
    if (*(int *)(&g_sndSlots + param_1 * 4) == 0) {
      uVar1 = 1;
    }
    else {
      if ((*(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) >> 6 & 1) == 0) {
        iVar4 = (**(code **)(**(int **)(*(int *)(&g_sndSlots + param_1 * 4) + 0xbc) + 0x48))
                          (*(undefined4 *)(*(int *)(&g_sndSlots + param_1 * 4) + 0xbc));
        if (iVar4 != 0) {
          return 9;
        }
      }
      else {
        iVar4 = *(int *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x38 +
                        *(int *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x18) * 4);
        if (iVar4 == 0) {
          return 1;
        }
        iVar2 = (**(code **)(**(int **)(iVar4 + 0xbc) + 0x48))(*(undefined4 *)(iVar4 + 0xbc));
        if (iVar2 != 0) {
          return 9;
        }
        *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) =
             *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) & 0xffffffbf;
        *(uint *)(iVar4 + 4) = *(uint *)(iVar4 + 4) & 0xfffffffe;
        *(uint *)(iVar4 + 4) = *(uint *)(iVar4 + 4) & 0xfffffffd;
        *(uint *)(iVar4 + 4) = *(uint *)(iVar4 + 4) | 4;
        *(uint *)(iVar4 + 4) = *(uint *)(iVar4 + 4) & 0xffffffdf;
      }
      if ((*(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 8) >> 1 & 1) == 0) {
        local_10 = 0;
        while ((local_10 < 0x10 &&
               (piVar3 = (int *)(*(int *)(&g_sndSlots + param_1 * 4) + local_10 * 0xc + 0xc0),
               *piVar3 != 0))) {
          iVar4 = (**(code **)(*(int *)*piVar3 + 0x48))(*piVar3);
          if (iVar4 != 0) {
            return 9;
          }
          local_10 = local_10 + 1;
        }
      }
      else if ((0 < DAT_10007028) && (DAT_10007028 = DAT_10007028 + -1, DAT_10007028 == 0)) {
        FUN_1000448d();
      }
      *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) =
           *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) & 0xfffffffe;
      *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) =
           *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) & 0xfffffffd;
      *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) =
           *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) | 4;
      *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) =
           *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) & 0xffffffdf;
      uVar1 = 0;
    }
  }
  else {
    uVar1 = 5;
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x1000290C
undefined4 PlayMidiFile(void)

{
                    /* 0x290c  10  PlayMidiFile */
  return 0;
}

// FUNCTION: MAGSND 0x1000291E
undefined4 __cdecl SetPitch(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int *piVar2;
  int local_8;
  
                    /* 0x291e  11  SetPitch */
  if ((param_1 < 0x110) && (-1 < param_1)) {
    EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    if (*(int *)(&g_sndSlots + param_1 * 4) == 0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar1 = 1;
    }
    else {
      (**(code **)(**(int **)(*(int *)(&g_sndSlots + param_1 * 4) + 0xbc) + 0x44))
                (*(undefined4 *)(*(int *)(&g_sndSlots + param_1 * 4) + 0xbc),param_2);
      local_8 = 0;
      while ((local_8 < 0x10 &&
             (piVar2 = (int *)(*(int *)(&g_sndSlots + param_1 * 4) + local_8 * 0xc + 0xc0),
             *piVar2 != 0))) {
        (**(code **)(*(int *)*piVar2 + 0x44))(*piVar2,param_2);
        local_8 = local_8 + 1;
      }
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar1 = 0;
    }
  }
  else {
    uVar1 = 5;
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x10002A24
undefined4 GetPitch(void)

{
                    /* 0x2a24  12  GetPitch */
  return 0;
}

// FUNCTION: MAGSND 0x10002A36
/* WARNING: Removing unreachable block (ram,0x10002ab6) */

undefined4 __cdecl SetVol(int param_1,uint param_2)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  int local_10;
  
                    /* 0x2a36  13  SetVol */
  if ((param_1 < 0x110) && (-1 < param_1)) {
    EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    if (*(int *)(&g_sndSlots + param_1 * 4) == 0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar2 = 1;
    }
    else {
      if (400 < param_2) {
        param_2 = 400;
      }
      if ((*(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) >> 6 & 1) == 0) {
        *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x1f0) = param_2;
        *(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x1f0) = param_2;
        param_2 = (param_2 * 5 + -2000) * 2;
        (**(code **)(**(int **)(*(int *)(&g_sndSlots + param_1 * 4) + 0xbc) + 0x3c))
                  (*(undefined4 *)(*(int *)(&g_sndSlots + param_1 * 4) + 0xbc),param_2);
      }
      else {
        iVar1 = *(int *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x38 +
                        *(int *)(*(int *)(&g_sndSlots + param_1 * 4) + 0x18) * 4);
        if (iVar1 == 0) {
          LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
          return 1;
        }
        *(uint *)(iVar1 + 0x1f0) = param_2;
        param_2 = (param_2 * 5 + -2000) * 2;
        (**(code **)(**(int **)(iVar1 + 0xbc) + 0x3c))(*(undefined4 *)(iVar1 + 0xbc),param_2);
      }
      local_10 = 0;
      while ((local_10 < 0x10 &&
             (piVar3 = (int *)(*(int *)(&g_sndSlots + param_1 * 4) + local_10 * 0xc + 0xc0),
             *piVar3 != 0))) {
        (**(code **)(*(int *)*piVar3 + 0x3c))(*piVar3,param_2);
        local_10 = local_10 + 1;
      }
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar2 = 0;
    }
  }
  else {
    uVar2 = 5;
  }
  return uVar2;
}

// FUNCTION: MAGSND 0x10002C32
undefined4 GetVol(void)

{
                    /* 0x2c32  14  GetVol */
  return 0;
}

// FUNCTION: MAGSND 0x10002C44
undefined4 __cdecl SetPan(int param_1,int param_2)

{
  undefined4 uVar1;
  int *piVar2;
  int local_8;
  
                    /* 0x2c44  15  SetPan */
  if ((param_1 < 0x110) && (-1 < param_1)) {
    EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    if (*(int *)(&g_sndSlots + param_1 * 4) == 0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar1 = 1;
    }
    else {
      (**(code **)(**(int **)(*(int *)(&g_sndSlots + param_1 * 4) + 0xbc) + 0x40))
                (*(undefined4 *)(*(int *)(&g_sndSlots + param_1 * 4) + 0xbc),param_2 * 10);
      local_8 = 0;
      while ((local_8 < 0x10 &&
             (piVar2 = (int *)(*(int *)(&g_sndSlots + param_1 * 4) + local_8 * 0xc + 0xc0),
             *piVar2 != 0))) {
        (**(code **)(*(int *)*piVar2 + 0x40))(*piVar2,param_2 * 10);
        local_8 = local_8 + 1;
      }
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar1 = 0;
    }
  }
  else {
    uVar1 = 5;
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x10002D55
undefined4 GetPan(void)

{
                    /* 0x2d55  16  GetPan */
  return 0;
}

// FUNCTION: MAGSND 0x10002D67
undefined4 UpdateSnd(void)

{
  struct {
    undefined4 *node;
    undefined4 *next;
  } s;
  
                     /* 0x2d67  17  UpdateSnd */
  if (DAT_1000706c != 0) {
    return 0xd;
  }

  EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  s.node = DAT_10007018;
  while (s.node != (undefined4 *)0x0) {
    if ((*(byte *)(s.node + 1) & 1) != 0) {
      if (((uint)s.node[1] >> 1 & 1) != 0) {
        if ((uint)s.node[0x7c] > 5) {
          s.node[0x7c] = s.node[0x7c] - 5;
        }
        else {
          s.node[0x7c] = 0;
        }
        SetVol(*(int *)((char *)s.node + 0x10), *(int *)((char *)s.node + 0x1f0));
      }
  
      if (((uint)s.node[2] >> 5 & 1) != 0) {
        if (((uint)s.node[2] >> 6 & 1) == 0) {
          FUN_10003d60(s.node);
        }
      }
      else {
        FUN_10003748((int)s.node);
      }
    }
  
    if (((uint)s.node[1] >> 2 & 1) != 0) {
      if (((uint)s.node[2] >> 2 & 1) != 0) {
        s.next = *(undefined4 **)((char *)s.node + 0x200);
        UnloadSnd(*(int *)((char *)s.node + 0x10));
        s.node = s.next;
        continue;
      }
    }
    s.node = *(undefined4 **)((char *)s.node + 0x1f8);
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10002EB5
undefined4 __cdecl GetSndTime(int param_1,uint *param_2)

{
  undefined4 uVar1;
  uint uVar2;
  uint local_20;
  int local_1c;
  uint local_18;
  double local_14;
  uint local_c;
  undefined1 local_8 [4];
  
                    /* 0x2eb5  20  GetSndTime */
  if ((param_1 < 0x110) && (-1 < param_1)) {
    EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    if (*(int *)(&g_sndSlots + param_1 * 4) == 0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar1 = 1;
    }
    else {
      local_1c = *(int *)(&g_sndSlots + param_1 * 4);
      (**(code **)(**(int **)(local_1c + 0xbc) + 0x10))
                (*(undefined4 *)(local_1c + 0xbc),&local_20,local_8);
      local_18 = *(uint *)(local_1c + 0x1dc) % *(uint *)(local_1c + 0xb0);
      if (local_18 < local_20) {
        *(int *)(local_1c + 0x1dc) = *(int *)(local_1c + 0x1dc) + (local_20 - local_18);
      }
      else {
        *(int *)(local_1c + 0x1dc) =
             *(int *)(local_1c + 0x1dc) + (*(int *)(local_1c + 0xb0) - local_18);
        *(int *)(local_1c + 0x1dc) = *(int *)(local_1c + 0x1dc) + local_20;
      }
      if ((*(uint *)(local_1c + 8) >> 5 & 1) != 0) {
        local_c = *(uint *)(local_1c + 0x1dc) / *(uint *)(local_1c + 0x8c);
        while (*(uint *)(local_1c + 0xa0) < local_c) {
          PostMessageA(g_sndHwnd,0x3bd,0,local_c);
          *(int *)(local_1c + 0xa0) = *(int *)(local_1c + 0xa0) + 1;
        }
      }
      local_14 = (double)*(uint *)(local_1c + 0x1dc);
      if (*(int *)(local_1c + 0x80) == 0x15888) {
        uVar2 = ftol();
        *param_2 = uVar2;
      }
      else if (*(int *)(local_1c + 0x80) == 0xac44) {
        uVar2 = ftol();
        *param_2 = uVar2;
      }
      else if (*(int *)(local_1c + 0x80) == 0x5622) {
        uVar2 = ftol();
        *param_2 = uVar2;
      }
      else {
        if (*(int *)(local_1c + 0x80) != 0x2b11) {
          LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
          return 8;
        }
        uVar2 = ftol();
        *param_2 = uVar2;
      }
      *(uint *)(&DAT_100074e8 + DAT_10007074 * 0x10) = DAT_10007038;
      *(uint *)(&DAT_100074ec + DAT_10007074 * 0x10) = *param_2;
      DAT_10007074 = DAT_10007074 + 1;
      DAT_10007074 = DAT_10007074 & 0xff;
      if (*param_2 < DAT_10007038) {
        LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
        uVar1 = 5;
      }
      else {
        DAT_10007038 = *param_2;
        LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
        uVar1 = 0;
      }
    }
  }
  else {
    uVar1 = 5;
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x1000314D
undefined4 __cdecl GetSndState(int param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  
                    /* 0x314d  22  GetSndState */
  if ((param_1 < 0x110) && (-1 < param_1)) {
    EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    if (*(int *)(&g_sndSlots + param_1 * 4) == 0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar1 = 1;
    }
    else {
      if ((*(byte *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) & 1) == 0) {
        if ((*(uint *)(*(int *)(&g_sndSlots + param_1 * 4) + 4) >> 3 & 1) == 0) {
          *param_2 = 0;
        }
        else {
          *param_2 = 2;
        }
      }
      else {
        *param_2 = 1;
      }
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar1 = 0;
    }
  }
  else {
    uVar1 = 5;
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x1000320D
undefined4 ResetSnd(void)

{
                    /* 0x320d  21  ResetSnd */
  return 0;
}

// FUNCTION: MAGSND 0x1000321F
undefined4 __cdecl IsSndLoaded(int param_1,undefined4 *param_2)

{
  int local_8;
  
                     /* 0x321f  26  IsSndLoaded */
  if (0 < param_1) {
    EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    for (local_8 = g_activeSndListHead; local_8 != 0; local_8 = *(int *)(local_8 + 0x200)) {
      if (*(int *)(local_8 + 0x14) == param_1) {
        *param_2 = *(undefined4 *)(local_8 + 0x10);
        LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
        return 1;
      }
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  }
  return 0;
}

// FUNCTION: MAGSND 0x100032AD
undefined4 __cdecl GetLRUSnd(int *param_1,int param_2,int param_3)

{
  int local_14;
  int local_10;
  uint local_c;
  undefined4 local_8;
  
                    /* 0x32ad  27  GetLRUSnd */
  local_8 = 0;
  local_c = 0xffffffff;
  if (((param_3 == 0) || (param_3 <= param_2)) || (0xff < param_3)) {
    local_10 = 0;
    param_3 = 0xff;
  }
  else {
    local_10 = param_2;
  }
  EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  while (local_10 < param_3) {
    if (*(int *)(&g_sndSlots + local_10 * 4) == 0) {
      local_14 = local_10;
      local_8 = 1;
      break;
    }
    if (*(uint *)(*(int *)(&g_sndSlots + local_10 * 4) + 0xc) < local_c) {
      local_c = *(uint *)(*(int *)(&g_sndSlots + local_10 * 4) + 0xc);
      local_14 = *(int *)(*(int *)(&g_sndSlots + local_10 * 4) + 0x10);
    }
    local_10 = local_10 + 1;
  }
  *param_1 = local_14;
  LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  return local_8;
}

// FUNCTION: MAGSND 0x100033A2
void * __cdecl GetAVISndBuff(int param_1,uint param_2)

{
  SndInstance *sndObj;
  void *local_28;
  int local_24;
  undefined4 local_20;
  uint local_1c;
  int local_18;
  int local_14;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  
                    /* 0x33a2  23  GetAVISndBuff */
  local_28 = 0;
  local_14 = 0;
  local_c = 0;
  local_10 = 0;
  local_20 = 0;
  if ((param_1 < 0x110) && (0xff < param_1)) {
    EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    local_24 = *(int *)(&g_sndSlots + param_1 * 4);
    if (local_24 == 0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      local_28 = 0;
    }
    else if ((*(uint *)(local_24 + 4) >> 7 & 1) == 0) {
      sndObj = (SndInstance *)local_24;
      local_1c = param_2 % *(uint *)(local_24 + 0x94);
      local_8 = *(int *)(local_24 + 0x8c) * local_1c;
      local_18 = (**(code **)(**(int **)(local_24 + 0xbc) + 0x2c))
                           (*(undefined4 *)(local_24 + 0xbc),local_8,
                            *(undefined4 *)(local_24 + 0x8c),&local_28,&local_c,&local_14,&local_10,
                            0);
      if (local_18 == 0) {
        if (local_14 == 0) {
          sndObj->dsLockedPtr = (void *)local_28;
          *(uint *)(local_24 + 4) = *(uint *)(local_24 + 4) | 0x80;
          LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
        }
        else {
          (**(code **)(**(int **)(local_24 + 0xbc) + 0x4c))
                    (*(undefined4 *)(local_24 + 0xbc),local_28,local_c,local_14,local_10);
          LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
          local_28 = 0;
        }
      }
      else {
        local_28 = 0;
      }
    }
    else {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      local_28 = 0;
    }
  }
  else {
    local_28 = 0;
  }
  return local_28;
}

// FUNCTION: MAGSND 0x1000352C
undefined4 __cdecl ReleaseAVISndBuff(int param_1)

{
  SndInstance *sndObj;
  int iVar1;
  undefined4 uVar2;
  
                    /* 0x352c  24  ReleaseAVISndBuff */
  if ((param_1 < 0x110) && (0xff < param_1)) {
    EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    iVar1 = *(int *)(&g_sndSlots + param_1 * 4);
    if (iVar1 == 0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar2 = 0;
    }
    else if ((*(uint *)(iVar1 + 4) >> 7 & 1) == 0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar2 = 0xe;
    }
    else {
      sndObj = (SndInstance *)iVar1;
      if (sndObj->dsLockedPtr == (void *)0x0) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar2 = 0xf;
    }
    else {
      (**(code **)(**(int **)(iVar1 + 0xbc) + 0x4c))
                (*(undefined4 *)(iVar1 + 0xbc),sndObj->dsLockedPtr,
                 *(undefined4 *)(iVar1 + 0x8c),0,0);
      *(uint *)(iVar1 + 4) = *(uint *)(iVar1 + 4) & 0xffffff7f;
      sndObj->dsLockedPtr = (void *)0x0;
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
      uVar2 = 0;
    }
    }
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}

// FUNCTION: MAGSND 0x10003656
int __cdecl FUN_10003656(undefined4 *param_1)

{
  int iVar1;
  
  param_1[1] = param_1[1] & 0xfffffffe;
  param_1[1] = param_1[1] & 0xfffffffd;
  param_1[1] = param_1[1] & 0xffffffef;
  param_1[1] = param_1[1] & 0xfffffffb;
  param_1[0x74] = 0;
  param_1[0x75] = 0;
  param_1[0x76] = 0;
  param_1[0x77] = 0;
  DAT_10007038 = 0;
  if (((uint)param_1[2] >> 5 & 1) == 0) {
    param_1[0x67] = param_1[0x68];
    mmioSetInfo((HMMIO)param_1[0x72],(LPCMMIOINFO)(param_1 + 0x60),0);
    iVar1 = FUN_10004e4c(param_1,0);
    if (iVar1 != 0) {
      return iVar1;
    }
  }
  else {
    PrimeAviAudio(param_1,0);
  }
  param_1[1] = param_1[1] | 0x20;
  return 0;
}

// FUNCTION: MAGSND 0x10003748
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl FUN_10003748(int param_1)

{
  uint uVar1;
  int local_38;
  int local_34;
  void *local_30;
  uint local_2c;
  uint local_28;
  uint local_24;
  undefined4 local_20;
  int local_1c;
  uint local_18;
  int local_14;
  void *local_10;
  size_t local_c;
  size_t local_8;
  
  local_20 = 0;
  local_34 = 0;
  local_18 = 0;
  local_2c = 0;
  local_24 = 0;
  local_1c = 0;
  local_30 = (void *)0x0;
  local_10 = (void *)0x0;
  local_8 = 0;
  local_c = 0;
  local_28 = 0;
  EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  (**(code **)(**(int **)(param_1 + 0xbc) + 0x10))
            (*(undefined4 *)(param_1 + 0xbc),&local_34,&local_20);
  *(int *)(param_1 + 0x1dc) =
       *(int *)(param_1 + 0x1dc) + (local_34 - *(int *)(param_1 + 0x1dc) & 0xffffU);
  if (*(uint *)(param_1 + 0x1cc) < *(uint *)(param_1 + 0x1dc)) {
    *(int *)(param_1 + 0x1dc) = *(int *)(param_1 + 0x1dc) - *(int *)(param_1 + 0x1cc);
  }
  if ((((*(uint *)(param_1 + 4) >> 4 & 1) == 0) || (*(uint *)(param_1 + 0x1d4) < 0x10000)) &&
     (((*(uint *)(param_1 + 4) >> 1 & 1) == 0 || (*(int *)(param_1 + 0x1f0) != 0)))) {
    uVar1 = local_34 - *(int *)(param_1 + 0x1d8) & 0xffff;
    local_24 = *(int *)(param_1 + 0x1cc) - *(int *)(param_1 + 0x1d0);
    if ((*(uint *)(param_1 + 4) >> 4 & 1) == 0) {
      local_2c = *(int *)(param_1 + 0x1d0) - *(int *)(param_1 + 0x1d4);
      if (0x10000 < local_2c) {
        _DAT_1000707c = _DAT_1000707c + 1;
      }
    }
    else {
      local_2c = 0;
    }
    local_1c = local_24 + local_2c;
    local_18 = uVar1;
    if (local_1c == 0) {
      local_18 = 0;
      local_28 = uVar1;
    }
    if ((local_18 == 0) && (local_28 == 0)) {
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    }
    else {
      if (local_2c < local_18) {
        local_18 = local_2c;
      }
      if (local_28 + local_18 != 0) {
        local_14 = (**(code **)(**(int **)(param_1 + 0xbc) + 0x2c))
                             (*(undefined4 *)(param_1 + 0xbc),*(undefined4 *)(param_1 + 0x1d8),
                              local_28 + local_18,&local_30,&local_8,&local_10,&local_c,0);
        if (local_14 != 0) {
          LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
          return;
        }
        *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) | 0x80;
        if (local_28 == 0) {
          mmioGetInfo(*(HMMIO *)(param_1 + 0x1c8),(LPMMIOINFO)(param_1 + 0x180),0);
          memmove(local_30,*(void **)(param_1 + 0x19c),local_8);
          *(int *)(param_1 + 0x19c) = *(int *)(param_1 + 0x19c) + local_8;
          if (local_10 != (void *)0x0) {
            memmove(local_10,*(void **)(param_1 + 0x19c),local_c);
            *(int *)(param_1 + 0x19c) = *(int *)(param_1 + 0x19c) + local_c;
          }
          mmioSetInfo(*(HMMIO *)(param_1 + 0x1c8),(LPCMMIOINFO)(param_1 + 0x180),0);
          local_2c = local_2c - local_18;
          local_1c = local_1c - local_18;
          *(int *)(param_1 + 0x1d4) = *(int *)(param_1 + 0x1d4) + local_18;
        }
        else {
          if (*(short *)(param_1 + 0x86) == 8) {
            local_38 = 0x80;
          }
          else {
            local_38 = 0;
          }
          memset(local_30,local_38,local_8);
          if (local_10 != (void *)0x0) {
            memset(local_10,local_38,local_c);
          }
          *(int *)(param_1 + 0x1d4) = *(int *)(param_1 + 0x1d4) + local_28;
        }
        *(uint *)(param_1 + 0x1d8) = *(int *)(param_1 + 0x1d8) + local_28 + local_18 & 0xffff;
        (**(code **)(**(int **)(param_1 + 0xbc) + 0x4c))
                  (*(undefined4 *)(param_1 + 0xbc),local_30,local_8,local_10,local_c);
        *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) & 0xffffff7f;
      }
      if (((local_1c != 0) && (local_24 != 0)) && ((local_2c == 0 || (local_2c < local_18 * 2)))) {
        mmioAdvance(*(HMMIO *)(param_1 + 0x1c8),(LPMMIOINFO)(param_1 + 0x180),0);
        if (local_24 < *(int *)(param_1 + 0x194) - local_2c) {
          *(int *)(param_1 + 0x1d0) = *(int *)(param_1 + 0x1d0) + local_24;
        }
        else {
          *(int *)(param_1 + 0x1d0) =
               *(int *)(param_1 + 0x1d0) + (*(int *)(param_1 + 0x194) - local_2c);
        }
      }
      if (local_1c == 0) {
        if ((*(byte *)(param_1 + 8) & 1) == 0) {
          if ((*(uint *)(param_1 + 4) >> 4 & 1) == 0) {
            *(undefined4 *)(param_1 + 0x1e4) = *(undefined4 *)(param_1 + 0x1d8);
            *(undefined4 *)(param_1 + 0x1d4) = 0;
            *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) | 0x10;
          }
        }
        else {
          *(undefined4 *)(param_1 + 0x19c) = *(undefined4 *)(param_1 + 0x1a0);
          mmioSetInfo(*(HMMIO *)(param_1 + 0x1c8),(LPCMMIOINFO)(param_1 + 0x180),0);
          mmioSeek(*(HMMIO *)(param_1 + 0x1c8),*(LONG *)(param_1 + 0x1e0),0);
          mmioGetInfo(*(HMMIO *)(param_1 + 0x1c8),(LPMMIOINFO)(param_1 + 0x180),0);
          mmioAdvance(*(HMMIO *)(param_1 + 0x1c8),(LPMMIOINFO)(param_1 + 0x180),0);
          *(int *)(param_1 + 0x1d0) = *(int *)(param_1 + 0x19c) - *(int *)(param_1 + 0x198);
          if (*(uint *)(param_1 + 0x1cc) < *(uint *)(param_1 + 0x1d0)) {
            *(undefined4 *)(param_1 + 0x1d0) = *(undefined4 *)(param_1 + 0x1cc);
          }
          *(undefined4 *)(param_1 + 0x1d4) = 0;
        }
      }
      LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    }
  }
  else {
    (**(code **)(**(int **)(param_1 + 0xbc) + 0x48))(*(undefined4 *)(param_1 + 0xbc));
    if ((0 < DAT_10007028) && (DAT_10007028 = DAT_10007028 + -1, DAT_10007028 == 0)) {
      FUN_1000448d();
    }
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) & 0xfffffffe;
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) & 0xfffffffd;
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) | 4;
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) & 0xffffffdf;
    if ((*(uint *)(param_1 + 8) >> 4 & 1) != 0) {
      *(uint *)(*(int *)(&g_sndSlots + *(int *)(param_1 + 0x10) * 4) + 4) =
           *(uint *)(*(int *)(&g_sndSlots + *(int *)(param_1 + 0x10) * 4) + 4) & 0xffffffbf;
      *(uint *)(*(int *)(&g_sndSlots + *(int *)(param_1 + 0x10) * 4) + 4) =
           *(uint *)(*(int *)(&g_sndSlots + *(int *)(param_1 + 0x10) * 4) + 4) & 0xfffffffe;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  }
  return;
}

// FUNCTION: MAGSND 0x10003D60
undefined4 __cdecl FUN_10003d60(undefined4 *param_1)

{
  uint uVar1;
  uint uVar2;
  uint local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  uint local_34;
  uint local_30;
  undefined1 local_2c [4];
  undefined4 local_28;
  undefined4 local_24;
  uint local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  uint local_c;
  uint local_8;
  
  local_28 = 0;
  local_48 = 0;
  local_40 = 0;
  local_38 = 0;
  local_24 = 0;
  local_44 = 0;
  local_10 = 0;
  local_8 = 0;
  local_c = 0;
  local_3c = 0;
  (**(code **)(*(int *)param_1[0x2f] + 0x10))(param_1[0x2f],&local_48,&local_28);
  local_34 = (uint)param_1[0x77] % (uint)param_1[0x2c];
  if (local_34 < local_48) {
    param_1[0x77] = param_1[0x77] + (local_48 - local_34);
  }
  else {
    param_1[0x77] = param_1[0x77] + (param_1[0x2c] - local_34);
    param_1[0x77] = param_1[0x77] + local_48;
  }
  local_30 = local_48 / (uint)param_1[0x23];
  local_20 = (uint)param_1[0x76] / (uint)param_1[0x23];
  if (local_30 != local_20) {
    if (((((uint)param_1[1] >> 4 & 1) == 0) || ((uint)param_1[0x75] < (uint)param_1[0x2c])) &&
       ((((uint)param_1[1] >> 1 & 1) == 0 || (param_1[0x7c] != 0)))) {
      local_18 = param_1[0x23];
      if (((uint)param_1[1] >> 4 & 1) == 0) {
        uVar1 = param_1[0x74];
        uVar2 = param_1[0x24];
        local_14 = (**(code **)(*(int *)param_1[0x2f] + 0x2c))
                             (param_1[0x2f],param_1[0x76],local_18,&local_44,&local_8,&local_10,
                              &local_c,0);
        if (local_14 != 0) {
          return 10;
        }
        AVIStreamRead(*param_1,uVar1 / uVar2,local_8 / (uint)param_1[0x24],local_44,local_8,local_2c
                      ,&local_1c);
        if (local_10 != 0) {
          AVIStreamRead(*param_1,local_1c + uVar1 / uVar2,local_c / (uint)param_1[0x24],local_10,
                        local_c,local_2c,&local_1c);
        }
        param_1[0x76] = param_1[0x76] + local_18;
        param_1[0x76] = (uint)param_1[0x76] % (uint)param_1[0x2c];
        param_1[0x74] = param_1[0x74] + local_18;
        param_1[0x75] = param_1[0x75] + local_18;
        if ((uint)param_1[0x73] <= (uint)param_1[0x74]) {
          param_1[1] = param_1[1] | 0x10;
          param_1[0x75] = 0;
        }
        (**(code **)(*(int *)param_1[0x2f] + 0x4c))(param_1[0x2f],local_44,local_8,local_10,local_c)
        ;
        param_1[1] = param_1[1] & 0xffffff7f;
      }
      else {
        param_1[0x75] = param_1[0x75] + local_18;
      }
    }
    else {
      (**(code **)(*(int *)param_1[0x2f] + 0x48))(param_1[0x2f]);
      if ((0 < DAT_10007028) && (DAT_10007028 = DAT_10007028 + -1, DAT_10007028 == 0)) {
        FUN_1000448d();
      }
      param_1[1] = param_1[1] & 0xfffffffe;
      param_1[1] = param_1[1] & 0xfffffffd;
      param_1[1] = param_1[1] | 4;
      param_1[1] = param_1[1] & 0xffffffdf;
      param_1[0x77] = 0;
    }
  }
  return 0;
}

// FUNCTION: MAGSND 0x100040C9
undefined4 __cdecl FUN_100040c9(int param_1,int *param_2)

{
  int iVar1;
  undefined4 uVar2;
  byte local_1c [4];
  undefined4 local_18;
  undefined4 local_14;
  int *local_10;
  int local_c;
  int *local_8;
  
  local_10 = *(int **)(param_1 + 0xbc);
  local_8 = local_10;
  iVar1 = (**(code **)(*local_10 + 0x24))(local_10,local_1c);
  if (iVar1 == 0) {
    if ((local_1c[0] & 2) == 0) {
      if ((local_1c[0] & 1) == 0) {
        *param_2 = (int)local_8;
        uVar2 = 0;
      }
      else {
        for (local_c = 0; local_14 = 0, local_c < 0x10; local_c = local_c + 1) {
          local_10 = *(int **)(param_1 + 0xc0 + local_c * 0xc);
          if (local_10 == (int *)0x0) {
            iVar1 = (**(code **)(*DAT_100084f0 + 0x14))(DAT_100084f0,local_8,&local_18);
            if (iVar1 != 0) {
              return 9;
            }
            *(undefined4 *)(param_1 + 0xc0 + local_c * 0xc) = local_18;
            *param_2 = *(int *)(param_1 + 0xc0 + local_c * 0xc);
            return 0;
          }
          iVar1 = (**(code **)(*local_10 + 0x24))(local_10,local_1c);
          if (iVar1 != 0) {
            return 9;
          }
          if ((local_1c[0] & 1) == 0) {
            *param_2 = (int)local_10;
            return 0;
          }
        }
        uVar2 = 9;
      }
    }
    else {
      uVar2 = 9;
    }
  }
  else {
    uVar2 = 9;
  }
  return uVar2;
}

// FUNCTION: MAGSND 0x10004239
void __cdecl FUN_10004239(int param_1)

{
  if (DAT_10007018 == 0) {
    DAT_10007018 = param_1;
  }
  else {
    *(int *)(DAT_1000701c + 0x1f8) = param_1;
    *(int *)(param_1 + 500) = DAT_1000701c;
  }
  DAT_1000701c = param_1;
  return;
}

// FUNCTION: MAGSND 0x10004292
void __cdecl FUN_10004292(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = *(int *)(param_1 + 500);
  iVar2 = *(int *)(param_1 + 0x1f8);
  iVar3 = iVar2;
  if (iVar1 != 0) {
    *(int *)(iVar1 + 0x1f8) = iVar2;
    iVar3 = DAT_10007018;
  }
  DAT_10007018 = iVar3;
  if (iVar2 != 0) {
    *(int *)(iVar2 + 500) = iVar1;
    iVar1 = DAT_1000701c;
  }
  DAT_1000701c = iVar1;
  return;
}

// FUNCTION: MAGSND 0x10004311
void __cdecl FUN_10004311(int param_1)

{
  if (g_activeSndListHead == 0) {
    g_activeSndListHead = param_1;
  }
  else {
    *(int *)(DAT_10007014 + 0x200) = param_1;
    *(int *)(param_1 + 0x1fc) = DAT_10007014;
  }
  DAT_10007014 = param_1;
  return;
}

// FUNCTION: MAGSND 0x1000436A
void __cdecl FUN_1000436a(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = *(int *)(param_1 + 0x1fc);
  iVar2 = *(int *)(param_1 + 0x200);
  iVar3 = iVar2;
  if (iVar1 != 0) {
    *(int *)(iVar1 + 0x200) = iVar2;
    iVar3 = g_activeSndListHead;
  }
  g_activeSndListHead = iVar3;
  if (iVar2 != 0) {
    *(int *)(iVar2 + 0x1fc) = iVar1;
    iVar1 = DAT_10007014;
  }
  DAT_10007014 = iVar1;
  return;
}

// FUNCTION: MAGSND 0x100043E9
void CALLBACK FUN_100043e9(UINT u1, UINT u2, DWORD dw1, DWORD dw2, DWORD dw3)

{
  (void)u1;
  (void)u2;
  (void)dw1;
  (void)dw2;
  (void)dw3;
  UpdateSnd();
  return;
}

// FUNCTION: MAGSND 0x10004400
undefined4 __cdecl StartUpdateTimer(undefined4 resolutionMs)

{
  MMRESULT MVar1;
  undefined4 uVar2;

  (void)resolutionMs;
  MVar1 = timeGetDevCaps((LPTIMECAPS)&DAT_100084f8,8);
  if (MVar1 == 0) {
    timeBeginPeriod(g_updatePeriodMs);
    g_updateTimerId = timeSetEvent(g_updatePeriodMs,DAT_100084f8,FUN_100043e9,0x100084e8,1);
    if (g_updateTimerId == 0) {
      timeEndPeriod(g_updatePeriodMs);
      uVar2 = 0xc;
    }
    else {
      g_updateTimerActive = 1;
      uVar2 = 0;
    }
  }
  else {
    uVar2 = 0xc;
  }
  return uVar2;
}

// FUNCTION: MAGSND 0x1000448D
void FUN_1000448d(void)

{
  if (g_updateTimerActive != 0) {
    EnterCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
    timeKillEvent(g_updateTimerId);
    g_updateTimerId = 0;
    timeEndPeriod(g_updatePeriodMs);
    g_updateTimerActive = 0;
    LeaveCriticalSection((LPCRITICAL_SECTION)&g_sndCs);
  }
  return;
}

// FUNCTION: MAGSND 0x100044F0
undefined4 __cdecl LoadFromFile(char *filename,SndInstance **outSnd)

{
  FILE *_File;
  undefined4 uVar1;

  _File = fopen(filename,DAT_1000708c);
  if (_File == (FILE *)0x0) {
    uVar1 = 7;
  }
  else {
    uVar1 = FUN_100049fe(_File,(int *)outSnd);
    fclose(_File);
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x1000454F
undefined4 __cdecl LoadWaveMmio(LPSTR filename,SndInstance **outSnd,undefined4 scratch)

{
  undefined4 uVar1;
  MMRESULT MVar2;
  DWORD DVar3;
  void *pvVar4;
  size_t sVar5;
  LONG LVar6;
  int iVar7;
  int iVar8;
  MMIOINFO *p_Var9;
  MMCKINFO *p_Var10;
  DWORD *pDVar11;
  FOURCC *pFVar12;
  MMCKINFO local_98;
  char local_84 [16];
  undefined2 local_74;
  MMIOINFO local_70;
  undefined4 local_28;
  DWORD local_24;
  LONG local_20;
  MMCKINFO local_1c;
  HMMIO local_8;

  local_28 = 0;
  memset(&local_70,0,0x48);
  local_70.cchBuffer = 0x10000;
  local_8 = mmioOpenA(filename,&local_70,0x10000);
  if (local_8 == (HMMIO)0x0) {
    uVar1 = 7;
  }
  else {
    local_1c.fccType = 0x45564157;
    MVar2 = mmioDescend(local_8,&local_1c,(MMCKINFO *)0x0,0x20);
    if (MVar2 == 0) {
      local_98.ckid = 0x20746d66;
      MVar2 = mmioDescend(local_8,&local_98,&local_1c,0x10);
      if (MVar2 == 0) {
        local_24 = local_98.cksize;
        DVar3 = mmioRead(local_8,local_84,local_98.cksize);
        if (DVar3 == local_24) {
          local_74 = 0;
          mmioAscend(local_8,&local_98,0);
          local_98.ckid = 0x61746164;
          MVar2 = mmioDescend(local_8,&local_98,&local_1c,0x10);
          if (MVar2 == 0) {
            mmioGetInfo(local_8,&local_70,0);
            mmioAdvance(local_8,&local_70,0);
            local_20 = local_70.lBufOffset;
            scratch = 0xe8;
            pvVar4 = FUN_10005480(DAT_100084f0,0x10000,(undefined4 *)local_84,0xe8);
            *outSnd = (SndInstance *)pvVar4;
            if (*outSnd == 0) {
              mmioClose(local_8,0);
              uVar1 = 9;
            }
            else {
              *(LONG *)((int)*outSnd + 0x1e0) = local_20;
              *(HMMIO *)((int)*outSnd + 0x1c8) = local_8;
              memcpy((void *)((int)*outSnd + 0x180), &local_70, sizeof(MMIOINFO));
              *(DWORD *)((int)*outSnd + 0x1cc) = local_98.cksize;
              sVar5 = strlen(filename);
              pvVar4 = operator_new(sVar5 + 1);
              *(void **)*outSnd = pvVar4;
              if (*(int *)*outSnd == 0) {
                mmioClose(local_8,0);
                uVar1 = 3;
              }
              else {
                strcpy(*(char **)*outSnd,filename);
                *(uint *)(*outSnd + 8) = *(uint *)(*outSnd + 8) & 0xffffffdf;
                *(uint *)(*outSnd + 8) = *(uint *)(*outSnd + 8) | 2;
                *(undefined4 *)(*outSnd + 0x8c) = 0;
                *(undefined4 *)(*outSnd + 0x90) = 0;
                *(undefined4 *)(*outSnd + 0x94) = 0;
                mmioAscend(local_8,&local_98,0);
                local_98.ckid = 0x20657563;
                MVar2 = mmioDescend(local_8,&local_98,&local_1c,0x10);
                if (MVar2 == 0) {
                  iVar7 = *outSnd;
                  memcpy((void *)(iVar7 + 0x1c), &local_98.ckid, 5 * sizeof(FOURCC));
                  LVar6 = mmioRead(local_8,(HPSTR)(iVar7 + 0x30),4);
                  if (LVar6 != 4) {
                    FUN_10005562(*outSnd);
                    mmioClose(local_8,0);
                    return 8;
                  }
                  pvVar4 = operator_new(*(int *)(iVar7 + 0x30) * 0x18);
                  *(void **)(iVar7 + 0x34) = pvVar4;
                  if (*(int *)(iVar7 + 0x34) == 0) {
                    FUN_10005562(*outSnd);
                    mmioClose(local_8,0);
                    return 3;
                  }
                  local_24 = *(int *)(iVar7 + 0x30) * 0x18;
                  LVar6 = mmioRead(local_8,*(HPSTR *)(iVar7 + 0x34),local_24);
                  if (LVar6 != local_24) {
                    FUN_10005562(*outSnd);
                    mmioClose(local_8,0);
                    operator_delete(*(void **)(iVar7 + 0x34));
                    return 8;
                  }
                }
                uVar1 = 0;
              }
            }
          }
          else {
            mmioClose(local_8,0);
            uVar1 = 8;
          }
        }
        else {
          mmioClose(local_8,0);
          uVar1 = 8;
        }
      }
      else {
        mmioClose(local_8,0);
        uVar1 = 8;
      }
    }
    else {
      mmioClose(local_8,0);
      uVar1 = 8;
    }
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x10004986
void __cdecl FUN_10004986(undefined4 *param_1)

{
  mmioClose((HMMIO)param_1[0x72],0);
  if (param_1[0xd] != 0) {
    operator_delete((void *)param_1[0xd]);
  }
  if (((uint)param_1[2] >> 5 & 1) == 0) {
    operator_delete((void *)*param_1);
  }
  return;
}

// FUNCTION: MAGSND 0x100049FE
undefined4 __cdecl FUN_100049fe(FILE *param_1,int *param_2)

{
  undefined4 uVar1;
  int iVar2;
  void *pvVar3;
  undefined4 local_50 [4];
  undefined2 local_40;
  void *local_3c;
  int local_38;
  uint local_34;
  int local_30;
  int local_2c;
  int local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined1 local_1c [4];
  undefined4 local_18;
  int local_14;
  size_t local_10;
  size_t local_c;
  undefined4 local_8;
  
  local_3c = (void *)0x0;
  local_24 = 0;
  local_10 = 0;
  local_20 = 0;
  local_c = 0;
  local_8 = 0;
  local_34 = 0;
  local_38 = 0;
  fseek(param_1,0,0);
  fread(&local_30,1,0xc,param_1);
  local_34 = local_2c + local_38 + 8;
  if ((local_30 == 0x46464952) && (local_28 == 0x45564157)) {
    iVar2 = FUN_10004d3f(param_1,0x20746d66,local_38,local_34);
    if (iVar2 == 0) {
      uVar1 = 8;
    }
    else {
      fread(local_1c,1,8,param_1);
      fread(local_50,1,0x10,param_1);
      local_40 = 0;
      iVar2 = FUN_10004d3f(param_1,0x61746164,local_38,local_34);
      if (iVar2 == 0) {
        uVar1 = 8;
      }
      else {
        fread(local_1c,1,8,param_1);
        local_8 = 0xea;
        pvVar3 = FUN_10005480(DAT_100084f0,local_18,local_50,0xea);
        *param_2 = (int)pvVar3;
        if (*param_2 == 0) {
          uVar1 = 9;
        }
        else {
          local_14 = (**(code **)(**(int **)(*param_2 + 0xbc) + 0x2c))
                               (*(undefined4 *)(*param_2 + 0xbc),0,local_18,&local_3c,&local_10,
                                &local_24,&local_20,0);
          if (local_14 == 0) {
            local_c = fread(local_3c,1,local_10,param_1);
            if (local_10 == local_c) {
              local_14 = (**(code **)(**(int **)(*param_2 + 0xbc) + 0x4c))
                                   (*(undefined4 *)(*param_2 + 0xbc),local_3c,local_10,local_24,
                                    local_20);
              if (local_14 == 0) {
                uVar1 = 0;
              }
              else {
                (**(code **)(**(int **)(*param_2 + 0xbc) + 8))(*(undefined4 *)(*param_2 + 0xbc));
                operator_delete((void *)*param_2);
                uVar1 = 9;
              }
            }
            else {
              FUN_10005562((SndInstance *)*param_2);
              uVar1 = 9;
            }
          }
          else {
            FUN_10005562((SndInstance *)*param_2);
            uVar1 = 9;
          }
        }
      }
    }
  }
  else {
    uVar1 = 8;
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x10004C8A
undefined4 __cdecl FUN_10004c8a(int param_1,int param_2)

{
  SndInstance *sndObj;
  ushort uVar1;
  int iVar2;
  int iVar3;
  
  sndObj = (SndInstance *)param_1;
  mmioGetInfo(sndObj->hMmio,(LPMMIOINFO)&sndObj->mmioInfo,0);
  uVar1 = sndObj->waveFmt.nBlockAlign;
  iVar2 = sndObj->mmioBaseOffset;
  iVar3 = FUN_100058d0((int)&sndObj->mmioInfo);
  mmioSeek(sndObj->hMmio,((uint)uVar1 * param_2 + iVar2) - iVar3,1);
  mmioGetInfo(sndObj->hMmio,(LPMMIOINFO)&sndObj->mmioInfo,0);
  mmioAdvance(sndObj->hMmio,(LPMMIOINFO)&sndObj->mmioInfo,0);
  return 0;
}

// FUNCTION: MAGSND 0x10004D3F
undefined4 __cdecl FUN_10004d3f(FILE *param_1,int param_2,long param_3,uint param_4)

{
  undefined4 uVar1;
  int local_1c;
  uint local_18;
  uint local_14;
  int local_10 [3];
  
  fseek(param_1,param_3,0);
  fread(local_10,1,0xc,param_1);
  if (local_10[0] == param_2) {
    fseek(param_1,param_3,0);
    uVar1 = 1;
  }
  else {
    while( true ) {
      local_14 = ftell(param_1);
      fread(&local_1c,1,8,param_1);
      if (param_4 <= local_14) break;
      if (local_1c == param_2) {
        fseek(param_1,local_14,0);
        return 1;
      }
      local_14 = ftell(param_1);
      if ((local_18 & 1) != 0) {
        local_18 = local_18 + 1;
      }
      fseek(param_1,local_18,1);
    }
    fseek(param_1,param_3,0);
    uVar1 = 0;
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x10004E4C
undefined4 __cdecl FUN_10004e4c(undefined4 *param_1,int param_2)

{
  undefined4 uVar1;
  int local_4c;
  int local_44;
  HPSTR local_40;
  int local_3c;
  int local_38;
  undefined4 local_34;
  size_t local_30;
  HPSTR local_2c;
  int local_28;
  int local_24;
  int local_20;
  undefined4 local_1c;
  undefined4 local_18;
  int local_14;
  int local_10;
  int local_c;
  undefined4 local_8;
  
  local_40 = (HPSTR)0x0;
  local_1c = 0;
  local_2c = (HPSTR)0x0;
  local_8 = 0;
  local_18 = 0;
  local_34 = 0;
  local_38 = 0;
  if (((uint)param_1[2] >> 5 & 1) == 0) {
    FUN_10004c8a((int)param_1,param_2);
  }
  local_3c = (uint)*(ushort *)(param_1 + 0x21) * param_2;
  local_28 = param_1[0x73];
  local_4c = local_28 - local_3c;
  local_30 = 0x10000;
  if ((local_3c < 0) || (local_28 < local_3c)) {
    uVar1 = 5;
  }
  else {
    local_20 = local_3c;
    local_c = local_3c;
    local_14 = (**(code **)(*(int *)param_1[0x2f] + 0x2c))
                         (param_1[0x2f],0,0x10000,&local_40,&local_8,&local_1c,&local_18,0);
    if (local_14 == 0) {
      local_2c = local_40;
      local_14 = 0;
      while (local_38 == 0) {
        if (local_4c < (int)local_30) {
          if (((uint)param_1[2] >> 5 & 1) == 0) {
            local_10 = mmioRead((HMMIO)param_1[0x72],local_2c,local_4c);
          }
          else {
            AVIStreamRead(*param_1,param_2,
                          (int)(0x10000 /
                               (ulonglong)(longlong)(int)(uint)*(ushort *)(param_1 + 0x21)),local_2c
                          ,local_4c,&local_10,0);
          }
          local_2c = local_2c + local_10;
          local_30 = local_30 - local_10;
          local_4c = 0;
          local_20 = param_1[0x73];
          if ((*(byte *)(param_1 + 2) & 1) == 0) {
            memset(local_2c,0,local_30);
            local_38 = 1;
          }
          else {
            if (((uint)param_1[2] >> 5 & 1) == 0) {
              FUN_10004c8a((int)param_1,0);
            }
            local_20 = 0;
            local_4c = param_1[0x73];
            local_c = 0;
          }
        }
        else {
          if (((uint)param_1[2] >> 5 & 1) == 0) {
            local_10 = mmioRead((HMMIO)param_1[0x72],local_2c,local_30);
          }
          else {
            AVIStreamRead(*param_1,param_2,
                          (int)(0x10000 /
                               (ulonglong)(longlong)(int)(uint)*(ushort *)(param_1 + 0x21)),local_2c
                          ,0x10000,&local_10,&local_44);
          }
          local_30 = local_30 - local_10;
          local_c = local_c + local_10;
          local_4c = local_4c - local_10;
          local_20 = local_20 + local_10;
          param_2 = param_2 + local_44;
          local_38 = 1;
        }
      }
      mmioGetInfo((HMMIO)param_1[0x72],(LPMMIOINFO)(param_1 + 0x60),0);
      if (((uint)param_1[2] >> 5 & 1) == 0) {
        mmioAdvance((HMMIO)param_1[0x72],(LPMMIOINFO)(param_1 + 0x60),0);
      }
      else {
        FUN_10005770((SndInstance *)param_1,param_2);
      }
      local_24 = local_4c;
      if (0xffff < local_4c) {
        local_24 = 0x10000;
      }
      param_1[0x74] = param_1[0x73] - (local_4c - local_24);
      param_1[0x75] = param_1[0x73] - local_4c;
      param_1[0x76] = 0;
      local_14 = (**(code **)(*(int *)param_1[0x2f] + 0x4c))
                           (param_1[0x2f],local_40,local_8,local_1c,local_18);
      if (local_14 == 0) {
        uVar1 = 0;
      }
      else {
        FUN_10004986(param_1);
        FUN_10005562((SndInstance *)param_1);
        uVar1 = 9;
      }
    }
    else {
      if (((uint)param_1[2] >> 5 & 1) == 0) {
        FUN_10004986(param_1);
      }
      else {
        FUN_1000575e();
      }
      FUN_10005562((SndInstance *)param_1);
      uVar1 = 9;
    }
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x1000524C
undefined4 __cdecl PrimeAviAudio(SndInstance *sndObj,undefined4 unused)

{
  undefined4 uVar1;
  int local_34;
  uint local_30;
  undefined1 local_2c [4];
  uint local_28;
  int local_24;
  undefined1 local_20 [4];
  int local_1c;
  int local_18;
  undefined4 local_14;
  int local_10;
  undefined4 local_c;
  int local_8;

  (void)unused;
  local_24 = sndObj->blockCount * (uint)sndObj->blockBytes;
  local_8 = (**(code **)(*(int *)sndObj->dsBuffer + 0x2c))
                      (sndObj->dsBuffer,0,local_24,&local_34,&local_c,&local_18,&local_14,0);
  if (local_8 == 0) {
    if (local_18 == 0) {
      sndObj->ringCursorBytes = 0;
      sndObj->writeCursorBytes = 0;
      sndObj->readCursorBytes = 0;
      local_1c = local_34;
      local_10 = 0;
      local_28 = (uint)sndObj->blockBytes / (uint)sndObj->blockScale;
      local_8 = 0;
      for (local_30 = 0; local_30 < (uint)sndObj->blockCount; local_30 = local_30 + 1) {
        AVIStreamRead(sndObj->streamOrMmio,local_10,local_28,local_1c,sndObj->blockBytes,local_2c,local_20);
        sndObj->ringCursorBytes = sndObj->ringCursorBytes + sndObj->blockBytes;
        sndObj->ringCursorBytes = (uint)sndObj->ringCursorBytes % (uint)sndObj->dsDesc.dwBufferBytes;
        sndObj->writeCursorBytes = sndObj->writeCursorBytes + sndObj->blockBytes;
        sndObj->readCursorBytes = sndObj->readCursorBytes + sndObj->blockBytes;
        local_1c = local_1c + sndObj->blockBytes;
        local_10 = local_10 + local_28;
      }
      local_8 = (**(code **)(*(int *)sndObj->dsBuffer + 0x4c))
                          (sndObj->dsBuffer,local_34,local_c,local_18,local_14);
      if (local_8 == 0) {
        uVar1 = 0;
      }
      else {
        FUN_10004986((undefined4 *)sndObj);
        FUN_10005562(sndObj);
        uVar1 = 9;
      }
    }
    else {
      (**(code **)(*(int *)sndObj->dsBuffer + 0x4c))(sndObj->dsBuffer,local_34,local_c,local_18,local_14);
      FUN_1000575e();
      FUN_10005562(sndObj);
      uVar1 = 9;
    }
  }
  else {
    FUN_1000575e();
    FUN_10005562(sndObj);
    uVar1 = 9;
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x10005480
SndInstance * __cdecl FUN_10005480(int *param_1,undefined4 param_2,undefined4 *param_3,undefined4 param_4)

{
  SndInstance *sndObj;
  int iVar1;
  
  sndObj = (SndInstance *)operator_new(0x204);
  memset(sndObj,0,0x204);
  sndObj->dsDesc.dwSize = 0x14;
  sndObj->dsDesc.dwFlags = param_4;
  sndObj->dsDesc.dwBufferBytes = param_2;
  sndObj->dsDesc.lpwfxFormat = param_3;
  *(undefined4 *)&sndObj->waveFmt = *param_3;
  *(undefined4 *)((byte *)&sndObj->waveFmt + 4) = param_3[1];
  *(undefined4 *)((byte *)&sndObj->waveFmt + 8) = param_3[2];
  *(undefined4 *)((byte *)&sndObj->waveFmt + 0xc) = param_3[3];
  *(undefined2 *)((byte *)&sndObj->waveFmt + 0x10) = *(undefined2 *)(param_3 + 4);
  iVar1 = (**(code **)(*param_1 + 0xc))(param_1,(int)&sndObj->dsDesc,(int)&sndObj->dsBuffer,0);
  if (iVar1 != 0) {
    operator_delete(sndObj);
    sndObj = (SndInstance *)0x0;
  }
  return sndObj;
}

// FUNCTION: MAGSND 0x10005562
undefined4 __cdecl FUN_10005562(SndInstance *param_1)

{
  undefined4 uVar1;
  
  if (param_1 == (SndInstance *)0x0) {
    uVar1 = 5;
  }
  else {
    (**(code **)(**(int **)((int)param_1 + 0xbc) + 8))(*(undefined4 *)((int)param_1 + 0xbc));
    operator_delete(param_1);
    uVar1 = 0;
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x100055BB
undefined4 __cdecl LoadAviAudioStream(void *stream,SndInstance **outSnd,undefined4 scratch)

{
  undefined4 uVar1;
  void *pvVar2;
  SndInstance *sndObj;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 local_fc [4];
  undefined2 local_ec;
  undefined4 local_e8 [18];
  undefined1 local_a0 [4];
  int local_9c;
  int local_98;
  undefined4 local_94;
  int local_90 [8];
  int local_70;
  int local_68;
  int local_60;

  local_94 = 0x12;
  AVIStreamInfoA((PAVISTREAM)stream,local_90,0x8c);
  if (local_90[0] == 0x73647561) {
    AVIStreamRead((PAVISTREAM)stream,0,1,0,0,local_a0,0);
    local_98 = local_60 * local_70;
    AVIStreamReadFormat((PAVISTREAM)stream,0,local_fc,&local_94);
    local_ec = 0;
    scratch = 0xe8;
    local_9c = local_68 * 0xb;
    pvVar2 = FUN_10005480(DAT_100084f0,local_9c,local_fc,0xe8);
    *outSnd = (SndInstance *)pvVar2;
    if (*outSnd == 0) {
      uVar1 = 9;
    }
    else {
      sndObj = *outSnd;
      sndObj->mmioBaseOffset = 0;
      sndObj->hMmio = (HMMIO)0;
      puVar4 = local_e8;
      puVar5 = (undefined4 *)&sndObj->mmioInfo;
      for (iVar3 = 0x12; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      sndObj->dataBytes = local_98;
      sndObj->streamOrMmio = stream;
      sndObj->flags2 = sndObj->flags2 | 0x20;
      sndObj->flags2 = sndObj->flags2 | 2;
      sndObj->blockBytes = local_68;
      sndObj->blockScale = local_60;
      sndObj->blockCount = 0xb;
      sndObj->timeBytes = 0;
      uVar1 = 0;
    }
  }
  else {
    uVar1 = 8;
  }
  return uVar1;
}

// FUNCTION: MAGSND 0x10005770
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl FUN_10005770(SndInstance *sndObj,undefined4 param_2)

{
  undefined4 *param_1;
  int local_1c;
  undefined4 local_c;
  size_t local_8;
  
  param_1 = (undefined4 *)sndObj;
  local_c = 0;
  local_8 = param_1[0x74] - param_1[0x75];
  if (param_1[0x68] - param_1[0x67] != local_8) {
    _DAT_10007088 = _DAT_10007088 + 1;
  }
  local_1c = param_1[0x65] - local_8;
  if ((int)(param_1[0x73] - param_1[0x74]) < (int)(param_1[0x65] - local_8)) {
    local_1c = param_1[0x73] - param_1[0x74];
  }
  if (local_8 != 0) {
    memmove((void *)param_1[0x66],(void *)param_1[0x67],local_8);
  }
  param_1[0x67] = param_1[0x66] + local_8;
  AVIStreamRead(*param_1,param_2,local_1c / (int)(uint)*(ushort *)(param_1 + 0x21),param_1[0x67],
                local_1c,&local_c,0);
  param_1[0x68] = param_1[0x65] + param_1[0x66];
  param_1[0x67] = param_1[0x66];
  mmioSetInfo((HMMIO)param_1[0x72],(LPCMMIOINFO)(param_1 + 0x60),0);
  return;
}

// FUNCTION: MAGSND 0x100058D0
int __cdecl FUN_100058d0(int param_1)

{
  MMIOINFO *info;
  
  info = (MMIOINFO *)param_1;
  return (int)info->lDiskOffset - ((int)info->pchEndWrite - (int)info->pchEndRead);
}

// FUNCTION: MAGSND 0x1000575E
undefined4 FUN_1000575e(void)
{
  return 0;
}

long __cdecl ftol(void)
{
  return 0;
}

void * __cdecl operator_new(unsigned int size)
{
  return malloc(size);
}

void __cdecl operator_delete(void *p)
{
  if (p)
    free(p);
}
