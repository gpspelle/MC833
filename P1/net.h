#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <strings.h>

#define PORT "7904"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
#define MAX_DATA_SIZE 1024 // max number of bytes we can get at once 
#define BUFF_SIZE 1024

int treat_call(char client_message[BUFF_SIZE], char buffer[100000]);
void *get_in_addr(struct sockaddr *sa);
void sigchld_handler(int s);
