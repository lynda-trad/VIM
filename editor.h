#ifndef CLONE_EDITOR_H
#define CLONE_EDITOR_H

#include <termios.h>

#define WIN_X 90
#define WIN_Y 40
#define BUFF_SIZE 1024

void die(const char *s);

void disableRawMode();

void enableRawMode();

void clear_term();

void refresh_screen();

void editorDrawRows();

void moveCursor();

void cursor_to_top();

void cursor_to_bottom_left();

void cursor_to_location(int x, int y);

void print_cursor();

void cmd_key_pressed(char key);

char *get_file(const char *path);

unsigned int get_amount_lines(const char *s);

void print_file(const char *s, unsigned int n);

int write_to_file(const char *path, const char *str);

#endif //CLONE_EDITOR_H
