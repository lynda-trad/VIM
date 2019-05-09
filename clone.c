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

#include "mode.h"
#include "clone.h"
#include "editor.h"
#include "terminal.h"

void handler()
{
    //ignores the signal CTRL+C
}

void die(const char *s)
{
    //error handler
    fflush(stdout);
    clear_term();
    perror(s);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    //windows resize
    //xterm -geometry 90x40

    //ignores CTRL+C
    struct sigaction act;
    act.sa_handler = &handler;
    act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &act, NULL);

    fflush(stdout);
    clear_term();

    if(argc > 1)
    {
        file = malloc(strlen(argv[1]) + 1);
        strcpy(file, argv[1]);
        writing_buff.buff = get_file(argv[1]);
    }
    else
        writing_buff.buff = malloc(sizeof(char) * BUFF_SIZE);


    enableRawMode();
    choosing_mode();

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
            cursor_to_top_left();
            int fd;
            fd = open(argv[1],O_RDONLY);

            if (fd < 0)
            {
                fprintf(stderr,"can not open file");
                exit(EXIT_FAILURE);
            }

            //prints the file, not able to change it yet

            print_file(writing_buff.buff, get_amount_lines(writing_buff.buff));

            cursor_to_top_left();

            char key;
            for(int i = 0; i < 100; ++i) //testing arrows
            {
                read(STDIN_FILENO,&key,1);
                cmd_key_pressed_buf(writing_buff.buff,key);
                //cmd_key_pressed_buf(s,key);
            }

            free(writing_buff.buff);
            close(fd);
            sleep(3);
        }
        else // pas demandé dans l'énoncé je crois
            {
            disableRawMode();
            clear_term();
            editorDrawRows();
            cursor_to_bottom_left();
            write(STDOUT_FILENO, "\r-- INSERT --", 13);

            //writes from STDIN_FILENO
            enableRawMode();
            cursor_to_top_left();

            char key;
            for (int i = 0; i < 100; ++i)
            {
                read(STDIN_FILENO,&key,1);
                cmd_key_pressed_buf(writing_buff.buff,key);
            }

            free(writing_buff.buff);
            sleep(3);
        }
    }
    else
        if(current_mode.type == 1)
        {
            normal_mode();

            /*
            disableRawMode();
            clear_term();
            editorDrawRows();
            cursor_to_top_left();
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
            */
        }

    clear_term();
    //write(STDOUT_FILENO,"\rENDING\n",9);
    printf("Ending\n");
    disableRawMode();
    exit(EXIT_SUCCESS);
}
