#ifndef SHANDALAR_MAGSND_H
#define SHANDALAR_MAGSND_H

#include <windows.h>
#include "inttypes.h"

typedef struct Sound_t
{
  int field_0;
  int field_4;
  int field_8;
  int field_C;
  int field_10;
  int field_14;
  int field_18;
  int field_1C;
} Sound;

undefined4 __cdecl InitSnd(int hwnd_as_int, undefined4 unused, BYTE flags);
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
void * __cdecl GetAVISndBuff(int param_1, uint param_2);
undefined4 __cdecl ReleaseAVISndBuff(int param_1);
undefined4 __cdecl GetSndHWND(void);
undefined4 __cdecl IsSndLoaded(int num, undefined4 *out_loaded);
undefined4 __cdecl GetLRUSnd(int *out_num, int start, int end);

#endif
