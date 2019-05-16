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
#include <sys/time.h>
#include <time.h>

#define PORT "7904"  // the port users will be connecting to
#define intPORT 7904  // the port users will be connecting to
#define BACKLOG 20   // how many pending connections queue will hold
#define MAX_DATA_SIZE 10240 // max number of bytes we can get at once 
#define BUFF_SIZE 10240
#define WR_SIZE 1024
#define IMAGE_SIZE 200000

#define MAXBUFLEN 100
#define IMAGE_READ 500

long int treat_call(char client_message[BUFF_SIZE], char buffer[BUFF_SIZE], char image[100000]);
void *get_in_addr(struct sockaddr *sa);
void sigchld_handler(int s);
