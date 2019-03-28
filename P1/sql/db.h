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

void return_error();
void create_DBuser();
void create_table();
void add_exps();
void add_habs();
void add_user();
void test_image();
