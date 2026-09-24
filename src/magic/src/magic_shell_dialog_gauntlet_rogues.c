#include "magic_shell_dialogs.h"
#include <stdlib.h>
#include <string.h>

extern char global_base_directory[];
char *CsvParseNextField(char **text);

// GLOBAL: MAGIC 0x007abcf0
shell_rogue_profile_t shell_rogue_profiles[200];
// GLOBAL: MAGIC 0x008b3bd0
int shell_rogue_count;

// FUNCTION: MAGIC 0x004cd5e4
int shell_load_rogue_profiles(void)
{
  struct
  {
    char *contents;
    HANDLE file;
    int result;
    DWORD bytes_read;
    char path[264];
    DWORD file_size;
    char *next_field;
    int profile_index;
    char *cursor;
  } s;
  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\Rogues.CSV");
  s.file = CreateFileA(s.path, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, 0x8000080, NULL);
  if (s.file != INVALID_HANDLE_VALUE)
  {
    s.file_size = GetFileSize(s.file, NULL);
    s.contents = (char *)malloc(s.file_size + 1);
    if (s.contents != NULL)
    {
      ReadFile(s.file, s.contents, s.file_size, &s.bytes_read, NULL);
      s.cursor = s.contents;
      s.cursor = strchr(s.cursor, '\n') + 1;
      for (s.profile_index = 0;
           s.profile_index < 200 &&
           s.cursor < s.contents + s.file_size - 0x19;
           ++s.profile_index)
      {
        s.next_field = CsvParseNextField(&s.cursor);
        strcpy(shell_rogue_profiles[s.profile_index].name, s.cursor);
        s.cursor = s.next_field;
        s.next_field = CsvParseNextField(&s.cursor);
        shell_rogue_profiles[s.profile_index].difficulty =
            *s.cursor - '1';
        s.cursor = s.next_field;
        s.next_field = CsvParseNextField(&s.cursor);
        strcpy(shell_rogue_profiles[s.profile_index].deck_name, s.cursor);
        s.cursor = s.next_field;
        s.next_field = CsvParseNextField(&s.cursor);
        strcpy(shell_rogue_profiles[s.profile_index].face_name, s.cursor);
        s.cursor = s.next_field;
      }
      shell_rogue_count = s.profile_index - 1;
      s.result = 1;
      free(s.contents);
    }
    else
      s.result = 0;
    CloseHandle(s.file);
  }
  else
    s.result = 0;
  return s.result;
}
