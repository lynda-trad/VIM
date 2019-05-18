#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "mice.h"
#include "mode.h"
#include "clone.h"
#include "editor.h"
#include "terminal.h"


void initEditor()
{
    cursor.C_X       = 1;
    cursor.C_Y       = 1;
    writing_buff.cur = 0;
}

void cmd_key_pressed_buf(char* buffer, char key)
{
    switch(key)
    {
        case 9: //TAB pour passer en mode Normal
            normal_mode();
        break;

        case 27:
            moveCursorBuf(buffer);
            break;

        case 126: // DEL ( == '~' )
                delete_character(key);
            break;

        case 127:

            if(cursor.C_X == 1)
            {
                unsigned int c;
                c = cursor.C_Y;
                if (c != 1)
                    delete_character(key);
            }
            else
            if(cursor.C_X >= 2)
            {
                unsigned int c;
                c = ((cursor.C_X -1)* WIN_Y + cursor.C_X) - 1;
                if (c != 0 && c != 1)
                    delete_character(key);
            }
            /*
            if(writing_buff.cur != 0 && writing_buff.cur != 1)
                delete_character(key);
            */
        break;

        default :
            if (iscntrl(key) && key != '\r')
            { // ignores CTRL V
            }
            else
            {
                if(!file) //feuille blanche
                {
                    if( key == '\r' ) // ENTER == '\n' dans notre texte
                        key = '\n';

                    add_character(key);
                }
                else
                {   //file en argument
                    if (key == '\r')
                        key = '\n';

                    if (writing_buff.cur > writing_buff.len)
                    {
                        add_character(key);
                    }
                    else
                    {
                        add_character_file(key);
                    }
                }
            }
        break;
    }
}

void add_character(char key)
{
    write(STDOUT_FILENO,&key,1);
    writing_buff.buff[writing_buff.cur] = key;
    ++writing_buff.cur;
    ++writing_buff.len;
    increment_cursor();
    //print_cursor();
}

void add_character_file(char key)
{
    //decallage des char dans le buffer
    writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
    unsigned int cx  = cursor.C_X;
    unsigned int cy  = cursor.C_Y;

    //incremente
    ++writing_buff.len;

    memmove(&writing_buff.buff[writing_buff.cur], &writing_buff.buff[writing_buff.cur-1], writing_buff.len - (writing_buff.cur-1));

    //on met le nouveau char a la position
    writing_buff.buff[writing_buff.cur-1] = key;

    //print le nouveau texte
    clear_term();
    editorDrawRows();
    cursor_to_top_left();
    print_file(writing_buff.buff, get_amount_lines(writing_buff.buff));

    //on replace le curseur la ou on est
    cursor_to_location_buf(cx, cy);
    increment_cursor();
    print_cursor();
}

void delete_character(char key)
{

    //backspace
    if (key == 127)
    {
        writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
        if (writing_buff.cur <= writing_buff.len && writing_buff.cur != 0 && writing_buff.cur != 1 )
        {
            unsigned int cx = cursor.C_X;
            unsigned int cy = cursor.C_Y;

            memmove(&writing_buff.buff[writing_buff.cur - 3], &writing_buff.buff[writing_buff.cur - 2],
                    writing_buff.len - (writing_buff.cur - 2));

            //writing_buff.buff[writing_buff.len-2] = 0;
            //writing_buff.buff[writing_buff.len-1] = 0;
            writing_buff.buff[writing_buff.len]   = 0;
            --writing_buff.len;

            fflush(stdout);
            cursor_to_left(curseur);
            clear_term();
            editorDrawRows();
            cursor_to_top_left();
            print_file(writing_buff.buff, get_amount_lines(writing_buff.buff));

            cursor_to_location_buf(cx-1, cy);
            print_cursor();
            writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
        }
    }
    //delete
    if (key == '~')
    {
        writing_buff.cur = get_pos_cur_buffer(cursor.C_X, cursor.C_Y);
        if (writing_buff.cur <= writing_buff.len - 1 && writing_buff.cur > 0)
        {
            unsigned int cx = cursor.C_X;
            unsigned int cy = cursor.C_Y;

            memmove(&writing_buff.buff[writing_buff.cur - 2], &writing_buff.buff[writing_buff.cur - 1],
                    writing_buff.len - (writing_buff.cur - 2));

           //writing_buff.buff[writing_buff.len - 2] = 0;
           //writing_buff.buff[writing_buff.len - 1] = 0;
            writing_buff.buff[writing_buff.len]     = 0;

            --writing_buff.len;
            clear_term();
            editorDrawRows();
            cursor_to_top_left();
            print_file(writing_buff.buff, get_amount_lines(writing_buff.buff));

            cursor_to_location_buf(cx, cy);
            print_cursor();
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

    //ret[all_r] = '\0';

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

unsigned int get_line(const char* buffer, unsigned int pos)
{
	unsigned int i;
	unsigned int line;
	i = 0;
	line = 1;
	
	while (i != pos)
    {
		if(buffer[i] == '\n')
		{
			++line;
		}
		++i;
    }
	return line;
}

unsigned int get_amount_characters_in_line(const char* buffer, unsigned int line)
{
    unsigned int nb_char_line;
    unsigned int l;
    unsigned int i;

    nb_char_line = 0;
    l = 1;
    i = 0;

//On récupère l'indice i du buffer à partir duquel buffer[i] se trouve au dernier caractère de la ligne line-1
    while (l != line)
    {
	    while(buffer[i] != '\n' && buffer[i])
	    {
			++i;
	    }
		++i;
	    ++l;
    }

	while(buffer[i] != '\n' && buffer[i] != 0)
	{
		++i;
		++nb_char_line;
	}

//Pour le dernier caractère'\n'
	++nb_char_line;
	
    return nb_char_line;
}

unsigned int get_pos_cur_buffer(unsigned int x, unsigned int y)
{
// Indice position courante buffer = somme du nombre de caractère par ligne  + nb de colonnes de la dernière ligne (cursor.C_X) +1
	unsigned int somme;
	somme = 0;
	for(unsigned int i = 1; i < y; ++i)
	{
		somme = somme + get_amount_characters_in_line(writing_buff.buff,i);
	}
	somme = somme + x + 1;
	
	return somme;
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

    if ((fd = open(path, O_CREAT|O_RDWR|O_TRUNC, 0644)) > 0)
    {
        write(fd, str, strlen(str) + 1);
        write(fd, "\0", 1);
        close(fd);
        return 0;
    }
    return -1;
}
