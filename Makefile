#Makefile for client.c, client_functions.c, server.c, server_functions.c

CC = gcc -g

all: echo_c echo_s log_s 

echo_c:		echo_c.c 
	$(CC) -c echo_c.c
	$(CC) -o echo_c echo_c.o 

echo_s:		echo_s.c server_functions.c server2.h
	$(CC) -c echo_s.c
	$(CC) -c server_functions.c
	$(CC) -o echo_s  echo_s.o server_functions.o

log_s:		log_s.c server_functions.c server2.h
	$(CC) -c log_s.c
	$(CC) -c server_functions.c
	$(CC) -o log_s log_s.o server_functions.o

clean:
	rm echo_c log_s echo_s 
