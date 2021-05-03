#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define packetSize 64

char ip[30]="143.110.178.212", hostname[100];
struct sockaddr_in serverAdd;

// ping packet structure
struct pingPacket
{
    struct icmphdr hdr;
    char msg[packetSize-sizeof(struct icmphdr)];
};

// Calculating the Check Sum
unsigned short checksum(void *msg, int len)
{    
    unsigned short *buf = msg;
    unsigned int sum=0;
    unsigned short result;
  
    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}
  
void getHostName()
{
    
    char temp[NI_MAXHOST];
    serverAdd.sin_family = AF_INET;
    serverAdd.sin_addr.s_addr = inet_addr(ip);

    if (getnameinfo((struct sockaddr *)&serverAdd, sizeof(struct sockaddr_in), temp,
                    sizeof(temp), NULL, 0, NI_NAMEREQD))
    {
        printf("Could not resolve reverse lookup of hostname\n");
        exit(0);
    }
    strcpy(hostname, temp);
    printf("\nHOSTNAME: %s & IP : %s\n", hostname, ip);
    printf("%d\n",serverAdd.sin_addr.s_addr);
}

void sendPing(int fd)
{
    int ttl_val=64;
    struct pingPacket p;
    int k=0,flag=1;
    struct sockaddr_in r_addr;


    //SOL_IP (set/configure various IP packet options, IP layer behaviors, [as here] netfilter module options)
    //IP_TTL (Set or retrieve the current time-to-live field that is used in every packet sent from this socket.)
    if (setsockopt(fd, SOL_IP, IP_TTL,&ttl_val, sizeof(ttl_val)) != 0)
    {
        printf("\nSetting socket options to TTL failed!\n");
        return;
    }
    else
    {
        printf("\nSocket set to TTL..\n");
    }
    while(1)
    {
        int i;
        bzero(&p,sizeof(p));
        p.hdr.type = ICMP_ECHO;
        p.hdr.un.echo.id = getpid();
        p.hdr.un.echo.sequence = k++;
        p.hdr.checksum = checksum(&p,sizeof(p));

        usleep(100000);
        printf("%d\n",serverAdd.sin_addr.s_addr);
        if(sendto(fd, &p, sizeof(p), 0, (struct sockaddr*)&serverAdd, sizeof(serverAdd))<=0)
        {
            perror("error\n");
            flag=0;
        }
        
        int len = sizeof(&r_addr);
        if ( recvfrom(fd, &p, sizeof(p), 0, 
             (struct sockaddr*)&r_addr, &len) <= 0
              && k>1) 
        {
            printf("\nPacket receive failed!\n");
        }
        else
        {
            if(flag)
            {
                if(!(p.hdr.type ==69 && p.hdr.code==0)) 
                {
                    printf("Error..Packet received with ICMP type %d code %d\n", 
                           p.hdr.type, p.hdr.code);
                }
                else
                {
                    printf("Sending....");  
                    k++;
                }
            }
        }
    }

}

int main()
{
    int server_fd = 0;
    printf("Enter the IP Address to ping : ");
    fgets(ip, sizeof(ip), stdin);

    
    //printf("%s\n",ip);
    getHostName();
    printf("Conecting to Your Server...............\n");
    // Creating socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        printf("\nServer Error...!!\n");
        return 0;
    }
    else
    {
        printf("\nServer Ready...\n");
    }
    sendPing(server_fd);
}
