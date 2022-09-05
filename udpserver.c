#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

/* function to reverse a string */
char *strrev(char *s)
{
	int len,i;
	static char *reversed;

	/* obtain the string length */
	len = strlen(s);

	/* allocate storage for the reversed string copy */
	reversed = malloc( sizeof(char) * len + 1 );
	if( reversed==NULL )
	{
		fprintf(stderr,"Unable to allocate memory\n");
		exit(1);
	}

	/* copy characters from the end of 's' to the start of 'reversed' */
	i = 0;				/* index for the reversed string */
	while(len)			/* process the length of the string */
	{
		/* subtract 1 from len to account for the string
		   starting at offset 0, not 1 */
		*(reversed+i) = *(s+len-1);	/* copy the character */
		i++;						/* inc. the reversed index */
		len--;						/* dec. the s index */
	}
	/* always cap the string! */
	*(reversed+i) = '\0';

	return(reversed);
}

/* UDP server returns the passed string reversed */
int main()
{
	struct addrinfo hints,*server;
	struct sockaddr client;
	socklen_t client_size;
	int r,sockfd;
	const int size = 1024;
	char input[size],*buffer;

	/* configure the server */
	memset(&hints, 0, sizeof(hints));	/* use memset_s() */
	hints.ai_family = AF_INET;			/* IPv4 connection */
	hints.ai_socktype = SOCK_DGRAM;		/* UDP, datagram */
	hints.ai_flags = AI_PASSIVE;		/* accept any connection */
	r = getaddrinfo(0, "8080", &hints, &server);	/* 0==localhost */
	if( r!=0 )
	{
		perror("failed");
		exit(1);
	}

	/* create the socket */
	sockfd = socket(server->ai_family,server->ai_socktype,server->ai_protocol);
	if( sockfd==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* bind the server to a socket */
	r = bind( sockfd,server->ai_addr,server->ai_addrlen);
	if( r==-1 )
	{
		perror("failed");
		exit(1);
	}
	puts("Server is listening...");

	/* obtain the string from the client */
	client_size = sizeof(struct sockaddr);
	r = recvfrom(
			sockfd,
			input,
			size,
			0,
			&client,
			&client_size
			);
	if( r==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* reverse the string in 'input' into 'buffer' */
	buffer = strrev(input);

	/* return the string to the client */
	r = sendto(
			sockfd,
			buffer,
			strlen(buffer),
			0,
			&client,
			client_size
			);
	if( r==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* clean-up and close */
	freeaddrinfo(server);
	close(sockfd);

	return(0);
}
