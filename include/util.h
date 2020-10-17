#define LEN(X) (sizeof(X) / sizeof((X)[0]))

void
sclose(int fd);

void *
smalloc(size_t size);

void *
srealloc(void *ptr, size_t size);

int
sopen(char *fileName, int flag);

void
sdup2(int srcfd, int dstfd);

void
spipe(int pipefd[2]);
