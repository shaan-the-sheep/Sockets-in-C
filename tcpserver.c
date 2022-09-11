#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#define PORT ("8080")
#define MAX_TXT_SZ 50

struct socket_data {  // Used to be thead_data
	int clientfd;
	struct sockaddr clientaddr;
	socklen_t client_len;
	int size;
	char input[MAX_TXT_SZ];
	int next_free_thr_index;
};


struct thread_pool{
	pthread_t thread_id;
	struct socket_data sock_data;
};

#define THREAD_POOL_SZ 10
struct thread_pool pool[THREAD_POOL_SZ];

// Creates a new thread with the passed in fn from the next available in the
// thread pool.
// Returns the index of the new thread pool object, else -1
int assign_pool_thread(int next_free_thr_index,
					   void *(*thr_fn)(void* arg),
					   struct socket_data sock_data)
{
	//printf("In assign_pool_thread. next_free_thr_index: %d\n", next_free_thr_index);
	
	if(next_free_thr_index >= THREAD_POOL_SZ)
		return -1;
	
	// Update pool thread's socket data
	pool[next_free_thr_index].sock_data = sock_data;
	// Need to zero out the txt array otherwise it contains garbage.
 	memset(pool[next_free_thr_index].sock_data.input, 0, MAX_TXT_SZ);

	int ret = pthread_create(&(pool[next_free_thr_index].thread_id),
							 NULL, thr_fn, &(pool[next_free_thr_index].sock_data));
	if (ret != 0)
		return -1;

	printf("New client: %d\n", pool[next_free_thr_index].sock_data.clientfd);
	return next_free_thr_index;
}

void* threadfunction(void *arg){
	int retval;
	struct socket_data *my_data;
	my_data = (struct socket_data *) arg;
	for(;;){ 
		printf("Waiting for client data\n");
		//recvfrom is a blocking function; thread stops here until recvfrom returns
		int r = recvfrom(my_data->clientfd,
						my_data->input,
						my_data->size,
						0,
						&(my_data->clientaddr),
						&(my_data->client_len));		
		if(r == -1)
		{
			printf("Here");
			perror("failed");
			close(my_data->clientfd);
			pthread_exit(&retval);
		}
		if(strncmp(my_data->input, "exit", 4) == 0){
			printf("Exiting\n");
			close(my_data->clientfd);
			pthread_exit(&retval);
			printf("omg\n");
		}
		printf("clientfd %d: %s\n", my_data->clientfd, my_data->input); 
		// Clear out the text otherwise it hangs around when we print the next
		// message we receive
 		memset(my_data->input, 0, MAX_TXT_SZ);
	}
	return(0);
	
}

int main()
{
	struct addrinfo hints, *server;
	int r, sockfd, clientfd;
	struct sockaddr clientaddr;
	socklen_t client_len;

	// Thread pool of 10 client threads
	static int next_free_thr_indx = 0;

	/* configure the server to use the localhost, port PORT, TCP */
	memset( &hints, 0, sizeof(struct addrinfo) );	/* use memset_s() */
	hints.ai_family = AF_INET;			/* IPv4 connection */
	hints.ai_socktype = SOCK_STREAM;	/* TCP, streaming */
	hints.ai_flags = AI_PASSIVE;		/* listen to any interface */
	r = getaddrinfo( 0, PORT, &hints, &server );
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

	/* accept a new connection */
	client_len = sizeof(clientaddr);
	for (;;) {
		clientfd = accept(sockfd, &clientaddr, &client_len);
		if( clientfd==-1 )
		{
			perror("failed");
			exit(1);
		}

		struct socket_data sock_data;
		sock_data.clientfd = clientfd;
		sock_data.clientaddr = clientaddr;
		sock_data.client_len = client_len;
		sock_data.size = MAX_TXT_SZ;
		int ret = assign_pool_thread(next_free_thr_indx, threadfunction, sock_data);
		if (ret >= 0) {
			next_free_thr_indx += 1;
			//printf("Incremented next_free_thr_indx to %d\n", next_free_thr_indx);
		}
		else {
			/* Here we should really take some action to abort the execution
			but first ensure all client threads get destroyed and join them
			here */
		}
	}

	//pthread_join(thread1, NULL);
	return 0;
}



