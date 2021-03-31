CC=gcc
CFLAGS= -Wall -Wextra -fsanitize=address -g -pthread
LDLIBS= -ljson-c
#CXXFLAGS= -Inetworking -Isaved_users
VPATH=networking:saved_users:messages

all: server client

server: server.c users.c messages.c

client: client.c users.c messages.c


clean:
	${RM} client server

.PHONY: clean
