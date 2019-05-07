#ifndef CLONE_TERMINAL_H
#define CLONE_TERMINAL_H

#define WIN_X 90
#define WIN_Y 42

typedef struct
{
    unsigned int C_X;
    unsigned int C_Y;
} cursor_t;

//cursor coordinates
cursor_t cursor;

void clear_term();

void editorDrawRows();

void moveCursor();

void increment_cursor();

void cursor_to_top();

void cursor_to_bottom_left();

void cursor_to_location(int x, int y);

void print_cursor();

void initEditor();

#endif //CLONE_TERMINAL_H
