#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#define rootDB "root"
#define pswd_root "123"
#define userDB "redes"
#define pswd "123"
#define network "localhost"
#define DB "MC833"
#define tabela "Usuarios"


typedef struct user {
    char email[100];
    char nome[100];
    char sobrenome[100];
    char foto[100];
    char residencia[100];
    char formacao[100];
}userT;

void return_error(MYSQL *con) {
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);        
}

void create_database(MYSQL *con) {  

    if (mysql_query(con, "DROP database IF EXISTS "DB";")) {
        return_error(con);
    }

    if (mysql_query(con, "CREATE database "DB";")) {
        return_error(con);
    } else {
        printf("Succesfully created Database "DB"\n");
    }

}

void create_DBuser(MYSQL *con) {  

    mysql_query(con, "FLUSH PRIVILEGES;");
    mysql_query(con, "DROP User IF EXISTS '"userDB"'@'"network"';");

    if (mysql_query(con, "CREATE User '"userDB"'@'"network"' IDENTIFIED BY '"pswd"';")) {
        return_error(con);
    } else {
        printf("Succesfully created User "userDB"\n");
        mysql_query(con, "GRANT ALL PRIVILEGES ON *.* TO '"userDB"'@'"network"';");
    }

}

void create_table(MYSQL *con) {  

    if (mysql_query(con, "DROP TABLE IF EXISTS "tabela";")) {
        return_error(con);
    }

    if (mysql_query(con, "CREATE TABLE "tabela"(Personid int NOT NULL AUTO_INCREMENT, email Text, nome Text, sobrenome Text, foto MediumBlob, residencia Text, formacao Text, PRIMARY KEY(Personid));")) {
        return_error(con);
    } else {
        printf("Succesfully created Table "tabela"\n");
    }

}

void add_user(MYSQL *con, userT *u) {

    FILE *fp = fopen(u->foto, "rb");
    if (fp == NULL) {
        fprintf(stderr, "cannot open image file %s\n", u->foto);    
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
        
    int flen = ftell(fp);
          
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

    char data[flen+1];
    int size = fread(data, 1, flen, fp);
                    
    if (ferror(fp)) {
        fprintf(stderr, "fread() failed\n");
        int r = fclose(fp);
        if (r == EOF) {
            fprintf(stderr, "cannot close file handler\n");
        }
        exit(1);      
    }
                      
    int r = fclose(fp);
    if (r == EOF) {
        fprintf(stderr, "cannot close file handler\n");
    }

    char query[100000] = "INSERT INTO "tabela" (email, nome, sobrenome, foto, residencia, formacao) VALUES('";
    strcat(query, u->email);
    strcat(query, "', '");
    strcat(query, u->nome);
    strcat(query, "', '");
    strcat(query, u->sobrenome);
    /* A foto é um parametro (esta em chunk) */
    strcat(query, "', '%s', '");
    strcat(query, u->residencia);
    strcat(query, "', '");
    strcat(query, u->formacao);
    strcat(query, "')");

    printf("Query: [%s]\n", query);
    
    char chunk[2*size+1];
    mysql_real_escape_string(con, chunk, data, size);

    size_t st_len = strlen(query);

    char query_[st_len + 2*size+1]; 
    int len = snprintf(query_, st_len + 2*size+1, query, chunk);

    if (mysql_real_query(con, query_, len)) {
        return_error(con);
    }
    
}

void test_image(MYSQL *con) {

    FILE *fp = fopen("woman2.jpg", "wb");
      
    if (fp == NULL) {
        fprintf(stderr, "cannot open image file\n");    
        exit(1);
    }

    if (mysql_query(con, "SELECT foto FROM "tabela" WHERE Personid=1")) {
        return_error(con);
    }
      
    MYSQL_RES *result = mysql_store_result(con);
        
    if (result == NULL) {
        return_error(con);
    }  

    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned long *lengths = mysql_fetch_lengths(result);
        
    if (lengths == NULL) {
        return_error(con);
    }
          
    fwrite(row[0], lengths[0], 1, fp);

    if (ferror(fp)) {            
        fprintf(stderr, "fwrite() failed\n");
        mysql_free_result(result);
        mysql_close(con);

        exit(1);      
    }  
              
    int r = fclose(fp);

    if (r == EOF) {
        fprintf(stderr, "cannot close file handler\n");
    }
                  
    mysql_free_result(result);
}

int main() {

    MYSQL *con_root = mysql_init(NULL);

    if (con_root == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con_root));
        exit(1);
    }

    if (mysql_real_connect(con_root, network, rootDB, pswd_root, NULL, 0, NULL, 0) == NULL) {
        return_error(con_root);
    }  

    /* Create a user for our MySQL environment using root access */
    create_DBuser(con_root);

    mysql_close(con_root);
    MYSQL *con = mysql_init(NULL);

    if (mysql_real_connect(con, network, userDB, pswd, NULL, 0, NULL, 0) == NULL) {
        return_error(con);
    }  

    /* Created database once */
    create_database(con);

    /* Select specific database for use */
    if (mysql_query(con, "Use "DB";")) {
        return_error(con);
    } else {
        printf("Succesfully using "DB"\n");
    }

    /* Creating our user table */
    create_table(con);

    /* Creating user structure */
    userT gabriel;
    strcpy(gabriel.email, "g172358@dac.unicamp.br");
    strcpy(gabriel.nome, "Gabriel");
    strcpy(gabriel.sobrenome, "Pellegrino da Silva");
    strcpy(gabriel.foto, "g172358.jpg");
    strcpy(gabriel.residencia, "Araras");
    strcpy(gabriel.formacao, "Engenharia da Computacao");

    add_user(con, &gabriel);

    userT giovanna;
    strcpy(giovanna.email, "g173304@dac.unicamp.br");
    strcpy(giovanna.nome, "Giovanna");
    strcpy(giovanna.sobrenome, "Vendramini");
    strcpy(giovanna.foto, "g173304.jpg");
    strcpy(giovanna.residencia, "Campinas");
    strcpy(giovanna.formacao, "Engenharia da Computacao");

    add_user(con, &giovanna);

    userT rafael;
    strcpy(rafael.email, "r186145@dac.unicamp.br");
    strcpy(rafael.nome, "Rafael");
    strcpy(rafael.sobrenome, "Figueiredo Prudencio");
    strcpy(rafael.foto, "r186145.jpg");
    strcpy(rafael.residencia, "Campinas");
    strcpy(rafael.formacao, "Engenharia da Computacao");

    add_user(con, &rafael);

    userT matheus;
    strcpy(matheus.email, "m184403@dac.unicamp.br");
    strcpy(matheus.nome, "Matheus");
    strcpy(matheus.sobrenome, "Rotta Alves");
    strcpy(matheus.foto, "m184403.jpg");
    strcpy(matheus.residencia, "Campinas");
    strcpy(matheus.formacao, "Engenharia da Computacao");

    add_user(con, &matheus);

    userT luma;
    strcpy(luma.email, "l183171@dac.unicamp.br");
    strcpy(luma.nome, "Luma");
    strcpy(luma.sobrenome, "Oliveira Lombello");
    strcpy(luma.foto, "l183171.jpg");
    strcpy(luma.residencia, "Araras");
    strcpy(luma.formacao, "Engenharia da Computacao");

    add_user(con, &luma);


    return 0;
}

