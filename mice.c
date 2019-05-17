//
// Created by Lynda on 10/05/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "mode.h"
#include "clone.h"
#include "editor.h"
#include "terminal.h"
#include "mice.h"

void print_mouse_cursor(char * buffer)
{
    sprintf(buffer, "\x1b[%d;%dH", mouse.M_Y, mouse.M_X);
}

void initMouse()
{
    mouse.M_X = 1;
    mouse.M_Y = 1;
}

void update_mouse(signed char x, signed char y)
{
	if(mouse.M_X >=1 && mouse.M_X < WIN_X && mouse.M_Y >= 1 && mouse.M_Y< WIN_Y){
		mouse.M_X += x;
		mouse.M_Y += y;
	}
}

void read_mouse(int fd, char* buffer)
{
    int bytes;
    int left;
//     int middle, right;
    signed char x, y;
    initMouse();
    
// Informations de la souris (droite/gauche/milieu et coordonnés +1/-1 à ajouter à mouse);
    char mice_data[3];

    while (1) 
    {
        // Read Mouse
        bytes = read(fd, mice_data, sizeof(mice_data));

        if (bytes > 0)
        {
		   
//Récupération des informations du fichier mice
            left = mice_data[0] & 0x1; // 1 quand clic gauche 0 quand relaché
//             right = mice_data[0] & 0x2;
//             middle = mice_data[0] & 0x4;
		  x = mice_data[1]; // 0 quand on descend ou monte // -1 quand on va a gauche // 1 quand on va a droite
            y = mice_data[2]; // 0 quand on va a gauche ou a droite // -1 quand on descend // 1 quand on monte
            
// Mise à jour des coordonnées du curseur de la souris en fonction de x et y si clique gauche
		  if(left==1){
			update_mouse(x,y);
		  
//          	printf(" x=%d, y=%d, left=%d, middle=%d, right=%d\n", x, y, left, middle, right);
// 		 	printf("M.x=%d M.y=%d", mouse.C_X, mouse.C_Y); // coordonnées de la position actuelle de la souris
		  
// Affichage du curseur dans le buffer 
			print_mouse_cursor(buffer);
			
// Colorier le texte/ curseur
// 			sprintf(buffer, "\x1b[91m%s", acolorier);

			clear_term();
			editorDrawRows();
			cursor_to_top_left();
 			print_file(writing_buff.buff, writing_buff.len);
			}

	   }
    }
 
}
	
