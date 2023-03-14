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

// structures:

typedef struct {
    char name[100];
    
} Conteudo;

// Me node structure
typedef struct me{

    char *ip;
    int port;

    char *ip_nodeserver;
    int port_nodeserver;

    int id; // my id
    int ext; // extern neighborhood id
    int bck; // backup neighborhood id

    int my_net; // net nº where I belong


    Conteudo conteudos[100]; // array de conteúdos
    int num_conteudos; // actual nº of conteudos in the list

} No;


// auxiliar functions

// function to print the command menu
void print_menu() {

  printf("\n--------------------Command Menu-------------------\n");  
  printf("1) join net id: Entrada de um nó na rede net com identificador id.\n");
  printf("2) djoin net id: bootid bootIP bootTCP Entrada de um nó na rede net com identificador id,\n");
  printf("   que se sabe ser único na rede. É passado à aplicação o identificador e o contacto de um nó da rede,\n");
  printf("   através dos parâmetros bootid, bootIP e bootTCP, ao qual o nó se deverá ligar sem interrogar o servidor de nós.\n");
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


typedef struct me me_node;
me_node me_ptr;

int main(int argc, char *argv[]) {

    // define the structure inside main
    struct me me_ptr;

    // create a user input variable
    char input[500];

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


    // print the menu
    print_menu();
    while (1) {
        printf("\n>> Command input: ");
        fgets(input, 100, stdin);
        input[strcspn(input, "\n")] = 0; // Remove the newline character

        char *first_word = strtok(input, " ");

        if (strcmp(first_word, "join") == 0) {
            printf("Command selected: %s\n", first_word);
        }

        if (strcmp(first_word, "djoin") == 0) {
            printf("Command selected: %s\n", first_word);
        }

        if (strcmp(first_word, "create") == 0) {
            printf("Command selected: %s\n", first_word);

            // takes the content name and saves it to the structure
            char *content_name = strtok(NULL," ");
            if (content_name == NULL) { 
                printf(">> Error: Not a valid content.\n");
            } else {
            No no;
            create(content_name, &no); // passa o nome do conteúdo e o ponteiro para a estrutura No para a função create()
            }

        }

        if (strcmp(first_word, "delete") == 0) {
            printf("Command selected: %s\n", first_word);
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

        if (strcmp(first_word, "get") == 0) {
            printf("Command selected: %s\n", first_word);
        }

        if (strcmp(first_word, "show") == 0) {
            printf("Command selected: %s\n", first_word);
        }

        if (strcmp(first_word, "sn") == 0) {
            printf("Command selected: %s\n", first_word);
            No no;
            show_names(&no);
        }

        if (strcmp(first_word, "sr") == 0) {
            printf("Command selected: %s\n", first_word);
        }

        if (strcmp(first_word, "leave") == 0) {
            printf("Command selected: %s\n", first_word);
        }

        if (strcmp(first_word, "exit") == 0) {
            printf("Command selected: %s\n", first_word);
            printf("Exiting the program...");
            break;
        }

        print_menu();


    }


    }

    return 0;
    }


