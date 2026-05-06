#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define NETWORK_IMPL
#include "game_support.h"
#include "manalinkinterface/manalinkinterface.h"

void AddCardToCLPacket(unsigned short card_in_packet);
int GetCardFromCLPacket(int packet_index);

// FUNCTION: MAGIC 0x0049e8bb
int FUN_0049e8bb(int player,
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
    int stop_selection;
    int packet_card;
    int selected_count;
    char prompt[200];
    int available_cards[500];
    int index;
    unsigned int show_bigcard;
    int graveyard_copy[492];
    int unused_slot;
  } locals;
  int selection;

  (void)unused;

  if (selected_indices == 0 || max_choices < 1)
  {
    return 0;
  }
  else if (count < 1 || graveyard == NULL || graveyard[0] == -1)
  {
    return 0;
  }

  memcpy(locals.graveyard_copy, graveyard, count << 2);
  if (available == NULL)
  {
    for (locals.index = 0; locals.index < count; ++locals.index)
    {
      locals.available_cards[locals.index] = 1;
    }
  }
  else
  {
    memcpy(locals.available_cards, available, count << 2);
  }

  if (active_player == player && (unk_00926804 & 2) != 0)
  {
    locals.stop_selection = 1;
  }
  else
  {
    locals.stop_selection = 0;
  }

  locals.selected_count = 0;
  while (locals.stop_selection == 0 && locals.selected_count < max_choices)
  {
    if (locals.selected_count < num_prompt_lines)
    {
      strcpy(locals.prompt, ((char **)prompt_lines)[locals.selected_count]);
    }
    else
    {
      strcpy(locals.prompt, ((char **)prompt_lines)[num_prompt_lines - 1]);
    }

    locals.show_bigcard = (unsigned int)(locals.selected_count < highlighted_choices);
    selection = FUN_0049e6aa(locals.graveyard_copy,
                             0,
                             locals.available_cards,
                             count,
                             &unk_008b40e0,
                             locals.show_bigcard,
                             locals.prompt);
    if (selection == -1)
    {
      locals.stop_selection = 1;
    }
    else
    {
      ((int *)selected_indices)[locals.selected_count] = selection;
      ++locals.selected_count;
      locals.graveyard_copy[selection] = unk_008b28f8;
      locals.available_cards[selection] = 0;
    }
  }

  if ((unk_00926804 & 2) != 0)
  {
    if (active_player == player)
    {
      TENTATIVE_wait_for_network_result(player, 0x16);
      locals.packet_card = 0;
      locals.index = 0;
      while (locals.packet_card != -1)
      {
        locals.selected_count = locals.index;
        locals.packet_card = GetCardFromCLPacket(locals.index);
        if (locals.packet_card != -1)
        {
          ((int *)selected_indices)[locals.index] = locals.packet_card;
          ++locals.index;
        }
      }
    }
    else
    {
      for (locals.index = 0; locals.index < locals.selected_count; ++locals.index)
      {
        AddCardToCLPacket((short)((int *)selected_indices)[locals.index]);
      }
      AddCardToCLPacket(0xffff);
      TENTATIVE_send_network_result(player, 0x16);
    }
  }

  return locals.selected_count;
}

// FUNCTION: MAGIC 0x00501e96
void AddCardToCLPacket(unsigned short card_in_packet)
{
  if (unk_0091ca94 != 0 && unk_0091ca96 - 1 < ((int)unk_0091ca94 + (((int)unk_0091ca94 >> 0x1f) & 0xfU)) >> 4)
  {
    ++unk_0091ca96;
    unk_0091ca98 = realloc(unk_0091ca98, unk_0091ca96 * 0x20);
  }

  *(unsigned short *)((char *)unk_0091ca98 + unk_0091ca94 * 2) = card_in_packet;
  ++unk_0091ca94;
}

// FUNCTION: MAGIC 0x00501f3f
int GetCardFromCLPacket(int packet_index)
{
  int packet_card;

  packet_card = (int)*(short *)((char *)unk_0091ca98 + packet_index * 2);
  if (packet_card == -1)
  {
    if (0x10 < unk_0091ca94)
    {
      free(unk_0091ca98);
      unk_0091ca98 = malloc(0x20);
    }
    unk_0091ca96 = 1;
    unk_0091ca94 = 0;
  }

  return packet_card;
}

// FUNCTION: MAGIC 0x00501143
int FUN_00501143(int player, char packet_type)
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
  } locals;

  (void)player;

  locals.packet_size = unk_0091ca94 * 2 + 6;
  unk_0091ca90 = packet_type;
  unk_0091ca92 = (short)unk_0091d07c;
  ++unk_0091d07c;

  locals.packet_number = -1;
  locals.packet_kind = 2;
  locals.global_packet = malloc(locals.packet_size);
  locals.write_ptr = locals.global_packet;
  *locals.write_ptr = (short)unk_0091ca90;
  locals.write_ptr[1] = unk_0091ca92;
  locals.write_ptr[2] = unk_0091ca94;
  locals.write_ptr += 3;

  locals.source_ptr = unk_0091ca98;
  for (locals.index = 0; locals.index < unk_0091ca94; ++locals.index)
  {
    *locals.write_ptr = *locals.source_ptr;
    ++locals.source_ptr;
    ++locals.write_ptr;
  }

  locals.send_result = FamInterface_SendPacket(locals.packet_header, 1);
  free(locals.global_packet);
  if (locals.send_result == 1)
  {
    return 0;
  }

  if (0x10 < unk_0091ca94)
  {
    free(unk_0091ca98);
    unk_0091ca98 = malloc(0x20);
  }

  unk_0091ca90 = '\0';
  unk_0091ca94 = 0;
  unk_0091ca96 = 1;
  return 1;
}

// FUNCTION: MAGIC 0x00501c19
int FUN_00501c19(int player, int packet_type, unsigned char *packet)
{
  int index;
  short *read_ptr;
  short *write_ptr;

  (void)player;
  (void)packet_type;

  unk_0091ca90 = *packet;
  unk_0091ca92 = *(short *)(packet + 2);
  read_ptr = (short *)(packet + 4);
  unk_0091ca94 = *read_ptr;
  ++read_ptr;
  if (unk_0091ca94 >= 0x1f5)
  {
    return 0;
  }

  if (0x10 < unk_0091ca94)
  {
    unk_0091ca98 = realloc(unk_0091ca98, unk_0091ca94 * 2);
  }

  write_ptr = unk_0091ca98;
  for (index = 0; index < unk_0091ca94; ++index)
  {
    *write_ptr = *read_ptr;
    ++read_ptr;
    ++write_ptr;
  }

  return 1;
}

// FUNCTION: MAGIC 0x00502124
void append_to_trace_txt(char *text)
{
  FILE *trace_file;

  if (unk_008a9000 != 1)
  {
    trace_file = fopen("Trace.txt", "at");
    if (trace_file != NULL)
    {
      fwrite(text, strlen(text), 1, trace_file);
      fclose(trace_file);
    }
  }
}

// FUNCTION: MAGIC 0x00500a40
void FUN_00500a40(char *packet_data, int packet_size)
{
  FILE *trace_file;
  int packet_index;

  if (unk_008a9000 != 1)
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
void FUN_00500b2c(int expected_packet_number, int actual_packet_number)
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
void FUN_00500bb0(int expected_packet_type, int actual_packet_type)
{
  static const char *packet_names[] = {
      "NULL",        "COINTOSS",    "ANTE",       "HAND",       "LIBRARY",
      "PLAYORDRAW",  "MULLIGAN",    "DUELPARAMETERS", "GUESTRESPONSE", "STARTDUEL",
      "DUELRESULTS", "SAVEDGAME",   "PICKACARD",  "NEWFULLCARD","QUESTION",
      "QUESTIONMANA","GRABMANA",    "XPOOL",      "CHEATCARD",  "PHASESTOPPER"};
  char message[100];
  char trace[500];

  sprintf(trace, "Was expecting a %s packet but got a %s packet!\n", packet_names[expected_packet_type], packet_names[actual_packet_type]);
  append_to_trace_txt(trace);
  play_sound_effect(WAV_ENDPHASE);
  sprintf(message, "Was expecting a %s packet but got a %s packet!", packet_names[expected_packet_type], packet_names[actual_packet_type]);
  MessageBoxA((HWND)0, message, "Multiplayer duel error", 0x10);
}

// FUNCTION: MAGIC 0x00501b6e
int FUN_00501b6e(void)
{
  add_card_to_hand(active_player, unk_007a7d0c);
  ++hand_count[active_player];
  return 1;
}

// FUNCTION: MAGIC 0x00501bb2
int FUN_00501bb2(void)
{
  int color;

  for (color = 0; color < 8; ++color)
  {
    raw_mana_available[active_player][color] = *(int *)((char *)&unk_009251b0 + color * 4 + 4);
  }
  copy_mana_pool_to_display();
  return 1;
}

// FUNCTION: MAGIC 0x005012d7
int TENTATIVE_wait_for_network_result(int player, signed int packet_type)
{
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
  *(int *)&unk_009266d0[0x7c] = 1;

  do
  {
    wait_result = WaitForSingleObject((HANDLE)unk_00789a44, 0xffffffff);
  } while (wait_result != 0);

  for (;;)
  {
    if (s.got_requested_packet != 0)
    {
      ReleaseMutex((HANDLE)unk_00789a44);
      *(int *)&unk_009266d0[0x7c] = 0;
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
            FUN_004b4110(3);
            FamInterface_Flush();
            ReleaseMutex((HANDLE)unk_00789a44);
            *(int *)&unk_009266d0[0x7c] = 0;
            return 0;
          }

          append_to_trace_txt("The duel has ended because your opponent has dropped.\n");
          ReleaseMutex((HANDLE)unk_00789a44);
          *(int *)&unk_009266d0[0x7c] = 0;
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
      FUN_00500bb0(packet_type, s.packet_data[0]);
      FUN_00500a40((char *)s.packet_data, s.packet_size);
      s.packet_data_as_short = (short *)(s.packet_data + 2);
      FUN_00500b2c(unk_007a7d6c, *s.packet_data_as_short);

      if (unk_008b60e8 != 0)
      {
        FUN_004b4110(3);
      }

      FamInterface_EndSession();
      FamInterface_Flush();
      ReleaseMutex((HANDLE)unk_00789a44);
      *(int *)&unk_009266d0[0x7c] = 0;
      return 0;
    }

    append_to_trace_txt("\n");
    FUN_00500a40((char *)s.packet_data, s.packet_size);
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
      memcpy(&unk_007a7d08, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_007a7d08 + 2) == unk_007a7d6c)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, unk_007a7d6c);
        append_to_trace_txt(s.message);
      }
      else
      {
        FUN_00500b2c(unk_007a7d6c, *(short *)((char *)&unk_007a7d08 + 2));
      }
      ++unk_007a7d6c;
      s.got_requested_packet = 1;
      break;

    case 3:
    case 4:
    case 0x16:
      FUN_00501c19(player, packet_type, s.packet_data);
      s.got_requested_packet = 1;
      break;

    case 7:
      memcpy(&unk_008b34a0, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_008b34a0 + 2) == unk_007a7d6c)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, unk_007a7d6c);
        append_to_trace_txt(s.message);
      }
      else
      {
        FUN_00500b2c(unk_007a7d6c, *(short *)((char *)&unk_008b34a0 + 2));
      }
      ++unk_007a7d6c;
      s.got_requested_packet = 1;
      break;

    case 0xc:
      memcpy(&unk_008cf200, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_008cf200 + 2) == unk_007a7d6c)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, unk_007a7d6c);
        append_to_trace_txt(s.message);
      }
      else
      {
        FUN_00500b2c(unk_007a7d6c, *(short *)((char *)&unk_008cf200 + 2));
      }
      ++unk_007a7d6c;
      s.got_requested_packet = 1;
      break;

    case 0xd:
    case 0xe:
    case 0xf:
      memcpy(&unk_008b2938, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_008b2938 + 2) == unk_007a7d6c)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, unk_007a7d6c);
        append_to_trace_txt(s.message);
      }
      else
      {
        FUN_00500b2c(unk_007a7d6c, *(short *)((char *)&unk_008b2938 + 2));
      }
      ++unk_007a7d6c;
      s.got_requested_packet = 1;
      break;

    case 0x11:
      memcpy(&unk_009251b0, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_009251b0 + 2) == unk_007a7d6c)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, unk_007a7d6c);
        append_to_trace_txt(s.message);
      }
      else
      {
        FUN_00500b2c(unk_007a7d6c, *(short *)((char *)&unk_009251b0 + 2));
      }
      ++unk_007a7d6c;
      FUN_00501bb2();
      s.got_requested_packet = 0;
      break;

    case 0x12:
      memcpy(&unk_007a7d08, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_007a7d08 + 2) == unk_007a7d6c)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, unk_007a7d6c);
        append_to_trace_txt(s.message);
      }
      else
      {
        FUN_00500b2c(unk_007a7d6c, *(short *)((char *)&unk_007a7d08 + 2));
      }
      ++unk_007a7d6c;
      FUN_00501b6e();
      s.got_requested_packet = 0;
      break;

    case 0x13:
      memcpy(&unk_008b27f0, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_008b27f0 + 2) == unk_007a7d6c)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, unk_007a7d6c);
        append_to_trace_txt(s.message);
      }
      else
      {
        FUN_00500b2c(unk_007a7d6c, *(short *)((char *)&unk_008b27f0 + 2));
      }
      ++unk_007a7d6c;
      s.got_requested_packet = 1;
      break;

    case 0x15:
      memcpy(&unk_008cf3a0, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_008cf3a0 + 2) == unk_007a7d6c)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, unk_007a7d6c);
        append_to_trace_txt(s.message);
      }
      else
      {
        FUN_00500b2c(unk_007a7d6c, *(short *)((char *)&unk_008cf3a0 + 2));
      }
      ++unk_007a7d6c;
      s.got_requested_packet = 1;
      break;

    case 0x1b:
    case 0x1c:
      memcpy(&unk_00926080, s.packet_data, s.packet_size);
      if (*(short *)((char *)&unk_00926080 + 2) == unk_007a7d6c)
      {
        sprintf(s.message, "Player %d is receiving a %s packet. This is packet number %d.\n", player, packet_name, unk_007a7d6c);
        append_to_trace_txt(s.message);
      }
      else
      {
        FUN_00500b2c(unk_007a7d6c, *(short *)((char *)&unk_00926080 + 2));
      }
      ++unk_007a7d6c;
      s.got_requested_packet = 1;
      break;

    default:
      GlobalUnlock((HGLOBAL)s.packet_handle);
      GlobalFree((HGLOBAL)s.packet_handle);
      sprintf(s.message, "There was an error reading a packet of type %d", packet_type);
      MessageBoxA((HWND)0, s.message, "Packet Error", 0);
      ReleaseMutex((HANDLE)unk_00789a44);
      *(int *)&unk_009266d0[0x7c] = 0;
      return 0;
    }

    GlobalUnlock((HGLOBAL)s.packet_handle);
    GlobalFree((HGLOBAL)s.packet_handle);
  }
}

// FUNCTION: MAGIC 0x00500d74
int TENTATIVE_send_network_result(int player, signed int packet_type)
{
  if (packet_type == 3 || packet_type == 4 || packet_type == 0x16)
  {
    return FUN_00501143(player, (char)packet_type);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00501d78
void FUN_00501d78(int player)
{
  int phase;

  unk_008b27f0 = 0x13;
  for (phase = 0; phase < 0x26; ++phase)
  {
    DAT_008b27f4[phase] = (DAT_007abc90[0x26 + phase] & 1) << 2;
    DAT_008b27f4[0x26 + phase] = (DAT_007abc90[phase] & 1) << 2;
  }

  TENTATIVE_send_network_result(player, 0x13);
}

// FUNCTION: MAGIC 0x00501deb
void FUN_00501deb(int player)
{
  int phase;
  int target_player;

  if (TENTATIVE_wait_for_network_result(player, 0x13) != 0)
  {
    for (target_player = 0; target_player < 2; ++target_player)
    {
      for (phase = 0; phase < 0x26; ++phase)
      {
        DAT_007abc90[target_player * 0x26 + phase] =
            DAT_008b27f4[target_player * 0x26 + phase] | (DAT_007abc90[target_player * 0x26 + phase] & 3);
      }
    }
  }
}
