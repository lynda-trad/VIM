#ifndef CLONE_MODE_H
#define CLONE_MODE_H

struct mode_s current_mode;

enum mode_t
{
    INSERT ,
    NORMAL
};

struct mode_s
{
    mode_t type;
};

void change_mode(unsigned char c, struct mode_s *m);

void normal_mode();

void insertion_mode();

int parse_line(char *s, char **argv[]);

#endif //CLONE_MODE_H
