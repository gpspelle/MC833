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

void get_time(FILE *fp) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    fprintf(fp, "%s.%09ld;", buff+9, ts.tv_nsec);
}

long int req6(char *email, char buffer[BUFF_SIZE], char image[IMAGE_SIZE], MYSQL *con) {

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
    long int flen = 0;
    MYSQL_ROW row;
    while((row = mysql_fetch_row(result))) {
        for(int i = 1; i < num_fields; i++) {
            switch(i) {
                case 1: strcat(buffer, "Email: ") ; break;
                case 2: strcat(buffer, "Nome: "); break;
                case 3: strcat(buffer, "Sobrenome: "); break;
                case 4: strcat(buffer, "Foto: "); 
                        FILE *fp = fopen(row[i], "rb");
                        if (fp == NULL) {
                            fprintf(stderr, "cannot open image file %s\n", row[i]);    
                            exit(1);
                        }

                        fseek(fp, 0, SEEK_END);
                          
                        if (ferror(fp)) {
                                    
                            fprintf(stderr, "fseek() failed\n");
                            int r = fclose(fp);

                            if (r == EOF) {
                                fprintf(stderr, "cannot close file handler\n");          
                            }    
                                                      
                            exit(1);
                        }  
                            
                        flen = ftell(fp);
                              
                        if (flen == -1) {
                                        
                            perror("error occurred");
                            int r = fclose(fp);

                            if (r == EOF) {
                                fprintf(stderr, "cannot close file handler\n");
                            }
                            exit(1);      
                        }
                                
                        fseek(fp, 0, SEEK_SET);
                                  
                        if (ferror(fp)) {
                            fprintf(stderr, "fseek() failed\n");
                            int r = fclose(fp);
                            if (r == EOF) {
                                fprintf(stderr, "cannot close file handler\n");
                            }                                  
                            exit(1);
                        }
                        fread(image, 1, flen, fp);
                        image[flen+1] = '\0';
                        break;
                case 5: strcat(buffer, "Residencia "); break;
                case 6: strcat(buffer, "Formacao: "); break;
            }
            strcat(buffer, row[i]);

            if(i != 2) {
                strcat(buffer, "\n");
            } else {
                strcat(buffer, " ");
            }
        }

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

        int entrei = 0;
        strcat(buffer, "Habilidades: ");
        while((row_ = mysql_fetch_row(result_))) {
            if(entrei) {
                strcat(buffer, ", ");
            }
            entrei = 1;
            for(int j = 0; j < num_fields_; j++) {
                strcat(buffer, row_[j]); 
            }
        }

        strcat(buffer, "\n");
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
        
        int counter = 1;
        char a[3];
        strcat(buffer, "Experiencia: ");
        while((row__ = mysql_fetch_row(result__))) {
            for(int j = 0; j < num_fields__; j++) {
                if(counter != 1) {
                    strcat(buffer, "\t     ");
                }
                strcat(buffer, "(");
                sprintf(a, "%d", counter);
                strcat(buffer, a);
                strcat(buffer, ") ");
                strcat(buffer, row__[j]); 
                strcat(buffer, "\n");
            }
            counter += 1;
        }
        
        return flen;
    }

    return -1;

}

long int treat_call(char client_message[BUFF_SIZE], char buffer[BUFF_SIZE], char image[IMAGE_SIZE]) {

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
        while((row = mysql_fetch_row(result))) {
            for(int i = 0; i < num_fields; i++) {
                if(i == 0) {
                    strcat(buffer, "Nome: ");
                } else {
                    strcat(buffer, " Sobrenome: ");
                }

                strcat(buffer, row[i]);
            }
            strcat(buffer, "\n");
        }

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

                int entrei = 0;
                strcat(buffer, "Habilidades: ");
                while((row_ = mysql_fetch_row(result_))) {
                    if(entrei) {
                        strcat(buffer, ", ");
                    }
                    entrei = 1;
                    for(int j = 0; j < num_fields_; j++) {
                        strcat(buffer, row_[j]); 
                    }
                }
                strcat(buffer, "\n");
            }
        }

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
                strcat(query_, ", '"); 
                strcat(query_, experiencia);
                strcat(query_, "');");
                printf("Query: [%s]\n", query_);
                if(mysql_query(con, query_)) {
                    return_error(con);
                }
            }
        }
       
        strcpy(buffer, "Successfully added new exp to specific an user"); 

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

                int counter = 1;
                char a[3];
                strcat(buffer, "Experiencia: ");
                while((row_ = mysql_fetch_row(result_))) {
                    for(int j = 0; j < num_fields_; j++) {
                        if(counter != 1) {
                            strcat(buffer, "\t      ");
                        }
                        strcat(buffer, "(");
                        sprintf(a, "%d", counter);
                        strcat(buffer, a);
                        strcat(buffer, ") ");
                        strcat(buffer, row_[j]); 
                        strcat(buffer, "\n");
                    }
                    counter += 1;
                }
            }
        }

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
        long int flen = 0;
        MYSQL_ROW row;

        while((row = mysql_fetch_row(result))) {
            for(int i = 1; i < num_fields; i++) {
                switch(i) {
                    case 1: strcat(buffer, "Email: ") ; break;
                    case 2: strcat(buffer, "Nome: "); break;
                    case 3: strcat(buffer, "Sobrenome: "); break;
                    case 4: strcat(buffer, "Foto: "); 
                            FILE *fp = fopen(row[i], "rb");
                            if (fp == NULL) {
                                fprintf(stderr, "cannot open image file %s\n", row[i]);    
                                exit(1);
                            }

                            fseek(fp, 0, SEEK_END);
                              
                            if (ferror(fp)) {
                                        
                                fprintf(stderr, "fseek() failed\n");
                                int r = fclose(fp);

                                if (r == EOF) {
                                    fprintf(stderr, "cannot close file handler\n");          
                                }    
                                                          
                                exit(1);
                            }  
                                
                            flen = ftell(fp);
                                  
                            if (flen == -1) {
                                            
                                perror("error occurred");
                                int r = fclose(fp);

                                if (r == EOF) {
                                    fprintf(stderr, "cannot close file handler\n");
                                }
                                exit(1);      
                            }
                                    
                            fseek(fp, 0, SEEK_SET);
                                      
                            if (ferror(fp)) {
                                fprintf(stderr, "fseek() failed\n");
                                int r = fclose(fp);
                                if (r == EOF) {
                                    fprintf(stderr, "cannot close file handler\n");
                                }                                  
                                exit(1);
                            }
                            fread(image, 1, flen, fp);
                            image[flen+1] = '\0';
                            break;
                    case 5: strcat(buffer, "Residencia "); break;
                    case 6: strcat(buffer, "Formacao: "); break;
                }

                strcat(buffer, row[i]);

                if(i != 2) {
                    strcat(buffer, "\n");
                } else {
                    strcat(buffer, " ");
                }
            }

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

            int entrei = 0;
            strcat(buffer, "Habilidades: ");
            while((row_ = mysql_fetch_row(result_))) {
                if(entrei) {
                    strcat(buffer, ", ");
                }
                entrei = 1;
                for(int j = 0; j < num_fields_; j++) {
                    strcat(buffer, row_[j]); 
                }
            }

            strcat(buffer, "\n");
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
            
            int counter = 1;
            char a[3];
            strcat(buffer, "Experiencia: ");
            while((row__ = mysql_fetch_row(result__))) {
                for(int j = 0; j < num_fields__; j++) {
                    if(counter != 1) {
                        strcat(buffer, "\t     ");
                    }
                    strcat(buffer, "(");
                    sprintf(a, "%d", counter);
                    strcat(buffer, a);
                    strcat(buffer, ") ");
                    strcat(buffer, row__[j]); 
                    strcat(buffer, "\n");
                }
                counter += 1;
            }
            
            mysql_close(con);
            return flen;

        }

    } else if (!strcmp(command, "email_info")) {
        char *email = strtok (NULL, ";");
        char query[1000] = "select email, nome, sobrenome, foto from "tabelaUSER" where email='";
        strcat(query, email);
        strcat(query, "';"); 

        if (mysql_query(con, query)) {
            return_error(con);
        } else {
            printf("Succesfully got email, nome, sobrenome, foto from "tabelaUSER" for specific email\n");
        }

        MYSQL_RES *result = mysql_store_result(con);
        
        if (result == NULL) {
            return_error(con);
        }

        int num_fields = mysql_num_fields(result);
        long int flen = 0;
        MYSQL_ROW row;
        while((row = mysql_fetch_row(result))) {
            for(int i = 0; i < num_fields; i++) {
                switch(i) {
                    case 0: strcat(buffer, "Email: ") ; break;
                    case 1: strcat(buffer, "Nome: "); break;
                    case 2: strcat(buffer, "Sobrenome: "); break;
                    case 3: strcat(buffer, "Foto: "); 
                            FILE *fp = fopen(row[i], "rb");
                            if (fp == NULL) {
                                fprintf(stderr, "cannot open image file %s\n", row[i]);    
                                exit(1);
                            }

                            fseek(fp, 0, SEEK_END);
                              
                            if (ferror(fp)) {
                                        
                                fprintf(stderr, "fseek() failed\n");
                                int r = fclose(fp);

                                if (r == EOF) {
                                    fprintf(stderr, "cannot close file handler\n");          
                                }    
                                                          
                                exit(1);
                            }  
                                
                            flen = ftell(fp);
                                  
                            if (flen == -1) {
                                            
                                perror("error occurred");
                                int r = fclose(fp);

                                if (r == EOF) {
                                    fprintf(stderr, "cannot close file handler\n");
                                }
                                exit(1);      
                            }
                                    
                            fseek(fp, 0, SEEK_SET);
                                      
                            if (ferror(fp)) {
                                fprintf(stderr, "fseek() failed\n");
                                int r = fclose(fp);
                                if (r == EOF) {
                                    fprintf(stderr, "cannot close file handler\n");
                                }                                  
                                exit(1);
                            }
                            fread(image, 1, flen, fp);
                            image[flen+1] = '\0';
                            break;
                }
                strcat(buffer, row[i]);

                if(i != 1) {
                    strcat(buffer, "\n");
                } else {
                    strcat(buffer, " ");
                }
            }

            mysql_close(con);
            return flen;
        }

    }
    else {
        mysql_close(con);
        return -2;
    }

    mysql_close(con);
    return -1;
}

int main(int argc, char *argv[]) {
    setenv("TZ", "PST8PDT", 1);
    tzset();

    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    //socklen_t sin_size;
    struct sockaddr_in servaddr, cliaddr;
    //struct sigaction sa;
    //int yes=1;
    //char s[INET6_ADDRSTRLEN];
    //int rv;
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        perror("socket creation failed"); 
        exit(1); 
    } 

    if(argc < 2) {
        printf("Numero invalido de argumentos para o servidor\n");
        return 0;
    }
    /* usage: ./servidor 1 
              ./servidor 2 */
    char path[100] = "";
    strcat(path, argv[1]);
    strcat(path, "_servidor.out");

    memset(&servaddr, 0, sizeof servaddr);
    memset(&cliaddr, 0, sizeof cliaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    //servaddr.sin_socktype = SOCK_DGRAM;
    //servaddr.sin_flags = AI_PASSIVE; // use my IP

    /*if ((rv = getaddrinfo(NULL, PORT, &servaddr, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }*/

    // loop through all the results and bind to the first we can
    /*for(p = servinfo; p != NULL; p = p->ai_next) {
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
    }*/

    //freeaddrinfo(servinfo); // all done with this structure

    /*if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }*/

    /*sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }*/

    if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { 
	perror("bind failed"); 
	exit(EXIT_FAILURE); 
    }

    printf("server: waiting for connections...\n");
    int exit_ = 0;

    while(1) {  // main accept() loop
        socklen_t sin_size = sizeof cliaddr;

        /*inet_ntop(cliaddr.ss_family,
                get_in_addr((struct sockaddr *)&cliaddr),
                s, sizeof s);
        
        printf("server: got connection from %s\n", s);
        */
        FILE *fp = fopen(path, "w");
        while(!exit_) { //client loop
            char client_message[BUFF_SIZE] = "";
            char a[20] = "";
            char b[20] = "";
            char *arroba = "@";
            long int numbytes = -1;
            size_t bytes_written = 0;
            size_t bytes_to_write = 0;
            //while((recvfrom(sockfd, client_message, BUFF_SIZE, 0, (struct sockaddr *)&cliaddr, &sin_size) <= 0));
            int received = recvfrom(sockfd, client_message, BUFF_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &sin_size);

            client_message[received] = '\0';

            printf("client_message: [%s]\n", client_message);

            if(!strcmp(client_message, "listar_tudo")) {
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

                MYSQL_RES *result;
                char query[1000] = "select email from "tabelaUSER";";

                if (mysql_query(con, query)) {
                    return_error(con);
                } else {
                    printf("Succesfully got all emails from "tabelaUSER"\n");
                }

                result = mysql_store_result(con);
                
                if (result == NULL) {
                    return_error(con);
                }

                int num_fields = mysql_num_fields(result);
                int num_rows = mysql_num_rows(result);

                sprintf(a, "%ld", num_rows);
                strcat(a, arroba);

                bytes_to_write = strlen(a);
                get_time(fp);

                while (bytes_written - bytes_to_write != 0) {
                    size_t written;

                    do {
                        written = sendto(sockfd, a + bytes_written, (bytes_to_write - bytes_written), 0, (struct sockaddr *)&cliaddr, sin_size);
                    } while((written == -1) && (errno == EINTR));

                    if(written == -1) {
                        break;
                    }

                    bytes_written += written;
                } 

                MYSQL_ROW row;
                while((row = mysql_fetch_row(result))) {
                    for(int i = 0; i < num_fields; i++) {
                        char buffer[BUFF_SIZE] = "";
                        char image[IMAGE_SIZE] = "";
                        long int image_size = req6(row[i], buffer, image, con);

                        sprintf(a, "%ld", strlen(buffer));
                        strcat(a, arroba);

                        bytes_to_write = strlen(a);
                        bytes_written = 0;
                        while (bytes_written - bytes_to_write != 0) {
                            size_t written;

                            do {
                                written = sendto(sockfd, a + bytes_written, (bytes_to_write - bytes_written), 0, (struct sockaddr *)&cliaddr, sin_size);
                            } while((written == -1) && (errno == EINTR));

                            if(written == -1) {
                                break;
                            }

                            bytes_written += written;
                        }

                        bytes_written = 0;
                        bytes_to_write = strlen(buffer);

                        while (bytes_written - bytes_to_write != 0) {
                            size_t written;

                            do {
                                written = sendto(sockfd, buffer + bytes_written, (bytes_to_write - bytes_written), 0,  (struct sockaddr *)&cliaddr, sin_size);
                            } while((written == -1) && (errno == EINTR));

                            if(written == -1) {
                                break;
                            }

                            bytes_written += written;
                        }

                        sprintf(b, "%ld", image_size);
                        strcat(b, arroba); 

                        bytes_written = 0;
                        bytes_to_write = strlen(b);

                        while (bytes_written != bytes_to_write) {
                            size_t written;

                            do {
                                written = sendto(sockfd, b + bytes_written, (bytes_to_write - bytes_written), 0, (struct sockaddr *)&cliaddr, sin_size);
                            } while((written == -1) && (errno == EINTR));

                            if (written == -1) {
                                break;
                            }
                            bytes_written += written;
                        }

                        bytes_written = 0;
                        bytes_to_write = image_size;

                        while (bytes_written != bytes_to_write) {
                            size_t written;

                            do {
                                written = sendto(sockfd, image + bytes_written, (bytes_to_write - bytes_written), 0, (struct sockaddr *)&cliaddr, sin_size);
                            } while((written == -1) && (errno == EINTR));

                            if (written == -1) {
                                break;
                            }
                            bytes_written += written;
                        }
                    }
                }

                mysql_close(con);
                
            } else {
                char buffer[BUFF_SIZE] = "";
                char image[IMAGE_SIZE] = "";
                char a[20] = "";
                char b[20] = "";
                char *arroba = "@";

                long int r = treat_call(client_message, buffer, image);
                printf("Retorno: %d\n", r);
                printf("Buffer: [%s]\n", buffer);
                get_time(fp);

                switch(r) {
                    case -2: printf("ENTRADA INVALIDA\n"); 
                            exit_ = 1;
                            break;
                    case -1: 
                            sprintf(a, "%ld", strlen(buffer));
                            strcat(a, arroba);

                            bytes_to_write = strlen(a);

                            while (bytes_written - bytes_to_write != 0) {
                                size_t written;

                                do {
                                    written = sendto(sockfd, a + bytes_written, (bytes_to_write - bytes_written), 0, (struct sockaddr *)&cliaddr, sin_size);
                                } while((written == -1) && (errno == EINTR));

                                if(written == -1) {
                                    break;
                                }

                                bytes_written += written;
                            } 

                            bytes_written = 0;
                            bytes_to_write = strlen(buffer);

                            while (bytes_written - bytes_to_write != 0) {
                                size_t written;

                                do {
                                    written = sendto(sockfd, buffer + bytes_written, (bytes_to_write - bytes_written), 0, (struct sockaddr *)&cliaddr, sin_size);
                                } while((written == -1) && (errno == EINTR));

                                if(written == -1) {
                                    break;
                                }

                                bytes_written += written;
                            } 
                            break;
                    default: sprintf(a, "%ld", strlen(buffer));
                            strcat(a, arroba);

                            bytes_to_write = strlen(a);
                            while (bytes_written - bytes_to_write != 0) {
                                size_t written;

                                do {
                                    written = sendto(sockfd, a + bytes_written, (bytes_to_write - bytes_written), 0, (struct sockaddr *)&cliaddr, sin_size);
                                } while((written == -1) && (errno == EINTR));

                                if(written == -1) {
                                    break;
                                }

                                bytes_written += written;
                            }

                            bytes_written = 0;
                            bytes_to_write = strlen(buffer);

                            while (bytes_written - bytes_to_write != 0) {
                                size_t written;

                                do {
                                    written = sendto(sockfd, buffer + bytes_written, (bytes_to_write - bytes_written), 0, (struct sockaddr *)&cliaddr, sin_size);
                                } while((written == -1) && (errno == EINTR));

                                if(written == -1) {
                                    break;
                                }

                                bytes_written += written;
                            }

                            sprintf(b, "%ld", r);
                            strcat(b, arroba); 

                            bytes_written = 0;
                            bytes_to_write = strlen(b);

                            while (bytes_written != bytes_to_write) {
                                size_t written;

                                do {
                                    written = sendto(sockfd, b + bytes_written, (bytes_to_write - bytes_written), 0, (struct sockaddr *)&cliaddr, sin_size);
                                } while((written == -1) && (errno == EINTR));

                                if (written == -1) {
                                    break;
                                }
                                bytes_written += written;
                            }

                            bytes_written = 0;
                            bytes_to_write = r;

                            while (bytes_written != bytes_to_write) {
                                size_t written;

                                do {
                                    written = sendto(sockfd, image + bytes_written, (bytes_to_write - bytes_written), 0, (struct sockaddr *)&cliaddr, sin_size);
                                } while((written == -1) && (errno == EINTR));

                                if (written == -1) {
                                    break;
                                }
                                bytes_written += written;
                            }

                            break;   
                }

            }

            get_time(fp);
            fprintf(fp, "\n");
            fflush(fp);

        }
        
        fclose(fp);    
    } 

    return 0;
}
