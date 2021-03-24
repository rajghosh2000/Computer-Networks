#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define PORT 5000

struct fruitDatabase
{
  char fruitId[10];
  char fruitName[50];
  int quantity;
  char soldTimestmp[200];
};

struct userTransactions
{
  char usrIp[20];
  int usrPort;
  char buyTimestmp[200];
};

int compare(char a[], char b[])
{
  int flag = 0, i = 0;                 // integer variables declaration
  while (a[i] != '\0' && b[i] != '\0') // while loop
  {
    if (a[i] != b[i])
    {
      flag = 1;
      break;
    }
    i++;
  }
  if (flag == 0)
    return 0;
  else
    return 1;
}

int main()
{

  int server_fd = 0, conn = 0, n = 0, recVal = 0;
  struct sockaddr_in serverAdd, peer_addr;
  FILE *fp;
  char *filename = "dataBase.txt";
  char *fname = "fruitSet.txt";
  char *fn = "rememberLine.txt";

  int choice;
  printf("1) Print list to Unique transactions\n2) Start Server\n");
  printf("Enter your choice : ");
  scanf("%d", &choice);

  while (choice > 3)
  {
    printf("Err!!! Press any number between 1 or 2");
    printf("Enter your choice : ");
    scanf("%d", &choice);
  }

  if (choice == 1)
  {
    fp = fopen(filename, "r");
    char ch;
    if (fp == NULL)
    {
      perror("Unable to open file!");
      exit(1);
    }
    printf("IP ADDRESS\tPORT\tTIMESTAMP\n");
    printf("-------------------------------------\n");
    while ((ch = fgetc(fp)) != EOF)
    {
      int j = 0;
      printf("%c", ch);
    }

    fclose(fp);
    return 0;
  }
  else
  {
    struct fruitDatabase fruit[5];

    //Initial fruits data
    strcpy(fruit[0].fruitId, "F01");
    strcpy(fruit[1].fruitId, "F02");
    strcpy(fruit[2].fruitId, "F03");
    strcpy(fruit[3].fruitId, "F04");
    strcpy(fruit[4].fruitId, "F05");

    strcpy(fruit[0].fruitName, "Apple");
    strcpy(fruit[1].fruitName, "Banana");
    strcpy(fruit[2].fruitName, "Grapes");
    strcpy(fruit[3].fruitName, "Orange");
    strcpy(fruit[4].fruitName, "Cucumber");

    char ch, tempNum[5], k = 0;
    int j, i, n, arrCnt = 0;

    fp = fopen(fname, "r");
    while ((ch = fgetc(fp)) != EOF)
    {
      if (ch == ' ' || ch == '\n')
      {
        fruit[arrCnt].quantity = atoi(tempNum);
        k = 0;
        arrCnt++;
      }
      else
      {
        tempNum[k++] = ch;
      }
    }

    fruit[arrCnt].quantity = atoi(tempNum);
    arrCnt++;
    fclose(fp);

    time_t timestmp;
    timestmp = time(NULL);

    //user database
    struct userTransactions usrTrans[500];

    //set of socket descriptors
    fd_set readfds;

    char sendArr[1025];
    char readArr[1024];

    int usrInc = 0;

    char t[1090], cpyFruitId[300], cpyQty[3000];

    printf("Conecting to Your Server...............\n");
    // Creating socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Server Ready...\n");

    memset(&serverAdd, '0', sizeof(serverAdd));
    memset(sendArr, '0', sizeof(sendArr));

    serverAdd.sin_family = AF_INET;
    serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAdd.sin_port = htons(PORT);

    if ((bind(server_fd, (struct sockaddr *)&serverAdd, sizeof(serverAdd))) < 0)
    {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }

    if ((listen(server_fd, 10)) < 0)
    {
      perror("listen");
      exit(EXIT_FAILURE);
    }

    socklen_t addr_size;
    addr_size = sizeof(struct sockaddr_in);

    while (1)
    {
      if ((conn = accept(server_fd, (struct sockaddr *)&peer_addr, &addr_size)) < 0) // accept awaiting request
      {
        perror("accept");
        exit(EXIT_FAILURE);
      }

      //read the data send by client
      recVal = recv(conn, readArr, sizeof(readArr), 0);
      char ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(peer_addr.sin_addr), ip, INET_ADDRSTRLEN);
      if (recVal <= 0)
      {
        printf("Connection may be lost\n");
      }

      readArr[recVal] = '\0';

      int qty;

      for (i = 3; i < (strlen(readArr)); i++)
      {
        cpyQty[i - 3] = readArr[i];
      }
      cpyQty[i - 3] = '\0';
      qty = atoi(cpyQty);

      memset(cpyQty, '0', sizeof(cpyQty));

      for (i = 0; i < 3; i++)
      {
        cpyFruitId[i] = readArr[i];
      }
      cpyFruitId[i] = '\0';

      int pos = -1;

      for (j = 0; j < 5; j++)
      {
        char tempChar[5];
        strcpy(tempChar, fruit[j].fruitId);
        int test = compare(cpyFruitId, tempChar);

        switch (test)
        {
        case 0:
          pos = j;
          break;
        case 1:
          continue;
        }
      }
      int qtyPresent = fruit[pos].quantity;
      if (qtyPresent >= qty)
      {
        int qtyRemaining = qtyPresent - qty;
        fruit[pos].quantity = qtyRemaining;
        strcpy(fruit[pos].soldTimestmp, asctime(localtime(&timestmp)));

        printf("Iteam Sold\n");
        printf("-------------------------------------------\n");
        printf("%s\t", fruit[pos].fruitId);
        printf("%s\t", fruit[pos].fruitName);
        printf("%d\t", fruit[pos].quantity);
        printf("%s\n", fruit[pos].soldTimestmp);

        fp = fopen(fname, "w");

        for(k=0;k<5;k++)
        {
          fprintf(fp,"%d ",fruit[k].quantity);
        }

        fclose(fp);

        //Updating buyer database
        strcpy(usrTrans[usrInc].usrIp, ip);
        usrTrans[usrInc].usrPort = ntohs(peer_addr.sin_port);
        strcpy(usrTrans[usrInc].buyTimestmp, asctime(localtime(&timestmp)));

        printf("Customer Info : \n");
        printf("-------------------------------------------\n");
        printf("%s\t", usrTrans[usrInc].usrIp);
        printf("%d\t", usrTrans[usrInc].usrPort);
        printf("%s\n", usrTrans[usrInc].buyTimestmp);

        fp = fopen(filename, "a+");

        fprintf(fp, "%s\t", usrTrans[usrInc].usrIp);
        fprintf(fp, "%d\t", usrTrans[usrInc].usrPort);
        fprintf(fp, "%s", usrTrans[usrInc].buyTimestmp);

        fclose(fp);

        usrInc++;
        strcpy(sendArr, "2");
        if ((send(conn, sendArr, strlen(sendArr), 0)) == -1)
        {
          printf("Problem\n");
          fprintf(stderr, "\nMessage not Send\n");
          close(sendArr);
        }
        printf("----------------------------------------------------------\n");
      }
      else
      {
        strcpy(sendArr, "1");
        if ((send(conn, sendArr, strlen(sendArr), 0)) == -1)
        {
          printf("Problem\n");
          fprintf(stderr, "\nMessage not Send\n");
          close(sendArr);
        }
      }
    }
  }
}