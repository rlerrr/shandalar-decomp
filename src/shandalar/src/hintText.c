#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "shandalar.h"
#include "shandalar_global_strings.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

typedef struct
{
  int first;
  int second;
} HintPair;

extern char g_ui_message_buffer[0x1000];
extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern card_data_t global_cards_data[];
extern long DAT_0097db40[0x100];
extern HintPair DAT_0097e450[0x100];

int DeckContainsCsvid(int csvid);
int FUN_0056c705(int card_id);
DWORD FUN_00564e70(char *dst, DWORD max_length, LPCVOID format, ...);
char *FUN_0057e826(char *dst, char *src);
int ReadSpriteEntryPointers(EncodedImage **out_sprite_entries, char *sprite_path);
int ScaleUiCoordinate(int value);
int MeasureTextSpanWidth(FacemakerWindowBounds *window, char *text, int length);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text);
unsigned int WaitForInputEventUnlessBlocked(void);
void FreeSpriteBlob(void *sprite_blob);

// GLOBAL: SHANDALAR 0x00580f94
char DAT_00580f94[] = "rt";
// GLOBAL: SHANDALAR 0x00580f98
char s_hints_txt_00580f98[] = "hints.txt";
// GLOBAL: SHANDALAR 0x00580fa4
char s_______00580fa4[] = "%[^\n]";
// GLOBAL: SHANDALAR 0x00580fac
char DAT_00580fac[] = "\n";
// GLOBAL: SHANDALAR 0x00580fb0
char DAT_00580fb0[] = "\n";
// GLOBAL: SHANDALAR 0x00580fb4
char DAT_00580fb4[] = "\n";
// GLOBAL: SHANDALAR 0x00580fb8
char DAT_00580fb8[] = "\n";
// GLOBAL: SHANDALAR 0x00580fbc
char DAT_00580fbc[] = "\n";
// GLOBAL: SHANDALAR 0x00580fc0
char DAT_00580fc0[] = "\n";
// GLOBAL: SHANDALAR 0x00580fc4
char DAT_00580fc4[] = "\x94";
// GLOBAL: SHANDALAR 0x00580fc8
char s_BuyButtons_spr_00580fc8[] = "BuyButtons.spr";

// FUNCTION: SHANDALAR 0x0041871f
void AppendTownHintBodyText(int hint_index)
{
  char hint_text[256];
  FILE *hints_file;
  int item_count;

  hints_file = fopen(s_hints_txt_00580f98, DAT_00580f94);
  fseek(hints_file, DAT_0097db40[hint_index], 0);
  item_count = fscanf(hints_file, s_______00580fa4, hint_text);
  strcat(g_ui_message_buffer, hint_text);
  fclose(hints_file);
}

// FUNCTION: SHANDALAR 0x00418a8f
char *FindTextWordBreak(char *text)
{
  while ((*text != '\0') && (*text == ' ') && (*text != '\n'))
  {
    text++;
  }

  while ((*text != '\0') && (*text != ' ') && (*text != '\n'))
  {
    text++;
  }

  if (*text != '\0')
  {
    return text;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: SHANDALAR 0x00418b24
char *WrapTextToWidthForDropCap(char *src, char *dst, int max_width)
{
  int word_width;
  char *word_end;
  int line_width;
  size_t word_length;
  char *scan;
  char current_line[256];

  line_width = 0;
  scan = src;
  current_line[0] = '\0';
  dst[0] = '\0';

  while ((word_end = FindTextWordBreak(scan)) != (char *)0)
  {
    word_length = (int)word_end - (int)scan;
    word_width = MeasureTextSpanWidth(PTR_DAT_005832b4, scan, word_length);
    if (max_width < line_width + word_width)
    {
      strcat(current_line, DAT_00580fac);
      strcat(dst, current_line);
      current_line[0] = '\0';
      while ((scan != (char *)0) && (*scan == ' '))
      {
        scan++;
        word_length--;
      }
      strncat(current_line, scan, word_length);
      while ((*word_end != '\0') && (*word_end == '\n'))
      {
        strcat(dst, DAT_00580fb0);
        word_end++;
      }
      line_width = MeasureTextSpanWidth(PTR_DAT_005832b4, word_end, word_length);
      scan = word_end;
    }
    else
    {
      word_width = MeasureTextSpanWidth(PTR_DAT_005832b4, scan, word_length);
      line_width += word_width;
      strncat(current_line, scan, word_length);
      if (*word_end == '\n')
      {
        strcat(dst, current_line);
      }
      while ((scan = word_end, *word_end != '\0') && (*word_end == '\n'))
      {
        strcat(dst, DAT_00580fb4);
        line_width = 0;
        current_line[0] = '\0';
        word_end++;
      }
    }
  }

  word_width = MeasureTextSpanWidth(PTR_DAT_005832b4, scan, word_length);
  if (max_width < line_width + word_width)
  {
    strcat(current_line, DAT_00580fb8);
    strcat(dst, current_line);
    strcat(dst, scan);
  }
  else
  {
    strcat(dst, current_line);
    strcat(dst, scan);
  }

  return dst;
}

// FUNCTION: SHANDALAR 0x00418e15
char *WrapTextToWidthForPopup(char *src, char *dst, int max_width)
{
  int done;
  int word_width;
  char *word_end;
  int line_width;
  size_t word_length;
  char *scan;
  char current_line[256];

  if (dst == (char *)0)
  {
    dst = (char *)0;
  }
  else if (src == (char *)0)
  {
    dst[0] = '\0';
  }
  else
  {
    dst[0] = '\0';
    scan = src;
    current_line[0] = '\0';
    line_width = 0;
    done = 0;

    while (done == 0)
    {
      if (*scan == '\0')
      {
        strcat(dst, current_line);
        done = 1;
      }
      else if (*scan == '\n')
      {
        strcat(current_line, DAT_00580fbc);
        strcat(dst, current_line);
        current_line[0] = '\0';
        line_width = 0;
        scan++;
      }
      else
      {
        word_end = scan;
        while (*word_end == ' ')
        {
          word_end++;
        }
        while ((*word_end != ' ') && (*word_end != '\n') && (*word_end != '\0'))
        {
          word_end++;
        }
        word_length = (int)word_end - (int)scan;
        word_width = MeasureTextSpanWidth(PTR_DAT_005832b4, scan, word_length);
        if (max_width < word_width + line_width)
        {
          strcat(current_line, DAT_00580fc0);
          strcat(dst, current_line);
          current_line[0] = '\0';
          line_width = 0;
          while (*scan == ' ')
          {
            scan++;
            word_length--;
          }
        }
        strncat(current_line, scan, word_length);
        line_width += word_width;
        scan = word_end;
      }
    }
  }

  return dst;
}

// FUNCTION: SHANDALAR 0x0041905e
void ShowTownHintTextPopup(int hint_index)
{
  int card_name_index;
  int popup_width;
  int popup_y;
  int popup_x;
  int i;
  EncodedImage *loaded_button_sprites[12];
  int player_has_first_card;
  EncodedImage *button_sprites[12];
  char wrapped_hint_text[1000];

  if (DAT_0097e450[hint_index].second != -1)
  {
    card_name_index = DeckContainsCsvid(DAT_0097e450[hint_index].first);
    player_has_first_card = (card_name_index != 0);
    card_name_index = FUN_0056c705((&DAT_0097e450[hint_index].first)[player_has_first_card ^ 1]);
    popup_width = FUN_0056c705((&DAT_0097e450[hint_index].first)[player_has_first_card]);
    FUN_00564e70(g_ui_message_buffer, 0x1000, gs_hinttext_0077e580[0], global_cards_data[popup_width].name,
                 global_cards_data[card_name_index].name);
  }
  else
  {
    card_name_index = FUN_0056c705(DAT_0097e450[hint_index].first);
    sprintf(g_ui_message_buffer, gs_hinttext_0077e580[1], global_cards_data[card_name_index].name);
  }

  AppendTownHintBodyText(hint_index);
  FUN_0057e826(g_ui_message_buffer, DAT_00580fc4);
  ReadSpriteEntryPointers(loaded_button_sprites, s_BuyButtons_spr_00580fc8);

  button_sprites[10] = loaded_button_sprites[0];
  button_sprites[11] = loaded_button_sprites[1];
  button_sprites[6] = loaded_button_sprites[2];
  for (i = 0; i < 3; i++)
  {
    button_sprites[7 + i] = loaded_button_sprites[3 + i];
  }
  for (i = 0; i < 3; i++)
  {
    button_sprites[3 + i] = loaded_button_sprites[6 + i];
  }
  for (i = 0; i < 3; i++)
  {
    button_sprites[i] = loaded_button_sprites[9 + i];
  }

  card_name_index = ScaleUiCoordinate(0x10f);
  popup_width = ScaleUiCoordinate(0xc5);
  popup_y = ScaleUiCoordinate(0x34);
  popup_x = ScaleUiCoordinate(0xdc);
  DrawEncodedImageResampled(PTR_DAT_005832b4, popup_x, popup_y, popup_width, card_name_index, loaded_button_sprites[0]);

  card_name_index = ScaleUiCoordinate(0x85);
  WrapTextToWidthForPopup(g_ui_message_buffer, wrapped_hint_text, card_name_index);
  DrawTextAt(PTR_DAT_005832b4, 0xfe, 0x140, 0xbc, wrapped_hint_text);
  WaitForInputEventUnlessBlocked();
  FreeSpriteBlob(button_sprites[10]);
}
