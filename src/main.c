#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "interpreter.h"

int p_interactive;

void
sigint_handler(int signum)
{
	return;
}

void init(int argc, char *argv[])
{
	if (argc > 2)
		errx(EXIT_FAILURE, "too many arguments");
	else if (argc == 2)
		if (freopen(argv[1], "r", stdin) == NULL)
			err(EXIT_FAILURE, "freopen");
	p_interactive = isatty(fileno(stdin));
	if (p_interactive)
		signal(SIGINT, sigint_handler);
}

int
main(int argc, char *argv[])
{
	init(argc, argv);
	routine();
	return 0;
}
