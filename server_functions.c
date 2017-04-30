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


/******** DOSTUFF() *********************
There is a separate instance of this function
for each connection.  It handles all communication
once a connnection has been established.
*****************************************/
void dostuff(int sock, char* ip)
{
	int n;
	char buffer[256]; //The server reads characters from the socket connection into this buffer.
	/*
	we would only get to this point after a client has successfully connected to our server. 
	This code initializes the buffer using the bzero() function, and then reads from the socket.
	We will wait to read until something has been written for obvious reasons.
	*/
	bzero(buffer, 256);
	n = read(sock, buffer, 255);
	if (n < 0) 
		error("ERROR reading from socket");
	printf("Here is the message: %s\n", buffer);
	n = write(sock, "I got your message", 18);
	FILE *f;
	f = fopen("echo.log", "a+"); // a+ (create + append) option will allow appending which is useful in a log file
	if (f == NULL) 
	{ 
	/* Something is wrong   */
	}
    time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
    //printf("%s",asctime( localtime(&ltime) ) );
	fprintf(f,"%d", ltime);
	fprintf(f,"   ");
	fprintf(f,"%s", buffer);
	fprintf(f, " was recieved from ");
	fprintf(f,"%s\n", ip);
	fclose(f);
	if (n < 0) 
		error("ERROR writing to socket");
}
/*
This function is called when a system call fails. 
It displays a message about the error on stderr and then aborts the program.
If everything goes wrong this is our solution.
*
*/

/* function to get and send IP address
char* get_addr(struct sockaddr_in sin_addr, struct sigaction *sa, struct in_addr curr) 
{
	char ipAddr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(sa), ipAddr, INET_ADDRSTRLEN);
	return inet_ntoa(curr);
}
*/
void error(const char *msg)
{
	perror(msg);
	exit(1);
}
