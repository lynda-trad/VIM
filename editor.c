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

#include "editor.h"
#include "mode.h"

struct termios old_t;

void cursor_to_top_left()
{
    write(STDOUT_FILENO, "\x1b[H", 3);
    CUR_X=0;
    CUR_Y=0;
}

void cursor_to_bottom_left()
{
    write(STDOUT_FILENO, "\033[90;0f", 9);
    CUR_X=0;
    CUR_Y=39;
    //write(STDOUT_FILENO, "\x1b[0B\x1b[999B", 12);
    //write(STDOUT_FILENO, "\x1b[00\x1b[999B", 12);
}

//Se déplacer d'un caractère/d'une colonne à gauche
void cursor_to_left(char* buffer)
{
	if(CUR_Y > 0 && CUR_Y < WIN_Y){
		CUR_Y= CUR_Y-1;
		sprintf(buffer, "\x1b[%d;%dH", CUR_X, CUR_Y);
// 		color_cursor(buffer);
 		print_file(buffer, get_amount_lines(buffer));
	}
}

//Se déplacer d'un caractère/d'une colonne à droite
void cursor_to_right(char* buffer)
{
	if(CUR_Y >= 0 && CUR_Y < WIN_Y){
		CUR_Y= CUR_Y+1;
		sprintf(buffer, "\x1b[%d;%dH", CUR_X, CUR_Y);
// 		color_cursor(buffer);
 		print_file(buffer, get_amount_lines(buffer));
	}
}

//Descendre d'une ligne
void cursor_to_bottom(char* buffer)
{
	if(CUR_X >= 0 && CUR_X < WIN_X){
		CUR_X= CUR_X+1;
		sprintf(buffer, "\x1b[%d;%dH", CUR_X, CUR_Y);
// 		color_cursor(buffer);
 		print_file(buffer, get_amount_lines(buffer));
	}
}

//Monter d'une ligne
void cursor_to_top(char* buffer)
{
	if(CUR_X > 0 && CUR_X < WIN_X){
		CUR_X= CUR_X-1;
		sprintf(buffer, "\x1b[%d;%dH", CUR_X, CUR_Y);
// 		color_cursor(buffer);
 		print_file(buffer, get_amount_lines(buffer));	
		
	}
}

//Mettre le curseur à la position (x,y) dans le texte affiché
void cursor_to_location(char* buffer, int x, int y)
{
    sprintf(buffer, "\x1b[%d;%dH", x, y);
//     color_cursor(buffer);
    CUR_X = x;
    CUR_Y = y;
    print_file(buffer, get_amount_lines(buffer));	

}

//Colorier uniquement l'arrière-plan du caractère situé à la position courante du texte
/*void color_cursor(char* buffer)
{
	sprintf(buffer, "\x1b[91m%s", acolorier);
}
*/

void moveCursorBuf(char * buffer)
{
	int i; 
	char troisieme, k1;

	for (i=0;i<2;++i)
	{
        read(STDIN_FILENO, &k1, 1);
	    if(i==1)
	        troisieme = k1;
	}
	
	switch(troisieme)
    {
		case 'A':
//             write(STDOUT_FILENO,"up",3);
		  cursor_to_top(buffer);
            break;
		case 'B':
//               write(STDOUT_FILENO,"down",5);
		  cursor_to_bottom(buffer);

            break;
		case 'C':
//               write(STDOUT_FILENO,"right",6);
		  cursor_to_right(buffer);

            break;
		case 'D':
//               write(STDOUT_FILENO,"left",5);
		  cursor_to_left(buffer);

            break;
    }
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
            break;
		case 'B':
            write(STDOUT_FILENO,"down\n",5);
            break;
		case 'C':
            write(STDOUT_FILENO,"right\n",6);
            break;
		case 'D':
            write(STDOUT_FILENO,"left\n",5);
            break;
    }
}
void cmd_key_pressed_buf( char* buffer, char key)
{

    if (key == 27)
    {
        /*
        char k1;
        if(read(STDIN_FILENO, &k1, 1) != 0)
        */
        moveCursorBuf(buffer);
        /*
        else
            if(current_mode.type == INSERT)
            change_mode(key, &current_mode);
        */
    }
    else
    {
        if (iscntrl(key))
        {
            printf("\r\n");
        }
        else
        {
            /*
            if(key == 105)
                if(current_mode.type == NORMAL)
                    change_mode(key, &current_mode);
                else
                    write(STDOUT_FILENO,&key,1);
            else
             */
                write(STDOUT_FILENO,&key,1);
        }
    }
}

void cmd_key_pressed(char key)
{

    if (key == 27)
    {
        /*
        char k1;
        if(read(STDIN_FILENO, &k1, 1) != 0)
        */
        moveCursor();
        /*
        else
            if(current_mode.type == INSERT)
            change_mode(key, &current_mode);
        */
    }
    else
    {
        if (iscntrl(key))
        {
            printf("\r\n");
        }
        else
        {
            /*
            if(key == 105)
                if(current_mode.type == NORMAL)
                    change_mode(key, &current_mode);
                else
                    write(STDOUT_FILENO,&key,1);
            else
             */
                write(STDOUT_FILENO,&key,1);
        }
    }
}

void die(const char *s)
{
    //error handler
    clear_term();
    perror(s);
    exit(EXIT_FAILURE);
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
    cursor_to_top_left();

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
