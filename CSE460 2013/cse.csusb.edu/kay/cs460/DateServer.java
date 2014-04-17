import java.net.*;
import java.io.*;

public class DateServer
{
    public static void main(String[] args) {
        try {
            ServerSocket sock = new ServerSocket(6013);

            // listen for connection
            while (true) {
                Socket client = sock.accept(); // blocks for a connection

                // create a formatted print object: PrintWriter(stream, autoflush)
                // autoflush == true means println() forces send
                PrintWriter pout = new PrintWriter(client.getOutputStream(), true);

                // write date to socket
                pout.println(new java.util.Date().toString());

                client.close();
            }
        }
        catch (IOException ioe) {
            System.err.println(ioe);
        }
    }
}
