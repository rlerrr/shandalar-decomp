#ifndef MAGIC_SEALED_BUILD_H
#define MAGIC_SEALED_BUILD_H

#include <stdio.h>

extern FILE *g_sealed_deck_report;
extern FILE *g_sealed_deck_build_log;
extern FILE *g_sealed_deck_build_extra_log;
extern int g_sealed_deck_build_report_enabled;
extern int g_sealed_deck_build_report_extra_enabled;

int shell_build_sealed_deck(int *card_pool, int pool_count, int free_land_count,
                           int difficulty, int unknown, int minimum_size,
                           int maximum_size, int *deck, int *deck_count);
int shell_evaluate_sealed_deck(int *deck, int count, int *weights,
                              int *card_values);
unsigned int shell_reduce_sealed_deck_colors(int *deck, int *count,
                                            int minimum_size);
int shell_choose_sealed_card_to_remove(int *deck, int count, int *deck_value);

#endif
