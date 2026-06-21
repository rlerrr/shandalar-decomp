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
  size_t comment_pos;
  size_t removed_count;

  comment_pos = 0;
  removed_count = 0;
  if (line_len == 0)
  {
    return 0;
  }

  if (*line == ';')
  {
    return line_len;
  }

  while (1)
  {
    comment_pos = strcspn(line + comment_pos, ";");
    if (comment_pos == line_len)
    {
      return removed_count;
    }

    if (line[comment_pos - 1] != '\\')
    {
      line[comment_pos] = '\0';
      return (line_len - comment_pos) + removed_count;
    }

    strcpy(line + (comment_pos - 1), line + comment_pos);
    removed_count = removed_count + 1;
  }
}

// FUNCTION: SHANDALAR 0x004c83bb
int ReadIniTrimmedLine(FILE *file, char *line_out)
{
  char *read_result;
  size_t line_len;
  size_t leading_trimmed_count;
  size_t comment_trimmed_count;
  int trailing_trimmed_count;
  int trimmed_len;

  do
  {
    do
    {
      read_result = fgets(line_out, 0x200, file);
      if (read_result == (char *)0)
      {
        *line_out = '\0';
        return 0;
      }

      line_len = strlen(line_out);
      assert((unsigned int)(line_out[line_len] != '\n'), "D:\\Newmagic\\multiplayer\\sid\\iniFile.c", 0xc0,
             "Line too long in input file");
      trimmed_len = (int)line_len - 1;
      line_out[trimmed_len] = '\0';
    } while (trimmed_len == 0);

    leading_trimmed_count = strspn(line_out, " \t");
    comment_trimmed_count = StripIniCommentSuffix(line_out + leading_trimmed_count, (size_t)(trimmed_len - (int)leading_trimmed_count));
    trimmed_len = (trimmed_len - (int)leading_trimmed_count) - (int)comment_trimmed_count;
    trailing_trimmed_count = TrimIniTrailingWhitespace(line_out + leading_trimmed_count, trimmed_len);
    trimmed_len = trimmed_len - trailing_trimmed_count;
  } while (trimmed_len == 0);

  strcpy(line_out, line_out + leading_trimmed_count);
  return trimmed_len;
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
char *BuildIniSectionHeader(char *section_name, int unused)
{
  // GLOBAL: SHANDALAR 0x005b7958
  static char section_header_005b7958[0x100];
  void *idk = &unused;

  strcpy(section_header_005b7958, "[");
  strcat(section_header_005b7958, section_name);
  strcat(section_header_005b7958, "]");
  return section_header_005b7958;
}

// FUNCTION: SHANDALAR 0x004c8036
char *FindIniHeaderEntry(FILE *file, char *headers_section_name, char *entry_name)
{
  // GLOBAL: SHANDALAR 0x005b7a58
  static char entry_line_005b7a58[0x200];

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
int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name)
{
  char *entry_line;
  int entry_count;
  char *bracket_section_name;
  int found_section;
  int entry_index;
  int *entry_table;
  char *decoded_write_ptr;
  char decoded_line[0x200];
  char *table_entry_str;
  char *scan_ptr;

  entry_line = FindIniHeaderEntry(ini_file, "[headers]", section_name);
  if (entry_line == (char *)0)
  {
    return (int *)0;
  }

  entry_line = strchr(entry_line, ':');
  entry_count = atoi(entry_line + 1);
  bracket_section_name = BuildIniSectionHeader(section_name, 0);
  found_section = SeekIniLine(ini_file, bracket_section_name);
  if (found_section == 0)
  {
    return (int *)0;
  }

  entry_table = (int *)malloc((size_t)entry_count << 2);
  memset(entry_table, 0, (size_t)entry_count << 2);
  strcpy(decoded_line, BuildIniSectionHeader(section_name, 0));

  found_section = SeekIniLine(ini_file, decoded_line);
  assert(found_section, "D:\\Newmagic\\multiplayer\\sid\\iniFile.c", 0x7b, "Could not locate section header!");

  entry_index = 0;
  while (entry_index < entry_count && ReadIniSectionEntry(ini_file, decoded_line))
  {
    decoded_write_ptr = decoded_line;
    scan_ptr = decoded_line;
    while (*scan_ptr != '\0')
    {
      if (*scan_ptr == '\\')
      {
        scan_ptr = scan_ptr + 1;
        if (*scan_ptr == 'n')
        {
          *decoded_write_ptr = '\n';
          decoded_write_ptr = decoded_write_ptr + 1;
          scan_ptr = scan_ptr + 1;
          continue;
        }
        if (*scan_ptr == 'r')
        {
          *decoded_write_ptr = '\r';
          decoded_write_ptr = decoded_write_ptr + 1;
          scan_ptr = scan_ptr + 1;
          continue;
        }
        if (*scan_ptr == 'x')
        {
          *decoded_write_ptr = (char)((IniHexDigitToInt((int)scan_ptr[1]) << 4) + IniHexDigitToInt((int)scan_ptr[2]));
          decoded_write_ptr = decoded_write_ptr + 1;
          scan_ptr = scan_ptr + 3;
          continue;
        }
      }

      *decoded_write_ptr = *scan_ptr;
      decoded_write_ptr = decoded_write_ptr + 1;
      scan_ptr = scan_ptr + 1;
    }

    *decoded_write_ptr = '\0';
    table_entry_str = (char *)malloc(strlen(decoded_line) + 2);
    entry_table[entry_index] = (int)table_entry_str;
    strcpy(table_entry_str, decoded_line);
    entry_index = entry_index + 1;
  }

  return entry_table;
}
