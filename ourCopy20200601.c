#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

// Some stuff is missing here

int main (int argc, char* argv[]) 
{
	if (argc < 3) 
    {
		fprintf(stderr,"Usage: ourCopy <sourceFile> <destFile>\n");
		exit(EXIT_FAILURE);
	}

	const char* sourceFilename  = argv[1];
	const char* destFilename    = argv[2];

    int sourceFd = open(sourceFilename,0_RDONLY);

    if (sourceFd < 0) 
    {
        fprintf(stderr,"ERROR: Cannot open %s\n",sourceFilename);
        exit(EXIT_FAILURE);
    }

    int destFd = creat(destFilename,0660);

    if (destFd < 0) 
    {
        fprintf(stderr,"ERROR: Cannot open %s\n",destFilename);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_LEN];
    int numBytes;

    while (numBytes = read(sourceFd,buffer,BUFFER_LEN) > 0) 
    {
        write(destFd,buffer,numBytes);
    }

    close(destFd);
    close(sourceFd);

    return(EXIT_SUCCESS);
}