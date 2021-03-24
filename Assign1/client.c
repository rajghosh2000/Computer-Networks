#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 5000

int main(int argc , char *argv[])
{
    int client_fd;
    struct sockaddr_in server_add;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Client : socket retrieve success\n");

    server_add.sin_family = AF_INET;
    server_add.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &server_add.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 


    if (connect(client_fd, (struct sockaddr *)&server_add, sizeof(server_add)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    int choice,qty,recVal = 0;
    char readArr[1000],toSend[50],part[7],toStr[10];
    strcpy(toSend,"F0");
    printf("\n--------------------- Welcome to Fruit store --------------------");
    printf("\n--------- Items Available ------------ ");
    printf("\n 1) APPLE\n 2) BANANA\n 3) GRAPES\n 4) ORANGES\n 5) CUCUMBER\n---------- SELECT A FRUIT TO BUY (Press the serial number to buy the fruit) :");
    scanf("%d",&choice);
    while(choice >6)
    {
        printf("Err!!! Enter a number from the list above between 1 to 5\n");
        printf("Enter your choice again : ");
        scanf("%d",&choice);
        printf("\n");
    }

    printf("\nOuantity Required : ");
    scanf("%d",&qty);

    sprintf(part, "%d", choice);
    sprintf(toStr,"%d",qty);

    strcat(part,toStr);
    strcat(toSend,part);
    if ((send(client_fd,toSend,strlen(toSend),0))== -1) 
    {
        fprintf(stderr, "\nMessage not Send\n");
        close(toSend);
    }
    printf("Processing.................\n");

    recVal = read(client_fd,readArr,sizeof(readArr));
    readArr[recVal]='\0';
    if (recVal <= 0)
    {
      printf("Connection may be lost\n");
      exit(1);
    }
    if(strcmp(readArr,"2")==0)
    {
        printf("Thanking You For Buying From Us. Visit Again\n");
    }
    else if(strcmp(readArr,"1")==0)
    {
        printf("Sorry but quantity is insufficient. Kindly visit later\n");
    }
    close(client_fd);
    return 0;
}