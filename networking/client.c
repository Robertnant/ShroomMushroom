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

#define USER_PATH ".files/.user"
#define CHAT_PATH "design/Main/chat.txt"
#define MAX_BUFFER 10000
#define PORT 8080
#define SA struct sockaddr 

struct message *message;
struct publicKey *receiver_keys;
struct privateKey *privkey;
struct user* user;
struct user* target_user;
int sockfd; 

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
    struct user *user = get_user_path(USER_PATH);
    struct user_pub *pub = &(user->pub);

    // Get public key from user file.
    struct publicKey *pubkey = malloc(sizeof(struct publicKey));
    pubkey->g = pub->g;
    pubkey->q = pub->q;
    pubkey->h = pub->h;

    char *key = pubtoString(pubkey);

    // Free memory.
    free(pubkey);
    free(user);

    return key;
}

// Function to save message to chat log.
void saveMessage(char *msg, FILE *file)
{
    fprintf(file, "[ROBERT]%s", msg);
}

// Function to send message.
void sendMessage(char *buff)
{
    // Step 1: Get receiver's public key (HARDCODED FOR NOW).
    message->receiver = user->number;   // TODO: "user" should be changed to "target_user"
    char *key = requestKey(message, sockfd);
    receiver_keys = stringtoPub(key);
    printf("Received key: g->%s q->%s h->%s\n", receiver_keys->g, 
            receiver_keys->q, receiver_keys->h);

    // Step 2: Encrypt message.
    printf("\nEncrypting typed message using received key\n");

    struct cyphers *cyphers= malloc(sizeof(struct cyphers));
    encrypt_gamal(buff, receiver_keys, cyphers);

    // Free receiver public key.
    free(key);
    free(receiver_keys);


    // Step 3: Generate JSON with cyphers.
    printf("\nConverting encryption into JSON\n");
    //sleep(0.5);
    
    char * time = malloc(sizeof(char) * 5);
    strcpy(time, "1010");
    char * sender = malloc(sizeof(char) * 11);
    strcpy(sender, "0776727908");
    

    message->type = TEXT;
    message->content = cyphers->en_msg;
    message->p = cyphers->p;
    message->size = cyphers->size;
    message->time = time; //"1010";
    message->sender = sender; //"077644562";
    message->receiver = NULL; 
    message->filename = 0;

    int jsonSize;
    char *json = genMessage(message, &jsonSize);
    

    // Reset message structure for next incoming message.
    freeMessage(message);

    // Step 4: Send JSON to server.
    printf("Sending JSON to server\n");
    //sleep(2);

    int e = write(sockfd, json, jsonSize); 
    if (e == -1)
        errx(1, "Write error");

    // Free memory.
    // free(cyphers->en_msg);
    // free(cyphers->p);
    free(cyphers);
    free(json);
}

// Function to retrieve incoming message.
void retrieveMessage(FILE *contacts)
{
    // Step 5: Receive incoming message from other client.
    // (For now just itself).
    // bzero(json, jsonSize);
    
    int jsonSize = MAX_BUFFER;
    char json[MAX_BUFFER];

    if (read(sockfd, json, jsonSize) == -1)
        errx(1, "Error reading incoming messages");

    parseMessage(json, message);

    printf("\nReceived a message from %s\n", message->sender);
    //sleep(2);

    // free(cyphers);
    struct cyphers *cyphers = malloc(sizeof(struct message)); // WARNING
    cyphers->en_msg = message->content;
    cyphers->p = message->p;
    cyphers->size = message->size;

    // Step 6: Decrypt message and save to chat file.
    char *res = decrypt_gamal(cyphers, privkey);
    printf("Decrypting received message\n");
    printf("Received message: %s\n", res);
    saveMessage(res, contacts);

    // Free memory.
    free(res);
    freeMessage(message);
    free(cyphers);
    // free(json);
}

void func() 
{ 
    // char buff[MAX_BUFFER]; 
    // int n;
    // n = 0;

    // Get private key.
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

    // Open chat file.
    FILE * contacts = fopen(CHAT_PATH, "a");
    if (!contacts)
        errx(EXIT_FAILURE, "Failed to open chat file");

    // Send message.
    char buff[MAX_BUFFER] = "My name is Roberto.\n";
    sendMessage(buff);

    // Receive message.
    retrieveMessage(contacts);

    // Free memory.
    mpz_clear(a);
    mpz_clear(q);
    freePriv(privkey);
    fclose(contacts);
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
    //char* key = pubtoString(user->pub);
    int n;
    tmp_msg->type = INIT; 
    if ((n = asprintf(&tmp_msg->content, "%s %s %s %s-%s-%s", user->username,\
                    user->number, user->UID,\
                    user->pub.g, user->pub.q, user->pub.h)) < 1)
        errx(1, "Weird error sending generated user data");
    int l;
    buf = genMessage(tmp_msg, &l);

    write(fd, buf, l);
    // free(user);
    free(buf);
    freeMessage(tmp_msg);
    free(tmp_msg);
    //free(key);
    return user;
}

void addContact(int fd, char number[])
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
    
    write(fd, mess, l);
    free(mess);

    // Waiting for a response
    char buf[MAX_BUFFER];
    read(fd, buf, MAX_BUFFER);
    parseMessage(buf, message);
    
    if (strcmp(message->content, "(null)") == 0)
        printf("USER NOT FOUND ERROR\n");


    struct user* new_user = parseUser(message->content);
   
    // Saving user into contacts.txt
    FILE * contacts = fopen(".files/contacts.txt", "a");
    fprintf(contacts, "%s-%s\n", new_user->username, new_user->number);
    fclose(contacts); 


    // Save to specific file
    char *path;
    asprintf(&path, ".files/contacts/%s", user->number);
    save_user_path(new_user, path);
    free(path);
    free(user);

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
        write(sockfd, msg, l);
        free(msg);
        printf("Identification done!\n");
    }
    else
    {
        printf("USER CONFIGURATION NOT FOUND!\n");
        //fetch user and number data to run through this function
        struct registration_data *reg_data = malloc(sizeof(struct registration_data));
        show_registration(reg_data);
    }

    gtk_main();
    
    // Chatting function.
    func();

    // Close socket.
    close(sockfd); 
} 
