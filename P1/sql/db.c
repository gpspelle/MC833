#include "db.h"

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

    if (mysql_query(con, "DROP TABLE IF EXISTS "tabelaUSER";")) {
        return_error(con);
    }
    
    if (mysql_query(con, "DROP TABLE IF EXISTS "tabelaEXP";")) {
        return_error(con);
    }
    
    if (mysql_query(con, "DROP TABLE IF EXISTS "tabelaHAB";")) {
        return_error(con);
    }

    if (mysql_query(con, "CREATE TABLE "tabelaUSER"(Personid int NOT NULL AUTO_INCREMENT, email Text, nome Text, sobrenome Text, foto Text, residencia Text, formacao Text, PRIMARY KEY(Personid));")) {
        return_error(con);
    } else {
        printf("Succesfully created Table "tabelaUSER"\n");
    }
    
    if (mysql_query(con, "CREATE TABLE "tabelaEXP"(Expid int NOT NULL AUTO_INCREMENT, Personid int, exp Text, PRIMARY KEY(Expid));")) {
        return_error(con);
    } else {
        printf("Succesfully created Table "tabelaEXP"\n");
    }

    if (mysql_query(con, "CREATE TABLE "tabelaHAB"(Habid int NOT NULL AUTO_INCREMENT, Personid int, hab Text, PRIMARY KEY(Habid));")) {
        return_error(con);
    } else {
        printf("Succesfully created Table "tabelaHAB"\n");
    }

}

void add_exps(MYSQL *con, char exps[][100], char *email, int num_exp) {

    char query[100000] = "SELECT Personid FROM "tabelaUSER" WHERE email = '";
    strcat(query, email);
    strcat(query, "';");

    if (mysql_query(con, query)) {
        return_error(con);
    }
      
    MYSQL_RES *result = mysql_store_result(con);
        
    if (result == NULL) {
        return_error(con);
    }
    
    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;
   
    while ((row = mysql_fetch_row(result))) { 
        for(int i = 0; i < num_fields; i++) { 
            for(int j = 0; j < num_exp; j++) {
                char query[100000] = "INSERT INTO "tabelaEXP" (Personid, exp) VALUES(";
                strcat(query, row[i]);
                strcat(query, ", '");
                strcat(query, exps[j]);
                strcat(query, "');");
                printf("Query: [%s]\n", query);
                if (mysql_query(con, query)) {
                    return_error(con);
                }
            }
        } 
    }
}

void add_habs(MYSQL *con, char habs[][100], char *email, int num_habs) {

    char query[100000] = "SELECT Personid FROM "tabelaUSER" WHERE email = '";
    strcat(query, email);
    strcat(query, "';");

    if (mysql_query(con, query)) {
        return_error(con);
    }
      
    MYSQL_RES *result = mysql_store_result(con);
        
    if (result == NULL) {
        return_error(con);
    }
    
    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;
   
    while ((row = mysql_fetch_row(result))) { 
        for(int i = 0; i < num_fields; i++) { 
            for(int j = 0; j < num_habs; j++) {
                char query[100000] = "INSERT INTO "tabelaHAB" (Personid, hab) VALUES(";
                strcat(query, row[i]);
                strcat(query, ", '");
                strcat(query, habs[j]);
                strcat(query, "');");
                printf("Query: [%s]\n", query);
                if (mysql_query(con, query)) {
                    return_error(con);
                }
            }
        } 
    }
}

void add_user(MYSQL *con, userT *u, char *cwd) {

    char photo_path[1024];
    strcpy(photo_path, cwd);
    strcat(photo_path, u->foto);
    FILE *fp = fopen(photo_path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "cannot open image file %s\n", photo_path);    
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

    char query[100000] = "INSERT INTO "tabelaUSER" (email, nome, sobrenome, foto, residencia, formacao) VALUES('";
    strcat(query, u->email);
    strcat(query, "', '");
    strcat(query, u->nome);
    strcat(query, "', '");
    strcat(query, u->sobrenome);
    strcat(query, "', '");
    strcat(query, photo_path);
    strcat(query, "', '");
    strcat(query, u->residencia);
    strcat(query, "', '");
    strcat(query, u->formacao);
    strcat(query, "');");

    printf("Query: [%s]\n", query);
    
    if (mysql_query(con, query)) {
        return_error(con);
    }
    
}

void test_image(MYSQL *con) {

    FILE *fp = fopen("woman2.jpg", "wb");
      
    if (fp == NULL) {
        fprintf(stderr, "cannot open image file\n");    
        exit(1);
    }

    if (mysql_query(con, "SELECT foto FROM "tabelaUSER" WHERE Personid=1")) {
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
