typedef struct {
	char *name;
	void (*func)(char **);
} Builtin;

int builtin(struct AST *ast);

void builtin_cd(char **argv);
void builtin_exit(char **argv);
