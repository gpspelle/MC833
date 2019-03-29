#include "db.h"

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

    char cwd[1024];
    chdir("/path/to/change/directory/to");
    getcwd(cwd, sizeof(cwd));

    /* Creating user structure */
    userT gabriel;
    strcpy(gabriel.email, "g172358@dac.unicamp.br");
    strcpy(gabriel.nome, "Gabriel");
    strcpy(gabriel.sobrenome, "Pellegrino da Silva");
    strcpy(gabriel.foto, "/photos/g172358.jpg");
    strcpy(gabriel.residencia, "Araras");
    strcpy(gabriel.formacao, "Engenharia da Computacao");
   
    char exps_gab[3][100]; 

    strcpy(exps_gab[0], "CONPEC");
    strcpy(exps_gab[1], "CEPETRO");
    strcpy(exps_gab[2], "SEMANTIX");

    
    char habs_gab[4][100];
    strcpy(habs_gab[0], "C");
    strcpy(habs_gab[1], "Python");
    strcpy(habs_gab[2], "SQL");
    strcpy(habs_gab[3], "PARLER FRANCAIS");

    add_user(con, &gabriel, cwd);

    add_exps(con, exps_gab, gabriel.email, 3);
    add_habs(con, habs_gab, gabriel.email, 4);

    userT giovanna;
    strcpy(giovanna.email, "g173304@dac.unicamp.br");
    strcpy(giovanna.nome, "Giovanna");
    strcpy(giovanna.sobrenome, "Vendramini");
    strcpy(giovanna.foto, "/photos/g173304.jpg");
    strcpy(giovanna.residencia, "Campinas");
    strcpy(giovanna.formacao, "Engenharia da Computacao");

    char exps_gi[1][100]; 
    strcpy(exps_gi[0], "BIOMEDICA");
    
    char habs_gi[2][100];
    strcpy(habs_gi[0], "VHDL");
    strcpy(habs_gi[1], "DEBUGAR");

    add_user(con, &giovanna, cwd);

    add_exps(con, exps_gi, giovanna.email, 1);
    add_habs(con, habs_gi, giovanna.email, 2);

    userT rafael;
    strcpy(rafael.email, "r186145@dac.unicamp.br");
    strcpy(rafael.nome, "Rafael");
    strcpy(rafael.sobrenome, "Figueiredo Prudencio");
    strcpy(rafael.foto, "/photos/r186145.jpg");
    strcpy(rafael.residencia, "Campinas");
    strcpy(rafael.formacao, "Engenharia da Computacao");

    char exps_raf[2][100];
    strcpy(exps_raf[0], "GRUPO DE ROBOTICA");
    strcpy(exps_raf[1], "ANALISE DE IMAGENS E VIDEOS");
   
    char habs_raf[3][100]; 
    strcpy(habs_raf[0], "PYTHON");
    strcpy(habs_raf[1], "C");
    strcpy(habs_raf[2], "LEAGUE OF LEGENDS");

    add_user(con, &rafael, cwd);

    add_exps(con, exps_raf, rafael.email, 2);
    add_habs(con, habs_raf, rafael.email, 3);

    userT matheus;
    strcpy(matheus.email, "m184403@dac.unicamp.br");
    strcpy(matheus.nome, "Matheus");
    strcpy(matheus.sobrenome, "Rotta Alves");
    strcpy(matheus.foto, "/photos/m184403.jpg");
    strcpy(matheus.residencia, "Campinas");
    strcpy(matheus.formacao, "Engenharia da Computacao");

    char exps_mat[2][100];
    strcpy(exps_mat[0], "CONPEC");
    strcpy(exps_mat[1], "CEPETRO");
    
    char habs_mat[4][100];
    strcpy(habs_mat[0], "C");
    strcpy(habs_mat[1], "C++");
    strcpy(habs_mat[2], "HABLA ITALLIANO");
    strcpy(habs_mat[3], "SER O BARNEY");

    add_user(con, &matheus, cwd);

    add_exps(con, exps_mat, matheus.email, 2);
    add_habs(con, habs_mat, matheus.email, 4);

    userT luma;
    strcpy(luma.email, "l183171@dac.unicamp.br");
    strcpy(luma.nome, "Luma");
    strcpy(luma.sobrenome, "Oliveira Lombello");
    strcpy(luma.foto, "/photos/l183171.jpg");
    strcpy(luma.residencia, "Araras");
    strcpy(luma.formacao, "Engenharia da Computacao");

    char exps_lum[2][100];
    strcpy(exps_lum[0], "CONPEC");
    strcpy(exps_lum[1], "INICIACAO CIENTIFICA");
   
    char habs_lum[3][100];
    strcpy(habs_lum[0], "C");
    strcpy(habs_lum[1], "ONTOLOGIAS");
    strcpy(habs_lum[2], "PARLER FRANCAIS");

    add_user(con, &luma, cwd);
    
    add_exps(con, exps_lum, luma.email, 2);
    add_habs(con, habs_lum, luma.email, 3);

    userT ricardo;
    strcpy(ricardo.email, "vendraminiefamilia@gmail.com");
    strcpy(ricardo.nome, "Ricardo");
    strcpy(ricardo.sobrenome, "Vendramini");
    strcpy(ricardo.foto, "/photos/ricardo.jpg");
    strcpy(ricardo.residencia, "Sao Jose do Rio Preto");
    strcpy(ricardo.formacao, "Engenharia Civil");

    char exps_ric[2][100];
    strcpy(exps_ric[0], "MUSICA");
    strcpy(exps_ric[1], "PROFESSOR");
   
    char habs_ric[3][100];
    strcpy(habs_ric[0], "SAXOFONE");
    strcpy(habs_ric[1], "FLAUTA");
    strcpy(habs_ric[2], "VIOLINO");

    add_user(con, &ricardo, cwd);
    
    add_exps(con, exps_ric, ricardo.email, 2);
    add_habs(con, habs_ric, ricardo.email, 3);

    userT marcos;
    strcpy(marcos.email, "marcoosrs@gmail.com");
    strcpy(marcos.nome, "Marcos");
    strcpy(marcos.sobrenome, "Roberto e Souza");
    strcpy(marcos.foto, "/photos/marcos.jpg");
    strcpy(marcos.residencia, "Santa Adelia");
    strcpy(marcos.formacao, "Ciencia da Computacao");

    char exps_mar[2][100];
    strcpy(exps_mar[0], "CCNA EXPLORATION");
    strcpy(exps_mar[1], "MONITORIA DE BD");
   
    char habs_mar[3][100];
    strcpy(habs_mar[0], "PROCESSAMENTO E ANALISE DE IMAGENS E VIDEOS");
    strcpy(habs_mar[1], "APRENDIZADO DE MAQUINA");
    strcpy(habs_mar[2], "VISAO COMPUTACIONAL");

    add_user(con, &marcos, cwd);
    
    add_exps(con, exps_mar, marcos.email, 2);
    add_habs(con, habs_mar, marcos.email, 3);

    return 0;
}

