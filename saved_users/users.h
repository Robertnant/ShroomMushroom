#ifndef USERS_H
#define USERS_H

#define USERS_FILE "saved_users/Users"
#define PIPES_FILE "saved_users/Pipes"

#include "../security/elgamal.h"

#define MAX_BUFF 87

struct user_priv
{
    char a[MAX_BUFF];
    char q[MAX_BUFF];
};

struct user_pub
{
    char g[MAX_BUFF];
    char q[MAX_BUFF];
    char h[MAX_BUFF];
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

char* user_to_string(struct user* user, size_t * l);

struct user * get_contact(char number[]);

#endif
