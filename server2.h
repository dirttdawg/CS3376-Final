#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

void dostuff(int , char*); //Function prototype
char* get_addr(struct sockaddr_in, sa);
void error(const char *msg);
#endif 
