#ifndef DUEL_ENGINE_H
#define DUEL_ENGINE_H

#include <windows.h>

#define CLASS_MAGICGAME_MAINCLASS "MAGICGAME_MainClass"
#define CLASS_MAGICGAME_LIFE "MAGICGAME_LifeClass"
#define CLASS_MAGICGAME_FULL_CARD "MAGICGAME_FullCardClass"
#define CLASS_MAGICGAME_MANA_SUMMARY "MAGICGAME_ManaSummaryClass"
#define CLASS_MAGICGAME_HAND "MAGICGAME_HandClass"
#define CLASS_MAGICGAME_CHAT "MAGICGAME_ChatClass"
#define CLASS_MAGICGAME_CARD "MAGICGAME_CardClass"
#define CLASS_MAGICGAME_PHASE_DISPLAY "MAGICGAME_PhaseDisplayClass"
#define CLASS_MAGICGAME_ATTACK_PHASE_DISPLAY "MAGICGAME_AttackPhaseDisplayClass"
#define CLASS_MAGICGAME_TERRITORY "MAGICGAME_TerritoryClass"
#define CLASS_MAGICGAME_LIBRARY "MAGICGAME_LibraryClass"
#define CLASS_MAGICGAME_GRAVEYARD "MAGICGAME_GraveyardClass"
#define CLASS_MAGICGAME_ATTACK "MAGICGAME_AttackClass"
#define CLASS_MAGICGAME_SPELL_CHAIN "MAGICGAME_SpellChainClass"
#define CLASS_MAGICGAME_FACE "MAGICGAME_FaceClass"
#define CLASS_MAGICGAME_SCROLLBAR "MAGICGAME_ScrollbarClass"
#define CLASS_MAGICTHEME_ICON_BUTTON "MAGICTHEME_IconButtonClass"
#define CLASS_MAGICGAME_BIG_CARD_CHOICE "MAGICGAME_BigCardChoiceClass"
#define CLASS_MAGICGAME_BIG_CARD_CARD "MAGICGAME_BigCardCardClass"
#define CLASS_MAGIC_PALETTE "MAGIC_PaletteClass"
#define CLASS_MAGIC_CUE_CARD "MAGIC_CueCardClass"
#define CLASS_MAGIC_PLAYER_DIRECTIVE "MAGIC_PlayerDirectiveClass"
#define CLASS_MAGIC_TELL_USER "MAGIC_TellUserClass"
#define CLASS_SHUFFLE_CARD "ShuffleCard"
#define CLASS_EXPANDED_GRAVEYARD "ExpandedGraveyard"
#define CLASS_GRAVEYARD_CARDS "GraveyardCards"
#define CLASS_ATTACK_SWORD_SHIELD "AttackSwordShield"
#define CLASS_ATTACK_MINIMIZED "AttackMinimized"
#define CLASS_SPELL_MINIMIZED "SpellMinimized"

#define SET_DUEL_WNDCLASS(wndclass_, style_, wndproc_, extra_, icon_, background_, class_name_) \
  (wndclass_).style = (style_);                                                                 \
  (wndclass_).lpfnWndProc = (wndproc_);                                                         \
  (wndclass_).cbClsExtra = 0;                                                                   \
  (wndclass_).cbWndExtra = (extra_);                                                            \
  (wndclass_).hInstance = g_app_instance;                                                       \
  (wndclass_).hIcon = (icon_);                                                                  \
  (wndclass_).hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);                              \
  (wndclass_).hbrBackground = (background_);                                                    \
  (wndclass_).lpszMenuName = (LPCSTR)0;                                                         \
  (wndclass_).lpszClassName = (class_name_)

LRESULT CALLBACK wndproc_MAGICGAME_MainClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_LifeClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_FullCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_ManaSummaryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_HandClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_ChatClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_CardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_PhaseDisplayClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_AttackPhaseDisplayClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_TerritoryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_LibraryClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_ShuffleCard(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_GraveyardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_ExpandedGraveyard(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_GraveyardCards(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_AttackClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_AttackSwordShield(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_AttackMinimized(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_SpellChainClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_SpellMinimized(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_FaceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_ScrollbarClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICTHEME_IconButtonClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_BigCardChoiceClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGICGAME_BigCardCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGIC_PaletteClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGIC_CueCardClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGIC_PlayerDirectiveClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK wndproc_MAGIC_TellUserClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int register_MAGICGAME_AttackPhaseDisplayClass(LPCSTR class_name);
void destroy_MAGICGAME_AttackPhaseDisplayClass(LPCSTR class_name);

/* Single header library style */
#ifdef DUEL_ENGINE_IMPL
#define DUEL_ENGINE_EXTERN
#else
#define DUEL_ENGINE_EXTERN extern
#endif

// GLOBAL: MAGIC 0x008cf1b4
// GLOBAL: SHANDALAR 0x008e3304
DUEL_ENGINE_EXTERN HWND g_duel_window_hwnd;

// GLOBAL: MAGIC 0x0074875c
// GLOBAL: SHANDALAR 0x00748420
DUEL_ENGINE_EXTERN HWND g_main_window_hwnd;

// GLOBAL: MAGIC 0x008cf078
// GLOBAL: SHANDALAR 0x008e31c8
DUEL_ENGINE_EXTERN HWND g_duel_help_owner_hwnd;

// GLOBAL: MAGIC 0x00938e30
// GLOBAL: SHANDALAR 0x0094cf60
DUEL_ENGINE_EXTERN HACCEL g_duel_accelerators;

// GLOBAL: MAGIC 0x00789110
// GLOBAL: SHANDALAR 0x0079fea0
DUEL_ENGINE_EXTERN CRITICAL_SECTION g_duel_render_lock;

// GLOBAL: MAGIC 0x00791414
// GLOBAL: SHANDALAR 0x007a81a4
DUEL_ENGINE_EXTERN int g_duel_cheats_state;

// GLOBAL: MAGIC 0x007a7d20
// GLOBAL: SHANDALAR 0x007beab0
DUEL_ENGINE_EXTERN int g_duel_surface_reset_state;

// GLOBAL: MAGIC 0x00777c0c
// GLOBAL: SHANDALAR 0x0078e99c
DUEL_ENGINE_EXTERN int g_duel_dialog_refresh_state;

// GLOBAL: MAGIC 0x0078970c
// GLOBAL: SHANDALAR 0x007a049c
DUEL_ENGINE_EXTERN int g_duel_palette_refresh_pending;

// GLOBAL: MAGIC 0x007abc84
// GLOBAL: SHANDALAR 0x007bfe84
DUEL_ENGINE_EXTERN int g_duel_tick_adjustment;

// GLOBAL: MAGIC 0x009252e0
// GLOBAL: SHANDALAR 0x00939410
DUEL_ENGINE_EXTERN int g_duel_network_state;

// GLOBAL: MAGIC 0x0092664c
// GLOBAL: SHANDALAR 0x0093a77c
DUEL_ENGINE_EXTERN int g_duel_special_land_card_ids[6];

// GLOBAL: MAGIC 0x008a9000
// GLOBAL: SHANDALAR 0x008bd200
DUEL_ENGINE_EXTERN int g_duel_ai_mode_state;

// GLOBAL: MAGIC 0x0091c504
// GLOBAL: SHANDALAR 0x00930644
DUEL_ENGINE_EXTERN int g_duel_mode_flags;

// GLOBAL: MAGIC 0x008d033c
// GLOBAL: SHANDALAR 0x008e448c
DUEL_ENGINE_EXTERN int g_duel_run_mode;

// GLOBAL: MAGIC 0x008b3d0c
// GLOBAL: SHANDALAR 0x008c7e8c
DUEL_ENGINE_EXTERN int g_duel_creature_type;

// GLOBAL: MAGIC 0x007775b4
// GLOBAL: SHANDALAR 0x0078df80
DUEL_ENGINE_EXTERN int g_duel_message_loop_active;

// GLOBAL: MAGIC 0x0091c4f8
// GLOBAL: SHANDALAR 0x00930638
DUEL_ENGINE_EXTERN HWND g_duel_life_window_hwnd;

// GLOBAL: MAGIC 0x00895204
// GLOBAL: SHANDALAR 0x008a9404
DUEL_ENGINE_EXTERN HWND g_duel_full_card_window_hwnd;

// GLOBAL: MAGIC 0x0091c0f0
// GLOBAL: SHANDALAR 0x00930230
DUEL_ENGINE_EXTERN HWND g_duel_phase_display_window_hwnd;

// GLOBAL: MAGIC 0x008cff0c
// GLOBAL: SHANDALAR 0x008e405c
DUEL_ENGINE_EXTERN HWND g_duel_attack_phase_window_hwnd;

// GLOBAL: MAGIC 0x00896714
// GLOBAL: SHANDALAR 0x008aa914
DUEL_ENGINE_EXTERN HWND g_duel_card_preview_window_hwnd;

// GLOBAL: MAGIC 0x008b3274
// GLOBAL: SHANDALAR 0x008c7424
DUEL_ENGINE_EXTERN HWND g_duel_player_battlefield_window_hwnd;

// GLOBAL: MAGIC 0x008cc2d8
// GLOBAL: SHANDALAR 0x008e0458
DUEL_ENGINE_EXTERN HWND g_duel_cue_card_window_hwnd;

// GLOBAL: MAGIC 0x008cc6fc
// GLOBAL: SHANDALAR 0x008e087c
DUEL_ENGINE_EXTERN HWND g_duel_player_chat_window_hwnd;

// GLOBAL: MAGIC 0x008cd920
// GLOBAL: SHANDALAR 0x008e1aa0
DUEL_ENGINE_EXTERN HWND g_duel_life_status_window_1_hwnd;

// GLOBAL: MAGIC 0x00938e28
// GLOBAL: SHANDALAR 0x0094cf58
DUEL_ENGINE_EXTERN HWND g_duel_life_status_window_2_hwnd;

// GLOBAL: MAGIC 0x007912dc
// GLOBAL: SHANDALAR 0x007a806c
DUEL_ENGINE_EXTERN int g_tell_user_button_state;

// GLOBAL: MAGIC 0x007aaea4
// GLOBAL: SHANDALAR 0x007bf0a4
DUEL_ENGINE_EXTERN int g_tell_user_default_button_state;

#ifdef DUEL_ENGINE_IMPL
// GLOBAL: MAGIC 0x0055e000
// GLOBAL: SHANDALAR 0x0057f08c
int g_magicgame_chat_font_window_long_offset = 0;

// GLOBAL: MAGIC 0x0055e004
// GLOBAL: SHANDALAR 0x0057f090
int g_magicgame_chat_width_window_long_offset = 4;

// GLOBAL: MAGIC 0x0055e008
// GLOBAL: SHANDALAR 0x0057f094
int g_magicgame_chat_window_extra_bytes = 8;
#else
extern int g_magicgame_chat_font_window_long_offset;
extern int g_magicgame_chat_width_window_long_offset;
extern int g_magicgame_chat_window_extra_bytes;
#endif

// GLOBAL: MAGIC 0x0057f780
// GLOBAL: SHANDALAR 0x005aa624
DUEL_ENGINE_EXTERN COLORREF g_magicgame_chat_text_color;

// GLOBAL: MAGIC 0x0057f784
// GLOBAL: SHANDALAR 0x005aa628
DUEL_ENGINE_EXTERN HFONT g_magicgame_chat_font;

// GLOBAL: MAGIC 0x00708b9c
// GLOBAL: SHANDALAR 0x00732214
DUEL_ENGINE_EXTERN HMENU g_magicgame_full_card_menu;

// GLOBAL: MAGIC 0x006f6ef0
// GLOBAL: SHANDALAR 0x005b7750
DUEL_ENGINE_EXTERN HANDLE g_magicgame_big_card_choice_pic;

// GLOBAL: MAGIC 0x007087a4
// GLOBAL: SHANDALAR 0x006695dc
DUEL_ENGINE_EXTERN HANDLE g_magicgame_attack_phase_display_pic;

// GLOBAL: MAGIC 0x00716240
// GLOBAL: SHANDALAR 0x0074d268
DUEL_ENGINE_EXTERN int g_duel_exit_code;

DWORD WINAPI RunDuelEngineThreadProc(LPVOID creature_type);
int create_duel_child_windows(HWND parent_window);
int run_duel_coin_flip_dialogs(unsigned int *starting_player,
                               unsigned int *mulligan_accepted,
                               unsigned int starting_player_value,
                               int random_starting_player,
                               int opponent_ante_card,
                               int player_ante_card,
                               int redraw_result,
                               int ante_result,
                               int ante_info,
                               int *out_flag);

#undef DUEL_ENGINE_EXTERN
#endif
