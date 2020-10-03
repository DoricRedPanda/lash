#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include "ast.h"
#include "util.h"
#include "tokenizer.h"


static void
printBar(void)
{
	char *user = getenv("USER");

	putchar(strcmp(user, "root") ? '$' : '#');
	putchar(' ');
}

static void
openreplace(char *file, int oldfd)
{
	int fd;

	if (file) {
		fd = sopen(file, oldfd);
		sdup2(fd, oldfd);
		sclose(fd);
	}
}

static void
redirectFiles(char *input, char *output)
{
	if (input)
		openreplace(input, STDIN);
	if (output)
		openreplace(output, STDOUT);
}

static int
execute(struct AST *ast)
{
	pid_t pid;
	int wstatus;

	pid = fork();
	if (pid < 0) {
		perror("fork failed");
	} else if (pid == 0) {
		redirectFiles(ast->input, ast->output);
		execvp(ast->token[0], ast->token);
		/*unreachable*/
		err(1, "%s", ast->token[0]);
	}
	wait(&wstatus);
	return WIFEXITED(wstatus) ? WEXITSTATUS(wstatus): 1;
}

int
interpret(struct AST *ast) {
	int ret;

	if (!ast)
		return 1;
	switch (ast->type) {
	case NODE_CONJ:
		ret = interpret(ast->l) || interpret(ast->r);
		break;
	case NODE_DISJ:
		ret = interpret(ast->l) && interpret(ast->r);
		break;
	case NODE_PIPE:
		break;
	case NODE_COMMAND:
		ret = execute(ast);
		break;
	}
	return ret;
}

static struct AST *
parse()
{
	struct AST *ast = getASTNode(), *opNode;
	NodeType type;

	type = readToken(&(ast->token), &(ast->input), &(ast->output));
	ast->type = NODE_COMMAND;
	if (type != NODE_COMMAND) {
		opNode = getASTNode();
		opNode->type = type;
		opNode->l = ast;
		opNode->r = parse();
		ast = opNode;
	}
	return ast;
}

void
routine(void)
{
	struct AST *ast = NULL;

	for (;;) {
		printBar();
		ast = parse();
		interpret(ast);
		freeAST(ast);
	}
}


