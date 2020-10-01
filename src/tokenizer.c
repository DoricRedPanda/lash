#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "tokenizer.h"

char *
getWord(int *ending)
{
	char *word = NULL;
	int ch;
	size_t len = 0;

	for (;;) {
		ch = getchar();
		len++;
		word = srealloc(word, len * sizeof(char));
		word[len - 1] = ch;
		if (ch < 0)
			exit(0);
		if (strchr(" \n\t", ch)) {
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

char **
getToken()
{
	char **token = NULL;
	char *word;
	int ending;
	size_t len = 0;

	for (;;) {
		word = getWord(&ending);
		len++;
		token = srealloc(token, len * sizeof(char *));
		token[len - 1] = word;
		if (strchr("\n", ending)) {
			len++;
			token = srealloc(token, len * sizeof(char *));
			token[len - 1] = NULL;
			break;
		}
	}
	return token;
}

void
freeToken(char **token)
{
	for (int i = 0; token[i]; i++)
		free(token[i]);
	free(token);
}
