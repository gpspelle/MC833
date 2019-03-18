#include <unistd.h> 
#include <fcntl.h> 
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

void create_socket(int client_socket) {

    char client_message[1024];
    char *command;
    int new_socket = client_socket;
  
    recv(new_socket, client_message , 1024 , 0);

    printf("Recebi a mensagem do cliente: [%s]\n", client_message);
    command = strtok (client_message, ";");
    if(!strcmp(command, "listar_curso")) {
        /*(1) listar todas as pessoas formadas em um determinado curso;*/
        char *curso = strtok (NULL, ";");

    } else if (!strcmp(command, "listar_habilidades")) {
        /*listar as habilidades dos perfis que moram em uma determinada cidade;*/
        char *cidade = strtok (NULL, ";");

    } else if (!strcmp(command, "add")) {
        /*acrescentar uma nova experiência em um perfil;*/
        char *experiencia = strtok (NULL, ";");
        char *email = strtok(NULL, ";") ;

    } else if (!strcmp(command, "email_experiencia")) {
        /*dado o email do perfil, retornar sua experiência;*/
        char *email = strtok (NULL, ";");

    } else if (!strcmp(command, "listar_tudo")) {
        /*listar todas as informações de todos os perfis;*/

    } else if (!strcmp(command, "email_tudo")) {
        /*dado o email de um perfil, retornar suas informações*/
        char *email = strtok (NULL, ";");
    }

    send(new_socket,".", 1 ,0);
    close(new_socket);

    }

int main() {

    int server_socket, new_socket;
    struct sockaddr_in server_address;
    struct sockaddr_storage server_storage;
    socklen_t addr_size;

    /* Process ID table */
    pid_t pid[20];

    /* Creating our socket */
    server_socket = socket(PF_INET, SOCK_STREAM, 0);

    /* Set to IPv4 */
    server_address.sin_family = AF_INET;

    /* Using a high value as port number in order to avoid problems already
     * used ports  */
    server_address.sin_port = htons(10000);

    /* Using localhost, a.k.a, 127.0.0.1 */ 
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* TODO: discover what does this do Set all bits of the padding field to 0 */
    memset(server_address.sin_zero, '\0', sizeof server_address.sin_zero);

    //Bind the address struct to the socket 
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    // Waiting a max volume of 20 paralallel process asking for services
    if(listen(server_socket, 20) == 0) {
        printf("Server is working. Ready for receiving messages.\n");
    } else {
        printf("ERROR! LISTEN PROBLEM\n");
    }
    
    int i = 0;
    while(1) {

        /* Creates a new socket for our new running process */
        addr_size = sizeof(server_storage);
        new_socket = accept(server_socket, (struct sockaddr *) &server_storage, &addr_size);

        int pid_child = 0;
        if ((pid_child = fork())==0) {
            /* For the child process  */
            create_socket(new_socket);
        } else {
            pid[i++] = pid_child;
            if(i >= 19) {
                i = 0;
                while(i < 20) {
                    waitpid(pid[i++], NULL, 0);
                }
                i = 0;
            }
        }
    }
    return 0;
}


