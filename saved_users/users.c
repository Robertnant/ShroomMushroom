#define _GNU_SOURCE

#include <stdio.h>
#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "users.h"



unsigned int hash(char *key)
{
    unsigned int hash = 0;
    while (*key) 
    {
        hash += *key;
        hash += hash << 10;
        hash ^= hash >> 6;
        key++;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

char * to_string(unsigned int key)
{
    char * res;
    asprintf(&res, "%u", key);
    return res;
}

char* get_id(char num[])
{
    // 07 76 72 79 08
    if(num[0] != '0' || strlen(num) != 10)
        errx(1, "Wrong phone number format!");
    return to_string(hash(num));
}

char * get_filename(char directory[], char filename[])
{
    char* res;
    asprintf(&res, "%s/%s", directory, filename);
    return res;
}


void save_user(struct user * user)
{
    char *file_name = get_filename(USERS_FILE, user->number);
    char *pipe_name = get_filename(PIPES_FILE, user->UID);

    printf("FILE NAME: %s\n", file_name);
    FILE* user_file = fopen(file_name, "wb");
    if (user_file == NULL)
        errx(1, "Couldn't open file");
    fwrite(user, sizeof(struct user), 1, user_file);
    fclose(user_file);

    mkfifo(pipe_name, 0666);

    free(file_name);
    free(pipe_name);
}

struct user* get_user(char number[])
{
    char* filename = get_filename(USERS_FILE, number);
    FILE* user_file = fopen(filename, "r");
    free(filename);
    if (!user_file)
        errx(3, "USER DOES NOT EXIST!");
    struct user* user = (struct user*) malloc(sizeof(struct user));
    fread(user, sizeof(struct user), 1, user_file);

    return user;
}

//public key variable needs to be added 
struct user* init_user(char username[], char number[])
{
    struct user* new =  malloc(sizeof(struct user));
    if (new == NULL)
    {
        errx(2, "Could not create new user structure!");
    }
    char * id = get_id(number);
    strcpy(new->username, username);
    strcpy(new->number, number);
    strcpy(new->UID, id);
    free(id);

    save_user(new);
    //free(new);
    return new;
}


/*
int main()
{
    init_user("sergiombd", "0776727908");

    return 0;
}
*/
