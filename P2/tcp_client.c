#include "net.h"
#include <sys/stat.h>

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void get_time(FILE *fp, char *i) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    fprintf(fp, "%s %s.%09ld;", i, buff+9, ts.tv_nsec);
}

int main(int argc, char *argv[]) {

    setenv("TZ", "PST8PDT", 1);
    tzset();

    printf("%d\n", argc);

    if(argc < 3) {
        printf("Numero invalido de argumentos para o cliente\n");
        return 0;
    }

    /* usage: ./client 1 1 lalala 
              ./client 2 1 lalalal */
    char path[100] = "";
    strcat(path, argv[1]);
    strcat(path, "_client.out");
    FILE *fp_output = fopen(path, "wb");
    char fi_path[100] = "";
    strcat(fi_path, argv[1]);
    strcat(fi_path, ".in");

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
        printf("7. Dado o email de um perfil, retornar seu nome, sobrenome e foto\n");

        char input = argv[2][0];
        char message[300] = "";
        strcat(message, argv[1]); 
        strcat(message, ";");

        switch(input) {
            case '7': strcat(message, "email_info;");
                    strcat(message, argv[3]);
                    break;
            default:
                    close(sockfd);
                    fclose(fp_output); 
                    return 0;
        }

        printf("Message [%s]\n", message);

        char buf[MAX_DATA_SIZE] = "";
        int numbytes = -1;  
        get_time(fp_output, argv[1]);

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

        char *email = strtok(message, ";");
        email = strtok (NULL, ";");
        char path[1000] = "dados/";
        strcat(path, email); 
        strcat(path, ".jpg");
        struct stat st = {0};

        if (stat("dados/", &st) == -1) {
            mkdir("dados/", 0700);
        }

        FILE *fp = fopen(path, "wb");
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

        size_t bytesToWrite = bytesRead;
        size_t bytesWritten = 0;
        fwrite(image, 1, bytesToWrite, fp);

        fclose(fp);
        free(image);

        get_time(fp_output, argv[1]);
        fprintf(fp_output, "\n");
        fflush(fp_output);
    }
   

    close(sockfd);
    fclose(fp_output); 

    return 0;

}
