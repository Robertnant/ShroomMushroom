#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

struct client
{
    int fd;
    struct user* user;
    struct client* next;
    struct client* prev;
};

struct client * get_sentinel();

void free_clients(struct client* sentinel);

void free_client(struct client* client);

#endif
