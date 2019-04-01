#include "net.h"
#include <sys/stat.h>

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void get_time() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    printf("Current time: %s.%09ld UTC\n", buff, ts.tv_nsec);
}

void client_call(char *input, int num_input) {

    int sockfd;
    long int numbytes = -1;  
    char buf[MAX_DATA_SIZE] = "";
    char buf1[MAX_DATA_SIZE] = "";
    char *image;
    //char *image1;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return;
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
        return;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                    s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    if (send(sockfd, input, MAX_DATA_SIZE, 0) == -1) {
        perror("send");
        exit(1);
    }   

    char a[20] = "";
    char b[20] = "";

    while(1) {
        char kkey0[1];
        numbytes = read(sockfd, kkey0, 1); 

        if(*kkey0 == '@') {
            break;
        }
        strcat(a, kkey0);
    }

    numbytes = atoi(a);
    //int k;
    size_t bytesRead = 0;
    size_t bytesToRead = numbytes;

    while (bytesToRead != bytesRead) {
        size_t readThisTime;
        do {
            readThisTime = read(sockfd, buf + bytesRead, (bytesToRead - bytesRead));
        } while((readThisTime == -1) && (errno == EINTR));

        if (readThisTime == -1) {
            /* Real error. Do something appropriate. */
            break;
        }
        bytesRead += readThisTime;
    }

    //while((k = read(sockfd, buf, numbytes)) < 0);
    buf[strlen(buf)-1] = '\0';
    printf("[%s]\n", buf);
    numbytes = -1;

    if(num_input == 6) {
        char *email = strtok(input, ";");
        email = strtok (NULL, ";");
        char path[1000] = "dados/";
        strcat(path, email); 
        strcat(path, ".jpg");
        struct stat st = {0};

        if (stat("dados/", &st) == -1) {
            mkdir("dados/", 0700);
        }

        FILE *fp = fopen(path, "wb");

        while(1) {
            char kkey1[1];
            numbytes = read(sockfd, kkey1, 1); 
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
                readThisTime = read(sockfd, image + bytesRead, (bytesToRead - bytesRead));

                
            } while((readThisTime == -1) && (errno == EINTR));

            if (readThisTime == -1) {
                /* Real error. Do something appropriate. */
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

            if (writtenThisTime == -1) {
                break;
            }
            bytesWritten += writtenThisTime;
        }

        //while((x = read(sockfd, image, numbytes)) < 0);

        //fwrite(image, 1, numbytes, fp);
        //fprintf(fp, "%s", image);
        fclose(fp);
    }

    close(sockfd);

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
                    break;
            case 2: printf("Qual cidade: ");
                    scanf(" %[^\n]s", cidade); 
                    strcpy(message, "listar_habilidades;");
                    strcat(message, cidade);
                    break;
            case 3: printf("Qual experiencia: ");
                    scanf(" %[^\n]s", experiencia);
                    printf("Qual email: ");
                    scanf(" %s", email);
                    strcpy(message, "add;");
                    strcat(message, experiencia);
                    strcat(message, ";");
                    strcat(message, email);
                    break;
            case 4: printf("Qual email: ");
                    scanf(" %s", email);
                    strcpy(message, "email_experiencia;");
                    strcat(message, email);
                    break;
            case 5: strcpy(message, "listar_tudo"); 
                    break;
            case 6: printf("Qual email: ");
                    scanf(" %s", email);
                    strcpy(message, "email_tudo;");
                    strcat(message, email);
                    break;
            default:
                    exit(1);
        }

        printf("[%s]\n", message);
        client_call(message, input);
    }

    return 0;

}
