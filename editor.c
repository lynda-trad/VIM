//
// Created by Lynda on 27/04/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

//window
#include <sys/ioctl.h>

#include "editor.h"

struct termios old_t;

void cursor_to_top()
{
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void cursor_to_bottom_left()
{
    //write(STDOUT_FILENO, "\x1b[0B\x1b[999B", 12);

    write(STDOUT_FILENO, "\x1b[00\x1b[999B", 12);
}

void moveCursor()
{	
	//enableRawMode();
	int i; 
	char troisieme, k1;

	for (i=0;i<3;i++)
	{
        read(STDIN_FILENO, &k1, 1);
	    if(i==2)
	        troisieme = k1;
	}
	
	switch(troisieme)
    {
		case 65:
            write(STDOUT_FILENO,"up\n",3);
            break;
		case 66:
            write(STDOUT_FILENO,"down\n",5);
            break;
		case 67:
            write(STDOUT_FILENO,"right\n",6);
            break;
		case 68:
            write(STDOUT_FILENO,"left\n",5);
            break;
    }
    //disableRawMode();
}

void die(const char *s)
{
    //error handler
    clear_term();
    perror(s);
    exit(1);
}

void disableRawMode()
{
    //disables raw mode
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_t) == -1)
        die("tcsetattr failed : disableRawMode()");
}

void enableRawMode()
{
    //enables raw mode
    tcgetattr(STDIN_FILENO, &old_t);
    atexit(disableRawMode);

    struct termios new_t = old_t;

    new_t.c_lflag &= ~(ICANON);
    new_t.c_lflag &= ~(ECHO);

    // disables CTRL+S and CTRL+Q
    new_t.c_iflag &= ~(IXON);

    // disables CTRL+M for now == '\n' in insertion mode
    new_t.c_iflag &= ~(ICRNL);

    // disables CTRL+V for now
    new_t.c_iflag &= ~(IEXTEN);

    //other flags
    new_t.c_iflag &= ~(BRKINT | INPCK | ISTRIP);
    new_t.c_cflag |= (CS8);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_t) == -1)
        die("tcsetattr failed : enableRawMode()");
}

void clear_term()
{
    //2 options
    printf("\033[H\033[2J");

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
