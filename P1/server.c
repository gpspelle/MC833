#include "net.h"
#include "sql/db.h"

void sigchld_handler(int s) {
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int treat_call(char client_message[BUFF_SIZE], char buffer[100000]) {

    char *command;
  
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
        fprintf(stderr, "Succesfully using "DB"\n");
    }

    printf("Client_message: [%s]\n", client_message);
    command = strtok (client_message, ";");
    if(!strcmp(command, "listar_curso")) {
        /*listar todas as pessoas formadas em um determinado curso;*/
        char *curso = strtok (NULL, ";");
        char query[1000] = "select nome,sobrenome from "tabelaUSER" where formacao='";
        strcat(query, curso);
        strcat(query, "';"); 

        if (mysql_query(con, query)) {
            return_error(con);
        } else {
            printf("Succesfully got nome,sobrenome from "tabelaUSER" for specific degree\n");
        }

        MYSQL_RES *result = mysql_store_result(con);
        
        if (result == NULL) {
            return_error(con);
        }

        int num_fields = mysql_num_fields(result);
        MYSQL_ROW row;
        //char buffer[1000000] = "";
        while((row = mysql_fetch_row(result))) {
            for(int i = 0; i < num_fields; i++) {
                strcat(buffer, row[i]);
                if (i != num_fields - 1) {
                    strcat(buffer, " ");
                }
            }
            strcat(buffer, "\n");
        }

        return 1;
        //int len = strlen(buffer);
        //buffer[len-1] = '\0';
        //len = strlen(buffer);
        //send(new_socket, buffer, len, 0);

    } else if (!strcmp(command, "listar_habilidades")) {
        /*listar as habilidades dos perfis que moram em uma determinada cidade;*/
        char *cidade = strtok (NULL, ";");
        char query[1000] = "select Personid from "tabelaUSER" where residencia='";
        strcat(query, cidade);
        strcat(query, "';"); 

        if (mysql_query(con, query)) {
            return_error(con);
        } else {
            printf("Succesfully get Personid from "tabelaUSER" for specific residence\n");
        }

        MYSQL_RES *result = mysql_store_result(con);
        
        if (result == NULL) {
            return_error(con);
        }

        int num_fields = mysql_num_fields(result);
        MYSQL_ROW row;

        //char buffer[1000000] = "";
        while((row = mysql_fetch_row(result))) {
            for(int i = 0; i < num_fields; i++) {
                
                char query_[1000] = "select hab from "tabelaHAB" where Personid='";
                strcat(query_, row[i]);
                strcat(query_, "';");

                if (mysql_query(con, query_)) {
                    return_error(con);
                }

                MYSQL_RES *result_ = mysql_store_result(con);
        
                if (result_ == NULL) {
                    return_error(con);
                }

                int num_fields_ = mysql_num_fields(result_);
                MYSQL_ROW row_;

                while((row_ = mysql_fetch_row(result_))) {
                    for(int j = 0; j < num_fields_; j++) {
                        strcat(buffer, row_[j]); 
                        strcat(buffer, "\n");
                    }
                }
            }
        }

        return 1;
        //int len = strlen(buffer);
        //buffer[len-1] = '\0';
        //len = strlen(buffer);
        //send(new_socket, buffer, len, 0);

    } else if (!strcmp(command, "add")) {
        /*acrescentar uma nova experiência em um perfil, dado o email;*/
        char *experiencia = strtok (NULL, ";");
        char *email = strtok(NULL, ";") ;

        char query[1000] = "select Personid from "tabelaUSER" where email='";
        strcat(query, email);
        strcat(query, "';"); 

        if (mysql_query(con, query)) {
            return_error(con);
        } else {
            printf("Succesfully get Personid from "tabelaUSER"\n");
        }

        MYSQL_RES *result = mysql_store_result(con);
        
        if (result == NULL) {
            return_error(con);
        }

        int num_fields = mysql_num_fields(result);
        MYSQL_ROW row;
    
        while((row = mysql_fetch_row(result))) {
           for(int i = 0; i < num_fields; i++) {
                char query_[10000] = "insert into "tabelaEXP" (Personid, exp) VALUES(";
                strcat(query_, row[i]);
                strcat(query_, "', '"); 
                strcat(query_, experiencia);
                strcat(query_, "');");
                printf("Query: [%s]\n", query_);
                if(mysql_query(con, query_)) {
                    return_error(con);
                }
            }
        }
       
        strcpy(buffer, "Successfully added new exp to specific an user"); 
        return 0;
        //char message[1000] = "Successfully added new exp to specific an user";
        //send(new_socket, message, strlen(message), 0);

    } else if (!strcmp(command, "email_experiencia")) {
        /*dado o email do perfil, retornar sua experiência;*/
        char *email = strtok (NULL, ";");
        char query[1000] = "select Personid from "tabelaUSER" where email='";
        strcat(query, email);
        strcat(query, "';"); 

        if (mysql_query(con, query)) {
            return_error(con);
        } else {
            printf("Succesfully get Personid from "tabelaUSER"\n");
        }

        MYSQL_RES *result = mysql_store_result(con);
        
        if (result == NULL) {
            return_error(con);
        }

        int num_fields = mysql_num_fields(result);
        MYSQL_ROW row;

        //char buffer[1000000] = "";
        while((row = mysql_fetch_row(result))) {
            for(int i = 0; i < num_fields; i++) {
                
                char query_[1000] = "select exp from "tabelaEXP" where Personid='";
                strcat(query_, row[i]);
                strcat(query_, "';");

                if (mysql_query(con, query_)) {
                    return_error(con);
                }

                MYSQL_RES *result_ = mysql_store_result(con);
        
                if (result_ == NULL) {
                    return_error(con);
                }

                int num_fields_ = mysql_num_fields(result_);
                MYSQL_ROW row_;

                while((row_ = mysql_fetch_row(result_))) {
                    for(int j = 0; j < num_fields_; j++) {
                        strcat(buffer, row_[j]); 
                        strcat(buffer, "\n");
                    }
                }
            }
        }

        return 0;
        //int len = strlen(buffer);
        //buffer[len-1] = '\0';
        //len = strlen(buffer);
        //send(new_socket, buffer, len, 0);

    } else if (!strcmp(command, "listar_tudo")) {
        /*listar todas as informações de todos os perfis;*/

        char query[1000] = "select * from "tabelaUSER";";

        if (mysql_query(con, query)) {
            return_error(con);
        } else {
            printf("Succesfully get * from "tabelaUSER"\n");
        }

        MYSQL_RES *result = mysql_store_result(con);
        
        if (result == NULL) {
            return_error(con);
        }

        int num_fields = mysql_num_fields(result);
        MYSQL_ROW row;
        //char buffer[10000000] = "";

        while((row = mysql_fetch_row(result))) {
            for(int i = 1; i < num_fields; i++) {
                strcat(buffer, row[i]);
                strcat(buffer, "\n");

                char query_[1000] = "select hab from "tabelaHAB" where Personid='";
                strcat(query_, row[0]);
                strcat(query_, "';");

                if (mysql_query(con, query_)) {
                    return_error(con);
                }

                MYSQL_RES *result_ = mysql_store_result(con);
        
                if (result_ == NULL) {
                    return_error(con);
                }

                int num_fields_ = mysql_num_fields(result_);
                MYSQL_ROW row_;

                while((row_ = mysql_fetch_row(result_))) {
                    for(int j = 0; j < num_fields_; j++) {
                        strcat(buffer, row_[j]); 
                        if(j != num_fields_ -1)
                            strcat(buffer, ", ");
                    }
                }

                char query__[1000] = "select exp from "tabelaEXP" where Personid='";
                strcat(query__, row[0]);
                strcat(query__, "';");

                if (mysql_query(con, query__)) {
                    return_error(con);
                }

                MYSQL_RES *result__ = mysql_store_result(con);
        
                if (result__ == NULL) {
                    return_error(con);
                }

                int num_fields__ = mysql_num_fields(result__);
                MYSQL_ROW row__;

                while((row__ = mysql_fetch_row(result__))) {
                    for(int j = 0; j < num_fields__; j++) {
                        strcat(buffer, row__[j]); 
                        strcat(buffer, "\n");
                    }
                }
            }
        }

        return 0;
        //int len = strlen(buffer);
        //buffer[len-1] = '\0';
        //len = strlen(buffer);
        //send(new_socket, buffer, len, 0);

    } else if (!strcmp(command, "email_tudo")) {
        /*dado o email de um perfil, retornar suas informações*/
        char *email = strtok (NULL, ";");

        char query[1000] = "select * from "tabelaUSER" where email='";
        strcat(query, email);
        strcat(query, "';"); 

        if (mysql_query(con, query)) {
            return_error(con);
        } else {
            printf("Succesfully got * from "tabelaUSER" for specific email\n");
        }

        MYSQL_RES *result = mysql_store_result(con);
        
        if (result == NULL) {
            return_error(con);
        }

        int num_fields = mysql_num_fields(result);
        MYSQL_ROW row;
        //char buffer[1000000] = "";
        while((row = mysql_fetch_row(result))) {
            for(int i = 0; i < num_fields; i++) {
                strcat(buffer, row[i]);
                if (i != num_fields - 1) {
                    strcat(buffer, " ");
                }
            }
            strcat(buffer, "\n");
        }
        //int len = strlen(buffer);
        //buffer[len-1] = '\0';
        //len = strlen(buffer);
        //send(new_socket, buffer, len, 0);
    } else {
        mysql_close(con);
        //close(new_socket);
        return 0;
        
    }

    mysql_close(con);
    //close(new_socket);
    return 1;
}

int main() {

    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    socklen_t sin_size;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                        sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
	sin_size = sizeof their_addr;
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if (new_fd == -1) {
		perror("accept");
		continue;
	}

	inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr),
		s, sizeof s);
	printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sockfd);
            char client_message[BUFF_SIZE];
            char buffer[100000];
            read(new_fd, client_message, sizeof(client_message));
            printf("[%s]\n", client_message);
	    int r = treat_call(client_message, buffer);
            write(new_fd, buffer, sizeof(buffer));
            exit(1);
        }
        close(new_fd);
    } 

    return 0;
}
