EXEC=clone
SOURCES=clone.c editor.c mode.c terminal.c mice.c
OBJECTS=$(SOURCES:.c=.o)
CC=gcc

CFLAGS=-W -Wall -Werror -std=gnu99 -g
LFLAGS=-lm
 
.PHONY: default clean
 
default: $(EXEC)

mice.o: mice.c mice.h
terminal.o : terminal.c terminal.h
mode.o : mode.c mode.h
editor.o : editor.c editor.h
clone.o : clone.c clone.h editor.h mode.h mice.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
 
$(EXEC): $(OBJECTS)
	$(CC) -o $@ $^ $(LFLAGS)

clean:
	rm -rf $(EXEC) $(OBJECTS) $(SOURCES:.c=.c~) $(SOURCES:.c=.h~) Makefile~
