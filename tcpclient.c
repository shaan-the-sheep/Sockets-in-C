#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>

int main()
{
	const char *server = "127.0.0.1";	/* change to server's address */
	int r,sockfd;
	struct addrinfo hints,*host;
	const int buffer_size = 1024;
	char buffer[buffer_size];

	memset(&hints, 0, sizeof(struct addrinfo));		/* memset_s() */
	hints.ai_family = AF_INET;			/* IPv4 connection */
	hints.ai_socktype = SOCK_STREAM;	/* TCP, streaming */
	r = getaddrinfo( server, "8080", &hints, &host);
	if( r!=0 )
	{
		perror("TCP client");
		exit(1);
	}

	/* create a socket */
	sockfd = socket(host->ai_family,host->ai_socktype,host->ai_protocol);
	if( sockfd==-1 )
	{
		perror("TCP client");
		exit(1);
	}

	/* connect and get the info */
	r = connect(sockfd,host->ai_addr,host->ai_addrlen);
	if( r == -1)
	{
		perror("TCP client");
		exit(1);
	}

	/* upon connection, the host should send our IP address */
	r = recv(sockfd, buffer, buffer_size, 0);
	buffer[r] = '\0';
	printf("%s\n",buffer);

	/* close-up */
	freeaddrinfo(host);
	close(sockfd);

	return(0);
}
