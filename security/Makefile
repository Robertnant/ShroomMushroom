CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -fsanitize=address -lm -g

all: elgamal

elgamal: elgamal.c
	$(CC) $(CFLAGS) -o elgamal elgamal.c

clean:
	${RM} *.o
	${RM} *.d
	rm -f elgamal
