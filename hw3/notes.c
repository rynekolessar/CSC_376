#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define COUNT_FILEPATH "output.txt"

const int TEXT_LEN = 4096;

/**
 * C wrapper for java or python
 */

pid_t launchHunspell(int *toHunspell, int *toUrlVisitor)
{
	pid_t childId = fork();

	if (childId < 0)
	{
		// Complain
	}

	if (childId == 0)
	{
		close(toHunspell[1]);
		dup2(toHunspell[0], STDIN_FILENO);

		close(toUrlVisitor[0]);
		dup2(toUrlVisitor[1], STDOUT_FILENO);

		execl("usr/bin/hunspell", "Hunspell", "-a", "-d", "en_US", NULL);
		fprintf(stderr, "Cannot find /usr/bin/hunspell\n");
		exit(EXIT_FAILURE);
	}

	close(toHunspell[0]);
	close(toUrlVisitor[0]);

	return (childId);
}

// toHunspell == &toHunspell[0]

pid_t launchUrlVisitor(int *toHunspell, int *toUrlVisitor)
{
	pid_t childId = fork();

	if (childId < 0)
	{
		// Complain
	}
	if (childId == 0)
	{
		dup2(toHunspell[1], STDOUT_FILENO);
		dup2(toUrlVisitor[0], STDIN_FILENO);
		execl("usr/bin/pythonyJava", "pythonyJava", arguments, NULL);
		fprintf(stderr, "Cannot find .l\n");
		exit(EXIT_FAILURE);
	}

	close(toHunspell[1]);
	close(toUrlVisitor[0]);

	return (childId);
}

int main()
{
	int toHunspell[2];
	int toUrlVisitor[2];

	if ((pipe(toHunspell) < 0) || (pipe(toUrlVisitor) < 0))
	{
		fprintf(stderr, "pipe() error %s\n", strerrno(errno));
		exit(EXIT_FAILURE);
	}

	pid_t hunspellId = launchHunspell(toHunspell, toUrlVisitor);
	pid_t toUrlVisitorId = launchHunspell(toHunspell, toUrlVisitor);

	wait(NULL);
	kill(hunspellId, SIGINT);
	wait(NULL);

	char text[TEXT_LEN];
	int numBytes;
	int fileD = open(COUNT_FILEPATH, O_RDONLY);

	if (fileD < 0)
	{
		fprintf(stderr, "That dumb-ass pythonJava did not give my a file!\n");
		exit(EXIT_FAILURE);
	}
	while ((numBytes = read(fileD, text, TEXT_LEN)) > 0)
	{
		text[numBytes] = '\0';
		printf("%s", text);
	}

	close(fileD);
	return (EXIT_SUCCESS);
}
