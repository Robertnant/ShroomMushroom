CC=gcc

CPPFLAGS = `pkg-config --cflags gtk+-3.0` -MMD -export-dynamic -rdynamic
CFLAGS = -Wall -Wextra -std=c99 -g -export-dynamic -rdynamic -fsanitize=address
LDFLAGS = -rdynamic
LDLIBS = `pkg-config --libs gtk+-3.0` -lm


all: interface

interface:interface.c

clean:
	${RM} interface interface.o interface.d