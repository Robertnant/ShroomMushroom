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

#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

struct message *message;

// TODO: Uncomment once request handling is implemented on server.
/* Request receiver's public keys for encryption.
struct publicKey *requestKey(struct message *message, int sockfd)
{
    // Create request.
    // (Make request as JSON using ADD).

    // Send query to server.
    if (write(sockfd, query, strlen(json)) == -1)
        errx(1, "Write error occured");

    free(query);

    // Read response and parse public key.
    // (Size shouldn't exceed 150 bytes).
    // Might need to use MAX instead but
    // only after increase MAX value.
    char buffer[150];
    if (read(sockfd, buffer, 150) == -1)
        errx(1, "Read error occured");

    // TODO: Implement this return function.
    // (Sergio is coding this function).
    return parseKey(buffer);
}
*/

// Temporary public key request function. Hardcoded keys.
// (These harcoded keys will be temporary used by server as well).
char *requestKeyTmp(struct message *message, int sockfd)
{
    char *key = calloc(120, sizeof(char));
    strcpy(key, "288106915-100000000000288106913-60547751503877636969");

    return key;
}

void func(int sockfd, struct message *message) 
{ 
    char buff[MAX]; 
    int n;
   

    for (;;) 
    { 
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : "); 
        n = 0;

        // Add text to buffer till newline is written.
        while (n < MAX && (buff[n++] = getchar()) != '\n')
            ;

        // Check for exit signal.
        if ((strncmp(buff, "exit", 4)) == 0)
        {
            printf("Client exited...\n"); 
            break;
        }

        // Step 0: Server asks for receiver and type (this is will be done with GTK).
        // of message that will be sent by user.
        // char *receiver = "Sergio";

        // Step 1: Get receiver's public key (hard coded for now).
        message->receiver = "077644562";
        char *key = requestKeyTmp(message, 2);

        printf("Requested key: %s\n", key);

        struct publicKey *receiver_keys = stringtoPub(key);

        // Step 2: Encrypt message.
        struct cyphers *cyphers= malloc(sizeof(struct cyphers));

        printf("g -> %s\n", receiver_keys -> g);
        printf("q -> %s\n", receiver_keys -> q);
        printf("h -> %s\n", receiver_keys -> h);

        encrypt_gamal(buff, receiver_keys, cyphers);

        // Free receiver public key.
        free(key);
        free(receiver_keys);

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

        printf("JSON: %s\n", json);
        
        // Step 4: Send JSON to server.
        write(sockfd, json, jsonSize); 
        
        // Free json.
        free(json);

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

void init_procedure(int fd, char username[], char number[])
{
    struct user* user = init_user_path(username, number, ".user");
    char* buf;
    int n;
    if ((n = asprintf(&buf, "%s %s %s", user->username, user->number, user->UID)) < 1)
        errx(1, "Weird error sending generated user data");
    // 
    write(fd, buf, n);
    free(user);
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
        struct user* user = get_user_path(".user");
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

        // TODO: This is a test (user was not freed).
        free(user);
    }
    else
    {
        printf("USER CONFIGURATION NOT FOUND!\n");
        //fetch user and number data to run through this function
        char *username = "sergiombd";
        char number[] = "0776727908";         
        init_procedure(sockfd, username, number);
    }


    func(sockfd, message); 

    // Free message structure.
    free(message);

    // Close socket.
    close(sockfd); 
} 
