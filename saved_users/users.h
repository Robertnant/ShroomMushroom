#ifndef USERS_H
#define USERS_H

#define USERS_FILE "saved_users/Users"
#define PIPES_FILE "saved_users/Pipes"

#include "../security/elgamal.h"


struct user_priv
{
    char a[80];
    char q[80];
};

struct user_pub
{
    char g[80];
    char q[80];
    char h[80];
};

struct user
{
    char username[17];
    char number[11];
    char UID[11];
    struct user_priv priv;
    struct user_pub pub;
};

char * get_filename(char directory[], char filename[]);

struct user* get_user(char number[]);

struct user* get_user_path(char path[]);

void save_user(struct user* user);

void save_user_path(struct user * user, char * path);

struct user* parseUser(char string[]);

struct user* init_user(char username[], char number[]);

struct user* init_user_path(char username[], char number[], char path[]);

#endif
