#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "util.h"
#include "tokenizer.h"

static char *oper[] = {
	[NODE_PIPE] = "|",
	[NODE_CONJ] = "&&",
	[NODE_DISJ] = "||",
	[NODE_AMPR] = "&",
};

static int
skipComment(FILE *file, int ch)
{
	if (ch == '#')
		while (ch != '\n' && ch != EOF)
			ch = fgetc(file);
	return ch;
}

static char *
getWord(FILE *file, int *ending)
{
	char *word = NULL;
	int ch = 0;
	size_t len = 0;

	for (;;) {
		ch = fgetc(file);
		ch = skipComment(file, ch);
		word = srealloc(word, ++len * sizeof(char));
		word[len - 1] = ch;
		if (ch == EOF || strchr("\n \t><", ch)) {
			*ending = ch;
			if (errno != 0) {
				*ending = '\n';
				putchar('\n');
				errno = 0;
			}
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
parseRedirection(FILE *file, int *ending, char **input, char **output)
{
	char **ptr = NULL;

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
	free(*ptr);
	while (!(*ptr = getWord(file, ending)))
		;
}

static NodeType
checkOper(const char word[])
{
	NodeType type = NODE_COMMAND;

	for (NodeType i = NODE_PIPE; i < NODE_COMMAND; i++)
		if (!strcmp(word, oper[i])) {
			type = i;
			break;
		}
	return type;
}

NodeType
readToken(FILE *file, char ***token, char **input, char **output)
{
	char **t = NULL, *word;
	int ending = 0, len = 0;
	NodeType type = NODE_COMMAND;

	do {
		word = getWord(file, &ending);
		parseRedirection(file, &ending, input, output);
		if (word) {
			type = checkOper(word);
			if (type != NODE_COMMAND) {
				free(word);
				break;
			}
			t = srealloc(t, ++len * sizeof(char *));
			t[len - 1] = word;
		}
	} while (ending != EOF && ending != '\n');
	if (len) {
		t = srealloc(t, (len + 1) * sizeof(char *));
		t[len] = NULL;
	} else if (ending == EOF) { /* empty line ended with EOF */
		exit(EXIT_SUCCESS);
	}
	*token = t;
	return type;
}

void
freeToken(char **token)
{
	if (!token)
		return;
	for (int i = 0; token[i]; i++)
		free(token[i]);
	free(token);
}
