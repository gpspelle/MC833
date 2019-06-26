package engine;

import java.rmi.*;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import compute.Interface;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;
import java.io.PrintWriter;
import java.io.File;
import java.io.FileWriter;

public class Server implements Interface {
    private Database banco;

    public Server() {
        super();
        String url = "jdbc:mysql://localhost:3306/MC833";
        String login = "root";
        String senha = "123";
        this.banco = new Database(url, login, senha);
        this.banco.connect();
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

    public static void main(String[] args) {

        Interface engine = new Server();
        int port = 9999;
        String name = "Interface";

        //Interface stub =
        //        (Interface) UnicastRemoteObject.exportObject(engine, 0);

        try {
            LocateRegistry.createRegistry(9999);
            System.out.println("Servidor up");
        } catch (Exception e) {
            System.err.println("Server exception");
        }
        
        String hostname = "177.220.85.232";    
	System.setProperty("java.rmi.server.hostname", hostname);

        String bindLocation = "//" + hostname + ":" + port + name;

        try { 
            Naming.bind(bindLocation, engine);
            // in case  of failure use
            // Naming.bind(bindLocation, stub)
            System.out.println("Server is ready at: " + bindLocation);
        } catch (Exception e) {
            System.out.println("Server failed: " + e);
        }
        
        //registry.rebind(name, stub);
    }
}
