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
extern FacemakerWindowBounds *g_page0_window_bounds;
extern card_data_t global_cards_data[];
extern long g_hint_text_offsets[0x100];
extern HintPair g_hint_card_pairs[0x100];

int DeckContainsCsvid(int csvid);
int find_internal_card_id_by_csv_id(card_id_t card_id);
DWORD FormatMessageFromStringStripCarriageReturns(char *dst, DWORD max_length, LPCVOID format, ...);
char *AppendString(char *dst, char *src);
int ReadSpriteEntryPointers(EncodedImage **out_sprite_entries, char *sprite_path);
int ScaleUiCoordinate(int value);
int MeasureTextSpanWidth(FacemakerWindowBounds *window, char *text, int length);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text, ...);
unsigned int WaitForInputEventUnlessBlocked(void);
void FreeSpriteBlob(void *sprite_blob);

// FUNCTION: SHANDALAR 0x0041871f
void AppendTownHintBodyText(int hint_index)
{
  struct
  {
    char hint_text[256];
    FILE *hints_file;
    int item_count;
  } s;

  s.hints_file = fopen("hints.txt", "rt");
  fseek(s.hints_file, g_hint_text_offsets[hint_index], 0);
  s.item_count = fscanf(s.hints_file, "%[^\n]", s.hint_text);
  strcat(g_ui_message_buffer, s.hint_text);
  fclose(s.hints_file);
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

#pragma intrinsic(memset)

// FUNCTION: SHANDALAR 0x00418b24
char *WrapTextToWidthForDropCap(char *src, char *dst, int max_width)
{
  struct
  {
    char *word_end;
    int line_width;
    size_t word_length;
    char *scan;
    char current_line[256];
  } s;

  s.line_width = 0;
  s.scan = src;
  s.current_line[0] = '\0';
  memset(&s.current_line[1], 0, 254);
  dst[0] = '\0';

  while ((s.word_end = FindTextWordBreak(s.scan)) != (char *)0)
  {
    s.word_length = (int)s.word_end - (int)s.scan;
    if (max_width < s.line_width + MeasureTextSpanWidth(g_page0_window_bounds, s.scan, s.word_length))
    {
      strcat(s.current_line, "\n");
      strcat(dst, s.current_line);
      s.current_line[0] = '\0';
      while ((s.scan != (char *)0) && (*s.scan == ' '))
      {
        s.scan++;
        s.word_length--;
      }
      strncat(s.current_line, s.scan, s.word_length);
      while ((*s.word_end != '\0') && (*s.word_end == '\n'))
      {
        strcat(dst, "\n");
        s.word_end++;
      }
      s.scan = s.word_end;
      s.line_width = MeasureTextSpanWidth(g_page0_window_bounds, s.scan, s.word_length);
    }
    else
    {
      s.line_width += MeasureTextSpanWidth(g_page0_window_bounds, s.scan, s.word_length);
      strncat(s.current_line, s.scan, s.word_length);
      if (*s.word_end == '\n')
      {
        strcat(dst, s.current_line);
      }
      while ((*s.word_end != '\0') && (*s.word_end == '\n'))
      {
        strcat(dst, "\n");
        s.word_end++;
        s.line_width = 0;
        s.current_line[0] = '\0';
      }
      s.scan = s.word_end;
    }
  }

  if (max_width < s.line_width + MeasureTextSpanWidth(g_page0_window_bounds, s.scan, s.word_length))
  {
    strcat(s.current_line, "\n");
    strcat(dst, s.current_line);
    strcat(dst, s.scan);
  }
  else
  {
    strcat(dst, s.current_line);
    strcat(dst, s.scan);
  }

  return dst;
}

// FUNCTION: SHANDALAR 0x00418e15
char *WrapTextToWidthForPopup(char *src, char *dst, int max_width)
{
  struct
  {
    char *word_end;
    int line_width;
    size_t word_length;
    int done;
    char *scan;
    int word_width;
    char current_line[256];
  } s;

  if (dst == (char *)0)
  {
    return 0;
  }
  if (src == (char *)0)
  {
    dst[0] = '\0';
    return dst;
  }
  dst[0] = '\0';
  s.scan = src;
  s.current_line[0] = '\0';
  s.line_width = 0;
  s.done = 0;

  while (s.done == 0)
  {
    if (*s.scan == '\0')
    {
      strcat(dst, s.current_line);
      s.done = 1;
    }
    else if (*s.scan == '\n')
    {
      strcat(s.current_line, "\n");
      strcat(dst, s.current_line);
      s.current_line[0] = '\0';
      s.line_width = 0;
      s.scan++;
    }
    else
    {
      s.word_end = s.scan;
      while (*s.word_end == ' ')
      {
        s.word_end++;
      }
      while ((*s.word_end != ' ') && (*s.word_end != '\n') && (*s.word_end != '\0'))
      {
        s.word_end++;
      }
      s.word_length = (int)s.word_end - (int)s.scan;
      s.word_width = MeasureTextSpanWidth(g_page0_window_bounds, s.scan, s.word_length);
      if (max_width < s.word_width + s.line_width)
      {
        strcat(s.current_line, "\n");
        strcat(dst, s.current_line);
        s.current_line[0] = '\0';
        s.line_width = 0;
        while (*s.scan == ' ')
        {
          s.scan++;
          s.word_length--;
        }
      }
      strncat(s.current_line, s.scan, s.word_length);
      s.line_width += s.word_width;
      s.scan = s.word_end;
    }
  }

  return dst;
}

// FUNCTION: SHANDALAR 0x0041905e
void ShowTownHintTextPopup(int hint_index)
{
  struct
  {
    int i;
    EncodedImage *loaded_button_sprites[12];
    int card_name_index;
    int player_has_first_card;
    EncodedImage *button_sprites[10];
    char wrapped_hint_text[1000];
    EncodedImage *button_sprite_10;
    EncodedImage *button_sprite_11;
  } s;

  if (g_hint_card_pairs[hint_index].second != -1)
  {
    s.player_has_first_card = (DeckContainsCsvid(g_hint_card_pairs[hint_index].first) != 0);
    FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_hinttext_0077e580[0],
                                                global_cards_data[find_internal_card_id_by_csv_id((&g_hint_card_pairs[hint_index].first)[s.player_has_first_card])].name,
                                                global_cards_data[find_internal_card_id_by_csv_id((&g_hint_card_pairs[hint_index].first)[s.player_has_first_card ^ 1])].name);
  }
  else
  {
    sprintf(g_ui_message_buffer, gs_hinttext_0077e580[1],
            global_cards_data[find_internal_card_id_by_csv_id(g_hint_card_pairs[hint_index].first)].name);
  }

  AppendTownHintBodyText(hint_index);
  AppendString(g_ui_message_buffer, "\x94");
  ReadSpriteEntryPointers(s.loaded_button_sprites, "BuyButtons.spr");

  s.button_sprite_10 = s.loaded_button_sprites[0];
  s.button_sprite_11 = s.loaded_button_sprites[1];
  s.button_sprites[6] = s.loaded_button_sprites[2];
  for (s.i = 0; s.i < 3; s.i++)
  {
    s.button_sprites[7 + s.i] = s.loaded_button_sprites[3 + s.i];
  }
  for (s.i = 0; s.i < 3; s.i++)
  {
    s.button_sprites[3 + s.i] = s.loaded_button_sprites[6 + s.i];
  }
  for (s.i = 0; s.i < 3; s.i++)
  {
    s.button_sprites[s.i] = s.loaded_button_sprites[9 + s.i];
  }

  DrawEncodedImageResampled(g_page0_window_bounds,
                            ScaleUiCoordinate(0xdc),
                            ScaleUiCoordinate(0x34),
                            ScaleUiCoordinate(0xc5),
                            ScaleUiCoordinate(0x10f),
                            s.button_sprite_10);

  WrapTextToWidthForPopup(g_ui_message_buffer, s.wrapped_hint_text, ScaleUiCoordinate(0x85));
  DrawTextAt(g_page0_window_bounds, 0xfe, 0x140, 0xbc, s.wrapped_hint_text);
  WaitForInputEventUnlessBlocked();
  FreeSpriteBlob(s.button_sprite_10);
}
