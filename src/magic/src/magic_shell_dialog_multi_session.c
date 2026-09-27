#include "magic_shell.h"
#include "network.h"
#include "global_other.h"
#include "manalinkinterface/manalinkinterface.h"
#include <stdio.h>
void __cdecl _endthread(void);

void initialize_screen_name_profile(screen_name_file_t *profile, int use_current_time);

// GLOBAL: MAGIC 0x008b2950
screen_name_file_t g_multiplayer_opponent_profile;

// FUNCTION: MAGIC 0x0048fc23
void shell_load_multiplayer_opponent_profile(void)
{
  struct
  {
    FILE *file;
    char filename[100];
    char unused[100];
  } s;

  sprintf(s.filename, "Manalink\\Opponent.scn", s.unused);
  s.file = fopen(s.filename, "rb");
  if (s.file == NULL)
    initialize_screen_name_profile(&g_multiplayer_opponent_profile, 0);
  else
  {
    fread(&g_multiplayer_opponent_profile, 0x748, 1, s.file);
    fclose(s.file);
  }
}

// FUNCTION: MAGIC 0x00502196
void __cdecl shell_multiplayer_idle_worker(void *context)
{
  struct
  {
    unsigned int duel_state;
    int reported_idle;
    int done;
    HWND hwnd;
  } s;

  s.hwnd = (HWND)context;
  s.done = 0;
  s.reported_idle = 0;
  while (s.done == 0)
  {
    switch (WaitForSingleObject((HANDLE)g_network_packet_event, 180000))
    {
    case WAIT_OBJECT_0:
      FamInterface_GetDuelState(&s.duel_state);
      if (s.duel_state == 0)
        s.done = 1;
      s.reported_idle = 0;
      break;
    case WAIT_TIMEOUT:
      if (g_waiting_for_network_packet == 0)
      {
        FamInterface_GetDuelState(&s.duel_state);
        if (s.duel_state != 0)
        {
          if (s.reported_idle == 0)
          {
            FamInterface_GoneToMovies();
            s.reported_idle = 1;
          }
        }
        else
          s.done = 1;
      }
      break;
    default:
      OutputDebugStringA("The gone to the movies timer thread has exited abnormally.");
      s.done = 1;
      break;
    }
    ResetEvent((HANDLE)g_network_packet_event);
  }
  _endthread();
}

// FUNCTION: MAGIC 0x00459f7c
int shell_send_multiplayer_save(char *filename)
{
  struct
  {
    int copied_bytes;
    long file_size;
    FILE *destination;
    FILE *source;
  } s;

  s.copied_bytes = 0;
  s.source = fopen(filename, "rb");
  if (s.source == NULL)
    return 0;
  s.destination = fopen("manalink\\mpautosave.e1m", "wb");
  if (s.destination == NULL)
  {
    fclose(s.source);
    return 0;
  }
  fseek(s.source, 0, SEEK_END);
  s.file_size = ftell(s.source);
  fseek(s.source, 0, SEEK_SET);
  while (s.copied_bytes < s.file_size)
  {
    s.copied_bytes++;
    fputc(fgetc(s.source), s.destination);
  }
  fclose(s.source);
  fclose(s.destination);
  FamInterface_SendFile(2, "manalink\\mpautosave.e1m");
}
