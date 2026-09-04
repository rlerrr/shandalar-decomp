#include <windows.h>
#include <mmsystem.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>
#include <errno.h>

#include "defs.h"
#include "shandalar.h"
#include "shandalar_internal.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/global_other.h"
#include "magic/src/game_support.h"
#include "magic/src/duel_engine.h"
#include "magic/src/shared_startup.h"
#include "shandalar_global_strings.h"
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/card_db.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

// FUNCTION: SHANDALAR 0x005019ad
void SaveGameToSlot(int save_slot_index)
{
  int save_drive_index;
#ifdef _DEBUG
  if (save_slot_index == 3)
  {
    // Disable auto-save in the debugger
    return;
  }
#endif
  g_save_errno = 0;
  global_saveload_loading = g_save_errno;
  HideMouseCursorNested();
  save_drive_index = GetSaveDriveIndex();
  if (save_drive_index != -1)
  {
    if (save_slot_index == -1)
    {
      ShowMouseCursorNested();
      save_slot_index = RunTextMenuAt(g_ui_message_buffer, 0x30, 0x20);
      HideMouseCursorNested();
    }

    if (save_slot_index != -1)
    {
      g_selected_save_slot_index = save_slot_index;
      g_save_file_path[7] = (char)int_to_hex_digit(save_slot_index);
      if (SaveGameWithMessage(g_save_file_path) != 0)
      {
        if (g_save_errno == 0)
        {
          strcpy(g_ui_message_buffer, " Game has been saved.\n");
        }
        else
        {
          strcpy(g_ui_message_buffer, " Game NOT saved.\n");
          FillGraphicsRect(g_page0_window_bounds, 0x40, 0x7f, 0xc0, 0x22, 0xc);
        }

        if (g_save_errno == 0xd)
        {
          strcat(g_ui_message_buffer, " Write access denied.\n");
        }

        if (g_save_errno == 0x1c)
        {
          strcat(g_ui_message_buffer, " Disk Full.\n");
        }
        strcat(g_ui_message_buffer, " Press key to continue.\n");
      }
    }
  }
  ShowMouseCursorNested();
}

// FUNCTION: SHANDALAR 0x005031a8
int RunLoadGameMenu(void)
{
  return RunLoadSaveMenu(0);
}

// FUNCTION: SHANDALAR 0x00501b7d
int GetSaveDriveIndex(void)
{
  char current_directory_buffer[0x100];
  int key;

  if (g_save_path_needs_init == -1)
  {
    GetCurrentDirectoryA(0x100, current_directory_buffer);
    g_save_file_path[0] = current_directory_buffer[0];
  }

  return tolower(g_save_file_path[0]) - 0x61;

  // The rest of this is unreachable
  FillGraphicsRect(g_page0_window_bounds, 0, 0, global_screen_width, global_screen_height, 0xf);

  do
  {
    if (global_saveload_loading != 0)
    {
      strcpy(g_ui_message_buffer, "  Which drive contains your\n    saved game files?\n\n            ");
    }
    else
    {
      strcpy(g_ui_message_buffer, "  Which drive contains your\n     Save Game disk?\n\n            ");
    }
    g_ui_message_buffer[strlen(g_ui_message_buffer)] = (char)(g_save_path_needs_init + 'A');
    strcat(g_ui_message_buffer, ":\n\n    Press drive letter and\nReturn when disk is inserted.\n");
    strcat(g_ui_message_buffer, "    Press Escape to cancel.\n");
    DrawWrappedLairText(g_ui_message_buffer, 0x63, 0x50, 0x48, 0);
    key = PopNormalizedQueuedKeyInput();

    if (key == 'A' || key == 'a')
    {
      g_save_path_needs_init = 0;
    }
    if (key == 'B' || key == 'b')
    {
      g_save_path_needs_init = 1;
    }
    if (key == 'C' || key == 'c')
    {
      g_save_path_needs_init = 2;
    }
    if (key == 'D' || key == 'd')
    {
      g_save_path_needs_init = 3;
    }
    if (key == 'E' || key == 'e')
    {
      g_save_path_needs_init = 4;
    }
    if (key == 'F' || key == 'f')
    {
      g_save_path_needs_init = 5;
    }
    if (key == 0x1b)
    {
      g_save_path_needs_init = -1;
    }

    FillGraphicsRect(g_page0_window_bounds, 0x50, 0x58, 0xa0, 0x18, 0xf);
  } while (key != 0xd && key != 0x1b);

  FillGraphicsRect(g_page0_window_bounds, 8, 8, 0x130, 0xb8, 0xf);
  if (g_save_path_needs_init != -1)
  {
    if (IsSaveDriveAvailable(g_save_path_needs_init) == 0)
    {
      strcpy(g_ui_message_buffer, "No Disk in Drive A.\n");
      g_ui_message_buffer[strlen(g_ui_message_buffer)] = (char)(g_save_path_needs_init + 'A');
      RunTextMenuAt(g_ui_message_buffer, 0x64, 0x50);
      return -1;
    }
  }

  g_save_file_path[0] = (char)(g_save_path_needs_init + 'a');
  return g_save_path_needs_init;
}

// FUNCTION: SHANDALAR 0x00501e32
int IsSaveDriveAvailable(int drive_index)
{
  return 0;
}

// FUNCTION: SHANDALAR 0x00501e44
int LoadGameFromPath(char *save_file_path)
{
  struct
  {
    int saved_clip_rect[4];
    FacemakerWindowBounds page4_bounds;
    FacemakerWindowBounds *page4_bounds_ptr;
    DIBSurface *page4_dib;
    int image_height;
    int image_width;
    int slot_index;
    int player_index;
  } s;

  strcpy(save_file_path + 9, "SVE");
  g_save_file_fd = _open(save_file_path, 0x8000);
  if (g_save_file_fd == -1)
  {
    strcpy(g_ui_message_buffer, "File Error: ");
    strcat(g_ui_message_buffer, save_file_path);
    strcat(g_ui_message_buffer, "\n");
    RunTextMenuAt(g_ui_message_buffer, 100, 0x50);
    return 0;
  }

  global_saveload_loading = 1;
  save_or_load_ver1();
  _close(g_save_file_fd);

  for (s.player_index = 0; s.player_index < 2; s.player_index = s.player_index + 1)
  {
    for (s.slot_index = 0; s.slot_index < 0x96; s.slot_index = s.slot_index + 1)
    {
      if (global_card_instances[s.player_index][s.slot_index].internal_card_id != -1)
      {
        g_active_cards_count[s.player_index] = s.slot_index;
      }
    }
  }

  if (g_duel_active == 0)
  {
    HideMouseCursorNested();
    strcpy(save_file_path + 9, "map");
    LoadPcxIntoPage(2, save_file_path);
    ShowMouseCursorNested();
  }

  s.page4_bounds.page_number = 4;
  s.page4_bounds.clip_left = 0;
  s.page4_bounds.clip_top = 0;
  s.page4_bounds.max_x = 800;
  s.page4_bounds.max_y = 600;
  s.page4_bounds.draw_shadow_enabled = 1;
  s.page4_bounds.text_color = 0xf;
  s.page4_bounds.unk_1c = 4;
  s.page4_bounds.font_slot = 0;
  s.page4_bounds_ptr = &s.page4_bounds;
  s.image_width = 0x89;
  s.image_height = 0xa9;
  if (g_facemaker_page4_dib == 0)
  {
    s.page4_dib = (DIBSurface *)CreateGraphicsPage(4, s.image_width * 2, s.image_height, 8);
    SetGraphicsPage(4, s.page4_dib);
  }
  else
  {
    g_graphics_pages[4] = g_facemaker_page4_dib;
  }

  PushGraphicsClipRect((AdvMenuRect *)s.saved_clip_rect, s.page4_bounds_ptr, 0, 0, s.image_width * 2, s.image_height);
  FillGraphicsRect(s.page4_bounds_ptr, 0, 0, s.image_width * 2, s.image_height, 0);
  strcpy(save_file_path + 9, "fce");
  LoadPcxIntoPage(4, save_file_path);
  BuildFacemakerPortraitSprites(&s.page4_bounds);
  LoadPcxIntoPage(4, save_file_path);
  g_facemaker_page4_bitmap = g_graphics_pages[4]->hBitmap;
  g_facemaker_page4_dib = g_graphics_pages[4];
  SelectObject(g_graphics_pages[4]->hTempDC, g_graphics_pages[4]->hPreviousBitmap);
  g_graphics_pages[4] = (DIBSurface *)0;
  return 1;
}

// FUNCTION: SHANDALAR 0x005020fe
int SaveGameToPath(char *save_file_path)
{
  struct
  {
    FacemakerWindowBounds page4_bounds;
    FacemakerWindowBounds *page4_bounds_ptr;
  } s;

  if (g_duel_active == 0)
  {
    strcpy(save_file_path + 9, "map");
    if (ExportGraphicsPage(2, save_file_path) != 0)
    {
      strcpy(g_ui_message_buffer, "Error writing map file.\n");
      RunTextMenuAt(g_ui_message_buffer, 4, 0x40);
      g_save_errno = 1;
      return 0;
    }
  }

  strcpy(save_file_path + 9, "SVE");
  g_save_file_fd = _open(save_file_path, 0x8301, 0x80);
  if (g_save_file_fd == -1)
  {
    strcpy(g_ui_message_buffer, "File Error: ");
    strcat(g_ui_message_buffer, save_file_path);
    strcat(g_ui_message_buffer, "\n");
    RunTextMenuAt(g_ui_message_buffer, 100, 0x50);
    return 0;
  }

  global_saveload_loading = 0;
  save_or_load_ver1();
  _close(g_save_file_fd);

  s.page4_bounds.page_number = 4;
  s.page4_bounds.clip_left = 0;
  s.page4_bounds.clip_top = 0;
  s.page4_bounds.max_x = 800;
  s.page4_bounds.max_y = 600;
  s.page4_bounds.draw_shadow_enabled = 1;
  s.page4_bounds.text_color = 0xf;
  s.page4_bounds.unk_1c = 4;
  s.page4_bounds.font_slot = 0;
  s.page4_bounds_ptr = &s.page4_bounds;

  g_graphics_pages[4] = g_facemaker_page4_dib;
  SelectObject(g_graphics_pages[4]->hTempDC, g_facemaker_page4_bitmap);

  strcpy(save_file_path + 9, "fce");
  ExportEncodedImage(4, 0, 0, g_facemaker_page4_dib->width, g_facemaker_page4_dib->height, 0, save_file_path);

  SelectObject(g_graphics_pages[4]->hTempDC, g_graphics_pages[4]->hPreviousBitmap);
  g_graphics_pages[4] = (DIBSurface *)0;

  if (g_save_errno != 0)
  {
    return 0;
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x00501aec
int SaveGameWithMessage(char *save_file_path)
{
  strcpy(g_ui_message_buffer, " ");
  strcat(g_ui_message_buffer, "\n ... save in progress.\n");
  SaveGameToPath(save_file_path);
  return 1;
}

// FUNCTION: SHANDALAR 0x005226e0
int LoadStatWinDllExports(void)
{
  int i;

  for (i = 0; i < 3; i = i + 1)
  {
    g_statwin_exports_by_ordinal[i] = 0;
  }

  g_statwin_dll_module = LoadLibraryA("statwin.dll");
  if (g_statwin_dll_module == (HANDLE)0)
  {
    return 1;
  }

  for (i = 0; i < 3; i = i + 1)
  {
    g_statwin_exports_by_ordinal[i] = (int)GetProcAddress((HMODULE)g_statwin_dll_module, (LPCSTR)((i + 1U) & 0xffff));
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004ed135
int RestoreAdventureUiPaletteAndFocus(void)
{
  SetFocus(g_main_window_hwnd);
  LoadPcxIntoPageNoPalette("advfac64.pic");
  ReadPalette("todpal.tr", (char *)0);
  SelectPalette(g_graphics_pages[0]->hTempDC, g_palette_handle, FALSE);
  RealizePalette(g_graphics_pages[0]->hTempDC);
  RecountDeckCardTotals();
  ClearInputAndWaitForMouseRelease();
  return 0;
}

// FUNCTION: SHANDALAR 0x0050318e
int RunSaveMenuAndSelectSlot(void)
{
  return RunLoadSaveMenu(1);
}

// FUNCTION: SHANDALAR 0x00522786
void UnloadStatWinDllExports(void)
{
  int i;

  if (g_statwin_dll_module != (HANDLE)0)
  {
    FreeLibrary((HMODULE)g_statwin_dll_module);
    g_statwin_dll_module = (HANDLE)0;
  }

  for (i = 0; i < 3; i = i + 1)
  {
    g_statwin_exports_by_ordinal[i] = 0;
  }
}

