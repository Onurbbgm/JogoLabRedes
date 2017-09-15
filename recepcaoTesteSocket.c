//tcp client file do video Socket Programming Tutorial in c for beginners
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>  //estrutura ifr
#include <netinet/ether.h> //header ethernet
#include <netinet/in.h> //definicao de protocolos
#include <arpa/inet.h> //funcoes para manipulacao de enderecos IP

#include <netinet/in_systm.h> //tipos de dados


int main(){
	//create a socket
	int network_socket;
	//socket(AF_INET,SOCK_RAW, esspecifica o protocolo - para raw socket)
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
			
	//specify an address for the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status =	connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address));
	//check for error with the connection
	if(connection_status == -1){
		printf("There was an error making a connecetion to the remote socket\n\n");
	}
	//receive data from the server
	char server_response[256];
	//recv(network_socket, &server_response, sizeof(server_response), 0);
	
	//print out the server's response
	//printf("The server sent the data: %s\n", server_response);
	while (1) {
		//char server_response[256];
   		//recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);
		recv(network_socket, &server_response, sizeof(server_response), 0);
		printf("The server sent the data: %s\n", server_response);
		// impress�o do conteudo - exemplo Endereco Destino e Endereco Origem
		//printf("MAC Destino: %x:%x:%x:%x:%x:%x \n", buff1[0],buff1[1],buff1[2],buff1[3],buff1[4],buff1[5]);
		//printf("MAC Origem:  %x:%x:%x:%x:%x:%x \n\n", buff1[6],buff1[7],buff1[8],buff1[9],buff1[10],buff1[11]);
	}
	//and then close the socket	
	//close(network_socket);
	return 0;
}
