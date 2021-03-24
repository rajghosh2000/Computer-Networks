Write a C program using TCP socket to implement the following: (60 marks)
i. Server maintains records of fruits in the format: fruit-name quantity Last-sold (server timestamp), 
ii. Multiple client purchase the fruits one at a time,
iii. The fruit quantity is updated each time any fruit is sold, 
iv. Show warning messages to the client if the quantity requested is not available. 
v. Display the customer ids <IP, port> who has done transactions already. This list should be updated in the server everytime a transaction occurs.
vi. The total number of unique customers who did some transaction will be displayed to the customer everytime.





     
        
        printf("Fruit ID ------------- Fruit Name ----------------- Quantity ----------------- Last Sold\n\n");
        for (i = 0; i < 5; i++)
        {
          for (j = 0; j < 4; j++)
          {
            if (j == 3)
            {
              printf("%s ", *(fruit[i] + j));
            }
            else
            {
              printf("%s ----------------------", *(fruit[i] + j));
            }
          }
          printf("\n");
        }
        printf("\n");
        for (i = 0; i < usrInc; i++)
        {
          for (j = 0; j < 4; j++)
          {
            printf("%s ", *(usrTrans[i] + j));
          }
          printf("\n");
        }
      }
      else
      {
        strcpy(sendArr, "1");
        if ((send(server_fd, sendArr, strlen(sendArr), 0)) == -1)
        {
          fprintf(stderr, "\nMessage not Send\n");
          close(sendArr);
        }
      }
    

    // "ntohs(peer_addr.sin_port)" function is for finding port number of client
    printf("connection established with IP : %s and PORT : %d\n", ip, ntohs(peer_addr.sin_port));

    close(conn); //Close Connection Socket
    sleep(1);
  } //End of Inner While...