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
 
#define	 LINE_LEN			256

#define	 SERVICE_ADDR_SEPARATORY_STR	"://"

#define	 SERVICE_ADDR_SEPARATORY_STR_LEN	\
	 				(sizeof(SERVICE_ADDR_SEPARATORY_STR)-1)

char*	 enterUrlName	(char*	urlName,
	 		 int	urlNameLen
			)
{
  printf("\n"
	 "URL "
  	 "(e.g. ftp://ctilinux1.cstcis.cti.depaul.edu)"
	 " or a blank line to stop\n"
	 ": "
	);
  fgets(urlName,urlNameLen,stdin);

  char*	cPtr	= strchr(urlName,'\n');

  if  (cPtr != NULL)
    *cPtr	= '\0';

  return(urlName);
}


void	parse		(char*		serviceName,
			 int		serviceNameLen,
			 char*		addrName,
			 int		addrNameLen,
			 const char*	urlName
			)
{
  const char*	cPtr;

  for  (cPtr = urlName;  isspace(*cPtr);  cPtr++);

  const char*	separatoryPtr	= strstr(cPtr,SERVICE_ADDR_SEPARATORY_STR);

  if  (separatoryPtr == NULL)
  {
    strncpy(serviceName,"",serviceNameLen);
    strncpy(addrName,cPtr,addrNameLen);
  }
  else
  {
    int	numServiceChars	= separatoryPtr-cPtr;
    strncpy(serviceName,cPtr,numServiceChars);
    serviceName[numServiceChars]	= '\0';
    strncpy(addrName,separatoryPtr+SERVICE_ADDR_SEPARATORY_STR_LEN,addrNameLen);
  }

}


void	describe	(const char*	serviceName,
			 const char*	addrName
			)
{
  struct addrinfo* hostPtr;
  struct addrinfo* run;
  int	status	= getaddrinfo
			(addrName,
			 (serviceName[0] == '\0')
			 ? NULL
			 : serviceName,
			 NULL,
			 &hostPtr
			);

  if  (status != 0)
  {
    fprintf(stderr,gai_strerror(status));
    return;
  }

  for  (run = hostPtr;  run != NULL;  run = run->ai_next)
  {
    char hostname[NI_MAXHOST] = "";
 
    int   error = getnameinfo(run->ai_addr, run->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 

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
    case SOCK_STREAM :  printf(" TCP)\n");    break;
    case SOCK_DGRAM :   printf(" UDP)\n");    break;
    case SOCK_SEQPACKET:printf(" sequenced, reliable packet)\n");  break;
    case SOCK_RAW :     printf(" raw network protocol)\n");  break;
    case SOCK_RDM :     printf(" reliable w/o ordering)\n");  break;
    default :           printf(" unknown protocol?)\n");
    }

  }

  freeaddrinfo(hostPtr);
}


int	main		()
{
  char	urlName[LINE_LEN];

  while  ( *enterUrlName(urlName,LINE_LEN) != '\0' )
  {
    char	serviceName[LINE_LEN];
    char	addressName[LINE_LEN];

    parse(serviceName,LINE_LEN,addressName,LINE_LEN,urlName);
    describe(serviceName,addressName);
  }

  return(EXIT_SUCCESS);
}
