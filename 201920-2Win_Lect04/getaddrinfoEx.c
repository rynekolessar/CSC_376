#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#ifndef   NI_MAXHOST
#define   NI_MAXHOST 1025
#endif
 
void	describe	(const char*	nodeNamePtr
			)
{
  struct addrinfo*	hostPtr;
  struct addrinfo*	run;
  int			status	= getaddrinfo
					(nodeNamePtr,
					 NULL,
					 NULL,
					 &hostPtr);

  if  (status != 0)
  {
    fprintf(stderr,gai_strerror(status));
    return;
  }

  for  (run = hostPtr;  run != NULL;  run = run->ai_next)
  {
    struct in_addr	*addr;
    char		hostname[NI_MAXHOST] = "";
    char		ipstr[INET_ADDRSTRLEN];
    int			error = getnameinfo
				(run->ai_addr,
				 run->ai_addrlen,
				 hostname,
				 NI_MAXHOST,
				 NULL,
				 0,
				 0); 

    if (error != 0)
    {
      fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(error));
      continue;
    }

    if (*hostname == '\0')
      printf("%-32s:",run->ai_canonname);
    else
      printf("%-32s:", hostname);

    switch  (run->ai_family)
    {
    case AF_INET :    printf("  (IPv4,"); break;
    case AF_INET6 :   printf("  (IPv6,"); break;
    case AF_UNSPEC :  printf("  (IPv4 & IPv6,"); break;
    case AF_UNIX :    printf("  (local Unix,");  break;
    case AF_IPX :     printf("  (Novell,");      break;
    case AF_APPLETALK:printf("  (Appletalk,");  break;
    case AF_PACKET:   printf("  (Lo-level packet,"); break;
    default :         printf("  (Unknown family?,");
    }

    switch  (run->ai_socktype)
    {
    case SOCK_STREAM :  printf(" TCP)");    break;
    case SOCK_DGRAM :   printf(" UDP)");    break;
    case SOCK_SEQPACKET:printf(" sequenced, reliable packet)");  break;
    case SOCK_RAW :     printf(" raw network protocol)");  break;
    case SOCK_RDM :     printf(" reliable w/o ordering)");  break;
    default :           printf(" unknown protocol?)");
    }

    fputc('\n',stdout);

    if  (run->ai_family == AF_INET)
    {
      struct sockaddr_in* ipv4	= (struct sockaddr_in*)run->ai_addr;
      addr			= &(ipv4->sin_addr);
      inet_ntop(run->ai_family,&(ipv4->sin_addr),ipstr,sizeof(ipstr));
    }
    else
    {
      struct sockaddr_in6* ipv6	= (struct sockaddr_in6*)run->ai_addr;
      addr			= (struct in_addr*)&(ipv6->sin6_addr);
      inet_ntop(run->ai_family,&(ipv6->sin6_addr),ipstr,sizeof(ipstr));
    }


    if  (ipstr[0] != '\0')
      printf("%s\n\n",ipstr);
    else
      printf("\n");
  }

  freeaddrinfo(hostPtr);
}


int	main		(int argc, char* argv[])
{
  if  (argc < 2)
  {
    fprintf(stderr,"Usage: getAllByName <url>\n");
    exit(EXIT_FAILURE);
  }

  describe(argv[1]);
  return(EXIT_SUCCESS);
}
