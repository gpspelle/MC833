package engine;

import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import compute.Interface;
import java.util.ArrayList;

public class Server implements Interface {

    public Server() {

        String url = "jdbc:mysql://localhost:3306/MC833";
        String login = "root";
        String senha = "123";
        this.database banco = new database(url, login, senha);
        this.banco.connect();
        super();
    }

    public String executeTask(int req, String par1, String par2) {
        //realizar consulta no banco de dados
        String resultado = null;
        if (req==1){
            List<String> lista = banco.op1(par1);
            for (String i : lista) {
                resultado = resultado + i;
            }
            resultado = "recebi req 1";
            resultado = resultado + par1;
        }
        else if (req==2){
            List<String> lista = banco.op2(par1);
            for (String i : lista) {
                resultado = resultado + i;
            }
            resultado = "recebi req 2";
        }
        else if (req==3){
            boolean success = banco.op3(par1, par2);
            if (success = true) {
                resultado = "Sucesso";
            }
            else {
                resultado = "Falha";
            }
            resultado = "recebi req 3";
        }

        else if (req==4){
            resultado = banco.op4(par1);
            resultado = "recebi req 4";
        }

        else if (req==5) {
            List<String> lista = banco.op5();
            for (String i : lista) {
                resultado = resultado + i;
            }
            resultado = "recebi req 5";
        }

        else if (req==6){
            resultado = banco.op6(par1);
            resultado = "recebi req 6";
        }
        return resultado;
    }

    public static void main(String[] args) {

        try {
            String name = "Interface";
            Interface engine = new Server();
            Interface stub =
                    (Interface) UnicastRemoteObject.exportObject(engine, 0);
            Registry registry = LocateRegistry.createRegistry(9999);
            registry.rebind(name, stub);
            System.out.println("Servidor up");
        } catch (Exception e) {
            System.err.println("Server exception:");
            e.printStackTrace();
        }
    }
}
