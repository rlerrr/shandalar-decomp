#include <defs.h>

// FUNCTION: MAGIC 0x004a116d
int choose_a_color_dialog(int player, const char *prompt, int use_color_names_instead_of_land, int ai_choice, unsigned int available_colors)
{
  (void)player;
  (void)prompt;
  (void)use_color_names_instead_of_land;

  if (ai_choice > 0 && ai_choice < 6)
  {
    if ((available_colors & (1 << (unsigned char)ai_choice)) != 0)
    {
      return ai_choice;
    }
  }
  if ((available_colors & COLOR_TEST_WHITE) != 0)
  {
    return COLOR_WHITE;
  }
  if ((available_colors & COLOR_TEST_BLUE) != 0)
  {
    return COLOR_BLUE;
  }
  if ((available_colors & COLOR_TEST_BLACK) != 0)
  {
    return COLOR_BLACK;
  }
  if ((available_colors & COLOR_TEST_RED) != 0)
  {
    return COLOR_RED;
  }
  if ((available_colors & COLOR_TEST_GREEN) != 0)
  {
    return COLOR_GREEN;
  }
  return -1;
}