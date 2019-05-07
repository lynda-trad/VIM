//
// Created by lynda on 07/05/19.
//

#ifndef CLONE_TERMINAL_H
#define CLONE_TERMINAL_H

#define WIN_X 90
#define WIN_Y 42

struct cursor_t
{
    unsigned int C_X;
    unsigned int C_Y;
};

void clear_term();

void refresh_screen();

void editorDrawRows();

void moveCursor();

void cursor_to_top();

void cursor_to_bottom_left();

void cursor_to_location(int x, int y);

void print_cursor();


#endif //CLONE_TERMINAL_H
