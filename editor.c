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

        case 126: //maybe DEL ( == '~' )
            delete_character(key);
            break;

        case 127:
            delete_character(key);
        break;

        default :
            if( key == '\r' ) // ENTER == '\n' dans notre texte
            {
                key = '\n';
                write(STDOUT_FILENO,&key,1);
                writing_buff.buff[writing_buff.len] = key;
                ++writing_buff.len;
            }
            else if (iscntrl(key))
            { // ignores CTRL V
                //write(STDOUT_FILENO,"\r\n",2);
            }
            else
            {
                write(STDOUT_FILENO,&key,1);
                writing_buff.buff[writing_buff.len] = key;
                ++writing_buff.cur;
                ++writing_buff.len;

            /*
                if(file)
                {
                    write(STDOUT_FILENO, &key, 1);

                    //decallage du reste
                    memmove(&writing_buff.cur + 2, &writing_buff.cur + 1, writing_buff.len - writing_buff.cur);

                    //on met le nouveau char a la position
                    writing_buff.buff[writing_buff.cur] = key;
                    ++writing_buff.len;
                }
                else
                    if(!file)
                {
             }
            */

            }
        break;
    }
}

void delete_character(char key)
{
    //backspace

 /*    // EXEMPLE
    if (key == 127)
    {
        memmove(&writing_buff.buff[5], &writing_buff.buff[6], writing_buff.len - 6);
        writing_buff.buff[writing_buff.len] = 0;
        --cursor.C_X;
        --writing_buff.cur;
        --writing_buff.len;

		    sprintf(writing_buff.buff, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);
		    print_file(writing_buff.buff, get_amount_lines(writing_buff.buff));
    }*/
    

    if (key == 127)
    {
		writing_buff.cur=get_pos_cur_buffer(cursor.C_X,cursor.C_Y); // avec cette fonction ça ne fait plus segfault et on voit bien le curseur reculer mais efface toujours pas
        memmove(&writing_buff.buff[writing_buff.cur - 1], &writing_buff.buff[writing_buff.cur], writing_buff.len - writing_buff.cur);

        writing_buff.buff[writing_buff.len] = 0;
        --cursor.C_X;
        --writing_buff.cur;
        --writing_buff.len;
	   sprintf(writing_buff.buff, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X);//affiche le curseur
        print_file(writing_buff.buff, get_amount_lines(writing_buff.buff));

    }
    if (key == '~')
    {
		writing_buff.cur=get_pos_cur_buffer(cursor.C_X,cursor.C_Y);
        memmove(&writing_buff.buff[writing_buff.cur + 1], &writing_buff.buff[writing_buff.cur + 2], writing_buff.len - writing_buff.cur);
        writing_buff.buff[writing_buff.len] = 0;
 	   sprintf(writing_buff.buff, "\x1b[K"); // pas bon mais commande qui permet d'effacer toute la ligne depuis la position courante...
        --writing_buff.len;
// 	   sprintf(writing_buff.buff, "\x1b[%d;%dH", cursor.C_Y, cursor.C_X); //affiche le curseur
        print_file(writing_buff.buff, get_amount_lines(writing_buff.buff));
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
    //writing_buff.cur = all_r; // placer cur a la fin du fichier

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
    l = 1;
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

//Pour le dernier caractère'\n'
	nb_char_line++;
	
    return nb_char_line;
    
}

unsigned int get_pos_cur_buffer(unsigned int x, unsigned int y)
{
// Indice position courante buffer = somme du nombre de caractère par ligne  + nb de colonnes de la dernière ligne (cursor.C_X) +1
	unsigned int somme;
	somme = 0;
	for(unsigned int i = 1; i < y; i++)
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
