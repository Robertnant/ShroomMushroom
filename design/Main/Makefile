CC=gcc

CPPFLAGS = `pkg-config --cflags gtk+-3.0` -MMD -export-dynamic -rdynamic
CFLAGS = -Wall -Wextra -std=c99 -g -export-dynamic -rdynamic -fsanitize=address -Wimplicit-function-declaration
LDFLAGS = -rdynamic
LDLIBS = `pkg-config --libs gtk+-3.0` -lm


all: interface_full

interface: interface_full.c

clean:
	${RM} interface_full *.o *.d *.glade~
