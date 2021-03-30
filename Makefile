CC=gcc
CFLAGS= -Wall -Wextra -Werror -g -pthread

all: server client

server: networking/server.c saved_users/users.c

client: networking/client.c saved_users/users.c


clean:
	${RM} -f client server

.PHONY: clean
