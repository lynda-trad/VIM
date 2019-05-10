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

void cmd_key_pressed_buf(char* buffer, char key)
{

    switch(key)
    {
        case 9: //TAB pour passer en mode Normal
            normal_mode();
        break;

        case 127:
            delete_character(key);
        break;

        case 27:
            moveCursorBuf(buffer);
        break;

        default :
            if (iscntrl(key))
            {
                write(STDOUT_FILENO,"\r\n",2);
                //delete_character(key);
            }
            else
            {
                write(STDOUT_FILENO,&key,1);
                writing_buff.buff[writing_buff.len] = key;
                ++writing_buff.len;
            }
        break;
    }
}

//Segfault tant qu'on code pas le buffer
void delete_character(char key)
{

/*
 *   SUPPR : Erase to the right -> pas égal à 8
 *   ^[[3~
 *   27 + [ + 3 + ~
 *   case 27 : commence comme flèches
 *   mais a la place de A B C D c'est 3~
 *
 *  BACKSPACE : Erase to the left
 *      read(STDIN_FILENO, &c, 1);
 *      c == 127 == backspace
 * */

//      key = BACKSPACE ; Erase to the left
        if (key == 127)
        {
    		char* new_buff = malloc(sizeof(writing_buff.buff)-1);
	    	unsigned int i;

//      On copie tout ce qui est avant le curseur

		    for(i = 0; i < writing_buff.cur; i++)
		    {
			    new_buff[i] = writing_buff.buff[i];
		    }

//      On copie tout ce qui est juste après le curseur pour supprimer le caractère qui est actuellement dans le curseur

            for(unsigned int j = writing_buff.cur + 1; j< writing_buff.len; j++)
		    {
    		    new_buff[i+1] = writing_buff.buff[j];
	    	}

		    writing_buff.buff = new_buff;
		    print_file(writing_buff.buff, get_amount_lines(writing_buff.buff));

		    cursor.C_Y--;
              writing_buff.cur--;
		    writing_buff.len--;
	    }
        else if(key == 126) // ~ tilde a la fin de SUPPR
        {
            char* new_buff = malloc(sizeof(writing_buff.buff)-1);
            unsigned int i;

//      On copie tout ce qui est avant le curseur, inclus

            for(i = 0; i < writing_buff.cur + 1; i++)
            {
                new_buff[i] = writing_buff.buff[i];
            }

//      On copie tout ce qui est après le char qu'on veut supprimer, il est juste après le curseur

            for(unsigned int j = writing_buff.cur + 2; j< writing_buff.len; j++)
            {
                new_buff[i+1] = writing_buff.buff[j];
            }

            writing_buff.buff = new_buff;
            print_file(writing_buff.buff, get_amount_lines(writing_buff.buff));

            writing_buff.len--;
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

unsigned int get_line(const char* buffer, unsigned int pos)
{
	unsigned int i;
	unsigned int line;
	i = 0;
	line = 0;
	
	while (i != pos)
    {
		if(buffer[i] == '\n')
		{
			line++;
		}
		i++;
    }
	return line;
}

unsigned int get_amount_characters_in_line(const char* buffer, unsigned int line)
{
    unsigned int nb_char_line;
    unsigned int l;
    unsigned int i;

    nb_char_line = 0;
    l = 0;
    i = 0;

//On récupère l'indice i du buffer à partir duquel buffer[i] se trouve au dernier caractère de la ligne line-1
    while (l != line)
    {
	    while(buffer[i] != '\n')
	    {
			i++;
	    }
		i++;
	    l++;
    }

	while(buffer[i] != '\n')
	{
		i++;
		nb_char_line++;
	}

//Pour le dernier caractère	
	nb_char_line++;
	
    return nb_char_line;
    
}

unsigned int get_pos_cur_buffer(unsigned int x, unsigned int y)
{
// Indice position courante buffer = somme du nombre de caractère par ligne (donc cursor.C_Y * nb de caractère dans la ligne) + nb de colonnes de la dernière ligne (cursor.C_X) +1
	unsigned int somme;
	somme = 0;
	for(unsigned int i = 0; i<y; i++)
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
