public class main {

    public static void main(String[] args) {

        String url = "jdbc:mysql://localhost:3306/MC833";
        String login = "root";
        String senha = "123";
        database Db = new database(url, login, senha);
        Db.connect();
        Db.get_names();
    }
}
