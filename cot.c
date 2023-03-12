#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// Me node structure
struct me{

	int id;
    char *ip;
    int port;

    char *ip_nodeserver;
    int port_nodeserver;
};

typedef struct me me_node;
me_node me_ptr;


int main(int argc, char *argv[]) {

    struct me me_ptr;

    // verify the number of arguments
    if (argc != 5) {
        printf("Usage: ./cot IP TCP regIP regUDP");
    }

    else{

    // allocate the structure strings
    me_ptr.ip=malloc(250);
    me_ptr.ip_nodeserver=malloc(250);

    // save the input info (cot IP TCP regIP regUDP) to the structure
    sscanf(argv[1],"%s", me_ptr.ip);
    sscanf(argv[2], "%d", &me_ptr.port);
    sscanf(argv[3],"%s", me_ptr.ip_nodeserver);
    sscanf(argv[4], "%d", &me_ptr.port_nodeserver);

    // verify the input format (cot IP TCP regIP regUDP)
    if(argv[1][3]!= '.' || argv[1][7]!='.')
        {
            printf("\n > Warning: Invalid TCP IP adress.\n");
            exit(1);
        }

    if (me_ptr.port < 0 || me_ptr.port > 65535) 
        {
            printf("\n > Warning: TCP Port adress.\n");
            exit(1);
        }

    if (strcmp(argv[3], "193.136.138.142") != 0) 
    {
        printf("\n > Warning: Invalid UDP IP adress for the Server of Nodes.\n");
        exit(1);
    }

    if (me_ptr.port_nodeserver != 59000) 
    {
        printf("\n > Warning: Invalid UDP Port for the Server of Nodes.\n");
        exit(1);
    }
    

    // show the input info accepted
    printf("- My IP: %s\n", me_ptr.ip);
    printf("- My Port: %d\n", me_ptr.port);
    printf("- Server of Nodes UDP IP: %s\n", me_ptr.ip_nodeserver);
    printf("- Server of Nodes UDP Port: %d\n", me_ptr.port_nodeserver);











    }

    return 0;
    }


