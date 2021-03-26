CC=gcc
CFLAGS= -Wall -Wextra -Werror -g -pthread

all: server client

client: client.c saved_users/users.c

server: server.c saved_users/users.c

clean:
	${RM} -f client server
