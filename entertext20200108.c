#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// Kinda like a multithreaded program

const int BUFFER_LEN = 256;

int main () 
{
    int fd[2];

    pipe2(fd,0_NONBLOCK);

    if(fork() == 0)
    {
        while (1)
        {
            char buffer[BUFFER_LEN];

            sleep(1);
            int numBytes = read(fd[0],buffer,BUFFER_LEN);

            if (numBytes > 0) 
            {
                printf("%s\n",buffer);
            }
            else 
            {
                printf("nothing yet\n");
            }
            
        }
    }

    while(1)
    {
        printf("Enter some text: ");
        fgets(buffer,BUFFER_LEN,stdin);

    }
}