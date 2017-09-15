//tcp server file do video Socket Programming Tutorial in c for beginners

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/in.h>

int main(){
	char server_message[256] = "You have reached the server!";	
	int retValue = 0;
	//create the server socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	//define the server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;
	
	//bind the socket to our specified IPand port
	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

	listen(server_socket, 5);
		
	int client_socket;
	client_socket = accept(server_socket, NULL, NULL);
	
	//send the message	
	//send(client_socket, server_message, sizeof(server_message), 0);
	 while(1) {
	    /* Envia pacotes de 64 bytes */
 	   if((retValue = send(client_socket, server_message, sizeof(server_message), 0)) < 0) {
	       printf("ERROR! sendto(%d) \n",retValue);
	       exit(1);
	    }
	       printf("Send success (%s).\n", server_message);
	  }

	//close the socket

	//close(server_socket);

	return 0;
}



