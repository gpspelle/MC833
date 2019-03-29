#include "net.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void client_call(char *input) {

    int sockfd, numbytes;  
    char buf[MAX_DATA_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("luma", PORT, &hints, &servinfo)) != 0) {
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

    if (send(sockfd, input, MAX_DATA_SIZE-1, 0) == -1) {
        perror("send");
        exit(1);
    }   

    while((numbytes = read(sockfd, buf, MAX_DATA_SIZE-1)) < 0);
    buf[numbytes] = '\0';

    printf("[%s]\n", buf);

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
