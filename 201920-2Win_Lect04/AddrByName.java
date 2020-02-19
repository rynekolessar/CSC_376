import	java.net.*;

public class AddrByName
{
  public static void main	(String[]	args)
  {
    if  (args.length < 1)
    {
      System.err.println("Usage: java AddrByName <URL>");
      return;
    }

    try
    {
      InetAddress	address = InetAddress.getByName(args[0]);
      System.out.println(address);
    }
    catch  (UnknownHostException ex)
    {
      System.out.println("Could not find " + args[0]);
    }
  }
}
