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
#include "mice.h"


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

    // disables echoeing, canonical mode and CTRL+V
    new_t.c_lflag &= ~(ICANON | ECHO | IEXTEN);

    // disables CTRL+S, CTRL+Q and CTRL+M
    new_t.c_iflag &= ~(IXON | ICRNL);

    //other flags
    new_t.c_iflag &= ~(BRKINT | INPCK | ISTRIP);
    new_t.c_cflag |= (CS8);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_t) == -1)
        die("tcsetattr failed : enableRawMode()");
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

    for (y = 0; y < WIN_Y-10; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void print_cursor()
{
    sprintf(curseur, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
    print_file(curseur, get_amount_lines(curseur));
}

void moveCursorBuf(char *buffer)
{
    int i;
    char troisieme, k1;

    for (i = 0; i < 2; ++i)
    {
        read(STDIN_FILENO, &k1, 1);
        if(i == 1 )
            troisieme = k1;
    }

    switch(troisieme)
    {
        case 'A': //up
            cursor_to_top(buffer);
            break;
        case 'B': //down
            cursor_to_bottom(buffer);
            break;
        case 'C': //right
            cursor_to_right(buffer);
            break;
        case 'D': //left
            cursor_to_left(buffer);
            break;
        case 3:
            read(STDIN_FILENO, &k1, 1);
            if( k1 == '~')
                delete_character(k1);
            break;
    }
}

void increment_cursor()
{
    if(file)
    {
        if (cursor.C_X < WIN_X)
            ++cursor.C_X;
        else if (cursor.C_Y < WIN_Y) {
            ++cursor.C_Y;
            cursor.C_X = 1;
        }

        writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
    }
    else
    {
        if (cursor.C_X < WIN_X)
            ++cursor.C_X;
        else if (cursor.C_Y < WIN_Y) {
            ++cursor.C_Y;
            cursor.C_X = 1;
        }
    }
}

void cursor_to_top_left()
{
    write(STDOUT_FILENO, "\x1b[H", 3);
//  write(STDOUT_FILENO, "\033[1;1f", 9);

    cursor.C_X       = 1;
    cursor.C_Y       = 1;
    writing_buff.cur = 0;
}

void cursor_to_bottom_left()
{
    write(STDOUT_FILENO, "\033[45;1f", 9);
    cursor.C_X = 1;
    cursor.C_Y = 45;
}

//Monter d'une ligne dans un texte affich??
void cursor_to_top(char* buffer)
{
    if(cursor.C_Y > 1 && cursor.C_Y < WIN_Y )
    {
        cursor.C_Y--;
        sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);

        print_file(buffer, get_amount_lines(buffer));

        writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
    }
    	else
	{
		writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
		print_file(buffer, get_amount_lines(buffer));
	}
}

//Descendre d'une ligne dans un texte affich??
void cursor_to_bottom(char* buffer)
{
    //if(writing_buff.cur + WIN_Y <= writing_buff.len )
    if (cursor.C_Y >= 1 && cursor.C_Y < WIN_Y - 1 && cursor.C_X >= 1 && cursor.C_X < WIN_X)
    {
        cursor.C_Y++;
        sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);

        print_file(buffer, get_amount_lines(buffer));

        writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
    } else {
        writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
        sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
        print_file(buffer, get_amount_lines(buffer));
    }
    //}
}

//Se d??placer d'un caract??re/d'une colonne ?? droite dans un texte affich??
void cursor_to_right(char* buffer)
{
    if(writing_buff.cur < writing_buff.len-1)
    {  
        if (cursor.C_X >= 1 && cursor.C_X < WIN_X)
        {
            cursor.C_X++;
            sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
            writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
            print_file(buffer, get_amount_lines(buffer));
        }
        else
        {
            writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
            sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
            print_file(buffer, get_amount_lines(buffer));
        }
    }
}

//Se d??placer d'un caract??re/d'une colonne ?? gauche dans un texte affich??
void cursor_to_left(char* buffer)
{
    if(cursor.C_X > 1 && cursor.C_X <  WIN_X )
    {
        cursor.C_X--;
        sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
        writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);

        print_file(buffer, get_amount_lines(buffer));
    }
    else
	{
		writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
		print_file(buffer, get_amount_lines(buffer));
	}
}

//Mettre le curseur ?? la position (x,y) dans le texte affich??
void cursor_to_location_buf(unsigned int x, unsigned int y)
{
    if(x >= 1 && x < WIN_X && y >= 1 && y < WIN_Y)
    {
        cursor.C_X = x;
        cursor.C_Y = y;
	   sprintf(curseur, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
        writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);

    }
	else
	{
		writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
		sprintf(curseur, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
	}
}

cursor_t get_cursor()
{
	return cursor;
}

/*
//Tentative de d??tecter fin des lignes pour ne pas laisser le curseur flotter dans le vide

//Monter d'une ligne dans un texte affich??
void cursor_to_top(char* buffer)
{	
//Nombre de caract??res de la ligne courante et celle du dessus
	unsigned int line_up= get_amount_characters_in_line( writing_buff.buff, get_line(writing_buff.buff, writing_buff.cur)-1 );
	unsigned int line_cur= get_amount_characters_in_line( writing_buff.buff, get_line(writing_buff.buff, writing_buff.cur) );
	unsigned int difference;

//Tant que le curseur reste dans la fenetre 
		
    if(cursor.C_Y > 1 && cursor.C_Y < WIN_Y && cursor.C_X >= 1 && cursor.C_X <= line_cur)
    {
		
//Si on se trouve au dernier caract??re d'une ligne
		if(writing_buff.buff[writing_buff.cur]=='\n')
		{
			
// Et si la ligne du haut contient un plus petit nombre de caract??res que la ligne courante, on se d??cale ?? gauche difference fois puis on monte le curseur,
			if (line_up < line_cur)
			{
				difference= line_cur - line_up;
				cursor.C_X= cursor.C_X - difference;
				cursor.C_Y--;
				writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
				sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
				print_file(buffer, get_amount_lines(buffer));
			} 
			
//Dans le cas contraire, on monte le curseur d'une ligne.			
			else 
			{
			cursor.C_Y--;
			sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
			print_file(buffer, get_amount_lines(buffer));
			writing_buff.cur = get_pos_cur_buffer( cursor.C_X, cursor.C_Y);
// 			writing_buff.cur = (cursor.C_X-1) * (cursor.C_Y-1) + cursor.C_X;
			}
		}
		
// Dans le cas g??n??ral, on monte le curseur d'une ligne ??galement	
		else 
		{
			cursor.C_Y--;
			writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
			sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
			print_file(buffer, get_amount_lines(buffer));
// 			writing_buff.cur = (cursor.C_X-1) * (cursor.C_Y-1) + cursor.C_X;
		}
	} 
	else
	{
		writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
		print_file(buffer, get_amount_lines(buffer));
	}
}

//Descendre d'une ligne dans un texte affich??
void cursor_to_bottom(char* buffer)
{
	
//Nombre de caract??res de la ligne courante et celle de dessous
	unsigned int line_down= get_amount_characters_in_line( writing_buff.buff, get_line(writing_buff.buff, writing_buff.cur)+1 );
	unsigned int line_cur= get_amount_characters_in_line( writing_buff.buff, get_line(writing_buff.buff, writing_buff.cur) );
	unsigned int difference;

//Tant que le curseur reste dans la fenetre 

    if(cursor.C_Y >= 1 && cursor.C_Y < WIN_Y && cursor.C_X >= 1 && cursor.C_X <= line_cur)
    {
		
//Si on se trouve au dernier caract??re d'une ligne
		if(writing_buff.buff[writing_buff.cur]=='\n')
		{

// Et si la ligne du bas contient un plus petit nombre de caract??res que la ligne courante, on se d??cale ?? gauche difference fois puis on descend le curseur d'une ligne
			if (line_down < line_cur)
			{
				difference= line_cur - line_down;
				cursor.C_X= cursor.C_X - difference;
				cursor.C_Y++;
				writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
				sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);		
				print_file(buffer, get_amount_lines(buffer));
			} 
			
//Dans le cas contraire, on descend le curseur d'une ligne.			
			else 
			{
		
			cursor.C_Y++;
			sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);		
			print_file(buffer, get_amount_lines(buffer));
			writing_buff.cur = get_pos_cur_buffer( cursor.C_X, cursor.C_Y);
// 			writing_buff.cur = (cursor.C_X-1) * (cursor.C_Y-1) + cursor.C_X;
			}
			
		} 
//Dans le cas en g??n??ral, on descend le curseur d'une ligne ??galement
		else 
		{
			cursor.C_Y++;
			sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);		
			print_file(buffer, get_amount_lines(buffer));
			writing_buff.cur = get_pos_cur_buffer( cursor.C_X, cursor.C_Y);
// 			writing_buff.cur = (cursor.C_X-1) * (cursor.C_Y-1) + cursor.C_X;
		}
	}
	else
	{
		writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
		print_file(buffer, get_amount_lines(buffer));
	}
}

//Se d??placer d'un caract??re/d'une colonne ?? droite dans un texte affich??
void cursor_to_right(char* buffer)
{	
  	unsigned int end= get_amount_characters_in_line( writing_buff.buff, get_line(writing_buff.buff, writing_buff.cur) );
	
    if(cursor.C_X >= 1 && cursor.C_X <= end)
	{
		cursor.C_X++;
		writing_buff.cur ++;
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);		
// 		color_cursor(buffer);
 		print_file(buffer, get_amount_lines(buffer));
//         writing_buff.cur = (cursor.C_X-1) * (cursor.C_Y-1) + cursor.C_X;
	}
	else
	{
		writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
		print_file(buffer, get_amount_lines(buffer));
	}
}

//Se d??placer d'un caract??re/d'une colonne ?? gauche dans un texte affich??
void cursor_to_left(char* buffer)
{
	unsigned int end= get_amount_characters_in_line( writing_buff.buff, get_line(writing_buff.buff, writing_buff.cur) );
	
    if(cursor.C_X > 1 && cursor.C_X <= end)
	{
		cursor.C_X--;
		writing_buff.cur --;
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);		
 		print_file(buffer, get_amount_lines(buffer));
//         writing_buff.cur = (cursor.C_X-1) * (cursor.C_Y-1) + cursor.C_X;
	}
	else
	{
		writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
		sprintf(buffer, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
		print_file(buffer, get_amount_lines(buffer));
	}
}

//Mettre le curseur ?? la position (x,y) dans le texte affich??
void cursor_to_location_buf(char* buffer, int x, int y)
{
    if(x > 0 && x < WIN_X && y > 0 && y < WIN_Y)
    {
		sprintf(buffer, "\x1b[%d;%dH", y, x);
		cursor.C_X = x;
		cursor.C_Y = y;
		writing_buff.cur = get_pos_cur_buffer( cursor.C_X, cursor.C_Y);
// 		writing_buff.cur = (cursor.C_X-1) * (cursor.C_Y-1) + cursor.C_X;
		print_file(buffer, get_amount_lines(buffer));
    }
}



//Colorier uniquement l'arri??re-plan du caract??re situ?? ?? la position courante du texte
void color_cursor(char* buffer)
{
	sprintf(buffer, "\x1b[91m%s", acolorier);
}

*/
