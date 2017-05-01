/* A simple server in the internet domain using TCP
The port number is passed as an argument
This version runs forever, forking off a separate
process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "server2.h"
#include  <stdio.h>
#include  <signal.h>
#include  <stdlib.h>

void INThandler(int);
//Thisll help us understand the ctrl-c
void  INThandler(int sig)
{
 	char  c;
	signal(sig, SIG_IGN);
	printf("OUCH, did you hit Ctrl-C?\n"
	"Do you really want to quit? [y/n] ");
	c = getchar();
	if (c == 'y' || c == 'Y')
	{
		printf("echo_s is stopping\n"); //prints when echo_s will be terminated
		exit(0);
	}
	else
	{
		signal(SIGINT, INThandler);
		getchar(); // Get new line character
	}
	FILE *f;
	f = fopen("echo.log", "a");//appends the text at the end of the specified file
	if(f==NULL)//makes sure the file exists
	{
		printf("Error when opening the file.");
		exit(1);
	}
	fprintf(f, "echo_s is stopping\n");//then logs the statement
}	

int main(int argc, char *argv[])
{
	signal(SIGINT, INThandler);
	while(1)
	pause();
	/*
	sockfd and newsockfd store the values returned by the socket system call and the accept system call.
	portno stores the port number on which the server accepts connections.
    clilen stores the size of the address of the client.
	n contains the number of characters read or written.
	*/
	int sockfd, newsockfd, portno, pid, pid2, status;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr; //serv_addr will have the server address. cli_addr will have the client address.
	/*
	The user needs to pass in the port number on which the server will accept connections as an argument. 
	This code displays an error message if the user fails to do this. In other words we are checking to ensure that
	we are entering in valid arguements.
	*/
	if (argc < 3) 
	{
		fprintf(stderr, "ERROR, incorrect format \n");
		exit(1);
	}
	/*
	The socket call takes three arguments. 
	The first is the address domain of the socket.
	The second argument is the type of socket. Whether it is UDP/TCP.
	The third argument is the protocol.
	*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0); //SOCK_STREAM and SOCK_DGRAM. I get to decide which I want at this point?
	if (sockfd < 0) // Basically if we return -1
		error("ERROR opening socket");
	bzero((char *)&serv_addr, sizeof(serv_addr)); //This line initializes serv_addr to zeros.
	/*
	The port number on which the server will listen for connections is passed in as an argument, 
	and this statement uses the atoi() function to convert this from a string of digits to an integer. 
	We need to make this conversion in order to continue. We need the port number.
	*/
	//DH:
	//Given that this log already takes an argument for the port number,
	//all that has to happen is to take it in the format specified
	//by the assignment.
	//So it will ignore -port and read in the number.
	portno = atoi(argv[2]);
	//The first field is short sin_family, which contains a code for the address family. It should always be set to the symbolic constant AF_INET.
	serv_addr.sin_family = AF_INET;
	//The third field of sockaddr_in is a structure of type struct in_addr. This field contains the IP address of the host.
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	//The second field of serv_addr is unsigned short sin_port, which contain the port number. convert this to network byte order using the function htons().
	serv_addr.sin_port = htons(portno);
	//The bind() system call binds a socket to an address, in this case the address of the current host and port number on which the server will run.
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Can fail if the socket is already in use.
		error("ERROR on binding");
	listen(sockfd, 5); //The listen system call allows the process to listen on the socket for connections.
	/*
	The accept() system call causes the process to block until a client connects to the server. 
	Thus, it wakes up the process when a connection from a client has been successfully established.
	*/
	char* ip;
	ip=inet_ntoa(serv_addr.sin_addr);
	clilen = sizeof(cli_addr);
	while (1) //Simple way to make this an infinite loop.
	{
		newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			error("ERROR on accept");
		
		//--------------------------------------------//
		//thinkiii.blogspot.com/2009/12/double-fork-to-avoid-zombie-process.html
		//parent process
		if (pid = fork())
		{
			close(sockfd);
			dostuff(newsockfd, ip);
			//waits for child to finish
			waitpid(pid, &status, NULL);
			exit(0);
		}
		//child process 1, which terminates once process 2 is created
		else if (!pid)
		{
			if (pid2 = fork())
			{
				exit(0);
			}
			else if (!pid2)
			{
				//child process 3
				close(newsockfd);
			}
		}
		else
		{
			//error
		}
		//---------------------------------------------//
	} /* end of while */
	close(sockfd);
	return 0; /* we never get here */
}
