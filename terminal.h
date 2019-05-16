#ifndef CLONE_TERMINAL_H
#define CLONE_TERMINAL_H

#include <termios.h>

#define WIN_X 120
#define WIN_Y 80

typedef struct
{
    unsigned int C_X; // Colonnes--> Max 90
    unsigned int C_Y; // Lignes--> Max 40
} cursor_t;

//cursor coordinates
cursor_t cursor;

struct termios old_t;

void disableRawMode();

void enableRawMode();

void clear_term();

void editorDrawRows();

void print_cursor();

void moveCursorBuf(char * buffer);

void increment_cursor();

void cursor_to_top_left();

void cursor_to_bottom_left();

void cursor_to_top(char* buffer);

void cursor_to_bottom(char* buffer);

void cursor_to_right(char* buffer);

void cursor_to_left(char* buffer);

void cursor_to_location_buf(unsigned int x, unsigned int y);

// void color_cursor(char* buffer);

#endif //CLONE_TERMINAL_H
