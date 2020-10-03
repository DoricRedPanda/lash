#include <signal.h>

#include "interpreter.h"


static void
sigintHandler(int sig)
{
	return;
}

int main(void)
{
	signal(SIGINT, sigintHandler);
	routine();
	return 0;
}
