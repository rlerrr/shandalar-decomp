#include "network.h"
#include <stdio.h>
#include <string.h>

extern int g_duel_ai_mode_state;
extern int g_duel_trace_counter;

// FUNCTION: MAGIC 0x00501fd8
void shell_begin_duel_trace(char *message)
{
  struct
  {
    FILE *file;
    char header[52];
  } trace;
  char date[20] = "Mar 24 1998";
  char build_time[20] = "11:22:22";

  if (g_duel_ai_mode_state == 1)
    return;
  g_next_outgoing_card_list_packet_number =
      g_next_expected_network_packet_number = 0;
  g_duel_trace_counter = 0;
  trace.file = fopen("Trace.txt", "wt");
  if (trace.file == NULL)
    return;
  strcpy(trace.header, date);
  strcat(trace.header, " ");
  strcat(trace.header, build_time);
  strcat(trace.header, "\n");
  fwrite(trace.header, strlen(trace.header), 1, trace.file);
  fwrite(message, strlen(message), 1, trace.file);
  fclose(trace.file);
}
