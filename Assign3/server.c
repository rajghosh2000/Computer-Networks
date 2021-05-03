#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8080

// Driver code
int main()
{
    int server_fd, acceptClient;
    char buffer[50000];
    char msg[500], dateMsg[500];
    struct sockaddr_in servaddr, cliaddr;
    pid_t childpid;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("[+]Server Socket is created.\n");

    bzero(&servaddr, sizeof(servaddr));
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(server_fd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("[+]Bind to port %d\n", PORT);
    printf("[+] Wating for clients .....\n");

    int len, n, m, conn = 0;
    len = sizeof(cliaddr);
    socklen_t addr_size;
    addr_size = sizeof(struct sockaddr_in);

    while (1)
    {  
        time_t ltime; /* calendar time */
        n = recvfrom(server_fd, (char *)buffer, 5000,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     &len);
        buffer[n] = '\0';
        if (strcmp(buffer, "exit") == 0)
        {
            printf("Disconnected from %s:%d\n\n\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        }
        else
        {   
            ltime = time(NULL); /* get current cal time */
            printf("Client from %s:%d has sent : %s\n\n",inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), buffer);
            printf("Enter the msg to send to client %s:%d  : ",inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            fgets(msg, sizeof(msg), stdin);
            sendto(server_fd, (const char *)msg, strlen(msg),
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                   len);
            printf("You : %s\n", msg);
        }
    }
    close(server_fd);
    return 0;
}