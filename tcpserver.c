#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

struct thread_data{
	int clientfd;
	//struct addrinfo *server;
	struct sockaddr clientaddr;
	socklen_t client_len;
	int size;
	char buffer[1024];
	char input[1024];
	char shit[5];
};

void* threadfunction(void *arg){
	int retval;
	struct thread_data *my_data;
	my_data = (struct thread_data *) arg;
	printf("%s\n", my_data->shit);  // debug
	for(;;){
		printf("Waiting for client data\n");
		int r = recvfrom(
			my_data->clientfd,
			my_data->input,
			my_data->size,
			0,
			&(my_data->clientaddr),
			&(my_data->client_len)
		);		
		if(r == -1)
		{
			perror("failed");
			pthread_exit(&retval);
		}
		if(strncmp(my_data->input, "exit", 4) == 0){
			printf("Exiting\n");
			pthread_exit(&retval);
			printf("omg\n");
		}
		printf("%s\n", my_data->input); 
	}
	/* getnameinfo(&(my_data->clientaddr),  // this sets up buffer
				my_data->client_len,
				my_data->buffer,
				my_data->size, 0, 0, NI_NUMERICHOST);

	send(my_data->socfd, my_data->buffer, strlen(my_data->buffer),0);  send the address 
	close(my_data->socfd);  clean-up and close */
	//close(my_data->socfd);
	return(0);
	
}

int main()
{
	struct addrinfo hints, *server;
	int r, sockfd, clientfd;
	struct sockaddr clientaddr;
	socklen_t client_len;
	const int size = 1024;
	//char buffer[size];
	//char input[size];

	pthread_t thread1;
	pthread_t thread2;
	struct thread_data s1;
	strncpy(s1.shit, "aaa", 3);
	struct thread_data s2;
	strncpy(s2.shit, "bbb", 3);

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
	if(r == -1)
	{
		perror("failed");
		exit(1);
	}

	/* listen for incoming connections */
	printf("The TCP Server is listening...\n");
	r = listen(sockfd,1);
	if(r ==-1)
	{
		perror("failed");
		exit(1);
	}

	// hacky thread management
	int thr_num = 0;

	/* accept a new connection */
	client_len = sizeof(clientaddr);
	for (;;) {
		clientfd = accept(sockfd, &clientaddr, &client_len);
		if( clientfd==-1 )
		{
			perror("failed");
			exit(1);
		}
		if (thr_num == 0) {
			s1.clientfd = clientfd;
			//s1.server = server;
			s1.clientaddr = clientaddr;
			s1.client_len = client_len;
			s1.size = 1024;
			thr_num++;
			pthread_create(&thread1, NULL, threadfunction, &s1);
		}
		if (thr_num == 1) {
			s2.clientfd = clientfd;
			//s2.server = server;
			s2.clientaddr = clientaddr;
			s2.client_len = client_len;
			s2.size = 1024;
			pthread_create(&thread2, NULL, threadfunction, &s2);
		}
		//char input[1024]; 
	}

	pthread_join(thread1, NULL);
	return 0;
}


	/* client is now connected */
	/* no need to receive anything; just send them their address */


