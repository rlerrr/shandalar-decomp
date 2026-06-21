#include <windows.h>
#include "mystdbool.h"
#include "magsnd.h"

#define SND_MAX 26

// GLOBAL: DECKDLL 0x101054b8
static HMODULE global_hmodule_magsnd_dll;

// GLOBAL: DECKDLL 0x1003a864
// GLOBAL: MAGIC 0x00570f50
static int global_sound_status = 0; // 0 = not loaded, 1 = loaded and ok, 2 = loaded and error I think

// GLOBAL: DECKDLL 0x1003a868
static bool global_sound_unk1 = false;

// GLOBAL: DECKDLL 0x1003a860
static bool global_sound_unk2 = false;

// GLOBAL: DECKDLL 0x1003a86c
static char *PTR_s_magsnd_1003a86c = "magsnd";

typedef DWORD (__cdecl *PFN_InitSnd)(HWND hwnd, DWORD unused, BYTE flags);
typedef void  (__cdecl *PFN_ReleaseSnd)(void);
typedef int   (__cdecl *PFN_LoadSnd)(LPSTR path, int num, Sound *snd);
typedef DWORD (__cdecl *PFN_UnloadSnd)(int num);
typedef DWORD (__cdecl *PFN_UnloadAllSnds)(void);
typedef DWORD (__cdecl *PFN_PlaySnd)(int num, Sound *snd);
typedef int   (__cdecl *PFN_PlaySndFile)(LPSTR path, int num, Sound *snd);
typedef DWORD (__cdecl *PFN_StopSnd)(int num);
typedef void  (__cdecl *PFN_StopAllSnds)(void);
typedef DWORD (__cdecl *PFN_PlayMidiFile)(void);
typedef DWORD (__cdecl *PFN_SetPitch)(int num, DWORD pitch);
typedef DWORD (__cdecl *PFN_GetPitch)(void);
typedef DWORD (__cdecl *PFN_SetVol)(int num, unsigned int vol);
typedef DWORD (__cdecl *PFN_GetVol)(void);
typedef DWORD (__cdecl *PFN_SetPan)(int num, int pan);
typedef DWORD (__cdecl *PFN_GetPan)(void);
typedef DWORD (__cdecl *PFN_UpdateSnd)(void);
typedef int   (__cdecl *PFN_SetSndMarker)(int num, unsigned int marker);
typedef int   (__cdecl *PFN_PlaySndMarker)(int num, unsigned int marker);
typedef DWORD (__cdecl *PFN_GetSndTime)(int num, unsigned int *out_time);
typedef DWORD (__cdecl *PFN_ResetSnd)(void);
typedef DWORD (__cdecl *PFN_GetSndState)(int num, DWORD *out_state);
typedef void *(__cdecl *PFN_GetAVISndBuff)(int param_1, unsigned int param_2);
typedef DWORD (__cdecl *PFN_ReleaseAVISndBuff)(int param_1);
typedef HWND  (__cdecl *PFN_GetSndHWND)(void);
typedef DWORD (__cdecl *PFN_IsSndLoaded)(int num, DWORD *out_loaded);
typedef DWORD (__cdecl *PFN_GetLRUSnd)(int *out_num, int start, int end);

typedef struct SndApiVTable {
    PFN_InitSnd            InitSnd;
    PFN_ReleaseSnd         ReleaseSnd;
    PFN_LoadSnd            LoadSnd;
    PFN_UnloadSnd          UnloadSnd;
    PFN_UnloadAllSnds      UnloadAllSnds;
    PFN_PlaySnd            PlaySnd;
    PFN_PlaySndFile        PlaySndFile;
    PFN_StopSnd            StopSnd;
    PFN_StopAllSnds        StopAllSnds;
    PFN_PlayMidiFile       PlayMidiFile;
    PFN_SetPitch           SetPitch;
    PFN_GetPitch           GetPitch;
    PFN_SetVol             SetVol;
    PFN_GetVol             GetVol;
    PFN_SetPan             SetPan;
    PFN_GetPan             GetPan;
    PFN_UpdateSnd          UpdateSnd;
    PFN_SetSndMarker       SetSndMarker;
    PFN_PlaySndMarker      PlaySndMarker;
    PFN_GetSndTime         GetSndTime;
    PFN_ResetSnd           ResetSnd;
    PFN_GetSndState        GetSndState;
    PFN_GetAVISndBuff      GetAVISndBuff;
    PFN_ReleaseAVISndBuff  ReleaseAVISndBuff;
    PFN_GetSndHWND         GetSndHWND;
    PFN_IsSndLoaded        IsSndLoaded;
    PFN_GetLRUSnd          GetLRUSnd;
} SndApiVTable;

// GLOBAL: DECKDLL 0x101054c0
// GLOBAL: MAGVID 0x10028a50
// GLOBAL: STATWIN 0x10017da0
// GLOBAL: MAGIC 0x007775d0
static SndApiVTable global_sound_vtable;
typedef int (WINAPI *Int_fn_etc)();
#define global_sound_fns ((Int_fn_etc*)(void*)&global_sound_vtable)

// FUNCTION: DECKDLL 0x1002d4d3
// FUNCTION: MAGIC 0x004859f1
int sound_unload(int idx)
{
  if (global_sound_status ==0)
    return 4;

  global_sound_vtable.UnloadSnd(idx);
  return;
}

// FUNCTION: DECKDLL 0x1002d421
// FUNCTION: MAGIC 0x0048593f
void sound_close() {
  if (!global_sound_status)
    return;

  global_sound_status = 0;

  if (global_sound_unk1 && !global_sound_unk2)
    global_sound_vtable.ReleaseSnd();

  FreeLibrary(global_hmodule_magsnd_dll);

  clear_sound_imports_table();

  global_hmodule_magsnd_dll = NULL;
  global_sound_unk2 = false;
  global_sound_unk1 = false;
}

// FUNCTION: DECKDLL 0x1002d534
// FUNCTION: MAGIC 0x00485a52
int sound_play(int num, Sound *snd)
{
  if ((global_sound_status == 0) || (global_sound_status == 2)) {
    return 4;
  }

  global_sound_vtable.PlaySnd(num,snd);
  return;
}

// FUNCTION: DECKDLL 0x1002d5c2
int sound_stop(int a1)
{
  if (!global_sound_status || global_sound_status == 2) 
    return 4;

  global_sound_vtable.StopSnd(a1);
  return;
}

// FUNCTION: DECKDLL 0x1002daee
// FUNCTION: MAGIC 0x0048600c
void clear_sound_imports_table(void)
{
  int i;

  for (i = 0; i < SND_MAX + 1; ++i)
    global_sound_fns[i] = NULL;
}

// FUNCTION: DECKDLL 0x1002d2c0
// FUNCTION: MAGVID 0x10008010
// FUNCTION: STATWIN 0x10002da0
// FUNCTION: MAGIC 0x004857e0
// FUNCTION: SHANDALAR 0x0056cf20
int init_sound_dll(HWND hwnd, int a2, int a3)
{
  int i;
  int result = 0;

  if (global_sound_status)
    return 2;

  global_hmodule_magsnd_dll = LoadLibrary(PTR_s_magsnd_1003a86c);

  if (global_hmodule_magsnd_dll) {
    for (i = 0; i < SND_MAX + 1; ++i) {
      if (!(global_sound_fns[i] = GetProcAddress(global_hmodule_magsnd_dll, (i + 1) & 0xffff))) {
        FreeLibrary(global_hmodule_magsnd_dll);
        clear_sound_imports_table();
        return 4;
      }
    }
  }
  else
    return 4;

  if (hwnd || (a3 & 2)) {
    if ((result = global_sound_vtable.InitSnd(hwnd, a2, a3))) {
      FreeLibrary(global_hmodule_magsnd_dll);
      clear_sound_imports_table();
      return result;    
    }
    
    global_sound_unk1 = true;
  } else {
    FreeLibrary(global_hmodule_magsnd_dll);
    clear_sound_imports_table();
    return 5;
  }

  if (a3 & 2)
    global_sound_unk2 = true;
  global_sound_status = 1;

  return 0;
}

// FUNCTION: DECKDLL 0x1002d497
// FUNCTION: MAGIC 0x004859b5
int sound_load(const char *path, int num, Sound *snd)
{
  if (global_sound_status == 0)
    return 4;

  global_sound_vtable.LoadSnd(path, num, snd);
  return;
}

// FUNCTION: DECKDLL 0x1002d850
int set_sound_loop(int num, int num2)
{
  if (global_sound_status == 0 || global_sound_status == 2)
    return 4;

  global_sound_vtable.SetSndMarker(num, num2);
  return;
}

// FUNCTION: MAGIC 0x00485f81
int sound_is_loaded(int sound_id, int *out_loaded_sound_num)
{
  if ((global_sound_status == 0) || (global_sound_status == 2)) {
    return 0;
  }

  return global_sound_vtable.IsSndLoaded(sound_id, (DWORD *)out_loaded_sound_num);
}

// FUNCTION: MAGIC 0x00485fc3
int sound_get_lru(int *out_num, int start, int end)
{
  if ((global_sound_status == 0) || (global_sound_status == 2)) {
    return 4;
  }

  return global_sound_vtable.GetLRUSnd(out_num, start, end);
}
