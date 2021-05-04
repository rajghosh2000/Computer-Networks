#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include "fileName.h"

#define PORT 8080



int main()
{
    int client_fd, chk = 0, i = 0;
    char buffer[1024],buff[1024],c,f[1];
    char msg[500], Msg[500];
    struct sockaddr_in servaddr;

    int index = getpid();
    
    sprintf(fN[index].fname,"%d",index);
    strcat(fN[index].fname,".txt");

    printf("%s\n",fN[index].fname);

    fp[index] = fopen(fN[index].fname,"w");
    if(fp[index] == NULL)
    {
        printf("Client Didn't Respond\n");
        exit(1);
    }


    // Creating socket file descriptor
    if ((client_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n, len;
    while (1)
    {
        printf("Enter the msg to send to client : ");
        scanf("%[^\n]%*c", msg);
        sendto(client_fd, (const char *)msg, strlen(msg),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        if (strcmp(msg, "exit") == 0)
        {
            printf("\nSummary of the conversation : \n");
            fclose(fp[index]);

            fp[index] = fopen(fN[index].fname, "r");
            c = fgetc(fp[index]);
            while (c != EOF)
            {
                printf("%c", c);
                c = fgetc(fp[index]);
            }
            fclose(fp[index]);
            remove(fN[index].fname);

            close(client_fd);
            printf("\n[-]Disconnected from server.\n");
            exit(1);
        }
        printf("You : %s\n", msg);

        fputs("CLIENT : ",fp[index]);
        fputs(msg,fp[index]);
        fputs("\n",fp[index]);


        printf("Server is Tying ......\n");

        clock_t begin = clock();
        
        n = recvfrom(client_fd, (char *)buffer, 1024,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        buffer[n] = '\0';
        clock_t end = clock();
        double delay = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Server : %s\n", buffer);

        fputs("SERVER : ",fp[index]);
        fputs(buffer,fp[index]);

        printf("Response Time : %f ms\n\n", delay);
    }
    return 0;
}
