#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

#include <signal.h>

#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mode.h"
#include "clone.h"
#include "editor.h"
#include "terminal.h"


void choosing_mode()
{
    write(STDOUT_FILENO,"Press i to enter Insertion mode. Press ESCAPE to enter Normal mode.\n", 68);
    unsigned char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 105 && c!=27 );
    fflush(stdout);
    clear_term();
    editorDrawRows();
    change_mode(c, &current_mode);
}

void change_mode(unsigned char c, struct mode_s *m)
{
    //goes from Insertion mode to Normal mode or vice versa
    switch(c)
    {
        case 105 :
            m->type = INSERT;
//            insertion_mode();
            break;
        case 27 :
            m->type = NORMAL;
//            normal_mode();
            break;
    }
}

int parse_line(char *s, char **argv[])
{
    // parses the line s
    unsigned int i;
    unsigned int len;
    unsigned int wordl;

    char **tmp;
    char *debw;

    i = 0;
    len = 0;
    tmp = malloc(sizeof(char*) * 1);

    while(s[i] && s[i] != '\n')
    {
        while (s[i] == ' ')
        {
            ++i;
        }

        debw = &s[i];
        wordl = 0;

        while(s[i] && s[i] != ' ' && s[i] != '\n')
        {
            ++wordl;
            ++i;
        }

        if(wordl)
        {
            tmp[len] = malloc(sizeof(char) * wordl + 1);

            memcpy(tmp[len], debw, wordl);

            tmp[len][wordl]= '\0';

            ++len;

            tmp = realloc(tmp, sizeof(char*) * (len + 1));
        }
    }

    tmp[len] = NULL;
    argv[0] = tmp;

    return len;
}

void normal_mode()
{
    cursor_to_bottom_left();
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 105 && c!= 58);

    if(c == 105)
        insertion_mode();
    else
    if(c == 58)
    {
        disableRawMode();
        write(STDOUT_FILENO, "\r \r", 3);
        write(STDOUT_FILENO, &c, 1);

        /*
        //need to code backspace
        read(STDIN_FILENO,&c,1);
        if (c == 27)
        {
            char troisieme;
            char k1;

            for (int i = 0; i < 2; ++i)
            {
                read(STDIN_FILENO, &k1, 1);
                if (i == 1)
                    troisieme = k1;
            }

            if (troisieme == 127)
            {
                write(STDOUT_FILENO, "\r \r", 3);
                printf("hello");
                insertion_mode();
            }
        }
        */

        char *s = malloc(sizeof(char) * 30);
        s[0] = c;
        while (read(STDIN_FILENO, &s[0], 29) < 0);

        char **tab;
        parse_line(s, &tab);

        if (!strcmp(tab[0], "q"))
        {
            free(s);
            free(tab);
            //write(STDOUT_FILENO,"\rEXITING\n",10);
            printf("EXITING\n");
            fflush(STDIN_FILENO);
            exit(EXIT_SUCCESS);
        }

        if (!strcmp(tab[0], "w") && !tab[1])
        {
            /*
            int fd = open(file, O_CREAT|O_RDWR|O_TRUNC, 0644);
            if(fd < 0 )
                die("open failed\n");
            write(fd,writing_buff.buff,writing_buff.len);
            close(fd);
*           */

            write_to_file(file, writing_buff.buff);

            free(writing_buff.buff);
            free(s);
            free(tab);

            printf("SAVED TO %s\n", file);
            fflush(STDIN_FILENO);
            exit(EXIT_SUCCESS);
        }

        if (!strcmp(tab[0], "w") && tab[1])
        {
            /*
            int fd = open(tab[1], O_CREAT|O_RDWR|O_TRUNC, 0644);
            if(fd < 0 )
                die("open failed\n");
            write(fd,writing_buff.buff, writing_buff.len);
            close(fd);
            */

            write_to_file(tab[1], writing_buff.buff);

            free(writing_buff.buff);
            free(s);
            free(tab);

            printf("SAVED TO %s\n", tab[1]);
            fflush(STDIN_FILENO);
            exit(EXIT_SUCCESS);
        }

        free(writing_buff.buff);
        free(s);
        free(tab);
    }
}

void insertion_mode()
{
    disableRawMode();
    cursor_to_bottom_left();
    write(STDOUT_FILENO, "\r-- INSERT --", 13);

    initEditor();
    //writes from STDIN_FILENO
    enableRawMode();
    cursor_to_top_left();

    char key;
    while(1)
    {
        read(STDIN_FILENO,&key,1);
        cmd_key_pressed(key);
        increment_cursor();
        cursor_to_location(cursor.C_X,cursor.C_Y);
    }
}
