#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "tokenizer.h"

char *
getWord(int *ending)
{
	char *word = NULL;
	int ch;
	size_t len = 0;

	for (;;) {
		ch = getchar();
		word = srealloc(word, ++len * sizeof(char));
		word[len - 1] = ch;
		if (ch <= 0 || strchr(" \n\t><", ch)) {
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
void
parseRedirection(char **word, int *ending, char **input, char **output)
{
	switch (*ending) {
	case '<':
		if (*input)
			free(*input);
		while (!(*input = getWord(ending)))
		       ;
		break;
	case '>':
		if (*output)
			free(*output);
		while (!(*output = getWord(ending)))
		       ;
		break;
	}
}

size_t
readToken(char ***token, char **input, char **output)
{
	char **t = NULL, *word;
	int ending;
	size_t len = 0;

	do {
		word = getWord(&ending);
		parseRedirection(&word, &ending, input, output);
		if (word) {
			t = srealloc(t, ++len * sizeof(char *));
			t[len - 1] = word;
		}
	} while (ending > 0 && !strchr("\n", ending));
	if (!len) {
		if (ending == EOF)  /* empty line ended with EOF */
			exit(EXIT_SUCCESS);
		*token = NULL;
		return 0;
	}
	t = srealloc(t, (len + 1) * sizeof(char *));
	t[len] = NULL;
	*token = t;
	return len;
}

void
freeToken(char **token)
{
	for (int i = 0; token[i]; i++)
		free(token[i]);
	free(token);
}
