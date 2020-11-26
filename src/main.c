#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "interpreter.h"

int p_interactive;

void
handler(int signum)
{
	return;
}

void
install_handler(void)
{
	struct sigaction setup_action;
	sigset_t block_mask;

	sigemptyset(&block_mask);
	sigaddset(&block_mask, SIGINT);
	sigaddset(&block_mask, SIGTSTP);
	setup_action.sa_handler = handler;
	setup_action.sa_mask = block_mask;
	setup_action.sa_flags = 0;
	sigaction(SIGINT, &setup_action, NULL);
	sigaction(SIGTSTP, &setup_action, NULL);
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
		install_handler();
	return file;
}

int
main(int argc, char *argv[])
{
	FILE *file;

	file = init(argc, argv);
	routine(file);
	fclose(file);
	return 0;
}
