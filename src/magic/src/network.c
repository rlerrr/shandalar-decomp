#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define NETWORK_IMPL
#include "game_support.h"
#include "manalinkinterface/manalinkinterface.h"
#include "global_strings.h"

void AddCardToCLPacket(int card_in_packet);
int GetCardFromCLPacket(int packet_index);
void append_to_trace_txt(char *text);
void report_unexpected_network_packet_number(int expected_packet_number, int actual_packet_number);

// GLOBAL: MAGIC 0x0057cba8
char packet_names_0057cba8[20][20] = {
    "NULL",          "COINTOSS",        "ANTE",       "HAND",          "LIBRARY",
    "PLAYORDRAW",    "MULLIGAN",        "DUELPARAMETERS", "GUESTRESPONSE", "STARTDUEL",
    "DUELRESULTS",   "SAVEDGAME",       "PICKACARD",  "NEWFULLCARD",   "QUESTION",
    "QUESTIONMANA",  "GRABMANA",        "XPOOL",      "CHEATCARD",     "PHASESTOPPER"};

// GLOBAL: MAGIC 0x0057d440
char gs_player_receiving_packet_0057d440[] = "Player %d is receiving a %s packet. This is packet number %d.\n";

// GLOBAL: MAGIC 0x0057d480
char gs_packet_error_0057d480[] = "Packet Error";

// GLOBAL: MAGIC 0x0057d490
char gs_memory_allocation_error_in_ReadCLPacket_0057d490[] = "Memory allocation error in ReadCLPacket!";

// GLOBAL: MAGIC 0x0057d4bc
char gs_packet_error_0057d4bc[] = "Packet Error";

// GLOBAL: MAGIC 0x0057d4cc
char gs_memory_allocation_error_in_AddCardToCLPacket_0057d4cc[] = "Memory allocation error in AddCardToCLPacket!";

// FUNCTION: MAGIC 0x0049e8bb
// FUNCTION: SHANDALAR 0x0053a265
int select_multiple_cards_from_card_list(int player,
                 int *graveyard,
                 int unused,
                 void *available,
                 int count,
                 int prompt_lines,
                 int num_prompt_lines,
                 int selected_indices,
                 int highlighted_choices,
                 int max_choices)
{
  struct
  {
    int selected_count;
    int selection;
    char prompt[200];
    int available_cards[500];
    int index;
    unsigned int require_selection;
    int graveyard_copy[500];
    int unused_slot;
    int packet_card;
    int stop_selection;
  } s;

  (void)unused;

  if (selected_indices == 0 || max_choices <= 0)
  {
    return 0;
  }
  if (count <= 0 || graveyard == NULL || graveyard[0] == -1)
  {
    return 0;
  }

  memcpy(s.graveyard_copy, graveyard, count << 2);
  if (available == NULL)
  {
    for (s.index = 0; count > s.index; ++s.index)
    {
      s.available_cards[s.index] = 1;
    }
  }
  else
  {
    memcpy(s.available_cards, available, count << 2);
  }

  if (g_other_player == player && (g_duel_network_flags & 2) != 0)
  {
    s.stop_selection = 1;
  }
  else
  {
    s.stop_selection = 0;
  }

  s.selected_count = 0;
  while (s.stop_selection == 0 && s.selected_count < max_choices)
  {
    if (num_prompt_lines > s.selected_count)
    {
      strcpy(s.prompt, ((char **)prompt_lines)[s.selected_count]);
    }
    else
    {
      strcpy(s.prompt, ((char **)prompt_lines)[num_prompt_lines - 1]);
    }

    if (s.selected_count >= highlighted_choices)
    {
      s.require_selection = 0;
      s.unused_slot = (int)gs_done_008b40e0;
    }
    else
    {
      s.require_selection = 1;
      s.unused_slot = 0;
    }

    s.selection = show_cardlist(s.graveyard_copy,
                                0,
                                s.available_cards,
                                count,
                                s.prompt,
                                s.require_selection,
                                gs_done_008b40e0);
    if (s.selection == -1)
    {
      s.stop_selection = 1;
    }
    else
    {
      ((int *)selected_indices)[s.selected_count] = s.selection;
      ++s.selected_count;
      s.graveyard_copy[s.selection] = g_draw_card_placeholder_internal_card_id;
      s.available_cards[s.selection] = 0;
    }
  }

  if ((g_duel_network_flags & 2) != 0)
  {
    if (g_other_player == player)
    {
      TENTATIVE_wait_for_network_result(player, 0x16);
      s.packet_card = 0;
      s.index = s.packet_card;
      while (s.packet_card != -1)
      {
        s.packet_card = GetCardFromCLPacket(s.index);
        if (s.packet_card != -1)
        {
          ((int *)selected_indices)[s.index] = s.packet_card;
          ++s.index;
        }
      }
      s.selected_count = s.index;
    }
    else
    {
      for (s.index = 0; s.index < s.selected_count; ++s.index)
      {
        AddCardToCLPacket(((int *)selected_indices)[s.index]);
      }
      AddCardToCLPacket(-1);
      TENTATIVE_send_network_result(player, 0x16);
    }
  }

  return s.selected_count;
}

// FUNCTION: MAGIC 0x00501e96
// FUNCTION: SHANDALAR 0x0056c959
void AddCardToCLPacket(int card_in_packet)
{
#ifdef MAGIC
  unsigned short *write_ptr;

  if ((int)g_card_list_packet_card_count != 0)
  {
    if ((int)(g_card_list_packet_capacity_blocks - 1) < ((int)g_card_list_packet_card_count / 0x10))
    {
      ++g_card_list_packet_capacity_blocks;

      g_card_list_packet_cards = realloc(g_card_list_packet_cards, ((int)g_card_list_packet_capacity_blocks * 0x10) * 2);
      if (g_card_list_packet_cards == NULL)
      {
        MessageBoxA((HWND)0,
                    gs_memory_allocation_error_in_AddCardToCLPacket_0057d4cc,
                    gs_packet_error_0057d4bc,
                    0x10);
      }
    }
  }

  write_ptr = (unsigned short *)g_card_list_packet_cards;
  write_ptr = (unsigned short *)((char *)write_ptr + ((int)g_card_list_packet_card_count * 2));
  *write_ptr = (unsigned short)card_in_packet;
  ++g_card_list_packet_card_count;
#endif
}

// FUNCTION: MAGIC 0x00501f3f
// FUNCTION: SHANDALAR 0x0056c964
int GetCardFromCLPacket(int packet_index)
{
#ifdef MAGIC
  int packet_card;
  short *read_ptr;

  read_ptr = (short *)g_card_list_packet_cards;
  read_ptr = (short *)((char *)read_ptr + packet_index * 2);
  packet_card = (int)*read_ptr;
  if (packet_card == -1)
  {
    if ((int)g_card_list_packet_card_count > 0x10)
    {
      free(g_card_list_packet_cards);
      g_card_list_packet_cards = malloc(0x20);
      if (g_card_list_packet_cards == NULL)
      {
        MessageBoxA((HWND)0, "Memory allocation error in GetCardFromCLPacket!", "Packet Error", 0x10);
      }
    }
    g_card_list_packet_capacity_blocks = 1;
    g_card_list_packet_card_count = 0;
  }

  return packet_card;
#endif
}

#ifdef MAGIC
// FUNCTION: MAGIC 0x00501143
int send_card_list_packet(int player, char packet_type)
{
  struct
  {
    unsigned char packet_header[32];
    int packet_number;
    int packet_kind;
    size_t packet_size;
    void *global_packet;
    int index;
    short *source_ptr;
    int send_result;
    short *write_ptr;
  } s;

  (void)player;

  s.packet_size = g_card_list_packet_card_count * 2 + 6;
  g_card_list_packet_type = packet_type;
  g_card_list_packet_number = (short)g_next_outgoing_card_list_packet_number;
  ++g_next_outgoing_card_list_packet_number;

  s.packet_number = -1;
  s.packet_kind = 2;
  s.global_packet = malloc(s.packet_size);
  s.write_ptr = s.global_packet;
  *s.write_ptr = (short)g_card_list_packet_type;
  s.write_ptr[1] = g_card_list_packet_number;
  s.write_ptr[2] = g_card_list_packet_card_count;
  s.write_ptr += 3;

  s.source_ptr = g_card_list_packet_cards;
  for (s.index = 0; s.index < g_card_list_packet_card_count; ++s.index)
  {
    *s.write_ptr = *s.source_ptr;
    ++s.source_ptr;
    ++s.write_ptr;
  }

  s.send_result = FamInterface_SendPacket(s.packet_header, 1);
  free(s.global_packet);
  if (s.send_result == 1)
  {
    return 0;
  }

  if (0x10 < g_card_list_packet_card_count)
  {
    free(g_card_list_packet_cards);
    g_card_list_packet_cards = malloc(0x20);
  }

  g_card_list_packet_type = '\0';
  g_card_list_packet_card_count = 0;
  g_card_list_packet_capacity_blocks = 1;
  return 1;
}
#endif

// FUNCTION: MAGIC 0x00501c19
int read_card_list_packet(int player, int packet_type, unsigned char *packet)
{
  struct
  {
    char trace[0x64];
    int index;
    short *write_ptr;
    unsigned char *read_ptr;
  } s;

  s.read_ptr = packet;

  g_card_list_packet_type = *packet;

  s.read_ptr += 2;
  g_card_list_packet_number = *(short *)s.read_ptr;
  s.read_ptr += 2;

  if ((int)g_card_list_packet_number != g_next_expected_network_packet_number)
  {
    report_unexpected_network_packet_number(g_next_expected_network_packet_number, g_card_list_packet_number);
  }
  else
  {
    sprintf(s.trace, gs_player_receiving_packet_0057d440, player, packet_names_0057cba8[packet_type], g_next_expected_network_packet_number);
    append_to_trace_txt(s.trace);
  }

  ++g_next_expected_network_packet_number;

  g_card_list_packet_card_count = *(short *)s.read_ptr;
  s.read_ptr += 2;
  if ((int)g_card_list_packet_card_count > 0x1f4)
  {
    return 0;
  }

  if ((int)g_card_list_packet_card_count > 0x10)
  {
    g_card_list_packet_cards = (short *)realloc(g_card_list_packet_cards, (int)g_card_list_packet_card_count * 2);
    if (g_card_list_packet_cards == NULL)
    {
      MessageBoxA((HWND)0, gs_memory_allocation_error_in_ReadCLPacket_0057d490, gs_packet_error_0057d480, 0x10);
    }
  }

  s.write_ptr = g_card_list_packet_cards;
  for (s.index = 0; s.index < (int)g_card_list_packet_card_count; ++s.index)
  {
    *s.write_ptr = *(short *)s.read_ptr;
    s.read_ptr += 2;
    ++s.write_ptr;
  }

  return 1;
}

// FUNCTION: MAGIC 0x00502124
// FUNCTION: SHANDALAR 0x0056c97a
void append_to_trace_txt(char *text)
{
  // We always leave this in place for debugging, but shandalar doesn't actually contain it
#if defined(MAGIC) || defined(_DEBUG)
  FILE *trace_file;

  if (g_duel_ai_mode_state == 1)
    return;

  trace_file = fopen("Trace.txt", "at");

  if (trace_file == NULL)
    return;

  fwrite(text, strlen(text), 1, trace_file);
  fclose(trace_file);
#endif
}

// FUNCTION: MAGIC 0x00500a40
void trace_network_packet(char *packet_data, int packet_size)
{
  FILE *trace_file;
  int packet_index;

  if (g_duel_ai_mode_state != 1)
  {
    trace_file = fopen("Trace.txt", "at");
    if (trace_file != NULL)
    {
      fprintf(trace_file, "   packet type: %1d\n", (int)*packet_data);
      fprintf(trace_file, "   packet number: %1d\n", (int)packet_data[2]);
      packet_data += 4;
      fprintf(trace_file, "   ");
      for (packet_index = 4; packet_index < packet_size; ++packet_index)
      {
        fprintf(trace_file, "%1d", (int)*packet_data);
        ++packet_data;
      }
      fprintf(trace_file, "\n");
      fclose(trace_file);
    }
  }
}

// FUNCTION: MAGIC 0x00500b2c
void report_unexpected_network_packet_number(int expected_packet_number, int actual_packet_number)
{
  char message[100];
  char trace[500];

  play_sound_effect(WAV_ENDPHASE);
  sprintf(trace, "Was expecting packet number %d but got packet number %d !\n", expected_packet_number, actual_packet_number);
  append_to_trace_txt(trace);
  play_sound_effect(WAV_ENDPHASE);
  sprintf(message, "Was expecting packet number %d but got packet number %d !\n", expected_packet_number, actual_packet_number);
  MessageBoxA((HWND)0, message, "Multiplayer duel error", 0x10);
}

// FUNCTION: MAGIC 0x00500bb0
int ReportUnexpectedNetworkPacketType(int expected_packet_type, int actual_packet_type)
{
#ifdef MAGIC
  char message[100];
  char trace[500];

  sprintf(trace, "Was expecting a %s packet but got a %s packet!\n", packet_names_0057cba8[expected_packet_type], packet_names_0057cba8[actual_packet_type]);
  append_to_trace_txt(trace);
  play_sound_effect(WAV_ENDPHASE);
  sprintf(message, "Was expecting a %s packet but got a %s packet!", packet_names_0057cba8[expected_packet_type], packet_names_0057cba8[actual_packet_type]);
  MessageBoxA((HWND)0, message, "Multiplayer duel error", 0x10);
#else
  return 0;
#endif
}

// FUNCTION: MAGIC 0x00501b6e
int apply_cheat_card_packet(void)
{
  add_card_to_hand(g_other_player, g_network_result_value);
  ++g_duel_summary.hand_counts[g_other_player];
  return 1;
}

// FUNCTION: MAGIC 0x00501bb2
int apply_xpool_packet_to_active_player(void)
{
  int color;

  for (color = 0; color < 8; ++color)
  {
    g_raw_mana_available[g_other_player][color] = g_xpool_network_packet.raw_mana_available[color];
  }
  copy_mana_pool_to_display();
  return 1;
}

// FUNCTION: MAGIC 0x005012d7
// FUNCTION: SHANDALAR 0x0056c91f
int TENTATIVE_wait_for_network_result(int player, signed int packet_type)
{
#ifdef MAGIC
  struct
  {
    unsigned char receive_packet_storage[32];
    int packet_id;
    int unused_24;
    unsigned int packet_size;
    int packet_handle;
    char message[100];
    int receive_result;
    int got_requested_packet;
    short *packet_data_as_short;
    unsigned char *packet_data;
  } s;
  unsigned long wait_result;
  const char *packet_name;

  s.got_requested_packet = 0;
  s.receive_result = 0;
  g_waiting_for_network_packet = 1;

  do
  {
    wait_result = WaitForSingleObject((HANDLE)global_mutex_ReadPacket, 0xffffffff);
  } while (wait_result != 0);

  for (;;)
  {
    if (s.got_requested_packet != 0)
    {
      ReleaseMutex((HANDLE)global_mutex_ReadPacket);
      g_waiting_for_network_packet = 0;
      return 1;
    }

    do
    {
      s.packet_id = 0x2fffffff;
      s.receive_result = FamInterface_ReceivePacket(2, s.receive_packet_storage);

      if (s.receive_result == 4)
      {
        if (s.packet_id != 0x2fffffff)
        {
          append_to_trace_txt("The manalink said we didn't have a packet but it lied.\n");
        }

        if (FamInterface_HasOpponent() == 0)
        {
          if (unk_008b60e8 != 0 && packet_type >= 0xc)
          {
            append_to_trace_txt("The duel has ended because your opponent has dropped.\n");
            exit_duel_thread(3);
            FamInterface_Flush();
            ReleaseMutex((HANDLE)global_mutex_ReadPacket);
            g_waiting_for_network_packet = 0;
            return 0;
          }

          append_to_trace_txt("The duel has ended because your opponent has dropped.\n");
          ReleaseMutex((HANDLE)global_mutex_ReadPacket);
          g_waiting_for_network_packet = 0;
          return 0;
        }
      }
      else if (s.receive_result != 0)
      {
        sprintf(s.message, "Manalink returned an error(%d).\n", s.receive_result);
        append_to_trace_txt(s.message);
      }
    } while (s.receive_result != 0);

    s.packet_data = (unsigned char *)GlobalLock((HGLOBAL)s.packet_handle);
    if (s.packet_data[0] != packet_type && s.packet_data[0] != 0x12 && s.packet_data[0] != 0x11)
    {
      ReportUnexpectedNetworkPacketType(packet_type, s.packet_data[0]);
      trace_network_packet((char *)s.packet_data, s.packet_size);
      s.packet_data_as_short = (short *)(s.packet_data + 2);
      report_unexpected_network_packet_number(g_next_expected_network_packet_number, *s.packet_data_as_short);

      if (unk_008b60e8 != 0)
      {
        exit_duel_thread(3);
      }

      FamInterface_EndSession();
      FamInterface_Flush();
      ReleaseMutex((HANDLE)global_mutex_ReadPacket);
      g_waiting_for_network_packet = 0;
      return 0;
    }

    append_to_trace_txt("\n");
    trace_network_packet((char *)s.packet_data, s.packet_size);
    SetEvent((HANDLE)unk_0092607c);

    packet_name = "GENERIC";
    switch (packet_type)
    {
    case 0:
      packet_name = "NULL";
      break;
    case 1:
      packet_name = "COINTOSS";
      break;
    case 2:
      packet_name = "ANTE";
      break;
    case 3:
      packet_name = "HAND";
      break;
    case 4:
      packet_name = "LIBRARY";
      break;
    case 5:
      packet_name = "PLAYORDRAW";
      break;
    case 6:
      packet_name = "MULLIGAN";
      break;
    case 7:
      packet_name = "DUELPARAMETERS";
      break;
    case 8:
      packet_name = "GUESTRESPONSE";
      break;
    case 9:
      packet_name = "STARTDUEL";
      break;
    case 10:
      packet_name = "DUELRESULTS";
      break;
    case 11:
      packet_name = "SAVEDGAME";
      break;
    case 12:
      packet_name = "PICKACARD";
      break;
    case 13:
      packet_name = "NEWFULLCARD";
      break;
    case 14:
      packet_name = "QUESTION";
      break;
    case 15:
      packet_name = "QUESTIONMANA";
      break;
    case 16:
      packet_name = "GRABMANA";
      break;
    case 17:
      packet_name = "XPOOL";
      break;
    case 18:
      packet_name = "CHEATCARD";
      break;
    case 19:
      packet_name = "PHASESTOPPER";
      break;
    }

    switch (s.packet_data[0])
    {
    case 1:
    case 2:
    case 5:
    case 6:
    case 8:
    case 9:
    case 10:
    case 0x10:
    case 0x14:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
      memcpy(&g_network_result_packet_type, s.packet_data, s.packet_size);
      if (*(short *)((char *)&g_network_result_packet_type + 2) == g_next_expected_network_packet_number)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, g_next_expected_network_packet_number);
        append_to_trace_txt(s.message);
      }
      else
      {
        report_unexpected_network_packet_number(g_next_expected_network_packet_number, *(short *)((char *)&g_network_result_packet_type + 2));
      }
      ++g_next_expected_network_packet_number;
      s.got_requested_packet = 1;
      break;

    case 3:
    case 4:
    case 0x16:
      read_card_list_packet(player, packet_type, s.packet_data);
      s.got_requested_packet = 1;
      break;

    case 7:
      memcpy(&unk_008b34a0, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_008b34a0 + 2) == g_next_expected_network_packet_number)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, g_next_expected_network_packet_number);
        append_to_trace_txt(s.message);
      }
      else
      {
        report_unexpected_network_packet_number(g_next_expected_network_packet_number, *(short *)((char *)&unk_008b34a0 + 2));
      }
      ++g_next_expected_network_packet_number;
      s.got_requested_packet = 1;
      break;

    case 0xc:
      memcpy(&g_target_selection_network_packet, s.packet_data, s.packet_size);
      if (*(short *)((char *)&g_target_selection_network_packet + 2) == g_next_expected_network_packet_number)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, g_next_expected_network_packet_number);
        append_to_trace_txt(s.message);
      }
      else
      {
        report_unexpected_network_packet_number(g_next_expected_network_packet_number, *(short *)((char *)&g_target_selection_network_packet + 2));
      }
      ++g_next_expected_network_packet_number;
      s.got_requested_packet = 1;
      break;

    case 0xd:
    case 0xe:
    case 0xf:
      memcpy(&unk_008b2938, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_008b2938 + 2) == g_next_expected_network_packet_number)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, g_next_expected_network_packet_number);
        append_to_trace_txt(s.message);
      }
      else
      {
        report_unexpected_network_packet_number(g_next_expected_network_packet_number, *(short *)((char *)&unk_008b2938 + 2));
      }
      ++g_next_expected_network_packet_number;
      s.got_requested_packet = 1;
      break;

    case 0x11:
      memcpy(&g_xpool_network_packet, s.packet_data, s.packet_size);
      if (g_xpool_network_packet.packet_number == g_next_expected_network_packet_number)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, g_next_expected_network_packet_number);
        append_to_trace_txt(s.message);
      }
      else
      {
        report_unexpected_network_packet_number(g_next_expected_network_packet_number, g_xpool_network_packet.packet_number);
      }
      ++g_next_expected_network_packet_number;
      apply_xpool_packet_to_active_player();
      s.got_requested_packet = 0;
      break;

    case 0x12:
      memcpy(&g_network_result_packet_type, s.packet_data, s.packet_size);
      if (*(short *)((char *)&g_network_result_packet_type + 2) == g_next_expected_network_packet_number)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, g_next_expected_network_packet_number);
        append_to_trace_txt(s.message);
      }
      else
      {
        report_unexpected_network_packet_number(g_next_expected_network_packet_number, *(short *)((char *)&g_network_result_packet_type + 2));
      }
      ++g_next_expected_network_packet_number;
      apply_cheat_card_packet();
      s.got_requested_packet = 0;
      break;

    case 0x13:
      memcpy(&unk_008b27f0, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_008b27f0 + 2) == g_next_expected_network_packet_number)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, g_next_expected_network_packet_number);
        append_to_trace_txt(s.message);
      }
      else
      {
        report_unexpected_network_packet_number(g_next_expected_network_packet_number, *(short *)((char *)&unk_008b27f0 + 2));
      }
      ++g_next_expected_network_packet_number;
      s.got_requested_packet = 1;
      break;

    case 0x15:
      memcpy(&unk_008cf3a0, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_008cf3a0 + 2) == g_next_expected_network_packet_number)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, g_next_expected_network_packet_number);
        append_to_trace_txt(s.message);
      }
      else
      {
        report_unexpected_network_packet_number(g_next_expected_network_packet_number, *(short *)((char *)&unk_008cf3a0 + 2));
      }
      ++g_next_expected_network_packet_number;
      s.got_requested_packet = 1;
      break;

    case 0x1b:
    case 0x1c:
      memcpy(&unk_00926080, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_00926080 + 2) == g_next_expected_network_packet_number)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, g_next_expected_network_packet_number);
        append_to_trace_txt(s.message);
      }
      else
      {
        report_unexpected_network_packet_number(g_next_expected_network_packet_number, *(short *)((char *)&unk_00926080 + 2));
      }
      ++g_next_expected_network_packet_number;
      s.got_requested_packet = 1;
      break;

    default:
      GlobalUnlock((HGLOBAL)s.packet_handle);
      GlobalFree((HGLOBAL)s.packet_handle);
      sprintf(s.message, "There was an error reading a packet of type %d", packet_type);
      MessageBoxA((HWND)0, s.message, "Packet Error", 0);
      ReleaseMutex((HANDLE)global_mutex_ReadPacket);
    g_waiting_for_network_packet = 0;
      return 0;
    }

    GlobalUnlock((HGLOBAL)s.packet_handle);
    GlobalFree((HGLOBAL)s.packet_handle);
  }
#else
  return 0;
#endif
}

// FUNCTION: MAGIC 0x00500d74
// FUNCTION: SHANDALAR 0x0056c931
int TENTATIVE_send_network_result(int player, signed int packet_type)
{
#ifdef MAGIC
  if (packet_type == 3 || packet_type == 4 || packet_type == 0x16)
  {
    return send_card_list_packet(player, (char)packet_type);
  }
#endif
  return 0;
}

// FUNCTION: MAGIC 0x00501d78
// FUNCTION: SHANDALAR 0x0056c943
void send_battlefield_status_packet(int player)
{
#ifdef MAGIC
  int phase;

  unk_008b27f0 = 0x13;
  for (phase = 0; phase < 0x26; ++phase)
  {
    DAT_008b27f4[phase] = (g_duel_phase_stop_settings[1].phase_flags[phase] & PHASE_STOP_ENABLED) << 2;
    DAT_008b27f4[0x26 + phase] = (g_duel_phase_stop_settings[0].phase_flags[phase] & PHASE_STOP_ENABLED) << 2;
  }

  TENTATIVE_send_network_result(player, 0x13);
#endif
}

// FUNCTION: MAGIC 0x00501deb
// FUNCTION: SHANDALAR 0x0056c94e
void receive_battlefield_status_packet(int player)
{
#ifdef MAGIC
  int phase;
  int target_player;

  if (TENTATIVE_wait_for_network_result(player, 0x13) != 0)
  {
    for (target_player = 0; target_player < 2; ++target_player)
    {
      for (phase = 0; phase < 0x26; ++phase)
      {
        g_duel_phase_stop_settings[target_player].phase_flags[phase] =
            DAT_008b27f4[target_player * 0x26 + phase] | (g_duel_phase_stop_settings[target_player].phase_flags[phase] & (PHASE_STOP_ENABLED | PHASE_STOP_SUPPRESSED));
      }
    }
  }
#endif
}
