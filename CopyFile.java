import java.io.OutputStream;
import java.io.InputStream;


public static void main (String[] args) 
{
    if (args.length < 2)
    {
        System.err.println("Usage: java CopyFile <source> <class>\n");
        return;
    }

    String source = args[0];
    String dest = args[1];

    try 
    {
        FileInputStream input = new FileInputStream(source);
        FileOutputStream output = new FileOutputStream(dest);

        final int BUFFER_LEN = 4096;

        byte [ ] buffer = new byte[BUFFER_LEN];

        int numBytes;

        while ( (numBytes = input.read(buffer,0,BUFFER_LEN)) > 0)
        {
            // incomplete
        }
        //inomplete
    }

    // incomplete
}