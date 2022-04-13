#include "screen.h"
#include "../cpu/ports.h"
#include "../libc/mem.h"

s32 get_cursor_offset();
void set_cursor_offset(s32 offset);
s32 print_char(s8 c, s32 col, s32 row, s8 attr);
s32 get_offset(s32 col, s32 row);
s32 get_offset_row(s32 offset);
s32 get_offset_col(s32 offset);

// Public API functions
void kprint_at(s8 *message, s32 col, s32 row) {
  s32 offset;
  if (col >= 0 && row >= 0) {
    offset = get_offset(col, row);
  } else {
    offset = get_cursor_offset();
    row = get_offset_row(offset);
    col = get_offset_col(offset);
  }

  s32 i = 0;
  while (message[i] != 0) {
    offset = print_char(message[i++], col, row, WHITE_ON_BLACK);

    row = get_offset_row(offset);
    col = get_offset_col(offset);
  }
}

void kprint(s8 *message) { kprint_at(message, -1, -1); }

void kprint_backspace() {
  int offset = get_cursor_offset() - 2;
  int row = get_offset_row(offset);
  int col = get_offset_col(offset);
  print_char(' ', col, row, WHITE_ON_BLACK);
  set_cursor_offset(offset);
}

void clear_screen() {
  s8 *screen = (s8 *)VIDEO_ADDRESS;

  s32 screen_size = MAX_COLS * MAX_ROWS;
  s32 i;
  for (i = 0; i < screen_size; i++) {
    screen[i * 2] = ' ';
    screen[i * 2 + 1] = WHITE_ON_BLACK;
  }

  set_cursor_offset(get_offset(0, 0));
}

// Private API Functions
s32 print_char(s8 c, s32 col, s32 row, s8 attr) {
  u8 *vidmem = (u8 *)VIDEO_ADDRESS;
  if (!attr)
    attr = WHITE_ON_BLACK;

  // Error control: print a red 'E' if the coords aren't right
  if (col >= MAX_COLS || row >= MAX_ROWS) {
    vidmem[2 * (MAX_COLS) * (MAX_ROWS)-2] = 'E';
    vidmem[2 * (MAX_COLS) * (MAX_ROWS)-1] = RED_ON_WHITE;
    return get_offset(col, row);
  }

  s32 offset;
  if (col >= 0 && row >= 0)
    offset = get_offset(col, row);
  else
    offset = get_cursor_offset();

  if (c == '\n') {
    row = get_offset_row(offset);
    offset = get_offset(0, row + 1);
  } else {
    vidmem[offset] = c;
    vidmem[offset + 1] = attr;
    offset += 2;
  }

  // Check if the offset is over screen size and scroll
  if (offset >= MAX_ROWS * MAX_COLS * 2) {
    s32 i;
    for (i = 1; i < MAX_ROWS; i++)
      memory_copy((u8 *)(get_offset(0, i) + VIDEO_ADDRESS),
                  (u8 *)(get_offset(0, i - 1) + VIDEO_ADDRESS), MAX_COLS * 2);

    char *last_line = (char *)(get_offset(0, MAX_ROWS - 1) + VIDEO_ADDRESS);
    for (i = 0; i < MAX_COLS * 2; i++)
      last_line[i] = 0;

    offset -= 2 * MAX_COLS;
  }

  set_cursor_offset(offset);
  return offset;
}

s32 get_cursor_offset() {
  port_byte_out(REG_SCREEN_CTRL, 14);

  s32 offset = port_byte_in(REG_SCREEN_DATA) << 8;

  port_byte_out(REG_SCREEN_CTRL, 15);
  offset += port_byte_in(REG_SCREEN_DATA);

  return offset * 2;
}

void set_cursor_offset(s32 offset) {
  offset = offset / 2;
  port_byte_out(REG_SCREEN_CTRL, 14);
  port_byte_out(REG_SCREEN_DATA, (offset >> 8));
  port_byte_out(REG_SCREEN_CTRL, 15);
  port_byte_out(REG_SCREEN_DATA, (offset & 0xff));
}

s32 get_offset(s32 col, s32 row) { return 2 * (row * MAX_COLS + col); }
s32 get_offset_row(s32 offset) { return offset / (2 * MAX_COLS); }
s32 get_offset_col(s32 offset) {
  return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2;
}
