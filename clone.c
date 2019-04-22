//
// Created by Lynda on 22/04/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <termios.h>
#include <unistd.h>

#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

static void handler()
{
    //ignores the signals CTRL+C
}

enum mode_t
{
    INSERT ,
    NORMAL
};

struct mode_s
{
    mode_t type;
};

void change_mode(unsigned char c, struct mode_s *m)
{
    if(c == 105)
        m->type = INSERT;
    else if(c == 27)
        m->type = NORMAL;
}

int main(int argc, char **argv)
{
    //ignores CTRL+C
    struct sigaction act;
    act.sa_handler = &handler;
    act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &act, NULL);

    //non canonic mode
    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    //opens vim with a filename
    if(argc > 1)
    {
        int fd;
        fd = open(argv[1],O_RDONLY);

        if (fd < 0)
        {
            fprintf(stderr,"can not open file");
            exit(EXIT_FAILURE);
        }
        char *s = malloc(1024 * sizeof(char));

        //prints the file, not able to change it yet
        int n;
        while( (n = read(fd, s, 1024)) )
            write(STDOUT_FILENO,s,n);
    }

    //start of vim, mode not defined
    struct mode_s current_mode;
    printf("Press i to enter Insert mode.\nPress ESCAPE to enter Normal mode.\n");
    unsigned char c;
    scanf("%c",&c);

    change_mode(c, &current_mode);

    //checking
    if(current_mode.type == 0)
        printf("Insert mode\n");
    else
    if(current_mode.type == 1)
        printf("Normal mode\n");

    /*
    while(1)
    {
        change_mode(fgetc(stdin),&current_mode);
        //utilisation du clone de vim
    }
    */

    exit(EXIT_SUCCESS);
}