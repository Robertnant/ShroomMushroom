#ifndef CLIENT_H
#define CLIENT_H

#include "../saved_users/users.h"

int sockfd;
struct user* user;

struct user* init_procedure(int fd, char username[], char number[]);

#endif
