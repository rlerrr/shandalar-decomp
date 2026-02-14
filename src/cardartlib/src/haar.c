#include <stdlib.h>
#include <STDDEF.H>
#include <string.h>
#include <windows.h>
#include "haar.h"
#include "assert.h"
#include "huffman.h"

// Forward declarations for globals referenced elsewhere.
int Palette_FindNearestEntryIndex(int param_1, int param_2, int param_3, byte *param_4);
uint Rgb888_QuantizeToF8(uint param_1);
void RotateDwordsLeft1(undefined4 *param_1, uint param_2);
undefined4 QuantizeBgr24ToNearestPaletteColorInPlace(uint *bgr24, int height, int width, int row_padding);
int DitherBgr24ToPaletteColors(int dither_kernel_id, int serpentine, uint *bgr24, int height, int width,
                                int row_padding);
int InitErrorDiffusionDeltaTables(int dither_kernel_id, int* delta_table_ptrs_base);
void MemZeroDwords(undefined8 *param_1, uint param_2);

void CopyBgr24RectIntoStridedBuffer(undefined8 *dst_bgr24, undefined8 *src_bgr24, int dst_x, int dst_y,
                                    int rect_width, int rect_height, int dst_stride_pixels);
void CopyBytes(void *dst, const void *src, size_t size);
void SetBytes(void *dst, int value, size_t num);
void Haar2D_ReconstructInPlace(int *coeffs, int full_size, int base_size);
void Haar_CombineSumDiff(int *src_a, int *src_b, int *dst, int width, int rows, undefined4 src_stride_unused,
                         int dst_stride);
void Haar_CombineSumDiffHalf(int *src_a, int *src_b, int *dst, int width, int rows,
                             undefined4 src_stride_unused, int dst_stride);
undefined1 *YuvPlanesToBgr24(undefined1 *out_bgr24, int *luma, int width, int height, int chroma_u, int chroma_v,
                             int chroma_stride, undefined4 unused_chroma_height, int chroma_is_420);
undefined4 Wvl_UnpackPieces(int param_1, int *param_2);

// GLOBAL: CARDARTLIB 0x1001d258
undefined4 global_dither_kernel_id = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d25c
undefined4 global_serpentine = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d260
int DAT_1001d260[0xa] = {0,0,2,4,12,7,10,12,12,0};

// GLOBAL: CARDARTLIB 0x1001d288
int DAT_1001d288[8] = {0,0,2,2,3,2,3,3};

// GLOBAL: CARDARTLIB 0x1001d2b0
int DAT_1001d2b0[0xa] = {0,0,16,16,42,32,32,48};

// GLOBAL: CARDARTLIB 0x1001d2d8
undefined1 DAT_1001d2d8[0xd80];

// GLOBAL: CARDARTLIB 0x10031ca0
undefined1 DAT_10031ca0[0x400];

// GLOBAL: CARDARTLIB 0x1001e058
undefined1 * PTR_DAT_1001e058 = DAT_10031ca0 + 0x200;

// GLOBAL: CARDARTLIB 0x1001e05c
undefined4 DAT_1001e05c = 0xFFFFFFFF;

// GLOBAL: CARDARTLIB 0x1001e060
undefined4 DAT_1001e060 = 0xFFFFFFFF;

// GLOBAL: CARDARTLIB 0x1001e0e0
int DAT_1001e0e0 = 0x00000004;

// GLOBAL: CARDARTLIB 0x100326d8
unsigned char DAT_100326d8[0x1000];

// GLOBAL: CARDARTLIB 0x1001e090
char s_Not_enough_memory_for_delta_arra_1001e090[] = "Not enough memory for delta array\r\n";

// GLOBAL: CARDARTLIB 0x1001e0b4
char s_D__Newmagic_sources_NedCard_Pale_1001e0b4[] = "D:\\Newmagic\\sources\\NedCard\\Palette.c";

// GLOBAL: CARDARTLIB 0x1001e118
unsigned char * g_waveletScaleToByteTable = DAT_100326d8 + 0x400;

// GLOBAL: CARDARTLIB 0x1001e124
undefined4 g_haarScratchInit = 0x00000000;

// GLOBAL: CARDARTLIB 0x100eaf00
undefined1 DAT_100eaf00[0x2000];

// GLOBAL: CARDARTLIB 0x1001e128
undefined1 * g_yuvClampTable = DAT_100eaf00 + 0x400;

// GLOBAL: CARDARTLIB 0x10032ae0
undefined4 g_waveletScaleToByteTableInit = 0x00000000;

// GLOBAL: CARDARTLIB 0x100ea09c
undefined4 g_yuvClampTableInit = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001e14c
char s_wavelet_pieces_has_illegal_value_1001e14c[] = "wavelet pieces has illegal value: %d";

// GLOBAL: CARDARTLIB 0x1001e174
char s_D__Newmagic_sources_NedCard_haar_1001e174[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: CARDARTLIB 0x1001e198
char s_Only_Works_on_24_bit_images_1001e198[] = "Only Works on 24 bit images\n";

// GLOBAL: CARDARTLIB 0x1001e1b8
char s_D__Newmagic_sources_NedCard_haar_1001e1b8[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: CARDARTLIB 0x1001e1dc
char s_Only_Works_on_24_bit_images_1001e1dc[] = "Only Works on 24 bit images\n";

// GLOBAL: CARDARTLIB 0x1001e1fc
char s_D__Newmagic_sources_NedCard_haar_1001e1fc[] = "D:\\Newmagic\\sources\\NedCard\\haar.c";

// GLOBAL: CARDARTLIB 0x10032c98
undefined1 DAT_10032c98[0x2000];

// GLOBAL: CARDARTLIB 0x100ad498
unsigned char DAT_100ad498[0x200];

// GLOBAL: CARDARTLIB 0x100edb10
undefined1 DAT_100edb10[5 * 0x8060];

// GLOBAL: CARDARTLIB 0x100ecb04
undefined4 _DAT_100ecb04 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100322cc
undefined4 DAT_100322cc = 0x00000000;

// GLOBAL: CARDARTLIB 0x100322d0
undefined4 DAT_100322d0 = 0x00000000;

// GLOBAL: CARDARTLIB 0x100322d8
undefined1 DAT_100322d8 = 0x00;

// GLOBAL: CARDARTLIB 0x100322d9
undefined1 DAT_100322d9 = 0x00;

// GLOBAL: CARDARTLIB 0x100ea31c
int *g_haarScratchB = (int *)0x0;

// GLOBAL: CARDARTLIB 0x100ea320
int *g_haarScratchA = (int *)0x0;

// GLOBAL: CARDARTLIB 0x10117100
void *DAT_10117100[0x41];

// GLOBAL: CARDARTLIB 0x101221e8
undefined4 global_color_depth = 0x00000000;

// FUNCTION: CARDARTLIB 0x1000570f
// FUNCTION: DRAWCARDLIB 0x10001cbf
int DitherBgr24ToPaletteColors(int dither_kernel_id,int serpentine,uint *bgr24,int height,int width,int row_padding)
{
  struct {
    short *err_cell;        /* [ebp-0x94] (also reused as temp) */
    int *delta_table;       /* [ebp-0x90] */
    short *err_px;          /* [ebp-0x8c] */
    int err_r;              /* [ebp-0x88] */
    int x_byte_step;        /* [ebp-0x84] */
    int nearest_rgb;        /* [ebp-0x80] */
    int x_start;            /* [ebp-0x7c] */
    undefined *kernel_ptr;  /* [ebp-0x78] */
    int kernel_count;       /* [ebp-0x74] */
    int r_clamped;          /* [ebp-0x70] */
    int err_row_ptrs[6];    /* [ebp-0x6c] */
    int y;                  /* [ebp-0x54] */
    undefined2 pad_50;      /* [ebp-0x50] */
    undefined2 pad_4e;      /* [ebp-0x4e] */
    undefined2 pad_4c_w;    /* [ebp-0x4c] */
    undefined2 pad_4a;      /* [ebp-0x4a] */
    int x;                  /* [ebp-0x48] */
    int k_row;              /* [ebp-0x44] */
    int k_x_off;            /* [ebp-0x40] */
    int err_b;              /* [ebp-0x3c] */
    int x_step;             /* [ebp-0x38] */
    uint g_clamped;         /* [ebp-0x34] */
    int err_g;              /* [ebp-0x30] */
    int *k_it;              /* [ebp-0x2c] */
    int rgb_clamped;        /* [ebp-0x28] */
    uint src_rgb;           /* [ebp-0x24] */
    undefined2 pad_20;      /* [ebp-0x20] */
    undefined2 pad_1e;      /* [ebp-0x1e] */
    undefined2 pad_1c;      /* [ebp-0x1c] */
    undefined2 pad_1a;      /* [ebp-0x1a] */
    int x_end;              /* [ebp-0x18] */
    uint clear_dwords;      /* [ebp-0x14] */
    undefined2 pad_10;      /* [ebp-0x10] */
    undefined2 pad_e;       /* [ebp-0xe] */
    undefined2 pad_c_w;     /* [ebp-0xc] */
    undefined2 pad_a;       /* [ebp-0xa] */
    int x_byte_off;         /* [ebp-0x8] */
    uint b_clamped;         /* [ebp-0x4] */
  } s;

  s.pad_10 = 0;
  s.pad_e = 0;
  s.pad_c_w = 0;
  s.pad_a = 0;
  s.pad_50 = 0;
  s.pad_4e = 0;
  s.pad_4c_w = 0;
  s.pad_4a = 0;
  s.pad_20 = 0xffff;
  s.pad_1e = 0xffff;
  s.pad_1c = 0xffff;
  s.pad_1a = 0;
  s.nearest_rgb = -1;
  s.x_step = 1;
  s.rgb_clamped = 0;
  s.kernel_ptr = &DAT_1001d2d8[(dither_kernel_id << 6) * 3];
  s.clear_dwords = (uint)((width + 10) * 8) >> 2;
  
  if (dither_kernel_id == 0) {
    DAT_1001e05c = dither_kernel_id;
    return 0;
  }
  else if (dither_kernel_id == 1) {
    DAT_1001e05c = dither_kernel_id;
    return QuantizeBgr24ToNearestPaletteColorInPlace(bgr24,height,width,row_padding);
  }

  if (DAT_100322d0 == 0) {
    for (s.x = -0x200; s.x < 0x200; s.x = s.x + 1) {
      if ((0 <= s.x) && (s.x <= 0xff)) {
        PTR_DAT_1001e058[s.x] = (undefined1)s.x;
      }
      else {
        if (s.x < 0) {
          PTR_DAT_1001e058[s.x] = 0;
        }
        else {
          PTR_DAT_1001e058[s.x] = 0xff;
        }
      }
    }
    DAT_100322d0 = 1;
  }

  if (DAT_1001e05c != dither_kernel_id) {
    for (s.x = 0; (uint)s.x < 0x41; s.x = s.x + 1) {
      if (DAT_10117100[s.x] != (void *)0x0) {
        free(DAT_10117100[s.x]);
        DAT_10117100[s.x] = (void *)0x0;
      }
    }
    InitErrorDiffusionDeltaTables(dither_kernel_id,DAT_10117100);
    DAT_1001e05c = dither_kernel_id;
  }

  for (s.x = 0; (uint)s.x < 5; s.x = s.x + 1) {
    SetBytes((void *)(DAT_100edb10 + s.x * 0x8060),0,0x8060);
    s.err_row_ptrs[s.x + 1] = (int)(DAT_100edb10 + s.x * 0x8060) + 0x28;
  }

  s.kernel_count = DAT_1001d260[dither_kernel_id];
  for (s.y = 0; s.y < height; s.y = s.y + 1, *(int *)&bgr24 += width * 3 + row_padding) {
    if (0 < s.x_step) {
      s.x_start = 0;
      s.x_end = width;
      s.x_byte_step = 3;
    }
    else {
      s.x_start = width + -1;
      s.x_end = -1;
      s.x_byte_step = -3;
    }

    s.x = s.x_start;
    s.x_byte_off = s.x_start + s.x_start + s.x_start;
    for (; s.x != s.x_end; s.x = s.x + s.x_step, s.x_byte_off = s.x_byte_off + s.x_byte_step) {
      s.src_rgb = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) & 0xffffff;
      *(uint *)&((byte*)bgr24)[s.x_byte_off] = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) & 0xff000000;

      s.err_px = (short *)s.err_row_ptrs[1] + (s.x << 2);

      if (s.src_rgb == 0) {
        s.nearest_rgb = 0;
        s.r_clamped = s.nearest_rgb;
        s.g_clamped = s.r_clamped;
        s.b_clamped = s.g_clamped;
      }
      else if (s.src_rgb == 0xffffff) {
        s.r_clamped = 0xff;
        s.g_clamped = s.r_clamped;
        s.b_clamped = s.g_clamped;
        s.nearest_rgb = 0xffffff;
      }
      else {

        s.b_clamped = s.src_rgb & 0xff;
        s.b_clamped += ((int)*s.err_px >> 8);
        s.b_clamped = (uint)(byte)PTR_DAT_1001e058[s.b_clamped];

        s.g_clamped = ((byte*)&s.src_rgb)[1];
        s.g_clamped += ((int)s.err_px[1] >> 8);
        s.g_clamped = (uint)(byte)PTR_DAT_1001e058[s.g_clamped];

        s.r_clamped = (s.src_rgb & 0xff0000) >> 16;
        s.r_clamped += ((int)s.err_px[2] >> 8);
        s.r_clamped = (uint)(byte)PTR_DAT_1001e058[s.r_clamped];

        s.src_rgb = s.g_clamped << 8 | s.r_clamped << 0x10 | s.b_clamped;
 
        s.nearest_rgb = Octree_FindNearestColor(s.src_rgb);
      }
      *(uint *)&((byte*)bgr24)[s.x_byte_off] = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) | s.nearest_rgb;

      s.err_b = s.b_clamped - (s.nearest_rgb & 0xff);
      s.err_g = s.g_clamped - (s.nearest_rgb >> 8 & 0xff);
      s.err_r = s.r_clamped - ((uint)s.nearest_rgb >> 16);
      //s.err_row_ptrs[0] = 0;
      s.k_it = s.kernel_ptr;
      s.err_row_ptrs[0] = (s.kernel_count << 2) + s.k_it;
      for (; s.err_row_ptrs[0] > s.k_it; s.k_it += 0x4) {
        s.k_x_off = s.k_it[1];
        s.k_row = s.k_it[2];
        s.delta_table = s.k_it[3];
        s.err_cell = ((s.k_x_off + s.x) << 2) + (short *)(s.err_row_ptrs[s.k_row + 1]);

        s.err_cell[0] = s.delta_table[s.err_b] + s.err_cell[0];
        s.err_cell[1] = s.delta_table[s.err_g] + s.err_cell[1];
        s.err_cell[2] = s.delta_table[s.err_r] + s.err_cell[2];
      }
    }

    RotateDwordsLeft1(s.err_row_ptrs + 1,DAT_1001d288[dither_kernel_id]);
    memset((void *)(s.err_row_ptrs[DAT_1001d288[dither_kernel_id]] + -0x28),0,s.clear_dwords << 2);
    if (serpentine != 0) {
      s.x_step = -s.x_step;
      s.kernel_ptr = (undefined *)((int)DAT_1001d2d8 + (uint)(s.x_step == -1) * 0x6c0 + dither_kernel_id * 0xc0);
    }
  }
}

// FUNCTION: CARDARTLIB 0x10005cb4
// FUNCTION: DRAWCARDLIB 0x10002264
void RotateDwordsLeft1(undefined4 *param_1, uint param_2)
{
  undefined4 uVar1;  
  uVar1 = *param_1;

  memcpy(param_1,param_1 + 1, (param_2 - 1) * 4);
  param_1[param_2 -1] = uVar1;
}

// FUNCTION: CARDARTLIB 0x10005cf5
// FUNCTION: DRAWCARDLIB 0x100022a5
int InitErrorDiffusionDeltaTables(int dither_kernel_id,int* delta_table_ptrs_base)
{
  struct {
    int local_14;
    int local_10; // EBP - 0xc
    int iVar2; // EBP - 8
    int iVar1; // EBP - 4
  } s;
  
  s.iVar1 = DAT_1001d2b0[dither_kernel_id] >> 1;
  for (s.local_10 = 0; s.local_10 < DAT_1001d260[dither_kernel_id]; s.local_10 = s.local_10 + 1) {
    s.iVar2 = *(int*)&DAT_1001d2d8[(s.local_10 << 4) + ((dither_kernel_id << 6) * 3)];

    if (delta_table_ptrs_base[s.iVar2] != 0) {
      *(int *)&DAT_1001d2d8[0xc + ((dither_kernel_id << 6) * 3) + (s.local_10 << 4)] = delta_table_ptrs_base[s.iVar2] + 0x400;
      continue;
    }
    
    delta_table_ptrs_base[s.iVar2] = malloc(0x800);
    assert((uint)(delta_table_ptrs_base[s.iVar2] != 0),s_D__Newmagic_sources_NedCard_Pale_1001e0b4,
            0x4fd,s_Not_enough_memory_for_delta_arra_1001e090);
    for (s.local_14 = -0x100; s.local_14 < 0x100; s.local_14 = s.local_14 + 1) {
      *(int *)( delta_table_ptrs_base[s.iVar2] + 0x400 + s.local_14 * 4) =
            ((s.iVar2 * s.local_14 + s.iVar1) * 0x100) / DAT_1001d2b0[dither_kernel_id];
    }
    *(int *)&DAT_1001d2d8[0xc + ((dither_kernel_id << 6) * 3) + (s.local_10 << 4)] = delta_table_ptrs_base[s.iVar2] + 0x3fc;
  }
  
  for (s.local_10 = 0; s.local_10 < DAT_1001d260[dither_kernel_id]; s.local_10 = s.local_10 + 1) {
    s.iVar2 = *(int*)&DAT_1001d2d8[0xc + 0x6b4 + ((dither_kernel_id << 6) * 3) + (s.local_10 << 4)];
    
    *(int*)&DAT_1001d2d8[0xc + 0x6b4 + 0xc + ((dither_kernel_id << 6) * 3) + (s.local_10 << 4)] = delta_table_ptrs_base[s.iVar2] + 0x3fc;
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x100062c7
// FUNCTION: DRAWCARDLIB 0x10002877
undefined4 DitherBgr24ToRgbQuantizedF8(int dither_kernel_id,int serpentine,uint *bgr24,int height,int width,int row_padding)
{
  struct {
    short *err_cell;        /* [ebp-0x94] (also reused as temp) */
    int *delta_table;       /* [ebp-0x90] */
    short *err_px;          /* [ebp-0x8c] */
    int err_r;              /* [ebp-0x88] */
    int x_byte_step;        /* [ebp-0x84] */
    int quant_rgb;          /* [ebp-0x80] */
    int x_start;            /* [ebp-0x7c] */
    undefined *kernel_ptr;  /* [ebp-0x78] */
    int kernel_count;       /* [ebp-0x74] */
    int r_clamped;          /* [ebp-0x70] */
    int err_row_ptrs[6];    /* [ebp-0x6c] */
    int y;                  /* [ebp-0x54] */
    undefined2 pad_50;      /* [ebp-0x50] */
    undefined2 pad_4e;      /* [ebp-0x4e] */
    undefined2 pad_4c_w;    /* [ebp-0x4c] */
    undefined2 pad_4a;      /* [ebp-0x4a] */
    int x;                  /* [ebp-0x48] */
    int k_row;              /* [ebp-0x44] */
    int k_x_off;            /* [ebp-0x40] */
    int err_b;              /* [ebp-0x3c] */
    int x_step;             /* [ebp-0x38] */
    uint g_clamped;         /* [ebp-0x34] */
    int err_g;              /* [ebp-0x30] */
    int *k_it;              /* [ebp-0x2c] */
    int rgb_clamped;        /* [ebp-0x28] */
    uint src_rgb;           /* [ebp-0x24] */
    undefined2 pad_20;      /* [ebp-0x20] */
    undefined2 pad_1e;      /* [ebp-0x1e] */
    undefined2 pad_1c;      /* [ebp-0x1c] */
    undefined2 pad_1a;      /* [ebp-0x1a] */
    int x_end;              /* [ebp-0x18] */
    uint clear_dwords;      /* [ebp-0x14] */
    undefined2 pad_10;      /* [ebp-0x10] */
    undefined2 pad_e;       /* [ebp-0xe] */
    undefined2 pad_c_w;     /* [ebp-0xc] */
    undefined2 pad_a;       /* [ebp-0xa] */
    int x_byte_off;         /* [ebp-0x8] */
    uint b_clamped;         /* [ebp-0x4] */
  } s;

  s.pad_10 = 0;
  s.pad_e = 0;
  s.pad_c_w = 0;
  s.pad_a = 0;
  s.pad_50 = 0;
  s.pad_4e = 0;
  s.pad_4c_w = 0;
  s.pad_4a = 0;
  s.pad_20 = 0xffff;
  s.pad_1e = 0xffff;
  s.pad_1c = 0xffff;
  s.pad_1a = 0;
  s.quant_rgb = -1;
  s.x_step = 1;
  s.rgb_clamped = 0;
  s.kernel_ptr = &DAT_1001d2d8[(dither_kernel_id << 6) * 3];
  s.clear_dwords = (uint)((width + 10) * 8) >> 2;

  if (DAT_100322cc == 0) {
    for (s.x = -0x200; s.x < 0x200; s.x = s.x + 1) {
      if ((0 <= s.x) && (s.x <= 0xff)) {
        PTR_DAT_1001e058[s.x] = (undefined1)s.x;
      }
      else {
        if (s.x < 0) {
          PTR_DAT_1001e058[s.x] = 0;
        }
        else {
          PTR_DAT_1001e058[s.x] = 0xff;
        }
      }
    }
    DAT_100322cc = 1;
  }

  if (DAT_1001e060 != dither_kernel_id) {
    for (s.x = 0; (uint)s.x < 0x41; s.x = s.x + 1) {
      if (DAT_10117100[s.x] != (void *)0x0) {
        free(DAT_10117100[s.x]);
        DAT_10117100[s.x] = (void *)0x0;
      }
    }
    InitErrorDiffusionDeltaTables(dither_kernel_id,DAT_10117100);
    DAT_1001e060 = dither_kernel_id;
  }

  for (s.x = 0; (uint)s.x < 5; s.x = s.x + 1) {
    memset((void *)(DAT_100edb10 + s.x * 0x8060),0,0x8060);
    s.err_row_ptrs[s.x + 1] = (int)(DAT_100edb10 + s.x * 0x8060) + 0x28;
  }

  s.kernel_count = DAT_1001d260[dither_kernel_id];
  for (s.y = 0; s.y < height; s.y = s.y + 1, *(int *)&bgr24 += width * 3 + row_padding) {
    if (0 < s.x_step) {
      s.x_start = 0;
      s.x_end = width;
      s.x_byte_step = 3;
    }
    else {
      s.x_start = width + -1;
      s.x_end = -1;
      s.x_byte_step = -3;
    }

    s.x = s.x_start;
    s.x_byte_off = s.x_start + s.x_start + s.x_start;
    for (; s.x != s.x_end; s.x = s.x + s.x_step, s.x_byte_off = s.x_byte_off + s.x_byte_step) {
      s.src_rgb = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) & 0xffffff;
      *(uint *)&((byte*)bgr24)[s.x_byte_off] = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) & 0xff000000;

      s.err_px = (short *)s.err_row_ptrs[1] + (s.x << 2);

      s.b_clamped = s.src_rgb & 0xff;
      s.b_clamped += ((int)*s.err_px >> 8);
      s.b_clamped = (uint)(byte)PTR_DAT_1001e058[s.b_clamped];

      s.g_clamped = ((byte*)&s.src_rgb)[1];
      s.g_clamped += ((int)s.err_px[1] >> 8);
      s.g_clamped = (uint)(byte)PTR_DAT_1001e058[s.g_clamped];

      s.r_clamped = (s.src_rgb & 0xff0000) >> 16;
      s.r_clamped += ((int)s.err_px[2] >> 8);
      s.r_clamped = (uint)(byte)PTR_DAT_1001e058[s.r_clamped];

      s.src_rgb = s.g_clamped << 8 | s.r_clamped << 0x10 | s.b_clamped;
      if (s.src_rgb == 0) {
        s.quant_rgb = 0;
      }
      else if (s.src_rgb == 0xffffff) {
        s.quant_rgb = 0xffffff;
      }
      else {
        s.quant_rgb = Rgb888_QuantizeToF8(s.src_rgb);
      }
      *(uint *)&((byte*)bgr24)[s.x_byte_off] = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) | s.quant_rgb;

      s.err_b = s.b_clamped - (s.quant_rgb & 0xff);
      s.err_g = s.g_clamped - (s.quant_rgb >> 8 & 0xff);
      s.err_r = s.r_clamped - (s.quant_rgb >> 16 & 0xff);
      s.err_row_ptrs[0] = 0;
      s.k_it = s.kernel_ptr;
      for (; s.err_row_ptrs[0] < s.kernel_count; s.err_row_ptrs[0]++, s.k_it += 0x4) {
        s.k_x_off = s.k_it[1];
        s.k_row = s.k_it[2];
        s.delta_table = s.k_it[3];
        s.err_cell = (short *)(s.err_row_ptrs[s.k_row + 1] +
                          (s.k_x_off + s.x) * 8);

        s.err_cell[0] = s.delta_table[s.err_b] + s.err_cell[0];
        s.err_cell[1] = s.delta_table[s.err_g] + s.err_cell[1];
        s.err_cell[2] = s.delta_table[s.err_r] + s.err_cell[2];
      }
    }

    RotateDwordsLeft1(s.err_row_ptrs + 1,DAT_1001d288[dither_kernel_id]);
    memset((void *)(s.err_row_ptrs[DAT_1001d288[dither_kernel_id]] + -0x28),0,s.clear_dwords << 2);
    if (serpentine != 0) {
      s.x_step = -s.x_step;
      s.kernel_ptr = &DAT_1001d2d8[(uint)(s.x_step == -1) * 0x6c0 + dither_kernel_id * 0xc0];
    }
  }
  return 1;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100068d0
// FUNCTION: DRAWCARDLIB 0x10002e80
uint Rgb888_QuantizeToF8(uint param_1)
{
  return param_1 & 0xf8f8f8;
}

// FUNCTION: CARDARTLIB 0x10006be3
// FUNCTION: DRAWCARDLIB 0x10008663
undefined8 * Wvl_DecodeHaar(int *param_1,undefined8 *param_2)
{
  struct {
    int bgr_tmp;         /* [ebp-0x4c] */
    int scale_i;         /* [ebp-0x48] */
    int piece_idx;       /* [ebp-0x44] */
    int local_40;        /* [ebp-0x40] */
    int have_dst;        /* [ebp-0x3c] */
    int chroma_h;        /* [ebp-0x38] */
    int local_34;        /* [ebp-0x34] */
    int v_plane;         /* [ebp-0x30] */
    int height_px;       /* [ebp-0x2c] */
    int chroma_w;        /* [ebp-0x28] */
    int width_px;        /* [ebp-0x24] */
    int base_size;       /* [ebp-0x20] */
    int alloc_bytes;     /* [ebp-0x1c] */
    int dword_count;     /* [ebp-0x18] */
    int block_width;     /* [ebp-0x14] */
    int block_height;    /* [ebp-0x10] */
    int y_plane;        /* [ebp-0xc] */
    int *u_plane;        /* [ebp-0x8] */
    int pieces_per_row;  /* [ebp-0x4] */
  } s;

  s.have_dst = 0;
  if (g_waveletScaleToByteTableInit == 0) {
    for (s.scale_i = -0x400; s.scale_i <= 0x400; s.scale_i = s.scale_i + 1) {
      if (s.scale_i >= 0 && s.scale_i <= 0xf8) {
          g_waveletScaleToByteTable[s.scale_i] = (char)((s.scale_i * 255) / 0xf8);
      }
      else if (s.scale_i < 0xa) {
        g_waveletScaleToByteTable[s.scale_i] = 0;
      }
      else {
        g_waveletScaleToByteTable[s.scale_i] = 0xff;
      }
    }
    g_waveletScaleToByteTableInit = 1;
  }

  if (param_2 == (undefined8 *)0x0) {
    param_2 = malloc(param_1[0x24] + 2000);
    MemZeroDwords(param_2,(param_1[0x24] + 2000) / 4);
  }
  else {
    MemZeroDwords(param_2,(param_1[0x24] + 2000) / 4);
    s.have_dst = 1;
  }

  _DAT_100ecb04 = Wvl_UnpackPieces((int)param_2,param_1);

  if (param_1[10] == 1) {
    s.pieces_per_row = 1;
  }
  else if (param_1[10] == 4) {
    s.pieces_per_row = 2;
  }
  else if (param_1[10] == 0x10) {
    s.pieces_per_row = 4;
  }
  else {
    assert(0,s_D__Newmagic_sources_NedCard_haar_1001e174,0x15e,
           s_wavelet_pieces_has_illegal_value_1001e14c,param_1[10]);
  }

  s.chroma_h = param_1[7];
  s.height_px = s.chroma_h;
  s.block_width = param_1[7] / s.pieces_per_row;
  s.y_plane = s.block_width;
  s.alloc_bytes = param_1[9];
  s.base_size = param_1[8] / s.pieces_per_row;
  s.width_px = &param_1[11];
  s.local_34 = &param_1[15];
  s.u_plane = &param_1[19];

  for (s.piece_idx = 0; s.piece_idx < param_1[10]; s.piece_idx = s.piece_idx + 1) {
    //s.chroma_h = s.block_height;
    //s.chroma_w = s.block_width;
    if (*param_1 != 0) {
      s.chroma_w = (s.y_plane / s.pieces_per_row) / ((param_1[10] == 1) + 1);
      //s.chroma_h = s.y_plane / s.pieces_per_row /  (param_1[10] <= 2 ? 2 : 1);
      s.v_plane = (s.base_size / s.pieces_per_row) / ((param_1[10] == 1) + 1);
    } else {
      s.chroma_w = s.y_plane;
      s.v_plane = s.base_size;
    }

    s.local_40 = (int *)((s.y_plane * s.y_plane + s.chroma_w * s.chroma_w * 2 + 0x40) * s.piece_idx * 4 +
                        (int)param_2);
    //s. = (int)s.y_plane;
    s.block_height = (int *)(s.local_40 + s.y_plane * s.y_plane * 4 + 0x80);
    s.dword_count = (int)s.block_height + s.chroma_w * s.chroma_w * 4 + 0x80;

    Haar2D_ReconstructInPlace(s.local_40,s.y_plane, s.alloc_bytes);
    Haar2D_ReconstructInPlace(s.block_height,s.chroma_w,s.alloc_bytes);
    Haar2D_ReconstructInPlace(s.dword_count,s.chroma_w,s.alloc_bytes);

    if (s.piece_idx < param_1[10] / 2) {
      s.bgr_tmp = (int)YuvPlanesToBgr24(DAT_10032c98,s.local_40,s.y_plane,s.y_plane,
                                        s.block_height,s.dword_count,s.chroma_w,s.chroma_w,*param_1);
    }
    else if (param_1[10] > 1) {
      s.bgr_tmp = (int)YuvPlanesToBgr24(DAT_10032c98,s.local_40,s.y_plane,param_1[8] - s.block_width,
                                        s.block_height,s.dword_count,s.chroma_w,s.v_plane,*param_1);
    }
    else {
      s.bgr_tmp = (int)YuvPlanesToBgr24(DAT_10032c98,s.local_40,s.y_plane,s.base_size,
                                        s.block_height,s.dword_count,s.chroma_w,s.v_plane,*param_1);
    }

    if (param_1[10] > 1) {

      CopyBgr24RectIntoStridedBuffer(param_2,(undefined8 *)s.bgr_tmp,
                                      (s.height_px / s.pieces_per_row) * (s.piece_idx % s.pieces_per_row),
                                      (s.height_px / s.pieces_per_row) * (s.piece_idx / s.pieces_per_row), s.y_plane,
                                      s.y_plane,s.height_px,s.height_px);
      free((undefined8 *)s.bgr_tmp);
    }
    else {
      if (s.have_dst == 0) {
        free(param_2);
      }
      param_2 = (undefined8 *)s.bgr_tmp;
    }
  }
  return param_2;
}

// FUNCTION: CARDARTLIB 0x10007035
// FUNCTION: DRAWCARDLIB 0x10008ab5
void CopyBgr24RectIntoStridedBuffer(undefined8 *dst_bgr24,undefined8 *src_bgr24,int dst_x,int dst_y,int rect_width,
                               int rect_height,int dst_stride_pixels, int unused)
{
  int row;
  
  *(int *)&dst_bgr24 += (dst_stride_pixels * dst_y + dst_x) * 3;

  for (row = 0;row < rect_height;row = row + 1, *(int *)&dst_bgr24 += dst_stride_pixels * 3,*(int *)&src_bgr24 += rect_width * 3 ) {
      CopyBytes(dst_bgr24,src_bgr24,rect_width * 3);
  }
}

// FUNCTION: CARDARTLIB 0x10007238
// FUNCTION: DRAWCARDLIB 0x10008cb8
void CopyBytes(void *dst,const void *src,size_t num)
{
  //TODO: this looks like real inline asm but who knows
  __asm {
    mov edi, dst
    mov esi, src
    mov ecx, num
    push ecx
    shr ecx, 3
    je 0x11
    fld qword ptr [esi]
    fstp qword ptr [edi]
    add esi, 8
    add edi, 8
    dec ecx
    jne -0x11
    pop ecx
    and ecx, 7
    je 0x2
    rep movsb
  }
}

// FUNCTION: CARDARTLIB 0x10007273
// FUNCTION: DRAWCARDLIB 0x10008cf3
void SetBytes(void *dst, int value, size_t num)
{
  //This is weird because it doesn't do anything?
  __int64 uVar1;
  
  uVar1 = (__int64)(value << 8 | value);
  uVar1 |= uVar1 << 16;
  uVar1 |= uVar1 << 32;

  //TODO: this looks like real inline asm but who knows
  __asm {
    fld qword ptr [ebp - 8]
    mov edi, dst
    mov ecx, num
    push ecx
    shr ecx, 3
    dec ecx
    fst qword ptr [edi]
    add edi, 8
    dec ecx
    jne -0xc
    fstp qword ptr [edi]
    pop ecx
    and ecx, 7
    mov eax, value
    rep stosb
  }
}

// FUNCTION: CARDARTLIB 0x100072dc
// FUNCTION: DRAWCARDLIB 0x10008d5c
void Haar2D_ReconstructInPlace(int *coeffs,int full_size,int base_size)
{
  struct {
    int *idk;
    int *piVar1; // ebp - 0x18
    int *haarScratchA; // ebp - 0x14
    int *haarScratchB; // ebp - 0x10
    int i; // ebp - 0xc
    int pad2; // ebp - 8
    int *pad3; // ebp - 4
  } s;
  
  if (g_haarScratchInit == 0) {
    g_haarScratchA = malloc(0x32000);
    s.haarScratchA = g_haarScratchA;
    g_haarScratchB = malloc(0x32000);

    s.haarScratchB = g_haarScratchB;
    g_haarScratchInit = 1;
  }
  else {
    s.haarScratchA = g_haarScratchA;
    s.haarScratchB = g_haarScratchB;
  }


  for (s.i = base_size; s.i < full_size; s.i = s.i << 1) {
    s.pad2 = coeffs;
    s.idk = &coeffs[s.i*s.i];

    Haar_CombineSumDiff(s.pad2,s.idk,s.haarScratchA,s.i,s.i,s.i * 2,s.i);

    s.pad3 = &coeffs[s.i*s.i*2];
    s.piVar1 = &coeffs[s.i*s.i*3];

    Haar_CombineSumDiff(s.pad3,s.piVar1,s.haarScratchB, s.i,
                        s.i,s.i * 2,s.i);
                        
    Haar_CombineSumDiffHalf(s.haarScratchA,s.haarScratchB,coeffs,s.i,s.i * 2,s.i * 2,s.i * 2);
  }
}

// FUNCTION: CARDARTLIB 0x10007419
// FUNCTION: DRAWCARDLIB 0x10008e99
void Haar_CombineSumDiff(int *src_a,int *src_b,int *dst,int width,int rows,undefined4 src_stride_unused,int dst_stride)
{
  struct {
    int *src_a_end; // ebp - 0x10
    int *src_a_row0; // ebp - 0xc
    int *dst_row; // ebp - 8
    int row; // ebp - 4
  } s;
  
  s.row = dst_stride;

  for (s.row = 0; s.row < rows; s.row++, dst++, src_b++) {
    s.dst_row = dst;
    s.src_a_row0 = src_a;
    s.src_a_end = (width - 1) + src_a + 1;
    s.dst_row = s.dst_row + dst_stride * 2;
    src_a++;

    for (; s.src_a_end > src_a; src_a++, src_b++, s.dst_row += dst_stride * 2) {
      *s.dst_row = *src_b + *src_a;

      *(int *)((char *)s.dst_row + dst_stride * 4) = *src_a - *src_b;
    }
    
    *dst = *src_b + *s.src_a_row0;
    dst[dst_stride] = *s.src_a_row0 - *src_b;
  }
}

// FUNCTION: CARDARTLIB 0x100074ee
// FUNCTION: DRAWCARDLIB 0x10008f6e
void Haar_CombineSumDiffHalf(int *src_a,int *src_b,int *dst,int width,int rows,undefined4 src_stride_unused,int dst_stride)
{
  struct {
    int *src_a_end; // ebp - 0x10
    int *src_a_row0; // ebp - 0xc
    int *dst_row; // ebp - 8
    int row; // ebp - 4
  } s;
  
  for (s.row = 0; s.row < rows; s.row++, dst++, src_b++) {
    s.dst_row = dst;
    s.src_a_row0 = src_a;
    s.src_a_end = (width - 1) + src_a + 1;
    s.dst_row = s.dst_row + dst_stride * 2;
    src_a++;

    for (; s.src_a_end > src_a; src_a++, src_b++, s.dst_row += dst_stride * 2) {
      *s.dst_row = (*src_b + *src_a) >> 1;
      *(int *)((char *)s.dst_row + dst_stride * 4) = (*src_a - *src_b) >> 1;
    }

    *dst = (*src_b + *s.src_a_row0) >> 1;
    dst[dst_stride] = (*s.src_a_row0 - *src_b) >> 1;
  }
}

// FUNCTION: CARDARTLIB 0x100075ef
// FUNCTION: DRAWCARDLIB 0x1000906f
undefined1 * YuvPlanesToBgr24(undefined1 *out_bgr24,int *luma,int width,int height,int chroma_u,int chroma_v,
                 int chroma_stride,undefined4 unused_chroma_height,int chroma_is_420)

{
  struct {
    int y; /* ebp - 0x2c */
    int u; /* ebp - 0x28 */
    int v; /* ebp - 0x24 */
    undefined1 *out_base; /* ebp - 0x20 */
    int row; /* ebp - 0x1c */
    int col; /* ebp - 0x18 */
    int green; /* ebp - 0x14 */
    int *chroma_u_ptr; /* ebp - 0x10 */
    int *chroma_v_ptr; /* ebp - 0xc */
    int red; /* ebp - 8 */
    int blue; /* ebp - 4 */
  } s;
  
  if (g_yuvClampTableInit == 0) {
    for (s.col = -0x400; s.col < 0x1c00; s.col = s.col + 1) {
      if (0 < s.col) {
        g_yuvClampTable[s.col] = (char)(((s.col >> 2) <= 0xff) ? (s.col >> 2) : 0xff);
      } else {
        g_yuvClampTable[s.col] = 0;
      } 
    }
    g_yuvClampTableInit = 1;
  }
  if (out_bgr24 == (undefined1 *)0x0) {
    out_bgr24 = malloc(width * width * 3 + 0x10);
    s.out_base = out_bgr24;
  } else {
    s.out_base = out_bgr24;
  }
  for (s.row = 0; s.row < height; s.row = s.row + 1) {
    s.y = s.row;
    if (chroma_is_420 != 0) {
      s.y = s.y / 2;
    }

    s.y = (s.y * chroma_stride) << 2;
    s.chroma_u_ptr = (int *)(chroma_u + s.y);
    s.chroma_v_ptr = (int *)(chroma_v + s.y);

    for (s.col = 0; s.col < width; s.col = s.col + 1, luma = luma + 1, out_bgr24 = out_bgr24 + 3) {
      s.y = *luma;

      if (chroma_is_420 != 0) {
        if ((s.col & 1) == 0) {
          s.u = *s.chroma_u_ptr;
          s.v = *s.chroma_v_ptr;
        } else {
          s.u = (s.chroma_u_ptr[((uint)(width - 1) - (uint)s.col) != 0] + *s.chroma_u_ptr) / 2;
          s.v = (s.chroma_v_ptr[((uint)(width - 1) - (uint)s.col) != 0] + *s.chroma_v_ptr) / 2;
        }

        s.red = ((s.v >> 3) + (s.v >> 1) + s.v) - 0x333 + s.y;
        s.blue = (s.u * 2) - 0x400 + s.y;
        s.green = (((s.y * 2) - (s.y >> 2)) - (s.red >> 1)) - ((s.blue >> 2) - (s.blue >> 4));

        out_bgr24[0] = g_yuvClampTable[s.blue];
        out_bgr24[1] = g_yuvClampTable[s.green];
        out_bgr24[2] = g_yuvClampTable[s.red];

        if ((s.col & 1) != 0) {
          s.chroma_u_ptr = s.chroma_u_ptr + 1;
          s.chroma_v_ptr = s.chroma_v_ptr + 1;
        }
      } else {
        s.u = *s.chroma_u_ptr;
        s.v = *s.chroma_v_ptr;

        s.red = ((s.v >> 3) + (s.v >> 1) + s.v) - 0x333 + s.y;
        s.blue = (s.u * 2) - 0x400 + s.y;
        s.green = (((s.y * 2) - (s.y >> 2)) - (s.red >> 1)) - ((s.blue >> 2) - (s.blue >> 4));

        out_bgr24[0] = g_yuvClampTable[s.blue];
        out_bgr24[1] = g_yuvClampTable[s.green];
        out_bgr24[2] = g_yuvClampTable[s.red];

        s.chroma_u_ptr = s.chroma_u_ptr + 1;
        s.chroma_v_ptr = s.chroma_v_ptr + 1;
      }
    }
  }
  return s.out_base;
}

// FUNCTION: CARDARTLIB 0x100078b8
// FUNCTION: DRAWCARDLIB 0x10009338
undefined4 Wvl_UnpackPieces(int param_1,int *param_2)
{
  struct {
    byte *dst_y;        /* ebp - 0x3c */
    int *wvl;           /* ebp - 0x38 */
    int node_count;     /* ebp - 0x34 */
    int chroma_w;       /* ebp - 0x30 */
    int base_size;      /* ebp - 0x2c */
    int chroma_h;       /* ebp - 0x28 */
    int layer;          /* ebp - 0x24 */
    uint *symbol_table; /* ebp - 0x20 */
    int full_w;         /* ebp - 0x1c */
    byte *dst_v;        /* ebp - 0x18 */
    byte *bitstream;    /* ebp - 0x14 */
    byte *huff_data;    /* ebp - 0x10 */
    int full_h;         /* ebp - 0x0c */
    int tmp;            /* ebp - 0x08 */
    byte *dst_u;        /* ebp - 0x04 */
  } s;

  s.wvl = param_2;

  s.full_w = s.wvl[7] / (2 - (s.wvl[10] < 2));
  s.full_h = s.full_w;
  s.chroma_w = s.full_w / (2 - (s.wvl[0] < 1));
  s.chroma_h = s.chroma_w;
  s.base_size = s.wvl[9];

  s.huff_data = (byte *)s.wvl[0x68];
  s.bitstream = s.huff_data;
  s.node_count = *(int *)s.bitstream;
  s.bitstream = s.bitstream + 4;
  s.symbol_table = (uint *)s.bitstream;
  *s.symbol_table = 0x80000000;
  s.bitstream = s.bitstream + (s.node_count << 2);
  s.bitstream =
      s.bitstream +
      Huffman13_Init((undefined4)s.bitstream,(undefined4)s.symbol_table,(undefined4)s.node_count);

  for (s.layer = 0; s.layer < s.wvl[10]; s.layer = s.layer + 1) {
    s.tmp = s.chroma_w * s.chroma_h;
    s.tmp = (s.full_w * s.full_h) + s.tmp * 2;
    s.tmp = s.tmp + 0x40;
    s.dst_y = (byte *)(param_1 + ((s.tmp * s.layer) << 2));

    s.dst_u += (s.full_w * s.full_h) << 2 + 0x80;

    s.dst_v += (s.chroma_w * s.chroma_h) << 2 + 0x80;

    memcpy(s.dst_y,s.bitstream,(s.base_size * s.base_size) << 2);

    s.dst_y += (s.base_size * s.base_size) << 2;

    s.bitstream += (s.base_size * s.base_size) << 2;

    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)s.dst_y,(uint *)s.bitstream,s.wvl[s.layer + 0x17]);
    s.bitstream = s.bitstream + s.wvl[s.layer + 0x17];

    memcpy(s.dst_u,s.bitstream,(s.base_size * s.base_size) << 2);

    s.dst_u += (s.base_size * s.base_size) << 2;

    s.bitstream += (s.base_size * s.base_size) << 2;

    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)s.dst_u,(uint *)s.bitstream,s.wvl[s.layer + 0x1b]);
    s.bitstream = s.bitstream + s.wvl[s.layer + 0x1b];

    memcpy(s.dst_v,s.bitstream,(s.base_size * s.base_size) << 2);

    s.dst_v += (s.base_size * s.base_size) << 2;

    s.bitstream += (s.base_size * s.base_size) << 2;

    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)s.dst_v,(uint *)s.bitstream,s.wvl[s.layer + 0x1f]);
    s.bitstream = s.bitstream + s.wvl[s.layer + 0x1f];
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x1000807f
// FUNCTION: DRAWCARDLIB 0x10009aff
uint * Wvl_DecodeToBgr24(byte *param_1,int *wvl_entry,int width,int height)
{
  struct WvlDecodeToBgr24Stack {
    int unk_5060;
    int row_padding;
    int y_step;
    int x_step;
    uint *out_base;
    uint x_map[4096];
    byte *decoded; // ebp - 0x104c
    int y; // ebp - 0x1048
    int y_acc; // ebp - 0x1044
    int x; // ebp - 0x1040
    int out_nonnull; // ebp - 0x103c
    int src_width; // ebp - 0x1038
    int *y_map_ptr; // ebp - 0x1034
    int x_acc; // ebp - 0x1030
    int *x_map_ptr; // ebp - 0x102c
    byte *out_start; // ebp - 0x1028
    byte *out_ptr; // ebp - 0x1024
    int y_scale; // ebp - 0x1020
    int row_pad; // ebp - 0x101c
    int row_bytes; // ebp - 0x1018
    int x_scale; // ebp - 0x1014
    int zero_1010; // ebp - 0x1010
    byte *src_ptr; // ebp - 0x100c
    uint y_map[1016]; // ebp - 0x1008
    int tmp_28; // ebp - 0x28
    int tmp_24; // ebp - 0x24
    int tmp_20; // ebp - 0x20
    int tmp_1c; // ebp - 0x1c
    int tmp_18; // ebp - 0x18
    int tmp_14; // ebp - 0x14
    int tmp_10; // ebp - 0x10
    int tmp_c; // ebp - 0xc
    int tmp_8; // ebp - 0x8
    int tmp_4; // ebp - 0x4
  } s;

  s.x_acc = 0;
  s.y_acc = 0;
  s.out_base = param_1;
  s.out_start = (byte *)0;
  s.x_map_ptr = s.x_map;
  s.y_map_ptr = s.y_map;
  if (param_1 != (uint *)0) {
    s.out_nonnull = 1;
  } else {
    s.out_nonnull = 0;
  }

  if (wvl_entry == (int *)0) {
    return (uint *)0;
  }

  s.x_scale = wvl_entry[7];
  s.y_scale = wvl_entry[8];
  s.x_scale <<= 0x10;
  s.y_scale <<= 0x10;
  s.x_step = s.x_scale / width;
  s.y_step = s.y_scale / height;

  if (wvl_entry[0x6a] != 0) {
    s.decoded = (byte *)wvl_entry[0x6b];
  } else {
    s.decoded = (byte *)Wvl_DecodeHaar(wvl_entry,(undefined8 *)&DAT_10032c98);
  }

  s.src_width = wvl_entry[7];
  s.tmp_8 = 3;
  s.zero_1010 = 0;
  s.row_pad = (DAT_1001e0e0 - (width * 3) % DAT_1001e0e0) % DAT_1001e0e0;

  if (param_1 == (uint *)0) {
    param_1 = (uint *)&DAT_100ad498;
    s.out_base = param_1;
  } else {
    s.out_base = param_1;
  }
  
  for (s.x = 0, s.x_acc = 0; s.x < width; s.x += 1, s.x_acc += s.x_step, s.x_map_ptr += 1) {
    if (s.tmp_8 == 3) {
      *s.x_map_ptr = (uint)(s.x_acc >> 8);
    } else {
      assert(0,s_D__Newmagic_sources_NedCard_haar_1001e1b8,0x6f7,
             s_Only_Works_on_24_bit_images_1001e198);
    }    
  }

  //param_1 = s.out_base;
  if (wvl_entry[8] < height) {
    param_1 += (height - wvl_entry[8]) * (s.row_pad + width * 3);
    s.out_ptr = param_1;
  }
  else
    s.out_ptr = (byte *)param_1;
  
  for (s.y = 0; s.y < wvl_entry[8]; s.y += 1, param_1 += s.row_pad) {
    s.tmp_4 = (int)s.decoded + (s.src_width * s.tmp_8 + s.zero_1010) * s.y;
    s.x_map_ptr = s.x_map;

    for (s.x = 0; s.x < width; s.x += 1, s.x_map_ptr += 1) {
      s.src_ptr = (byte *)(s.tmp_4 + (((int)*s.x_map_ptr >> 8) * 3));

      param_1[0] = ((int)((uint)s.src_ptr[3] - (uint)s.src_ptr[0]) * (*s.x_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[0];
      param_1[1] =  ((int)((uint)s.src_ptr[4] - (uint)s.src_ptr[1]) * (*s.x_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[1];
      param_1[2] =  ((int)((uint)s.src_ptr[5] - (uint)s.src_ptr[2]) * (*s.x_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[2];

      param_1 = (uint *)((int)param_1 + 3);
    }
  }

  for (s.x = 0, s.y_acc = 0;s.x < height; s.x++, s.y_acc += s.y_step, s.y_map_ptr++) {
    if (s.tmp_8 == 3) {
      *s.y_map_ptr = (uint)(s.y_acc >> 8);    
    } else {
      assert(0,s_D__Newmagic_sources_NedCard_haar_1001e1fc,0x730,
             s_Only_Works_on_24_bit_images_1001e1dc);
    }
  }

  s.row_bytes = width * 3 + s.row_pad;
  if (height < wvl_entry[8]) {
    memcpy(s.x_map,(void *)((wvl_entry[8] - 1) * s.row_bytes + (int)s.out_base),s.row_bytes);
  }

  for (s.x = 0; s.x < width; s.x++) {
    param_1 = (uint *)((int)s.out_base + s.x * 3);
    s.tmp_4 = (int*)(s.out_ptr + s.x*3);
    s.y_map_ptr = s.y_map;
    
    for (s.y = 0; s.y < height - 1; s.y++, s.y_map_ptr++, param_1 += s.row_bytes) {
      s.src_ptr = MIN((int)*s.y_map_ptr >> 8, wvl_entry[8] - 2) * s.row_bytes + s.tmp_4;

      param_1[0] = ((int)((uint)s.src_ptr[s.row_bytes] - (uint)s.src_ptr[0]) * (*s.y_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[0];
      param_1[1] = ((int)((uint)s.src_ptr[s.row_bytes + 1] - (uint)s.src_ptr[1]) * (*s.y_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[1];
      param_1[2] = ((int)((uint)s.src_ptr[s.row_bytes + 2] - (uint)s.src_ptr[2]) * (*s.y_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[2];
    }
  }

  if (height < wvl_entry[8]) {
    memcpy((void *)((height - 1) * s.row_bytes + (int)s.out_base),s.x_map,s.row_bytes);
  } else {
    memset((void *)((height - 1) * s.row_bytes + (int)s.out_base),0,s.row_bytes);
  }

  s.unk_5060 = 4;
  s.row_padding = (s.unk_5060 - ((width * 3) % s.unk_5060)) % s.unk_5060;
  if (global_dither_kernel_id == 0) {
    QuantizeBgr24ToNearestPaletteColorInPlace(s.out_base,height,width,s.row_padding);
  } else if (global_color_depth == 16) {
    DitherBgr24ToRgbQuantizedF8(global_dither_kernel_id,global_serpentine,s.out_base,height,width,s.row_padding);
  } else if (global_color_depth == 8) {
    DitherBgr24ToPaletteColors(global_dither_kernel_id,global_serpentine,s.out_base,height,width,s.row_padding);
  }

  return s.out_base;
}

