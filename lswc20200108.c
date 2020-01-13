#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

// This program does: 'ls | wc'
// It works

int main () 
{
	int 	fd[2];
	
	if (pipe(fd) < 0) 
    {
        fprintf(stderr,"Pipe() failed: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    pid_t   lsChild = fork();

    if (lsChild < 0)
    {
        fprintf(stderr,"fork() failed: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

	if (lsChild == 0) 
    {
        close(fd[0]);
        dup2(fd[1],STDOUT_FILENO);
        execl("/bin/ls","ls",NULL);
        fprintf(stderr,"Cannot execl() ls: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    close(fd[1]);

    pid_t   wcChild = fork();

    if (wcChild < 0)
    {
        fprintf(stderr,"fork() failed: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

	if (wcChild == 0) 
    {
        dup2(fd[0],STDIN_FILENO);
        execl("/usr/bin/wc","wc",NULL);
        fprintf(stderr,"Cannot execl() ls: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    close(fd[0]);
    wait(NULL);
    wait(NULL);
    return(EXIT_SUCCESS);
}