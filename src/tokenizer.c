#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "util.h"
#include "tokenizer.h"

static char *binoper[] = {
	[NODE_PIPE] = "|",
	[NODE_CONJ] = "&&",
	[NODE_DISJ] = "||",
};

static char *
getWord(int *ending)
{
	char *word = NULL;
	int ch;
	size_t len = 0;

	for (;;) {
		ch = getchar();
		word = srealloc(word, ++len * sizeof(char));
		word[len - 1] = ch;
		if (ch <= 0 || strchr("\n \t><", ch)) {
			*ending = ch;
			if (len == 1) {
				free(word);
				return NULL;
			}
			word[len - 1] = '\0';
			break;
		}
	}
	return word;
}

/* TODO: handle [n]redirect_op word, handle errors */
static void
parseRedirection(char **word, int *ending, char **input, char **output)
{
	char **ptr;

	switch (*ending) {
	case '<':
		ptr = input;
		break;
	case '>':
		ptr = output;
		break;
	default:
		return;
	}
	if (*ptr)
		free(*ptr);
	while (!(*ptr = getWord(ending)))
		;
}

static NodeType
checkbinop(char *word)
{
	NodeType type = NODE_COMMAND;

	for (NodeType i = NODE_PIPE; i < NODE_COMMAND; i++)
		if (!strcmp(word, binoper[i])) {
			type = i;
			break;
		}
	return type;
}

NodeType
readToken(char ***token, char **input, char **output)
{
	char **t = NULL, *word;
	int ending = 0, len = 0;
	NodeType type = NODE_COMMAND;

	do {
		word = getWord(&ending);
		parseRedirection(&word, &ending, input, output);
		if (word) {
			type = checkbinop(word);
			if (type != NODE_COMMAND) {
				free(word);
				break;
			}
			t = srealloc(t, ++len * sizeof(char *));
			t[len - 1] = word;
		}
	} while (ending > 0 && ending != '\n');
	if (!len) {
		if (ending == EOF)  /* empty line ended with EOF */
			exit(EXIT_SUCCESS);
	} else {
		t = srealloc(t, (len + 1) * sizeof(char *));
		t[len] = NULL;
	}
	*token = t;
	return type;
}

void
freeToken(char **token)
{
	for (int i = 0; token[i]; i++)
		free(token[i]);
	free(token);
}
