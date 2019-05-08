#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include "mode.h"
#include "clone.h"
#include "editor.h"
#include "terminal.h"

void initEditor()
{
    cursor.C_X = 0;
    cursor.C_Y = 0;
    writing_buff.cur = 0;
}

void clear_term()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    cursor_to_top_left();
}

void editorDrawRows()
{
    //draws the line of tildes like vim
    int y;

    for (y = 0; y < WIN_Y-2; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

//Gestion du curseur en mode insertion sans fichier donné en argument

void cursor_to_top_left()
{
    write(STDOUT_FILENO, "\x1b[H", 3);
//write(STDOUT_FILENO, "\033[1;1f", 9);
    cursor.C_X=0;
    cursor.C_Y=0;
}

void cursor_to_bottom_left()
{
    write(STDOUT_FILENO, "\033[42;1f", 9);
    cursor.C_X=0;
    cursor.C_Y=39;
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

//Gestion du curseur en mode insertion pour un texte déjà affiché (par un fichier donné en argument)

void moveCursorBuf(char * buffer)
{
    int i;
    char troisieme, k1;

    for (i = 0;i < 2; ++i)
    {
        read(STDIN_FILENO, &k1, 1);
        if(i ==1 )
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

//Monter d'une ligne dans un texte affiché
void cursor_to_top(char* buffer)
{
	if(cursor.C_X > 0 && cursor.C_X < WIN_X)
	{
		cursor.C_X--;
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_X, cursor.C_Y);
// 		color_cursor(buffer);
 		print_file(buffer, get_amount_lines(buffer));
	}
}

//Descendre d'une ligne dans un texte affiché
void cursor_to_bottom(char* buffer)
{
	if(cursor.C_X >= 0 && cursor.C_X < WIN_X)
	{
		cursor.C_X++;
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_X, cursor.C_Y);
// 		color_cursor(buffer);
 		print_file(buffer, get_amount_lines(buffer));

        writing_buff.cur = (cursor.C_X-1) * (cursor.C_Y-1) + cursor.C_X;
	}
}

//Se déplacer d'un caractère/d'une colonne à droite dans un texte affiché
void cursor_to_right(char* buffer)
{
	if(cursor.C_Y >= 0 && cursor.C_Y < WIN_Y)
	{
		cursor.C_Y++;
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_X, cursor.C_Y);
// 		color_cursor(buffer);
 		print_file(buffer, get_amount_lines(buffer));

        writing_buff.cur = (cursor.C_X-1) * (cursor.C_Y-1) + cursor.C_X;
	}
}

//Se déplacer d'un caractère/d'une colonne à gauche dans un texte affiché
void cursor_to_left(char* buffer)
{
	if(cursor.C_Y > 0 && cursor.C_Y < WIN_Y)
	{
		cursor.C_Y--;
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_X, cursor.C_Y);
// 		color_cursor(buffer);
 		print_file(buffer, get_amount_lines(buffer));

        writing_buff.cur = (cursor.C_X-1) * (cursor.C_Y-1) + cursor.C_X;
	}
}

//Mettre le curseur à la position (x,y) dans le texte affiché
void cursor_to_location_buf(char* buffer, int x, int y)
{
    if(x > 0 && x < WIN_X && y > 0 && y < WIN_Y)
    {
	    sprintf(buffer, "\x1b[%d;%dH", x, y);
//      color_cursor(buffer);
    	cursor.C_X = x;
    	cursor.C_Y = y;
    	print_file(buffer, get_amount_lines(buffer));

        writing_buff.cur = (cursor.C_X-1) * (cursor.C_Y-1) + cursor.C_X;
    }
}

//Colorier uniquement l'arrière-plan du caractère situé à la position courante du texte
/*void color_cursor(char* buffer)
{
	sprintf(buffer, "\x1b[91m%s", acolorier);
}
*/
