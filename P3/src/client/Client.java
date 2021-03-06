package client;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import compute.Projeto;
import java.util.Scanner;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.rmi.RemoteException;
import java.util.concurrent.TimeUnit;
import java.io.PrintWriter;
import java.io.File;
import java.io.FileWriter;

public class Client {
    Projeto servidor = null;
    Registry registry = null;

    public Client() {
      try {
          String name = "Projeto";
          this.registry = LocateRegistry.getRegistry("177.220.85.173", 1337); //pegando localhost, necessário passar URL em null para comunicação entre 2 hosts

          this.servidor = (Projeto) registry.lookup(name);
      } catch (Exception e) {
          e.printStackTrace();
      }
    }

    public String requisitar(int req, String par1, String par2) throws RemoteException{
        //realizar a requisição e obter o retorno
	System.out.println(this.servidor);
        return this.servidor.executeTask(req, par1, par2);
    }

    public static void main(String args[]) throws IOException {

        Client cliente = new Client();
        System.out.println("Seja bem vindo ao sistema do cliente. Aqui voce podera realizar as seguintes operacoes: \n");
        System.out.println("1. Listar todas as pessoas formadas em um determinado curso;\n");
        System.out.println("2. Listar as habilidades dos perfis que moram em uma determinada cidade;\n");
        System.out.println("3. Acrescentar uma nova experiencia em um perfil;\n");
        System.out.println("4. Dado o email do perfil, retornar suas experiencias;\n");
        System.out.println("5. Listar todas as informacoes de todos os perfis;\n");
        System.out.println("6. Dado o email de um perfil, retornar suas informacoes;\n");

	//realizar a leitura da entrada do cliente
        String resultado = null;
        Scanner in = new Scanner(System.in);
        BufferedReader reader =
                new BufferedReader(new InputStreamReader(System.in));
        System.out.println("Escolha uma requisição: ");
        String req_ = in.nextLine();
	int req = Integer.parseInt(req_);
        long startTime = System.nanoTime();
        if (req==1){
            System.out.println("Qual curso: ");
	    String par1 = in.nextLine();
            resultado = cliente.requisitar(req, par1, null);
        }
        else if (req==2){
            System.out.println("Qual cidade: ");
	    String par1 = in.nextLine();
            resultado = cliente.requisitar(req, par1, null);
        }
        else if (req==3){
            System.out.println("Qual experiencia: ");
	    String par1 = in.nextLine();
            System.out.println("Qual email: ");
	    String par2 = in.nextLine();
            resultado = cliente.requisitar(req, par1, par2);
        }

        else if (req==4){
            System.out.println("Qual email: ");
	    String par1 = in.nextLine();
            resultado = cliente.requisitar(req, par1, null);
        }

        else if (req==5) {
            resultado = cliente.requisitar(req, null, null);
        }
        else if (req==6){
            System.out.println("Qual email: ");
	    String par1 = in.nextLine();
            resultado = cliente.requisitar(req, par1, null);
        }
        else{
            resultado = "Requisição Inválida";
        }

        long endTime = System.nanoTime();
        long timeElapsed = endTime - startTime;

        PrintWriter pw = null;

        try {
           File file = new File("client_time_" + Long.toString(req) + ".txt");
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

        System.out.println(resultado);
    }
}
