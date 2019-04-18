#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#define localhost "127.0.0.1"

void clientThread() {

    char message[1000];
    char buffer[1024];
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
    // Create the socket. 
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    //Configure settings of the server address
    // Address family is Internet 
    serverAddr.sin_family = AF_INET;

    //Set port number, using htons function 
    serverAddr.sin_port = htons(7799);

    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr(localhost);

    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Connect the socket to the server using the address
    addr_size = sizeof serverAddr;

    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

    strcpy(message,"Hello");

    if(send(clientSocket , message , strlen(message) , 0) < 0) {
        printf("Send failed\n");
    }

    //Read the message from the server into the buffer
    if(recv(clientSocket, buffer, 1024, 0) < 0) {
       printf("Receive failed\n");
    }

    //Print the received message
    printf("Data received: %s\n",buffer);
    close(clientSocket);

}

int main() {

    clientThread();
    return 0;

}
