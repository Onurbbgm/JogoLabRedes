#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <netdb.h>

#define PORTA_INICIAL      1
#define PORTA_FINAL     7000

int scaneador(unsigned short porta, unsigned int hacker, unsigned int
vitima);
unsigned short in_cksum (unsigned short *end, int tamanho);
void alarm_handler(int i);

int tempo_esgotado = 0;

main(int argc, char *argv[])
{
unsigned short portas;
unsigned int porta_inicial, porta_final;
struct servent *servico;

if(argc == 1) {
printf("***************************************************************\n");
printf("************ SYN SCAN - DESENVOLVIDO POR NASH LEON ************\n");
printf("***************************************************************\n");
printf("\nUso: %s <ip_local> <ip_vitima> <porta_inicial> <porta_final>\n",argv[0]);
exit(0);
}
if(geteuid() != 0) {
fprintf(stderr,"Amigo, voce necessita ser root!!\n");
fprintf(stderr,"Essas estruturas(RAW SOCKET) necessitam disso!!\n");
fprintf(stderr,"Sinto muito amigo.\nParando execucao!!\n\n");
exit(1);
}
if (argc > 2)
{
  porta_inicial = (PORTA_INICIAL);
  porta_final = (PORTA_FINAL);
}
if (argc > 3)
{
  porta_inicial =  atoi(argv[3]);
  porta_final = atoi (argv[4]);
}
for(portas = porta_inicial; portas < porta_final; portas++){
if(scaneador(portas, inet_addr(argv[1]), inet_addr(argv[2]))==1){
servico = getservbyport(htons(portas),"tcp");
printf("A porta %d esta aberta mano!! O Servico eh [%s]\n",portas,(servico
== NULL) ? "Desconhecido" : servico-> s_name);
}
}
return 0;
}
int scaneador(unsigned short porta, unsigned int hacker, unsigned int vitima)
{
   struct tcphdr meu_tcp;
   struct recv_tcp {
      struct iphdr ip;
      struct tcphdr tcp;
      unsigned char blah[65535];
   } recv_tcp;

   struct pseudo_header {
      unsigned int source_address;
      unsigned int dest_address;
      unsigned char placeholder;
      unsigned char protocol;
      unsigned short tcp_length;
      struct tcphdr tcp;
   }pseudo_header;

   int Meusocket;
   struct sockaddr_in alvo;
   int tam_alvo;
   static int blah = 0;

   blah++;
meu_tcp.source = getpid() + blah;
meu_tcp.dest = htons(porta);
meu_tcp.seq = getpid() + blah;
meu_tcp.ack_seq = 0;
meu_tcp.res1 = 0;
meu_tcp.doff = 5;
//meu_tcp.res2 = 0;
meu_tcp.fin = 0;
meu_tcp.syn = 1; /* o bit syn aqui eh ligado! */
meu_tcp.rst = 0;
meu_tcp.psh = 0;
meu_tcp.ack = 0;
meu_tcp.urg = 0;
meu_tcp.window = htons(512);
meu_tcp.check = 0;
meu_tcp.urg_ptr = 0;

pseudo_header.source_address = hacker;
pseudo_header.dest_address = vitima;
pseudo_header.placeholder = 0;

pseudo_header.protocol = IPPROTO_TCP;
pseudo_header.tcp_length = htons(20);
bcopy(&meu_tcp, &pseudo_header.tcp, 20);
meu_tcp.check = in_cksum((unsigned short *)&pseudo_header, 32);

alvo.sin_family = AF_INET;
alvo.sin_port = htons(porta);
alvo.sin_addr.s_addr = vitima;
tam_alvo=sizeof(alvo);

signal(SIGALRM, alarm_handler);

Meusocket = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
if(Meusocket < 0)
{
 fprintf(stderr, "Nao pode abrir raw socket!\n");
 exit(1);
}

sendto(Meusocket, &meu_tcp, 20, 0, (struct sockaddr *)&alvo,tam_alvo);
tempo_esgotado = 0;
alarm(10);
while(1)
{
read(Meusocket, (struct recv_tcp *)&recv_tcp, 65535);
if(tempo_esgotado == 1) {
close(Meusocket);
tempo_esgotado=0;
return -1;
}
if(recv_tcp.tcp.dest == (getpid() + blah)){
alarm(0);
close(Meusocket);
if(recv_tcp.tcp.rst == 1) return 0;
else return 1;
  }
 }
}

/* Checksum */

unsigned short in_cksum(unsigned short *end, int tamanho)
{
register long           sum;
u_short                 oddbyte;
register u_short        answer;

sum = 0;
while (tamanho > 1)  {
sum += *end++;
tamanho -= 2;
 }
if (tamanho == 1) {
oddbyte = 0;
*((u_char *) &oddbyte) = *(u_char *)end;
sum += oddbyte;
}

sum  = (sum >> 16) + (sum & 0xffff);
sum += (sum >> 16);
answer = ~sum;
return(answer);
}

void alarm_handler (int i)
{
   alarm(0);
   tempo_esgotado = 1;
}
