#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>

#include "fileName.h"

#define PORT 8080

int client_fd = -1, k;
char c;

void signalHandler()
{
    /*
        This is the defined code i.e when the interrupt is called using CTRL + C then this function is called. 
    */
    if (client_fd < 0)
    {
        printf("Client Stooped\n");
        exit(1);
    }
    else
    {
        printf("\nSummary of the conversation : \n");
        fclose(fp[k]);

        fp[k] = fopen(fN[k].fname, "r");
        c = fgetc(fp[k]);
        while (c != EOF)
        {
            printf("%c", c);
            c = fgetc(fp[k]);
        }
        fclose(fp[k]);
        remove(fN[k].fname);

        close(client_fd);
        printf("\n[-]Disconnected from server.\n");
        exit(1);
    }
}

int main()
{
    int chk = 0, i = 0;
    char buffer[1024], cliTime[100], serTime[100];
    char msg[500], Msg[500];
    struct sockaddr_in servaddr;

    k = getpid();

    sprintf(fN[k].fname, "%d", k);
    strcat(fN[k].fname, ".txt");


    fp[k] = fopen(fN[k].fname, "w");
    if (fp[k] == NULL)
    {
        printf("Client Didn't Respond\n");
        exit(1);
    }

    // Creating socket file descriptor
    if ((client_fd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("10.0.0.1");

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
            fclose(fp[k]);

            fp[k] = fopen(fN[k].fname, "r");
            c = fgetc(fp[k]);
            while (c != EOF)
            {
                printf("%c", c);
                c = fgetc(fp[k]);
            }
            fclose(fp[k]);
            remove(fN[k].fname);

            close(client_fd);
            printf("\n[-]Disconnected from server.\n");
            exit(1);
        }
        printf("You : %s\n", msg);
        printf("Server is Tying ......\n");

        fputs("CLIENT : ", fp[k]);
        fputs(msg, fp[k]);
        fputs("\n\t", fp[k]);

        clock_t begin = clock();

        n = recvfrom(client_fd, (char *)cliTime, 100,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        cliTime[n] = '\0';

        n = recvfrom(client_fd, (char *)buffer, 1024,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        buffer[n] = '\0';

        n = recvfrom(client_fd, (char *)serTime, 100,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        serTime[n] = '\0';
        clock_t end = clock();
        double delay = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("Server : %s", buffer);
        
	printf("Response Time : %f ms\n\n", delay);
	
        fputs("Message sent at : ", fp[k]);
        fputs(cliTime, fp[k]);
        fputs("\n", fp[k]);
        fputs("SERVER : ", fp[k]);
        fputs(buffer, fp[k]);
        fputs("\tMessage sent at : ", fp[k]);
        fputs(serTime, fp[k]);
        fputs("\n--------------------------------------------------\n", fp[k]);

        

        signal(SIGINT, signalHandler);
    }
    return 0;
}
