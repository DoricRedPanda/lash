void
sclose(int fd);

void *
srealloc(void *ptr, int size);

int
sopen(char *fileName, int flag);

void
sdup2(int oldfd, int newfd);


void
spipe(int pipefd[2]);
