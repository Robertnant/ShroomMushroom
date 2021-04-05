CC=gcc
CFLAGS= -Wall -Wextra -g -lm -pthread
LDLIBS= -ljson-c
#CXXFLAGS= -Inetworking -Isaved_users
VPATH=networking:saved_users:messages:security

all: server client

server: server.c users.c messages.c tools.c elgamal.c

client: client.c users.c messages.c tools.c elgamal.c


clean:
	${RM} client server

remove:
	${RM} saved_users/Pipes/*[0-9]* saved_users/Users/*[0-9]*
	${RM} .user

.PHONY: clean
