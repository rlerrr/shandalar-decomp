#define GLOBAL_STRINGS_IMPL
#include "global_strings.h"
#include "global_state.h"
#include <stdio.h>
#include "cardartlib/src/assert.h"

// FUNCTION: MAGIC 0x004ec830
int load_text(const char *file_name, const char *section_name)
{
  struct
  {
    FILE *f;
    char line[300];
    char path[128];
    char section_line[300]; // ebp - 0x134
    unsigned int num_text;
    int i;
  } s;

  strcpy(s.section_line, "@");
  strcat(s.section_line, section_name);
  strcat(s.section_line, "\n");

  strcpy(s.path, global_base_directory);
  strcat(s.path, "\\");
  strcat(s.path, file_name);
  // if (!strchr(file_name, '.'))
  //   strcat(s.path, ".txt");

  s.f = fopen(s.path, "rt");
  if (s.f == NULL)
    return -1;

  while (strcmp(s.section_line, s.line))
  {
    if (!fgets(s.line, 300, s.f))
    {
      fclose(s.f);
      return -1;
    }
  }

  fscanf(s.f, "%d", &s.num_text);
  fgets(s.line, 300, s.f);
  if (s.num_text > 225)
  {
    fclose(s.f);
    return -1;
  }

  for (s.i = 0; s.i < (int)s.num_text; s.i++)
  {
    if (!fgets(text_lines[s.i], 300, s.f))
    {
      fclose(s.f);
      return -1;
    }
    text_lines[s.i][strlen(text_lines[s.i]) - 1] = 0;
  }

  fclose(s.f);
  return s.num_text;
}

// FUNCTION: MAGIC 0x004eca6d
static int FUN_004eca6d(char *filename, char *section_name)
{
  int loaded_count;
  int line_index;
  size_t line_length;
  int out_index;
  int char_index;

  if (unk_008a9000 == 1)
  {
    loaded_count = 0;
  }
  else
  {
    loaded_count = load_text(filename, section_name);
    for (line_index = 0; line_index < abs(loaded_count); ++line_index)
    {
      line_length = strlen(text_lines[line_index]);
      out_index = 0;
      for (char_index = 0; char_index < (int)line_length; ++char_index)
      {
        if (text_lines[line_index][char_index] == '\\' && text_lines[line_index][char_index + 1] == 'n')
        {
          text_lines[line_index][out_index] = '\n';
          ++char_index;
        }
        else
        {
          text_lines[line_index][out_index] = text_lines[line_index][char_index];
        }

        ++out_index;
      }

      text_lines[line_index][out_index] = '\0';
    }
  }

  return loaded_count;
}

// FUNCTION: MAGIC 0x00491f1e
void FUN_00491f1e(char *filename)
{
  int loaded_count;
  int color_index;

  load_text("MP_UISTRINGS.TXT", "MULTIPLAYERSTRINGS");
  strcpy(DAT_007a7d30, text_lines[0]);

  load_text(filename, "GAMETITLE");
  strcpy(DAT_Magic_The_Gathering_00789460, text_lines[0]);

  load_text(filename, "COLORWORDS");
  loaded_count = 0;
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(DAT_00709210[color_index], text_lines[loaded_count]);
    ++loaded_count;
  }
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(DAT_00709390[color_index], text_lines[loaded_count]);
    ++loaded_count;
  }
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(DAT_00709190[color_index], text_lines[loaded_count]);
    ++loaded_count;
  }

  load_text(filename, "COLORLESSMANA");
  strcpy(DAT_00709210[0], text_lines[0]);
  strcpy(DAT_00709390[0], text_lines[0]);

  load_text(filename, "LANDWORDS");
  loaded_count = 0;
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(DAT_00709310[color_index], text_lines[loaded_count]);
    ++loaded_count;
  }
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(DAT_00709290[color_index], text_lines[loaded_count]);
    ++loaded_count;
  }
  for (color_index = 1; color_index < 6; ++color_index)
  {
    strcpy(DAT_00709410[color_index], text_lines[loaded_count]);
    ++loaded_count;
  }

  load_text(filename, "ABILITYWORDS");
  loaded_count = 0;
  for (color_index = 0; color_index < 0x11; ++color_index)
  {
    strcpy(DAT_008b4de0[color_index], text_lines[loaded_count]);
    ++loaded_count;
  }

  loaded_count = load_text(filename, "HUNTING_SUBTYPENAMES");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_00926930[color_index], text_lines[color_index]);
  }

  loaded_count = load_text(filename, "WORDSTOHACK_BLACK");
  DAT_008cd924 = loaded_count;
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_00789c80[COLOR_BLACK][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSTOHACK_BLUE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_00789c80[COLOR_BLUE][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSTOHACK_GREEN");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_00789c80[COLOR_GREEN][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSTOHACK_RED");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_00789c80[COLOR_RED][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSTOHACK_WHITE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_00789c80[COLOR_WHITE][color_index], text_lines[color_index]);
  }

  load_text(filename, "WORDSHACKED_BLACK");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_0091d260[COLOR_BLACK][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSHACKED_BLUE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_0091d260[COLOR_BLUE][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSHACKED_GREEN");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_0091d260[COLOR_GREEN][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSHACKED_RED");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_0091d260[COLOR_RED][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSHACKED_WHITE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_0091d260[COLOR_WHITE][color_index], text_lines[color_index]);
  }

  loaded_count = load_text(filename, "WORDSTOSLEIGHT_BLACK");
  DAT_00789c34 = loaded_count;
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_00781bd0[COLOR_BLACK][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSTOSLEIGHT_BLUE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_00781bd0[COLOR_BLUE][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSTOSLEIGHT_GREEN");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_00781bd0[COLOR_GREEN][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSTOSLEIGHT_RED");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_00781bd0[COLOR_RED][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSTOSLEIGHT_WHITE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_00781bd0[COLOR_WHITE][color_index], text_lines[color_index]);
  }

  load_text(filename, "WORDSLEIGHTED_BLACK");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_008a9c10[COLOR_BLACK][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSLEIGHTED_BLUE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_008a9c10[COLOR_BLUE][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSLEIGHTED_GREEN");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_008a9c10[COLOR_GREEN][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSLEIGHTED_RED");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_008a9c10[COLOR_RED][color_index], text_lines[color_index]);
  }
  load_text(filename, "WORDSLEIGHTED_WHITE");
  for (color_index = 0; color_index < loaded_count; ++color_index)
  {
    strcpy(DAT_008a9c10[COLOR_WHITE][color_index], text_lines[color_index]);
  }

  load_text(filename, "PROMPT_STILLTHINKING");
  strcpy(DAT_0091bd10, text_lines[0]);

  load_text(filename, "ABOUTBOX");
  strcpy(DAT_007aa930, text_lines[0]);
  strcat(DAT_007aa930, DAT_005721a4);
  strcat(DAT_007aa930, text_lines[1]);
  strcat(DAT_007aa930, DAT_005721a8);
  strcat(DAT_007aa930, text_lines[2]);

  load_text(filename, "PROMPT_MAIN");
  strcpy(DAT_008cc710, text_lines[0]);
  strcpy(DAT_00777970, text_lines[1]);
  strcpy(DAT_007ab2d0, text_lines[2]);
  strcpy(DAT_0091cf50, text_lines[3]);
  strcpy(DAT_0093d990, text_lines[4]);
  strcpy(DAT_008b3d20, text_lines[5]);
  strcpy(DAT_007a7af0, text_lines[6]);
  strcpy(DAT_008b4140, text_lines[7]);

  load_text(filename, "PROMPT_NEWFULLCARD");
  strcpy(DAT_0091b150, text_lines[0]);

  load_text(filename, "PROMPT_ACTION");
  strcpy(DAT_008b4850, text_lines[0]);
  strcpy(DAT_007912e0, text_lines[1]);
  strcpy(DAT_008ce290, text_lines[2]);

  load_text(filename, "PROMPT_FASTEFFECTS");
  strcpy(DAT_009396a0, text_lines[0]);
  strcpy(DAT_0093a850, text_lines[1]);
  strcpy(DAT_00926520, text_lines[2]);

  load_text(filename, "PROMPT_CHECKFEPHASE");
  strcpy(DAT_00939200, text_lines[0]);
  strcpy(DAT_007a79c0, text_lines[1]);
  strcpy(DAT_008b4720, text_lines[2]);
  strcpy(DAT_0091b2d0, text_lines[3]);
  strcpy(DAT_00789320, text_lines[4]);
  strcpy(DAT_00777d30, text_lines[5]);
  strcpy(DAT_0091c100, text_lines[6]);
  strcpy(DAT_008cee80, text_lines[7]);
  strcpy(DAT_008b45f0, text_lines[8]);
  strcpy(DAT_008a8c40, text_lines[9]);
  strcpy(DAT_0091d080, text_lines[10]);
  strcpy(DAT_0091c680, text_lines[0xb]);

  load_text("promptsX1.txt", "PROMPT_ENDHEALING");
  strcpy(DAT_00789740, text_lines[0]);

  load_text("promptsX1.txt", "PROMPT_ATTACKERSELECTION");
  strcpy(DAT_008b32d0, text_lines[0]);

  load_text("promptsX1.txt", "PROMPT_BLOCKERSELECTION");
  strcpy(DAT_00926210, text_lines[0]);

  load_text("promptsX1.txt", "PROMPT_GAINLIFE");
  strcpy(DAT_007895e0, text_lines[0]);

  load_text("promptsX2.txt", "PROMPT_TURNSEQUENCE");
  strcpy(DAT_009263f0, text_lines[0]);
  strcpy(DAT_0091c510, text_lines[1]);
  strcpy(DAT_008b26c0, text_lines[2]);
  strcpy(DAT_007ab020, text_lines[3]);

  load_text(filename, "PROMPT_SPECIALFEPHASE");
  strcpy(DAT_0091c840, text_lines[0]);
  strcpy(DAT_007aaef0, text_lines[1]);
  strcpy(DAT_00777ab0, text_lines[2]);
  strcpy(DAT_0091cbd0, text_lines[3]);
  strcpy(DAT_008a8ed0, text_lines[4]);
  strcpy(DAT_008961c0, text_lines[5]);
  strcpy(DAT_00896400, text_lines[6]);
  strcpy(DAT_00925d40, text_lines[7]);
  strcpy(DAT_008cf080, text_lines[8]);
  strcpy(DAT_00925c00, text_lines[9]);
  strcpy(DAT_008cf560, text_lines[10]);
  strcpy(DAT_00939570, text_lines[0xb]);
  strcpy(DAT_007a7880, text_lines[0xc]);
  strcpy(DAT_00789a50, text_lines[0xd]);
  strcpy(DAT_008b43a0, text_lines[0xe]);
  strcpy(DAT_008cd980, text_lines[0xf]);

  load_text(filename, "PROMPT_GRABMANA");
  strcpy(DAT_0091c230, text_lines[0]);
  strcpy(DAT_008a9010, text_lines[1]);
  strcpy(DAT_00791420, text_lines[2]);

  load_text(filename, "PROMPT_ILLEGALTARGET");
  strcpy(DAT_008b3e50, text_lines[0]);
  strcpy(DAT_008ce7e0, text_lines[1]);

  load_text(filename, "PROMPT_ILLEGALTARGETWHY");
  strcpy(DAT_008b3110, text_lines[0]);
  strcpy(DAT_0091a6e0, text_lines[1]);
  strcpy(DAT_0091cd00, text_lines[2]);
  strcpy(DAT_0091bbe0, text_lines[3]);
  strcpy(DAT_007aacc0, text_lines[4]);
  strcpy(DAT_008a9a00, text_lines[5]);
  strcpy(DAT_0091a810, text_lines[6]);
  strcpy(DAT_008ce3c0, text_lines[7]);
  strcpy(DAT_008cfde0, text_lines[8]);
  strcpy(DAT_008cea20, text_lines[9]);
  strcpy(DAT_00781aa0, text_lines[10]);
  strcpy(DAT_007894b0, text_lines[0xb]);
  strcpy(DAT_008b34c0, text_lines[0xc]);
  strcpy(DAT_0091be40, text_lines[0xd]);
  strcpy(DAT_0093d860, text_lines[0xe]);
  strcpy(DAT_008ced40, text_lines[0xf]);
  strcpy(DAT_0091c360, text_lines[0x10]);
  strcpy(DAT_00925080, text_lines[0x11]);
  strcpy(DAT_00896540, text_lines[0x12]);
  strcpy(DAT_0091bfc0, text_lines[0x13]);
  strcpy(DAT_0091caa0, text_lines[0x14]);
  strcpy(DAT_007ab160, text_lines[0x15]);
  strcpy(DAT_008cc850, text_lines[0x16]);
  strcpy(DAT_008cf3c0, text_lines[0x17]);
  strcpy(DAT_00939050, text_lines[0x18]);
  strcpy(DAT_008ce6a0, text_lines[0x19]);
  strcpy(DAT_007911b0, text_lines[0x1a]);
  strcpy(DAT_008b3be0, text_lines[0x1b]);
  strcpy(DAT_008ceb50, text_lines[0x1c]);

  load_text(DAT_UIStrings_txt_008962f0, "DIALOGBUTTONS");
  strcpy(DAT_00924800, text_lines[0]);
  strcpy(unk_008a8c20, text_lines[1]);
  strcpy(unk_008b40e0, text_lines[2]);

  load_text(DAT_UIStrings_txt_008962f0, "DIALOG_DUELOPTIONS");
  for (color_index = 0; color_index < 0x13; ++color_index)
  {
    strcpy(DAT_008b35f0[color_index], text_lines[color_index]);
  }

  load_text(DAT_UIStrings_txt_008962f0, "CARDTITLES");
  strcpy(unk_008cfd30, text_lines[0]);
  strcpy(unk_00926750, text_lines[1]);
  strcpy(DAT_007aaeb0, text_lines[2]);
  strcpy(DAT_008b4100, text_lines[3]);
  strcpy(unk_008b4330, text_lines[4]);

  load_text(DAT_UIStrings_txt_008962f0, "MULTIBLOCKCARD");
  strcpy(unk_008cf040, text_lines[0]);
  strcpy(DAT_00925e70, text_lines[1]);

  load_text(DAT_UIStrings_txt_008962f0, "ENCHANTMENTTYPENAME");
  strcpy(DAT_008b3f80, text_lines[0]);

  load_text(DAT_UIStrings_txt_008962f0, "ARTISTLINE");
  strcpy(DAT_00789130, text_lines[0]);

  load_text(DAT_UIStrings_txt_008962f0, "WINDOWTITLES");
  strcpy(DAT_009267a0, text_lines[0]);
  strcpy(DAT_00926010, text_lines[1]);
  strcpy(DAT_008b42f0, text_lines[2]);
  strcpy(DAT_0091c820, text_lines[3]);
  strcpy(DAT_00777bf0, text_lines[4]);
  strcpy(DAT_008951d0, text_lines[5]);

  load_text(DAT_UIStrings_txt_008962f0, "CUECARD_OTHER");
  strcpy(DAT_008b4280, text_lines[0]);
  strcpy(DAT_00926090, text_lines[1]);
  strcpy(DAT_00789bd0, text_lines[2]);
  strcpy(DAT_0091ca20, text_lines[3]);
  strcpy(DAT_0091c7b0, text_lines[4]);
  strcpy(DAT_0091c9b0, text_lines[5]);
  strcpy(DAT_008b2900, text_lines[6]);
  strcpy(DAT_0093a810, text_lines[7]);

  load_text(DAT_UIStrings_txt_008962f0, "CUECARD_LIFE");
  strcpy(DAT_008b30a0, text_lines[0]);
  strcpy(DAT_009252f0, text_lines[1]);
  strcpy(DAT_009394a0, text_lines[2]);
  strcpy(DAT_008a9ba0, text_lines[3]);
  strcpy(DAT_0091d1b0, text_lines[4]);
  strcpy(DAT_008b2890, text_lines[5]);
  strcpy(DAT_007a7810, text_lines[6]);
  strcpy(DAT_008cf4f0, text_lines[7]);

  load_text(DAT_UIStrings_txt_008962f0, "CUECARD_MANAPOOL");
  for (color_index = 0; color_index < 7; ++color_index)
  {
    strcpy(DAT_008cff20[color_index], text_lines[color_index]);
  }
  for (color_index = 0; color_index < 7; ++color_index)
  {
    strcpy(DAT_008cc2e0[color_index], text_lines[color_index + 7]);
  }

  load_text(DAT_UIStrings_txt_008962f0, "CUECARD_PHASEBAR");
  strcpy(DAT_008a8d80, text_lines[0]);
  strcpy(DAT_008a9b30, text_lines[1]);
  strcpy(DAT_0091c490, text_lines[2]);
  strcpy(DAT_00924790, text_lines[3]);
  strcpy(DAT_00926380, text_lines[4]);
  strcpy(DAT_00925fa0, text_lines[5]);
  strcpy(DAT_00938e40, text_lines[6]);
  strcpy(DAT_007aae40, text_lines[7]);
  strcpy(DAT_008b3280, text_lines[8]);
  strcpy(DAT_007a7c20, text_lines[9]);
  strcpy(DAT_0091b110, text_lines[10]);
  strcpy(DAT_008b2840, text_lines[0xb]);
  strcpy(DAT_007a7cd0, text_lines[0xc]);
  strcpy(DAT_00926670, text_lines[0xd]);
  strcpy(DAT_00939350, text_lines[0xe]);
  strcpy(DAT_00789b90, text_lines[0xf]);
  strcpy(DAT_008966e0, text_lines[0x10]);
  strcpy(DAT_0093a7c0, text_lines[0x11]);
  strcpy(DAT_00925a90, text_lines[0x12]);
  strcpy(DAT_00925bc0, text_lines[0x13]);
  strcpy(DAT_0091d220, text_lines[0x14]);
  strcpy(DAT_008cec80, text_lines[0x15]);
  strcpy(DAT_00789870, text_lines[0x16]);

  load_text(DAT_UIStrings_txt_008962f0, "CUECARD_SMALLCARD");
  strcpy(DAT_007a77d0, text_lines[0]);
  strcpy(DAT_00925040, text_lines[1]);
  strcpy(DAT_0091b290, text_lines[2]);
  strcpy(DAT_008a8e40, text_lines[3]);
  strcpy(DAT_00789c40, text_lines[4]);
  strcpy(DAT_008b4990, text_lines[5]);
  strcpy(DAT_008cf230, text_lines[6]);
  strcpy(DAT_0091c640, text_lines[7]);
  strcpy(DAT_008a8e00, text_lines[8]);
  strcpy(DAT_007aae00, text_lines[9]);

  FUN_004eca6d(DAT_UIStrings_txt_008962f0, "PROMPT_RESOLUTIONERROR");
  strcpy(DAT_007898b0, text_lines[0]);
}
