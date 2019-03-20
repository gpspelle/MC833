#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#define user "root"
#define pswd "123"
#define network "localhost"

void create_database(MYSQL *con) {  

    if (mysql_real_connect(con, network, user, pswd, NULL, 0, NULL, 0) == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }  

    char a[] = "CREATE database ";
    char b[] = "MC833;";

    strcat(a, b);

    if (mysql_query(con, a)) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    } else {
        printf("Succesfully created Database MC833\n");
    }

    mysql_close(con);
}

int main() {

    MYSQL *con = mysql_init(NULL);

    if (con == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    create_database(con);
    return 0;
}

