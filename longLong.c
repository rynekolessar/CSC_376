#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <endian.h>

/**
* 	Program from 2020-01-15 in class
*/

unsigned long long ourhtnll (unsigned long long ull)
{
	int seven		   = 7;
	int isLittleEndian = (*(char*)&seven == 0x07);

	if (isLittleEndian)
	{
		unsigned int* ptr = (unsigned int*)&ull;
		unsigned int  tmp = htonl(ptr[0]);
		
		ptr[0] = htonl(ptr[1]);
		ptr[1] = htonl(tmp);
	}

	return(ull);
}

int main() 
{
	// unfinished	
}