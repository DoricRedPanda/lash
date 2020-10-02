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
		err(1, "pipe");
}

void
sclose(int fd)
{
	if (close(fd))
		err(1, "close");
}

void *
smalloc(size_t size)
{
	void *ptr = malloc(size);
	if (ptr == NULL)
		err(1, "malloc");
	return ptr;
}

void *
srealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (ptr == NULL)
		err(1, "realloc");
	return ptr;
}

int
sopen(char *fileName, int flag)
{
	int fd = -1;

	if (fileName == NULL) {
		return flag;
	} else if (flag == 0) {
		fd = open(fileName, O_RDONLY);
	} else if (flag == 1) {
		fd = open(fileName,
			O_WRONLY | O_CREAT | O_TRUNC, 0644);
	}
	if (fd < 0)
		err(1, "open");
	return fd;
}

void
sdup2(int oldfd, int newfd)
{
	if (dup2(oldfd, newfd) < 0)
		err(1, "dup2");
}
