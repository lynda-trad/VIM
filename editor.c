#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

//window
#include <sys/ioctl.h>

#include "mode.h"
#include "clone.h"
#include "editor.h"
#include "terminal.h"

struct termios old_t;

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

void cmd_key_pressed(char key)
{
    if (key == 27)
    {
        moveCursor();
    }
    else
    {
        if (iscntrl(key))
        { //need to code backspace with cursor
            write(STDOUT_FILENO,"\r\n",2);
        }
        else
        {
            write(STDOUT_FILENO,&key,1);
            increment_cursor();
        }
    }
}

void cmd_key_pressed_buf( char* buffer, char key)
{

    if (key == 27)
    {
        moveCursorBuf(buffer);

    }
    else
    {
        if (iscntrl(key))
        {
	   delete_character(key);
        }
        else
        {
                write(STDOUT_FILENO,&key,1);
        }
    }
}

//Ne marche pas, détecte pas BACKSPACE ou SUPPR
void delete_character(char key)
{
/*
 //A partir de la position courante, buffer[i]=buffer[i+1]
		for(unsigned int i= writing_buff.cur+1; i< writing_buff.len; i++){
			writing_buff.buff[i]= writing_buff.buff[i+1];
		}
		print_file(writing_buff.buff, get_amount_lines(writing_buff.buff));
	}
*/

if (iscntrl(key))
{
//key= SUPPR ou BACKSPACE
    if (key==8 || key==127)
    {
		char* new_buff= malloc(sizeof(writing_buff.buff)-1);
		unsigned int i;
// On copie tout ce qui est avant le curseur 
		for(i=0;i< writing_buff.cur;i++){
			new_buff[i]= writing_buff.buff[i];
		}
// On copie tout ce qui est juste après le curseur pour supprimer le caractère qui est actuellement dans le curseur
		for(unsigned int j= writing_buff.cur+1; j< writing_buff.len; j++){
			new_buff[i+1]= writing_buff.buff[j];
		}
		writing_buff.buff= new_buff;
		print_file(writing_buff.buff, get_amount_lines(writing_buff.buff));
		cursor.C_Y--;
	}

}
}
	


char *get_file(const char *path)
{
    char *ret;
    char buff[BUFF_SIZE];
    int all_r;
    int r;
    int fd;

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        die("open failed\n");
    }

    ret = malloc(BUFF_SIZE + 1);
    all_r = 0;

    while ( (r = (int)read(fd, buff, BUFF_SIZE)) > 0)
    {
        memcpy(&ret[all_r], buff, (size_t)r);
        all_r += r;
        ret = realloc(ret, (size_t)(all_r + BUFF_SIZE));
    }

    close(fd);

    ret[all_r] = '\0';
    writing_buff.len = all_r;
    return ret;
}

unsigned int get_amount_lines(const char *s)
{
    unsigned int i;
    unsigned int lines;
    unsigned int col;

    lines = 0;
    i = 0;
    col = 0;

    while (s[i]  && lines < WIN_Y)
    {
        if (s[i] == '\n' ||  col >= WIN_X)
        {
            col = 0;
            ++lines;
        }
        else
        {
            ++col;
        }
        ++i;
    }

    return i;
}

void print_file(const char *s, unsigned int n)
{
    unsigned int i;
    i = 0;

    while (s[i] && i < n)
    {
        if (s[i] == '\n')
        {
            printf("\r\n");
        }
        else
        {
            write(STDOUT_FILENO,&s[i],1);
        }
        ++i;
    }
}

int write_to_file(const char *path, const char *str)
{
    int fd;

    if ((fd = open(path, O_CREAT|O_RDWR|O_TRUNC)) > 0)
    {
        write(fd, str, strlen(str) + 1);
        close(fd);
        return 0;
    }
    return -1;
}
