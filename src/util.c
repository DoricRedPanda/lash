#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "util.h"

void
spipe(int pipefd[2])
{
	if (pipe(pipefd))
		err(EXIT_FAILURE, "pipe");
}

void
sclose(int fd)
{
	if (close(fd))
		err(EXIT_FAILURE, "close");
}

void *
smalloc(size_t size)
{
	void *ptr = malloc(size);

	if (ptr == NULL)
		err(EXIT_FAILURE, "malloc");
	return ptr;
}

void *
srealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (ptr == NULL)
		err(EXIT_FAILURE, "realloc");
	return ptr;
}

int
sopen(char *fileName, int flag)
{
	int fd = -1;

	if (fileName == NULL) {
		return flag;
	} else if (flag == STDIN_FILENO) {
		fd = open(fileName, O_RDONLY);
	} else if (flag == STDOUT_FILENO) {
		fd = open(fileName,
			O_WRONLY | O_CREAT | O_TRUNC, 0644);
	}
	if (fd < 0)
		err(EXIT_FAILURE, "open");
	return fd;
}

void
sdup2(int srcfd, int dstfd)
{
	if (dup2(srcfd, dstfd) < 0)
		err(EXIT_FAILURE, "dup2");
}
