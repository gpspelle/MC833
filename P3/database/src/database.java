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
    public List<String> op2 (String cidade) {
        String query = "select hab from " + tabelaUSER + " as u, " + tabelaHAB + " as h, where u.Personid = h.Personid and u.residencia'=" + cidade +"';";
        List<String> habs = new ArrayList<>();
        try {
            Statement stm = conn.createStatement();
            try {
                ResultSet rs = stm.executeQuery(query);
                while (rs.next()) {
                    String hab = rs.getString("hab");
                    habs.add("Habilidade: " + hab + "\n----------------------------------------\n");
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

        return habs;

    }

    // Adicionar Nova Experiencia em um Perfil
    public boolean op3 (String exp, String email) {
        String query = "select Personid from " + tabelaUSER + " where email='" + email +"';";
        try {
            Statement stm = conn.createStatement();
            try {
                ResultSet rs = stm.executeQuery(query);
                String id="";
                while (rs.next()) {
                    id = rs.getString("Personid");
                }
                System.out.println("\nConsulta realizada parcialmente com sucesso!!!\n");
                query = "insert into " + tabelaEXP + " (Personid, exp) VALUES(" + id + ", '" + exp + "');";
                if (!stm.execute(query)) {
                    return false;
                }

            } catch (Exception ex) {
                System.out.println("\nErro no resultset!");
                return false;
            }
        } catch (Exception ex) {
            System.out.println("\nErro no statement!");
            return false;
        }

        return true;
    }

    // Dado o email, retorna a experiencia
    public String op4 (String email) {
        String query = "select exp from " + tabelaUSER + " as u, " + tabelaEXP + " as e, where u.Personid = e.Personid and u.email'=" + email +"';";
        String exps = "Experiencia: \t\t";
        try {
            Statement stm = conn.createStatement();
            try {
                int counter = 1;
                ResultSet rs = stm.executeQuery(query);
                while (rs.next()) {
                    String exp = rs.getString("exp");
                    exps.concat("(" + Integer.toString(counter) + ")" + exp + "\n\t\t");
                    counter += 1;
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

        return exps;

    }

    // Todas as informaçoes de todos os perfis
    public List<String> op5 () {
        String query = "select email from " + tabelaUSER + ";";
        List<String> users = new ArrayList<>();
         try {
            Statement stm = conn.createStatement();
            try {
                ResultSet rs = stm.executeQuery(query);
                while (rs.next()) {
                    String email = rs.getString("email");
                    users.add(op6(email));
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

        return users;

    }

    // Dado o email, retornar as informaçoes
    public String op6 (String email) {
        String query = "select email, nome, sobrenome, residencia, formacao from " + tabelaUSER + " where email='" + email + "';";
        String info = "";
        try {
            Statement stm = conn.createStatement();
            try {
                ResultSet rs = stm.executeQuery(query);
                while (rs.next()) {
                    String nome = rs.getString("nome");
                    String sobrenome = rs.getString("sobrenome");
                    String residencia = rs.getString("residencia");
                    String formacao = rs.getString("formacao");
                    info.concat("Email: " + email + "\nNome: " + nome + " Sobrenome: " + sobrenome + "\nResidencia: " + residencia + "\nFormacao: " + formacao + "\n----------------------------------------\n");
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

        return info;
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

