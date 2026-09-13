#ifndef __CARDARTLIB_H__
#define __CARDARTLIB_H__

#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
	__declspec(dllimport) int LoadBigArt(card_id_t id, int version, int width, int height);
	__declspec(dllimport) int LoadSmallArt(card_id_t id, int version, int width, int height);
	__declspec(dllimport) void DestroyBigArt(card_id_t id, int version);
	__declspec(dllimport) void DestroySmallArt(card_id_t id, int version);
	__declspec(dllimport) int IsBigArtRightSize(card_id_t id, int version, int width, int height);
	__declspec(dllimport) int IsSmallArtRightSize(card_id_t id, int version, int width, int height);
	__declspec(dllimport) void DestroyAllBigArts(void);
	__declspec(dllimport) void DestroyAllSmallArts(void);
	__declspec(dllimport) void *IsBigArtIn(card_id_t id, int version);
	__declspec(dllimport) int IsSmallArtIn(card_id_t id, int version);
	__declspec(dllimport) int ReloadBigArtIfWrongSize(card_id_t id, int version, int width, int height);
	__declspec(dllimport) int ReloadSmallArtIfWrongSize(card_id_t id, int version, int width, int height);
	__declspec(dllimport) int DrawBigArt(HDC hdc, const RECT* rect, card_id_t id, int version);
	__declspec(dllimport) int DrawSmallArt(HDC hdc, const RECT* rect, card_id_t id, int version);
	int WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID reserved);
#ifdef __cplusplus
}
#endif

#endif
