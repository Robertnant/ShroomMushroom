CC=gcc
CFLAGS= -Wall -Wextra -fsanitize=address -g -lm -pthread
LDLIBS= -ljson-c
#CXXFLAGS= -Inetworking -Isaved_users
VPATH=networking:saved_users:messages:security

all: server client

server: server.c users.c messages.c tools.c elgamal.c

client: client.c users.c messages.c tools.c elgamal.c


clean:
	${RM} client server

.PHONY: clean
