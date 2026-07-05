#ifndef SHARED_STARTUP_H
#define SHARED_STARTUP_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char global_playdeck_path[0x110];
extern char global_faces_path[0x110];
extern char global_duelart_path[0x110];
extern char global_duelsounds_path[0x110];
extern char global_savegame_path[0x110];
extern HDC DAT_00789310;
extern HBITMAP DAT_00926808;
extern HGDIOBJ DAT_0093a980;
extern void *DAT_008cefb0;
extern int DAT_00939334;
extern int DAT_00638ca8;
extern HANDLE DAT_00776518;
extern int DAT_00715fa0;
extern CRITICAL_SECTION DAT_00926910;
extern int DAT_007a7d7c;
extern CRITICAL_SECTION DAT_009266b0;
extern int DAT_00637a94;

void set_global_base_directory(char *path);
int setup_paths_and_load_text_etc(char *message_buffer);
int FUN_00509210(void);
int FUN_004a7b3d(void);
void FUN_00442f6a(void);
void FUN_004432ff(void);
void FUN_004a5d72(void);
int FUN_004c0c20(const char *filename);
unsigned int setup_shared_startup(void);
void FUN_00422bea(void);
int FUN_00469099(void);

#ifdef __cplusplus
}
#endif

#endif
