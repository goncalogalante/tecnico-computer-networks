#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "functions.h"

#define NEW 1
#define EXTERN 2
#define WITHDRAW 3
#define QUERY 4
#define CONTENT 5
#define NOCONTENT 6

#define NODESLIST 7
#define OKREG 8
#define OKUNREG 9

int leave(No *new_node)

{
	int fd_i;

	if(strcmp(new_node->id, new_node->bck_node->id) == 0) // Saída de Nó ancora
			{
				// to implement
			}
	if(strcmp(new_node->id, new_node->bck_node->id) != 0)
	{
		//percorrer lista dos internos 
		for (int i = 0; i < new_node->num_nodes; i++) {
			fd_i = atoi(new_node->intr_nodes[i].intr_fd);
			close(fd_i);
		}
		//close socket do externo
		//close(new_node->ext_node->listen_tcp_fd);

		// remove interns from the structure
		int i;
		for (i = 0; i < new_node->num_nodes; i++) {
		memset(&new_node->intr_nodes[i], 0, sizeof(NodeInfo)); // set all values in the node to zero
		}
		new_node->num_nodes = 0; // set the number of nodes to zero

		// remove externo = igual a ele proprio
		strcpy(new_node->ext_node->id, new_node->id);
		strcpy(new_node->ext_node->ip, new_node->ip );
		strcpy(new_node->ext_node->port, new_node->port);

		// remove bck = igual a ele proprio
		strcpy(new_node->bck_node->id, new_node->id);
		strcpy(new_node->bck_node->ip, new_node->ip);
		strcpy(new_node->bck_node->port, new_node->port);
		
	}

	return 0;


}


int msg_received(char *msg_declaration)

{	
	// between tcp nodes
	if 		(strcmp(msg_declaration,"NEW")  == 0) return NEW;
	else if (strcmp(msg_declaration,"EXTERN")  == 0) return EXTERN;
	else if (strcmp(msg_declaration,"WITHDRAW")   == 0) return WITHDRAW;
	else if (strcmp(msg_declaration,"QUERY")   == 0) return QUERY;
	else if (strcmp(msg_declaration,"CONTENT")   == 0) return CONTENT;
	else if (strcmp(msg_declaration,"NOCONTENT")  == 0) return NOCONTENT;

	// server of nodes
	else if (strcmp(msg_declaration,"OKREG")  == 0) return OKREG;
	else if (strcmp(msg_declaration,"OKUNREG")  == 0) return OKUNREG;
	else if (strcmp(msg_declaration,"NODESLIST")  == 0) return NODESLIST;
	
	return -1;
}



int comm_treatment(No *new_node, int fd)

{

	ssize_t aux_bytes;
	char buffer[128], msg[6], id_msg[3], ip_msg[128], port_msg[6];

	bzero(buffer, 128);

	fd_set testfds;

	struct linger lo = { 1, 0 };

	aux_bytes=read(fd,buffer,128);  // aux_bytes=read(tempfd,buffer,128); 
	
	if (aux_bytes == -1)
		{
			perror("read");
			//leave(new_node);
			exit(1);
		}

	else if(aux_bytes == 0) //no more data to be read means that the fd that is sending info is closed
	{
		setsockopt(fd, SOL_SOCKET, SO_LINGER, &lo, sizeof(lo));
		close(fd);

		return(-1);
	}	


	printf("chegou: %s", buffer);
	sscanf(buffer, "%s",msg);

	switch(msg_received(msg))
	{
		case NEW: // NEW 02(id_msg) IP02(ip_msg) PORT02(port_msg)

			sscanf(buffer, "%s %s %s %s", msg, id_msg, ip_msg, port_msg);

			if(strcmp(new_node->id, new_node->ext_node->id) == 0) // Nó sozinho
			{
				strcpy(new_node->ext_node->id, id_msg);
				strcpy(new_node->ext_node->ip, ip_msg);
				strcpy(new_node->ext_node->port, port_msg);

				aux_bytes = sprintf(buffer, "EXTERN %s %s %s\n", new_node->ext_node->id, new_node->ext_node->ip, new_node->ext_node->port);

				aux_bytes = write(fd,buffer,aux_bytes);
			}
			else{
				
				//----atualizar internos----

				//printf("new_node -> num_nodes %d", new_node->num_nodes);

				strcpy(new_node->intr_nodes[new_node->num_nodes].id, id_msg);
				strcpy(new_node->intr_nodes[new_node->num_nodes].ip, ip_msg);
				strcpy(new_node->intr_nodes[new_node->num_nodes].port, port_msg);
				
				sprintf(new_node->intr_nodes[new_node->num_nodes].intr_fd, "%d", fd);


				// increment the number of nodes
				new_node->num_nodes++;


				//----atualizar internos----

				aux_bytes = sprintf(buffer, "EXTERN %s %s %s\n", new_node->ext_node->id, new_node->ext_node->ip, new_node->ext_node->port);

				aux_bytes = write(fd,buffer,aux_bytes);
				
			}
		

			break;


		case EXTERN:

			sscanf(buffer, "%s %s %s %s", msg, id_msg, ip_msg, port_msg);

			strcpy(new_node->bck_node->id, id_msg);
			strcpy(new_node->bck_node->ip, ip_msg);
			strcpy(new_node->bck_node->port, port_msg);
		

		break;


	}

	return 0;


}


// create server TCP socket
int tcp_socket(No *new_node)
{

	int fd, errcode=0, aux=0;

	fd = socket(AF_INET,SOCK_STREAM,0);
	if (fd == -1) exit(1); //error

	memset(&new_node->listen_tcp_hints,0,sizeof(struct addrinfo));
	new_node->listen_tcp_hints.ai_family = AF_INET; //IPv4
	new_node->listen_tcp_hints.ai_socktype = SOCK_STREAM; //TCP socket
	new_node->listen_tcp_hints.ai_protocol = IPPROTO_TCP; //TCP protocol
	new_node->listen_tcp_hints.ai_flags = AI_PASSIVE;

	errcode = getaddrinfo(NULL, new_node->port, &new_node->listen_tcp_hints, &(new_node->listen_tcp_res));
	if((errcode)!=0)/*error*/
	{
		gai_strerror(errcode);
		perror("getaddrinfo");
		exit(1);
	}
    

	aux = bind(fd, new_node->listen_tcp_res->ai_addr, new_node->listen_tcp_res->ai_addrlen);
	
	if(aux == -1) 
	{
		if(errno == 98)
		{
			printf("Connection already exists.\n");
			//return 0;
			exit (0);
		}
		perror("bind");
		/*error*/ exit(1);
	}


	if(listen(fd,5) == -1) /*error*/ exit(1);

	new_node->listen_tcp_fd = fd;

	return(0);

}

// create server UDP socket
int udp_socket(No *new_node)
{
	int fd, errcode=0, aux=0;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) exit(1); //error

	memset(&new_node->listen_udp_hints, 0, sizeof(struct addrinfo));
	new_node->listen_udp_hints.ai_family = AF_INET; // IPv4
	new_node->listen_udp_hints.ai_socktype = SOCK_DGRAM; // UDP Socket
	new_node->listen_udp_hints.ai_protocol = IPPROTO_UDP; // UDP Protocol
	new_node->listen_udp_hints.ai_flags = AI_PASSIVE;

	errcode = getaddrinfo(NULL, new_node->port, &new_node->listen_udp_hints, &new_node->listen_udp_res);
	if((errcode)!=0)/*error*/
	{
		gai_strerror(errcode);
		perror("getaddrinfo");
		exit(1);
	}

	aux = bind(fd, new_node->listen_udp_res->ai_addr, new_node->listen_udp_res->ai_addrlen);
	if(aux == -1) 
	{
		if(errno == 98)
		{
			printf("Connection already exists.\n");
			return 0;
		}
		perror("bind");
		/*error*/ exit(1);
	}

	new_node->listen_udp_fd = fd;

	return 0;
}



// create tree   ->  if (utilizado quando djoin id ip port = id ip port da estrutura)
int create_tree(No *new_node)
{
	udp_socket(new_node); // udp comm with server of nodes
    tcp_socket(new_node); // tcp comm 

	strcpy(new_node->ext_node->id, new_node->id);
	strcpy(new_node->bck_node->id, new_node->id);

	strcpy(new_node->ext_node->port, new_node->port);
	strcpy(new_node->bck_node->port, new_node->port);

	strcpy(new_node->ext_node->ip, new_node->ip);
	strcpy(new_node->bck_node->ip, new_node->ip);


	new_node->ext_node->listen_tcp_res = new_node->listen_tcp_res;
	new_node->bck_node->listen_tcp_res = new_node->listen_tcp_res;

	new_node->ext_node->listen_tcp_fd = new_node->listen_tcp_fd;
	new_node->bck_node->listen_tcp_fd = new_node->listen_tcp_fd;

	printf("Node initialized.\n");


	return 0;
	
}


// djoin function
int djoin(No *new_node, char *net, char *id, char*id_boot, char *ip_boot, char *port_boot)

{   
    
    int fd, errcode;
    size_t aux;

    //socklen_t addrlen;
	struct addrinfo hints, *res;
	//struct sockaddr_in addr;
	char /*buffer[128],*/ mensagem[128];


    fd = socket(AF_INET, SOCK_STREAM,0);  //TCP socket
	if (fd == -1) exit(1); //error

    memset(&hints, 0, sizeof (hints));
	hints.ai_family=AF_INET; //IPv4
	hints.ai_socktype=SOCK_STREAM; //TCP socket

    errcode = getaddrinfo(ip_boot, port_boot,&hints,&res);
	if(errcode!=0)
	{
		gai_strerror(errcode);
		/*error*/exit(1);
	}

    // verify connect error (-1)
    aux = connect(fd,res->ai_addr,res->ai_addrlen);
	if(aux==-1)
	{
		perror("connect");
		/*error*/exit(1);
	}

    // NEW ID IP TCP -> mensagem
    aux = sprintf(mensagem,"NEW %s %s %s\n",new_node->id,new_node->ip,new_node->port);
	printf("Mensagem: %s\n",mensagem);


    // write in socket
    aux=write(fd,mensagem,aux);
	if(aux==-1)
	{
		perror("write");
		/*error*/exit(1);
	}

	 // update the extern node structure (id, ip, port)
	new_node->ext_node->listen_tcp_fd = fd;
	strcpy(new_node->ext_node->id,id_boot);
	strcpy(new_node->ext_node->port, port_boot);
	strcpy(new_node->ext_node->ip, ip_boot);



return 0;
}

// show 
void show(No *new_node)
{


	printf("\text\t\tNode\t\tbck\t");
	printf("\n");

	printf("id\t%s\t\t%s\t\t%s", new_node->ext_node->id, new_node->id, new_node->bck_node->id);
	printf("\n");

	printf("ip\t%s\t%s\t%s", new_node->ext_node->ip, new_node->ip, new_node->bck_node->ip);
	printf("\n");

	printf("port\t%s\t\t%s\t\t%s", new_node->ext_node->port, new_node->port, new_node->bck_node->port);
	printf("\n");

	printf("\n-----Intr:-----\n");
    for (int i = 0; i < new_node->num_nodes; i++) {
        printf("%s %s %s %s\n", new_node->intr_nodes[i].id, new_node->intr_nodes[i].ip, new_node->intr_nodes[i].port, new_node->intr_nodes[i].intr_fd);
    }
    printf("-----Intr-----\n");


}

