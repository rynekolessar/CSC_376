import	java.net.*;

public class DePaulByName
{
  public static void main	(String[]	args)
  {
    try
    {
      InetAddress	address
			= InetAddress.getByName("www.depaul.edu");
      System.out.println(address);

      address		= InetAddress.getByName("216.220.178.116");
      System.out.println(address.getHostName());
    }
    catch  (UnknownHostException ex)
    {
      System.out.println("Could not find www.depaul.edu");
    }
  }
}
