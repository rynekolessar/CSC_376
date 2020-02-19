package hw2;

/**
 * Distributed Systems
 * Homework 2
 * 2020-02-11
 * Ryne Kolessar
 */

import java.io.*;
import java.net.URL;
import java.util.Scanner;

public class DownloadWebpage {

	public static void downloadWebpage(String website) {
		try {
			URL url = new URL(website);

			BufferedReader in = new BufferedReader(new InputStreamReader(url.openStream()));

			BufferedWriter out = new BufferedWriter(new FileWriter("page.html"));

			String line;

			while ((line = in.readLine()) != null) {
				out.write(line);
			}

			out.close();
			in.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void main(String args[]) throws IOException {
		Scanner in = new Scanner(System.in);
		System.out.println("Enter URL: ");
		String url = in.nextLine();
		downloadWebpage(url);
		in.close();
	}
}