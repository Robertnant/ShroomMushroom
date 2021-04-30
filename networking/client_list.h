#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H


#include <pthread.h>


struct client
{
    int fd;
    pthread_t listening;
    pthread_t sending;
    struct user* user;
    struct client* next;
    struct client* prev;
};

struct client * get_sentinel();

void free_clients(struct client* sentinel);

void free_client(struct client* client);

#endif
