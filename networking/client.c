#define _GNU_SOURCE
#include <unistd.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <err.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../messages/messages.h"
#include "../saved_users/users.h"
#include "../security/elgamal.h"
#include "../security/tools.h"
#include "../design/Registration/reg_page.h"
#include "../design/Main/interface_full.h"
#include "../design/addcontact/add_contact.h"
#include "client.h"

#define USER_PATH ".files/.user"
#define CHAT_PATH "design/Main/chat.txt"
#define INTERFACE_PATH "design/Main/interface_full.glade"
#define CONTACTS_PATH ".files/contacts.txt"
#define MAX_BUFFER 10000
#define PORT 8080
#define SA struct sockaddr 

// TODO: 1. Make step 0 of "func" method use input from GTK text entry.
// TODO: 2. Make "saveMessage" function use client username.
// TODO: 3. Make new "getReceiverInfo" function in GTK file which will be called when 
//          the interface contact button for the user will be selected.
//          Return type: either struct user or struct *publickey.

// Temporary public key request function. Hardcoded keys.
// (These harcoded keys will be temporary used by server as well).
char *requestKey(struct message *message, int sockfd)
{
    if (sockfd)
        printf("\n");

    printf("Requesting public key of: %s\n", message->receiver);
    //struct user *user = get_user_path(USER_PATH);
    struct user_pub *pub = &(target_user->pub);

    printf("Target user is %s (%s)\n", target_user->username, target_user->number);
    
    // Get public key from user file.
    struct publicKey *pubkey = malloc(sizeof(struct publicKey));
    pubkey->g = pub->g;
    pubkey->q = pub->q;
    pubkey->h = pub->h;
    
    char *key = pubtoString(pubkey);

    // Free memory.
    //free(pubkey);
    //free(user);

    return key;
}

int exists(char filename[])
{
    return (access(filename, F_OK ) == 0);
}

struct user* init_procedure(int fd, char username[], char number[])
{
    struct user* user = init_user_path(username, number, USER_PATH);
    struct message* tmp_msg = (struct message *) calloc(1, sizeof(struct message));
    char* buf;

    int n;
    tmp_msg->type = INIT; 
    if ((n = asprintf(&tmp_msg->content, "%s %s %s %s-%s-%s", user->username,\
                    user->number, user->UID,\
                    user->pub.g, user->pub.q, user->pub.h)) < 1)
        errx(1, "Weird error sending generated user data");
    int l;
    buf = genMessage(tmp_msg, &l);

    rewrite(fd, buf, l);
    free(buf);
    freeMessage(tmp_msg);
    free(tmp_msg);

    return user;
}

int addContact(int fd, char number[])
{
    // Sending the request
    message->type = ADD;
    message->sender = user->number;
    message->receiver = number;
    message->filename = NULL;
    message->content = NULL;
    message->size = 0;
    message->time = NULL;

    int l;    
    char * mess = genMessage(message, &l);
    
    rewrite(fd, mess, l);
    free(mess);

    // Waiting for a response
    char buf[MAX_BUFFER];
    read(fd, buf, MAX_BUFFER);
    printf("BUFFER => %s\n", buf);
    //parseMessage(buf, message);
    
    if (strcmp(buf, "(null)") == 0)
        return 0;

    struct user* new_user = parseUser(buf);
    
    if(!new_user)
        return 0;

    // Saving user into contacts.txt
    FILE * contacts = fopen(".files/contacts.txt", "a");
    fprintf(contacts, "%s-%s\n", new_user->username, new_user->number);
    fclose(contacts); 


    // Save to specific file
    char *path;
    asprintf(&path, ".files/contacts/%s", new_user->number);
    printf("PATH FOR USER: %s\n", path);
    printf("username -> %s ... pub.h -> %s\n", new_user->username, new_user->pub.h);
    save_user_path(new_user, path);
    free(path);
    free(new_user);
    return 1;

}

// Get private key.
void getPrivKey() 
{ 
    struct user_priv *priv = &(user->priv);
    mpz_t a, q;
    mpz_init(a);
    mpz_init(q);
    string_largenum(priv->a, a);
    string_largenum(priv->q, q);

    privkey = malloc(sizeof(struct privateKey));
    mpz_init(privkey->a);
    mpz_init(privkey->q);
    mpz_set(privkey->a, a);
    mpz_set(privkey->q, q);

    // Free memory.
    mpz_clear(a);
    mpz_clear(q);
}

int main() 
{ 
    // Check if user is already initialized, if not create it
    // if initialized, open it and send identification

    struct sockaddr_in servaddr; //, cli; 
    
    // Create and verify socket.
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) 
    { 
        errx(EXIT_FAILURE, "Failed to create socket..."); 
    }
    else
        printf("Socket creation succesful...\n"); 

    bzero(&servaddr, sizeof(servaddr)); 

    // Set IP and PORT.
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 

    // Connect client to server.
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0)
        errx(EXIT_FAILURE, "Server connection failed..."); 
    else
        printf("Connection to server successful...\n"); 

    // Run interface.
    gtk_init(NULL, NULL);

    int registered = 0;

    message = (struct message*) calloc(1, sizeof(struct message));
    if (exists(USER_PATH))
    {
        printf("Starting identification procedure...\n");
        user = get_user_path(USER_PATH);
        // send simple message with UID as content (use the function robert will implement)
        message->type = IDENTIFICATION;
        message->content = user->UID;
        message->sender = user->number;

        int l;
        char *msg = genMessage(message, &l);
        rewrite(sockfd, msg, l);
        free(msg);
        printf("Identification done!\n");
        show_interface(INTERFACE_PATH, CONTACTS_PATH, CHAT_PATH);
        // Get private key.
        getPrivKey();
    }
    else
    {
        printf("USER CONFIGURATION NOT FOUND!\n");
        //fetch user and number data to run through this function
        struct registration_data *reg_data = malloc(sizeof(struct registration_data));
        show_registration(reg_data);
        registered = 1;
    }
    

    
    // Display registration page.
    gtk_main();
    

    if (registered)
    {
        // Get private key.
        getPrivKey();
        
        // Display main interface.
        show_interface(INTERFACE_PATH, CONTACTS_PATH, CHAT_PATH);
        gtk_main();
    }

    // Free private key.
    freePriv(privkey);

    // Close socket.
    close(sockfd); 
} 
