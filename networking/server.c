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
#include <gmodule.h>

#include "../saved_users/users.h"
#include "../messages/messages.h"
#include "../security/elgamal.h"
#include "../security/tools.h"
#include "client_list.h"

#define MAX_BUF_SIZE 100
#define PORT 8080
#define SA struct sockaddr
#define MESSAGE_SIZE sizeof(struct message)

int running = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct client* sentinel;

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


struct pipe_content
{
    char * message;
    size_t size;
    char * number;
};

void * send_to(void* arg)
{
    signal(SIGPIPE, SIG_IGN);
    struct pipe_content* args = arg;
    
    int fd;

    struct user* tmp_target_user = get_user(args->number);
    char* filename = get_filename(PIPES_FILE, tmp_target_user->UID);

    int sent = 0;
    while (!sent)
    {
        while((fd = open(filename, O_WRONLY)) <= 0){sleep(1);}
        sent = rewrite(fd, args->message, args->size);
    }

    printf("MESSAGE SENT SUCCESSFULLY\n");
    
    free(tmp_target_user);
    free(filename);

    free(args->message);
    free(args->number);
    free(args);
    close(fd);
    return NULL;
}


//void send_new_user();

void * listen_to_client( void * arg )
{
    struct client * self = (struct client *) arg;
    int sockfd =  self->fd;
    char buff[MAX_BUF_SIZE]; 
    int er;

    printf("Listening to client...\n");
    for (;;) 
    {
        bzero(buff, MAX_BUF_SIZE);

        GString *json_string = g_string_new(NULL);
         
        int found = 0;
        while((er = read(sockfd, buff, MAX_BUF_SIZE - 1)) > 0)
        {
            json_string = g_string_append(json_string, buff);

            if(g_str_has_prefix(buff, "{"))
            {
                printf("\nfound\n");
                found = 1;
            }
            if(g_str_has_suffix(buff, "}"))
            {
                printf("\nfound2\n");
                found++;
                break;
            }
            else if (!found)
            {
                g_string_free(json_string, TRUE);
                json_string = g_string_new(NULL);
            }
            bzero(buff, MAX_BUF_SIZE);
        }
        if (er <= 0)
        {
            printf("Freeing client space\n");
            free_client(self);
            printf("Closed connection with client\n");
            return NULL;
        }
        if (!found)
        {
            g_string_free(json_string, TRUE);
            continue;
        }
        gchar * final = g_string_free(json_string, FALSE);

        // read the message from client and copy it in buffer 
        //while((er = read(*sockfd, buff, MAX_BUF_SIZE)) > 0)
        //{

        printf("\nFINALLLL: %s\n", final);
        //for (size_t i = 0; i < finalLen; i++)
        //    printf("%c", final[i]);
       
        resetMessage(message);

        parseMessage(final, message);

        if (message->sender)
            printf("Sender: %s\n", message->sender);
        if (message->receiver)
            printf("Receiver: %s\n", message->receiver);
        if (message->type)
            printf("Type: %d\n", message->type);

        struct user* receiver = get_user(message->receiver);

        char * user_message;
        size_t l;
        char *tmp_buf;
        char *tmp_number;
        pthread_t id;
        struct pipe_content* content;

        switch (message->type)
        {
            case TEXT:
                l = strlen(final) + 1;   // Extra 1 for NULL byte.
                
                content = malloc(sizeof(struct pipe_content));
                
                // In theory tmp_buf size should be l-1.
                tmp_buf = malloc(l * sizeof(char)); 
                tmp_number = malloc(11 * sizeof(char));
                
                strcpy(tmp_buf, final);
                strcpy(tmp_number, message->receiver);
                
                content->message = tmp_buf;
                content->number = tmp_number;
                content->size = l;
                
                pthread_create(&id, NULL, send_to, (void*) content);
                
                break;
            
            case ADD:
                // get pub key and user
                // send the to "sender"
                user_message = user_to_string(receiver, &l);
                printf("User message: %s\n", user_message);
                rewrite(sockfd, user_message, l);
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
        bzero(buff, MAX_BUF_SIZE);
        freeMessage(message);

        // Add newline.
        printf("\n");

        g_free(final);
    }

    // Free memory.
    //freeMessage(message);
    free(message);
    
    //printf("Freeing client space\n");
    //free_client(self);
    printf("Closed connection with client\n");
    return NULL;
}

void connect_client(char pipe[], int client)
{
    printf("\nMessage sent to receiving client.\n");

    char buf[MAX_BUF_SIZE];
    char * filename = get_filename(PIPES_FILE, pipe);
    //if (fd < 0)
    //    errx(1, "Couldn't open pipe for client redirection");
    //printf("FD IS %d\n", fd);
    //if (dup2(fd, client) < 0)
    //    errx(1, "Could not forward data to client");
    int r;
    int fd;
    while (!is_connected(sentinel, pipe));
    while (1)
    {
        fd = open(filename, O_RDONLY);
        if (fd > 0)
        {
            while ((r = read(fd, buf, MAX_BUF_SIZE)) > 0)
            {
                printf("Got: \n");
                for (int i = 0; i < r; i++)
                    printf("%c", buf[i]);

                while (fd > 0 && !rewrite(client, buf, r))
                    fd = open(filename, O_RDONLY);
            }

        }
    }

    free(filename);
}

void * sending_from_pipe (void * arg)
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


    sentinel = get_sentinel();
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
        // printf("The buffer: %s\n", buf);
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

        struct client* user = (struct client*) malloc(sizeof(struct client));
        sentinel->next = user;
        user->prev = sentinel;
        
        if (curr)
            curr->prev = user;
        user->next = curr;
        curr = user;

        curr->fd = connfd;
        curr->user = tmp_user;
        

        pthread_create(&curr->listening, NULL, listen_to_client, (void *) user);
        pthread_create(&curr->sending, NULL, sending_from_pipe, (void *) user);
        
        freeMessage(message);
        printf("New user connected!\n");
    }
}
