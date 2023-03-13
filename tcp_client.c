//test.c
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

int connect(int sockfd,const struct sockaddr *serv_addr,
socklen_t addrlen);


int main(void)
{
struct addrinfo hints,*res;
int fd,n;
ssize_t nbytes,nleft,nwritten,nread;
char *ptr,buffer[128+1];


fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
if(fd==-1)exit(1);//error
memset(&hints,0,sizeof hints);
hints.ai_family=AF_INET;//IPv4
hints.ai_socktype=SOCK_STREAM;//TCP socket

//n=getaddrinfo("tejo.tecnico.ulisboa.pt","58001",&hints,&res);
n=getaddrinfo("127.0.0.1","58111",&hints,&res);

//n=getaddrinfo("193.136.138.142","59000",&hints,&res);


if(n!=0)/*error*/exit(1);
n=connect(fd,res->ai_addr,res->ai_addrlen);
if(n==-1)/*error*/exit(1);





ptr=strcpy(buffer,"experimento\n");
nbytes=8;
nleft=nbytes;

while(nleft>0){nwritten=write(fd,ptr,nleft);

if(nwritten<=0)/*error*/exit(1);
nleft-=nwritten;
ptr+=nwritten;}
nleft=nbytes; ptr=buffer;

while(nleft>0){nread=read(fd,ptr,nleft);

if(nread==-1)/*error*/exit(1);
else if(nread==0)break;//closed by peer
nleft-=nread;
ptr+=nread;}
nread=nbytes-nleft;
buffer[nread] = '\0';
printf("echo: %s\n", buffer);
close(fd);
exit(0);
}
