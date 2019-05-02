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

void get_time(FILE *fp, char *s) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    fprintf(fp, "%s %s.%09ld;", s, buff+9, ts.tv_nsec);
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

    /* Removing client ID */
    command = strtok (client_message, ";");
    command = strtok (NULL, ";");
    if (!strcmp(command, "email_info")) {
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
                    case 0: strcat(buffer, "Email: ");
                            strcat(buffer, row[i]);
                            break;
                    case 1: strcat(buffer, "Nome: ");
                            strcat(buffer, row[i]);
                            break;
                    case 2: strcat(buffer, "Sobrenome: ");
                            strcat(buffer, row[i]);
                            break;
                    case 3: strcat(buffer, "");
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

                if(i == 0) {
                    strcat(buffer, "\n");
                } else if(i == 1) {
                    strcat(buffer, " ");
                }
            }

            mysql_close(con);
            return flen;
        }
    } else {
        mysql_close(con);
        return -2;
    }

    mysql_close(con);
    return -1;
}

int main(int argc, char *argv[]) {
    setenv("TZ", "PST8PDT", 1);
    tzset();

    if(argc < 2) {
        printf("Numero invalido de argumentos para o servidor\n");
        return 0;
    }
    /* usage: ./servidor 1 
              ./servidor 2 */
    char path[100] = "";
    strcat(path, argv[1]);
    strcat(path, "_servidor.out");

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage cliaddr;
    char s[INET6_ADDRSTRLEN];
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }
    printf("listener: waiting to recvfrom...\n");

    int exit_ = 0;

    socklen_t sin_size = sizeof cliaddr;

    FILE *fp = fopen(path, "w");
    while(!exit_) { //client loop
        char client_message[BUFF_SIZE] = "";
        long int numbytes = -1;
        size_t bytes_written = 0;
        size_t bytes_to_write = 0;

        printf("Esperando mensagem\n");
        sin_size = sizeof(cliaddr);
        if((numbytes = recvfrom(sockfd, client_message, MAXBUFLEN-1, 0, (struct sockaddr *)&cliaddr, &sin_size)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        char num[2];
        num[0] = client_message[0];
        if(client_message[1] != ';') {
            num[1] = client_message[1];
        }
        
        printf("Recebi mensagem\n");

        client_message[numbytes] = '\0';
        printf("listener: got packet from %s\n", inet_ntop(cliaddr.ss_family, get_in_addr((struct sockaddr *)&cliaddr), s, sizeof s));
        printf("listener: packet is %d bytes long\n", numbytes);
        printf("listener: packet contains \"%s\"\n", client_message);

        char buffer[BUFF_SIZE] = "0#";
        char image[IMAGE_SIZE] = "";

        long int r = treat_call(client_message, buffer, image);
        get_time(fp, num);
        int written;

        switch(r) {
            case -2: printf("ENTRADA INVALIDA\n"); 
                    exit_ = 1;
                    break;
            default: 
                    
                    written = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cliaddr, sin_size);
                    char a[8] = "1#";
                    char b[6] = "";
                    sprintf(b, "%d", r);
                    strcat(a, b);

                    sendto(sockfd, a, strlen(a), 0, (struct sockaddr *)&cliaddr, sin_size);

                    size_t bytes_written = 0;
                    int image_counter = 0;
                    int counter = 0;
                    for(; counter < r / IMAGE_READ; counter++) {
                        char bla[IMAGE_READ+6]= "2#___";

                        int aux = counter;
                        bla[2] = '0' +  aux/100;
                        aux %= 100;
                        bla[3] = '0' + aux / 10;
                        aux %= 10;
                        bla[4] = '0' + aux;

                        for(int c = 0; c < IMAGE_READ; c++) {
                            bla[c+5] = image[bytes_written + c];
                        }
                        bla[IMAGE_READ + 5] = '\0';

                        written = sendto(sockfd, bla, IMAGE_READ+5, 0, (struct sockaddr *)&cliaddr, sin_size);
                        written -= 5;
                        bytes_written += written;
                    }

                    char bla[IMAGE_READ+6]= "2#___";
    
                    bla[2] = '0' +  counter/100;
                    counter %= 100;
                    bla[3] = '0' + counter / 10;
                    counter %= 10;
                    bla[4] = '0' + counter;

                    for(int c = 0; c < r%IMAGE_READ; c++) {
                        bla[c+5] = image[bytes_written + c];
                    }
                    
                    bla[r%IMAGE_READ + 5] = '\0';

                    written = sendto(sockfd, bla, (r%IMAGE_READ) + 5, 0, (struct sockaddr *)&cliaddr, sin_size);
                    
                    break;   
        }

        get_time(fp, num);
        fprintf(fp, "\n");
        fflush(fp);

    }
    
    freeaddrinfo(servinfo);
    fclose(fp);    

    return 0;
}
