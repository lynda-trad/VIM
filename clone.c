//
// Created by Lynda on 22/04/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#include <unistd.h>

#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>

#include "clone.h"
#include "editor.h"

struct mode_s current_mode;

enum mode_t
{
    INSERT ,
    NORMAL
};

struct mode_s
{
    mode_t type;
};

static void handler()
{
    //ignores the signal CTRL+C
}

char readKey()
{
    int n;
    char c;

    while ( (n = read(STDIN_FILENO, &c, 1) ) != 1)
    {
        if (n == -1 && errno != EAGAIN)
            die("read failed");
    }

    return c;
}

void keyPressed(struct mode_s *m)
{
    char c = readKey();

    if (iscntrl(c))
    {
        printf("%d\r\n", c);
    }
    else
    {
        printf("%d ('%c')\r\n", c, c);
    }

    switch (c)
    {
        case 105 :
        case 27 :
            change_mode(c, m);
            break;
    }
}

void change_mode(unsigned char c, struct mode_s *m)
{
    //goes from Insertion mode to Normal mode or vice versa
    switch(c)
    {
        case 105 :
            m->type = INSERT;
            break;
        case 27 :
            m->type = NORMAL;
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

int main(int argc, char **argv)
{
    //windows resize
    //xterm -geometry 90x40
    clear_term();

    //ignores CTRL+C
    struct sigaction act;
    act.sa_handler = &handler;
    act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &act, NULL);

    //non canonic mode
    enableRawMode();

    printf("Press i to enter Insertion mode. Press ESCAPE to enter Normal mode.\n");
    unsigned char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 105 && c!=27 );
    change_mode(c, &current_mode);

    //checking
    if(current_mode.type == 0)
        printf("\nINSERTION\n");
    else
    if(current_mode.type == 1)
        printf("\nNORMAL\n");

    if(current_mode.type == 0)
    {
        //write from STDIN_FILENO
        if(argc > 1)
        {
            editorDrawRows();
            write(STDOUT_FILENO, "\x1b[H", 3);

            int fd;
            fd = open(argv[1],O_RDONLY);

            if (fd < 0)
            {
                fprintf(stderr,"can not open file");
                exit(EXIT_FAILURE);
            }
            char *buffer = malloc(1024 * sizeof(char));

            //prints the file, not able to change it yet
            int n;
            while( (n = read(fd, buffer, 1024)) )
                write(STDOUT_FILENO, buffer, n);

            write(STDOUT_FILENO, "\x1b[H", 3);
            moveCursor(read(STDIN_FILENO, &c, 1));

            free(buffer);
            close(fd);
        }
        else
        {
            clear_term();
            editorDrawRows();
            write(STDOUT_FILENO, "\x1b[H", 3);
            moveCursor(read(STDIN_FILENO, &c, 1));
        }
    }
    else
        if(current_mode.type == 1)
        {
            disableRawMode();
            clear_term();
            editorDrawRows();
            write(STDOUT_FILENO, "\x1b[H", 3);
            write(STDOUT_FILENO, "\x1b[0B\x1b[999B", 12);

            char *s = malloc(sizeof(char) * 30);
            while (read(STDIN_FILENO, s, 30) < 0);

            char **tab;
            parse_line(s, &tab);

            if (!strcmp(tab[0], ":q"))
            {
                free(tab);
                free(s);
                printf("Exiting.\n");
                fflush(STDIN_FILENO);
                exit(EXIT_SUCCESS);
            }
                free(s);
                free(tab);
        }

    clear_term();
    printf("Ending.\n");
    disableRawMode();
    exit(EXIT_SUCCESS);
}
