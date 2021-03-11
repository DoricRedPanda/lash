typedef struct {
	char *name;
	void (*func)(char *argv[]);
} Builtin;

int builtin(char *argv[]);

void builtin_cd(char *argv[]);
void builtin_exit(char *argv[]);
