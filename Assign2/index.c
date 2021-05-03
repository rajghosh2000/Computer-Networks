#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>


typedef unsigned char u8;
typedef unsigned short int u16;

char ipUser[30]="143.110.178.212", hostname[100];

void help(const char *p);

void getHostName()
{
    
    char temp[1025];
	struct sockaddr_in serverAdd;
    serverAdd.sin_family = AF_INET;
    serverAdd.sin_addr.s_addr = inet_addr(ipUser);

    if (getnameinfo((struct sockaddr *)&serverAdd, sizeof(struct sockaddr_in), temp,
                    sizeof(temp), NULL, 0, 8))
    {
        printf("Could not resolve reverse lookup of hostname\n");
        exit(0);
    }
    strcpy(hostname, temp);
    printf("\nHOSTNAME: %s & IP : %s\n", hostname, ipUser);
    
}

int main(int argc, char **argv)
{	
	int packetsRequired;
	printf("Enter the IP Address to ping : ");
    fgets(ipUser, sizeof(ipUser), stdin);

	printf("Enter the packets to be sent : ");
	scanf("%d",&packetsRequired);

	getHostName();

	int payload_size = 0, sent, sent_size;

    struct timeval tv_out;
    tv_out.tv_sec = 1;
    tv_out.tv_usec = 0;

	if (argc > 3)
	{
		payload_size = atoi(argv[3]);
	}

	//Raw socket - if you use IPPROTO_ICMP, then kernel will fill in the correct ICMP header checksum, if IPPROTO_RAW, then it wont
	int sockfd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);

	if (sockfd < 0)
	{
		perror("could not create socket");
		return (0);
	}

	int on = 64;

	// We shall provide IP headers
	if (setsockopt (sockfd, IPPROTO_IP, IP_HDRINCL, (const char*)&tv_out, sizeof (tv_out)) == -1)
	{
		perror("setsockopt");
		return (0);
	}

	
	if (setsockopt (sockfd, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof (on)) == -1)
	{
		perror("setsockopt");
		return (0);
	}

	
	int packet_size = sizeof (struct iphdr) + sizeof (struct icmphdr) + payload_size;
	char *packet = (char *) malloc (packet_size);

	if (!packet)
	{
		perror("out of memory");
		close(sockfd);
		return (0);
	}

	//ip header
	struct iphdr *ip = (struct iphdr *) packet;
	struct icmphdr *icmp = (struct icmphdr *) (packet + sizeof (struct iphdr));

	
	memset (packet, 0, packet_size);

	ip->version = 4;
	ip->ihl = 5;
	ip->tos = 0;
	ip->tot_len = htons (packet_size);
	ip->id = rand ();
	ip->frag_off = 0;
	ip->ttl = 255;
	ip->protocol = IPPROTO_ICMP;
	ip->daddr = ip;
  	icmp->type = ICMP_ECHO;
	icmp->code = 0;
  	icmp->un.echo.sequence = rand();
  	icmp->un.echo.id = rand();
	icmp->checksum = 0;

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = ipUser;
	memset(&servaddr.sin_zero, 0, sizeof (servaddr.sin_zero));
	time_t start_t, end_t;
   	double diff_t;

   	printf("Starting of the program...\n");
  	time(&start_t);
	
	printf("Trying to Send....\n\n");

	while (1)
	{
		memset(packet + sizeof(struct iphdr) + sizeof(struct icmphdr), rand() % 255, payload_size);

		icmp->checksum = 0;

		if(sent<=packetsRequired)
		{
			if ( (sent_size = sendto(sockfd, packet, packet_size, 0, (struct sockaddr*) &servaddr, sizeof (servaddr))) < 1)
		{
			perror("send failed\n");
			break;
		}
			++sent;
			usleep(100000);	//microseconds
			time(&end_t);
   			diff_t = difftime(end_t, start_t);
			printf("Packets Sent in Process -> %d time taken : %f\n", sent,diff_t);
			fflush(stdout);
		}
		else if(sent==packetsRequired+1)
		{
			printf("Packets Sent : %d\n",packetsRequired);
			sent++;
			exit(0);
		}

		
	}

	free(packet);
	close(sockfd);

	return (0);
}