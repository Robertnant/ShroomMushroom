#define _GNU_SOURCE

#include <stdio.h>
#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "users.h"
#include "../security/elgamal.h"
#include "../security/tools.h"


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
    if(strlen(num) != 10)
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
    // Create user and pipe paths.
    char *file_name = get_filename(USERS_FILE, user->number);
    char *pipe_name = get_filename(PIPES_FILE, user->UID);

    // Save user and pipe.
    FILE* user_file = fopen(file_name, "wb");
    if (user_file == NULL)
        errx(1, "Couldn't open file");

    fwrite(user, sizeof(struct user), 1, user_file);
    fclose(user_file);

    mkfifo(pipe_name, 0666);

    // Free memory.
    free(file_name);
    free(pipe_name);
}


void save_user_path(struct user * user, char * path)
{
    // Save user file
    FILE* user_file = fopen(path, "wb");
    if (user_file == NULL)
        errx(1, "Couldn't open file");

    fwrite(user, sizeof(struct user), 1, user_file);
    fclose(user_file);
}

struct user* get_user(char number[])
{
    char* filename = get_filename(USERS_FILE, number);
    FILE* user_file = fopen(filename, "r");
    free(filename);
    if (!user_file)
        return NULL;
    //errx(3, "USER DOES NOT EXIST!");

    struct user* user = (struct user*) malloc(sizeof(struct user));
    fread(user, sizeof(struct user), 1, user_file);

    // Close file.
    fclose(user_file);

    return user;
}

struct user* get_user_path(char path[])
{
    FILE* user_file = fopen(path, "r");
    if (!user_file)
        return NULL;

    struct user* user = (struct user*) malloc(sizeof(struct user));
    
    if (!user)
        return NULL;
    
    fread(user, sizeof(struct user), 1, user_file);

    // Close file.
    fclose(user_file);
    
    return user;
}


struct user* get_contact(char number[])
{
    char * path = get_filename(".files/contacts", number);
    struct user* res = get_user_path(path);
    free(path);
    return res;
}


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


struct user* parseUser(char string[])
{
    struct user* res = (struct user *) calloc(1, sizeof(struct user));
    
    char *token = strtok(string, " ");
    strcpy(res->username, token);

    token = strtok(NULL, " ");
    strcpy(res->number, token);
    
    token = strtok(NULL, " ");
    strcpy(res->UID, token);
    
    token = strtok(NULL, " ");
    
    // Parsing the public key
    token = strtok(token, "-");
    strcpy(res->pub.g, token);

    token = strtok(token, "-");
    strcpy(res->pub.q, token);
    

    token = strtok(token, "-");
    strcpy(res->pub.h, token);
    
    // res->pub = stringtoPub(token);

    return res;
}

char* user_to_string(struct user* user, size_t * l)
{
    char * res;
    size_t tmp;
    if (user)
        tmp = asprintf(&res, "%s %s %s %s-%s-%s", user->username, user->number, user->UID,
            user->pub.g, user->pub.q, user->pub.h);
    else
        tmp = asprintf(&res, "(null)");

    if (l)
        *l = tmp;
    return res;
}


//public key variable needs to be added 
struct user* init_user_path(char username[], char number[], char path[])
{
    struct user* new =  calloc(1, sizeof(struct user));
    if (new == NULL)
    {
        errx(2, "Could not create new user structure!");
    }
    char * id = get_id(number);
    strcpy(new->username, username);
    strcpy(new->number, number);
    strcpy(new->UID, id);
    free(id);

    publicKey* pubKey = (publicKey *) malloc(sizeof(publicKey));
    privateKey* privKey = (privateKey *) malloc(sizeof(privateKey));

    generateKeys(pubKey, privKey);
    printf("Generated new ElGamal key pair\n");
    char *a_str = largenum_string(privKey->a);
    char *q_str = largenum_string(privKey->q);
    printf("Generated keys pub -> %s-%s-%s and priv -> %s-%s\n", 
            pubKey->g, pubKey->q, pubKey->h, a_str, q_str);

    // Free strings.
    free(a_str);
    free(q_str);

    // Save public keys.
    strcpy(new->pub.g, pubKey->g);
    strcpy(new->pub.q, pubKey->q);
    strcpy(new->pub.h, pubKey->h);
    
    // Save private keys.
    char *a = largenum_string(privKey->a); 
    strcpy(new->priv.a, a);
    strcpy(new->priv.q, pubKey->q);
    
    // Free memory.
    free(a);
    freeKeys(pubKey, privKey);

    save_user_path(new, path);
    printf("Saved user info for %s into .user file\n", new->username);

    //free(new);
    return new;
}

