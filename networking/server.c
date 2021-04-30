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
#include "../security/tools.h"
#include "client_list.h"

#define MAX 10000
#define PORT 8080
#define SA struct sockaddr
#define MESSAGE_SIZE sizeof(struct message)

int running = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


struct message* message; // = (struct message*) malloc(MESSAGE_SIZE);


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
    pthread_mutex_unlock(&mutex);
    free(message);
    //freeMessage(message);
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

    for (;;) 
    { 
        bzero(buff, MAX); 
 
        // read the message from client and copy it in buffer 
        while((er = read(*sockfd, buff, MAX)) > 0)
        {
            // If msg contains "Exit" then server exit and chat ended. 
            if (strncmp("exit", buff, 4) == 0) 
            { 
                printf("Server Exit...\n"); 
                return NULL; 
            }

            // printf("From client: ");
            parseMessage(buff, message);

            printf("Sender: %s\n", message->sender);
            printf("Receiver: %s\n", message->receiver);
            printf("Type: %d\n", message->type);

            struct user* receiver = get_user(message->receiver);
            int file_fd;
            
            char * user_message;
            size_t l;

            switch (message->type)
            {
                case TEXT:
                    file_fd = open(receiver->UID, O_WRONLY);
                    write(file_fd, buff, er);
                    close(file_fd);
                    break;
                
                case ADD:
                    // get pub key and user
                    // send the to "sender"
                    user_message = user_to_string(receiver, &l);
                    write(*sockfd, user_message, l);
                    break;

                case IDENTIFICATION: case INIT:
                    printf("Attempting to identify/register at wrong moment!\n");
                    break;
                
                default:
                    printf("Feature not implemented yet!\n");
                    break;

            }
            // Send message back to receiver.
            // (For now send back to client).
            
            //send_to(num, buff, err);
            bzero(buff, MAX);
            freeMessage(message);
        }

        // Add newline.
        printf("\n");

        if(er < 1)
            return NULL;
  
         
    }

    // Free memory.
    //freeMessage(message);
    free(message);

    return NULL;
}

void connect_client(char pipe[], int client)
{
    char buf[MAX];
    char * filename = get_filename(PIPES_FILE, pipe);
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        errx(1, "Couldn't open pipe for client redirection");
    free(filename);
    //printf("FD IS %d\n", fd);
    //if (dup2(fd, client) < 0)
    //    errx(1, "Could not forward data to client");
    int r;
    while (1)
    {
        while ((r = read(fd, buf, MAX)) > 0)
            write(client, buf, r);
    }
}

void * sending_from_pipe(void * arg)
{
    struct client* user = arg;
    connect_client(user->user->UID, user->fd);
    return NULL;
}


int main()
{

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
                save_user(tmp_user);
                break;


            default:
                printf("MESSAGE NOT RECOGNIZED!\n");
                continue;
            
        }
        freeMessage(message);
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
        
        //pthread_t id;
        pthread_create(&curr->listening, NULL, listen_to_client, &(user->fd));
        pthread_create(&curr->sending, NULL, listen_to_client, &(user->fd));
        


        // To uncomment whenever we create the user identification procedure
        // connect_client(user->UID, user->fd);
        
        printf("New user connected!\n");
    }
}
