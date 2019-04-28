//
// Created by lynda on 28/04/19.
//

#ifndef MAIN_CLONE_H
#define MAIN_CLONE_H

enum mode_t;

struct mode_s;

static void handler();

void change_mode(unsigned char c, struct mode_s *m);

int parse_line(char *s, char **argv[]);

char readKey();

void keyPressed(struct mode_s *m);

#endif //MAIN_CLONE_H
