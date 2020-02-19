import java.net.*;

public class MyAddress
{
  public static void main (String[] args)
  {
    try
    {
      InetAddress address
			= InetAddress.getLocalHost();
      String	  dottedQuad
			= address.getHostAddress();
      System.out.println("Name: " + address);
      System.out.println("IP: " + dottedQuad);
    }
    catch  (UnknownHostException ex)
    {
      System.out.println("Could not find this computer's address.");
    }
  }
}
