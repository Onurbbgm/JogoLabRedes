#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<signal.h>
#include<stdbool.h>
#include<sys/socket.h>
#include<sys/types.h>

#include<linux/if_packet.h>
#include<netinet/in.h>		 
#include<netinet/if_ether.h>    // for ethernet header
#include<netinet/ip.h>		// for ip header
#include<netinet/udp.h>		// for udp header
#include<netinet/tcp.h>
#include<arpa/inet.h>  
/* 
    96 bit (12 bytes) pseudo header needed for tcp header checksum calculation 
*/
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};
 struct sockaddr saddr;
struct sockaddr_in source,dest;
unsigned short iphdrlen;
/*
    Generic checksum calculation function
*/
unsigned short csum(unsigned short *ptr,int nbytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
     
    return(answer);
}
 
int main (void)
{
    //Create a raw socket
    int saddr_len, buflen; 
    int s = socket (AF_INET, SOCK_RAW, IPPROTO_TCP);
    unsigned char* buffer = (unsigned char *)malloc(65536);
    memset(buffer,0,65536);
    //char server_message[256] = "You have reached the server!";
    if(s == -1)
    {
        //socket creation failed, may be because of non-root privileges
        perror("Failed to create socket");
	return -1;
        //exit(1);
    }
    while(1){
    
    //sockaddr saddr;
    saddr_len = sizeof(saddr);
    //buflen = recvfrom(s, buffer, 65536,0, &saddr, (socklen_t *)&saddr_len);
    buflen = recvfrom(s,buffer,65536,0,&saddr,(socklen_t *)&saddr_len);
    if(buflen<0){
    	printf("error in reading recvfrom function\n");
    } 
    //struct iphdr *ip = (struct iphdr*)(buffer + sizeof (struct ethhdr));
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    printf("\nEthernet Header\n");
    printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
    printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
    printf("\t|-Protocol : %d\n",eth->h_proto);

    //unsigned short iphdrlen;
    
    struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    iphdrlen =ip->ihl*4;
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = ip->saddr;     
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = ip->daddr;
    
    printf("\nIP Header\n");
    printf("\t|-Version : %d\n",(unsigned int)ip->version);
 
    printf("\t|-Internet Header Length : %d DWORDS or %d Bytes\n",(unsigned int)ip->ihl,((unsigned int)(ip->ihl))*4);
 
    printf("\t|-Type Of Service : %d\n",(unsigned int)ip->tos);
 
    printf("\t|-Total Length : %d Bytes\n",ntohs(ip->tot_len));
 
    printf("\t|-Identification : %d\n",ntohs(ip->id));
 
    printf("\t|-Time To Live : %d\n",(unsigned int)ip->ttl);
 
    printf("\t|-Protocol : %d\n",(unsigned int)ip->protocol);
 
    printf("\t|-Header Checksum : %d\n",ntohs(ip->check));
 
    printf("\t|-Source IP : %s\n", inet_ntoa(source.sin_addr));
 
    printf("\t|-Destination IP : %s\n",inet_ntoa(dest.sin_addr));

   struct tcphdr *tcp = (struct tcphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));
   printf("\nTCP Header\n");
   printf("\t|-Source Port          : %u\n",ntohs(tcp->source));
   printf("\t|-Destination Port     : %u\n",ntohs(tcp->dest));
   printf("\t|-Sequence Number      : %u\n",ntohl(tcp->seq));
   printf("\t|-Acknowledge Number   : %u\n",ntohl(tcp->ack_seq));
   printf("\t|-Header Length        : %d DWORDS or %d BYTES\n" ,(unsigned int)tcp->doff,(unsigned int)tcp->doff*4);
   printf("\t|----------Flags-----------\n");
   printf("\t\t|-Urgent Flag          : %d\n",(unsigned int)tcp->urg);
   printf("\t\t|-Acknowledgement Flag : %d\n",(unsigned int)tcp->ack);
   printf("\t\t|-Push Flag            : %d\n",(unsigned int)tcp->psh);
   printf("\t\t|-Reset Flag           : %d\n",(unsigned int)tcp->rst);
   printf("\t\t|-Synchronise Flag     : %d\n",(unsigned int)tcp->syn);
   printf("\t\t|-Finish Flag          : %d\n",(unsigned int)tcp->fin);
   printf("\t|-Window size          : %d\n",ntohs(tcp->window));
   printf("\t|-Checksum             : %d\n",ntohs(tcp->check));
   printf("\t|-Urgent Pointer       : %d\n",tcp->urg_ptr);

   int i=0;
   unsigned char * data = (buffer + iphdrlen  + sizeof(struct ethhdr) + sizeof(struct tcphdr));
   printf("\nData\n");
   int remaining_data = buflen - (iphdrlen  + sizeof(struct ethhdr) + sizeof(struct tcphdr));
   for(i=0;i<remaining_data;i++)
   {
	if(i!=0 && i%16==0)
		printf("\n");
	printf(" %.2X ",data[i]);
   }
   }
    
    return 0;
}
