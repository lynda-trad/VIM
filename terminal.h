#ifndef CLONE_TERMINAL_H
#define CLONE_TERMINAL_H

#define WIN_X 90
#define WIN_Y 42

typedef struct
{
     int C_X;
     int C_Y;
} cursor_t;

//cursor coordinates
cursor_t cursor;

void initEditor();

void clear_term();

void editorDrawRows();

void cursor_to_top_left();

void cursor_to_bottom_left();

void cursor_to_location(int x, int y);

void print_cursor();

void moveCursor();

void increment_cursor();

void cursor_to_top(char* buffer);

void cursor_to_bottom(char* buffer);

void cursor_to_right(char* buffer);

void cursor_to_left(char* buffer);

// void color_cursor(char* buffer);

void moveCursorBuf(char * buffer);



#endif //CLONE_TERMINAL_H
