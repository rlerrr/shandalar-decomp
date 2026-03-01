#include "magsnd.h"
#include "defs.h"
#include "mystdbool.h"
#include <DSOUND.H>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <vfw.h>

#pragma intrinsic(memcpy)

typedef unsigned char byte;
typedef int (__stdcall code)();

typedef struct SndInstance SndInstance;
typedef struct SndDuplicateBufferEntry SndDuplicateBufferEntry;

struct SndDuplicateBufferEntry {
  LPDIRECTSOUNDBUFFER buffer; /* 0x000 */
  int reserved1;              /* 0x004 */
  int reserved2;              /* 0x008 */
};

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
  SndInstance *markers[0x11];     /* 0x034 */
  WAVEFORMATEX waveFmt;   /* 0x078 (18 bytes) */
  unsigned short pad_08a;  /* 0x08a */
  int blockBytes;          /* 0x08c */
  int blockScale;          /* 0x090 */
  int blockCount;          /* 0x094 */
  byte pad_098[0xa0 - 0x98];
  uint timeEventCount;     /* 0x0a0 */
  void *dsLockedPtr;       /* 0x0a4: pointer from IDirectSoundBuffer::Lock */
  DSBUFFERDESC dsDesc;  /* 0x0a8 (DSBUFFERDESC, size 0x14) */
  LPDIRECTSOUNDBUFFER dsBuffer;          /* 0x0bc: LPDIRECTSOUNDBUFFER */
  SndDuplicateBufferEntry duplicateBuffers[0x10]; /* 0x0c0 - 0x180 */
  MMIOINFO mmioInfo;       /* 0x180 */
  HMMIO hMmio;             /* 0x1c8 */
  int dataBytes;           /* 0x1cc */
  uint writeCursorBytes;       /* 0x1d0: bytes written/consumed from stream */
  uint readCursorBytes;        /* 0x1d4 */
  uint ringCursorBytes;        /* 0x1d8 */
  uint timeBytes;              /* 0x1dc */
  int mmioBaseOffset;          /* 0x1e0 */
  uint ringCursorSavedBytes;   /* 0x1e4 */
  int pan;                     /* 0x1e8 */
  int playbackRateHz;          /* 0x1ec */
  int volume;                  /* 0x1f0 */
  SndInstance *updatePrev;     /* 0x1f4: used by AddToUpdateList/RemoveFromUpdateList */
  SndInstance *updateNext;     /* 0x1f8 */
  SndInstance *activePrev;     /* 0x1fc: used by AddToActiveList/RemoveFromActiveList */
  SndInstance *activeNext;     /* 0x200 */
};
STATIC_ASSERT(sizeof(SndInstance) == 0x204, SndInstance_wrong_size);

undefined4 __cdecl InitSnd(int param_1, undefined4 param_2, byte param_3);
void __cdecl ReleaseSnd(void);
undefined4 __cdecl GetSndHWND(void);
int __cdecl LoadSnd(LPSTR param_1, int param_2, Sound *param_3);
undefined4 __cdecl UnloadSnd(int param_1);
undefined4 __cdecl UnloadAllSnds(void);
undefined4 __cdecl PlaySnd(int slot, Sound *sound);
int __cdecl PlaySndFile(LPSTR filename, int slot, Sound *sound);
undefined4 __cdecl StopSnd(int slot);
void __cdecl StopAllSnds(void);
undefined4 __cdecl PlayMidiFile(void);
undefined4 __cdecl SetPitch(int slot, undefined4 pitchHz);
undefined4 __cdecl GetPitch(void);
undefined4 __cdecl SetVol(int slot, uint volume);
undefined4 __cdecl GetVol(void);
undefined4 __cdecl SetPan(int slot, int pan);
undefined4 __cdecl GetPan(void);
undefined4 __cdecl UpdateSnd(void);
int __cdecl SetSndMarker(int slot, uint markerIndex);
int __cdecl PlaySndMarker(int slot, uint markerIndex);
int __cdecl LoadMarkerInstance(SndInstance *parent, int markerIndex);
int __cdecl PrimeMarkerInstance(SndInstance *parent, int markerIndex);
undefined4 __cdecl GetSndTime(int slot, uint *timeMs);
undefined4 __cdecl ResetSnd(void);
undefined4 __cdecl GetSndState(int slot, undefined4 *outState);
void * __cdecl GetAVISndBuff(int slot, uint bytesRequested);
undefined4 __cdecl ReleaseAVISndBuff(int slot);
undefined4 __cdecl IsSndLoaded(int slot, undefined4 *outLoaded);
undefined4 __cdecl GetLRUSnd(int *outSlot, int loadId, int skipSlot);

void __cdecl FUN_10003748(int value);
void __cdecl AddToUpdateList(SndInstance *snd);
void __cdecl RemoveFromUpdateList(SndInstance *snd);
void __cdecl AddToActiveList(SndInstance *snd);
void __cdecl RemoveFromActiveList(SndInstance *snd);
undefined4 __cdecl AcquireFreeDsBuffer(int slot,int *outBuffer);
undefined4 __cdecl StartUpdateTimer(undefined4 resolutionMs);
void __cdecl StopUpdateTimer(void);
void CALLBACK UpdateTimerProc(UINT u1, UINT u2, DWORD dw1, DWORD dw2, DWORD dw3);
void __cdecl CloseMmioAndFreeSource(SndInstance *snd);
undefined4 __cdecl LoadFromFile(char *filename, SndInstance **outSnd);
undefined4 __cdecl LoadWaveMmio(LPSTR filename, SndInstance **outSnd, undefined4 scratch);
undefined4 __cdecl PrimeDsBufferFromSource(SndInstance *snd,int startSample);
undefined4 __cdecl PrimeAviAudio(undefined4 *snd);
undefined4 __cdecl LoadAviAudioStream(void *stream, SndInstance **outSnd, undefined4 scratch);
void __cdecl RefillAviMmioBuffer(SndInstance *sndObj, long streamStartSample);
int __cdecl MmioTellFromInfo(MMIOINFO *info);
undefined4 __cdecl LoadWaveFromFileHandle(FILE *file,int *outSource);
undefined4 __cdecl FindRiffChunk(FILE *file,int riffType,long chunkOffset,uint chunkSize);
SndInstance * __cdecl FUN_10005480(int *sndData, undefined4 aviStream, undefined4 *streamInfo, undefined4 waveFormatTag);
undefined4 __cdecl DestroySndInstance(SndInstance *snd);
void * __cdecl operator_new(unsigned int size);
void __cdecl operator_delete(void *p);

typedef unsigned short ushort;

typedef struct SndTimeSample {
  uint prevTime;
  uint currTime;
  uint unused08;
  uint unused0c;
} SndTimeSample;

// GLOBAL: MAGSND 0x10006000
double g_sndTimeDiv_88200 = 88.2;

// GLOBAL: MAGSND 0x10006008
double g_sndTimeDiv_44100 = 44.1;

// GLOBAL: MAGSND 0x10006010
double g_sndTimeDiv_22050 = 22.05;

// GLOBAL: MAGSND 0x10006018
double g_sndTimeDiv_11025 = 11.025;

// GLOBAL: MAGSND 0x10007040
DSBUFFERDESC g_primaryDsDesc;

// GLOBAL: MAGSND 0x10007058
WAVEFORMATEX g_primaryWaveFormat;

// GLOBAL: MAGSND 0x1000708C
char g_fileModeRb[] = "rb";

// GLOBAL: MAGSND 0x10007030
undefined4 g_sndLoadSerial = 0x00000000;

// GLOBAL: MAGSND 0x100070A8
SndInstance *g_sndSlots[0x110];
// GLOBAL: MAGSND 0x1000707C
undefined4 _DAT_1000707c = 0x00000000;

// GLOBAL: MAGSND 0x10007088
undefined4 g_mmioInfoMismatchCount = 0x00000000;

// GLOBAL: MAGSND 0x100074E8
SndTimeSample g_sndTimeSamples[0x100];

// GLOBAL: MAGSND 0x10008500
CRITICAL_SECTION g_sndCs;

// GLOBAL: MAGSND 0x10007010
SndInstance *g_activeSndListHead = (SndInstance *)0x0;

// GLOBAL: MAGSND 0x10007014
SndInstance *g_activeSndTail = (SndInstance *)0x0;

// GLOBAL: MAGSND 0x10007018
SndInstance *g_updateSndHead = (SndInstance *)0x0;

// GLOBAL: MAGSND 0x1000701C
SndInstance *g_updateSndTail = (SndInstance *)0x0;

// GLOBAL: MAGSND 0x10007020
undefined4 g_updateTimerEnabled = 0x00000001;

// GLOBAL: MAGSND 0x10007024
undefined4 g_updateTimerActive = 0x00000000;

// GLOBAL: MAGSND 0x10007028
int g_updateTimerUsers = 0x00000000;

// GLOBAL: MAGSND 0x1000702C
undefined4 g_updateResolutionMs = 0x00000019;

// GLOBAL: MAGSND 0x10007034
undefined4 g_sndRefCount = 0x00000000;

// GLOBAL: MAGSND 0x10007038
uint g_lastSndTime = 0x00000000;

// GLOBAL: MAGSND 0x1000706C
undefined4 DAT_1000706c = 0x00000000;

// GLOBAL: MAGSND 0x10007074
uint g_sndTimeSampleIndex = 0x00000000;

// GLOBAL: MAGSND 0x10007080
undefined4 g_updatePeriodMs = 0x00000021;

// GLOBAL: MAGSND 0x10007090
undefined4 DAT_10007090 = 0x00000000;

// GLOBAL: MAGSND 0x100070A0
LPDIRECTSOUNDBUFFER g_primaryDsBuffer;

// GLOBAL: MAGSND 0x100084E8
undefined4 g_sndHwnd = 0x00000000;

// GLOBAL: MAGSND 0x100084F0
IDirectSound *g_directSound = (int *)0x0;

// GLOBAL: MAGSND 0x100084F4
undefined4 g_updateTimerId = 0x00000000;

// GLOBAL: MAGSND 0x100084F8
TIMECAPS g_timeCaps = {0};

// GLOBAL: MAGSND 0x10008590
code *DAT_10008590 = (code *)0x0;

// FUNCTION: MAGSND 0x10001000
undefined4 __cdecl InitSnd(int param_1,undefined4 param_2,uint param_3)
{
  struct {
    HRESULT local_8;
    DWORD format;
  } s;
  
  if (param_3 & (byte)0x2) {
    if (g_sndRefCount == 0) {
      return 4;
    }
  }
  if (param_3 & 2) {
    if (g_sndRefCount != 0) {
      return 0;
    }
  }
  if (g_sndRefCount == 0) {
    if (param_1 != 0) {
      s.local_8 = DirectSoundCreate(0,&g_directSound,0);
      if (s.local_8 != 0) {
        return 4;
      }
      s.local_8 = g_directSound->lpVtbl->SetCooperativeLevel(g_directSound,param_1,3);
      if (s.local_8 != 0) {
        ReleaseSnd();
        return 4;
      }
      s.local_8 = g_directSound->lpVtbl->CreateSoundBuffer(g_directSound, &g_primaryDsDesc, &g_primaryDsBuffer, 0);
      if (s.local_8 != 0) {
        ReleaseSnd();
        return 4;
      }
      s.local_8 = g_primaryDsBuffer->lpVtbl->SetFormat(g_primaryDsBuffer, &g_primaryWaveFormat);
      if (s.local_8 != 0) {
        g_primaryDsBuffer->lpVtbl->GetFormat(g_primaryDsBuffer, &g_primaryWaveFormat, 0x12, &s.format);
      }
      g_sndHwnd = param_1;
      g_sndRefCount = g_sndRefCount + 1;
      InitializeCriticalSection(&g_sndCs);
    }
  }
  if ((param_3 & 1) != 0) {
    if (g_updateTimerActive != 0) {
      StopUpdateTimer();
    }
    g_updateTimerEnabled = 0;
  }
  return 0;
}

// FUNCTION: MAGSND 0x10001176
void ReleaseSnd(void)
{
  if (g_sndRefCount == 0)
    return;

  UnloadAllSnds();
  if (g_updateTimerActive != 0) {
    StopUpdateTimer();
  }
  g_directSound->lpVtbl->Release(g_directSound);
  g_directSound = (int *)0x0;
  g_sndHwnd = 0;
  g_sndRefCount = 0;
  DeleteCriticalSection(&g_sndCs);
  return;
}

// FUNCTION: MAGSND 0x100011E8
undefined4 GetSndHWND(void)

{
                    /* 0x11e8  25  GetSndHWND */
  return g_sndHwnd;
}

// FUNCTION: MAGSND 0x100011FD
int __cdecl LoadSnd(LPSTR path,int slot,Sound *sound)
{
  int result;

  if ((sound != (Sound *)0) && ((sound->field_1C >> 4 & 1U) != 0) && (slot != 0)) {
    if (0x10f < slot || slot < 0x100) {
      return 5;
    }
    if (g_sndSlots[slot] != (SndInstance *)0x0) {
      return 0;
    } 
  }
  else {
   if (0x100 < slot || slot < 0) {
      return 5;
    }
    if (g_sndSlots[slot] != (SndInstance *)0x0) {
      return 0;
    }
  }
  if ((sound != (Sound *)0) && ((sound->field_1C >> 2 & 1U) != 0)) {   
    if ((sound->field_1C >> 4 & 1U) != 0) {
      result = LoadAviAudioStream((void *)path,(&g_sndSlots[slot]),0);
      if (result != 0) {
        g_sndSlots[slot] = (SndInstance *)0;
        return result;
      }
      if ((sound->field_1C >> 5 & 1U) != 0) {
        g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 0x40;
      }
      else {
        ((undefined4 (__cdecl *)(undefined4 *,int))PrimeAviAudio)((undefined4 *)g_sndSlots[slot],0);
        g_sndSlots[slot]->flags2 &= ~0x40u;
      }
      g_sndLoadSerial += 1;
    }
    else {
      result = LoadWaveMmio(path,(&g_sndSlots[slot]),0);
      if (result != 0) {
        g_sndSlots[slot] = (SndInstance *)0;
        return result;
      }
      PrimeDsBufferFromSource(g_sndSlots[slot],0);
    }
    g_sndSlots[slot]->flags = (int)g_sndSlots[slot]->flags | 0x20;
    AddToUpdateList(g_sndSlots[slot]);
    g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 2;
  }
  else {
     result = LoadFromFile(path,(&g_sndSlots[slot]));
    if (result != 0) {
      g_sndSlots[slot] = (SndInstance *)0;
      return result;
    }
  }
  
  EnterCriticalSection(&g_sndCs);
  AddToActiveList(g_sndSlots[slot]);
  g_sndSlots[slot]->slotIndex = slot;
  if ((sound != (Sound *)0) && (sound->field_18 != 0)) {
    g_sndSlots[slot]->loadId = sound->field_18;
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x100014BA
undefined4 __cdecl UnloadSnd(int slot)
{
  uint i;

  if ((slot > 0x10f) || (0 > slot)) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  FUN_100026d1(slot);
  RemoveFromActiveList(g_sndSlots[slot]);
  if (((g_sndSlots[slot]->flags2 >> 1) & 1) != 0) {
    RemoveFromUpdateList(g_sndSlots[slot]);
    if (((g_sndSlots[slot]->flags2 >> 5) & 1) == 0) {
      CloseMmioAndFreeSource(g_sndSlots[slot]);
      for (i = 0; i < g_sndSlots[slot]->markerCount; i = i + 1) {
        if (g_sndSlots[slot]->markers[i + 1] != (void *)0x0) {
          CloseMmioAndFreeSource(g_sndSlots[slot]->markers[i + 1]);
        }
      }
    }
  }
  DestroySndInstance(g_sndSlots[slot]);
  g_sndSlots[slot] = (SndInstance *)0;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x1000161D
undefined4 UnloadAllSnds(void)
{
  EnterCriticalSection(&g_sndCs);
  while (g_activeSndListHead != (SndInstance *)0x0) {
    UnloadSnd(g_activeSndListHead->slotIndex);
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10001668
undefined4 __cdecl PlaySnd(int slot,Sound *sound)
{
  int unused;
  if (0x10f < slot || slot < 0) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  unused = FUN_10001701(g_sndSlots[slot],(int *)sound);
  g_sndSlots[slot]->lruCounter++;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10001701
int __cdecl FUN_10001701(SndInstance *snd,int *playConfig)

{
  struct {
    int freqValue;
    LPDIRECTSOUNDBUFFER dsBuffer;
    int sampleRate;
    int panValue;
    uint playFlags;
    int result;
  } s;

  s.dsBuffer = snd->dsBuffer;
  s.freqValue = 0;
  s.sampleRate = 0;
  s.panValue = 0;
  s.playFlags = 0;
  s.result = 0;
  if (((snd->flags2 >> 1) & 1) != 0) {
    if ((snd->flags & 1) != 0) {
      FUN_100026d1(snd->slotIndex);
    }
    if ((g_updateTimerEnabled == 1) && (g_updateTimerActive == 0)) {
      s.result = StartUpdateTimer(g_updateResolutionMs);
      if (s.result != 0) {
        UnloadSnd(snd->slotIndex);
        return s.result;
      }
    }
    if (g_updateTimerEnabled != 0) {
      g_updateTimerUsers = g_updateTimerUsers + 1;
    }
    s.playFlags |= 1;
    if (((snd->flags >> 5) & 1) == 0) {
      s.result = FUN_10003656(snd);
      if (s.result != 0) {
        UnloadSnd(snd->slotIndex);
        return s.result;
      }
    }
  }
  else {
    if ((playConfig == (int *)0x0) || (((uint)playConfig[7] >> 1 & 1) == 0)) {
      s.result = AcquireFreeDsBuffer((int)snd,(int *)&s.dsBuffer);
      if (s.result != 0) {
        return s.result;
      }
      snd->dsBuffer = s.dsBuffer;
    }
    else {
      s.dsBuffer = snd->dsBuffer;
    }
  }
  if (playConfig == (int *)0x0) {
    s.freqValue = 0;
    s.sampleRate = snd->waveFmt.nSamplesPerSec;
    s.panValue = 0;
    snd->volume = 400;
    snd->playbackRateHz = s.sampleRate;
    snd->pan = 0;
  }
  else {
    int targetVolume;

    targetVolume = *playConfig;
    if (targetVolume > 400) {
      targetVolume = 400;
    }
    snd->volume = targetVolume;
    s.freqValue = (targetVolume * 5 + -2000) * 2;
    if (playConfig[1] == 0) {
      s.sampleRate = snd->waveFmt.nSamplesPerSec;
    }
    else {
      s.sampleRate = playConfig[1];
    }
    snd->playbackRateHz = s.sampleRate;
    if (playConfig[2] == 0) {
      s.panValue = 0;
    }
    else {
      s.panValue = playConfig[2];
    }
    snd->pan = s.panValue;
    s.panValue = s.panValue * 10;
    if ((*(byte *)(playConfig + 7) & 1) != 0) {
      s.playFlags |= 1;
      snd->flags2 |= 1;
    }
    if (((uint)playConfig[7] >> 3 & 1) != 0) {
      snd->flags2 |= 4;
    }
  }
  s.dsBuffer->lpVtbl->SetVolume(s.dsBuffer,s.freqValue);
  s.dsBuffer->lpVtbl->SetFrequency(s.dsBuffer,s.sampleRate);
  s.dsBuffer->lpVtbl->SetPan(s.dsBuffer,s.panValue);
  s.dsBuffer->lpVtbl->SetCurrentPosition(s.dsBuffer,0);
  s.result = s.dsBuffer->lpVtbl->Play(s.dsBuffer,0,0,s.playFlags);
  if (s.result == 0) {
    snd->flags |= 1;
    snd->flags &= 0xffffffdf;
    return 0;
  }
  return 9;
}

// FUNCTION: MAGSND 0x10001A1F
int __cdecl PlaySndFile(LPSTR filename,int slot,Sound *param_3)

{
  struct {
    undefined1 local_1c[4];
    int volume;
    int pan;
    int nSamplesPerSec;
    int local_c;
    undefined4 local_8;
  } s;

  s.local_8 = 0;
  s.volume = 0;
  s.nSamplesPerSec = 0;
  if ((0xff < slot) || (slot < 0)) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if ((int)g_sndSlots[slot] != 0) {
    LeaveCriticalSection(&g_sndCs);
    return 2;
  }
  s.local_c = LoadWaveMmio(filename,(&g_sndSlots[slot]),s.local_8);
  if (s.local_c != 0) {
    LeaveCriticalSection(&g_sndCs);
    return s.local_c;
  }
  AddToActiveList(g_sndSlots[slot]);
  AddToUpdateList(g_sndSlots[slot]);
  if ((g_updateTimerEnabled == 1) && (g_updateTimerActive == 0)) {
    s.local_c = StartUpdateTimer(g_updateResolutionMs);
    if (s.local_c != 0) {
      UnloadSnd(slot);
      LeaveCriticalSection(&g_sndCs);
      return s.local_c;
    }
  }
  g_sndSlots[slot]->slotIndex = slot;
  if (param_3 != (Sound *)0) {
    s.volume = param_3->field_0;
    g_sndSlots[slot]->volume = s.volume;
    if (400 < s.volume) {
      s.volume = 400;
    }
    s.volume = (s.volume * 5 - 2000) * 2;
    if (param_3->field_4 != 0) {
      s.nSamplesPerSec = param_3->field_4;
    }
    else {
      s.nSamplesPerSec = (int)g_sndSlots[slot]->waveFmt.nSamplesPerSec;
    }
    g_sndSlots[slot]->playbackRateHz = s.nSamplesPerSec;

    if (param_3->field_8 != 0) {
      s.pan = param_3->field_8;      
    } else {
      s.pan = 0;
    }
    g_sndSlots[slot]->pan = s.pan;
    s.pan = s.pan * 10;
    if (param_3->field_1C & 1) {
      g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 1;
    }
    if (((uint)((int *)param_3)[7] >> 3 & 1) != 0) {
      g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 4;
    }
  }
  else {
    s.volume = 0;
    g_sndSlots[slot]->volume = 400;
    s.nSamplesPerSec = (int)g_sndSlots[slot]->waveFmt.nSamplesPerSec;
    g_sndSlots[slot]->playbackRateHz = s.nSamplesPerSec;
    s.pan = 0;
    g_sndSlots[slot]->pan = s.pan;
  }
  g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 2;

  g_sndSlots[slot]->dsBuffer->lpVtbl->SetVolume(g_sndSlots[slot]->dsBuffer, s.volume);
  g_sndSlots[slot]->volume = s.volume;

  g_sndSlots[slot]->dsBuffer->lpVtbl->SetFrequency(g_sndSlots[slot]->dsBuffer,s.nSamplesPerSec);
  g_sndSlots[slot]->playbackRateHz = s.nSamplesPerSec;

  g_sndSlots[slot]->dsBuffer->lpVtbl->SetPan(g_sndSlots[slot]->dsBuffer,s.pan);
  g_sndSlots[slot]->pan = s.pan;

  PrimeDsBufferFromSource(g_sndSlots[slot],0);
  
  g_sndSlots[slot]->dsBuffer->lpVtbl->Play(g_sndSlots[slot]->dsBuffer,0,0,1);
  g_sndSlots[slot]->dsBuffer->lpVtbl->GetCurrentPosition(g_sndSlots[slot]->dsBuffer,(int)&g_sndSlots[slot]->ringCursorBytes,s.local_1c);
  g_sndSlots[slot]->flags = (int)g_sndSlots[slot]->flags | 1;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10001E52
int __cdecl LoadMarkerInstance(SndInstance *parent,int markerIndex)
{
  struct {
    SndInstance **markerSlot;
    int markerMetaIndex;
    char *markerMeta;
    int result;
    int markerStartSample;
  } s;

  s.result = 0;
  s.markerStartSample= 0;
  s.markerMetaIndex = 0;

  s.markerMeta = &parent->markers[markerIndex * 3 - 3];
  s.markerSlot = &parent->markers[markerIndex - 1] + 1;
  s.result = LoadWaveMmio((LPSTR)parent->streamOrMmio,s.markerSlot,0x10);
  if (s.result != 0) {
    return s.result;
  }
  
  s.markerStartSample = *(int *)(s.markerMeta + 0x14);
  s.result = PrimeDsBufferFromSource(*s.markerSlot,s.markerStartSample);
  if (s.result != 0) {
    return s.result;
  }

  (*s.markerSlot)->flags = (int)(*s.markerSlot)->flags | 0x20;
  AddToUpdateList(*s.markerSlot);
  (*s.markerSlot)->flags2 = (int)(*s.markerSlot)->flags2 | 2;
  (*s.markerSlot)->flags2 = (int)(*s.markerSlot)->flags2 | 0x10;
  (*s.markerSlot)->flags2 = (parent->flags2 & 1) | ((*s.markerSlot)->flags2 & 0xfffffffe);
  (*s.markerSlot)->slotIndex = parent->slotIndex;
  return s.result;
}

// FUNCTION: MAGSND 0x10001F6F
int __cdecl PrimeMarkerInstance(SndInstance *parent,int markerIndex)

{
  struct {
    int markerMetaIndex;
    SndInstance *markerInstance;
    char *markerMeta;
    int markerStartSample;
    int result;
  } s;

  s.markerStartSample = 0;
  s.result = 0;
  s.markerMetaIndex = 0;

  if (((parent->flags >> 6) & 1) != 0) {
    if (parent->activeMarkerIndex == markerIndex - 1) {
      return 0xd;
    }
  }
 
  s.markerMeta = (char *)parent->markers[0xd] + (markerIndex * 3 - 3) * 8;
  s.markerInstance = parent->markers[markerIndex];
  s.markerStartSample = *(int *)(s.markerMeta + 0x14);
  s.result = PrimeDsBufferFromSource(s.markerInstance,s.markerStartSample);
  if (s.result != 0) {
    return s.result;
  }

  s.markerInstance->flags |= 0x20;
  s.markerInstance->flags &= 0xfffffffb;
  s.markerInstance->flags &= 0xffffffef;
  s.markerInstance->ringCursorBytes = 0;
  s.markerInstance->flags2 = (parent->flags2 & 1) | (s.markerInstance->flags2 & 0xfffffffe);
  s.markerInstance->volume = parent->volume;
  s.markerInstance->playbackRateHz = parent->playbackRateHz;
  s.markerInstance->pan = parent->pan;
  return 0;
}

// FUNCTION: MAGSND 0x1000208C
void __cdecl SwapActiveInstances(int *activeInstanceSlot,int *newInstanceSlot)

{
  struct {
    int i;
    int old;
  } s;

  s.old = *activeInstanceSlot;
  *activeInstanceSlot = *newInstanceSlot;
  s.i = 0;
  for (; s.i < 0x10; s.i = s.i + 1) {
    if (*(int *)(s.old + 0x38 + s.i * 4) == *activeInstanceSlot) {
      *(int *)(*activeInstanceSlot + 0x38 + s.i * 4) = s.old;
    }
    else {
      *(int *)(*activeInstanceSlot + 0x38 + s.i * 4) = *(int *)(s.old + 0x38 + s.i * 4);
    }
    *(undefined4 *)(s.old + 0x38 + s.i * 4) = 0;
  }
  *(int *)(*activeInstanceSlot + 0x10) = *(int *)(s.old + 0x10);
  *(uint *)(s.old + 8) = *(uint *)(s.old + 8) | 0x10;
  *(uint *)(*activeInstanceSlot + 8) = *(uint *)(*activeInstanceSlot + 8) & 0xffffffef;
  RemoveFromActiveList((SndInstance *)s.old);
  AddToActiveList((SndInstance *)*activeInstanceSlot);
  return;
}

// FUNCTION: MAGSND 0x1000215C
int __cdecl SetSndMarker(int slot,uint markerIndex)

{
  struct {
    SndInstance *snd;
    int result;
  } local;


                    /* 0x215c  18  SetSndMarker */
  if ((slot > 0xff) || (slot < 0)) {
    return 5;
  }

  EnterCriticalSection(&g_sndCs);
  local.snd = g_sndSlots[slot];
  if (local.snd == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if ((local.snd->markerCount == 0) || (local.snd->markerCount <= markerIndex)) {
    LeaveCriticalSection(&g_sndCs);
    return 5;
  }
  if (local.snd->markers[markerIndex] != (void *)0x0) {
    local.result = PrimeMarkerInstance(local.snd,markerIndex);
  }
  else {
    local.result = LoadMarkerInstance(local.snd,markerIndex);
  }
  LeaveCriticalSection(&g_sndCs);
  return local.result;
}

// FUNCTION: MAGSND 0x10002249
int __cdecl PlaySndMarker(int slot,uint markerIndex)

{
  int result;
  SndInstance *snd;
  SndInstance *marker;
  int volume;

  if ((slot > 0xff) || (slot < 0)) {
    return 5;
  }
  if (((int)markerIndex > 0x10) || ((int)markerIndex < 0)) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  snd = g_sndSlots[slot];
  if (snd == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if ((snd->markerCount == 0) || (snd->markerCount <= markerIndex)) {
    LeaveCriticalSection(&g_sndCs);
    return 5;
  }
  marker = snd->markers[markerIndex];
  if (marker == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if (((marker->flags2 >> 5) & 1) == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }

  if ((snd->flags & 1) != 0) {
    FUN_100026d1(slot);
  }

  volume = snd->volume;
  marker->volume = volume;
  marker->playbackRateHz = snd->playbackRateHz;
  marker->pan = snd->pan;

  result = FUN_10001701(marker,&volume);
  if (result == 0) {
    snd->flags |= 0x40;
    snd->activeMarkerIndex = markerIndex - 1;
    snd->flags |= 1;
  }
  LeaveCriticalSection(&g_sndCs);
  return result;
}

// FUNCTION: MAGSND 0x1000246E
undefined4 __cdecl StopSnd(int slot)

{
  struct {
    int status;
    int markerIndex;
    SndInstance *marker;
    int *dupEntry;
    uint i;
  } s;
  
                     /* 0x246e  8  StopSnd */
  if ((0x10f < slot) || (slot < 0)) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if (((g_sndSlots[slot]->flags >> 6) & 1) != 0) {
    s.markerIndex = g_sndSlots[slot]->activeMarkerIndex;
    s.marker = g_sndSlots[slot]->markers[s.markerIndex + 1];
    if (s.marker == (SndInstance *)0x0) {
      LeaveCriticalSection(&g_sndCs);
      return 1;
    }
    s.marker->flags |= 2;
  }
  else {
    if (((g_sndSlots[slot]->flags2 >> 1) & 1) != 0) {
      g_sndSlots[slot]->flags |= 2;
    }
    else {
      s.status = (**(code **)(**(int **)((int)g_sndSlots[slot] + 0xbc) + 0x48))
                       (*(undefined4 *)((int)g_sndSlots[slot] + 0xbc));
      if (s.status != 0) {
        LeaveCriticalSection(&g_sndCs);
        return 9;
      }
    }
  }
  if (((g_sndSlots[slot]->flags2 >> 1) & 1) == 0) {
    for (s.i = 0; s.i < 0x10; s.i = s.i + 1) {
      s.dupEntry = (int *)((int)g_sndSlots[slot] + s.i * 0xc + 0xc0);
      if (*s.dupEntry == 0) {
        break;
      }
      s.status = (**(code **)(*(int *)*s.dupEntry + 0x48))(*s.dupEntry);
      if (s.status != 0) {
        LeaveCriticalSection(&g_sndCs);
        return 9;
      }
    }
  }
  else if (g_updateTimerUsers > 0) {
    g_updateTimerUsers = g_updateTimerUsers - 1;
    if (g_updateTimerUsers == 0) {
      StopUpdateTimer();
    }
  }
  g_sndSlots[slot]->flags &= ~1;
  g_sndSlots[slot]->flags &= ~2;
  g_sndSlots[slot]->flags |= 4;
  g_sndSlots[slot]->flags &= ~0x20;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10002676
void StopAllSnds(void)
{
  SndInstance *node;
  EnterCriticalSection(&g_sndCs);
  node = g_activeSndListHead;
  while (node != (SndInstance *)0x0) {
    StopSnd(node->slotIndex);
    node = node->activeNext;
  }
  LeaveCriticalSection(&g_sndCs);
  return;
}

// FUNCTION: MAGSND 0x100026D1
undefined4 __cdecl FUN_100026d1(int slot)
{ 
  struct {
    int status;
    LPLPDIRECTSOUNDBUFFER subPtr;
    int i;
    SndInstance *markerInstance;
    int markerIndex;
  } s;

  if (slot > 0x10f || slot < 0) {
    return 5;
  }
  if (g_sndSlots[slot] == (SndInstance *)0x0) {
    return 1;
  }
  if (((g_sndSlots[slot]->flags >> 6) & 1) != 0) {
    s.markerIndex = g_sndSlots[slot]->activeMarkerIndex;
    s.markerInstance = g_sndSlots[slot]->markers[s.markerIndex + 1];
    if (s.markerInstance == 0) {
      return 1;
    }

    s.status = s.markerInstance->dsBuffer->lpVtbl->Stop(s.markerInstance->dsBuffer);
    if (s.status != 0) {
      return 9;
    }
    g_sndSlots[slot]->flags &= 0xffffffbf;
    s.markerInstance->flags &=0xfffffffe;
    s.markerInstance->flags &= 0xfffffffd;
    s.markerInstance->flags = (int)s.markerInstance->flags | 4;
    s.markerInstance->flags &= 0xffffffdf;
  }
  else {
    s.status = g_sndSlots[slot]->dsBuffer->lpVtbl->Stop(g_sndSlots[slot]->dsBuffer);
    if (s.status != 0) {
      return 9;
    }
  }
  if (((g_sndSlots[slot]->flags2 >> 1) & 1) == 0) {
    for (s.i = 0; s.i < 0x10; s.i = s.i + 1) {
      s.subPtr = &g_sndSlots[slot]->duplicateBuffers[s.i].buffer;
      if ((*s.subPtr)!= 0x0) {        
        s.status = (*s.subPtr)->lpVtbl->Stop(*s.subPtr);
        if (s.status != 0) {
          return 9;
        }
      }
      else
        break;
    }
  }
  else if (g_updateTimerUsers > 0) {
    g_updateTimerUsers = g_updateTimerUsers - 1;
    if (g_updateTimerUsers == 0) {
      StopUpdateTimer();
    }
  }
  g_sndSlots[slot]->flags &= 0xfffffffe;
  g_sndSlots[slot]->flags &= 0xfffffffd;
  g_sndSlots[slot]->flags = (int)g_sndSlots[slot]->flags | 4;
  g_sndSlots[slot]->flags &= 0xffffffdf;
  return 0;
}

// FUNCTION: MAGSND 0x1000290C
undefined4 PlayMidiFile(void)

{
                    /* 0x290c  10  PlayMidiFile */
  return 0;
}

// FUNCTION: MAGSND 0x1000291E
undefined4 __cdecl SetPitch(int slot,undefined4 pitchHz)

{
  struct {
    LPLPDIRECTSOUNDBUFFER duplicatePtr;
    int index;
  } s;

                    /* 0x291e  11  SetPitch */
  if ((0x10f < slot) || (slot < 0)) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }

  g_sndSlots[slot]->dsBuffer->lpVtbl->SetFrequency(g_sndSlots[slot]->dsBuffer,pitchHz);
  for (s.index = 0; s.index < 0x10; s.index = s.index + 1) {
    s.duplicatePtr = &g_sndSlots[slot]->duplicateBuffers[s.index].buffer;
    if (*s.duplicatePtr == (LPDIRECTSOUNDBUFFER)0x0) {
      break;
    }
    (*s.duplicatePtr)->lpVtbl->SetFrequency(*s.duplicatePtr,pitchHz);
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
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
  struct {
    undefined4 result;
    int i;
    int markerInst;
    int markerIndex;
  } s;

                    /* 0x2a36  13  SetVol */
  s.result = 0;

  if (param_1 > 0x10f) {
    return 5;
  }
  if (param_1 < 0) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if ((int)g_sndSlots[param_1] == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if (400 < param_2) {
    param_2 = 400;
  }
  if (param_2 < 0) {
    param_2 = 0;
  }

  if ((*(uint *)((int)g_sndSlots[param_1] + 4) >> 6 & 1) != 0) {
    s.markerIndex = *(int *)((int)g_sndSlots[param_1] + 0x18);
    s.markerInst = *(int *)((int)g_sndSlots[param_1] + s.markerIndex * 4 + 0x38);
    if (s.markerInst == 0) {
      LeaveCriticalSection(&g_sndCs);
      return 1;
    }
    *(uint *)(s.markerInst + 0x1f0) = param_2;
    param_2 = (param_2 * 5 + -2000) * 2;
    (**(code **)(**(int **)(s.markerInst + 0xbc) + 0x3c))
              (*(undefined4 *)(s.markerInst + 0xbc),param_2);
  }
  else {
    *(uint *)((int)g_sndSlots[param_1] + 0x1f0) = param_2;
    *(uint *)((int)g_sndSlots[param_1] + 0x1f0) = param_2;
    param_2 = (param_2 * 5 + -2000) * 2;
    (**(code **)(**(int **)((int)g_sndSlots[param_1] + 0xbc) + 0x3c))
              (*(undefined4 *)((int)g_sndSlots[param_1] + 0xbc),param_2);
  }
  for (s.i = 0; s.i < 0x10; s.i = s.i + 1) {
    s.markerInst = *(int *)((int)g_sndSlots[param_1] + s.i * 0xc + 0xc0);
    if (s.markerInst == 0) {
      break;
    }
    (**(code **)(*(int *)s.markerInst + 0x3c))(s.markerInst,param_2);
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10002C32
undefined4 GetVol(void)

{
                    /* 0x2c32  14  GetVol */
  return 0;
}

// FUNCTION: MAGSND 0x10002C44
undefined4 __cdecl SetPan(int slot,int pan)

{
  struct {
    LPLPDIRECTSOUNDBUFFER slot;
    int index;
  } s;

  s.slot = 0;
  if ((0x10f < slot) || (slot < 0)) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }

  pan = (pan * 5) * 2;
  //pan = pan + pan;
  g_sndSlots[slot]->dsBuffer->lpVtbl->SetPan(g_sndSlots[slot]->dsBuffer,pan);
  for (s.index = 0; s.index < 0x10; s.index++) {
    s.slot = &g_sndSlots[slot]->duplicateBuffers[s.index].buffer;
    if (*s.slot != NULL) {
      (*s.slot)->lpVtbl->SetPan(*s.slot, pan);
    }
    else
      break;
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
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
    SndInstance *node;
    SndInstance *next;
  } s;
  
                     /* 0x2d67  17  UpdateSnd */
  if (DAT_1000706c != 0) {
    return 0xd;
  }

  EnterCriticalSection(&g_sndCs);
  s.node = g_updateSndHead;
  while (s.node != (SndInstance *)0x0) {
    if ((s.node->flags & 1) != 0) {
      if (((s.node->flags >> 1) & 1) != 0) {
        if ((uint)s.node->volume > 5) {
          s.node->volume = s.node->volume - 5;
        }
        else {
          s.node->volume = 0;
        }
        SetVol(s.node->slotIndex, s.node->volume);
      }
  
      if (((s.node->flags2 >> 5) & 1) != 0) {
        if (((s.node->flags2 >> 6) & 1) == 0) {
          FUN_10003d60((undefined4 *)s.node);
        }
      }
      else {
        FUN_10003748((int)(s.node));
      }
    }
  
    if (((s.node->flags >> 2) & 1) != 0) {
      if (((s.node->flags2 >> 2) & 1) != 0) {
        s.next = s.node->activeNext;
        UnloadSnd(s.node->slotIndex);
        s.node = s.next;
        continue;
      }
    }
    s.node = s.node->updateNext;
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10002EB5
undefined4 __cdecl GetSndTime(int param_1,uint *param_2)
{
  // TODO: this has QWORD alignment problems
  struct  {
    int64_t local_24;
    uint local_1c;
    SndInstance *local_18;
    uint local_14;
    double local_10;
    uint local_8;
    undefined1 local_4 [4];
  } s;

                     /* 0x2eb5  20  GetSndTime */
  if (0x10f < param_1 || param_1 < 0) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[param_1] == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  s.local_18 = g_sndSlots[param_1];
  s.local_18->dsBuffer->lpVtbl->GetCurrentPosition(s.local_18->dsBuffer,&s.local_1c,s.local_4);
  s.local_14 = s.local_18->timeBytes % s.local_18->dsDesc.dwBufferBytes;
  if (s.local_14 < s.local_1c) {
    s.local_18->timeBytes += (s.local_1c - s.local_14);
  }
  else {
    s.local_18->timeBytes += (s.local_18->dsDesc.dwBufferBytes - s.local_14);
    s.local_18->timeBytes += s.local_1c;
  }
  if (((s.local_18->flags2 >> 5) & 1) != 0) {
    s.local_8 = s.local_18->timeBytes / s.local_18->blockBytes;
    while (s.local_18->timeEventCount < s.local_8) {
      PostMessageA(g_sndHwnd,0x3bd,0,s.local_8);
      s.local_18->timeEventCount++;
    }
  }
  s.local_24 = s.local_18->timeBytes;
  s.local_10 = s.local_24;
  if (s.local_18->waveFmt.nAvgBytesPerSec == 0x15888) {
    *param_2 = (uint)(long)(s.local_10 / g_sndTimeDiv_88200);
  }
  else if (s.local_18->waveFmt.nAvgBytesPerSec == 0xac44) {
    *param_2 = (uint)(long)(s.local_10 / g_sndTimeDiv_44100);
  }
  else if (s.local_18->waveFmt.nAvgBytesPerSec == 0x5622) {
    *param_2 = (uint)(long)(s.local_10 / g_sndTimeDiv_22050);
  }
  else if (s.local_18->waveFmt.nAvgBytesPerSec == 0x2b11) {
    *param_2 = (uint)(long)(s.local_10 / g_sndTimeDiv_11025);
  }
  else {
    
    LeaveCriticalSection(&g_sndCs);
    return 8;
  }
  g_sndTimeSamples[g_sndTimeSampleIndex].prevTime = g_lastSndTime;
  g_sndTimeSamples[g_sndTimeSampleIndex].currTime = *param_2;
  g_sndTimeSampleIndex = g_sndTimeSampleIndex + 1;
  g_sndTimeSampleIndex = g_sndTimeSampleIndex & 0xff;
  if (*param_2 < g_lastSndTime) {
    LeaveCriticalSection(&g_sndCs);
    return 5;
  }
  g_lastSndTime = *param_2;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x1000314D
undefined4 __cdecl GetSndState(int param_1,undefined4 *param_2)

{
                     /* 0x314d  22  GetSndState */
  if ((0x10f < param_1) || (param_1 < 0)) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[param_1] == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if ((*(byte *)((int)g_sndSlots[param_1] + 4) & 1) != 0) {
    *param_2 = 1;
  }
  else if (((*(uint *)((int)g_sndSlots[param_1] + 4) >> 3) & 1) != 0) {
    *param_2 = 2;
  }
  else {
    *param_2 = 0;
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
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
  int activeNode;
  
                     /* 0x321f  26  IsSndLoaded */
  if (param_1 <= 0) {
    return 0;
  }
  EnterCriticalSection(&g_sndCs);
  activeNode = (int)g_activeSndListHead;
  while (activeNode != 0) {
    if (*(int *)(activeNode + 0x14) == param_1) {
      *param_2 = *(undefined4 *)(activeNode + 0x10);
      LeaveCriticalSection(&g_sndCs);
      return 1;
    }
    activeNode = *(int *)(activeNode + 0x200);
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x100032AD
undefined4 __cdecl GetLRUSnd(int *param_1,int param_2,int param_3)

{
  struct {
    int local_10;
    int local_c;
    uint local_8;
    int local_4;
  } s;
  
                    /* 0x32ad  27  GetLRUSnd */
  s.local_4 = 0;
  s.local_8 = -1;
  if (param_3 == 0) {
    s.local_c = 0;
    param_3 = 0xff;
  }
  else {
    if (param_3 <= param_2) {
      s.local_c = 0;
      param_3 = 0xff;
    }
    else {
      if (param_3 < 0x100) {
        s.local_c = param_2;
      }
      else {
        s.local_c = 0;
        param_3 = 0xff;
      }
    }
  }
  EnterCriticalSection(&g_sndCs);
  for (; s.local_c < param_3; s.local_c = s.local_c + 1) {
    if ((int)g_sndSlots[s.local_c] == 0) {
      s.local_10 = s.local_c;
      s.local_4 = 1;
      break;
    }
    if (*(uint *)((int)g_sndSlots[s.local_c] + 0xc) < s.local_8) {
      s.local_8 = *(uint *)((int)g_sndSlots[s.local_c] + 0xc);
      s.local_10 = *(int *)((int)g_sndSlots[s.local_c] + 0x10);
    }
  }
  *param_1 = s.local_10;
  LeaveCriticalSection(&g_sndCs);
  return s.local_4;
}

// FUNCTION: MAGSND 0x100033A2
void * __cdecl GetAVISndBuff(int param_1,uint param_2)

{
  struct {
    void *ptr1;
    int slot;
    undefined4 zero1;
    uint blockIndex;
    int hr;
    void *ptr2;
    uint bytes2;
    uint bytes1;
    int offset;
  } s;

                    /* 0x33a2  23  GetAVISndBuff */
  s.ptr1 = 0;
  s.ptr2 = 0;
  s.bytes1 = 0;
  s.bytes2 = 0;
  s.zero1 = 0;

  if (param_1 > 0x10f) {
    return 0;
  }
  if (param_1 < 0x100) {
    return 0;
  }
  EnterCriticalSection(&g_sndCs);
  s.slot = (int)g_sndSlots[param_1];
  if (s.slot == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }
  if (((*(uint *)(s.slot + 4) >> 7) & 1) != 0) {
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }

  s.blockIndex = param_2 % *(uint *)(s.slot + 0x94);
  s.offset = *(int *)(s.slot + 0x8c) * s.blockIndex;
  s.hr = (**(code **)(**(int **)(s.slot + 0xbc) + 0x2c))
               (*(undefined4 *)(s.slot + 0xbc),s.offset,*(undefined4 *)(s.slot + 0x8c),
                &s.ptr1,&s.bytes1,&s.ptr2,&s.bytes2,0);
  if (s.hr != 0) {
    return 0;
  }
  if (s.ptr2 != 0) {
    (**(code **)(**(int **)(s.slot + 0xbc) + 0x4c))
              (*(undefined4 *)(s.slot + 0xbc),s.ptr1,s.bytes1,s.ptr2,s.bytes2);
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }
  *(void **)(s.slot + 0xa4) = s.ptr1;
  *(uint *)(s.slot + 4) = *(uint *)(s.slot + 4) | 0x80;
  LeaveCriticalSection(&g_sndCs);
  return s.ptr1;
}

// FUNCTION: MAGSND 0x1000352C
undefined4 __cdecl ReleaseAVISndBuff(int param_1)

{
  struct {
    int local_18;
    int local_14;
    int local_10;
    int local_c;
    int local_8;
    int local_4;
  } s;

                    /* 0x352c  24  ReleaseAVISndBuff */
  s.local_18 = 0;
  s.local_c = 0;
  s.local_4 = 0;
  s.local_8 = 0;
  s.local_10 = 0;

  if ((param_1 > 0x10f) || (param_1 < 0x100)) {
    return 0;
  }

  EnterCriticalSection(&g_sndCs);
  s.local_14 = (int)g_sndSlots[param_1];
  if (s.local_14 == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }
  if (((*(uint *)(s.local_14 + 4) >> 7) & 1) == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 0xe;
  }
  if (*(int *)(s.local_14 + 0xa4) == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 0xf;
  }
  (**(code **)(**(int **)(s.local_14 + 0xbc) + 0x4c))
            (*(undefined4 *)(s.local_14 + 0xbc),*(undefined4 *)(s.local_14 + 0xa4),
             *(undefined4 *)(s.local_14 + 0x8c),0,0);
  *(uint *)(s.local_14 + 4) = *(uint *)(s.local_14 + 4) & 0xffffff7f;
  *(undefined4 *)(s.local_14 + 0xa4) = 0;
  LeaveCriticalSection(&g_sndCs);
  return 0;
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
  g_lastSndTime = 0;
  if (((uint)param_1[2] >> 5 & 1) == 0) {
    param_1[0x67] = param_1[0x68];
    mmioSetInfo((HMMIO)param_1[0x72],(LPCMMIOINFO)(param_1 + 0x60),0);
    iVar1 = PrimeDsBufferFromSource(param_1,0);
    if (iVar1 != 0) {
      return iVar1;
    }
  }
  else {
    PrimeAviAudio((undefined4 *)param_1);
  }
  param_1[1] = param_1[1] | 0x20;
  return 0;
}

// FUNCTION: MAGSND 0x10003748
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl FUN_10003748(SndInstance * snd)
{
  struct {
    int local_34;
    uint local_30;
    void *local_2c;
    uint local_28;
    uint local_24;
    uint local_20;
    uint local_1c;
    uint local_18;
    uint local_14;
    int local_10;
    void *local_c;
    size_t local_8;
    size_t local_4;
  } s;

  s.local_1c = 0;
  s.local_30 = 0;
  s.local_14 = 0;
  s.local_28 = 0;
  s.local_20 = 0;
  s.local_18 = 0;
  s.local_2c = (void *)0x0;
  s.local_c = (void *)0x0;
  s.local_4 = 0;
  s.local_8 = 0;
  s.local_24 = 0;

  EnterCriticalSection(&g_sndCs);
  snd->dsBuffer->lpVtbl->GetCurrentPosition(snd->dsBuffer,&s.local_30,&s.local_1c);
  snd->timeBytes += (s.local_30 - snd->timeBytes & 0xffff);
  if (snd->dataBytes < snd->timeBytes) {
    snd->timeBytes -= snd->dataBytes;
  }
  if ((((snd->flags >> 4) & 1) != 0 && (0x10000 <= snd->readCursorBytes)) ||
     (((snd->flags >> 1) & 1) != 0 && (snd->volume == 0))) {
    snd->dsBuffer->lpVtbl->Stop(snd->dsBuffer);
    if (g_updateTimerUsers > 0 && --g_updateTimerUsers == 0) {
      StopUpdateTimer();
    }
    snd->flags &= 0xfffffffe;
    snd->flags &= 0xfffffffd;
    snd->flags = (int)snd->flags | 4;
    snd->flags &= 0xffffffdf;
    if (((snd->flags2 >> 4) & 1) != 0) {
      g_sndSlots[snd->slotIndex]->flags &= 0xffffffbf;
      g_sndSlots[snd->slotIndex]->flags &= 0xfffffffe;
    }
    LeaveCriticalSection(&g_sndCs);
  }
  else {
    s.local_14 = (int)(s.local_30 - snd->ringCursorBytes & 0xffff);
    s.local_20 = snd->dataBytes - snd->writeCursorBytes;
    if (((snd->flags >> 4) & 1) == 0) {
      s.local_28 = snd->writeCursorBytes - snd->readCursorBytes;
      if (0x10000 < s.local_28) {
        _DAT_1000707c = _DAT_1000707c + 1;
      }
    }
    else {
      s.local_28 = 0;
    }
    s.local_18 = s.local_20 + s.local_28;
    if (s.local_18 == 0) {
      s.local_24 = s.local_14;
      s.local_14 = 0U;
    }
    if ((s.local_14 == 0) && (s.local_24 == 0)) {
      LeaveCriticalSection(&g_sndCs);
    }
    else {
      if (s.local_28 < s.local_14) {
        s.local_14 = s.local_28;
      }
      if (s.local_24 + s.local_14 != 0) {
        s.local_10 = snd->dsBuffer->lpVtbl->Lock(snd->dsBuffer,snd->ringCursorBytes,
                          s.local_24 + s.local_14,&s.local_2c,&s.local_4,&s.local_c,&s.local_8,0);
        if (s.local_10 != 0) {
          LeaveCriticalSection(&g_sndCs);
          return;
        }
        snd->flags = (int)snd->flags | 0x80;
        if (s.local_24 == 0) {
          mmioGetInfo(snd->hMmio,&snd->mmioInfo,0);
          memmove(s.local_2c,snd->mmioInfo.pchNext,s.local_4);
          snd->mmioInfo.pchNext += s.local_4;
          if (s.local_c != (void *)0x0) {
            memmove(s.local_c,snd->mmioInfo.pchNext,s.local_8);
            snd->mmioInfo.pchNext += s.local_8;
          }
          mmioSetInfo(snd->hMmio,&snd->mmioInfo,0);
          s.local_28 -= s.local_14;
          s.local_18 -= s.local_14;
          snd->readCursorBytes += s.local_14;
        }
        else {
          if (snd->waveFmt.wBitsPerSample == 8) {
            s.local_34 = 0x80;
          }
          else {
            s.local_34 = 0;
          }
          memset(s.local_2c,s.local_34,s.local_4);
          if (s.local_c != (void *)0x0) {
            memset(s.local_c,s.local_34,s.local_8);
          }
          snd->readCursorBytes += s.local_24;
        }
        snd->ringCursorBytes = (snd->ringCursorBytes + s.local_24 + s.local_14) & 0xffff;
        snd->dsBuffer->lpVtbl->Unlock(snd->dsBuffer,s.local_2c,s.local_4,s.local_c,s.local_8);
        snd->flags &= 0xffffff7f;
      }
      if (((s.local_18 != 0) && (s.local_20 > 0)) &&
         ((s.local_28 == 0 || (s.local_28 < s.local_14 * 2)))) {
        mmioAdvance(snd->hMmio,&snd->mmioInfo,0);
        if (s.local_20 < snd->mmioInfo.cchBuffer - s.local_28) {
          snd->writeCursorBytes += s.local_20;
        }
        else {
          snd->writeCursorBytes += snd->mmioInfo.cchBuffer - s.local_28;
        }
      }
      if (s.local_18 == 0) {
        if ((snd->flags2 & 1) != 0) {
          snd->mmioInfo.pchNext = snd->mmioInfo.pchEndRead;
          mmioSetInfo(snd->hMmio,&snd->mmioInfo,0);
          mmioSeek(snd->hMmio,snd->mmioBaseOffset,0);
          mmioGetInfo(snd->hMmio,&snd->mmioInfo,0);
          mmioAdvance(snd->hMmio,&snd->mmioInfo,0);
          snd->writeCursorBytes = (uint)(snd->mmioInfo.pchNext - snd->mmioInfo.pchBuffer);
          if (snd->dataBytes < snd->writeCursorBytes) {
            snd->writeCursorBytes = snd->dataBytes;
          }
          snd->readCursorBytes = 0;
         
        }
        else if (((snd->flags >> 4) & 1) == 0) {
          snd->ringCursorSavedBytes = snd->ringCursorBytes;
          snd->readCursorBytes = 0;
          snd->flags = (int)snd->flags | 0x10;
        }
      }
      LeaveCriticalSection(&g_sndCs);
      return;
    }
  }
}

// FUNCTION: MAGSND 0x10003D60
undefined4 __cdecl FUN_10003d60(undefined4 *param_1)
{
  struct {
    uint local_48;
    uint local_44;
    void *local_40;
    uint local_3c;
    uint local_38;
    uint local_34;
    uint local_30;
    uint local_2c;
    long local_28;
    uint local_24;
    uint local_20;
    uint local_1c;
    long local_18;
    uint local_14;
    int local_10;
    void *local_c;
    uint local_8;
    uint local_4;
  } s;

  s.local_24 = 0;
  s.local_44 = 0;
  s.local_3c = 0;
  s.local_34 = 0;
  s.local_20 = 0;
  s.local_40 = (void *)0x0;
  s.local_c = (void *)0x0;
  s.local_4 = 0;
  s.local_8 = 0;
  s.local_38 = 0;

  (**(code **)(*(int *)param_1[0x2f] + 0x10))(param_1[0x2f],&s.local_44,&s.local_24);

  s.local_30 = (uint)param_1[0x77] % (uint)param_1[0x2c];
  if (s.local_44 > s.local_30) {
    param_1[0x77] = param_1[0x77] + (s.local_44 - s.local_30);
  }
  else {
    param_1[0x77] = param_1[0x77] + ((uint)param_1[0x2c] - s.local_30);
    param_1[0x77] = param_1[0x77] + s.local_44;
  }

  s.local_2c = s.local_44 / (uint)param_1[0x23];
  s.local_1c = (uint)param_1[0x76] / (uint)param_1[0x23];
  if (s.local_2c == s.local_1c) {
    return 0;
  }

  if (((((uint)param_1[1] >> 4 & 1) != 0) && ((uint)param_1[0x2c] <= (uint)param_1[0x75])) ||
     ((((uint)param_1[1] >> 1 & 1) != 0) && (param_1[0x7c] == 0))) {
    (**(code **)(*(int *)param_1[0x2f] + 0x48))(param_1[0x2f]);
    if (g_updateTimerUsers > 0 && --g_updateTimerUsers == 0) {
      StopUpdateTimer();
    }
    param_1[1] = param_1[1] & 0xfffffffe;
    param_1[1] = param_1[1] & 0xfffffffd;
    param_1[1] = (int)param_1[1] | 4;
    param_1[1] = param_1[1] & 0xffffffdf;
    param_1[0x77] = 0;
    return 0;
  }

  s.local_14 = (uint)param_1[0x23];
  if (((uint)param_1[1] >> 4 & 1) == 0) {
    s.local_48 = (uint)param_1[0x74] / (uint)param_1[0x24];
    s.local_10 = (**(code **)(*(int *)param_1[0x2f] + 0x2c))
                         (param_1[0x2f],param_1[0x76],s.local_14,&s.local_40,&s.local_4,&s.local_c,
                          &s.local_8,0);
    if (s.local_10 != 0) {
      return 10;
    }
    AVIStreamRead((PAVISTREAM)*param_1,(LONG)s.local_48,(LONG)(s.local_4 / (uint)param_1[0x24]),
                  s.local_40,(LONG)s.local_4,&s.local_28,&s.local_18);
    if (s.local_c != (void *)0x0) {
      AVIStreamRead((PAVISTREAM)*param_1,(LONG)(s.local_18 + s.local_48),
                    (LONG)(s.local_8 / (uint)param_1[0x24]),s.local_c,(LONG)s.local_8,&s.local_28,
                    &s.local_18);
    }
    param_1[0x76] = param_1[0x76] + s.local_14;
    param_1[0x76] = (uint)param_1[0x76] % (uint)param_1[0x2c];
    param_1[0x74] = param_1[0x74] + s.local_14;
    param_1[0x75] = param_1[0x75] + s.local_14;
    if ((uint)param_1[0x73] <= (uint)param_1[0x74]) {
      param_1[1] = (int)param_1[1] | 0x10;
      param_1[0x75] = 0;
    }
    (**(code **)(*(int *)param_1[0x2f] + 0x4c))
              (param_1[0x2f],s.local_40,s.local_4,s.local_c,s.local_8);
    param_1[1] = param_1[1] & 0xffffff7f;
  }
  else {
    param_1[0x75] = param_1[0x75] + s.local_14;
  }

  return 0;
}

// FUNCTION: MAGSND 0x100040C9
undefined4 __cdecl AcquireFreeDsBuffer(int param_1,int *param_2)
{
  struct {
    uint status;
    undefined4 dupOut;
    int hr;
    int *buffer;
    int index;
    int *candidate;
  } s;

  s.buffer = *(int **)(param_1 + 0xbc);
  s.candidate = s.buffer;
  s.hr = (**(code **)(*s.buffer + 0x24))(s.buffer,&s.status);
  if (s.hr != 0) {
    return 9;
  }
  if ((s.status & 2) != 0) {
    return 9;
  }
  if ((s.status & 1) != 0) {
    for (s.index = 0; s.index < 0x10; s.index = s.index + 1) {
      s.buffer = *(int **)(param_1 + 0xc0 + s.index * 0xc);
      if (s.buffer != (int *)0x0) {
        s.hr = (**(code **)(*s.buffer + 0x24))(s.buffer,&s.status);
        if (s.hr != 0) {
          return 9;
        }
        if ((s.status & 1) != 0) {
          continue;
        }
        else {
          *param_2 = (int)s.buffer;
          return 0;
        }
      } else {
        s.hr = g_directSound->lpVtbl->DuplicateSoundBuffer(g_directSound,s.candidate,&s.dupOut);
        if (s.hr != 0) {
          return 9;
        } else {
          *(undefined4 *)(param_1 + 0xc0 + s.index * 0xc) = s.dupOut;
          *param_2 = *(int *)(param_1 + 0xc0 + s.index * 0xc);
          return 0;
        }
      }
    }
  } else {
    *param_2 = (int)s.candidate;
    return 0;
  }
  return 9;
}

// FUNCTION: MAGSND 0x10004239
void __cdecl AddToUpdateList(SndInstance *snd)

{
  if (g_updateSndHead == (SndInstance *)0x0) {
    g_updateSndHead = snd;
    g_updateSndTail = g_updateSndHead;
  }
  else {
    g_updateSndTail->updateNext = snd;
    snd->updatePrev = g_updateSndTail;
    g_updateSndTail = snd;
  }
  return;
}

// FUNCTION: MAGSND 0x10004292
void __cdecl RemoveFromUpdateList(SndInstance *snd)

{
  SndInstance *prev;
  SndInstance *next;
  
  prev = (SndInstance *)0x0;
  next = (SndInstance *)0x0;
  prev = snd->updatePrev;
  next = snd->updateNext;
  if (prev != (SndInstance *)0x0) {
    prev->updateNext = next;
  }
  else {
    g_updateSndHead = next;
  }
  if (next != (SndInstance *)0x0) {
    next->updatePrev = prev;
  }
  else {
    g_updateSndTail = prev;
  }
  return;
}

// FUNCTION: MAGSND 0x10004311
void __cdecl AddToActiveList(SndInstance *snd)

{
  if (g_activeSndListHead == (SndInstance *)0x0) {
    g_activeSndListHead = snd;
    g_activeSndTail = g_activeSndListHead;
  }
  else {
    g_activeSndTail->activeNext = snd;
    snd->activePrev = g_activeSndTail;
    g_activeSndTail = snd;
  }
  return;
}

// FUNCTION: MAGSND 0x1000436A
void __cdecl RemoveFromActiveList(SndInstance *snd)

{
  SndInstance *prev;
  SndInstance *next;
  
  prev = (SndInstance *)0x0;
  next = (SndInstance *)0x0;
  prev = snd->activePrev;
  next = snd->activeNext;
  if (prev != (SndInstance *)0x0) {
    prev->activeNext = next;
  }
  else {
    g_activeSndListHead = next;
  }
  if (next != (SndInstance *)0x0) {
    next->activePrev = prev;
  }
  else {
    g_activeSndTail = prev;
  }
  return;
}

// FUNCTION: MAGSND 0x100043E9
 void CALLBACK UpdateTimerProc(UINT u1, UINT u2, DWORD dw1, DWORD dw2, DWORD dw3)

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
  (void)resolutionMs;
  if (timeGetDevCaps(&g_timeCaps,8) != 0) {
    return 0xc;
  }
  timeBeginPeriod(g_updatePeriodMs);
  g_updateTimerId = timeSetEvent(g_updatePeriodMs,g_timeCaps.wPeriodMin,UpdateTimerProc,(DWORD)g_sndHwnd,1);
  if (g_updateTimerId == 0) {
    timeEndPeriod(g_updatePeriodMs);
    return 0xc;
  }
  g_updateTimerActive = 1;
  return 0;
}

// FUNCTION: MAGSND 0x1000448D
void StopUpdateTimer(void)

{
  if (g_updateTimerActive != 0) {
    EnterCriticalSection(&g_sndCs);
    timeKillEvent(g_updateTimerId);
    g_updateTimerId = 0;
    timeEndPeriod(g_updatePeriodMs);
    g_updateTimerActive = 0;
    LeaveCriticalSection(&g_sndCs);
  }
  return;
}

// FUNCTION: MAGSND 0x100044F0
undefined4 __cdecl LoadFromFile(char *filename,SndInstance **outSnd)

{
  FILE *file;
  undefined4 status;

  file = fopen(filename,g_fileModeRb);
  if (file == (FILE *)0x0) {
    return 7;
  }
  status = LoadWaveFromFileHandle(file,(int *)outSnd);
  fclose(file);
  return status;
}

// FUNCTION: MAGSND 0x1000454F
undefined4 __cdecl LoadWaveMmio(LPSTR filename,SndInstance **outSnd,undefined4 scratch)

{
  struct {
    undefined4 local_a0;
    undefined4 local_9c;
    MMCKINFO local_98;
    char local_84[16];
    undefined2 local_74;
    byte pad_72[6];
    MMIOINFO local_70;
    undefined4 local_28;
    DWORD local_24;
    LONG local_20;
    MMCKINFO local_1c;
    HMMIO local_8;
  } s;
  
  s.local_28 = 0;
  memset(&s.local_70,0,0x48);
  s.local_70.cchBuffer = 0x10000;
  s.local_8 = mmioOpenA(filename,&s.local_70,0x10000);
  if (s.local_8 == (HMMIO)0x0) {
    return 7;
  }
  s.local_1c.fccType = 0x45564157;
  if (mmioDescend(s.local_8,&s.local_1c,(MMCKINFO *)0x0,0x20) != 0) {
    mmioClose(s.local_8,0);
    return 8;
  }
  s.local_98.ckid = 0x20746d66;
  if (mmioDescend(s.local_8,&s.local_98,&s.local_1c,0x10) != 0) {
    mmioClose(s.local_8,0);
    return 8;
  }
  s.local_24 = s.local_98.cksize;
  if (mmioRead(s.local_8,s.local_84,s.local_98.cksize) != s.local_24) {
    mmioClose(s.local_8,0);
    return 8;
  }
  s.local_74 = 0;
  mmioAscend(s.local_8,&s.local_98,0);
  s.local_98.ckid = 0x61746164;
  if (mmioDescend(s.local_8,&s.local_98,&s.local_1c,0x10) != 0) {
    mmioClose(s.local_8,0);
    return 8;
  }
  mmioGetInfo(s.local_8,&s.local_70,0);
  mmioAdvance(s.local_8,&s.local_70,0);
  s.local_20 = s.local_70.lBufOffset;
  scratch = 0xe8;
  *outSnd = FUN_10005480(g_directSound,0x10000,(undefined4 *)s.local_84,0xe8);
  if (*outSnd == (SndInstance *)0x0) {
    mmioClose(s.local_8,0);
    return 9;
  }
  (*outSnd)->mmioBaseOffset = s.local_20;
  (*outSnd)->hMmio = s.local_8;
  *(MMIOINFO *)((byte *)*outSnd + 0x180) = s.local_70;
  (*outSnd)->dataBytes = s.local_98.cksize;
  (*outSnd)->streamOrMmio = operator_new(strlen(filename) + 1);
  if ((*outSnd)->streamOrMmio == (void *)0x0) {
    mmioClose(s.local_8,0);
    return 3;
  }
  strcpy((char *)(*outSnd)->streamOrMmio,filename);
  (*outSnd)->flags2 = (*outSnd)->flags2 & 0xffffffdf;
  (*outSnd)->flags2 = (*outSnd)->flags2 | 2;
  (*outSnd)->blockBytes = 0;
  (*outSnd)->blockScale = 0;
  (*outSnd)->blockCount = 0;
  mmioAscend(s.local_8,&s.local_98,0);
  s.local_98.ckid = 0x20657563;
  if (mmioDescend(s.local_8,&s.local_98,&s.local_1c,0x10) == 0) {
    *(MMCKINFO *)((byte *)*outSnd + 0x1c) = s.local_98;
    if (mmioRead(s.local_8,(HPSTR)((byte *)*outSnd + 0x30),4) != 4) {
      DestroySndInstance(*outSnd);
      mmioClose(s.local_8,0);
      return 8;
    }
    *(void **)((byte *)*outSnd + 0x34) =
         operator_new(*(int *)((byte *)*outSnd + 0x30) * 0x18);
    if (*(void **)((byte *)*outSnd + 0x34) == (void *)0x0) {
      DestroySndInstance(*outSnd);
      mmioClose(s.local_8,0);
      return 3;
    }
    s.local_24 = *(int *)((byte *)*outSnd + 0x30) * 0x18;
    if (mmioRead(s.local_8,*(HPSTR *)((byte *)*outSnd + 0x34),s.local_24) != (LONG)s.local_24) {
      DestroySndInstance(*outSnd);
      mmioClose(s.local_8,0);
      operator_delete(*(void **)((byte *)*outSnd + 0x34));
      return 8;
    }
  }
  return 0;
}

// FUNCTION: MAGSND 0x10004986
void __cdecl CloseMmioAndFreeSource(SndInstance *snd)

{
  mmioClose(snd->hMmio,0);
  if (snd->markers[0] != 0) {
    operator_delete(snd->markers[0]);
  }
  if (((snd->flags2 >> 5) & 1) == 0) {
    operator_delete(snd->streamOrMmio);
  }
  return;
}

// FUNCTION: MAGSND 0x100049FE
undefined4 __cdecl LoadWaveFromFileHandle(FILE *param_1,int *param_2)

{
  int createFlags;
  int bytesRead;
  int bytes1;
  int lockHr;
  int chunkHdr[2];
  int bytes2;
  void *ptr2;
  int waveTag;
  int riffSize;
  int riffTag;
  int riffEnd;
  int seek0;
  void *ptr1;
  undefined2 cbSize;
  undefined4 waveFmt[4];
  int tmp_50;
  int tmp_54;

  ptr1 = (void *)0x0;
  ptr2 = (void *)0x0;
  bytes1 = 0;
  bytes2 = 0;
  bytesRead = 0;
  createFlags = 0;
  riffEnd = 0;
  seek0 = 0;

  fseek(param_1,seek0,0);
  fread(&riffTag,1,0xc,param_1);
  riffEnd = riffSize + seek0 + 8;
  if ((riffTag != 0x46464952) || (waveTag != 0x45564157)) {
    return 8;
  }

  if (FindRiffChunk(param_1,0x20746d66,seek0,(uint)riffEnd) == 0) {
    return 8;
  }
  fread(chunkHdr,1,8,param_1);
  fread(waveFmt,1,0x10,param_1);
  cbSize = 0;
  if (FindRiffChunk(param_1,0x61746164,seek0,(uint)riffEnd) == 0) {
    return 8;
  }
  fread(chunkHdr,1,8,param_1);
  createFlags = 0xea;
  ptr1 = FUN_10005480(g_directSound,chunkHdr[1],waveFmt,createFlags);
  *param_2 = (int)ptr1;
  if (*param_2 == 0) {
    return 9;
  }

  lockHr = (**(code **)(**(int **)(*param_2 + 0xbc) + 0x2c))
                       (*(undefined4 *)(*param_2 + 0xbc),0,chunkHdr[1],&ptr1,&bytes1,&ptr2,&bytes2,
                        0);
  if (lockHr != 0) {
    DestroySndInstance((SndInstance *)*param_2);
    return 9;
  }
  bytesRead = fread(ptr1,1,chunkHdr[1],param_1);
  if (bytesRead != bytes1) {
    DestroySndInstance((SndInstance *)*param_2);
    return 9;
  }
  lockHr = (**(code **)(**(int **)(*param_2 + 0xbc) + 0x4c))
                       (*(undefined4 *)(*param_2 + 0xbc),ptr1,bytes1,ptr2,bytes2);
  if (lockHr != 0) {
    (**(code **)(**(int **)(*param_2 + 0xbc) + 8))(*(undefined4 *)(*param_2 + 0xbc));
    tmp_50 = *param_2;
    tmp_54 = tmp_50;
    operator_delete((void *)tmp_54);
    return 9;
  }
  return 0;
}

// FUNCTION: MAGSND 0x10004C8A
undefined4 __cdecl SeekMmioToSample(SndInstance *sndObj,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;

  mmioGetInfo(sndObj->hMmio,(LPMMIOINFO)&sndObj->mmioInfo,0);
  iVar1 = (int)(uint)sndObj->waveFmt.nBlockAlign;
  iVar2 = iVar1 * param_2 + sndObj->mmioBaseOffset;
  iVar3 = MmioTellFromInfo(&sndObj->mmioInfo);
  mmioSeek(sndObj->hMmio,iVar2 - iVar3,1);
  mmioGetInfo(sndObj->hMmio,(LPMMIOINFO)&sndObj->mmioInfo,0);
  mmioAdvance(sndObj->hMmio,(LPMMIOINFO)&sndObj->mmioInfo,0);
  return 0;
}

// FUNCTION: MAGSND 0x10004D3F
undefined4 __cdecl FindRiffChunk(FILE *param_1,int param_2,long param_3,uint param_4)

{
  struct {
    int chunk[2];
    int pos;
    int hdr[3];
  } s;

  fseek(param_1,param_3,0);
  fread(s.hdr,1,0xc,param_1);
  if (s.hdr[0] == param_2) {
    fseek(param_1,param_3,0);
    return 1;
  }
  while (1) {
    s.pos = ftell(param_1);
    fread(s.chunk,1,8,param_1);
    if (param_4 <= (uint)s.pos) {
      fseek(param_1,param_3,0);
      return 0;
    }
    if (s.chunk[0] == param_2) {
      fseek(param_1,s.pos,0);
      return 1;
    }
    s.pos = ftell(param_1);
    if ((s.chunk[1] & 1) != 0) {
      s.chunk[1] = s.chunk[1] + 1;
    }
    fseek(param_1,s.chunk[1],1);
  }
}

// FUNCTION: MAGSND 0x10004E4C
undefined4 __cdecl PrimeDsBufferFromSource(SndInstance *snd,int startSample)
{
  struct {
    int bytesRemainingInData;
    int samplesReadAdvance;
    int blockAlignCopy;
    HPSTR lockPtr1;
    int startOffsetBytes;
    int done;
    int streamSampleCount;
    int bytesRemainingToFill;
    HPSTR writePtr;
    int sourceTotalBytes;
    int wrapWindowBytes;
    int sourceOffsetBytes;
    HPSTR lockPtr2;
    int lockSize2;
    int lockHr;
    int bytesRead;
    int streamByteOffset;
    int lockSize1;
  } s;

  s.lockPtr1 = (HPSTR)0x0;
  s.lockPtr2 = (HPSTR)0x0;
  s.writePtr = (HPSTR)0x0;
  s.lockSize1 = 0;
  s.lockSize2 = 0;
  s.streamSampleCount = 0;
  s.done = 0;
  if (((snd->flags2 >> 5) & 1) == 0) {
    SeekMmioToSample(snd,startSample);
  }

  s.startOffsetBytes = (uint)snd->waveFmt.nBlockAlign * startSample;
  s.sourceOffsetBytes = s.startOffsetBytes;
  s.sourceTotalBytes = snd->dataBytes;
  s.bytesRemainingInData = s.sourceTotalBytes - s.sourceOffsetBytes;
  s.bytesRemainingToFill = 0x10000;
  s.samplesReadAdvance = 0;
  s.streamByteOffset = s.startOffsetBytes;
  
  if ((s.startOffsetBytes < 0) || (s.sourceTotalBytes < s.startOffsetBytes)) {
    return 5;
  }
  
  s.lockHr = snd->dsBuffer->lpVtbl->Lock(snd->dsBuffer,0,0x10000,&s.lockPtr1,&s.lockSize1,&s.lockPtr2,&s.lockSize2,0);
  if (s.lockHr != 0) {
    if (((snd->flags2 >> 5) & 1) != 0) {
      FUN_1000575e((undefined4 *)snd);
    }
    else {
      CloseMmioAndFreeSource(snd);
    }
    DestroySndInstance(snd);
    return 9;
  }

  s.writePtr = s.lockPtr1;
  while (s.done == 0) {
    if (s.bytesRemainingInData < s.bytesRemainingToFill) {
      if (((snd->flags2 >> 5) & 1) == 0) {
        s.bytesRead = mmioRead(snd->hMmio,s.writePtr,s.bytesRemainingInData);
      }
      else {
        s.blockAlignCopy = (int)snd->waveFmt.nBlockAlign;
        s.streamSampleCount = 0x10000 / s.blockAlignCopy;
        AVIStreamRead((PAVISTREAM)snd->streamOrMmio,startSample,s.streamSampleCount,s.writePtr,s.bytesRemainingInData,&s.bytesRead,0);
      }
      s.writePtr = s.writePtr + s.bytesRead;
      s.bytesRemainingToFill = s.bytesRemainingToFill - s.bytesRead;
      s.bytesRemainingInData = 0;
      s.wrapWindowBytes = snd->dataBytes;
      if ((snd->flags2 & 1) == 0) {
        memset(s.writePtr,0,(size_t)s.bytesRemainingToFill);
        s.done = 1;
      }
      else {
        if (((snd->flags2 >> 5) & 1) == 0) {
          SeekMmioToSample(snd,0);
        }
        s.wrapWindowBytes = 0;
        s.bytesRemainingInData = snd->dataBytes;
        s.streamByteOffset = 0;
      }
    }
    else {
      if (((snd->flags2 >> 5) & 1) == 0) {
        s.bytesRead = mmioRead(snd->hMmio,s.writePtr,s.bytesRemainingToFill);
      }
      else {
        s.blockAlignCopy = (int)snd->waveFmt.nBlockAlign;
        s.streamSampleCount = 0x10000 / s.blockAlignCopy;
        AVIStreamRead((PAVISTREAM)snd->streamOrMmio,startSample,s.streamSampleCount,s.writePtr,0x10000,&s.bytesRead,
                      &s.samplesReadAdvance);
      }
      s.bytesRemainingToFill = s.bytesRemainingToFill - s.bytesRead;
      s.streamByteOffset = s.streamByteOffset + s.bytesRead;
      s.bytesRemainingInData = s.bytesRemainingInData - s.bytesRead;
      s.wrapWindowBytes = s.wrapWindowBytes + s.bytesRead;
      startSample = startSample + s.samplesReadAdvance;
      s.done = 1;
    }
  }

  mmioGetInfo(snd->hMmio,&snd->mmioInfo,0);
  if (((snd->flags2 >> 5) & 1) == 0) {
    mmioAdvance(snd->hMmio,&snd->mmioInfo,0);
  }
  else {
    RefillAviMmioBuffer(snd,startSample);
  }

  s.wrapWindowBytes = s.bytesRemainingInData;
  if (0xffff < s.bytesRemainingInData) {
    s.wrapWindowBytes = 0x10000;
  }
  snd->writeCursorBytes = snd->dataBytes - (s.bytesRemainingInData - s.wrapWindowBytes);
  snd->readCursorBytes = snd->dataBytes - s.bytesRemainingInData;
  snd->ringCursorBytes = 0;
  
  s.lockHr = snd->dsBuffer->lpVtbl->Unlock(snd->dsBuffer,s.lockPtr1,s.lockSize1,s.lockPtr2,s.lockSize2);
  if (s.lockHr != 0) {
    CloseMmioAndFreeSource(snd);
    DestroySndInstance(snd);
    return 9;
  }
  return 0;
}

// FUNCTION: MAGSND 0x1000524C
undefined4 __cdecl PrimeAviAudio(undefined4 *param_1)

{
  struct {
    int local_30;
    uint local_2c;
    undefined1 local_28[4];
    uint local_24;
    int local_20;
    undefined1 local_1c[4];
    int local_18;
    int local_14;
    int local_10;
    int local_c;
    uint local_8;
    int local_4;
  } s;
  
  s.local_20 = param_1[0x25];
  s.local_20 = s.local_20 * param_1[0x23];
  s.local_4 = (**(code **)(*(int *)param_1[0x2f] + 0x2c))
                   (param_1[0x2f],0,s.local_20,&s.local_30,&s.local_8,&s.local_14,&s.local_10,0);
  if (s.local_4 != 0) {
    FUN_1000575e(param_1);
    DestroySndInstance((SndInstance *)param_1);
    return 9;
  }
  if (s.local_14 != 0) {
    (**(code **)(*(int *)param_1[0x2f] + 0x4c))
              (param_1[0x2f],s.local_30,s.local_8,s.local_14,s.local_10);
    FUN_1000575e(param_1);
    DestroySndInstance((SndInstance *)param_1);
    return 9;
  }
  param_1[0x76] = 0;
  param_1[0x74] = 0;
  param_1[0x75] = 0;
  s.local_18 = s.local_30;
  s.local_c = 0;
  s.local_24 = (uint)param_1[0x23] / (uint)param_1[0x24];
  s.local_4 = 0;
  for (s.local_2c = 0; s.local_2c < (uint)param_1[0x25]; s.local_2c = s.local_2c + 1) {
    AVIStreamRead((PAVISTREAM)*param_1,s.local_c,s.local_24,(void *)s.local_18,param_1[0x23],s.local_28,
                  s.local_1c);
    param_1[0x76] = param_1[0x76] + param_1[0x23];
    param_1[0x76] = (uint)param_1[0x76] % (uint)param_1[0x2c];
    param_1[0x74] = param_1[0x74] + param_1[0x23];
    param_1[0x75] = param_1[0x75] + param_1[0x23];
    s.local_18 = s.local_18 + param_1[0x23];
    s.local_c = s.local_c + s.local_24;
  }
  s.local_4 = (**(code **)(*(int *)param_1[0x2f] + 0x4c))
                   (param_1[0x2f],s.local_30,s.local_8,s.local_14,s.local_10);
  if (s.local_4 == 0) {
    return 0;
  }
  CloseMmioAndFreeSource((SndInstance *)param_1);
  DestroySndInstance((SndInstance *)param_1);
  return 9;
}

// FUNCTION: MAGSND 0x10005480
SndInstance * __cdecl FUN_10005480(IDirectSound *dSound,DWORD dwBufferBytes,WAVEFORMATEX *wfxFormat,DWORD dwFlags)
{
  struct {
    void *local_10;
    void *local_c;
    int local_8;
    SndInstance *result;
  } s;
  
  s.result = (SndInstance *)operator_new(0x204);
  memset(s.result,0,0x204);
  s.result->dsDesc.dwSize = 0x14;
  s.result->dsDesc.dwFlags = dwFlags;
  s.result->dsDesc.dwBufferBytes = dwBufferBytes;
  s.result->dsDesc.lpwfxFormat = wfxFormat;
  memcpy(&s.result->waveFmt, wfxFormat, sizeof(WAVEFORMATEX));

  s.local_8 = dSound->lpVtbl->CreateSoundBuffer(dSound,&s.result->dsDesc,&s.result->dsBuffer,0);
  if (s.local_8 != 0) {
    s.local_c = s.result;
    s.local_10 = s.local_c;
    operator_delete(s.local_10);
    return (SndInstance *)0x0;
  }
  else
    return s.result;
}

// FUNCTION: MAGSND 0x10005562
undefined4 __cdecl DestroySndInstance(SndInstance *param_1)
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
  struct {
    WAVEFORMATEX local_fc;
    byte pad_e6[2];
    MMIOINFO local_e8;
    LONG local_9c;
    int local_98;
    int local_94;
    int local_90;
    AVISTREAMINFOA local_8c;
  } s;
  
  s.local_90 = 0x12;
  AVIStreamInfoA((PAVISTREAM)stream,&s.local_8c,0x8c);
  if (s.local_8c.fccType != 0x73647561) {
    return 8;
  }
  AVIStreamRead((PAVISTREAM)stream,0,1,0,0,&s.local_9c,0);
  s.local_94 = (int)s.local_8c.dwSampleSize * (int)s.local_8c.dwLength;
  AVIStreamReadFormat((PAVISTREAM)stream,0,&s.local_fc,(LONG *)&s.local_90);
  s.local_fc.cbSize = 0;
  scratch = 0xe8;
  s.local_98 = (int)s.local_8c.dwSuggestedBufferSize * 0xb;
  *outSnd = FUN_10005480(g_directSound,s.local_98,&s.local_fc,scratch);
  if (*outSnd == (SndInstance *)0x0) {
    return 9;
  }
  (*outSnd)->mmioBaseOffset = 0;
  (*outSnd)->hMmio = (HMMIO)0;
  (*outSnd)->mmioInfo = s.local_e8;
  (*outSnd)->dataBytes = s.local_94;
  (*outSnd)->streamOrMmio = stream;
  (*outSnd)->flags2 = (*outSnd)->flags2 | 0x20;
  (*outSnd)->flags2 = (*outSnd)->flags2 | 2;
  (*outSnd)->blockBytes = s.local_8c.dwSuggestedBufferSize;
  (*outSnd)->blockScale = s.local_8c.dwSampleSize;
  (*outSnd)->blockCount = 0xb;
  (*outSnd)->timeBytes = 0;
  return 0;
}

// FUNCTION: MAGSND 0x10005770
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl RefillAviMmioBuffer(SndInstance *sndObj,long streamStartSample)

{
  int local_1c;
  long local_c;
  size_t local_8;

  local_c = 0;
  local_8 = (size_t)(sndObj->writeCursorBytes - sndObj->readCursorBytes);
  if (((int)sndObj->mmioInfo.pchEndRead - (int)sndObj->mmioInfo.pchNext) != (int)local_8) {
    g_mmioInfoMismatchCount = g_mmioInfoMismatchCount + 1;
  }
  local_1c = (int)sndObj->mmioInfo.cchBuffer - (int)local_8;
  if (((int)sndObj->dataBytes - (int)sndObj->writeCursorBytes) < local_1c) {
    local_1c = (int)sndObj->dataBytes - (int)sndObj->writeCursorBytes;
  }
  if (local_8 != 0) {
    memmove((void *)sndObj->mmioInfo.pchBuffer,(void *)sndObj->mmioInfo.pchNext,local_8);
  }
  sndObj->mmioInfo.pchNext = sndObj->mmioInfo.pchBuffer + local_8;
  AVIStreamRead((PAVISTREAM)sndObj->streamOrMmio,streamStartSample,
                local_1c / (int)(uint)sndObj->waveFmt.nBlockAlign,sndObj->mmioInfo.pchNext,local_1c,
                &local_c,0);
  sndObj->mmioInfo.pchEndRead = sndObj->mmioInfo.pchBuffer + sndObj->mmioInfo.cchBuffer;
  sndObj->mmioInfo.pchNext = sndObj->mmioInfo.pchBuffer;
  mmioSetInfo(sndObj->hMmio,&sndObj->mmioInfo,0);
  return;
}

// FUNCTION: MAGSND 0x100058D0
int __cdecl MmioTellFromInfo(MMIOINFO *info) { return (int)info->lDiskOffset - ((int)info->pchEndRead - (int)info->pchNext); }

// FUNCTION: MAGSND 0x1000575E
undefined4 __cdecl FUN_1000575e(undefined4 *param_1) { (void)param_1; return 0; }

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
