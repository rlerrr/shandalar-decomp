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
typedef int(__stdcall code)();

typedef struct SndInstance SndInstance;
typedef struct SndDuplicateBufferEntry SndDuplicateBufferEntry;

// Why on earth is this a thing?
#define MAGSND_DELETE(toDelete) \
  do                            \
  {                             \
    void *tmp1 = toDelete;      \
    void *tmp2 = tmp1;          \
    operator_delete(tmp2);      \
  } while (false);

struct SndDuplicateBufferEntry
{
  LPDIRECTSOUNDBUFFER buffer; /* 0x000 */
  int reserved1;              /* 0x004 */
  int reserved2;              /* 0x008 */
};

struct SndInstance
{
  void *streamOrMmio;         /* 0x000: PAVISTREAM for AVI path, otherwise used as opaque */
  uint flags;                 /* 0x004 */
  uint flags2;                /* 0x008 */
  uint lruCounter;            /* 0x00c */
  int slotIndex;              /* 0x010 */
  int loadId;                 /* 0x014 */
  int activeMarkerIndex;      /* 0x018 */
  MMCKINFO mmck;              /* 0x01c */
  uint markerCount;           /* 0x030 */
  SndInstance *markers[0x11]; /* 0x034 */
  WAVEFORMATEX waveFmt;       /* 0x078 (18 bytes) */
  unsigned short pad_08a;     /* 0x08a */
  uint blockBytes;            /* 0x08c */
  int blockScale;             /* 0x090 */
  int blockCount;             /* 0x094 */
  byte pad_098[0xa0 - 0x98];
  uint timeEventCount;                            /* 0x0a0 */
  void *dsLockedPtr;                              /* 0x0a4: pointer from IDirectSoundBuffer::Lock */
  DSBUFFERDESC dsDesc;                            /* 0x0a8 (DSBUFFERDESC, size 0x14) */
  LPDIRECTSOUNDBUFFER dsBuffer;                   /* 0x0bc: LPDIRECTSOUNDBUFFER */
  SndDuplicateBufferEntry duplicateBuffers[0x10]; /* 0x0c0 - 0x180 */
  MMIOINFO mmioInfo;                              /* 0x180 */
  HMMIO hMmio;                                    /* 0x1c8 */
  int dataBytes;                                  /* 0x1cc */
  uint writeCursorBytes;                          /* 0x1d0: bytes written/consumed from stream */
  uint readCursorBytes;                           /* 0x1d4 */
  uint ringCursorBytes;                           /* 0x1d8 */
  uint timeBytes;                                 /* 0x1dc */
  int mmioBaseOffset;                             /* 0x1e0 */
  uint ringCursorSavedBytes;                      /* 0x1e4 */
  int pan;                                        /* 0x1e8 */
  int sampleRate;                                 /* 0x1ec */
  int volume;                                     /* 0x1f0 */
  SndInstance *updatePrev;                        /* 0x1f4: used by AddToUpdateList/RemoveFromUpdateList */
  SndInstance *updateNext;                        /* 0x1f8 */
  SndInstance *activePrev;                        /* 0x1fc: used by AddToActiveList/RemoveFromActiveList */
  SndInstance *activeNext;                        /* 0x200 */
};
#if !(defined(MODERN_FIXES) && defined(__clang__))
STATIC_ASSERT(sizeof(SndInstance) == 0x204, SndInstance_wrong_size);
#endif

void __cdecl ReleaseSnd(void);
int __cdecl LoadSnd(LPSTR path, int slot, Sound *sound);
undefined4 __cdecl UnloadSnd(int slot);
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
void *__cdecl GetAVISndBuff(int slot, uint bytesRequested);
undefined4 __cdecl ReleaseAVISndBuff(int slot);
int __cdecl IsSndLoaded(int loadId, int *outSlot);
undefined4 __cdecl GetLRUSnd(int *outSlot, int startSlot, int endSlot);

void __cdecl UpdateMmioSnd(SndInstance *snd);
int __cdecl UpdateAviSnd(SndInstance *snd);
void __cdecl AddToUpdateList(SndInstance *snd);
void __cdecl RemoveFromUpdateList(SndInstance *snd);
void __cdecl AddToActiveList(SndInstance *snd);
void __cdecl RemoveFromActiveList(SndInstance *snd);
undefined4 __cdecl AcquireFreeDsBuffer(SndInstance *snd, LPDIRECTSOUNDBUFFER *outBuffer);
int __cdecl PlaySndInstance(SndInstance *snd, Sound *sound);
undefined4 __cdecl StopSndNow(int slot);
int __cdecl ResetAndPrimeSndInstance(SndInstance *snd);
undefined4 __cdecl StartUpdateTimer(undefined4 resolutionMs);
void __cdecl StopUpdateTimer(void);
void CALLBACK UpdateTimerProc(UINT u1, UINT u2, DWORD dw1, DWORD dw2, DWORD dw3);
void __cdecl CloseMmioAndFreeSource(SndInstance *snd);
undefined4 __cdecl LoadFromFile(char *filename, SndInstance **outSnd);
undefined4 __cdecl LoadWaveMmio(LPSTR filename, SndInstance **outSnd, undefined4 scratch);
undefined4 __cdecl PrimeDsBufferFromSource(SndInstance *snd, int startSample);
undefined4 __cdecl PrimeAviAudio(SndInstance *snd, int unused);
undefined4 __cdecl LoadAviAudioStream(void *stream, SndInstance **outSnd, undefined4 scratch);
void __cdecl RefillAviMmioBuffer(SndInstance *sndObj, long streamStartSample);
int __cdecl MmioTellFromInfo(MMIOINFO *info);
undefined4 __cdecl LoadWaveFromFileHandle(FILE *file, SndInstance **outSnd);
undefined4 __cdecl FindRiffChunk(FILE *file, int chunkId, long searchStart, uint searchEnd);
SndInstance *__cdecl CreateSndInstance(IDirectSound *dSound, DWORD dwBufferBytes, WAVEFORMATEX *wfxFormat, DWORD dwFlags);
undefined4 __cdecl DestroySndInstance(SndInstance *snd);
undefined4 __cdecl CleanupAviStream(SndInstance *snd);
void *__cdecl operator_new(unsigned int size);
void __cdecl operator_delete(void *p);

typedef unsigned short ushort;

typedef struct SndTimeSample
{
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
undefined4 g_updateSndDisabled = 0x00000000;

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
undefined4 __cdecl InitSnd(HWND hwnd, undefined4 unused, uint flags)
{
  struct
  {
    HRESULT hr;
    DWORD format;
  } s;

  if (flags & (byte)0x2)
  {
    if (g_sndRefCount == 0)
    {
      return 4;
    }
  }
  if (flags & 2)
  {
    if (g_sndRefCount != 0)
    {
      return 0;
    }
  }
  if (g_sndRefCount == 0)
  {
    if (hwnd != 0)
    {
      s.hr = DirectSoundCreate(0, &g_directSound, 0);
      if (s.hr != 0)
      {
        return 4;
      }
      s.hr = g_directSound->lpVtbl->SetCooperativeLevel(g_directSound, hwnd, 3);
      if (s.hr != 0)
      {
        ReleaseSnd();
        return 4;
      }
      s.hr = g_directSound->lpVtbl->CreateSoundBuffer(g_directSound, &g_primaryDsDesc, &g_primaryDsBuffer, 0);
      if (s.hr != 0)
      {
        ReleaseSnd();
        return 4;
      }
      s.hr = g_primaryDsBuffer->lpVtbl->SetFormat(g_primaryDsBuffer, &g_primaryWaveFormat);
      if (s.hr != 0)
      {
        g_primaryDsBuffer->lpVtbl->GetFormat(g_primaryDsBuffer, &g_primaryWaveFormat, 0x12, &s.format);
      }
      g_sndHwnd = hwnd;
      g_sndRefCount = g_sndRefCount + 1;
      InitializeCriticalSection(&g_sndCs);
    }
  }
  if ((flags & 1) != 0)
  {
    if (g_updateTimerActive != 0)
    {
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
  if (g_updateTimerActive != 0)
  {
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
int __cdecl LoadSnd(LPSTR path, int slot, Sound *sound)
{
  int result;

  if ((sound != (Sound *)0) && ((sound->flags >> 4 & 1U) != 0) && (slot != 0))
  {
    if (0x10f < slot || slot < 0x100)
    {
      return 5;
    }
    if (g_sndSlots[slot] != (SndInstance *)0x0)
    {
      return 0;
    }
  }
  else
  {
    if (0x100 < slot || slot < 0)
    {
      return 5;
    }
    if (g_sndSlots[slot] != (SndInstance *)0x0)
    {
      return 0;
    }
  }
  if ((sound != (Sound *)0) && ((sound->flags >> 2 & 1U) != 0))
  {
    if ((sound->flags >> 4 & 1U) != 0)
    {
      result = LoadAviAudioStream((void *)path, (&g_sndSlots[slot]), 0);
      if (result != 0)
      {
        g_sndSlots[slot] = (SndInstance *)0;
        return result;
      }
      if ((sound->flags >> 5 & 1U) != 0)
      {
        g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 0x40;
      }
      else
      {
        PrimeAviAudio(g_sndSlots[slot], 0);
        g_sndSlots[slot]->flags2 &= ~0x40u;
      }
      g_sndLoadSerial += 1;
    }
    else
    {
      result = LoadWaveMmio(path, (&g_sndSlots[slot]), 0);
      if (result != 0)
      {
        g_sndSlots[slot] = (SndInstance *)0;
        return result;
      }
      PrimeDsBufferFromSource(g_sndSlots[slot], 0);
    }
    g_sndSlots[slot]->flags = (int)g_sndSlots[slot]->flags | 0x20;
    AddToUpdateList(g_sndSlots[slot]);
    g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 2;
  }
  else
  {
    result = LoadFromFile(path, (&g_sndSlots[slot]));
    if (result != 0)
    {
      g_sndSlots[slot] = (SndInstance *)0;
      return result;
    }
  }

  EnterCriticalSection(&g_sndCs);
  AddToActiveList(g_sndSlots[slot]);
  g_sndSlots[slot]->slotIndex = slot;
  if ((sound != (Sound *)0) && (sound->loadId != 0))
  {
    g_sndSlots[slot]->loadId = sound->loadId;
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x100014BA
undefined4 __cdecl UnloadSnd(int slot)
{
  uint i;

  if ((slot > 0x10f) || (0 > slot))
  {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  StopSndNow(slot);
  RemoveFromActiveList(g_sndSlots[slot]);
  if (((g_sndSlots[slot]->flags2 >> 1) & 1) != 0)
  {
    RemoveFromUpdateList(g_sndSlots[slot]);
    if (((g_sndSlots[slot]->flags2 >> 5) & 1) == 0)
    {
      CloseMmioAndFreeSource(g_sndSlots[slot]);
      for (i = 0; i < g_sndSlots[slot]->markerCount; i = i + 1)
      {
        if (g_sndSlots[slot]->markers[i + 1] != (void *)0x0)
        {
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
  while (g_activeSndListHead != (SndInstance *)0x0)
  {
    UnloadSnd(g_activeSndListHead->slotIndex);
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10001668
undefined4 __cdecl PlaySnd(int slot, Sound *sound)
{
  int unused;
  if (0x10f < slot || slot < 0)
  {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  unused = PlaySndInstance(g_sndSlots[slot], sound);
  g_sndSlots[slot]->lruCounter++;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

#define ONE 1

// FUNCTION: MAGSND 0x10001701
int __cdecl PlaySndInstance(SndInstance *snd, Sound *sound)

{
  struct
  {
    int result2;                  // ebp - 0x1c
    int freqValue;                // ebp - 0x18
    LPDIRECTSOUNDBUFFER dsBuffer; // ebp - 0x14
    uint playFlags;               // ebp - 0x10
    int panValue;                 // ebp - 0xc
    int sampleRate;               // ebp - 0x8
    int result;                   // ebp - 0x4
  } s;

  s.playFlags = 0;
  s.freqValue = 0;
  s.sampleRate = 0;
  if (((snd->flags2 >> ONE) & 1) != 0)
  {
    if ((snd->flags & 1) != 0)
    {
      StopSndNow(snd->slotIndex);
    }
    if ((g_updateTimerEnabled == 1) && (g_updateTimerActive == 0))
    {
      s.result = StartUpdateTimer(g_updateResolutionMs);
      if (s.result != 0)
      {
        UnloadSnd(snd->slotIndex);
        return s.result;
      }
    }
    if (g_updateTimerEnabled != 0)
    {
      g_updateTimerUsers = g_updateTimerUsers + 1;
    }
    s.playFlags |= 1;
    s.dsBuffer = snd->dsBuffer;
    if (((snd->flags >> 5) & 1) == 0)
    {
      s.result = ResetAndPrimeSndInstance(snd);
      if (s.result != 0)
      {
        UnloadSnd(snd->slotIndex);
        return s.result;
      }
    }
  }
  else
  {
    if ((sound == NULL) || ((sound->flags >> 1 & 1) == 0))
    {
      s.result = AcquireFreeDsBuffer(snd, &s.dsBuffer);
      if (s.result != 0)
      {
        return s.result;
      }
    }
    else
    {
      s.dsBuffer = snd->dsBuffer;
    }
  }
  if (sound != NULL)
  {
    s.freqValue = sound->volume;
    if (s.freqValue > 400)
    {
      s.freqValue = 400;
    }
    snd->volume = s.freqValue;
    s.freqValue = (s.freqValue - 400) * 10;
    if (sound->sampleRate != 0)
    {
      s.sampleRate = sound->sampleRate;
    }
    else
    {
      s.sampleRate = snd->waveFmt.nSamplesPerSec;
    }
    snd->sampleRate = s.sampleRate;
    if (sound->pan != 0)
    {
      s.panValue = sound->pan;
    }
    else
    {
      s.panValue = 0;
    }
    snd->pan = s.panValue;
    s.panValue = s.panValue * 10;
    if ((sound->flags & 1) != 0)
    {
      s.playFlags |= 1;
      snd->flags2 = (int)snd->flags2 | 1;
    }
    if ((sound->flags >> 3 & 1) != 0)
    {
      snd->flags2 = (int)snd->flags2 | 4;
    }
  }
  else
  {
    s.freqValue = 0;
    snd->volume = 400;
    s.sampleRate = snd->waveFmt.nSamplesPerSec;
    snd->sampleRate = s.sampleRate;
    s.panValue = 0;
    snd->pan = 0;
  }
  s.dsBuffer->lpVtbl->SetVolume(s.dsBuffer, s.freqValue);
  s.dsBuffer->lpVtbl->SetFrequency(s.dsBuffer, s.sampleRate);
  s.dsBuffer->lpVtbl->SetPan(s.dsBuffer, s.panValue);
  s.dsBuffer->lpVtbl->SetCurrentPosition(s.dsBuffer, 0);
  if (s.dsBuffer->lpVtbl->Play(s.dsBuffer, 0, 0, s.playFlags))
  {
    s.result2 = 1;
  }
  else
  {
    s.result2 = 0;
  }
  if (s.result2 != 0)
  {
    return 9;
  }

  snd->flags = (int)snd->flags | 1;
  snd->flags &= 0xffffffdf;
  return 0;
}

// FUNCTION: MAGSND 0x10001A1F
int __cdecl PlaySndFile(LPSTR filename, int slot, Sound *sound)

{
  struct
  {
    DWORD writeCursorBytes;
    int volume;
    int pan;
    int nSamplesPerSec;
    int result;
    undefined4 scratch;
  } s;

  s.scratch = 0;
  s.volume = 0;
  s.nSamplesPerSec = 0;
  if ((0xff < slot) || (slot < 0))
  {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if ((int)g_sndSlots[slot] != 0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 2;
  }
  s.result = LoadWaveMmio(filename, (&g_sndSlots[slot]), s.scratch);
  if (s.result != 0)
  {
    LeaveCriticalSection(&g_sndCs);
    return s.result;
  }
  AddToActiveList(g_sndSlots[slot]);
  AddToUpdateList(g_sndSlots[slot]);
  if ((g_updateTimerEnabled == 1) && (g_updateTimerActive == 0))
  {
    s.result = StartUpdateTimer(g_updateResolutionMs);
    if (s.result != 0)
    {
      UnloadSnd(slot);
      LeaveCriticalSection(&g_sndCs);
      return s.result;
    }
  }
  g_sndSlots[slot]->slotIndex = slot;
  if (sound != (Sound *)0)
  {
    s.volume = sound->volume;
    g_sndSlots[slot]->volume = s.volume;
    if (400 < s.volume)
    {
      s.volume = 400;
    }
    s.volume = (s.volume - 400) * 10;
    if (sound->sampleRate != 0)
    {
      s.nSamplesPerSec = sound->sampleRate;
    }
    else
    {
      s.nSamplesPerSec = (int)g_sndSlots[slot]->waveFmt.nSamplesPerSec;
    }
    g_sndSlots[slot]->sampleRate = s.nSamplesPerSec;

    if (sound->pan != 0)
    {
      s.pan = sound->pan;
    }
    else
    {
      s.pan = 0;
    }
    g_sndSlots[slot]->pan = s.pan;
    s.pan = s.pan * 10;
    if (sound->flags & 1)
    {
      g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 1;
    }
    if ((sound->flags >> 3 & 1) != 0)
    {
      g_sndSlots[slot]->flags2 = (int)g_sndSlots[slot]->flags2 | 4;
    }
  }
  else
  {
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

  g_sndSlots[slot]->dsBuffer->lpVtbl->SetFrequency(g_sndSlots[slot]->dsBuffer, s.nSamplesPerSec);
  g_sndSlots[slot]->sampleRate = s.nSamplesPerSec;

  g_sndSlots[slot]->dsBuffer->lpVtbl->SetPan(g_sndSlots[slot]->dsBuffer, s.pan);
  g_sndSlots[slot]->pan = s.pan;

  PrimeDsBufferFromSource(g_sndSlots[slot], 0);

  g_sndSlots[slot]->dsBuffer->lpVtbl->Play(g_sndSlots[slot]->dsBuffer, 0, 0, 1);
  g_sndSlots[slot]->dsBuffer->lpVtbl->GetCurrentPosition(g_sndSlots[slot]->dsBuffer, (LPDWORD)&g_sndSlots[slot]->ringCursorBytes, &s.writeCursorBytes);
  g_sndSlots[slot]->flags = (int)g_sndSlots[slot]->flags | 1;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10001E52
int __cdecl LoadMarkerInstance(SndInstance *parent, int markerIndex)
{
  struct
  {
    SndInstance **markerSlot;
    int markerMetaIndex;
    SndInstance **markerMeta;
    int result;
    int markerStartSample;
  } s;

  s.result = 0;
  s.markerStartSample = 0;
  s.markerMetaIndex = 0;

  s.markerMeta = &parent->markers[markerIndex * 3 - 3];
  s.markerSlot = &parent->markers[markerIndex - 1] + 1;
  s.result = LoadWaveMmio((LPSTR)parent->streamOrMmio, s.markerSlot, 0x10);
  if (s.result != 0)
  {
    return s.result;
  }

  s.markerStartSample = (*s.markerMeta)->loadId;
  s.result = PrimeDsBufferFromSource(*s.markerSlot, s.markerStartSample);
  if (s.result != 0)
  {
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
int __cdecl PrimeMarkerInstance(SndInstance *parent, int markerIndex)
{
  struct
  {
    int markerMetaIndex;
    SndInstance *markerInstance;
    SndInstance *markerMeta;
    int result;
    int markerStartSample;
  } s;

  s.result = 0;
  s.markerStartSample = 0;
  s.markerMetaIndex = 0;

  if (((parent->flags >> 6) & 1) != 0)
  {
    if (parent->activeMarkerIndex == markerIndex - 1)
    {
      return 0xd;
    }
  }

  s.markerMeta = parent->markers[(markerIndex - 1) * 6];
  s.markerInstance = parent->markers[markerIndex];
  s.markerStartSample = s.markerMeta->loadId;
  s.result = PrimeDsBufferFromSource(s.markerInstance, s.markerStartSample);
  if (s.result != 0)
  {
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
void __cdecl SwapActiveInstances(int *activeInstanceSlot, int *newInstanceSlot)
{
  struct
  {
    int i;
    int old;
  } s;

  s.old = *activeInstanceSlot;
  *activeInstanceSlot = *newInstanceSlot;
  s.i = 0;
  for (; s.i < 0x10; s.i = s.i + 1)
  {
    if (*(int *)(s.old + 0x38 + s.i * 4) == *activeInstanceSlot)
    {
      *(int *)(*activeInstanceSlot + 0x38 + s.i * 4) = s.old;
    }
    else
    {
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
  struct
  {
    SndInstance *snd;
    int result;
  } s;

  s.result = 0;
  if ((slot > 0xff) || (slot < 0))
  {
    return 5;
  }

  EnterCriticalSection(&g_sndCs);
  s.snd = g_sndSlots[slot];
  if (s.snd == (SndInstance *)0x0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if ((s.snd->markerCount == 0) || (s.snd->markerCount < markerIndex))
  {
    LeaveCriticalSection(&g_sndCs);
    return 5;
  }
  if (s.snd->markers[markerIndex] != (void *)0x0)
  {
    s.result = PrimeMarkerInstance(s.snd, markerIndex);
  }
  else
  {
    s.result = LoadMarkerInstance(s.snd, markerIndex);
  }
  LeaveCriticalSection(&g_sndCs);
  return s.result;
}

// FUNCTION: MAGSND 0x10002249
int __cdecl PlaySndMarker(int slot, uint markerIndex)
{
  struct
  {
    SndInstance *marker; // ebp - 0x2c
    SndInstance *snd;    // ebp - 0x28
    int result;          // ebp - 0x24
    Sound sound;         // ebp - 0x20
  } s;

  s.result = 0;

  if ((slot > 0xff) || (slot < 0))
  {
    return 5;
  }
  if (((int)markerIndex > 0x10) || ((int)markerIndex < 0))
  {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  s.snd = g_sndSlots[slot];
  if (s.snd == (SndInstance *)0x0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if ((s.snd->markerCount == 0) || (s.snd->markerCount < markerIndex))
  {
    LeaveCriticalSection(&g_sndCs);
    return 5;
  }
  s.marker = s.snd->markers[markerIndex];
  if (s.marker == (SndInstance *)0x0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if (((s.marker->flags >> 5) & 1) == 0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }

  if ((s.snd->flags & 1) != 0)
  {
    StopSndNow(slot);
  }

  s.marker->volume = g_sndSlots[slot]->volume;
  s.sound.volume = s.marker->volume;
  s.marker->sampleRate = g_sndSlots[slot]->sampleRate;
  s.sound.sampleRate = s.marker->sampleRate;
  s.marker->pan = g_sndSlots[slot]->pan;
  s.sound.pan = s.marker->pan;
  s.sound.flags = (s.marker->flags2 & 1) | (s.sound.flags & 0xfffffffe);

  s.result = PlaySndInstance(s.marker, &s.sound);
  if (s.result == 0)
  {
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
  struct
  {
    int status;
    SndDuplicateBufferEntry *duplicateBuffer;
    int i;
    SndInstance *marker;
    int markerIndex;
  } s;

  if ((0x10f < slot) || (slot < 0))
  {
    return 5;
  }

  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if (((g_sndSlots[slot]->flags >> 6) & 1) != 0)
  {
    s.markerIndex = g_sndSlots[slot]->activeMarkerIndex;
    s.marker = g_sndSlots[slot]->markers[s.markerIndex + 1];
    if (s.marker == (SndInstance *)0x0)
    {
      LeaveCriticalSection(&g_sndCs);
      return 1;
    }
    s.marker->flags = (int)s.marker->flags | 2;
  }
  else
  {
    if (((g_sndSlots[slot]->flags2 >> 1) & 1) != 0)
    {
      g_sndSlots[slot]->flags = (int)g_sndSlots[slot]->flags | 2;
    }
    else
    {
      s.status = g_sndSlots[slot]->dsBuffer->lpVtbl->Stop(g_sndSlots[slot]->dsBuffer);
      if (s.status != 0)
      {
        LeaveCriticalSection(&g_sndCs);
        return 9;
      }
    }
  }
  if (((g_sndSlots[slot]->flags2 >> 1) & 1) == 0)
  {
    for (s.i = 0; s.i < 0x10; s.i++)
    {
      s.duplicateBuffer = &g_sndSlots[slot]->duplicateBuffers[s.i];
      if (s.duplicateBuffer->buffer != 0)
      {
        s.status = s.duplicateBuffer->buffer->lpVtbl->Stop(s.duplicateBuffer->buffer);
        if (s.status != 0)
        {
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
  while (node != (SndInstance *)0x0)
  {
    StopSnd(node->slotIndex);
    node = node->activeNext;
  }
  LeaveCriticalSection(&g_sndCs);
  return;
}

// FUNCTION: MAGSND 0x100026D1
undefined4 __cdecl StopSndNow(int slot)
{
  struct
  {
    int status;
    LPLPDIRECTSOUNDBUFFER subPtr;
    int i;
    SndInstance *markerInstance;
    int markerIndex;
  } s;

  if (slot > 0x10f || slot < 0)
  {
    return 5;
  }
  if (g_sndSlots[slot] == (SndInstance *)0x0)
  {
    return 1;
  }
  if (((g_sndSlots[slot]->flags >> 6) & 1) != 0)
  {
    s.markerIndex = g_sndSlots[slot]->activeMarkerIndex;
    s.markerInstance = g_sndSlots[slot]->markers[s.markerIndex + 1];
    if (s.markerInstance == 0)
    {
      return 1;
    }

    s.status = s.markerInstance->dsBuffer->lpVtbl->Stop(s.markerInstance->dsBuffer);
    if (s.status != 0)
    {
      return 9;
    }
    g_sndSlots[slot]->flags &= 0xffffffbf;
    s.markerInstance->flags &= 0xfffffffe;
    s.markerInstance->flags &= 0xfffffffd;
    s.markerInstance->flags = (int)s.markerInstance->flags | 4;
    s.markerInstance->flags &= 0xffffffdf;
  }
  else
  {
    s.status = g_sndSlots[slot]->dsBuffer->lpVtbl->Stop(g_sndSlots[slot]->dsBuffer);
    if (s.status != 0)
    {
      return 9;
    }
  }
  if (((g_sndSlots[slot]->flags2 >> 1) & 1) == 0)
  {
    for (s.i = 0; s.i < 0x10; s.i = s.i + 1)
    {
      s.subPtr = &g_sndSlots[slot]->duplicateBuffers[s.i].buffer;
      if ((*s.subPtr) != 0x0)
      {
        s.status = (*s.subPtr)->lpVtbl->Stop(*s.subPtr);
        if (s.status != 0)
        {
          return 9;
        }
      }
      else
        break;
    }
  }
  else if (g_updateTimerUsers > 0)
  {
    g_updateTimerUsers = g_updateTimerUsers - 1;
    if (g_updateTimerUsers == 0)
    {
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
undefined4 __cdecl SetPitch(int slot, undefined4 pitchHz)
{
  struct
  {
    LPLPDIRECTSOUNDBUFFER duplicatePtr;
    int index;
  } s;

  s.duplicatePtr = 0;
  if ((0x10f < slot) || (slot < 0))
  {
    return 5;
  }

  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }

  g_sndSlots[slot]->dsBuffer->lpVtbl->SetFrequency(g_sndSlots[slot]->dsBuffer, pitchHz);
  for (s.index = 0; s.index < 0x10; s.index = s.index + 1)
  {
    s.duplicatePtr = &g_sndSlots[slot]->duplicateBuffers[s.index].buffer;
    if (*s.duplicatePtr != (LPDIRECTSOUNDBUFFER)0x0)
    {
      (*s.duplicatePtr)->lpVtbl->SetFrequency(*s.duplicatePtr, pitchHz);
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
undefined4 __cdecl SetVol(int slot, uint volume)
{
  struct
  {
    SndDuplicateBufferEntry *duplicateBuffer;
    int i;
    SndInstance *markerInst;
    int markerIndex;
  } s;

  s.duplicateBuffer = 0;

  if (slot > 0x10f || slot < 0)
  {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if ((int)g_sndSlots[slot] == 0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if (400 < volume)
  {
    volume = 400;
  }
  if (volume < 0)
  {
    volume = 0;
  }

  if ((*(uint *)((int)g_sndSlots[slot] + 4) >> 6 & 1) != 0)
  {
    s.markerIndex = g_sndSlots[slot]->activeMarkerIndex;
    s.markerInst = g_sndSlots[slot]->markers[s.markerIndex + 1];
    if (s.markerInst == 0)
    {
      LeaveCriticalSection(&g_sndCs);
      return 1;
    }
    else
    {
      s.markerInst->volume = volume;
      volume = (volume - 400) * 10;
      s.markerInst->dsBuffer->lpVtbl->SetVolume(s.markerInst->dsBuffer, volume);
    }
  }
  else
  {
    g_sndSlots[slot]->volume = volume;
    g_sndSlots[slot]->volume = volume;
    volume = (volume - 400) * 10;
    g_sndSlots[slot]->dsBuffer->lpVtbl->SetVolume(g_sndSlots[slot]->dsBuffer, volume);
  }
  for (s.i = 0; s.i < 0x10; s.i = s.i + 1)
  {
    s.duplicateBuffer = &g_sndSlots[slot]->duplicateBuffers[s.i];
    if (s.duplicateBuffer->buffer != 0)
    {
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
undefined4 __cdecl SetPan(int slot, int pan)

{
  struct
  {
    LPLPDIRECTSOUNDBUFFER duplicateBuffer;
    int index;
  } s;

  s.duplicateBuffer = 0;
  if ((0x10f < slot) || (slot < 0))
  {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }

  pan = (pan * 5) * 2;
  // pan = pan + pan;
  g_sndSlots[slot]->dsBuffer->lpVtbl->SetPan(g_sndSlots[slot]->dsBuffer, pan);
  for (s.index = 0; s.index < 0x10; s.index++)
  {
    s.duplicateBuffer = &g_sndSlots[slot]->duplicateBuffers[s.index].buffer;
    if (*s.duplicateBuffer != NULL)
    {
      (*s.duplicateBuffer)->lpVtbl->SetPan(*s.duplicateBuffer, pan);
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
  struct
  {
    SndInstance *node;
    SndInstance *next;
  } s;

  if (g_updateSndDisabled != 0)
  {
    return 0xd;
  }

  EnterCriticalSection(&g_sndCs);
  s.node = g_updateSndHead;
  while (s.node != (SndInstance *)0x0)
  {
    if ((s.node->flags & 1) != 0)
    {
      if (((s.node->flags >> 1) & 1) != 0)
      {
        if ((uint)s.node->volume > 5)
        {
          s.node->volume = s.node->volume - 5;
        }
        else
        {
          s.node->volume = 0;
        }
        SetVol(s.node->slotIndex, s.node->volume);
      }

      if (((s.node->flags2 >> 5) & 1) != 0)
      {
        if (((s.node->flags2 >> 6) & 1) == 0)
        {
          UpdateAviSnd(s.node);
        }
      }
      else
      {
        UpdateMmioSnd(s.node);
      }
    }

    if (((s.node->flags >> 2) & 1) != 0 && ((s.node->flags2 >> 2) & 1) != 0)
    {
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

#pragma pack(push, 4)
// FUNCTION: MAGSND 0x10002EB5
undefined4 __cdecl GetSndTime(int slot, uint *outSndTime)
{
  struct
  {
    unsigned long playCursorBytes;
    SndInstance *snd;
    uint prevCursorBytes;
    double timeBytes;
    uint blockIndex;
    unsigned long writeCursorBytes;
  } s;

  if (0x10f < slot || slot < 0)
  {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  s.snd = g_sndSlots[slot];
  s.snd->dsBuffer->lpVtbl->GetCurrentPosition(s.snd->dsBuffer, &s.playCursorBytes, &s.writeCursorBytes);
  s.prevCursorBytes = s.snd->timeBytes % s.snd->dsDesc.dwBufferBytes;
  if (s.prevCursorBytes < s.playCursorBytes)
  {
    s.snd->timeBytes += (s.playCursorBytes - s.prevCursorBytes);
  }
  else
  {
    s.snd->timeBytes += (s.snd->dsDesc.dwBufferBytes - s.prevCursorBytes);
    s.snd->timeBytes += s.playCursorBytes;
  }
  if (((s.snd->flags2 >> 5) & 1) != 0)
  {
    s.blockIndex = s.snd->timeBytes / s.snd->blockBytes;
    while (s.snd->timeEventCount < s.blockIndex)
    {
      PostMessageA(g_sndHwnd, 0x3bd, 0, s.blockIndex);
      s.snd->timeEventCount++;
    }
  }
  s.timeBytes = (double)s.snd->timeBytes;
  if (s.snd->waveFmt.nAvgBytesPerSec == 0x15888)
  {
    *outSndTime = (uint)(long)(s.timeBytes / g_sndTimeDiv_88200);
  }
  else if (s.snd->waveFmt.nAvgBytesPerSec == 0xac44)
  {
    *outSndTime = (uint)(long)(s.timeBytes / g_sndTimeDiv_44100);
  }
  else if (s.snd->waveFmt.nAvgBytesPerSec == 0x5622)
  {
    *outSndTime = (uint)(long)(s.timeBytes / g_sndTimeDiv_22050);
  }
  else if (s.snd->waveFmt.nAvgBytesPerSec == 0x2b11)
  {
    *outSndTime = (uint)(long)(s.timeBytes / g_sndTimeDiv_11025);
  }
  else
  {

    LeaveCriticalSection(&g_sndCs);
    return 8;
  }
  g_sndTimeSamples[g_sndTimeSampleIndex].prevTime = g_lastSndTime;
  g_sndTimeSamples[g_sndTimeSampleIndex].currTime = *outSndTime;
  g_sndTimeSampleIndex = g_sndTimeSampleIndex + 1;
  g_sndTimeSampleIndex = g_sndTimeSampleIndex & 0xff;
  if (*outSndTime < g_lastSndTime)
  {
    LeaveCriticalSection(&g_sndCs);
    return 5;
  }
  g_lastSndTime = *outSndTime;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}
#pragma pack(pop)

// FUNCTION: MAGSND 0x1000314D
undefined4 __cdecl GetSndState(int slot, undefined4 *outSndState)
{
  if ((0x10f < slot) || (slot < 0))
  {
    return 5;
  }
  EnterCriticalSection(&g_sndCs);
  if (g_sndSlots[slot] == (SndInstance *)0x0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 1;
  }
  if ((g_sndSlots[slot]->flags & 1) != 0)
  {
    *outSndState = 1;
  }
  else if (((g_sndSlots[slot]->flags >> 3) & 1) != 0)
  {
    *outSndState = 2;
  }
  else
  {
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
int __cdecl IsSndLoaded(int loadId, int *outSlot)

{
  SndInstance *activeNode;

  /* 0x321f  26  IsSndLoaded */
  if (loadId <= 0)
  {
    return 0;
  }
  EnterCriticalSection(&g_sndCs);
  activeNode = g_activeSndListHead;
  while (activeNode != NULL)
  {
    if (activeNode->loadId == loadId)
    {
      *outSlot = activeNode->slotIndex;
      LeaveCriticalSection(&g_sndCs);
      return 1;
    }
    activeNode = activeNode->activeNext;
  }
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x100032AD
undefined4 __cdecl GetLRUSnd(int *outSlot, int startSlot, int endSlot)
{
  struct
  {
    int selectedSlot;
    int slot;
    uint lowestLruCounter;
    int foundEmptySlot;
  } s;

  s.foundEmptySlot = 0;
  s.lowestLruCounter = -1;
  if (endSlot != 0 && endSlot > startSlot && endSlot < 0x100)
  {
    s.slot = startSlot;
  }
  else
  {
    s.slot = 0;
    endSlot = 0xff;
  }

  EnterCriticalSection(&g_sndCs);
  for (; s.slot < endSlot; s.slot = s.slot + 1)
  {
    if (g_sndSlots[s.slot] != 0)
    {
      if (g_sndSlots[s.slot]->lruCounter < s.lowestLruCounter)
      {
        s.lowestLruCounter = g_sndSlots[s.slot]->lruCounter;
        s.selectedSlot = g_sndSlots[s.slot]->slotIndex;
      }
    }
    else
    {
      s.selectedSlot = s.slot;
      s.foundEmptySlot = 1;
      break;
    }
  }
  *outSlot = s.selectedSlot;
  LeaveCriticalSection(&g_sndCs);
  return s.foundEmptySlot;
}

// FUNCTION: MAGSND 0x100033A2
void *__cdecl GetAVISndBuff(int slot, uint blockNumber)
{
  struct
  {
    void *lockPtr1;
    SndInstance *snd;
    undefined4 unusedZero;
    uint blockIndex;
    int hr;
    void *lockPtr2;
    uint lockBytes2;
    uint lockBytes1;
    int offset;
  } s;

  s.lockPtr1 = 0;
  s.lockPtr2 = 0;
  s.lockBytes1 = 0;
  s.lockBytes2 = 0;
  s.unusedZero = 0;

  if (slot > 0x10f || slot < 0x100)
  {
    return 0;
  }
  EnterCriticalSection(&g_sndCs);
  s.snd = g_sndSlots[slot];
  if (s.snd == 0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }
  if (((s.snd->flags >> 7) & 1) != 0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }

  s.blockIndex = blockNumber % s.snd->blockCount;
  s.offset = s.snd->blockBytes * s.blockIndex;
  s.hr = s.snd->dsBuffer->lpVtbl->Lock(
      s.snd->dsBuffer, s.offset, s.snd->blockBytes,
      &s.lockPtr1, &s.lockBytes1, &s.lockPtr2, &s.lockBytes2, 0);
  if (s.hr != 0)
  {
    return 0;
  }
  if (s.lockPtr2 != 0)
  {
    s.snd->dsBuffer->lpVtbl->Unlock(s.snd->dsBuffer, s.lockPtr1, s.lockBytes1, s.lockPtr2, s.lockBytes2);
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }
  s.snd->dsLockedPtr = s.lockPtr1;
  s.snd->flags = (int)s.snd->flags | 0x80;
  LeaveCriticalSection(&g_sndCs);
  return s.lockPtr1;
}

// FUNCTION: MAGSND 0x1000352C
undefined4 __cdecl ReleaseAVISndBuff(int slot)
{
  struct
  {
    int unused_18;
    SndInstance *snd;
    int unused_10;
    int unused_c;
    int unused_8;
    int unused_4;
  } s;

  s.unused_18 = 0;
  s.unused_c = 0;
  s.unused_4 = 0;
  s.unused_8 = 0;
  s.unused_10 = 0;

  if ((slot > 0x10f) || (slot < 0x100))
  {
    return 0;
  }

  EnterCriticalSection(&g_sndCs);
  s.snd = g_sndSlots[slot];
  if (s.snd == 0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 0;
  }
  if (((s.snd->flags >> 7) & 1) == 0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 0xe;
  }
  if (s.snd->dsLockedPtr == 0)
  {
    LeaveCriticalSection(&g_sndCs);
    return 0xf;
  }
  s.snd->dsBuffer->lpVtbl->Unlock(s.snd->dsBuffer, s.snd->dsLockedPtr, s.snd->blockBytes, 0, 0);
  s.snd->flags &= 0xffffff7f;
  s.snd->dsLockedPtr = 0;
  LeaveCriticalSection(&g_sndCs);
  return 0;
}

// FUNCTION: MAGSND 0x10003656
int __cdecl ResetAndPrimeSndInstance(SndInstance *snd)
{
  struct
  {
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
  if ((snd->flags2 >> 5 & 1) != 0)
  {
    PrimeAviAudio(snd, 0);
  }
  else
  {
    s.mmioInfo = &snd->mmioInfo;
    s.mmioInfo->pchNext = s.mmioInfo->pchEndRead;

    mmioSetInfo(snd->hMmio, s.mmioInfo, 0);

    s.result = PrimeDsBufferFromSource(snd, 0);
    if (s.result != 0)
    {
      return s.result;
    }
  }
  snd->flags = (int)snd->flags | 0x20;
  return 0;
}

// FUNCTION: MAGSND 0x10003748
void __cdecl UpdateMmioSnd(SndInstance *snd)
{
  struct
  {
    int silenceByte;
    uint playCursorBytes;
    void *lockPtr1;
    uint bufferedSourceBytes;
    uint silenceBytes;
    uint bytesUntilDataEnd;
    uint writeCursorBytes;
    uint pendingSourceBytes;
    uint sourceBytes;
    int hr;
    void *lockPtr2;
    size_t lockBytes2;
    size_t lockBytes1;
  } s;

  s.writeCursorBytes = 0;
  s.playCursorBytes = 0;
  s.sourceBytes = 0;
  s.bufferedSourceBytes = 0;
  s.bytesUntilDataEnd = 0;
  s.pendingSourceBytes = 0;
  s.lockPtr1 = (void *)0x0;
  s.lockPtr2 = (void *)0x0;
  s.lockBytes1 = 0;
  s.lockBytes2 = 0;
  s.silenceBytes = 0;

  EnterCriticalSection(&g_sndCs);
  snd->dsBuffer->lpVtbl->GetCurrentPosition(snd->dsBuffer, &s.playCursorBytes, &s.writeCursorBytes);
  snd->timeBytes += (s.playCursorBytes - snd->timeBytes & 0xffff);
  if (snd->dataBytes < snd->timeBytes)
  {
    snd->timeBytes -= snd->dataBytes;
  }
  if ((((snd->flags >> 4) & 1) != 0 && (0x10000 <= snd->readCursorBytes)) ||
      (((snd->flags >> 1) & 1) != 0 && (snd->volume == 0)))
  {
    snd->dsBuffer->lpVtbl->Stop(snd->dsBuffer);
    if (g_updateTimerUsers > 0 && --g_updateTimerUsers == 0)
    {
      StopUpdateTimer();
    }
    snd->flags &= 0xfffffffe;
    snd->flags &= 0xfffffffd;
    snd->flags = (int)snd->flags | 4;
    snd->flags &= 0xffffffdf;
    if (((snd->flags2 >> 4) & 1) != 0)
    {
      g_sndSlots[snd->slotIndex]->flags &= 0xffffffbf;
      g_sndSlots[snd->slotIndex]->flags &= 0xfffffffe;
    }
    LeaveCriticalSection(&g_sndCs);
  }
  else
  {
    s.sourceBytes = (int)(s.playCursorBytes - snd->ringCursorBytes & 0xffff);
    s.bytesUntilDataEnd = snd->dataBytes - snd->writeCursorBytes;
    if (((snd->flags >> 4) & 1) == 0)
    {
      s.bufferedSourceBytes = snd->writeCursorBytes - snd->readCursorBytes;
      if (0x10000 < s.bufferedSourceBytes)
      {
        _DAT_1000707c = _DAT_1000707c + 1;
      }
    }
    else
    {
      s.bufferedSourceBytes = 0;
    }
    s.pendingSourceBytes = s.bytesUntilDataEnd + s.bufferedSourceBytes;
    if (s.pendingSourceBytes == 0)
    {
      s.silenceBytes = s.sourceBytes;
      s.sourceBytes = 0U;
    }
    if ((s.sourceBytes == 0) && (s.silenceBytes == 0))
    {
      LeaveCriticalSection(&g_sndCs);
    }
    else
    {
      if (s.bufferedSourceBytes < s.sourceBytes)
      {
        s.sourceBytes = s.bufferedSourceBytes;
      }
      if (s.silenceBytes + s.sourceBytes != 0)
      {
        s.hr = snd->dsBuffer->lpVtbl->Lock(snd->dsBuffer, snd->ringCursorBytes,
                                           s.silenceBytes + s.sourceBytes, &s.lockPtr1, &s.lockBytes1,
                                           &s.lockPtr2, &s.lockBytes2, 0);
        if (s.hr != 0)
        {
          LeaveCriticalSection(&g_sndCs);
          return;
        }
        snd->flags = (int)snd->flags | 0x80;
        if (s.silenceBytes == 0)
        {
          mmioGetInfo(snd->hMmio, &snd->mmioInfo, 0);
          memmove(s.lockPtr1, snd->mmioInfo.pchNext, s.lockBytes1);
          snd->mmioInfo.pchNext += s.lockBytes1;
          if (s.lockPtr2 != (void *)0x0)
          {
            memmove(s.lockPtr2, snd->mmioInfo.pchNext, s.lockBytes2);
            snd->mmioInfo.pchNext += s.lockBytes2;
          }
          mmioSetInfo(snd->hMmio, &snd->mmioInfo, 0);
          s.bufferedSourceBytes -= s.sourceBytes;
          s.pendingSourceBytes -= s.sourceBytes;
          snd->readCursorBytes += s.sourceBytes;
        }
        else
        {
          if (snd->waveFmt.wBitsPerSample == 8)
          {
            s.silenceByte = 0x80;
          }
          else
          {
            s.silenceByte = 0;
          }
          memset(s.lockPtr1, s.silenceByte, s.lockBytes1);
          if (s.lockPtr2 != (void *)0x0)
          {
            memset(s.lockPtr2, s.silenceByte, s.lockBytes2);
          }
          snd->readCursorBytes += s.silenceBytes;
        }
        snd->ringCursorBytes = (snd->ringCursorBytes + s.silenceBytes + s.sourceBytes) & 0xffff;
        snd->dsBuffer->lpVtbl->Unlock(snd->dsBuffer, s.lockPtr1, s.lockBytes1, s.lockPtr2, s.lockBytes2);
        snd->flags &= 0xffffff7f;
      }
      if (((s.pendingSourceBytes != 0) && (s.bytesUntilDataEnd > 0)) &&
          ((s.bufferedSourceBytes == 0 || (s.bufferedSourceBytes < s.sourceBytes * 2))))
      {
        mmioAdvance(snd->hMmio, &snd->mmioInfo, 0);
        if (s.bytesUntilDataEnd < snd->mmioInfo.cchBuffer - s.bufferedSourceBytes)
        {
          snd->writeCursorBytes += s.bytesUntilDataEnd;
        }
        else
        {
          snd->writeCursorBytes += snd->mmioInfo.cchBuffer - s.bufferedSourceBytes;
        }
      }
      if (s.pendingSourceBytes == 0)
      {
        if ((snd->flags2 & 1) != 0)
        {
          snd->mmioInfo.pchNext = snd->mmioInfo.pchEndRead;
          mmioSetInfo(snd->hMmio, &snd->mmioInfo, 0);
          mmioSeek(snd->hMmio, snd->mmioBaseOffset, 0);
          mmioGetInfo(snd->hMmio, &snd->mmioInfo, 0);
          mmioAdvance(snd->hMmio, &snd->mmioInfo, 0);
          snd->writeCursorBytes = (uint)(snd->mmioInfo.pchNext - snd->mmioInfo.pchBuffer);
          if (snd->dataBytes < snd->writeCursorBytes)
          {
            snd->writeCursorBytes = snd->dataBytes;
          }
          snd->readCursorBytes = 0;
        }
        else if (((snd->flags >> 4) & 1) == 0)
        {
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
int __cdecl UpdateAviSnd(SndInstance *snd)
{
  struct
  {
    uint aviStartSample;
    uint playCursorBytes;
    void *lockPtr1;
    uint unused_3c;
    uint unused_38;
    uint unused_34;
    uint playBlockIndex;
    uint ringBlockIndex;
    long aviBytesRead;
    uint writeCursorBytes;
    uint unused_20;
    uint previousRingBlockIndex;
    long aviSamplesRead;
    uint blockBytes;
    int hr;
    void *lockPtr2;
    uint lockBytes2;
    uint lockBytes1;
  } s;

  s.writeCursorBytes = 0;
  s.playCursorBytes = 0;
  s.unused_3c = 0;
  s.unused_34 = 0;
  s.unused_20 = 0;
  s.lockPtr1 = (void *)0x0;
  s.lockPtr2 = (void *)0x0;
  s.lockBytes1 = 0;
  s.lockBytes2 = 0;
  s.unused_38 = 0;

  snd->dsBuffer->lpVtbl->GetCurrentPosition(snd->dsBuffer, &s.playCursorBytes, &s.writeCursorBytes);

  s.playBlockIndex = snd->timeBytes % snd->dsDesc.dwBufferBytes;
  if (s.playCursorBytes > s.playBlockIndex)
  {
    snd->timeBytes = snd->timeBytes + (s.playCursorBytes - s.playBlockIndex);
  }
  else
  {
    snd->timeBytes = snd->timeBytes + (snd->dsDesc.dwBufferBytes - s.playBlockIndex);
    snd->timeBytes = snd->timeBytes + s.playCursorBytes;
  }

  s.ringBlockIndex = s.playCursorBytes / snd->blockBytes;
  s.previousRingBlockIndex = snd->ringCursorBytes / snd->blockBytes;
  if (s.previousRingBlockIndex == s.ringBlockIndex)
  {
    return 0;
  }

  if (((((uint)snd->flags >> 4 & 1) != 0) && (snd->dsDesc.dwBufferBytes <= snd->readCursorBytes)) ||
      ((((uint)snd->flags >> 1 & 1) != 0) && (snd->volume == 0)))
  {
    snd->dsBuffer->lpVtbl->Stop(snd->dsBuffer);
    if (g_updateTimerUsers > 0 && --g_updateTimerUsers == 0)
    {
      StopUpdateTimer();
    }
    snd->flags = snd->flags & 0xfffffffe;
    snd->flags = snd->flags & 0xfffffffd;
    snd->flags = (int)snd->flags | 4;
    snd->flags = snd->flags & 0xffffffdf;
    snd->timeBytes = 0;
    return 0;
  }

  s.blockBytes = snd->blockBytes;
  if (((uint)snd->flags >> 4 & 1) == 0)
  {
    s.aviStartSample = snd->writeCursorBytes / snd->blockScale;
    s.hr = snd->dsBuffer->lpVtbl->Lock(snd->dsBuffer, snd->ringCursorBytes, s.blockBytes, &s.lockPtr1,
                                       &s.lockBytes1, &s.lockPtr2, &s.lockBytes2, 0);
    if (s.hr != 0)
    {
      return 10;
    }
    AVIStreamRead((PAVISTREAM)snd->streamOrMmio, (LONG)s.aviStartSample,
                  (LONG)(s.lockBytes1 / snd->blockScale), s.lockPtr1, (LONG)s.lockBytes1,
                  &s.aviBytesRead, &s.aviSamplesRead);
    if (s.lockPtr2 != (void *)0x0)
    {
      AVIStreamRead((PAVISTREAM)snd->streamOrMmio, (LONG)(s.aviSamplesRead + s.aviStartSample),
                    (LONG)(s.lockBytes2 / snd->blockScale), s.lockPtr2, (LONG)s.lockBytes2,
                    &s.aviBytesRead, &s.aviSamplesRead);
    }
    snd->ringCursorBytes = snd->ringCursorBytes + s.blockBytes;
    snd->ringCursorBytes = snd->ringCursorBytes % snd->dsDesc.dwBufferBytes;
    snd->writeCursorBytes = snd->writeCursorBytes + s.blockBytes;
    snd->readCursorBytes = snd->readCursorBytes + s.blockBytes;
    if ((uint)snd->dataBytes <= snd->writeCursorBytes)
    {
      snd->flags = (int)snd->flags | 0x10;
      snd->readCursorBytes = 0;
    }
    snd->dsBuffer->lpVtbl->Unlock(snd->dsBuffer, s.lockPtr1, s.lockBytes1, s.lockPtr2, s.lockBytes2);
    snd->flags = snd->flags & 0xffffff7f;
  }
  else
  {
    snd->readCursorBytes = snd->readCursorBytes + s.blockBytes;
  }

  return 0;
}

// FUNCTION: MAGSND 0x100040C9
undefined4 __cdecl AcquireFreeDsBuffer(SndInstance *snd, LPDIRECTSOUNDBUFFER *outBuffer)
{
  struct
  {
    uint status;
    LPDIRECTSOUNDBUFFER dupOut;
    int hr;
    LPDIRECTSOUNDBUFFER buffer;
    int index;
    LPDIRECTSOUNDBUFFER candidate;
  } s;

  s.buffer = snd->dsBuffer;
  s.candidate = s.buffer;
  s.hr = s.buffer->lpVtbl->GetStatus(s.buffer, &s.status);
  if (s.hr != 0)
  {
    return 9;
  }
  if ((s.status & 2) != 0)
  {
    return 9;
  }
  if ((s.status & 1) != 0)
  {
    for (s.index = 0; s.index < 0x10; s.index = s.index + 1)
    {
      s.buffer = snd->duplicateBuffers[s.index].buffer;
      if (s.buffer != NULL)
      {
        s.hr = s.buffer->lpVtbl->GetStatus(s.buffer, &s.status);
        if (s.hr != 0)
        {
          return 9;
        }
        if ((s.status & 1) != 0)
        {
          continue;
        }
        else
        {
          *outBuffer = s.buffer;
          return 0;
        }
      }
      else
      {
        s.hr = g_directSound->lpVtbl->DuplicateSoundBuffer(g_directSound, s.candidate, &s.dupOut);
        if (s.hr != 0)
        {
          return 9;
        }
        else
        {
          snd->duplicateBuffers[s.index].buffer = s.dupOut;
          *outBuffer = snd->duplicateBuffers[s.index].buffer;
          return 0;
        }
      }
    }
  }
  else
  {
    *outBuffer = s.candidate;
    return 0;
  }
  return 9;
}

// FUNCTION: MAGSND 0x10004239
void __cdecl AddToUpdateList(SndInstance *snd)

{
  if (g_updateSndHead == (SndInstance *)0x0)
  {
    g_updateSndHead = snd;
    g_updateSndTail = g_updateSndHead;
  }
  else
  {
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
  if (prev != (SndInstance *)0x0)
  {
    prev->updateNext = next;
  }
  else
  {
    g_updateSndHead = next;
  }
  if (next != (SndInstance *)0x0)
  {
    next->updatePrev = prev;
  }
  else
  {
    g_updateSndTail = prev;
  }
  return;
}

// FUNCTION: MAGSND 0x10004311
void __cdecl AddToActiveList(SndInstance *snd)

{
  if (g_activeSndListHead == (SndInstance *)0x0)
  {
    g_activeSndListHead = snd;
    g_activeSndTail = g_activeSndListHead;
  }
  else
  {
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
  if (prev != (SndInstance *)0x0)
  {
    prev->activeNext = next;
  }
  else
  {
    g_activeSndListHead = next;
  }
  if (next != (SndInstance *)0x0)
  {
    next->activePrev = prev;
  }
  else
  {
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
  if (timeGetDevCaps(&g_timeCaps, 8) != 0)
  {
    return 0xc;
  }
  timeBeginPeriod(g_updatePeriodMs);
  g_updateTimerId = timeSetEvent(g_updatePeriodMs, g_timeCaps.wPeriodMin, UpdateTimerProc, (DWORD)&g_sndHwnd, 1);
  if (g_updateTimerId == 0)
  {
    timeEndPeriod(g_updatePeriodMs);
    return 0xc;
  }
  g_updateTimerActive = 1;
  return 0;
}

// FUNCTION: MAGSND 0x1000448D
void StopUpdateTimer(void)

{
  if (g_updateTimerActive != 0)
  {
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
undefined4 __cdecl LoadFromFile(char *filename, SndInstance **outSnd)

{
  FILE *file;
  undefined4 status;

  file = fopen(filename, "rb");
  if (file == (FILE *)0x0)
  {
    return 7;
  }
  status = LoadWaveFromFileHandle(file, (int *)outSnd);
  fclose(file);
  return status;
}

// FUNCTION: MAGSND 0x1000454F
undefined4 __cdecl LoadWaveMmio(LPSTR filename, SndInstance **outSnd, undefined4 dwFlags)
{
  typedef struct SndInstanceSlice_t
  {
    MMCKINFO mmck;              /* 0x01c */
    uint markerCount;           /* 0x030 */
    SndInstance *markers[0x11]; /* 0x034 */
  } SndInstanceSlice;

  struct
  {
    SndInstanceSlice *sndInstanceHacked;
    MMCKINFO riffChunk;
    WAVEFORMATEX riffHeader; // ebp - 0x80
    byte padbytes[2];
    MMIOINFO pmmioinfo;
    undefined4 unused;   // ebp - 0x24
    DWORD riffChunkSize; // ebp - 0x20
    LONG mmioBaseOffset; // ebp - 0x1c
    MMCKINFO riff;       // ebp - 0x18
    HMMIO fileHandle;    // ebp - 0x4
  } s;

  s.unused = 0;
  memset(&s.pmmioinfo, 0, sizeof(MMIOINFO));
  s.pmmioinfo.cchBuffer = 0x10000;
  s.fileHandle = mmioOpenA(filename, &s.pmmioinfo, 0x10000);
  if (s.fileHandle == (HMMIO)0x0)
  {
    return 7;
  }
  s.riff.fccType = 0x45564157;
  if (mmioDescend(s.fileHandle, &s.riff, (MMCKINFO *)0x0, MMIO_FINDRIFF) != 0)
  {
    mmioClose(s.fileHandle, 0);
    return 8;
  }
  s.riffChunk.ckid = 0x20746d66;
  if (mmioDescend(s.fileHandle, &s.riffChunk, &s.riff, MMIO_FINDCHUNK) != 0)
  {
    mmioClose(s.fileHandle, 0);
    return 8;
  }
  s.riffChunkSize = s.riffChunk.cksize;
  if (mmioRead(s.fileHandle, &s.riffHeader, s.riffChunkSize) != s.riffChunkSize)
  {
    mmioClose(s.fileHandle, 0);
    return 8;
  }

  s.riffHeader.cbSize = 0;
  mmioAscend(s.fileHandle, &s.riffChunk, 0);
  s.riffChunk.ckid = 0x61746164;
  if (mmioDescend(s.fileHandle, &s.riffChunk, &s.riff, 0x10) != 0)
  {
    mmioClose(s.fileHandle, 0);
    return 8;
  }
  mmioGetInfo(s.fileHandle, &s.pmmioinfo, 0);
  mmioAdvance(s.fileHandle, &s.pmmioinfo, 0);
  s.mmioBaseOffset = s.pmmioinfo.lBufOffset;
  dwFlags = 0xe8;
  *outSnd = CreateSndInstance(g_directSound, 0x10000, &s.riffHeader, dwFlags);
  if (*outSnd != (SndInstance *)0x0)
  {
    (*outSnd)->mmioBaseOffset = s.mmioBaseOffset;
    (*outSnd)->hMmio = s.fileHandle;
    (*outSnd)->mmioInfo = s.pmmioinfo;
    (*outSnd)->dataBytes = s.riffChunk.cksize;
    (*outSnd)->streamOrMmio = operator_new(strlen(filename) + 1);
    if ((*outSnd)->streamOrMmio != (void *)0x0)
    {
      strcpy((char *)(*outSnd)->streamOrMmio, filename);
    }
    else
    {
      mmioClose(s.fileHandle, 0);
      return 3;
    }
    (*outSnd)->flags2 = (*outSnd)->flags2 & 0xffffffdf;
    (*outSnd)->flags2 = (int)(*outSnd)->flags2 | 2;
    (*outSnd)->blockBytes = 0;
    (*outSnd)->blockScale = 0;
    (*outSnd)->blockCount = 0;
  }
  else
  {
    mmioClose(s.fileHandle, 0);
    return 9;
  }
  mmioAscend(s.fileHandle, &s.riffChunk, 0);
  s.riffChunk.ckid = 0x20657563;

  if (mmioDescend(s.fileHandle, &s.riffChunk, &s.riff, 0x10) == 0)
  {
    // TODO: this is fuckin weird no?
    s.sndInstanceHacked = &(*outSnd)->mmck;
    memcpy(s.sndInstanceHacked, &s.riffChunk, sizeof(MMCKINFO));
    if (mmioRead(s.fileHandle, &s.sndInstanceHacked->markerCount, 4) != 4)
    {
      DestroySndInstance(*outSnd);
      mmioClose(s.fileHandle, 0);
      return 8;
    }
    s.sndInstanceHacked->markers[0] = operator_new(s.sndInstanceHacked->markerCount * 0x18);
    if (s.sndInstanceHacked->markers[0] == (void *)0x0)
    {
      DestroySndInstance(*outSnd);
      mmioClose(s.fileHandle, 0);
      return 3;
    }
    s.riffChunkSize = s.sndInstanceHacked->markerCount * 0x18;
    if (mmioRead(s.fileHandle, s.sndInstanceHacked->markers[0], s.riffChunkSize) != (LONG)s.riffChunkSize)
    {
      DestroySndInstance(*outSnd);
      mmioClose(s.fileHandle, 0);
      MAGSND_DELETE(s.sndInstanceHacked->markers[0]);
      return 8;
    }
  }
  return 0;
}

// FUNCTION: MAGSND 0x10004986
void __cdecl CloseMmioAndFreeSource(SndInstance *snd)
{
  struct
  {
    void *tmp4;
    void *tmp3;
    void *tmp2;
    void *tmp1;
  } s;

  mmioClose(snd->hMmio, 0);
  if (snd->markers[0] != 0)
  {
    s.tmp1 = snd->markers[0];
    s.tmp3 = s.tmp1;
    operator_delete(s.tmp3);
  }
  if (((snd->flags2 >> 5) & 1) == 0)
  {
    s.tmp2 = snd->streamOrMmio;
    s.tmp4 = s.tmp2;
    operator_delete(s.tmp4);
  }
  return;
}

// FUNCTION: MAGSND 0x100049FE
undefined4 __cdecl LoadWaveFromFileHandle(FILE *file, SndInstance **outSnd)
{
  struct
  {
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
    int bytes1;      // ebp - 0xc
    int bytesRead;   // ebp - 0x8
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

  fseek(file, s.seek0, 0);

  // This reads riffTag, riffSize, and waveTag
  fread(&s.riffTag, 1, 0xc, file);
  s.riffEnd = s.riffSize + s.seek0 + 8;
  if ((s.riffTag != 0x46464952) || (s.waveTag != 0x45564157))
  {
    return 8;
  }

  if (FindRiffChunk(file, 0x20746d66, s.seek0, (uint)s.riffEnd) == 0)
  {
    return 8;
  }
  fread(s.chunkHdr, 1, 8, file);
  fread(s.waveFmt, 1, 0x10, file);
  s.cbSize = 0;
  if (FindRiffChunk(file, 0x61746164, s.seek0, (uint)s.riffEnd) == 0)
  {
    return 8;
  }
  fread(s.chunkHdr, 1, 8, file);
  s.createFlags = 0xea;

  *outSnd = CreateSndInstance(g_directSound, s.chunkHdr[1], s.waveFmt, s.createFlags);
  //*outSnd = s.bytes1;
  if (*outSnd == 0)
  {
    return 9;
  }

  s.lockHr = (*outSnd)->dsBuffer->lpVtbl->Lock((*outSnd)->dsBuffer, 0, s.chunkHdr[1], &s.ptr1, &s.bytes1, &s.ptr2, &s.bytes2, 0);
  if (s.lockHr != 0)
  {
    DestroySndInstance(*outSnd);
    return 9;
  }
  s.bytesRead = fread(s.ptr1, 1, s.bytes1, file);
  if (s.bytesRead != s.bytes1)
  {
    DestroySndInstance(*outSnd);
    return 9;
  }
  s.lockHr = (*outSnd)->dsBuffer->lpVtbl->Unlock((*outSnd)->dsBuffer, s.ptr1, s.bytes1, s.ptr2, s.bytes2);
  if (s.lockHr != 0)
  {
    (*outSnd)->dsBuffer->lpVtbl->Release((*outSnd)->dsBuffer);
    MAGSND_DELETE(*outSnd);
    return 9;
  }
  return 0;
}

// FUNCTION: MAGSND 0x10004C8A
undefined4 __cdecl SeekMmioToSample(SndInstance *sndObj, int sampleIndex)
{
  struct
  {
    int seekDelta;
    int currentOffset;
    int targetOffset;
  } s;

  mmioGetInfo(sndObj->hMmio, (LPMMIOINFO)&sndObj->mmioInfo, 0);
  s.targetOffset = sndObj->waveFmt.nBlockAlign * sampleIndex + sndObj->mmioBaseOffset;
  s.currentOffset = MmioTellFromInfo(&sndObj->mmioInfo);
  s.seekDelta = s.targetOffset - s.currentOffset;
  mmioSeek(sndObj->hMmio, s.seekDelta, 1);
  mmioGetInfo(sndObj->hMmio, (LPMMIOINFO)&sndObj->mmioInfo, 0);
  mmioAdvance(sndObj->hMmio, (LPMMIOINFO)&sndObj->mmioInfo, 0);
  return 0;
}

// FUNCTION: MAGSND 0x10004D3F
undefined4 __cdecl FindRiffChunk(FILE *file, int chunkId, long searchStart, uint searchEnd)
{
  struct
  {
    int chunk[2];
    int pos;
    int hdr[3];
  } s;

  fseek(file, searchStart, 0);
  fread(s.hdr, 1, 0xc, file);
  if (s.hdr[0] == chunkId)
  {
    fseek(file, searchStart, 0);
    return 1;
  }
  while (1)
  {
    s.pos = ftell(file);
    fread(s.chunk, 1, 8, file);
    if (searchEnd <= (uint)s.pos)
    {
      fseek(file, searchStart, 0);
      return 0;
    }
    if (s.chunk[0] == chunkId)
    {
      break;
    }

    s.pos = ftell(file);
    if ((s.chunk[1] & 1) != 0)
    {
      s.chunk[1] = s.chunk[1] + 1;
    }
    fseek(file, s.chunk[1], 1);
  }

  fseek(file, s.pos, 0);
  return 1;
}

// FUNCTION: MAGSND 0x10004E4C
undefined4 __cdecl PrimeDsBufferFromSource(SndInstance *snd, int startSample)
{
  struct
  {
    int bytesRemainingInData; // ebp - 0x48
    int samplesReadAdvance;   // ebp - 0x44
    int blockAlignCopy;       // ebp - 0x40
    HPSTR lockPtr1;           // ebp - 0x3c
    int startOffsetBytes;     // ebp - 0x38
    int done;                 // ebp - 0x34
    int streamSampleCount;    // ebp - 0x30
    int bytesRemainingToFill; // ebp - 0x2c
    HPSTR writePtr;           // ebp - 0x28
    int sourceTotalBytes;     // ebp - 0x24
    int wrapWindowBytes;      // ebp - 0x20
    int sourceOffsetBytes;    // ebp - 0x1c
    HPSTR lockPtr2;           // ebp - 0x18
    int lockSize2;            // ebp - 0x14
    int lockHr;               // ebp - 0x10
    int bytesRead;            // ebp - 0xc
    int streamByteOffset;     // ebp - 0x8
    int lockSize1;            // ebp - 0x4
  } s;

  s.lockPtr1 = (HPSTR)0x0;
  s.lockPtr2 = (HPSTR)0x0;
  s.writePtr = (HPSTR)0x0;
  s.lockSize1 = 0;
  s.lockSize2 = 0;
  s.streamSampleCount = 0;
  s.done = 0;
  if (((snd->flags2 >> 5) & 1) == 0)
  {
    SeekMmioToSample(snd, startSample);
  }

  s.startOffsetBytes = (uint)snd->waveFmt.nBlockAlign * startSample;
  s.sourceOffsetBytes = s.startOffsetBytes;
  s.sourceTotalBytes = snd->dataBytes;
  s.bytesRemainingInData = s.sourceTotalBytes - s.sourceOffsetBytes;
  s.bytesRemainingToFill = 0x10000;
  s.samplesReadAdvance = 0;
  s.streamByteOffset = s.startOffsetBytes;

  if ((s.startOffsetBytes < 0) || (s.sourceTotalBytes < s.startOffsetBytes))
  {
    return 5;
  }

  s.lockHr = snd->dsBuffer->lpVtbl->Lock(snd->dsBuffer, 0, 0x10000, &s.lockPtr1, &s.lockSize1, &s.lockPtr2, &s.lockSize2, 0);
  if (s.lockHr != 0)
  {
    if (((snd->flags2 >> 5) & 1) != 0)
    {
      CleanupAviStream((undefined4 *)snd);
    }
    else
    {
      CloseMmioAndFreeSource(snd);
    }
    DestroySndInstance(snd);
    return 9;
  }

  s.writePtr = s.lockPtr1;
  while (s.done == 0)
  {
    if (s.bytesRemainingInData >= s.bytesRemainingToFill)
    {
      if (((snd->flags2 >> 5) & 1) != 0)
      {
        AVIStreamRead((PAVISTREAM)snd->streamOrMmio, startSample, 0x10000 / snd->waveFmt.nBlockAlign, s.writePtr, 0x10000, &s.bytesRead,
                      &s.blockAlignCopy);
      }
      else
      {
        s.bytesRead = mmioRead(snd->hMmio, s.writePtr, s.bytesRemainingToFill);
      }
      s.bytesRemainingToFill = s.bytesRemainingToFill - s.bytesRead;
      s.samplesReadAdvance += s.bytesRead;
      s.streamByteOffset += s.bytesRead;
      s.bytesRemainingInData = s.bytesRemainingInData - s.bytesRead;
      s.sourceOffsetBytes += s.bytesRead;
      startSample = startSample + s.blockAlignCopy;
      s.done = 1;
    }
    else
    {
      if (((snd->flags2 >> 5) & 1) != 0)
      {
        AVIStreamRead((PAVISTREAM)snd->streamOrMmio, startSample, 0x10000 / snd->waveFmt.nBlockAlign, s.writePtr, s.bytesRemainingInData, &s.bytesRead, 0);
      }
      else
      {
        s.bytesRead = mmioRead(snd->hMmio, s.writePtr, s.bytesRemainingInData);
      }
      s.writePtr = s.writePtr + s.bytesRead;
      s.bytesRemainingToFill = s.bytesRemainingToFill - s.bytesRead;
      s.bytesRemainingInData = 0;
      s.sourceOffsetBytes = snd->dataBytes;
      if ((snd->flags2 & 1) == 0)
      {
        memset(s.writePtr, 0, (size_t)s.bytesRemainingToFill);
        s.done = 1;
      }
      else
      {
        if (((snd->flags2 >> 5) & 1) == 0)
        {
          SeekMmioToSample(snd, 0);
        }
        s.sourceOffsetBytes = 0;
        s.bytesRemainingInData = snd->dataBytes;
        s.streamByteOffset = 0;
        s.samplesReadAdvance = 0;
      }
    }
  }

  mmioGetInfo(snd->hMmio, &snd->mmioInfo, 0);
  if (((snd->flags2 >> 5) & 1) != 0)
  {
    RefillAviMmioBuffer(snd, startSample);
  }
  else
  {
    mmioAdvance(snd->hMmio, &snd->mmioInfo, 0);
  }

  s.wrapWindowBytes = min(0x10000, s.bytesRemainingInData);
  snd->writeCursorBytes = snd->dataBytes - (s.bytesRemainingInData - s.wrapWindowBytes);
  snd->readCursorBytes = snd->dataBytes - s.bytesRemainingInData;
  snd->ringCursorBytes = 0;

  s.lockHr = snd->dsBuffer->lpVtbl->Unlock(snd->dsBuffer, s.lockPtr1, s.lockSize1, s.lockPtr2, s.lockSize2);
  if (s.lockHr != 0)
  {
    CloseMmioAndFreeSource(snd);
    DestroySndInstance(snd);
    return 9;
  }
  return 0;
}

// FUNCTION: MAGSND 0x1000524C
undefined4 __cdecl PrimeAviAudio(SndInstance *snd, int unused)
{
  struct
  {
    HPSTR lockPtr1;
    uint blockIndex;
    LONG aviBytesRead;
    uint samplesPerBlock;
    int lockBytesTotal;
    LONG aviSamplesRead;
    HPSTR writePtr;
    HPSTR lockPtr2;
    int lockBytes2;
    int streamSample;
    uint lockBytes1;
    int hr;
  } s;

  s.lockBytesTotal = snd->blockCount * snd->blockBytes;
  s.hr = snd->dsBuffer->lpVtbl->Lock(snd->dsBuffer, 0, s.lockBytesTotal, &s.lockPtr1, &s.lockBytes1, &s.lockPtr2, &s.lockBytes2, 0);
  if (s.hr != 0)
  {
    CleanupAviStream(snd);
    DestroySndInstance(snd);
    return 9;
  }
  if (s.lockPtr2 != 0)
  {
    snd->dsBuffer->lpVtbl->Unlock(snd->dsBuffer, s.lockPtr1, s.lockBytes1, s.lockPtr2, s.lockBytes2);
    CleanupAviStream(snd);
    DestroySndInstance(snd);
    return 9;
  }
  snd->ringCursorBytes = 0;
  snd->writeCursorBytes = 0;
  snd->readCursorBytes = 0;
  s.writePtr = s.lockPtr1;
  s.streamSample = 0;
  s.samplesPerBlock = snd->blockBytes / snd->blockScale;
  // s.hr = 0;
  for (s.blockIndex = 0; s.blockIndex < snd->blockCount; s.blockIndex = s.blockIndex + 1)
  {
    AVIStreamRead((PAVISTREAM *)snd->streamOrMmio, s.streamSample, s.samplesPerBlock, (void *)s.writePtr, snd->blockBytes, &s.aviBytesRead,
                  &s.aviSamplesRead);
    snd->ringCursorBytes = snd->ringCursorBytes + snd->blockBytes;
    snd->ringCursorBytes = snd->ringCursorBytes % snd->dsDesc.dwBufferBytes;
    snd->writeCursorBytes = snd->writeCursorBytes + snd->blockBytes;
    snd->readCursorBytes = snd->readCursorBytes + snd->blockBytes;
    s.writePtr += (int)snd->blockBytes;
    s.streamSample = s.streamSample + s.samplesPerBlock;
  }
  s.hr = snd->dsBuffer->lpVtbl->Unlock(snd->dsBuffer, s.lockPtr1, s.lockBytes1, s.lockPtr2, s.lockBytes2);
  if (s.hr != 0)
  {
    CloseMmioAndFreeSource((SndInstance *)snd);
    DestroySndInstance((SndInstance *)snd);
    return 9;
  }
  return 0;
}

// FUNCTION: MAGSND 0x10005480
SndInstance *__cdecl CreateSndInstance(IDirectSound *dSound, DWORD dwBufferBytes, WAVEFORMATEX *wfxFormat, DWORD dwFlags)
{
  struct
  {
    void *deletePtr2;
    void *deletePtr1;
    int hr;
    SndInstance *result;
  } s;

  s.result = (SndInstance *)operator_new(sizeof(SndInstance));
  memset(s.result, 0, sizeof(SndInstance));
  s.result->dsDesc.dwSize = 0x14;
  s.result->dsDesc.dwFlags = dwFlags;
  s.result->dsDesc.dwBufferBytes = dwBufferBytes;
  s.result->dsDesc.lpwfxFormat = wfxFormat;
  memcpy(&s.result->waveFmt, wfxFormat, sizeof(WAVEFORMATEX));

  s.hr = dSound->lpVtbl->CreateSoundBuffer(dSound, &s.result->dsDesc, &s.result->dsBuffer, 0);
  if (s.hr != 0)
  {
    s.deletePtr1 = s.result;
    s.deletePtr2 = s.deletePtr1;
    operator_delete(s.deletePtr2);
    return (SndInstance *)0x0;
  }
  else
    return s.result;
}

// FUNCTION: MAGSND 0x10005562
undefined4 __cdecl DestroySndInstance(SndInstance *snd)
{
  if (snd == (SndInstance *)0x0)
  {
    return 5;
  }

  snd->dsBuffer->lpVtbl->Release(snd->dsBuffer);
  MAGSND_DELETE(snd);
  return 0;
}

// FUNCTION: MAGSND 0x100055BB
undefined4 __cdecl LoadAviAudioStream(void *stream, SndInstance **outSnd, undefined4 scratch)

{
  struct
  {
    WAVEFORMATEX waveFmt;
    byte pad_e6[2];
    MMIOINFO mmioInfo;
    LONG firstReadBytes;
    int bufferBytes;
    int dataBytes;
    int formatSize;
    AVISTREAMINFOA streamInfo;
  } s;

  s.formatSize = 0x12;
  AVIStreamInfoA((PAVISTREAM)stream, &s.streamInfo, 0x8c);
  if (s.streamInfo.fccType != 0x73647561)
  {
    return 8;
  }
  AVIStreamRead((PAVISTREAM)stream, 0, 1, 0, 0, &s.firstReadBytes, 0);
  s.dataBytes = (int)s.streamInfo.dwSampleSize * (int)s.streamInfo.dwLength;
  AVIStreamReadFormat((PAVISTREAM)stream, 0, &s.waveFmt, (LONG *)&s.formatSize);
  s.waveFmt.cbSize = 0;
  scratch = 0xe8;
  s.bufferBytes = (int)s.streamInfo.dwSuggestedBufferSize * 0xb;
  *outSnd = CreateSndInstance(g_directSound, s.bufferBytes, &s.waveFmt, scratch);
  if (*outSnd != (SndInstance *)0x0)
  {
    (*outSnd)->mmioBaseOffset = 0;
    (*outSnd)->hMmio = (HMMIO)0;
    (*outSnd)->mmioInfo = s.mmioInfo;
    (*outSnd)->dataBytes = s.dataBytes;
    (*outSnd)->streamOrMmio = stream;
    (*outSnd)->flags2 = (int)(*outSnd)->flags2 | 0x20;
    (*outSnd)->flags2 = (int)(*outSnd)->flags2 | 2;
    (*outSnd)->blockBytes = s.streamInfo.dwSuggestedBufferSize;
    (*outSnd)->blockScale = s.streamInfo.dwSampleSize;
    (*outSnd)->blockCount = 0xb;
    (*outSnd)->timeBytes = 0;
  }
  else
    return 9;
  return 0;
}

// FUNCTION: MAGSND 0x10005770
void __cdecl RefillAviMmioBuffer(SndInstance *sndObj, long streamStartSample)
{
  struct
  {
    int unused_1c;
    int bytesToRead;
    int unused_14;
    int bytesRemainingInData;
    int unused_c;
    long aviBytesRead;
    size_t bufferedBytes;
  } s;

  s.aviBytesRead = 0;
  s.unused_c = 0;
  s.unused_14 = 0;
  s.unused_1c = 0;
  s.bufferedBytes = sndObj->writeCursorBytes - sndObj->readCursorBytes;
  if ((sndObj->mmioInfo.pchEndRead - sndObj->mmioInfo.pchNext) != s.bufferedBytes)
  {
    g_mmioInfoMismatchCount = g_mmioInfoMismatchCount + 1;
  }
  s.bytesRemainingInData = sndObj->dataBytes - sndObj->writeCursorBytes;
  s.bytesToRead = sndObj->mmioInfo.cchBuffer - s.bufferedBytes;
  if (s.bytesToRead > s.bytesRemainingInData)
  {
    s.bytesToRead = s.bytesRemainingInData;
  }
  if (s.bufferedBytes != 0)
  {
    memmove((void *)sndObj->mmioInfo.pchBuffer, (void *)sndObj->mmioInfo.pchNext, s.bufferedBytes);
  }
  sndObj->mmioInfo.pchNext = &sndObj->mmioInfo.pchBuffer[s.bufferedBytes];
  AVIStreamRead((PAVISTREAM)sndObj->streamOrMmio, streamStartSample,
                s.bytesToRead / (int)(uint)sndObj->waveFmt.nBlockAlign, sndObj->mmioInfo.pchNext, s.bytesToRead,
                &s.aviBytesRead, 0);
  sndObj->mmioInfo.pchEndRead = sndObj->mmioInfo.pchBuffer + sndObj->mmioInfo.cchBuffer;
  sndObj->mmioInfo.pchNext = sndObj->mmioInfo.pchBuffer;
  mmioSetInfo(sndObj->hMmio, &sndObj->mmioInfo, 0);
  return;
}

// FUNCTION: MAGSND 0x100058D0
int __cdecl MmioTellFromInfo(MMIOINFO *info) { return (int)info->lDiskOffset - ((int)info->pchEndRead - (int)info->pchNext); }

// FUNCTION: MAGSND 0x1000575E
undefined4 __cdecl CleanupAviStream(SndInstance *snd)
{
  (void)snd;
  return 0;
}

long __cdecl ftol(void)
{
  return 0;
}

void *__cdecl operator_new(unsigned int size)
{
  return malloc(size);
}

void __cdecl operator_delete(void *p)
{
  if (p)
    free(p);
}
