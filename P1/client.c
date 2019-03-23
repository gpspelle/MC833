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
    char buffer[1024];
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
    printf("[%s]\n",buffer);
    close(client_socket);

}

int main() {

    for(;;) {
        printf("Seja bem vindo ao sistema do cliente. Aqui voce podera realizar as seguintes operacoes: \n");
        printf("1. Listar todas as pessoas formadas em um determinado curso;\n");
        printf("2. Listar as habilidades dos perfis que moram em uma determinada cidade;\n");
        printf("3. Acrescentar uma nova experiencia em um perfil;\n");
        printf("4. Dado o email do perfil, retornar suas experiencias;\n");
        printf("5. Listar todas as informacoes de todos os perfis;\n");
        printf("6. Dado o email de um perfil, retornar suas informacoes;\n");

        int input;
        scanf(" %d", &input);

        char curso[100];
        char cidade[100];
        char experiencia[100];
        char email[100];
        char message[300] = "";

        switch(input) {
            case 1: printf("Qual curso: ");
                    scanf(" %[^\n]s", curso); 
                    strcpy(message, "listar_curso;");
                    strcat(message, curso);
                    client_call(message); 
                    break;
            case 2: printf("Qual cidade: ");
                    scanf(" %[^\n]s", cidade); 
                    strcpy(message, "listar_habilidades;");
                    strcat(message, cidade);
                    client_call(message); 
                    break;
            case 3: printf("Qual experiencia: ");
                    scanf(" %[^\n]s", experiencia);
                    printf("Qual email: ");
                    scanf(" %s", email);
                    strcpy(message, "add;");
                    strcat(message, experiencia);
                    strcat(message, ";");
                    strcat(message, email);
                    client_call(message);
                    break;
            case 4: printf("Qual email: ");
                    scanf(" %s", email);
                    strcpy(message, "email_experiencia;");
                    strcat(message, email);
                    client_call(message); 
                    break;
            case 5: strcpy(message, "listar_tudo"); 
                    client_call(message);
                    break;
            case 6: printf("Qual email: ");
                    scanf(" %s", email);
                    strcpy(message, "email_tudo");
                    strcat(message, email);
                    client_call(message);
                    break;
        }
    }

    return 0;

}
