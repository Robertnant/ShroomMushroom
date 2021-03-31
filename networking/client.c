#include <unistd.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <err.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../saved_users/users.h"

#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 
void func(int sockfd) 
{ 
    char buff[MAX]; 
    int n;
   
    char *json_test = "{\"sender\": \"123456789\",\
    \"receiver\": \"987654321\",\
    \"type\": 5,\
    \"content\": \"Encrypted message\",\
    \"time\": \"1845689\",\
    \"filename\": \"file.txt\"\
    }";


    size_t len = strlen(json_test);
    write(sockfd, json_test, len);

    for (;;) 
    { 
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : "); 
        n = 0;

        // Add text to buffer till newline is written.
        while (n < MAX && (buff[n++] = getchar()) != '\n')
            ;

        // Send buffer data to server.
        write(sockfd, buff, sizeof(buff)); 

        // bzero(buff, sizeof(buff)); 
        // read(sockfd, buff, sizeof(buff)); 
        // printf("From Server : %s", buff); 

        // Check for exit signal.
        if ((strncmp(buff, "exit", 4)) == 0)
        { 
            printf("Client exited...\n"); 
            break; 
        }
    } 
} 

int main() 
{ 

    struct user* User = init_user("sergiombd", "0776727908");
    printf("Address of user is %p\n", User);

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

    // Chat function. 
    func(sockfd); 

    // Close socket.
    close(sockfd); 
} 
