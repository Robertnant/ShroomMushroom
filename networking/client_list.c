#include <stdlib.h>
#include "../saved_users/users.h"
#include "client_list.h"


struct client * get_sentinel()
{
    static struct client * sentinel = NULL;
    if (!sentinel)
    {
        sentinel = (struct client *) malloc(sizeof(struct client));
        sentinel->next = NULL;
        sentinel->prev = NULL;
        sentinel->fd = -1;
        sentinel->user = (struct user*) malloc(sizeof(struct user));
    }
    return sentinel;
}

void free_clients(struct client* sentinel)
{
    if(sentinel->next)
        free_clients(sentinel->next);
    free(sentinel->user);
    free(sentinel);
}

void free_client(struct client* client)
{
    // pthread_mutex_lock(&mutex);
    // Remove the current client from the list
    struct client* prev = client->prev;
    struct client* next = client->next;
    prev->next = next;
    if (next)
        next->prev = prev;
    free(client->user);
    free(client);
    // pthread_mutex_unlock(&mutex);
}

