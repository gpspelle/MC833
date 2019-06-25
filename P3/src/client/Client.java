package client;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import compute.Interface;
import java.util.Scanner;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.rmi.RemoteException;

public class Client {
    public String requisitar(int req, String par1, String par2, Interface servidor) throws RemoteException{
        //realizar a requisição e obter o retorno
        String resultado=servidor.executeTask(req, par1, par2);
        return resultado;
    }

    public String leitura(Interface servidor) throws IOException{
        //realizar a leitura da entrada do cliente
        String resultado;
        Scanner in = new Scanner(System.in);
        BufferedReader reader =
                new BufferedReader(new InputStreamReader(System.in));
        System.out.println("Escolha uma requisição: ");
        int req = in.nextInt();
        if (req==1){
            System.out.println("Qual curso: ");
            String par1 = reader.readLine();
            System.out.println(req);
            System.out.println(par1);
            System.out.println("Chamar função 1");
            resultado = requisitar(req, par1, null, servidor);
        }
        else if (req==2){
            System.out.println("Qual cidade: ");
            String par1 = reader.readLine();
            System.out.println(req);
            System.out.println(par1);
            System.out.println("Chamar função 2");
            resultado = requisitar(req, par1, null, servidor);
        }
        else if (req==3){
            System.out.println("Qual experiencia: ");
            String par1 = reader.readLine();
            System.out.println("Qual email: ");
            String par2 = reader.readLine();
            System.out.println(req);
            System.out.println(par1);
            System.out.println(par2);
            System.out.println("Chamar função 3");
            resultado = requisitar(req, par1, par2, servidor);
        }

        else if (req==4){
            System.out.println("Qual email: ");
            String par1 = reader.readLine();
            System.out.println(req);
            System.out.println(par1);
            System.out.println("Chamar função 4");
            resultado = requisitar(req, par1, null, servidor);
        }

        else if (req==5) {
            System.out.println("Chamar função 5");
            resultado = requisitar(req, null, null, servidor);
        }
        else if (req==6){
            System.out.println("Qual email: ");
            String par1 = reader.readLine();
            System.out.println(req);
            System.out.println(par1);
            System.out.println("Chamar função 6");
            resultado = requisitar(req, par1, null, servidor);
        }
        else{
            resultado = "Requisição Inválida";
        }
        return resultado;
    }

    public static void main(String args[]) throws IOException {
        Interface servidor = null;

        try {
            String name = "Interface";
            Registry registry = LocateRegistry.getRegistry(null, 9999); //pegando localhost, necessário passar URL em null para comunicação entre 2 hosts
            servidor = (Interface) registry.lookup(name);
        } catch (Exception e) {
            System.err.println("Client exception:");
            e.printStackTrace();
        }

        Client cliente = new Client();
        System.out.println("Seja bem vindo ao sistema do cliente. Aqui voce podera realizar as seguintes operacoes: \n");
        System.out.println("1. Listar todas as pessoas formadas em um determinado curso;\n");
        System.out.println("2. Listar as habilidades dos perfis que moram em uma determinada cidade;\n");
        System.out.println("3. Acrescentar uma nova experiencia em um perfil;\n");
        System.out.println("4. Dado o email do perfil, retornar suas experiencias;\n");
        System.out.println("5. Listar todas as informacoes de todos os perfis;\n");
        System.out.println("6. Dado o email de um perfil, retornar suas informacoes;\n");
        String resultado=cliente.leitura(servidor);
        System.out.println(resultado);
    }
}
