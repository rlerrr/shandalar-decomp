#include <windows.h>
#include "magvid.h"

#define MAGVID_FN_COUNT 19

typedef int(__cdecl *PFN_InitVid)(int hwnd, HINSTANCE instance, int flags);
typedef void(__cdecl *PFN_ReleaseVid)(void);
typedef int(__cdecl *PFN_LoadAvi)(char *path, int *out_avi, AviPosition *position, unsigned int flags);
typedef int(__cdecl *PFN_UnloadAvi)(int avi);
typedef int(__cdecl *PFN_PlayAvi)(int avi);
typedef int(__cdecl *PFN_StopAvi)(int avi);
typedef int(__cdecl *PFN_SetVidCallback)(int avi, int callback);
typedef int(__cdecl *PFN_SetVidBackground)(int avi, int background);
typedef int(__cdecl *PFN_SetVidBackgroundToBmp)(int avi, char *path, int flags);
typedef int(__cdecl *PFN_SetVidBackgroundToDib)(int avi, int dib);
typedef int(__cdecl *PFN_DrawVidBackground)(int avi);
typedef int(__cdecl *PFN_UnusedVidFn)(void);
typedef int(__cdecl *PFN_SetVidForeground)(int avi, int foreground);
typedef int(__cdecl *PFN_PaintVid)(int avi);
typedef int(__cdecl *PFN_SetVidPos)(int avi, AviPosition *position);
typedef int(__cdecl *PFN_VidStatus)(int avi);
typedef int(__cdecl *PFN_LinkVids)(int avi, int linked_avi);
typedef int(__cdecl *PFN_SetVidThreadPriority)(int avi, int priority);
typedef int(__cdecl *PFN_SetVidTransparency)(int avi, int transparency);

typedef struct MagVidApiVTable
{
  PFN_InitVid InitVid;                                 // 0
  PFN_ReleaseVid ReleaseVid;                           // 1
  PFN_LoadAvi LoadAvi;                                 // 2
  PFN_UnloadAvi UnloadAvi;                             // 3
  PFN_PlayAvi PlayAvi;                                 // 4
  PFN_StopAvi StopAvi;                                 // 5
  PFN_SetVidCallback SetVidCallback;                   // 6
  PFN_SetVidBackground SetVidBackground;               // 7
  PFN_SetVidBackgroundToBmp SetVidBackgroundToBmp;     // 8
  PFN_SetVidBackgroundToDib SetVidBackgroundToDib;     // 9
  PFN_DrawVidBackground DrawVidBackground;             // 10
  PFN_UnusedVidFn UnusedVidFn;                         // 11
  PFN_SetVidForeground SetVidForeground;               // 12
  PFN_PaintVid PaintVid;                               // 13
  PFN_SetVidPos SetVidPos;                             // 14
  PFN_VidStatus VidStatus;                             // 15
  PFN_LinkVids LinkVids;                               // 16
  PFN_SetVidThreadPriority SetVidThreadPriority;       // 17
  PFN_SetVidTransparency SetVidTransparency;           // 18
} MagVidApiVTable;

typedef int(__cdecl *MagVidFn)(void);

// GLOBAL: STATWIN 0x10017d8c
static HMODULE g_magvid_module;
// GLOBAL: STATWIN 0x1000d718
static int g_video_dll_status;
// GLOBAL: STATWIN 0x1000d71c
static char *PTR_s_magvid_1000d71c = "magvid";
// GLOBAL: STATWIN 0x10017d40
static MagVidApiVTable g_magvid_vtable;
#define g_magvid_fns ((MagVidFn *)(void *)&g_magvid_vtable)
// GLOBAL: STATWIN 0x10017d90
int *g_magvid_delay_fn;

static void __cdecl clear_video_imports_table(void);

// FUNCTION: STATWIN 0x10003610
int __cdecl init_video_dll(int hwnd, HINSTANCE instance, int flags)
{
  int i;
  int result;

  result = 0;
  if (g_video_dll_status != 0)
  {
    return 6;
  }

  g_magvid_module = LoadLibraryA(PTR_s_magvid_1000d71c);
  if (g_magvid_module != 0)
  {
    for (i = 0; i < MAGVID_FN_COUNT; i++)
    {
      g_magvid_fns[i] = (MagVidFn)GetProcAddress(g_magvid_module, (LPCSTR)((i + 1U) & 0xffff));
      if (g_magvid_fns[i] == 0)
      {
        FreeLibrary(g_magvid_module);
        clear_video_imports_table();
        return 7;
      }
    }

    g_magvid_delay_fn = (int *)GetProcAddress(g_magvid_module, (LPCSTR)((i + 1U) & 0xffff));
  }
  else
  {
    return 7;
  }

  result = g_magvid_vtable.InitVid(hwnd, instance, flags);
  if (result != 0)
  {
    FreeLibrary(g_magvid_module);
    clear_video_imports_table();
    return result;
  }

  g_video_dll_status = 1;
  return 0;
}

// FUNCTION: STATWIN 0x1000373a
int __cdecl release_video_dll(void)
{
  if (g_video_dll_status == 0)
  {
    return 7;
  }

  g_video_dll_status = 0;
  g_magvid_vtable.ReleaseVid();
  FreeLibrary(g_magvid_module);
  clear_video_imports_table();
  return 0;
}

// FUNCTION: STATWIN 0x10003bae
static void __cdecl clear_video_imports_table(void)
{
  int i;

  for (i = 0; i < MAGVID_FN_COUNT; i++)
  {
    g_magvid_fns[i] = 0;
  }
}

// FUNCTION: STATWIN 0x10003784
int __cdecl load_avi(char *path, int *out_avi, AviPosition *position, unsigned int flags)
{
  if (g_video_dll_status == 0)
  {
    return 7;
  }
  return g_magvid_vtable.LoadAvi(path, out_avi, position, flags);
}

// FUNCTION: STATWIN 0x100037c4
int __cdecl unload_avi(int avi)
{
  if (g_video_dll_status == 0)
  {
    return 7;
  }
  return g_magvid_vtable.UnloadAvi(avi);
}

// FUNCTION: STATWIN 0x100037f8
int __cdecl play_avi(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.PlayAvi(avi);
}

// FUNCTION: STATWIN 0x10003839
int __cdecl stop_avi(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.StopAvi(avi);
}

// FUNCTION: STATWIN 0x1000387a
int __cdecl set_vid_callback(int avi, int callback)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.SetVidCallback(avi, callback);
}

// FUNCTION: STATWIN 0x100038bf
int __cdecl set_vid_background(int avi, int background)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.SetVidBackground(avi, background);
}

// FUNCTION: STATWIN 0x10003904
int __cdecl set_vid_background_to_bmp(int avi, char *path, int flags)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.SetVidBackgroundToBmp(avi, path, flags);
}

// FUNCTION: STATWIN 0x1000394d
int __cdecl paint_vid(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.PaintVid(avi);
}

// FUNCTION: STATWIN 0x1000398e
int __cdecl set_vid_pos(int avi, AviPosition *position)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.SetVidPos(avi, position);
}

// FUNCTION: STATWIN 0x100039d3
int __cdecl set_vid_background_to_dib(int avi, int dib)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.SetVidBackgroundToDib(avi, dib);
}

// FUNCTION: STATWIN 0x10003a18
int __cdecl set_vid_foreground(int avi, int foreground)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.SetVidForeground(avi, foreground);
}

// FUNCTION: STATWIN 0x10003a5d
int __cdecl set_vid_transparency(int avi, int transparency)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.SetVidTransparency(avi, transparency);
}

// FUNCTION: STATWIN 0x10003aa2
int __cdecl draw_vid_background(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.DrawVidBackground(avi);
}

// FUNCTION: STATWIN 0x10003ae3
int __cdecl vid_status(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.VidStatus(avi);
}

// FUNCTION: STATWIN 0x10003b24
int __cdecl link_vids(int avi, int linked_avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.LinkVids(avi, linked_avi);
}

// FUNCTION: STATWIN 0x10003b69
int __cdecl set_vid_thread_priority(int avi, int priority)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return g_magvid_vtable.SetVidThreadPriority(avi, priority);
}