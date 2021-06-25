#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <time.h>
#include <net/if_arp.h>

#define PCAP_BUF_SIZE 1024

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
// #define ETHER_ADDR_LEN	6

/* Ethernet header */
struct sniff_ethernet
{
    u_char ether_dhost[ETHER_ADDR_LEN]; /* destination host address */
    u_char ether_shost[ETHER_ADDR_LEN]; /* source host address */
    u_short ether_type;                 /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip
{
    u_char ip_vhl;                 /* version << 4 | header length >> 2 */
    u_char ip_tos;                 /* type of service */
    u_short ip_len;                /* total length */
    u_short ip_id;                 /* identification */
    u_short ip_off;                /* fragment offset field */
    #define IP_RF 0x8000               /* reserved fragment flag */
    #define IP_DF 0x4000               /* don't fragment flag */
    #define IP_MF 0x2000               /* more fragments flag */
    #define IP_OFFMASK 0x1fff          /* mask for fragmenting bits */
    u_char ip_ttl;                 /* time to live */
    u_char ip_p;                   /* protocol */
    u_short ip_sum;                /* checksum */
    struct in_addr ip_src, ip_dst; /* source and dest address */
};
#define IP_HL(ip) (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip) (((ip)->ip_vhl) >> 4)

int main()
{
    char errbuff[PCAP_ERRBUF_SIZE];
    char fname[] = "s1-flag.pcap";
    /*
    * Opening file
    */
    pcap_t *pcap = pcap_open_offline(fname, errbuff);

    /* declare pointers to packet headers */
    const struct ether_header *ethernet; /* The ethernet header [1] */
    const struct sniff_ip *ip;           /* The IP header */
    const struct icmphdr *icmp;
    const struct arpd_request *arp;
    int size_ip;

    const struct pcap_pkthdr *header;

    /*
    * header
    */
    const u_char *data;

    /*
    * Looping through packets and print them to screen
    */
    u_int packetCount = 0;
    int returnValue;
    while (returnValue = pcap_next_ex(pcap, &header, &data) >= 0)
    {

        // Show the packet number
        printf("Packet No. %i\n", ++packetCount);

        // Show the size in bytes of the packet
        printf("Packet size: %d bytes\n", header->len);

        // Show a warning if the length captured is different
        if (header->len != header->caplen)
            printf("Warning! Capture size different than packet size: %ld bytes\n", (header->len));

        // Show Epoch Time
        printf("Epoch Time: %d:%d seconds\n", header->ts.tv_sec, header->ts.tv_usec);
        /* define ethernet header */
        ethernet = (struct ether_header *)(data);
        printf("L2 Header:\nsrc: %s\ndst: %s\nether_type: ", ether_ntoa((struct ether_addr *)ethernet->ether_shost), ether_ntoa((struct ether_addr *)ethernet->ether_dhost));
        
        switch (ntohs(ethernet->ether_type))
        {
        case ETHERTYPE_ARP:
            printf("arp\n");
            // arp = (struct arpd_request*)(packet + SIZE_ETHERNET);
            // printf("req: %d\n", ntohs((uint16_t)(arp->req)));
            // printf("dev: %d\n", ntohl((uint32_t)(arp->dev)));
            break;

        case ETHERTYPE_IP:
            printf("ipv4\n");
            
            /* define/compute ip header offset */
            ip = (struct sniff_ip *)(data + SIZE_ETHERNET);
            size_ip = IP_HL(ip) * 4;
            if (size_ip < 20)
            {
                printf("   * Invalid IP header length: %u bytes\n", size_ip);
            }

            /* print source and destination IP addresses */
            printf("\n\tL3 Header:\n");
            printf("\tFrom: %s\n", inet_ntoa(ip->ip_src));
            printf("\tTo: %s\n", inet_ntoa(ip->ip_dst));
            printf("\tTTL: %d\n", ip->ip_ttl);
            printf("\tHeader Checksum: %#x\n", ntohs((uint16_t)(ip->ip_sum)));

            /* determine protocol */
            switch (ip->ip_p)
            {
            case IPPROTO_TCP:
                printf("\tProtocol: TCP\n");
                break;
            case IPPROTO_UDP:
                printf("\tProtocol: UDP\n");
            case IPPROTO_ICMP:
                printf("\tProtocol: ICMP\n");

                icmp = (struct icmphdr *)(data + SIZE_ETHERNET + size_ip);

                printf("\t\tL4 Header:\n");
                printf("\t\tType: ");
                switch (icmp->type)
                {
                case ICMP_ECHO:
                    printf("ICMP ECHO (request)\n");
                    break;

                case ICMP_ECHOREPLY:
                    printf("ICMP REPLY\n");
                    break;

                default:
                    printf("OTHER\n");
                    break;
                }
                printf("\t\tCode: %d\n", icmp->code);
                printf("\t\tChecksum: %#x\n", ntohs(icmp->checksum));
                printf("\t\tSequence: %d\n", ntohs(icmp->un.echo.sequence));

            case IPPROTO_IP:
                printf("\tProtocol: IP\n");
            default:
                printf("\tProtocol: unknown\n");
            }
            break;

        default:
            printf("unknown\n");
            break;
        }
        // loop through the packet and print it as hexidecimal representations of octets
        for (u_int i = 0; (i < header->caplen); i++)
        {
            // Start printing on the next after every 16 octets
            if ((i % 16) == 0)
                printf("\n");

            // Print each octet as hex (x), make sure there is always two characters (.2).
            printf("%.2x ", data[i]);
        }

        // Add two lines between packets
        printf("\n=================================================================\n");
        printf("\n\n");
    }
}
