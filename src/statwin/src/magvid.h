#ifndef STATWIN_MAGVID_H
#define STATWIN_MAGVID_H

#include <windows.h>

typedef struct AviPosition_t
{
  short x;
  short y;
} AviPosition;

#ifdef __cplusplus
extern "C"
{
#endif

int __cdecl init_video_dll(int hwnd, HINSTANCE instance, int flags);
int __cdecl release_video_dll(void);
int __cdecl load_avi(char *path, int *out_avi, AviPosition *position, unsigned int flags);
int __cdecl play_avi(int avi);
int __cdecl stop_avi(int avi);
int __cdecl unload_avi(int avi);
int __cdecl set_vid_callback(int avi, int callback);
int __cdecl set_vid_background(int avi, int background);
int __cdecl set_vid_background_to_bmp(int avi, char *path, int flags);
int __cdecl paint_vid(int avi);
int __cdecl set_vid_pos(int avi, AviPosition *position);
int __cdecl set_vid_background_to_dib(int avi, int dib);
int __cdecl set_vid_foreground(int avi, int foreground);
int __cdecl draw_vid_background(int avi);
int __cdecl link_vids(int avi, int linked_avi);
int __cdecl set_vid_thread_priority(int avi, int priority);
int __cdecl set_vid_transparency(int avi, int transparency);
int __cdecl vid_status(int avi);

extern int *g_magvid_delay_fn;

#ifdef __cplusplus
}
#endif

#endif
