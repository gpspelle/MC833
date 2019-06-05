import java.sql.*;

public class database {
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

