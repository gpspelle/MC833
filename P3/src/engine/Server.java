package engine;

import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import compute.Projeto;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;
import java.io.PrintWriter;
import java.io.File;
import java.io.FileWriter;

public class Server implements Projeto {
    public Database banco;

    public Server() {
        super();
        this.banco = null;
    }

    public String executeTask(int req, String par1, String par2) {
        //realizar consulta no banco de dados
        String resultado = "";

        long startTime = System.nanoTime();
        if (req==1){
            List<String> lista = banco.op1(par1);
            for (String i : lista) {
                resultado = resultado + i;
            }
        }
        else if (req==2){
            List<String> lista = banco.op2(par1);
            for (String i : lista) {
                resultado = resultado + i;
            }
        }
        else if (req==3){
            boolean success = banco.op3(par1, par2);
            if (success = true) {
                resultado = "Sucesso";
            }
            else {
                resultado = "Falha";
            }
        }

        else if (req==4){
            resultado = banco.op4(par1);
        }

        else if (req==5) {
            List<String> lista = banco.op5();
            for (String i : lista) {
                resultado = resultado + i;
            }
        }

        else if (req==6){
            resultado = banco.op6(par1);
        }

        long endTime = System.nanoTime();
        long timeElapsed = endTime - startTime;

        PrintWriter pw = null;

        try {
           File file = new File("server_time_" + Long.toString(req) + ".txt");
           FileWriter fw = new FileWriter(file, true);
           pw = new PrintWriter(fw);
           pw.println(Long.toString(timeElapsed));
        } catch (Exception e) {
           e.printStackTrace();
        } finally {
           if (pw != null) {
              pw.close();
           }
        }

        return resultado;
    }

    public static String getIP() throws UnknownHostException{
      DatagramSocket socket = null;
      try {
        socket = new DatagramSocket();
        socket.connect(InetAddress.getByName("8.8.8.8"), 10002);
        return socket.getLocalAddress().getHostAddress();
      } catch(Exception ex) {
        return InetAddress.getLocalHost().getHostAddress();
      }
    }
    public static void main(String[] args) {

        String url = "jdbc:mysql://localhost:3306/MC833";
        String login = "root";
        String senha = "123";

        try {
            String name = "Projeto";
            Projeto engine = new Server();
            ((Server) engine).banco = new Database(url, login, senha);
            ((Server) engine).banco.connect();
            Projeto stub = (Projeto) UnicastRemoteObject.exportObject(engine, 0);

            System.setProperty("java.rmi.server.hostname", getIP());

            Registry registry = LocateRegistry.createRegistry(1337);
            registry.rebind(name, stub);
            System.out.println("Servidor up");
        } catch (Exception e) {
            System.err.println("Server exception:");
            e.printStackTrace();

        }
    }
}
