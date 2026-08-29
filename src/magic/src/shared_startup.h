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
extern char global_savegame_path[0x108];
extern HDC g_shared_offscreen_dc;
extern HBITMAP g_shared_offscreen_bitmap;
extern HGDIOBJ g_shared_offscreen_prev_object;
extern void *g_shared_offscreen_bits;
extern int g_display_color_depth;
extern int g_is_win9x;
extern HANDLE g_mpstime_vxd_handle;
extern int g_shared_startup_lock_initialized;
extern CRITICAL_SECTION g_shared_startup_lock;
extern int g_shared_startup_completed;
extern CRITICAL_SECTION g_card_render_lock;
extern int g_shared_startup_allocation;

void set_global_base_directory(char *path);
int setup_paths_and_load_text_etc(char *message_buffer);
int CopyRawCardNamesAndRarities(void);
int InitMpstimeVxdTimer(void);
void FreeBaseTextBuffer(void);
void free_duel_interface_resource_buffer(void);
void LoadDuelInterfaceRegistryOptions(void);
int LoadRarityCsv(const char *filename);
unsigned int setup_shared_startup(void);
void ShutdownSharedStartupResources(void);
int ShutdownSharedStartup(void);

#ifdef __cplusplus
}
#endif

#endif
