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

#include "mice.h"
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
    //xterm -geometry 120x80

    curseur = malloc(sizeof(char)* 2048);
    souris  = malloc(sizeof(char)* 2048);

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

    clear_term();
    disableRawMode();
    exit(EXIT_SUCCESS);
}