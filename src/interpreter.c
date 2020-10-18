#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include "flags.h"
#include "ast.h"
#include "util.h"
#include "tokenizer.h"
#include "interpreter.h"
#include "builtin.h"

#define REDIRECT(UNUSED, DST, SRC) \
		(sclose(UNUSED), sdup2(SRC, DST), sclose(SRC))

static void interpret(struct AST *ast);

static void
printBar(void)
{
	char *user = getenv("USER");

	putchar(strcmp(user, "root") ? '$' : '#');
	putchar(' ');
}

static void
openReplace(char *file, int dstfd)
{
	int srcfd;

	if (file) {
		srcfd = sopen(file, dstfd);
		sdup2(srcfd, dstfd);
		sclose(srcfd);
	}
}

static void
redirectFiles(char *input, char *output)
{
	if (input)
		openReplace(input, STDIN_FILENO);
	if (output)
		openReplace(output, STDOUT_FILENO);
}

static void
execute(struct AST *ast)
{
	redirectFiles(ast->input, ast->output);
	if (builtin(ast))
		exit(EXIT_SUCCESS);
	execvp(ast->token[0], ast->token);
	err(EXIT_FAILURE, "%s", ast->token[0]);
}

static void
interpretPipe(struct AST *ast)
{
	pid_t pid;
	int fd[2];

	spipe(fd);
	pid = fork();
	if (pid < 0) {
		err(EXIT_FAILURE, "fork");
	} else if (!pid) { /* child */
		REDIRECT(fd[STDIN_FILENO], STDOUT_FILENO, fd[STDOUT_FILENO]);
		interpret(ast->l);
	} else { /* parent */
		REDIRECT(fd[STDOUT_FILENO], STDIN_FILENO, fd[STDIN_FILENO]);
		interpret(ast->r);
	}
}

static void
interpretJunction(struct AST *ast)
{
	int status, pid;

	pid = fork();
	if (pid < 0) {
		err(EXIT_FAILURE, "fork");
	} else if (!pid) {
		interpret(ast->l);
	} else {
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status))
			exit(WTERMSIG(status));
		status = WEXITSTATUS(status);
		switch (ast->type) {
		case NODE_CONJ:
			if (!status)
				interpret(ast->r);
			else
				exit(EXIT_FAILURE);
			break;
		case NODE_DISJ:
			if (status)
				interpret(ast->r);
			else
				exit(EXIT_SUCCESS);
			break;
		default:
			errx(EXIT_FAILURE, "unreachable");
		}
	}
	err(EXIT_FAILURE, "WRONG");
}

static void
interpret(struct AST *ast)
{
	if (!ast)
		errx(EXIT_FAILURE, "Syntax error");
	switch (ast->type) {
	case NODE_CONJ: /* FALLTHROUGH */
	case NODE_DISJ:
		interpretJunction(ast);
		break;
	case NODE_PIPE:
		interpretPipe(ast);
		break;
	default:
		execute(ast);
	}
	err(EXIT_FAILURE, "WRONG");
}

static struct AST *
parse(FILE *file, int *bg)
{
	struct AST *ast = NULL, *old = NULL;
	char **token = NULL, *input = NULL, *output = NULL;
	NodeType type;

	do {
		type = readToken(file, &token, &input, &output);
		if (type == NODE_AMPR) {
			*bg = 1;
			type = NODE_COMMAND;
		}
		if (!token) {
			free(input);
			free(output);
			if (ast)
				warnx("Syntax error");
			freeAST(ast);
			return NULL;
		}
		*(ast ? &(ast->r) : &ast) = getASTNode(
			token, input, output, NODE_COMMAND);
		if (type != NODE_COMMAND) {
			old = ast;
			ast = getASTNode(NULL, NULL, NULL, type);
			ast->l = old;
		}
	} while (type != NODE_COMMAND);
	return ast;
}

void
routine(FILE *file)
{
	struct AST *ast = NULL;
	int bg;
	pid_t pid;

	for (;;) {
		if (p_interactive)
			printBar();
		bg = 0;
		ast = parse(file, &bg);
		if (ast) {
			pid = (!bg && builtin(ast)) ? -1 : fork();
			if (!pid) {
				if (!p_interactive)
					fclose(file);
				if (bg)
					setpgid(getpid(), getpid());
				interpret(ast);
			}
			freeAST(ast);
			if (!bg)
				waitpid(pid, NULL, 0);
			while (waitpid(-1, NULL, WNOHANG) > 0)
				; /* catch zombies */
		}
	}
	fclose(file);
}

