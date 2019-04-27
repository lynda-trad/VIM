//
// Created by Lynda on 27/04/19.
//

#ifndef CLONE_EDITOR_H
#define CLONE_EDITOR_H

#include <termios.h>

void die(const char *s);

void disableRawMode();

void enableRawMode();

void clear_term();

void editorDrawRows();

void moveCursor(unsigned char c);

#endif //CLONE_EDITOR_H
