CC = gcc
#CFLAGS = `pkg-config --cflags glib-2.0` -std=c99 -Wall -Wextra -Werror -lm -g
CFLAGS = `pkg-config --cflags glib-2.0` -std=c99 -Wall -Wextra -Werror -fsanitize=address -lm -g
LIBS = `pkg-config --libs glib-2.0` -ljson-c -lgmp -lm

all: elgamal, huffman

elgamal: elgamal.c
	$(CC) $(CFLAGS) -o elgamal elgamal.c tools.c huffman.c $(LIBS)

huffman: huffman.c
	$(CC) $(CFLAGS) -o huffman huffman.c tools.c $(LIBS)

clean:
	${RM} *.o
	${RM} *.d
	rm -f elgamal huffman
