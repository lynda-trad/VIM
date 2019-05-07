#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include "mode.h"
#include "editor.h"
#include "terminal.h"

void cursor_to_top()
{
    write(STDOUT_FILENO, "\x1b[H", 3);
    //write(STDOUT_FILENO, "\033[1;1f", 9);
}

void cursor_to_bottom_left()
{
    write(STDOUT_FILENO, "\033[42;1f", 9);
    //write(STDOUT_FILENO, "\x1b[0B\x1b[999B", 12);
    //write(STDOUT_FILENO, "\x1b[00\x1b[999B", 12);
}

void cursor_to_location(int x, int y)
{
    char buf[32];
    sprintf(buf, "\x1b[%d;%dH",x ,y);
}

void print_cursor()
{
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", cursor.C_Y + 1, cursor.C_X + 1);
}

void moveCursor()
{
    int i;
    char troisieme;
    char k1;

    for (i = 0; i < 2; ++i)
    {
        read(STDIN_FILENO, &k1, 1);
        if(i == 1)
            troisieme = k1;
    }

    switch(troisieme)
    {
        case 'A':
            write(STDOUT_FILENO,"up\n",3);
            if (cursor.C_Y != 0)
                cursor.C_Y--;
            break;
        case 'B':
            write(STDOUT_FILENO,"down\n",5);
            if (cursor.C_Y != WIN_Y - 1)
                cursor.C_Y++;
            break;
        case 'C':
            write(STDOUT_FILENO,"right\n",6);
            if (cursor.C_X != WIN_X - 1)
                cursor.C_X++;
            break;
        case 'D':
            write(STDOUT_FILENO,"left\n",5);
            if (cursor.C_X != 0)
                cursor.C_X--;
            break;
        default :
            read(STDIN_FILENO, &k1, 1);
            if( k1 == '\0')
                change_mode(troisieme, &current_mode);
            break;
    }
}

void increment_cursor()
{
    if (cursor.C_X != WIN_X - 1)
        cursor.C_X++;
    else
    {
        cursor.C_X = 0;
        cursor.C_Y++;
    }
}

void clear_term()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    cursor_to_top();
}

void editorDrawRows()
{
    //draws the line of tildes like vim
    int y;

    for (y = 0; y < WIN_Y-2; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void initEditor()
{
    cursor.C_X = 0;
    cursor.C_Y = 0;
}