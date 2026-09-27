#ifndef MAGIC_SEALED_OPEN_PACKS_H
#define MAGIC_SEALED_OPEN_PACKS_H

#include <windows.h>
#include "magic_sealed_deck.h"

extern sealed_deck_pack_t *g_sealed_deck_open_packs;
extern int g_sealed_deck_open_pack_count;

void shell_fit_sealed_pack_rect(RECT *result, RECT *bounds,
    int pack_width, int pack_height);
int shell_draw_sealed_starter_pack(HDC dc, RECT *rect, int pack_type,
    int variation, int selected);
int shell_draw_sealed_booster_pack(HDC dc, RECT *rect, int pack_type,
    int variation, int selected);
int shell_draw_large_sealed_booster(HDC dc, RECT *rect, int pack_type,
    int variation, int opened, int cards_remaining);
int shell_draw_large_sealed_starter(HDC dc, RECT *rect, int pack_type,
    int variation, int opened, int cards_remaining);

BOOL CALLBACK shell_sealed_open_packs_dialog_proc(
    HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
int shell_draw_sealed_open_packs_background(HDC dc, RECT *bounds,
    HDC background_dc, HBITMAP background, int width, int height);
void shell_load_sealed_open_packs_resources(HBITMAP *background,
    HBITMAP *highlight, RECT *title, RECT *packs, RECT *opened_pack,
    RECT *cards, COLORREF *title_color, COLORREF *text_color,
    COLORREF *help_color, HBRUSH *button_brush, HPEN *light_pen,
    HPEN *dark_pen, COLORREF *button_color);
void shell_release_sealed_open_packs_resources(HBITMAP background,
    HBITMAP highlight, HBRUSH button_brush, HPEN light_pen, HPEN dark_pen);
int shell_sealed_open_card_rect(RECT *result, int stack_index,
    int revealed_count, RECT *bounds);
void shell_reveal_sealed_pack_cards(HWND hwnd, sealed_deck_pack_t *pack,
    int direction, int card_count, HBITMAP background, RECT *pack_rect,
    RECT *cards_rect);

#endif
