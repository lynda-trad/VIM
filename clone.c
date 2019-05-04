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
        write(STDOUT_FILENO,"\nINSERTION\n", 11);
    else
    if(current_mode.type == 1)
        write(STDOUT_FILENO,"\nNORMAL\n",8);

    clear_term();

    if(current_mode.type == 0)
    {
        if(argc > 1)
        {
            disableRawMode();
            clear_term();
            editorDrawRows();

            // writes from file
            enableRawMode();
            cursor_to_top();
            int fd;
            fd = open(argv[1],O_RDONLY);

            if (fd < 0)
            {
                fprintf(stderr,"can not open file");
                exit(EXIT_FAILURE);
            }

            //prints the file, not able to change it yet

            /*
            char *buffer = malloc(BUFF_SIZE * sizeof(char));
            int n;
            while( (n = read(fd, buffer, 1024)) )
                write(STDOUT_FILENO, buffer, n);
            */

            char *s = get_file(argv[1]);
            print_file(s, get_amount_lines(s));

            cursor_to_top();

            char key;
            for(int i = 0; i < 4; ++i) //testing arrows
            {
                read(STDIN_FILENO,&key,1);
                cmd_key_pressed(key);
            }

            //free(buffer);
            free(s);
            close(fd);
            sleep(3);
        }
        else
            {
            disableRawMode();
            clear_term();
            editorDrawRows();
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
            sleep(3);
        }
    }
    else
        if(current_mode.type == 1)
        {
            disableRawMode();
            clear_term();
            editorDrawRows();
            cursor_to_top();
            cursor_to_bottom_left();
            write(STDOUT_FILENO, "\r \r", 3);

            char *s = malloc(sizeof(char) * 30);
            while (read(STDIN_FILENO, s, 30) < 0);

            char **tab;
            parse_line(s, &tab);

            if (!strcmp(tab[0], ":q"))
            {
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

    clear_term();
    //write(STDOUT_FILENO,"\rENDING\n",9);
    printf("Ending\n");
    disableRawMode();
    exit(EXIT_SUCCESS);
}