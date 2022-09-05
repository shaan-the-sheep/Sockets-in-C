#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

int main()
{
	struct addrinfo hints,*server;
	int r,sockfd,clientfd;
	struct sockaddr clientaddr;
	socklen_t client_len;
	const int size = 1024;
	char buffer[size];

	/* configure the server to use the localhost, port 8080, TCP */
	memset( &hints, 0, sizeof(struct addrinfo) );	/* use memset_s() */
	hints.ai_family = AF_INET;			/* IPv4 connection */
	hints.ai_socktype = SOCK_STREAM;	/* TCP, streaming */
	hints.ai_flags = AI_PASSIVE;		/* listen to any interface */
	r = getaddrinfo( 0, "8080", &hints, &server );
	if( r!= 0 )
	{
		perror("failed");
		exit(1);
	}

	/* create the socket to use for communications */
	sockfd = socket(server->ai_family,server->ai_socktype,server->ai_protocol);
	if( sockfd==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* bind the server to a socket */
	r = bind(sockfd,server->ai_addr,server->ai_addrlen);
	if( r==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* listen for incoming connections */
	printf("The TCP Server is listening...\n");
	r = listen(sockfd,1);
	if( r==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* accept a new connection */
	client_len = sizeof(clientaddr);
	clientfd = accept(sockfd,&clientaddr,&client_len);
	if( clientfd==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* client is now connected */
	/* no need to receive anything; just send them their address */
	getnameinfo(&clientaddr,client_len,buffer,size,0,0,NI_NUMERICHOST);
		
	/* send the address */
	send(clientfd,buffer,strlen(buffer),0);
	
	/* clean-up and close */
	close(clientfd);
	freeaddrinfo(server);
	close(sockfd);

	return(0);
}
