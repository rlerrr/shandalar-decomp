#include <stdlib.h>
#include <STDDEF.H>
#include <string.h>
#include <windows.h>
#include "haar.h"
#include "catalog.h"
#include "assert.h"
#include "huffman.h"

// Forward declarations for globals referenced elsewhere.
int Palette_FindNearestEntryIndex(int red, int green, int blue, byte *palette);
uint Rgb888_QuantizeToF8(uint rgb);
void RotateDwordsLeft1(undefined4 *dwords, uint count);
undefined4 QuantizeBgr24ToNearestPaletteColorInPlace(uint *bgr24, int height, int width, int row_padding);
int DitherBgr24ToPaletteColors(int dither_kernel_id, int serpentine, uint *bgr24, int height, int width,
                               int row_padding);
int InitErrorDiffusionDeltaTables(int dither_kernel_id, int *delta_table_ptrs_base);
int Octree_FindNearestColor(uint rgb);
void MemZeroDwords(undefined8 *dst, uint dword_count);

void CopyBgr24RectIntoStridedBuffer(byte *dst_bgr24, byte *src_bgr24, int dst_x, int dst_y,
                                    int rect_width, int rect_height, int dst_stride_pixels, int unused);
void CopyBytes(void *dst, const void *src, size_t size);
void SetBytes(void *dst, int value, size_t num);
void Haar2D_ReconstructInPlace(int *coeffs, int full_size, int base_size);
void Haar_CombineSumDiff(int *src_a, int *src_b, int *dst, int width, int rows, undefined4 src_stride_unused,
                         int dst_stride);
void Haar_CombineSumDiffHalf(int *src_a, int *src_b, int *dst, int width, int rows,
                             undefined4 src_stride_unused, int dst_stride);
undefined1 *YuvPlanesToBgr24(undefined1 *out_bgr24, int *luma, int width, int height, int *chroma_u, int *chroma_v,
                             int chroma_stride, undefined4 unused_chroma_height, int chroma_is_420);
BOOL Wvl_UnpackPieces(byte *dst, WvlEntry *wvl);

// GLOBAL: CARDARTLIB 0x100ea098
// GLOBAL: DRAWCARDLIB 0x100f1f68
undefined4 g_activeWvlCatalogHandle;

// GLOBAL: CARDARTLIB 0x1001d258
// GLOBAL: DRAWCARDLIB 0x10021048
undefined4 global_dither_kernel_id = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d25c
// GLOBAL: DRAWCARDLIB 0x1002104c
// GLOBAL: SHANDALAR 0x00581ebc
undefined4 global_serpentine = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d260
// GLOBAL: DRAWCARDLIB 0x10021050
// GLOBAL: SHANDALAR 0x00581ec0
int g_ditherKernelTapCounts[0xa] = {0, 0, 2, 4, 12, 7, 10, 12, 12, 0};

// GLOBAL: CARDARTLIB 0x1001d288
// GLOBAL: DRAWCARDLIB 0x10021078
// GLOBAL: SHANDALAR 0x00581ee8
int g_ditherKernelErrorRowCounts[8] = {0, 0, 2, 2, 3, 2, 3, 3};

// GLOBAL: CARDARTLIB 0x1001d2b0
// GLOBAL: DRAWCARDLIB 0x100210a0
// GLOBAL: SHANDALAR 0x00581f10
int g_ditherKernelDivisors[0xa] = {0, 0, 16, 16, 42, 32, 32, 48, 236, 0};

// GLOBAL: CARDARTLIB 0x1001d2d8
// GLOBAL: DRAWCARDLIB 0x100210c8
// GLOBAL: SHANDALAR 0x00581f38
undefined1 g_ditherKernelTable[0xd80] = {
#include "g_ditherKernelTable.inc"
};

// GLOBAL: CARDARTLIB 0x10031aa0
// GLOBAL: DRAWCARDLIB 0x10028af0
// GLOBAL: SHANDALAR 0x005a82f0
undefined1 g_u8ClampTableStorage[0x400];

// GLOBAL: CARDARTLIB 0x1001e058
// GLOBAL: DRAWCARDLIB 0x10021e48
// GLOBAL: SHANDALAR 0x00582cb8
undefined1 *g_u8ClampTable = g_u8ClampTableStorage + 0x200;

// GLOBAL: CARDARTLIB 0x1001e05c
// GLOBAL: DRAWCARDLIB 0x10021e4c
// GLOBAL: SHANDALAR 0x00582cbc
undefined4 g_paletteDitherKernelCachedId = 0xFFFFFFFF;

// GLOBAL: CARDARTLIB 0x1001e060
// GLOBAL: DRAWCARDLIB 0x10021e50
// GLOBAL: SHANDALAR 0x00582cc0
undefined4 g_rgbQuantKernelCachedId = 0xFFFFFFFF;

// GLOBAL: CARDARTLIB 0x1001e0e0
// GLOBAL: DRAWCARDLIB 0x100223c0
// GLOBAL: SHANDALAR 0x00593cd0
int g_rowAlignBytes = 0x00000004;

// GLOBAL: CARDARTLIB 0x100322d8
// GLOBAL: DRAWCARDLIB 0x1003a1a8
unsigned char g_waveletScaleTableStorage[0x801];

// GLOBAL: CARDARTLIB 0x10032adc
// GLOBAL: DRAWCARDLIB 0x1003a9ac
undefined4 g_wvlCatalogsInitialized;

// GLOBAL: CARDARTLIB 0x1001e11c
// GLOBAL: DRAWCARDLIB 0x100223fc
undefined4 g_smallArtCatalogHandle = 0x00000001;

// GLOBAL: CARDARTLIB 0x1001e120
// GLOBAL: DRAWCARDLIB 0x10022400
undefined4 g_medArtCatalogHandle = 0x00000002;

// GLOBAL: CARDARTLIB 0x10032ae8
// GLOBAL: DRAWCARDLIB 0x1003a9b8
WvlEntry g_wvlEntryScratch;

// GLOBAL: CARDARTLIB 0x1001e118
// GLOBAL: DRAWCARDLIB 0x100223f8
unsigned char *g_waveletScaleToByteTable = g_waveletScaleTableStorage + 0x400;

// GLOBAL: CARDARTLIB 0x1001e124
// GLOBAL: DRAWCARDLIB 0x10022404
undefined4 g_haarScratchInit = 0x00000000;

// GLOBAL: CARDARTLIB 0x100eaB00
// GLOBAL: DRAWCARDLIB 0x10124550
undefined1 g_yuvClampTableStorage[0x2000];

// GLOBAL: CARDARTLIB 0x1001e128
// GLOBAL: DRAWCARDLIB 0x10022408
undefined1 *g_yuvClampTable = g_yuvClampTableStorage + 0x400;

// GLOBAL: CARDARTLIB 0x10032ae0
// GLOBAL: DRAWCARDLIB 0x1003a9b0
undefined4 g_waveletScaleToByteTableInit;

// GLOBAL: CARDARTLIB 0x100ea09c
// GLOBAL: DRAWCARDLIB 0x100f1f6c
undefined4 g_yuvClampTableInit;

#ifdef MODERN_FIXES
// Scratch space needed is dependant on screen size, allocate some bigass buffers to support 4k
undefined1 g_haarDecodeScratch[0x7a800 * 10];
unsigned char g_catalogReadScratch[0x3CC00 * 10];
#else
// GLOBAL: CARDARTLIB 0x10032c98
// GLOBAL: DRAWCARDLIB 0x1003ab68
// GLOBAL: SHANDALAR 0x0067ad80
undefined1 g_haarDecodeScratch[0x7a800];

// GLOBAL: CARDARTLIB 0x100ad498
// GLOBAL: DRAWCARDLIB 0x100b5368
// GLOBAL: SHANDALAR 0x006f5580
unsigned char g_catalogReadScratch[0x3CC00];
#endif

// GLOBAL: CARDARTLIB 0x100edb10
// GLOBAL: DRAWCARDLIB 0x10128e40
// GLOBAL: SHANDALAR 0x00952bf0
undefined1 g_errorDiffusionRowStorage[5 * 0x8060];

// GLOBAL: CARDARTLIB 0x100ecb04
// GLOBAL: DRAWCARDLIB 0x10126554
BOOL g_unpackResultUnused;

// GLOBAL: CARDARTLIB 0x100322cc
// GLOBAL: DRAWCARDLIB 0x1002931c
// GLOBAL: SHANDALAR 0x005a8b1c
undefined4 g_rgbQuantClampInit;

// GLOBAL: CARDARTLIB 0x100322d0
// GLOBAL: DRAWCARDLIB 0x10029320
// GLOBAL: SHANDALAR 0x005a8b20
undefined4 g_paletteClampInit;

// GLOBAL: CARDARTLIB 0x100ea31c
// GLOBAL: DRAWCARDLIB 0x10124540
int *g_haarScratchB;

// GLOBAL: CARDARTLIB 0x100ea320
// GLOBAL: DRAWCARDLIB 0x10124544
int *g_haarScratchA;

// GLOBAL: CARDARTLIB 0x10117100
// GLOBAL: DRAWCARDLIB 0x10152430
// GLOBAL: SHANDALAR 0x0097c1e0
void *g_errorDiffusionDeltaTablePtrs[0x41];

// GLOBAL: CARDARTLIB 0x101221e8
// GLOBAL: DRAWCARDLIB 0x10124538
undefined4 global_color_depth;

// FUNCTION: CARDARTLIB 0x1000570f
// FUNCTION: DRAWCARDLIB 0x10001cbf
// FUNCTION: SHANDALAR 0x0042ec23
int DitherBgr24ToPaletteColors(int dither_kernel_id, int serpentine, uint *bgr24, int height, int width, int row_padding)
{
  struct
  {
    short *err_cell;       /* [ebp-0x94] (also reused as temp) */
    int *delta_table;      /* [ebp-0x90] */
    short *err_px;         /* [ebp-0x8c] */
    int err_r;             /* [ebp-0x88] */
    int x_byte_step;       /* [ebp-0x84] */
    int nearest_rgb;       /* [ebp-0x80] */
    int x_start;           /* [ebp-0x7c] */
    undefined *kernel_ptr; /* [ebp-0x78] */
    int kernel_count;      /* [ebp-0x74] */
    int r_clamped;         /* [ebp-0x70] */
    int *err_row_ptrs[6];  /* [ebp-0x6c] */
    int y;                 /* [ebp-0x54] */
    undefined2 pad_50;     /* [ebp-0x50] */
    undefined2 pad_4e;     /* [ebp-0x4e] */
    undefined2 pad_4c_w;   /* [ebp-0x4c] */
    undefined2 pad_4a;     /* [ebp-0x4a] */
    int x;                 /* [ebp-0x48] */
    int k_row;             /* [ebp-0x44] */
    int k_x_off;           /* [ebp-0x40] */
    int err_b;             /* [ebp-0x3c] */
    int x_step;            /* [ebp-0x38] */
    uint g_clamped;        /* [ebp-0x34] */
    int err_g;             /* [ebp-0x30] */
    int *k_it;             /* [ebp-0x2c] */
    int rgb_clamped;       /* [ebp-0x28] */
    uint src_rgb;          /* [ebp-0x24] */
    undefined2 pad_20;     /* [ebp-0x20] */
    undefined2 pad_1e;     /* [ebp-0x1e] */
    undefined2 pad_1c;     /* [ebp-0x1c] */
    undefined2 pad_1a;     /* [ebp-0x1a] */
    int x_end;             /* [ebp-0x18] */
    uint clear_dwords;     /* [ebp-0x14] */
    undefined2 pad_10;     /* [ebp-0x10] */
    undefined2 pad_e;      /* [ebp-0xe] */
    undefined2 pad_c_w;    /* [ebp-0xc] */
    undefined2 pad_a;      /* [ebp-0xa] */
    int x_byte_off;        /* [ebp-0x8] */
    uint b_clamped;        /* [ebp-0x4] */
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
  s.kernel_ptr = &g_ditherKernelTable[(dither_kernel_id << 6) * 3];
  s.clear_dwords = (uint)((width + 10) * 8) >> 2;

  if (dither_kernel_id == 0)
  {
    g_paletteDitherKernelCachedId = dither_kernel_id;
    return 0;
  }
  else if (dither_kernel_id == 1)
  {
    g_paletteDitherKernelCachedId = dither_kernel_id;
    return QuantizeBgr24ToNearestPaletteColorInPlace(bgr24, height, width, row_padding);
  }

  if (g_paletteClampInit == 0)
  {
    for (s.x = -0x200; s.x < 0x200; s.x = s.x + 1)
    {
      if ((0 <= s.x) && (s.x <= 0xff))
      {
        g_u8ClampTable[s.x] = (undefined1)s.x;
      }
      else
      {
        if (s.x < 0)
        {
          g_u8ClampTable[s.x] = 0;
        }
        else
        {
          g_u8ClampTable[s.x] = 0xff;
        }
      }
    }
    g_paletteClampInit = 1;
  }

  if ((int)g_paletteDitherKernelCachedId != dither_kernel_id)
  {
    for (s.x = 0; (uint)s.x < 0x41; s.x = s.x + 1)
    {
      if (g_errorDiffusionDeltaTablePtrs[s.x] != (void *)0x0)
      {
        free(g_errorDiffusionDeltaTablePtrs[s.x]);
        g_errorDiffusionDeltaTablePtrs[s.x] = (void *)0x0;
      }
    }
    InitErrorDiffusionDeltaTables(dither_kernel_id, (int *)g_errorDiffusionDeltaTablePtrs);
    g_paletteDitherKernelCachedId = dither_kernel_id;
  }

  for (s.x = 0; (uint)s.x < 5; s.x = s.x + 1)
  {
    SetBytes((void *)(g_errorDiffusionRowStorage + s.x * 0x8060), 0, 0x8060);
    s.err_row_ptrs[s.x + 1] = (int *)((int)(g_errorDiffusionRowStorage + s.x * 0x8060) + 0x28);
  }

  s.kernel_count = g_ditherKernelTapCounts[dither_kernel_id];
  for (s.y = 0; s.y < height; s.y = s.y + 1, *(int *)&bgr24 += width * 3 + row_padding)
  {
    if (0 < s.x_step)
    {
      s.x_start = 0;
      s.x_end = width;
      s.x_byte_step = 3;
    }
    else
    {
      s.x_start = width + -1;
      s.x_end = -1;
      s.x_byte_step = -3;
    }

    s.x = s.x_start;
    s.x_byte_off = s.x_start + s.x_start + s.x_start;
    for (; s.x != s.x_end; s.x = s.x + s.x_step, s.x_byte_off = s.x_byte_off + s.x_byte_step)
    {
      s.src_rgb = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) & 0xffffff;
      *(uint *)&((byte *)bgr24)[s.x_byte_off] = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) & 0xff000000;

      s.err_px = (short *)s.err_row_ptrs[1] + (s.x << 2);

      if (s.src_rgb == 0)
      {
        s.nearest_rgb = 0;
        s.r_clamped = s.nearest_rgb;
        s.g_clamped = s.r_clamped;
        s.b_clamped = s.g_clamped;
      }
      else if (s.src_rgb == 0xffffff)
      {
        s.r_clamped = 0xff;
        s.g_clamped = s.r_clamped;
        s.b_clamped = s.g_clamped;
        s.nearest_rgb = 0xffffff;
      }
      else
      {

        s.b_clamped = s.src_rgb & 0xff;
        s.b_clamped += ((int)*s.err_px >> 8);
        s.b_clamped = (uint)(byte)g_u8ClampTable[s.b_clamped];

        s.g_clamped = ((byte *)&s.src_rgb)[1];
        s.g_clamped += ((int)s.err_px[1] >> 8);
        s.g_clamped = (uint)(byte)g_u8ClampTable[s.g_clamped];

        s.r_clamped = (s.src_rgb & 0xff0000) >> 16;
        s.r_clamped += ((int)s.err_px[2] >> 8);
        s.r_clamped = (uint)(byte)g_u8ClampTable[s.r_clamped];

        s.src_rgb = s.g_clamped << 8 | s.r_clamped << 0x10 | s.b_clamped;

        s.nearest_rgb = Octree_FindNearestColor(s.src_rgb);
      }
      *(uint *)&((byte *)bgr24)[s.x_byte_off] = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) | s.nearest_rgb;

      s.err_b = s.b_clamped - (s.nearest_rgb & 0xff);
      s.err_g = s.g_clamped - (s.nearest_rgb >> 8 & 0xff);
      s.err_r = s.r_clamped - ((uint)s.nearest_rgb >> 16);

      s.k_it = (int *)s.kernel_ptr;
      s.err_row_ptrs[0] = &s.k_it[s.kernel_count * 4];
      for (; s.err_row_ptrs[0] > s.k_it; s.k_it += 0x4)
      {
        s.k_x_off = s.k_it[1];
        s.k_row = s.k_it[2];
        s.delta_table = (int *)s.k_it[3];
        s.err_cell = ((s.k_x_off + s.x) << 2) + (short *)(s.err_row_ptrs[s.k_row + 1]);

        s.err_cell[0] = s.delta_table[s.err_b] + s.err_cell[0];
        s.err_cell[1] = s.delta_table[s.err_g] + s.err_cell[1];
        s.err_cell[2] = s.delta_table[s.err_r] + s.err_cell[2];
      }
    }

    RotateDwordsLeft1((undefined4 *)(s.err_row_ptrs + 1), g_ditherKernelErrorRowCounts[dither_kernel_id]);
    memset((void *)(s.err_row_ptrs[g_ditherKernelErrorRowCounts[dither_kernel_id]] + -10), 0, s.clear_dwords << 2);
    if (serpentine != 0)
    {
      s.x_step = -s.x_step;
      s.kernel_ptr = (undefined *)((int)g_ditherKernelTable + (uint)(s.x_step == -1) * 0x6c0 + dither_kernel_id * 0xc0);
    }
  }
}

// FUNCTION: CARDARTLIB 0x10005cb4
// FUNCTION: DRAWCARDLIB 0x10002264
// FUNCTION: MAGIC 0x004bae9a
// FUNCTION: SHANDALAR 0x0042f1c7
void RotateDwordsLeft1(undefined4 *dwords, uint count)
{
  undefined4 uVar1;
  uVar1 = *dwords;

  memcpy(dwords, dwords + 1, (count - 1) * 4);
  dwords[count - 1] = uVar1;
}

// FUNCTION: CARDARTLIB 0x10005cf5
// FUNCTION: DRAWCARDLIB 0x100022a5
// FUNCTION: SHANDALAR 0x0042f208
int InitErrorDiffusionDeltaTables(int dither_kernel_id, int *delta_table_ptrs_base)
{
  struct
  {
    int delta;
    int tap_index; // EBP - 0xc
    int tap_weight; // EBP - 8
    int half_divisor; // EBP - 4
  } s;

  s.half_divisor = g_ditherKernelDivisors[dither_kernel_id] >> 1;
  for (s.tap_index = 0; s.tap_index < g_ditherKernelTapCounts[dither_kernel_id]; s.tap_index = s.tap_index + 1)
  {
    s.tap_weight = *(int *)&g_ditherKernelTable[(s.tap_index << 4) + ((dither_kernel_id << 6) * 3)];

    if (delta_table_ptrs_base[s.tap_weight] != 0)
    {
      *(int *)&g_ditherKernelTable[0xc + ((dither_kernel_id << 6) * 3) + (s.tap_index << 4)] = delta_table_ptrs_base[s.tap_weight] + 0x400;
      continue;
    }

    delta_table_ptrs_base[s.tap_weight] = (int)malloc(0x800);
    assert((uint)(delta_table_ptrs_base[s.tap_weight] != 0), "D:\\Newmagic\\sources\\NedCard\\Palette.c",
           0x4fd, "Not enough memory for delta array\r\n");
    for (s.delta = -0x100; s.delta < 0x100; s.delta = s.delta + 1)
    {
      *(int *)(delta_table_ptrs_base[s.tap_weight] + 0x400 + s.delta * 4) =
          ((s.tap_weight * s.delta + s.half_divisor) * 0x100) / g_ditherKernelDivisors[dither_kernel_id];
    }
    *(int *)&g_ditherKernelTable[0xc + ((dither_kernel_id << 6) * 3) + (s.tap_index << 4)] = delta_table_ptrs_base[s.tap_weight] + 0x3fc;
  }

  for (s.tap_index = 0; s.tap_index < g_ditherKernelTapCounts[dither_kernel_id]; s.tap_index = s.tap_index + 1)
  {
    s.tap_weight = *(int *)&g_ditherKernelTable[0xc + 0x6b4 + ((dither_kernel_id << 6) * 3) + (s.tap_index << 4)];

    *(int *)&g_ditherKernelTable[0xc + 0x6b4 + 0xc + ((dither_kernel_id << 6) * 3) + (s.tap_index << 4)] = delta_table_ptrs_base[s.tap_weight] + 0x3fc;
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x100062c7
// FUNCTION: DRAWCARDLIB 0x10002877
// FUNCTION: SHANDALAR 0x0042f7dc
undefined4 DitherBgr24ToRgbQuantizedF8(int dither_kernel_id, int serpentine, uint *bgr24, int height, int width, int row_padding)
{
  struct
  {
    short *err_cell;       /* [ebp-0x94] (also reused as temp) */
    int *delta_table;      /* [ebp-0x90] */
    short *err_px;         /* [ebp-0x8c] */
    int err_r;             /* [ebp-0x88] */
    int x_byte_step;       /* [ebp-0x84] */
    int quant_rgb;         /* [ebp-0x80] */
    int x_start;           /* [ebp-0x7c] */
    undefined *kernel_ptr; /* [ebp-0x78] */
    int kernel_count;      /* [ebp-0x74] */
    int r_clamped;         /* [ebp-0x70] */
    int err_row_ptrs[6];   /* [ebp-0x6c] */
    int y;                 /* [ebp-0x54] */
    undefined2 pad_50;     /* [ebp-0x50] */
    undefined2 pad_4e;     /* [ebp-0x4e] */
    undefined2 pad_4c_w;   /* [ebp-0x4c] */
    undefined2 pad_4a;     /* [ebp-0x4a] */
    int x;                 /* [ebp-0x48] */
    int k_row;             /* [ebp-0x44] */
    int k_x_off;           /* [ebp-0x40] */
    int err_b;             /* [ebp-0x3c] */
    int x_step;            /* [ebp-0x38] */
    uint g_clamped;        /* [ebp-0x34] */
    int err_g;             /* [ebp-0x30] */
    int *k_it;             /* [ebp-0x2c] */
    int rgb_clamped;       /* [ebp-0x28] */
    uint src_rgb;          /* [ebp-0x24] */
    undefined2 pad_20;     /* [ebp-0x20] */
    undefined2 pad_1e;     /* [ebp-0x1e] */
    undefined2 pad_1c;     /* [ebp-0x1c] */
    undefined2 pad_1a;     /* [ebp-0x1a] */
    int x_end;             /* [ebp-0x18] */
    uint clear_dwords;     /* [ebp-0x14] */
    undefined2 pad_10;     /* [ebp-0x10] */
    undefined2 pad_e;      /* [ebp-0xe] */
    undefined2 pad_c_w;    /* [ebp-0xc] */
    undefined2 pad_a;      /* [ebp-0xa] */
    int x_byte_off;        /* [ebp-0x8] */
    uint b_clamped;        /* [ebp-0x4] */
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
  s.kernel_ptr = &g_ditherKernelTable[(dither_kernel_id << 6) * 3];
  s.clear_dwords = (uint)((width + 10) * 8) >> 2;

  if (g_rgbQuantClampInit == 0)
  {
    for (s.x = -0x200; s.x < 0x200; s.x = s.x + 1)
    {
      if ((0 <= s.x) && (s.x <= 0xff))
      {
        g_u8ClampTable[s.x] = (undefined1)s.x;
      }
      else
      {
        if (s.x < 0)
        {
          g_u8ClampTable[s.x] = 0;
        }
        else
        {
          g_u8ClampTable[s.x] = 0xff;
        }
      }
    }
    g_rgbQuantClampInit = 1;
  }

  if ((int)g_rgbQuantKernelCachedId != dither_kernel_id)
  {
    for (s.x = 0; (uint)s.x < 0x41; s.x = s.x + 1)
    {
      if (g_errorDiffusionDeltaTablePtrs[s.x] != (void *)0x0)
      {
        free(g_errorDiffusionDeltaTablePtrs[s.x]);
        g_errorDiffusionDeltaTablePtrs[s.x] = (void *)0x0;
      }
    }
    InitErrorDiffusionDeltaTables(dither_kernel_id, (int *)g_errorDiffusionDeltaTablePtrs);
    g_rgbQuantKernelCachedId = dither_kernel_id;
  }

  for (s.x = 0; (uint)s.x < 5; s.x = s.x + 1)
  {
    memset((void *)(g_errorDiffusionRowStorage + s.x * 0x8060), 0, 0x8060);
    s.err_row_ptrs[s.x + 1] = (int)(g_errorDiffusionRowStorage + s.x * 0x8060) + 0x28;
  }

  s.kernel_count = g_ditherKernelTapCounts[dither_kernel_id];
  for (s.y = 0; s.y < height; s.y = s.y + 1, *(int *)&bgr24 += width * 3 + row_padding)
  {
    if (0 < s.x_step)
    {
      s.x_start = 0;
      s.x_end = width;
      s.x_byte_step = 3;
    }
    else
    {
      s.x_start = width + -1;
      s.x_end = -1;
      s.x_byte_step = -3;
    }

    s.x = s.x_start;
    s.x_byte_off = s.x_start + s.x_start + s.x_start;
    for (; s.x != s.x_end; s.x = s.x + s.x_step, s.x_byte_off = s.x_byte_off + s.x_byte_step)
    {
      s.src_rgb = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) & 0xffffff;
      *(uint *)&((byte *)bgr24)[s.x_byte_off] = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) & 0xff000000;

      s.err_px = (short *)s.err_row_ptrs[1] + (s.x << 2);

      s.b_clamped = s.src_rgb & 0xff;
      s.b_clamped += ((int)*s.err_px >> 8);
      s.b_clamped = (uint)(byte)g_u8ClampTable[s.b_clamped];

      s.g_clamped = ((byte *)&s.src_rgb)[1];
      s.g_clamped += ((int)s.err_px[1] >> 8);
      s.g_clamped = (uint)(byte)g_u8ClampTable[s.g_clamped];

      s.r_clamped = (s.src_rgb & 0xff0000) >> 16;
      s.r_clamped += ((int)s.err_px[2] >> 8);
      s.r_clamped = (uint)(byte)g_u8ClampTable[s.r_clamped];

      s.src_rgb = s.g_clamped << 8 | s.r_clamped << 0x10 | s.b_clamped;
      if (s.src_rgb == 0)
      {
        s.quant_rgb = 0;
      }
      else if (s.src_rgb == 0xffffff)
      {
        s.quant_rgb = 0xffffff;
      }
      else
      {
        s.quant_rgb = Rgb888_QuantizeToF8(s.src_rgb);
      }
      *(uint *)&((byte *)bgr24)[s.x_byte_off] = *(uint *)(s.x_byte_off + (ptrdiff_t)bgr24) | s.quant_rgb;

      s.err_b = s.b_clamped - (s.quant_rgb & 0xff);
      s.err_g = s.g_clamped - (s.quant_rgb >> 8 & 0xff);
      s.err_r = s.r_clamped - (s.quant_rgb >> 16 & 0xff);
      s.err_row_ptrs[0] = 0;
      s.k_it = (int *)s.kernel_ptr;
      for (; s.err_row_ptrs[0] < s.kernel_count; s.err_row_ptrs[0]++, s.k_it += 0x4)
      {
        s.k_x_off = s.k_it[1];
        s.k_row = s.k_it[2];
        s.delta_table = (int *)s.k_it[3];
        s.err_cell = (short *)(s.err_row_ptrs[s.k_row + 1] +
                               (s.k_x_off + s.x) * 8);

        s.err_cell[0] = s.delta_table[s.err_b] + s.err_cell[0];
        s.err_cell[1] = s.delta_table[s.err_g] + s.err_cell[1];
        s.err_cell[2] = s.delta_table[s.err_r] + s.err_cell[2];
      }
    }

    RotateDwordsLeft1((undefined4 *)(s.err_row_ptrs + 1), g_ditherKernelErrorRowCounts[dither_kernel_id]);
    memset((void *)(s.err_row_ptrs[g_ditherKernelErrorRowCounts[dither_kernel_id]] + -0x28), 0, s.clear_dwords << 2);
    if (serpentine != 0)
    {
      s.x_step = -s.x_step;
      s.kernel_ptr = &g_ditherKernelTable[(uint)(s.x_step == -1) * 0x6c0 + dither_kernel_id * 0xc0];
    }
  }
  return 1;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100068d0
// FUNCTION: DRAWCARDLIB 0x10002e80
// FUNCTION: MAGIC 0x004bbac0
// FUNCTION: SHANDALAR 0x0042fde0
uint Rgb888_QuantizeToF8(uint rgb)
{
  return rgb & 0xf8f8f8;
}

// FUNCTION: CARDARTLIB 0x100068f0
// FUNCTION: DRAWCARDLIB 0x10008370
// FUNCTION: MAGIC 0x0041f670
WvlEntry *Catalog_LoadWvlEntry(int catalog_id, char *wvl_path, int decode_haar)
{
  struct
  {
    char fullpath[0x108]; /* [ebp-0x418] */
    char dir[0x104];      /* [ebp-0x310] */
    char fname[0x100];    /* [ebp-0x20c] */
    char ext[0x100];      /* [ebp-0x10c] */
    size_t entry_size;    /* [ebp-0x0c] */
    WvlEntry *entry;      /* [ebp-0x08] */
    char *dir_end;        /* [ebp-0x04] */
  } s;

  s.entry = &g_wvlEntryScratch;
  EnterCriticalSection(&global_critical_section_for_catalog);
  _splitpath(wvl_path, (char *)0x0, s.dir, s.fname, s.ext);
  if (g_wvlCatalogsInitialized == 0)
  {
    strcpy(s.fullpath, s.dir);
    strcat(s.fullpath, "SmallArt.cat");
    g_smallArtCatalogHandle = Catalog_Open(s.fullpath);
    strcpy(s.fullpath, s.dir);
    strcat(s.fullpath, "MedArt.cat");
    g_medArtCatalogHandle = Catalog_Open(s.fullpath);
    g_wvlCatalogsInitialized = 1;
  }
  s.dir_end = s.dir + strlen(s.dir);

  if (catalog_id == 0)
  {
    g_activeWvlCatalogHandle = g_smallArtCatalogHandle;
  }
  else if (catalog_id == 1)
  {
    g_activeWvlCatalogHandle = g_medArtCatalogHandle;
  }
  else
  {
    /* NOTE: Original does not leave the critical section on this path. */
    return (WvlEntry *)0x0;
  }

  strcpy(s.dir, s.fname);
  strcat(s.dir, s.ext);
  _strlwr(s.dir);

  if (s.entry != (WvlEntry *)0x0)
  {
    memset(s.entry, 0, 0x1b0);
    strcpy(s.entry->wvl_path, wvl_path);
    s.entry->data_ptr = (byte *)&g_catalogReadScratch;
    s.entry_size = Catalog_ReadEntry(g_activeWvlCatalogHandle, s.dir, (void **)&s.entry->data_ptr);
    if (s.entry_size == (size_t)-1)
    {
      strcat(wvl_path, "\n");
      OutputDebugStringA((LPCSTR)wvl_path);
      LeaveCriticalSection(&global_critical_section_for_catalog);
      return (WvlEntry *)0x0;
    }

    s.entry->data_size = s.entry_size - 0x9c;
    memcpy(s.entry, s.entry->data_ptr, 0x9c);
    s.entry->data_ptr = s.entry->data_ptr + 0x9c;
    if (s.entry->pieces == 4)
    {
      s.entry->width = s.entry->width << 1;
      s.entry->height = s.entry->height << 1;
    }

    if (decode_haar != 0)
    {
      s.entry->decoded_ptr = Wvl_DecodeHaar(s.entry, (byte *)0x0);
      if (s.entry->decoded_ptr != 0)
      {
        s.entry->decoded_flag = 1;
        LeaveCriticalSection(&global_critical_section_for_catalog);
      }
      else
      {
        Catalog_Unlock(s.entry);
        LeaveCriticalSection(&global_critical_section_for_catalog);
        return (WvlEntry *)0x0;
      }
    }
  }

  /* NOTE: Original does not leave the critical section on this path. */
  return s.entry;
}

// FUNCTION: CARDARTLIB 0x10006be3
// FUNCTION: DRAWCARDLIB 0x10008663
// FUNCTION: MAGIC 0x0041f965
// FUNCTION: SHANDALAR 0x00566693
byte *Wvl_DecodeHaar(WvlEntry *wvl, byte *dst)
{
  struct
  {
    byte *bgr_tmp;      /* [ebp-0x4c] */
    int scale_i;        /* [ebp-0x48] */
    int piece_idx;      /* [ebp-0x44] */
    int *y_plane;       /* [ebp-0x40] */
    int have_dst;       /* [ebp-0x3c] */
    int image_size;     /* [ebp-0x38] */
    int *unused2;       /* [ebp-0x34] */
    int chroma_base;    /* [ebp-0x30] */
    int height_px;      /* [ebp-0x2c] */
    int chroma_w;       /* [ebp-0x28] */
    int *unused1;       /* [ebp-0x24] */
    int base_size;      /* [ebp-0x20] */
    int alloc_bytes;    /* [ebp-0x1c] */
    int *v_plane;       /* [ebp-0x18] */
    int block_width;    /* [ebp-0x14] */
    int *u_plane;       /* [ebp-0x10] */
    int block_height;   /* [ebp-0xc] */
    int *unused3;       /* [ebp-0x8] */
    int pieces_per_row; /* [ebp-0x4] */
  } s;

  s.have_dst = 0;
  if (g_waveletScaleToByteTableInit == 0)
  {
    for (s.scale_i = -0x400; s.scale_i <= 0x400; s.scale_i = s.scale_i + 1)
    {
      if (s.scale_i >= 0 && s.scale_i <= 0xf8)
      {
        g_waveletScaleToByteTable[s.scale_i] = (char)((s.scale_i * 255) / 0xf8);
      }
      else if (s.scale_i < 0xa)
      {
        g_waveletScaleToByteTable[s.scale_i] = 0;
      }
      else
      {
        g_waveletScaleToByteTable[s.scale_i] = 0xff;
      }
    }
    g_waveletScaleToByteTableInit = 1;
  }

  if (dst == NULL)
  {
    dst = malloc(wvl->decode_bytes + 2000);
    MemZeroDwords((undefined8 *)dst, (wvl->decode_bytes + 2000) / 4);
  }
  else
  {
    MemZeroDwords((undefined8 *)dst, (wvl->decode_bytes + 2000) / 4);
    s.have_dst = 1;
  }

  g_unpackResultUnused = Wvl_UnpackPieces(dst, wvl);

  if (wvl->pieces == 1)
  {
    s.pieces_per_row = 1;
  }
  else if (wvl->pieces == 4)
  {
    s.pieces_per_row = 2;
  }
  else if (wvl->pieces == 0x10)
  {
    s.pieces_per_row = 4;
  }
  else
  {
    assert(0, "D:\\Newmagic\\sources\\NedCard\\haar.c", 0x15e,
           "wavelet pieces has illegal value: %d", wvl->pieces);
  }

  s.image_size = wvl->width;
  s.height_px = s.image_size;
  s.block_width = wvl->width / s.pieces_per_row;
  s.block_height = s.block_width;
  s.alloc_bytes = wvl->base_size;
  s.base_size = wvl->height / s.pieces_per_row;
  s.unused1 = &wvl->unk_2c;
  s.unused2 = &wvl->unk_3c;
  s.unused3 = &wvl->unk_4c;

  for (s.piece_idx = 0; s.piece_idx < wvl->pieces; s.piece_idx = s.piece_idx + 1)
  {
    if (wvl->chroma_is_420 != 0)
    {
      s.chroma_w = (s.block_height / s.pieces_per_row) / (((wvl->pieces == 1) ? 2 : 1));
      s.chroma_base = (s.base_size / s.pieces_per_row) / (((wvl->pieces == 1) ? 2 : 1));
    }
    else
    {
      s.chroma_w = s.block_height;
      s.chroma_base = s.base_size;
    }

    s.y_plane = (int *)&dst[(s.block_height * s.block_height + s.chroma_w * s.chroma_w * 2 + 0x40) * s.piece_idx * 4];
    s.u_plane = &s.y_plane[s.block_height * s.block_height] + 0x20;
    s.v_plane = &s.u_plane[s.chroma_w * s.chroma_w] + 0x20;

    Haar2D_ReconstructInPlace(s.y_plane, s.block_height, s.alloc_bytes);
    Haar2D_ReconstructInPlace(s.u_plane, s.chroma_w, s.alloc_bytes);
    Haar2D_ReconstructInPlace(s.v_plane, s.chroma_w, s.alloc_bytes);

    if (s.piece_idx < wvl->pieces / 2)
    {
      s.bgr_tmp = YuvPlanesToBgr24(g_haarDecodeScratch, s.y_plane, s.block_height, s.block_height,
                                   s.u_plane, s.v_plane, s.chroma_w, s.chroma_w, wvl->chroma_is_420);
    }
    else if (wvl->pieces > 1)
    {
      s.bgr_tmp = YuvPlanesToBgr24(g_haarDecodeScratch, s.y_plane, s.block_height, wvl->height - s.block_width,
                                   s.u_plane, s.v_plane, s.chroma_w, s.chroma_base, wvl->chroma_is_420);
    }
    else
    {
      s.bgr_tmp = YuvPlanesToBgr24(g_haarDecodeScratch, s.y_plane, s.block_height, s.base_size,
                                   s.u_plane, s.v_plane, s.chroma_w, s.chroma_base, wvl->chroma_is_420);
    }

    if (wvl->pieces > 1)
    {

      CopyBgr24RectIntoStridedBuffer(dst, s.bgr_tmp,
                                     (s.height_px / s.pieces_per_row) * (s.piece_idx % s.pieces_per_row),
                                     (s.height_px / s.pieces_per_row) * (s.piece_idx / s.pieces_per_row), s.block_height,
                                     s.block_height, s.height_px, s.height_px);
      free(s.bgr_tmp);
    }
    else
    {
      if (s.have_dst == 0)
      {
        free(dst);
      }
      dst = s.bgr_tmp;
    }
  }
  return dst;
}

// FUNCTION: CARDARTLIB 0x10007035
// FUNCTION: DRAWCARDLIB 0x10008ab5
void CopyBgr24RectIntoStridedBuffer(byte *dst_bgr24, byte *src_bgr24, int dst_x, int dst_y, int rect_width,
                                    int rect_height, int dst_stride_pixels, int unused)
{
  int row;

  dst_bgr24 += (dst_stride_pixels * dst_y + dst_x) * 3;

  for (row = 0; row < rect_height; row = row + 1, dst_bgr24 += dst_stride_pixels * 3, src_bgr24 += rect_width * 3)
  {
    CopyBytes(dst_bgr24, src_bgr24, rect_width * 3);
  }
}

// FUNCTION: CARDARTLIB 0x10007238
// FUNCTION: DRAWCARDLIB 0x10008cb8
void CopyBytes(void *dst, const void *src, size_t num)
{
#ifdef MODERN_FIXES
  memcpy(dst, src, num);
#else
  // TODO: this looks like real inline asm but who knows
  __asm {
    mov edi, dst
    mov esi, src
    mov ecx, num
    push ecx
    shr ecx, 3
    je copy_bytes_tail
  copy_bytes_qword_loop:
    fld qword ptr [esi]
    fstp qword ptr [edi]
    add esi, 8
    add edi, 8
    dec ecx
    jne copy_bytes_qword_loop
  copy_bytes_tail:
    pop ecx
    and ecx, 7
    je copy_bytes_done
    rep movsb
  copy_bytes_done:
  }
#endif
}

// FUNCTION: CARDARTLIB 0x10007273
// FUNCTION: DRAWCARDLIB 0x10008cf3
// FUNCTION: SHANDALAR 0x00566d25
void SetBytes(void *dst, int value, size_t num)
{
#ifdef MODERN_FIXES
  memset(dst, value, num);
#else
  // This is weird because it doesn't do anything?
  __int64 uVar1;

  uVar1 = (__int64)(value << 8 | value);
  uVar1 |= uVar1 << 16;
  uVar1 |= uVar1 << 32;

  // TODO: this looks like real inline asm but who knows
  __asm {
    fld qword ptr [ebp - 8]
    mov edi, dst
    mov ecx, num
    push ecx
    shr ecx, 3
    dec ecx
  set_bytes_qword_loop:
    fst qword ptr [edi]
    add edi, 8
    dec ecx
    jne set_bytes_qword_loop
    fstp qword ptr [edi]
    pop ecx
    and ecx, 7
    mov eax, value
    rep stosb
  }
#endif
}

// FUNCTION: CARDARTLIB 0x100072dc
// FUNCTION: DRAWCARDLIB 0x10008d5c
void Haar2D_ReconstructInPlace(int *coeffs, int full_size, int base_size)
{
  struct
  {
    int *idk;
    int *piVar1;       // ebp - 0x18
    int *haarScratchA; // ebp - 0x14
    int *haarScratchB; // ebp - 0x10
    int i;             // ebp - 0xc
    int *pad2;         // ebp - 8
    int *pad3;         // ebp - 4
  } s;

  if (g_haarScratchInit == 0)
  {
    g_haarScratchA = malloc(0x32000);
    s.haarScratchA = g_haarScratchA;
    g_haarScratchB = malloc(0x32000);

    s.haarScratchB = g_haarScratchB;
    g_haarScratchInit = 1;
  }
  else
  {
    s.haarScratchA = g_haarScratchA;
    s.haarScratchB = g_haarScratchB;
  }

  for (s.i = base_size; s.i < full_size; s.i = s.i << 1)
  {
    s.pad2 = coeffs;
    s.idk = &coeffs[s.i * s.i];

    Haar_CombineSumDiff(s.pad2, s.idk, s.haarScratchA, s.i, s.i, s.i * 2, s.i);

    s.pad3 = &coeffs[s.i * s.i * 2];
    s.piVar1 = &coeffs[s.i * s.i * 3];

    Haar_CombineSumDiff(s.pad3, s.piVar1, s.haarScratchB, s.i,
                        s.i, s.i * 2, s.i);

    Haar_CombineSumDiffHalf(s.haarScratchA, s.haarScratchB, coeffs, s.i, s.i * 2, s.i * 2, s.i * 2);
  }
}

// FUNCTION: CARDARTLIB 0x10007419
// FUNCTION: DRAWCARDLIB 0x10008e99
void Haar_CombineSumDiff(int *src_a, int *src_b, int *dst, int width, int rows, undefined4 src_stride_unused, int dst_stride)
{
  struct
  {
    int *src_a_end;  // ebp - 0x10
    int *src_a_row0; // ebp - 0xc
    int *dst_row;    // ebp - 8
    int row;         // ebp - 4
  } s;

  s.row = dst_stride;

  for (s.row = 0; s.row < rows; s.row++, dst++, src_b++)
  {
    s.dst_row = dst;
    s.src_a_row0 = src_a;
    s.src_a_end = (width - 1) + src_a + 1;
    s.dst_row = s.dst_row + dst_stride * 2;
    src_a++;

    for (; s.src_a_end > src_a; src_a++, src_b++, s.dst_row += dst_stride * 2)
    {
      *s.dst_row = *src_b + *src_a;

      *(int *)((char *)s.dst_row + dst_stride * 4) = *src_a - *src_b;
    }

    *dst = *src_b + *s.src_a_row0;
    dst[dst_stride] = *s.src_a_row0 - *src_b;
  }
}

// FUNCTION: CARDARTLIB 0x100074ee
// FUNCTION: DRAWCARDLIB 0x10008f6e
void Haar_CombineSumDiffHalf(int *src_a, int *src_b, int *dst, int width, int rows, undefined4 src_stride_unused, int dst_stride)
{
  struct
  {
    int *src_a_end;  // ebp - 0x10
    int *src_a_row0; // ebp - 0xc
    int *dst_row;    // ebp - 8
    int row;         // ebp - 4
  } s;

  for (s.row = 0; s.row < rows; s.row++, dst++, src_b++)
  {
    s.dst_row = dst;
    s.src_a_row0 = src_a;
    s.src_a_end = (width - 1) + src_a + 1;
    s.dst_row = s.dst_row + dst_stride * 2;
    src_a++;

    for (; s.src_a_end > src_a; src_a++, src_b++, s.dst_row += dst_stride * 2)
    {
      *s.dst_row = (*src_b + *src_a) >> 1;
      *(int *)((char *)s.dst_row + dst_stride * 4) = (*src_a - *src_b) >> 1;
    }

    *dst = (*src_b + *s.src_a_row0) >> 1;
    dst[dst_stride] = (*s.src_a_row0 - *src_b) >> 1;
  }
}

// FUNCTION: CARDARTLIB 0x100075ef
// FUNCTION: DRAWCARDLIB 0x1000906f
undefined1 *YuvPlanesToBgr24(undefined1 *out_bgr24, int *luma, int width, int height, int *chroma_u, int *chroma_v,
                             int chroma_stride, undefined4 unused_chroma_height, int chroma_is_420)

{
  struct
  {
    int y;                /* ebp - 0x2c */
    int u;                /* ebp - 0x28 */
    int v;                /* ebp - 0x24 */
    undefined1 *out_base; /* ebp - 0x20 */
    int row;              /* ebp - 0x1c */
    int col;              /* ebp - 0x18 */
    int green;            /* ebp - 0x14 */
    int *chroma_u_ptr;    /* ebp - 0x10 */
    int *chroma_v_ptr;    /* ebp - 0xc */
    int red;              /* ebp - 8 */
    int blue;             /* ebp - 4 */
  } s;

  if (g_yuvClampTableInit == 0)
  {
    for (s.col = -0x400; s.col < 0x1c00; s.col = s.col + 1)
    {
      if (0 < s.col)
      {
        g_yuvClampTable[s.col] = (char)(((s.col >> 2) <= 0xff) ? (s.col >> 2) : 0xff);
      }
      else
      {
        g_yuvClampTable[s.col] = 0;
      }
    }
    g_yuvClampTableInit = 1;
  }
  if (out_bgr24 == (undefined1 *)0x0)
  {
    out_bgr24 = malloc(width * width * 3 + 0x10);
    s.out_base = out_bgr24;
  }
  else
  {
    s.out_base = out_bgr24;
  }

  for (s.row = 0; s.row < height; s.row++)
  {
    if (chroma_is_420 != 0)
    {
      s.chroma_u_ptr = &chroma_u[s.row / 2 * chroma_stride];
      s.chroma_v_ptr = &chroma_v[s.row / 2 * chroma_stride];
    }
    else
    {
      s.chroma_u_ptr = &chroma_u[s.row * chroma_stride];
      s.chroma_v_ptr = &chroma_v[s.row * chroma_stride];
    }

    for (s.col = 0; s.col < width; s.col++, luma++, out_bgr24 += 3)
    {
      s.y = *luma;

      if (chroma_is_420 != 0)
      {
        if ((s.col & 1) == 0)
        {
          s.u = *s.chroma_u_ptr;
          s.v = *s.chroma_v_ptr;
        }
        else
        {
          s.u = (*s.chroma_u_ptr + s.chroma_u_ptr[((uint)(width - 1) - (uint)s.col) != 0]) / 2;
          s.v = (*s.chroma_v_ptr + s.chroma_v_ptr[((uint)(width - 1) - (uint)s.col) != 0]) / 2;
        }

        s.red = ((s.v >> 3) + (s.v >> 1) + s.v) - 0x333 + s.y;
        s.blue = (s.u * 2) - 0x400 + s.y;
      }
      else
      {
        s.u = *s.chroma_u_ptr;
        s.v = *s.chroma_v_ptr;

        s.red = ((s.v >> 3) + (s.v >> 1) + s.v) - 0x333 + s.y;
        s.blue = (s.u * 2) - 0x400 + s.y;
      }

      s.green = (((s.y * 2) - (s.y >> 2)) - (s.red >> 1)) - ((s.blue >> 2) - (s.blue >> 4));
      out_bgr24[0] = g_yuvClampTable[s.blue];
      out_bgr24[1] = g_yuvClampTable[s.green];
      out_bgr24[2] = g_yuvClampTable[s.red];

      if (chroma_is_420 != 0)
      {
        if ((s.col & 1) != 0)
        {
          s.chroma_u_ptr = s.chroma_u_ptr + 1;
          s.chroma_v_ptr = s.chroma_v_ptr + 1;
        }
      }
      else
      {
        s.chroma_u_ptr = s.chroma_u_ptr + 1;
        s.chroma_v_ptr = s.chroma_v_ptr + 1;
      }
    }
  }
  return s.out_base;
}

// FUNCTION: CARDARTLIB 0x100078b8
// FUNCTION: DRAWCARDLIB 0x10009338
BOOL Wvl_UnpackPieces(byte *dst, WvlEntry *wvl)
{
  struct
  {
    byte *dst_y;        /* ebp - 0x3c */
    WvlEntry *wvl;      /* ebp - 0x38 */
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

  s.wvl = wvl;

  s.full_w = s.wvl->width / ((s.wvl->pieces == 1) ? 1 : 2);
  s.full_h = s.full_w;
  s.chroma_w = s.full_h / ((s.wvl->chroma_is_420 == 0) ? 1 : 2);
  s.chroma_h = s.chroma_w;

  s.bitstream = wvl->data_ptr;
  s.huff_data = s.bitstream;
  s.base_size = s.wvl->base_size;

  s.tmp = (s.full_w * s.full_h) + s.chroma_w * s.chroma_h * 2;

  s.node_count = *(int *)s.bitstream;
  s.bitstream = s.bitstream + 4;
  s.symbol_table = (uint *)s.bitstream;
  *s.symbol_table = 0x80000000;
  s.bitstream = s.bitstream + (s.node_count << 2);
  s.bitstream =
      s.bitstream +
      Huffman13_Init((undefined4)s.bitstream, (undefined4)s.symbol_table, (undefined4)s.node_count);

  for (s.layer = 0; s.layer < s.wvl->pieces; s.layer = s.layer + 1)
  {
    s.dst_y = &dst[(((s.tmp + 0x40) * s.layer) << 2)];

    s.dst_u = &s.dst_y[((s.full_w * s.full_h) << 2)] + 0x80;

    s.dst_v = &s.dst_u[((s.chroma_w * s.chroma_h) << 2)] + 0x80;

    memcpy(s.dst_y, s.bitstream, (s.base_size * s.base_size) << 2);

    s.dst_y += (s.base_size * s.base_size) << 2;

    s.bitstream += (s.base_size * s.base_size) << 2;

    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)s.dst_y, (uint *)s.bitstream, s.wvl->huff_bytes_y[s.layer]);
    s.bitstream = s.bitstream + s.wvl->huff_bytes_y[s.layer];

    memcpy(s.dst_u, s.bitstream, (s.base_size * s.base_size) << 2);

    s.dst_u += (s.base_size * s.base_size) << 2;

    s.bitstream += (s.base_size * s.base_size) << 2;

    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)s.dst_u, (uint *)s.bitstream, s.wvl->huff_bytes_u[s.layer]);
    s.bitstream = s.bitstream + s.wvl->huff_bytes_u[s.layer];

    memcpy(s.dst_v, s.bitstream, (s.base_size * s.base_size) << 2);

    s.dst_v += (s.base_size * s.base_size) << 2;

    s.bitstream += (s.base_size * s.base_size) << 2;

    Huffman13_DecodeDwordsWithZeroRuns((undefined8 *)s.dst_v, (uint *)s.bitstream, s.wvl->huff_bytes_v[s.layer]);
    s.bitstream = s.bitstream + s.wvl->huff_bytes_v[s.layer];
  }
  return 0;
}

// FUNCTION: CARDARTLIB 0x1000807f
// FUNCTION: DRAWCARDLIB 0x10009aff
// FUNCTION: SHANDALAR 0x005678fa
uint *Wvl_DecodeToBgr24(byte *out_bgr24, WvlEntry *wvl_entry, int width, int height)
{
  struct
  {
    int output_row_align;
    int row_padding;
    int y_step;
    int x_step;
    byte *out_base;
    uint x_map[4096];
    byte *decoded;    // ebp - 0x104c
    int y;            // ebp - 0x1048
    int y_acc;        // ebp - 0x1044
    int x;            // ebp - 0x1040
    int out_nonnull;  // ebp - 0x103c
    int src_width;    // ebp - 0x1038
    int *y_map_ptr;   // ebp - 0x1034
    int x_acc;        // ebp - 0x1030
    int *x_map_ptr;   // ebp - 0x102c
    byte *out_start;  // ebp - 0x1028
    byte *out_ptr;    // ebp - 0x1024
    int y_scale;      // ebp - 0x1020
    int row_pad;      // ebp - 0x101c
    int row_bytes;    // ebp - 0x1018
    int x_scale;      // ebp - 0x1014
    int zero_1010;    // ebp - 0x1010
    byte *src_ptr;    // ebp - 0x100c
    uint y_map[1016]; // ebp - 0x1008
    int tmp_28;       // ebp - 0x28
    int tmp_24;       // ebp - 0x24
    int tmp_20;       // ebp - 0x20
    int tmp_1c;       // ebp - 0x1c
    int tmp_18;       // ebp - 0x18
    int tmp_14;       // ebp - 0x14
    int tmp_10;       // ebp - 0x10
    int tmp_c;        // ebp - 0xc
    int tmp_8;        // ebp - 0x8
    byte *tmp_4;      // ebp - 0x4
  } s;

  s.x_acc = 0;
  s.y_acc = 0;
  s.out_base = out_bgr24;
  s.out_start = (byte *)0;
  s.x_map_ptr = s.x_map;
  s.y_map_ptr = s.y_map;
  if (out_bgr24 != (byte *)0)
  {
    s.out_nonnull = 1;
  }
  else
  {
    s.out_nonnull = 0;
  }

  if (wvl_entry == (WvlEntry *)0)
  {
    return (uint *)0;
  }

  s.x_scale = wvl_entry->width;
  s.y_scale = wvl_entry->height;
  s.x_scale <<= 0x10;
  s.y_scale <<= 0x10;
  s.x_step = s.x_scale / width;
  s.y_step = s.y_scale / height;

  if (wvl_entry->decoded_flag != 0)
  {
    s.decoded = wvl_entry->decoded_ptr;
  }
  else
  {
    s.decoded = Wvl_DecodeHaar(wvl_entry, g_haarDecodeScratch);
  }

  s.src_width = wvl_entry->width;
  s.tmp_8 = 3;
  s.zero_1010 = 0;
  s.row_pad = (g_rowAlignBytes - (width * 3) % g_rowAlignBytes) % g_rowAlignBytes;

  if (out_bgr24 == (byte *)0)
  {
    out_bgr24 = g_catalogReadScratch;
    s.out_base = out_bgr24;
  }
  else
  {
    s.out_base = out_bgr24;
  }

  for (s.x = 0, s.x_acc = 0; s.x < width; s.x += 1, s.x_acc += s.x_step, s.x_map_ptr += 1)
  {
    if (s.tmp_8 == 3)
    {
      *s.x_map_ptr = (uint)(s.x_acc >> 8);
    }
    else
    {
      assert(0, "D:\\Newmagic\\sources\\NedCard\\haar.c", 0x6f7,
             "Only Works on 24 bit images\n");
    }
  }

  if (wvl_entry->height < height)
  {
    out_bgr24 += (height - wvl_entry->height) * (s.row_pad + width * 3);
    s.out_ptr = out_bgr24;
  }
  else
    s.out_ptr = (byte *)out_bgr24;

  for (s.y = 0; s.y < wvl_entry->height; s.y += 1, out_bgr24 += s.row_pad)
  {
    s.tmp_4 = s.decoded + (s.src_width * s.tmp_8 + s.zero_1010) * s.y;
    s.x_map_ptr = s.x_map;

    for (s.x = 0; s.x < width; s.x += 1, s.x_map_ptr += 1)
    {
      s.src_ptr = s.tmp_4 + (((int)*s.x_map_ptr >> 8) * 3);

      out_bgr24[0] = ((int)((uint)s.src_ptr[3] - (uint)s.src_ptr[0]) * (*s.x_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[0];
      out_bgr24[1] = ((int)((uint)s.src_ptr[4] - (uint)s.src_ptr[1]) * (*s.x_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[1];
      out_bgr24[2] = ((int)((uint)s.src_ptr[5] - (uint)s.src_ptr[2]) * (*s.x_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[2];

      out_bgr24 += 3;
    }
  }

  for (s.x = 0, s.y_acc = 0; s.x < height; s.x++, s.y_acc += s.y_step, s.y_map_ptr++)
  {
    if (s.tmp_8 == 3)
    {
      *s.y_map_ptr = (uint)(s.y_acc >> 8);
    }
    else
    {
      assert(0, "D:\\Newmagic\\sources\\NedCard\\haar.c", 0x730,
             "Only Works on 24 bit images\n");
    }
  }

  s.row_bytes = width * 3 + s.row_pad;
  if (height < wvl_entry->height)
  {
    memcpy(s.x_map, &s.out_base[(wvl_entry->height - 1) * s.row_bytes], s.row_bytes);
  }

  for (s.x = 0; s.x < width; s.x++)
  {
    out_bgr24 = &s.out_base[s.x * 3];
    s.tmp_4 = s.out_ptr + s.x * 3;
    s.y_map_ptr = s.y_map;

    for (s.y = 0; s.y < height - 1; s.y++, s.y_map_ptr++, out_bgr24 += s.row_bytes)
    {
      s.src_ptr = (byte *)(MIN((int)*s.y_map_ptr >> 8, wvl_entry->height - 2) * s.row_bytes + (int)s.tmp_4);

      out_bgr24[0] = ((int)((uint)s.src_ptr[s.row_bytes] - (uint)s.src_ptr[0]) * (*s.y_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[0];
      out_bgr24[1] = ((int)((uint)s.src_ptr[s.row_bytes + 1] - (uint)s.src_ptr[1]) * (*s.y_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[1];
      out_bgr24[2] = ((int)((uint)s.src_ptr[s.row_bytes + 2] - (uint)s.src_ptr[2]) * (*s.y_map_ptr & 0xff) >> 8) + (int)(uint)s.src_ptr[2];
    }
  }

  if (height < wvl_entry->height)
  {
    memcpy(&s.out_base[(height - 1) * s.row_bytes], s.x_map, s.row_bytes);
  }
  else
  {
    memset(&s.out_base[(height - 1) * s.row_bytes], 0, s.row_bytes);
  }

  s.output_row_align = 4;
  s.row_padding = (s.output_row_align - ((width * 3) % s.output_row_align)) % s.output_row_align;
  if (global_dither_kernel_id == 0)
  {
    QuantizeBgr24ToNearestPaletteColorInPlace((uint *)s.out_base, height, width, s.row_padding);
  }
  else if (global_color_depth == 16)
  {
    DitherBgr24ToRgbQuantizedF8(global_dither_kernel_id, global_serpentine, (uint *)s.out_base, height, width, s.row_padding);
  }
  else if (global_color_depth == 8)
  {
    DitherBgr24ToPaletteColors(global_dither_kernel_id, global_serpentine, (uint *)s.out_base, height, width, s.row_padding);
  }

  return (uint *)s.out_base;
}
