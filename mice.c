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
    sprintf(buffer, "\x1b[%d;%dH", mouse.C_Y, mouse.C_X);
}

void initMouse()
{
    mouse.C_X = 0;
    mouse.C_Y = 0;
}

int update_mouse(){
	mouse.C_X += mice_data[1];
	mouse.C_Y += mice_data[2];
	return 0;
}

int main(){
	int fd, bytes;
    const char *pDevice = "/dev/input/mice";

// Open Mouse
    fd = open(pDevice, O_RDWR | O_NONBLOCK);
    if (fd == -1) {
        printf("ERROR Opening %s\n", pDevice);
        return -1;
    }
    
    int left, middle, right;
    signed char x, y;
    char buffer[5]="Hello";
    initMouse();
    
    while (1) {
        // Read Mouse
        bytes = read(fd, mice_data, sizeof(mice_data));

        if (bytes > 0)
        {
            left = mice_data[0] & 0x1; // 1 quand clic gauche 0 quand relaché
            right = mice_data[0] & 0x2;
            middle = mice_data[0] & 0x4;
		  x = mice_data[1]; // 0 quand on descend ou monte // -1 quand on va a gauche // 1 quand on va a droite
            y = mice_data[2]; // 0 quand on va a gauche ou a droite // -1 quand on descend // 1 quand on monte
		  update_mouse();
            printf(" x=%d, y=%d, left=%d, middle=%d, right=%d\n", x, y, left, middle, right);
		  printf("M.x=%d M.y=%d", mouse.C_X, mouse.C_Y); // coordonnées de la position actuelle de la souris
		  print_mouse_cursor(buffer);

        }
    }

     close(fd);

	return 0;
}
	
/*

int main()
{
    int fd, bytes;
    unsigned char data[3];

    const char *pDevice = "/dev/input/mice";

// Open Mouse
    fd = open(pDevice, O_RDWR);
    if (fd == -1) {
        printf("ERROR Opening %s\n", pDevice);
        return -1;
    }
    int left, middle, right;
    signed char x, y;
    while (1) {
        // Read Mouse
        bytes = read(fd, data, sizeof(data));

        if (bytes > 0)
        {
            left = data[0] & 0x1; // 1 quand clic gauche 0 quand relaché
            right = data[0] & 0x2;
            middle = data[0] & 0x4;

            x = data[1]; // 0 quand on descend ou monte // -1 quand on va a gauche // 1 quand on va a droite
            y = data[2]; // 0 quand on va a gauche ou a droite // -1 quand on descend // 1 quand on monte
            printf("x=%d, y=%d, left=%d, middle=%d, right=%d\n", x, y, left, middle, right);
        }
    }
    mouse.C_X = x;
    mouse.C_Y = y;
    

    exit(0);
}
*/
