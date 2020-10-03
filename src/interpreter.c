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
#include "interpreter.h"


static int interpret(struct AST *ast);

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

static void
execute(struct AST *ast)
{
	redirectFiles(ast->input, ast->output);
	execvp(ast->token[0], ast->token);
	/*should be unreachable*/
	err(1, "%s", ast->token[0]);
}

static int
interpretPipe(struct AST *ast)
{
	pid_t pid;
	int fd[2], status;

	spipe(fd);
	pid = fork();
	if (pid < 0) {
		err(1, "fork");
	} else if (!pid) { /* child */
		sclose(fd[0]);
		sdup2(fd[1], STDOUT);
		sclose(fd[1]);
		interpret(ast->l);
	} else { /* parent */
		sclose(fd[1]);
		sdup2(fd[0], STDIN);
		sclose(fd[0]);
		interpret(ast->r);
		waitpid(pid, &status, 0);
		status = WEXITSTATUS(status);
	}
	return status;
}

static int
interpretJunction(struct AST *ast)
{
	int status, pid;

	pid = fork();
	if (pid < 0) {
		err(1, "fork");
	} else if (!pid) {
		interpret(ast->l);
	} else {
		waitpid(pid, &status, 0);
		status = !WEXITSTATUS(status);
		switch (ast->type) {
		case NODE_CONJ:
			status = status && interpret(ast->r);
			break;
		case NODE_DISJ:
			status = status || interpret(ast->r);
			break;
		default: ; /* silent warning */
		}
	}
	return status;
}

static int
interpret(struct AST *ast)
{
	int ret;

	if (!ast)
		return EXIT_FAILURE;
	switch (ast->type) {
	case NODE_CONJ: /* FALLTHROUGH */
	case NODE_DISJ:
		ret = interpretJunction(ast);
		break;
	case NODE_PIPE:
		ret = interpretPipe(ast);
		break;
	case NODE_COMMAND:
		execute(ast); /* should not return */
	}
	return ret;
}

static struct AST *
parse()
{
	struct AST *ast;
	char **token = NULL, *input = NULL, *output = NULL;
	NodeType type;

	type = readToken(&token, &input, &output);
	if (type == NODE_COMMAND && token == NULL) {
		free(input);
		free(output);
		return NULL;
	} else if (type != NODE_COMMAND) {
		ast = getASTNode(NULL, NULL, NULL, type);
		ast->l = getASTNode(token, input, output, NODE_COMMAND);
		ast->r = parse();
	} else {
		ast = getASTNode(token, input, output, NODE_COMMAND);
	}
	return ast;
}

void
routine(void)
{
	struct AST *ast = NULL;
	pid_t pid;

	for (;;) {
		printBar();
		ast = parse();
		if (ast) {
			pid = fork();
			if (!pid)
				interpret(ast);
			freeAST(ast);
			waitpid(pid, NULL, 0);
		}
	}
}

