CC=gcc
CFLAGS= -Wall -Wextra -Werror -g -pthread
#CXXFLAGS= -Inetworking -Isaved_users
VPATH=networking:saved_users

all: server client

server: server.c users.c

client: client.c users.c


clean:
	${RM} -f client server

.PHONY: clean
