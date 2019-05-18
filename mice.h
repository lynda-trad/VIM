#ifndef CLONE_MICE_H
#define CLONE_MICE_H

char *souris;

typedef struct
{
    unsigned int M_X;
    unsigned int M_Y; 
} mouse_t;

//mouse cursor coordinates
mouse_t mouse;


void print_mouse_cursor();
void initMouse();
void update_mouse(signed char x, signed char y);
void read_mouse(int fd, char* buffer);




#endif //CLONE_MICE_H
