/* Note: run this program as root user
 * cliente 
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
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};
 
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

void envio(int n){
	char* escolha;
	sprintf(escolha,"%d",n);
	 int s = socket (AF_INET, SOCK_RAW, IPPROTO_TCP);
    char server_message[4096];
    if(s == -1)
    {
        //socket creation failed, may be because of non-root privileges
        perror("Failed to create socket");
       // exit(1);
    }
     
    //Datagram to represent the packet
    char datagram[4096] , source_ip[32] , *data , *pseudogram;
     
    //zero out the packet buffer
    //memset (datagram, 0, 4096);
    memset (server_message, 0, 4096);
     
    //IP header
//    struct iphdr *iph = (struct iphdr *) datagram;
    struct iphdr *iph = (struct iphdr *) server_message; 
    //TCP header
//    struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
    struct tcphdr *tcph = (struct tcphdr *) (server_message + sizeof (struct ip));
    struct sockaddr_in sin;
    struct pseudo_header psh;
     
    //Data part
//    data = datagram + sizeof(struct iphdr) + sizeof(struct tcphdr);
    data = server_message + sizeof(struct iphdr) + sizeof(struct tcphdr);
  //  strcpy(data , "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    strcpy(data , escolha);
    //some address resolution
    strcpy(source_ip , "192.168.25.30");
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr ("1.2.3.4");
     
    //Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr) + strlen(data);
    iph->id = htonl (54321); //Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;      //Set to 0 before calculating checksum
    iph->saddr = inet_addr ( source_ip );    //Spoof the source ip address
    iph->daddr = sin.sin_addr.s_addr;
     
    //Ip checksum
//    iph->check = csum ((unsigned short *) datagram, iph->tot_len);
    iph->check = csum ((unsigned short *) server_message, iph->tot_len); 
    //TCP Header
    tcph->source = htons (1234);
    tcph->dest = htons (80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;  //tcp header size
    tcph->fin=0;
    tcph->syn=1;
    tcph->rst=0;
    tcph->psh=0;
    tcph->ack=0;
    tcph->urg=0;
    tcph->window = htons (5840); /* maximum allowed window size */
    tcph->check = 0; //leave checksum 0 now, filled later by pseudo header
    tcph->urg_ptr = 0;
     
    //Now the TCP checksum
    psh.source_address = inet_addr( source_ip );
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr) + strlen(data) );
     
    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data);
    pseudogram = malloc(psize);
     
    memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr) + strlen(data));
     
    tcph->check = csum( (unsigned short*) pseudogram , psize);
     
    //IP_HDRINCL to tell the kernel that headers are included in the packet
    int one = 1;
    const int *val = &one;
     
    if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
    {
        perror("Error setting IP_HDRINCL");
        //exit(0);
    }
     //server_message[4096] = "You have reached the server(exemplo)!";

	//strcpy(server_message, "10");
    //loop if you want to flood :)
   // while (1)
    //{
        //Send the packet
      // if (sendto (s, datagram, iph->tot_len ,  0, (struct sockaddr *) &sin, sizeof (sin)) < 0)

        //{
          //  perror("sendto failed");
        //}
       if (sendto (s, server_message, iph->tot_len ,  0, (struct sockaddr *) &sin, sizeof (sin)) < 0)

       {
            perror("sendto failed");
        }
        //Data send successfully
        else
        {
            //printf ("Packet Send. Length : %d \n" , iph->tot_len);
	   printf("Send success (%s).\n", data);
        }

}
void ethernet_header(unsigned char* buffer,int buflen)
{
	struct ethhdr *eth = (struct ethhdr *)(buffer);
	printf("\nEthernet Header\n");
	printf("\t|-Source Address	: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
	printf("\t|-Destination Address	: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
	printf("\t|-Protocol		: %d\n",eth->h_proto);

}

int ip_header(unsigned char* buffer,int buflen)
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
	if(strcmp(inet_ntoa(dest.sin_addr),"1.2.3.4")!=0){
			printf("NAO E O PACOTE QUE EU QUERO!!!!!!!\n");
			return 0;
	}
	else{return 1;}
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

int tcp_header(unsigned char* buffer,int buflen)
{
	printf("\n*************************TCP Packet******************************");
   	ethernet_header(buffer,buflen);
  	int verifica = ip_header(buffer,buflen);
	if(verifica == 0){
		return 0;	
	}else{
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
	return 1;
	}

printf("*****************************************************************\n\n\n");
}

int udp_header(unsigned char* buffer, int buflen)
{
	printf("\n*************************UDP Packet******************************");
	ethernet_header(buffer,buflen);
	int verifica = ip_header(buffer,buflen);
	printf("\nUDP Header\n");
	if(verifica == 0){
		return 0;	
	}else{
	struct udphdr *udp = (struct udphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));
	printf("\t|-Source Port    	: %d\n" , ntohs(udp->source));
	printf("\t|-Destination Port	: %d\n" , ntohs(udp->dest));
	printf("\t|-UDP Length      	: %d\n" , ntohs(udp->len));
	printf("\t|-UDP Checksum   	: %d\n" , ntohs(udp->check));

	payload(buffer,buflen);

	printf("*****************************************************************\n\n\n");
	return 1;	
	}


}

int data_process(unsigned char* buffer,int buflen)
{
	struct iphdr *ip = (struct iphdr*)(buffer + sizeof (struct ethhdr));
	++total;
	int verifica = 0;
	/* we will se UDP Protocol only*/ 
	switch (ip->protocol)    //see /etc/protocols file 
	{

		case 6:
			++tcp;
		verifica = tcp_header(buffer,buflen);
		return verifica;
			break;

		case 17:
			++udp;
			verifica = udp_header(buffer,buflen);
			return verifica;
			break;

		default:
			++other;
			return 0;

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
	int verifica = 0;
	while(verifica==0)
	{
		saddr_len=sizeof saddr;
		buflen=recvfrom(sock_r,buffer,65536,0,&saddr,(socklen_t *)&saddr_len);


		if(buflen<0)
		{
			printf("error in reading recvfrom function\n");
			return -1;
		}
		//fflush(log_txt);
		verifica = data_process(buffer,buflen);
		printf("Num verifica: %d\n",verifica);

	}
		int familia = 0;
		scanf("%d", &familia);
		printf("%d",familia);
		//envio(familia);
		printf("Enviado");

	//close(sock_r);// use signals to close socket 
	printf("DONE!!!!\n");

}
