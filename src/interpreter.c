#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include "tokenizer.h"


static void
printBar(void)
{
	char *user = getenv("USER");

	putchar(strcmp(user, "root") ? '$' : '#');
	putchar(' ');
}

pid_t
execute(char **token)
{
	pid_t pid;

	pid = fork();
	if (pid < 0) {
		perror("fork failed");
		pid = -1;
	} else if (pid == 0) {
		execvp(token[0], token);
		/*unreachable*/
		err(1, "%s", token[0]);
	}
	return pid;
}

void
routine(void)
{
	char **token;

	for (;;) {
		printBar();
		if (!readToken(&token, NULL, NULL))
			continue;
		execute(token);
		freeToken(token);
		wait(NULL);
	}
}


