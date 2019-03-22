#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#define userBD "redes"
#define pswd "123"
#define network "localhost"
#define databasea "MC833"
#define databaseb "MC833;"
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

    if (mysql_query(con, "DROP database IF EXISTS "databaseb)) {
        return_error(con);
    }

    if (mysql_query(con, "CREATE database "databaseb)) {
        return_error(con);
    } else {
        printf("Succesfully created Database "databasea"\n");
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

    MYSQL *con = mysql_init(NULL);

    if (con == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, network, userBD, pswd, NULL, 0, NULL, 0) == NULL) {
        return_error(con);
    }  

    /* Created database once */
    create_database(con);

    /* Select specific database for use */
    if (mysql_query(con, "Use "databaseb)) {
        return_error(con);
    } else {
        printf("Succesfully using "databasea"\n");
    }

    /* Creating our user table */
    create_table(con);

    /* Creating user structure */
    userT gabriel;
    strcpy(gabriel.email, "g172358@dac.unicamp.br");
    strcpy(gabriel.nome, "Gabriel");
    strcpy(gabriel.sobrenome, "Pellegrino da Silva");
    strcpy(gabriel.foto, "g172358.jpg");
    strcpy(gabriel.residencia, "Campinas");
    strcpy(gabriel.formacao, "Engenharia da Computacao");

    add_user(con, &gabriel);

    test_image(con);

    return 0;
}

