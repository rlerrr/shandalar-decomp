#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"
#include "facemaker_types.h"

#pragma intrinsic(memcpy)

extern int LoadBitmapFontFromFile(int font_id, FILE *file);
extern int SetFontStyleSize(int font_id, unsigned int style);
extern int DrawTextFormatted(FacemakerWindowBounds *dst, int text_id, int draw_shadow, int scale_to_screen, int center_x, int center_y,
                             int x, int y, int *format_and_args);
extern void PutGraphicsPixel(FacemakerWindowBounds *window_bounds, int x, int y, unsigned int color_index);
extern void WriteGraphicsScanline(unsigned int *param_1, int param_2, int param_3, int param_4, unsigned int param_5);
extern char s_D__NewMagic__sources__sidlib__lib_c_0040d0ec[];
extern DIBSurface *g_graphics_pages[10];
extern PALETTEENTRY g_palette_entries[256];
extern RGBQUAD g_palette_rgb[256];
extern int g_graphics_bpp;
extern HDC global_main_hdc;

// GLOBAL: FACEMAKER 0x004199c8
// GLOBAL: SHANDALAR 0x0073a828
int *g_sprite_blob_base;

// GLOBAL: FACEMAKER 0x004199cc
// GLOBAL: SHANDALAR 0x0073a82c
int *g_sprite_blob_cursor;

// GLOBAL: FACEMAKER 0x004189b8
// GLOBAL: SHANDALAR 0x00739818
int g_resample_step_x;

// GLOBAL: FACEMAKER 0x004189bc
// GLOBAL: SHANDALAR 0x0073981c
int g_resample_step_y;

// GLOBAL: FACEMAKER 0x004189c0
// GLOBAL: SHANDALAR 0x00739820
int g_resample_clip_right;

// GLOBAL: FACEMAKER 0x004189c8
// GLOBAL: SHANDALAR 0x00739828
int g_resample_first_dst_x_by_src_x[0x400];

// GLOBAL: FACEMAKER 0x004199d0
int g_resample_source_height;

// GLOBAL: FACEMAKER 0x004199d4
int g_resample_accum_x;

// GLOBAL: FACEMAKER 0x004199d8
int g_resample_accum_y;

// GLOBAL: FACEMAKER 0x004199dc
int g_resample_cached_height;

// GLOBAL: FACEMAKER 0x004199e0
int g_resample_clip_left;

// GLOBAL: FACEMAKER 0x004199e4
int g_resample_cached_width;

// GLOBAL: FACEMAKER 0x004199e8
// GLOBAL: SHANDALAR 0x0073a848
int g_resample_src_x_for_dst_x[0x400];

// GLOBAL: FACEMAKER 0x0041a9e8
int g_resample_source_width;

// GLOBAL: FACEMAKER 0x00423344
int g_loaded_font_count;

// GLOBAL: FACEMAKER 0x004233d0
// GLOBAL: SHANDALAR 0x009837fc
FontSlot g_font_slots[0x10];

// GLOBAL: FACEMAKER 0x0040d224
char g_file_read_mode[] = "rb";

// GLOBAL: FACEMAKER 0x0040d228
char s_D__NewMagic__sources__sidlib__text_c_0040d228[] = "D:\\NewMagic\\sources\\sidlib\\text.c";

// GLOBAL: FACEMAKER 0x0040d200
char s_Can_not_load_more_than__d_fonts_0040d200[] = "Can not load more than %d fonts\n";

// GLOBAL: FACEMAKER 0x0040c0b4
char s_File__s_could_not_be_opened__EXITING_0040c0b4[] = "File %s could not be opened: EXITING\r\n";

// GLOBAL: FACEMAKER 0x0040c0b0
// GLOBAL: SHANDALAR 0x005861c4
char *PTR_s_File__s_could_not_be_opened__EXI_0040c0b0 = s_File__s_could_not_be_opened__EXITING_0040c0b4;

// FUNCTION: SHANDALAR 0x0057a4e0
// FUNCTION: FACEMAKER 0x00407b10
#pragma optimize("gty", on)
int LoadFontCollection(char *path)
{
    FILE *font_file;
    long font_offsets[16];
    int font_count;
    int font_index;

    font_count = 0;
    font_file = fopen(path, g_file_read_mode);
    assert((unsigned int)(font_file != (FILE *)0), s_D__NewMagic__sources__sidlib__text_c_0040d228, 0x83,
           PTR_s_File__s_could_not_be_opened__EXI_0040c0b0, path);
    
    fread(&font_count, 2, 1, font_file);
    g_loaded_font_count = font_count;
    assert((unsigned int)(font_count < 0x10), s_D__NewMagic__sources__sidlib__text_c_0040d228, 0x8a,
           s_Can_not_load_more_than__d_fonts_0040d200, 0x10);
    if (font_count > 0)
    {
        font_index = 1;
        do
        {
            font_offsets[font_index] = 0;
            fread(&font_offsets[font_index], 2, 1, font_file);
            font_index = font_index + 1;
        } while (font_index <= font_count);
    }

    font_index = 1;
    if (font_count > 0)
    {
        do
        {
            fseek(font_file, font_offsets[font_index], 0);
            LoadBitmapFontFromFile(font_index, font_file);
            font_index = font_index + 1;
        } while (font_index <= font_count);
    }

    fclose(font_file);
    return font_count;
}

// FUNCTION: SHANDALAR 0x0057a5f0
// FUNCTION: FACEMAKER 0x00407c20
int LoadBitmapFontFromFile(int font_id, FILE *file)
{
    int row_bytes;
    int bitmap_size;
    int remaining;
    unsigned int glyph_count;
    unsigned int height;
    unsigned int row_padding;
    unsigned int rows_remaining;
    FontSlot *font;
    unsigned char *bitmap_data;
    HBITMAP bitmap;

    font = &g_font_slots[font_id];
    fseek(file, -8, 1);
    fread(font, 1, 8, file);
    glyph_count = ((unsigned int)font->last_char - (unsigned int)font->first_char) + 1;
    if (font->has_packed_widths == '\0')
    {
        fseek(file, -8 - glyph_count, 1);
        fread(font->data.bitmap.glyph_advance + (unsigned int)font->first_char, 1, glyph_count, file);
        fseek(file, 8, 1);
    }

    height = (unsigned int)font->point_size;
    row_bytes = (unsigned int)font->glyph_width * glyph_count;
    row_padding = row_bytes & 1;
    font->row_padding = (unsigned char)row_padding;
    if (font->bitmap_data != (unsigned char *)0)
    {
        free(font->bitmap_data);
    }
    if (font->bitmap_normal != (HGDIOBJ)0)
    {
        DeleteObject(font->bitmap_normal);
    }
    if (font->bitmap_inverted != (HGDIOBJ)0)
    {
        DeleteObject(font->bitmap_inverted);
    }

    bitmap_size = (row_padding + row_bytes) * height;
    bitmap_data = malloc(bitmap_size + 0x2710);
    font->bitmap_data = bitmap_data;
    rows_remaining = height;
    while (rows_remaining != 0)
    {
        fread(bitmap_data, 1, row_bytes, file);
        bitmap_data = bitmap_data + row_padding + row_bytes;
        rows_remaining = rows_remaining - 1;
    }

    bitmap = CreateBitmap(row_bytes * 8, height, 1, 1, font->bitmap_data);
    font->bitmap_normal = bitmap;
    bitmap_data = font->bitmap_data;
    remaining = bitmap_size;
    while (remaining > 0)
    {
        *bitmap_data = (unsigned char)~*bitmap_data;
        bitmap_data = bitmap_data + 1;
        remaining = remaining - 1;
    }

    bitmap = CreateBitmap(row_bytes * 8, height, 1, 1, font->bitmap_data);
    font->bitmap_inverted = bitmap;
    bitmap_data = font->bitmap_data;
    while (bitmap_size > 0)
    {
        *bitmap_data = (unsigned char)~*bitmap_data;
        bitmap_data = bitmap_data + 1;
        bitmap_size = bitmap_size - 1;
    }

    if (font->hdc == (HDC)0)
    {
        font->hdc = CreateCompatibleDC((HDC)0);
    }
    font->font_loaded = 0;
    return 0;
}

// FUNCTION: SHANDALAR 0x0057a7f0
// FUNCTION: FACEMAKER 0x00407e20
int LoadSystemFont(int font_id, unsigned int point_size, char *font_file, char *font_name,
                         int weight, DWORD italic)
{
    char c;
    FontSlot *font;
    HFONT font_handle;
    HDC hdc;
    unsigned int ulen;
    unsigned int block_count;
    char *src;
    char *dst;
    unsigned int dword_count;
    TEXTMETRICA text_metrics;

    AddFontResourceA(font_file);
    font = &g_font_slots[font_id];
    font_handle = CreateFontA(100, 0, 0, 0, weight, italic, 0, 0, 0, 0, 0, 0, 0, font_name);
    font->hfont = font_handle;

    ulen = 0xffffffff;
    do
    {
        src = font_file;
        if (ulen == 0)
        {
            break;
        }
        ulen--;
        src = font_file + 1;
        c = *font_file;
        font_file = src;
    } while (c != '\0');
    ulen = ~ulen;
    src = src - ulen;
    dst = font->data.gdi.font_file;
    for (block_count = ulen >> 2; block_count != 0; block_count--)
    {
        *(unsigned int *)dst = *(unsigned int *)src;
        src += 4;
        dst += 4;
    }
    for (dword_count = ulen & 3; dword_count != 0; dword_count--)
    {
        *dst = *src;
        src++;
        dst++;
    }

    ulen = 0xffffffff;
    do
    {
        src = font_name;
        if (ulen == 0)
        {
            break;
        }
        ulen--;
        src = font_name + 1;
        c = *font_name;
        font_name = src;
    } while (c != '\0');
    ulen = ~ulen;
    src = src - ulen;
    dst = font->data.gdi.font_name;
    for (block_count = ulen >> 2; block_count != 0; block_count--)
    {
        *(unsigned int *)dst = *(unsigned int *)src;
        src += 4;
        dst += 4;
    }
    for (dword_count = ulen & 3; dword_count != 0; dword_count--)
    {
        *dst = *src;
        src++;
        dst++;
    }

    font->font_loaded = 1;

    hdc = GetDC((HWND)0);
    SelectObject(hdc, font->hfont);
    GetTextMetricsA(hdc, &text_metrics);
    ReleaseDC((HWND)0, hdc);
    font->tm_max = text_metrics.tmAscent;
    font->tm_min = text_metrics.tmInternalLeading;

    SetFontStyleSize(font_id, point_size);
    return 1;
}

// FUNCTION: SHANDALAR 0x0057a900
// FUNCTION: FACEMAKER 0x00407f30
int SetFontStyleSize(int font_id, unsigned int style)
{
    FontSlot *font;
    FARPROC import_proc;
    HFONT current_font;
    HDC hdc;
    TEXTMETRICA text_metrics;
    LOGFONTA log_font;

    font = &g_font_slots[font_id];
    if (font->font_loaded == 0)
    {
        return 0;
    }
    if ((unsigned int)font->point_size == style)
    {
        return (int)font->hfont;
    }

    current_font = font->hfont;
    import_proc = (FARPROC)GetObjectA;
    ((int(__stdcall *)(HANDLE, int, LPVOID))import_proc)(current_font, 0x3c, &log_font);
    DeleteObject(current_font);
    font->point_size = (unsigned char)style;
    log_font.lfHeight = (int)(style * 100) / (font->tm_max - font->tm_min);
    font->hfont = CreateFontIndirectA(&log_font);
    import_proc = (FARPROC)GetDC;
    hdc = ((HDC(__stdcall *)(HWND))import_proc)((HWND)0);
    SelectObject(hdc, font->hfont);
    GetTextMetricsA(hdc, &text_metrics);
    ReleaseDC((HWND)0, hdc);
    font->tm_leading = text_metrics.tmInternalLeading + text_metrics.tmExternalLeading;
    return (int)font->hfont;
}

// FUNCTION: SHANDALAR 0x0057bb90
// FUNCTION: FACEMAKER 0x00408da0
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image)
{
    int row_index;
    int page_number;
    DIBSurface *page;
    int row_stride;
    int sprite_row_count;
    int dst_row_ptr;
    int is_raw_span;
    int span_length;
    int draw_to_page;
    unsigned int span_offset;
    unsigned char source_pixel;
    unsigned char *span_data_bytes;
    int span_data_ptr;

    page_number = dst->page_number;
    if (encoded_image == (EncodedImage *)0)
    {
        return;
    }

    page = g_graphics_pages[page_number];
    y = y + (int)encoded_image->first_row;
    sprite_row_count = (int)encoded_image->row_count;
    span_data_ptr = (int)encoded_image->spans;
    row_stride = page->rowPadding + page->width;
    dst_row_ptr = (int)((char *)page->pBits + x + row_stride * y);
    draw_to_page = page_number;
    row_index = 0;
    if (sprite_row_count <= 0)
    {
        return;
    }

    do
    {
        is_raw_span = 0;
        span_offset = (unsigned int)*(unsigned char *)span_data_ptr;
        span_data_ptr = span_data_ptr + 1;
        if (span_offset != 0xff)
        {
            span_length = (int)*(unsigned char *)span_data_ptr;
            span_data_ptr = span_data_ptr + 1;
            if (span_length == 0xfe)
            {
                is_raw_span = 1;
                span_length = (int)*(unsigned char *)span_data_ptr;
                span_data_ptr = span_data_ptr + is_raw_span;
            }

            if (y + row_index < 0)
            {
                span_data_ptr = span_data_ptr + span_length;
            }
            else if (is_raw_span != 0)
            {
                if (draw_to_page != 0)
                {
                    span_data_bytes = (unsigned char *)span_data_ptr;
                    memcpy((void *)(dst_row_ptr + span_offset), span_data_bytes, (size_t)span_length);
                    span_data_ptr = (int)(span_data_bytes + span_length);
                }
                else
                {
                    span_data_bytes = (unsigned char *)span_data_ptr;
                    WriteGraphicsScanline((unsigned int *)span_data_bytes, draw_to_page, x + (int)span_offset,
                                 y + row_index, (unsigned int)span_length);
                    span_data_ptr = (int)(span_data_bytes + span_length);
                }
            }
            else if (draw_to_page != 0)
            {
                is_raw_span = 0;
                if (0 < span_length)
                {
                    do
                    {
                        source_pixel = *(unsigned char *)span_data_ptr;
                        if (source_pixel != '\0')
                        {
                            *(unsigned char *)(dst_row_ptr + span_offset + is_raw_span) = source_pixel;
                        }
                        is_raw_span = is_raw_span + 1;
                        span_data_ptr = span_data_ptr + 1;
                    } while (is_raw_span < span_length);
                }
            }
            else
            {
                is_raw_span = 0;
                if (0 < span_length)
                {
                    do
                    {
                        source_pixel = *(unsigned char *)span_data_ptr;
                        if (source_pixel != '\0')
                        {
                            PutGraphicsPixel(dst, x + (int)span_offset + is_raw_span, y + row_index,
                                         (unsigned int)source_pixel);
                        }
                        is_raw_span = is_raw_span + 1;
                        span_data_ptr = span_data_ptr + 1;
                    } while (is_raw_span < span_length);
                }
            }
        }

        row_index = row_index + 1;
        dst_row_ptr = dst_row_ptr + row_stride;
    } while (row_index < sprite_row_count);
}

// FUNCTION: SHANDALAR 0x0057bfa0
// FUNCTION: FACEMAKER 0x00408f20
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height,
                                       EncodedImage *encoded_image)
{
    short row_count;
    int page_number;
    int source_sprite_width;
    int source_sprite_height;
    int i;
    int j;
    int k;
    int row_stride;
    int row_base;
    int row_index;
    int top_clip;
    int run_start;
    int run_end;
    int source_row;
    int next_source_row;
    int row_delta;
    int draw_y;
    int mapping_index;
    int run_offset;
    unsigned int run_x;
    unsigned int run_length;
    unsigned int segment_length;
    unsigned char *row_data;
    unsigned char *next_row_data;
    unsigned char tmp_line[0x404];
    DIBSurface *surface;
    int *lookup_ptr;
    int use_raw_copy;

    page_number = dst->page_number;
    use_raw_copy = 0;
    if (encoded_image == (EncodedImage *)0)
    {
        return;
    }
    if (dst->max_x < x)
    {
        return;
    }
    if (dst->max_y < y)
    {
        return;
    }

    surface = g_graphics_pages[page_number];
    source_sprite_width = (int)encoded_image->width;
    source_sprite_height = (int)encoded_image->height;
    if ((g_resample_cached_width != width) || (g_resample_cached_height != height) ||
        (g_resample_source_width != source_sprite_width) || (g_resample_source_height != source_sprite_height))
    {
        g_resample_step_x = (source_sprite_width << 0x10) / width;
        g_resample_step_y = (source_sprite_height << 0x10) / height;
        for (i = 0; i < 0x400; i = i + 1)
        {
            g_resample_first_dst_x_by_src_x[i] = -1;
        }
        i = 0;
        g_resample_accum_x = 0;
        if (-1 < width + 2)
        {
            do
            {
                j = g_resample_accum_x >> 0x10;
                g_resample_src_x_for_dst_x[i] = j;
                if (g_resample_first_dst_x_by_src_x[j] == -1)
                {
                    g_resample_first_dst_x_by_src_x[j] = i;
                }
                i = i + 1;
                g_resample_accum_x = g_resample_accum_x + g_resample_step_x;
            } while (i <= width + 2);
        }
        if (width < source_sprite_width && -1 < source_sprite_width)
        {
            lookup_ptr = g_resample_first_dst_x_by_src_x;
            i = source_sprite_width + 1;
            do
            {
                if (*lookup_ptr == -1)
                {
                    *lookup_ptr = lookup_ptr[-1];
                }
                lookup_ptr = lookup_ptr + 1;
                i = i - 1;
            } while (i != 0);
        }
        g_resample_cached_width = width;
        g_resample_cached_height = height;
        g_resample_source_width = source_sprite_width;
        g_resample_source_height = source_sprite_height;
    }

    if (x < dst->clip_left)
    {
        g_resample_clip_left = dst->clip_left - x;
    }
    else
    {
        g_resample_clip_left = 0;
    }
    if (dst->max_x < x + width)
    {
        g_resample_clip_right = dst->max_x - x;
    }
    else
    {
        g_resample_clip_right = width;
    }

    top_clip = (int)encoded_image->first_row;
    g_resample_accum_y = 0;
    if (0 < top_clip)
    {
        do
        {
            g_resample_accum_y = g_resample_accum_y + g_resample_step_y;
            y = y + 1;
        } while (g_resample_accum_y >> 0x10 < top_clip);
    }

    row_data = (unsigned char *)encoded_image->spans;
    row_count = encoded_image->row_count;
    row_stride = surface->width + surface->rowPadding;
    row_index = 0;
    row_base = (int)surface->pBits + y * row_stride + x;
    if ((g_resample_accum_y >> 0x10) < row_count + top_clip)
    {
        do
        {
            source_row = g_resample_accum_y >> 0x10;
            next_row_data = row_data + 1;
            next_source_row = (g_resample_step_y + g_resample_accum_y) >> 0x10;
            run_x = (unsigned int)*row_data;
            if (run_x != 0xff)
            {
                segment_length = (unsigned int)*next_row_data;
                next_row_data = row_data + 2;
                use_raw_copy = segment_length == 0xfe;
                if (use_raw_copy)
                {
                    next_row_data = row_data + 3;
                    segment_length = (unsigned int)row_data[2];
                }
                draw_y = row_index + y;
    if (dst->clip_top <= draw_y)
                {
                    if (dst->max_y < draw_y)
                    {
                        return;
                    }
                    run_start = g_resample_first_dst_x_by_src_x[run_x];
                    if (run_start <= g_resample_clip_left)
                    {
                        run_start = g_resample_clip_left;
                    }
                    run_end = g_resample_first_dst_x_by_src_x[segment_length + run_x];
                    if (g_resample_clip_right <= g_resample_first_dst_x_by_src_x[segment_length + run_x])
                    {
                        run_end = g_resample_clip_right;
                    }
                    if (g_resample_src_x_for_dst_x[run_start] - (int)run_x < 0)
                    {
                        run_start = run_start + 1;
                    }
                    if (page_number == 0)
                    {
                        k = run_start;
                        if (use_raw_copy)
                        {
                            while (k < run_end)
                            {
                                tmp_line[k] = next_row_data[g_resample_src_x_for_dst_x[k] - run_x];
                                k = k + 1;
                            }
                            WriteGraphicsScanline((unsigned int *)(tmp_line + run_start), 0, x + run_start, draw_y,
                                         run_end - run_start);
                        }
                        else
                        {
                            while (run_start < run_end)
                            {
                                mapping_index = g_resample_src_x_for_dst_x[run_start] - run_x;
                                if (next_row_data[mapping_index] != 0)
                                {
                                    PutGraphicsPixel(dst, x + run_start, draw_y, (unsigned int)next_row_data[mapping_index]);
                                }
                                run_start = run_start + 1;
                            }
                        }
                    }
                    else
                    {
                        while (run_start < run_end)
                        {
                            mapping_index = g_resample_src_x_for_dst_x[run_start] - run_x;
                            if (next_row_data[mapping_index] != 0)
                            {
                                *(unsigned char *)(row_base + run_start) = next_row_data[mapping_index];
                            }
                            run_start = run_start + 1;
                        }
                    }
                }
                next_row_data = next_row_data + segment_length;
            }

            if (next_source_row != source_row)
            {
                row_delta = next_source_row - source_row;
                row_data = next_row_data;
                if (row_delta != 1)
                {
                    run_offset = row_delta - 2;
                    if (row_delta != 1)
                    {
                        do
                        {
                            row_data = next_row_data + 1;
                            if (*next_row_data != 0xff)
                            {
                                run_length = (unsigned int)*row_data;
                                row_data = next_row_data + 2;
                                if (run_length == 0xfe)
                                {
                                    run_length = (unsigned int)*row_data;
                                    row_data = next_row_data + 3;
                                }
                                row_data = row_data + run_length;
                            }
                            next_row_data = row_data;
                            run_offset = run_offset - 1;
                        } while (run_offset != -1);
                    }
                }
            }

            row_index = row_index + 1;
            g_resample_accum_y = g_resample_accum_y + g_resample_step_y;
            row_base = row_base + row_stride;
        } while ((g_resample_accum_y >> 0x10) < row_count + top_clip);
    }
}

// FUNCTION: SHANDALAR 0x0057aae0
// FUNCTION: FACEMAKER 0x004080d0
int MeasureMultilineTextWidth(FacemakerWindowBounds *param_1, char *param_2)
{
    char *line_ptr;
    char c;
    FontSlot *font;
    int line_width;
    int max_width;
    int char_width;
    HDC page_hdc;
    HGDIOBJ old_object;
    unsigned int ulen;
    ABC abc;

    line_width = 0;
    max_width = -1;
    font = &g_font_slots[param_1->font_slot];
    if (font->font_loaded != 0)
    {
        page_hdc = g_graphics_pages[param_1->page_number]->hTempDC;
        old_object = SelectObject(page_hdc, font->hfont);
        line_ptr = param_2;
        c = *param_2;
        while (c != '\0')
        {
            if (*param_2 == '\n')
            {
                ulen = 0xffffffff;
                *param_2 = '\0';
                do
                {
                    if (ulen == 0)
                    {
                        break;
                    }
                    ulen = ulen - 1;
                    c = *line_ptr;
                    line_ptr = line_ptr + 1;
                } while (c != '\0');
                GetTextExtentPoint32A(page_hdc, line_ptr - (~ulen), ~ulen - 1, (LPSIZE)&abc);
                if (max_width < abc.abcA + 2)
                {
                    max_width = abc.abcA + 2;
                }
                line_ptr = param_2 + 1;
                *param_2 = '\n';
            }
            param_2 = param_2 + 1;
            c = *param_2;
        }

        ulen = 0xffffffff;
        do
        {
            if (ulen == 0)
            {
                break;
            }
            ulen = ulen - 1;
            c = *line_ptr;
            line_ptr = line_ptr + 1;
        } while (c != '\0');
        GetTextExtentPoint32A(page_hdc, line_ptr - (~ulen), ~ulen - 1, (LPSIZE)&abc);
        if (max_width < abc.abcA + 2)
        {
            max_width = abc.abcA + 2;
        }
        SelectObject(page_hdc, old_object);
        return max_width;
    }

    c = *param_2;
    while (c != '\0')
    {
        c = *param_2;
        if (c == '\n')
        {
            if (max_width < line_width)
            {
                max_width = line_width;
            }
            line_width = 0;
        }
        else
        {
            if (font->font_loaded == 0)
            {
                if (font->has_packed_widths == 0)
                {
                    char_width = (int)font->data.bitmap.glyph_advance[(unsigned char)c] + (int)(unsigned char)font->unk_05;
                }
                else
                {
                    char_width = (int)font->has_packed_widths + (int)(unsigned char)font->unk_05;
                }
            }
            else
            {
                page_hdc = GetDC((HWND)0);
                SelectObject(page_hdc, font->hfont);
                GetCharABCWidthsA(page_hdc, (int)(unsigned char)c, (int)(unsigned char)c, &abc);
                ReleaseDC((HWND)0, page_hdc);
                char_width = abc.abcB + abc.abcC + abc.abcA;
            }
            line_width = line_width + char_width;
        }
        param_2 = param_2 + 1;
        c = *param_2;
    }
    if (max_width <= line_width)
    {
        max_width = line_width;
    }
    return max_width;
}

// FUNCTION: SHANDALAR 0x0057ae50
// FUNCTION: FACEMAKER 0x004082e0
int DrawTextLine(FacemakerWindowBounds *param_1, int param_2, int param_3, char *param_4)
{
  struct
  {
    union
    {
      int local_30;
      HDC hdc;
      char *line_ptr;
    } slot30;
    int char_width;
    unsigned int ch;
    HGDIOBJ old_page_object;
    unsigned int text_color;
    HDC page_hdc;
    HDC font_hdc;
    char *start;
    ABC abc;
  } local;
  int line_left;
  unsigned int ulen;

  line_left = param_2;
  local.start = param_4;
  if (*param_4 == '\0')
  {
    return 0;
  }
  if (param_1->clip_top <= param_3)
  {
    FontSlot *font;

    font = &g_font_slots[param_1->font_slot];
    if (font->font_loaded == 0)
    {
      if ((int)((unsigned int)font->point_size + (unsigned int)font->unk_06 + param_3) > param_1->max_y)
      {
        return 0;
      }
    }
    else if ((int)((unsigned int)font->point_size + font->tm_leading + param_3) > param_1->max_y)
    {
      return 0;
    }

    if (font->font_loaded != 0)
    {
      local.page_hdc = g_graphics_pages[param_1->page_number]->hTempDC;
      local.old_page_object = SelectObject(local.page_hdc, font->hfont);
      local.text_color = (unsigned int)param_1->text_color;
      if (0xfd < (int)local.text_color)
      {
        local.text_color = 0xfe;
      }
      SetTextColor(local.page_hdc, local.text_color & 0xffff | 0x1000000);
      SetBkMode(local.page_hdc, 1);
      ulen = strlen(param_4);
      TextOutA(local.page_hdc, param_2, param_3 - font->tm_leading, param_4, ulen);
      SelectObject(local.page_hdc, local.old_page_object);
#ifdef MODERN_FIXES
      GdiFlush();
#endif
      return 1;
    }

    local.page_hdc = g_graphics_pages[param_1->page_number]->hTempDC;
    local.font_hdc = font->hdc;
    SelectObject(local.font_hdc, font->bitmap_inverted);
    SetTextColor(local.page_hdc, 0x1000000);
    SetBkColor(local.page_hdc, 0xffffff);
    local.slot30.line_ptr = param_4;
    do
    {
      local.ch = (unsigned int)*local.slot30.line_ptr;
      if (font->font_loaded != 0)
      {
        local.slot30.hdc = GetDC((HWND)0);
        SelectObject(local.slot30.hdc, font->hfont);
        GetCharABCWidthsA(local.slot30.hdc, local.ch, local.ch, &local.abc);
        ReleaseDC((HWND)0, local.slot30.hdc);
        local.char_width = local.abc.abcB + local.abc.abcC + local.abc.abcA;
      }
      else if (font->has_packed_widths == 0)
      {
        local.char_width = (int)font->data.bitmap.glyph_advance[local.ch] + (int)(unsigned char)font->unk_05;
      }
      else
      {
        local.char_width = (int)font->has_packed_widths + (int)(unsigned char)font->unk_05;
      }
      local.slot30.line_ptr = local.slot30.line_ptr + 1;
      BitBlt(local.page_hdc, param_2, param_3, local.char_width, (unsigned int)font->point_size, local.font_hdc,
             (local.ch - (unsigned int)font->first_char) * (unsigned int)font->glyph_width * 8, 0, 0x8800c6);
      param_2 = param_2 + local.char_width;
    } while (*local.slot30.line_ptr != '\0');

    param_2 = line_left;
    local.text_color = 0xfe;
    if (param_1->text_color != 0xff)
    {
      local.text_color = (unsigned int)param_1->text_color;
    }
    SelectObject(local.font_hdc, font->bitmap_normal);
    SetTextColor(local.page_hdc, 0x1000000);
    SetBkColor(local.page_hdc, local.text_color & 0xffff | 0x1000000);
    do
    {
      local.ch = (unsigned int)*param_4;
      if (font->font_loaded != 0)
      {
        local.slot30.hdc = GetDC((HWND)0);
        SelectObject(local.slot30.hdc, font->hfont);
        GetCharABCWidthsA(local.slot30.hdc, local.ch, local.ch, &local.abc);
        ReleaseDC((HWND)0, local.slot30.hdc);
        local.char_width = local.abc.abcB + local.abc.abcC + local.abc.abcA;
      }
      else if (font->has_packed_widths == 0)
      {
        local.char_width = (int)font->data.bitmap.glyph_advance[local.ch] + (int)(unsigned char)font->unk_05;
      }
      else
      {
        local.char_width = (int)font->has_packed_widths + (int)(unsigned char)font->unk_05;
      }
      BitBlt(local.page_hdc, param_2, param_3, local.char_width, (unsigned int)font->point_size, local.font_hdc,
             (local.ch - (unsigned int)font->first_char) * (unsigned int)font->glyph_width * 8, 0, 0xee0086);
      param_2 = param_2 + local.char_width;
      param_4 = param_4 + 1;
    } while (*param_4 != '\0');
    return (int)param_4 - (int)local.start;
  }
  return 0;
}

/* Matching note: callers pass `&format` from their own variadic frame, so this
 * intentionally takes a stack-pack pointer instead of a C `...` signature. */
// FUNCTION: SHANDALAR 0x0057b230
// FUNCTION: FACEMAKER 0x004086c0
int DrawTextFormatted(FacemakerWindowBounds *dst, int text_color, int draw_shadow, int scale_to_screen, int center_x, int center_y,
                         int x, int y, int *format_and_args)
{
    char current_char;
    int formatted_length;
    char *scan_ptr;
    int line_x;
    int line_has_more;
    char *line_end;
    char *line_start;
    int line_count;
    int saved_color;
    volatile int stack_layout_pad;
    char formatted_text[1024];
    char *format_string;
    va_list args;

    format_string = (char *)*format_and_args;
    args = (va_list)(format_and_args + 1);
    formatted_length = _vsnprintf(formatted_text, sizeof(formatted_text) - 1, format_string, args);
    formatted_text[sizeof(formatted_text) - 1] = '\0';
    stack_layout_pad = formatted_length;

    scan_ptr = formatted_text;
    line_count = 0;
    current_char = *scan_ptr;
    while (current_char != '\0')
    {
        if (current_char == '\n')
        {
            line_count = line_count + 1;
        }
        scan_ptr = scan_ptr + 1;
        current_char = *scan_ptr;
    }
    if ((scan_ptr != formatted_text) && (scan_ptr[-1] != '\n'))
    {
        line_count = line_count + 1;
    }
    if (scale_to_screen != 0)
    {
        x = (g_graphics_pages[0]->width * x) / 0x280;
        y = (g_graphics_pages[0]->height * y) / 0x1e0;
    }
    if (center_y != 0)
    {
        y = y - ((int)g_font_slots[dst->font_slot].point_size * line_count) / 2;
    }
    if (-1 < text_color)
    {
        saved_color = dst->text_color;
        dst->text_color = text_color;
    }
    line_start = formatted_text;
    while (line_count != 0)
    {
        line_has_more = line_count != 0;
        line_count = line_count - 1;
        if (!line_has_more)
        {
            break;
        }
        current_char = *line_start;
        for (line_end = line_start; current_char != '\0' && *line_end != '\n'; line_end = line_end + 1)
        {
            current_char = line_end[1];
        }
        *line_end = '\0';
        line_x = x;
        if (center_x != 0)
        {
            line_x = MeasureMultilineTextWidth(dst, line_start);
            line_x = x - line_x / 2;
        }
        if (draw_shadow != 0)
        {
            saved_color = dst->text_color;
            dst->text_color = 0;
            DrawTextLine(dst, line_x + 1, y + 1, line_start);
            dst->text_color = saved_color;
        }
        DrawTextLine(dst, line_x, y, line_start);
        *line_end = '\n';
        line_start = line_end + 1;
        if (g_font_slots[dst->font_slot].font_loaded == 0)
        {
            y = y + (int)g_font_slots[dst->font_slot].point_size + (int)g_font_slots[dst->font_slot].unk_06;
        }
        else
        {
            y = y + (int)g_font_slots[dst->font_slot].point_size + g_font_slots[dst->font_slot].tm_leading;
        }
    }
    if (-1 < text_color)
    {
        dst->text_color = saved_color;
    }
    return formatted_length;
}

// FUNCTION: FACEMAKER 0x004088d0
// FUNCTION: SHANDALAR 0x0057b620
void DrawTextAt(FacemakerWindowBounds *dst, int text_id, int x, int y, char *text, ...)
{
#ifdef SHANDALAR
    DrawTextFormatted(dst, text_id, 1, 1, 1, 1, x, y, (int *)&text);
#else
    DrawTextFormatted(dst, text_id, 0, 0, 1, 1, x, y, (int *)&text);
#endif
}
#pragma optimize("", off)
