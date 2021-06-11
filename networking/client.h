#ifndef CLIENT_H
#define CLIENT_H

#include "../saved_users/users.h"

int sockfd;
struct user* user;
struct user* target_user;
struct message *message;
struct publicKey *receiver_keys;
struct privateKey *privkey;

int addContact(int fd, char number[]);

struct user* init_procedure(int fd, char username[], char number[], char avatar[]);
char *requestKey(struct message *message, int sockfd);

#endif
