#ifndef CLONE_EDITOR_H
#define CLONE_EDITOR_H

#include <termios.h>

#define BUFF_SIZE 1024

void die(const char *s);

void disableRawMode();

void enableRawMode();

void cmd_key_pressed(char key);

void cmd_key_pressed_buf( char* buffer, char key);

char *get_file(const char *path);

unsigned int get_amount_lines(const char *s);

void print_file(const char *s, unsigned int n);

int write_to_file(const char *path, const char *str);

#endif //CLONE_EDITOR_H
