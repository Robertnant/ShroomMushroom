CC=gcc
CFLAGS= `pkg-config --cflags gtk+-3.0` `pkg-config --cflags glib-2.0` -Wall -Wextra -lm -g -pthread -std=c99
LDLIBS= `pkg-config --libs gtk+-3.0`  -ljson-c -lgmp -lm
VPATH=networking:saved_users:messages:security:design/Registration:design/Main:design/addcontact
LIBS= `pkg-config --libs glib-2.0` -lgmp -lm


all: server client

server: server.c users.c messages.c tools.c elgamal.c huffman.c client_list.c

client: client.c users.c messages.c tools.c elgamal.c huffman.c reg_page.c interface_full.c add_contact.c


clean:
	${RM} client server *.d *.o

remove:
	${RM} saved_users/Pipes/*[0-9]* saved_users/Users/*[0-9]*
	${RM} .user

bigclean:
	${RM} client server *.d *.o
	${RM} design/Registration/reg_*~
	${RM} saved_users/Pipes/*[0-9]* saved_users/Users/*[0-9]*
	${RM} .files/.user .files/contacts/*[0-9]* .files/contacts.txt .files/chats/*[0-9]*

clearchat:
	${RM} .files/chats/*[0-9]*

clearcontacts:
	${RM} .files/contacts.txt .files/contacts/*[0-9]*

install:
	apt install libjson-c-dev
	apt install libgtk-3-dev
	apt install libgmp3-dev

.PHONY: all clean remove bigclean clearchat clearcontacts
