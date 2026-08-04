#include <stddef.h>

#define RPBITS_TABLE_ENTRIES 2048
#define RPBITS_TABLE_BYTES (RPBITS_TABLE_ENTRIES * 3)
#define RPBITS_SYMBOL_STACK_SIZE 4096

typedef struct RpBitsPalettePacket RpBitsPalettePacket;

extern int global_pcxw_image_width;
extern int global_pcxw_image_height;
extern unsigned char *rpbits_stream_ptr;
extern unsigned char *rpbits_stream_end;
extern int (*rpbits_stream_refill)(void);

void RpBits_ApplyPalette(RpBitsPalettePacket *palette_data);

static unsigned char decoder_table[RPBITS_TABLE_BYTES];
static unsigned char symbol_stack[RPBITS_SYMBOL_STACK_SIZE];
static int symbol_stack_count;

static unsigned int remaining_count;
static unsigned int code_mask;
static unsigned int saved_edx;
static unsigned int bit_buffer;
static unsigned int last_node_index;
static unsigned int DAT_10026550;

static unsigned char repeat_count;
static unsigned char last_literal;
static unsigned char code_bits;
static unsigned char max_code_bits;
static unsigned char bit_count;
static unsigned char packed_mode;
static unsigned char last_symbol;

static unsigned char *stream_limit;

static int refill_stream(void)
{
  int bytes_read;
  ptrdiff_t buffer_space;

  if (rpbits_stream_refill == 0)
  {
    stream_limit = rpbits_stream_ptr;
    return 0;
  }

  bytes_read = rpbits_stream_refill();
  if (bytes_read <= 0)
  {
    stream_limit = rpbits_stream_ptr;
    return 0;
  }

  buffer_space = rpbits_stream_end - rpbits_stream_ptr;
  if (buffer_space >= 0 && bytes_read > buffer_space)
  {
    bytes_read = (int)buffer_space;
  }

  stream_limit = rpbits_stream_ptr + bytes_read;
  return bytes_read;
}

static unsigned int read_stream_byte(void)
{
  if (stream_limit == 0)
  {
    stream_limit = rpbits_stream_end;
  }

  if (rpbits_stream_ptr >= stream_limit)
  {
    if (refill_stream() <= 0)
    {
      return 0;
    }
  }

  if (rpbits_stream_ptr >= stream_limit)
  {
    return 0;
  }

  return (unsigned int)*rpbits_stream_ptr++;
}

static unsigned int read_stream_word(void)
{
  unsigned int low_byte;
  unsigned int high_byte;

  low_byte = read_stream_byte();
  high_byte = read_stream_byte();
  return low_byte | (high_byte << 8);
}

static unsigned int decoder_table_get_word(unsigned int index)
{
  unsigned int table_offset;

  if (index >= RPBITS_TABLE_ENTRIES)
  {
    return 0xffff;
  }

  table_offset = index * 3;
  return (unsigned int)decoder_table[table_offset] |
         ((unsigned int)decoder_table[table_offset + 1] << 8);
}

static void decoder_table_set_word(unsigned int index, unsigned int value)
{
  unsigned int table_offset;

  if (index >= RPBITS_TABLE_ENTRIES)
  {
    return;
  }

  table_offset = index * 3;
  decoder_table[table_offset] = (unsigned char)value;
  decoder_table[table_offset + 1] = (unsigned char)(value >> 8);
}

static unsigned int decoder_table_get_symbol(unsigned int index)
{
  if (index >= RPBITS_TABLE_ENTRIES)
  {
    return 0;
  }

  return (unsigned int)decoder_table[index * 3 + 2];
}

static void decoder_table_set_symbol(unsigned int index, unsigned int value)
{
  if (index >= RPBITS_TABLE_ENTRIES)
  {
    return;
  }

  decoder_table[index * 3 + 2] = (unsigned char)value;
}

static void push_symbol(unsigned int symbol)
{
  if (symbol_stack_count < RPBITS_SYMBOL_STACK_SIZE)
  {
    symbol_stack[symbol_stack_count++] = (unsigned char)symbol;
  }
}

static int pop_symbol(void)
{
  if (symbol_stack_count <= 0)
  {
    return 0;
  }

  symbol_stack_count--;
  return (int)symbol_stack[symbol_stack_count];
}

static void write_stream_word_to_dest(unsigned char **dest, unsigned char *limit, unsigned int value)
{
  if (limit == 0 || *dest + 1 < limit)
  {
    (*dest)[0] = (unsigned char)value;
    (*dest)[1] = (unsigned char)(value >> 8);
  }

  *dest += 2;
}

void RpBits_ResetDecoder(void)
{
  unsigned int table_index;

  code_bits = 9;
  code_mask = 0x01ff;
  saved_edx = 0x100;

  for (table_index = 0; table_index < RPBITS_TABLE_ENTRIES; table_index++)
  {
    decoder_table_set_word(table_index, 0xffff);
  }

  for (table_index = 0; table_index < 0x100; table_index++)
  {
    decoder_table_set_symbol(table_index, table_index);
  }
}

void RpBits_InitStream(void)
{
  unsigned int initial_word;

  if ((global_pcxw_image_width | global_pcxw_image_height) == 0)
  {
    return;
  }

  repeat_count = 0;
  last_literal = 0;
  symbol_stack_count = 0;

  initial_word = read_stream_word();
  if ((initial_word & 0xff) > 0x0b)
  {
    initial_word = (initial_word & 0xff00) | 0x0b;
  }

  max_code_bits = (unsigned char)initial_word;
  bit_buffer = initial_word;
  bit_count = 8;

  RpBits_ResetDecoder();
}

int RpBits_DecodeSymbol(void)
{
  unsigned int buffered_bits;
  unsigned int available_bits;
  unsigned int code;
  unsigned int current_code;
  unsigned int walk_code;
  unsigned int prefix;
  unsigned int leaf_symbol;
  unsigned int next_code;
  unsigned int next_word;

  if (symbol_stack_count > 0)
  {
    return pop_symbol();
  }

  buffered_bits = bit_buffer >> (16 - bit_count);
  available_bits = bit_count;

  while (available_bits < code_bits)
  {
    next_word = read_stream_word();
    bit_buffer = next_word;
    buffered_bits |= next_word << available_bits;
    available_bits += 16;
  }

  available_bits -= code_bits;
  bit_count = (unsigned char)available_bits;
  code = buffered_bits & code_mask;

  next_code = saved_edx;
  current_code = code;
  walk_code = code;
  if (code >= next_code)
  {
    current_code = next_code;
    walk_code = last_node_index;
    push_symbol(last_symbol);
  }

  for (;;)
  {
    if (walk_code >= RPBITS_TABLE_ENTRIES)
    {
      return 0;
    }

    prefix = decoder_table_get_word(walk_code);
    if (prefix == 0xffff)
    {
      break;
    }

    push_symbol(decoder_table_get_symbol(walk_code));
    walk_code = prefix;
  }

  leaf_symbol = decoder_table_get_symbol(walk_code);
  last_symbol = (unsigned char)leaf_symbol;
  push_symbol(leaf_symbol);

  if (next_code < RPBITS_TABLE_ENTRIES)
  {
    decoder_table_set_symbol(next_code, leaf_symbol);
    decoder_table_set_word(next_code, last_node_index);
  }

  next_code++;
  if (next_code > code_mask)
  {
    code_bits++;
    code_mask = (code_mask << 1) | 1;
  }

  if (code_bits > max_code_bits)
  {
    RpBits_ResetDecoder();
    next_code = saved_edx;
  }
  else
  {
    saved_edx = next_code;
  }

  last_node_index = current_code;
  return pop_symbol();
}

static void decode_run_to_buffer(void *dst, int count)
{
  unsigned char *out;
  int output_bytes;
  int symbol;
  int packed_symbol_count;

  if (dst == 0 || count <= 0)
  {
    return;
  }

  out = (unsigned char *)dst;
  output_bytes = 0;

  if (packed_mode != 0)
  {
    packed_symbol_count = (count + 1) >> 1;
    remaining_count = (unsigned int)packed_symbol_count;
  }
  else
  {
    remaining_count = (unsigned int)count;
  }

  while (remaining_count != 0 && output_bytes < count)
  {
    if (repeat_count != 0)
    {
      symbol = (int)last_literal;
      repeat_count--;
    }
    else
    {
      symbol = RpBits_DecodeSymbol() & 0xff;
      if (symbol == 0x90)
      {
        symbol = RpBits_DecodeSymbol() & 0xff;
        if (symbol == 0)
        {
          symbol = 0x90;
          last_literal = (unsigned char)symbol;
        }
        else
        {
          repeat_count = (unsigned char)(symbol - 1);
          symbol = (int)last_literal;
          repeat_count--;
        }
      }
      else
      {
        last_literal = (unsigned char)symbol;
      }
    }

    if (packed_mode != 0)
    {
      out[output_bytes++] = (unsigned char)(symbol & 0x0f);
      if (output_bytes < count)
      {
        out[output_bytes++] = (unsigned char)((symbol >> 4) & 0x0f);
      }
    }
    else
    {
      out[output_bytes++] = (unsigned char)symbol;
    }

    remaining_count--;
  }
}

void RpBits_DecodeRun(void *dst, int count)
{
  decode_run_to_buffer(dst, count);
}

void RpBits_DecodeImage(void *dst, int count)
{
  decode_run_to_buffer(dst, count);
}

void RpBits_ReadTables(unsigned short *palette)
{
  unsigned int first_word;
  unsigned int second_word;
  unsigned int words_to_copy;
  unsigned char *dest;
  unsigned char *original_dest;
  unsigned char *dest_limit;

  stream_limit = rpbits_stream_end;

  for (;;)
  {
    first_word = read_stream_word();
    if ((first_word & 0xff) == 0x58)
    {
      packed_mode = (unsigned char)((first_word >> 8) & 1);
      DAT_10026550 = read_stream_word();
      global_pcxw_image_width = (int)read_stream_word();
      global_pcxw_image_height = (int)read_stream_word();
      RpBits_InitStream();
      return;
    }

    dest = decoder_table;
    if (first_word == 0x304d || first_word == 0x314d)
    {
      if (palette == 0)
      {
        dest = decoder_table + 2;
      }
      else if (palette != (unsigned short *)1)
      {
        dest = (unsigned char *)palette;
      }
    }

    original_dest = dest;
    if (dest >= decoder_table && dest < decoder_table + RPBITS_TABLE_BYTES)
    {
      dest_limit = decoder_table + RPBITS_TABLE_BYTES;
    }
    else
    {
      dest_limit = 0;
    }

    write_stream_word_to_dest(&dest, dest_limit, first_word);

    second_word = read_stream_word();
    write_stream_word_to_dest(&dest, dest_limit, second_word);

    words_to_copy = second_word >> 1;
    while (words_to_copy != 0)
    {
      write_stream_word_to_dest(&dest, dest_limit, read_stream_word());
      words_to_copy--;
    }

    if (original_dest == decoder_table)
    {
      RpBits_ApplyPalette((RpBitsPalettePacket *)decoder_table);
    }
  }
}
