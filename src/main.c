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

FILE *
init(int argc, char *argv[])
{
	FILE *file;

	if (argc > 2)
		errx(EXIT_FAILURE, "too many arguments");
	else if (argc == 2) {
		if ((file = fopen(argv[1], "r")) == NULL)
			err(EXIT_FAILURE, "fopen");
		p_interactive = 0;
	} else if (argc == 1) {
		file = stdin;
		p_interactive = isatty(fileno(file));
		if (!p_interactive)
			err(1, "wrong usage");
	}
	if (p_interactive)
		signal(SIGINT, sigint_handler);
	return file;
}

int
main(int argc, char *argv[])
{
	FILE *file;

	file = init(argc, argv);
	routine(file);
	return 0;
}
