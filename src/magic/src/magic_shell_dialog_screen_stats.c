#include "magic_shell_screen_name.h"
#include "magic_shell_dialogs.h"
#include <stdio.h>
#include <stdlib.h>

// FUNCTION: MAGIC 0x0048ffe8
void shell_apply_screen_name_profile(HWND hwnd)
{
  struct
  {
    union
    {
      WPARAM selection;
      int skill_index;
    } value;
    char text[100];
  } s;
  s.value.selection = SendDlgItemMessageA(hwnd, 0x741, CB_FINDSTRINGEXACT, (WPARAM)-1,
      (LPARAM)g_screen_name_profile.screen_name);
  if (s.value.selection != (WPARAM)-1)
    SendDlgItemMessageA(hwnd, 0x741, CB_SETCURSEL, s.value.selection, 0);
  else
  {
    s.value.selection = SendDlgItemMessageA(hwnd, 0x741, CB_FINDSTRINGEXACT, (WPARAM)-1,
        (LPARAM)g_screen_name_playface_resources.default_name);
    SendDlgItemMessageA(hwnd, 0x741, CB_SETCURSEL, s.value.selection, 0);
    shell_save_match_screen_name_profile();
  }
  SetDlgItemTextA(hwnd, 0x6f0, g_screen_name_profile.screen_name);
  SetDlgItemTextA(hwnd, 0x6f3, g_screen_name_profile.real_name);
  SetDlgItemTextA(hwnd, 0x6f1, g_screen_name_profile.personal_quote);
  SetDlgItemTextA(hwnd, 0x739, g_screen_name_profile.email);
  SetDlgItemTextA(hwnd, 0x737, g_screen_name_profile.date_text);
  s.value.selection = SendDlgItemMessageA(hwnd, 0x711, CB_FINDSTRINGEXACT, (WPARAM)-1,
      (LPARAM)g_screen_name_profile.playface_name);
  if (s.value.selection != (WPARAM)-1)
    SendDlgItemMessageA(hwnd, 0x711, CB_SETCURSEL, s.value.selection, 0);
  else
  {
    SendDlgItemMessageA(hwnd, 0x711, CB_SETCURSEL, 0, 0);
    s.value.selection = SendDlgItemMessageA(hwnd, 0x711, CB_GETCURSEL, 0, 0);
    SendDlgItemMessageA(hwnd, 0x711, CB_GETLBTEXT, 0, (LPARAM)g_screen_name_profile.playface_name);
    shell_save_match_screen_name_profile();
  }
  shell_format_gauntlet_match_text(s.text, 100, g_screen_name_dci_record_label, g_screen_name_profile.unk_728,
      g_screen_name_profile.unk_72a, g_screen_name_profile.unk_72c,
      g_screen_name_profile.unk_728 * 100 / __max(1,
      g_screen_name_profile.unk_72c + g_screen_name_profile.unk_728 +
      g_screen_name_profile.unk_72a));
  SetDlgItemTextA(hwnd, 0x73a, s.text);
  shell_format_gauntlet_match_text(s.text, 100, g_screen_name_multiplayer_record_label,
      g_screen_name_profile.dci_wins, g_screen_name_profile.dci_losses,
      g_screen_name_profile.dci_draws_or_unused, g_screen_name_profile.dci_wins * 100 / __max(1,
      g_screen_name_profile.dci_wins + g_screen_name_profile.dci_losses + g_screen_name_profile.dci_draws_or_unused));
  SetDlgItemTextA(hwnd, 0x73c, s.text);
  shell_format_gauntlet_match_text(s.text, 100, g_screen_name_record_label, g_screen_name_profile.mp_wins,
      g_screen_name_profile.mp_losses, g_screen_name_profile.mp_draws,
      g_screen_name_profile.mp_wins * 100 / __max(1,
      g_screen_name_profile.mp_draws + g_screen_name_profile.mp_wins +
      g_screen_name_profile.mp_losses));
  SetDlgItemTextA(hwnd, 0x716, s.text);
  sprintf(s.text, g_screen_name_rank_format, g_screen_name_profile.dci_rank_display);
  SetDlgItemTextA(hwnd, 0x73b, s.text);
  s.value.skill_index = g_screen_name_profile.mp_wins * 100 / __max(1,
      g_screen_name_profile.mp_draws + g_screen_name_profile.mp_wins + g_screen_name_profile.mp_losses);
  s.value.skill_index /= 10;
  if (g_screen_name_profile.mp_wins == 0)
    s.value.skill_index = 0;
  if (s.value.skill_index > 9)
    s.value.skill_index = 9;
  if (s.value.skill_index < 0)
    s.value.skill_index = 0;
  sprintf(s.text, g_screen_name_skill_format, g_screen_name_skill_names[s.value.skill_index]);
  SetDlgItemTextA(hwnd, 0x73d, s.text);
  sprintf(s.text, g_screen_name_concede_format, g_screen_name_profile.concede_count);
  SetDlgItemTextA(hwnd, 0x73e, s.text);
  sprintf(s.text, g_screen_name_disconnect_format, g_screen_name_profile.disconnect_count);
  SetDlgItemTextA(hwnd, 0x73f, s.text);
}
