typedef enum {
	NODE_PIPE,
	NODE_CONJ,
	NODE_DISJ,
	NODE_AMPR,
	NODE_COMMAND, /* always last */
} NodeType;

struct AST {
	char **token;
	char *input;
	char *output;
	struct AST *l;
	struct AST *r;
	NodeType type;
};

struct AST *getASTNode(char *token[], char input[], char output[], NodeType);

void freeAST(struct AST *ast);
