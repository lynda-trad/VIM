//
// Created by Lynda on 22/04/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <termios.h>
#include <unistd.h>

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

int main()
{
    //passe en mode non canonique
    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    //debut de vim pas de mode défini
    struct mode_s current_mode;
    printf("Press i to enter Insert mode.\nPress ESCAPE to enter Normal mode.\n");
    unsigned char c;
    scanf("%c",&c);

    change_mode(c, &current_mode);

    //verif
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