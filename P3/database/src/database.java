import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class database {

    private static final String PORT = "7904";
    private static final int BACKLOG = 20;
    private static final int MAX_DATA_SIZE = 10240;
    private static final int BUFF_SIZE = 10240;
    private static final int WR_SIZE = 1024;

    private static final String tabelaUSER = "Usuarios";
    private static final String tabelaEXP = "Experiencias";
    private static final String tabelaHAB = "Habilidades";

    private String url;
    private String login;
    private String senha;
    private Connection conn;

    public database(String url, String login, String senha) {
        this.url = url;
        this.login = login;
        this.senha = senha;
    }

    public boolean connect() {
        try {
            Class.forName("com.mysql.cj.jdbc.Driver").newInstance();
            System.out.println("\nDriver carregado com sucesso!\n");
            try {
                this.conn = DriverManager.getConnection(url, login, senha);
                return true;

            } catch (Exception ex) {
                System.out.println("\nErro no connection!");
                return false;
            }

        } catch (Exception ex) {
            System.out.println("\nDriver nao pode ser carregado!");
            return false;
        }
    }

    // Pessoas formadas em um curso (formacao)
    public List<String> op1 (String formacao) {
        String query = "select nome,sobrenome from " + tabelaUSER + " where formacao='" + formacao + "';";
        List<String> names = new ArrayList<>();
        try {
            Statement stm = conn.createStatement();
            try {
                ResultSet rs = stm.executeQuery(query);
                while (rs.next()) {
                    String nome = rs.getString("nome");
                    String sobrenome = rs.getString("sobrenome");
                    names.add("Nome: " + nome + "\nSobrenome: " + sobrenome + "\n----------------------------------------\n");
                }
                System.out.println("\nConsulta realizada com sucesso!!!\n");
            } catch (Exception ex) {
                System.out.println("\nErro no resultset!");
                return null;
            }
        } catch (Exception ex) {
            System.out.println("\nErro no statement!");
            return null;
        }

        return names;
    }

    // Habilidades dos perfis que moram em uma determinada cidade
    public void op2 (String cidade) {

    }

    // Adicionar Nova Experiencia em um Perfil
    public void op3 (String exp, String email) {

    }

    // Dado o email, retorna a experiencia
    public void op4 (String email) {

    }

    // Todas as informaçoes de todos os perfis
    public void op5 () {

    }

    // Dado o email, retornar as informaçoes
    public void op6 (String email) {

    }


    public void get_names() {
        try {
            String sql = "SELECT nome, sobrenome FROM Usuarios";
            Statement stm = conn.createStatement();
            try {
                ResultSet rs = stm.executeQuery(sql);
                while (rs.next()) {
                    String nome = rs.getString("nome");
                    String sobrenome = rs.getString("sobrenome");
                    System.out.println("Nome: " + nome + "\nSobrenome: " + sobrenome);
                    System.out.println("---------------------------------------");
                }
                System.out.println("\nConsulta realizada com sucesso!!!\n");
            } catch (Exception ex) {
                System.out.println("\nErro no resultset!");
            }
        } catch (Exception ex) {
            System.out.println("\nErro no statement!");
        }
    }
}

