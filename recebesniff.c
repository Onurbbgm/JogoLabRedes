/* Note: run this program as root user
 * Author:Subodh Saxena 
 */
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
#include<arpa/inet.h>           // to avoid warning at inet_ntoa

//FILE* log_txt;
int total,tcp,udp,icmp,igmp,other,iphdrlen;

struct sockaddr saddr;
struct sockaddr_in source,dest;

int hex_to_int(char c){
        int first = c / 16 - 3;
        int second = c % 16;
        int result = first*10 + second;
        if(result > 9) result--;
        return result;
}

int hex_to_ascii(char c, char d){
        int high = hex_to_int(c) * 16;
        int low = hex_to_int(d);
        return high+low;
}


void ethernet_header(unsigned char* buffer,int buflen)
{
	struct ethhdr *eth = (struct ethhdr *)(buffer);
	printf("\nEthernet Header\n");
	printf("\t|-Source Address	: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
	printf("\t|-Destination Address	: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
	printf("\t|-Protocol		: %d\n",eth->h_proto);

}

void ip_header(unsigned char* buffer,int buflen)
{
	struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));

	iphdrlen =ip->ihl*4;

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = ip->saddr;     
	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = ip->daddr;     

	printf("\nIP Header\n");

	printf("\t|-Version              : %d\n",(unsigned int)ip->version);
	printf("\t|-Internet Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)ip->ihl,((unsigned int)(ip->ihl))*4);
	printf("\t|-Type Of Service   : %d\n",(unsigned int)ip->tos);
	printf("\t|-Total Length      : %d  Bytes\n",ntohs(ip->tot_len));
	printf("\t|-Identification    : %d\n",ntohs(ip->id));
	printf("\t|-Time To Live	    : %d\n",(unsigned int)ip->ttl);
	printf("\t|-Protocol 	    : %d\n",(unsigned int)ip->protocol);
	printf("\t|-Header Checksum   : %d\n",ntohs(ip->check));
	printf("\t|-Source IP         : %s\n", inet_ntoa(source.sin_addr));
	printf("\t|-Destination IP    : %s\n",inet_ntoa(dest.sin_addr));
}

void payload(unsigned char* buffer,int buflen)
{
	int i=0;
	unsigned char * data = (buffer + iphdrlen  + sizeof(struct ethhdr) + sizeof(struct udphdr));
	printf("\nData\n");
	int remaining_data = buflen - (iphdrlen  + sizeof(struct ethhdr) + sizeof(struct udphdr));
	for(i=0;i<remaining_data;i++)
	{
		//if(i!=0 && i%16==0)
		//	printf("\n");
		
		printf("%c",data[i]);
	}
	
	//printf("E o texto:\n");
	//char * str = (buffer + iphdrlen  + sizeof(struct ethhdr) + sizeof(struct udphdr));
	//printf("Texto::%s",data);
	//int leng = strlen(str);
	//char buf = 0;
	//for(i = 0; i < leng; i++){
          ///     if(i % 2 != 0){
             //          printf("%c", hex_to_ascii(buf, str[i]));
               // }else{
                 //      buf = str[i];
                //}
       // }

        printf("\n");
	
	



}

void tcp_header(unsigned char* buffer,int buflen)
{
	printf("\n*************************TCP Packet******************************");
   	ethernet_header(buffer,buflen);
  	ip_header(buffer,buflen);

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

	payload(buffer,buflen);

printf("*****************************************************************\n\n\n");
}

void udp_header(unsigned char* buffer, int buflen)
{
	printf("\n*************************UDP Packet******************************");
	ethernet_header(buffer,buflen);
	ip_header(buffer,buflen);
	printf("\nUDP Header\n");

	struct udphdr *udp = (struct udphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));
	printf("\t|-Source Port    	: %d\n" , ntohs(udp->source));
	printf("\t|-Destination Port	: %d\n" , ntohs(udp->dest));
	printf("\t|-UDP Length      	: %d\n" , ntohs(udp->len));
	printf("\t|-UDP Checksum   	: %d\n" , ntohs(udp->check));

	payload(buffer,buflen);

	printf("*****************************************************************\n\n\n");



}

void data_process(unsigned char* buffer,int buflen)
{
	struct iphdr *ip = (struct iphdr*)(buffer + sizeof (struct ethhdr));
	++total;
	/* we will se UDP Protocol only*/ 
	switch (ip->protocol)    //see /etc/protocols file 
	{

		case 6:
			++tcp;
			tcp_header(buffer,buflen);
			break;

		case 17:
			++udp;
			udp_header(buffer,buflen);
			break;

		default:
			++other;

	}
	//printf("TCP: %d  UDP: %d  Other: %d  Toatl: %d  \r",tcp,udp,other,total);


}



int main()
{

	int sock_r,saddr_len,buflen;

	unsigned char* buffer = (unsigned char *)malloc(65536); 
	memset(buffer,0,65536);

	//log_txt=fopen("log.txt","w");
	//if(!log_txt)
	//{
	//	printf("unable to open log.txt\n");
	//	return -1;

	//}

	printf("starting .... \n");

	sock_r=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL)); 
	if(sock_r<0)
	{
		printf("error in socket\n");
		return -1;
	}

	while(1)
	{
		saddr_len=sizeof saddr;
		buflen=recvfrom(sock_r,buffer,65536,0,&saddr,(socklen_t *)&saddr_len);


		if(buflen<0)
		{
			printf("error in reading recvfrom function\n");
			return -1;
		}
		//fflush(log_txt);
		data_process(buffer,buflen);

	}

	close(sock_r);// use signals to close socket 
	printf("DONE!!!!\n");

}
