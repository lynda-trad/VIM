#ifndef CLONE_EDITOR_H
#define CLONE_EDITOR_H

#include <termios.h>

#define BUFF_SIZE 1024

typedef struct
{
    char *buff;
    unsigned int len; //où est le dernier caractere écrit
    unsigned int cur; //où se trouve le caractère qu'on est en train d'écrire
} buff_t;

buff_t writing_buff;

void disableRawMode();

void enableRawMode();

void cmd_key_pressed(char key);

void cmd_key_pressed_buf( char* buffer, char key);

char *get_file(const char *path);

unsigned int get_amount_lines(const char *s);

void print_file(const char *s, unsigned int n);

int write_to_file(const char *path, const char *str);

#endif //CLONE_EDITOR_H
