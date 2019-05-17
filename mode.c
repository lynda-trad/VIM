#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

#include <signal.h>
#include <poll.h>

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
    //where you choose the mode
    write(STDOUT_FILENO,"Press i to enter Insertion mode. Press TAB to enter Normal mode.\n\r", 68);
    write(STDOUT_FILENO,"\n\rIn Normal mode:\n\r\n\r :i = go back to Insertion mode\n\r :w = save to current opened file\n\r", 89);
    write(STDOUT_FILENO," :w <filename> = save to <filename>\n\r :q = quit clone\n\r :q! = quit without saving\n\r", 83);
    unsigned char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 105 && c!= 9 );
    fflush(stdout);
    change_mode(c);
}

void change_mode(unsigned char c)
{
    //goes from Insertion mode to Normal mode or vice versa
    switch(c)
    {
        case 105 :
            insertion_mode();
            break;
        case 9 :
            normal_mode();
            break;
    }
}

void insertion_mode()
{
    fflush(stdout);
    disableRawMode();
    clear_term();
    editorDrawRows();

    disableRawMode();
    cursor_to_bottom_left();
    write(STDOUT_FILENO, "\r-- INSERT --", 13);

    initEditor();
    //writes from STDIN_FILENO
    enableRawMode();
    cursor_to_top_left();
    print_file(writing_buff.buff, writing_buff.len);
    cursor_to_top_left();

    /*
	int fd0, fd1, bytes;

    const char *pDevice = "/dev/input/mice";

//  Open Mouse & STDIN

    fd0 = dup(STDIN_FILENO);
    fd1 = open(pDevice, O_RDWR | O_NONBLOCK);

    if (fd0 == -1 || fd1 == -1)
        die("open failed");

    int left, middle, right;
    signed char x, y;

    struct pollfd pfds[2];
    pfds[0].fd = fd0;
    pfds[0].events = POLLIN;
    pfds[1].fd = fd1;
    pfds[1].events = POLLIN;

    */

    char key;
    while(1)
    {
        read(STDIN_FILENO,&key,1);
        /*
            poll(&pfds, 2, -1);
            for(int i = 0; i < 2; ++i)
            {
                if(pfds[i].revents & POLLIN)
                {
                    if(i = 0)
                    {
                        read(fd0,&key,1);
                        cmd_key_pressed_buf(curseur, key);
                    }
                    else if(i = 1)
                        //mice
                }
            }
         */
        cmd_key_pressed_buf(curseur, key);
    }
}

void normal_mode()
{
    fflush(stdout);
    disableRawMode();
    clear_term();
    editorDrawRows();

    enableRawMode();
    cursor_to_top_left();
    print_file(writing_buff.buff, writing_buff.len);

    cursor_to_bottom_left();
    write(STDOUT_FILENO, "\rPress i now to go back to Insertion mode. Press : to start a command.\n\r",72);
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 105 && c!= 58);

    if(c == 105)
        insertion_mode();
    else
    if(c == 58)
    {
        cursor_to_bottom_left();
        disableRawMode();
        write(STDOUT_FILENO, "\r                                                                     \r", 71);
        write(STDOUT_FILENO, &c, 1);

        // ligne de commande
        char *s = malloc(sizeof(char) * 40);
        while (read(STDIN_FILENO, s, 40) < 0);

        char **tab;
        parse_line(s, &tab);

        if (!strcmp(tab[0], "i")) //goes back to Insertion mode
        {
            free(s);
            free(tab);
            fflush(STDIN_FILENO);
            enableRawMode();
            insertion_mode();
        }
        else
        if (!strcmp(tab[0], "q!"))
        {
            free(writing_buff.buff);
            free(s);
            free(tab);
            free(curseur);
            printf("EXITING\n");
            fflush(STDIN_FILENO);
            exit(EXIT_SUCCESS);
        }
        else
        if (!strcmp(tab[0], "q"))
        {
            if(writing_buff.buff[0])
            {
                write(STDOUT_FILENO, "Command :q! if you don't want to save.", 38);
                sleep(1);
                free(s);
                free(tab);
                fflush(STDIN_FILENO);
                enableRawMode();
                normal_mode();
            }
            else
            {
                free(writing_buff.buff);
                free(s);
                free(tab);
                free(curseur);
                printf("EXITING\n");
                fflush(STDIN_FILENO);
                exit(EXIT_SUCCESS);
            }
        }
        else
        if (!strcmp(tab[0], "w") && !tab[1])
        {
            if(file)
                write_to_file(file, writing_buff.buff);
            else
            {
                write(STDOUT_FILENO, "Please put a filename after :w.", 30);
                sleep(1);
                free(s);
                free(tab);
                fflush(STDIN_FILENO);
                enableRawMode();
                normal_mode();
            }

            free(writing_buff.buff);
            free(s);
            free(tab);

            printf("SAVED TO %s\n", file);
            fflush(STDIN_FILENO);
            exit(EXIT_SUCCESS);
        }
        else
        if (!strcmp(tab[0], "w") && tab[1])
        {
            write_to_file(tab[1], writing_buff.buff);

            free(writing_buff.buff);
            free(s);
            free(tab);

            printf("SAVED TO %s\n", tab[1]);
            fflush(STDIN_FILENO);
            exit(EXIT_SUCCESS);
        }
        else
        {
            write(STDOUT_FILENO, "Command does not exist!", 23);
            write(STDOUT_FILENO,"\n\rIn Normal mode:\n\r\n\r :i = go back to Insertion mode\n\r :w = save to current opened file\n\r", 89);
            write(STDOUT_FILENO," :w <filename> = save to <filename>\n\r :q = quit clone\n\r :q! = quit without saving\n\r", 83);
            write(STDOUT_FILENO,"You can enter a new command in 5 seconds.\n\r", 43);
            sleep(5);
            free(s);
            free(tab);
            fflush(STDIN_FILENO);
            enableRawMode();
            normal_mode();
        }
        free(writing_buff.buff);
        free(s);
        free(tab);

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