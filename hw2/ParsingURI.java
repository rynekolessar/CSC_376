package hw2;

/**
 * Distributed Systems
 * Homework 2
 * 2020-02-11
 * Ryne Kolessar
 */

import java.net.*;
import java.util.*;

class ParsingURI {

    public static void main(String args[]) {
        Scanner getUriScanner = new Scanner(System.in);
        System.out.println("Enter URL: ");

        String enteredURI = getUriScanner.nextLine();

        if (!(enteredURI.contains("http") || enteredURI.contains("https"))) {
            enteredURI = "http://" + enteredURI;
        }
        try {
            URL websiteUrl = new URL(enteredURI);

            System.out.println("protocol    = " + websiteUrl.getProtocol());
            System.out.println("userinfo    = " + websiteUrl.getAuthority());
            System.out.println("host        = " + websiteUrl.getHost());
            System.out.println("port        = " + websiteUrl.getPort());
            System.out.println("path        = " + websiteUrl.getPath());
            System.out.println("query       = " + websiteUrl.getQuery());
        } catch (MalformedURLException ex) {
            System.out.println("Please enter a valid URL");

        }
        getUriScanner.close();

    }
}