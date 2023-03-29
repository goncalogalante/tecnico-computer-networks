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

// intr structure
typedef struct {
    char ip[100];
    char port[100];
    char id[100];
    char intr_fd[100];
} NodeInfo;


typedef struct {
    char name[100];
    
} Conteudo;

// Me node structure
typedef struct me{

    char id[2]; // my id
    char ip[64]; // my ip
    char port[6]; // my port
    
    char my_net[3]; // net nº where I belong

    char ip_nodeserver[64];
    char port_nodeserver[6];

    int listen_tcp_fd; // listen_tcp_fd
    int listen_udp_fd; // listen_udp_fd

    struct addrinfo listen_tcp_hints, *listen_tcp_res;
	struct addrinfo listen_udp_hints, *listen_udp_res;

    struct sockaddr_in listen_udp_addr, listen_tcp_addr;

    struct me *ext_node, *bck_node;

    Conteudo conteudos[100]; // array of contents
    int num_conteudos; // actual nº of contents in the list

    // intr nodes structure initialization
    NodeInfo intr_nodes[100];
    int num_nodes;

} No;


/*declare functions*/

int leave(No *new_node);

int msg_received(char *msg_declaration);

int comm_treatment(No *new_node, int fd);

int udp_socket(No *new_node);

int tcp_socket(No *new_node);

int create_tree(No *new_node);

int djoin(No *new_node, char *net, char *id, char *id_boot, char *ip_boot, char *port_boot);

void show(No *new_node);


