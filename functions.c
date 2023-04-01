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


int get(No *new_node, char *dest, char *name)
{
	ssize_t aux_bytes;
	char buffer[150];

	int aux_ver = 0;

	// ver se existem fds (conexões ao ńo)
	for (int a = 0; a < 100; a++)
			{
				if(new_node->tempfd_array[a]!=0)
				{
					aux_ver ++;
				}
				
			}


	
	for (int i = 0; i < new_node->num_entradas; i++) 
	{
		// dest esta no Destino
		if(strcmp(dest, new_node->matrix[i][0].nodes_id)==0)
		{

			// 1) condição  tempfd[] = 0 (no connections accepted), djoin/join
			if(new_node->ext_node->listen_tcp_fd != 0 && aux_ver == 0)
					{
						aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, new_node->id, name);

						aux_bytes = write(new_node->ext_node->listen_tcp_fd, buffer, aux_bytes);

					}

			
			for (int i = 0; i < new_node->num_entradas; i++)
			{
			
			// 2) condição tempfd[] != 0 (connections accepted) , no djoin/join
			if(new_node->ext_node->listen_tcp_fd == 0 && aux_ver != 0)
			{

				aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, new_node->id, name);

				int aux = atoi(new_node->matrix[i][1].nodes_id);

				aux_bytes = write(new_node->tempfd_array[aux], buffer, aux_bytes);


			}
			}



			// 3) condição tempfd[] != 0 (connections accepted) + djoin/join
			if(new_node->ext_node->listen_tcp_fd != 0 && aux_ver != 0)
			{
				for (int i = 0; i < new_node->num_entradas; i++)
					{

				aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, new_node->id, name);

				int aux = atoi(new_node->matrix[i][0].nodes_id);

				aux_bytes = write(new_node->tempfd_array[aux], buffer, aux_bytes);
			
					}

				aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, new_node->id, name);


				aux_bytes = write(new_node->ext_node->listen_tcp_fd, buffer, aux_bytes);	


			}



		}

		// dest não esta no Destino
		else if(strcmp(dest, new_node->matrix[i][0].nodes_id)!=0)
		{

			// 1) condição  tempfd[] = 0 (no connections accepted), djoin/join
			if(new_node->ext_node->listen_tcp_fd != 0 && aux_ver == 0)
					{
						aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, new_node->id, name);

						aux_bytes = write(new_node->ext_node->listen_tcp_fd, buffer, aux_bytes);

					}

			
			for (int i = 0; i < new_node->num_entradas; i++)
			{
				
				// 2) condição tempfd[] != 0 (connections accepted) , no djoin/join
				if(new_node->ext_node->listen_tcp_fd == 0 && aux_ver != 0)
				{

					aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, new_node->id, name);

					int aux = atoi(new_node->matrix[i][0].nodes_id);

					aux_bytes = write(new_node->tempfd_array[aux], buffer, aux_bytes);

				}
			}


				// 3) condição tempfd[] != 0 (connections accepted) + djoin/join
				if(new_node->ext_node->listen_tcp_fd != 0 && aux_ver != 0)
				{
					for (int i = 0; i < new_node->num_entradas; i++)
						{

					aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, new_node->id, name);

					int aux = atoi(new_node->matrix[i][0].nodes_id);

					aux_bytes = write(new_node->tempfd_array[aux], buffer, aux_bytes);
				
						}

					aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, new_node->id, name);


					aux_bytes = write(new_node->ext_node->listen_tcp_fd, buffer, aux_bytes);	


				}

		}

		break;
	}

	//bzero(buffer, 150);


	return 0;
}


int leave(No *new_node)

{
    int fd_i;
	struct linger lo = { 1, 0 };

	fd_set testfds;

	char buffer[128];
	ssize_t aux_bytes;


	if(new_node->ext_node->listen_udp_fd!=0)
	{
		
		aux_bytes = sprintf(buffer, "UNREG %s %s\n", new_node->my_net, new_node->id);

		aux_bytes=sendto(new_node->ext_node->listen_udp_fd,buffer,aux_bytes,0, new_node->listen_udp_res->ai_addr,new_node->listen_udp_res->ai_addrlen);	

	}



	// MANDAR WITHDRAWS


	//---------------------- terminal do leave

    if(strcmp(new_node->id, new_node->bck_node->id) == 0) // Saída de Nó ancora
            {
                
			for (int i = 0; i < 100; i++)
			{
				if(new_node->tempfd_array[i]!=0)
				{
				//printf("FDS no close: %d\n", new_node->tempfd_array[i]);
				close(new_node->tempfd_array[i]);
				//printf("FDS no close: %d\n", new_node->tempfd_array[i]);
				new_node->tempfd_array[i] = 0;
				}
			}

	
			if(new_node->listen_tcp_fd!=0)
			{
			close(new_node->listen_tcp_fd);
			new_node->listen_tcp_fd = 0;
			}

			if(new_node->ext_node->listen_tcp_fd!=0)
			{
			close(new_node->ext_node->listen_tcp_fd);
			new_node->ext_node->listen_tcp_fd = 0;
			}

			
			if(new_node->ext_node->listen_udp_fd==0)
			{
			bzero(new_node->ext_node, sizeof(No));	
			bzero(new_node->bck_node, sizeof(No));
			}


			//remove interns from the structure
			int i;
			for (i = 0; i < new_node->num_nodes; i++) {
			memset(&new_node->intr_nodes[i], 0, sizeof(NodeInfo)); // set all values in the node to zero
			}
			new_node->num_nodes = 0; // set the number of nodes to zero

	
			memset(new_node->id, 0, sizeof(new_node->id)); // set all values in the node to zero
	
			}



    if(strcmp(new_node->id, new_node->bck_node->id) != 0)
    {
      
		for (int i = 0; i < 100; i++)
		{
			if(new_node->tempfd_array[i]!=0)
			{
			close(new_node->tempfd_array[i]);
			new_node->tempfd_array[i] = 0;
			}
		}


		if(new_node->listen_tcp_fd!=0)
			{
			close(new_node->listen_tcp_fd);
			new_node->listen_tcp_fd = 0;
			}


		if(new_node->ext_node->listen_tcp_fd!=0)
			{
			close(new_node->ext_node->listen_tcp_fd);
			new_node->ext_node->listen_tcp_fd = 0;
			}


		if(new_node->ext_node->listen_udp_fd==0)
			{
			bzero(new_node->ext_node, sizeof(No));	
			bzero(new_node->bck_node, sizeof(No));
			}

        //remove interns from the structure
        int i;
        for (i = 0; i < new_node->num_nodes; i++) {
        memset(&new_node->intr_nodes[i], 0, sizeof(NodeInfo)); // set all values in the node to zero
        }
        new_node->num_nodes = 0; // set the number of nodes to zero

		memset(new_node->id, 0, sizeof(new_node->id)); // set id to zero


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

	char buffer[500], msg[10], id_msg[3], ip_msg[128], port_msg[6];
	struct linger lo = { 1, 0 };
	char orig[3], dest[3], name[128]; // auxiliar QUERY

	//join
	char net[04];
	


	bzero(buffer, 500);

	fd_set testfds;

	char fd_temporario[03];

	aux_bytes=read(fd,buffer,500);  // aux_bytes=read(tempfd,buffer,128);

	for(int i = 0; i<100; i++)
	{
		if(new_node->tempfd_array[i] == fd)
		{
			sprintf(fd_temporario, "%02d", i);
		}
	}

	if (aux_bytes == -1)
		{
			perror("read");
			//leave(new_node);
			exit(1);
		}

	if (aux_bytes == 0) // 05 (leave) ->  º  01

		{
			printf("AQUI V0");

			// caso 1) //nao deu djoin | 03 dá leave
			if(new_node->ext_node->listen_tcp_fd == 0)
			{

			printf("AQUI V1");

			int fd_aux = 0;
			fd_aux = atoi(fd_temporario);

			close(new_node->tempfd_array[fd_aux]);
			new_node->tempfd_array[fd_aux] = 0;

			//withdraw caso 1
			for (int m = 0; m < 100; m++)
				{
					if(new_node->tempfd_array[m]!=0)
					{
						aux_bytes = sprintf(buffer, "WITHDRAW %s \n", fd_aux);

						aux_bytes = write(new_node->tempfd_array[m],buffer,aux_bytes);
					}
				}

			// tirar os bacanos do widthdraw
				int nentry_aux = 0;
				nentry_aux = new_node->num_entradas;

				for (int i = 0; i < nentry_aux; i++)
				{
					if(strcmp(new_node->matrix[i][0].nodes_id, fd_aux)==0 || strcmp(new_node->matrix[i][1].nodes_id, fd_aux)==0)
					{
						strcpy(new_node->matrix[i][0].nodes_id, "");
						strcpy(new_node->matrix[i][1].nodes_id, "");
						
						for(int l = i; l < nentry_aux; l++)
						{
							strcpy(new_node->matrix[l-1][0].nodes_id, new_node->matrix[l][0].nodes_id);
							strcpy(new_node->matrix[l-1][1].nodes_id, new_node->matrix[l][1].nodes_id);

						}
						new_node->num_entradas--;


					}
				}







				if(strcmp(fd_temporario, new_node->ext_node->id)==0)
				{	

					// colocar como externo o primeiro interno (promover 1º interno a ancora)
					strcpy(new_node->ext_node->id, new_node->intr_nodes[0].id);
					strcpy(new_node->ext_node->ip, new_node->intr_nodes[0].ip);
					strcpy(new_node->ext_node->port, new_node->intr_nodes[0].port);


					// atualizar com o externo e consequentemente mudar o backup
					for (int d=0; d < 100; d++)
					{
						if(new_node->tempfd_array[d]!=0)
						{
						aux_bytes = sprintf(buffer, "EXTERN %s %s %s\n", new_node->ext_node->id, new_node->ext_node->ip, new_node->ext_node->port);

						aux_bytes = write(new_node->tempfd_array[d],buffer,aux_bytes);	
						}		
					}	


					for(int z=1; z < new_node->num_nodes; z++)
					{
						strcpy(new_node->intr_nodes[z-1].id, new_node->intr_nodes[z].id);
						strcpy(new_node->intr_nodes[z-1].ip, new_node->intr_nodes[z].ip);
						strcpy(new_node->intr_nodes[z-1].port, new_node->intr_nodes[z].port);
						strcpy(new_node->intr_nodes[z-1].intr_fd, new_node->intr_nodes[z].intr_fd);
					}
					new_node->num_nodes--;
				



					// tabelas de expediçao!!!!

				}

			}
			
			// caso 02) 01 da leave : 03
			if(new_node->ext_node->listen_tcp_fd != 0 && strcmp(new_node->id, new_node->bck_node->id)==0)
			{

				printf("AQUI V2");

				close(new_node->ext_node->listen_tcp_fd);
				new_node->ext_node->listen_tcp_fd = 0;

				// withdraw case 02 
				for (int m = 0; m < 100; m++)
				{
					if(new_node->tempfd_array[m]!=0)
					{
						aux_bytes = sprintf(buffer, "WITHDRAW %s \n", new_node->ext_node->id);

						aux_bytes = write(new_node->tempfd_array[m],buffer,aux_bytes);
					}
				}

				// tirar os bacanos do widthdraw
				int nentry_aux = 0;
				nentry_aux = new_node->num_entradas;

				for (int i = 0; i < nentry_aux; i++)
				{
					if(strcmp(new_node->matrix[i][0].nodes_id, new_node->ext_node->id)==0 || strcmp(new_node->matrix[i][1].nodes_id, new_node->ext_node->id)==0)
					{
						strcpy(new_node->matrix[i][0].nodes_id, "");
						strcpy(new_node->matrix[i][1].nodes_id, "");
						
						for(int l = i; l < nentry_aux; l++)
						{
							strcpy(new_node->matrix[l-1][0].nodes_id, new_node->matrix[l][0].nodes_id);
							strcpy(new_node->matrix[l-1][1].nodes_id, new_node->matrix[l][1].nodes_id);

						}
						new_node->num_entradas--;


					}
				}





				// se nao tiver nenhum interno
				if(new_node->num_nodes!=0)
				{

				// colocar como externo o primeiro interno (promover 1º interno a ancora)
				strcpy(new_node->ext_node->id, new_node->intr_nodes[0].id);
				strcpy(new_node->ext_node->ip, new_node->intr_nodes[0].ip);
				strcpy(new_node->ext_node->port, new_node->intr_nodes[0].port);


					// atualizar com o externo e consequentemente mudar o backup
					for (int d=0; d < 100; d++)
					{
						if(new_node->tempfd_array[d]!=0) // 08    d != ext node id
						{
							int fdnodeid = 0;
							fdnodeid = atoi(new_node->ext_node->id);
							if(d != fdnodeid)
								{
								aux_bytes = sprintf(buffer, "EXTERN %s %s %s\n", new_node->ext_node->id, new_node->ext_node->ip, new_node->ext_node->port);

								aux_bytes = write(new_node->tempfd_array[d],buffer,aux_bytes);	
								}		
						}
					}	


				for(int x=1; x < new_node->num_nodes; x++)
					{
						strcpy(new_node->intr_nodes[x-1].id, new_node->intr_nodes[x].id);
						strcpy(new_node->intr_nodes[x-1].ip, new_node->intr_nodes[x].ip);
						strcpy(new_node->intr_nodes[x-1].port, new_node->intr_nodes[x].port);
						strcpy(new_node->intr_nodes[x-1].intr_fd, new_node->intr_nodes[x].intr_fd);
					}
					new_node->num_nodes--;

				}

				if(new_node->num_nodes==0)
				{
					
					strcpy(new_node->ext_node->id, new_node->id);

				}

			}



			//caso 3) caso 07 conectado a 05
			if(new_node->ext_node->listen_tcp_fd != 0 && strcmp(new_node->id, new_node->bck_node->id)!=0)
			{

				printf("AQUI V3");

				close(new_node->ext_node->listen_tcp_fd);
				new_node->ext_node->listen_tcp_fd = 0;

				// withdraw case 02 
				for (int m = 0; m < 100; m++)
				{
					if(new_node->tempfd_array[m]!=0)
					{
						aux_bytes = sprintf(buffer, "WITHDRAW %s \n", new_node->ext_node->id);

						aux_bytes = write(new_node->tempfd_array[m],buffer,aux_bytes);
					}
				}


				// tirar os bacanos do widthdraw
				int nentry_aux = 0;
				nentry_aux = new_node->num_entradas;

				for (int i = 0; i < nentry_aux; i++)
				{
					if(strcmp(new_node->matrix[i][0].nodes_id, new_node->ext_node->id)==0 || strcmp(new_node->matrix[i][1].nodes_id, new_node->ext_node->id)==0)
					{
						strcpy(new_node->matrix[i][0].nodes_id, "");
						strcpy(new_node->matrix[i][1].nodes_id, "");
						
						for(int l = i; l < nentry_aux; l++)
						{
							strcpy(new_node->matrix[l-1][0].nodes_id, new_node->matrix[l][0].nodes_id);
							strcpy(new_node->matrix[l-1][1].nodes_id, new_node->matrix[l][1].nodes_id);

						}
						new_node->num_entradas--;


					}
				}


				djoin(new_node, new_node->my_net, new_node->id, new_node->bck_node->id, new_node->bck_node->ip,new_node->bck_node->port);



			}

			return -1;
		}


	sscanf(buffer, "%s",msg);
	printf("Recebi: %s\n:", buffer);


	printf("msg_received: %d\n", msg_received(msg));

	switch(msg_received(msg))
	{
		case NEW: // NEW 02(id_msg) IP02(ip_msg) PORT02(port_msg)

			sscanf(buffer, "%s %s %s %s", msg, id_msg, ip_msg, port_msg);


			// update tabela de expedição
			if (new_node->num_dv < 2){
			new_node->num_dv++;}
			new_node->num_entradas++;

			for (int i = 0; i < new_node->num_entradas; i++) {
				for(int j = 0; j < new_node->num_dv; j++){

					if (strcmp(new_node->matrix[i][j].nodes_id,"")==0)
					{
					strcpy(new_node->matrix[i][j].nodes_id, id_msg);
					}
				}
			}
		

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

				strcpy(new_node->intr_nodes[new_node->num_nodes].id, id_msg);
				strcpy(new_node->intr_nodes[new_node->num_nodes].ip, ip_msg);
				strcpy(new_node->intr_nodes[new_node->num_nodes].port, port_msg);
				
				sprintf(new_node->intr_nodes[new_node->num_nodes].intr_fd, "%d", fd);


				// increment the number of nodes
				new_node->num_nodes++;

				aux_bytes = sprintf(buffer, "EXTERN %s %s %s\n", new_node->ext_node->id, new_node->ext_node->ip, new_node->ext_node->port);

				aux_bytes = write(fd,buffer,aux_bytes);
				
			}
			
			int indice_fd = atoi(id_msg);
			new_node->tempfd_array[indice_fd] = fd;
		

			break;


		case EXTERN:

			sscanf(buffer, "%s %s %s %s", msg, id_msg, ip_msg, port_msg);

			strcpy(new_node->bck_node->id, id_msg);
			strcpy(new_node->bck_node->ip, ip_msg);
			strcpy(new_node->bck_node->port, port_msg);

			// update tabela de expedição
			// caso nó não âncora a receber extern
			if(strcmp(new_node->id, new_node->bck_node->id)!=0)
			{

				if (new_node->num_dv < 2){
					new_node->num_dv++;}
					new_node->num_entradas++;

					for (int i = 0; i < new_node->num_entradas; i++) {
						for(int j = 0; j < new_node->num_dv; j++){

							if (strcmp(new_node->matrix[i][j].nodes_id,"")==0){
								if(j==0)
								{
								strcpy(new_node->matrix[i][j].nodes_id, id_msg);
								}
								else
								{
								strcpy(new_node->matrix[i][j].nodes_id, new_node->ext_node->id);
								}
							}

						}
					}
			}


		

			break;

		case QUERY: 
			


			sscanf(buffer, "%s %s %s %s", msg, dest, orig, name);

			// dest == Node id (QUERY 03 05 teste.pdf entra no NÓ 03) (CHECKED)
			if(strcmp(dest,new_node->id)==0)
			{	

					if (new_node->num_dv < 2){
					new_node->num_dv++;}
					new_node->num_entradas++;

					for (int i = 0; i < new_node->num_entradas; i++) {
						for(int j = 0; j < new_node->num_dv; j++){

							if (strcmp(new_node->matrix[i][j].nodes_id,"")==0){
								if(j==0)
								{
								strcpy(new_node->matrix[i][j].nodes_id, orig);
								}
								else
								{
								strcpy(new_node->matrix[i][j].nodes_id, new_node->ext_node->id);
								}
							}

						}
					}


					// percorrer a lista dos conteudos
					for (int i; i < new_node->num_conteudos; i++)
					{
						// se o nome do conteudo do QUERY = nome do conteudo do ńo
						if(strcmp(name, new_node->conteudos->name)==0)
						{

							int auxiliar_cont04 = 0;
							auxiliar_cont04 = new_node->num_entradas;
							int contador_04 = 0;

							if(new_node->ext_node->listen_tcp_fd != 0)
							{
						
							aux_bytes = sprintf(buffer, "CONTENT %s %s %s\n", orig, dest, name);

							aux_bytes = write(new_node->ext_node->listen_tcp_fd, buffer,aux_bytes);
							}

						
							/*aux_bytes = sprintf(buffer, "CONTENT %s %s %s\n", orig, dest, name);

							int indice_dest = atoi(dest);

							aux_bytes = write(new_node->tempfd_array[indice_dest],buffer,aux_bytes);*/



							for (int r = 0; r < auxiliar_cont04; r++)
								{
								if (strcmp(orig, new_node->matrix[r][0].nodes_id) == 0)
								{
									char indice_auxi[03]; 

									strcpy(indice_auxi,new_node->matrix[r][1].nodes_id);
									
									aux_bytes = sprintf(buffer, "CONTENT %s %s %s\n", orig, dest, name);

									int indice_auxi_ = atoi(indice_auxi);

									aux_bytes = write(new_node->tempfd_array[indice_auxi_],buffer,aux_bytes);
								}
								}

						}
						else // se o nome do conteudo do QUERY 1= (não houver esse ficheiro naquele nó destino)
						{
							if(new_node->ext_node->listen_tcp_fd != 0)
							{
							aux_bytes = sprintf(buffer, "NOCONTENT %s %s %s\n", orig, dest, name);

							aux_bytes = write(new_node->ext_node->listen_tcp_fd, buffer,aux_bytes);
							}
							
							else{
							aux_bytes = sprintf(buffer, "NOCONTENT %s %s %s\n", orig, dest, name);

							int indice_dest = atoi(dest);

							aux_bytes = write(new_node->tempfd_array[indice_dest],buffer,aux_bytes);
							}

						}
						break;

					}

			}


			// dest != Node ID  	QUERY 10 05 test.txt  :01 (FALTA EXPEDICAO!!!)
			else if(strcmp(dest, new_node->id) != 0 && strcmp(orig, new_node->id) != 0)
			{	
				int cntr = 0;
				int auxiliar = 0;
				auxiliar = new_node->num_entradas;
				int contador_02 = 0;
				int auxiliar_cont02 = 0;
				auxiliar_cont02 = new_node->num_entradas;
				int cntr_1 = 0;

				// Verificação da tabela de expedição
				for (int i = 0; i < auxiliar; i++) 
				{
					
					// caso dest está no Destino CASO 08 
					if(strcmp(dest, new_node->matrix[i][0].nodes_id)==0)
					{
						

						aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, orig, name);

						int indice_dest = atoi(dest);

						aux_bytes = write(new_node->tempfd_array[indice_dest],buffer,aux_bytes);

					
					// caso o destino seja o externo do proprio nó manda por new_node->ext_node->listen_tcp_fd 8fd externo)
						if(new_node->ext_node->listen_tcp_fd != 0 && strcmp(new_node->ext_node->id, dest)==0) // se ele tiver dado djoin listen_tcp_fd ->>>>>>
						{
							

							aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, orig, name);

							int aux = atoi(new_node->matrix[i][0].nodes_id);

							aux_bytes = write(new_node->ext_node->listen_tcp_fd,buffer,aux_bytes);

						}

						//cntr ++;

						// No need to update expedition table "query"

						//new_node->num_entradas++;

						strcpy(new_node->matrix[auxiliar][0].nodes_id, orig);
						strcpy(new_node->matrix[auxiliar][1].nodes_id, new_node->ext_node->id);
					
						//new_node->num_entradas++;


					} 

					if(strcmp(dest, new_node->matrix[i][0].nodes_id)!=0) // 02 != 04(dest)  //  04 != 05 
					{
						
						// 03 ->>>> avaliar 01 (correto) || 05 != 03
						if(strcmp(orig, new_node->matrix[i][0].nodes_id)!=0)
							{

								cntr_1++;

								int contador_10 = 0;
								int auxiliar_cont10 = 0;
								auxiliar_cont10 = new_node->num_entradas;
							
								int contador_03 = 0;
								int auxiliar_cont03 = 0;
								auxiliar_cont03 = new_node->num_entradas;
								


								for (int g = 0; g < auxiliar_cont10+1; g++)
								{
								if (strcmp(dest, new_node->matrix[g][0].nodes_id) == 0)
								{
									contador_10++;
		
								}
								}

					

								// 04 04 // 02 02 04 04

								if(strcmp(new_node->matrix[i][0].nodes_id, dest)==0 && contador_10!=0)
								{

									aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, orig, name);

									int aux_destino = atoi(new_node->matrix[i][1].nodes_id);

									aux_bytes = write(new_node->tempfd_array[aux_destino],buffer,aux_bytes);
									

								}



								for (int a = 0; a < new_node->num_entradas; a++)
								{

								//query -> fd
								if(strcmp(fd_temporario, new_node->matrix[a][1].nodes_id)!=0 && strcmp(new_node->matrix[a][1].nodes_id, dest)!=0 && contador_10==0)
								{

								//printf(" foi aqui que entrei v2");

								aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, orig, name);

								int aux = atoi(new_node->matrix[a][1].nodes_id);

								aux_bytes = write(new_node->tempfd_array[aux],buffer,aux_bytes);

								}
								}




								for (int p = 0; p < new_node->num_nodes; p++)
								{

									int aux__ = atoi(new_node->intr_nodes[p].intr_fd);


								if(new_node->ext_node->listen_tcp_fd != 0 && fd == aux__ && strcmp(new_node->matrix[i][0].nodes_id,new_node->intr_nodes[p].id) == 0) 
								{
									printf("entrei aqui");

									aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, orig, name);

									aux_bytes = write(new_node->ext_node->listen_tcp_fd,buffer,aux_bytes);

								}
								}

								

								for (int k = 0; k < auxiliar_cont03+1; k++)
								{
								if (strcmp(orig, new_node->matrix[k][0].nodes_id) == 0)
								{
									contador_03++;
									printf("contador: %d", contador_03);

								}
								}

								if (contador_03 == 0)
								{
									if (new_node->num_dv < 2){
										new_node->num_dv++;}
										new_node->num_entradas++;

										strcpy(new_node->matrix[new_node->num_entradas-1][0].nodes_id, orig);
										strcpy(new_node->matrix[new_node->num_entradas-1][1].nodes_id, new_node->ext_node->id);

								}

							}


						if(strcmp(orig, new_node->matrix[i][0].nodes_id)==0) // 03 orig, 03 exp
						{

							printf("entrei! %s %s\n", orig, new_node->matrix[i][0].nodes_id);
							
							for (int t = 0; t < new_node->num_nodes; t++)
								{

									printf("intr nodes p id: %s\n", new_node->intr_nodes[t].id); 
									printf("fd temporario: %s\n", fd_temporario);
									int aux_farto = atoi(new_node->intr_nodes[t].intr_fd);

								if(new_node->ext_node->listen_tcp_fd != 0 && fd == aux_farto && strcmp(new_node->matrix[i][0].nodes_id,new_node->intr_nodes[t].id) == 0) 
								{
									printf("entrei aqui");

									aux_bytes = sprintf(buffer, "QUERY %s %s %s\n", dest, orig, name);

									aux_bytes = write(new_node->ext_node->listen_tcp_fd,buffer,aux_bytes);

								}
								}
			

							for (int k = 0; k < auxiliar_cont02; k++)
								{
								if (strcmp(orig, new_node->matrix[k][0].nodes_id) == 0)
								{
									contador_02++;

								}
								}

								if (contador_02 == 0)
								{
									if (new_node->num_dv < 2){
										new_node->num_dv++;}
										new_node->num_entradas++;

										strcpy(new_node->matrix[new_node->num_entradas-1][0].nodes_id, orig);
										strcpy(new_node->matrix[new_node->num_entradas-1][1].nodes_id, new_node->ext_node->id);

								}

						}
					}

				}

			}

		break;

		case CONTENT:
		
			sscanf(buffer, "%s %s %s %s", msg, orig, dest, name);


			int cntr_aux = 0;
			int auxiliar_cont = 0;
			auxiliar_cont = new_node->num_entradas;
			int contador = 0;

			// CONTENT 10 05 test.txt  NÓ 01 
			if (strcmp(new_node->id, orig)!=0)
			{

				cntr_aux ++;

				/*aux_bytes = sprintf(buffer, "CONTENT %s %s %s\n", orig, dest, name);

				int indice_orig = atoi(orig);

				aux_bytes = write(new_node->tempfd_array[indice_orig],buffer,aux_bytes);*/

				
				
				for(int b = 0; b < new_node->num_entradas; b++)
				{
				if(strcmp(orig, new_node->matrix[b][0].nodes_id)==0)
				{

					if(strcmp(new_node->matrix[b][1].nodes_id, new_node->ext_node->id)!=0 || strcmp(new_node->id, new_node->bck_node->id));
					{
					aux_bytes = sprintf(buffer, "CONTENT %s %s %s\n", orig, dest, name);

					int aux_fartalhao = atoi(new_node->matrix[b][1].nodes_id);

					aux_bytes = write(new_node->tempfd_array[aux_fartalhao],buffer,aux_bytes);
					}

					if(strcmp(new_node->matrix[b][1].nodes_id, new_node->ext_node->id)==0)

					{

						aux_bytes = sprintf(buffer, "CONTENT %s %s %s\n", orig, dest, name);

						aux_bytes = write(new_node->ext_node->listen_tcp_fd,buffer,aux_bytes);


					}
						
				
				}
				}


				for (int i = 0; i < auxiliar_cont; i++)
				{
				if (strcmp(dest, new_node->matrix[i][0].nodes_id) == 0)
				{
					contador++;
				}
				}

				if (contador == 0)
				{
					if (new_node->num_dv < 2){
						new_node->num_dv++;}
						new_node->num_entradas++;

						strcpy(new_node->matrix[new_node->num_entradas-1][0].nodes_id, dest);
						strcpy(new_node->matrix[new_node->num_entradas-1][1].nodes_id, fd_temporario);
				}

		

			}


			// orig nó 05 , id nó 05 (CHECKED) // 03 03
			else if(strcmp(new_node->id, orig)==0 && cntr_aux==0)
			{

				if (new_node->num_dv < 2){
					new_node->num_dv++;}
					new_node->num_entradas++;

				for (int i = 0; i < new_node->num_entradas; i++) {
						for(int j = 0; j < new_node->num_dv; j++){

							if (strcmp(new_node->matrix[i][j].nodes_id,"")==0){
								if(j==0)
								{
								strcpy(new_node->matrix[i][j].nodes_id, dest);
								}
								else
								{
								strcpy(new_node->matrix[i][j].nodes_id, new_node->ext_node->id);
								}
							}

						}
					}



			}


		break;

		case NOCONTENT:

		//igual ao content até agr , usar wireshark para ver envio!

		sscanf(buffer, "%s %s %s %s", msg, orig, dest, name);


			int cntr_aux1 = 0;
			int auxiliar_cont1 = 0;
			auxiliar_cont1 = new_node->num_entradas;
			int contador1 = 0;

			// CONTENT 10 05 test.txt  NÓ 01 
			if (strcmp(new_node->id, orig)!=0)
			{

				cntr_aux1 ++;

				aux_bytes = sprintf(buffer, "NOCONTENT %s %s %s\n", orig, dest, name);

				int indice_orig = atoi(orig);

				aux_bytes = write(new_node->tempfd_array[indice_orig],buffer,aux_bytes);

			
				if (new_node->ext_node->listen_tcp_fd != 0)
				{

					aux_bytes = sprintf(buffer, "NOCONTENT %s %s %s\n", orig, dest, name);

					aux_bytes = write(new_node->ext_node->listen_tcp_fd,buffer,aux_bytes);

				}

				for (int i = 0; i < auxiliar_cont1; i++)
				{
				if (strcmp(dest, new_node->matrix[i][0].nodes_id) == 0)
				{
					contador1++;
				}
				}

				if (contador1 == 0)
				{
					if (new_node->num_dv < 2){
						new_node->num_dv++;}
						new_node->num_entradas++;

						strcpy(new_node->matrix[new_node->num_entradas-1][0].nodes_id, dest);
						strcpy(new_node->matrix[new_node->num_entradas-1][1].nodes_id, fd_temporario);
				}
		

			}


			// orig nó 05 , id nó 05 (CHECKED) 
			else if(strcmp(new_node->id, orig)==0 && cntr_aux1==0)
			{

				if (new_node->num_dv < 2){
					new_node->num_dv++;}
					new_node->num_entradas++;

				for (int i = 0; i < new_node->num_entradas; i++) {
						for(int j = 0; j < new_node->num_dv; j++){

							if (strcmp(new_node->matrix[i][j].nodes_id,"")==0){
								if(j==0)
								{
								strcpy(new_node->matrix[i][j].nodes_id, dest);
								}
								else
								{
								strcpy(new_node->matrix[i][j].nodes_id, new_node->ext_node->id);
								}
							}

						}
					}



			}

		break;

		case WITHDRAW:
			
			sscanf(buffer, "%s %s", msg, id_msg);

			for (int m = 0; m < 100; m++)
				{
					if(new_node->tempfd_array[m]!=0)
					{
						aux_bytes = sprintf(buffer, "WITHDRAW %s \n", id_msg);

						aux_bytes = write(new_node->tempfd_array[m],buffer,aux_bytes);
					}
				}


			int nentry_aux = 0;
			nentry_aux = new_node->num_entradas;

			for (int i = 0; i < nentry_aux; i++)
			{
				if(strcmp(new_node->matrix[i][0].nodes_id, id_msg)==0 || strcmp(new_node->matrix[i][1].nodes_id, id_msg)==0)
				{
					strcpy(new_node->matrix[i][0].nodes_id, "");
					strcpy(new_node->matrix[i][1].nodes_id, "");
					
					for(int l = i; l < nentry_aux; l++)
					{
						strcpy(new_node->matrix[l-1][0].nodes_id, new_node->matrix[l][0].nodes_id);
						strcpy(new_node->matrix[l-1][1].nodes_id, new_node->matrix[l][1].nodes_id);

					}
					new_node->num_entradas--;


				}
			}

			




		break;

		

		//join
		case NODESLIST:

		sscanf(buffer, "%s %s", msg, net);

		printf("%d", strlen(buffer));


		/*for (int i = 14; i<strlen(buffer); i++)
		{
			printf("Valor do elemento %d da string = %c\n",i, buffer[i]);
		}*/


		aux_bytes = sprintf(buffer, "REG %s %s %s %s\n", net, new_node->id, new_node->ip, new_node->port);

		aux_bytes=sendto(new_node->ext_node->listen_udp_fd,buffer,aux_bytes,0, new_node->listen_udp_res->ai_addr,new_node->listen_udp_res->ai_addrlen);

		printf("comida bem forte\n");

		//create_tree(new_node);


		break;

		case OKUNREG:

		
		bzero(new_node->ext_node, sizeof(No));	
		bzero(new_node->bck_node, sizeof(No));
		

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

	// update tabela de expedição com djoin
	if (new_node->num_dv < 2){
		new_node->num_dv++;}
	new_node->num_entradas++;

	for (int i = 0; i < new_node->num_entradas; i++) {
		for(int j = 0; j < new_node->num_dv; j++){

			if (strcmp(new_node->matrix[i][j].nodes_id,"")==0){
			strcpy(new_node->matrix[i][j].nodes_id, id_boot);
			}
		}
	}




return 0;
}

void show_topology(No *new_node)
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


void show_routing(No *new_node)	
{

	printf("\tDestino\t\tVizinho\t");
	printf("\n");

    for (int i = 0; i < new_node->num_entradas; i++) {
		for (int j = 0; j < new_node->num_dv; j++)
		{
        printf("\t%s\t ", new_node->matrix[i][j].nodes_id);
		}
	printf("\n");
    }


}

void clear_routing(No *new_node)
{

	for (int i = 0; i < new_node->num_entradas; i++) 
	{
		for (int j = 0; j < new_node->num_dv; j++)
		{
		memset(&new_node->matrix[i][j], 0, sizeof(expedicao));
		}
	}
	
}

void show_fds(No *new_node)
{

	for (int i = 0; i < 100; i++) 
	{
		printf("%d %d\n", new_node->tempfd_array[i], i);
	}
	
}


int join(No *new_node, char *net, char *id)
{

	//UDP CLIENT
	int fd,errcode;
	ssize_t n;
	socklen_t addrlen;

	struct addrinfo hints, *res;
	struct sockaddr_in addr;
	char buffer[128];

	fd_set testfds;


	fd = socket(AF_INET,SOCK_DGRAM,0); //UDP socket
	if(fd==-1) //error
	{
		leave(new_node);
		exit(1);
	}


	memset(&new_node->listen_udp_hints,0,sizeof (new_node->listen_udp_hints));
	new_node->listen_udp_hints.ai_family=AF_INET; //IPv4
	new_node->listen_udp_hints.ai_socktype=SOCK_DGRAM; //UDP socket
	

	errcode=getaddrinfo(new_node->ip_nodeserver,new_node->port_nodeserver,&new_node->listen_udp_hints,&new_node->listen_udp_res);

	if(errcode!=0) //error
	{
		exit(1);
	}

	bzero(buffer,128);
	n = sprintf(buffer, "NODES %s\n", net);

	n=sendto(fd,buffer,n,0,new_node->listen_udp_res->ai_addr,new_node->listen_udp_res->ai_addrlen);

	if(n==-1)
	{
		exit(1);
	}

	addrlen=sizeof(new_node->listen_udp_addr);


	new_node->ext_node->listen_udp_fd = fd;
	

	return 0;
}