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

    struct timeval timeout;      
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

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
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;
    strcat(path, argv[1]);
    strcat(path, "_client.out");
    FILE *fp_output = fopen(path, "wb");

    char s[INET6_ADDRSTRLEN];
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	        return 1;
    }
    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        exit(1);
    }

    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                                                        sizeof(timeout)) < 0)
        error("setsockopt failed\n");
    
    for(;;) {
        printf("Seja bem vindo ao sistema do cliente. Aqui voce podera realizar as seguintes operacoes: \n");
        printf("7. Dado o email de um perfil, retornar seu nome, sobrenome e foto\n");

        char input = argv[2][0]; 
        char email[100];
        char message[300] = "";
        strcat(message, argv[1]); 
        strcat(message, ";");
        size_t readThisTime = IMAGE_READ;

        switch(input) {
            case '7': strcpy(email, argv[3]);
                    strcat(message, "email_info;");
                    strcat(message, email);
                    break;
            default:
                    close(sockfd);
                    fclose(fp_output); 
                    return 0;
        }

        printf("Message [%s]\n", message);

        char *buf = malloc(sizeof(char) * MAX_DATA_SIZE);
        long int numbytes = -1;  
        get_time(fp_output, argv[1]);

        sendto(sockfd, message, strlen(message), 0, p->ai_addr, p->ai_addrlen);

        numbytes = recvfrom(sockfd, buf, MAX_DATA_SIZE-1, 0, (struct sockaddr *)&their_addr, &addr_len);

        char num = buf[0];
        if (num != '0') {
            fprintf(fp_output, "%s FAILED - RECEIVED OUT OF ORDER MESSAGE %c\n", argv[1], num);
            break;
        }

        buf[numbytes] = '\0';
        printf("listener: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
        printf("listener: packet is %d bytes long\n", numbytes);
        printf("[%s]\n", buf);
        buf += 2;

        char *p_email = strtok(message, ";");
        p_email = strtok (NULL, ";");
        p_email = strtok (NULL, ";");
        buf -= 2;
        buf = realloc(buf, sizeof(char)*10);
        char path[1000] = "dados/";
        strcat(path, p_email); 
        strcat(path, ".jpg");
        struct stat st = {0};

        if (stat("dados/", &st) == -1) {
            mkdir("dados/", 0700);
        }

        FILE *fp = fopen(path, "wb");

        long int received = recvfrom(sockfd, buf, 10, 0, (struct sockaddr *)&their_addr, &addr_len);

        buf[received] = '\0';
        num = buf[0];
        if(num != '1') {
            fprintf(fp_output, "%s FAILED - RECEIVED OUT OF ORDER MESSAGE %c\n", argv[1], num);
            break;
        }
        
        buf += 2;
        size_t bytes_read = 0;
        size_t bytes_to_read = atoi(buf);
        char *image = malloc(sizeof(char)*bytes_to_read+1);

        int last_received = -1;
        int breaked = 0;
        buf -= 2;
        free(buf);
        while(bytes_read != bytes_to_read) {
       
            char *buff = malloc(sizeof(char) * (IMAGE_READ + 6)); 

            do {

                int x = 0;
                if(bytes_to_read - bytes_read > IMAGE_READ) {
                    x = IMAGE_READ + 5;
                } else {
                    x = (bytes_to_read % IMAGE_READ) + 5;
                }

                readThisTime = recvfrom(sockfd, buff, x, 0, (struct sockaddr *)&their_addr, &addr_len);

                buff[readThisTime+1] = '\0';

            } while((readThisTime == -1) && (errno == EINTR));
            
            if(readThisTime == -1) {
                fprintf(fp_output, "%s FAILED - TRIED TO READ WITH NOTHING TO READ\n", argv[1]);
                breaked = 1;
                free(buff);
                break;
            }

            num = buff[0];
            if(num !=  '2') {
                fprintf(fp_output, "%s FAILED - RECEIVED OUT OF ORDER MESSAGE %c\n", argv[1], num);
                breaked = 1;
                free(buff);
                break;
            }

            buff += 2;
            char a[4];
            a[0] = buff[0];
            a[1] = buff[1];
            a[2] = buff[2];
            a[3] = '\0';

            int received = atoi(a);
            if(received < last_received) {
                fprintf(fp_output, "%s FAILED - RECEIVED OUT OF ORDER MESSAGE IMAGE %d %d\n", argv[1], received, last_received);
                breaked = 1;
                buff -= 2;
                free(buff);
                break;
            }

            last_received = received;
            buff += 3;

            readThisTime -= 5;

            for(int c = 0; c < readThisTime; c++) {
                image[bytes_read + c] = buff[c]; 
            }

            bytes_read += readThisTime;

            buff -= 5;
            free(buff);

        }

        if(breaked) {
            fflush(fp_output);
            fclose(fp);
            free(image);
            break;
        }

        printf("listener: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
        printf("listener: packet is %d bytes long\n", readThisTime);
        printf("Lemos: %d Esperavamos: %d\n", bytes_read, bytes_to_read);

        size_t bytesToWrite = bytes_read;
        size_t bytesWritten = 0;

        fwrite(image, 1, bytesToWrite, fp);

        fclose(fp);
        free(image);

        get_time(fp_output, argv[1]);
        fprintf(fp_output, "\n");
        fflush(fp_output);
        
        break;
    }

   
    freeaddrinfo(servinfo); // all done with this structure

    close(sockfd);
    fclose(fp_output); 

    return 0;

}
