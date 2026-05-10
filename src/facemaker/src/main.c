#include <process.h>
#include <windows.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <string.h>
#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"
#include "defs.h"
#include "facemaker_types.h"

#ifdef _fileno
#undef _fileno
#endif

typedef struct FacemakerWindowBounds
{
  char pad_0[12];
  int max_x;
  int max_y;
} FacemakerWindowBounds;

typedef struct FaceControl FaceControl;

typedef void(__cdecl *FaceControlCallback)(FaceControl *control);

typedef struct FaceControl
{
  int x;
  int y;
  int width;
  int height;
  int unknown_10;
  FaceControlCallback on_activate;
  int value_index;
  int value_delta;
  int state;
  int page_by_mode[4];
} FaceControl;

typedef struct FaceInitStack
{
  char char_temp;
  char pad_001[3];
  char *scan_ptr;
  int tmp_y;
  int tmp_x;
  char pad_010[0x100];
  FILE *file;
  unsigned int style_values[6];
  int loop_index;
  int face_slot_x;
  int control_slot;
  int button_text_ids[5];
} FaceInitStack;

extern DIBSurface * FUN_00406810(int width, int height, int bits_per_pixel);
extern DIBSurface * FUN_004069e0(int page_number, int width, int height, int bits_per_pixel);
extern int FUN_004065e0();
extern int FUN_004065f0(char *executable_name, char *config_name);
extern DIBSurface * FUN_00406610(void);
extern DIBSurface * FUN_00406620(int page_number);
extern DIBSurface * FUN_00406650(void);
extern void FUN_00406bd0(int page_number, DIBSurface *page);
extern char s_D__NewMagic__sources__sidlib__lib_c_0040d0ec[];
extern int FUN_00407b10(char *path);
extern int FUN_00407c20(int font_id, FILE *file);
extern void FUN_00407210(int *dst, unsigned int dst_x, int dst_y, unsigned int width,
                                 DWORD height, int *src, int src_x, int src_y);
extern void FUN_00408da0(int *dst, int x, int y, int src_page);
extern void FUN_00408f20(int *dst, int x, int y, int width, int height, int src_page);
extern void FUN_00409eb0(int page_number, int x, int y, char *path, void *opaque);
extern unsigned int FUN_00402600(int value);
extern void FUN_00402621(int *dst, int dst_x, int dst_y, int width, int height, int *src,
                                 int src_x, int src_y);
extern void FUN_0040268a(int *dst, int x, int y, int src_page);
extern int FUN_00407e20(int font_id, unsigned int point_size, char *font_file, char *font_name,
                                int weight, DWORD italic);
extern void FUN_00406cc0(int enabled);
extern int FUN_004093d0(void);
extern void FUN_0040a110(int page_number, char *path);
extern size_t FUN_00408970(void *param_1, char *param_2);
extern int FUN_004089c0(int *param_1, char *param_2);
extern int FUN_004086c0(int *dst, int text_id, int a3, int a4, int a5, int a6, int x, int y,
                                int *arg9);
extern void FUN_00407570(void *dst, int dst_x, int dst_y, int src_w, int src_h, void *src,
                                 int src_x, int src_y, int copy_w, int copy_h);
extern int FUN_0040369c(void);
extern int FUN_00404d70(char *path, int counts_out, void *entries_out, int *first_entry_out);
extern int FUN_00402a84(FaceControl *control);
extern int FUN_00402ad9(FaceControl *control);
extern void FUN_00403573(int index, int selected);
extern int FUN_004028d1(FaceControl *control, int mode);
extern void FUN_004032e4(void);
extern void FUN_00408900(void);
extern void FUN_0040a130(int page_number, char *path);
extern int FUN_00407f30(int font_id, unsigned int style);
extern void FUN_004088d0(int *dst, int text_id, int x, int y, char *text);
extern int FUN_00408a50(int page_number, int x, int y, unsigned int width, int height);
extern void FUN_00408920(void);
extern void FUN_004075d0(unsigned int *param_1, int param_2, int param_3, int param_4,
                                 unsigned int param_5);
extern void FUN_004076c0(unsigned int *param_1, int param_2, int param_3, int param_4,
                                 unsigned int param_5);
extern void FUN_0040aa20(double *dst, double *src, unsigned int size);
extern int DAT_0040d088;
extern int DAT_00426570;
extern RGBQUAD DAT_00426580[256];
extern FontSlot DAT_004233d0[0x10];
extern char DAT_0040d224[];

// GLOBAL: FACEMAKER 0x0040c010
FacemakerWindowBounds DAT_0040c010;

// GLOBAL: FACEMAKER 0x0040c034
FacemakerWindowBounds *PTR_DAT_0040c034 = &DAT_0040c010;

// GLOBAL: FACEMAKER 0x0040c05c
FacemakerWindowBounds *PTR_DAT_0040c05c = &DAT_0040c010;

// GLOBAL: FACEMAKER 0x0040c0ac
FacemakerWindowBounds *PTR_DAT_0040c0ac = &DAT_0040c010;

// GLOBAL: FACEMAKER 0x0040c9e4
int DAT_0040c9e4 = 1;

// GLOBAL: FACEMAKER 0x0040c188
int DAT_0040c188;

// GLOBAL: FACEMAKER 0x0040c190
int DAT_0040c190;

// GLOBAL: FACEMAKER 0x0040c194
int DAT_0040c194;

// GLOBAL: FACEMAKER 0x0040c1c0
int DAT_0040c1c0;

// GLOBAL: FACEMAKER 0x0040c1c4
int DAT_0040c1c4;

// GLOBAL: FACEMAKER 0x0040c1e0
int DAT_0040c1e0;

// GLOBAL: FACEMAKER 0x0040c1e4
int DAT_0040c1e4;

// GLOBAL: FACEMAKER 0x0040c1e8
FaceControl DAT_0040c1e8[0x15];

// GLOBAL: FACEMAKER 0x0040c208
int DAT_0040c208;

// GLOBAL: FACEMAKER 0x0040c284
FaceControl DAT_0040c284;

// GLOBAL: FACEMAKER 0x0040c2b8
FaceControl DAT_0040c2b8;

// GLOBAL: FACEMAKER 0x0040c354
FaceControl DAT_0040c354;

// GLOBAL: FACEMAKER 0x0040c388
FaceControl DAT_0040c388;

// GLOBAL: FACEMAKER 0x0041afb4
HDC global_main_hdc;

// GLOBAL: FACEMAKER 0x0041ae40
char DAT_0041ae40[0x105];

// GLOBAL: FACEMAKER 0x0041b024
char DAT_0041b024[256];

// GLOBAL: FACEMAKER 0x0041af50
char DAT_0041af50[0x64];

// GLOBAL: FACEMAKER 0x0041afc0
char DAT_0041afc0[0x64];

// GLOBAL: FACEMAKER 0x0041b028
HWND DAT_0041b028;

// GLOBAL: FACEMAKER 0x0041b030
char DAT_0041b030[0x105];

// GLOBAL: FACEMAKER 0x0041c140
HINSTANCE DAT_0041c140;

// GLOBAL: FACEMAKER 0x0041c144
int DAT_0041c144;

// GLOBAL: FACEMAKER 0x0041c150
char *DAT_0041c150[1000];

// GLOBAL: FACEMAKER 0x0041d110
int global_screen_width;

// GLOBAL: FACEMAKER 0x0041d114
int global_screen_height;

// GLOBAL: FACEMAKER 0x0041d120
char *DAT_0041d120[1000];

// GLOBAL: FACEMAKER 0x0041d0f0
char DAT_0041d0f0[0x420];

// GLOBAL: FACEMAKER 0x0041e3f0
int DAT_0041e3f0[4];

// GLOBAL: FACEMAKER 0x0041e400
int DAT_0041e400[4];

// GLOBAL: FACEMAKER 0x0041e410
int DAT_0041e410;

// GLOBAL: FACEMAKER 0x0041e420
char DAT_0041e420[0x80];

// GLOBAL: FACEMAKER 0x0041e4a0
int DAT_0041e4a0;

// GLOBAL: FACEMAKER 0x0041e4a4
int DAT_0041e4a4;

// GLOBAL: FACEMAKER 0x0041e4b0
int DAT_0041e4b0[10];

// GLOBAL: FACEMAKER 0x0041e4dc
int DAT_0041e4dc;

// GLOBAL: FACEMAKER 0x0041e4e0
int DAT_0041e4e0[4];

// GLOBAL: FACEMAKER 0x0041e500
int DAT_0041e500[3];

// GLOBAL: FACEMAKER 0x0041e510
int DAT_0041e510[4];

// GLOBAL: FACEMAKER 0x0041e520
int DAT_0041e520[2];

// GLOBAL: FACEMAKER 0x0041e530
int DAT_0041e530[10];

// GLOBAL: FACEMAKER 0x0041e558
int DAT_0041e558[2];

// GLOBAL: FACEMAKER 0x0041e560
int DAT_0041e560;

// GLOBAL: FACEMAKER 0x0041e570
char *DAT_0041e570[1000];

// GLOBAL: FACEMAKER 0x0041e0c0
int DAT_0041e0c0;

// GLOBAL: FACEMAKER 0x0041e0d0
char DAT_0041e0d0[0x340];

// GLOBAL: FACEMAKER 0x0041e564
int DAT_0041e564;

extern DIBSurface *DAT_00426540[10];
extern int DAT_00426568;
extern int DAT_0042656c;

// GLOBAL: FACEMAKER 0x00423338
int DAT_00423338;

// GLOBAL: FACEMAKER 0x0042333c
int DAT_0042333c;

// GLOBAL: FACEMAKER 0x0040c658
char DAT_0040c658[] = "rt";

// GLOBAL: FACEMAKER 0x0040c65c
char s_FaceButtons_txt_0040c65c[] = "FaceButtons.txt";

// GLOBAL: FACEMAKER 0x0040c66c
char s________c_0040c66c[] = "%[^\n]%*c";

// GLOBAL: FACEMAKER 0x0040c678
char s________c_0040c678[] = "%[^\n]%*c";

// GLOBAL: FACEMAKER 0x0040c684
char s________c_0040c684[] = "%[^\n]%*c";

// GLOBAL: FACEMAKER 0x0040c690
char s__d__d__d__d__d_0040c690[] = "%d %d,%d,%d,%d";

// GLOBAL: FACEMAKER 0x0040c6a0
char s_FaceArt_facebut1_pcx_0040c6a0[] = "FaceArt/facebut1.pcx";

// GLOBAL: FACEMAKER 0x0040c6b8
char s_FaceArt_facfrsh2New_pcx_0040c6b8[] = "FaceArt/facfrsh2New.pcx";

// GLOBAL: FACEMAKER 0x0040c6d0
char DAT_0040c6d0[] = "rt";

// GLOBAL: FACEMAKER 0x0040c6d4
char s_FaceData_txt_0040c6d4[] = "FaceData.txt";

#pragma optimize("gy", on)

// FUNCTION: FACEMAKER 0x004065f0
int FUN_004065f0(char *executable_name, char *config_name)
{
  (void)executable_name;
  if (config_name != (char *)0)
  {
    return FUN_00407b10(config_name);
  }
  return 0;
}

// FUNCTION: FACEMAKER 0x004065e0
int FUN_004065e0()
{
  return 0;
}

// FUNCTION: FACEMAKER 0x00406610
DIBSurface * FUN_00406610(void)
{
  return FUN_00406650();
}

// FUNCTION: FACEMAKER 0x00406620
DIBSurface * FUN_00406620(int page_number)
{
  if (page_number == 0)
  {
    return FUN_00406650();
  }
  return FUN_004069e0(page_number, DAT_0042656c, DAT_00426568, 8);
}

#pragma optimize("", off)

// FUNCTION: FACEMAKER 0x00402600
unsigned int FUN_00402600(int value)
{
  return (unsigned int)((global_screen_width * value) / 0x280);
}

// FUNCTION: FACEMAKER 0x00402621
void FUN_00402621(int *dst, int dst_x, int dst_y, int width, int height, int *src, int src_x,
                          int src_y)
{
  FUN_00407210(dst, FUN_00402600(dst_x), FUN_00402600(dst_y), FUN_00402600(width),
               FUN_00402600(height), src, FUN_00402600(src_x), FUN_00402600(src_y));
}

// FUNCTION: FACEMAKER 0x0040268a
void FUN_0040268a(int *dst, int x, int y, int src_page)
{
  int local_src_page;

  local_src_page = src_page;
  if (global_screen_width == 640)
  {
    FUN_00408da0(dst, x, y, src_page);
  }
  else
  {
    FUN_00408f20(dst, FUN_00402600(x), FUN_00402600(y),
                 FUN_00402600((int)*(short *)((char *)local_src_page + 4)),
                 FUN_00402600((int)*(short *)((char *)local_src_page + 6)), src_page);
  }
}

// FUNCTION: FACEMAKER 0x004093d0
int FUN_004093d0(void)
{
  return -1;
}

// FUNCTION: FACEMAKER 0x0040369c
int FUN_0040369c(void)
{
  FaceInitStack local;

  local.control_slot = 0;
  local.button_text_ids[1] = 0xce;
  local.button_text_ids[2] = 0x3b;
  local.button_text_ids[3] = 0x3b;
  local.button_text_ids[4] = 0x3b;
  local.style_values[0] = 10;
  local.style_values[1] = 10;
  local.style_values[2] = 8;
  local.style_values[3] = 10;
  local.button_text_ids[0] = 2;

  local.file = fopen(s_FaceButtons_txt_0040c65c, DAT_0040c658);
  fscanf(local.file, s________c_0040c66c, DAT_0041afc0);
  fscanf(local.file, s________c_0040c678, DAT_0041af50);
  for (local.loop_index = 0; local.loop_index < 4; local.loop_index = local.loop_index + 1)
  {
    fscanf(local.file, s________c_0040c684, DAT_0041e420 + local.loop_index * 0x20);
  }

  fscanf(local.file, s__d__d__d__d__d_0040c690, local.button_text_ids, local.style_values,
         local.style_values + 1, local.style_values + 2, local.style_values + 3);
  fclose(local.file);

  FUN_00408900();
  FUN_0040a130(2, s_FaceArt_facebut1_pcx_0040c6a0);
  *(int *)((char *)PTR_DAT_0040c0ac + 0x20) = local.button_text_ids[0];

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    FUN_00407f30(local.button_text_ids[0], local.style_values[local.loop_index]);
    FUN_004088d0((int *)PTR_DAT_0040c0ac, local.button_text_ids[local.loop_index + 1],
                 local.face_slot_x + 0x2b, local.style_values[5] + 0xe, DAT_0041e420 + 0x20);
    DAT_0041e4e0[local.loop_index] =
        FUN_00408a50(2, local.face_slot_x, local.style_values[5], 0x56, 0x21);
    DAT_0040c1e8[1].page_by_mode[local.loop_index] = DAT_0041e4e0[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    FUN_00407f30(local.button_text_ids[0], local.style_values[local.loop_index]);
    FUN_004088d0((int *)PTR_DAT_0040c0ac, local.button_text_ids[local.loop_index + 1],
                 local.face_slot_x + 0x2b, local.style_values[5] + 0xe, DAT_0041e420);
    DAT_0041e510[local.loop_index] =
        FUN_00408a50(2, local.face_slot_x, local.style_values[5], 0x56, 0x21);
    DAT_0040c1e8[2].page_by_mode[local.loop_index] = DAT_0041e510[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    DAT_0041e400[local.loop_index] =
        FUN_00408a50(2, local.face_slot_x, local.style_values[5], 0x25, 0x1d);
    for (local.style_values[4] = 5; (int)local.style_values[4] < 0x14;
         local.style_values[4] = local.style_values[4] + 2)
    {
      DAT_0040c1e8[local.style_values[4]].page_by_mode[local.loop_index] = DAT_0041e400[local.loop_index];
    }
  }

  for (local.loop_index = 0; local.loop_index < 4;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    DAT_0041e3f0[local.loop_index] =
        FUN_00408a50(2, local.face_slot_x, local.style_values[5], 0x25, 0x1d);
    for (local.style_values[4] = 6; (int)local.style_values[4] < 0x16;
         local.style_values[4] = local.style_values[4] + 2)
    {
      DAT_0040c1e8[local.style_values[4]].page_by_mode[local.loop_index] = DAT_0041e3f0[local.loop_index];
    }
  }

  for (local.loop_index = 0; local.loop_index < 3;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    DAT_0041e500[local.loop_index] =
        FUN_00408a50(2, local.face_slot_x, local.style_values[5], 0x40, 0x20);
    DAT_0040c1e8[0].page_by_mode[local.loop_index] = DAT_0041e500[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 2;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    FUN_00407f30(local.button_text_ids[0], local.style_values[local.loop_index]);
    FUN_004088d0((int *)PTR_DAT_0040c0ac, local.button_text_ids[local.loop_index + 1],
                 local.face_slot_x + 0x20, local.style_values[5] + 0xe, DAT_0041e420 + 0x40);
    DAT_0041e558[local.loop_index] =
        FUN_00408a50(2, local.face_slot_x, local.style_values[5], 0x40, 0x1d);
    DAT_0040c1e8[3].page_by_mode[local.loop_index] = DAT_0041e558[local.loop_index];
  }

  for (local.loop_index = 0; local.loop_index < 2;
       local.loop_index = local.loop_index + 1, local.control_slot = local.control_slot + 1)
  {
    local.face_slot_x = (local.control_slot % 5) * 0x70;
    local.style_values[5] = (local.control_slot / 5) * 0x60;
    FUN_00407f30(local.button_text_ids[0], local.style_values[local.loop_index]);
    FUN_004088d0((int *)PTR_DAT_0040c0ac, local.button_text_ids[local.loop_index + 1],
                 local.face_slot_x + 0x20, local.style_values[5] + 0xe, DAT_0041e420 + 0x60);
    DAT_0041e520[local.loop_index] =
        FUN_00408a50(2, local.face_slot_x, local.style_values[5], 0x40, 0x1d);
    DAT_0040c1e8[4].page_by_mode[local.loop_index] = DAT_0041e520[local.loop_index];
  }

  local.face_slot_x = (local.control_slot % 5) * 0x70;
  local.style_values[5] = (local.control_slot / 5) * 0x60;
  local.control_slot = local.control_slot + 1;
  DAT_0041e4a0 = FUN_00408a50(2, local.face_slot_x, local.style_values[5], 9, 0x1d);

  local.face_slot_x = (local.control_slot % 5) * 0x70;
  local.style_values[5] = (local.control_slot / 5) * 0x60;
  local.control_slot = local.control_slot + 1;
  DAT_0041e4a4 = FUN_00408a50(2, local.face_slot_x, local.style_values[5], 3, 0x1d);

  FUN_0040a130(2, s_FaceArt_facfrsh2New_pcx_0040c6b8);
  DAT_0041e4dc = FUN_00408a50(2, 0, 0, 0x8a, 0xac);
  FUN_00408920();

  local.file = fopen(s_FaceData_txt_0040c6d4, DAT_0040c6d0);
  local.face_slot_x = _fileno(local.file);
  local.style_values[5] = _filelength(local.face_slot_x);
  local.scan_ptr = malloc(local.style_values[5]);
  DAT_0041d120[0] = local.scan_ptr;
  local.scan_ptr = DAT_0041d120[0];
  fread(local.scan_ptr, local.style_values[5], 1, local.file);
  fclose(local.file);

  DAT_0040c1e0 = 1;
  DAT_0041d120[0] = local.scan_ptr;
  local.scan_ptr = local.scan_ptr + 1;
  for (local.loop_index = 1; local.loop_index < (int)local.style_values[5];
       local.loop_index = local.loop_index + 1)
  {
    if (*local.scan_ptr == '\n')
    {
      *local.scan_ptr = '\0';
      if (local.scan_ptr[1] != '\n')
      {
        DAT_0041d120[DAT_0040c1e0] = local.scan_ptr + 1;
        DAT_0040c1e0 = DAT_0040c1e0 + 1;
      }
    }
    local.scan_ptr = local.scan_ptr + 1;
  }

  DAT_0040c1e0 = DAT_0040c1e0 - 1;
  for (local.loop_index = 0; local.loop_index < DAT_0040c1e0; local.loop_index = local.loop_index + 1)
  {
    local.scan_ptr = DAT_0041d120[local.loop_index];
    while (*local.scan_ptr != ',')
    {
      local.scan_ptr = local.scan_ptr + 1;
    }
    *local.scan_ptr = '\0';
    for (;;)
    {
      local.scan_ptr = local.scan_ptr + 1;
      if (*local.scan_ptr != ' ')
      {
        if (*local.scan_ptr != '\t')
        {
          break;
        }
      }
    }
    DAT_0041e570[local.loop_index] = local.scan_ptr;
    while (*local.scan_ptr != ',' && *local.scan_ptr != ' ' && *local.scan_ptr != '\t')
    {
      local.scan_ptr = local.scan_ptr + 1;
    }
    local.char_temp = *local.scan_ptr;
    *local.scan_ptr = '\0';
    for (;;)
    {
      local.scan_ptr = local.scan_ptr + 1;
      if (*local.scan_ptr != ' ')
      {
        if (*local.scan_ptr != '\t')
        {
          break;
        }
      }
    }
    DAT_0041c150[local.loop_index] = local.scan_ptr;
  }

  return 0;
}

// FUNCTION: FACEMAKER 0x00402a84
int FUN_00402a84(FaceControl *control)
{
  int old_mode;

  old_mode = control->state;
  DAT_0041e560 = 1;
  control->state = 0;
  FUN_004028d1(control, 3);
  DAT_0041e560 = 0;
  control->state = 3;
  return old_mode;
}

// FUNCTION: FACEMAKER 0x00402ad9
int FUN_00402ad9(FaceControl *control)
{
  int old_mode;

  old_mode = control->state;
  control->state = 0;
  DAT_0041e560 = 1;
  FUN_004028d1(control, 0);
  DAT_0041e560 = 0;
  return old_mode;
}

// FUNCTION: FACEMAKER 0x004028d1
int FUN_004028d1(FaceControl *control, int mode)
{
  int in_bounds;

  if (DAT_0041e560 == 0)
  {
    if (DAT_0042333c < (int)FUN_00402600(control->x) ||
        (int)FUN_00402600(control->width + control->x) < DAT_0042333c)
    {
      in_bounds = 0;
    }
    else if (DAT_00423338 < (int)FUN_00402600(control->y) ||
             (int)FUN_00402600(control->y + control->height) < DAT_00423338)
    {
      in_bounds = 0;
    }
    else
    {
      in_bounds = 1;
    }
    if (!in_bounds)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  if (control->page_by_mode[mode] != 0)
  {
    FUN_0040268a((int *)PTR_DAT_0040c034, control->x, control->y, control->page_by_mode[mode]);
  }

  if (control->page_by_mode[mode] != 0)
  {
    FUN_0040268a((int *)PTR_DAT_0040c05c, control->x, control->y, control->page_by_mode[mode]);
  }

  if (mode == 2 && control->on_activate != (FaceControlCallback)0)
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: FACEMAKER 0x004032e4
void FUN_004032e4(void)
{
  int face_index;

  FUN_0040268a((int *)PTR_DAT_0040c05c, 0xd, 0x24, DAT_0041e4dc);
  FUN_0040268a((int *)PTR_DAT_0040c05c, 0xd, 0x24, *(int *)DAT_0041d0f0);
  for (face_index = 0; face_index < *(volatile int *)&DAT_0041e410; face_index++)
  {
    if (DAT_0041e4b0[face_index] >= 0 &&
        *(int *)((char *)DAT_0041e0d0 + DAT_0041e4b0[face_index] * 4 + face_index * 0x50) != 0)
    {
      FUN_0040268a((int *)PTR_DAT_0040c05c, 0xd, 0x24,
                   *(int *)((char *)DAT_0041e0d0 + DAT_0041e4b0[face_index] * 4 + face_index * 0x50));
    }
  }

  FUN_00402621((int *)PTR_DAT_0040c05c, 0xd, 0x24, 0x89, 0xa9, (int *)PTR_DAT_0040c034, 0xd, 0x24);
}

// FUNCTION: FACEMAKER 0x00403573
void FUN_00403573(int index, int selected)
{
  int face_slot_x;
  int face_slot_y;

  face_slot_x = ((index + 2) % 4) * 0x9f + 0xd;
  face_slot_y = ((index + 2) / 4) * 0xdf + 0x24;
  FUN_0040268a((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y, DAT_0041e4dc);
  if (DAT_0041e4b0[index + DAT_0041e0c0] >= 0 &&
      *(int *)((char *)DAT_0041e0d0 + DAT_0041e4b0[index + DAT_0041e0c0] * 4 +
               (index + DAT_0041e0c0) * 0x50) != 0)
  {
    FUN_0040268a((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y,
                 *(int *)((char *)DAT_0041e0d0 + DAT_0041e4b0[index + DAT_0041e0c0] * 4 +
                          (index + DAT_0041e0c0) * 0x50));
  }

  FUN_00402621((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y, 0x89, 0xab, (int *)PTR_DAT_0040c034,
               face_slot_x, face_slot_y);
  if (selected != 0)
  {
    FUN_004032e4();
  }
}

// FUNCTION: FACEMAKER 0x00403fd9
DWORD WINAPI FUN_00403fd9(LPVOID unused)
{
  char *last_backslash;
  int page_count;
  int page_number;
  int face_index;
  int face_slot_x;
  int face_slot_y;
  int visible_face_count;
  int button_index;
  DIBSurface *page;
  unsigned int sign_bits;
  FILE *recent_face_file;
  char recent_face_name[256];
  char face_art_path[260];

  (void)unused;

  GetModuleFileNameA((HMODULE)0, DAT_0041ae40, 0x105);
  SetCurrentDirectoryA(DAT_0041ae40);
  last_backslash = strrchr(DAT_0041ae40, '\\');
  *last_backslash = '\0';
  sprintf(DAT_0041b030, "%s\\PlayFace\\*.pic", DAT_0041ae40);
  FUN_004065e0(FUN_004065f0("mgraphic.exe", "fonts.cv"));
  FUN_00406610();

  page_count = 3;
  for (page_number = 0; page_number < page_count; ++page_number)
  {
    if (page_number == 1 && DAT_00426540[0]->width < 800)
    {
      page = FUN_004069e0(page_number, 800, 600, 8);
    }
    else
    {
      page = FUN_00406620(page_number);
    }
    FUN_00406bd0(page_number, page);
  }

  PTR_DAT_0040c0ac->max_x = global_screen_width;
  PTR_DAT_0040c05c->max_x = PTR_DAT_0040c0ac->max_x;
  PTR_DAT_0040c034->max_x = PTR_DAT_0040c05c->max_x;
  PTR_DAT_0040c0ac->max_y = global_screen_height;
  PTR_DAT_0040c05c->max_y = PTR_DAT_0040c0ac->max_y;
  PTR_DAT_0040c034->max_y = PTR_DAT_0040c05c->max_y;
  DAT_0040d088 = 0;

  if (global_screen_width == 640)
  {
    FUN_00407e20(1, 0xb, "tt0300m_.ttf", "MPZurich Cn BT", 400, 0);
    FUN_00407e20(2, 9, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    FUN_00407e20(4, FUN_00402600(10), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(5, FUN_00402600(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    FUN_00407e20(6, FUN_00402600(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(7, FUN_00402600(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }
  else if (global_screen_width == 800)
  {
    FUN_00407e20(1, FUN_00402600(8), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(2, 9, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    FUN_00407e20(4, FUN_00402600(10), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(5, FUN_00402600(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    FUN_00407e20(6, FUN_00402600(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(7, FUN_00402600(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }
  else if (global_screen_width == 1024)
  {
    FUN_00407e20(1, FUN_00402600(9), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(2, 10, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    FUN_00407e20(4, FUN_00402600(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(5, FUN_00402600(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    FUN_00407e20(6, FUN_00402600(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    FUN_00407e20(7, FUN_00402600(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }

  *(int *)((char *)PTR_DAT_0040c034 + 0x20) = 1;
  FUN_00406cc0(0);
  DAT_0040c188 = FUN_004093d0();
  FUN_0040a110(2, "FaceArt/facebk0New.pcx");
  FUN_00407570(PTR_DAT_0040c0ac, 0, 0, 0x280, 0x1e0, PTR_DAT_0040c05c, 0, 0, global_screen_width,
               global_screen_height);
  FUN_0040369c();

  recent_face_file = fopen("FaceMostRecent.txt", "rt");
  if (recent_face_file != (FILE *)0)
  {
    fscanf(recent_face_file, "%d, %s", &DAT_0040c1e4, recent_face_name);
    if (_stricmp(DAT_0041e570[DAT_0040c1e4], recent_face_name) != 0)
    {
      DAT_0040c1e4 = 0;
    }
    fclose(recent_face_file);
  }

  strcpy(face_art_path, "FaceArt/");
  strcat(face_art_path, DAT_0041e570[DAT_0040c1e4]);
  DAT_0041e410 = FUN_00404d70(face_art_path, 0x41e530, &DAT_0041e0d0, (int *)DAT_0041d0f0);
  FUN_00402a84(&DAT_0040c354);
  if (DAT_0041e410 < 7)
  {
    FUN_00402a84(&DAT_0040c388);
  }

  for (face_index = 0; face_index < 6; face_index++)
  {
    sign_bits = (int)(face_index + 2U) >> 0x1f;
    face_slot_x = ((((face_index + 2U ^ sign_bits) - sign_bits & 3U) ^ sign_bits) - sign_bits) * 0x9f +
                  0xd;
    face_slot_y = ((int)(face_index + 2 + ((face_index + 2 >> 0x1f) & 3U)) >> 2) * 0xdf + 0x24;
    FUN_0040268a((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y, DAT_0041e4dc);
    FUN_00402621((int *)PTR_DAT_0040c05c, face_slot_x, face_slot_y, 0x89, 0xa9,
                 (int *)PTR_DAT_0040c034, face_slot_x, face_slot_y);
  }

  face_index = 0;
  while (1)
  {
    visible_face_count = DAT_0041e410;
    if (5 < DAT_0041e410)
    {
      visible_face_count = 6;
    }
    if (visible_face_count <= face_index)
    {
      break;
    }
    FUN_00403573(face_index, 0);
    face_index++;
  }

  for (face_index = 0; face_index < 6; face_index++)
  {
    FUN_0040268a((int *)PTR_DAT_0040c05c, *(int *)((char *)&DAT_0040c190 + face_index * 8),
                 *(int *)((char *)&DAT_0040c194 + face_index * 8), DAT_0041e4a0);
  }

  for (face_index = 0; face_index < 4; face_index++)
  {
    FUN_0040268a((int *)PTR_DAT_0040c05c, *(int *)((char *)&DAT_0040c1c0 + face_index * 8),
                 ((int *)&DAT_0040c1c4)[face_index * 2], DAT_0041e4a0);
  }

  *(int *)PTR_DAT_0040c034 = 1;
  DAT_0041e560 = 1;
  button_index = 1;
  while (1)
  {
    visible_face_count = DAT_0041e410;
    if (5 < DAT_0041e410)
    {
      visible_face_count = 6;
    }
    if (visible_face_count * 2 + 9 <= button_index)
    {
      break;
    }
    FUN_004028d1(&DAT_0040c1e8[button_index], 0);
    button_index++;
  }

  while (button_index < 0x15)
  {
    FUN_004028d1(&DAT_0040c1e8[button_index], 3);
    button_index++;
  }

  FUN_00402a84(&DAT_0040c284);
  FUN_00402a84(&DAT_0040c2b8);
  DAT_0040c208 = 3;
  DAT_0041e560 = 0;
  *(int *)PTR_DAT_0040c034 = 0;
  FUN_00406cc0(1);
  FUN_004032e4();
  DAT_0041c144 = 1;

  return 0;
}

// FUNCTION: FACEMAKER 0x00405a49
LRESULT CALLBACK FUN_00405a49(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// FUNCTION: FACEMAKER 0x00405fa8
void FUN_00405fa8(void)
{
  HDC hdc;

  if (DAT_0040c9e4 == 0)
  {
    SetSystemPaletteUse(global_main_hdc, 1);
  }
  else
  {
    hdc = GetDC((HWND)0);
    SelectPalette(hdc, GetStockObject(15), FALSE);
    SetSystemPaletteUse(hdc, 1);
    RealizePalette(hdc);
    ReleaseDC((HWND)0, hdc);
  }
}

// FUNCTION: FACEMAKER 0x004061bd
LONG FUN_004061bd(DWORD width, DWORD height)
{
  DEVMODEA dev_mode;

  if (height == 0 && width == 0)
  {
    return ChangeDisplaySettingsA((DEVMODEA *)0, 0);
  }

  if (height == 0 || width == 0)
  {
    return -1;
  }

  dev_mode.dmSize = 0x9c;
  dev_mode.dmFields = 0x180000;
  dev_mode.dmPelsWidth = width;
  dev_mode.dmPelsHeight = height;
  return ChangeDisplaySettingsA(&dev_mode, 0);
}

// FUNCTION: FACEMAKER 0x0040623c
void FUN_0040623c(void)
{
  FUN_004061bd(0, 0);
}

// FUNCTION: FACEMAKER 0x00405510
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
  char *slash;
  DWORD thread_id;
  int horzres;
  unsigned int matched_mode;
  char module_path[256];
  MSG msg;
  int custom_mode_selected;
  WNDCLASSA wndclass;
  HWND main_hwnd;

  (void)prevInstance;

  custom_mode_selected = 0;
  DAT_0041b028 = FindWindowA("FaceMakerMainClass", (LPCSTR)0);
  if (DAT_0041b028 != (HWND)0)
  {
    ShowWindow(DAT_0041b028, nShowCmd);
    BringWindowToTop(DAT_0041b028);
    SetForegroundWindow(DAT_0041b028);
    return 0;
  }

  srand(GetTickCount());
  DAT_0041c140 = hInstance;
  wndclass.style = 0x23;
  wndclass.lpfnWndProc = FUN_00405a49;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = (HICON)0;
  wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  wndclass.hbrBackground = GetStockObject(4);
  wndclass.lpszMenuName = (LPCSTR)0;
  wndclass.lpszClassName = "FaceMakerMainClass";
  if (RegisterClassA(&wndclass) == 0)
  {
    MessageBoxA((HWND)0, "Couldn't register the classes", (LPCSTR)0, 0x1010);
    return 0;
  }

#ifdef MODERN_FIXES
  GetCurrentDirectoryA(0x100, DAT_0041b024);
#else
  // lmfao it's a miracle this doesn't crash (nBufferLength == 0 so the write to address 0x100 is elided)
  GetCurrentDirectoryA(DAT_0041b024[0], 0x100);
#endif

  strcpy(module_path, (*__p___argv())[0]);
  slash = strrchr(module_path, '\\');
  *slash = '\0';
  if (_chdir(module_path) != 0)
  {
    assert_noabort(0, "D:\\NewMagic\\Sources\\1player\\FaceMaker\\code\\test.c", 0xa2,
                   "Path Set FAILED");
  }

  atexit(FUN_0040623c);
  if (GetDeviceCaps(GetDC((HWND)0), HORZRES) > 1024)
  {
    FUN_004061bd(1024, 768);
  }

  while (*cmdLine != '\0' && cmdLine[1] != '\0')
  {
    matched_mode = 0;
    if (cmdLine[1] == 'S')
    {
      DAT_0040c9e4 = 0;
    }

    if (cmdLine[1] == '6')
    {
      global_screen_width = 640;
      global_screen_height = 480;
      FUN_004069e0(0, global_screen_width, global_screen_height, -1);
      custom_mode_selected = 1;
    }

    matched_mode = (unsigned int)(cmdLine[1] == '6');
    if (cmdLine[1] == '8')
    {
      global_screen_width = 800;
      global_screen_height = 600;
      FUN_004069e0(0, global_screen_width, global_screen_height, -1);
      matched_mode = 1;
      custom_mode_selected = 1;
    }

    if (cmdLine[1] == '1')
    {
      global_screen_width = 1024;
      global_screen_height = 768;
      FUN_004069e0(0, global_screen_width, global_screen_height, -1);
      matched_mode = 1;
      custom_mode_selected = 1;
    }

    if (matched_mode != 0)
    {
      PTR_DAT_0040c034->max_x = global_screen_width - 1;
      PTR_DAT_0040c034->max_y = global_screen_height - 1;
      break;
    }

    *cmdLine = '\0';
  }

  if (matched_mode == 0)
  {
    horzres = GetDeviceCaps(GetDC((HWND)0), HORZRES);
    switch (horzres)
    {
    case 640:
      global_screen_width = 640;
      global_screen_height = 480;
      break;
    case 800:
      global_screen_width = 800;
      global_screen_height = 600;
      break;
    case 1024:
      global_screen_width = 1024;
      global_screen_height = 768;
      break;
    }

    PTR_DAT_0040c034->max_x = global_screen_width - 1;
    PTR_DAT_0040c034->max_y = global_screen_height - 1;
  }

  DAT_0041b028 = CreateWindowExA(-(unsigned int)(DAT_0040c9e4 == 0) & 8, "FacemakerMainClass",
                                 "Magic: Facemaker", 0x80000000, 0, 0, global_screen_width,
                                 global_screen_height, (HWND)0, (HMENU)0, hInstance, (LPVOID)0);
  main_hwnd = DAT_0041b028;
  ShowWindow(DAT_0041b028, nShowCmd);
  global_main_hdc = GetDC(main_hwnd);
  BringWindowToTop(DAT_0041b028);
  SetForegroundWindow(DAT_0041b028);
  SetFocus(DAT_0041b028);
  if (custom_mode_selected != 0)
  {
    DAT_00426540[0]->hTempDC = global_main_hdc;
    SelectPalette(DAT_00426540[0]->hTempDC, DAT_00426540[0]->hPalette, FALSE);
    RealizePalette(DAT_00426540[0]->hTempDC);
    SetStretchBltMode(DAT_00426540[0]->hTempDC, 3);
  }

  SetSystemPaletteUse(global_main_hdc, 2);
  atexit(FUN_00405fa8);
  CreateThread((LPSECURITY_ATTRIBUTES)0, 0, FUN_00403fd9, (LPVOID)0, 0, &thread_id);
  while (GetMessageA(&msg, (HWND)0, 0, 0) != 0)
  {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }

  GdiFlush();
  return DAT_0041e564;
}
