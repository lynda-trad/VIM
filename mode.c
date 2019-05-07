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
#include "editor.h"
#include "terminal.h"

void choosing_mode()
{
    printf("Press i to enter Insertion mode. Press ESCAPE to enter Normal mode.\n");
    unsigned char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 105 && c!=27 );
    change_mode(c, &current_mode);
}

void change_mode(unsigned char c, struct mode_s *m)
{
    refresh_screen();
    //goes from Insertion mode to Normal mode or vice versa
    switch(c)
    {
        case 105 :
            m->type = INSERT;
            insertion_mode();
            break;
        case 27 :
            m->type = NORMAL;
            normal_mode();
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

void insertion_mode()
{
    disableRawMode();
    cursor_to_bottom_left();
    write(STDOUT_FILENO, "\r-- INSERT --", 13);

    //writes from STDIN_FILENO
    enableRawMode();
    cursor_to_top();

    char key;
    for (int i = 0; i < 4; ++i)
    {
        read(STDIN_FILENO,&key,1);
        cmd_key_pressed(key);
    }
    //sleep(3);
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
            read(STDIN_FILENO,&c,1);
            if (c == 8)
                insertion_mode();
            */

            char *s = malloc(sizeof(char) * 30);
            while (read(STDIN_FILENO, s, 30) < 0);

            char **tab;
            parse_line(s, &tab);

            if (!strcmp(tab[0], "q")) {
                free(s);
                free(tab);
                //write(STDOUT_FILENO,"\rEXITING\n",10);
                printf("EXITING\n");
                fflush(STDIN_FILENO);
                exit(EXIT_SUCCESS);
            }

            free(s);
            free(tab);
        }
}