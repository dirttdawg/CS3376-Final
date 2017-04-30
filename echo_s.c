#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
 
#define MAX_ENQUEUED 3
#define BUF_LEN 256
#define PORT_STR "9999"
 
/* ------------------------------------------------------------ */
/* How to clean up after dead child processes                   */
/* ------------------------------------------------------------ */
 
void wait_for_zombie(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0) ;
}
 
/* ------------------------------------------------------------ */
/* Core of implementation of a child process                    */
/* ------------------------------------------------------------ */
 
void echo_lines(int csock)
{
    char buf[BUF_LEN];
    int r;
 
    while( (r = read(csock, buf, BUF_LEN)) > 0 ) 
	{
        (void)write(csock, buf, r);
    }
    exit(EXIT_SUCCESS);
}
 
/* ------------------------------------------------------------ */
/* Core of implementation of the parent process                 */
/* ------------------------------------------------------------ */
 
void take_connections_forever(int ssock)
{
    for(;;) 
	{
        struct sockaddr addr;
        socklen_t addr_size = sizeof(addr);
        int csock;
 
        /* Block until we take one connection to the server socket */
        csock = accept(ssock, &addr, &addr_size);
 
        /* If it was a successful connection, spawn a worker process to service it */
        if ( csock == -1 ) 
		{
            perror("accept");
        } else if ( fork() == 0 ) 
		{
            close(ssock);
            echo_lines(csock);
        } else 
		{
            close(csock);
        }
    }
}
 
/* ------------------------------------------------------------ */
/* The server process's one-off setup code                      */
/* ------------------------------------------------------------ */
 
int main()
{
	struct in_addr curr;
	curr.s_addr = 10.24;
	int choice=0;
	printf("Enter 1 for type TCP and 2 for type GDP");
	scanf("%d", &choice);
    struct addrinfo hints, hints2, *res, *res2;
    struct sigaction sa, sa2;
    int sock;
	int sock2;
	if(choice==1)
	{
	/* Look up the address to bind to */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ( getaddrinfo(NULL, PORT_STR, &hints, &res) != 0 ) 
	{
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }
    /* Make a socket */
    if ( (sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1 ) 
	{
        perror("socket");
        exit(EXIT_FAILURE);
    }
    /* Arrange to clean up child processes (the workers) */
    sa.sa_handler = wait_for_zombie;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if ( sigaction(SIGCHLD, &sa, NULL) == -1 ) 
	{
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    /* Associate the socket with its address */
    if ( bind(sock, res->ai_addr, res->ai_addrlen) != 0 ) 
	{
        perror("bind");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(res);
    /* State that we've opened a server socket and are listening for connections */
    if ( listen(sock, MAX_ENQUEUED) != 0 ) 
	{
        perror("listen");
        exit(EXIT_FAILURE);
    }
 
    /* Serve the listening socket until killed */
    take_connections_forever(sock);
    return EXIT_SUCCESS;
	}
	if(choice==2)
	{
	/* Look up the address to bind to */
    memset(&hints2, 0, sizeof(struct addrinfo));
	hints2.ai_family= AF_UNSPEC;
	hints2.ai_socktype=SOCK_DGRAM;
	hints2.ai_flags = AI_PASSIVE;
	if ( getaddrinfo(NULL, PORT_STR, &hints2, &res2) != 0 ) 
	{
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }
    /* Make a socket */
	if( (sock2= socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
    /* Arrange to clean up child processes (the workers) */
	sa2.sa_handler= wait_for_zombie;
	sigemptyset(&sa2.sa_mask);
	sa2.sa_flags = SA_RESTART;
	if ( sigaction(SIGCHLD, &sa2, NULL) == -1 ) 
	{
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    /* Associate the socket with its address */
	if ( bind(sock2, res2->ai_addr, res2->ai_addrlen) != 0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(res2);
    /* State that we've opened a server socket and are listening for connections */
    if ( listen(sock2, MAX_ENQUEUED) != 0 ) 
	{
        perror("listen");
        exit(EXIT_FAILURE);
    }

    
   // was supposed to send info to log server
   //  sendto(sock, PORT_STR, strlen(PORT_STR), 0, get_addr(res2, sa, curr), ntohs(res2));
 
    /* Serve the listening socket until killed */
    take_connections_forever(sock2);
    return EXIT_SUCCESS;	
	}
}
