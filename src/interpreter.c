#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include "util.h"
#include "tokenizer.h"
#include "ast.h"

static void
printBar(void)
{
	char *user = getenv("USER");

	putchar(strcmp(user, "root") ? '$' : '#');
	putchar(' ');
}

void
redirectFiles(char *input, char *output)
{
	int fd;

	if (input) {
		fd = sopen(input, 0);
		sdup2(fd, 0);
		sclose(fd);
	}
	if (output) {
		fd = sopen(output, 1);
		sdup2(fd, 1);
		sclose(fd);
	}
}

pid_t
execute(struct AST *ast)
{
	pid_t pid;

	if (!ast)
		return -1;
	pid = fork();
	if (pid < 0) {
		perror("fork failed");
		pid = -1;
	} else if (pid == 0) {
		redirectFiles(ast->input, ast->output);
		execvp(ast->token[0], ast->token);
		/*unreachable*/
		err(1, "%s", ast->token[0]);
	}
	return pid;
}

struct AST *
parse()
{
	struct AST *ast = getASTNode();

	readToken(&(ast->token), &(ast->input), &(ast->output));
	ast->type = NODE_COMMAND;
	return ast;
}

void
routine(void)
{
	struct AST *ast = NULL;

	for (;;) {
		printBar();
		ast = parse();
		execute(ast);
		freeAST(ast);
		wait(NULL);
	}
}


