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
    exit(0);
}