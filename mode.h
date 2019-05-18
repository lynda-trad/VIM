#ifndef CLONE_MODE_H
#define CLONE_MODE_H

//STDIN & mice
int fd0, fd1;

void choosing_mode();

void change_mode(unsigned char c);

void normal_mode();

void insertion_mode();

int parse_line(char *s, char **argv[]);

#endif //CLONE_MODE_H
