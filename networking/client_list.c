#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
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
    printf("Removing from list\n");
    struct client* prev = client->prev;
    struct client* next = client->next;
    prev->next = next;
    if (next)
        next->prev = prev;
    printf("Cancelling thread sending\n");
    pthread_cancel(client->sending);
    printf("Cancelling thread listening\n");
    pthread_cancel(client->listening);
    printf("Freeing memory\n");
    free(client->user);
    free(client);
    // pthread_mutex_unlock(&mutex);
}

int is_connected(struct client* sent, char UID[])
{
    if (!sent)
        return 0;
    if (strcmp(UID, sent->user->UID) == 0)
        return 1;
    else
        return is_connected(sent->next, UID);
}

