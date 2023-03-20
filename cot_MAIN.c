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
#include <ctype.h>
#include "functions.h"


// auxiliar functions:

// function to print the command menu
void print_cmdmenu() {

  printf("\n--------------------Command Menu-------------------\n");  
  printf("1) join net id: Entrada de um nó na rede net com identificador id.\n");
  printf("2) djoin net id bootid bootIP bootTCP: Entrada de um nó na rede net com identificador id,\n");
  printf("que se sabe ser único na rede. É passado à aplicação o identificador e o contacto de um nó da rede,\n");
  printf("através dos parâmetros bootid, bootIP e bootTCP, ao qual o nó se deverá ligar sem interrogar o servidor de nós.\n");
  printf("3) create name: É criado um conteúdo de nome name.\n");
  printf("4) delete name: É apagado o conteúdo de nome name.\n");
  printf("5) get dest name: Pesquisa do conteúdo com o nome name localizado no nó dest.\n");
  printf("6) show topology (st): Mostra os identificadores e os contactos dos vizinhos internos, do vizinho externo e do vizinho de recuperação.\n");
  printf("7) show names (sn): Mostra os nomes dos conteúdos presentes no nó.\n");
  printf("8) show routing (sr): Mostra a tabela de expedição do nó.\n");
  printf("9) leave: Saída do nó da rede.\n");
  printf("10) exit: Fecho da aplicação.\n");
}

// function to create a content name for the node 
void create(char *nome_conteudo, No *no)
{

Conteudo new_conteudo;
strcpy(new_conteudo.name, nome_conteudo); 
no->conteudos[no->num_conteudos] = new_conteudo;
no->num_conteudos++;

}

// function to delete a content name for the node
void delete(char *nome_conteudo, No *no)
{

    int i, j;
    for (i = 0; i < no->num_conteudos; i++) {
        if (strcmp(no->conteudos[i].name, nome_conteudo) == 0) { // find the content to be deleted
            
            for (j = i; j < no->num_conteudos-1; j++) {
                no->conteudos[j] = no->conteudos[j+1];
            }
            no->num_conteudos--; 
            printf("Content removed.\n");
        }
    }

}

// function to show the names of contents in the node
void show_names(No *no)
{

    printf("\n-----Contents of the node:-----\n");
    for (int i = 0; i < no->num_conteudos; i++) {
        printf("%s\n", no->conteudos[i].name);
    }
    printf("-----End of contents-----\n");

}

// function to present the menu
int menu(char *input, No *me_ptr)
{
    char ip[64]; // ip addressto connect
    char port[6]; // port to connect
    char id[3]; // id to connect



        printf("\n>> Command input: ");
        fgets(input, 100, stdin);
        input[strcspn(input, "\n")] = 0; // remove the newline character

        char *first_word = strtok(input, " ");


        /*------- JOIN --------*/
        if (strcmp(first_word, "join") == 0) {
            
        }

        /*------- DJOIN -------- djoin NET ID BOOTID BOOTIP BOOTCP */
        if (strcmp(first_word, "djoin") == 0) {

            int is_valid = 0;    

            // NET verification
            char *net_number = strtok(NULL," ");

            // verify if NET is NUll or different than 3 digits
            if (net_number == NULL || strlen(net_number) != 3) { 
                printf(">> Error: Invalid NET.\n");
            }
            else {

                // verify if the NET digit string is an integer value
                for (int i = 0; i < strlen(net_number); i++) {
                if (!isdigit(net_number[i])) {
                    printf(">> Error: Invalid NET.\n");
                    break;
                }
                else{
                
                    // update the NET for the node structure
                    int net_aux;
                    net_aux = atoi(net_number);

                    // verify conditions for NET number
                    if (net_aux < 0 || net_aux > 999) {
                        printf(">> Error: Invalid NET.\n");
                    }
                    else
                    {
                    // NET update for the node structure    
                        strcpy(me_ptr->my_net, net_number);
                        is_valid = 1;
                    }
                
                }
    
             }

            } 

            // ID verification
            char *id_number = strtok(NULL," ");
            
            // verify if ID digited nºs is NUll or different than 3 digits
            if (id_number == NULL || strlen(id_number) != 2) { 
                printf(">> Error: Invalid ID.\n");
            }

            else {

                // verify if the ID digit string is an integer value
                for (int i = 0; i < strlen(id_number); i++) {
                if (!isdigit(id_number[i])) {
                    printf(">> Error: Invalid NET.\n");
                    break;
                }
                else{
                    
                    // convert string to integer variable 
                    int id_aux;
                    id_aux= atoi(id_number);
                    // verify conditions for ID number
                    if (id_aux < 0 || id_aux > 99) {
                        printf(">> Error: Invalid NET.\n");
                    }

                    // ID update for the node structure
                    strcpy(me_ptr->id, id_number);
                    is_valid = 2;
                
                }
    
             }


            } 

            // BOOTID verification
            char *bootid = strtok(NULL," ");
            if (bootid == NULL) { 
                printf(">> Error: Invalid bootID.\n");
            }
            else
            {
                strcpy(id, bootid);
                is_valid = 3;
            }

            // BOOTIP verification
            char *bootIP = strtok(NULL," ");
            if (bootIP == NULL) { 
                printf(">> Error: Invalid bootIP.\n");
            }
            else
            {
                strcpy(ip, bootIP);
                is_valid = 4;
            }
    
            // BOOTTPC verification
            char *bootTCP = strtok(NULL," ");
            if (bootTCP == NULL) { 
                printf(">> Error: Invalid bootTCP.\n");
            }
            else
            {
                strcpy(port, bootTCP);
                is_valid = 5;
            }

            
            if (is_valid==5)
            {
                printf("-------------------------------------------------\n");
                printf("Node ID: %s | Node NET: %s\n", me_ptr->id, me_ptr->my_net);
                printf("bootid: %s | bootIP: %s | bootTCP: %s\n", id, ip,port );
                printf("-------------------------------------------------\n");
                create_tree(me_ptr);
                djoin(me_ptr, me_ptr->my_net, me_ptr->id, id, ip, port);
            }
         }


        /*------- CREATE --------*/
        if (strcmp(first_word, "create") == 0) {
            

            // takes the content name and saves it to the structure
            char *content_name = strtok(NULL," ");
            if (content_name == NULL) { 
                printf(">> Error: Not a valid content.\n");
            } else {
            No no;
            create(content_name, &no); // passa o nome do conteúdo e o ponteiro para a estrutura No para a função create()
            }

        }

        /*------- DELETE --------*/
        if (strcmp(first_word, "delete") == 0) {
            
            // takes the content name and saves it to the structure
            char *content_name = strtok(NULL," ");
            if (content_name == NULL) { 
                printf(">> Error: Not a valid content.\n");
            }
            else {
            No no;
            delete(content_name, &no); // passa o nome do conteúdo e o ponteiro para a estrutura No para a função create()
            }
        }

        /*------- GET --------*/
        if (strcmp(first_word, "get") == 0) {
            
        }

        /*------- SHOW TOPOLOGY --------*/
        if (strcmp(first_word, "st") == 0) {
            
        }

        /*------- SHOW NAMES --------*/
        if (strcmp(first_word, "sn") == 0) {
            No no;
            show_names(&no);
        }

        /*------- SHOW ROUTING --------*/
        if (strcmp(first_word, "sr") == 0) {
            
        }

        /*------- LEAVE --------*/
        if (strcmp(first_word, "leave") == 0) {
            
        }

        /*------- EXIT --------*/
        if (strcmp(first_word, "exit") == 0) {
            printf("Exiting the program...");
            exit;
        }

}



/*--------------------------------------------*/


typedef struct me me_node;
me_node me_ptr;

int main(int argc, char *argv[]) {

    // define the structure inside main
    struct me me_ptr;

    // create a user input variable
    char input[150];

    // allocate memory for the intr array
    //int num_intr = 4;
    //my_node.intr = malloc(num_intr * sizeof(int));
    //if (my_node.intr == NULL) {
        // error handling
    //}

    // allocate the structure strings
    //me_ptr.ip=malloc(250);
    //me_ptr.ip_nodeserver=malloc(250);


    // verify the number of arguments

    if (argc != 5) {
        printf("Usage: ./cot IP TCP regIP regUDP");
    }

    else{

    // save the input info (cot IP TCP regIP regUDP) to the structure
    sscanf(argv[1],"%s", me_ptr.ip);
    sscanf(argv[2], "%s", me_ptr.port);
    sscanf(argv[3],"%s", me_ptr.ip_nodeserver);
    sscanf(argv[4], "%s", me_ptr.port_nodeserver);

    // verify the input format (cot IP TCP regIP regUDP)
    if(argv[1][3]!= '.' || argv[1][7]!='.')
    {
        printf("\n > Warning: Invalid TCP IP adress.\n");
        exit(1);
    }

    if (atoi(me_ptr.port) < 0 || atoi(me_ptr.port) > 65535) 
    {
        printf("\n > Warning: TCP Port adress.\n");
        exit(1);
    }

    if (strcmp(argv[3], "193.136.138.142") != 0) 
    {
        printf("\n > Warning: Invalid UDP IP adress for the Server of Nodes.\n");
        exit(1);
    }

    if (atoi(me_ptr.port_nodeserver) != 59000) 
    {
        printf("\n > Warning: Invalid UDP Port for the Server of Nodes.\n");
        exit(1);
    }
    
    // show the input info accepted
    printf("- My IP: %s\n", me_ptr.ip);
    printf("- My Port: %s\n", me_ptr.port);
    printf("- Server of Nodes UDP IP: %s\n", me_ptr.ip_nodeserver);
    printf("- Server of Nodes UDP Port: %s\n", me_ptr.port_nodeserver);


    // print the menu
    print_cmdmenu();


    // ------------------------ SELECT -------------------------


    // RUN SELECT
    while (1) {

            menu(input, &me_ptr);

    }


    }

    return 0;
    }


