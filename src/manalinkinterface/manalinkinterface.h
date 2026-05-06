#ifndef __MANALINKINTERFACE_H__
#define __MANALINKINTERFACE_H__

#include <windows.h>
#include "inttypes.h"

#ifdef __cplusplus
extern "C" {
#endif

int WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID reserved);

BOOL InitFamInterfaceGpd(void);
BOOL CloseFamInterfaceGpd(void);
int FamInterface_Flush(void);
int __cdecl FamInterface_SendPacket(void *packet_header, unsigned int packet_kind);
int __cdecl FamInterface_ReceivePacket(unsigned int packet_kind, void *packet_storage);
int __cdecl FamInterface_WaitForOpponent(unsigned int wait_kind);
int FamInterface_Taunt(void);
int __cdecl FamInterface_SetDuelVersion(unsigned char major_version, unsigned char minor_version);
int __cdecl FamInterface_SetDuelState(int duel_state);
int __cdecl FamInterface_GetDuelState(unsigned int *out_duel_state);
int __cdecl FamInterface_PostDuelResult(unsigned int duel_result);
BOOL FamInterface_UpdateScreenName(void);
BOOL FamInterface_EndSession(void);
BOOL FamInterface_GoneToMovies(void);
unsigned int FamInterface_IsManaLinkRunning(void);
unsigned int FamInterface_IsNetworkAlive(void);
unsigned int FamInterface_IsNetworkTEN(void);
unsigned int FamInterface_IsHost(void);
int FamInterface_HasOpponent(void);
int __cdecl FamInterface_SendFile(unsigned int file_kind, char *filename);

#ifdef __cplusplus
}
#endif

#endif
