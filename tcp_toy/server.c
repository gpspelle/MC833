#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <sys/types.h>

#define BUFF_SIZE 1024

void create_socket(int  clientSocket) {

    char client_message[BUFF_SIZE];
    char buffer[BUFF_SIZE];
    int newSocket = clientSocket;
  
    recv(newSocket , client_message , BUFF_SIZE , 0);

    // Send message to the client socket 
    char *message = malloc(sizeof(client_message)+20);

    strcpy(message,"Hello Client : ");
    strcat(message,client_message);
    strcat(message,"\n");
    strcpy(buffer,message);
    free(message);
    sleep(1);

    send(newSocket,buffer,BUFF_SIZE,0);
    printf("Exit socketThread \n");
    close(newSocket);

}

int main() {

    int serverSocket, new_socket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    //Create the socket. 
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);

    // Configure settings of the server address struct
    // Address family = Internet 
    serverAddr.sin_family = AF_INET;

    //Set port number, using htons function to use proper byte order 
    serverAddr.sin_port = htons(7799);

    //Set IP address to localhost 
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Set all bits of the padding field to 0 
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Bind the address struct to the socket 
    bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    //Listen on the socket, with 40 max connection requests queued 
    if(listen(serverSocket,1)==0) {
        printf("Listening\n");
    } else {
        printf("Error\n");
    }
    int i = 0;
    
    while(1) {

        i+=1;
        /*---- Accept call creates a new socket for the incoming connection ----*/

        addr_size = sizeof serverStorage;
        new_socket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

        int pid_child = 0;
        if ((pid_child = fork())==0) {
            /* For the child process  */
            printf("running server: %d\n", i);
            create_socket(new_socket);
            exit(1);
        } else {
            close(new_socket);
            continue;
        }
    }
    return 0;
}
