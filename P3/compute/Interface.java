package compute;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface Interface extends Remote {
    public String executeTask(int req, String par1, String par2) throws RemoteException;
}