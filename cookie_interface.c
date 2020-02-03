// Modified from cookie_interface.c
/***************************************************************************
* 					_ _       ___   _
* Project 			___| | | |  _\ | |
* 				   / __| | | | |_) | |
*				  | (__| |_| | _ < | |___
* 				   \___|\___/|_| \_\_____|
*
* Copyright (C) 1998 - 2018, Daniel Stenberg, <daniel@haxx.se>, et al.
*
* This software is licensed as described in the file COPYING, which
* you should have received as part of this distribution. The terms
* are also available at https://curl.haxx.se/docs/copyright.html.
*
* You may opt to use, copy, modify, merge, publish, distribute and/or sell
* copies of the Software, and permit persons to whom the Software is
* furnished to do so, under the terms of the COPYING file.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
ANY
* KIND, either express or implied.
*
***************************************************************************/
/* <DESC>
* Import and export cookies with COOKIELIST.
* </DESC>
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <curl/curl.h>

#define VERBOSE_MODE0 "-v"
#define VERBOSE_MODE1 "--verbose"

const int MAX_LEN = 4096;


void pressEnter ()
{
 	printf("Press \"Enter\" to continue:");
 	while (fgetc(stdin) != '\n');
}


struct MemoryStruct {
 	char *memory;
 	size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
 	size_t realsize = size * nmemb;
 	struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 	char *ptr = realloc(mem->memory, mem->size + realsize + 1);
 	if(ptr == NULL) {
 		/* out of memory! */
 		printf("not enough memory (realloc returned NULL)\n");
 		return 0;
 	}
 	mem->memory = ptr;
 	memcpy(&(mem->memory[mem->size]), contents, realsize);
 	mem->size += realsize;
 	mem->memory[mem->size] = 0;
 	return realsize;
}
static void print_cookies(CURL *curl)
{
 	 CURLcode res;
 	 struct curl_slist *cookies;
 	 struct curl_slist *nc;
 	 int i;
 	 char domain[MAX_LEN];
 	 char isAccessible[10];
 	 char path[MAX_LEN];
 	 char isSecure[10];
 	 time_t expiration;
 	 char name[MAX_LEN];
 	 char value[MAX_LEN];
 	 printf("Cookies, curl knows:\n");
 	 res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
 	 if(res != CURLE_OK) {
 	 	fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
 	 	curl_easy_strerror(res));
 	 	exit(1);
 	 }
 	 nc = cookies;
 	 i = 1;
 	 while(nc) {
 	  	domain[0] = isAccessible[0] = path[0] = isSecure[0] =
 	  	name[0] = value[0] = '\0';
 	  	expiration = 0;
 	  	sscanf(nc->data,"%s %s %s %s %ld %s %s",domain,isAccessible,path,isSecure,&expiration,name,value);
 	  	printf("[%d]\tdomain:\t\t%s\n""\tisAccessible:\t%s\n\tpath:\t\t%s\n\t"
 	  	"isSecure:\t%s\n\texpiration:\t%s"
 	  	"\tname:\t\t%s\n\tvalue:\t\t%s\n\n",
 	  	i,domain,isAccessible,path,
 	  	isSecure,ctime(&expiration),
 	  	name,value
 	  	);
 	  	nc = nc->next;
 	  	i++;
 	 }
 	 if(i == 1) {
 	  	printf("(none)\n");
 	 }
 	 curl_slist_free_all(cookies);
}
void showUsage ()
{
	fprintf(stderr,
 	"Usage:\tcookie_interface <URL> [--verbose]\n"
 	"Where:\n"
 	"<URL>:\t\tWhich site to investigate\n"
 	"--verbose:\tPrint header and other communication with server\n"
 	);
}
