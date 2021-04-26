CC=gcc

CPPFLAGS = `pkg-config --cflags gtk+-3.0` -MMD -export-dynamic -rdynamic
CFLAGS = -Wall -Wextra -std=c99 -g -export-dynamic -rdynamic -fsanitize=address
LDFLAGS = -rdynamic
LDLIBS = `pkg-config --libs gtk+-3.0` -lm


all: reg_page

reg_page:reg_page.c

clean:
	${RM} reg_page reg_page.o reg_page.d