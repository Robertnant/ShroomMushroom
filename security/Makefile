CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -fsanitize=address -lm -g

all: rsa

rsa: rsa.c
	$(CC) $(CFLAGS) -o rsa rsa.c

elgamal: elgamal.c
	$(CC) $(CFLAGS) -o elgamal elgamal.c

clean:
	${RM} *.o
	${RM} *.d
	rm -f rsa elgamal
