#ifndef CLONE_MICE_H
#define CLONE_MICE_H

typedef struct
{
    unsigned int C_X;
    unsigned int C_Y; 
} mouse_t;

//mouse cursor coordinates
mouse_t mouse;

// Informations de la souris (droite/gauche/milieu et coordonnés +1/-1 à ajouter à mouse);
char mice_data[3];



void print_mouse_cursor();
void initMouse();
int update_mouse();




#endif //CLONE_MICE_H
