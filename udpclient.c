#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

int main()
{
	struct addrinfo hints,*host;
	int r,sockfd,x;
	const int size = 1024;
	char input[size],buffer[size];

	/* configure the remote address */
	memset( &hints, 0, sizeof(hints) );	/* use memset_s() */
	hints.ai_family = AF_INET;			/* IPv4 */
	hints.ai_socktype = SOCK_DGRAM;		/* UDP connection */
	r = getaddrinfo( 0, "8080", &hints, &host );
	if( r!=0 )
	{
		perror("failed");
		exit(1);
	}

	/* create the socket */
	sockfd = socket(host->ai_family,host->ai_socktype,host->ai_protocol);
	if( sockfd==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* prompt for input */
	printf("Type a string: ");
	fgets(input,size,stdin);
	/* remove the newline */
	for( x=0; x<size; x++ )
	{
		if( input[x]=='\n' )		/* replace the newline */
		{
			input[x] = '\0';		/* with the null char */
			break;
		}
	}

	/* send the string to the server */
	r = sendto(
			sockfd,
			input,
			strlen(input),
			0,
			host->ai_addr,
			host->ai_addrlen
			);

	r = recvfrom(
			sockfd,
			buffer,
			size,
			0,
			host->ai_addr,
			&host->ai_addrlen
			);
	buffer[r] = '\0';
	printf("%s\n",buffer);

	/* all done, clean-up */
	freeaddrinfo(host);
	close(sockfd);

	return(0);
}
