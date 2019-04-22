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
    //ignores the signal CTRL+C
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


int parse_line(char *s, char **argv[])
{ // parses the line s
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
    //ignores CTRL+C
    struct sigaction act;
    act.sa_handler = &handler;
    act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &act, NULL);

    //non canonic mode
    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    //cfmakeraw(&newt); //decallage etrange
    newt.c_lflag &= ~(ICANON);
    newt.c_lflag &= ~ECHO;

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
        char *buffer = malloc(1024 * sizeof(char));

        //prints the file, not able to change it yet
        int n;
        while( (n = read(fd, buffer, 1024)) )
            write(STDOUT_FILENO, buffer, n);

        free(buffer);
        close(fd);
    }

    //start of vim, mode not defined
    struct mode_s current_mode;
    printf("Press i to enter Insert mode. Press ESCAPE to enter Normal mode.\n");
    unsigned char c;
    scanf("%c",&c);

    change_mode(c, &current_mode);

    //checking
    if(current_mode.type == 0)
        printf("\nInsert mode\n");
    else
    if(current_mode.type == 1)
        printf("\nNormal mode\n");


    //while(1)
    //{
        //change_mode(fgetc(stdin),&current_mode);

    if(current_mode.type == 0)
    {
        //write from STDIN_FILENO
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        exit(EXIT_SUCCESS);
    }
    else
        if(current_mode.type == 1)
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            char c;
            if(read(STDIN_FILENO, &c, 1) == ':')
            {
                char *s = malloc(sizeof(char)* 100);
                int n;

                while( (n = read(STDIN_FILENO, s, 100)))
                    write(STDOUT_FILENO, s, n);

                char **tab;
                parse_line(s, &tab);

                if(!strcmp(argv[0],"q"))
                {
                    free(tab);
                    free(s);
                    fflush(STDIN_FILENO);
                    exit(EXIT_SUCCESS);
                }

                if(!strcmp(argv[0],"w") && !argv[1])
                if(argc > 1)
                {
                    //enregistre modifs dans filename qu'on avait ouvert auparavant
                    free(s);
                    free(tab);
                    exit(EXIT_SUCCESS);
                }

                if(!strcmp(argv[0],"w") && argv[1])
                {
                    //enregistre ce qu'on a ecrit dans fichier (argv[1])
                    free(s);
                    free(tab);
                    exit(EXIT_SUCCESS);
                }

                free(s);
                free(tab);
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                exit(EXIT_SUCCESS);
            }
        }
//    }


    //at the end, it goes back to canonic mode
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    exit(EXIT_SUCCESS);
}


// xterm -geometry 80x24 -e /chemin/vers/programme
// sprintf(buffer, "\x1b[%d;%dH",x ,y); pour deplacer le curseur a la position x y