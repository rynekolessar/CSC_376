import	java.net.*;

public class ProtocolTester
{
  public static void main (String[] args)
  {
    if  (args.length < 1)
    {
      System.out.println("Give a URL with a protocol on the command line, e.g.");
      System.out.println("http://www.wherever.com");
      System.out.println("https://www.wherever.com");
      System.out.println("ftp://www.wherever.com");
      System.out.println("mailto:me@wherever.com");
      System.out.println("telnet://www.wherever.com");
      System.out.println("file:///www.wherever.com");
      System.out.println("gopher://www.wherever.com");
      System.out.println("ldap://www.wherever.com");
      System.out.println("jar://www.wherever.com");
    }
    else
      testProtocol(args[0]);
  }

  private static void testProtocol(String url)
  {
    try
    {
      URL u = new URL(url);

      System.out.println(u.getProtocol() + " is supported");
    }
    catch (MalformedURLException ex)
    {
      String protocol = url.substring(0,url.indexOf(':'));

      System.out.println(protocol + " is not supported");
    }
  }
}
