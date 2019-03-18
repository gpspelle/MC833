#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <fcntl.h> 

void client_call(char *input) {

    char message[1024];
    char buffer[1000];
    int client_socket;
    struct sockaddr_in server_address;
    socklen_t addr_size;
    
    // Create the socket. 
    client_socket = socket(PF_INET, SOCK_STREAM, 0);

    //Configure settings of the server address
    // Address family is Internet 
    server_address.sin_family = AF_INET;

    //Set port number, using htons function 
    server_address.sin_port = htons(10000);

    //Set IP address to localhost
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    memset(server_address.sin_zero, '\0', sizeof(server_address.sin_zero));

    //Connect the socket to the server using the address
    addr_size = sizeof(server_address);

    connect(client_socket, (struct sockaddr *) &server_address, addr_size);

    strcpy(message, input);

    printf("[%s]\n", message);

    if(send(client_socket , message , 1024 , 0) < 0) {
        /* Something went wrong while sending our message */
        printf("ERROR: SEND PROBLEM\n");
    }

    //Read the message from the server into the buffer
    if(recv(client_socket, buffer, 1024, 0) < 0) {
       /* Something went wrong while receiving a response message */
       printf("ERROR: RECEIVE PROBLEM\n");
    }

    //Print the received message
    //printf("[%s]\n",buffer);
    close(client_socket);

}

int main() {

    for(int i = 0; i < 9; i++) {
        char input[1024];
        scanf(" %s", input);
        client_call(input);
    }

    return 0;

}
