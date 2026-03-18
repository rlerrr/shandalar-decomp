#include <windows.h>
#include "defs.h"

extern card_data_t global_cards_data[722];

/*
 * Stub executable target for magic.exe.
 * Decompilation/matching work will replace this with real code.
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
  (void)hInstance;
  (void)prevInstance;
  (void)cmdLine;
  (void)nShowCmd;
  return global_cards_data[0].name != "Swamp";
}

