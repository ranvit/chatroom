/**
 * Chatroom Lab
 * CS 241 - Fall 2016
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define MAX_CLIENTS 8

void *process_client(void *p);
void write_to_clients(const char *message, size_t size);

static volatile int sessionEnd;
static volatile int serverSocket;

static volatile int clientsCount;
static volatile int clients[MAX_CLIENTS];

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct addrinfo hints, *result;
int exit_flag = 0;
//pthread_attr_t attr;

void close_server() {
  // Use a signal handler to call this function and close the server
  // How to you let clients stop waiting for the server?

//	printf("Handling SIGINT\n");

	sessionEnd = 1;

	exit_flag = 1;
	freeaddrinfo(result);
	shutdown(serverSocket, 2);
	pthread_mutex_destroy(&mutex);
//	pthread_attr_destroy(&attr);
}

void run_server(char *port) {

	signal(SIGINT, close_server);

  /*QUESTION 1*/
  /*QUESTION 2*/
  /*QUESTION 3*/

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  /*QUESTION 8*/

	int optval = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

  /*QUESTION 4*/
  /*QUESTION 5*/
  /*QUESTION 6*/

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int err_check = getaddrinfo(NULL, port, &hints, &result);
	if(err_check != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err_check));
		exit(1);
	}

  /*QUESTION 9*/

	if (bind(serverSocket, result->ai_addr, result->ai_addrlen) != 0) {
		perror(NULL);
		exit(1);
	}

  /*QUESTION 10*/

	if (listen(serverSocket, MAX_CLIENTS) != 0) {
		perror(NULL);
		exit(1);
	}


  for (int i = 0; i < MAX_CLIENTS; i++)
    clients[i] = -1;

  while (sessionEnd == 0) {
    if (clientsCount < MAX_CLIENTS) {
      // Can now start accepting and processing client connections
      printf("Waiting for connection...\n");

      /*QUESTION 11*/
      // I wonder what are the structs for :)
      struct sockaddr clientAddr;
      socklen_t clientAddrlen = sizeof(struct sockaddr);
      memset(&clientAddr, 0, sizeof(struct sockaddr));

      int clientFD = -1;

	/*while(clientFD == -1 && sessionEnd == 0)*/ clientFD = accept(serverSocket, &clientAddr, &clientAddrlen);
	if(sessionEnd) break;

//	printf("ClientFD is %d\nserverSocket is %d\n", clientFD, serverSocket);

      // Looking for a lowest available clientId
      // Writing client file descriptor to clients array
      intptr_t clientId = -1;
      pthread_mutex_lock(&mutex);
      for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] == -1) {
          clients[i] = clientFD;

          // Printing out IP address of newly joined clients
          char clientIp[INET_ADDRSTRLEN];
          if (inet_ntop(AF_INET, &clientAddr, clientIp, clientAddrlen) != 0) {
            printf("Client %d joined on %s\n", i, clientIp);
          }
          clientId = i;
          break;
        }
      clientsCount++;
      printf("Currently servering %d clients\n", clientsCount);
      pthread_mutex_unlock(&mutex);

      // Launching a new thread to serve the client
      pthread_t thread;
      int retval = pthread_create(&thread, NULL, process_client, (void *)clientId);
      if (retval != 0) {
        perror("pthread_create():");
        exit(1);
      }
    }
  }
  // Be sure to free the address info here

//	close_server();
	if(sessionEnd == 0){
		freeaddrinfo(result);
		shutdown(serverSocket, 2);
		pthread_mutex_destroy(&mutex);
	}
}

// Broadcasting message to every active client
// DO NOT MODIFY THE FUNCTION BELOW
void write_to_clients(const char *message, size_t size) {
  pthread_mutex_lock(&mutex);
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i] != -1) {
      ssize_t retval = write_message_size(size, clients[i]);
      if (retval > 0)
        retval = write_all_to_socket(clients[i], message, size);
      if (retval == -1)
        perror("write():");
      printf("sent %zu bytes\n", retval);
    }
  }
  pthread_mutex_unlock(&mutex);
}

// Read messages from client and sends it out to everyone
// DO NOT MODIFY THE FUNCTION BELOW
void *process_client(void *p) {
  pthread_detach(pthread_self());
  intptr_t clientId = (intptr_t)p;
  ssize_t retval = 1;
  char *buffer = NULL;

  while (retval > 0 && sessionEnd == 0) {
    retval = get_message_size(clients[clientId]);
    if (retval > 0) {
      buffer = calloc(1, retval);
      retval = read_all_from_socket(clients[clientId], buffer, retval);
    }
    if (retval > 0)
      write_to_clients(buffer, retval);

    free(buffer);
    buffer = NULL;
  }

  printf("User %d left\n", (int)clientId);
  close(clients[clientId]);

  pthread_mutex_lock(&mutex);
  clients[clientId] = -1;
  clientsCount--;
  pthread_mutex_unlock(&mutex);

  return NULL;
}

int main(int argc, char **argv) {

  if (argc != 2) {
    fprintf(stderr, "./server <port>\n");
    return -1;
  }

  signal(SIGINT, close_server);
  run_server(argv[1]);

  return 0;
}
