#ifndef DECKDLL_H	// -*- tab-width:8; c-basic-offset:2; -*-
#define DECKDLL_H 1
// DeckDll: standalone and integrated deckbuilder.

#include <string.h>
#include <math.h>
#include <stdio.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "../defs.h"
#include "resources.h"


typedef struct GlobalDeckEntry_t
{
  csvid_t GDE_csvid;
  iid_t GDE_iid;
  int GDE_Available;
  int GDE_DecksBits;
} GlobalDeckEntry;

typedef struct DeckEntry_t
{
  csvid_t DeckEntry_csvid;
  int DeckEntry_Amount;
  const char* DeckEntry_FullName;
} DeckEntry;

#ifdef __cplusplus
extern "C" {
#endif
  // Our exports
  BOOL WINAPI DllEntryPoint(HINSTANCE dll, DWORD reason, LPVOID reserved);
  BOOL WINAPI DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved);
  WPARAM WINAPI DeckBuilderMain(HWND parent_hwnd, int db_flags_1, int db_flags_2);
  WPARAM deckbuilder_main(HWND parent_hwnd, int db_flags_1, int db_flags_2);
  void Deckdll_initialize_for_shandalar(const card_ptr_t* i_raw_cards_ptr,
					int i_available_slots,
					char* i_card_coded,
					int (*check_card_count_fn)(const DeckEntry*, int, int),
					int (*is_valid_card_fn)(int),
					BOOL (*colors_match_fn)(iid_t, color_test_t),
					int (*check_colors_inout_edited_deck_fn)(const GlobalDeckEntry*, int, BOOL));

#ifdef __cplusplus
};
#endif

#endif
