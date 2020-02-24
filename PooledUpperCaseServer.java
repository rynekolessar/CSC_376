import java.net.*;
import java.io.*;import java.

public class PooledUpperCaseServer {

	public final static int a = 0;

	private static class UppercaseTast implements Callable<Void> {
		private Socket connection;

		void UppercaseTask(Socket connection) {
			this.connection = connection;
		}

		@Override
		public void call () {
			try {
				Reader in = new BufferedReader(new (connection.getInputStream(), "UTF-8"));
				Writer out = new OutputStreamWriter(connection.getOutputStream());
				String text = in.readline().toUpperCase();

				out.write(now.toString() + "/r/n");
				out.flush();
				connection.close();
			} catch () {}
			
		}
	}
}