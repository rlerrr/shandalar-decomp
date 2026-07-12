#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <windows.h>
#include "defs.h"

#ifdef NETWORK_IMPL
#define NETWORK_EXTERN
#else
#define NETWORK_EXTERN extern
#endif

// GLOBAL: MAGIC 0x00789a44
NETWORK_EXTERN HANDLE global_mutex_ReadPacket;

// GLOBAL: MAGIC 0x007a7d6c
NETWORK_EXTERN int unk_007a7d6c;

// GLOBAL: MAGIC 0x007a7d08
// GLOBAL: SHANDALAR 0x007bea98
NETWORK_EXTERN char g_network_result_packet_type;

// GLOBAL: MAGIC 0x007a7d0c
// GLOBAL: SHANDALAR 0x007bea9c
NETWORK_EXTERN int g_network_result_value;

// GLOBAL: MAGIC 0x0092607c
NETWORK_EXTERN int unk_0092607c;

// GLOBAL: MAGIC 0x00926080
NETWORK_EXTERN char unk_00926080;

// GLOBAL: MAGIC 0x008b27f0
NETWORK_EXTERN char unk_008b27f0;

// GLOBAL: MAGIC 0x008b27f4
NETWORK_EXTERN unsigned char DAT_008b27f4[0x4c];

// GLOBAL: MAGIC 0x008b2938
NETWORK_EXTERN char unk_008b2938;
// GLOBAL: MAGIC 0x008b293c
NETWORK_EXTERN int DAT_008b293c;

// GLOBAL: MAGIC 0x008b34a0
NETWORK_EXTERN char unk_008b34a0;

// GLOBAL: MAGIC 0x008b60e8
NETWORK_EXTERN int unk_008b60e8;

// GLOBAL: MAGIC 0x0091ca90
NETWORK_EXTERN char unk_0091ca90;

// GLOBAL: MAGIC 0x0091ca92
NETWORK_EXTERN short unk_0091ca92;

// GLOBAL: MAGIC 0x0091ca94
NETWORK_EXTERN short unk_0091ca94;

// GLOBAL: MAGIC 0x0091ca96
NETWORK_EXTERN char unk_0091ca96;

// GLOBAL: MAGIC 0x0091ca98
NETWORK_EXTERN short *unk_0091ca98;

// GLOBAL: MAGIC 0x0091d07c
NETWORK_EXTERN int unk_0091d07c;

// GLOBAL: MAGIC 0x009251b0
NETWORK_EXTERN char unk_009251b0;

// GLOBAL: MAGIC 0x008cf200
NETWORK_EXTERN char unk_008cf200;

// GLOBAL: MAGIC 0x008cf3a0
NETWORK_EXTERN char unk_008cf3a0;

int FUN_0049e8bb(int player,
                 int *graveyard,
                 int unused,
                 void *available,
                 int count,
                 int prompt_lines,
                 int num_prompt_lines,
                 int selected_indices,
                 int highlighted_choices,
                 int max_choices);

void send_battlefield_status_packet(int player);
void receive_battlefield_status_packet(int player);
int ReportUnexpectedNetworkPacketType(int expected_packet_type, int actual_packet_type);
int TENTATIVE_wait_for_network_result(int player, signed int packet_type);
int TENTATIVE_send_network_result(int player, signed int packet_type);

#undef NETWORK_EXTERN
#endif
