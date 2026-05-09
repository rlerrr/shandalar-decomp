#ifndef __MANALINKINTERFACE_H__
#define __MANALINKINTERFACE_H__

#include <windows.h>
#include "inttypes.h"

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport) BOOL InitFamInterfaceGpd(void);
__declspec(dllimport) BOOL CloseFamInterfaceGpd(void);
__declspec(dllimport) int FamInterface_Flush(void);
__declspec(dllimport) int FamInterface_SendPacket(void *packet_header, unsigned int packet_kind);
__declspec(dllimport) int FamInterface_ReceivePacket(unsigned int packet_kind, void *packet_storage);
__declspec(dllimport) int FamInterface_WaitForOpponent(unsigned int wait_kind);
__declspec(dllimport) int FamInterface_Taunt(void);
__declspec(dllimport) int FamInterface_SetDuelVersion(unsigned char major_version, unsigned char minor_version);
__declspec(dllimport) int FamInterface_SetDuelState(int duel_state);
__declspec(dllimport) int FamInterface_GetDuelState(unsigned int *out_duel_state);
__declspec(dllimport) int FamInterface_PostDuelResult(unsigned int duel_result);
__declspec(dllimport) BOOL FamInterface_UpdateScreenName(void);
__declspec(dllimport) BOOL FamInterface_EndSession(void);
__declspec(dllimport) BOOL FamInterface_GoneToMovies(void);
__declspec(dllimport) unsigned int FamInterface_IsManaLinkRunning(void);
__declspec(dllimport) unsigned int FamInterface_IsNetworkAlive(void);
__declspec(dllimport) unsigned int FamInterface_IsNetworkTEN(void);
__declspec(dllimport) unsigned int FamInterface_IsHost(void);
__declspec(dllimport) int FamInterface_HasOpponent(void);
__declspec(dllimport) int FamInterface_SendFile(unsigned int file_kind, char *filename);

#ifdef __cplusplus
}
#endif

#endif
