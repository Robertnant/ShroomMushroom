CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -fsanitize=address -lm -g
LIBS = `pkg-config --cflags glib-2.0` -lgmp

all: elgamal, huffman

elgamal: elgamal.c
	$(CC) $(CFLAGS) -o elgamal elgamal.c tools.c $(LIBS)

huffman: huffman.c
	$(CC) $(CFLAGS) -o huffman huffman.c $(LIBS)

clean:
	${RM} *.o
	${RM} *.d
	rm -f elgamal huffman
