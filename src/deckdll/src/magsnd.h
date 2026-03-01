#ifndef MAGSND_H
#define MAGSND_H

#include <windows.h>

typedef struct Sound_t
{ // very little idea what any of these fields do.
  int volume; // 0x00
  int sampleRate; // 0x04
  int pan; // 0x08
  int field_C; // 0x0c
  int field_10; // 0x10
  int field_14; // 0x14
  int loadId; // 0x18
  unsigned int flags; // 0x1c
} Sound;

int sound_unload(int idx);
void sound_close();
int sound_play(int num, Sound *snd);
int sound_stop(int a1);
void clear_sound_imports_table(void);
int init_sound_dll(HWND hwnd, int a2, int a3);
int sound_load(const char *path, int num, Sound *snd);
int set_sound_loop(int num, int num2);

#endif