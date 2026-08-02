#ifndef SHANDALAR_MAGSND_H
#define SHANDALAR_MAGSND_H

#include <windows.h>
#include "inttypes.h"

typedef struct Sound_t
{
  int volume; // 0x00
  int sampleRate; // 0x04
  int pan; // 0x08
  int field_C; // 0x0c
  int field_10; // 0x10
  int field_14; // 0x14
  int loadId; // 0x18
  int flags; // 0x1c
} Sound;

undefined4 __cdecl InitSnd(HWND hwnd, undefined4 unused, BYTE flags);
void __cdecl ReleaseSnd(void);
int __cdecl LoadSnd(LPSTR path, int num, Sound *snd);
undefined4 __cdecl UnloadSnd(int num);
undefined4 __cdecl UnloadAllSnds(void);
undefined4 __cdecl PlaySnd(int num, Sound *snd);
int __cdecl PlaySndFile(LPSTR path, int num, Sound *snd);
undefined4 __cdecl StopSnd(int num);
void __cdecl StopAllSnds(void);
undefined4 __cdecl PlayMidiFile(void);
undefined4 __cdecl SetPitch(int num, undefined4 pitch);
undefined4 __cdecl GetPitch(void);
undefined4 __cdecl SetVol(int num, uint vol);
undefined4 __cdecl GetVol(void);
undefined4 __cdecl SetPan(int num, int pan);
undefined4 __cdecl GetPan(void);
undefined4 __cdecl UpdateSnd(void);
int __cdecl SetSndMarker(int num, uint marker);
int __cdecl PlaySndMarker(int num, uint marker);
undefined4 __cdecl GetSndTime(int num, uint *out_time);
undefined4 __cdecl ResetSnd(void);
undefined4 __cdecl GetSndState(int num, undefined4 *out_state);
void * __cdecl GetAVISndBuff(int slot, uint blockNumber);
undefined4 __cdecl ReleaseAVISndBuff(int slot);
HWND __cdecl GetSndHWND(void);
int __cdecl IsSndLoaded(int loadId, int *out_slot);
undefined4 __cdecl GetLRUSnd(int *out_num, int start, int end);

#endif
