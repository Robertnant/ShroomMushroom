CC=gcc
CFLAGS= `pkg-config --cflags gtk+-3.0` -Wall -Wextra -g -lm -pthread -std=c99 -fsanitize=address
LDLIBS= `pkg-config --libs gtk+-3.0` -ljson-c -lgmp
VPATH=networking:saved_users:messages:security:design/Registration

all: server client

server: server.c users.c messages.c tools.c elgamal.c client_list.c

client: client.c users.c messages.c tools.c elgamal.c reg_page.c 


clean:
	${RM} client server *.d *.o

remove:
	${RM} saved_users/Pipes/*[0-9]* saved_users/Users/*[0-9]*
	${RM} .user

bigclean:
	${RM} client server *.d *.o
	${RM} saved_users/Pipes/*[0-9]* saved_users/Users/*[0-9]*
	${RM} .files/.user .files/contacts/*[0-9]*

.PHONY: all clean remove bigclean
