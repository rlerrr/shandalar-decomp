#ifndef MAGVID_H
#define MAGVID_H

#include <windows.h>

enum MagVidResult
{
  MAGVID_OK = 0,
  MAGVID_ERROR = 1,
  MAGVID_INVALID_ARGUMENT = 2,
  MAGVID_NO_FREE_SLOT = 3,
  MAGVID_NOT_LOADED = 5,
  MAGVID_THREAD_ERROR = 8
};

enum MagVidLoadFlags
{
  MAGVID_USE_EXISTING_WINDOW = 4
};

struct MagVidPosition
{
  short x;
  short y;
};

class MagDib;
class AviPlayer;

struct MagVid
{
  int index;                    // 0x00
  unsigned int flags;           // 0x04
  AviPlayer *player;            // 0x08
  int reserved_0c;              // 0x0c
  HWND window;                  // 0x10
  int owns_window;              // 0x14
  HWND parent_window;           // 0x18
  HDC device_context;           // 0x1c
  CRITICAL_SECTION lock;        // 0x20
  int playing;                  // 0x38
  int stop_requested;           // 0x3c
  HANDLE thread;                // 0x40
  DWORD thread_id;              // 0x44
  int thread_active;            // 0x48
  int reserved_4c;              // 0x4c
  MagDib *background;           // 0x50
  int owns_background;          // 0x54
  int x;                        // 0x58
  int y;                        // 0x5c
  int reserved_60;              // 0x60
  MagVid *linked_video;         // 0x64
  int is_linked;                // 0x68
  int callback;                 // 0x6c
};

#ifdef __cplusplus
extern "C" {
#endif

int __cdecl InitVid(HWND owner, HINSTANCE instance, unsigned int flags);
int __cdecl ReleaseVid(void);
int __cdecl LoadAVI(const char *path, int *out_video, MagVidPosition *position,
                    unsigned int flags);
int __cdecl UnloadAVI(int video);
int __cdecl PlayAVI(int video);
int __cdecl StopAVI(int video);
int __cdecl SetVidCallBack(int video, int callback);
int __cdecl SetVidBackground(int background, int video);
int __cdecl SetVidBackgroundToBMP(BITMAPINFOHEADER *bitmap, void *bits,
                                  int video);
int __cdecl SetVidBackgroundToDIB(MagDib *dib, int video);
int __cdecl DrawVidBackground(int video);
int __cdecl PasteToVidBackground(MagDib *dib, MagVidPosition *position,
                                 RECT *source, int video);
int __cdecl SetVidForeground(MagDib *dib, int video);
int __cdecl PaintVid(int video);
int __cdecl SetVidPos(int video, MagVidPosition *position);
int __cdecl VidStatus(int video);
int __cdecl LinkVids(int video, int linked_video);
int __cdecl SetVidThreadPriority(int video, int priority);
int __cdecl SetVidTransparency(int video, int transparency);

extern int _delay;

#ifdef __cplusplus
}
#endif

#endif
