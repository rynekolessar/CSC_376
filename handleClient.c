#include <stdio.h>
#include <stdlib.h>

const int BUFFER_LEN = 256;
const int DEFAULT_PORT_NUM = 200002;

void handleClient(int fd)
{
    int first, second, sum, numRead;

    numRead = read(fd, &first, sizeof(first));
    numRead = read(fd, &second, sizeof(second));

    sum = htonl(ntohl(first) + ntohl(second)); // Convert to network endianness

    write(fd, &sum, sizeof(sum));

    printf("Sending: %d + %d = %d", first, second, sum);
    close(fd);
}

int sigChildHandler(int sig)
{
    // TODO:
    return (0);
}

int main()
{
    // TODO: Maybe
    return (0);
}