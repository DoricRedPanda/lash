typedef enum {
	NODE_COMMAND,
	NODE_PIPE,
	NODE_CONJ,
	NODE_DISJ,
} NodeType;

struct AST {
	char **token;
	char *input;
	char *output;
	struct AST *l;
	struct AST *r;
	NodeType type;
};

struct AST *getASTNode(void);

void freeAST(struct AST *ast);
