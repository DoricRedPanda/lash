#include <stdlib.h>

#include "ast.h"
#include "tokenizer.h"
#include "util.h"

struct AST *
getASTNode()
{
	struct AST *ast = smalloc(sizeof(struct AST));

	ast->token = NULL;
	ast->input = NULL;
	ast->output = NULL;
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
