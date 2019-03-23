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
#include "sql/db.h"

void create_socket(int client_socket) {

    char client_message[1024];
    char *command;
    int new_socket = client_socket;
  
    recv(new_socket, client_message , 1024 , 0);

    MYSQL *con = mysql_init(NULL);

    if (con == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, network, userDB, pswd, NULL, 0, NULL, 0) == NULL) {
        return_error(con);
    }

    /* Select specific database for use */
    if (mysql_query(con, "Use "DB";")) {
        return_error(con);
    } else {
        printf("Succesfully using "DB"\n");
    }

    printf("Client_message: [%s]\n", client_message);
    command = strtok (client_message, ";");
    if(!strcmp(command, "listar_curso")) {
        /*listar todas as pessoas formadas em um determinado curso;*/
        char *curso = strtok (NULL, ";");
        printf("Curso: [%s]\n", curso);
        char query[1000] = "select nome,sobrenome from Usuarios where formacao='";
        strcat(query, curso);
        strcat(query, "';"); 

        if (mysql_query(con, query)) {
            return_error(con);
        } else {
            printf("Succesfully did task number 1\n");
        }

        MYSQL_RES *result = mysql_store_result(con);
        
        if (result == NULL) {
            return_error(con);
        }

        int num_fields = mysql_num_fields(result);
        MYSQL_ROW row;
        char buffer[1000000] = "";
        while((row = mysql_fetch_row(result))) {
            for(int i = 0; i < num_fields; i++) {
                strcat(buffer, row[i]);
                if (i != num_fields - 1) {
                    strcat(buffer, " ");
                }
            }
            strcat(buffer, "\n");
        }
        int len = strlen(buffer);
        buffer[len-1] = '\0';
        len = strlen(buffer);
        send(new_socket, buffer, len, 0);

    } else if (!strcmp(command, "listar_habilidades")) {
        /*listar as habilidades dos perfis que moram em uma determinada cidade;*/
        char *cidade = strtok (NULL, ";");
        send(new_socket,"2", 1 ,0);

    } else if (!strcmp(command, "add")) {
        /*acrescentar uma nova experiência em um perfil;*/
        char *experiencia = strtok (NULL, ";");
        char *email = strtok(NULL, ";") ;
        send(new_socket,"3", 1 ,0);

    } else if (!strcmp(command, "email_experiencia")) {
        /*dado o email do perfil, retornar sua experiência;*/
        char *email = strtok (NULL, ";");
        send(new_socket,"4", 1 ,0);

    } else if (!strcmp(command, "listar_tudo")) {
        /*listar todas as informações de todos os perfis;*/
        send(new_socket,"5", 1 ,0);

    } else if (!strcmp(command, "email_tudo")) {
        /*dado o email de um perfil, retornar suas informações*/
        char *email = strtok (NULL, ";");
        send(new_socket,"6", 1 ,0);
    }

    mysql_close(con);
    close(new_socket);

}

int main() {

    int server_socket, new_socket;
    struct sockaddr_in server_address;
    struct sockaddr_storage server_storage;
    socklen_t addr_size;

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
    
    for(;;) {

        /* Creates a new socket for our new running process */
        addr_size = sizeof(server_storage);
        new_socket = accept(server_socket, (struct sockaddr *) &server_storage, &addr_size);

        int pid_child = 0;
        if ((pid_child = fork())==0) {
            /* For the child process  */
            create_socket(new_socket);
            break;
        } else {
            close(new_socket);
            continue;
        }
    }
    return 0;
}


