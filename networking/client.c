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

#include "../messages/messages.h"
#include "../saved_users/users.h"
#include "../security/elgamal.h"
#include "../security/tools.h"

#define MAX 10000 
#define PORT 8080 
#define SA struct sockaddr 

struct message *message;
struct user* user;



// Temporary public key request function. Hardcoded keys.
// (These harcoded keys will be temporary used by server as well).
char *requestKeyTmp(struct message *message, int sockfd)
{
    struct user *user = get_user_path(".user");
    struct user_pub *pub = &(user->pub);

    // Get public key from user file.
    struct publicKey *pubkey = malloc(sizeof(struct publicKey));
    pubkey->g = pub->g;
    pubkey->q = pub->q;
    pubkey->h = pub->h;

    printf("Test: %s\n", pub->g);

    char *key = pubtoString(pubkey);

    printf("Received requested key: %s\n", key);

    // Free memory.
    free(pubkey);

    return key;
}

void func(int sockfd, struct message *message) 
{ 
    char buff[MAX]; 
    int n = 1;

    while (n) 
    { 
        n--;
        bzero(buff, MAX); 
        // printf("Enter the string : "); 
        // n = 0;

        // Add text to buffer till newline is written.
        // while (n < MAX && (buff[n++] = getchar()) != '\n')
        //    ;
        
        // scanf("%s", buff);
 
        // Step 0: Server asks for receiver and type (this is will be done with GTK).
        // of message that will be sent by user.
        // char *receiver = "Sergio";

        // Step 1: Get receiver's public key (hard coded for now).
        message->receiver = "077644562";
        // char *key = requestKeyTmp(message, 2);
        // struct publicKey *receiver_keys = stringtoPub(key);
        // New. Old above.
        struct user *user = get_user_path(".user");
        struct user_pub *pub = &(user->pub);
        struct publicKey *receiver_keys = malloc(sizeof(struct publicKey));
        receiver_keys->g = pub->g;
        receiver_keys->q = pub->q;
        receiver_keys->h = pub->h;


        // Step 2: Encrypt message.
        struct cyphers *cyphers= malloc(sizeof(struct cyphers));

        encrypt_gamal("This should work", receiver_keys, cyphers);

        // Step 3: Decrypt own message. TODO Remove
        struct user_priv *priv = &(user->priv);
        uint128_t a = string_largenum(priv->a);
        uint128_t q = string_largenum(priv->q);

        struct privateKey *privkey = malloc(sizeof(struct privateKey));
        privkey->a = a;    
        privkey->q = q;

        printf("Received requested private key: a:%s q:%s\n", 
                largenum_string(a), largenum_string(q));
        
        // char *res = decrypt_gamal(cyphers, privKey);
        // printf("Decryption on client's side: %s\n", res);

        // Free receiver public key.
        // free(key);
        // free(receiver_keys);

        // Step 3: Generate JSON with cyphers (maybe hard code don't know yet).
        message->type = TEXT;
        message->content = cyphers->en_msg;
        message->p = cyphers->p;
        message->size = cyphers->size;
        message->time = "1015";
        message->sender = "Robert";
        // message->receiver = "Sergio";
        message->filename = 0;

        int jsonSize;
        char *json = genMessage(message, &jsonSize);
        
        // Step 4: Send JSON to server.
        int e = write(sockfd, json, jsonSize); 
        if (e == -1)
            errx(1, "Write error");
        else
            printf("Encrypted data (JSON) sent to server: %s\n",
                    json);       
        // Check for exit signal.
        if ((strncmp(buff, "exit", 4)) == 0)
        {
            printf("Client exited...\n"); 
            break;
        }

        
        // TODO Read.
        // (For now the server won't send the data
        // to a receiver. It will have the private key of the "receiver". It will just decrypt and print it).
        // Note: normally the server should never have a private key. But since identification is not done,
        // we can do this for now.

        // Do the opposite for the server.

        
    } 
}

int exists(char filename[])
{
    return (access(filename, F_OK ) == 0);
}

struct user* init_procedure(int fd, char username[], char number[])
{
    struct user* user = init_user_path(username, number, ".user");
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
    //free(message);
    //free(key);
    return user;
}

int main() 
{ 
    // Check if user is already initialized, if not create it
    
    // if initialized, open it and send identification


    int sockfd; 
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


    /*
    char *json_test = "{\"sender\": \"0776727909\",\
    \"receiver\": \"0776727908\",\
    \"type\": 5,\
    \"content\": \"1005838116\",\
    \"time\": \"1845689\",\
    \"filename\": \"file.txt\"\
    }";
    

    //size_t len = strlen(json_test);
    //write(sockfd, json_test, len);
    // Chat function. 
    
    */
    
    message = (struct message*) calloc(1, sizeof(struct message));
    if (exists(".user"))
    {
        printf("Starting identification procedure...\n");
        user = get_user_path(".user");
        // send simple message with UID as content (use the function robert will implement)
        message->type = IDENTIFICATION;
        message->content = user->UID;
        message->sender = user->number;
        //message->receiver[0] = 0;
        //message->time[0] = 0;
        //message->filename[0] = 0;

        int l;
        char *msg = genMessage(message, &l);
        write(sockfd, msg, l);
        free(msg);
        printf("Identification done!\n");

        // TODO: This is a test (user was not freed).
        free(user);
    }
    else
    {
        printf("USER CONFIGURATION NOT FOUND!\n");
        //fetch user and number data to run through this function
        char *username = "sergiombd";
        char number[] = "0776727908";         
        user = init_procedure(sockfd, username, number);
    }


    func(sockfd, message); 

    // Free message structure.

    // Close socket.
    close(sockfd); 
} 
