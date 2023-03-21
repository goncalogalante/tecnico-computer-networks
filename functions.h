#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

typedef struct {
    char name[100];
    
} Conteudo;

// Me node structure
typedef struct me{

    char ip[64]; // my ip
    char port[6]; // my port

    char ip_nodeserver[64];
    char port_nodeserver[6];

    char id[2]; // my id
    char ext[2]; // extern neighborhood id
    char bck[2]; // backup neighborhood id

    char my_net[3]; // net nº where I belong

    int listen_tcp_fd; // listen_tcp_fd

    struct addrinfo listen_tcp_hints, *listen_tcp_res;
	struct addrinfo listen_udp_hints, *listen_udp_res;

    struct sockaddr_in listen_udp_addr,listen_udpchord_addr, listen_tcp_addr;

    struct me *ext_node, *bck_node;

    Conteudo conteudos[100]; // array of contents
    int num_conteudos; // actual nº of contents in the list

    char **intr; // pointer to a dynamic array of strings
    int num_intr; // actual number of elements in the array





} No;


/*declare functions*/

int tcp_socket(No *new_node);

int create_tree(No *new_node);

int djoin(No *new_node, char *net, char *id, char *id_boot, char *ip_boot, char *port_boot);

void show(No *new_node);


