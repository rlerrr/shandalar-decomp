#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cardartlib/src/assert.h"

// FUNCTION: SHANDALAR 0x004c8381
int IniHexDigitToInt(int ch)
{
  if (ch >= 'a' && 'f' >= ch)
  {
    return ch - 'W';
  }
  else
  {
    return ch - '0';
  }
}

// FUNCTION: SHANDALAR 0x004c8587
int TrimIniTrailingWhitespace(char *line, int line_len)
{
  int trimmed_count;

  trimmed_count = 0;
  if (line_len == 0)
  {
    return 0;
  }

  while (line[(line_len - 1) - trimmed_count] == ' ' || line[(line_len - 1) - trimmed_count] == '\t')
  {
    trimmed_count = trimmed_count + 1;
  }

  line[line_len - trimmed_count] = '\0';
  return trimmed_count;
}

// FUNCTION: SHANDALAR 0x004c84c7
size_t StripIniCommentSuffix(char *line, size_t line_len)
{
  struct
  {
    size_t removed_count;
    size_t comment_pos;
  } s;

  s.comment_pos = 0;
  s.removed_count = 0;
  if (line_len == 0)
  {
    return 0;
  }

  if (*line == ';')
  {
    return line_len;
  }

  while ((s.comment_pos = strcspn(line + s.comment_pos, ";")) != line_len)
  {
    if (line[s.comment_pos - 1] == '\\')
    {
      strcpy(line + (s.comment_pos - 1), line + s.comment_pos);
      s.removed_count = s.removed_count + 1;
    }
    else
    {
      line[s.comment_pos] = '\0';
      return (line_len - s.comment_pos) + s.removed_count;
    }
  }

  return s.removed_count;
}

// FUNCTION: SHANDALAR 0x004c83bb
int ReadIniTrimmedLine(FILE *file, char *line_out)
{
  struct
  {
    int trimmed_len;
    int unused;
    size_t leading_trimmed_count;
  } s;

  while (1)
  {
    if (fgets(line_out, 0x200, file) == (char *)0)
    {
      *line_out = '\0';
      return 0;
    }

    s.trimmed_len = strlen(line_out);
    assert(line_out[s.trimmed_len] != '\n', "D:\\Newmagic\\multiplayer\\sid\\iniFile.c", 0xc0,
           "Line too long in input file\n");
    line_out[--s.trimmed_len] = '\0';
    if (s.trimmed_len == 0)
    {
      continue;
    }

    s.leading_trimmed_count = strspn(line_out, " \t");
    s.trimmed_len -= s.leading_trimmed_count;
    s.trimmed_len -= (int)StripIniCommentSuffix(line_out + s.leading_trimmed_count, s.trimmed_len);

    if ((s.trimmed_len -= TrimIniTrailingWhitespace(line_out + s.leading_trimmed_count, s.trimmed_len)) == 0)
    {
      continue;
    }

    strcpy(line_out, line_out + s.leading_trimmed_count);
    return s.trimmed_len;
  }
}

// FUNCTION: SHANDALAR 0x004c7f20
int SeekIniLine(FILE *file, char *target_line)
{
  // GLOBAL: SHANDALAR 0x005b7758
  static char seek_line_005b7758[0x200];

  fseek(file, 0, 0);

  while (ReadIniTrimmedLine(file, seek_line_005b7758))
  {
    if (_stricmp(seek_line_005b7758, target_line) == 0)
      return 1;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004c7f85
int ReadIniSectionEntry(FILE *file, char *line_out)
{
  if (ReadIniTrimmedLine(file, line_out))
  {
    if (*line_out == '[')
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    return 1;
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x004c7fe3
char *BuildIniSectionHeader(char *section_name, char *unused, int unused2)
{
  // GLOBAL: SHANDALAR 0x005b7958
  static char section_header_005b7958[0x100];
  void *idk = &unused;

  (void)unused2;

  strcpy(section_header_005b7958, "[");
  strcat(section_header_005b7958, section_name);
  strcat(section_header_005b7958, "]");
  return section_header_005b7958;
}

// FUNCTION: SHANDALAR 0x004c8036
char *FindIniHeaderEntry(FILE *file, char *headers_section_name, char *entry_name)
{
  // GLOBAL: SHANDALAR 0x005b7a58
  static char entry_line_005b7a58[0x100];

  size_t entry_name_len;
  int found_headers_section;

  entry_name_len = strlen(entry_name);
  if (SeekIniLine(file, headers_section_name) == 0)
  {
    return (char *)0;
  }

  while (found_headers_section = ReadIniSectionEntry(file, entry_line_005b7a58))
  {
    if (_strnicmp(entry_line_005b7a58, entry_name, entry_name_len) == 0)
      break;
  }

  if (found_headers_section)
  {
    return entry_line_005b7a58;
  }
  else
  {
    return (char *)0;
  }
}

// FUNCTION: SHANDALAR 0x004c80d8
int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name, char *scratch)
{
  struct
  {
    int entry_count_remaining;
    int entry_count;
    int *entry_table;
    char *decoded_write_ptr;
    char decoded_line[0x200];
    char *entry_line;
    int *table_entry_ptr;
  } s;

  s.entry_line = FindIniHeaderEntry(ini_file, "[headers]", section_name);
  if (s.entry_line == (char *)0)
  {
    return (int *)0;
  }

  s.entry_line = strchr(s.entry_line, ':');
  s.entry_count = atoi(s.entry_line + 1);
  if (SeekIniLine(ini_file, BuildIniSectionHeader(section_name, scratch, 0)) == 0)
  {
    return (int *)0;
  }

  s.entry_table = (int *)malloc((size_t)s.entry_count << 2);
  s.table_entry_ptr = s.entry_table;
  memset(s.entry_table, 0, (size_t)s.entry_count << 2);
  strcpy(s.decoded_line, BuildIniSectionHeader(section_name, scratch, 0));

  assert(SeekIniLine(ini_file, s.decoded_line), "D:\\Newmagic\\multiplayer\\sid\\iniFile.c", 0x7b,
         "Could not locate section header %s\n", s.decoded_line);

  while ((s.entry_count_remaining = s.entry_count--) != 0 && ReadIniSectionEntry(ini_file, s.decoded_line) != 0)
  {
    s.entry_line = s.decoded_line;
    s.decoded_write_ptr = s.entry_line;
    while (*s.entry_line != '\0')
    {
      if (*s.entry_line == '\\')
      {
        s.entry_line = s.entry_line + 1;
        switch (*s.entry_line)
        {
        case 'n':
          *s.decoded_write_ptr = '\n';
          s.decoded_write_ptr = s.decoded_write_ptr + 1;
          s.entry_line = s.entry_line + 1;
          break;
        case 'r':
          *s.decoded_write_ptr = '\r';
          s.decoded_write_ptr = s.decoded_write_ptr + 1;
          s.entry_line = s.entry_line + 1;
          break;
        case 'x':
          *s.decoded_write_ptr = (char)((IniHexDigitToInt((int)s.entry_line[1]) << 4) +
                                        IniHexDigitToInt((int)s.entry_line[2]));
          s.decoded_write_ptr = s.decoded_write_ptr + 1;
          s.entry_line = s.entry_line + 3;
          break;
        }
      }
      else
      {
        *s.decoded_write_ptr = *s.entry_line;
        s.entry_line = s.entry_line + 1;
        s.decoded_write_ptr = s.decoded_write_ptr + 1;
      }
    }

    *s.decoded_write_ptr = '\0';
    *s.table_entry_ptr = (int)malloc(strlen(s.decoded_line) + 2);
    strcpy((char *)*s.table_entry_ptr, s.decoded_line);
    s.table_entry_ptr = s.table_entry_ptr + 1;
  }

  return s.entry_table;
}
