#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "tokenizer.h"
#include "util.h"

struct AST *
getASTNode(char **token, char *input, char *output, NodeType type)
{
	struct AST *ast = smalloc(sizeof(struct AST));

	ast->token = token;
	ast->input = input;
	ast->output = output;
	ast->type = type;
	ast->l = NULL;
	ast->r = NULL;
	return ast;
}

void
freeAST(struct AST *ast)
{
	if (!ast)
		return;
	if (ast->type != NODE_COMMAND) {
		freeAST(ast->l);
		freeAST(ast->r);
	} else {
		if (ast->token)
			freeToken(ast->token);
		if (ast->input)
			free(ast->input);
		if (ast->output)
			free(ast->output);
	}
	free(ast);
}
