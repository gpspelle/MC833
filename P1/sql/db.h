#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql.h>

#define rootDB "root"
#define pswd_root "123"
#define userDB "redes"
#define pswd "123"
#define network "localhost"
#define DB "MC833"
#define tabelaUSER "Usuarios"
#define tabelaEXP "Experiencias"
#define tabelaHAB "Habilidades"

typedef struct user {
    char email[100];
    char nome[100];
    char sobrenome[100];
    char foto[100];
    char residencia[100];
    char formacao[100];
}userT;

void return_error(MYSQL *con);
void create_database(MYSQL *con);
void create_DBuser(MYSQL *con);
void create_table(MYSQL *con);
void add_exps(MYSQL *con, char [][100], char *email, int num_exp);
void add_habs(MYSQL *con,char [][100], char *email, int num_habs);
void add_user(MYSQL *con, userT *u, char *cwd);
void test_image(MYSQL *con);
