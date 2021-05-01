CC=gcc

CPPFLAGS = `pkg-config --cflags gtk+-3.0` -MMD -export-dynamic -rdynamic
CFLAGS = -Wall -Wextra -std=c99 -g -export-dynamic -rdynamic -fsanitize=address
LDFLAGS = -rdynamic
LDLIBS = `pkg-config --libs gtk+-3.0` -lm


all: add_contact

reg_page: add_contact.c

clean:
	${RM} add_contact add_contact.o add_contact.d
