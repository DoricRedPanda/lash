#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <stdlib.h>

#include "util.h"
#include "builtin.h"

static char cwd[PATH_MAX + 1];
static char owd[PATH_MAX + 1];

int
builtin(char *argv[])
{
	static Builtin builtins[] = {
		{ "cd",     &builtin_cd },
		{ "exit",   &builtin_exit },
	};

	if (!argv)
		return 0;
	for (unsigned long i = 0; i < LEN(builtins); i++) {
		if (!strcmp(builtins[i].name, argv[0])) {
			(*builtins[i].func)(argv);
			return 1;
		}
	}
	return 0;
}

void
builtin_cd(char *argv[])
{
	char *dir = argv[1];
	int isowd = 0;

	if (!dir) {
		dir = getenv("HOME");
		if (!dir)
			warn("getenv HOME %s", argv[0]);
	} else if (strcmp(dir, "-") == 0) {
		dir = getenv("OWD");
		if (!dir)
			warn("getenv OWD %s", argv[0]);
		isowd = 1;
	}
	getcwd(owd, PATH_MAX);
	if (chdir(dir)) {
		warn("chdir");
	} else {
		getcwd(cwd, PATH_MAX);
		setenv("PWD", cwd, 1);
		setenv("OWD", owd, 1);
		if (isowd)
			puts(dir);
	}
}

void
builtin_exit(char *argv[])
{
	if (!argv[1])
		exit(EXIT_SUCCESS);
	else
		exit(atoi(argv[1]));
}
