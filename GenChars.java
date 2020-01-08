// From Elliote Rusty Harold
// "Java Network Programming"
// O'Reilly, 2014

import java.io.OutputStream;
import java.io.IOException;

public class GenChars 
{
    public static void generateChars (OutputStream out) throws IOException 
    {
        int firstPrintChar      = 33;
        int numPrintChars       = 94;
        int numCharsPerLine     = 72;
        int start               = firstPrintChar;

        byte[] line = new byte[numCharsPerLine + 2];
        
        while (true) 
        {
            for (int i = start; i < start + numCharsPerLine; i++) 
            {
                line[i-start] = (byte)((i-firstPrintChar) % numPrintChars +firstPrintChar);
            }

            line[numCharsPerLine+0] = (byte)'\r';
            line[numCharsPerLine+1] = (byte)'\n';

            out.write(line);
            start = ((start+1) - firstPrintChar) % numPrintChars + firstPrintChar;
        }
    }
    public static void main(String[] args) {
        try {
            generateChars(System.out);
        }
        catch (IOException except) {}
    }
}