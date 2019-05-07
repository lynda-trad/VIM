#ifndef CLONE_EDITOR_H
#define CLONE_EDITOR_H

#include <termios.h>

#define WIN_X 90
#define WIN_Y 40
#define BUFF_SIZE 1024

//Abscisse (colonnes)
int CUR_X;

//Ordonnée (lignes)
int CUR_Y;


void die(const char *s);

void disableRawMode();

void enableRawMode();

void clear_term();

void editorDrawRows();

void cursor_to_top_left();

void cursor_to_bottom_left();

void cursor_to_left(char* buffer);

void cursor_to_bottom(char* buffer);

void cursor_to_top(char* buffer);

void cursor_to_right(char* buffer);

void color_cursor(char* buffer);

void moveCursorBuf(char* buffer);

void moveCursor();

void cmd_key_pressed_buf( char* buffer, char key);

void cmd_key_pressed(char key);

char *get_file(const char *path);

unsigned int get_amount_lines(const char *s);

void print_file(const char *s, unsigned int n);

#endif //CLONE_EDITOR_H
