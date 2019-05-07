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

//cursor coordinates
struct cursor_t cursor;

void cursor_to_top()
{
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void cursor_to_bottom_left()
{
    write(STDOUT_FILENO, "\033[90;0f", 9);
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
    char troisieme, k1;

    for (i=0;i<2;++i)
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
    }
}

void clear_term()
{
    //2 options
    printf("\033[H\033[2J");
    cursor_to_top();

    /*
    write(STDOUT_FILENO, "\x1b[2J", 4);
    cursor_to_top();
    */
}

void editorDrawRows()
{
    //draws the line of tildes like vim
    int y;

    for (y = 0; y < WIN_Y; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void refresh_screen()
{
    clear_term();
    editorDrawRows();
    cursor_to_top();
}
