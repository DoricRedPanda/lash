void
sclose(int fd);

void *
smalloc(size_t size);

void *
srealloc(void *ptr, size_t size);

int
sopen(char *fileName, int flag);

void
sdup2(int oldfd, int newfd);

void
spipe(int pipefd[2]);
