#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <err.h>

#include "../saved_users/users.h"
#include "../messages/messages.h"
#include "../security/elgamal.h"

#define MAX 10000 
#define PORT 8080 
#define SA struct sockaddr 
#define MESSAGE_SIZE sizeof(struct message)

int running = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


struct message* message; // = (struct message*) malloc(MESSAGE_SIZE);


//#############################################################################
//  CLIENT LIST (to put in a separare file? NO BECAUSE WE NEED GLOBAL MUTEX)
//#############################################################################

struct client
{
    int fd;
    struct user* user;
    struct client* next;
    struct client* prev;
};

struct client * get_sentinel()
{
    static struct client * sentinel = NULL;
    if (!sentinel)
    {
        sentinel = (struct client *) malloc(sizeof(struct client));
        sentinel->next = NULL;
        sentinel->prev = NULL;
        sentinel->fd = -1;
        sentinel->user = (struct user*) malloc(sizeof(struct user));
    }
    return sentinel;
}

void free_clients(struct client* sentinel)
{
    if(sentinel->next)
        free_clients(sentinel->next);
    free(sentinel->user);
    free(sentinel);
}

void free_client(struct client* client)
{
    pthread_mutex_lock(&mutex);
    // Remove the current client from the list
    struct client* prev = client->prev;
    struct client* next = client->next;
    prev->next = next;
    if (next)
        next->prev = prev;
    free(client->user);
    free(client);
    pthread_mutex_unlock(&mutex);
}

//#############################################################################

void end_connection()
{
    int sockfd; 
    struct sockaddr_in servaddr;
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) 
    { 
        printf("Socket creation for end signaling failed..\n"); 
        exit(0); 
    } 
    bzero(&servaddr, sizeof(servaddr)); 

    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 

    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) 
    { 
        printf("Ending connection with the server failed...\n"); 
        exit(0); 
    } 
} 


void interrupt(int err)
{
    running = 0;
    end_connection();
    pthread_mutex_lock(&mutex);
    free_clients(get_sentinel());
    pthread_mutex_unlock(&mutex);
    freeMessage(message);
    printf("Program interrupted with error %d\n", err);
    exit(0);
}


void send_to(char number[], char buffer[], size_t buf_size)
{
    int fd;
    
    struct user* user = get_user(number);
    
    char* filename = get_filename(PIPES_FILE, user->UID);

    fd = open(filename, O_WRONLY);
    free(user);
    free(filename);

    write(fd, buffer, buf_size);

    close(fd);
}


//void send_new_user();

void * listen_to_client( void * arg )
{
    int * sockfd = (int *) arg;
    char buff[MAX]; 
    int er;
    printf("Listening to client...\n");

    //char num[] = "0776727908";
    //char message[] = "HelloWorld!";
    for (;;) 
    { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        while((er = read(*sockfd, buff, sizeof(buff))) > 0)
        {
            printf("From client: ");
            printf("%s\n", buff);
            //send_to(num, buff, err);
            bzero(buff, MAX); 
        }

        // Add newline.
        printf("\n");

        if(er < 1)
            return NULL;
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) 
        { 
            printf("Server Exit...\n"); 
            return NULL; 
        } 
    }
    return NULL;
}

void connect_client(char pipe[], int client)
{
    char * filename = get_filename(PIPES_FILE, pipe);
    int fd = open(filename, O_RDONLY | O_NONBLOCK);
    if (fd < 0)
        errx(1, "Couldn't open pipe for client redirection");
    free(filename);
    //printf("FD IS %d\n", fd);
    if (dup2(fd, client) < 0)
        errx(1, "Could not forward data to client");
}

struct user* parseUser(char string[])
{
    struct user* res = (struct user *) malloc(sizeof(struct user));
    
    char *token = strtok(string, " ");
    strcpy(res->username, token);

    token = strtok(NULL, " ");
    strcpy(res->number, token);
    
    token = strtok(NULL, " ");
    strcpy(res->UID, token);
    
    token = strtok(NULL, " ");
    res->pub = stringtoPub(token);

    return res;
}



int main()
{
    char user[] = "sergiombd";
    char number[] = "0776727908";
    free(init_user(user, number));

    message = (struct message*) malloc(sizeof(struct message));
    //printf("address of message -> %p\n", message);


    int sockfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) 
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    
    // Let server unbind from port after connection
    int value = 1;
    int err = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
    if (err == -1)
        printf("Could not set port to unbind after connection\n");

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) 
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");




    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) 
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);


    struct client* sentinel = get_sentinel();
    struct client* curr = sentinel->next;
    signal(SIGINT, interrupt);

    char buf[1024];
    //struct user* tmp_user = (struct user*) malloc(sizeof(struct user));
    struct user* tmp_user;

    while(running)
    {
        // Accept the data packet from client and verification
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (!running)
            return 0;
        if (connfd < 0) 
        {
            printf("Server acccept failed...\n");
            exit(0);
        }
        else
            printf("Server acccept the client...\n");

        
        // Get through JSON UID and number to identify you
        
        memset(buf, 0, 1024 * sizeof(char));
        
 
        int r;
        r = read(connfd, buf, 1024);
        if (r <= 0)
            errx(1, "Error with identification process");
        
        parseMessage(buf, message);
        // printStruct(message);
        switch (message->type)
        {
            case IDENTIFICATION:
                if(strcmp(message->sender,"(null)") != 0)
                {   
                    tmp_user = get_user(message->sender);

                    if (tmp_user!=NULL && strcmp(tmp_user->UID, message->content) == 0)
                        printf("USER %s IDENTIFIED SUCCESSFULLY\n", tmp_user->username);
                    else
                    {
                        printf("FRAUD DETECTED OR NONE EXSISTING USER\n");
                        close(connfd);
                        continue;
                    }
                }
                else
                    printf("Weird error: couldn't read message\n");
                break;


            case INIT:
                printf("Starting init procedure..\n");
                tmp_user = parseUser(message->content);

                break;


            default:
                printf("MESSAGE NOT RECOGNIZED!\n");
                continue;
            
        }
        // Free user.
        // TODO: Remove this after Sergio implements function to handle this.
        //free(tmp_user);
        

        struct client* user = (struct client*) malloc(sizeof(struct client));
        sentinel->next = user;
        user->prev = sentinel;
        
        if (curr)
            curr->prev = user;
        user->next = curr;
        curr = user;

        curr->fd = connfd;
        curr->user = tmp_user;
        

        // connect_client(curr->user->UID, curr->fd);
        // implement functions to get username, UID, and password
        // we'll need to get user from phone number and verify if UID matches (security)
        // 
         
        //struct user* real_user = get_user(number); // do that instead of the thing below


        /*
        // Might need to literally copy the memory
        user->user->username = ;
        user->user->UID = ;
        user->user->number = ;
        */
        
        pthread_t id;
        pthread_create(&id, NULL, listen_to_client, &(user->fd));
        


        // To uncomment whenever we create the user identification procedure
        // connect_client(user->UID, user->fd);
        
        printf("New user connected!\n");
    }
}
