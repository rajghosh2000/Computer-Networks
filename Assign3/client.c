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

int main()
{
    int client_fd, chk = 0, i = 0;
    char buffer[1024], fN[10], c,f[1];
    char msg[500], Msg[500];
    struct sockaddr_in servaddr;

    FILE *fw, *fr;

    fr = fopen("config.txt", "r");

    while ((c = getc(fr)) != EOF)
    {
        fN[i++] = c;
    }
    fclose(fr);

    int tempName = atoi(fN);

    fr = fopen("config.txt", "w");
    int t = tempName + 1;
    sprintf(f, "%d", t);
    fwrite(&f, sizeof(f), 1, fr);
    fclose(fr);

    strcat(fN, ".txt");

    if ((fw = fopen(fN, "w")) == NULL)
    {
        printf("ERROR IN CLIENT\n");
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
            fclose(fw);

            fw = fopen(fN, "r");
            c = fgetc(fw);
            while (c != EOF)
            {
                printf("%c", c);
                c = fgetc(fw);
            }
            fclose(fw);
            remove(fN);

            close(client_fd);
            printf("\n[-]Disconnected from server.\n");
            exit(1);
        }
        printf("You : %s\n", msg);

        fputs("CLIENT : ",fw);
        fputs(msg,fw);
        fputs("\n",fw);


        printf("Server is Tying ......\n");

        clock_t begin = clock();
        n = recvfrom(client_fd, (char *)buffer, 1024,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        buffer[n] = '\0';
        clock_t end = clock();
        double delay = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Server : %s\n", buffer);

        fputs("SERVER : ",fw);
        fputs(buffer,fw);
        printf("Response Time : %f ms\n\n", delay);
    }
    return 0;
}
