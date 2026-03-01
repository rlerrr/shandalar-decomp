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

//Why on earth is this a thing?
#define MAGSND_DELETE(toDelete) \
do { \
  void * tmp1 = toDelete; \
  void * tmp2 = tmp1; \
  operator_delete(tmp2); \
} while(false);

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
  MMCKINFO mmck;        /* 0x01c */
  uint markerCount;        /* 0x030 */
  SndInstance *markers[0x11];     /* 0x034 */
  WAVEFORMATEX waveFmt;   /* 0x078 (18 bytes) */
  unsigned short pad_08a;  /* 0x08a */
  uint blockBytes;          /* 0x08c */
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
  int sampleRate;          /* 0x1ec */
  int volume;                  /* 0x1f0 */
  SndInstance *updatePrev;     /* 0x1f4: used by AddToUpdateList/RemoveFromUpdateList */
  SndInstance *updateNext;     /* 0x1f8 */
  SndInstance *activePrev;     /* 0x1fc: used by AddToActiveList/RemoveFromActiveList */
  SndInstance *activeNext;     /* 0x200 */
};
STATIC_ASSERT(sizeof(SndInstance) == 0x204, SndInstance_wrong_size);

void __cdecl ReleaseSnd(void);
int __cdecl LoadSnd(LPSTR param_1, int slot, Sound *param_3);
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
undefined4 __cdecl PrimeAviAudio(SndInstance *snd, int unused);
undefined4 __cdecl LoadAviAudioStream(void *stream, SndInstance **outSnd, undefined4 scratch);
void __cdecl RefillAviMmioBuffer(SndInstance *sndObj, long streamStartSample);
int __cdecl MmioTellFromInfo(MMIOINFO *info);
undefined4 __cdecl LoadWaveFromFileHandle(FILE *file, SndInstance **outSnd);
undefined4 __cdecl FindRiffChunk(FILE *file,int riffType,long chunkOffset,uint chunkSize);
SndInstance * __cdecl CreateSndInstance(IDirectSound *dSound,DWORD dwBufferBytes,WAVEFORMATEX *wfxFormat,DWORD dwFlags);
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
DSBUFFERDESC g_primaryDsDesc = {20, 1, 0, 0, NULL};

// GLOBAL: MAGSND 0x10007058
WAVEFORMATEX g_primaryWaveFormat = {1, 2, 22050, 88200, 4, 16, 0};

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

// GLOBAL: MAGSND 0x100070A0
LPDIRECTSOUNDBUFFER g_primaryDsBuffer;

// GLOBAL: MAGSND 0x100084E8
HWND g_sndHwnd = 0x00000000;

// GLOBAL: MAGSND 0x100084F0
IDirectSound *g_directSound = NULL;

// GLOBAL: MAGSND 0x100084F4
undefined4 g_updateTimerId = 0x00000000;

// GLOBAL: MAGSND 0x100084F8
TIMECAPS g_timeCaps = {0};

// FUNCTION: MAGSND 0x10001000
undefined4 __cdecl InitSnd(HWND param_1,undefined4 param_2,uint param_3)
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
  g_directSound = NULL;
  g_sndHwnd = 0;
  g_sndRefCount = 0;
  DeleteCriticalSection(&g_sndCs);
  return;
}

// FUNCTION: MAGSND 0x100011E8
HWND GetSndHWND(void)
{
  return g_sndHwnd;
}

// FUNCTION: MAGSND 0x100011FD
int __cdecl LoadSnd(LPSTR path,int slot,Sound *sound)
{
  int result;

  if ((sound != (Sound *)0) && ((sound->flags >> 4 & 1U) != 0) && (slot != 0)) {
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
  if ((sound != (Sound *)0) && ((sound->flags >> 2 & 1U) != 0)) {   
    if ((sound->flags >> 4 & 1U) != 0) {
      result = LoadAviAudioStream((void *)path,(&g_sndSlots[slot]),0);
      if (result != 0) {
        g_sndSlots[slot] = (SndInstance *)0;
        return result;
      }
      if ((sound->flags >> 5 & 1U) != 0) {
        g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 0x40;
      }
      else {
        PrimeAviAudio(g_sndSlots[slot],0);
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
  if ((sound != (Sound *)0) && (sound->loadId != 0)) {
    g_sndSlots[slot]->loadId = sound->loadId;
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
  unused = FUN_10001701(g_sndSlots[slot], sound);
  g_sndSlots[slot]->lruCounter++;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

#define ONE 1

// FUNCTION: MAGSND 0x10001701
int __cdecl FUN_10001701(SndInstance *snd, Sound *sound)

{
  struct {
    int result2; // ebp - 0x1c
    int freqValue; // ebp - 0x18
    LPDIRECTSOUNDBUFFER dsBuffer; // ebp - 0x14
    uint playFlags; // ebp - 0x10
    int panValue; // ebp - 0xc
    int sampleRate; // ebp - 0x8
    int result; // ebp - 0x4
  } s;

  s.playFlags = 0;
  s.freqValue = 0;
  s.sampleRate = 0;
  if (((snd->flags2 >> ONE) & 1) != 0) {
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
    s.dsBuffer = snd->dsBuffer;
    if (((snd->flags >> 5) & 1) == 0) {
      s.result = FUN_10003656(snd);
      if (s.result != 0) {
        UnloadSnd(snd->slotIndex);
        return s.result;
      }
    }
  }
  else {
    if ((sound == NULL) || ((sound->flags >> 1 & 1) == 0)) {
      s.result = AcquireFreeDsBuffer((int)snd,(int *)&s.dsBuffer);
      if (s.result != 0) {
        return s.result;
      }
    }
    else {
      s.dsBuffer = snd->dsBuffer;
    }
  }
  if (sound != NULL) { 
    s.freqValue = sound->volume;
    if (s.freqValue > 400) {
      s.freqValue = 400;
    }
    snd->volume = s.freqValue;
    s.freqValue = (s.freqValue - 400) * 10;
    if (sound->sampleRate != 0) {
      s.sampleRate = sound->sampleRate;      
    }
    else {
      s.sampleRate = snd->waveFmt.nSamplesPerSec;
    }
    snd->sampleRate = s.sampleRate;
    if (sound->pan != 0) {
      s.panValue = sound->pan;
    }
    else {
      s.panValue = 0;
    }
    snd->pan = s.panValue;
    s.panValue = s.panValue * 10;
    if ((sound->flags & 1) != 0) {
      s.playFlags |= 1;
      snd->flags2 = (int)snd->flags2 | 1;
    }
    if ((sound->flags >> 3 & 1) != 0) {
      snd->flags2 = (int)snd->flags2 | 4;
    }
  }
  else {
    s.freqValue = 0;
    snd->volume = 400;
    s.sampleRate = snd->waveFmt.nSamplesPerSec;
    snd->sampleRate = s.sampleRate;
    s.panValue = 0;
    snd->pan = 0;
  }
  s.dsBuffer->lpVtbl->SetVolume(s.dsBuffer,s.freqValue);
  s.dsBuffer->lpVtbl->SetFrequency(s.dsBuffer,s.sampleRate);
  s.dsBuffer->lpVtbl->SetPan(s.dsBuffer,s.panValue);
  s.dsBuffer->lpVtbl->SetCurrentPosition(s.dsBuffer,0);
  if (s.dsBuffer->lpVtbl->Play(s.dsBuffer,0,0,s.playFlags)) {
    s.result2 = 1;
  } else {
    s.result2 = 0;
  }
  if (s.result2 != 0) {
    return 9;
  } 

  snd->flags = (int)snd->flags | 1;
  snd->flags &= 0xffffffdf;
  return 0;
}

// FUNCTION: MAGSND 0x10001A1F
int __cdecl PlaySndFile(LPSTR filename,int slot,Sound *sound)

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
  if (sound != (Sound *)0) {
    s.volume = sound->volume;
    g_sndSlots[slot]->volume = s.volume;
    if (400 < s.volume) {
      s.volume = 400;
    }
    s.volume = (s.volume - 400) * 10;
    if (sound->sampleRate != 0) {
      s.nSamplesPerSec = sound->sampleRate;
    }
    else {
      s.nSamplesPerSec = (int)g_sndSlots[slot]->waveFmt.nSamplesPerSec;
    }
    g_sndSlots[slot]->sampleRate = s.nSamplesPerSec;

    if (sound->pan != 0) {
      s.pan = sound->pan;      
    } else {
      s.pan = 0;
    }
    g_sndSlots[slot]->pan = s.pan;
    s.pan = s.pan * 10;
    if (sound->flags & 1) {
      g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 1;
    }
    if ((sound->flags >> 3 & 1) != 0) {
      g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 4;
    }
  }
  else {
    s.volume = 0;
    g_sndSlots[slot]->volume = 400;
    s.nSamplesPerSec = (int)g_sndSlots[slot]->waveFmt.nSamplesPerSec;
    g_sndSlots[slot]->sampleRate = s.nSamplesPerSec;
    s.pan = 0;
    g_sndSlots[slot]->pan = s.pan;
  }
  g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 2;

  g_sndSlots[slot]->dsBuffer->lpVtbl->SetVolume(g_sndSlots[slot]->dsBuffer, s.volume);
  g_sndSlots[slot]->volume = s.volume;

  g_sndSlots[slot]->dsBuffer->lpVtbl->SetFrequency(g_sndSlots[slot]->dsBuffer,s.nSamplesPerSec);
  g_sndSlots[slot]->sampleRate = s.nSamplesPerSec;

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
    SndInstance **markerMeta;
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
  
  s.markerStartSample = (*s.markerMeta)->loadId;
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
    SndInstance *markerMeta;
    int result;
    int markerStartSample;
  } s;

  s.result = 0;
  s.markerStartSample = 0;
  s.markerMetaIndex = 0;

  if (((parent->flags >> 6) & 1) != 0) {
    if (parent->activeMarkerIndex == markerIndex - 1) {
      return 0xd;
    }
  }
 
  s.markerMeta = parent->markers[(markerIndex - 1) * 6];
  s.markerInstance = parent->markers[markerIndex];
  s.markerStartSample = s.markerMeta->loadId;
  s.result = PrimeDsBufferFromSource(s.markerInstance,s.markerStartSample);
  if (s.result != 0) {
    return s.result;
  }

  s.markerInstance->flags = (int)s.markerInstance->flags | 0x20;
  s.markerInstance->flags &= 0xfffffffb;
  s.markerInstance->flags &= 0xffffffef;
  s.markerInstance->ringCursorBytes = 0;
  s.markerInstance->flags2 = (parent->flags2 & 1) | (s.markerInstance->flags2 & 0xfffffffe);
  s.markerInstance->volume = parent->volume;
  s.markerInstance->sampleRate = parent->sampleRate;
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
  *(uint *)(s.old + 8) = *(int *)(s.old + 8) | 0x10;
  *(uint *)(*activeInstanceSlot + 8) = *(uint *)(*activeInstanceSlot + 8) & 0xffffffef;
  RemoveFromActiveList((SndInstance *)s.old);
  AddToActiveList((SndInstance *)*activeInstanceSlot);
  return;
}

// FUNCTION: MAGSND 0x1000215C
int __cdecl SetSndMarker(int slot, uint markerIndex)
{
  struct {
    SndInstance *snd;
    int result;
  } s;

  s.result = 0;
  if ((slot > 0xff) || (slot < 0)) {
    return 5;
  }

  EnterCriticalSection(&g_sndCs);
  s.snd = g_sndSlots[slot];
  if (s.snd == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if ((s.snd->markerCount == 0) || (s.snd->markerCount < markerIndex)) {
    LeaveCriticalSection(&g_sndCs);
    return 5;
  }
  if (s.snd->markers[markerIndex] != (void *)0x0) {
    s.result = PrimeMarkerInstance(s.snd,markerIndex);
  }
  else {
    s.result = LoadMarkerInstance(s.snd,markerIndex);
  }
  LeaveCriticalSection(&g_sndCs);
  return s.result;
}

// FUNCTION: MAGSND 0x10002249
int __cdecl PlaySndMarker(int slot,uint markerIndex)
{
  struct {
    SndInstance *marker; // ebp - 0x2c
    SndInstance *snd; // ebp - 0x28
    int result; // ebp - 0x24
    Sound sound; // ebp - 0x20
  } s;

  s.result = 0;

  if ((slot > 0xff) || (slot < 0)) {
    return 5;
  }
  if (((int)markerIndex > 0x10) || ((int)markerIndex < 0)) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  s.snd = g_sndSlots[slot];
  if (s.snd == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if ((s.snd->markerCount == 0) || (s.snd->markerCount < markerIndex)) {
    LeaveCriticalSection(&g_sndCs);
    return 5;
  }
  s.marker = s.snd->markers[markerIndex];
  if (s.marker == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if (((s.marker->flags >> 5) & 1) == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }

  if ((s.snd->flags & 1) != 0) {
    FUN_100026d1(slot);
  }

  s.marker->volume = g_sndSlots[slot]->volume;
  s.sound.volume = s.marker->volume;
  s.marker->sampleRate = g_sndSlots[slot]->sampleRate;
  s.sound.sampleRate = s.marker->sampleRate;
  s.marker->pan  = g_sndSlots[slot]->pan;
  s.sound.pan = s.marker->pan;
  s.sound.flags = (s.marker->flags2 & 1) | (s.sound.flags & 0xfffffffe);

  s.result = FUN_10001701(s.marker, &s.sound);
  if (s.result == 0) {
    g_sndSlots[slot]->flags = (int)g_sndSlots[slot]->flags | 0x40;
    g_sndSlots[slot]->activeMarkerIndex = markerIndex - 1;
    g_sndSlots[slot]->flags = (int)g_sndSlots[slot]->flags | 1;
  }
  LeaveCriticalSection(&g_sndCs);
  return s.result;
}

// FUNCTION: MAGSND 0x1000246E
undefined4 __cdecl StopSnd(int slot)
{
  struct {
    int status;
    SndDuplicateBufferEntry *duplicateBuffer;
    int i;
    SndInstance *marker;
    int markerIndex;
  } s;
  
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
    s.marker->flags = (int)s.marker->flags | 2;
  }
  else {
    if (((g_sndSlots[slot]->flags2 >> 1) & 1) != 0) {
      g_sndSlots[slot]->flags = (int)g_sndSlots[slot]->flags | 2;
    }
    else {
      s.status = g_sndSlots[slot]->dsBuffer->lpVtbl->Stop(g_sndSlots[slot]->dsBuffer);
      if (s.status != 0) {
        LeaveCriticalSection(&g_sndCs);
        return 9;
      }
    }
  }
  if (((g_sndSlots[slot]->flags2 >> 1) & 1) == 0) {
    for (s.i = 0; s.i < 0x10; s.i++) {
      s.duplicateBuffer = &g_sndSlots[slot]->duplicateBuffers[s.i];      
      if (s.duplicateBuffer->buffer != 0) {
        s.status = s.duplicateBuffer->buffer->lpVtbl->Stop(s.duplicateBuffer->buffer);
        if (s.status != 0) {
          LeaveCriticalSection(&g_sndCs);
          return 9;
        }
      }
      else
        break;
    }
    g_sndSlots[slot]->flags &= ~1;
  }

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

  s.duplicatePtr = 0;
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
    if (*s.duplicatePtr != (LPDIRECTSOUNDBUFFER)0x0) {
      (*s.duplicatePtr)->lpVtbl->SetFrequency(*s.duplicatePtr,pitchHz);
    }
    else
      break;
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10002A24
undefined4 GetPitch(void)
{
  return 0;
}

// FUNCTION: MAGSND 0x10002A36
undefined4 __cdecl SetVol(int slot,uint volume)
{
  struct {
    SndDuplicateBufferEntry *duplicateBuffer;
    int i;
    SndInstance *markerInst;
    int markerIndex;
  } s;

  s.duplicateBuffer = 0;

  if (slot > 0x10f || slot < 0) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if ((int)g_sndSlots[slot] == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if (400 < volume) {
    volume = 400;
  }
  if (volume < 0) {
    volume = 0;
  }

  if ((*(uint *)((int)g_sndSlots[slot] + 4) >> 6 & 1) != 0) {
    s.markerIndex = g_sndSlots[slot]->activeMarkerIndex;
    s.markerInst = g_sndSlots[slot]->markers[s.markerIndex + 1];
    if (s.markerInst == 0) {
      LeaveCriticalSection(&g_sndCs);
      return 1;
    } else {
      s.markerInst->volume = volume;
      volume = (volume - 400) * 10;
      s.markerInst->dsBuffer->lpVtbl->SetVolume(s.markerInst->dsBuffer, volume);
    }
  }
  else {
    g_sndSlots[slot]->volume = volume;
    g_sndSlots[slot]->volume = volume;
    volume = (volume - 400) * 10;
    g_sndSlots[slot]->dsBuffer->lpVtbl->SetVolume(g_sndSlots[slot]->dsBuffer, volume);
  }
  for (s.i = 0; s.i < 0x10; s.i = s.i + 1) {
    s.duplicateBuffer = &g_sndSlots[slot]->duplicateBuffers[s.i];
    if (s.duplicateBuffer->buffer != 0) {      
      s.duplicateBuffer->buffer->lpVtbl->SetVolume(s.duplicateBuffer->buffer, volume);
    }
    else
    break;
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
  return 0;
}

// FUNCTION: MAGSND 0x10002D67
undefined4 UpdateSnd(void)
{
  struct {
    SndInstance *node;
    SndInstance *next;
  } s;
  
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
        FUN_10003748(s.node);
      }
    }
  
    if (((s.node->flags >> 2) & 1) != 0 && ((s.node->flags2 >> 2) & 1) != 0) {
      s.next = s.node->activeNext;
      UnloadSnd(s.node->slotIndex);
      s.node = s.next;
    }
    else
      s.node = s.node->updateNext;

  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

#pragma pack(push,4)
// FUNCTION: MAGSND 0x10002EB5
undefined4 __cdecl GetSndTime(int param_1,uint *outSndTime)
{
  struct {
    unsigned long local_1c;
    SndInstance *local_18;
    uint local_14;
    double local_10;
    uint local_8;
    unsigned long local_4;
  } s;

  if (0x10f < param_1 || param_1 < 0) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[param_1] == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  s.local_18 = g_sndSlots[param_1];
  s.local_18->dsBuffer->lpVtbl->GetCurrentPosition(s.local_18->dsBuffer,&s.local_1c,&s.local_4);
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
  s.local_10 = (double)s.local_18->timeBytes;
  if (s.local_18->waveFmt.nAvgBytesPerSec == 0x15888) {
    *outSndTime = (uint)(long)(s.local_10 / g_sndTimeDiv_88200);
  }
  else if (s.local_18->waveFmt.nAvgBytesPerSec == 0xac44) {
    *outSndTime = (uint)(long)(s.local_10 / g_sndTimeDiv_44100);
  }
  else if (s.local_18->waveFmt.nAvgBytesPerSec == 0x5622) {
    *outSndTime = (uint)(long)(s.local_10 / g_sndTimeDiv_22050);
  }
  else if (s.local_18->waveFmt.nAvgBytesPerSec == 0x2b11) {
    *outSndTime = (uint)(long)(s.local_10 / g_sndTimeDiv_11025);
  }
  else {
    
    LeaveCriticalSection(&g_sndCs);
    return 8;
  }
  g_sndTimeSamples[g_sndTimeSampleIndex].prevTime = g_lastSndTime;
  g_sndTimeSamples[g_sndTimeSampleIndex].currTime = *outSndTime;
  g_sndTimeSampleIndex = g_sndTimeSampleIndex + 1;
  g_sndTimeSampleIndex = g_sndTimeSampleIndex & 0xff;
  if (*outSndTime < g_lastSndTime) {
    LeaveCriticalSection(&g_sndCs);
    return 5;
  }
  g_lastSndTime = *outSndTime;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}
#pragma pack(pop)

// FUNCTION: MAGSND 0x1000314D
undefined4 __cdecl GetSndState(int param_1,undefined4 *outSndState)
{
  if ((0x10f < param_1) || (param_1 < 0)) {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[param_1] == (SndInstance *)0x0) {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if ((g_sndSlots[param_1]->flags & 1) != 0) {
    *outSndState = 1;
  }
  else if (((g_sndSlots[param_1]->flags >> 3) & 1) != 0) {
    *outSndState = 2;
  }
  else {
    *outSndState = 0;
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
    int slot;
    uint local_8;
    int local_4;
  } s;
  
  s.local_4 = 0;
  s.local_8 = -1;
  if (param_3 != 0 && param_3 > param_2 && param_3 < 0x100) {
    s.slot = param_2;
  }
  else {
    s.slot = 0;
    param_3 = 0xff;
  }

  EnterCriticalSection(&g_sndCs);
  for (; s.slot < param_3; s.slot = s.slot + 1) {
    if (g_sndSlots[s.slot] != 0) {
      if (g_sndSlots[s.slot]->lruCounter < s.local_8) {
        s.local_8 = g_sndSlots[s.slot]->lruCounter;
        s.local_10 = g_sndSlots[s.slot]->slotIndex;
      }
    } else {
      s.local_10 = s.slot;
      s.local_4 = 1;
      break;
    }
  }
  *param_1 = s.local_10;
  LeaveCriticalSection(&g_sndCs);
  return s.local_4;
}

// FUNCTION: MAGSND 0x100033A2
void * __cdecl GetAVISndBuff(int slot,uint param_2)
{
  struct {
    void *ptr1;
    SndInstance *slot;
    undefined4 zero1;
    uint blockIndex;
    int hr;
    void *ptr2;
    uint bytes2;
    uint bytes1;
    int offset;
  } s;

  s.ptr1 = 0;
  s.ptr2 = 0;
  s.bytes1 = 0;
  s.bytes2 = 0;
  s.zero1 = 0;

  if (slot > 0x10f || slot < 0x100) {
    return 0;
  }
  EnterCriticalSection(&g_sndCs);
  s.slot = g_sndSlots[slot];
  if (s.slot == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }
  if (((s.slot->flags >> 7) & 1) != 0) {
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }

  s.blockIndex = param_2 % s.slot->blockCount;
  s.offset = s.slot->blockBytes * s.blockIndex;
  s.hr = s.slot->dsBuffer->lpVtbl->Lock(
               s.slot->dsBuffer,s.offset,s.slot->blockBytes,
                &s.ptr1,&s.bytes1,&s.ptr2,&s.bytes2,0);
  if (s.hr != 0) {
    return 0;
  }
  if (s.ptr2 != 0) {
    s.slot->dsBuffer->lpVtbl->Unlock(s.slot->dsBuffer,s.ptr1,s.bytes1,s.ptr2,s.bytes2);
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }
  s.slot->dsLockedPtr = s.ptr1;
  s.slot->flags = (int)s.slot->flags | 0x80;
  LeaveCriticalSection(&g_sndCs);
  return s.ptr1;
}

// FUNCTION: MAGSND 0x1000352C
undefined4 __cdecl ReleaseAVISndBuff(int slot)
{
  struct {
    int local_18;
    SndInstance * slot;
    int local_10;
    int local_c;
    int local_8;
    int local_4;
  } s;

  s.local_18 = 0;
  s.local_c = 0;
  s.local_4 = 0;
  s.local_8 = 0;
  s.local_10 = 0;

  if ((slot > 0x10f) || (slot < 0x100)) {
    return 0;
  }

  EnterCriticalSection(&g_sndCs);
  s.slot = g_sndSlots[slot];
  if (s.slot == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }
  if (((s.slot->flags >> 7) & 1) == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 0xe;
  }
  if (s.slot->dsLockedPtr == 0) {
    LeaveCriticalSection(&g_sndCs);
    return 0xf;
  }
  s.slot->dsBuffer->lpVtbl->Unlock(s.slot->dsBuffer, s.slot->dsLockedPtr,s.slot->blockBytes,0,0);
  s.slot->flags &= 0xffffff7f;
  s.slot->dsLockedPtr = 0;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10003656
int __cdecl FUN_10003656(SndInstance *snd)
{
  struct {
    MMIOINFO *mmioInfo;
    int result;
  } s;
  
  snd->flags &= 0xfffffffe;
  snd->flags &= 0xfffffffd;
  snd->flags &= 0xffffffef;
  snd->flags &= 0xfffffffb;
  snd->writeCursorBytes = 0;
  snd->readCursorBytes = 0;
  snd->ringCursorBytes = 0;
  snd->timeBytes = 0;
  g_lastSndTime = 0;
  if ((snd->flags2 >> 5 & 1) != 0) {
    PrimeAviAudio(snd, 0);
  } else {
    s.mmioInfo = &snd->mmioInfo;
    s.mmioInfo->pchNext = s.mmioInfo->pchEndRead;

    mmioSetInfo(snd->hMmio, s.mmioInfo, 0);

    s.result = PrimeDsBufferFromSource(snd, 0);
    if (s.result != 0) {
      return s.result;
    }
  }
  snd->flags = (int)snd->flags | 0x20;
  return 0;
}

// FUNCTION: MAGSND 0x10003748
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
      if (s.buffer != NULL) {
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
  g_updateTimerId = timeSetEvent(g_updatePeriodMs,g_timeCaps.wPeriodMin,UpdateTimerProc,&g_sndHwnd,1);
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
undefined4 __cdecl LoadWaveMmio(LPSTR filename, SndInstance **outSnd, undefined4 dwFlags)
{
  typedef struct SndInstanceSlice_t {
    MMCKINFO mmck; /* 0x01c */
    uint markerCount; /* 0x030 */
    SndInstance *markers[0x11]; /* 0x034 */
  } SndInstanceSlice;

  struct {
    SndInstanceSlice *sndInstanceHacked;
    MMCKINFO riffChunk;
    WAVEFORMATEX riffHeader; // ebp - 0x80
    byte padbytes[2];
    MMIOINFO pmmioinfo;
    undefined4 unused; // ebp - 0x24
    DWORD riffChunkSize; // ebp - 0x20
    LONG mmioBaseOffset; // ebp - 0x1c
    MMCKINFO riff; // ebp - 0x18
    HMMIO fileHandle; // ebp - 0x4
  } s;

  s.unused = 0;
  memset(&s.pmmioinfo,0,sizeof(MMIOINFO));
  s.pmmioinfo.cchBuffer = 0x10000;
  s.fileHandle = mmioOpenA(filename,&s.pmmioinfo,0x10000);
  if (s.fileHandle == (HMMIO)0x0) {
    return 7;
  }
  s.riff.fccType = 0x45564157;
  if (mmioDescend(s.fileHandle,&s.riff,(MMCKINFO *)0x0,MMIO_FINDRIFF) != 0) {
    mmioClose(s.fileHandle,0);
    return 8;
  }
  s.riffChunk.ckid = 0x20746d66;
  if (mmioDescend(s.fileHandle,&s.riffChunk,&s.riff,MMIO_FINDCHUNK) != 0) {
    mmioClose(s.fileHandle,0);
    return 8;
  }
  s.riffChunkSize = s.riffChunk.cksize;
  if (mmioRead(s.fileHandle,&s.riffHeader,s.riffChunkSize) != s.riffChunkSize) {
    mmioClose(s.fileHandle,0);
    return 8;
  }

  s.riffHeader.cbSize = 0;
  mmioAscend(s.fileHandle,&s.riffChunk,0);
  s.riffChunk.ckid = 0x61746164;
  if (mmioDescend(s.fileHandle,&s.riffChunk,&s.riff,0x10) != 0) {
    mmioClose(s.fileHandle,0);
    return 8;
  }
  mmioGetInfo(s.fileHandle,&s.pmmioinfo,0);
  mmioAdvance(s.fileHandle,&s.pmmioinfo,0);
  s.mmioBaseOffset = s.pmmioinfo.lBufOffset;
  dwFlags = 0xe8;
  *outSnd = CreateSndInstance(g_directSound, 0x10000, &s.riffHeader, dwFlags);
  if (*outSnd != (SndInstance *)0x0) {    
    (*outSnd)->mmioBaseOffset = s.mmioBaseOffset;
    (*outSnd)->hMmio = s.fileHandle;
    (*outSnd)->mmioInfo = s.pmmioinfo;
    (*outSnd)->dataBytes = s.riffChunk.cksize;
    (*outSnd)->streamOrMmio = operator_new(strlen(filename) + 1);
    if ((*outSnd)->streamOrMmio != (void *)0x0) {
      strcpy((char *)(*outSnd)->streamOrMmio,filename);
    } else {
      mmioClose(s.fileHandle,0);
      return 3;
    }
    (*outSnd)->flags2 = (*outSnd)->flags2 & 0xffffffdf;
    (*outSnd)->flags2 = (int)(*outSnd)->flags2 | 2;
    (*outSnd)->blockBytes = 0;
    (*outSnd)->blockScale = 0;
    (*outSnd)->blockCount = 0;
  } else {
    mmioClose(s.fileHandle,0);
    return 9;
  }
  mmioAscend(s.fileHandle,&s.riffChunk,0);
  s.riffChunk.ckid = 0x20657563;

  if (mmioDescend(s.fileHandle,&s.riffChunk,&s.riff,0x10) == 0) {
    //TODO: this is fuckin weird no?
    s.sndInstanceHacked = &(*outSnd)->mmck;
    memcpy(s.sndInstanceHacked, &s.riffChunk, sizeof(MMCKINFO));
    if (mmioRead(s.fileHandle, &s.sndInstanceHacked->markerCount, 4) != 4) {
      DestroySndInstance(*outSnd);
      mmioClose(s.fileHandle,0);
      return 8;
    }
    s.sndInstanceHacked->markers[0] = operator_new(s.sndInstanceHacked->markerCount * 0x18);
    if (s.sndInstanceHacked->markers[0] == (void *)0x0) {
      DestroySndInstance(*outSnd);
      mmioClose(s.fileHandle,0);
      return 3;
    }
    s.riffChunkSize = s.sndInstanceHacked->markerCount * 0x18;
    if (mmioRead(s.fileHandle,s.sndInstanceHacked->markers[0],s.riffChunkSize) != (LONG)s.riffChunkSize) {
      DestroySndInstance(*outSnd);
      mmioClose(s.fileHandle,0);
      MAGSND_DELETE(s.sndInstanceHacked->markers[0]);
      return 8;
    }
  }
  return 0;
}

// FUNCTION: MAGSND 0x10004986
void __cdecl CloseMmioAndFreeSource(SndInstance *snd)
{
  struct {
    void * tmp4;
    void * tmp3;
    void * tmp2;
    void * tmp1;
  } s;

  mmioClose(snd->hMmio,0);
  if (snd->markers[0] != 0) {
    s.tmp1 = snd->markers[0];
    s.tmp3 = s.tmp1;
    operator_delete(s.tmp3);
  }
  if (((snd->flags2 >> 5) & 1) == 0) {
    s.tmp2 = snd->streamOrMmio;
    s.tmp4 = s.tmp2;
    operator_delete(s.tmp4);
  }
  return;
}

// FUNCTION: MAGSND 0x100049FE
undefined4 __cdecl LoadWaveFromFileHandle(FILE *file, SndInstance **outSnd)
{
  struct {
    undefined4 waveFmt[4];
    undefined2 cbSize;
    void *ptr1; // ebp - 0x38
    int seek0;
    int riffEnd;
    int riffTag;
    int riffSize;
    int waveTag;
    void *ptr2; // ebp - 0x20
    int bytes2; // ebp - 0x1c
    int chunkHdr[2];
    HRESULT lockHr;
    int bytes1; // ebp - 0xc
    int bytesRead; // ebp - 0x8
    int createFlags; // ebp - 0x4
  } s;

  s.ptr1 = (void *)0x0;
  s.ptr2 = (void *)0x0;
  s.bytes1 = 0;
  s.bytes2 = 0;
  s.bytesRead = 0;
  s.createFlags = 0;
  s.riffEnd = 0;
  s.seek0 = 0;

  fseek(file,s.seek0,0);

  //This reads riffTag, riffSize, and waveTag
  fread(&s.riffTag,1,0xc,file);
  s.riffEnd = s.riffSize + s.seek0 + 8;
  if ((s.riffTag != 0x46464952) || (s.waveTag != 0x45564157)) {
    return 8;
  }

  if (FindRiffChunk(file,0x20746d66,s.seek0,(uint)s.riffEnd) == 0) {
    return 8;
  }
  fread(s.chunkHdr,1,8,file);
  fread(s.waveFmt,1,0x10,file);
  s.cbSize = 0;
  if (FindRiffChunk(file,0x61746164,s.seek0,(uint)s.riffEnd) == 0) {
    return 8;
  }
  fread(s.chunkHdr,1,8,file);
  s.createFlags = 0xea;

  *outSnd  = CreateSndInstance(g_directSound,s.chunkHdr[1],s.waveFmt,s.createFlags);
  //*outSnd = s.bytes1;
  if (*outSnd == 0) {
    return 9;
  }

  s.lockHr = (*outSnd)->dsBuffer->lpVtbl->Lock((*outSnd)->dsBuffer,0,s.chunkHdr[1],&s.ptr1,&s.bytes1,&s.ptr2,&s.bytes2,0);
  if (s.lockHr != 0) {
    DestroySndInstance(*outSnd);
    return 9;
  }
  s.bytesRead = fread(s.ptr1,1,s.bytes1,file);
  if (s.bytesRead != s.bytes1) {
    DestroySndInstance(*outSnd);
    return 9;
  }
  s.lockHr = (*outSnd)->dsBuffer->lpVtbl->Unlock((*outSnd)->dsBuffer,s.ptr1,s.bytes1,s.ptr2,s.bytes2);
  if (s.lockHr != 0) {
    (*outSnd)->dsBuffer->lpVtbl->Release((*outSnd)->dsBuffer);
    MAGSND_DELETE(*outSnd);
    return 9;
  }
  return 0;
}

// FUNCTION: MAGSND 0x10004C8A
undefined4 __cdecl SeekMmioToSample(SndInstance *sndObj,int param_2)
{
  struct {
    int iVar1;
    int iVar3;
    int iVar2;
  }s;

  mmioGetInfo(sndObj->hMmio,(LPMMIOINFO)&sndObj->mmioInfo,0);
  s.iVar2 = sndObj->waveFmt.nBlockAlign * param_2 + sndObj->mmioBaseOffset;
  s.iVar3 = MmioTellFromInfo(&sndObj->mmioInfo);
  s.iVar1 = s.iVar2 - s.iVar3;
  mmioSeek(sndObj->hMmio,s.iVar1,1);
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
      break;
    }

    s.pos = ftell(param_1);
    if ((s.chunk[1] & 1) != 0) {
      s.chunk[1] = s.chunk[1] + 1;
    }
    fseek(param_1,s.chunk[1],1);
  }
  
  fseek(param_1,s.pos,0);
  return 1;
}

// FUNCTION: MAGSND 0x10004E4C
undefined4 __cdecl PrimeDsBufferFromSource(SndInstance *snd,int startSample)
{
  struct {
    int bytesRemainingInData; // ebp - 0x48
    int samplesReadAdvance; // ebp - 0x44
    int blockAlignCopy; // ebp - 0x40
    HPSTR lockPtr1; // ebp - 0x3c
    int startOffsetBytes; // ebp - 0x38
    int done; // ebp - 0x34
    int streamSampleCount; // ebp - 0x30
    int bytesRemainingToFill; // ebp - 0x2c
    HPSTR writePtr; // ebp - 0x28
    int sourceTotalBytes; // ebp - 0x24
    int wrapWindowBytes; // ebp - 0x20
    int sourceOffsetBytes; // ebp - 0x1c
    HPSTR lockPtr2; // ebp - 0x18
    int lockSize2; // ebp - 0x14
    int lockHr; // ebp - 0x10
    int bytesRead; // ebp - 0xc
    int streamByteOffset; // ebp - 0x8
    int lockSize1; // ebp - 0x4
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
    if (s.bytesRemainingInData >= s.bytesRemainingToFill) {
      if (((snd->flags2 >> 5) & 1) != 0) {
        AVIStreamRead((PAVISTREAM)snd->streamOrMmio,startSample,0x10000 / snd->waveFmt.nBlockAlign,s.writePtr,0x10000,&s.bytesRead,
                      &s.blockAlignCopy);
      }
      else {
        s.bytesRead = mmioRead(snd->hMmio,s.writePtr,s.bytesRemainingToFill);
      }
      s.bytesRemainingToFill = s.bytesRemainingToFill - s.bytesRead;
      s.samplesReadAdvance += s.bytesRead;
      s.streamByteOffset += s.bytesRead;
      s.bytesRemainingInData = s.bytesRemainingInData - s.bytesRead;
      s.sourceOffsetBytes += s.bytesRead;
      startSample = startSample + s.blockAlignCopy;
      s.done = 1;
    }
    else {
      if (((snd->flags2 >> 5) & 1) != 0) {
        AVIStreamRead((PAVISTREAM)snd->streamOrMmio,startSample,0x10000 / snd->waveFmt.nBlockAlign,s.writePtr,s.bytesRemainingInData,&s.bytesRead,0);
      }
      else {
        s.bytesRead = mmioRead(snd->hMmio,s.writePtr,s.bytesRemainingInData);
      }
      s.writePtr = s.writePtr + s.bytesRead;
      s.bytesRemainingToFill = s.bytesRemainingToFill - s.bytesRead;
      s.bytesRemainingInData = 0;
      s.sourceOffsetBytes = snd->dataBytes;
      if ((snd->flags2 & 1) == 0) {
        memset(s.writePtr,0,(size_t)s.bytesRemainingToFill);
        s.done = 1;
      }
      else {
        if (((snd->flags2 >> 5) & 1) == 0) {
          SeekMmioToSample(snd,0);
        }
        s.sourceOffsetBytes = 0;
        s.bytesRemainingInData = snd->dataBytes;
        s.streamByteOffset = 0;
        s.samplesReadAdvance = 0;
      }
    }
  }

  mmioGetInfo(snd->hMmio,&snd->mmioInfo,0);
  if (((snd->flags2 >> 5) & 1) != 0) {
    RefillAviMmioBuffer(snd,startSample);
  }
  else {
    mmioAdvance(snd->hMmio,&snd->mmioInfo,0);
  }

  s.wrapWindowBytes = min(0x10000, s.bytesRemainingInData);
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
undefined4 __cdecl PrimeAviAudio(SndInstance *snd, int unused)
{
  struct {
    int buff;
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
  
  s.local_20 = snd->blockCount * snd->blockBytes;
  s.local_4 = snd->dsBuffer->lpVtbl->Lock(snd->dsBuffer,0,s.local_20,&s.buff,&s.local_8,&s.local_14,&s.local_10,0);
  if (s.local_4 != 0) {
    FUN_1000575e(snd);
    DestroySndInstance(snd);
    return 9;
  }
  if (s.local_14 != 0) {
    snd->dsBuffer->lpVtbl->Unlock(snd->dsBuffer,s.buff,s.local_8,s.local_14,s.local_10);
    FUN_1000575e(snd);
    DestroySndInstance(snd);
    return 9;
  }
  snd->ringCursorBytes = 0;
  snd->writeCursorBytes = 0;
  snd->readCursorBytes = 0;
  s.local_18 = s.buff;
  s.local_c = 0;
  s.local_24 = snd->blockBytes / snd->blockScale;
  //s.local_4 = 0;
  for (s.local_2c = 0; s.local_2c < snd->blockCount; s.local_2c = s.local_2c + 1) {
    AVIStreamRead((PAVISTREAM*)snd->streamOrMmio,s.local_c,s.local_24,(void *)s.local_18,snd->blockBytes,s.local_28,
                  s.local_1c);
    snd->ringCursorBytes = snd->ringCursorBytes + snd->blockBytes;
    snd->ringCursorBytes = snd->ringCursorBytes % snd->dsDesc.dwBufferBytes;
    snd->writeCursorBytes = snd->writeCursorBytes + snd->blockBytes;
    snd->readCursorBytes = snd->readCursorBytes + snd->blockBytes;
    s.local_18 += (int)snd->blockBytes;
    s.local_c = s.local_c + s.local_24;
  }
  s.local_4 = snd->dsBuffer->lpVtbl->Unlock(snd->dsBuffer,s.buff,s.local_8,s.local_14,s.local_10);
  if (s.local_4 != 0) {
    CloseMmioAndFreeSource((SndInstance *)snd);
    DestroySndInstance((SndInstance *)snd);
    return 9;
  }
  return 0;
}

// FUNCTION: MAGSND 0x10005480
SndInstance * __cdecl CreateSndInstance(IDirectSound *dSound,DWORD dwBufferBytes,WAVEFORMATEX *wfxFormat,DWORD dwFlags)
{
  struct {
    void *local_10;
    void *local_c;
    int local_8;
    SndInstance *result;
  } s;
  
  s.result = (SndInstance *)operator_new(sizeof(SndInstance));
  memset(s.result, 0, sizeof(SndInstance));
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
undefined4 __cdecl DestroySndInstance(SndInstance *snd)
{
  if (snd == (SndInstance *)0x0) {
    return 5;
  }

  snd->dsBuffer->lpVtbl->Release(snd->dsBuffer);
  MAGSND_DELETE(snd);
  return 0;
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
  *outSnd = CreateSndInstance(g_directSound,s.local_98,&s.local_fc,scratch);
  if (*outSnd != (SndInstance *)0x0) {
    (*outSnd)->mmioBaseOffset = 0;
    (*outSnd)->hMmio = (HMMIO)0;
    (*outSnd)->mmioInfo = s.local_e8;
    (*outSnd)->dataBytes = s.local_94;
    (*outSnd)->streamOrMmio = stream;
    (*outSnd)->flags2 = (int)(*outSnd)->flags2 | 0x20;
    (*outSnd)->flags2 = (int)(*outSnd)->flags2 | 2;
    (*outSnd)->blockBytes = s.local_8c.dwSuggestedBufferSize;
    (*outSnd)->blockScale = s.local_8c.dwSampleSize;
    (*outSnd)->blockCount = 0xb;
    (*outSnd)->timeBytes = 0;
  }
  else
    return 9;
  return 0;
}

// FUNCTION: MAGSND 0x10005770
void __cdecl RefillAviMmioBuffer(SndInstance *sndObj,long streamStartSample)
{
  struct {
    int local_1c;
    int local_18;
    int local_14;
    int local_10;
    int local_c;
    long local_8;
    size_t local_4;
  } s;

  s.local_8 = 0;
  s.local_c = 0;
  s.local_14 = 0;
  s.local_1c = 0;
  s.local_4 = sndObj->writeCursorBytes - sndObj->readCursorBytes;
  if ((sndObj->mmioInfo.pchEndRead - sndObj->mmioInfo.pchNext) != s.local_4) {
    g_mmioInfoMismatchCount = g_mmioInfoMismatchCount + 1;
  }
  s.local_10 = sndObj->dataBytes - sndObj->writeCursorBytes;
  s.local_18 = sndObj->mmioInfo.cchBuffer - s.local_4;
  if (s.local_18 > s.local_10) {    
    s.local_18 = s.local_10;
  }
  if (s.local_4 != 0) {
    memmove((void *)sndObj->mmioInfo.pchBuffer,(void *)sndObj->mmioInfo.pchNext,s.local_4);
  }
  sndObj->mmioInfo.pchNext = &sndObj->mmioInfo.pchBuffer[s.local_4];
  AVIStreamRead((PAVISTREAM)sndObj->streamOrMmio,streamStartSample,
                s.local_18 / (int)(uint)sndObj->waveFmt.nBlockAlign,sndObj->mmioInfo.pchNext,s.local_18,
                &s.local_8,0);
  sndObj->mmioInfo.pchEndRead = sndObj->mmioInfo.pchBuffer + sndObj->mmioInfo.cchBuffer;
  sndObj->mmioInfo.pchNext = sndObj->mmioInfo.pchBuffer;
  mmioSetInfo(sndObj->hMmio,&sndObj->mmioInfo,0);
  return;
}

// FUNCTION: MAGSND 0x100058D0
int __cdecl MmioTellFromInfo(MMIOINFO *info) { return (int)info->lDiskOffset - ((int)info->pchEndRead - (int)info->pchNext); }

// FUNCTION: MAGSND 0x1000575E
undefined4 __cdecl FUN_1000575e(SndInstance *snd) { (void)snd; return 0; }

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

