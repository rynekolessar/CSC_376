import java.io.*;
import java.net.*;
import java.util.regex.*;

class	FinalProjTester
{

  public static String getPage	(URL	url)
  {
    String	toReturn	= "";
    InputStream	in		= null;

    try
    {

      in = url.openStream();

      // buffer the input to increase performance
      in = new BufferedInputStream(in);

      // chain the InputStream to a Reader
      Reader r = new InputStreamReader(in);
      int    c;

      while ((c = r.read()) != -1 )
      {
	toReturn += (char)c;
      }
    }
    catch (IOException ex)
    {
      System.err.println(ex);
    }
    finally
    {
      if (in != null)
      {
	try
	{
	  in.close();
	}
	catch (IOException e)
	{
	  // ignore
	}
      }
    }

    return(toReturn);
  }

  public static void main (String args[])
  {
    if (args.length < 1)
    {
      System.err.println("Usage:\tjava FinalProjTester <url>\n");
      System.exit(1);
    }

    String	urlString	= args[0];

    try
    {
      String		toEvaluate;
      BufferedReader	reader	= new BufferedReader
					(new InputStreamReader(System.in));
      QueryString	query	= new QueryString();

      System.out.print("toEvaluate: ");
      toEvaluate	= reader.readLine();

      query.add("toEvaluate",toEvaluate);

      URL		url	= new URL(urlString);
      String		protocol= url.getProtocol();
      String		host	= url.getHost();
      String		path	= "symEval.swt";
      int		port	= url.getPort();

      url			= new URL(protocol,
					  host,
					  port,
					  path + "?" + query
					 );
      String		text	= getPage(url);
      PrintWriter	out	= new PrintWriter("downloaded.html");

      System.out.println(urlString);
      System.out.println(text);
      out.println(text);
      out.close();
    }
    catch (MalformedURLException ex)
    {
      System.err.println(args[0] + " is not a parseable URL");
    }
    catch (IOException ex)
    {
      System.err.println("IOException: " + ex);
    }
  }

}
