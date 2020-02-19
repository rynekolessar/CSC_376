import java.net.*;
import java.io.*;
import java.util.Scanner;

public class SendTwoInts {

	public static void main(String[] args) {
		int first, second, sum;
		Scanner scan = new Scanner(System.in);

		System.out.println("Please enter an integer: ");
		first = scan.nextInt();

		System.out.println("Please enter another integer: ");
		second = scan.nextInt();

		sum = first + second;

		scan.close();

		try {
			Socket sock = new Socket("localhost", 200001);
			DataOutputStream out = new DataOutputStream(sock.getOutputStream());
			DataInputStream in = new DataInputStream(sock.getInputStream());

			out.writeInt(first);
			out.writeInt(second);
			sum = in.readInt();

			System.out.println(first + " + " + second + " = " + sum);
			sock.close();
		} catch (IOException e) {
		}
	}
}