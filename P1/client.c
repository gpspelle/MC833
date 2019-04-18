#include "net.h"
#include <sys/stat.h>

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

int main(int argc, char *argv[]) {

    setenv("TZ", "PST8PDT", 1);
    tzset();

    printf("%d\n", argc);

    if(argc < 2) {
        printf("Numero invalido de argumentos para o cliente\n");
        return 0;
    }
    /* usage: ./client 1 
              ./client 2 */
    char path[100] = "";
    strcat(path, argv[1]);
    strcat(path, "_client.out");
    FILE *fp_output = fopen(path, "wb");
    char fi_path[100] = "";
    strcat(fi_path, argv[1]);
    strcat(fi_path, ".in");
    FILE *fi = fopen(fi_path, "rb");

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 0;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 0;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                    s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    for(;;) {
        printf("Seja bem vindo ao sistema do cliente. Aqui voce podera realizar as seguintes operacoes: \n");
        printf("1. Listar todas as pessoas formadas em um determinado curso;\n");
        printf("2. Listar as habilidades dos perfis que moram em uma determinada cidade;\n");
        printf("3. Acrescentar uma nova experiencia em um perfil;\n");
        printf("4. Dado o email do perfil, retornar suas experiencias;\n");
        printf("5. Listar todas as informacoes de todos os perfis;\n");
        printf("6. Dado o email de um perfil, retornar suas informacoes;\n");

        int input;
        fscanf(fi, " %d", &input);

        char curso[100];
        char cidade[100];
        char experiencia[100];
        char email[100];
        char message[300] = "";

        switch(input) {
            case 1: printf("Qual curso: ");
                    fscanf(fi, " %[^\n]s", curso); 
                    strcpy(message, "listar_curso;");
                    strcat(message, curso);
                    break;
            case 2: printf("Qual cidade: ");
                    fscanf(fi, " %[^\n]s", cidade); 
                    strcpy(message, "listar_habilidades;");
                    strcat(message, cidade);
                    break;
            case 3: printf("Qual experiencia: ");
                    fscanf(fi, " %[^\n]s", experiencia);
                    printf("Qual email: ");
                    fscanf(fi, " %s", email);
                    strcpy(message, "add;");
                    strcat(message, experiencia);
                    strcat(message, ";");
                    strcat(message, email);
                    break;
            case 4: printf("Qual email: ");
                    fscanf(fi, " %s", email);
                    strcpy(message, "email_experiencia;");
                    strcat(message, email);
                    break;
            case 5: strcpy(message, "listar_tudo"); 
                    break;
            case 6: printf("Qual email: ");
                    fscanf(fi, " %s", email);
                    strcpy(message, "email_tudo;");
                    strcat(message, email);
                    break;
            default:
                    close(sockfd);
                    fclose(fi);
                    fclose(fp_output); 
                    return 0;
        }

        if(input == 5) {

            size_t bytes_written = 0;
            size_t bytes_to_write = 0;
            bytes_to_write = strlen(message);
            
            get_time(fp_output);
            while (bytes_written - bytes_to_write != 0) {
                size_t written;

                do {
                    written = send(sockfd, message + bytes_written, (bytes_to_write - bytes_written), 0);
                } while((written == -1) && (errno == EINTR));

                if(written == -1) {
                    break;
                }

                bytes_written += written;
            }

            char quantidade_usuarios[20] = "";
            int numbytes;

            while(1) {
                char kkey0[1] = "";
                kkey0[0] = '\0';
                numbytes = recv(sockfd, kkey0, 1, 0); 
                kkey0[1] = '\0';

                if(*kkey0 == '@') {
                    break;
                }

                strcat(quantidade_usuarios, kkey0);
            }

            int num_usuarios = atoi(quantidade_usuarios);
            
            for(int i = 0; i < num_usuarios; i++) {
                char buf[MAX_DATA_SIZE] = "";
                char *image;

                char a[20] = "";
                char b[20] = "";

                while(1) {
                    char kkey0[1] = "";
                    kkey0[0] = '\0';
                    numbytes = recv(sockfd, kkey0, 1, 0); 
                    kkey0[1] = '\0';

                    if(*kkey0 == '@') {
                        break;
                    }

                    strcat(a, kkey0);
                }

                numbytes = atoi(a);
                size_t bytesRead = 0;
                size_t bytesToRead = numbytes;

                while (bytesToRead != bytesRead) {
                    size_t readThisTime;
                    do {
                        readThisTime = recv(sockfd, buf + bytesRead, (bytesToRead - bytesRead), 0);
                    } while((readThisTime == -1) && (errno == EINTR));

                    if(readThisTime == -1) {
                        break;
                    }  

                    bytesRead += readThisTime;
                }

                printf("Buf: %s\n", buf);

                char email[1000] = "";
                
                char *find = strstr(buf, "Email: ");

                while(find[0] != ' ') {
                    find+=sizeof(char);
                }

                find += sizeof(char);

                while(find[0] != '\n') {
                    strncat(email, find, 1);
                    find += sizeof(char);
                }

                numbytes = -1;

                char path[1000] = "dados/";
                strcat(path, email); 
                strcat(path, ".jpg");
                struct stat st = {0};

                if (stat("dados/", &st) == -1) {
                    mkdir("dados/", 0700);
                }

                FILE *fp = fopen(path, "wb");

                while(1) {
                    char kkey1[1] = "";
                    kkey1[0] = '\0';
                    numbytes = recv(sockfd, kkey1, 1, 0); 
                    kkey1[1] = '\0';

                    if(*kkey1 == '@') {
                        break;
                    }
                
                    strcat(b, kkey1);
                }

                numbytes = atoi(b);
                image = malloc(sizeof(char)*(numbytes+1));

                bytesRead = 0;
                bytesToRead = numbytes;

                while (bytesToRead != bytesRead) {
                    size_t readThisTime;
                    do {
                        readThisTime = recv(sockfd, image + bytesRead, (bytesToRead - bytesRead), 0);
                    } while((readThisTime == -1) && (errno == EINTR));

                    if(readThisTime == -1) {
                        break;
                    }

                    bytesRead += readThisTime;
                }

                size_t bytesToWrite = numbytes;
                size_t bytesWritten = 0;

                while (bytesWritten != bytesToWrite) {
                    size_t writtenThisTime;

                    do {
                       writtenThisTime = fwrite(image, 1, (bytesToWrite - bytesWritten), fp);
                    } while((writtenThisTime == -1) && (errno == EINTR));

                    if(writtenThisTime == -1) {
                        break;
                    }

                    bytesWritten += writtenThisTime;
                }

                fclose(fp);
                free(image);
                printf("[%s]\n", buf);
                printf("(%d, %d, %d)\n", bytesRead, bytesToRead, strlen(buf));
            }
   
        } else {

            printf("Message [%s]\n", message);

            char buf[MAX_DATA_SIZE] = "";
            long int numbytes = -1;  
            get_time(fp_output);

            size_t bytes_written = 0;
            size_t bytes_to_write = 0;
            bytes_to_write = strlen(message);

            while (bytes_written - bytes_to_write != 0) {
                size_t written;

                do {
                    written = send(sockfd, message + bytes_written, (bytes_to_write - bytes_written), 0);
                } while((written == -1) && (errno == EINTR));

                if(written == -1) {
                    break;
                }

                bytes_written += written;
            }

            char a[20] = "";

            while(1) {
                char kkey0[1] = "";
                kkey0[0] = '\0';
                numbytes = recv(sockfd, kkey0, 1, 0); 
                kkey0[1] = '\0';

                if(*kkey0 == '@') {
                    break;
                }

                strcat(a, kkey0);
            }

            numbytes = atoi(a);
            size_t bytesRead = 0;
            size_t bytesToRead = numbytes;

            while (bytesToRead != bytesRead) {
                size_t readThisTime;
                do {
                    readThisTime = recv(sockfd, buf + bytesRead, (bytesToRead - bytesRead), 0);
                } while((readThisTime == -1) && (errno == EINTR));

                if(readThisTime == -1) {
                    break;
                }  

                bytesRead += readThisTime;
            }

            printf("[%s]\n", buf);
            printf("(%d, %d, %d)\n", bytesRead, bytesToRead, strlen(buf));

        }
         
        if(input == 6) {
            char *email = strtok(message, ";");
            char buf[BUFF_SIZE] = "";
            email = strtok (NULL, ";");
            char path[1000] = "dados/";
            strcat(path, email); 
            strcat(path, ".jpg");
            struct stat st = {0};

            if (stat("dados/", &st) == -1) {
                mkdir("dados/", 0700);
            }

            FILE *fp = fopen(path, "wb");
            int numbytes;
            char b[20] = "";

            while(1) {
                char kkey1[1] = "";
                kkey1[0] = '\0';
                numbytes = recv(sockfd, kkey1, 1, 0); 
                kkey1[1] = '\0';

                if(*kkey1 == '@') {
                    break;
                }
            
                strcat(b, kkey1);
            }

            numbytes = atoi(b);
            char *image = malloc(sizeof(char)*(numbytes+1));

            size_t bytesRead = 0;
            size_t bytesToRead = numbytes;

            while (bytesToRead != bytesRead) {
                size_t readThisTime;
                do {
                    readThisTime = recv(sockfd, image + bytesRead, (bytesToRead - bytesRead), 0);
                } while((readThisTime == -1) && (errno == EINTR));

                if(readThisTime == -1) {
                    break;
                }

                bytesRead += readThisTime;
            }


            size_t bytesToWrite = numbytes;
            size_t bytesWritten = 0;

            while (bytesWritten != bytesToWrite) {
                size_t writtenThisTime;

                do {
                   writtenThisTime = fwrite(image, 1, (bytesToWrite - bytesWritten), fp);
                } while((writtenThisTime == -1) && (errno == EINTR));

                if(writtenThisTime == -1) {
                    break;
                }

                bytesWritten += writtenThisTime;
            }

            fclose(fp);
            free(image);
            printf("[%s]\n", buf);
            printf("(%d, %d, %d)\n", bytesRead, bytesToRead, strlen(buf));
        }

        get_time(fp_output);
        fprintf(fp_output, "\n");
    }
   
    close(sockfd);
    fclose(fp_output); 
    fclose(fi);

    return 0;

}
