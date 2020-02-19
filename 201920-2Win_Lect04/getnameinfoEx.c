#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
int main(int argc, char* argv[]) {
    if  (argc < 2)  {
	printf("Give a IP address "
	       "(e.g 8.8.8.8 or 216.220.178.116\n"
	      );
	return(EXIT_SUCCESS);
    }
    struct sockaddr_in sa;
    char node[NI_MAXHOST];
 
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
     
    inet_pton(AF_INET, argv[1], &sa.sin_addr);
    /* google-public-dns-a.google.com */
 
    int res = getnameinfo((struct sockaddr*)&sa, sizeof(sa),
                          node, sizeof(node),
                          NULL, 0, NI_NAMEREQD);
     
    if (res) {
        printf("error: %d\n", res);
        printf("%s\n", gai_strerror(res));
    }
    else
        printf("node=%s\n", node);
     
    return 0;
}
